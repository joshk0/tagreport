/* Filetype specific tag-scanning functions for TagReport. */

#include "config.h"
#include "ftfuncs.h"
#include "util.h"

#include <cassert>
#include <string>

using namespace std;

/* TagLib includes */
#include <fileref.h>
#include <tag.h>

#ifdef USE_FLAC

/* libFLAC includes */
#include <FLAC/metadata.h>

/* Read path using libFLAC and retrieve vorbis comments ARTIST
 * and TITLE. Returns true or false depending on success, but we
 * check for string blankness anyway back in traverse_dir. */
bool get_flac (struct Song* flac, const char* path)
{
  bool found_artist = false, found_title = false;
  FLAC__Metadata_SimpleIterator *it = FLAC__metadata_simple_iterator_new();

  if (!FLAC__metadata_simple_iterator_init(it, path, true, true))
    return false;

  do
  {
    if (FLAC__metadata_simple_iterator_get_block_type(it) ==
          FLAC__METADATA_TYPE_VORBIS_COMMENT)
    {
      unsigned nc = 0, l = 0;
      
      FLAC__StreamMetadata *sm;
      FLAC__StreamMetadata_VorbisComment_Entry *e;
      
      sm = FLAC__metadata_simple_iterator_get_block(it);

      nc = sm->data.vorbis_comment.num_comments;
      e = sm->data.vorbis_comment.comments;
      
      for (; nc > 0; nc--)
      {
        unsigned i;
        string field;

        /* XXX - no way to get a clean e->entry? */
        for (i = 0; i < e->length; i++)
          field += e->entry[i];
        
        if (field.find("ARTIST=") == 0)
        {
          flac->artist = field.substr(7, field.length() - 7);
          found_artist = true;
        }
        else if (field.find("TITLE=") == 0)
        {
          flac->title = field.substr(6, field.length() - 6);
          found_title = true;
        }
       
        if (found_artist && found_title)
        {
          FLAC__metadata_simple_iterator_delete (it);
          return true;
        }
        
        e++;
      }

      FLAC__metadata_simple_iterator_delete (it);
      return false; /* If we got here there's no vorbis comment */
    }
  }
  while (FLAC__metadata_simple_iterator_next(it));

  FLAC__metadata_simple_iterator_delete (it);
  return false;
}
#endif /* USE_FLAC */

bool get_taglib (struct Song * song, const char *path)
{
  TagLib::Tag *tag;
  TagLib::FileRef ref (path);

  assert (!ref.isNull());

  if ((tag = ref.tag()) == NULL)
  {
    DEBUG("Null tag; corrupt file encountered?", "");
    return false;
  }
        
  /* Do they both have artist and title fields? */
  if (!tag->artist().isNull() && !tag->title().isNull())
  {
    /* Non-null but empty tag */
    if (tag->artist().isEmpty() || tag->title().isEmpty()) 
      return false;
    else /* Tag is non-empty */
    {
      TagLib::String a, t;
            
      a = tag->artist().stripWhiteSpace();
      t = tag->title().stripWhiteSpace();
            
      if (!a.isEmpty() && !t.isEmpty())
      {
        song->artist = a.to8Bit(false);
        song->title = t.to8Bit(false);

        htmlify(song->artist);
        htmlify(song->title);
              
        return true;
      }
      else
        return false;
    }
  }
  else /* NO TAG! */
    return false;
}
