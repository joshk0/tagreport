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
  string term;
  replace_types i;  
};

extern struct map_dollars header_map[];
extern struct map_dollars body_map[];

extern key template_title;
extern key template_header;
extern key template_prebody;
extern key template_stats;
extern key template_body;

extern FILE * template_file;

bool replace_header (string & in, int count, const string & directory);
bool replace_body (string & in, const string & artist, const string & title);

#endif

