#ifndef included_TAGREPORT_H
#define included_TAGREPORT_H

#include <string>
#include "config.h"

#ifndef NDEBUG
# define DEBUG(x, y) cerr << "DEBUG: " << x << ' ' << y << endl
#else
# define DEBUG(x, y)
#endif

#ifndef ARRAY_SIZE
# define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#endif

/* Useful macro to strip path and extension from a filename (string). */
#define NOPATHEXT(full, path) full.substr(strlen(path) + 1, full.rfind('.') - strlen(path) - 1)

struct Song 
{
  std::string title;
  std::string artist;
};

const char usage[] = \
"TagReport " PACKAGE_VERSION " ( "
#ifdef USE_FLAC
"libflac "
#endif
#ifdef USE_OGG
"libvorbisfile "
#endif
#ifdef USE_TAGLIB
"taglib "
#endif
#ifdef USE_ID3TAG
"id3tag "
#endif
")\n"
"by Joshua Kwan <joshk@triplehelix.org>\n\
Syntax: tagreport [-h] [-v] [-t template] [-o output] directory ...\n"
#ifdef HAVE_GETOPT_LONG
"Options:\n\
  -h, --help\t\t\tDisplay this help message.\n\
  -t, --template [template]\tChoose a template file to use when creating\n\
  \t\t\t\tthe HTML report.\n\
  -f, --force\t\t\tForce parsing of template files to continue if\n\
  \t\t\t\terrors occur.\n\
  -o, --output [file]\t\tStore the result here (default ./playlist.htm)\n\
  -v, --verbose\t\t\tBe verbose. (By default on if debug is enabled)";
#else
"Options:\n\
  -h\t\tDisplay this help message.\n\
  -t template\tChoose a template file to use when creating the HTML report.\n\
  -f\t\tForce parsing of template files to continue if errors occur.\n\
  -o output\tStore the result here (default ./playlist.htm)\n\
  -v\t\tBe verbose. (By default on if debug is enabled)";
#endif

/* Global settings */
extern bool verbose, force;
extern char* template_fn;

#endif
