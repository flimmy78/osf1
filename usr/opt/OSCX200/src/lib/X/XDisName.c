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
/* $XConsortium: XDisName.c,v 11.6 91/05/11 17:03:04 gildea Exp $ */

/*
Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of M.I.T. not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission.  M.I.T. makes no representations about the
suitability of this software for any purpose.  It is provided "as is"
without express or implied warranty.
*/

/* XDisplayName.c */
/* 
 * Returns the name of the display XOpenDisplay would use.  This is better
 * than just printing the "display" variable in a program because that
 * could be NULL and/or there could be an environment variable set.
 * This makes it easier for programmers to provide meaningful error
 * messages. 
 *
 * 
 * For example, this is used in XOpenDisplay() as
 *	strncpy( displaybuf, XDisplayName( display ), sizeof(displaybuf) );
 *      if ( *displaybuf == '\0' ) return( NULL );
 *  This check is actually unnecessary because the next thing is an index()
 *  call looking for a ':' which will fail and we'll return(NULL).
 */
/* Written at Waterloo - JMSellens */

#include <stdio.h>

extern char *getenv();

char *
XDisplayName( display )
    char *display;
{
    char *d;
    if ( display != (char *)NULL && *display != '\0' )
	return( display );
    if ( (d = getenv( "DISPLAY" )) != (char *)NULL )
	return( d );
    return( "" );
}
