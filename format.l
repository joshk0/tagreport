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

unsigned int line = 1, col = 0;
unsigned int comment_begin_line, comment_begin_col;

static void ccomment(void);

#define YY_USER_ACTION col += yyleng;

%}

VALUE		\".*\"
MALSTRING	\"[^\"\n]*\n
WHITE		[\t ]*
COMMENT		^#.*
LONGCOMMENT	"/*"

%%             /* BEGIN RULES SECTION */

{VALUE}	{
  yytext[yyleng - 1] = '\0'; /* Kill the trailing quote */
  yytext++; /* Kill the leading quote */
  yylval.s = strdup(yytext);
  return TYPE_VALUE;
}

{MALSTRING} {
  /* yytext already contains a newline, no need for one here */
  cerr << "Error: unterminated string constant in template at line " << line << " (started at column " << col - yyleng + 1 << "): " << yytext;

  if (!force)
    exit(1);
}

{WHITE}		{ }
{COMMENT}	{ }
{LONGCOMMENT}	{
  comment_begin_line = line;
  comment_begin_col = col - 1;
  ccomment();
}

"="		{ return '='; }
"title"		{ return TYPE_TITLE; }
"headbody"	{ return TYPE_HEAD_BODY; }
"header"	{ return TYPE_HEADER; }
"stats"		{ return TYPE_STATS; }
"prebody"	{ return TYPE_PREBODY; }
"footer"	{ return TYPE_FOOTER; }
"body"		{ return TYPE_BODY; }
"bodytag"	{ return TYPE_BODY_TAG; }

\n		{ line++; col = 0; }

. {
  cerr << "Error: unrecognized token \"" << yytext[0] << "\" in template at line " << line << " column " << col;

 if (!force)
    exit(1);
}

%%

/* Ripped from ircd-hybrid/src/ircd_lexer.l */
void ccomment(void)
{
  int c;

  while (1)
  {
    while ((c = yyinput()) != '*' && c != EOF)
    {
      if (c == '\n')
      {
        col = 0;
	++line;
      }
      else
        ++col;
    }
      
    if (c == '*')
    {
      ++col;
      while ((c = yyinput()) == '*') ++col;
      if (c == '/') { ++col; break; }
    }
    if (c == EOF)
    {
      cerr << "Error: encountered end-of-file in comment starting on line " << comment_begin_line << ", column " << comment_begin_col << endl;

      if (force)
        return;
      else
        exit(1);

      break;
    }
  }
}
