%{
#include <stdio.h>
#include <string.h>

#include <utils.h>

extern struct entrybuff *ebp;
        
int yywrap(void)
{
        return 1;
}

%}

%union {
    int  value;
    char *word;
}


%token GENERAL AT BLUEMAC NAME TIMEOUT
%token EQUAL OBRACE USERNAME
%token EBRACE MAC
%token SEMICOLON

%token <value> DIGIT
%token <word>  WORD MAC



%%

commands:
    |        commands command
    ;


command: GENERAL generaltrailer 
    |    entrytrailer
    ;

    /* -====||  GENERAL  ||====- */
generaltrailer: OBRACE generlentry EBRACE
    ;

generlentry:
    |   generlentry TIMEOUT   EQUAL DIGIT SEMICOLON { set_gtimeout($4); }
    ;

    /* -====||  normal entry || ====- */
entrytrailer: WORD EQUAL OBRACE    userentry EBRACE   { ebp->type = USER;  ebp->name = $1; set_userentry(ebp); }
    | AT WORD EQUAL OBRACE groupentry EBRACE  { ebp->type = GROUP; ebp->name = $2; set_userentry(ebp); }
    ;

userentry:
    | userentry BLUEMAC EQUAL MAC SEMICOLON    { ebp->btmac   = $4; }
    | userentry NAME EQUAL WORD SEMICOLON      { ebp->btname  = $4; }
    | userentry TIMEOUT EQUAL DIGIT SEMICOLON  { ebp->timeout = $4; }
    ;

groupentry:
    | groupentry BLUEMAC EQUAL MAC SEMICOLON    { ebp->btmac   = $4; }
    | groupentry NAME EQUAL WORD SEMICOLON      { ebp->btname  = $4; }
    | groupentry TIMEOUT EQUAL DIGIT SEMICOLON  { ebp->timeout = $4; }
    ;
