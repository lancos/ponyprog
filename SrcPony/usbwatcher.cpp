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
#include <QTimer>

#include "ch341a.h"
#include "usbwatcher.h"

class USBWatcher;

int LIBUSB_CALL hotplug_callback(struct libusb_context *ctx, struct libusb_device *dev,
								 libusb_hotplug_event event, void *user_data)
{
	struct libusb_device_descriptor desc;

	(void)libusb_get_device_descriptor(dev, &desc);

	if (LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED == event)
	{
		emit reinterpret_cast<USBWatcher *>(user_data)->USBConnected((const quint16 &)desc.idVendor, (const quint16 &)desc.idProduct);
		return 0;
	}

	if (LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT == event)
	{
		emit reinterpret_cast<USBWatcher *>(user_data)->USBDisconnected();
	}

	return 0;
}

/**
 * @brief   constructor
 *          init vector of supported devices
 */
USBWatcher::USBWatcher(QObject *p) : QThread(p)
{
	vUSB << (usb_data)
	{
		CH34x_VENDOR_ID2, CH340_UART_PRODUCT
	};
	vUSB << (usb_data)
	{
		CH34x_VENDOR_ID2, CH341_UART_PRODUCT
	};

	// autostart 1 second later
	QTimer::singleShot(1000, this, SLOT(autoStart()));
}


USBWatcher::~USBWatcher()
{
	for (int i = 0; i < vHandle.count(); i++)
	{
		libusb_hotplug_deregister_callback(usb_ctx, vHandle.at(i));
	}

	vHandle.clear();

	libusb_exit(usb_ctx);

	exit(0);
}

void USBWatcher::autoStart()
{
	this->start();
}

void USBWatcher::run()
{
	hotplug_register();

	struct timeval zero_tv;
	zero_tv.tv_sec = 1; // timeout 1 sec
	zero_tv.tv_usec = 0;

	while (usb_thread_loop)
	{
		libusb_handle_events_timeout_completed(usb_ctx, const_cast<timeval *>(&zero_tv), NULL);
	}

	exit(0);
}


void USBWatcher::hotplug_register()
{
	usb_thread_loop = true;

	libusb_init(&usb_ctx);

	foreach (usb_data u, vUSB)
	{
		int rc;
		libusb_hotplug_callback_handle h;
		libusb_device_handle *devh;
		devh = libusb_open_device_with_vid_pid(usb_ctx, u.vendor, u.product);
		if (devh != NULL)   // connected
		{

#ifndef Q_OS_WIN32
			if (libusb_kernel_driver_active(devh, 0))
			{
				rc = libusb_detach_kernel_driver(devh, 0);

				if (rc)
				{
					qCritical("Failed to detach kernel driver: '%s'", strerror(-rc));
// 			CloseHandle();
// 			return -1;
				}
				else
				{
					qDebug() << "Detach kernel driver";
				}
			}
#endif
			quint16 v = u.vendor;
			quint16 p = u.product;

			// after detach from kernel it will automatically hotplugged from registered function
			// TODO what is under windows?
			emit USBConnected(v, p);
			libusb_close(devh);
		}

		rc = libusb_hotplug_register_callback(usb_ctx, (libusb_hotplug_event)(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
											  LIBUSB_HOTPLUG_ENUMERATE,
											  u.vendor,
											  u.product,
											  LIBUSB_HOTPLUG_MATCH_ANY,
											  hotplug_callback,
											  this,
											  &h);
		if (LIBUSB_SUCCESS != rc)
		{
			libusb_exit(usb_ctx);

			exit(0);
		}

		vHandle << h;
	}
}

