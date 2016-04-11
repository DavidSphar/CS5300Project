/* 
 * CS 5300 Compiler Construction - Spring 2016
 * The lexical analyzer for CPSL
 * Author: Tam Nguyen
 * Email: tamnguyenthe91@gmail.com
 * A-number: A02056664
 */

%option noyywrap yylineno
%{
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "Expression.h"
#include "LValue.h"
#include "cpsl.tab.h"

using namespace std;

char processChar(char *text);
%}

%%
 /* Comments */
\$.*\n 					{ /* ignore */ }

 /* Keywords */
array|ARRAY				{ return ARRAY;	}
else|ELSE				{ return ELSE;	}
if|IF					{ return IF; 	}
record|RECORD			{ return RECORD;}
then|THEN				{ return THEN; 	}
write|WRITE				{ return WRITE; }
begin|BEGIN				{ return BEGIN_;	}
elseif|ELSEIF			{ return ELSEIF;}
of|OF					{ return OF;	}
ref|REF					{ return REF;	}
to|TO					{ return TO;	}
chr|CHR					{ return CHR;	}
end|END					{ return END;	}
ord|ORD					{ return ORD;	}
repeat|REPEAT			{ return REPEAT;}
type|TYPE				{ return TYPE;	}
const|CONST				{ return CONST;	}
for|FOR					{ return FOR;	}
pred|PRED				{ return PRED;	}
return|RETURN			{ return RETURN;}
until|UNTIL				{ return UNTIL;	}
do|DO					{ return DO;	}
forward|FORWARD			{ return FORWARD;	}
procedure|PROCEDURE		{ return PROCEDURE;	}
stop|STOP				{ return STOP;	}
var|VAR					{ return VAR;	}
downto|DOWNTO			{ return DOWNTO;}
function|FUNCTION		{ return FUNCTION;	}
read|READ				{ return READ;	}
succ|SUCC				{ return SUCC;	}
while|WHILE				{ return WHILE;	}
true|TRUE               { return TRUE_LIT;  }
false|FALSE             { return FALSE_LIT; }

 /* Identifiers */
[a-zA-Z][a-zA-Z0-9_]*	{ yylval.strVal = strdup(yytext); return IDENTIFIER;	}

 /* Operators */
"<="					{ return LEQ;	}
">="					{ return GEQ;	}
"<>"					{ return NEQ;	}
":="					{ return ASSIGN;}
"+"						{ return PLUS;	}
"-"						{ return MINUS;	}
"*"						{ return MULT;	}
"/"						{ return DIV;	}
"&"						{ return AND;	}
"|"						{ return OR;	}
"~"						{ return NOT;	}
"="						{ return EQ;	}
">"						{ return GT;	}
"<"						{ return LT;	}
"%"						{ return MOD;	}

 /* Delimiters */
"."						{ return DOT;	}
","						{ return COMMA;	}
":"						{ return COLON;	}
";"						{ return SEMICOLON;	}
"("						{ return LPAREN;}
")"						{ return RPAREN;}
"["						{ return LBRACKET;	}
"]"						{ return RBRACKET;	}			

 /* Integer constants */
0[0-7]*					{ yylval.intVal = strtol(yytext, nullptr, 8); return INTEGER; } /* Octal number */
0x[0-9A-Fa-f]+			{ yylval.intVal = strtol(yytext, nullptr, 16); return INTEGER; } /* Hexadecimal number */
[1-9][0-9]*				{ yylval.intVal = atoi(yytext); return INTEGER; } /* Decimal number */


 /* Character constants */
'([^'\n\\]|\\[ -~])'				{ yylval.charVal = processChar(yytext); return CHARACTER; }

 /* String constants */
\"([^\"\n\\]|\\[ -~])*\"			{ yylval.strVal = strdup(yytext); return STRING;	}

 /* white space */
[ \t\n]+

 /* Invalid token */
[0-9]+[a-zA-Z_]+ 		{ std::cout << "Invalid token " << yytext << "abc" << std::endl; }
.						{ std::cout << "Invalid token " << yytext << "abc" << std::endl;  }

%%

char processChar(char *text) {
    if (text[1] == '\\') {
        switch(text[2]) {
            case 'n': return '\n';
            case 'r': return '\r';
            case 'b': return '\b';
            case 't': return '\t';
            case 'f': return '\f';
            default: return text[2];
        }
    }
    return text[1];
}