%{

#include <string>
#include <iostream>
#include <vector>

#include "templates.h"

#define WARN_SHADOW_AND_SET(e, f, g) \
if (!(e).is_default() && (e).is_set()) \
  cerr << "Warning: additional definition of '" << f \
       << "' shadows previous" << endl; \
(e).set (g)

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
%token   TYPE_BODY_TAG TYPE_HEAD_BODY
%token   <s> TYPE_VALUE

%type	<kt> KeyType

%%

Template: KeyPairs
	| { cerr << "Error: template file has no settings, continuing with defaults." << endl << "This is probably not what you want!" << endl; }
	;

KeyPairs: KeyPairs KeyPair
	| KeyPair
	;

KeyPair: KeyType '=' TYPE_VALUE {
  switch ($1)
  {
    case TYPE_TITLE:
      WARN_SHADOW_AND_SET(template_title, "title", $3);
      break;
      
    case TYPE_HEADER:
      WARN_SHADOW_AND_SET(template_header, "header", $3);
      break;
      
    case TYPE_STATS:
      WARN_SHADOW_AND_SET(template_stats, "stats", $3);
      break;
      
    case TYPE_PREBODY:
      WARN_SHADOW_AND_SET(template_prebody, "prebody", $3);
      break;

    case TYPE_BODY:
      WARN_SHADOW_AND_SET(template_body, "body", $3);
      break;

    case TYPE_BODY_TAG:
      WARN_SHADOW_AND_SET(template_body_tag, "bodytag", $3);
      break;

    case TYPE_HEAD_BODY:
      WARN_SHADOW_AND_SET(template_head_body, "headbody", $3);
      break;

    case TYPE_FOOTER:
      WARN_SHADOW_AND_SET(template_footer, "footer", $3);
      break;

    default: /* should NEVER ever ever happen */
      cout << "FATAL: received unexpected " << $1 << " in switch" << endl; 
      abort();
  }

  free($3);
};

KeyType : TYPE_TITLE	{ $$ = TYPE_TITLE; }
	| TYPE_HEAD_BODY { $$ = TYPE_HEAD_BODY; }
	| TYPE_HEADER	{ $$ = TYPE_HEADER; }
	| TYPE_STATS	{ $$ = TYPE_STATS; }
	| TYPE_PREBODY	{ $$ = TYPE_PREBODY; }
	| TYPE_FOOTER   { $$ = TYPE_FOOTER; }
	| TYPE_BODY	{ $$ = TYPE_BODY; }
	| TYPE_BODY_TAG { $$ = TYPE_BODY_TAG; }
	;

%%

void yyerror(char const* s)
{
  cerr << "Couldn't parse \"" << yytext << "\": " << s << endl;
  exit(1);
}
