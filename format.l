/* Parser for .def files for TagReport. */

%{

#include <string>
#include "y.tab.h"

using namespace std;
 
%}

KEY		[a-z]+
VALUE		\".*\"
BODYSTART	%body%
BODY		[.\n]*
WHITE		[\t ]*
COMMENT		^#.*

%%             /* BEGIN RULES SECTION */

KEY	{ yyltype.s = new string(yytext); return TYPE_KEY; }
VALUE	{ yytext[yyleng - 1] = '\0'; yytext++; yyltype.s = new string(yytext); return TYPE_VALUE; }
BODY	{ yyltype.s = new string(yytext); return BODY_START; }
=	{ return '='; }
WHITE	{ }
COMMENT { }

%%

 /* User functions */
