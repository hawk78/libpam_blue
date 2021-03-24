%{
/* definition section */
#include <stdio.h>
#include "parser.h"
#include "global.h"

#define MAXLEXERR 500

        
/* error stuff */
char lexerrbuff[MAXLEXERR];
int lineno = 1;
void yyerror(char *s);


%}

HXDIGD [0-9a-fA-F][0-9a-fA-F][:][0-9a-fA-F][0-9a-fA-F][:]
HXDIG  [0-9a-fA-F][0-9a-fA-F][:][0-9a-fA-F][0-9a-fA-F]

%%

#.*                       ;
general                   { return GENERAL; }
timeout                   { return TIMEOUT; }
username                  { return USERNAME; }
name                      { return NAME; }
bluemac                   { return BLUEMAC; }
@                         { return AT;}
;                         { return SEMICOLON; }
[a-zA-Z][a-zA-Z0-9_]*     { yylval.word  = (char *) strdup(yytext); return WORD; }
{HXDIGD}{HXDIGD}{HXDIG}   { yylval.word  = (char *) strdup(yytext); return MAC; }
[0-9][0-9]*               { yylval.value = atoi(yytext);   return DIGIT; }
=                         { return EQUAL; }
\{                        { return OBRACE; }
\}                        { return EBRACE; } 
\n.*                      { strncpy(lexerrbuff, yytext + 1, MAXLEXERR - 1);
                            lineno++;
                            yyless(1);
                          }
[ \t]+                    ;                   /* ignone whitespaces */ 
.                         ;                   /* FIXME: add a catch all rule */  

%%


void
yyerror(char *str)
{
        syslog (LOG_ERR, "Parse Error in configuration file!");
}
