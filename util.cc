/* Utility functions for use with TagReport. */

#include <ctime>
#include <cstring>
#include <string>

#include "html.h"

using namespace std;

/* Allocates stuff on the heap - free it! */
char* get_time_string (void)
{
    struct tm *now;
    time_t now_secs;
    char now_date[18];

    /* Get the current time */
    time(&now_secs);
    now = localtime(&now_secs);
    strftime (now_date, 18, "%H:%M, %F", now);

    return strdup(now_date);
}

/* Escapes illegal HTML characters into their long counterparts. */
void htmlify (string & in)
{
  unsigned int c, i, s;

  /* replace each found character in replacechars by respective 
   * entry in replacehtml */
  for (c = 0; c < in.length(); c++)
  {
    for (i = 0; i < 3; i++)
    {
      if (in[c] == replacechars[i])
        in.replace (c, 1, replacehtml[i]);
    }
  }

  /* replace spaces with two &nbsp;, it's the slightest bit overkill
   * i.e. htmlify("  ") should become " &nbsp;", but it becomes
   * "&nbsp;&nbsp;" */
  
  while ((s = in.find("  ")) != string::npos)
      in.replace(s, 2, "&nbsp;&nbsp;");
}
