AC_INIT(TagReport, [0.01a])

AC_CONFIG_HEADER([config.h])

dnl Variable initialization
no_getopt_long=0
use_FLAC=0
use_OGG=0
use_ID3=0
use_TAGLIB=0

AC_PROG_CXX

AC_LANG(C++)

if test "$ac_cv_cxx_compiler_gnu" = yes; then
  CXXFLAGS="$CXXFLAGS -Wall -W $INCLUDES"
fi

LEX_LANG=CXX
YACC_LANG=CXX

AC_PROG_LEX
AC_PROG_YACC
AC_PROG_INSTALL

AC_ARG_ENABLE(debug,
AC_HELP_STRING([--enable-debug], [Enable debugging messages in TagReport.]),
[debug="$enableval"], [debug=no])

if test "$debug" = yes; then
  DEBUG=""
else
  DEBUG="-DNDEBUG"
fi

AC_SUBST(DEBUG)

AC_ARG_WITH(taglib-prefix,
AC_HELP_STRING([--with-taglib-prefix=PATH], [Add another path to the search path for TagLib's headers.]),
[userpath="$withval"], [])

AC_MSG_CHECKING([for location of TagLib includes])
for dir in $userpath /usr /usr/local /opt; do
  if test -d "$dir/include/taglib"; then
    TAGLIB_ROOT="$dir"
    break
  fi
done

datadir="$datadir/tagreport"

if test -z "$TAGLIB_ROOT"; then
  AC_MSG_RESULT([not found])
  AC_MSG_ERROR([Could not find TagLib headers. Try using --with-taglib-prefix?])
else
  AC_MSG_RESULT($TAGLIB_ROOT)
fi

AC_SUBST(TAGLIB_ROOT)

save_CXXFLAGS="$CXXFLAGS"
save_CPPFLAGS="$CPPFLAGS"

CXXFLAGS="$CXXFLAGS -I$TAGLIB_ROOT/include/taglib -I/usr/local/include -L/usr/local/lib"
CPPFLAGS="$CPPFLAGS -I$TAGLIB_ROOT/include/taglib -I/usr/local/include -L/usr/local/lib"

AC_CHECK_PROGS(DOCBOOK, docbook-to-man, [:])

AC_CHECK_HEADER([FLAC/metadata.h],
[
  AC_CHECK_LIB([FLAC], FLAC__metadata_simple_iterator_new,
  [
    AC_DEFINE(USE_FLAC, 1, [FLAC support works. Don't change!])
    AC_SUBST(FLACLIB, -lFLAC)
    use_FLAC=1
  ])
])

AC_CHECK_HEADER(vorbis/codec.h,
[
  AC_CHECK_HEADER(vorbis/vorbisfile.h,
  [
    AC_CHECK_LIB(vorbisfile, ov_open,
    [
      AC_DEFINE(USE_OGG, 1, [Ogg Vorbis support works.])
      dnl Really, it's the user's problem if they have libvorbisfile
      dnl and not libvorbis and libogg too.
      AC_SUBST(OGGLIB, ["-lvorbisfile -lvorbis -logg"])
      use_OGG=1
    ], , [-lvorbis -logg])
  ]) 
])

AC_CHECK_HEADER(id3tag.h,
[
  AC_CHECK_LIB(id3tag, id3_file_tag,
  [
    AC_DEFINE(USE_ID3TAG, 1, [MAD ID3tag support works.])
    AC_SUBST(ID3TAGLIB, ["-lid3tag -lz"])
    use_ID3=1
  ])
])

AC_SEARCH_LIBS(getopt_long, gnugetopt, , [no_getopt_long=1])

dnl We do this outside the command to allow SEARCH_LIBS to do its $LIBS
dnl thing
if test $no_getopt_long -eq 0; then
  AC_DEFINE(HAVE_GETOPT_LONG, 1, [The getopt_long function is available.])
fi

AC_CHECK_HEADERS([getopt.h libiberty.h])

AC_SUBST(DOCBOOK)

AC_CHECK_HEADERS([fileref.h tag.h],
[
  AC_DEFINE(USE_TAGLIB, 1, [TagLib support works.])
  use_TAGLIB=1
])


if test $use_OGG -eq 1; then
  yes_OGG="yes"
else
  yes_OGG="no"
fi

if test $use_ID3 -eq 1; then
  yes_ID3="yes"
else
  yes_ID3="no"
fi

if test $use_TAGLIB -eq 1; then
  yes_TAGLIB="yes"
else
  yes_TAGLIB="no"
fi

if test $use_FLAC -eq 1; then
  yes_FLAC="yes"
else
  yes_FLAC="no"
fi

echo
echo "TagReport $PACKAGE_VERSION"
echo
echo "libflac support .............. $yes_FLAC"
echo "TagLib support ............... $yes_TAGLIB"
echo "libvorbisfile support ........ $yes_OGG"
echo "libid3tag support ............ $yes_ID3"
echo

if test $use_TAGLIB -eq 0 && test $use_OGG -eq 0 ] && test $use_ID3 -eq 0 && test $use_FLAC -eq 0; then
  AC_MSG_ERROR([You need to install at least one of the following: taglib, libvorbisfile, id3tag, libflac.])
fi

CXXFLAGS="$save_CXXFLAGS"
CPPFLAGS="$save_CPPFLAGS"

AC_CONFIG_FILES(Makefile)

AC_OUTPUT

echo
echo "Type 'make' to begin the build."
