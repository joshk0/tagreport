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

/* TagLib includes */
#include <fileref.h>
#include <tag.h>

#include <vector>
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

using namespace std;

static vector<struct Song*>* traverse_dir (char* begin);
static char* invoked_as;
static void clean (vector<struct Song*>* root);

int main (int argc, char* argv [])
{
  struct stat id;
  vector <struct Song*>* all_songs;
  invoked_as = argv[0];
  
  if (argc < 2)
  {
    cout << argv[0] << ": not enough arguments (syntax: " << basename(argv[0]) << " [directory])" << endl;
    exit(1);
  }
  else
  {
    stat (argv[1], &id);
    if (id.st_mode & S_IFDIR)
    {
      unsigned int i;
      all_songs = traverse_dir(argv[1]);

      for (i = 0; i < all_songs->size(); i++)
      {
	if ((*all_songs)[i]->artist != "")
	{
          cout << i << ". "
	       << (*all_songs)[i]->artist
	       << " - " << (*all_songs)[i]->title
	       << endl;
	}
	else
          cout << i << ". " << (*all_songs)[i]->title << endl;
      //output_html("playlist.htm", all_songs);
      }
    }
    else
    {
      cerr << argv[0] << ": " << argv[1] << ": not a directory!" << endl;
      return 1;
    }
  }
  
  clean(all_songs);
  return 0;
}

/* traverse_dir()
 * input: path to directory to traverse
 * output: playlist.htm (XXX its currently stdout)
 */

vector<struct Song*>* traverse_dir (char* begin)
{
  DIR* root;
  struct dirent * contents;
  struct stat dino;
  TagLib::Tag *tag;
  vector <struct Song *> *all_songs = new vector<struct Song*>;
  
  if ((root = opendir(begin)) != NULL)
  {
    while ((contents = readdir(root)) != NULL)
    {
      string fn = contents->d_name;
      ostringstream fp;
     
      fp << begin << "/" << fn;

      /* Skip .. and . */
      if (fn == "." || fn == "..")
        continue;

      /* Recurse if this entry is actually a directory */
      stat (fp.str().c_str(), &dino);
      
      if (dino.st_mode & S_IFDIR)
      {
	vector<struct Song*>* recursion;
	
#ifdef DEBUG
        cerr << "DEBUG: Recursing into " << fp.str() << endl;
#endif
	recursion = traverse_dir((char*)fp.str().c_str());
        all_songs->insert (all_songs->end(), recursion->begin(), recursion->end());
      }

      /* Otherwise, it's a normal file */
      else
      {
	/* will be used in all cases */
        struct Song * tmpsong = (struct Song *)malloc(sizeof(struct Song));
        
	/* Skip filenames with no extension */
        if (fn.find('.') == string::npos)
        {
#ifdef DEBUG
          cerr << "DEBUG: skipping extensionless file " << fn << endl;
#endif
	  free(tmpsong);
	  continue;
        }
        
	else if (fn[fn.length() - 1] == '.')
	{
#ifdef DEBUG
          cerr << "DEBUG: blank extension for file " << fn << endl;
#endif
	  free(tmpsong);
	  continue;
	}

        /* Ogg Vorbis or MP3 file? */
	else if (!strcasecmp(strrchr(contents->d_name, '.') + 1, "ogg")
            || !strcasecmp(strrchr(contents->d_name, '.') + 1, "mp3"))
	{
          TagLib::FileRef ref (fp.str().c_str());

          if(ref.isNull())
            abort();

          tag = ref.tag();
        
	  if (!tag->artist().isNull() && !tag->title().isNull())
	  {
            /* Non-null but empty tag */
	    if (tag->artist().isEmpty() || tag->title().isEmpty()) 
	    {
	      tmpsong->artist = "";
	      tmpsong->title = fn.substr(0, fn.rfind('.'));
	      all_songs->push_back(tmpsong);
	    }
	    else /* Tag is non-empty */
	    {
              TagLib::String a, t;
	      
	      a = tag->artist().stripWhiteSpace();
	      t = tag->title().stripWhiteSpace();
              
	      if (!a.isEmpty() && !t.isEmpty())
	      {
		tmpsong->artist = a.to8Bit(false);
		tmpsong->title = t.to8Bit(false);
		all_songs->push_back(tmpsong);
	      }
	      else
		goto printfn;
	    }
	  }
	  else /* NO TAG! */
	  {
            /* strip the extension from a filename without ID */
printfn:
            string noext = fn.substr(0, fn.rfind('.'));

	    if (noext != "")
	    {
	      tmpsong->artist = "";
	      tmpsong->title = noext;
	      all_songs->push_back(tmpsong);
	    }
	    else
	    {
              free(tmpsong);
	      continue;
	    }
	  }
	}
        
#ifdef DEBUG
        /* Note that we did not do anything with this file */
        else
	{
          cerr << "DEBUG: skipping unrecognized file " << fn << endl;
	  free(tmpsong);
        }
#endif
      }
    }
  }
  /* If we get here there was SOME sort of error with opendir() */
  else {
    cerr << invoked_as << ": Error reading directory: " << strerror(errno) << endl;
    exit(1);
  }

  closedir(root);

  return all_songs;
}

void clean (vector<struct Song *> * root)
{
  vector<struct Song *>::iterator v;

  for (v = root->begin(); v != root->end(); v++)
    free (*v);
}
