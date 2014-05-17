include common.mk

all:
	$(MAKE) -C src all

test:
	$(MAKE) -C src test

check: test
	./test

doc:
	cd doxygen && doxygen

doc-pdf: doc
	$(MAKE) -C doxygen/latex pdf

clean:
	$(MAKE) -C src clean
	rm -rf drow-engine test doxygen/latex/* doxygen/html/* vgcore* core

install:
	$(MAKE) -C install

uninstall:
	$(MAKE) -C uninstall


.PHONY: all doc doc-pdf clean install test uninstall
