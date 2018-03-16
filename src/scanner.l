%option prefix="mCc_parser_"

%option batch
%option bison-bridge
%option noinput
%option nounput
%option noyywrap
%option reentrant

%{
#include "parser.tab.h"

#define YYSTYPE MCC_PARSER_STYPE
%}
identifier    [a-zA-Z_][a-zA-Z0-9_]*
int_literal   [0-9]+
float_literal [0-9]+\.[0-9]+
string_literal \"[^"]*\"
bool_literal (true)|(false)


%%

{int_literal}     { yylval->TK_INT_LITERAL = atol(yytext); return TK_INT_LITERAL; }

{float_literal}   { yylval->TK_FLOAT_LITERAL = atof(yytext); return TK_FLOAT_LITERAL; }

{string_literal}   { yylval->TK_STRING_LITERAL = yytext; return TK_STRING_LITERAL; }

{bool_literal}    { yylval->TK_BOOL_LITERAL = strcmp("true", yytext) == 0 ? true : false;
		    return TK_BOOL_LITERAL;  }

"+"               { return TK_PLUS; }
"-"               { return TK_MINUS; }
"*"               { return TK_ASTER; }
"/"               { return TK_SLASH; }
"<"               { return TK_LESS; }
">"               { return TK_GREATER; }
"<="              { return TK_LESS_EQ; }
">="              { return TK_GREATER_EQ; }
"&&"              { return TK_AND; }
"||"              { return TK_OR; }
"=="              { return TK_EQUALS; }
"!="              { return TK_NOT_EQUALS; }
"!"               { return TK_NOT; }

";"               { return TK_SEMICOLON; }

"("               { return TK_LPARENTH; }
")"               { return TK_RPARENTH; }
"{"               { return TK_LBRACE; }
"}"               { return TK_RBRACE; }

"if"              { return TK_IF; }
"else"            { return TK_ELSE; }
"while"           { return TK_WHILE; }
"return"          { return TK_RETURN; }

[ \t\r\n]+        { /* ignore */ }

<<EOF>>           { return TK_END; }

{identifier}      { yylval->TK_IDENTIFIER = yytext; return TK_IDENTIFIER; }

.                 { fprintf(stderr, "invalid character '%c'\n", yytext[0]); }
