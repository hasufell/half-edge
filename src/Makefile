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

TARGET = drow-engine
HEADERS = err.h parser.h types.h print.h filereader.h gl_draw.h vec_math.h
OBJECTS = main.o parser.o print.o filereader.o gl_draw.o vec_math.c
INCS = -I.

CFLAGS += $(shell $(PKG_CONFIG) --cflags gl glu glib-2.0)
LIBS = $(shell $(PKG_CONFIG) --libs gl glu glib-2.0) -lglut -lm
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

$(TARGET): $(HEADERS) $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCS) -o ../$(TARGET) $(OBJECTS) $(LDFLAGS) $(LIBS)

install: $(TARGET)
	$(INSTALL_DIR) "$(DESTDIR)$(INSTALL_BINDIR)"
	$(INSTALL_BIN) $(TARGET) "$(DESTDIR)$(INSTALL_BINDIR)"

uninstall:
	rm "$(DESTDIR)$(INSTALL_BINDIR)/$(TARGET)"

clean:
	rm -f *.o $(TARGET) core vgcore*


.PHONY: all clean install uninstall
