%{

#include <string>
#include <iostream>
#include <vector>

#include "templates.h"

#define WARN_SHADOW(e) cerr << "Warning: additional definition of '" << e << "' shadows previous" << endl;
using namespace std;

string * template_title;
string * template_header;
string * template_prebody;
string * template_stats;
string * template_body;

%}

%union {
	string *s;
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
      if (template_title)
      {
        WARN_SHADOW("title");
	delete template_title;
      }
      
      template_title = new string ($3);
      break;
      
    case TYPE_HEADER:
      if (template_header)
      {
        WARN_SHADOW("header");
        delete template_header;
      }
      
      template_header = new string ($3);
      break;
      
    case TYPE_STATS:
      if (template_stats)
      {
        WARN_SHADOW("stats");
	delete template_stats;
      }

      template_stats = new string ($3);
      break;
      
    case TYPE_PREBODY:
      if (template_prebody)
      {
        WARN_SHADOW("prebody");
	delete template_prebody;
      }

      template_prebody = new string ($3);
      break;

    default: /* should NEVER ever ever happen */
      abort();
  }
}

KeyType : TYPE_TITLE	{ $$ = TYPE_TITLE; }
	| TYPE_HEADER	{ $$ = TYPE_HEADER; }
	| TYPE_STATS	{ $$ = TYPE_STATS; }
	| TYPE_PREBODY	{ $$ = TYPE_PREBODY; }
	| error		{ cerr << "Unrecognized key type at line " << @1 << ", continuing" << endl; }
	;

%%
