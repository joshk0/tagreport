%{

#include <string>
#include <iostream>
#include <vector>

#include "template.h"

using namespace std;

string body_format;
string 

%}

%union {
	string *s;
	int kt;
	vector<pair*> *set;
	pair* keypair;
	Template* tm;
}

%token   BODY_START TYPE_TITLE TYPE_HEADER TYPE_STATS TYPE_PREBODY TYPE_FOOTER
%token   <s> BODY TYPE_VALUE

%type	<set> KeyPairs
%type	<keypair> KeyPair
%type	<kt> KeyType
%type	<tm> Template

%%

Template: KeyPairs BODY_START BODY { $$ = new Template ($1, $3); }

KeyPairs: KeyPairs KeyPair	{ ($$=$1)->push_back($2); }
	| KeyPair		{ ($$=new vector<pair*>)->push_back($1); }
	;

KeyPair: KeyType '=' TYPE_VALUE { $$ = new pair ($1, $3); }

KeyType : TYPE_TITLE	{ $$ = TYPE_TITLE; }
	| TYPE_HEADER	{ $$ = TYPE_HEADER; }
	| TYPE_STATS	{ $$ = TYPE_STATS; }
	| TYPE_PREBODY	{ $$ = TYPE_PREBODY; }
	| error		{ cerr << "Unrecognized key type at line " << @1 << ", continuing" << endl; }
	;

%%
