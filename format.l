/* Parser for .def files for TagReport. */

%option nounput
%option noyywrap

%{

#include <string>
#include <iostream>

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
WHITE		[\n\t ]*
COMMENT		^#.*

%%             /* BEGIN RULES SECTION */

{VALUE}	{
	yytext[yyleng - 1] = '\0'; /* Kill the trailing quote */
	yytext++; /* Kill the leading quote */
	yylval.s = strdup(yytext);
	return TYPE_VALUE;
}

{WHITE}		{ }
{COMMENT}	{ }

"="		{ return '='; }
"title"		{ return TYPE_TITLE; }
"header"	{ return TYPE_HEADER; }
"stats"		{ return TYPE_STATS; }
"prebody"	{ return TYPE_PREBODY; }
"footer"	{ return TYPE_FOOTER; }
"body"		{ return TYPE_BODY; }
%%
