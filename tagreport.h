#ifndef included_TAGREPORT_H
#define included_TAGREPORT_H

#include <string>

#ifndef NDEBUG
# define DEBUG(x, y) cerr << "DEBUG: " << x << ' ' << y << endl
#else
# define DEBUG(x, y)
#endif

typedef struct Song 
{
  std::string title;
  std::string artist;
};

#endif
