# compiler, tools
CC = clang
PKG_CONFIG ?= pkg-config

# flags
CFLAGS ?= -march=native -O2 -pipe
CFLAGS += -std=c99 -pedantic -Wall -Wextra -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function
ifeq ($(shell $(CC) -v 2>&1 | grep 'gcc version' &>/dev/null && echo 1),1)
CFLAGS += -Wno-unused-but-set-variable
endif
LDFLAGS ?= -Wl,-O1 -Wl,--as-needed -Wl,--hash-style=gnu

ifndef NODEBUG
CFLAGS += -O0 -g3
endif

TARGET = test
HEADERS = cunit.h
OBJECTS = cunit.o cunit_filereader.o cunit_half_edge.o
INCS = -I. -I..

CFLAGS += $(shell $(PKG_CONFIG) --cflags gl glu glib-2.0)
LIBS = $(shell $(PKG_CONFIG) --libs gl glu glib-2.0) -lglut -lm -lcunit
CPPFLAGS += -D_XOPEN_SOURCE -D_XOPEN_SOURCE_EXTENDED -D_GNU_SOURCE

# install variables
INSTALL = install
INSTALL_BIN = install -m755
INSTALL_DIR = install -d
PREFIX = /usr/local
LIBDIR = lib64
INSTALL_BINDIR = $(PREFIX)/bin
INSTALL_LIBDIR = $(PREFIX)/$(LIBDIR)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCS) -c $*.c

all: $(TARGET)

drow-engine.a:
	$(MAKE) -C .. $@

$(TARGET): $(HEADERS) $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCS) -o ../../$(TARGET) \
		$(OBJECTS) ../drow-engine.a $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o $(TARGET) core vgcore*


.PHONY: all clean drow-engine.a install uninstall

