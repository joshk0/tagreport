/* HTML-related functions for TagReport. */

#include "config.h"
#include "tagreport.h"
#include "templates.h"
#include "html.h"

#include <iostream>
#include <cerrno>
#include <cassert>
#include <cstring>
#include <fstream>
#include <string>

using namespace std;

void write_html (const char* file, vector<struct Song*>* songs, char* target_dir)
{
  ofstream out;
  unsigned int i;
  
  out.open (file, ios::out | ios::trunc);

  if (!out.is_open())
  {
    cerr << "Failed to open file " << file << ": " << strerror(errno) << endl;
    if (!force)
      exit(1);
  }
    
  /* Write out canned headers to the target file */
  out << HTMLdtd << "<html>\n<head>\n<title>" << endl;

  OUTPUT_HEADER(out, songs, target_dir, template_title);
  out << "</title>" << endl;

  /* For CSS and JavaScript and such... */
  OUTPUT_HEADER_IF_SET(out, songs, target_dir, template_head_body);
  out << "</head>" << endl;

  OUTPUT_HEADER(out, songs, target_dir, template_body_tag);
  OUTPUT_HEADER_IF_SET(out, songs, target_dir, template_header);
    
  /* Some statistics ... */
  OUTPUT_HEADER(out, songs, target_dir, template_stats);
    
  /* Use literally. */
  OUTPUT_HEADER_IF_SET(out, songs, target_dir, template_prebody);
    
  /* Loop through the vector and HTML-output its contents. */
  assert (template_body.is_set());
  for (i = 0; i < songs->size(); i++)
    OUTPUT_BODY(out, (*songs)[i], i + 1);

  /* Footer output. Treated as a header for processing purposes. */
  OUTPUT_HEADER(out, songs, target_dir, template_footer);
  out << "</body>\n</html>" << endl;

  /* Flush the file and close it */
  out.close();
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
