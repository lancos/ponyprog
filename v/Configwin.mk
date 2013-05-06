#---------------------------------------------------------------------
# Select Debug or no Debug
#---------------------------------------------------------------------
#DEBUG	= yes
DEBUG	= no

OSNAME := $(shell uname -s | cut -c 1-5)

ifeq ($(OSNAME),MINGW)
TOOLCHAINPREFIX = 
else
#TOOLCHAINPREFIX = i586-mingw32msvc-
TOOLCHAINPREFIX = i686-w64-mingw32-
endif

CC	= $(TOOLCHAINPREFIX)gcc
CXX	= $(TOOLCHAINPREFIX)g++
RANLIB	= $(TOOLCHAINPREFIX)ranlib
WINDRES = $(TOOLCHAINPREFIX)windres

#---------------------------------------------------------------------
# Define filename extensions that are targeted by cleanup's
#---------------------------------------------------------------------
CLEANEXTS= *.bak *~ *.tmp core *.o
