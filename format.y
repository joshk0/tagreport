%{

#include <string>
#include <iostream>
#include <vector>

#include "templates.h"

#define WARN_SHADOW(e) cerr << "Warning: additional definition of '" << e << "' shadows previous" << endl

using namespace std;

extern int yylex(void);
extern void yyerror(const char*);

%}

%union {
	char* s;
	int kt;
}

%token   BODY_START TYPE_TITLE TYPE_HEADER TYPE_STATS TYPE_PREBODY TYPE_FOOTER
%token   <s> BODY TYPE_VALUE

%type	<kt> KeyType

%%

Template: KeyPairs BODY_START BODY

KeyPairs: KeyPairs KeyPair
	| KeyPair
	;

KeyPair: KeyType '=' TYPE_VALUE {
  switch ($1)
  {
    case TYPE_TITLE:
      if (template_title.is_set())
        WARN_SHADOW("title");
	
      template_title.set ($3);
      
      break;
      
    case TYPE_HEADER:
      if (template_header.is_set())
        WARN_SHADOW("header");
      
      template_header.set ($3);
      break;
      
    case TYPE_STATS:
      if (template_stats.is_set())
        WARN_SHADOW("stats");

      template_stats.set($3);
      break;
      
    case TYPE_PREBODY:
      if (template_prebody.is_set())
        WARN_SHADOW("prebody");

      template_prebody.set ($3);
      break;

    default: /* should NEVER ever ever happen */
      abort();
  }
}

KeyType : TYPE_TITLE	{ $$ = TYPE_TITLE; }
	| TYPE_HEADER	{ $$ = TYPE_HEADER; }
	| TYPE_STATS	{ $$ = TYPE_STATS; }
	| TYPE_PREBODY	{ $$ = TYPE_PREBODY; }
	| error		{ cerr << "Unrecognized key type at line " << @1.first_line << ", continuing" << endl; }
	;

%%

void yyerror(const char* s)
{
  cerr << "parse error: " << s << endl;
}
