CXX = g++
CXXFLAGS = -g -W -Wall -O0 $(INCLUDES) $(DEFS)
DEFS = -DNDEBUG

SRCS_C = lex.yy.c y.tab.c
SRCS_CXX = tagreport.cc templates.cc
OBJS = $(SRCS_C:.c=.o) $(SRCS_CXX:.cc=.o)

INCLUDES = -I/usr/local/include/taglib
LIBS = -ltag

all: tagreport

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

tagreport: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LIBS)

lex.yy.c: format.l
	flex $<

y.tab.h y.tab.c: format.y
	bison -d -y $<

lex.yy.c: templates.h y.tab.h
y.tab.c: templates.h 

.depend: $(SRCS_CXX)
	$(CXX) -MM $(SRCS_CXX) > .depend

clean:
	rm -f tagreport *.o core a.out lex.yy.c y.tab.c y.tab.h *~

-include .depend
