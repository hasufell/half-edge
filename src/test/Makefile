include ../../common.mk

TARGET = test
HEADERS = cunit.h
OBJECTS = cunit.o cunit_filereader.o cunit_half_edge.o cunit_vector.o
INCS = -I. -I..

CFLAGS += $(shell $(PKG_CONFIG) --cflags gl glu glib-2.0)
LIBS = $(shell $(PKG_CONFIG) --libs gl glu glib-2.0) -lglut -lm -lcunit
CPPFLAGS += -D_XOPEN_SOURCE -D_XOPEN_SOURCE_EXTENDED -D_GNU_SOURCE

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

