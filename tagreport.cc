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

using namespace std;

extern int yyparse (void);

static vector<struct Song*>* traverse_dir (char* begin);
static void clean (vector<struct Song*>* root);

#ifdef NDEBUG
bool verbose = false;
#else
bool verbose = true;
#endif

int main (int argc, char* argv [])
{
  struct stat id;
  vector <struct Song*>* all_songs = NULL;
  ostringstream tmpout;
  char *target = NULL, *outfile = NULL;
  int opt;
  
  template_file = NULL;
  
  /* read all options - for now only -o */
  while ((opt = getopt(argc, argv, "ht:o:v")) != -1)
  {
    switch(opt)
    {
      case 'h':
        puts(usage);
	return 0;

      case 'o':
        outfile = strdup(optarg);
        break;
        
      case 'v':
        verbose = true;
        break;

      case 't':
        if ((template_file = fopen (optarg, "r")) == NULL)
	{
          cerr << "Error reading template " << optarg << ": " << strerror(errno) << endl;
	  return 1;
	}
	else
	{
          yyparse();
          fclose (template_file);
	}
	break;
        
      case '?':
        return 1;
    }
  }
    
  if (optind < argc)
    target = strdup(argv[optind]);
  else
  {
    target = (char*)malloc(PATH_MAX + 1);;
    getcwd(target, PATH_MAX);
  }

  stat (target, &id);
  
  if (S_ISDIR(id.st_mode))
  {
    unsigned int i;
    ofstream out;
        
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
    
    if (verbose)
      cout << "Scanning ";
    
    all_songs = traverse_dir(target);

    if (verbose)
      cout << endl;
    
    /* Write out canned headers */
    out << HTMLdtd << "<html>\n<head>\n<title>" << endl;

    if (template_title.is_set())
      out << replace_header (template_title.get(), all_songs->size(), target);
    else
      out << "TagReport Generated Playlist";

    out << endl << "</title>" << endl;

    /* For CSS and JavaScript and such... */
    if (template_head_body.is_set())
      out << replace_header (template_head_body.get(), all_songs->size(), target) << endl;
   
    if (template_body_tag.is_set())
      out << replace_header (template_body_tag.get(), all_songs->size(), target);
    else
      out << "<body bgcolor=\"#000000\" text=\"#FFFFFF\">";
  
    if (template_header.is_set())
      out << replace_header (template_header.get(), all_songs->size(), target) << endl;

    /* Some statistics ... */
    
    if (template_stats.is_set())
    {
      out << replace_header (template_stats.get(), all_songs->size(), target) << endl;
    }
    else
    {
      char* now = get_time_string();
      
      out << "<h2>" << target << "</h2><hr />" << endl;
      out << "<p>Generated at " << now << "<br />" << endl;
      out << "Scanned " << all_songs->size() << " songs.</p>" << endl;
      out << "<p>";

      free(now);
    }
    
    /* Use literally. */
    if (template_prebody.is_set())
      out << replace_header(template_prebody.get(), all_songs->size(), target) << endl;
    
    /* Loop through the vector and HTML-output its contents. */
    for (i = 0; i < all_songs->size(); i++)
    {
      if (template_body.is_set())
      {
        out << replace_body ((*all_songs)[i]->artist, (*all_songs)[i]->title) << endl;
      }
      else
      {
        if ((*all_songs)[i]->artist != "")
        {
          out << i+1 << ". "
              << (*all_songs)[i]->artist
              << " - " << (*all_songs)[i]->title
              << "<br />" << endl;
        }
        else
          out << i << ". " << (*all_songs)[i]->title << "<br />" << endl;
      }
    }

    /* Footer output. Treated as a header for processing purposes. */
    if (template_footer.is_set())
      out << replace_header (template_footer.get(), all_songs->size(), target);
    else
      out << "</p>" << endl;

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
  }
  /* !S_ISDIR(dino.st_mode) */
  else
  {
    cerr << "Error opening " << target << ": not a directory!" << endl;
    return 1;
  }
 
  /* Miscellaneous memory management */
  if (all_songs)
    clean(all_songs);

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
  TagLib::Tag *tag;
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
          /* Grab the first three letters of the extension (if possible)
           * and store it in ext as lowercase. */
          ext = ext.substr(ext.rfind('.') + 1, 3);
        
          for (e = ext.begin(); e != ext.end(); e++)
            *e = tolower(*e);
        }

        /* Ogg Vorbis or MP3 file? */
        if (ext == "ogg" || ext == "mp3")
        {
          TagLib::FileRef ref (fp.str().c_str());

          assert (!ref.isNull());

          tag = ref.tag();
        
          /* Do they both have artist and title fields? */
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


