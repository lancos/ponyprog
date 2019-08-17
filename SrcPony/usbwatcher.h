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

#include <QObject>
#include <QVector>
#include <QTimer>

#include <libusb-1.0/libusb.h> //Include libsub

class usb_data
{
  public:

	bool operator==(const usb_data &a)
	{
		return (a.pid == pid && a.vid == vid);
	}
	quint16 vid;
	quint16 pid;
	//QString serial;
};

class USBWatcher : public QObject
{
	Q_OBJECT

	//QThread workerThread;

  public:
	USBWatcher();
	~USBWatcher();

	QVector <usb_data> vUSB;

  signals:
	void notify(bool connected, const quint16 &vid, const quint16 &pid);

  public slots:
	void doPoll();

  private:
	bool hotplug_register(quint16 vid = 0, quint16 pid = 0);
	void hotplug_deregister();

	libusb_hotplug_callback_handle cbHandle;
	libusb_context *usb_ctx;
	QTimer *timer;
};

#endif // USBWATCHER_H

