#ifndef _GNU_SOURCE
# define _GNU_SOURCE // asprintf, basename...
#endif

#define DEBUG

#include <taglib/vorbisfile.h>
#include <taglib/mpegfile.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/id3v2tag.h>

#include <cassert>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

using namespace std;

static void traverse_dir (char* begin);
static char* invoked_as;

int main (int argc, char* argv [])
{
  struct stat id;
  invoked_as = argv[0];
  
  if (argc < 2)
  {
    printf ("%s: not enough arguments (syntax: %s [directory])\n", argv[0], basename(argv[0]));
    exit(1);
  }
  else
  {
    stat (argv[1], &id);
    if (id.st_mode & S_IFDIR)
    {
      traverse_dir(argv[1]);
    }
    else
    {
      fprintf (stderr, "%s: %s: not a directory!\n", argv[0], argv[1]);
      return 1;
    }
  }
  
  return 0;
}

/* traverse_dir()
 * input: path to directory to traverse
 * output: playlist.htm (XXX its currently stdout)
 */

void traverse_dir (char* begin)
{
  DIR* root;
  struct dirent * contents;
  struct stat dino;
  char *comp, *fullpath = NULL;
  TagLib::Tag *tag;
  
  if ((root = opendir(begin)) != NULL)
  {
    while ((contents = readdir(root)) != NULL)
    {
      asprintf(&fullpath, "%s/%s", begin, contents->d_name);

      /* Skip .. and . */
      if (!strcmp(contents->d_name, ".") || !strcmp(contents->d_name, ".."))
      {
        free(fullpath);
        continue;
      }

      /* Recurse if this entry is actually a directory */
      stat (fullpath, &dino);
      
      if (dino.st_mode & S_IFDIR)
      {
#ifdef DEBUG
        fprintf(stderr, "DEBUG: Recursing into %s\n", fullpath);
#endif
        traverse_dir(fullpath);
      }

      /* Otherwise, it's a normal file */
      else
      {
        /* Skip filenames with no extension */
        if ((comp = strrchr(contents->d_name, '.')) == NULL)
        {
#ifdef DEBUG
          fprintf(stderr, "DEBUG: skipping extensionless file %s\n", contents->d_name);
#endif
          free(fullpath);
        }
        
	else if (contents->d_name[strlen(contents->d_name) - 1] == '.')
	{
#ifdef DEBUG
          fprintf(stderr, "DEBUG: blank extension for file %s\n", contents->d_name);
#endif
	  free(fullpath);
	}

        /* Ogg Vorbis or MP3 file? */
	else if (!strcasecmp(strrchr(contents->d_name, '.') + 1, "ogg")
            || !strcasecmp(strrchr(contents->d_name, '.') + 1, "mp3"))
	{
          TagLib::FileRef ref (fullpath);

          if(ref.isNull())
            abort();

          tag = ref.tag();
        
	  if (!tag->artist().isNull() && !tag->title().isNull())
	  {
	    if (tag->artist().isEmpty() || tag->title().isEmpty())
	    {
	      char *tmp = strrchr(contents->d_name, '.');

	      if (tmp)
	      {
		*tmp = '\0';
		puts(contents->d_name);
	      }
	      else
		abort();
	    }
	    else
	    {
	      printf("%s - %s\n", tag->artist().toCString(), tag->title().toCString());
	    }
	  }
	  else
	  {
            /* strip the extension from a filename without ID */
	    
            char* tmp = strrchr(contents->d_name, '.');

	    if (tmp)
	    {
	      *tmp  = '\0';
	      puts(contents->d_name);
	    }
	    else
	      abort(); /* SHOULD NOT HAPPEN */
	    
	  }
	}
        
#ifdef DEBUG
        /* Note that we did not do anything with this file */
        else
	{
          fprintf(stderr, "DEBUG: skipping unrecognized file %s\n", contents->d_name, contents->d_type);
        }
#endif
      }

      free (fullpath);
    }
  }
  /* If we get here there was SOME sort of error with opendir() */
  else {
    fprintf (stderr, "%s: Error reading directory: %s\n", invoked_as, strerror(errno));
    return;
  }

  closedir(root);
}
