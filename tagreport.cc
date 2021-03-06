/*
 * TagReport: a C++ program that outputs HTML formatted song lists from a
 * specified directory.
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
#include "html.h"
#include "templates.h"
#include "util.h"
#include "ftfuncs.h"

/* STL includes */
#include <vector>
#include <iostream>
#include <sstream>
#include <string>

/* C++ forwarding headers */
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/* C system headers */
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif

using namespace std;

static vector<struct Song*>* traverse_dir (char* begin);

#ifdef NDEBUG
bool verbose = false;
#else
bool verbose = true;
#endif

bool force = false;

char* template_fn = NULL;

int main (int argc, char* argv [])
{
  bool already_template = false;
  vector <struct Song*>* all_songs = NULL;
  vector <char*> targets;
  vector <char*>::iterator t;
  ostringstream tmpout;
  char *target = NULL, *outfile = NULL;
  int opt;

#ifdef HAVE_GETOPT_LONG
  struct option longopts [] = {
    { "help"    , 0, 0, 'h' },
    { "template", 1, 0, 't' },
    { "output"  , 1, 0, 'o' },
    { "verbose" , 0, 0, 'v' },
    { "force"   , 0, 0, 'f' },
    { 0, 0, 0, 0 }
  };
#endif
  
  /* read all options */
#ifdef HAVE_GETOPT_LONG
  while ((opt = getopt_long(argc, argv, "ht:o:vf", longopts, NULL)) != -1)
#else
  while ((opt = getopt(argc, argv, "ht:o:vf")) != -1)
#endif
  {
    switch(opt)
    {
      case 'h':
        puts(usage);
       return 0;

      case 'o':
        if (outfile)
        {
          cerr << "Warning: " << outfile << " will no longer be written to in favor of " << optarg << "." << endl;
          free(outfile);
        }

        outfile = strdup(optarg);
        break;
        
      case 'v':
        verbose = true;
        break;

      case 'f':
        force = true;
        break;
        
      case 't':
        if (already_template)
        { 
          assert(template_fn);
          cout << "Warning: " << template_fn << " will no longer be loaded in favor of " << optarg << "." << endl;
          free(template_fn);
        }
        
        if ((template_fn = guess_fn (optarg)) != NULL)
          already_template = true;

        break;
        
      case '?': /* Erroneous option was passed! */
#ifdef HAVE_GETOPT_LONG
        cout << "Try `tagreport --help' for more information." << endl;
#else
        cout << "Try `tagreport -h' for more information." << endl;
#endif
        return 1;
    }
  }

  /* Load the template into memory. */
  if (template_fn)
    read_template_file(template_fn);
 
  /* Add all targets on the command line. */
  if (optind < argc)
  {
    while (optind < argc)
      targets.push_back(strdup(argv[optind++]));
    
    /* Weed out the paths that do not exist. */
    verify (targets);
  }
  /* Scan the current directory tree by default. */
  else
  {
    target = (char*)malloc(PATH_MAX + 1);
    getcwd(target, PATH_MAX);
    targets.push_back(target);
  }

  /* No valid paths? */
  if (targets.size() == 0)
  {
    cout << "All targets invalidated! :( Exiting." << endl;
    return 1;
  }

  target = comma_delineate(targets);
  
  /* Default output location - $PWD/playlist.htm */
  if (!outfile)
    outfile = strdup("playlist.htm");
  
  tmpout << outfile << '.' << getpid();
  
  /* Actually scan the hierarchy. */
  if (verbose)
    cout << "Scanning ";
  
  for (t = targets.begin(); t != targets.end(); t++)
  {
    vector<struct Song*>* this_dir = traverse_dir(*t);

    /* Append subsequent traversals to all_songs */
    if (all_songs != NULL)
    {
      all_songs->insert (all_songs->end(), this_dir->begin(), this_dir->end());
      delete this_dir;
    }
    else
      all_songs = this_dir;
  }

  if (verbose)
    cout << endl;
  
  write_html (tmpout.str().c_str(), all_songs, target);
 
  /* Move temporary file to the real thing */
  if (rename (tmpout.str().c_str(), outfile) == -1)
  {
    cout << "error moving " << tmpout.str() << " to " << outfile << ": " << strerror(errno) << endl;
    cout << "Note: " << tmpout.str() << " was written to and still exists." << endl;
   
    /* We can't create outfile. */
    outfile = (char*)tmpout.str().c_str();
  }
   
  cout << "Wrote " << outfile;
    
  if (all_songs->size() != 0)
    cout << " (" << all_songs->size() << " songs) successfully." << endl;
  else
    cout << ", but found no valid songs!" << endl;
  
  /* Miscellaneous memory management */
  if (all_songs)
    clean(all_songs);

  clean(targets);

  free(target);
  free(outfile);
  
  return 0;
}

/* traverse_dir()
 * input: path to directory to traverse
 * Returns a vector of struct Song*s that will later be parsed to
 * output the HTML
 */

vector<struct Song*>* traverse_dir (char* begin)
{
  DIR* root;
  struct dirent * contents;
  struct stat dino;
  vector <struct Song *> *all_songs = new vector<struct Song*>;
  
  if ((root = opendir(begin)) != NULL)
  {
    if (verbose)
    {
      cout << "..";
      fflush(stdout);
    }

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
      
      if (S_ISDIR(dino.st_mode))
      {
        vector<struct Song*>* recursion;
        
        DEBUG("Recursing into", fp.str());
        
        recursion = traverse_dir((char*)fp.str().c_str());
        all_songs->insert (all_songs->end(), recursion->begin(), recursion->end());
        delete recursion; /* Already copied into all_songs */
      }

      /* Not a directory, assumed to be a normal file */
      else
      {
        struct Song * this_song = new struct Song;
        if (get_artist_title (this_song, fp.str(), begin))
          all_songs->push_back(this_song);
        else
          delete this_song;
      }
    }
  }
  /* If we get here there was SOME sort of error with opendir() */
  else
  {
    if (verbose)
      cout << endl;
    cerr << "Error reading directory " << begin << ": " << strerror(errno) << endl;
    exit(1);
  }

  closedir(root);

  return all_songs;
}
