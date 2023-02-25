//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2021   Claudio Lanconelli                           //
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

#include "globals.h"

class USBWatcher : public QObject
{
	Q_OBJECT

	//QThread workerThread;

  public:
	USBWatcher();
	~USBWatcher();

	bool hotplug_register(quint16 vid = 0, quint16 pid = 0);

	void hotplug_notify(bool connected, quint16 vid, quint16 pid)
	{
		VidPid *vObj = new VidPid(vid, pid);
		if (connected)
		{
			vUSB.append(vObj);
			emit notify(true, vid, pid);
		}
		else
		{
			int idx = vUSB.indexOf(vObj);
			if (idx != -1)
			{
				vUSB.remove(idx);
			}
			emit notify(false, vid, pid);
		}
		delete vObj;
	}

  signals:
	void notify(bool connected, quint16 vid, quint16 pid);

  public slots:
	void doPoll();

  private:
	void hotplug_deregister();

	QVector <VidPid *> vUSB;
	libusb_hotplug_callback_handle cbHandle;
	libusb_context *usb_ctx;
	QTimer *timer;
	int count;
};

#endif // USBWATCHER_H

