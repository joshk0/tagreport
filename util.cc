/* Utility functions for use with TagReport. */

#include "config.h"

#include <cerrno>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <sys/stat.h>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#ifdef HAVE_LIBIBERTY_H
#include <libiberty.h>
#endif

#include <FLAC/metadata.h>

#include "html.h"
#include "tagreport.h"

using namespace std;

/* Allocates stuff on the heap - free it! */
char* get_time_string (void)
{
  struct tm *now;
  time_t now_secs;
  char now_date[18];

  /* Get the current time */
  if (!time(&now_secs))
    perror("time");

  now = localtime(&now_secs);
  strftime (now_date, 18, "%H:%M, %Y-%m-%d", now);

  return strdup(now_date);
}

/* Escapes illegal HTML characters into their long counterparts. */
void htmlify (string & in)
{
  unsigned int c, i;

  /* replace each found character in replacechars by respective 
   * entry in replacehtml */
  for (c = 0; c < in.length(); c++)
  {
    for (i = 0; i < ARRAY_SIZE(replacechars); i++)
    {
      if (in[c] == replacechars[i])
        in.replace (c, 1, replacehtml[i]);
    }
  }

  /* replace spaces with two &nbsp;, it's the slightest bit overkill
   * i.e. htmlify("  ") should become " &nbsp;", but it becomes
   * "&nbsp;&nbsp;" */
  
  while ((c = in.find("  ")) != string::npos)
    in.replace(c, 2, "&nbsp;&nbsp;");
}

char* guess_fn (char* a)
{
  char* ext;
  ostringstream s, t;
  struct stat ex;

  if (stat(a, &ex) == 0)
    return strdup(a);

  /* Conditions:
   *
   * 1. Path is not absolute (prefixed with a forward slash)
   *
   * and either
   *
   * 2. The filename has no extension
   *
   * or
   *
   * 3. The filename has an empty extension
   * 4. The filename's extension is NOT 'def'
   *
   * then we should attempt to guess from our hardcoded paths and
   * the current path with def/.
   */
  else if (*a != '/' && ((ext = strrchr(a, '.')) == NULL ||
       ((a[strlen(a) - 1] != *ext) && strcmp(ext+1, "def"))))
  {
    s << "def/" << a << ".def";

    if (stat (s.str().c_str(), &ex) == 0)
      return strdup(s.str().c_str());
    else
    {
      ostringstream t;
      t << DATADIR << s.str();
	    
      if (stat(t.str().c_str(), &ex) == 0)
        return strdup(t.str().c_str());
      else
        return strdup(a);
     } 
  }

  else
  {
    cerr << "Error reading template " << a << ": " << strerror(errno) << endl;
    if (!force)
    {
      cerr << "Continuing with default template." << endl;
      return NULL;
    }
    else
      exit(1);
  }
}

char* comma_delineate (const vector<char*> & in)
{
  vector<char*>::const_iterator i;
  ostringstream a;

  for (i = in.begin(); i != in.end(); i++)
  {
    if (i + 1 != in.end())
      a << *i << ", ";
    else
      a << *i;
  }

  return strdup(a.str().c_str());
}

void verify (vector<char*> & targets)
{
  vector<char*>::iterator t;
  struct stat id;

  for (t = targets.begin(); t != targets.end(); t++)
  {
    DEBUG("Now at", *t);

    if (stat(*t, &id) == -1 || !S_ISDIR(id.st_mode))
    {
      cerr << "Error opening " << *t << ": not a directory!" << endl;
      targets.erase(t);

      t--;
    }
  }
}

#ifdef USE_FLAC
bool getflac (struct Song* flac, const char* path)
{
  bool found_artist = false, found_title = false;
  FLAC__Metadata_SimpleIterator *it = FLAC__metadata_simple_iterator_new();

  if (!FLAC__metadata_simple_iterator_init(it, path, true, true))
    return false;

  do
  {
    if (FLAC__metadata_simple_iterator_get_block_type(it) ==
          FLAC__METADATA_TYPE_VORBIS_COMMENT)
    {
      unsigned nc = 0, l = 0;
      
      FLAC__StreamMetadata *sm;
      FLAC__StreamMetadata_VorbisComment_Entry *e;
      
      sm = FLAC__metadata_simple_iterator_get_block(it);

      nc = sm->data.vorbis_comment.num_comments;
      e = sm->data.vorbis_comment.comments;
      
      for (; nc > 0; nc--)
      {
        unsigned i;
        string field;

        for (i = 0; i < e->length; i++)
          field += e->entry[i];
        
        if (field.find("ARTIST=") == 0)
        {
          flac->artist = field.substr(7, field.length() - 7);
          found_artist = true;
        }
        else if (field.find("TITLE=") == 0)
        {
          flac->title = field.substr(6, field.length() - 6);
          found_title = true;
        }
       
        if (found_artist && found_title)
        {
          FLAC__metadata_simple_iterator_delete (it);
          return true;
        }
        
        e++;
      }
    }
  }
  while (FLAC__metadata_simple_iterator_next(it));

  FLAC__metadata_simple_iterator_delete (it);
  return false;
}
#endif /* USE_FLAC */
