/* Lexical analyzer for .def files for TagReport. */

%option nounput
%option noyywrap

%{

#include <string>
#include <iostream>

using namespace std;

#include "config.h"
#include "y.tab.h"
#include "templates.h"
#include "tagreport.h"

%}

VALUE		\".*\"
MALSTRING	\"[^\"\n]*\n
WHITE		[\n\t ]*
COMMENT		^#.*

%%             /* BEGIN RULES SECTION */

{VALUE}	{
  yytext[yyleng - 1] = '\0'; /* Kill the trailing quote */
  yytext++; /* Kill the leading quote */
  yylval.s = strdup(yytext);
  return TYPE_VALUE;
}

{MALSTRING} {
  /* yytext already contains a newline, no need for one here */
  cerr << "Error: unterminated string constant in template: " << yytext;

  if (!force)
    exit(1);
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

. {
  cerr << "Error: unrecognized token \"" << yytext[0] << "\" in template";
  if (!force)
  {
    cerr << ", bailing out!" << endl;
    exit(1);
  }
  else
    cerr << ", continuing." << endl;
}

%%
