//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2019   Claudio Lanconelli                           //
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

#ifndef _I2CINTERFACE_H
#define _I2CINTERFACE_H


#include <libusb-1.0/libusb.h>

#include "ch341a.h"
#include "types.h"
#include "errcode.h"
#include "wait.h"

#include <QDebug>

//Some useful flags
#define SPIMODE_CPHA		0x01		// clock phase/edge
#define SPIMODE_CPOL		0x02		// clock polarity
#define SPIMODE_MASK		0x03

#define xMODE_RDONLY		0x04
#define xMODE_WRONLY		0x08

//#define I2CMODE_MASK		0x10

enum
{
	SPI_MODE_0 = (0 | 0),
	SPI_MODE_1 = (0 | SPIMODE_CPHA),
	SPI_MODE_2 = (SPIMODE_CPOL | 0),
	SPI_MODE_3 = (SPIMODE_CPOL | SPIMODE_CPHA)
};

#define SCLTIMEOUT      900     // enable SCL check and timing (for slaves that hold down the SCL line to slow the transfer)

class BusInterface
{
  public:
	BusInterface()
		: old_portno(-1),
		  usb_vid(0),
		  usb_pid(0),
		  installed(-1),
		  cmd2cmd_delay(0),
		  shot_delay(5),
		  i2c_mode(false),
		  spi_mode(false)
	{
	}

	virtual int Open(int port) = 0;
	virtual void Close() = 0;

	// for usb devices
	virtual int OpenUSB(uint16_t vid, uint16_t pid)
	{
		int ret_val = E2ERR_OPENFAILED;

		if (vid > 0 && pid > 0)
		{
			ret_val = GetUSBInterface()->Open(vid, pid); // or depended from selected

			if (ret_val == OK)
			{
				usb_vid = vid;
				usb_pid = pid;
			}
			else
			{
				usb_vid = 0;
				usb_pid = 0;
			}
		}
		return ret_val;
	}

	virtual int32_t StreamSPI(unsigned long chip_select, unsigned long length, unsigned char *buffer, unsigned char *buffer2)
	{
		int ret_val = GetUSBInterface()->StreamSPI(chip_select, length, buffer, buffer2);
		return ret_val;
	}

	virtual int32_t StreamI2C(uint iWriteLength, uint *iWriteBuffer, uint iReadLength, uint *oReadBuffer)
	{
		int ret_val = GetUSBInterface()->StreamI2C(iWriteLength, iWriteBuffer, iReadLength, oReadBuffer);
		return ret_val;
	}

	virtual int SetUSBSpeed(int spd)
	{
		int ret_val = GetUSBInterface()->SetStreamSpeed(spd);
		return ret_val;
	}

	virtual int TestOpen(int port)
	{
		qDebug() << __PRETTY_FUNCTION__ << "(" << port << ") IN";

		int ret_val = TestSave(port);

		TestRestore();

		qDebug() << __PRETTY_FUNCTION__ << "=" << ret_val << " OUT";

		return ret_val;
	}
	virtual int TestPort(int port)
	{
		qDebug() << __PRETTY_FUNCTION__ << "(" << port << ") IN";

		return TestOpen(port);
	}
	virtual int TestSave(int port)
	{
		int ret_val;

		qDebug() << __PRETTY_FUNCTION__ << "(" << port << ") IN";

		old_portno = installed;

		Close();

		if ((ret_val = Open(port)) == OK)
		{
		}

		qDebug() << __PRETTY_FUNCTION__ << "=" << ret_val << " OUT";

		return ret_val;
	}
	virtual void TestRestore()
	{
		qDebug() << __PRETTY_FUNCTION__ << "IN *** Inst=" << installed;

		if (IsInstalled())
		{
			Close();
		}

		if (old_portno >= 0)
		{
			Open(old_portno);
			old_portno = -1;
		}

		qDebug() << __PRETTY_FUNCTION__ << "OUT";
	}

	virtual int SetPower(bool onoff)
	{
		return OK;
	}
	virtual void SetControlLine(int res = 1)
	{
	}

	virtual void SetDataOut(int sda = 1) = 0;
	virtual void SetInvDataOut(int sda = 1)
	{
		SetDataOut(!sda);
	}
	virtual void SetClock(int scl = 1) = 0;
	virtual int GetDataIn() = 0;
	virtual int GetClock() = 0;
	virtual void SetClockData() = 0;
	virtual void ClearClockData()
	{ }
	virtual int IsClockDataUP() = 0;
	virtual int IsClockDataDOWN() = 0;

	int GetCmd2CmdDelay() const
	{
		return cmd2cmd_delay;
	}
	void SetCmd2CmdDelay(int delay)
	{
		if (delay >= 0)
		{
			cmd2cmd_delay = delay;
		}
	}

	bool IsInstalled() const
	{
		// high prio
		if (usb_pid > 0 && usb_vid > 0)
		{
			return true;
		}

		return (installed >= 0) ? true : false;
	}

	void SetUSB(int vid, int pid)
	{
		usb_vid = vid;
		usb_pid = pid;
	}

	void GetUSB(int &vid, int &pid)
	{
		vid = usb_vid;
		pid = usb_pid;
	}



	virtual void WaitMsec(unsigned int msec)
	{
		Flush();
		w.WaitMsec(msec);
	}

	virtual void WaitUsec(unsigned int usec)
	{
		Flush();
		w.WaitUsec(usec);
	}

	virtual void ShotDelay(int n = 1)
	{
		w.WaitUsec(shot_delay * n);
	}

	virtual int xferBit(int &err, int b, int mode = 0)
	{
		int ret = 0;

		if (!i2c_mode)	//(mode & I2CMODE_MASK) == 0)
		{
			switch (mode & SPIMODE_MASK)
			{
			case 3:
				SetClock(0);
				SetDataOut(b);
				ShotDelay();
				SetClock(1);
				if ((mode & xMODE_WRONLY) == 0)
				{
					ret = GetDataIn();
				}
				ShotDelay();
				break;
			case 2:
				SetDataOut(b);
				ShotDelay();
				SetClock(0);
				if ((mode & xMODE_WRONLY) == 0)
				{
					ret = GetDataIn();
				}
				ShotDelay();
				SetClock(1);
				break;
			case 1:
				SetClock(1);
				SetDataOut(b);
				ShotDelay();
				SetClock(0);
				if ((mode & xMODE_WRONLY) == 0)
				{
					ret = GetDataIn();
				}
				ShotDelay();
				break;
			case 0:
			default:
				SetDataOut(b);
				ShotDelay();
				SetClock(1);
				if ((mode & xMODE_WRONLY) == 0)
				{
					ret = GetDataIn();
				}
				ShotDelay();
				SetClock(0);
				break;
			}
		}
		else
		{
			//I2CBus
			SetDataOut(b);		// SDA must be high to receive data (low dominant)
			ShotDelay();		// tSU;DAT = 250 nsec (tLOW / 2 = 2 usec)
			SetClock(1);
#ifdef SCLTIMEOUT
			for (int k = SCLTIMEOUT; GetClock() == 0 && k > 0; k--)
			{
				WaitUsec(1);
			}
			if (GetClock() == 0)
			{
				return IICERR_SCLCONFLICT;
			}
#endif
			ShotDelay();		// tHIGH / 2 = 2 usec
			if ((mode & xMODE_WRONLY) == 0)
			{
				ret = GetDataIn();
			}
			ShotDelay();		// tHIGH / 2 = 2 usec
			SetClock(0);
			ShotDelay();		// tHD;DATA = 300 nsec (tLOW / 2 = 2 usec)
		}
		err = OK;
		return ret;
	}

	virtual uint8_t xferByte(int &err, uint8_t by, int mode = 0, int bpw = 8, bool lsb_first = false)
	{
		return (uint8_t)xferWord(err, by, mode, bpw, lsb_first);
	}

	virtual unsigned long xferWord(int &err, unsigned long word_out, int mode = 0, int bpw = 8, bool lsb_first = false)
	{
		uint32_t word_in = 0;
		uint32_t bitmask;
		err = OK;

		if (!i2c_mode)	//if ((mode & I2CMODE_MASK) == 0)
		{
			switch (mode & SPIMODE_MASK)
			{
			case 3:
			case 2:
				SetClock(1);
				break;
			case 1:
			case 0:
			default:
				SetClock(0);
				break;
			}
		}

		if (lsb_first)
		{
			bitmask = 1;
		}
		else
		{
			bitmask = 1 << (bpw - 1);
		}

		for (int k = 0; k < bpw; k++)
		{
			if (xferBit(err, word_out & bitmask, mode))
			{
				word_in |= bitmask;
			}

			if (lsb_first)
			{
				bitmask <<= 1;
			}
			else
			{
				bitmask >>= 1;
			}
		}
		SetDataOut(1);

		return word_in;
	}

	virtual void SetDelay(int delay)
	{
		if (delay >= 0)
		{
			shot_delay = delay;
		}
	}
	int GetDelay() const
	{
		return shot_delay;
	}

	// we are working with USB_Interface pointer because of it is
	// the abstract basic class for different supported chips
	USB_Interface *GetUSBInterface() const
	{
		// return common interface, abstract class based
		// TODO to check the VID, PID
		// if (usb_vid == 0x111 && usb_pid == 0x222)
		// {
		//
		// }

		return (USB_Interface *)&usbI;
	}

	void SetI2CMode(bool mode)
	{
		i2c_mode = mode;
		if (mode)
		{
			spi_mode = false;
		}
	}
	int GetI2CMode() const
	{
		return i2c_mode;
	}

	/**
	 * @brief write I2C or SPI directly to usb chip
	 *        they must be supported from chip: ch341a
	 */
	int WriteStream(uint8_t const *data, long length, int page_size = 0)
	{
		int ret = -1;
		if (usb_vid == 0 || usb_pid == 0)
		{
			return ret;
		}

		if (i2c_mode)
		{
			// usbI direct stream
			usbI.SetChipMode(USB_MODE_I2C);
			usbI.SetStreamSpeed(USB_DEFAULT_RATE);
			ret = usbI.Write(data, length);
		}

		if (spi_mode)
		{
			// usbI direct stream
			usbI.SetChipMode(USB_MODE_SPI);
			usbI.SetStreamSpeed(USB_DEFAULT_RATE);
			ret = usbI.Write(data, length);
		}
		return ret;
	}
	/**
	 * @brief read I2C or SPI directly from usb chip
	 *        they must be supported from chip: ch341a
	 */
	int ReadStream(uint8_t *data, long length, int page_size = 0)
	{
		int ret = -1;
		if (usb_vid == 0 || usb_pid == 0)
		{
			return ret;
		}

		if (i2c_mode)
		{
			// usbI direct stream
			usbI.SetChipMode(USB_MODE_I2C);
			usbI.SetStreamSpeed(USB_DEFAULT_RATE);
			ret = usbI.Read(data, length);
		}
		if (spi_mode)
		{
			// usbI direct stream
			usbI.SetChipMode(USB_MODE_SPI);
			usbI.SetStreamSpeed(USB_DEFAULT_RATE);
			ret = usbI.Read(data, length);
		}
		return ret;
	}

	void SetSPIMode(bool mode)
	{
		spi_mode = mode;
		if (mode)
		{
			i2c_mode = false;
		}
	}
	int GetSPIMode() const
	{
		return spi_mode;
	}

  protected:
	void Install(int val)
	{
		installed = val;
	}

	void DeInstall()
	{
		installed = -1;
		usb_pid = 0;
		usb_vid = 0;
	}

	int GetInstalled() const
	{
		return installed;
	}

	virtual int Flush()
	{
		return OK;
	}

	int old_portno;             // TestSave() save the status here
	int usb_vid;
	int usb_pid;



  private:
	int installed;              // -1 --> not installed, >= 0 number if the installed port
	int cmd2cmd_delay;			// <> 0 if a delay between commands is needed
	unsigned int shot_delay;	//delay unit to perform bus timing
	bool i2c_mode;
	bool spi_mode;

	/* supported device */
	ch341 usbI;

	Wait w;
};

#endif
