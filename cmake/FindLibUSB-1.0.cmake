# - try to find libusb-1 library
#
# Cache Variables: (probably not for direct use in your scripts)
#  LIBUSB_LIBRARY
#  LIBUSB_INCLUDE_DIR
#
# Non-cache variables you should use in your CMakeLists.txt:
#  LIBUSB_LIBRARIES
#  LIBUSB_INCLUDE_DIRS
#  LIBUSB_FOUND - if this is not true, do not attempt to use this library
#  LIBUSB_VERSION
#
# Requires these CMake modules:
#  ProgramFilesGlob
#  FindPackageHandleStandardArgs (known included with CMake >=2.6.2)
#
# Original Author:
# 2009-2010 Ryan Pavlik <rpavlik@iastate.edu> <abiryan@ryand.net>
# http://academic.cleardefinition.com
# Iowa State University HCI Graduate Program/VRAC
#
# Copyright Iowa State University 2009-2010.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


set(LIBUSB_ROOT_DIR
	"${LIBUSB_ROOT_DIR}"
	CACHE
	PATH
	"Root directory to search for libusb-1")

if(WIN32)
	include(ProgramFilesGlob)
# 	program_files_fallback_glob(_dirs "LibUSB-Win32")
	program_files_fallback_glob(_dirs "libusb-1.*")
	message(STATUS "Found libusb in directory: ${_dirs}" )
# 	program_files_fallback_glob(_dirs "libusb-win32")
# 	message(STATUS "Found libusb in directory: ${_dirs}" )
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		if(MSVC)
			set(_lib_suffixes lib/msvc_x64 MS64/static)
                else()
                        set(_lib_suffixes lib/mingw_x64 MinGW64/dll MinGW64/static)
		endif()
	else()
		if(MSVC)
			set(_lib_suffixes lib/msvc MS32/static)
		else() #(COMPILER_IS_GNUCXX)
			set(_lib_suffixes lib/gcc lib/mingw MinGW32/dll MinGW32/static)
		endif()
	endif()
else()
	set(_lib_suffixes)
	find_package(PkgConfig QUIET)
	if(PKG_CONFIG_FOUND)
		pkg_check_modules(PC_LIBUSB libusb-1.0)
	endif()
endif()

find_path(LIBUSB_INCLUDE_DIR
	NAMES
	libusb.h
	PATHS
	${PC_LIBUSB_INCLUDE_DIRS}
	${PC_LIBUSB_INCLUDEDIR}
	${_dirs}
	HINTS
	"${LIBUSB_ROOT_DIR}"
	PATH_SUFFIXES
	include/libusb-1.0
	include
	libusb-1.0)

find_library(LIBUSB_LIBRARY
	NAMES
	libusb-1.0
	usb-1.0
	PATHS
	${PC_LIBUSB_LIBRARY_DIRS}
	${PC_LIBUSB_LIBDIR}
	${_dirs}
	HINTS
	"${LIBUSB_ROOT_DIR}"
	PATH_SUFFIXES
	${_lib_suffixes})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Libusb-1.0
	DEFAULT_MSG
	LIBUSB_LIBRARY
	LIBUSB_INCLUDE_DIR)

	
if(LIBUSB_INCLUDE_DIR AND LIBUSB_LIBRARY)
    set(LIBUSB_FOUND true)
endif()


if(LIBUSB_FOUND)
	set(LIBUSB_LIBRARIES "${LIBUSB_LIBRARY}")

	set(LIBUSB_INCLUDE_DIRS "${LIBUSB_INCLUDE_DIR}")

	mark_as_advanced(LIBUSB_ROOT_DIR)
endif()

if(LIBUSB_FOUND AND NOT CMAKE_CROSSCOMPILING)
    if(LIBUSB_SKIP_VERSION_CHECK)
        message(STATUS "Skipping libusb version number check.")
        unset(LIBUSB_VERSION)
    else()
        message(STATUS "Checking libusb version...")

        if(WIN32)
            string(REPLACE ".lib" ".dll" LIBUSB_DLL "${LIBUSB_LIBRARIES}")
            try_run(LIBUSB_VERCHECK_RUN_RESULT
                    LIBUSB_VERCHECK_COMPILED
                    ${CMAKE_HELPERS_BINARY_DIR}
                    ${CMAKE_HELPERS_SOURCE_DIR}/libusb_version.c
                    CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${LIBUSB_INCLUDE_DIRS}"
                    RUN_OUTPUT_VARIABLE LIBUSB_VERSION
                    ARGS "\"${LIBUSB_DLL}\""
            )
        else()
            try_run(LIBUSB_VERCHECK_RUN_RESULT
                    LIBUSB_VERCHECK_COMPILED
                    ${CMAKE_HELPERS_BINARY_DIR}
                    ${CMAKE_HELPERS_SOURCE_DIR}/libusb_version.c
                    CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${LIBUSB_INCLUDE_DIRS}" "-DLINK_LIBRARIES=${LIBUSB_LIBRARIES}"
                    RUN_OUTPUT_VARIABLE LIBUSB_VERSION
            )
        endif()

        if (NOT LIBUSB_VERCHECK_COMPILED OR NOT LIBUSB_VERCHECK_RUN_RESULT EQUAL 0 )
            message(STATUS "${LIBUSB_VERSION}")
            set(LIBUSB_VERSION "0.0.0")
            message(WARNING "\nFailed to compile (compiled=${LIBUSB_VERCHECK_COMPILED}) or run (retval=${LIBUSB_VERCHECK_RUN_RESULT}) libusb version check.\n"
                             "This may occur if libusb is earlier than v1.0.16.\n"
                             "Setting LIBUSB_VERSION to ${LIBUSB_VERSION}.\n")
            return()
        endif()
    endif()
endif()

#  
mark_as_advanced(LIBUSB_LIBRARY LIBUSB_INCLUDE_DIR )

