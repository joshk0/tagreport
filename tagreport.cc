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

/* TagLib includes */
#include <fileref.h>
#include <tag.h>

/* STL includes */
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

/* C++ forwarding headers */
#include <cassert>
#include <cctype>
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

extern int yyparse (void);
extern FILE *yyin;

static vector<struct Song*>* traverse_dir (char* begin);
static void clean (vector<struct Song*>* root);
static void clean (vector<char*> & dirs);

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
  vector<char*>::iterator t;
  ostringstream tmpout;
  char *target = NULL, *outfile = NULL;
  int opt;
  unsigned int i;
  ofstream out;
        
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
  {
    if ((template_file = fopen (template_fn, "r")) == NULL)
    {
      cerr << "Error reading template " << template_fn << ": " << strerror(errno) << endl;
      free(template_fn);
      return 1;
    }
    else
    {
      yyin = template_file;
      yyparse();
      fclose(template_file);
      free(template_fn);
    }
  }
  
  if (optind < argc)
  {
    while (optind < argc)
      targets.push_back(strdup(argv[optind++]));
  }
  /* Scan the current directory tree by default. */
  else
  {
    target = (char*)malloc(PATH_MAX + 1);;
    getcwd(target, PATH_MAX);
    targets.push_back(target);
  }

  /* Weed out the paths that do not exist. */
  verify (targets);

  /* They ALL don't exist? */
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
    
  out.open (tmpout.str().c_str(), ios::out | ios::trunc);

  if (!out.is_open())
  {
    cerr << "Failed to open file " << tmpout.str() << ": " << strerror(errno) << endl;
    return 1;
  }
  
  /* Actually scan the hierarchy. */
  if (verbose)
    cout << "Scanning ";
  
  for (t = targets.begin(); t != targets.end(); t++)
  {
    vector<struct Song*>* this_dir = NULL;
    this_dir = traverse_dir(*t);

    /* Append this directory */

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
    
  /* Write out canned headers to the target file */
  out << HTMLdtd << "<html>\n<head>\n<title>" << endl;

  assert (template_title.is_set());
  
  OUTPUT_HEADER(out, all_songs, target, template_title);
  out << "</title>" << endl;

  /* For CSS and JavaScript and such... */
  OUTPUT_HEADER_IF_SET(out, all_songs, target, template_head_body);
  out << "</head>" << endl;

  assert (template_body_tag.is_set());
  OUTPUT_HEADER(out, all_songs, target, template_body_tag);
  OUTPUT_HEADER_IF_SET(out, all_songs, target, template_header);
    
  /* Some statistics ... */
  assert (template_stats.is_set());
  OUTPUT_HEADER(out, all_songs, target, template_stats);
    
  /* Use literally. */
  OUTPUT_HEADER_IF_SET(out, all_songs, target, template_prebody);
    
  /* Loop through the vector and HTML-output its contents. */
  assert (template_body.is_set());
  for (i = 0; i < all_songs->size(); i++)
    OUTPUT_BODY(out, (*all_songs)[i], i + 1);

  /* Footer output. Treated as a header for processing purposes. */
  assert (template_footer.is_set());
  OUTPUT_HEADER(out, all_songs, target, template_footer);
  out << "</body>\n</html>" << endl;

  /* Flush the file and close it */
  out.close();

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
        /* will be used in all cases */
        struct Song * tmpsong = new struct Song;
        string ext = fn;

        /* Skip filenames with no extension */
        if (fn.find('.') == string::npos)
        {
          DEBUG("skipping extensionless file", fn);
          
          delete tmpsong;
          continue;
        }

        /* Extension is there, but nothing */
        else if (fn[fn.length() - 1] == '.')
        {
          DEBUG("blank extension for file", fn);

          delete tmpsong;
          continue;
        }

        /* Valid extension */
        else
        {
          string::iterator e;
          /* Grab the first four letters of the extension (if possible)
           * and store it in ext as lowercase. */
          ext = ext.substr(ext.rfind('.') + 1, 4);
        
          for (e = ext.begin(); e != ext.end(); e++)
            *e = tolower(*e);
        }
#ifdef HAVE_METAFLAC
	if (ext == "flac") /* Free Lossless Audio Codec - no TagLib support */
        {
          metaflac(tmpsong, fp.str().c_str());

	  if (tmpsong->title == "" || tmpsong->artist == "")
            delete tmpsong; /* Just ignore it. */
	  else
            all_songs->push_back(tmpsong);
        }
	
        /* Ogg Vorbis or MP3 file? */
	else if (ext == "ogg" || ext == "mp3")
#else
        if (ext == "ogg" || ext == "mp3")
#endif

        {
          TagLib::Tag *tag;
          TagLib::FileRef ref (fp.str().c_str());

          assert (!ref.isNull());

          if ((tag = ref.tag()) == NULL)
          {
            DEBUG("Null tag; corrupt file encountered?", 0);
            continue;
          }
        
          /* Do they both have artist and title fields? */
          if (!tag->artist().isNull() && !tag->title().isNull())
          {
            /* Non-null but empty tag */
            if (tag->artist().isEmpty() || tag->title().isEmpty()) 
            {
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
              tmpsong->title = noext;

              htmlify(tmpsong->title);
              all_songs->push_back(tmpsong);
            }
            else /* This shouldn't happen, right? We checked for it before */
            {
              delete tmpsong;
              continue;
            }
          }
        }
        /* we did not do anything with this file.. */
        else
        {
          DEBUG ("skipping unrecognized file", fn);
          delete tmpsong;
        }
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

/* Simply purges the entries in a vector. */
void clean (vector<struct Song *> * root)
{
  vector<struct Song *>::iterator v;

  /* All strings are on the stack, we don't need to do anything with them */
  for (v = root->begin(); v != root->end(); v++)
    delete *v;

  delete root;
}

void clean (vector<char*> & dirs)
{
  vector<char*>::iterator t;

  for (t = dirs.begin(); t != dirs.end(); t++)
    free (*t);
}
