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
** COPYRIGHT (c) 1990 BY
** DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.
** ALL RIGHTS RESERVED.
**
** THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED
** ONLY  IN  ACCORDANCE  OF  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE
** INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER
** COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY
** OTHER PERSON.  NO TITLE TO AND  OWNERSHIP OF THE  SOFTWARE IS  HEREBY
** TRANSFERRED.
**
** THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE
** AND  SHOULD  NOT  BE  CONSTRUED  AS A COMMITMENT BY DIGITAL EQUIPMENT
** CORPORATION.
**
** DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS
** SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.
*/

/*
**++
**  Subsystem:
**	MEMEX HyperInformation Services
**
**  Version: V1.0
**
**  Abstract:
**	MEMEX Hyperapplication Registry definitions
**
**  Keywords:
**	{@keyword-list-or-none@}
**
**  Environment:
**	{@environment-description@}
**
**  Author:
**	Doug Rayner, MEMEX Project
**	W. Ward Clark, MEMEX Project
**
**  Creation Date: 16-Mar-1990
**
**  Modification History:
**--
*/

/*
**  Include Files
*/

/*
**  Macro Definitions
*/

#define _AnyTypeIdentifier "Any"
#define _AnnotateOperationId lwk_c_annotate_op_id

/*
**  Type Definitions
*/

/*
**  External Routine Declarations
*/

_DeclareFunction(_DDIFString LwkRegTypeName,
    (_String type_id));
_DeclareFunction(int LwkRegTypeOperations,
    (_String type_id, _String **operation_ids));
_DeclareFunction(_String LwkRegTypeDefaultOperation,
    (_String type_id));
_DeclareFunction(_Boolean LwkRegIsSuperType,
    (_String type_id, _String supertype_id));
_DeclareFunction(_DDIFString LwkRegOperationName,
    (_String type_id, _String operation_id));
_DeclareFunction(_String LwkRegOperationType,
    (_String type_id, _String operation_id));
_DeclareFunction(_String LwkRegOperationMethod,
    (_String type_id, _String operation_id));