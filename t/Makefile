
P = $(shell pwd)/..
CTHSMINC=$(P)
export CTHSMINC

default:
	@echo
	@for t in t[0-9][0-9].sh ; \
		do T=`basename $$t .sh` ./$$t || exit $$? ; \
	done

OUTPUTFILES = run-error run-output run-exitcode test-output test-error
export OUTPUTFILES

.PHONY: clean
clean:
	rm -f $(OUTPUTFILES)
	for d in t[0-9][0-9] ; do if test -f $$d/Makefile ; then $(MAKE) -C $$d clean ; fi ; done
