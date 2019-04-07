//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2019   Claudio Lanconelli                           //
//                                                                         //
//  https://github.com/lancos/ponyprog                                     //
//                                                                         //
//  LibUSB implementation for PonyProg (C) 2019 Eduard Kalinowski          //
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

#ifndef __USB_INTERFACE_H__
#define __USB_INTERFACE_H__


#include <libusb-1.0/libusb.h>

#define USB_MODE_NONE 0
#define USB_MODE_UART 1
#define USB_MODE_SPI  2
#define USB_MODE_I2C  3

class USB_Interface
{
  public:
	USB_Interface()
	{
		devHandle = 0;

		dtr = 0; // modem line
		rts = 0; // modem line

		rtsCtsEnabled = 0;
		dtrDsrEnabled = 0;

		// UART settings
		parity = 'N';
		bits = 8;
		stops = 1;
		flow_control = 0;
		baudRate = 9600;
		timeout = 300;

		// MODEM input lines
		ctsState = 0;
		dsrState = 0;
		dcdState = 0;
		ringState = 0;

		lcr = 0; // line control register
		lsr = 0; // line status register
		msr = 0; // modem status register
		mcr = 0; // modem control register
	};
// 	virtual ~USB_Interface();

	virtual int32_t Open(uint16_t vid, uint16_t pid) = 0;
	virtual void    Close() = 0;
	virtual int32_t Release(void) = 0;
#if 0
	virtual int32_t readEEPROM(uint8_t *buffer, uint32_t bytestoread);
	virtual int32_t writeEEPROM(uint8_t *buffer, uint32_t bytesum);
#endif
	virtual int32_t SetBaudRate(int32_t baudRate) = 0;
	virtual int32_t SetTimeouts(int16_t t) = 0;
	virtual int32_t SetBreakControl(int32_t state) = 0;
	virtual void    SetParity(uint8_t p) = 0;
	virtual void    SetBits(uint8_t b) = 0;
	virtual void    SetStops(uint8_t s) = 0;
	virtual void    SetFlowControl(uint8_t f) = 0;
	virtual int32_t SetDTR(int32_t dtr) = 0;
	virtual int32_t SetRTS(int32_t dtr) = 0;
	virtual int32_t GetDSR() = 0;
	virtual int32_t GetCTS() = 0;
	virtual int32_t SetRTSDTR(int state) = 0;
	virtual int32_t SetConfigLCR() = 0;
	virtual int32_t Probe() = 0;

// 	virtual int32_t GetStatus();
	virtual int32_t GetStatusRx() = 0;
	virtual int32_t GetStatusTx() = 0;
// 	virtual int32_t Read(uint8_t *buf, size_t len) = 0;
// 	virtual int32_t Write(uint8_t *buf, size_t len) = 0;

	virtual void    ReleaseInterface(void) = 0;
	virtual void    CloseHandle() = 0;

	virtual int32_t getState(void) = 0;
	virtual int32_t setControl(void) = 0;
	virtual int32_t init() = 0;

  public:
	struct libusb_device_handle *devHandle;

  protected:
	// MODEM output lines
	uint8_t dtr; // modem line
	uint8_t rts; // modem line

	uint8_t rtsCtsEnabled;
	uint8_t dtrDsrEnabled;

	// UART settings
	uint8_t parity;
	uint8_t bits;
	uint8_t stops;
	uint8_t flow_control;
	int32_t baudRate;
	int16_t timeout;

	// MODEM input lines
	uint8_t    ctsState;
	uint8_t    dsrState;
	uint8_t    dcdState;
	uint8_t    ringState;

	uint8_t lcr; // line control register
	uint8_t lsr; // line status register
	uint8_t msr; // modem status register
	uint8_t mcr; // modem control register
};


#endif
