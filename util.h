/* Headers for utility functions in TagReport. */

#ifndef INCLUDED_util_h
#define INCLUDED_util_h

#include "config.h"
#include <vector>

char* get_time_string (void);
char* guess_fn (char* a);
char* comma_delineate (const std::vector<char*> & in);
void htmlify (string &);
void verify (std::vector<char*> & targets);

#ifdef HAVE_METAFLAC
bool metaflac (struct Song* flac, const char* path);
#endif

#endif
