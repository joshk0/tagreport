/* Headers for utility functions in TagReport. */

#ifndef INCLUDED_util_h
#define INCLUDED_util_h

#include "config.h"
#include "tagreport.h"
#include <vector>
#include <string>

using namespace std;

char* get_time_string (void);
char* guess_fn (char* a);
char* comma_delineate (const vector<char*> & in);
void htmlify (string &);
void verify (vector<char*> & targets);
void clean (vector<struct Song*>* root);
void clean (vector<char*> & dirs);

#endif /* !INCLUDED_util_h */
