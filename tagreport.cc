#include <taglib/vorbisfile.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

using namespace std;

static void traverse_dir (char* begin);
static char* invoked_as;

int main (int argc, char* argv [])
{
  invoked_as = argv[0];
  
  if (argc < 2) {
    printf ("%s: not enough arguments (syntax: %s [directory])\n", argv[0], basename(argv[0]));
    exit(1);
  }
  else {
    traverse_dir(argv[1]);
  }
  
  return 0;
}

/* traverse_dir()
 * input: path to directory to traverse
 * output: playlist.htm (XXX its currently stdout)
 */

void traverse_dir (char* begin)
{
  /* XXX NOT DONE XXX */
  DIR* root;
  struct dirent * contents;
  
  if ((root = opendir(begin)) != NULL) {
    while ((contents = readdir(begin)) != NULL) {
      /* DURRRRRRRRRRRRRRRRRRR */
    }
  }
  else {
    printf ("%s: Error reading directory: %s\n", invoked_as, strerror(errno));
    return;
  }

  closedir(root);
}
