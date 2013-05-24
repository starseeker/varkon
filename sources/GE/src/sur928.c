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
/*  Function: varkon_sur_epts_ele                  File: sur928.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function eliminates equal iso-parameter segments defined    */
/*  in the EPOINT table.                                            */
/*                                                                  */
/*  Equal means that there are identical (in R*3 space) entry/exit  */
/*  points in another record, i.e. another patch.                   */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1996-04-27   Originally written                                 */
/*  1997-04-06   Comments added                                     */
/*  1999-11-20   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/********************************************************************/


/* --------- Short description of function ---------------------*/
/*                                                              */
/*sdescr varkon_sur_epts_ele   Eliminate equal EPOINT records   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------------- Flow chart ---------------------------------*/
/*               _______________________                            */
/*              !!                     !!                           */
/*              !! varkon_sur_epts_ele !!                           */
/*              !!     (sur928)        !!                           */
/*              !!_____________________!!                           */
/*         _________________!_____________________________          */
/*  ______!______   ______!______   ______!_______   _____!_____    */
/* !      1      !L!      2      ! !      3       ! !     4     !   */
/* ! Check input !a! Loop:     * ! ! Find curve   ! ! Printout  !   */
/* ! (for DEBUG) !b! All records ! ! branch start ! !  EPOINT   !   */
/* !_____________!e! in EPOINT   ! !   points     ! !(for DEBUG)!   */
/*                l!_____________! !______________! !___________!   */
/*                  ______!______    _____!_____     _____!_____    */
/*                r!           * !  !!         !!   !!         !!   */
/*                e! Loop:       !  !! cbranch !!   !! petable !!   */
/*                s! Point 1 & 2 !  !!_________!!   !!_________!!   */
/*                t! in record   !                                  */
/*                r!_____________!                                  */
/*                t       !                                         */
/*            ____________!____________                             */
/*           !                    o    !                            */
/*           ! If point not is linked: !                            */
/*           ! Compare point with all  !                            */
/*           ! (non-linked) points in  !                            */
/*           ! in the EPOINT table.    !                            */
/*           ! Update pointers when    !                            */
/*           ! point is equal or let   !                            */
/*           ! pointer be -1 (curve    !                            */
/*           ! branch start or end pt) !                            */
/*           !_________________________!                            */
/*                  _____!_____                                     */
/*                 !!         !!                                    */
/*                 !! compare !!                                    */
/*                 !!_________!!                                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short compare();       /* Compare current pt with EPOINT pts */
#ifdef  DEBUG
static short petable();       /* Printout of EPOINT                 */
#endif
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat  idpoint;    /* Identical point criterion            */
static DBint    i_r;        /* Loop index corresp. to EPOINT record */
static DBVector r3_1;       /* Current R3 pt 1 from table EPOINT    */
static DBVector r3_2;       /* Current R3 pt 2 from table EPOINT    */
static short    b_flag;     /* Boundary point flag                  */
                            /* Eq. 1: Two "normal" points           */
                            /* Eq. 2: Curve coincides iso-p.        */
                            /* Eq.-1: Points "used" in curve        */
                            /* Eq.-9: Do not use record !           */
                            /*        There are identical pts       */
                            /*        in another record !           */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_idpoint      * Identical points criterion       */
/*           varkon_lengv        * Length of a vector               */
/*           varkon_erpush       * Error message to terminal        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxxx failed in varkon_sur_epts_ele    */
/* SU2993 = Severe program error in varkon_sur_epts_ele (sur928).   */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus   varkon_sur_epts_ele (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EPOINT  *p_etable,    /* Entry/exit point table            (ptr) */
   DBint    no_ep )      /* Number of records in etable             */
/* Out:                                                             */
/*         - None -                                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */
   short  status;        /* Error code from a called function       */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data                                           */
/* ______________________                                           */
/*                                                                  */
/* For DEBUG: Check number of records (no_ep)                       */
/*                                                                 !*/

#ifdef DEBUG
if ( no_ep < 2 )
  {
  sprintf(errbuf,                         /* Program error SU2993:  */
  "(no_ep)%%varkon_sur_epts_ele(sur920"); /* no_ep < 2              */
  return(varkon_erpush("SU2993",errbuf)); /*                        */
  }
#endif

   idpoint   = varkon_idpoint();

/*!                                                                 */
/* 2. Eliminate equal isoparameter segments in EPOINT               */
/*                                                                 !*/

/*!                                                                 */
/* Loop all records in EPOINT table i_r= 1,...., no_ep              */
/*                                                                 !*/

for ( i_r=1; i_r<= no_ep; ++i_r )        /* Start loop EPOINT       */
  {

/*!                                                                 */
/*    Current entry/exit points to r3_1 and r3_2.                   */
/*                                                                  */
/*    Compare with all points in EPOINT and let pointers ptr1       */
/*    and ptr2 be -10 if segments are equal:                        */
/*    Call of compare.                                              */
/*                                                                 !*/
      r3_1.x_gm = (p_etable+  i_r - 1 )->curpt[0].r_x;
      r3_1.y_gm = (p_etable+  i_r - 1 )->curpt[0].r_y;
      r3_1.z_gm = (p_etable+  i_r - 1 )->curpt[0].r_z;
      r3_2.x_gm = (p_etable+  i_r - 1 )->curpt[1].r_x;
      r3_2.y_gm = (p_etable+  i_r - 1 )->curpt[1].r_y;
      r3_2.z_gm = (p_etable+  i_r - 1 )->curpt[1].r_z;
      b_flag    = (p_etable+  i_r - 1)->b_flag;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur928 r3_1 %f %f %f i_r    %d\n", 
     r3_1.x_gm,r3_1.y_gm,r3_1.z_gm,(int)i_r );            
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur928 r3_2 %f %f %f b_flag %d\n", r3_2.x_gm,r3_2.y_gm,r3_2.z_gm, b_flag );            
}
#endif

    if  ( b_flag == 2 )
      {
      status=compare(p_etable,no_ep);    /* Find a pt which is equal*/
      if (status<0) 
        {
        sprintf(errbuf,"compare%%varkon_sur_epts_ele (sur928)");
        return(varkon_erpush("SU2943",errbuf));
        }
      }

  }                                      /* End   loop EPOINT       */
/*!                                                                 */
/* End  all records in EPOINT       i_r=  1,....,no_ep              */
/*                                                                 !*/

/*!                                                                 */
/* 3. Printout of EPOINT table                                      */
/* ___________________________                                      */
/*                                                                  */
/* For DEBUG: Printout of the EPOINT table to SURPAC.DBG.           */
/*            Call of petable.                                      */
/*                                                                 !*/

#ifdef DEBUG
   status=petable(p_etable,no_ep);       /* Printout of EPOINT      */
   if (status<0) 
     {
     sprintf(errbuf,"petable%%varkon_sur_epts_ele (sur920)");
     return(varkon_erpush("SU2943",errbuf));
     }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/


/*!********* Internal ** function ** compare ************************/
/*                                                                  */

   static short compare(p_etable,no_ep)

/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function compares isoparameter curve segments in             */
/* table EPOINT and changes pointers ptr1 and ptr2 if               */
/* are equal.                                                       */
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   EPOINT  *p_etable;    /* Entry/exit point table            (ptr) */
   DBint    no_ep;       /* Number of records in etable             */
/* Out:                                                             */
/*-----------------------------------------------------------------!*/


/*!-------------------- Flow chart ---------------------------------*/
/*                   ___________                                    */
/*                  !!         !!                                   */
/*                  !! compare !!                                   */
/*                  !!_________!!                                   */
/*         _______________!______________________                   */
/*  ______!______   _______!_________   _________!__________        */
/* !      1      ! !       2         ! !         3          !       */
/* ! Initiations ! ! Find identical  ! ! Update input point !       */
/* !             ! ! point in EPOINT ! ! record if there is !       */
/* !_____________! !                 ! ! no identical point.!       */
/*                 !_________________! ! (a start/end curve !       */
/*                  _______!________   !  branch point)     !       */
/*                 !              * !  !____________________!       */
/*                 ! Loop:          !                               */
/*                 ! All remaining  !                               */
/*                 ! EPOINT records !                               */
/*                 !________________!                               */
/*                  _______!________                                */
/*                 !              * !                               */
/*                 ! Loop:          !                               */
/*                 ! Point 1 & 2 in !                               */
/*                 ! the record     !                               */
/*                 !________________!                               */
/*             ___________!___________                              */
/*   _________!_________    __________!___________                  */
/*  !                   !  !                      !                 */
/*  ! Retrieve pt from  !  ! Update records if    !                 */
/*  ! EPOINT. Calculate !  ! points are identical !                 */
/*  ! distance          !  !                      !                 */
/*  !___________________!  !______________________!                 */
/*           !________________                                      */
/*  _________!________    ____!____                                 */
/* !!                 !! !!       !!                                */
/* !! varkon_sur_eval !! !! lengv !!                                */
/* !!   (sur210)      !! !!_______!!                                */
/* !!_________________!!                                            */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    ic_r;        /* Loop index corresp. to record in EPOINT */
   DBfloat  d_11;        /* Distance between compared points  R*3   */
   DBfloat  d_22;        /* Distance between compared points  R*3   */
   DBfloat  d_12;        /* Distance between compared points  R*3   */
   DBfloat  d_21;        /* Distance between compared points  R*3   */
/*-----------------------------------------------------------------!*/

   DBfloat  v_d[3];      /* Vector for function lengv               */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Initializations                                               */
/* __________________                                               */
/*                                                                 !*/

/*!                                                                 */
/* 2. Determine if isoparameter segments are equal                  */
/* _______________________________________________                  */
/*                                                                  */
/* Loop remaining records in EPOINT ic_r= i_r+1, ... , no_ep        */
/*                                                                 !*/

for (ic_r=i_r+1; ic_r<= no_ep; ++ic_r)   /* Start loop EPOINT       */
  {

/*!                                                                 */
/*    Retrieve current entry/exit points from the EPOINT table      */
/*    and calculate the distances between the points.               */
/*    Calls of varkon_lengv (lengv).                                */
/*                                                                 !*/

      if ( (p_etable+ ic_r - 1)->b_flag != 2) goto nxtrec;

      v_d[0]=r3_1.x_gm-(p_etable+ic_r-1)->curpt[0].r_x;
      v_d[1]=r3_1.y_gm-(p_etable+ic_r-1)->curpt[0].r_y;
      v_d[2]=r3_1.z_gm-(p_etable+ic_r-1)->curpt[0].r_z;
      varkon_lengv(v_d,&d_11); 
      v_d[0]=r3_2.x_gm-(p_etable+ic_r-1)->curpt[1].r_x;
      v_d[1]=r3_2.y_gm-(p_etable+ic_r-1)->curpt[1].r_y;
      v_d[2]=r3_2.z_gm-(p_etable+ic_r-1)->curpt[1].r_z;
      varkon_lengv(v_d,&d_22);
      v_d[0]=r3_1.x_gm-(p_etable+ic_r-1)->curpt[1].r_x;
      v_d[1]=r3_1.y_gm-(p_etable+ic_r-1)->curpt[1].r_y;
      v_d[2]=r3_1.z_gm-(p_etable+ic_r-1)->curpt[1].r_z;
      varkon_lengv(v_d,&d_12);           /*                         */
      v_d[0]=r3_2.x_gm-(p_etable+ic_r-1)->curpt[0].r_x;
      v_d[1]=r3_2.y_gm-(p_etable+ic_r-1)->curpt[0].r_y;
      v_d[2]=r3_2.z_gm-(p_etable+ic_r-1)->curpt[0].r_z;
      varkon_lengv(v_d,&d_21);           /*                         */


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur928 compare ic_r= %d d_11 %f d_22 %f d_12 %f d_21 %f\n", 
    (int)ic_r, d_11, d_22, d_12, d_21  );
  }
#endif

     if       ( d_11 < idpoint && d_22 < idpoint )
       {
       (p_etable+ ic_r - 1)->b_flag = -9;
       }
     else if  ( d_11 < idpoint && d_22 < idpoint )
       {
       (p_etable+ ic_r - 1)->b_flag = -9;
       }

nxtrec:;                                 /* Label: Not b_flag= 2    */

  }                                      /* End   loop EPOINT       */

/*!                                                                 */
/* End remaining records in EPOINT  ic_r= i_r+1, ... , no_ep        */
/*                                                                 !*/

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/


#ifdef DEBUG
/*!********* Internal ** function ** petable ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function prints out the data in table EPOINT                 */

      static short petable(p_etable,no_ep)
/*                                                                  */
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   EPOINT  *p_etable;    /* Entry/exit point table            (ptr) */
   DBint    no_ep;       /* Number of records in etable             */
/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    ic_r;        /* Loop index corresp. to record in EPOINT */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Printout of input variables                                   */
/*                                                                 !*/

for (ic_r= 1; ic_r<= no_ep; ++ic_r)      /* Start loop EPOINT       */
  {

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "EPOINT record number:  %d \n", (int)ic_r);
     
  fprintf(dbgfil(SURPAC),
  "EPOINT u1= %20.15f v1= %20.15f     ptr1= %d\n",
     (p_etable+ic_r-1)->u1, (p_etable+ic_r-1)->v1,   
     (int)(p_etable+ic_r-1)->ptr1);
  fprintf(dbgfil(SURPAC),
  "EPOINT u2= %20.15f v2= %20.15f     ptr2= %d\n",
     (p_etable+ic_r-1)->u2, (p_etable+ic_r-1)->v2,   
     (int)(p_etable+ic_r-1)->ptr2);
  fprintf(dbgfil(SURPAC),
  "EPOINT b_flag %d d\n", (p_etable+ic_r-1)->b_flag);
  }


  }                                      /* End   loop EPOINT       */

    return(SUCCED);

} /* End of function                                                */
#endif
/********************************************************************/

