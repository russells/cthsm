
CXXFLAGS = -g -Wall -Werror -I $(CTHSMINC)

SRCS = t1.cc

OBJS = $(SRCS:.cc=.o)
DEPS = $(SRCS:.cc=.d)
DEPDEPS = Makefile

.SUFFIXES: .cc .d
%.d: %.cc $(DEPDEPS)
	@echo DEPENDS: $<
	@$(CXX) -MM -MD -E $(CXXFLAGS) $<

default:
	@echo No default target: t1 t2 clean
	@false

t1: t1.cc t1.hh

t2: t2.cc

t1: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)


ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.PHONY: clean realclean
clean:
	rm -f *.o *.d
	rm -f t1 t2
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
