/* File type specific tag-scanning functions for TagReport. */

#ifndef INCLUDED_ftfuncs_h
#define INCLUDED_ftfuncs_h

#include "config.h"
#include "tagreport.h"

#ifdef USE_FLAC
bool get_flac (struct Song* song, const char* path);
#endif

bool get_taglib(struct Song* song, const char* path);

#endif
