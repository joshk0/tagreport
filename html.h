/* HTML canned strings for use in tagreport. */

#ifndef INCLUDED_html_h
#define INCLUDED_html_h

#include <string>

/* Headers and footers */
static std::string HTMLdtd =
"<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n\
<!DOCTYPE html\n\
     PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n\
     \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
	  
static std::string HTMLheader =
"<html>\n\
<head>\n\
<title>TagReport Generated Playlist</title>\n\
</head>\n\
<body bgcolor=\"#000000\" text=\"#FFFFFF\">\n\
<h2>Playlist of ";

static std::string HTMLfooter =
"</p><hr /></body>\n\
</html>";

/* Character escaping for HTML */
static char replacechars [] = { '&', '<', '>' };
static char* replacehtml [] = { "&amp;", "&lt;", "&gt;" };

/* Easter egg quote. */
static std::string noMusicQuote =
"<i>Without music, life would be an error. The German imagines even God singing songs.</i><br />\n\
-- Friedrich Nietzsche<br />\n";

#endif
