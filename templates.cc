/* Template handling functions for TagReport. */

#include "templates.h"
#include "util.h"

#include <string>
#include <sstream>

using namespace std;

#include "default.h"

/* Default templates */
key template_title (DEFAULT_TITLE), template_body_tag(DEFAULT_BODY_TAG);
key template_stats (DEFAULT_STATS), template_prebody(DEFAULT_PREBODY);
key template_body (DEFAULT_BODY), template_footer(DEFAULT_FOOTER);
key template_head_body, template_header; /* unset by default */

FILE * template_file = NULL;

struct map_dollars header_map[] = {
  { "$count", COUNT, 6 },
  { "$date", DATE, 5 },
  { "$directory", DIRECTORY, 10 },
};

struct map_dollars body_map[] = {
  { "$artist", ARTIST, 7 },
  { "$title", TITLE, 6 },
  { "$num", NUMBER, 4 },
  { "$a-t", ARTIST_TITLE, 4 }
};

string replace_header (const string & in, int count, const string & directory)
{
  unsigned int i, j;
  string out = in;
  
  /* Replace all \n with actual newlines */
  while ((i = out.find ("\\n")) != string::npos)
    out.replace (i, 2, "\n"); 

  for (i = 0; i < 3; i++)
  {
    while ((j = out.find (header_map[i].term)) != string::npos)
    {
      ostringstream s;
      char* t;
      
      switch (header_map[i].i)
      {
        case COUNT:
          s << count;
          break;

        case DATE:
          t = get_time_string();
          s << t;
          free(t);
          break;

        case DIRECTORY:
          s << directory;
          break;

        default: /* wtf dude */
          abort();
      }
      
      out.replace (j, header_map[i].term_length, s.str());
    }
  }

  return out;
}

string replace_body (const string & artist, const string & title, unsigned int n)
{
  string out = template_body.get();
  unsigned int i, j;
  
  while ((i = out.find ("\\n")) != string::npos)
    out.replace (i, 2, "\n");

  for (i = 0; i < 4; i++)
  {
    while ((j = out.find(body_map[i].term)) != string::npos)
    {
      switch (body_map[i].i)
      {
        case ARTIST:
          out.replace (j, body_map[i].term_length, artist);
          break;
	  
        case TITLE:
          out.replace (j, body_map[i].term_length, title);
	  break;

        case ARTIST_TITLE:
	  if (artist == "") /* Left unset by traverse_dir */
	    out.replace (j, body_map[i].term_length, title);
	  else
	  {
            ostringstream s;
	    s << artist << " - " << title;
	    out.replace (j, body_map[i].term_length, s.str());
	  }
	  break;

	case NUMBER:
	{
          ostringstream s;
	  s << n;
          out.replace (j, body_map[i].term_length, s.str());
	  break;
	}

	default: /* wha? */
	  abort();
      }
    }
  }

  return out;
}
