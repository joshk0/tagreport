/* HTML template-related miscellany. */

#ifndef INCLUDED_templates_h
#define INCLUDED_templates_h

#include <string>
#include <stdio.h>

using namespace std;

typedef enum { ARTIST, TITLE, COUNT, DIRECTORY, DATE, NUMBER, ARTIST_TITLE } replace_types;

class key
{
  public:
    key () : isSet(false), isDefault(false) {};
    key (char* init) : isSet(true), isDefault(true), keyVal(init) {};
    inline bool is_set (void) const { return isSet; };
    inline bool is_default (void) const { return isDefault; };
    inline void set (const string & in) { keyVal = in; isSet = true; isDefault = false; };
    inline string get (void) const { return keyVal; };

  private:
    bool isSet, isDefault;
    string keyVal;
};

struct map_dollars
{
  char* term;
  replace_types i;
  int term_length;
};

extern const struct map_dollars header_map[];
extern const struct map_dollars body_map[];

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
string replace_body (const string & artist, const string & title, unsigned int n);

/* Some very useful macros. */
#define OUTPUT_HEADER_IF_SET(stream, vec, dir, tmpl) \
if ((tmpl).is_set()) \
  OUTPUT_HEADER(stream,vec,dir,tmpl)

#define OUTPUT_HEADER(stream, vec, dir, tmpl) \
  (stream) << replace_header((tmpl).get(), (vec)->size(), dir) << endl;

#define OUTPUT_BODY(stream, item, num) \
  (stream) << replace_body((item)->artist, (item)->title, num) << endl;

#endif

