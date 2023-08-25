# Makefile for rmcrap, Copyright (C) 2009  Clint Allen

# x86
CFLAGS = -Wall -O2 -march=native
# Sparc
#CFLAGS = -O2 -mcpu=v9

#DEBUG = -ggdb

LIBS = -lstdc++
PREFIX = ${HOME}

all:	rmcrap.cc
	gcc ${CFLAGS} ${LIBS} ${DEBUG} -o rmcrap rmcrap.cc

install:
	strip -s rmcrap
	install -s rmcrap ${PREFIX}/bin
