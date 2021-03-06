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
#ifndef lint
static char *rcsid = "@(#)$RCSfile: wordexp.c,v $ $Revision: 1.1.6.2 $ (DEC) $Date: 1993/06/08 20:46:25 $";
#endif
/*
 * (c) Copyright 1990, 1991, 1992, 1993 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
 */
/*
 * OSF/1 1.2
 */
/*
 * COMPONENT_NAME: (LIBCPAT) Standard C Library Pattern Functions 
 *
 * FUNCTIONS: wordexp, wordfree
 *
 * (C) COPYRIGHT International Business Machines Corp. 1991
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 * 
 * 1.2  com/lib/c/pat/wordexp.c, bos, bos320 5/27/91 15:28:29s
 */
/* name space pollution clean up */
#ifdef _NAME_SPACE_WEAK_STRONG
#include "pollution.h"
#if !defined(_THREAD_SAFE)
#pragma weak wordexp = __wordexp
#pragma weak wordfree = __wordfree
#endif
#endif
#ifdef _NAME_SPACE_WEAK_STRONG
#ifdef stat
#undef stat
#endif
#endif

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wchar.h>
#include <fcntl.h>
#include <limits.h>
#include <memory.h>
#include <paths.h>
#include <wordexp.h>

#define	SETU	"set -u; "
#define	SET	"set -- "
#define IFSENV	"export IFS=\""
#define SCRIPT	"; echo $#; while [ $# -gt 0 ]; do\n echo \"${#1}\"; echo \"$1\"; shift; done\n"

/************************************************************************/
/* Internal function prototypes						*/
/************************************************************************/

static FILE *wordexp_popen(char *, char *, int, pid_t *);
static int wordexp_pclose(FILE *, pid_t);

static
const char *
match( const char *p, int c)
{
    int wclen;
    while (*p && *p != c) {
	if (*p == '\\')
	    p++;
	wclen = mblen(p, MB_CUR_MAX);
	if (wclen < 1)
	    return NULL;
	p += wclen;
    }

    if (*p == c)
	return ++p;
    else
	return NULL;
}

/*
 * getmb - get 'nc' characters from file 'f' and put them into 'buf'.
 *
 * returns count of bytes put into 'buf'.
 */

static int
getmb( char *buf, int nc, FILE *f)
{
    char *p = buf;
    wint_t	wc;
    int		clen;

    while (nc--) {		/* Read each character */
	wc = fgetwc(f);
	if (wc == WEOF)
	    return (-1);		/* Shouldn't ever happen, right? */

	clen = wctomb(p, wc);	/* Convert to byte sequence */
	if (clen == -1)		/* Shouldn't ever happen! */
	    return (-1);

	p += clen;
    }

    *p = '\0';			/* Null terminate, just in case */
    return (p-buf);
}

/************************************************************************/
/* wordexp() - expand string to word list				*/
/************************************************************************/
int
wordexp(const char *pwords, wordexp_t *pwordexp, int flags)
{
	int	c;		/* quoted string match character	*/
	pid_t	child_pid;	/* pid of popen child			*/
	int	err_stat;	/* return error status			*/
	FILE	*f;		/* wordexp_popen pipe stream ptr	*/
	int	len;		/* expanded word length			*/
const	char	*p;		/* illegal character scan ptr		*/
	char	*pbuf;		/* shell command buffer			*/
	char	**pnext;	/* ptr to next we_wordv entry		*/
	char	*pwordv;	/* iptr to new we_wordv array		*/
	int	new_count;	/* # of new expanded words		*/
	int	old_count;	/* # of old expanded words		*/
	int	stat;		/* wordexp_pclose() return status	*/
	int	total_count;	/* total size of we_wordv		*/
	int	wclen;		/* # bytes in next character		*/
	char	buf[LINE_MAX];	/* expanded words from shell		*/
	int	mb_cur_max;
	char	*ifs;
/*
 * release previously allocated memory if reuse flag is set
 * clear append flag - just in case
 */
	if ((flags & WRDE_REUSE) != 0)
		{
		wordfree(pwordexp);
		flags &= ~WRDE_APPEND;
		}
/*
 * reject word string if it contains illegal unquoted character
 * or has syntax error (illegal multibyte or <backslash> followed by NUL)
 */

	p = pwords;
	while (*p)
		switch (*p)
			{
		case '\\':
			wclen = mblen(++p, MB_CUR_MAX);
			if (wclen < 1)
				return (WRDE_SYNTAX);
			p += wclen;
			break;

		case '$':
			p++;
			if (*p == '(') {
			    if ((flags & WRDE_NOCMD) != 0)
				return (WRDE_CMDSUB);
			    p = match(p, ')');
			    if (!p) return (WRDE_SYNTAX);
			    break;
			} else if (*p == '{') {
			    p = match(p, '}');
			    if (!p) return (WRDE_SYNTAX);
			    break;
			}

			while (*p && *p != ' ' && *p != '\t')
				{
				if (*p == '\\')
					p++;
				wclen = mblen(p, MB_CUR_MAX);
				if (wclen < 1)
					return (WRDE_SYNTAX);
				p += wclen;
				}
			break;

		case '"':
		case '\'':
		case '`':
			c = *p++;
			if ( (c == '`') && (flags & WRDE_NOCMD) != 0 )
			    	return (WRDE_CMDSUB);

			p = match(p,c);
			if (!p)
			    	return (WRDE_SYNTAX);

			break;

		case '\n':
		case '|':
		case '&':
		case ';':
		case '<':
		case '>':
		case '(':
		case ')':
		case '{':
		case '}':
			return (WRDE_BADCHAR);

		case '#':
			/*
			 * Embedded comments are ignored
			 */
			goto escape;

		default:
			wclen = mblen(p, MB_CUR_MAX);
			if (wclen < 1)
				return (WRDE_SYNTAX);
			p += wclen;
			break;
			}
/*
 * Here on end of string (or on a '#' - the comment character)
 */

escape:

	ifs = getenv("IFS");

/*
 * allocate shell command buffer with extra space needed by wordexp()
 */
	if (ifs) {
	  pbuf = malloc((p-pwords) +
		  strlen(IFSENV) + strlen(ifs) + 2 +	/* export IFS=str; */
		  strlen(SETU) + strlen(SET) + strlen(SCRIPT) + 1);
							/* terminating NULL */
	} else {
	  /* don't call strlen when ifs null */
	  pbuf = malloc((p-pwords) +
		  strlen(SETU) + strlen(SET) + strlen(SCRIPT) + 1);
							/* terminating NULL */
	}
	if (pbuf == (char *)0)
		return (WRDE_NOSPACE);
/*
 * build shell command
 */
	*pbuf = '\0';

	if ( ifs && *ifs ) {	/* If there's an IFS environment variable */
	    strcat(pbuf,IFSENV);	/*   pass IFS=<value>; down to the child  */
	    strcat(pbuf,ifs);
	    strcat(pbuf,"\";");
	}

	if ((flags & WRDE_UNDEF) != 0)
		strcat(pbuf, SETU);
	strcat(pbuf, SET);
	strncat(pbuf, pwords,p-pwords);
	strcat(pbuf, SCRIPT);
/*
 * invoke POSIX shell - which does the word expansion
 * release popen() command buffer memory
 */
	f = wordexp_popen(pbuf, "r", flags, &child_pid);
	free(pbuf);
	if (f == (FILE *)0)
		return (WRDE_EPOPEN);
	err_stat = 0;
/*
 * first returned string is decimal number of expanded words
 */
	if (fgets(buf, sizeof(buf), f) == 0)
		{
		err_stat = WRDE_ESHELL;
		goto errors;
		}
	new_count = atoi(buf);
	if (new_count < 0)
		{
		err_stat = WRDE_ESHELL;
		goto errors;
		}
/*
 * allocate a new we_wordv array, taking into account appending from
 *   a previous call and reserved NULL offsets
 */
	if ((flags & WRDE_APPEND) == 0)
		old_count = 0;
	else
		old_count = pwordexp->we_wordc;
	if ((flags & WRDE_DOOFFS) != 0)
		old_count += pwordexp->we_offs;
	total_count = new_count + old_count + 1;
	pwordv = (char *)malloc(total_count * sizeof(char *));
	if (pwordv == (char *)0)
		{
		err_stat = WRDE_NOSPACE;
		goto errors;
		}
/*
 * copy previous we_wordv array to newly allocated buffer, free old array memory
 *   or initialize new structure
 */
	if ((flags & WRDE_APPEND) != 0)
		{
		memcpy(pwordv, pwordexp->we_wordv, (old_count) * sizeof(char *));
		free(pwordexp->we_wordv);
		}
	else
		{
		pwordexp->we_wordc = 0;
		pwordexp->we_sflags = flags;
		if ((flags & WRDE_DOOFFS) != 0)
			{
			pwordexp->we_soffs = pwordexp->we_offs;
			memset(pwordv, 0, pwordexp->we_offs * sizeof(char *));
			}
		}
	pwordexp->we_wordv = (char **)pwordv;
	pnext = (char **)pwordv + old_count;
/*
 * read each expanded word into newly allocated memory
 * eliminate trailing <newline> if present
 * add trailing null pointer to we_wordv when all done
 */
	mb_cur_max = MB_CUR_MAX;

	while ((fgets(buf, sizeof(buf), f) != 0) && (new_count-- > 0))
		{
		char *np;

		len = atoi(buf) + 1; 		 /* Count trailing NL */

		np = malloc(len * mb_cur_max+1); /* Space for worst-case */
		if (np == NULL) {
		    err_stat = WRDE_NOSPACE;
		    goto errors;
		}
		len = getmb(np,len, f);		 /* Now 'len' is byte-count */
		if (len == -1) {
		    err_stat = WRDE_ESHELL;
		    goto errors;
		}
		    
		if (np[len-1] == '\n')
			np[len-1] = '\0';
		else
			len++;

		*pnext++ = np;
		pwordexp->we_wordc++;
		}

	*pnext = 0;
/*
 * close the pipe previously opened by popen
 *
 * NOTE: NEED TO RETURN SEPARATE ERROR RETURNS FOR CERTAIN SHELL ERRORS
 * BUT THEY HAVE NOT BEEN SPECIFIED YET
 */
errors:
	stat = wordexp_pclose(f, child_pid);
	if (stat && (flags & WRDE_UNDEF) != 0)
	    	return (WRDE_BADVAL);
	if (stat)
		return (WRDE_SYNTAX);

	return (err_stat);

}


/************************************************************************/
/* wordexp_popen() - establish pipe to POSIX shell			*/
/*		   - a rendition of popen() that execs /bin/psh		*/
/*									*/
/* 	popen() and pclose() are not used because popen() always	*/
/*	execs /bin/sh which may not be the POSIX shell			*/
/************************************************************************/
static FILE *
wordexp_popen(char *cmd, char *mode, int flags, pid_t *child_pid)
{
	FILE	*f;		/* stderr stream ptr			*/
	int	myside;		/* pipe read file descriptor		*/
	int	p[2];		/* pipe() file descriptor array		*/
	pid_t	pid;		/* pid of child				*/
	int	yourside;	/* pipe write file descriptor		*/
/*
 * create pipe read and write file descriptors
 */
	if(pipe(p) < 0)
		return(NULL);
	myside = p[0];
	yourside = p[1];
/*
 * create child to exec the POSIX shell
 * close child's pipe write file descriptor
 * close child's stdout
 * duplicate child's pipe read to stdout
 * close child's pipe read file descriptor
 * note: myside and yourside reverse roles in child
 */
	if((pid = fork()) == 0)
		{
		(void) close(myside);
		(void) close(1);
		(void) fcntl(yourside, F_DUPFD, 1);
		(void) close(yourside);
/*
 * redirect stderr to /dev/null if no error output allowed
 */
		if ((flags & WRDE_SHOWERR) == 0)
			if ((f = fdopen(2, "w")) != 0)
				freopen("/dev/null", mode, f);

		(void) execl(_PATH_KSH, "ksh", "-c", cmd, (char *)0);
		_exit(1);
		}
/*
 * as parent, save child's pid
 * close parent's pipe write file descriptor
 */
	if(pid == -1)
		return(NULL);
	*child_pid = pid;
	(void) close(yourside);
	return(fdopen(myside, mode));
}


/************************************************************************/
/* wordexp_pclose() - close files from previous wordexp_popen()		*/
/*		    - a rendition of pclose()				*/
/*									*/
/* 	popen() and pclose() are not used because popen() always	*/
/*	execs /bin/sh which may not be the POSIX shell			*/
/************************************************************************/
static int
wordexp_pclose(FILE *ptr, pid_t child_pid)
{
	int	status;		/* return status			*/
	struct sigaction ointact,oquitact,ohupact,ignact;

	ignact.sa_handler = SIG_IGN;
	sigemptyset(&(ignact.sa_mask));
	ignact.sa_flags = 0 ;
/*
 * close pipe's read file descriptor
 */
	(void) fclose(ptr);
/*
 * ignore followign signals until child terminates
 */
	sigaction(SIGHUP,&ignact,&ohupact);
	sigaction(SIGINT,&ignact,&ointact);
	sigaction(SIGQUIT,&ignact,&oquitact);
/*
 * wait for termination of child created by wordexp_popen
 */	
	if (child_pid <= 0)
		status = -1;
	else if (waitpid(child_pid,&status,0) == -1)
		status = -1;
	sigaction(SIGHUP,&ohupact,NULL);
	sigaction(SIGINT,&ointact,NULL);
	sigaction(SIGQUIT,&oquitact,NULL);

	return(status);
}


/************************************************************************/
/* wordfree() - release memory used in pwordexp structure		*/
/************************************************************************/
void
wordfree(wordexp_t *pwordexp)
{
	char	**pnext;

/*
 * skip over any reserved NULL pointers first
 * this is why the initial flags and we_offs was saved -
 *   prevent free()ing memory addresses placed in the
 *   reserved area by the caller
 */
	if ((pnext = pwordexp->we_wordv) == (char **)0)
		return;
	if ((pwordexp->we_sflags & WRDE_DOOFFS) != 0)
		pnext += pwordexp->we_soffs;
/*
 * release the memory associated with each expanded word
 */
	while (*pnext != 0)
		free(*pnext++);
/*
 * release the memory associated with the expanded word list
 */
	free(pwordexp->we_wordv);
	return;
}
