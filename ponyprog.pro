#-------------------------------------------------
#
# Project created by QtCreator 2013-05-10T22:49:41
#
#-------------------------------------------------

QT       += core gui


# TRANSLATIONS = cnc-qt.ts 

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ponyprog
TEMPLATE = app
DESTDIR = build

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui


# TODO: please check this
win32:INCLUDEPATH += $$PWD/windows 
# win32:INCLUDEPATH += C:/libusb-1.0.19/libusb
# win32:INCLUDEPATH += C:/Program Files (x86)/libusb-1.0.19
# win32:DEPENDPATH += C:/Program Files (x86)/libusb-1.0.19/libusb
# win32:DEPENDPATH += C:/libusb-1.0.19/libusb
win32:DEPENDPATH += $$PWD/windows

win64:INCLUDEPATH += $$PWD/windows 
# win64:INCLUDEPATH += C:/libusb-1.0.19/libusb
# win64:INCLUDEPATH += C:/libusb-1.0.19/libusb
# win64:INCLUDEPATH +=C:/Program Files/libusb-1.0.19
# win64:DEPENDPATH += C:/Program Files/libusb-1.0.19/libusb
# win64:DEPENDPATH += C:/libusb-1.0.19/libusb

# contains(QT_ARCH, i386) {
#     win32:INCLUDEPATH += C:/libusb-1.0.19/libusb
#     win32:INCLUDEPATH += C:/Program Files (x86)/libusb-1.0.19
#     win32:DEPENDPATH += C:/Program Files (x86)/libusb-1.0.19/libusb
#     win32:DEPENDPATH += C:/libusb-1.0.19/libusb
#     win32:message("32-bit")
# } else {
#     win32:INCLUDEPATH += C:/libusb-1.0.19/libusb
#     win32:INCLUDEPATH +=C:/Program Files/libusb-1.0.19
#     win32:DEPENDPATH += C:/Program Files/libusb-1.0.19/libusb
#     win32:DEPENDPATH += C:/libusb-1.0.19/libusb
#     win32:message("64-bit")
# }


# TODO: please add this for macx
# macx

unix:INCLUDEPATH +=/usr/local/include/
# unix:INCLUDEPATH +=/usr/local/include/libusb-1.0/
# unix:INCLUDEPATH +=/usr/include/libusb-1.0/

INCLUDEPATH += qhexedit2/src

SOURCES  += SrcPony/aboutmdlg.cpp \
            SrcPony/main.cpp \
            SrcPony/e2app.cpp \
            SrcPony/at25xxx.cpp \
            SrcPony/at93cbus.cpp \
            SrcPony/crc.cpp \
            SrcPony/e24xx-1.cpp \
            SrcPony/e24xx-2.cpp \
            SrcPony/e24xx.cpp \
            SrcPony/e24xx-5.cpp \
            SrcPony/e2profil.cpp \
            SrcPony/fusemdlg.cpp \
            SrcPony/intfbuf.cpp \
            SrcPony/lpt_io_interf.cpp \
            SrcPony/pgminter.cpp \
            SrcPony/picbus.cpp \
            SrcPony/progoption.cpp \
            SrcPony/sernumdlg.cpp \
            SrcPony/x2444.cpp \
            SrcPony/at17xxx.cpp \
            SrcPony/at89sbus.cpp \
            SrcPony/at93cxx8.cpp \
            SrcPony/csmfbuf.cpp \
            SrcPony/e2cmdw.cpp \
            SrcPony/e2awinfo.cpp \
            SrcPony/easyi2c_interf.cpp \
            SrcPony/i2cbus.cpp \
            SrcPony/ispinterf.cpp \
            SrcPony/microbus.cpp \
            SrcPony/pic125xx.cpp \
            SrcPony/picbusnew.cpp \
            SrcPony/spi-bus.cpp \
            SrcPony/at250bus2.cpp \
            SrcPony/at89sxx.cpp \
            SrcPony/at93cxx.cpp \
            SrcPony/device.cpp \
            SrcPony/eeptypes.cpp \
            SrcPony/imbus.cpp \
            SrcPony/linuxsysfsint.cpp \
            SrcPony/pic12bus.cpp \
            SrcPony/ponyioint.cpp \
            SrcPony/rs232int.cpp \
            SrcPony/sxbus.cpp \
            SrcPony/at250bus.cpp \
            SrcPony/at90sbus.cpp \
            SrcPony/binfbuf.cpp \
            SrcPony/dt006interf.cpp \
            SrcPony/e2dlg.cpp \
            SrcPony/filebuf.cpp \
            SrcPony/infomdlg.cpp \
            SrcPony/lpt_ext_interf.cpp \
            SrcPony/motsfbuf.cpp \
            SrcPony/pic168xx.cpp \
            SrcPony/portint.cpp \
            SrcPony/sde2506.cpp \
            SrcPony/wait.cpp \
            SrcPony/at250xx.cpp \
            SrcPony/at90sxx.cpp \
            SrcPony/busio.cpp \
            SrcPony/e2401.cpp \
            SrcPony/e2pfbuf.cpp \
            SrcPony/filldlg.cpp \
            SrcPony/interfconv.cpp \
            SrcPony/lptinterf.cpp \
            SrcPony/nvm3060.cpp \
            SrcPony/pic16xx.cpp \
            SrcPony/x2444bus.cpp \
            SrcPony/sdebus.cpp \
            qhexedit2/src/chunks.cpp \
            qhexedit2/src/commands.cpp \
            qhexedit2/src/qhexedit.cpp \

HEADERS  += SrcPony/e2app.h \ 
            SrcPony/e2awinfo.h \
            SrcPony/e2cmdw.h \ 
            SrcPony/globals.h \
            SrcPony/device.h \
            SrcPony/microbus.h \
            SrcPony/aboutmdlg.h \
            SrcPony/at25xxx.h \
            SrcPony/at93cbus.h \
            SrcPony/busio.h \
            SrcPony/dt006interf.h \
            SrcPony/e24xx.h \
            SrcPony/e2dlg.h \
            SrcPony/eeptypes.h \
            SrcPony/ispinterf.h \
            SrcPony/lpt_io_interf.h \
            SrcPony/pgminter.h \
            SrcPony/picbus.h \
            SrcPony/sde2506.h \
            SrcPony/sxbus.h \
            SrcPony/at17xxx.h \
            SrcPony/at89sbus.h \
            SrcPony/at93cxx8.h \
            SrcPony/e2401.h \
            SrcPony/e2pfbuf.h \
            SrcPony/errcode.h \
            SrcPony/i2cbus.h \
            SrcPony/jdminter.h \
            SrcPony/pic125xx.h \
            SrcPony/picbusnew.h \
            SrcPony/progoption.h \
            SrcPony/sdebus.h \
            SrcPony/types.h \
            SrcPony/at250bus2.h \
            SrcPony/at89sxx.h \
            SrcPony/at93cxx.h \
            SrcPony/crc.h \
            SrcPony/e24xx-1.h \
            SrcPony/e2phead.h \
            SrcPony/filebuf.h \
            SrcPony/imbus.h \
            SrcPony/linuxsysfsint.h \
            SrcPony/pic12bus.h \
            SrcPony/ponyioint.h \
            SrcPony/resource.h \
            SrcPony/sernumdlg.h \
            SrcPony/wait.h \
            SrcPony/at250bus.h \
            SrcPony/at90sbus.h \
            SrcPony/binfbuf.h \
            SrcPony/csmfbuf.h \
            SrcPony/e24xx-2.h \
            SrcPony/e2profil.h \
            SrcPony/filldlg.h \
            SrcPony/infomdlg.h \
            SrcPony/lpt_ext_interf.h \
            SrcPony/motsfbuf.h \
            SrcPony/pic168xx.h \
            SrcPony/portint.h \
            SrcPony/spi-bus.h \
            SrcPony/x2444bus.h \
            SrcPony/at250xx.h \
            SrcPony/at90sxx.h \
            SrcPony/businter.h \
            SrcPony/e24xx-5.h \
            SrcPony/easyi2c_interf.h \
            SrcPony/fusemdlg.h \
            SrcPony/intfbuf.h \
            SrcPony/lptinterf.h \
            SrcPony/nvm3060.h \
            SrcPony/pic16xx.h \
            SrcPony/ppuser.h \
            SrcPony/rs232int.h \
            SrcPony/string_table.h \
            SrcPony/x2444.h \
            qhexedit2/src/chunks.h \
            qhexedit2/src/commands.h \
            qhexedit2/src/qhexedit.h

FORMS    += SrcPony/forms/aboutdlg.ui \ 
            SrcPony/forms/blockdlg.ui \
            SrcPony/forms/editdlg.ui \
            SrcPony/forms/notesdlg.ui \
            SrcPony/forms/e2dlg.ui \
            SrcPony/forms/fusedlg.ui \
            SrcPony/forms/mainwindow.ui \
            SrcPony/forms/osccalibr.ui \
            SrcPony/forms/progoption.ui \
            SrcPony/forms/sernumcfg.ui

	    
# TRANSLATIONS = sources/translations/cnc_qt_en.ts \
# 	    sources/translations/cnc_qt_ru.ts \
# 	    sources/translations/cnc_qt_de.ts
               
	    
# TODO: please check this
win32:LIBS += -L$$PWD/windows/ 
# contains(QT_ARCH, i386) {
#     win32:LIBS += -LC:/Program Files (x86)/\libusb-1.0.19\Win32\Debug\lib\libusb-1.0
#     win32:LIBS += -LC:\libusb-1.0.19\Win32\Debug\lib\libusb-1.0
#     win32:message("32-bit")
# } else {
#     win32:LIBS += -LC:/Program Files/\libusb-1.0.19\Win32\Debug\lib\libusb-1.0
#     win32:LIBS += -LC:\libusb-1.0.19\Win32\Debug\lib\libusb-1.0
#     win32:message("64-bit")
# }


# TODO: please add this for macx
# macx:LIBS +=


# unix:!macx: LIBS += -L/usr/local/lib 



RESOURCES += SrcPony/ponyprog.qrc


QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS_RELEASE += -O3

