#ifndef included_TAGREPORT_H
#define included_TAGREPORT_H

#include <string>
#include "config.h"

#ifndef NDEBUG
# define DEBUG(x, y) cerr << "DEBUG: " << x << ' ' << y << endl
#else
# define DEBUG(x, y)
#endif

struct Song 
{
  std::string title;
  std::string artist;
};

const char usage[] = \
"TagReport " PACKAGE_VERSION " by Joshua Kwan <joshk@triplehelix.org>\n\
Syntax: tagreport [-h] [-v] [-t template] [-o output] directory\n\
Options:\n\
  -h\t\tDisplay this help message.\n\
  -t template\tChoose a template file to use when creating the HTML report.\n\
  -o output\tStore the result here (default ./playlist.htm)\n\
  -v\t\tBe verbose. (By default on if debug is enabled)";

extern bool verbose;

#endif
