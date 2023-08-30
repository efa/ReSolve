# Makefile: Copyright 2005-2023 Valerio Messina efa@iol.it
#
# Makefile is part of ReSolve - solve math expressions using discrete values
# ReSolve is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# ReSolve is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with ReSolve. If not, see <http://www.gnu.org/licenses/>.
#
# Makefile to build 'ReSolve' using GCC+GNUtoolchain on Linux
# Linux32=>Linux32, Linux64=>Linux64, MinGw32=>Win32, MinGw64=>Win64, macOS64=>macOS64
#
# To build for debug use: $ make BUILD=debug debug

HOST=$(shell uname)
PKG=Linux
ifneq ($(HOST),Darwin)
   HOST=$(shell uname -o)
else
   # on macOS run make inside $ jhbuild shell
   PKG=MacOS
endif
ifeq ($(HOST),Msys)
   PKG=Win
endif
BIT := $(shell getconf LONG_BIT)

BUILD?=release
ifeq ($(BUILD),debug)
   COPT=-O1 -g -fsanitize=address -fno-omit-frame-pointer
   LOPT=-fsanitize=address
else
   COPT=-O3
endif

CC=gcc
CFLAGS=-std=gnu11 -Wall $(COPT) -D__USE_MINGW_ANSI_STDIO=1
PKG_CONFIG_PATH=/usr/lib/x86_64-linux-gnu/pkgconfig:$PKG_CONFIG_PATH
GFLAGS=`pkg-config --cflags gtk+-3.0`
LDFLAGS=$(LOPT) -lm
GLDFLAGS=`pkg-config --libs gtk+-3.0`
TARGET=reSolve$(PKG)$(BIT)
GUI=reSolveGui$(PKG)$(BIT)

all: $(TARGET) $(GUI) $(TARGET2)
	@rm -f *.gch

$(GUI): fileIo.o exprParser.o reSolveLib.o reSolveGui.o
	$(CC) $^ $(LDFLAGS) $(GLDFLAGS) -o $@

$(TARGET): fileIo.o exprParser.o reSolveLib.o reSolveCli.o
	$(CC) $^ $(LDFLAGS) -o $@

fileIo.o: comType.h fileIo.h fileIo.c

exprParser.o: exprParser.h exprParser.c

reSolveLib.o: comType.h fileIo.h exprParser.h reSolveLib.h reSolveLib.c

reSolveCli.o: comType.h fileIo.h exprParser.h reSolveLib.h reSolveCli.c

reSolveGui.o: comType.h fileIo.h exprParser.h reSolveLib.h reSolveGui.c
	$(CC) $(CFLAGS) -c $^ $(GFLAGS)

strip:
ifeq ($(OS),Windows_NT) # on MINGW strip need .exe
	strip $(TARGET).exe $(GUI).exe
else
	strip $(TARGET) $(GUI)
endif

cleanobj:
	rm -f *.o
	@rm -f *.gch

cleanbin:
	rm -f $(TARGET) $(GUI)

clean: cleanobj cleanbin

debug: cleanobj all cleanobj

bin: all cleanobj strip

force: clean bin
	rm -f *.o

rel: force
	mv $(TARGET) $(GUI) ..

pkg: rel
	@echo ""
	@makeAppDir.sh $(PKG) $(BIT)
