
CXXFLAGS = -g -Wall -Werror -I $(CTHSMINC)

SRCS = testhsm.cc

OBJS = $(SRCS:.cc=.o)
DEPS = $(SRCS:.cc=.d)
DEPDEPS = Makefile

.SUFFIXES: .cc .d
%.d: %.cc $(DEPDEPS)
	@echo DEPENDS: $<
	@$(CXX) -MM -MD -E $(CXXFLAGS) $<

default: testhsm

testhsm: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)


ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.PHONY: clean realclean
clean:
	rm -f *.o *.d
	rm -f testhsm
	rm -f $(OUTPUTFILES)

realclean: clean docclean

CTHSM.dox: CTHSM.dox.header README CTHSM.dox.footer
	cat $^ > $@

.PHONY: test
test: default
	cd t && make

.PHONY: doco docclean
doco: CTHSM.dox
	mkdir -p doc
	doxygen Doxyfile

docclean:
	rm -f CTHSM.dox
	rm -rf doc
