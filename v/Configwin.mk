#---------------------------------------------------------------------
# Select Debug or no Debug
#---------------------------------------------------------------------
#DEBUG	= yes
DEBUG	= no

OSNAME := $(shell uname -s | cut -c 1-5)

ifeq ($(OSNAME),MINGW)
TOOLCHAINPREFIX = 
else
TOOLCHAINPREFIX = i586-mingw32msvc-
endif

CC	= $(TOOLCHAINPREFIX)gcc
CXX	= $(TOOLCHAINPREFIX)g++
RANLIB	= $(TOOLCHAINPREFIX)ranlib

PONYVER = 2.08b

#---------------------------------------------------------------------
# Define filename extensions that are targeted by cleanup's
#---------------------------------------------------------------------
CLEANEXTS= *.bak *~ *.tmp core *.o
