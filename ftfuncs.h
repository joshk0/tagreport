/* File type specific tag-scanning functions for TagReport. */

#ifndef INCLUDED_ftfuncs_h
#define INCLUDED_ftfuncs_h

#include "config.h"
#include "tagreport.h"
#include <string>

#ifdef USE_FLAC
bool get_flac (struct Song * flac, const char * path);
#endif

#ifdef USE_OGG
bool get_ogg (struct Song * ogg, const char * path);
#endif

#ifdef USE_ID3TAG
bool get_mp3 (struct Song * mp3, const char * path);
#endif

#ifdef USE_TAGLIB
bool get_taglib(struct Song * song, const char * path);
#endif

bool get_artist_title (struct Song * song, std::string fn, char* begin);

#endif /* !INCLUDED_ftfuncs_h */
