/*
 * TagReport: a C++ program that will eventually output HTML formatted
 * song lists from a specified directory.
 *
 * (C) 2003 Joshua Kwan <joshk@triplehelix.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define DEBUG

#include "tagreport.h"

#include <taglib/vorbisfile.h>
#include <taglib/mpegfile.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/id3v2tag.h>

#include <cassert>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

using namespace std;

static void traverse_dir (char* begin);
static char* invoked_as;

int main (int argc, char* argv [])
{
  struct stat id;
  invoked_as = argv[0];
  
  if (argc < 2)
  {
    printf ("%s: not enough arguments (syntax: %s [directory])\n", argv[0], basename(argv[0]));
    exit(1);
  }
  else
  {
    stat (argv[1], &id);
    if (id.st_mode & S_IFDIR)
    {
      traverse_dir(argv[1]);
    }
    else
    {
      fprintf (stderr, "%s: %s: not a directory!\n", argv[0], argv[1]);
      return 1;
    }
  }
  
  return 0;
}

/* traverse_dir()
 * input: path to directory to traverse
 * output: playlist.htm (XXX its currently stdout)
 */

void traverse_dir (char* begin)
{
  DIR* root;
  struct dirent * contents;
  struct stat dino;
  char *comp, *fullpath = NULL;
  const char *tmpartist = NULL, *tmptitle = NULL;
  TagLib::Tag *tag;
  
  if ((root = opendir(begin)) != NULL)
  {
    while ((contents = readdir(root)) != NULL)
    {
      int len = strlen(begin) + strlen(contents->d_name) + 2;
      fullpath = (char*)malloc(len);
      
      snprintf(fullpath, len, "%s/%s", begin, contents->d_name);

      /* Skip .. and . */
      if (!strcmp(contents->d_name, ".") || !strcmp(contents->d_name, ".."))
      {
        free(fullpath);
        continue;
      }

      /* Recurse if this entry is actually a directory */
      stat (fullpath, &dino);
      
      if (dino.st_mode & S_IFDIR)
      {
#ifdef DEBUG
        fprintf(stderr, "DEBUG: Recursing into %s\n", fullpath);
#endif
        traverse_dir(fullpath);
      }

      /* Otherwise, it's a normal file */
      else
      {
        /* Skip filenames with no extension */
        if ((comp = strrchr(contents->d_name, '.')) == NULL)
        {
#ifdef DEBUG
          fprintf(stderr, "DEBUG: skipping extensionless file %s\n", contents->d_name);
#endif
          free(fullpath);
        }
        
	else if (contents->d_name[strlen(contents->d_name) - 1] == '.')
	{
#ifdef DEBUG
          fprintf(stderr, "DEBUG: blank extension for file %s\n", contents->d_name);
#endif
	  free(fullpath);
	}

        /* Ogg Vorbis or MP3 file? */
	else if (!strcasecmp(strrchr(contents->d_name, '.') + 1, "ogg")
            || !strcasecmp(strrchr(contents->d_name, '.') + 1, "mp3"))
	{
          TagLib::FileRef ref (fullpath);

          if(ref.isNull())
            abort();

          tag = ref.tag();
        
	  if (!tag->artist().isNull() && !tag->title().isNull())
	  {
	    if (tag->artist().isEmpty() || tag->title().isEmpty())
	    {
	      char *tmp = strrchr(contents->d_name, '.');

	      if (tmp)
	      {
		*tmp = '\0';
		puts(contents->d_name);
	      }
	      else
		abort();
	    }
	    else
	    {
	      char *itera, *itert;
	      tmpartist = tag->artist().toCString();
	      tmptitle = tag->title().toCString();
	      
	      itera = (char*)tmpartist;
	      itert = (char*)tmptitle;
	      
	      itera += strlen(tmpartist)-1;
	      itert += strlen(tmptitle)-1;
	      
	      while (*itera == ' ')
	      {
		*itera = '\0';
	        itera--;
	      }

	      while (*itert == ' ')
	      {
		*itert = '\0';
	        itert--;
	      }
	      
	      if (strcmp(tmpartist, "") && strcmp(tmptitle, ""))
	      {
		/* stuff things into struct Song */
	        printf("%s - %s\n", tmpartist, tmptitle, contents->d_name);
	      }
	      else
		goto printfn;
	    }
	  }
	  else
	  {
printfn:
            /* strip the extension from a filename without ID */
	    
            char* tmp = strrchr(contents->d_name, '.');

	    if (tmp)
	    {
	      *tmp  = '\0';
	      puts(contents->d_name);
	    }
	    else
	      abort(); /* SHOULD NOT HAPPEN */
	    
	  }
	}
        
#ifdef DEBUG
        /* Note that we did not do anything with this file */
        else
	{
          fprintf(stderr, "DEBUG: skipping unrecognized file %s\n", contents->d_name, contents->d_type);
        }
#endif
      }

      free (fullpath);
    }
  }
  /* If we get here there was SOME sort of error with opendir() */
  else {
    fprintf (stderr, "%s: Error reading directory: %s\n", invoked_as, strerror(errno));
    return;
  }

  closedir(root);
}
