//=========================================================================//
//-------------------------------------------------------------------------//
// socksrv.h -- Header for SocketServer class                              //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997, 1998  Claudio Lanconelli                           //
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

#ifndef _SOCKETSERVER_H
#define _SOCKETSERVER_H

#define MAXMESG		129
#define	DEF_PORT	8534

#ifdef	WIN32
# include <winsock.h>
# define bzero(addr, size)	memset(addr, 0, size)
typedef int socklen_t;
#endif

#ifdef	_LINUX_
# include <unistd.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>

  typedef int SOCKET;
# define closesocket(s)	close(s)
# define INVALID_SOCKET	-1
#endif

typedef struct {
	char Id[6];
	int Action;
	char msg[MAXMESG];
	unsigned char checksum;
} PonyMessage;

enum {
	ACKNOWLEDGE,

	ACTION_READ,
	ACTION_WRITE,
	ACTION_VERIFY,
	ACTION_RESET,
	ACTION_LOAD,
	ACTION_RELOAD,
	ACTION_SAVE,
	ACTION_SAVEAS,
	ACTION_LOCK,
	ACTION_DOWNLOAD,

	LAST_ACTION
};

#ifdef	_UDP_SERVER
#include "e2cmdw.h"
#endif

class SocketServer
{
  public:		//---------------------------------------- public

#ifdef	_UDP_SERVER
	SocketServer(e2CmdWindow *cwin, int port = 0);
#else
	SocketServer(int port = 0);
#endif
	virtual ~SocketServer();

#ifdef	_UDP_SERVER
	int UDPMainLoop();
	int TCPReceive(void *buffer, int &size);
#endif

#ifdef	_UDP_CLIENT
	int UDPSendCommand(char *ip_address, int &command, char *param, int paramsize);
	int TCPSend(void *buffer, int size);
#endif

	int GetError(char *str, int len);

	void SetUDPPort(int new_port)
	{
		if (new_port > 1023 && new_port != tcp_port)
			udp_port = new_port;
	}
	void SetTCPPort(int new_port)
	{
		if (new_port > 1023 && new_port != udp_port)
			tcp_port = new_port;
	}

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private

#ifdef	_UDP_SERVER
	e2CmdWindow* cmdWin;
#endif

	void SetError(int n, char *str);

	int SocketInit();
	void SocketEnd();
	char *GetSockErrorString(int val);

	int CheckSum();		//Check for the correct SUM
	void CalcSum();		//Calculate the SUM

	SOCKET tcp_sockfd;
	SOCKET udp_sockfd;

	int tcp_port;		//port for TCP/IP protocol
	int udp_port;		//port for UDP/IP protocol

	int error_code;
	char error_msg[MAXMESG];

	PonyMessage msg;
};
#endif
