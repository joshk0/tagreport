AC_INIT(TagReport, [0.01a])

AC_CONFIG_HEADER([config.h])

dnl Variable initialization
no_getopt_long=0
use_FLAC=0
use_OGG=0
use_ID3=0
use_TAGLIB=0

AC_PROG_CXXCPP

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

save_CXXFLAGS="$CXXFLAGS"
save_CPPFLAGS="$CPPFLAGS"

if test -z "$TAGLIB_ROOT"; then
  AC_MSG_RESULT([not found])
  CXXFLAGS="$CXXFLAGS -I/usr/local/include -L/usr/local/lib"
  CPPFLAGS="$CPPFLAGS -I/usr/local/include"
else
  AC_MSG_RESULT($TAGLIB_ROOT)

  CXXFLAGS="$CXXFLAGS -I/usr/local/include -I$TAGLIB_ROOT/include/taglib -L/usr/local/lib"
  CPPFLAGS="$CPPFLAGS -I/usr/local/include -I$TAGLIB_ROOT/include/taglib"

  AC_CHECK_HEADER(tag.h,
  [
    AC_CHECK_HEADER(fileref.h,
    [
      AC_DEFINE(USE_TAGLIB, 1, [TagLib support works.])
      LIBS="$LIBS -ltag"
      TAGLIB_INCLUDE="-I\$(TAGLIB_ROOT)/include/taglib"
      TAGLIB_LD="-L\$(TAGLIB_ROOT)/lib"
      use_TAGLIB=1
    ])
  ])
  
  CXXFLAGS="$save_CXXFLAGS -I/usr/local/include -L/usr/local/lib"
  CPPFLAGS="$save_CPPFLAGS -I/usr/local/include"
fi

AC_CHECK_PROGS(DOCBOOK, docbook-to-man, [:])

AC_CHECK_HEADER([FLAC/metadata.h],
[
  AC_CHECK_LIB([FLAC], FLAC__metadata_simple_iterator_new,
  [
    AC_DEFINE(USE_FLAC, 1, [FLAC support works. Don't change!])
    LIBS="$LIBS -lFLAC"
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
      LIBS="$LIBS -lvorbisfile -lvorbis -logg"
      use_OGG=1
    ], , [-lvorbis -logg])
  ]) 
])

AC_CHECK_HEADER(id3tag.h,
[
  AC_CHECK_LIB(id3tag, id3_file_tag,
  [
    dnl Older versions don't have this.
    AC_CHECK_LIB(id3tag, id3_frame_field,
    [
      AC_DEFINE(USE_ID3TAG, 1, [MAD ID3tag support works.])
      LIBS="$LIBS -lid3tag -lz"
      use_ID3=1
    ], , [-lz])
  ], , [-lz])
])

AC_CHECK_FUNC(getopt, [AC_DEFINE(HAVE_DECL_GETOPT, 1, [Needed for GNUgetopt headers to work on systems without a native getopt_long.])])
AC_SEARCH_LIBS(getopt_long, gnugetopt,
[
  if test "$ac_cv_search_getopt_long" = "-lgnugetopt"; then
    LIBS="$LIBS -lgnugetopt"
  fi
], [no_getopt_long=1])

dnl We do this outside the command to allow SEARCH_LIBS to do its $LIBS
dnl thing
if test $no_getopt_long -eq 0; then
  AC_DEFINE(HAVE_GETOPT_LONG, 1, [The getopt_long function is available.])
fi

AC_CHECK_HEADERS([getopt.h libiberty.h])

AC_SUBST(DOCBOOK)

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

if test $use_TAGLIB -eq 0 && test $use_OGG -eq 0 && test $use_ID3 -eq 0 && test $use_FLAC -eq 0; then
  AC_MSG_ERROR([You need to install at least one of the following: taglib, libvorbisfile, id3tag, libflac.])
fi

AC_SUBST(TAGLIB_INCLUDE)
AC_SUBST(TAGLIB_LD)
AC_SUBST(TAGLIB_ROOT)

CXXFLAGS="$save_CXXFLAGS"
CPPFLAGS="$save_CPPFLAGS"

AC_CONFIG_FILES(Makefile)

AC_OUTPUT

echo
echo "Type 'make' to begin the build."
