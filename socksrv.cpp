//=========================================================================//
//-------------------------------------------------------------------------//
// socksrv.cpp -- SocketServer class                                       //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-1999   Claudio Lanconelli                           //
//                                                                         //
//  e-mail: lanconel@cs.unibo.it                                           //
//  http://www.cs.unibo.it/~lanconel                                       //
//                                                                         //
//-------------------------------------------------------------------------//
//                                                                         //
// This program is free software; you can redistribute it and/or           //
// modify it under the terms of the GNU  General Public License            //
// as published by the Free Software Foundation; either version2 of        //
// the License, or (at your option) any later version.                     //
//                                                                         //
// This program is distributed in the hope that it will be useful,         //
// but WITHOUT ANY WARRANTY; without even the implied warranty of          //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       //
// General Public License for more details.                                //
//                                                                         //
// You should have received a copy of the GNU  General Public License      //
// along with this program (see COPYING);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//-------------------------------------------------------------------------//
//=========================================================================//

#include <string.h>
#include <errno.h>

#include <sys/types.h>

#include "socksrv.h"

#ifdef	_UDP_SERVER
#include "e2app.h"
#include "e2awinfo.h"
#endif

#define	PONYID	"Pony"

#ifndef	OK
#define	OK	0
#endif

#ifdef	_UDP_SERVER
SocketServer::SocketServer(e2CmdWindow *cwin, int port)
#else
SocketServer::SocketServer(int port)
#endif
{
	cmdWin = cwin;

	error_code = OK;
	error_msg[0] = '\0';

	tcp_port = (port > 1024) ? port : DEF_PORT;
	udp_port = tcp_port + 1;

	udp_sockfd = INVALID_SOCKET;
	tcp_sockfd = INVALID_SOCKET;

	/*
	 * Initialize Sockets
	 */
	if ( !SocketInit()
#ifdef	_UDP_SERVER
				|| !cmdWin
#endif
							)
	{
		SetError(1, "error in Socket initialization");
	}
}

SocketServer::~SocketServer()
{
	if (udp_sockfd != INVALID_SOCKET)
	{
		closesocket(udp_sockfd);
		udp_sockfd = INVALID_SOCKET;
	}
	if (tcp_sockfd != INVALID_SOCKET)
	{
		closesocket(tcp_sockfd);
		tcp_sockfd = INVALID_SOCKET;
	}

	SocketEnd();
}

void SocketServer::SetError(int n, char *str)
{
	error_code = n;
	if (str)
		strncpy(error_msg, str, sizeof(error_msg));
	error_msg[sizeof(error_msg) - 1] = '\0';
}

int SocketServer::GetError(char *str, int len)
{
	if (str != 0 && len > 0)
	{
		strncpy(str, error_msg, len);
	}
	return error_code;
}

#ifdef	_UDP_CLIENT
int SocketServer::UDPSendCommand(char *ip_address, int &command, char *param, int paramsize)
{
	//Check parameters
	if ( ip_address == 0 || paramsize <= 0 || param == 0 ||
			command <= ACKNOWLEDGE || command >= LAST_ACTION )
	{
		SetError(-2, "client: bad parameters");
		return -2;
	}

	//Build the message
	strcpy(msg.Id, PONYID);
	msg.Action = command;
	strncpy(msg.msg, param, MAXMESG-1);

	//Prepare socket
	if (udp_sockfd != INVALID_SOCKET)
	{
		closesocket(udp_sockfd);
		udp_sockfd = INVALID_SOCKET;
	}

	/*
	 * Fill in the "serv_addr" field with the server address
	 */
	struct sockaddr_in	serv_addr, cli_addr;
	struct hostent *hostptr;
	unsigned int inaddr;

	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family      = AF_INET;
	serv_addr.sin_port        = htons(ip_port);
	if ((inaddr = inet_addr(ip_address)) != INADDR_NONE)
	{
		serv_addr.sin_addr.s_addr = inaddr;
	}
	else
	if ((hostptr = gethostbyname(ip_address)) != NULL)
	{
		serv_addr.sin_addr = *((struct in_addr*)hostptr->h_addr);
	}
	else
	{
		SetError(1, "client: bad server address");

		return 1;
	}

	/*
	 * Open the UDP socket
	 */
	udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_sockfd == INVALID_SOCKET)
	{
#ifdef	WIN32
		errno = WSAGetLastError();
		SetError(errno, GetSockErrorString(errno));
#else
		SetError(2, "client: can't open datagram socket");
#endif
		return 2;
	}

	/*
	 * Request for a local address
	 */
	bzero((char *)&cli_addr, sizeof(cli_addr));
	cli_addr.sin_family      = AF_INET;
	cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	cli_addr.sin_port        = htons(0);
	if (bind(udp_sockfd, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) < 0)
	{
		SetError(3, "client: can't bind local address");
		return 3;
	}

	/*
	 * Send the service request to the server
	 */
	CalcSum();
	int n = sizeof(msg);
	if (sendto(udp_sockfd, (char *)&msg, n, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != n)
	{
		SetError(5, "clinet: sendto error");
		return 5;
	}

	//Wait for reply
	fd_set rfds;
	struct timeval tv;

	/* Watch stdin (fd 0) to see when it has input. */
	FD_ZERO(&rfds);
	FD_SET(udp_sockfd, &rfds);

	// Wait 10 seconds for the ack
	tv.tv_sec = 10;
	tv.tv_usec = 0;

	n = select(udp_sockfd+1, &rfds, NULL, NULL, &tv);
	if (n < 0)       //Error
	{
		SetError(7, "client: select() error");
		return 7;
	}
	else
	if (n == 0)  //Timeout
	{
		SetError(8, "client: no response from server (timeout)");
		return 8;
	}

	//Read reply
	socklen_t fromlen = 0;
	n = recvfrom(udp_sockfd, (char *)&msg, sizeof(msg), 0, NULL, &fromlen);
	if (n < 0)
	{
		SetError(4, "client: recvfrom() error");
		return 4;
	}

	if (!CheckSum())
	{
		SetError(10, "Bad checksum\n");
		return 10;
	}

	if (msg.Action != ACKNOWLEDGE)
	{
		SetError(9, "client: bad acknowledge");
		return 9;
	}

	//Wait for result
	/* Watch stdin (fd 0) to see when it has input. */
	FD_ZERO(&rfds);
	FD_SET(udp_sockfd, &rfds);

	// Wait 10 minutes for the result
	tv.tv_sec = 60*10;
	tv.tv_usec = 0;

	n = select(udp_sockfd+1, &rfds, NULL, NULL, &tv);
	if (n < 0)       //Error
	{
		SetError(7, "client: select() error");
		return 7;
	}
	else
	if (n == 0)  //Timeout
	{
		SetError(11, "client: no response from server (timeout)");
		return 11;
	}

	//Read result
	fromlen = 0;
	n = recvfrom(udp_sockfd, (char *)&msg, sizeof(msg), 0, NULL, &fromlen);
	if (n < 0)
	{
		SetError(4, "client: recvfrom() error");
		return 4;
	}

	if (!CheckSum())
	{
		SetError(10, "Bad checksum\n");
		return 10;
	}

	//Return result
	command = msg.Action;
	strncpy(param, msg.msg, paramsize-1);
	param[paramsize-1] = '\0';

	if (udp_sockfd != INVALID_SOCKET)
	{
		closesocket(udp_sockfd);
		udp_sockfd = INVALID_SOCKET;
	}

	return OK;
}

int SocketServer::TCPSend(void *buffer, int size)
{
	return OK;
}

#endif

#ifdef	_UDP_SERVER
int SocketServer::TCPReceive(void *buffer, int &size)
{
	return OK;
}

int SocketServer::UDPMainLoop()
{
	struct sockaddr_in	serv_addr, cli_addr;

	if (udp_sockfd != INVALID_SOCKET)
	{
		closesocket(udp_sockfd);
		udp_sockfd = INVALID_SOCKET;
	}

	/*
	 * Open the UDP socket
	 */
	udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_sockfd == INVALID_SOCKET)
	{
#ifdef	WIN32
		errno = WSAGetLastError();
		SetError(errno, GetSockErrorString(errno));
#else
		SetError(2, "server: can't open datagram socket");
#endif
		return 2;
	}

	/*
	 * Bind to the local address to receive requests
	 */
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family      = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port        = htons(udp_port);

	if (bind(udp_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		SetError(3, "server: can't bind local address");
		return 3;
	}

	int progress = 0;
	while ( !THEAPP->GetAbortFlag() )
	{
		if (++progress > 200)
			progress = 1;

		if ( (progress & 1) )
			THEAPP->SetProgress(progress >> 1);

		fd_set rfds;
		struct timeval tv;

		/* Watch stdin (fd 0) to see when it has input. */
		FD_ZERO(&rfds);
		FD_SET(udp_sockfd, &rfds);

		/* Wait half a second */
		tv.tv_sec = 0;
		tv.tv_usec = 500000;

		int n = select(udp_sockfd+1, &rfds, NULL, NULL, &tv);
		/* Don't rely on the value of tv now! */
		if (n < 0)       //Error
		{
			SetError(7, "server: select() error");
			return 7;
		}
		else
		if (n == 0)  //Timeout
		{
			continue;
		}

		/*
		 * Wait for the request from clients
		 */
		socklen_t clilen = sizeof(cli_addr);
		n = recvfrom(udp_sockfd, (char *)&msg, sizeof(msg), 0, (struct sockaddr *)&cli_addr, &clilen);
		if (n != sizeof(msg))
		{
#ifdef	WIN32
			if (n < 0)
			{
				errno = WSAGetLastError();
				SetError(errno, GetSockErrorString(errno));
			}
			else
#endif
				SetError(4, "server: recvfrom error");

			return 4;
		}

		//int err = 0;

		/*
		 * Check the request
		 */
		if (strcmp(msg.Id, PONYID) != 0)
		{
			strcpy(msg.Id, PONYID);
			msg.Action = -1;
			strcpy(msg.msg, "Error: bad request\n");
		}
		else
		{
			//Check for parameters

			if ( !CheckSum() )
			{
				strcpy(msg.msg, "Error: bad checksum\n");
				msg.Action = -3;
			}
			else
			{
				if (msg.Action > ACKNOWLEDGE && msg.Action < LAST_ACTION)
				{
					int action = msg.Action;
					/*
					 * Send the acknowledge to the client
					 */
					msg.Action = ACKNOWLEDGE;
					msg.msg[0] = '\0';
					CalcSum();
					if (sendto(udp_sockfd, (char *)&msg, sizeof(msg), 0, (struct sockaddr *)&cli_addr, clilen) != sizeof(msg))
					{
						SetError(5, "server: sendto error");
						return 5;
					}

					switch(action)
					{
					case	ACTION_READ:
						msg.Action = cmdWin->CmdRead();
						break;
					case	ACTION_WRITE:
						msg.Action = cmdWin->CmdWrite();
						break;
					case	ACTION_VERIFY:
						msg.Action = cmdWin->CmdVerify();
						break;
					case	ACTION_RESET:
						msg.Action = cmdWin->CmdReset();
						break;
					case	ACTION_LOAD:
						break;
					case	ACTION_RELOAD:
						msg.Action = cmdWin->CmdReload();
						break;
					case	ACTION_SAVE:
						msg.Action = cmdWin->CmdSave();
						break;
					case	ACTION_SAVEAS:
						break;
					case	ACTION_LOCK:
						break;
					case	ACTION_DOWNLOAD:
						{
							e2AppWinInfo *aptr = cmdWin->GetAWInfo();
							if (aptr)
							{
								int bufsize = aptr->GetBufSize();
								msg.Action = TCPReceive(aptr->GetBufPtr(), bufsize);
							}
						}
						break;
					} // switch
				} //if
				else
				{
					strcpy(msg.msg, "Error: bad parameters\n");
					msg.Action = -2;
				}
			} //else
		}

		/*
		 * Send the result back to the client
		 */
		CalcSum();

		if (sendto(udp_sockfd, (char *)&msg, sizeof(msg), 0, (struct sockaddr *)&cli_addr, clilen) != sizeof(msg))
		{
			SetError(5, "server: sendto error");
			return 5;
		}
	}

	return OK;
}
#endif

void SocketServer::CalcSum()
{
	msg.checksum = 0;

	BYTE *bp = (BYTE *) &msg;
	int ck = 0;

	unsigned int k;
	for (k = 0; k < sizeof(PonyMessage); k++)
	{
		ck += bp[k];
	}
	ck &= 0xFF;

	msg.checksum = (unsigned char)(0x100 - ck);
}

int SocketServer::CheckSum()
{
	BYTE *bp = (BYTE *) &msg;
	unsigned char ck = 0;

	unsigned int k;
	for (k = 0; k < sizeof(PonyMessage); k++)
	{
		ck += bp[k];
	}
	return (ck == 0);
}

void SocketServer::SocketEnd()
{
#ifdef	WIN32
	WSACleanup(); 
#endif
}

int SocketServer::SocketInit()
{
#ifdef	WIN32
	WORD wVersionRequested; 
	WSADATA wsaData; 
	int err; 
	wVersionRequested = MAKEWORD(1, 0); 
 
	err = WSAStartup(wVersionRequested, &wsaData); 
 
	if (err != 0) 
		/* Tell the user that we couldn't find a useable */ 
		/* winsock.dll.     */ 
		return 0; 
 
	/* Confirm that the Windows Sockets DLL supports 1.0.*/ 
	/* Note that if the DLL supports versions greater */ 
	/* than 1.0 in addition to 1.0, it will still return */ 
	/* 1.0 in wVersion since that is the version we */ 
	/* requested. */ 
 
	if ( LOBYTE( wsaData.wVersion ) != 1 || 
			HIBYTE( wsaData.wVersion ) != 0 )
	{ 
	//	fprintf(stderr, "No usable WinSock DLL\n");
		WSACleanup(); 
		return 0; 
	} 
	/* The Windows Sockets DLL is acceptable. Proceed. */ 
#endif
	return 1;
}

#ifdef	WIN32
static struct {
	int errval;
	char *errstr;
} sockerrors[] = {
	{WSANOTINITIALISED, "A successful WSAStartup must occur before using this function."},
	{WSAENETDOWN,"The Windows Sockets implementation has detected that the network subsystem has failed."},
	{WSAEFAULT,"The fromlen argument was invalid: the from buffer was too small to accommodate the peer address."},
	{WSAEINTR,"	The (blocking) call was canceled using WSACancelBlockingCall."},
	{WSAEAFNOSUPPORT,"The specified address family is not supported."},
	{WSAEINPROGRESS,"A blocking Windows Sockets operation is in progress."},
	{WSAEINVAL,"The socket has not been bound with bind"},
	{WSAENOTCONN,"The socket is not connected (SOCK_STREAM only)"},
	{WSAENOTSOCK,"The descriptor is not a socket"},
	{WSAEOPNOTSUPP,"MSG_OOB was specified, but the socket is not of type SOCK_STREAM"},
	{WSAESHUTDOWN,"The socket has been shut down; it is not possible to recvfrom on a socket after shutdown has been invoked with how set to 0 or 2."},
	{WSAEWOULDBLOCK,"The socket is marked as nonblocking and the recvfrom operation would block"},
	{WSAEMSGSIZE,"The datagram was too large to fit into the specified buffer and was truncated"},
	{WSAECONNABORTED,"The virtual circuit was aborted due to timeout or other failure"},
	{WSAECONNRESET,"The virtual circuit was reset by the remote side"},
	{WSAEMFILE, "No more file descriptors are available."},
	{WSAENOBUFS, "No buffer space is available. The socket cannot be created."},
	{WSAEPROTONOSUPPORT, "The specified protocol is not supported."},
	{WSAEPROTOTYPE, "The specified protocol is the wrong type for this socket."},
	{WSAESOCKTNOSUPPORT, "The specified socket type is not supported in this address family."},
	{0, NULL}
};
#endif

char *SocketServer::GetSockErrorString(int val)
{
#ifdef	WIN32
	int k;
	for (k = 0; sockerrors[k].errstr != NULL; k++)
	{
		if (sockerrors[k].errval == val)
		{
			break;
		}
	}
	return sockerrors[k].errstr;
#else
	return 0;
#endif
}
