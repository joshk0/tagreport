CXX = g++
CXXFLAGS = -g3 -W -Wall -O0 $(INCLUDES) $(DEFS)
DEFS = -DNDEBUG
OBJS = tagreport.o lex.yy.o y.tab.o
INCLUDES = -I/usr/local/include/taglib
LIBS = -ltag -ll

all: tagreport

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

tagreport: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LIBS)

tagreport.cc: html.h tagreport.h

lex.yy.c: format.l
	flex -d $<

y.tab.c: format.y
	bison -d -y $<

y.tab.h: y.tab.c

format.y: templates.h
format.l: y.tab.h

clean:
	rm -f tagreport *.o core a.out lex.yy.c y.tab.c y.tab.h *~
