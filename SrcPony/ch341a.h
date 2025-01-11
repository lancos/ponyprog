//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2025   Claudio Lanconelli                           //
//                                                                         //
//  https://github.com/lancos/ponyprog                                     //
//                                                                         //
//  LibUSB implementation for PonyProg (C) 2019 Eduard Kalinowski          //
//                                                                         //
//  sources:                                                               //
//  Copyright (C) 2014 Pluto Yang (yangyj.ee@gmail.com)                    //
//  https://github.com/setarcos/ch341prog                                  //
//                                                                         //
//  Copyright (C) 2016 Eugene Hutorny (eugene@hutorny.in.ua)               //
//  https://github.com/hutorny/usbuart                                     //
//                                                                         //
//  Copyright (c) 2017 Gunar Schorcht (gunar@schorcht.net)                 //
//  https://github.com/gschorcht/spi-ch341-usb                             //
//  https://github.com/gschorcht/i2c-ch341-usb                             //
//                                                                         //
//  Copyright (c) 2018 Sarim Khan  (sarim2005@gmail.com)                   //
//  https://github.com/sarim/ch341a-bitbang-userland                       //
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

#ifndef __CH341_H__
#define __CH341_H__

#include <libusb-1.0/libusb.h>

#include <QObject>
#include <QTimer>

// #define    CONTROL_WRITE_ENDPOINT          0x02
// #define    CONTROL_READ_ENDPOINT           0x82


#define    MAX_EEPROM_SIZE              131072 /* For 24c1024*/

#define    DEFAULT_CONFIGURATION        0x01
#define    DEFAULT_TIMEOUT              300    // 300mS for USB timeouts

#if 0
#define    IN_BUF_SZ                    0x100
#define    EEPROM_WRITE_BUF_SZ          0x2b   // only for 24c64 / 24c32 ??
#define    EEPROM_READ_BULKIN_BUF_SZ    0x20
#define    EEPROM_READ_BULKOUT_BUF_SZ   0x65
#endif

#define    CH341_PACKET_LENGTH          0x20
#define    CH341_MAX_PACKETS            256
#define    CH341_MAX_PACKET_LEN         (CH341_PACKET_LENGTH * CH341_MAX_PACKETS)

#define    CH34x_VENDOR_ID1             0x4348
#define    CH34x_VENDOR_ID2             0x1A86

#define    CH341A_PRODUCT               0x5512 // for SPI, I2C 

#define    CH341_UART_PRODUCT           0x5523
#define    CH340_UART_PRODUCT           0x7523



#if 0

#define    CH341A_CMD_SET_OUTPUT        0xA1
#define    CH341A_CMD_IO_ADDR           0xA2
#define    CH341A_CMD_PRINT_OUT         0xA3
#define    CH341A_CMD_SPI_STREAM        0xA8
#define    CH341A_CMD_SIO_STREAM        0xA9
#define    CH341A_CMD_I2C_STREAM        0xAA
#define    CH341A_CMD_UIO_STREAM        0xAB

#define    CH341A_CMD_I2C_STM_STA       0x74
#define    CH341A_CMD_I2C_STM_STO       0x75
#define    CH341A_CMD_I2C_STM_OUT       0x80
#define    CH341A_CMD_I2C_STM_IN        0xC0
#define    CH341A_CMD_I2C_STM_MAX       ( min( 0x3F, CH341_PACKET_LENGTH ) )
#define    CH341A_CMD_I2C_STM_SET       0x60
#define    CH341A_CMD_I2C_STM_US        0x40
#define    CH341A_CMD_I2C_STM_MS        0x50
#define    CH341A_CMD_I2C_STM_DLY       0x0F
#define    CH341A_CMD_I2C_STM_END       0x00

// GPIO part
#define    CH341A_CMD_UIO_STM_IN        0x00
#define    CH341A_CMD_UIO_STM_DIR       0x40
#define    CH341A_CMD_UIO_STM_OUT       0x80
#define    CH341A_CMD_UIO_STM_US        0xC0
#define    CH341A_CMD_UIO_STM_END       0x20

#define    CH341A_STM_I2C_20K           0x00
#define    CH341A_STM_I2C_100K          0x01
#define    CH341A_STM_I2C_400K          0x02
#define    CH341A_STM_I2C_750K          0x03
#define    CH341A_STM_SPI_DBL           0x04

#endif


/*******************************/
/* baudrate calculation factor */
/*******************************/
#define    CH341_BPS_MOD_BASE           20000000
#define    CH341_BPS_MOD_BASE_OFS       1100

//Vendor define
#define    CH341_REQ_WRITE_TYPE         0x40
#define    CH341_REQ_READ_TYPE          0xC0


#define    CH341_REG_STAT               0x0706
#define    CH341_REG_BREAK              0x1805
#define    CH341_REG_LCR                0x2518
#define    CH341_REG_FLOW_CTRL          0x2727
#define    CH341_REG_BAUD1              0x1312
#define    CH341_REG_BAUD2              0x0f2c  /*is it right, or 0x0f14 ?? */

#define    CH341_RESET_VALUE            0x501f    /* line mode? */
#define    CH341_RESET_INDEX            0xd90a    /* baud rate? */

#define    CH341_REQ_READ_VERSION       0x5F
#define    CH341_REQ_WRITE_REG          0x9A
#define    CH341_REQ_READ_REG           0x95
#define    CH341_REQ_SERIAL_INIT        0xA1
#define    CH341_REQ_MODEM_CTRL         0xA4  /* DTR and RTS */

// ??? right
#define    CH341_NBREAK_BITS_REG        0x4001

#define    CH341_NBREAK_BITS_REG1       0x01
#define    CH341_NBREAK_BITS_REG2       0x40

#define    CH341_INPUT_BUF_SIZE         0x08

/* status returned in third interrupt answer byte, inverted in data
   from irq */
//For CMD 0xA4 CH341_REQ_MODEM_CTRL
#define    CH341_UART_CTS               0x01
#define    CH341_UART_DSR               0x02
#define    CH341_UART_RING              0x04
#define    CH341_UART_DCD               0x08
#define    CH341_BITS_MODEM_STAT        0x0f /* all bits */

#define    CH341_MULT_STAT              0x04 /* multiple status since last interrupt event */

#define    CH341_CONTROL_OUT            0x10 // TX
#define    CH341_CONTROL_DTR            0x20 // DTR, 1 << 5
#define    CH341_CONTROL_RTS            0x40 // RTS, 1 << 6

//Uart state
#define    CH341_UART_STATE             0x00
#define    CH341_UART_OVERRUN_ERROR     0x01
#define    CH341_UART_BREAK_ERROR       //no define
#define    CH341_UART_PARITY_ERROR      0x02
#define    CH341_UART_FRAME_ERROR       0x06
#define    CH341_UART_RECV_ERROR        0x02
#define    CH341_UART_STATE_TRANSIENT_MASK    0x07

// LCR
#define    CH341_LCR_ENABLE_RX          0x80
#define    CH341_LCR_ENABLE_TX          0x40
#define    CH341_LCR_MARK_SPACE         0x20
#define    CH341_LCR_PAR_EVEN           0x10
#define    CH341_LCR_ENABLE_PAR         0x08
#define    CH341_LCR_STOP_BITS_2        0x04
#define    CH341_LCR_CS8                0x03
#define    CH341_LCR_CS7                0x02
#define    CH341_LCR_CS6                0x01
#define    CH341_LCR_CS5                0x00

//Flow control values
#define    CH34X_FLOW_CONTROL_NONE      0x0000
#define    CH34X_FLOW_CONTROL_RTS_CTS   0x0101
#define    CH34X_FLOW_CONTROL_DSR_DTR   0x0202

//Port state
#if 0
#define    CH341A_PORTA_STATE            0x01
#define    CH341A_PORTB_STATE            0x02
#define    CH341A_PORTC_STATE            0x03
#endif

//CH34x Baud Rate
#define    CH341_BAUDRATE_FACTOR        1532620800
#define    CH341_BAUDRATE_DIVMAX        3

#define    CH341_DATA_IN                (0x2|LIBUSB_ENDPOINT_IN)
#define    CH341_DATA_OUT               (0x2|LIBUSB_ENDPOINT_OUT)
#define    CTRL_IN                      (LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN )
#define    CTRL_OUT                     (LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT )
//#define    CTRL_IN                      (LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN | LIBUSB_RECIPIENT_INTERFACE)
//#define    CTRL_OUT                     (LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT | LIBUSB_RECIPIENT_INTERFACE)
#define    DEFAULT_BAUD_RATE            9600


// das hier ausprobieren
#define    CONTROL_REQUEST_TYPE_IN     ( LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE )
#define    CONTROL_REQUEST_TYPE_OUT    ( LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE )

#define    MAX_CONTROL_IN_TRANSFER_SIZE 2
#define    MAX_CONTROL_OUT_TRANSFER_SIZE 2

#if 0
// CH341a READ EEPROM setup packet for the 24c64
//   this needs putting into a struct to allow convenient access to individual elements

#define CH341_EEPROM_READ_SETUP_CMD \
	"\xaa\x74\x83\xa0\x00\x00\x74\x81\xa1\xe0\x00\x00\x06\x04\x00\x00" \
	"\x00\x00\x00\x00\x40\x00\x00\x00\x11\x4d\x40\x77\xcd\xab\xba\xdc" \
	"\xaa\xe0\x00\x00\xc4\xf1\x12\x00\x11\x4d\x40\x77\xf0\xf1\x12\x00" \
	"\xd9\x8b\x41\x7e\x00\xf0\xfd\x7f\xf0\xf1\x12\x00\x5a\x88\x41\x7e" \
	"\xaa\xe0\x00\x00\x2a\x88\x41\x7e\x06\x04\x00\x00\x11\x4d\x40\x77" \
	"\xe8\xf3\x12\x00\x14\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00" \
	"\xaa\xdf\xc0\x75\x00"

// for 24c64
#define CH341_EEPROM_READ_NEXT_CMD  \
	"\xaa\x74\x83\xa0\x00\x00\x74\x81\xa1\xe0\x00\x00\x10\x00\x00\x00" \
	"\x00\x00\x00\x00\x8c\xf1\x12\x00\x01\x00\x00\x00\x00\x00\x00\x00" \
	"\xaa\xe0\x00\x00\x4c\xf1\x12\x00\x5d\x22\xd7\x5a\xdc\xf1\x12\x00" \
	"\x8f\x04\x44\x7e\x30\x88\x41\x7e\xff\xff\xff\xff\x2a\x88\x41\x7e" \
	"\xaa\xe0\x00\x7e\x00\x00\x00\x00\x69\x0e\x3c\x00\x12\x01\x19\x00" \
	"\x0f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x9c\x2e\x68\x00" \
	"\xaa\xdf\xc0\x75\x00"

#define CH341_EEPROM_READ_CMD_SZ 0x65

#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

class ch341;


class ch341 : public QObject
{
	Q_OBJECT

  public:
	ch341(QObject *parent = 0);
	~ch341();

	int32_t Open(uint16_t vid, uint16_t pid);
	void    SetVerbose(void);
	void    Close();
#if 0
	int32_t SetStream(uint32_t speed);
	int32_t SpiCapacity(void);
	int32_t SpiRead(uint8_t *buf, uint32_t add, uint32_t len);
	int32_t ReadStatus(void);
	int32_t WriteStatus(uint8_t status);
	int32_t EraseChip(void);
	int32_t SpiWrite(uint8_t *buf, uint32_t add, uint32_t len);
#endif

	int32_t Release(void);
#if 0
	int32_t readEEPROM(uint8_t *buffer, uint32_t bytestoread);
	int32_t writeEEPROM(uint8_t *buffer, uint32_t bytesum);
#endif
	int32_t SetBaudRate(int32_t baudRate);
	int32_t SetTimeouts(int16_t t);
	int32_t SetBreakControl(int32_t state);
	void    SetParity(uint8_t p);
	void    SetBits(uint8_t b);
	void    SetStops(uint8_t s);
	void    SetFlowControl(uint8_t f);
	int32_t SetDTR(int32_t dtr);
	int32_t SetRTS(int32_t dtr);
	int32_t GetDSR();
	int32_t GetCTS();
	int32_t SetRTSDTR(int state);
	int32_t SetConfigLCR();
	int32_t Probe();
	int32_t ClearChip();
	int32_t ResetChip();

// 	int32_t GetStatus();
	int32_t GetStatusRx();
	int32_t GetStatusTx();
// 	int32_t Read(uint8_t *buf, size_t len);
// 	int32_t Write(uint8_t *buf, size_t len);

	void    ReleaseInterface(void);
	void    CloseHandle();

  signals:
// 	void receivedData(uint8_t *data, size_t len);
	void breakChanged(bool set);

  public slots:
// 	void onRecieiveData(quint8 *data, qint16 len);
// 	void sendData(const uint8_t &data, size_t len);
	void triggerBreak(uint msecs);

  private slots:
	void breakTimeout();

  public:
	// for callback
	static int16_t read_completed;
	static int16_t write_completed;

	struct libusb_device_handle *devHandle;
//     struct libusb_transfer *ctrl_transfer;

  private:
	int32_t getModemState(void);
	int32_t setHandshakeByte(void);
	int32_t init();
// 	void    updateStatus();
	void    v_print(int mode, int len);
//     void    allocTransfer();


// 	int setAsync(unsigned char data);
#if 0
	void    updateStatus(uint8_t *data, size_t l);

	void    SpiChipSelect(uint8_t *ptr, bool selected);
	int32_t SpiStream(uint8_t *out, uint8_t *in, uint32_t len);
	int32_t usbTransfer(const char *func, uint8_t type, uint8_t *buf, int len);
#endif

  private:
	struct dv
	{
		uint8_t	dv_prescaler;
		uint8_t	dv_div;
		uint8_t	dv_mod;
	};

	struct uart_div
	{
		uint32_t dvr_high;
		uint32_t dvr_low;
		uint32_t dvr_base_clock;
		struct dv dvr_divider;
	};

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

//     struct libusb_transfer *ctrl_transfer;

//     uint8_t ctrl_buf[LIBUSB_CONTROL_SETUP_SIZE];

	// MODEM input lines
	uint8_t    ctsState;
	uint8_t    dsrState;
	uint8_t    dcdState;
	uint8_t    ringState;

	uint8_t lcr; // line control register
	uint8_t lsr; // line status register
	uint8_t msr; // modem status register
	uint8_t mcr; // modem control register

	uint8_t dev_vers;

	bool verbose;

	QTimer *breakTimer;
	int force_stop = 0;
};


#endif
