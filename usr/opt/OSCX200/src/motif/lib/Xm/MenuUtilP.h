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
/*
 * @(#)$RCSfile: MenuUtilP.h,v $ $Revision: 1.1.4.2 $ (DEC) $Date: 1993/05/06 15:42:15 $
 */
/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2.1
*/ 
/*   $RCSfile: MenuUtilP.h,v $ $Revision: 1.1.4.2 $ $Date: 1993/05/06 15:42:15 $ */
/*
*  (c) Copyright 1989, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS. */
#ifndef _XmMenuUtilP_h
#define _XmMenuUtilP_h
#if defined(VMS) || defined (__VMS)
#include <X11/apienvset.h>
#endif

#include <Xm/XmP.h>
#include <Xm/RowColumnP.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _XmTranslRec
{
  XtTranslations translations;
  struct _XmTranslRec * next;
};


/********    Private Function Declarations    ********/
#ifdef _NO_PROTO

extern Boolean _XmIsActiveTearOff ();
extern Widget _XmGetRC_PopupPosted ();
extern Boolean _XmGetInDragMode() ;
extern void _XmSetInDragMode() ;
extern void _XmSaveMenuProcContext() ;
extern XtPointer _XmGetMenuProcContext() ;
extern int _XmGrabPointer() ;
extern int _XmGrabKeyboard() ;
extern void _XmMenuSetInPMMode() ;
extern void _XmSetMenuTraversal() ;
extern void _XmLeafPaneFocusOut() ;
extern void _XmMenuTraverseLeft() ;
extern void _XmMenuTraverseRight() ;
extern void _XmMenuTraverseUp() ;
extern void _XmMenuTraverseDown() ;
extern void _XmMenuEscape() ;
extern void _XmRC_GadgetTraverseDown() ;
extern void _XmRC_GadgetTraverseUp() ;
extern void _XmRC_GadgetTraverseLeft() ;
extern void _XmRC_GadgetTraverseRight() ;
extern void _XmMenuTraversalHandler() ;
extern void _XmSaveCoreClassTranslations() ;
extern void _XmRestoreCoreClassTranslations() ;
extern XmMenuState _XmGetMenuState() ;

#else

extern Boolean _XmIsActiveTearOff (
                         Widget w) ;
extern Widget _XmGetRC_PopupPosted (
                         Widget wid) ;
extern Boolean _XmGetInDragMode( 
                        Widget widget) ;
extern void _XmSetInDragMode( 
                        Widget widget,
#if NeedWidePrototypes
                        int mode) ;
#else
                        Boolean mode) ;
#endif /* NeedWidePrototypes */
extern void _XmSaveMenuProcContext( 
                        XtPointer address) ;
extern XtPointer _XmGetMenuProcContext( void ) ;
extern int _XmGrabPointer( 
                        Widget widget,
                        int owner_events,
                        unsigned int event_mask,
                        int pointer_mode,
                        int keyboard_mode,
                        Window confine_to,
                        Cursor cursor,
                        Time time) ;
extern int _XmGrabKeyboard( 
                        Widget widget,
                        int owner_events,
                        int pointer_mode,
                        int keyboard_mode,
                        Time time) ;
extern void _XmMenuSetInPMMode( 
			Widget wid,
#if NeedWidePrototypes
                        int flag) ;
#else
                        Boolean flag) ;
#endif /* NeedWidePrototypes */
extern void _XmSetMenuTraversal( 
                        Widget wid,
#if NeedWidePrototypes
                        int traversalOn) ;
#else
                        Boolean traversalOn) ;
#endif /* NeedWidePrototypes */
extern void _XmLeafPaneFocusOut( 
                        Widget wid) ;
extern void _XmMenuTraverseLeft( 
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
extern void _XmMenuTraverseRight( 
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
extern void _XmMenuTraverseUp( 
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
extern void _XmMenuTraverseDown( 
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
extern void _XmMenuEscape( 
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
extern void _XmRC_GadgetTraverseDown( 
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
extern void _XmRC_GadgetTraverseUp( 
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
extern void _XmRC_GadgetTraverseLeft( 
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
extern void _XmRC_GadgetTraverseRight( 
                        Widget wid,
                        XEvent *event,
                        String *param,
                        Cardinal *num_param) ;
extern void _XmMenuTraversalHandler( 
                        Widget w,
                        Widget pw,
                        XmTraversalDirection direction) ;
extern void _XmSaveCoreClassTranslations( 
                        Widget widget) ;
extern void _XmRestoreCoreClassTranslations( 
                        Widget widget) ;
extern XmMenuState _XmGetMenuState(
                        Widget widget) ;

#endif /* _NO_PROTO */
/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#if defined(VMS) || defined (__VMS)
#include <X11/apienvrst.h>
#endif
#endif  /* _XmMenuUtilP_h */
/* DON'T ADD STUFF AFTER THIS #endif */