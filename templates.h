/* HTML template-related miscellany. */

#ifndef INCLUDED_templates_h
#define INCLUDED_templates_h

#include <string>
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

key template_title;
key template_header;
key template_prebody;
key template_stats;
key template_body;

#endif
