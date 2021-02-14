#CMakeLists.txt

SET(PONY_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/aboutmdlg.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/usbwatcher.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/Translator.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/e2app.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/at25xxx.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/at93cbus.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/crc.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/e24xx-1.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/e24xx-2.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/e24xx.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/e24xx-5.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/e2profil.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/fusemdlg.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/bitfield.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/blockmdlg.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/intfbuf.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/lpt_io_interf.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/pgminter.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/picbus.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/progoption.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/sernumdlg.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/x2444.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/at17xxx.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/at89sbus.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/at93cxx8.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/csmfbuf.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/e2cmdw.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/e2awinfo.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/easyi2c_interf.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/i2cbus.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/ispinterf.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/microbus.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/pic125xx.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/picbusnew.cpp
#                  ${CMAKE_CURRENT_SOURCE_DIR}/retrymdlg.cpp
#                 ${CMAKE_CURRENT_SOURCE_DIR}/ch341a.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/spi-bus.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/at250bus2.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/at89sxx.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/at93cxx.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/device.cpp
#                  ${CMAKE_CURRENT_SOURCE_DIR}/e2cnv.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/chipcollection.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/imbus.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/linuxsysfsint.cpp
#                  ${CMAKE_CURRENT_SOURCE_DIR}/modaldlg_utilities.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/pic12bus.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/ponyioint.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/rs232int.cpp
#                  ${CMAKE_CURRENT_SOURCE_DIR}/sxbus.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/at250bus.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/at90sbus.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/binfbuf.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/dt006interf.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/e2dlg.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/filebuf.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/infomdlg.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/lpt_ext_interf.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/mpsse_interf.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/motsfbuf.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/pic168xx.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/portint.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/sde2506.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/wait.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/at250xx.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/at90sxx.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/busio.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/e2401.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/e2pfbuf.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/filldlg.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/interfconv.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/lptinterf.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/nvm3060.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/pic16xx.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/x2444bus.cpp
#                  ${CMAKE_CURRENT_SOURCE_DIR}/profile.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/prefdialog.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/sdebus.cpp)


SET(PONY_NOMOC_HEADERS
                 ${CMAKE_CURRENT_SOURCE_DIR}/e2app.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/e2awinfo.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/Translator.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/globals.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/device.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/microbus.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/at25xxx.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/at93cbus.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/busio.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/dt006interf.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/e24xx.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/chipcollection.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/ispinterf.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/lpt_io_interf.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/mpsse_interf.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/pgminter.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/picbus.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/sde2506.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/at17xxx.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/at89sbus.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/at93cxx8.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/e2401.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/e2pfbuf.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/errcode.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/i2cbus.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/jdminter.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/pic125xx.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/picbusnew.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/sdebus.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/types.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/at250bus2.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/at89sxx.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/at93cxx.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/crc.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/e24xx-1.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/e2phead.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/filebuf.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/imbus.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/linuxsysfsint.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/pic12bus.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/ponyioint.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/resource.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/wait.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/at250bus.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/at90sbus.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/binfbuf.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/csmfbuf.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/e24xx-2.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/e2profil.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/lpt_ext_interf.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/motsfbuf.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/pic168xx.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/portint.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/spi-bus.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/x2444bus.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/at250xx.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/at90sxx.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/businter.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/e24xx-5.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/easyi2c_interf.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/intfbuf.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/lptinterf.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/nvm3060.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/pic16xx.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/ppuser.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/rs232int.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/x2444.h)


SET(PONY_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/fusemdlg.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/usbwatcher.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/bitfield.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/filldlg.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/blockmdlg.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/e2cmdw.h
#                 ${CMAKE_CURRENT_SOURCE_DIR}/ch341a.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/e2dlg.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/aboutmdlg.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/prefdialog.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/progoption.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/sernumdlg.h
                 ${CMAKE_CURRENT_SOURCE_DIR}/infomdlg.h)


SET(PONY_FORMS  ${CMAKE_CURRENT_SOURCE_DIR}/forms/aboutdlg.ui
                ${CMAKE_CURRENT_SOURCE_DIR}/forms/editdlg.ui
                ${CMAKE_CURRENT_SOURCE_DIR}/forms/bitfield.ui
                ${CMAKE_CURRENT_SOURCE_DIR}/forms/notesdlg.ui
                ${CMAKE_CURRENT_SOURCE_DIR}/forms/e2dlg.ui
                ${CMAKE_CURRENT_SOURCE_DIR}/forms/fusedlg.ui
                ${CMAKE_CURRENT_SOURCE_DIR}/forms/mainwindow.ui
                ${CMAKE_CURRENT_SOURCE_DIR}/forms/osccalibr.ui
                ${CMAKE_CURRENT_SOURCE_DIR}/forms/prefdialog.ui
                ${CMAKE_CURRENT_SOURCE_DIR}/forms/progoption.ui
                ${CMAKE_CURRENT_SOURCE_DIR}/forms/sernumcfg.ui
                PARENT_SCOPE)


SET(PONY_SOURCES ${PONY_SOURCES}
		PARENT_SCOPE)

SET(PONY_NOMOC_HEADERS ${PONY_NOMOC_HEADERS}
		PARENT_SCOPE)

SET(PONY_HEADERS ${PONY_HEADERS}
		PARENT_SCOPE)

# SET(FILES_TO_TRANSLATE ${FILES_TO_TRANSLATE} ${MAIN_SOURCES} ${MAIN_FORMS} ${MAIN_HEADERS}
#                  PARENT_SCOPE)


SET(APP_RESOURCES  ${CMAKE_CURRENT_SOURCE_DIR}/ponyprog.qrc
		 PARENT_SCOPE)


# cleanup $build/*.cpp~ on "make clean"
SET_DIRECTORY_PROPERTIES(PROPERTIES
    ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*~")

