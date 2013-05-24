/********************************************************************/
/*                                                                  */
/*  This file is part of the VARKON Geometry Library.               */
/*  URL:  http://www.varkon.com                                     */
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
/*  Function: varkon_sur_poiadr                    File: sur297.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the address (pointer) to a              */
/*  point in table SURPOI. Input is the point address in            */
/*  the point grid (a rectangular grid).                            */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-05   Originally written                                 */
/*  1996-05-28   Eliminated compilation warning                     */
/*  1997-06-05   Initialization of output variables                 */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_poiadr     Record number for pt in POISUR   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error (  ) in varkon_sur_poiadr          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/

     DBstatus         varkon_sur_poiadr (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBint    iuc,          /* Current address to corner point in file */
  DBint    ivc,          /* Current address to corner point in file */
  SURPOI  *p_surpoi,     /* Point table SURPOI (alloc. area)  (ptr) */
  DBint    nu_poi,       /* Number of points in U direction   (ptr) */
  DBint    nv_poi,       /* Number of points in V direction   (ptr) */
  DBint    iu_s,         /* Start U point                           */
  DBint    iv_s,         /* Start V point                           */
  DBint    iu_e,         /* End   U point                           */
  DBint    iv_e,         /* End   V point                           */
  DBint    f_p,          /* Flag ....    Eq:-1: No Eq 1: U Eq 2: V  */
  DBint   *p_recno,      /* Record number in table SURPOI     (ptr) */
  DBint   *p_dflag )     /* Eq. 1: Pt defined Eq. -1: Not def.(ptr) */
/* Out:                                                             */
/*                                                                  */
/*      Data to p_recno                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

  SURPOI *p_p;           /* Current record in POISUR          (ptr) */
  char   errbuf[80];     /* String for error message fctn erpush    */

#ifdef  DEBUG
  EVALS  xyz;            /* Point for Debug printout                */
#endif

/*!------------------ Theory ---------------------------------------*/
/*                                                                  */
/*                                                                  */
/*         Diagram showing relation between                         */
/*         SURPOI record no and point number                        */
/*                                                                  */
/* nv_poi=  4   +4  +8  +   +   +   +   +   +   +36                 */
/*                                                                  */
/*          3   +3  +7  +   +   +   +   +   +   +35                 */
/*                                                                  */
/*          2   +2  +6  +   +   +   +   +   +   +34                 */
/*                                                                  */
/*  ivc=    1   +1  +5  +9  +   +   +   +   +   +34                 */
/*                                                                  */
/*         iuc= 1   2   3   4   5   6   7   8   9  =nu_poi          */
/*                                                                  */
/*                                                                  */
/*   Record number if surface not is closed:                        */
/*   p_recno= (iuc-1)*nv_poi + ivc                                  */
/*                                                                  */
/*   Record number if surface is V closed and ivc > nv_poi          */
/*   p_recno= (iuc-1)*nv_poi + iv_s                                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/



/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* ___________________________________________                      */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur297 Enter***varkon_sur_poiadr* iuc %d ivc %d\n",iuc,ivc);
}
#endif
 

/* Initalization of output variables                                */

   *p_recno = I_UNDEF;
   *p_dflag = I_UNDEF;


if     ( iuc >= 1 && iuc <= nu_poi && ivc >= 1 && ivc <= nv_poi )  
   {
/*!                                                                 */
/* Let record address in SURPOI be:                                 */
   *p_recno= (iuc-1)*nv_poi+ivc;
/* if point 1 <= iuc <= nu_poi and 1 <= ivc <= nv_poi               */
/*                                                                 !*/
   }
else if( iuc >= 1 && iuc <= nu_poi && ivc > nv_poi && f_p == 2 )  
   {
/*!                                                                 */
/* Let record address in SURPOI be:                                 */
   *p_recno= (iuc-1)*nv_poi+iv_s;
/* if point 1 <= iuc <= nu_poi, ivc > nv_poi, surface V closed      */
/*                                                                 !*/
   }
else if( ivc >= 1 && ivc <= nv_poi && iuc > nu_poi && f_p == 1 )  
   {
/*!                                                                 */
/* Let record address in SURPOI be:                                 */
   *p_recno= (iu_s-1)*nv_poi+ivc;
/* if point 1 <= ivc <= nv_poi, iuc > nu_poi, surface U closed      */
/*                                                                 !*/
   }
else
   {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur297 Failure iuc %d ivc %d nu_poi %d nv_poi %d f_p %d\n",
    iuc ,  ivc ,  nu_poi ,  nv_poi, f_p );
  }
#endif
 sprintf(errbuf, "(address)%%varkon_sur_poiadr");
 return(varkon_erpush("SU2993",errbuf));
 }


/*!                                                                 */
/* 2. Check if point is defined                                     */
/* ____________________________                                     */
/*                                                                  */
/*                                                                 !*/

  p_p= p_surpoi + *p_recno - 1;

  if ( p_p->iu< 0 ||  p_p->iv < 0 )      /* Point not defined       */
     {                                   /*                         */
     *p_dflag = -1;                      /*                         */
     }                                   /*                         */
  else                                   /* Point is  defined       */
     {                                   /*                         */
     *p_dflag =  1;                      /*                         */
     }                                   /*                         */

#ifdef DEBUG                             /* Debug printout          */
    xyz       = p_p->spt;                /* xyz from SURPOI         */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur297 xyz   %f %f %f p_p %d\n",
                   xyz.r_x,xyz.r_y,xyz.r_z , p_p ); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur297 Exit*varkon_sur_poiadr iu %d iv %d *p_recno %d *p_dflag %d\n",
            p_p->iu,p_p->iv, *p_recno ,*p_dflag );
  fflush(dbgfil(SURPAC)); 
  }
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/
