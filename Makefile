CC = gcc
CXX = g++
CFLAGS = -g -Wall
CXXFLAGS = $(CFLAGS)

all: tagreport maildirtree

tagreport: tagreport.cc
	$(CXX) $(CXXFLAGS) -I/usr/local/include/taglib $< -o $@ -ltag

maildirtree: maildirtree.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f tagreport maildirtree *.o core a.out
