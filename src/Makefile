# Makefile to generate 'reSolve'
# Linux32=>Linux32, Linux64=>Linux64, MinGw32=>Win32, MinGw64=>Win64

HOST=$(shell uname -o)
PKG=Linux
ifeq ($(HOST),Msys)
   PKG=Win
endif
BITS := $(shell getconf LONG_BIT)

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
GFLAGS=`pkg-config --cflags --libs gtk+-3.0`
LDFLAGS=$(LOPT)
TARGET=reSolve$(PKG)64
GUI=reSolveGui$(PKG)64

all: $(TARGET) $(GUI)
	@rm -f *.gch

$(GUI): fileIo.o exprParser.o reSolveLib.o reSolveGui.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(GFLAGS)

$(TARGET): fileIo.o exprParser.o reSolveLib.o reSolveCli.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

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
	@makeAppDir.sh $(PKG) $(BITS)
