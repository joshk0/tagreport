/* HTML canned strings for use in tagreport. */

#ifndef INCLUDED_html_h
#define INCLUDED_html_h

#include <string>

/* Headers and footers */
const std::string HTMLdtd =
"<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n\
<!DOCTYPE html\n\
     PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n\
     \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
	  
/* Character escaping for HTML */
static char replacechars [] = { '&', '<', '>', '\"' };
static char* replacehtml [] = { "&amp;", "&lt;", "&gt;", "&quot;" };

#endif
