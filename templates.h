/* HTML template-related miscellany. */

#ifndef INCLUDED_templates_h
#define INCLUDED_templates_h

#include <string>
#include <sys/types.h>

using namespace std;

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

extern key template_title;
extern key template_header;
extern key template_prebody;
extern key template_stats;
extern key template_body;

extern FILE * template_file;

#endif
