AC_INIT(TagReport, [0.01a])

AC_CONFIG_HEADER([config.h])

AC_PROG_CXX

AC_LANG(C++)

if test "$ac_cv_cxx_compiler_gnu" = yes; then
	CXXFLAGS="$CXXFLAGS -Wall -W -Wno-unused"
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

CXXFLAGS="$CXXFLAGS -I$TAGLIB_ROOT/include/taglib"
CPPFLAGS="$CPPFLAGS -I$TAGLIB_ROOT/include/taglib"

AC_CHECK_PROGS(DOCBOOK, docbook-to-man, [:])
AC_CHECK_FUNCS(getopt_long)
AC_CHECK_HEADERS(getopt.h)

AC_SUBST(DOCBOOK)

AC_CHECK_HEADERS([fileref.h tag.h], , [AC_MSG_ERROR([Missing some TagLib headers, is your install complete?])])

CXXFLAGS="$save_CXXFLAGS"
CPPFLAGS="$save_CPPFLAGS"

AC_CONFIG_FILES(Makefile)

AC_OUTPUT
