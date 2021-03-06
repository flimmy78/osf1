/*
 * *****************************************************************
 * *                                                               *
 * *    Copyright (c) Digital Equipment Corporation, 1991, 1994    *
 * *                                                               *
 * *   All Rights Reserved.  Unpublished rights  reserved  under   *
 * *   the copyright laws of the United States.                    *
 * *                                                               *
 * *   The software contained on this media  is  proprietary  to   *
 * *   and  embodies  the  confidential  technology  of  Digital   *
 * *   Equipment Corporation.  Possession, use,  duplication  or   *
 * *   dissemination of the software and media is authorized only  *
 * *   pursuant to a valid written license from Digital Equipment  *
 * *   Corporation.                                                *
 * *                                                               *
 * *   RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure  *
 * *   by the U.S. Government is subject to restrictions  as  set  *
 * *   forth in Subparagraph (c)(1)(ii)  of  DFARS  252.227-7013,  *
 * *   or  in  FAR 52.227-19, as applicable.                       *
 * *                                                               *
 * *****************************************************************
 */
/*
 * HISTORY
 */
/********************************************************************/
/*                    Compiler Revisions                            */
/*                                                                  */
/*  init   Date        Revision/Changes                             */
/*  ----   ----        ----------------	                            */
/*  bsb   9/7/90       1.0 / Initial general customer release       */
/********************************************************************/
/*                  scsi_lex.c Revisions                            */
/*						                    */
/*  init   Date        Revision/Changes		                    */
/*  ----   ----        -----------------	                    */
/*  bsb   9/7/90       1.0 / Initial general customer release       */
/********************************************************************/

/*
    SCSI_LEX.C  -   Lexical analyzer for SCSI scripts compiler

    This program breaks a SCSI script into tokens which are passed on to another
    program for compilation.  It also maintains a symbol table, and is set up to
    perform multiple passes through the input, until all references are resolved to
    a final value.

    This program replaces a LEX generated program.  LEX is not used because although 
    the SCSI script syntax is very straightforward, the LEX generated program is very 
    large and inefficient.

    This program is designed to interface with a compiler generated by YACC, and has
    all the necessary support.  


*/

#include "scsi.h"
#include YACC_HEADER	 /* ytab.h */
 
char lex_buffer[MAX_LEX_IN_LEN];	 /* buffer of current line */
int line_no = 0;	                 /* current line number */
symbol_t    *SymTableStart;		  /* Start of symbols */
int         pass_param;				  /* is token a pass token */
int         array_param;			  /* is token an array name */
symbol_t    *SymTableEnd;			  /* end of symbols    */

/*
static void lex_init();					/* lex initialization function */
static int lex_error();					/* lex error routine */
static int handle_constant();			/* function to handle constants */
static int handle_identifier();		/* function to handle identifiers */
static int keyword_lookup();			/* function that determines if token is keyword */
static int add_symbol();				/* function that adds symbol to symbol table */
extern UINT32 convert();			  /* Convert value to unsigned long integer */
extern unsigned val();				  /* Converts hex to value */
extern symbol_t *symbol_lookup();	/* checks to see if symbol is in table */
extern char *skip_white();				/* skips blanks and tabs */
extern char *load_pass();				/* loads a pass token into buffer */
extern char *skip_ident();				 /* skips an idntifier token */

static BOOL         line_valid = 0;	  /* is line valid flag */
static char        *line_ptr = NULL;	/* ptr used to point to current token in line */
static symbol_t    FirstSym;				/* First Symbol */

static char ident_chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
static char pass_chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_()\";!@$%^[,]{}~\\?/`|&.<> +-#*";
int Count;

/* Get keyword array */

#include "keywords.h"


void lex_init()
/*
    This function initializes the lexical analyzer
*/
{
    FirstSym.value = 0;
    FirstSym.defined = 0;
    FirstSym.next = NULL;
    FirstSym.patch = NULL;
    FirstSym.endpatch = NULL;
    FirstSym.type = IDENTIFIER;
    FirstSym.entry = 0;
    SymTableStart = &FirstSym;
    SymTableEnd = NULL;                          

}

int yylex()
/*
    This function is called by the YACC generated compiler every time it wants a token.
    It returns the token value, and optionally returns a value in the variable yylval.
*/
{   
    char first;			/* points to first character in token */
    pass_param = 0;
    array_param = 0;
    Count = 0;			  /* variable is used to determine if pass parameter
                          has equal number of left and right brackets */

    if (!line_valid) {	  /* load lex_buffer from input file */
        if (fgets(lex_buffer,MAX_LEX_IN_LEN,input_file) != NULL) {
            line_valid = 1;
            line_no++;
            line_ptr = lex_buffer;
        }
        else {  /* End of input file - main will rewind for new pass */
            return 0;
        }
        
    }
    /* At this point, line_ptr points to valid text from the input file */
    
    line_ptr = skip_white(line_ptr);
    first = toupper(*line_ptr);
    if ( (first >= 'A' && first <= 'Z') || first == '_' ) 
       return handle_identifier();
    else if ( first >= '0' && first <= '9' )
        return handle_constant();
    else
        switch(first) {     /* Only simple operator characters are left now */

            /* First, check for specially handled characters */
            case '#':/*comment*/
            case '\n':      /* End of line */
            case '\0':      /* End of line at end of file */
                line_valid = 0;
                return EOL;

	   case ';':
	        line_ptr++;
		return EOL;

            /* Next check for 2-character operators */
            case '<':
                if ( *(line_ptr+1) != '<' )
                    return lex_error(UNEXPECTED_CHAR);
                else {

                    line_ptr += 2;
                    return(SHIFT_LEFT);
                }
            case '>':
                if ( *(line_ptr+1) != '>' )
                    return lex_error(UNEXPECTED_CHAR);
                else {
                    line_ptr += 2;
                    return(SHIFT_RIGHT);
                }

            /* Now check remaining one-character tokens */
            default:
                if (strchr("+-*/%&|^~(),=:",first) != NULL) {
                    line_ptr++;
                    return first;
                }
                else
                    return lex_error(UNEXPECTED_CHAR);
        }   /* End of switch */
}   /* End of yylex */


static int handle_constant()	/* handles constant values */
{
    if (*line_ptr != '0')	  /* base 10 constant */
        yylval.i = convert(10);
    else {
        line_ptr++;
        switch (toupper(*line_ptr)) {
            case 'X':						 /* base 16 constant */
                line_ptr++;
                yylval.i = convert(16);
                break;
            case 'B':						  /* binary constant */
                line_ptr++;
                yylval.i = convert(2);
                break;
            default:							 /* is it zero or base 8 */
                line_ptr--;     /* Back up, in case it's 0 by itself */
                yylval.i = convert(8);
                break;
        }
    }
    return(INTEGER);
}

UINT32 convert(base)			  /* convert base to unsiged long integer */
unsigned base;
{
    UINT32 value = 0;
    UINT32 old_val;
    BOOL flag = 0;
    unsigned digit;
    
    digit = val(*line_ptr);
    if (digit >= base)
        return lex_error(BAD_CONSTANT);
    for ( ;  digit < base; digit = val(*++line_ptr) ) {
        old_val = value;
        value = base * value + digit;
        if (value < old_val) {
            flag = 1;
            break;
        }
    }
    if (flag)
        return lex_error(CONSTANT_OVERFLOW);
    else
        return(value);
}

unsigned val(ch)
char ch;
{
    static char my_digs[] = "0123456789ABCDEF";
    char *index;
        
    index = strchr(my_digs,toupper(ch));
    if (index != NULL)
        return (index - my_digs);
    else
        return 255;
}


static int handle_identifier()
{
    int i;
    symbol_t *sym;
    char *ptr;
    i = keyword_lookup();
    if ((i != -1) && (keywords[i].token == PASS))	  /* is it pass parameter */
        {
           pass_param = 1;
           i = -1;
           ptr = line_ptr;
           while (*ptr != '(')
	          ptr++;
	         ptr++;
	  line_ptr = ptr;
     sym = symbol_lookup();
     if (sym!= NULL){	  /* was symbol in table */
       yylval.s = sym;
       return sym->type;
     }
     if (sym == NULL)
	  return add_symbol();	  /* symbol needs to be added to table */
     }
     else if ((i!= -1) && (keywords[i].token == PROC)){
          i = -1;
	  line_ptr = skip_white(line_ptr);
	  array_param = 1;
     sym = symbol_lookup();
     if (sym!= NULL){
       yylval.s = sym;
       return sym->type;
     }
     if (sym == NULL)
	  return add_symbol();
	  }
      if (i != -1) 
      {
        yylval.i = keywords[i].val;
        return keywords[i].token;
      }
      else 
      {
        sym = symbol_lookup();
        if (sym != NULL) 
        {
            yylval.s = sym;
            return sym->type;
         }
       if (sym == NULL) 
            return add_symbol();
       }
return sym->type;
}

static int keyword_lookup()
{

    int i;
    char *ptr,*entptr;

    for (i=0; i<keyword_entries; i++) {
        entptr = keywords[i].name;
        ptr = line_ptr;
        while (*entptr++ == toupper(*ptr++))
            if (*entptr == '\0' && (*ptr == '\0' || strchr(ident_chars,*ptr) == NULL)) { 
                line_ptr = ptr;
                return i;             
            }
    }
    return -1;   /* No match was found */
}

symbol_t *symbol_lookup()
{
    char *ptr,*entptr,*endptr,ch;
    int size;
    symbol_t *CurSym;
    char buf[MAX_IDENT_LEN+1];
    if (pass_param){
       endptr = load_pass(line_ptr);
       if (Count != 0)
       lex_error(MISMATCH_BRACKETS);
       }
    else
    endptr = skip_ident(line_ptr);
    size = endptr - line_ptr;
    if (size > MAX_IDENT_LEN)
	size = MAX_IDENT_LEN;
    strncpy(buf,line_ptr,size);
    buf[size] = '\0';

    for (CurSym = SymTableStart; CurSym != NULL; CurSym = CurSym->next)
    {
        entptr = CurSym->name;
        ptr = buf;
        while ((ch = *entptr++) == *ptr++)
            if (ch == '\0')
           {
      if (pass_param)
        line_ptr = ++endptr;
      else
		  line_ptr = endptr;
		return CurSym;
	    }
    }
    return NULL;   /* No match was found */
}        


char *skip_white(ptr)
char *ptr;                          
{             
    while (*ptr == ' ' || *ptr == '\t')
        ptr++;
    return ptr;
}


char *load_pass(ptr)
char *ptr;
{
Count = 1;					/* line_ptr already skipped over right bracket */
while ((Count != 0) && (strchr(pass_chars,*ptr) != NULL)){
  if (*ptr == ')')
   Count--;
  if (*ptr == '(')
   Count++;
  ptr++;
  }
if (*ptr == ')'){
  Count = -1;
  ptr--;
  }
else {
  while (*ptr != ')')
  ptr--;
  }
return ptr;
}

char *skip_ident(ptr)
char *ptr;
{
    while (*ptr && strchr(ident_chars,*ptr) != NULL)
        ptr++;
    return ptr;
}


static int lex_error(err_no)
UINT32 err_no;
{
    ErrorChar = line_ptr - lex_buffer;
    error(err_no,"");
    line_valid = 0;
    return EOL;
}

void lex_warning(warn_no)
UINT32 warn_no;
{
    ErrorChar = line_ptr - lex_buffer;
    error(warn_no,"");
}


static int add_symbol()
{
    char *ptr;
    int size;
    symbol_t *sym;


    /* Create new entry in symbol table */

    if (SymTableEnd == NULL) {      /* First time through */
        sym = SymTableStart;
        SymTableEnd = SymTableStart;
    }
    else {
        sym = SymTableEnd->next = (symbol_t *) scsi_malloc( sizeof(symbol_t) );
        SymTableEnd = sym;
    }


    /* Get name of symbol */
    if (pass_param == 1)
       ptr = load_pass(line_ptr);
    else
    ptr = skip_ident(line_ptr);
    size = ptr - line_ptr;
    if ( size > MAX_IDENT_LEN ) {
        lex_warning(IDENT_TOO_LONG);
        size = MAX_IDENT_LEN;
    }

    strncpy(sym->name,line_ptr,size);
    sym->name[size] = '\0';
    if (pass_param)
     line_ptr = ++ptr;
    else
     line_ptr = ptr;


    /* Initialize fields in the symbol */

    sym->value = 0;
    sym->defined = 0;
    sym->next = NULL;
    sym->patch = NULL;
    sym->endpatch = NULL;
    if (pass_param == 1)
      sym->type = PASSIFIER;
    else if (array_param == 1)
      sym->type = ARRAYNAME;
    else sym->type = IDENTIFIER;
    sym->entry = 0;


    /* Now return proper values */

    yylval.s = sym;
    return sym->type;
}


char  *scsi_malloc(size)
int size;
{
char   *p;
    if ((p = (char *) malloc(size + 4)) == NULL)
        error(MALLOC_ERR,"");
    return p;
}

int all_defined()
{
    symbol_t *sym;

    for (sym = SymTableStart; sym != NULL; sym = sym->next)
        if (sym->defined == 0)
            return 0;
    return 1;
}
