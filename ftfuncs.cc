/* Filetype specific tag-scanning functions for TagReport. */

#include "config.h"
#include "ftfuncs.h"
#include "util.h"

#include <cassert>
#include <string>

using namespace std;

/* TagLib includes */
#ifdef USE_TAGLIB
#include <fileref.h>
#include <tag.h>
#endif

#ifdef USE_OGG
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#endif

#ifdef USE_ID3TAG
#include <id3tag.h>
#endif

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
      unsigned nc;
      
      FLAC__StreamMetadata *sm;
      FLAC__StreamMetadata_VorbisComment_Entry *e;
      
      sm = FLAC__metadata_simple_iterator_get_block(it);

      nc = sm->data.vorbis_comment.num_comments;
      e = sm->data.vorbis_comment.comments;
      
      for (; nc > 0; nc--)
      {
        unsigned i;
        int eq;
        string field, fname;

        /* XXX - no way to get a clean e->entry? */
        for (i = 0; i < e->length; i++)
          field += e->entry[i];

        eq = field.find('=');
        assert (eq != string::npos);
        fname = field.substr(0, eq);
        
        if (!strcasecmp(fname.c_str(), "ARTIST"))
        {
          flac->artist = field.substr(7, field.length() - 7);
          htmlify(flac->artist);
          found_artist = true;
        }
        else if (!strcasecmp(fname.c_str(), "TITLE"))
        {
          flac->title = field.substr(6, field.length() - 6);
          htmlify(flac->title);
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

#ifdef USE_TAGLIB

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

#endif /* USE_TAGLIB */

#ifdef USE_OGG
/* This is much like the FLAC support because both use the Vorbis comment
 * format. */
bool get_ogg (struct Song * ogg, const char * path)
{
  OggVorbis_File ov;
  FILE* of;
  int i;
  bool found_title = false, found_artist = false;

  if ((of = fopen(path, "r")) == NULL)
  {
    perror("fopen");
    return false;
  }

  if (ov_open (of, &ov, NULL, -1) < 0)
  {
    fclose(of);
    return false;
  }

  /* ogg->seconds = (long)ov_time_total(&ov, -1) */;
  
  for (i = 0; i < ov.vc->comments; i++)
  {
    string field = ov.vc->user_comments[i], fname;
    int e = field.find('=');
    assert (e != string::npos);
    fname = field.substr(0, e);
    
    if (!strcasecmp(fname.c_str(), "ARTIST"))
    {
      ogg->artist = field.substr(7, field.length() - 7);
      htmlify(ogg->artist);
      found_artist = true;
    }
    else if (!strcasecmp(fname.c_str(), "TITLE"))
    {
      ogg->title = field.substr(6, field.length() - 6);
      htmlify(ogg->title);
      found_title = true;
    }
       
    if (found_artist && found_title)
    {
      ov_clear (&ov);
      return true;
    }
  }

  ov_clear (&ov);
  return true;
}
#endif /* USE_OGG */

#ifdef USE_ID3TAG
bool get_mp3 (struct Song *mp3, const char* path)
{
  struct id3_file *m = NULL;
  struct id3_tag *tag = NULL;
  union id3_field *field = NULL;
  struct id3_frame *frame = NULL;
  const id3_ucs4_t *str = NULL;

  /* The program will think up the right title :) */
  if (!(m = id3_file_open (path, ID3_FILE_MODE_READONLY)))
    return true;
  
  if (!(tag = id3_file_tag (m)))
    return true;
  
  if ((frame = id3_tag_findframe (tag, ID3_FRAME_TITLE, 0)))
  {
    if ((field = id3_frame_field (frame, 1)))
    {
      if ((str = id3_field_getstrings (field, 0)))
      {
        mp3->title = (const char*) id3_ucs4_latin1duplicate(str);
        htmlify(mp3->title);
      }
    }
  }

  if ((frame = id3_tag_findframe (tag, ID3_FRAME_ARTIST, 0)))
  {
    if ((field = id3_frame_field (frame, 1)))
    {
      if ((str = id3_field_getstrings (field, 0)))
      {
        mp3->artist = (const char*) id3_ucs4_latin1duplicate(str);
        htmlify(mp3->artist);
      }
    }
  }
  
  id3_file_close (m);
  return true;
}
#endif /* USE_ID3TAG */

bool get_artist_title (struct Song * song, string fn, char * begin)
{
  string ext = fn;
  
  /* Skip filenames with no extension */
  if (fn.rfind('.') == string::npos)
  {
    DEBUG("skipping extensionless file", fn);
    return false;  
  }

  /* Extension is there, but nothing */
  else if (fn[fn.length() - 1] == '.')
  {
    DEBUG("blank extension for file", fn);
    return false;
  }
  
  /* Valid extension */
  string::iterator e;
  
  /* Grab the first four letters of the extension (if possible)
   * and store it in ext as lowercase. */
  ext = ext.substr(ext.rfind('.') + 1, 4);
        
  for (e = ext.begin(); e != ext.end(); e++)
    *e = tolower(*e);

  /* Choose which function to use. */
  bool (*getfunc)(struct Song*, const char*);

#ifdef USE_FLAC
  if (ext == "flac")
  {
    getfunc = &get_flac;
    goto skipextcheck;
  }
#endif
#ifdef USE_OGG
  if (ext == "ogg")
  {
    getfunc = &get_ogg;
    goto skipextcheck;
  }
#endif
#ifdef USE_ID3TAG
  if (ext == "mp3")
  {
    getfunc = &get_mp3;
    goto skipextcheck;
  }
#endif
#if !defined(USE_ID3TAG) && defined(USE_TAGLIB)
  if (ext == "mp3")
  {
    getfunc = &get_taglib;
    goto skipextcheck;
  }
#endif
#if !defined(USE_OGG) && defined(USE_TAGLIB)
  if (ext == "ogg")
  {
    getfunc = &get_taglib;
    goto skipextcheck;
  }
#endif
  else
  {
     DEBUG ("skipping unrecognized file", fn);
     return false;
  }

skipextcheck:
  if (!(*getfunc)(song, fn.c_str()) || 
      (song->title == "" && song->artist == ""))
  {
    song->title = NOPATHEXT(fn, begin);
    htmlify(song->title);
  }
    
  return true;
}
