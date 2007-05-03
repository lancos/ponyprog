#===============================================================
# V Libray Build Makefile - Version 1.25 - 22Sep00
#
# Copyright (C) 1995-1999  Bruce E. Wampler
#
# This file is part of the V C++ GUI Framework, and is covered
# under the terms of the GNU Library General Public License,
# Version 2. This library has NO WARRANTY. See the source file
# vapp.cxx for more complete information about license terms.
#===============================================================
#
# Available targets: 
#	
#	default	: vlib
#	vlib	: builds V library
#	clean	: removes backup stuff
#
#===============================================================

CONFIG = ../Config.mk
include $(CONFIG)

#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#
# fix library name and C/C++ compile options for the library build
#
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

ifeq ($(TOOLKIT),Athena)
ifeq ($(USE_3D),no)
	CFLAGS	+= -DNO_V_3D
endif
endif

#Added -fpermissive for gcc 4
ifeq ($(Arch),linuxelf)
	CXFLAGSLIB	= -fpic -fpermissive -Wno-deprecated $(CFLAGS)
	CCFLAGSLIB	= -fpic $(CFLAGS)
else
	CXFLAGSLIB	= -fpermissive -Wno-deprecated $(CFLAGS)
	CCFLAGSLIB	= $(CFLAGS)
endif


#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#
# Object and header file lists
#
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

LibGLObjs	=\
	$(oDir)/vbglcnv.o \

LibObjs	=\
	$(oDir)/vapp.o \
	$(oDir)/vawinfo.o \
	$(oDir)/vbaseitm.o \
	$(oDir)/vbasewin.o \
	$(oDir)/vbtncmd.o \
	$(oDir)/vcanvas.o \
	$(oDir)/canvas.o \
	$(oDir)/vcbtncmd.o \
	$(oDir)/vchkboxc.o \
	$(oDir)/vclabelc.o \
	$(oDir)/vcmd.o \
	$(oDir)/vcmdpane.o \
	$(oDir)/vcmdwin.o \
	$(oDir)/vcmdprnt.o \
	$(oDir)/vcolor.o \
	$(oDir)/vcomboc.o \
	$(oDir)/vcpdc.o \
	$(oDir)/vdebug.o \
	$(oDir)/vdialog.o \
	$(oDir)/vfilesel.o \
	$(oDir)/vfinddlg.o \
	$(oDir)/vfont.o \
	$(oDir)/vfontsel.o \
	$(oDir)/vframec.o \
	$(oDir)/vicon.o \
	$(oDir)/vlabelc.o \
	$(oDir)/vlistc.o \
	$(oDir)/vmemdc.o \
	$(oDir)/vmenu.o \
	$(oDir)/vmodald.o \
	$(oDir)/vnotice.o \
	$(oDir)/vos.o \
	$(oDir)/vpopmenu.o \
	$(oDir)/vpsprntr.o \
	$(oDir)/vprogrsc.o \
	$(oDir)/vpsdc.o \
	$(oDir)/vradioc.o \
	$(oDir)/vreply.o \
	$(oDir)/vrepldlg.o \
	$(oDir)/vsliderc.o \
	$(oDir)/vslist.o \
	$(oDir)/vspinc.o \
        $(oDir)/vstartup.o \
	$(oDir)/vstatusp.o \
	$(oDir)/vtextc.o \
	$(oDir)/vtextcnv.o \
	$(oDir)/vtexted.o \
	$(oDir)/vtextinc.o \
	$(oDir)/vtimer.o \
	$(oDir)/vtipwin.o \
	$(oDir)/vutil.o \
	$(oDir)/vwindow.o \
	$(oDir)/vxdc.o \
	$(oDir)/vx2vkey.o \
	$(oDir)/vynreply.o

# Sorry, we MUST at least use the V versions of the 3D scroll bar and 3D slider
ifeq ($(USE_3D),no)
Lib3dObjs=\
	$(oDir)/aw3d.o \
	$(oDir)/awsim3d.o \
	$(oDir)/awscl3d.o \
	$(oDir)/awsld3d.o
endif

ifeq ($(USE_3D),yes)
Lib3dObjs=\
	$(oDir)/aw3d.o \
	$(oDir)/awcmd3d.o \
	$(oDir)/awlbl3d.o \
	$(oDir)/awscl3d.o \
	$(oDir)/awsld3d.o \
	$(oDir)/awsim3d.o \
	$(oDir)/awtog3d.o
endif

BASEDEP =\
	v_defs.h vbaseitm.h vbasewin.h vapp.h

PANEDEP	=\
	vpane.h vwindow.h

CMDDEP	=\
	v_defs.h vdialog.h vcmd.h vcmdprnt.h vxutil.h

CMDS	=\
	vbtncmd.h vcbtncmd.h vlabelc.h vframec.h vchkboxc.h vradioc.h \
	vprogrsc.h vsliderc.h vtextinc.h vlistc.h vcomboc.h vtextc.h \
	vcolor.h vpen.h vclabelc.h vspinc.h

	cd iconed ; $(MAKE) ARCH=$@ ; cd ..


#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#
# targets
#
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.PHONY: default
#default: vlib vlibgl
default: vlib

.PHONY: clean
clean:
	-rm -f $(CLEANEXTS)


#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#
# target for building the v library
#
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.PHONY:	vlibgl

ifeq ($(Arch),linuxelf)
vlibgl:	$(oDir) $(LibDir) $(LibDir)/$(LIBNAME)gl.so.$(VV)

$(LibDir)/$(LIBNAME)gl.so.$(VV):	$(LibGLObjs)
	-rm -f $(LibDir)/$(LIBNAME)gl.so.$(VV)
	-$(CXX) -shared -Wl,-soname,$(LIBNAME)gl.so.$(VV) \
		-o $(LibDir)/$(LIBNAME)gl.so.$(VV) $(LibGLObjs)
	-(cd $(VLibDir); \
		-rm -f $(LIBNAME)gl.so; \
		-ln -s $(LIBNAME)gl.so.$(VV) $(LIBNAME).so)
else
vlibgl:	$(LibDir)/$(LIBNAME)gl.a

$(LibDir)/$(LIBNAME)gl.a:	$(LibGLObjs)
	-rm -f $(LibDir)/$(LIBNAME)gl.a
	-$(AR) cr $(LibDir)/$(LIBNAME)gl.a $(LibGLObjs)
	-$(RANLIB) $(LibDir)/$(LIBNAME)gl.a
	-chmod 644 $(LibDir)/$(LIBNAME)gl.a
endif

.PHONY:	vlib

ifeq ($(Arch),linuxelf)
vlib:	$(oDir) $(LibDir) $(LibDir)/$(LIBNAME).so.$(VV)

$(LibDir)/$(LIBNAME).so.$(VV):	$(LibObjs) $(Lib3dObjs)
	rm -f $(LibDir)/$(LIBNAME).so.$(VV)
	$(CXX) -shared -Wl,-soname,$(LIBNAME).so.$(VV) \
		-o $(LibDir)/$(LIBNAME).so.$(VV) $(LibObjs) $(Lib3dObjs)
	(cd $(VLibDir); \
		rm -f $(LIBNAME).so; \
		ln -s $(LIBNAME).so.$(VV) $(LIBNAME).so)
else
vlib:	$(LibDir)/$(LIBNAME).a

$(LibDir)/$(LIBNAME).a:	$(LibObjs) $(Lib3dObjs)
	rm -f $(LibDir)/$(LIBNAME).a
	$(AR) cr $(LibDir)/$(LIBNAME).a $(LibObjs) $(Lib3dObjs)
	$(RANLIB) $(LibDir)/$(LIBNAME).a
	chmod 644 $(LibDir)/$(LIBNAME).a
endif


#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#
#    .o's for the library
#
#  note: $@ gives target ($(oDir)/xx.o, 
#	$< gives first dependency (xx.cxx)
#	$(CXX) -c $(CFLAGS) -o $@ $<
#
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

$(oDir)/vapp.o:	vapp.cxx $(BASEDEP) vfont.h vawinfo.h vcmdwin.h vctlclrs.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vawinfo.o:	vawinfo.cxx $(BASEDEP) vawinfo.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vbaseitm.o:	vbaseitm.cxx vbaseitm.h v_defs.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vbasewin.o:	vbasewin.cxx $(BASEDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vbglcnv.o:	vbglcnv.cxx vbglcnv.h $(BASEDEP) $(PANEDEP)
	-$(CXX) -c $(CXFLAGSLIB) -o $@ $<
	@echo "************************************************************************************"
	@echo "* PLEASE NOTE: To get OpenGL support for vbglcnv.cxx, you must have OpenGL or MESA *"
	@echo "* installed. You may safely ignore the above errors if you don't have OpenGL.      *"
	@echo "************************************************************************************"

$(oDir)/vbtncmd.o:	vbtncmd.cxx vbtncmd.h vctlclrs.h awtog3d.h vicon.h $(CMDDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vcanvas.o:	vcanvas.cxx vcanvas.h vfont.h vcpdc.h vbrush.h \
	vpen.h $(BASEDEP) $(PANEDEP) vctlclrs.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/canvas.o:	canvas.c canvas.h canvasp.h
	$(CC) -c $(CCFLAGSLIB) -o $@ $<

$(oDir)/vcbtncmd.o:	vcbtncmd.cxx vcbtncmd.h $(CMDDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vchkboxc.o:	vchkboxc.cxx vchkboxc.h vctlclrs.h vicon.h $(CMDDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vclabelc.o:	vclabelc.cxx vclabelc.h vicon.h $(CMDDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vcmd.o:	vcmd.cxx vcmd.h $(BASEDEP) $(PANEDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vcmdpane.o:	vcmdpane.cxx vcmdpane.h $(BASEDEP) $(PANEDEP) vctlclrs.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vcmdwin.o:	vcmdwin.cxx $(BASEDEP) vwindow.h vpane.h \
	vcmdwin.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vcmdprnt.o:	vcmdprnt.cxx $(BASEDEP) vcmdprnt.h $(CMDS)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vcomboc.o:	vcomboc.cxx vcomboc.h vctlclrs.h $(CMDDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vcolor.o:	vcolor.cxx vcolor.h $(CMDDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vdebug.o:	vdebug.cxx vdebug.h $(BASEDEP) vcmdprnt.h \
	vmodald.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vdialog.o:	vdialog.cxx vdialog.h $(BASEDEP) vcmdprnt.h vctlclrs.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vcpdc.o:	vcpdc.cxx vxdc.h vdc.h vcpdc.h vpen.h vpen.h canvas.h $(BASEDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vpsdc.o:	vpsdc.cxx vdc.h vpsdc.h vprinter.h vpsprntr.h $(BASEDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vfilesel.o:	vfilesel.cxx vfilesel.h $(BASEDEP) vcmdprnt.h \
	vmodald.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vfinddlg.o:	vfinddlg.cpp vfinddlg.h $(BASEDEP) vcmdprnt.h \
	vmodald.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vfontsel.o:	vfontsel.cxx vfontsel.h vfont.h $(BASEDEP) vcmdprnt.h \
	vmodald.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vfont.o:	vfont.cxx vfont.h vfontlst.h $(BASEDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vframec.o:	vframec.cxx vframec.h $(CMDDEP) vctlclrs.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vicon.o:	vicon.cxx vicon.h $(CMDDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vlabelc.o:	vlabelc.cxx vlabelc.h $(CMDDEP) vctlclrs.h vicon.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vlistc.o:	vlistc.cxx vlistc.h $(CMDDEP) vctlclrs.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vslist.o:	vslist.cpp vslist.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vmemdc.o:	vmemdc.cxx vmemdc.h vxdc.h vdc.h vmemdc.h vpen.h vpen.h canvas.h $(BASEDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vmenu.o:	vmenu.cxx vmenu.h $(BASEDEP) $(PANEDEP) vcmdwin.h vctlclrs.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vmodald.o:	vmodald.cxx vmodald.h vdialog.h \
	$(BASEDEP) vcmdprnt.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vnotice.o:	vnotice.cxx vnotice.h $(BASEDEP) vcmdprnt.h \
	vmodald.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vos.o:	vos.cxx vos.h 
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vpopmenu.o:	vpopmenu.cxx vpopmenu.h $(BASEDEP) $(PANEDEP) vcmdwin.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vprogrsc.o:	vprogrsc.cxx vprogrsc.h $(CMDDEP) awsld3d.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vpsprntr.o:	vpsprntr.cxx vpsprntr.h vprinter.h $(BASEDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vradioc.o:	vradioc.cxx vradioc.h vctlclrs.h vicon.h $(CMDDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vsliderc.o:	vsliderc.cxx vsliderc.h $(CMDDEP) awsld3d.h vctlclrs.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vrepldlg.o:	vrepldlg.cpp vrepldlg.h $(BASEDEP) vcmdprnt.h \
	vmodald.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vreply.o:	vreply.cxx vreply.h $(BASEDEP) vcmdprnt.h \
	vmodald.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vspinc.o:	vspinc.cxx vspinc.h vctlclrs.h $(CMDDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vstartup.o:	vstartup.cxx vapp.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vstatusp.o:	vstatusp.cxx vstatusp.h $(BASEDEP) $(PANEDEP) vctlclrs.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vtextc.o:	vtextc.cxx vtextc.h $(CMDDEP) vctlclrs.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vtextcnv.o:	vtextcnv.cxx vtextcnv.h vcanvas.h vcpdc.h \
	$(BASEDEP) $(PANEDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vtexted.o:	../srcwin/vtexted.cpp ../includew/v/vtexted.h vtextcnv.h vcanvas.h vcpdc.h \
	$(BASEDEP) $(PANEDEP)
	$(CXX) -c $(CXFLAGSLIB) -I../includew/v -o $@ $<

$(oDir)/vtextinc.o:	vtextinc.cxx vtextinc.h $(CMDDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vtimer.o:	vtimer.cxx vtimer.h vapp.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vtipwin.o:	vtipwin.cxx vtipwin.h vapp.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vutil.o:	vutil.cxx vutil.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vwindow.o:	vwindow.cxx $(BASEDEP) vwindow.h vpane.h \
	vx2vkey.h vctlclrs.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vxdc.o:	vxdc.cxx vxdc.h vdc.h vpen.h vpen.h canvas.h vicon.h $(BASEDEP)
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vx2vkey.o:	vx2vkey.cxx vx2vkey.h v_defs.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<

$(oDir)/vynreply.o:	vynreply.cxx vynreply.h $(BASEDEP) vcmdprnt.h \
	vmodald.h
	$(CXX) -c $(CXFLAGSLIB) -o $@ $<


#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#
#  .o's for the 3d Library
#
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

$(oDir)/aw3d.o:	aw3d.c aw3d.h aw3dp.h vctlclrs.h
	$(CC) -c $(CCFLAGSLIB) -DUSEGRAY -o $@ $<

$(oDir)/awcmd3d.o:	awcmd3d.c awcmd3d.h awcmd3dp.h
	$(CC) -c $(CCFLAGSLIB) -o $@ $<

$(oDir)/awscl3d.o:	awscl3d.c awscl3d.h awscl3dp.h
	$(CC) -c $(CCFLAGSLIB) -o $@ $<

$(oDir)/awsld3d.o:	awsld3d.c awsld3d.h awsld3dp.h
	$(CC) -c $(CCFLAGSLIB) -o $@ $<

$(oDir)/awlbl3d.o:	awlbl3d.c awlbl3d.h awlbl3dp.h
	$(CC) -c $(CCFLAGSLIB) -o $@ $<

$(oDir)/awsim3d.o:	awsim3d.c awsim3d.h awsim3dp.h
	$(CC) -c $(CCFLAGSLIB) -o $@ $<

$(oDir)/awtog3d.o:	awtog3d.c awtog3d.h awtog3dp.h
	$(CC) -c $(CCFLAGSLIB) -o $@ $<
