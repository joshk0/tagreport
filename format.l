/* Parser for .def files for TagReport. */

%option nounput
%option noyywrap

%{

#include <string>
using namespace std;

#include "y.tab.h"
#include "templates.h"

#define YY_INPUT(buf, result, max_size) \
{ \
  char * buff; \
\
  if ((buff = fgets (buf, max_size, template_file)) == NULL) \
  { \
    result = 0; \
    YY_FATAL_ERROR("input in flex scanner failed"); \
  } \
  else \
    result = strlen(buf); \
}

%}

VALUE		\".*\"
BODY		[.\n]*
WHITE		[\t ]*
COMMENT		^#.*

%%             /* BEGIN RULES SECTION */

{VALUE}	{
	yytext[yyleng - 1] = '\0'; /* Kill the trailing quote */
	yytext++; /* Kill the leading quote */
	yylval.s = strdup(yytext);
	return TYPE_VALUE;
}

{BODY}		{ yylval.s = strdup(yytext); return BODY; }
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
