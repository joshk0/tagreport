/* Template handling functions
 * XXX: for now, just defines variables needed */

#include "templates.h"
#include <string>
using namespace std;

/* Define variables previously extern'd */
key template_title, template_header, template_prebody;
key template_stats, template_body;

FILE * template_file;

struct map_dollars header_map[] = {
  { "$count", COUNT },
  { "$date", DATE },
  { "$directory", DIRECTORY },
};

struct map_dollars body_map[] = {
  { "$artist", ARTIST },
  { "$title", TITLE },
};

bool replace_header (string & in, int count, const string & directory)
{
  return true;
}

bool replace_body (string & in, const string & artist, const string & title)
{
  return true;
}
