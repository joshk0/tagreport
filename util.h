/* Headers for utility functions in TagReport. */

#ifndef INCLUDED_util_h
#define INCLUDED_util_h

#include <vector>

char* get_time_string (void);
void htmlify (string &);
char* guess_fn (char* a);
char* comma_delineate (const std::vector<char*> & in);

#endif
