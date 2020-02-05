//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2020   Claudio Lanconelli                           //
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

#include <QDebug>

#include "usbwatcher.h"

static int LIBUSB_CALL hotplug_callback(struct libusb_context *ctx, struct libusb_device *dev,
										libusb_hotplug_event event, void *user_data)
{
	struct libusb_device_descriptor desc;

	(void)libusb_get_device_descriptor(dev, &desc);

	USBWatcher *w = static_cast<USBWatcher *>(user_data);
	Q_CHECK_PTR(w);

	if (LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED == event)
	{
		w->hotplug_notify(true, desc.idVendor, desc.idProduct);
		qDebug() << "Connected VID:PID " << (hex) << desc.idVendor << " - " << desc.idProduct;
	}
	else if (LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT == event)
	{
		w->hotplug_notify(false, desc.idVendor, desc.idProduct);
		qDebug() << "Disconnected VID:PID " << (hex) << desc.idVendor << " - " << desc.idProduct;
	}

	return 0;
}

void USBWatcher::doPoll()
{
	if (count > 0)
	{
		if (usb_ctx)
		{
			struct timeval zero_tv = { .tv_sec = 0, .tv_usec = 0 };
			//zero_tv.tv_sec = 1; // timeout 1 sec

			int rv = libusb_handle_events_timeout_completed(usb_ctx,
					 const_cast<timeval *>(&zero_tv),
					 NULL);
			if (rv != LIBUSB_SUCCESS)
			{
				qWarning() << "libusb_handle_events_timeout_completed() failed: " << rv;
			}
		}
		else
		{
			//Don't support hotplug, simulate it with a timer event
			emit notify(false, 0, 0);
		}
	}
}

USBWatcher::USBWatcher()
	: cbHandle(0),
	  usb_ctx(0),
	  timer(0),
	  count(0)
{
	vUSB.clear();
}

USBWatcher::~USBWatcher()
{
	hotplug_deregister();
}

void USBWatcher::hotplug_deregister()
{
	if (count > 0)
	{
		if (timer)
		{
			timer->stop();
			delete timer;
		}
		if (usb_ctx)
		{
			libusb_hotplug_deregister_callback(usb_ctx, cbHandle);
			libusb_exit(usb_ctx);
			usb_ctx = 0;
		}
		count--;
	}
}

bool USBWatcher::hotplug_register(quint16 vid, quint16 pid)
{
	bool result = false;

	if (count == 0)
	{
		int tval = 1000;

		libusb_init(&usb_ctx);
		if (libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG))
		{
			int rc = libusb_hotplug_register_callback(usb_ctx,
					 (libusb_hotplug_event)(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
					 LIBUSB_HOTPLUG_ENUMERATE,
					 (vid == 0) ? LIBUSB_HOTPLUG_MATCH_ANY : vid,
					 (pid == 0) ? LIBUSB_HOTPLUG_MATCH_ANY : pid,
					 LIBUSB_HOTPLUG_MATCH_ANY,
					 hotplug_callback,
					 this,
					 &cbHandle);
			if (LIBUSB_SUCCESS != rc)
			{
				libusb_exit(usb_ctx);
				usb_ctx = 0;
			}
			else
			{
				tval = 100;
				result = true;
			}
		}
		else
		{
			libusb_exit(usb_ctx);
			usb_ctx = 0;

			qWarning() << "NO USB HotPlug capability (simulate it with a timer)";
		}

		QTimer *timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(doPoll()));
		timer->start(tval);
		count++;
	}

	return result;
}
