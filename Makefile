
CXXFLAGS = -g -Wall -Werror

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

.PHONY: clean
clean:
	rm -f *.o *.d
	rm -f testhsm

