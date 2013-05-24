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
/*  Function: varkon_ini_pbound      SNAME: sur774 File: sur774.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function initializes structure variabel PBOUND.             */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-03   Originally written                                 */
/*  1996-01-07   sur_no added                                       */
/*  1999-11-20   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_ini_bbox();      * Initialize BBOX              */
/*           varkon_ini_bcone();     * Initialize BCONE             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_ini_pbound     Initialize variable PBOUND       */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
      DBstatus    varkon_ini_pbound(

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   PBOUND *p_pb)         /* Surface creation pt and derivat.  (ptr) */
/* Out:                                                             */
/*       Data to p_pb                                               */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Initialization of variables                                   */
/* _____________________________                                    */
/*                                                                  */
/*                                                                 !*/

/*  Record number                                                   */
    p_pb->rec_no = I_UNDEF;    

/*  Patch type                                                      */
    p_pb->ptype  = I_UNDEF;    

/*  U and V for patch start and end points                          */
    p_pb->us=   F_UNDEF;
    p_pb->vs=   F_UNDEF;
    p_pb->ue=   F_UNDEF;
    p_pb->ve=   F_UNDEF;

/*  Number of solutions                                             */
    p_pb->nu0    = I_UNDEF;    
    p_pb->nu1    = I_UNDEF;    
    p_pb->nv0    = I_UNDEF;    
    p_pb->nv1    = I_UNDEF;    

/*  Data from surface number 1 or 2 (surface/surface interrogat.)   */
    p_pb->sur_no = I_UNDEF;    

/*!                                                                 */
/*    Initialize variable BBOX                                      */
/*    Call of varkon_ini_bbox  (sur772).                            */
/*                                                                 !*/

      varkon_ini_bbox (&p_pb->pbox ); 

/*!                                                                 */
/*    Initialize variable BCONE                                     */
/*    Call of varkon_ini_bcone (sur773).                            */
/*                                                                 !*/

      varkon_ini_bcone (&p_pb->pcone);


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
