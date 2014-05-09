all:
	$(MAKE) -C src all

doc:
	doxygen

doc-pdf: doc
	$(MAKE) -C latex pdf

clean:
	$(MAKE) -C src clean
	rm -rf drown-engine

install:
	$(MAKE) -C install

uninstall:
	$(MAKE) -C uninstall


.PHONY: all doc doc-pdf clean install uninstall
