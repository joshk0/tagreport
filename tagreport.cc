#ifndef _GNU_SOURCE
# define _GNU_SOURCE // asprintf, basename...
#endif

#include <taglib/vorbisfile.h>
#include <taglib/mpegfile.h>
#include <taglib/tag.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>

#include <errno.h>
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
  char *comp, *fullpath = NULL;
  
  if ((root = opendir(begin)) != NULL) {
    /* DURRRRRRRRRRRRRRRRRRR  not done yet */
    while ((contents = readdir(root)) != NULL)
    {

      asprintf(&fullpath, "%s/%s", begin, contents->d_name);
      
      if (contents->d_type == DT_DIR) {
        traverse_dir(contents->d_name);
      }

      else {
        /* Skip filenames with no extension */
        if ((comp = strrchr(contents->d_name, '.')) == NULL)
          continue;
	
        if (!strcasecmp(strrchr(contents->d_name, '.')+1, "ogg")) {
          TagLib::VorbisFile current_ogg (TagLib::String(fullpath));
        }
        else if (!strcasecmp(strrchr(contents->d_name, '.')+1, "mp3")) {
          TagLib::MPEGFile current_mp3 (TagLib::String(fullpath));
	}
#ifdef DEBUG
        else {
          fprintf(stderr, "DEBUG: Skipping file %s\n", contents->d_name);
        }
#endif
      }

      free (fullpath);
    }
  }
  else {
    printf ("%s: Error reading directory: %s\n", invoked_as, strerror(errno));
    return;
  }

  closedir(root);
}
