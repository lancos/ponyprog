#=======================================================================
#  Makefile for PonyProg (based on V lib makefile)
#  Copyright (C) 1997 Claudio Lanconelli
#
#  This program is part of PonyProg
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#=======================================================================

CONFIG=Config.mk
include $(CONFIG)

#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#Local configuration (changed from V Config)

oDir = .
Bin = .

CFLAGS	+= -D_LINUX_ -D_PONYPROG_ -D_UDP_SERVER -Wall

#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

PROG= $(Bin)/ponyprog

OBJS=\
        $(oDir)/e2app.o \
        $(oDir)/e2dlg.o \
        $(oDir)/e2cnv.o \
        $(oDir)/e2cmdw.o \
        $(oDir)/e2awinfo.o \
        $(oDir)/eeprom.o \
        $(oDir)/e24xx.o \
        $(oDir)/filebuf.o \
        $(oDir)/e2pfbuf.o \
        $(oDir)/hexfbuf.o \
        $(oDir)/busio.o \
        $(oDir)/portint.o \
        $(oDir)/pgminter.o \
        $(oDir)/ponyioint.o \
	$(oDir)/rs232int.o \
        $(oDir)/easyi2c_com.o \
	$(oDir)/easyi2c_lpt.o \
	$(oDir)/ispinterf.o \
	$(oDir)/lptinterf.o \
	$(oDir)/lpt_io_interf.o \
        $(oDir)/i2cbus.o \
        $(oDir)/crc.o \
        $(oDir)/infomdlg.o \
        $(oDir)/eeptypes.o \
	$(oDir)/profile.o \
	$(oDir)/e2profil.o \
	$(oDir)/intfbuf.o \
	$(oDir)/motsfbuf.o \
	$(oDir)/spi-bus.o \
	$(oDir)/at90sbus.o \
	$(oDir)/atmegabus.o \
	$(oDir)/at90sxx.o \
	$(oDir)/at89sbus.o \
	$(oDir)/at89sxx.o \
	$(oDir)/e24xx-2.o \
	$(oDir)/e24xx-5.o \
	$(oDir)/interfconv.o \
	$(oDir)/wait.o \
	$(oDir)/microbus.o \
	$(oDir)/at93cbus.o \
	$(oDir)/at93cxx.o \
	$(oDir)/at250bus.o \
	$(oDir)/at250bus2.o \
	$(oDir)/at250xx.o \
	$(oDir)/at25xxx.o \
	$(oDir)/picbus.o \
	$(oDir)/pic16xx.o \
	$(oDir)/sdebus.o \
	$(oDir)/sde2506.o \
	$(oDir)/avr1200bus.o \
	$(oDir)/filldlg.o \
	$(oDir)/retrymdlg.o \
	$(oDir)/fusemdlg.o \
	$(oDir)/sxbus.o \
	$(oDir)/socksrv.o

SRCS=\
        e2app.cpp \
        e2dlg.cpp \
        e2cnv.cpp \
        e2cmdw.cpp \
        e2awinfo.cpp \
        eeprom.cpp \
        e24xx.cpp \
        filebuf.cpp \
        e2pfbuf.cpp \
        hexfbuf.cpp \
        busio.cpp \
        portint.cpp \
        pgminter.cpp \
        ponyioint.cpp \
	rs232int.cpp \
        easyi2c_com.cpp \
	easyi2c_lpt.cpp \
	ispinterf.cpp \
	lptinterf.cpp \
	lpt_io_interf.cpp \
        i2cbus.cpp \
        crc.cpp \
        infomdlg.cpp \
        eeptypes.cpp \
	profile.cpp \
	e2profil.cpp \
	intfbuf.cpp \
	motsfbuf.cpp \
	spi-bus.cpp \
	at90sbus.cpp \
	atmegabus.cpp \
	at90sxx.cpp \
	at89sbus.cpp \
	at89sxx.cpp \
	e24xx-2.cpp \
	e24xx-5.cpp \
	interfconv.cpp \
	wait.cpp \
	microbus.cpp \
	at93cbus.cpp \
	at93cxx.cpp \
	at250bus.cpp \
	at250bus2.cpp \
	at250xx.cpp \
	at25xxx.cpp \
	picbus.cpp \
	pic16xx.cpp \
	sde2506.cpp \
	sdebus.cpp \
	avr1200bus.cpp \
	filldlg.cpp \
	retrymdlg.cpp \
	fusemdlg.cpp \
	sxbus.cpp \
	socksrv.cpp


#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.PHONY: default all objs clean cleanobj cleanall backup backuplink dep

default:	$(PROG)

all:
	$(MAKE) dep
	$(MAKE) $(PROG)

linux linuxelf:
	$(MAKE) ARCH=$@ $(PROG)

installLinux:
	$(MAKE)	ARCH=linux install

installLinuxElf:
	$(MAKE)	ARCH=linuxelf install

install:	$(PROG)
	chmod	755 $(PROG)
	chmod	+s $(PROG)
	cp $(PROG) /usr/local/bin

clean:
	-rm -f $(OBJS)
	-rm -f $(CLEANEXTS)

cleanobj: clean
	-rm -f $(OBJS)

cleanall: cleanobj
	-rm -f $(PROG)


backup:
	rm -f ../ponyprog.tar.gz
	cd obj/linux; rm -f *.o
	cd obj/linuxelf; rm -f *.o
	cd ..; tar cvfhz ponyprog.tar.gz ponyprog/*.cpp ponyprog/*.h ponyprog/Makefile ponyprog/Config.mk ponyprog/COPYING ponyprog/README ponyprog/icons ponyprog/obj ponyprog/bin
	chmod 640 ../ponyprog.tar.gz
	cd ..; mv ponyprog.tar.gz backups/ponyprog_`date +%d%h%y`.tar.gz
	cd ..; ln -s backups/ponyprog_`date +%d%h%y`.tar.gz ponyprog.tar.gz

backuplink:
	rm -f ../ponyproglink.tar.gz
	cd ..; tar cvfz ponyproglink.tar.gz ponyprog/*.cpp ponyprog/*.h ponyprog/Makefile ponyprog/Config.mk ponyprog/icons
	chmod 640 ../ponyproglink.tar.gz
	cd ..; mv ponyproglink.tar.gz backups/ponyproglink_`date +%d%h%y`.tar.gz
	cd ..; ln -s backups/ponyproglink_`date +%d%h%y`.tar.gz ponyproglink.tar.gz

objs:	$(OBJS)

$(PROG):	$(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

$(oDir)/%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

#%.d: %.cpp
#	$(SHELL) -ec '$(CXX) -MM $(CFLAGS) $< \
#		| sed '\''s/\($*\)\.o[ :]*/$(soDir)\/\1\.o $@: /g'\'' > $@'

%.d: %.cpp
	$(SHELL) -ec '$(CXX) -MM $(CFLAGS) $< \
		| sed '\''s/\($*\)\.o[ :]*/$(oDir)\/\1\.o $@: /g'\'' > $@'

dep:
	$(CXX) -MM $(CFLAGS) $(SRCS) > dep.file

dep.file:	$(SRCS)
	$(CXX) -MM $(CFLAGS) $(SRCS) > dep.file

#
# include a dependency files
#
#include $(SRCS:.cpp=.d)
include dep.file
