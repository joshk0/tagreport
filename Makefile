CXX = g++
CXXFLAGS = -g -W -Wall -O0

all: maildirtree

tagreport: tagreport.cc
	$(CXX) $(CXXFLAGS) -I/usr/local/include/taglib $< -o $@ -ltag

clean:
	rm -f tagreport *.o core a.out *~
