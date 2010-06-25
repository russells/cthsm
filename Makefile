
.PHONY: default
default:
	@echo Nothing to make.  Try "make test".

.PHONY: clean
clean: docclean testclean

CTHSM.dox: CTHSM.dox.header README CTHSM.dox.footer
	cat $^ > $@

.PHONY: test
test: default
	cd t && make

.PHONY: testclean
testclean:
	cd t && make clean

.PHONY: doco docclean
doco: CTHSM.dox
	mkdir -p doc
	doxygen Doxyfile

docclean:
	rm -f CTHSM.dox
	rm -rf doc
