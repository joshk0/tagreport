/* Contains the default TagReport template. */

#ifndef INCLUDED_default_h
#define INCLUDED_default_h

#define DEFAULT_TITLE "TagReport Generated Playlist"

#define DEFAULT_BODY_TAG "<body bgcolor=\"#000000\" text=\"#FFFFFF\">"

#define DEFAULT_STATS \
 "<h2>$directory</h2><hr />\n" \
 "<p>Generated at $date <br />\n" \
 "Scanned $count songs.<p>"

#define DEFAULT_PREBODY "<p>"

#define DEFAULT_BODY "$num. $a-t<br />"

#define DEFAULT_FOOTER "</p>"

#endif
