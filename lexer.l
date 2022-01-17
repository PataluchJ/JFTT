%option yylineno
%{
#include <iostream>

#include "Includes/Commands.hpp"
#include "Includes/Conditions.hpp"
#include "Includes/Identifiers.hpp"
#include "Includes/Expressions.hpp"
#include "Includes/Environment.hpp"
#include "Includes/Value.hpp"
#include "Includes/Nodes.hpp"

#include "parser.tab.hpp"

%}

%x COMMENT
%option noyywrap

%%

"("         {BEGIN(COMMENT);}
<COMMENT>{
    ")"     {BEGIN(INITIAL);}
    "("     { std::cout << "Error: nested comments.\n"; yyterminate();}
    <<EOF>> { std::cout << "Error: infinite comment.\n"; yyterminate();}
    .|\n    
}

"VAR"       {return VAR;}
"BEGIN"     {return P_BEGIN;}
"END"       {return END;}

"ASSIGN"    {return ASSIGN;}
"IF"        {return IF;}
"THEN"      {return THEN;}
"ELSE"      {return ELSE;}
"ENDIF"     {return ENDIF;}

"WHILE"     {return WHILE;}
"DO"        {return DO;}
"ENDWHILE"  {return ENDWHILE;}

"REPEAT"    {return REPEAT;}
"UNTIL"     {return UNTIL;}

"FOR"       {return FOR;}
"FROM"      {return FROM;}
"TO"        {return TO;}
"DO"        {return DO;}
"ENDFOR"    {return ENDFOR;}

"READ"      {return READ;}
"WRITE"     {return WRITE;}

"PLUS"      {return PLUS;}
"MINUS"     {return MINUS;}
"TIMES"     {return TIMES;}
"DIV"       {return DIV;}
"MOD"       {return MOD;}

"EQ"        {return EQ;}
"NEQ"       {return NEQ;}
"LE"        {return LE;}
"GE"        {return GE;}
"LEQ"       {return LEQ;}
"GEQ"       {return GEQ;}

"["         {return BRACKETL;}
"]"         {return BRACKETR;}

("-"?[0-9]+)    { yylval.num = std::stol(yytext); return number; }
[_a-z]+         { yylval.str = new std::string(yytext); return pidentifier; }

","             { return COMA;}
";"             { return SEM;}
":"             { return COL;}

[ \t\r\n]+

%%