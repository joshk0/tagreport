/* Template handling functions for TagReport. */

#include "templates.h"
#include "util.h"

#include <string>
#include <sstream>

using namespace std;

/* Define variables previously extern'd */
key template_title, template_header, template_prebody, template_head_body;
key template_stats, template_body_tag, template_body, template_footer;

FILE * template_file;

struct map_dollars header_map[] = {
  { "$count", COUNT, 6 },
  { "$date", DATE, 5 },
  { "$directory", DIRECTORY, 10 },
};

struct map_dollars body_map[] = {
  { "$artist", ARTIST, 7 },
  { "$title", TITLE, 6 },
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

string replace_body (const string & artist, const string & title)
{
  string out = template_body.get();
  unsigned int i, j;
  
  while ((i = out.find ("\\n")) != string::npos)
    out.replace (i, 2, "\n");

  for (i = 0; i < 2; i++)
  {
    while ((j = out.find(body_map[i].term)) != string::npos)
    {
      if (body_map[i].i == ARTIST)
        out.replace (j, body_map[i].term_length, artist);
	      
      else if (body_map[i].i == TITLE)
        out.replace (j, body_map[i].term_length, title);

      else
        abort();
    }
  }

  return out;
}
