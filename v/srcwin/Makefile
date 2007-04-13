#===============================================================
# V Libray Build Makefile - Version 1.25 - 22Sep00
#
# For the GNU WIN32 compilers: mingw32, cygwin and egcs variants
#
# Copyright (C) 1995-2000  Bruce E. Wampler
#
# This file is part of the V C++ GUI Framework, and is covered
# under the terms of the GNU Library General Public License,
# Version 2. This library has NO WARRANTY. See the source file
# vapp.cxx for more complete information about license terms.
#===============================================================
#
# Available targets: 
#	
#	all	: vlib
#	vlib	: builds V library
#	clean	: removes backup stuff
#
#===============================================================

CONFIG = ../Config.mk

include $(CONFIG)

oDirDll = $(oDir)dll

#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#
# fix library name and C/C++ compile options for the library build
#
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

	CFLAGSLIB	= $(CFLAGS)


#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#
# Object and header file lists
#
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

# vstartup.o MUST be first for gnuwin32 to work correctly!
LibGLObjs	=\
	$(oDir)/vbglcnv.o

LibObjs	=\
	$(oDir)/vstartup.o \
	$(oDir)/vapp.o \
	$(oDir)/vawinfo.o \
	$(oDir)/vbaseitm.o \
	$(oDir)/vbasewin.o \
	$(oDir)/vboxlblc.o \
	$(oDir)/vbrush.o \
	$(oDir)/vbtncmd.o \
	$(oDir)/vcanvas.o \
	$(oDir)/vcbtncmd.o \
	$(oDir)/vchkboxc.o \
	$(oDir)/vclabelc.o \
	$(oDir)/vcmd.o \
	$(oDir)/vcmdpane.o \
	$(oDir)/vcmdprnt.o \
	$(oDir)/vcmdwin.o \
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
	$(oDir)/vpen.o \
	$(oDir)/vpopmenu.o \
	$(oDir)/vprogrsc.o \
	$(oDir)/vradioc.o \
	$(oDir)/vreply.o \
	$(oDir)/vrepldlg.o \
	$(oDir)/vsliderc.o \
	$(oDir)/vslist.o \
	$(oDir)/vspinc.o \
	$(oDir)/vtextc.o \
	$(oDir)/vtextcnv.o \
	$(oDir)/vtexted.o \
	$(oDir)/vtextinc.o \
	$(oDir)/vthislst.o \
	$(oDir)/vtimer.o \
	$(oDir)/vutil.o \
	$(oDir)/vwindc.o \
	$(oDir)/vwindow.o \
	$(oDir)/vwinprdc.o \
	$(oDir)/vwinprtr.o \
	$(oDir)/vynreply.o

BASEDEP =\
	v_defs.h vbaseitm.h vbasewin.h vapp.h vwin32.h

PANEDEP	=\
	vpane.h vwindow.h

CMDDEP	=\
	v_defs.h vdialog.h vcmd.h vcmdprnt.h

CMDS	=\
	vbtncmd.h vcbtncmd.h vlabelc.h vframec.h vchkboxc.h vradioc.h \
	vprogrsc.h vsliderc.h vtextinc.h vlistc.h vcomboc.h vtextc.h \
	vcolor.h vpen.h vclabelc.h vspinc.h


#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#
# targets
#
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.PHONY: default
all: vlib vlibgl

.PHONY: cleanwin
cleanwin:
	-rm -f $(CLEANEXTS)
	-rm -f $(HOMEV)/appgen/tags
	-rm -f $(HOMEV)/appgen/*.bak
	-rm -f $(HOMEV)/appgen/*.o
	-rm -f $(HOMEV)/appgen/*.obj
	-rm -f $(HOMEV)/appgen/*.exe
	-rm -f $(HOMEV)/draw/*.bak
	-rm -f $(HOMEV)/draw/tags
	-rm -f $(HOMEV)/draw/*.o
	-rm -f $(HOMEV)/draw/*.obj
	-rm -f $(HOMEV)/drawex/*.bak
	-rm -f $(HOMEV)/drawex/tags
	-rm -f $(HOMEV)/examp/*.bak
	-rm -f $(HOMEV)/examp/tags
	-rm -f $(HOMEV)/iconed/*.bak
	-rm -f $(HOMEV)/iconed/tags
	-rm -f $(HOMEV)/iconed/*.o
	-rm -f $(HOMEV)/iconed/*.obj
	-rm -f $(HOMEV)/iconed/*.exe
	-rm -f $(HOMEV)/icons/*.bak
	-rm -f $(HOMEV)/icons/tags
	-rm -f $(HOMEV)/icons/*.o
	-rm -f $(HOMEV)/icons/*.obj
	-rm -f $(HOMEV)/icons/*.exe
	-rm -f $(HOMEV)/includew/v/*.bak
	-rm -f $(HOMEV)/includew/v/tags
	-rm -f $(HOMEV)/srcwin/*.bak
	-rm -f $(HOMEV)/srcwin/tags
	-rm -f $(HOMEV)/texted/*.bak
	-rm -f $(HOMEV)/texted/tags
	-rm -f $(HOMEV)/texted/*.o
	-rm -f $(HOMEV)/texted/*.obj
	-rm -f $(HOMEV)/texted/*.exe
	-rm -f $(HOMEV)/vrefman/*.bak
	-rm -f $(HOMEV)/vrefman/*.tmp
	-rm -f $(HOMEV)/vide/*.class
	-rm -f $(HOMEV)/vide/*.bak
	-rm -f $(HOMEV)/vide/tags
	-rm -f $(HOMEV)/vide/*.o
	-rm -f $(HOMEV)/vide/*.obj
	-rm -f $(HOMEV)/vide/*.exe
	-rm -f $(HOMEV)/vide/testgdb/*.o
	-rm -f $(HOMEV)/vide/testgdb/*.exe
	-rm -f $(HOMEV)/vide/testgdb/*.bak
	-rm -f $(HOMEV)/vide/testgdb/tags
	-rm -f $(HOMEV)/test/*.bak
	-rm -f $(HOMEV)/test/tags
	-rm -f $(HOMEV)/bin/*.exe
	-touch $(HOMEV)/bin/ThisIs
	-rm -f $(HOMEV)/lib/*.*
	-touch $(HOMEV)/lib/ThisIs
	-rm -f $(HOMEV)/obj/*.*
	-touch $(HOMEV)/obj/ThisIs
	-rm -f $(HOMEV)/objdll/*.*
	-touch $(HOMEV)/objdll/ThisIs

	-rm -f $(HOMEV)/msvc6/vlib/*.ncb
	-rm -f $(HOMEV)/msvc6/vlib/*.opt
	-rm -f $(HOMEV)/msvc6/vlib/Release/*.*
	-touch $(HOMEV)/msvc6/vlib/Release/ThisIs
	-rm -f $(HOMEV)/msvc6/vlib/Debug/*.*
	-touch $(HOMEV)/msvc6/vlib/Debug/ThisIs

	-rm -f $(HOMEV)/msvc6/vtestlib/*.ncb
	-rm -f $(HOMEV)/msvc6/vtestlib/*.opt
	-rm -f $(HOMEV)/msvc6/vtestlib/Release/*.*
	-touch $(HOMEV)/msvc6/vtestlib/Release/ThisIs
	-rm -f $(HOMEV)/msvc6/vtestlib/Debug/*.*
	-touch $(HOMEV)/msvc6/vtestlib/Debug/ThisIs

	-rm -f $(HOMEV)/msvc6/vide/*.ncb
	-rm -f $(HOMEV)/msvc6/vide/*.opt
	-rm -f $(HOMEV)/msvc6/vide/Release/*.*
	-touch $(HOMEV)/msvc6/vide/Release/ThisIs
	-rm -f $(HOMEV)/msvc6/vide/Debug/*.*
	-touch $(HOMEV)/msvc6/vide/Debug/ThisIs

	-rm -f $(HOMEV)/msvc6/vgl/*.ncb
	-rm -f $(HOMEV)/msvc6/vgl/*.opt
	-rm -f $(HOMEV)/msvc6/vgl/Release/*.*
	-touch $(HOMEV)/msvc6/vgl/Release/ThisIs
	-rm -f $(HOMEV)/msvc6/vgl/Debug/*.*
	-touch $(HOMEV)/msvc6/vgl/Debug/ThisIs

	-rm -f $(HOMEV)/msvc6/gears/*.ncb
	-rm -f $(HOMEV)/msvc6/gears/*.opt
	-rm -f $(HOMEV)/msvc6/gears/Release/*.*
	-touch $(HOMEV)/msvc6/gears/Release/ThisIs
	-rm -f $(HOMEV)/msvc6/gears/Debug/*.*
	-touch $(HOMEV)/msvc6/gears/Debug/ThisIs

	-rm -f $(HOMEV)/bccide/*.bak
	-rm -f $(HOMEV)/bccide/*.~de
	-rm -f $(HOMEV)/bccide/*.csm
	-rm -f $(HOMEV)/bccide/*.dsw
	-rm -f $(HOMEV)/bccide/*.mrt
	-rm -f $(HOMEV)/bccide/*.mbt
	-rm -f $(HOMEV)/bccide/*.obr

.PHONY: clean
clean:
	-rm -f $(CLEANEXTS)
	-rm -f $(oDir)/*.o


#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#
# target for building the v library
#
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
.PHONY:	vlibgl
vlibgl:	$(LibDir)/$(LIBNAME)gl.a

.PHONY:	vlib
vlib:	$(LibDir)/$(LIBNAME).a

$(LibDir)/$(LIBNAME).a:	$(LibObjs)
	rm -f $(LibDir)/$(LIBNAME).a
	$(AR) cr $(LibDir)/$(LIBNAME).a $(LibObjs) $(Lib3dObjs)
	$(RANLIB) $(LibDir)/$(LIBNAME).a

$(LibDir)/$(LIBNAME)gl.a:	$(LibGLObjs)
	rm -f $(LibDir)/$(LIBNAME)gl.a
	$(AR) cr $(LibDir)/$(LIBNAME)gl.a $(LibGLObjs)
	$(RANLIB) $(LibDir)/$(LIBNAME)gl.a


#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#
#    .o's for the library
#
#  note: $@ gives target ($(oDir)/xx.o, 
#	$< gives first dependency (xx.cpp)
#	$(CXX) -c $(CFLAGS) -o $@ $<
#
#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

$(oDir)/vapp.o:       vapp.cpp $(BASEDEP) vfont.h vawinfo.h vcmdwin.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vawinfo.o:    vawinfo.cpp $(BASEDEP) vawinfo.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vbaseitm.o:   vbaseitm.cpp vbaseitm.h v_defs.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vbasewin.o:   vbasewin.cpp $(BASEDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vbglcnv.o:   vbglcnv.cpp vbglcnv.h vcanvas.h vcpdc.h \
      $(BASEDEP) $(PANEDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vboxlblc.o:   vboxlblc.cpp $(BASEDEP) vboxlblc.h vcmdprnt.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vbrush.o:     vbrush.cpp $(BASEDEP) vbrush.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vbtncmd.o:    vbtncmd.cpp vbtncmd.h $(CMDDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vcanvas.o:    vcanvas.cpp vcanvas.h vfont.h vcpdc.h vbrush.h \
      vpen.h $(BASEDEP) $(PANEDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vcbtncmd.o:   vcbtncmd.cpp vcbtncmd.h $(CMDDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vchkboxc.o:   vchkboxc.cpp vchkboxc.h $(CMDDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vclabelc.o:   vclabelc.cpp vclabelc.h $(CMDDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vcmd.o:       vcmd.cpp vcmd.h $(BASEDEP) $(PANEDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vcmdpane.o:   vcmdpane.cpp vcmdpane.h $(BASEDEP) $(PANEDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vcmdwin.o:    vcmdwin.cpp $(BASEDEP) vwindow.h vpane.h vcmdwin.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vcmdprnt.o:   vcmdprnt.cpp $(BASEDEP) vcmdprnt.h $(CMDS)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vcomboc.o:    vcomboc.cpp vcomboc.h $(CMDDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vcolor.o:     vcolor.cpp vcolor.h $(CMDDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vdebug.o:     vdebug.cpp vdebug.h $(BASEDEP) vcmdprnt.h vmodald.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vdialog.o:    vdialog.cpp vdialog.h $(BASEDEP) vcmdprnt.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vcpdc.o:      vcpdc.cpp vdc.h vcpdc.h vpen.h $(BASEDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vpsdc.o:      vpsdc.cpp vdc.h vpsdc.h vprinter.h vpsprntr.h $(BASEDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vfilesel.o:   vfilesel.cpp vfilesel.h $(BASEDEP) vcmdprnt.h \
      vmodald.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vfinddlg.o:   vfinddlg.cpp vfinddlg.h $(BASEDEP) vcmdprnt.h \
      vmodald.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vfontsel.o:   vfontsel.cpp vfontsel.h vfont.h $(BASEDEP) vcmdprnt.h \
      vmodald.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vfont.o:      vfont.cpp vfont.h $(BASEDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vframec.o:    vframec.cpp vframec.h $(CMDDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vicon.o:      vicon.cpp vicon.h $(CMDDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vlabelc.o:    vlabelc.cpp vlabelc.h $(CMDDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vlistc.o:     vlistc.cpp vlistc.h $(CMDDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vmemdc.o:     vmemdc.cpp vmemdc.h vdc.h vpen.h vpen.h $(BASEDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vmenu.o:      vmenu.cpp vmenu.h $(BASEDEP) $(PANEDEP) vcmdwin.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vmodald.o:    vmodald.cpp vmodald.h vdialog.h \
      $(BASEDEP) vcmdprnt.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vnotice.o:    vnotice.cpp vnotice.h $(BASEDEP) vcmdprnt.h \
      vmodald.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vos.o:       vos.cpp vos.h $(BASEDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vpen.o:       vpen.cpp vpen.h $(BASEDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vpopmenu.o:      vpopmenu.cpp vpopmenu.h $(BASEDEP) $(PANEDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vprogrsc.o:   vprogrsc.cpp vprogrsc.h $(CMDDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vpsprntr.o:   vpsprntr.cpp vpsprntr.h vprinter.h $(BASEDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vradioc.o:    vradioc.cpp vradioc.h $(CMDDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vsliderc.o:   vsliderc.cpp vsliderc.h $(CMDDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vslist.o:   vslist.cpp vslist.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vrepldlg.o:   vrepldlg.cpp vrepldlg.h $(BASEDEP) vcmdprnt.h \
      vmodald.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vreply.o:     vreply.cpp vreply.h $(BASEDEP) vcmdprnt.h vmodald.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vspinc.o:     vspinc.cpp vspinc.h $(CMDDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vstartup.o:     vstartup.cpp vapp.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vtextc.o:     vtextc.cpp vtextc.h $(CMDDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vtextcnv.o:   vtextcnv.cpp vtextcnv.h vcanvas.h vcpdc.h \
      $(BASEDEP) $(PANEDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vtexted.o:    vtexted.cpp vtexted.h vtextcnv.h vcanvas.h vcpdc.h \
      $(BASEDEP) $(PANEDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vtextinc.o:   vtextinc.cpp vtextinc.h $(CMDDEP)
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vthislst.o:   vthislst.cpp vthislst.h vwin32.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vtimer.o:     vtimer.cpp vtimer.h vapp.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vutil.o:      vutil.cpp vutil.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vwindc.o:     vwindc.cpp $(BASEDEP) vwindc.h vmemdc.h vcanvas.h vicon.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vwindow.o:    vwindow.cpp $(BASEDEP) vwindow.h vpane.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vwinprdc.o:   vwinprdc.cpp $(BASEDEP) vwinprdc.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vwinprtr.o:   vwinprtr.cpp $(BASEDEP) vwinprtr.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<

$(oDir)/vynreply.o:   vynreply.cpp vynreply.h $(BASEDEP) vcmdprnt.h vmodald.h
	$(CXX) -c $(CFLAGSLIB) -o $@ $<
