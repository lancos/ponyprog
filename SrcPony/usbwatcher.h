//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2019   Claudio Lanconelli                           //
//                                                                         //
//  Copyright (C) 2019 LibUSB part, Eduard Kalinowski                      //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
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
// along with this program (see LICENSE);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//=========================================================================//


#ifndef USBWATCHER_H
#define USBWATCHER_H

#include <QThread>
#include <QObject>
#include <QVector>

#include <libusb-1.0/libusb.h> //Include libsub



class USBWatcher: public QThread
{
	Q_OBJECT
  public:
	USBWatcher(QObject *p);
	~USBWatcher();

	struct usb_data
	{
		quint16 vendor;
		quint16 product;
	};

	void hotplug_register();

  public:
	QVector <libusb_hotplug_callback_handle> vHandle;
	libusb_context *usb_ctx;

  signals:
	void USBConnected(const quint16 &vid, const quint16 &pid);
	void USBDisconnected();

  private slots:
	void autoStart();

  private:
	void run();

  private:
	QVector <usb_data> vUSB;
	bool usb_thread_loop;
};


#endif // USBWATCHER_H

