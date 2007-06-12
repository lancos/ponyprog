#===============================================================
# V Makefile - Version 1.25 - 22Sep00
#
# Copyright (C) 1995-2000  Bruce E. Wampler
#
# This file is part of the V C++ GUI Framework, and is covered
# under the terms of the GNU Library General Public License,
# Version 2. This library has NO WARRANTY. See the source file
# vapp.cxx for more complete information about license terms.
#
# Many thanks to Jan Broeckhove for contributing this
# set of makefiles, which are the standard method of building
# V beginning with Version 1.16.
#
# Support for Win 3.1 removed effective 1.22
#===============================================================
#
# IMPORTANT NOTE:
# This Makefile is the top level makefile used to build V on
# various Unix-like platforms. First and foremost, this Makefile
# requires a version of make compatible with Gnu make. One
# specific requirement is VPATH support, for example. There
# are other incompatibilities with gmake and the old standard
# Unix make.
#
# Also, all the Makefiles in this version are controlled by
# the file Config.mk. It defines various directory locations
# and things needed to make V work correctly. You should only
# have to edit Config.mk to get V compiled correctly.
#
# There are TWO versions of Config.mk - one for the standard
# Athena widget version of V, and one for the Motif version.
# To build the Athena version, copy ConfigX.mk to Config.mk
# and edit for your system. To build the Motif version,
# copy ConfigM.mk to Config.mk and edit for your system.
#

CONFIG = ./Config.mk
include $(CONFIG)

#===============================================================
# Available targets:
#
# 	all (default):	vlib, vtest, utils, examp ...
#	vlib:		build V library
#       vtest:		build exhaustive test program v1
#	utils:		vgen, viconed, vdraw, bmp2vbm
#	examples:	demos proto, drawex, tutor, icondemo
#	clean:		general cleanup
#	cleanmotif:	cleanup motif .o's, etc.
#	cleanathena:	cleanup Athena .o's, etc.
#	cleanobj:	cleanup .o's
#	cleanbin:	cleanup bins's
#	cleanall:	cleanup all non-source stuff
#	tar:		X and Windows Distribution
#	tarwin:		Windows only Distribution"
#	tarx:		X only distribution
#	tarfull:	everthing, including .o's, etc.
#	getwin:		copy MS-Windows source from /dosc
#	permissions:	make sources readable to world
#	installv:	run as root to install to /usr/local/v
#	installvlinux:	run as root to install to /usr/local
#	installunm:	install to ../pubv (UNM specific)
#
#===============================================================


#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#
# composite targets and targets for builds in subdirs
# (with some in-between targets to map target to directory)
#
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

SRCEXTS	=  *.cpp *.cxx *.c *.h Make* make* Config* *.tex *.txt

.PHONY: default all vlib vtest utils examples testlib
.PHONY: srcx test appgen iconed draw bmp2vbm tutor examp drawex icons

#default: all

default: vlib

all:	vlib vtest utils examples
	@echo "Library, test, utils, examples and tutorial made"

vlib: srcx

vtest: test

testlib:
	-rm bin/v1
	cd srcx ; $(MAKE)
	cd test ; $(MAKE)

utils: appgen iconed draw bmp2vbm

examples: examp drawex icons tutor

srcx test appgen iconed draw bmp2vbm tutor examp drawex icons:
	cd $@ ; $(MAKE)


#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#
# Platform specific MAKES
#
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.PHONY:	linux linuxelf sun4 sgi hpux aix solaris bsd
linux linuxelf sun4 sgi hpux aix solaris bsd:
	$(MAKE) ARCH=$@ vlib
	$(MAKE) ARCH=$@ vtest
	(cd appgen ; $(MAKE) ARCH=$@)
	(cd iconed ; $(MAKE) ARCH=$@)


#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#
# targets for cleaning up
#
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.PHONY:	clean
clean:
#	-rm -f $(CLEANEXTS) *.bkh v.tgz
#	for i in srcx appgen draw drawex examp iconed icons test tutor; do \
#		(cd $$i; $(MAKE) clean); \
#	done
#	for i in obj contrib vopengl includex/v includew/v srcwin; do \
#		(cd $$i; rm -f $(CLEANEXTS)); \
#	done
#	-rm -f bccide/*.bak bccide/*.tmp bccide/*.csm
#	-rm -f bccide/*.dsw bccide/*.obr bccide/*.tr
#	-rm -f watcom/*.bak watcom/*.tmp watcom/*/*.exe watcom/*/*.obj
#	-rm -f watcom/*/*.map watcom/*/*.lib watcom/*/*.err watcom/*/*.sym
#	-rm -f bccide/*.bak bccide/*.tmp bccide/*.\~* bccide/*.csm
	rm -f objx/*.o lib/libVx*

.PHONY: cleanathena
cleanathena:
	-rm -f objx/* lib/libVx.a bin/v1x
	date > objx/ThisIs

.PHONY:	cleantestlib
cleantestlib:
	-rm bin/v1

.PHONY:	cleanbin
cleanbin:
	-rm -f bin/*
	date > lib/ThisIs
	date > bin/ThisIs

.PHONY:	cleanobj
cleanobj:
	-rm -f obj/* objx/*
	-rm -f watcom/*/*.obj
	date > obj/ThisIs
	date > objx/ThisIs

.PHONY:	cleanall
cleanall: clean cleanobj
	-rm -f lib/* bin/*
	date > lib/ThisIs
	date > bin/ThisIs

#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#
# targets for making tar files
#
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.PHONY:	tar
tar: clean
	@echo "Making v.tgz - X and Windows Distribution"
	@echo "    Cleaning up old tar file"
	-rm ../v-$(VV).tar$(TARZ)
	cd .. ; tar cvf$(TARZ) v-$(VV).tar$(TARZ) v/Make* v/README v/C* \
	 v/rd* v/vrefman/* v/examp/* v/bmp2vbm/* v/tutor/* v/includex/* \
	 v/includew/* v/lib/ThisIs v/obj/ThisIs v/objx/ThisIs v/srcx/* \
	 v/srcwin/* v/test/* v/draw/* v/iconed/* v/drawex/* v/bin/ThisIs \
	 v/bccide v/icons/* v/watcom/* v/contrib/* v/appgen/* \
	 v/vopengl/* v/msvc/* v/gnuwin32/* ;  cd v

.PHONY:	tarwin
tarwin: clean
	@echo "Making vwin.tar - Windows Distribution"
	@echo "    Cleaning up old tar file"
	-rm ../vwin$(VVW).tar$(TARZ)
	cd .. ; tar cvf$(TARZ) vwin$(VVW).tar$(TARZ) v/Make* v/README v/rd* v/C*  \
	 v/vrefman/* v/examp/* v/bmp2vbm/* v/tutor/* v/include \
	 v/includew/* v/lib/ThisIs v/obj/ThisIs v/objx/ThisIs \
	 v/srcwin/* v/test/* v/draw/* v/drawex/* v/bin/ThisIs  \
	 v/bccide  v/icons/* v/iconed/* v/contrib/* \
	 v/appgen/* v/vopengl/* v/msvc/* v/gnuwin32/* ; cd v

.PHONY:	tarx
tarx: clean
	@echo "Making vx-$(VV).tar - X only distribution"
	@echo "    Cleaning up old tar file"
	-rm ../vx-$(VV).tar$(TARZ)
	cd .. ; tar cvf$(TARZ) vx-$(VV).tar$(TARZ) v/Make* v/README v/rd* v/C*  \
	 v/vrefman/* v/examp/* v/bmp2vbm/* v/tutor/* v/includex/* \
	 v/lib/ThisIs v/obj/ThisIs v/objx/ThisIs v/srcx/* v/icons/* \
	 v/test/* v/draw/* v/iconed/* v/drawex/* v/bin/ThisIs \
	 v/contrib/* v/appgen/* v/vopengl/* ; cd v

.PHONY:	tarxsrc
tarxsrc: clean
	@echo "Making vxsrc-$(VV).tar - X only distribution"
	@echo "    Cleaning up old tar file"
	-rm ../vxsrc-$(VV).tar$(TARZ)
	cd .. ; tar cvf$(TARZ) vxsrc-$(VV).tar$(TARZ) v/Make* v/README v/rd* v/C*  \
	 v/examp/* v/bmp2vbm/* v/tutor/* v/includex/* \
	 v/lib/*/ThisIs v/obj/*/ThisIs v/objm/*/ThisIs v/srcx/* v/icons/* \
	 v/test/* v/draw/* v/iconed/* v/drawex/* v/bin/ThisIs \
	 v/contrib/* v/appgen/* v/vopengl/* ; cd v

.PHONY:	tarfull
tarfull: clean
	@echo "Making vfull.tgz - FULL VERSION"
	@echo "    Cleaning up old tar file"
	-rm ../vfull.tar$(TARZ)
	cd .. ; tar cvf$(TARZ) vfull.tar$(TARZ) v/* ; cd v

#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#
# target for fixing permissions
#
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

PERMDIRS =	. vrefman  examp tutor includex includex/v includew/v lib \
		lib/* obj obj/* objm objm/* srcx srcwin test draw drawex iconed bin \
		bin/* bmp2vbm icons vopengl appgen contrib bccide watcom msvc gnuwin32

WINDIRS =	watcom/win32 watcom/win32db \
		msvc/vgen32 msvc/viconed msvc/vmsvc32 msvc/vtstms32 \
		msvc/vgen32/release msvc/viconed/release msvc/vmsvc32/release \
		msvc/vtstms32/release

PERMFILES =	Make* README rd* C* rd* vrefman/* includex/v/* \
		includew/v/* lib/* obj/* objx/* srcx/* srcwin/* test/* \
		draw/* iconed/* drawex/* examp/* bin/* tutor/* bmp2vbm/* bccide/* \
		icons/* watcom/*/* watcom/* contrib/*  \
		appgen/* vopengl/* gnuwin32/* msvc/*/* msvc/*

EXEFILES =	bin/* 

.PHONY:	permissions
permissions:
	-chmod 755 $(PERMDIRS)
	-chmod 755 $(WINDIRS)
	-chmod 644 $(PERMFILES)
	-chmod 755 $(PERMDIRS)
	-chmod 755 $(WINDIRS)
	-chmod 755 $(EXEFILES)


#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#
# installation targets
# These work best for dedicated systems where you can run as
# root. 
# IMPORTANT! It will be easier if you make a symbolic link
# in the home directory of root to whereever V is before
# doing the install. E.g.,
#
#    [root]# cd
#    [root]# ln -s /home/bruce/v v
#    [root]# cd v
#    [root]# make installLinux
#
# This makefile uses $(LibDir), which is
# set to HOME by default, so adding a link from the root
# directory of root to the real v directory makes this work right.
#
# The Linux version assumes that X11 binaries are at /usr/X11/bin.
# Some versions have /usr/X11R6/bin instead. If that is your case,
# you should also create a symbolic link in /usr: ln -s X11R6 X11
#
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.PHONY:	installLinux
installLinux:
	$(MAKE) ARCH=linux installVLinux

.PHONY:	uninstallLinux
uninstallLinux:
	$(MAKE) ARCH=linux uninstallVLinux

.PHONY:	install
install:
	$(MAKE) ARCH=linuxelf installVLinux
	echo "#######################################"
	echo "  You now need to ldconfig from root..."
	echo "#######################################"

.PHONY:	uninstall
uninstall:
	$(MAKE) ARCH=linuxelf uninstallVLinux

.PHONY:	installv
installv:
	-mkdir $(INSTALL_PREFIX)/v
	-mkdir $(INSTALL_PREFIX)/v/lib
	-mkdir $(INSTALL_PREFIX)/v/bin
	-mkdir $(INSTALL_PREFIX)/v/include
	-mkdir $(INSTALL_PREFIX)/v/include/v
	-chmod 755 $(INSTALL_PREFIX)/v 
	-chmod 755 $(INSTALL_PREFIX)/v/lib
	-chmod 755 $(INSTALL_PREFIX)/v/bin
	-chmod 755 $(INSTALL_PREFIX)/v/include
	-chmod 755 $(INSTALL_PREFIX)/v/include/v
	cp includex/v/* $(INSTALL_PREFIX)/v/include/v
	cp $(LibDir)/*.a $(INSTALL_PREFIX)/v/lib
	cp $(Bin)/* $(INSTALL_PREFIX)/v/bin
	-chmod 644 $(INSTALL_PREFIX)/v/include/v/*
	-chmod 644 $(INSTALL_PREFIX)/v/lib/*
	-chmod 755 $(INSTALL_PREFIX)/v/bin/*

.PHONY:	installgnuwin32
installgnuwin32:
#	-mkdir $(HOMEGNU)/include/v
	cp includew/v/*.h $(HOMEGNU)/include/v
	cp $(LibDir)/*.a $(HOMEGNU)/lib
	cp $(Bin)/* $(HOMEGNU)/bin

.PHONY:	installmingw32
installmingw32:
#	-mkdir $(HOMEGNU)/include/v
	rm -f $(HOMEGNU)/include/v/*
	cp ./includew/v/*.h $(HOMEGNU)/include/v
	cp $(LibDir)/*.a $(HOMEGNU)/lib
	cp $(Bin)/* $(HOMEGNU)/bin

.PHONY:	installVLinux
installVLinux:
	@echo "You must run this as root"
#	-mkdir /usr/include/v
#	-chmod 644 includex/v/*
#	cp includex/v/* /usr/include/v
ifeq ($(Arch),linuxelf)
	chmod 644 $(LibDir)/*.so.$(VV)
	cp $(LibDir)/*.so.$(VV) $(INSTALL_PREFIX)/lib
	rm -f $(INSTALL_PREFIX)/lib/$(LIBNAME).so.1
	ln -s $(INSTALL_PREFIX)/lib/$(LIBNAME).so.$(VV) $(INSTALL_PREFIX)/lib/$(LIBNAME).so.1
	rm -f $(INSTALL_PREFIX)/lib/$(LIBNAME).so
	ln -s $(INSTALL_PREFIX)/lib/$(LIBNAME).so.$(VV) $(INSTALL_PREFIX)/lib/$(LIBNAME).so
else
	-chmod 644 $(LibDir)/*.a
	cp $(LibDir)/*.a $(INSTALL_PREFIX)/lib
endif
#	-chmod 755 $(Bin)/*
#	cp $(Bin)/* /usr/$(Xdir11)/bin

.PHONY:	uninstalVlLinux
uninstallVLinux:
	@echo "You must run this as root"
#	-rm  /usr/include/v/*
ifeq ($(Arch),linuxelf)
	rm -f $(INSTALL_PREFIX)/lib/$(LIBNAME).so.1
	rm -f $(INSTALL_PREFIX)/lib/$(LIBNAME).so
	rm -f $(INSTALL_PREFIX)/lib/$(LIBNAME).so.$(VV)
else
	rm -f $(INSTALL_PREFIX)/lib/libV.a
	rm -f $(INSTALL_PREFIX)/lib/libVgl.a
endif
#	-rm /usr/$(Xdir11)/bin/viconed
#	-rm /usr/$(Xdir11)/bin/b2v
#	-rm /usr/$(Xdir11)/bin/icondemo
#	-rm /usr/$(Xdir11)/bin/proto
#	-rm /usr/$(Xdir11)/bin/tutapp
#	-rm /usr/$(Xdir11)/bin/v1
#	-rm /usr/$(Xdir11)/bin/vdraw
#	-rm /usr/$(Xdir11)/bin/vdrawex
