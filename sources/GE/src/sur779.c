/********************************************************************/
/*                                                                  */
/*  This file is part of the VARKON Geometry Library.               */
/*  URL:  http://varkon.sourceforge.net                             */
/*                                                                  */
/*  This library is free software; you can redistribute it and/or   */
/*  modify it under the terms of the GNU Library General Public     */
/*  License as published by the Free Software Foundation; either    */
/*  version 2 of the License, or (at your option) any later         */
/*  version.                                                        */
/*                                                                  */
/*  This library is distributed in the hope that it will be         */
/*  useful, but WITHOUT ANY WARRANTY; without even the implied      */
/*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR         */
/*  PURPOSE.  See the GNU Library General Public License for more   */
/*  details.                                                        */
/*                                                                  */
/*  You should have received a copy of the GNU Library General      */
/*  Public License along with this library; if not, write to the    */
/*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge,        */
/*  MA 02139, USA.                                                  */
/*                                                                  */
/*  (C)Microform AB 1984-1999, Gunnar Liden, gunnar@microform.se    */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_ini_gmseg                     File: sur779.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initiates structure variabel DBSeg.                */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-03   Originally written                                 */
/*  1995-03-08   123456789 --> 12345                                */
/*  1996-01-14   F_UNDEF and I_UNDEF                                */
/*  1997-02-09   Elimination of compilation warnings                */
/*  1999-11-24   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_gmseg      Initialize variable DBSeg        */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_ini_gmseg (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSeg  *p_seg )       /* Surface coordinates & derivatives (ptr) */
/* Out:                                                             */
/*       Data to p_seg                                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Initialize variables                                          */
/* ______________________                                           */
/*                                                                  */
/*                                                                 !*/

/*  Segment coefficients                                            */
    p_seg->c0x=             F_UNDEF;     
    p_seg->c0y=             F_UNDEF;     
    p_seg->c0z=             F_UNDEF;     
    p_seg->c0 =             F_UNDEF;     
    p_seg->c1x=             F_UNDEF;     
    p_seg->c1y=             F_UNDEF;     
    p_seg->c1z=             F_UNDEF;     
    p_seg->c1 =             F_UNDEF;     
    p_seg->c2x=             F_UNDEF;     
    p_seg->c2y=             F_UNDEF;     
    p_seg->c2z=             F_UNDEF;     
    p_seg->c2 =             F_UNDEF;     
    p_seg->c3x=             F_UNDEF;     
    p_seg->c3y=             F_UNDEF;     
    p_seg->c3z=             F_UNDEF;     
    p_seg->c3 =             F_UNDEF;     

/*  Next segment (GM pointer)                                       */
    p_seg->nxt_seg=         DBNULL;        

/*  Offset                                                          */
    p_seg->ofs=             F_UNDEF;     

/*  Length of segment                                               */
    p_seg->sl=              F_UNDEF;     

/*  Type of segment                                                 */
    p_seg->typ=             I_UNDEF;

/*  Subtype of segment                                              */
    p_seg->subtyp=          I_UNDEF;

/*  Surface 1    (GM pointer)                                       */
    p_seg->spek_gm=         DBNULL;        

/*  Surface 2    (GM pointer)                                       */
    p_seg->spek2_gm=        DBNULL;        

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
