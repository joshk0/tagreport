/* Parser for .def files for TagReport. */

%option nounput
%option noyywrap

%{

#include <string>
#include <iostream>

using namespace std;

#include "config.h"
#include "y.tab.h"
#include "templates.h"

/* Instruct lex to look in template_file for input. */
#define YY_INPUT(buf, result, max_size) \
{ \
  char * buff; \
\
  if ((buff = fgets (buf, max_size, template_file)) == NULL) \
  { \
    result = 0; \
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
"headbody"	{ return TYPE_HEAD_BODY; }
"header"	{ return TYPE_HEADER; }
"stats"		{ return TYPE_STATS; }
"prebody"	{ return TYPE_PREBODY; }
"footer"	{ return TYPE_FOOTER; }
"body"		{ return TYPE_BODY; }
"bodytag"	{ return TYPE_BODY_TAG; }

%%
