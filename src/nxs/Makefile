CXX_sources := nxsc.cpp codegen.cpp
C_sources := grammer.tab.c token.yy.c ast.c

# CXXFLAGS += $(shell llvm-config --cxxflags)
LDFLAGS += $(shell llvm-config --ldflags --system-libs --libs core)
CPPFLAGS += -g

nxsc: $(CXX_sources:.cpp=.o) $(C_sources:.c=.o)
	$(CXX) $^ -o $@ $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) 

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CXX) -M $(CPPFLAGS) $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.tab.c %.tab.h: %.y
	bison -d $<

%.yy.c: %.l
	flex -o $@ $<

include $(CXX_sources:.cpp=.d) $(C_sources:.c=.d)


clean:
	find . -type f -name "*.d"|xargs -n1 rm
	find . -type f -name "*.o"|xargs -n1 rm
	rm -rf ./nxs