/* HTML canned strings for use in tagreport. */

#ifndef INCLUDED_html_h
#define INCLUDED_html_h

/* Headers and footers */
const char HTMLdtd [] =
"<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n\
<!DOCTYPE html\n\
     PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n\
     \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
	  
/* Character escaping for HTML */
static const char replacechars [] = { '&', '<', '>', '\"' };
static const char* replacehtml [] = { "&amp;", "&lt;", "&gt;", "&quot;" };

#endif
