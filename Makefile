#=======================================================================
#  Makefile for PonyProg (based on V lib makefile)
#  Copyright (C) 1997-2001 Claudio Lanconelli
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

CONFIG=v/Config.mk
include $(CONFIG)

PONYVER = 2.07c
TAR = tar

#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#Local configuration (changed from V Config)

Bin = ./bin

CFLAGS	+= -D_LINUX_ -Wall -fpermissive -Wno-deprecated -I$(LINUX_HEADERS)

#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

PROGNAME = ponyprog2000
PROG = $(Bin)/$(PROGNAME)

SRCS=\
	modaldlg_utilities.cpp \
	e2app.cpp \
	e2dlg.cpp \
	e2cnv.cpp \
	e2cmdw.cpp \
	e2awinfo.cpp \
	device.cpp \
	e24xx.cpp \
	e24xx-1.cpp \
	e24xx-2.cpp \
	e24xx-5.cpp \
	e2401.cpp \
	filebuf.cpp \
	e2pfbuf.cpp \
	binfbuf.cpp \
	busio.cpp \
	portint.cpp \
	pgminter.cpp \
	ponyioint.cpp \
	rs232int.cpp \
	easyi2c_interf.cpp \
	ispinterf.cpp \
	lptinterf.cpp \
	lpt_io_interf.cpp \
	lpt_ext_interf.cpp \
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
	interfconv.cpp \
	wait.cpp \
	microbus.cpp \
	at93cbus.cpp \
	at93cxx.cpp \
	at93cxx8.cpp \
	at250bus.cpp \
	at250bus2.cpp \
	at250xx.cpp \
	at25xxx.cpp \
	picbus.cpp \
	pic16xx.cpp \
	picbusnew.cpp \
	pic168xx.cpp \
	pic12bus.cpp \
	pic125xx.cpp \
	sde2506.cpp \
	sdebus.cpp \
	avr1200bus.cpp \
	filldlg.cpp \
	retrymdlg.cpp \
	fusemdlg.cpp \
	progoption.cpp \
	sernumdlg.cpp \
	aboutmdlg.cpp \
	sxbus.cpp \
	imbus.cpp \
	nvm3060.cpp \
	at17xxx.cpp \
	csmfbuf.cpp \
	x2444.cpp \
	x2444bus.cpp \
	dt006interf.cpp

OBJS = $(SRCS:.cpp=.o)
PRPS = $(SRCS:.cpp=.i)


#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.PHONY: default all objs clean cleanobj cleanall backup backuplink

default: vlib $(PROG)

vlib:
	cd v; $(MAKE) vlib

installLinuxElf:
	$(MAKE)	ARCH=linuxelf install

buildtar:
	cp $(PROG) $(INSTALL_PREFIX)/bin
	cd /; $(TAR) cvfzP ponyprog-$(PONYVER).tar.gz $(INSTALL_PREFIX)/bin/$(PROGNAME) $(INSTALL_PREFIX)/lib/libVx.*
	mv /ponyprog-$(PONYVER).tar.gz .
	chown lanconel.lanconel ponyprog-$(PONYVER).tar.gz

install:	$(PROG)
	chmod	755 $(PROG)
#	chmod	+s $(PROG)
	cp	$(PROG) $(INSTALL_PREFIX)/bin
	cd v; $(MAKE) install

distrib:
	rm -f *.gz
	rm -f string_table.h
	echo "Start build..."
	echo "Build SWEDISH ************************"
	cp string_table_swedish.h string_table.h
	touch string_table.h
	$(MAKE) clean
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Swedish
	gzip -9 PonyProg2000-Swedish
	rm -f string_table.h
	echo "Build POLISH ************************"
	cp string_table_polish.h string_table.h
	touch string_table.h
	$(MAKE) clean
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Polish
	gzip -9 PonyProg2000-Polish
	rm -f string_table.h
	echo "Build HUNGARIAN ************************"
	cp string_table_hungarian.h string_table.h
	touch string_table.h
	$(MAKE) clean
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Hungarian
	gzip -9 PonyProg2000-Hungarian
	rm -f string_table.h
	echo "Build CHINESE ************************"
	cp string_table_chinese.h string_table.h
	touch string_table.h
	$(MAKE) dep
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Chinese
	gzip -9 PonyProg2000-Chinese
	rm -f string_table.h
	echo "Build ROMANESTE ************************"
	cp string_table_romaneste.h string_table.h
	touch string_table.h
	$(MAKE) dep
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Romaneste
	gzip -9 PonyProg2000-Romaneste
	rm -f string_table.h
	echo "Build UKRAINIAN ************************"
	cp string_table_ukrainian.h string_table.h
	touch string_table.h
	$(MAKE) dep
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Ukrainian
	gzip -9 PonyProg2000-Ukrainian
	rm -f string_table.h
	echo "Build BULGARIAN ************************"
	cp string_table_bulgarian.h string_table.h
	touch string_table.h
	$(MAKE) dep
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Bulgarian
	gzip -9 PonyProg2000-Bulgarian
	rm -f string_table.h
	echo "Build BRAZILIAN ************************"
	cp string_table_brazilian.h string_table.h
	touch string_table.h
	$(MAKE) dep
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Brazilian
	gzip -9 PonyProg2000-Brazilian
	rm -f string_table.h
	echo "Build SRPSKI ************************"
	cp string_table_srpski.h string_table.h
	touch string_table.h
	$(MAKE) dep
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Srpski
	gzip -9 PonyProg2000-Srpski
	rm -f string_table.h
	echo "Build SLOVAK ************************"
	cp string_table_slovak.h string_table.h
	touch string_table.h
	$(MAKE) dep
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Slovak
	gzip -9 PonyProg2000-Slovak
	rm -f string_table.h
	echo "Build RUSSIAN ************************"
	cp string_table_russian.h string_table.h
	touch string_table.h
	$(MAKE) dep
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Russian
	gzip -9 PonyProg2000-Russian
	rm -f string_table.h
	echo "Build PORTUGUESE ************************"
	cp string_table_portuguese.h string_table.h
	touch string_table.h
	$(MAKE) dep
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Portuguese
	gzip -9 PonyProg2000-Portuguese
	rm -f string_table.h
	echo "Build FRANCAIS ************************"
	cp string_table_francais.h string_table.h
	touch string_table.h
	$(MAKE) dep
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Francais
	gzip -9 PonyProg2000-Francais
	rm -f string_table.h
	echo "Build SPANISH ************************"
	cp string_table_spanish.h string_table.h
	touch string_table.h
	$(MAKE) dep
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Spanish
	gzip -9 PonyProg2000-Spanish
	rm -f string_table.h
	echo "Build HOLLANDS ************************"
	cp string_table_nederlands.h string_table.h
	touch string_table.h
	$(MAKE) dep
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Nederlands
	gzip -9 PonyProg2000-Nederlands
	rm -f string_table.h
	cp string_table_italiano.h string_table.h
	echo "Build ITALIANO ************************"
	touch string_table.h
	$(MAKE) dep
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Italiano
	gzip -9 PonyProg2000-Italiano
	rm -f string_table.h
	cp string_table_deutsch.h string_table.h
	echo "Build DEUTSCH ************************"
	touch string_table.h
	$(MAKE) dep
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Deutsch
	gzip -9 PonyProg2000-Deutsch
	rm -f string_table.h
	cp string_table_slovenski.h  string_table.h
	echo "Build SLOVENSKI ************************"
	touch string_table.h
	$(MAKE) dep
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Slovenski
	gzip -9 PonyProg2000-Slovenski
	rm -f string_table.h
	cp string_table_hrvatski.h  string_table.h
	echo "Build HRVATSKI ************************"
	touch string_table.h
	$(MAKE) dep
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-Hrvatski
	gzip -9 PonyProg2000-Hrvatski
	rm -f string_table.h
	cp string_table_english.h  string_table.h
	echo "Build ENGLISH ************************"
	touch string_table.h
	$(MAKE) dep
	$(MAKE) -j5 $(PROG)
	mv $(PROG) PonyProg2000-English
	gzip -9 PonyProg2000-English

clean:
	cd v; $(MAKE) clean
	-rm -f $(CLEANEXTS)
	-rm -f dep.file

cleanobj:
	-rm -f $(OBJS)

cleanall: cleanobj clean
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

prps:	$(PRPS)

$(PROG):	$(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

%.i: %.cpp
	$(CXX) $(CFLAGS) -E $< >$@

dep:
	$(CXX) -MM $(CFLAGS) $(SRCS) >dep.file

#dep.file:	$(SRCS)
dep.file:
	$(CXX) -MM $(CFLAGS) $(SRCS) >dep.file

include dep.file
