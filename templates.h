/* HTML template-related miscellany. */

#ifndef INCLUDED_templates_h
#define INCLUDED_templates_h

#include <string>
#include <sys/types.h>

using namespace std;

typedef enum { ARTIST, TITLE, COUNT, DIRECTORY, DATE } replace_types;

class key
{
  public:
    key () : isSet(false) {};
    inline bool is_set (void) const { return isSet; };
    inline void set (string in) { keyVal = in; isSet = true; };
    inline string get (void) const { return keyVal; };

  private:
    bool isSet;
    string keyVal;
};

struct map_dollars
{
  char* term;
  replace_types i;
  int term_length;
};

extern struct map_dollars header_map[];
extern struct map_dollars body_map[];

extern key template_title;
extern key template_header;
extern key template_head_body;
extern key template_prebody;
extern key template_stats;
extern key template_body;
extern key template_body_tag;
extern key template_footer;

extern FILE * template_file;

string replace_header (const string & in, int count, const string & directory);
string replace_body (const string & artist, const string & title);

#endif

