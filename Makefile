CC = gcc
CXX = g++
CFLAGS = -g -W -Wall -O0
CXXFLAGS = $(CFLAGS)

all: tagreport maildirtree

tagreport: tagreport.cc
	$(CXX) $(CXXFLAGS) -I/usr/local/include/taglib $< -o $@ -ltag

maildirtree: maildirtree.c
	$(CC) $(CFLAGS) $< -o $@

maildirtree.c: maildirtree.h

clean:
	rm -f tagreport maildirtree *.o core a.out
