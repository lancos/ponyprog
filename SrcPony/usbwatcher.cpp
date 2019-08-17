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

#include <QDebug>

#include "usbwatcher.h"

int LIBUSB_CALL hotplug_callback(struct libusb_context *ctx, struct libusb_device *dev,
								 libusb_hotplug_event event, void *user_data)
{
	struct libusb_device_descriptor desc;

	(void)libusb_get_device_descriptor(dev, &desc);

	//unsigned char ser[256] = "";
	//int n = libusb_get_string_descriptor_ascii(dev, desc.iSerialNumber, ser, sizeof(ser));
	struct usb_data ud = { .vid = desc.idVendor, .pid = desc.idProduct };
	//if (n > 0)
	//	ud.serial = QString(ser);

	//QVector <usb_data> *v;
	//v = (QVector <usb_data> *)user_data;
	USBWatcher *w = (USBWatcher *)user_data;
	Q_CHECK_PTR(w);

	if (LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED == event)
	{
		w->vUSB.append(ud);
		emit w->notify(true, (const quint16 &)desc.idVendor, (const quint16 &)desc.idProduct);

		qDebug() << "Connected VID:PID " << (hex) << desc.idVendor << " - " << desc.idProduct;
	}
	else if (LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT == event)
	{
		int idx = w->vUSB.indexOf(ud);
		if (idx != -1)
		{
			w->vUSB.remove(idx);
		}
		emit w->notify(false, (const quint16 &)desc.idVendor, (const quint16 &)desc.idProduct);

		qDebug() << "Disconnected VID:PID " << (hex) << desc.idVendor << " - " << desc.idProduct;
	}

	return 0;
}

void USBWatcher::doPoll()
{
	struct timeval zero_tv = { .tv_sec = 0, .tv_usec = 0 };
	//zero_tv.tv_sec = 1; // timeout 1 sec

	int rv = libusb_handle_events_timeout_completed(usb_ctx,
													const_cast<timeval *>(&zero_tv),
													NULL);
	if (rv != LIBUSB_SUCCESS)
		qWarning() << "libusb_handle_events_timeout_completed() failed: " << rv;
}

USBWatcher::USBWatcher()
	: cbHandle(0),
	  usb_ctx(0),
	  timer(0)
{
	//vUSB << (usb_data)
	//{
	//	CH34x_VENDOR_ID2, CH340_UART_PRODUCT
	//};
	//vUSB << (usb_data)
	//{
	//	CH34x_VENDOR_ID2, CH341_UART_PRODUCT
	//};

	vUSB.clear();

	hotplug_register();
}

USBWatcher::~USBWatcher()
{
	hotplug_deregister();
}

void USBWatcher::hotplug_deregister()
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
	}
}

bool USBWatcher::hotplug_register(quint16 vid, quint16 pid)
{
	libusb_init(&usb_ctx);

	int rc = libusb_hotplug_register_callback(usb_ctx,
											  (libusb_hotplug_event)(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
											  LIBUSB_HOTPLUG_ENUMERATE,
											  (vid == 0) ? LIBUSB_HOTPLUG_MATCH_ANY : vid,
											  (pid == 0) ? LIBUSB_HOTPLUG_MATCH_ANY : pid,
											  LIBUSB_HOTPLUG_MATCH_ANY,
											  hotplug_callback,
											  this, //&vUSB,
											  &cbHandle);
	if (LIBUSB_SUCCESS != rc)
	{
		libusb_exit(usb_ctx);
		return false;
	}
	else
	{
		QTimer *timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(doPoll()));
		timer->start(100);
		return true;
	}
}
