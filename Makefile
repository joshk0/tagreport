CXX = g++
CXXFLAGS = -g3 -W -Wall -O0
DEFS = -DNDEBUG

all: tagreport

%.o: %.c
	$(CXX) $(CXXFLAGS) $(DEFS) $< -o $@

tagreport: tagreport.cc tagreport.h html.h
	$(CXX) $(CXXFLAGS) $(DEFS) -I/usr/local/include/taglib $< -o $@ -ltag

lex.yy.c: format.l
	flex -d $<

y.tab.c: format.y
	bison -d -y $<

clean:
	rm -f tagreport *.o core a.out lex.yy.c y.tab.c *~
