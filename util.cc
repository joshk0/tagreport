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

#ifdef HAVE_METAFLAC
/* Fork metaflac for a file with path path and get its artist and title
 * comment fields. */
bool metaflac (struct Song* flac, const char* path)
{
  FILE* mf;
  ostringstream cmd;
  char buf[1024];
  unsigned int i;
  string fn = path;
  
  /* Ewwwww! Sick! */
  cmd << METAFLAC << " --show-vc-field=artist --show-vc-field=title \"" << path  << "\"";
  mf = popen(cmd.str().c_str(), "r");

  while (fgets(buf, 1024, mf) != NULL)
  {
    string h;

    h = buf;

    if (h.find("ARTIST=") == 0) /* should be first */
      flac->artist = h.substr(7, h.length() - 8);
    else if (h.find("TITLE=") == 0)
      flac->title = h.substr(6, h.length() - 7);
    else
    {
      DEBUG("read failed, buf is", h);
      return false;
    }
  }

  pclose(mf);
  
  return true;
}
#endif
