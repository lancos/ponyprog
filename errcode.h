//=========================================================================//
//-------------------------------------------------------------------------//
// errcode.h -- error codes                                                //
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

#ifndef	_ERRCODE_H
#define	_ERRCODE_H

#define	OK				0
#define	BADPARAM		-2			// Parametri alla funzione errati

#define	FILENOTFOUND	-3			// File non aperto in lettura
#define	CREATEERROR		-4			// File non aperto in scrittura
#define	BADFILETYPE		-5			// File di tipo errato
#define	READERROR		-6			// Errore in lettura dal file
#define	WRITEERROR		-7			// Errore in scrittura da file
#define	NOTHINGTOSAVE	-8			// Nessun contenuto da salvare
#define NOTSUPPORTED	-9			// Funzionalita` non supportata (ancora)

#define	E2P_TIMEOUT		-10

//#define IICERR_NOERR		OK
#define IICERR_BUSBUSY		-11
#define IICERR_NOTACK		-12
#define IICERR_NOADDRACK	-13
#define IICERR_SDACONFLICT	-14
#define	IICERR_SCLCONFLICT	-15
#define	E2ERR_OPENFAILED	-16
#define	E2ERR_ACCESSDENIED	-17
#define E2ERR_NOTINSTALLED	-18
#define IICERR_TIMEOUT		-19
#define	IICERR_STOP			-20

#define	E2ERR_WRITEFAILED	-21

#define	DEVICE_BADTYPE		-23
#define	DEVICE_UNKNOWN		-24
#define	DEVICE_LOCKED		-25
#define	OP_ABORTED			-26

#define	BUFFEROVERFLOW		-30
#define	OUTOFMEMORY			-31

//Codici di errore restituiti dall'interprete di comandi a menu
#define	CMD_BUFFEREMPTY		-39
#define CMD_NOTHINGTOWRITE	-40
#define	CMD_NOTHINGTOVERIFY	-41
#define	CMD_NOTHINGTOLOAD	-42
#define	CMD_NOTHINGTOSAVE	-43
#define	CMD_WRITEFAILED		-44
#define	CMD_VERIFYFAILED	-45
#define	CMD_ROLLOVERFAILED	-46

#endif

