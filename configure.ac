AC_INIT(TagReport, [0.01a])

AC_CONFIG_HEADER([config.h])

AC_PROG_CXX

AC_LANG(C++)

LEX_LANG=CXX
YACC_LANG=CXX

AC_PROG_LEX
AC_PROG_YACC
AC_PROG_INSTALL

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

if test -z "$TAGLIB_ROOT"; then
	AC_MSG_RESULT([not found])
	AC_MSG_ERROR([Could not find TagLib headers.])
else
	AC_MSG_RESULT($TAGLIB_ROOT)
fi

AC_SUBST(TAGLIB_ROOT)

save_CXXFLAGS="$CXXFLAGS"
save_CPPFLAGS="$CPPFLAGS"

CXXFLAGS="$CXXFLAGS -I$TAGLIB_ROOT/include/taglib"
CPPFLAGS="$CPPFLAGS -I$TAGLIB_ROOT/include/taglib"

AC_CHECK_HEADERS([fileref.h tag.h], , [AC_MSG_ERROR([Missing some TagLib headers, is your install complete?])])

CXXFLAGS="$save_CXXFLAGS"
CPPFLAGS="$save_CPPFLAGS"

AC_CONFIG_FILES(Makefile)

AC_OUTPUT
