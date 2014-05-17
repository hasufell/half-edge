include common.mk

all:
	$(MAKE) -C src all

test:
	$(MAKE) -C src test

check: test
	./test

doc:
	doxygen

doc-pdf: doc
	$(MAKE) -C latex pdf

clean:
	$(MAKE) -C src clean
	rm -rf drow-engine test latex/* html/* vgcore* core

install:
	$(MAKE) -C install

uninstall:
	$(MAKE) -C uninstall


.PHONY: all doc doc-pdf clean install test uninstall
