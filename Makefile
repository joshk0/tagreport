CXX = g++
CXXFLAGS = -g3 -W -Wall -O0
DEFS = -DNDEBUG

all: tagreport

tagreport: tagreport.cc
	$(CXX) $(CXXFLAGS) $(DEFS) -I/usr/local/include/taglib $< -o $@ -ltag

clean:
	rm -f tagreport *.o core a.out *~
