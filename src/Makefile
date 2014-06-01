include ../common.mk

TARGET = drow-engine
HEADERS = err.h common.h print.h filereader.h gl_draw.h vector.h half_edge.h bezier.h
OBJECTS = print.o filereader.o gl_draw.o vector.o half_edge.o half_edge_AS.o bezier.o
INCS = -I.

CFLAGS += $(shell $(PKG_CONFIG) --cflags gl glu glib-2.0)
LIBS = $(shell $(PKG_CONFIG) --libs gl glu glib-2.0) -lglut -lm
CPPFLAGS += -D_XOPEN_SOURCE -D_XOPEN_SOURCE_EXTENDED -D_GNU_SOURCE

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCS) -c $*.c

all: $(TARGET)

drow-engine.a: $(OBJECTS)
	$(AR) rcu $@ $^

test: drow-engine.a
	$(MAKE) -C test

$(TARGET): $(HEADERS) drow-engine.a main.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCS) \
		-o ../$(TARGET) \
		main.o drow-engine.a $(LDFLAGS) $(LIBS)

install: $(TARGET)
	$(INSTALL_DIR) "$(DESTDIR)$(INSTALL_BINDIR)"
	$(INSTALL_BIN) $(TARGET) "$(DESTDIR)$(INSTALL_BINDIR)"

uninstall:
	rm "$(DESTDIR)$(INSTALL_BINDIR)/$(TARGET)"

clean:
	$(MAKE) -C test clean
	rm -f *.o drow-engine.a $(TARGET) core vgcore*


.PHONY: all clean install test uninstall
