%{

#include <string>
#include <iostream>
#include <vector>

#include "templates.h"

#define WARN_SHADOW(e) cerr << "Warning: additional definition of '" << e << "' shadows previous" << endl

using namespace std;

extern int yylex(void);
extern void yyerror(const char*);
extern char *yytext;

%}

%union {
	char* s;
	int kt;
}

%token   TYPE_TITLE TYPE_HEADER TYPE_STATS TYPE_PREBODY TYPE_FOOTER TYPE_BODY
%token   <s> TYPE_VALUE

%type	<kt> KeyType

%%

Template: KeyPairs

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

      template_stats.set ($3);
      break;
      
    case TYPE_PREBODY:
      if (template_prebody.is_set())
        WARN_SHADOW("prebody");

      template_prebody.set ($3);
      break;

    case TYPE_BODY:
      if (template_body.is_set())
        WARN_SHADOW("body");

      template_body.set ($3);
      break;

    default: /* should NEVER ever ever happen */
      cout << "OWNED: received " << $1 << " in switch" << endl; 
      abort();
  }
}

KeyType : TYPE_TITLE	{ $$ = TYPE_TITLE; }
	| TYPE_HEADER	{ $$ = TYPE_HEADER; }
	| TYPE_STATS	{ $$ = TYPE_STATS; }
	| TYPE_PREBODY	{ $$ = TYPE_PREBODY; }
	| TYPE_FOOTER   { $$ = TYPE_FOOTER; }
	| TYPE_BODY	{ $$ = TYPE_BODY; }
	;

%%

void yyerror(const char* s)
{
  cerr << "couldn't parse \"" << yytext << "\": " << s << endl;
}
