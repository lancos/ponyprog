#===============================================================
# V Make Configuration file - Version 1.24 - 3/3/2000
#
# Copyright (C) 1995-2000  Bruce E. Wampler
# date:
#
# THIS IS THE Config.mk FILE FOR THE X Athena VERSION OF V
#
# To build an X version, set the TOOLKIT variable to Athena
# for the generic Athena version, or to Motif
# for the Lesstif/Motif version. (See comment below)
#
# This file is part of the V C++ GUI Framework, and is covered
# under the terms of the GNU Lesser General Public License,
# Version 2.1. This library has NO WARRANTY. See the source file
# vapp.cxx for more complete information about license terms.
#===============================================================


#---------------------------------------------------------------------
# Version info
#---------------------------------------------------------------------
VV	=	1.25
VVW	=	125


#---------------------------------------------------------------------
# HOMEV info
#---------------------------------------------------------------------
#HOMEV	=	$(HOME)/v
#HOMEV	=	..
HOMEV	=	$(HOME)/Progetti/PonyProg_Sourceforge/software/v

LINUX_HEADERS	=	/usr/src/linux-headers-2.6.22-14-generic/include

INSTALL_PREFIX	=	/usr/local

#---------------------------------------------------------------------
# Tools used in the makefile execution
#---------------------------------------------------------------------
#CC	=	gcc
#CXX	=	g++
CC	=	gcc-3.4
CXX	=	g++-3.4

#---------------------------------------------------------------------
# VPATH for dependencies on header files
#---------------------------------------------------------------------
VPATH=$(HOMEV)/includex/v


#---------------------------------------------------------------------
# Select the architecture of your system.
# These are the architectures that V has been extensively tested with:
# linux, linuelf, sun4, mips, sgi
#
# User contributed definitions are available for:
# hpux, aix, solaris, bsd
#---------------------------------------------------------------------
ARCH	= linux
#ARCH	= linuxelf
#ARCH	= sun4
#ARCH	= mips
#ARCH	= sgi
#ARCH	= hpux
#ARCH	= aix
#ARCH	= solaris
#ARCH	= bsd
Arch = $(ARCH)


#---------------------------------------------------------------------
# Select Athena or Motif TOOLKIT option.
#---------------------------------------------------------------------
TOOLKIT		=	Athena
#TOOLKIT	=	Motif

#---------------------------------------------------------------------
# Use this define for NO 3D on Athena Widgets
# For Motif use NO 3D i.e. USE_3D=no.
#---------------------------------------------------------------------
ifeq ($(TOOLKIT),Motif)
USE_3D = no
else
USE_3D = yes
endif


#---------------------------------------------------------------------
# Select X11R-version
#---------------------------------------------------------------------
#X11RV	= X11R5
X11RV	= X11R6

# where X11 files are
Xdir11	= X11R6
#Xdir11	= X11


#---------------------------------------------------------------------
# Select Debug or no Debug
#---------------------------------------------------------------------
#DEBUG	= yes
DEBUG	= no


#---------------------------------------------------------------------
# Select Develop or not - used for original development
#---------------------------------------------------------------------
#DEVEL	=	yes
DEVEL	= no

#---------------------------------------------------------------------
# Define filename extensions that are targeted by cleanup's
#---------------------------------------------------------------------
CLEANEXTS= *.bak *~ *.tmp core *.o *.obj


#---------------------------------------------------------------------
# Architecture dependent directory locations
#---------------------------------------------------------------------
VLibDir	=	$(HOMEV)/lib
ifeq ($(TOOLKIT),Motif)
oDir	=	$(HOMEV)/obj
endif
ifeq ($(TOOLKIT),Athena)
oDir	=	$(HOMEV)/objx
endif
LibDir	=	$(HOMEV)/lib
Bin	=	$(HOMEV)/bin


#---------------------------------------------------------------------
# Architecture independent 
#---------------------------------------------------------------------
INCDIR	=	-I$(HOMEV)/includex
LIBDIR	=	-L$(LibDir)
ifeq ($(TOOLKIT),Athena)
	LIBNAME	= libVx
	V1NAME 	= v1x
	LIBS	= -lVx -lXaw
endif
ifeq ($(TOOLKIT),Motif)
	LIBNAME	= libV
	V1NAME 	= v1
	LIBS	= -lV -lXm -lXp
endif


#---------------------------------------------------------------------
# Linux
#---------------------------------------------------------------------
ifeq ($(Arch),linux)
INCDIR	+=	-I/usr/$(X11RV)/include
LIBDIR	+=	-L/usr/$(X11RV)/lib
LIBS	+=	-lXmu -lXt -lXext -lX11
TARZ	=	z
RANLIB	=	ranlib
VGPATH	=	LINUX
endif


#---------------------------------------------------------------------
# Linux ELF shared library
#---------------------------------------------------------------------
ifeq ($(Arch),linuxelf)
INCDIR	+=	-I/usr/$(X11RV)/include
LIBDIR	+=	-L/usr/$(X11RV)/lib
LIBS	+=	-lXmu -lXt -lXext -lX11
TARZ	=	z
RANLIB	=	ranlib
VGPATH	=	LINUX
endif


#---------------------------------------------------------------------
# Sun4 - unm
#---------------------------------------------------------------------
ifeq ($(TOOLKIT),Athena)
ifeq ($(Arch),sun4)
INCDIR	+=	-I/usr/local/X11/include
LIBDIR	+=	-L/usr/local/X11/lib
LIBS	+=	-lXaw -lXmu -lXt -lXext -lX11
TARZ	=
RANLIB	=	ranlib
VGPATH	=	UNIX
endif
endif


#----------------------------------------------------------------------
# solaris - shared Motif/CDE libraries
#----------------------------------------------------------------------
ifeq ($(TOOLKIT),Motif)
ifeq ($(Arch),solaris)
INCDIR  +=      -I/usr/openwin/include -I/usr/dt/share/include -I$(HOMEV)/includex
LIBDIR  +=      -R/usr/dt/lib:/usr/openwin/lib -L/usr/dt/lib
LIBS    +=      -lXmu -lXt -lXext -lX11 -lnsl -lsocket
TARZ    =
RANLIB  =       true
VGPATH  =       UNIX
endif
endif

#---------------------------------------------------------------------
# mips - unm
#---------------------------------------------------------------------
ifeq ($(Arch),mips)
INCDIR	+=	-I/usr/local/X11/include
LIBDIR	+=	-L/usr/local/X11/lib
LIBS	+=	-lXaw -lXmu -lXt -lXext -lX11
TARZ	=
RANLIB	=	ranlib
VGPATH	=	UNIX
endif

#---------------------------------------------------------------------
# sgi
#	- the native compiler on IRIX 5.3 complains ad nauseam
#         about parameters that are declared but not referenced
#         in virtual functions; the -woff turns that warning off
#---------------------------------------------------------------------
ifeq ($(Arch),sgi)
INCDIR	+=	-I/usr/include
LIBDIR	+=	-L/usr/lib
LIBS	+=	-lXmu -lXt -lXext -lX11 -lm
TARZ	=
RANLIB	=	true
VGPATH	=	UNIX
ifneq ($(CXX),g++)
	CFLAGS += -woff 3262
endif
endif

#---------------------------------------------------------------------
# hpux:
# 	- /usr/include is included explicitly because on HP-UX 10.20
#	  mixing of GNU supplied and system supplied includes leads to
#	  problems
#       - X11 is available either in /usr/contrib or /usr/local
#         listing both can not hurt 
#----------------------------------------------------------------------
ifeq ($(Arch),hpux)
INCDIR	+=	-I/usr/include -I/usr/include/$(X11RV) \
		-I/usr/contrib/$(X11RV)/include \
		-I/usr/local/$(X11RV)/include
LIBDIR	+=	-L/usr/lib/$(X11RV) -L/usr/contrib/$(X11RV)/lib \
		-L/usr/local/$(X11RV)/lib
LIBS    +=	-lXmu -lXt -lSM -lICE -lXext -lX11
TARZ	=
RANLIB	=       true
VGPATH	=	UNIX
endif

#----------------------------------------------------------------------
# solaris - unm
#----------------------------------------------------------------------
# Note: For Solaris CC Workshop comiplers and OpenWindows linking:
#LIBDIR += -L/usr/openwin/lib -R/usr/openwin/lib
ifeq ($(Arch),solaris)
INCDIR	+=	-I/usr/local/X11/include -I$(HOMEV)/includex
LIBDIR	+=	-L/usr/local/X11/lib
LIBS	+=	-lXmu -lXt -lXext -lX11 -lnsl -lsocket
TARZ	=
RANLIB	=	true
VGPATH	=	UNIX
endif


#----------------------------------------------------------------------
# FreeBSD with gcc
#----------------------------------------------------------------------
ifeq ($(Arch),bsd)
INCDIR  +=       -I/usr/$(X11RV)/include
LIBDIR  +=       -L/usr/$(X11RV)/lib
LIBS    +=       -lV -lXmu -lXt -lXext -lX11 -lxm
TARZ    =
RANLIB  =       ranlib
VGPATH	=	UNIX
endif


#---------------------------------------------------------------------
# C/C++ compile options
#---------------------------------------------------------------------
#CFLAGS	+= -v $(INCDIR) 
CFLAGS	+= $(INCDIR) 
ifeq ($(DEBUG),no)
CFLAGS	+=	-O2
endif
ifeq ($(DEBUG),yes)
CFLAGS	+=	-g
endif
ifeq ($(TOOLKIT),Athena)
CFLAGS	+=	-DAthena
endif
ifeq ($(DEVEL),yes)
CFLAGS	+=	-DDEVEL
endif


#---------------------------------------------------------------------
# LINK/LOAD options
#---------------------------------------------------------------------
LDFLAGS	=	$(LIBDIR) $(LIBS)

