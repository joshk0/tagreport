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

#include "tagreport.h"

/* TagLib includes */
#include <fileref.h>
#include <tag.h>

#include <vector>
#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>

using namespace std;

static string HTMLdtd =
"<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n\
<!DOCTYPE html\n\
     PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n\
     \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
	  
static string HTMLheader =
"<html>\n\
<head>\n\
<title>TagReport Generated Playlist</title>\n\
</head>\n\
<body bgcolor=\"#000000\" text=\"#FFFFFF\">\n\
<h2>Playlist of ";

static string HTMLfooter =
"</p><hr /></body>\n\
</html>";

static vector<struct Song*>* traverse_dir (char* begin);
static char* invoked_as;
static void clean (vector<struct Song*>* root);
static void htmlify (string &);

int main (int argc, char* argv [])
{
  struct stat id;
  vector <struct Song*>* all_songs = NULL;
  char* outfile = NULL, *target = NULL;
  invoked_as = argv[0];

  stat (argv[1], &id);
  if (id.st_mode & S_IFDIR)
  {
    unsigned int i;
    int opt;
    ofstream out;
    struct tm *now;
    time_t now_secs;
    char now_date[18];

    /* Get the current time */
    time(&now_secs);
    now = localtime(&now_secs);
    strftime (now_date, 18, "%H:%M, %F", now);
    
    while ((opt = getopt(argc, argv, "o:")) != -1)
    {
      switch(opt)
      {
        case 'o':
          outfile = strdup(optarg);
	  break;
	
	case '?':
	  return 1;
      }
    }
    
    if (optind < argc)
      target = strdup(argv[optind]);
    else
      target = strdup(getenv("PWD"));
    
    if (!outfile) outfile = strdup("playlist.htm");
    out.open (outfile, ios::out | ios::trunc);

    if (!out.is_open())
    {
      cerr << "Failed to open file " << target << ": " << strerror(errno) << endl;
      return 1;
    }
    
    /* Write out canned headers */
    out << HTMLdtd << HTMLheader;
    
    cout << "Scanning ";
    all_songs = traverse_dir(target);
    cout << endl;
    
    out << target << "</h2><hr />" << endl;
    out << "<p>Generated at " << now_date << "<br />" << endl;
    out << "Scanned " << all_songs->size() << " songs.</p>" << endl;
    out << "<p>";
    
    for (i = 0; i < all_songs->size(); i++)
    {
      if ((*all_songs)[i]->artist != "")
      {
        out  << i+1 << ". "
             << (*all_songs)[i]->artist
             << " - " << (*all_songs)[i]->title
             << "<br />" << endl;
      }
      else
        out << i << ". " << (*all_songs)[i]->title << "<br />" << endl;
    }

    out << HTMLfooter;

    out.close();
    cout << "Wrote " << outfile << " successfully." << endl;
  }
  else
  {
    cerr << argv[0] << ": " << target << ": not a directory!" << endl;
    return 1;
  }
 
  /* Miscellaneous memory management */
  if (all_songs)
  {
    clean(all_songs);
    delete all_songs;
  }

  free(target);
  free(outfile);
  
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
    cout << "..";
    fflush(stdout);
    while ((contents = readdir(root)) != NULL)
    {
      string fn = contents->d_name;
      ostringstream fp;

      /* Skip .. and . */
      if (fn == "." || fn == "..")
        continue;

      fp << begin << "/" << fn;
      
      /* Recurse if this entry is actually a directory */
      stat (fp.str().c_str(), &dino);
      
      if (dino.st_mode & S_IFDIR)
      {
        vector<struct Song*>* recursion;
        
#ifndef NDEBUG
        cerr << "DEBUG: Recursing into " << fp.str() << endl;
#endif
        recursion = traverse_dir((char*)fp.str().c_str());
        all_songs->insert (all_songs->end(), recursion->begin(), recursion->end());
        delete recursion; /* Already copied into all_songs */
      }

      /* Otherwise, it's a normal file */
      else
      {
        /* will be used in all cases */
        struct Song * tmpsong = new struct Song;
        
        /* Skip filenames with no extension */
        if (fn.find('.') == string::npos)
        {
#ifndef NDEBUG
          cerr << "DEBUG: skipping extensionless file " << fn << endl;
#endif
          delete tmpsong;
          continue;
        }
        
        else if (fn[fn.length() - 1] == '.')
        {
#ifndef NDEBUG
          cerr << "DEBUG: blank extension for file " << fn << endl;
#endif
          delete tmpsong;
          continue;
        }

        /* Ogg Vorbis or MP3 file? */
        else if (!strcasecmp(strrchr(contents->d_name, '.') + 1, "ogg")
            || !strcasecmp(strrchr(contents->d_name, '.') + 1, "mp3"))
        {
          TagLib::FileRef ref (fp.str().c_str());

          if(ref.isNull())
	  {
            cout << *(char*)0 << endl;
	    exit(1);
	  }

          tag = ref.tag();
        
          if (!tag->artist().isNull() && !tag->title().isNull())
          {
            /* Non-null but empty tag */
            if (tag->artist().isEmpty() || tag->title().isEmpty()) 
            {
              tmpsong->artist = "";
              tmpsong->title = fn.substr(0, fn.rfind('.'));
	      htmlify(tmpsong->title);
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

		htmlify(tmpsong->artist);
		htmlify(tmpsong->title);
		
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

	      htmlify(tmpsong->title);
              all_songs->push_back(tmpsong);
            }
            else
            {
              delete tmpsong;
              continue;
            }
          }
        }
        
#ifndef NDEBUG
        /* Note that we did not do anything with this file */
        else
        {
          cerr << "DEBUG: skipping unrecognized file " << fn << endl;
          delete tmpsong;
        }
#endif
      }
    }
  }
  /* If we get here there was SOME sort of error with opendir() */
  else
  {
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
    delete *v;
}

static void htmlify (string & in)
{
  int i;
  unsigned int c;
  
  static char replacechars [] =
    { '&', '<', '>' };

  static char* replacehtml [] =
    { "&amp;", "&lt;", "&gt;" };

  for (c = 0; c < in.length(); c++)
  {
    for (i = 0; i < 3; i++)
    {
      if (in[c] == replacechars[i])
        in.replace (c, 1, replacehtml[i]);
    }
  }
}
