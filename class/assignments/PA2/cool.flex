/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */
int token_index = 0, strleng = 0;
#define END_CONTEXT_WITH_ERR BEGIN(INITIAL); return ERROR;

%}

/*
 * States
 */
%START COMMENT STRING

/*
 * Define names for regular expressions here.
 */

DIGIT           [0-9]
LCASE_LETTER    [a-z]
UCASE_LETTER    [A-Z]
ID_BODY         [A-Za-z0-9_]
WHITESPACE      [ \t\f\r\v]+

/*
 * tokens
 */
CLASS           class
ELSE            else
FI              fi
IF              if
IN              in
INHERITS        inherits
LET             let
LOOP            loop
POOL            pool
THEN            then
WHILE           while
CASE            case
ESAC            esac
OF              of
DARROW          =>
NEW             new
ISVOID          isvoid
/* STR_CONST       {STR_LEGAL_CHARS}* # string constants treated specially */
INT_CONST       {DIGIT}+
BOOL_CONST      true|false
TYPEID          {UCASE_LETTER}{ID_BODY}*
OBJECTID        {LCASE_LETTER}{ID_BODY}*
ASSIGN          <-
NOT             ~
LE              <=

%%

 /*
  *  Nested comments
  */
<INITIAL>\*\)       { cool_yylval.error_msg = "Unmatched *)"; 
                      END_CONTEXT_WITH_ERR; }
<INITIAL>\(\*       { BEGIN(COMMENT); }
<COMMENT><<EOF>>    { cool_yylval.error_msg = "EOF in comment";
                      END_CONTEXT_WITH_ERR;
                    }
<COMMENT>.|\n       ;                   /* munch all single chars */
<COMMENT>\*\)       { BEGIN(INITIAL); } /* end comment normally */

 /*
  * Single-line comments
  */
<INITIAL>--.*$      ;

 /*
  * Single character tokens
  */
<INITIAL>;          { return ';'; }
<INITIAL>:          { return ':'; }
<INITIAL>\.         { return '.'; }
<INITIAL>,          { return ','; }
<INITIAL>\(         { return '('; }
<INITIAL>\)         { return ')'; }
<INITIAL>\{         { return '{'; }
<INITIAL>\}         { return '}'; }
<INITIAL>=          { return '='; }
<INITIAL>\+         { return '+'; }
<INITIAL>-          { return '-'; }
<INITIAL>\*         { return '*'; }
<INITIAL>\/         { return '/'; }
<INITIAL><          { return '<'; }
<INITIAL>@          { return '@'; }

 /*
  *  The multiple-character operators.
  */
<INITIAL>{CLASS}    { return CLASS; }
<INITIAL>{ELSE}     { return ELSE; }
<INITIAL>{FI}       { return FI; }
<INITIAL>{IF}       { return IF; }
<INITIAL>{IN}       { return IN; }
<INITIAL>{INHERITS} { return INHERITS; }
<INITIAL>{LET}      { return LET; }
<INITIAL>{LOOP}     { return LOOP; }
<INITIAL>{POOL}     { return POOL; }
<INITIAL>{THEN}     { return THEN; }
<INITIAL>{WHILE}    { return WHILE; }
<INITIAL>{CASE}     { return CASE; }
<INITIAL>{ESAC}     { return ESAC; }
<INITIAL>{OF}       { return OF; }
<INITIAL>{DARROW}   { return DARROW; }
<INITIAL>{NEW}      { return NEW; }
<INITIAL>{ISVOID}   { return ISVOID; }
<INITIAL>{ASSIGN}   { return ASSIGN; }
<INITIAL>{NOT}      { return NOT; }
<INITIAL>{LE}       { return LE; }

 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */
<INITIAL>\"       { BEGIN(STRING); string_buf_ptr = string_buf; strleng = 0; }
<STRING>[^"\n]*$  { cool_yylval.error_msg = "Unterminated string constant"; /* unexpected newline */
                    END_CONTEXT_WITH_ERR;
                  }
<STRING><<EOF>>   { cool_yylval.error_msg = "EOF in string constant";       /* EOF character */
                    END_CONTEXT_WITH_ERR;
                  }
<STRING>.*\0.*\"  { cool_yylval.error_msg = "String contains NULL char";    /* NULL (\0) character */
                    END_CONTEXT_WITH_ERR;
                  }
<STRING>\\[btnf]  { char val = '\0';                                        /* special str chars */
                    switch (yytext[1]) {
                      case 'n':
                        val = '\n';
                        break;
                      case 'f':
                        val = '\f';
                        break;
                      case 't':
                        val = '\t';
                        break;
                      case 'b':
                        val = '\b';
                        break;
                      default:
                        break;
                    };
                    *string_buf_ptr++ = val;       strleng++; }  
<STRING>\\[^btnf] { *string_buf_ptr++ = yytext[1]; strleng++; }             /* non-special escaped chars, ignore escape */
<STRING>[^"]      { *string_buf_ptr++ = yytext[0]; strleng++; }             /* normal character */
<STRING>\"        { *string_buf_ptr++ = '\0';                               /* normal termination of string */
                    if (strleng > MAX_STR_CONST) {
                      cool_yylval.error_msg = "String too long.";
                      END_CONTEXT_WITH_ERR; 
                    }
                    cool_yylval.symbol = new Entry(string_buf, strleng, token_index++);
                    BEGIN(INITIAL); 
                    return STR_CONST; 
                  }

 /*
  * Int/Bool constants
  */
<INITIAL>{INT_CONST}  { cool_yylval.symbol = new Entry(yytext, yyleng, token_index++); return INT_CONST; }
<INITIAL>{BOOL_CONST} { cool_yylval.boolean = (yytext[0] == 't') ? true : false; return BOOL_CONST; }

 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  */
<INITIAL>{TYPEID}     { cool_yylval.symbol = new Entry(yytext, yyleng, token_index++); return TYPEID; }
<INITIAL>{OBJECTID}   { cool_yylval.symbol = new Entry(yytext, yyleng, token_index++); return OBJECTID; }

 /*
  * Nothing else matches
  */
<INITIAL>{WHITESPACE} ;
<INITIAL>\n           { curr_lineno++; }
<INITIAL>.            { cool_yylval.error_msg = yytext; return ERROR; }

%%
