/* Parser for .def files for TagReport. */

%{

#include <string>
#include "y.tab.h"

using namespace std;
 
%}

VALUE		\".*\"
BODY		[.\n]*
WHITE		[\t ]*
COMMENT		^#.*

%%             /* BEGIN RULES SECTION */

{VALUE}	{
	yytext[yyleng - 1] = '\0'; /* Kill the trailing quote */
	yytext++; /* Kill the leading quote */
	yyltype.s = new string(yytext);
	return TYPE_VALUE;
}

{BODY}		{ yyltype.s = new string(yytext); return BODY; }
{WHITE}		{ }
{COMMENT}	{ }

"="		{ return '='; }
"%body%"	{ return BODY_START; }
"title"		{ return TYPE_TITLE; }
"header"	{ return TYPE_HEADER; }
"stats"		{ return TYPE_STATS; }
"prebody"	{ return TYPE_PREBODY; }
"footer"	{ return TYPE_FOOTER; }

%%
