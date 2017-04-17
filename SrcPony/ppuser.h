/*
 * $Id$
 * User-space parallel port device driver (header file).
 *
 * Copyright (C) 1998 Tim Waugh <tim@cyberelk.demon.co.uk>
 *
 * May be freely distributed as part of Linux
 */

#define PP_MAJOR	99

#define PP_IOCTL	'p'

/* Set mode to e.g. PARPORT_MODE_PCEPP */
#define PPSETMODE	_IOW(PP_IOCTL, 0x80, int)

/* Read/write status */
#define PPRSTATUS	_IOR(PP_IOCTL, 0x81, unsigned char)
#define PPWSTATUS	_IOW(PP_IOCTL, 0x82, unsigned char)

/* Read/write control */
#define PPRCONTROL	_IOR(PP_IOCTL, 0x83, unsigned char)
#define PPWCONTROL	_IOW(PP_IOCTL, 0x84, unsigned char)

/* Read/write data */
#define PPRDATA		_IOR(PP_IOCTL, 0x85, unsigned char)
#define PPWDATA		_IOW(PP_IOCTL, 0x86, unsigned char)

/* Read/write econtrol */
#define PPRECONTROL	_IOR(PP_IOCTL, 0x87, unsigned char)
#define PPWECONTROL	_IOW(PP_IOCTL, 0x88, unsigned char)

/* Read/write FIFO */
#define PPRFIFO		_IOR(PP_IOCTL, 0x89, unsigned char)
#define PPWFIFO		_IOW(PP_IOCTL, 0x8a, unsigned char)

/* Claim the port to start using it */
#define PPCLAIM		_IO(PP_IOCTL, 0x8b)

/* Release the port when you aren't using it */
#define PPRELEASE	_IO(PP_IOCTL, 0x8c)

/* Yield the port (release it if another driver is waiting,
 * then reclaim) */
#define PPYIELD		_IO(PP_IOCTL, 0x8d)
