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
/*  Function: varkon_sur_epts_ord                  File: sur922.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function determines the order of the entry/exit points      */
/*  in the EPOINT table for all curve branches.                     */
/*                                                                  */
/*  A new, reordered EPOINT table will not be created. The order    */
/*  of the records are defined by two pointers in each record.      */
/*  The entry point pointer points to the previous record (patch)   */
/*  and the exit point pointer points to the next record (patch).   */
/*  (The linking technique is used).                                */
/*                                                                  */
/*  Output is also the start record numbers for all the curve       */
/*  branches.                                                       */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-11   Originally written                                 */
/*  1996-02-13   Debug                                              */
/*  1996-04-27   sur928 added                                       */
/*  1997-04-05   Closed curve in a closed surface                   */
/*  1997-04-07   Records with b_flag= -9 not handled, TOL2->idpoint */
/*  1999-11-20   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/********************************************************************/


/* --------- Short description of function ---------------------*/
/*                                                              */
/*sdescr varkon_sur_epts_ord   Order & find branches in EPOINT  */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------------- Flow chart ---------------------------------*/
/*               _______________________                            */
/*              !!                     !!                           */
/*              !! varkon_sur_epts_ord !!                           */
/*              !!     (sur922)        !!                           */
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

/*!New-Page--------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short compare();       /* Compare current pt with EPOINT pts */
static short chelink();       /* Check that all records are linked  */
static short oneclo();        /* The output is one closed curve     */
static short cbranch();       /* Finds curve branches start records */
#ifdef  DEBUG
static short petable();       /* Printout of EPOINT                 */
#endif
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat idpoint;     /* Identical point criterion            */
static DBint    i_r;        /* Loop index corresp. to EPOINT record */
static DBint    i_p;        /* Loop index corresp. to u1,v1 & u2,v2 */
static DBfloat  u_st,v_st;  /* The current UV pt from table EPOINT  */
static DBVector r3_st;      /* The current R3 pt from table EPOINT  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_idpoint          * Identical points criterion   */
/*           varkon_sur_epts_ele     * Eliminate equal iso-segments */
/*           varkon_lengv            * Length of a vector           */
/*           varkon_erpush           * Error message to terminal    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxxx failed in varkon_sur_epts_ord    */
/* SU2993 = Severe program error in varkon_sur_epts_ord (sur922).   */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus   varkon_sur_epts_ord (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EPOINT  *p_etable,    /* Entry/exit point table            (ptr) */
   DBint    no_ep,       /* Number of records in etable             */
   DBint   *p_no_br,     /* Number of curve branches          (ptr) */
   DBint    cbsr[] )     /* Curve branches start records (CBMAX)    */
/* Out:                                                             */
/*         - None -                                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  ptr;           /* Pointer ptr1 or ptr2  from EPOINT       */
   DBint  b_flag;        /* Boundary point flag                     */
                         /* Eq. 1: Two "normal" points              */
                         /* Eq. 2: Curve coincides iso-p.           */
                         /* Eq.-1: Points "used" in curve           */
                         /* Eq.-9: Do not use record !              */
                         /*        There are identical pts          */
                         /*        in another record !              */
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
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/* For DEBUG: Check number of records (no_ep)                       */
/*                                                                 !*/

#ifdef DEBUG
if ( no_ep < 2 )
  {
  sprintf(errbuf, "(no_ep < 2)%%sur922");
  return(varkon_erpush("SU2993",errbuf)); /*                        */
  }
#endif

/*!                                                                 */
/* Retrieve identical points criterion.                             */
/* Call of varkon_idpoint (sur741).                                 */
/*                                                                 !*/

   idpoint   = varkon_idpoint();

  idpoint = 3.0*idpoint;  /* Corresponds to previuos value 300*TOL2 */

  ptr = I_UNDEF;


/*!                                                                 */
/* 2. Eliminate equal iso-parametric curve segments                 */
/* ________________________________________________                 */
/*                                                                  */
/*                                                                 !*/

    status = varkon_sur_epts_ele
    (p_etable,no_ep); 
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_epts_ele%%sur922");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

/*!                                                                 */
/* 3. Link records in EPOINT                                        */
/* _________________________                                        */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/*                                                                  */
/* Loop all records in EPOINT table i_r= 1,...., no_ep              */
/*    Loop points in record         i_p= 1,2                        */
/*                                                                  */
/*                                                                 !*/

for ( i_r=1; i_r<= no_ep; ++i_r )        /* Start loop EPOINT       */
  {
  for ( i_p=1; i_p<= 2;     ++i_p )      /* Start loop u1,v1-u2,v2  */
    {

/*!                                                                 */
/*    Current entry/exit point to u_st,v_st, r3_st and              */
/*    corresponding pointer to ptr.                                 */
/*                                                                  */
/*    Compare with all points in EPOINT and update                  */
/*    pointer(s) if current point not is linked:                    */
/*    Call of compare if ptr= 0 (not linked).                       */
/*                                                                 !*/
    b_flag      = (p_etable+  i_r - 1 )->b_flag;
    if      ( i_p == 1 )
      {
      u_st      = (p_etable+  i_r - 1 )->u1;
      v_st      = (p_etable+  i_r - 1 )->v1;
      r3_st.x_gm= (p_etable+  i_r - 1 )->curpt[0].r_x;
      r3_st.y_gm= (p_etable+  i_r - 1 )->curpt[0].r_y;
      r3_st.z_gm= (p_etable+  i_r - 1 )->curpt[0].r_z;
      ptr       = (p_etable+  i_r - 1 )->ptr1;
      }
    else if ( i_p == 2 )
      {
      u_st      = (p_etable+  i_r - 1 )->u2;
      v_st      = (p_etable+  i_r - 1 )->v2;
      r3_st.x_gm= (p_etable+  i_r - 1 )->curpt[1].r_x;
      r3_st.y_gm= (p_etable+  i_r - 1 )->curpt[1].r_y;
      r3_st.z_gm= (p_etable+  i_r - 1 )->curpt[1].r_z;
      ptr       = (p_etable+  i_r - 1 )->ptr2;
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
      fprintf(dbgfil(SURPAC),
      "sur922 i_r= %d i_p= %d ptr= %d u_st= %f v_st= %f \n", 
       (int)i_r,(int)i_p, (int)ptr, u_st , v_st );            
  }
#endif

/* 1996-04-27    if ( ptr == 0 )  */     /* Point is not linked     */
    if ( ptr == 0 && b_flag != -9 )      /* Point is not linked     */
      {
      status=compare(p_etable,no_ep);    /* Find a pt which is equal*/
      if (status<0) 
        {
        sprintf(errbuf,"compare%%sur922");
        return(varkon_erpush("SU2943",errbuf));
        }
      }

    }                                    /* End   loop u1,v1-u2,v2  */
  }                                      /* End   loop EPOINT       */
/*!                                                                 */
/*    End points in record          i_p= 1,2                        */
/* End  all records in EPOINT       i_r=  1,....,no_ep              */
/*                                                                 !*/

/*!                                                                 */
/* Check that all records have been linked.                         */
/* Call of chelink.                                                 */
/*                                                                 !*/

   status=chelink(p_etable,no_ep); 
   if (status<0) 
     {
     sprintf(errbuf,"chelink%%sur922");
     return(varkon_erpush("SU2943",errbuf));
     }


/*!                                                                 */
/* 4. Find curve branch start points                                */
/* _________________________________                                */
/*                                                                  */
/* The curve branch start and end points have pointers              */
/* equal to -1. Find these points in the EPOINT table               */
/* and pick out (choose) the start points.                          */
/* Call of cbranch.                                                 */
/*                                                                 !*/

   status=cbranch(p_etable,no_ep,p_no_br,cbsr); 
   if (status<0) 
     {
     sprintf(errbuf,"cbranch%%sur922");
     return(varkon_erpush("SU2943",errbuf));
     }

/*!                                                                 */
/* 5. Printout of EPOINT table                                      */
/* ___________________________                                      */
/*                                                                  */
/* For DEBUG: Printout of the EPOINT table to SURPAC.DBG.           */
/*            Call of petable.                                      */
/*                                                                 !*/

#ifdef DEBUG
   status=petable(p_etable,no_ep);       /* Printout of EPOINT      */
   if (status<0) 
     {
     sprintf(errbuf,"petable%%sur922");
     return(varkon_erpush("SU2943",errbuf));
     }
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur922 Exit*varkon_sur_epts_ord No of curve branches= %d\n",
     (int)*p_no_br );
  fflush(dbgfil(SURPAC)); 
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
/* The function compares an input point with all points in          */
/* table EPOINT and update (linking) pointers in records.           */
/*                                                                  */
/* The input point is an un-linked point from EPOINT. The           */
/* distance between this point and all other (not yet linked)       */
/* points will be calculated until an identical point is found.     */
/* The (linking) pointers for the input point and the identical     */
/* point will be updated, e.g. two EPOINT records will be updated.  */
/*                                                                  */
/* If there is no identical point will only one EPOINT record be    */
/* updated. The (linking) pointer for the input point will be       */
/* set to -1. The -1 value corresponds to a start or end point      */
/* of a curve branch.                                              !*/
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


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    ic_r;        /* Loop index corresp. to record in EPOINT */
   DBint    ic_p;        /* Loop index corresp. to u1,v1-u2,v2      */
   DBfloat  dist;        /* Distance between compared points  R*3   */
   DBfloat  uvdist;      /* Distance between compared points  UV    */
/*-----------------------------------------------------------------!*/

   DBint    b_flag;      /* Boundary point flag                     */
                         /* Eq. 1: Two "normal" points              */
                         /* Eq. 2: Curve coincides iso-p.           */
                         /* Eq.-1: Points "used" in curve           */
                         /* Eq.-9: Do not use record !              */
                         /*        There are identical pts          */
                         /*        in another record !              */
   short    eflag;       /* End flag =1 for dist=0                  */
   DBfloat  v_d[3];      /* Vector for function lengv               */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Initiations                                                   */
/* ______________                                                   */
/*                                                                  */
/* End point flag (eflag= 1 corresponds to dist=0):                 */
   eflag= 0;
/*                                                                 !*/

/*!                                                                 */
/* 2. Find identical point in table EPOINT                          */
/* _______________________________________                          */
/*                                                                  */
/* Loop remaining records in EPOINT ic_r= i_r+1, ... , no_ep        */
/*    Loop points in record         ic_p= 1,2                       */
/*                                                                 !*/

for (ic_r=i_r+1; ic_r<= no_ep; ++ic_r)   /* Start loop EPOINT       */
  {
  b_flag      = (p_etable+  ic_r - 1 )->b_flag;
  if ( b_flag == -9 )goto nxtrec;  /* Patch with equal iso- segment */

  for ( ic_p=1; ic_p<= 2; ++ic_p )       /* Start loop u1,v1-u2,v2  */
    {

/*!                                                                 */
/*    Retrieve current entry/exit point from the EPOINT table       */
/*    and calculate the distance to the input point.                */
/*    Calls of varkon_lengv (lengv).                                */
/*                                                                 !*/

    if      ( ic_p == 1 )
      {
      v_d[0]=(p_etable+ic_r-1)->u1-u_st; /* Distance to u1,v1       */
      v_d[1]=(p_etable+ic_r-1)->v1-v_st; /*                         */
      v_d[2]= 0.0;                       /*                         */
      varkon_lengv(v_d,&uvdist);         /*                         */
      }
    else if ( ic_p == 2 )
      {
      v_d[0]=(p_etable+ic_r-1)->u2-u_st; /* Distance to u2,v2       */
      v_d[1]=(p_etable+ic_r-1)->v2-v_st; /*                         */
      v_d[2]= 0.0;                       /*                         */
      varkon_lengv(v_d,&uvdist);         /*                         */
      }

    if      ( ic_p == 1 )
      {
      v_d[0]=r3_st.x_gm-(p_etable+ic_r-1)->curpt[0].r_x;
      v_d[1]=r3_st.y_gm-(p_etable+ic_r-1)->curpt[0].r_y;
      v_d[2]=r3_st.z_gm-(p_etable+ic_r-1)->curpt[0].r_z;
      varkon_lengv(v_d,&dist);           /*                         */
      }
    else if ( ic_p == 2 )
      {
      v_d[0]=r3_st.x_gm-(p_etable+ic_r-1)->curpt[1].r_x;
      v_d[1]=r3_st.y_gm-(p_etable+ic_r-1)->curpt[1].r_y;
      v_d[2]=r3_st.z_gm-(p_etable+ic_r-1)->curpt[1].r_z;
      varkon_lengv(v_d,&dist);          /*                         */
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && dist <  100*idpoint  )
  {
  fprintf(dbgfil(SURPAC),
  "sur922 compare i_r= %d i_p= %d ic_r= %d ic_p= %d dist= %f\n", 
    (int)i_r,(int)i_p,(int)ic_r,(int)ic_p, dist );
  }
  if ( dist  >= TOL2-TOL1 && dist  < 5000.0*TOL2 )
/* if ( dist  >= 0.0       && dist  < 1000.0*TOL2 )   */
{
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur922 Distance in R*3 dist=  %f Distance in UV uvdist=  %f\n", 
      dist , uvdist );
  }
}
#endif

/*!                                                                 */
/*    For identical points (if dist < idpoint)                      */
/*        i. Update the record of the current (ic_p) point          */
/*           Let ptr1= i_r= input pt record  if ic_p=1              */
/*           ( or let ptr2= i_r if ic_p=2)                          */
/*       ii. Update the record of the input (i_r) point             */
/*           Let ptr1= ic_r= identical pt record  if i_p=1          */
/*           ( or let ptr2= ic_r if i_p=2)                          */
/*      iii. Exit from loop                                         */
/*           Let eflag= 1 (and let ic_r=no_ep and ic_p=2)           */
/*                                                                 !*/

/*   if (dist<=300*TOL2 && uvdist<1.0)*/ /* 1997-04-04              */
     if (dist<=idpoint               )   /* Identical points ?      */
       {                                 /* Start identical points  */
       if      ( ic_p  == 1 )            /* Update the record of    */
         {                               /* the identical point     */
         (p_etable+ic_r-1)->ptr1=i_r;    /*                         */
         }                               /*                         */
       else if ( ic_p  == 2 )            /*                         */
         {                               /*                         */
         (p_etable+ic_r -1)->ptr2=i_r;   /*                         */
         }                               /*                         */
       if      ( i_p  == 1 )             /*                         */
         {                               /* Update the record of the*/
         (p_etable+i_r -1)->ptr1=ic_r;   /* current pt u_st,v_st    */
         }                               /*                         */
       else if ( i_p  == 2 )             /*                         */
         {                               /*                         */
         (p_etable+i_r -1)->ptr2=ic_r;   /*                         */
         }                               /*                         */
         ic_r= no_ep;                    /* "Make it" end of loop   */
         ic_p= 2;                        /*                         */
         eflag=1;                        /* End flag = 1            */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
         {
         fprintf(dbgfil(SURPAC),
         "sur922*compare i_r = %d ptr1= %d ptr2= %d i_p  %d Linked pt\n", 
           (int)i_r, (int)(p_etable+i_r-1)->ptr1 
                    ,(int)(p_etable+i_r-1)->ptr2 ,(int)i_p );
         fprintf(dbgfil(SURPAC),
         "sur922*compare i_cr= %d ptr1= %d ptr2= %d ic_p %d Linked pt\n", 
           (int)ic_r,(int)(p_etable+ic_r-1)->ptr1 
                    ,(int)(p_etable+ic_r-1)->ptr2 ,(int)ic_p );
         }
#endif
#ifdef  DEBUG_TEMPORY          
         petable(p_etable,no_ep);        /* Printout of EPOINT      */
#endif

       }                                 /* End   identical points  */


    }                                    /* End   loop u1,v1-u2,v2  */
nxtrec:;                                 /* Label: b_flag = -9      */
  }                                      /* End   loop EPOINT       */

/*!                                                                 */
/*    End points in record          ic_p= 1,2                       */
/* End remaining records in EPOINT  ic_r= i_r+1, ... , no_ep        */
/*                                                                 !*/

/*!                                                                 */
/* 3. Start or end point of curve branch                            */
/* _____________________________________                            */
/*                                                                  */
/* Let the pointer of the input point be -1 if there is no          */
/* identical point in table EPOINT.                                 */
/* If end flag eflag= 0 :                                           */
/*    i. Update the record of the input (i_r) point                 */
/*       Let ptr1= -1 if i_p=1 or                                   */
/*       let ptr2= -1 if i_p=2                                      */

/*                                                                 !*/

  if ( eflag == 0  )                     /* No identical point      */
    {                                    /* Start of curve branch   */
    if      ( i_p  == 1 )                /*                         */
      {                                  /* Update the record of the*/
      (p_etable+i_r -1)->ptr1= -1;       /* current pt u_st,v_st    */
      }                                  /*                         */
    else if ( i_p  == 2 )                /*                         */
      {                                  /*                         */
      (p_etable+i_r -1)->ptr2= -1;       /*                         */
      }                                  /*                         */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
    {
    fprintf(dbgfil(SURPAC),
    "sur922*compare i_r = %d ptr1= %d ptr2= %d i_p  %d Unlinked pt\n", 
      (int)i_r, (int)(p_etable+i_r-1)->ptr1 
               ,(int)(p_etable+i_r-1)->ptr2, (int)i_p );
    }
#endif
    }   /* End: eflag = 0                         */

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
/* 1. Printout of all data in EPOINT                                */
/*                                                                 !*/

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), " \n");
  fprintf(dbgfil(SURPAC),
  "sur922*petable    Table EPOINT Number of records no_ep= %d\n", 
          (int)no_ep );
  fprintf(dbgfil(SURPAC), 
  "============================================================\n");
  fprintf(dbgfil(SURPAC), " \n");
  }
     
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
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "EPOINT branch= %d b_flag= %d\n", (int)(p_etable+ic_r-1)->branch,
                                    (int)(p_etable+ic_r-1)->b_flag);
  }


  }                                      /* End   loop EPOINT       */

    return(SUCCED);

} /* End of function                                                */
#endif
/********************************************************************/


/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!********* Internal ** function ** cbranch ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function finds the curve branches start (and end) records.   */

    static short cbranch(p_etable,no_ep,p_no_br,cbsr)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   EPOINT  *p_etable;    /* Entry/exit point table            (ptr) */
   DBint    no_ep;       /* Number of records in etable             */
   DBint   *p_no_br;     /* Number of curve branches          (ptr) */
   DBint    cbsr[];      /* Curve branches start records            */
/* Out:                                                             */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/

/*!-------------------- Flow chart ---------------------------------*/
/*                   ___________                                    */
/*                  !!         !!                                   */
/*                  !! cbranch !!                                   */
/*                  !!_________!!                                   */
/*   _____________________!____                                     */
/*  !    ____!_____     _______!_________                           */
/* !1!  !    2     !   !       3         !                          */
/* !I!  ! Find all !   ! Select curve    !                          */
/* !n!  !start/end !   ! branch start    !                          */
/* !i!  ! points   !   !    points       !                          */
/* !t!  !__________!   !_________________!                          */
/* !i!   ____!______    _______!________                            */
/* !a!  !           !  !              * !                           */
/* !t!  !Loop:      !  ! Loop: All      !                           */
/* !i!  !All EPOINT !  ! records (pts)  !                           */
/* !o!  ! records   !  !    in a_b()    !                           */
/* !n!  !___________!  !________________!                           */
/* !s!   ____!______           !                                    */
/* !_!  !           !          !                                    */
/*      !Loop:      !          !                                    */
/*      !Point 1 & 2!          !                                    */
/*      ! in record !          !                                    */
/*      !___________!          !                                    */
/*       _____!_____           !                                    */
/*      !           !          !                                    */
/*      ! record no !          !                                    */
/*      ! to a_b()  !          !                                    */
/*      ! if ptr is !          !                                    */
/*      !   -1      !          !                                    */
/*      !___________!          !                                    */
/*                             !                                    */
/*             ________________!______                              */
/*   _________!______       __________!___________                  */
/*  !                ! L   !                      !                 */
/*  ! Start point if ! a   !                      !                 */
/*  ! ptr not is -10 ! b   !                      !                 */
/*  ! distance       ! e   !                      !                 */
/*  !________________! l   !______________________!                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/
{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    ic_r;        /* Loop index corresp. to record in EPOINT */
   DBint    ic_p;        /* Loop index corresp. to u1,v1-u2,v2      */
   DBint    ptr;         /* Pointer ptr1 or ptr2  from EPOINT       */
   DBint    no_all;      /* Number of start and end points          */
   DBint    a_s[2*CBMAX];/* All records with -1 pointers (start)    */
   DBint    a_b[2*CBMAX];/* All records with -1 pointers            */
   DBint    i_s;         /* Loop index corresp. to a_s elements     */
   DBint    i_b;         /* Loop index corresp. to curve branch     */
   DBint    i_b_s;       /* Start value for loop i_b                */
   DBint    i_rprev;     /* Previous record                         */
   DBint    i_rnext;     /* Next     record                         */
/*-----------------------------------------------------------------!*/

   DBint    b_flag;      /* Boundary point flag                     */
                         /* Eq. 1: Two "normal" points              */
                         /* Eq. 2: Curve coincides iso-p.           */
                         /* Eq.-1: Points "used" in curve           */
                         /* Eq.-9: Do not use record !              */
                         /*        There are identical pts          */
                         /*        in another record !              */
   DBint    i_x;         /* Loop index for removal of end point     */
   DBint    no_iter;     /* Number of iterations (program check)    */
   DBint    no_recb;     /* Number of records in one branch         */
   DBint    no_rectot;   /* Number of records for all branches      */
   DBint    ic_s;        /* Closed curve start record               */
   DBint    ic_e;        /* Closed curve end   record               */
   DBint    ptr1_s;      /* Ptr ptr1 from first record in EPOINT    */
   DBint    ptr2_s;      /* Ptr ptr2 from first record in EPOINT    */
   DBint    ptr1_e;      /* Ptr ptr1 for end of curve               */
   DBint    ptr2_e;      /* Ptr ptr2 for end of curve               */
   short    status;      /* Error code from a called function       */
   char     errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initializations                                               */
/* __________________                                               */
/*                                                                  */
/* Initialize the number of output branches to zero                 */
/*                                                                 !*/
   *p_no_br= 0;          /* Start value for number of output branch.*/
   no_all= 0;
   no_iter=0;
   for (i_r= 1; i_r <= no_ep;  ++i_r ) 
   (p_etable+i_r-1)->branch = I_UNDEF;  

  ptr = I_UNDEF;

/*!                                                                 */
/* 2. Find all start and end points                                 */
/* ________________________________                                 */
/*                                                                  */
/* Loop all records in EPOINT       ic_r= 1,2,3, ... , no_ep        */
/*    Loop points in record         ic_p= 1,2                       */
/*                                                                  */
/*                                                                 !*/

for (ic_r= 1; ic_r<= no_ep; ++ic_r)      /* Start loop EPOINT       */
  {
  b_flag      = (p_etable+  i_r - 1 )->b_flag;
  if ( b_flag == -9 ) goto nxtrec;
  for ( ic_p=1; ic_p<= 2; ++ic_p )       /* Start loop u1,v1-u2,v2  */
    {

/*!                                                                 */
/*    Retrieve current entry/exit point from the EPOINT table       */
/*    For an start/end curve branch point (pointer= -1):            */
/*        i. Let total number of start/end points no_all=no_all+1   */
/*       ii. Record number for start/end point to array a_s         */
/*                                                                 !*/
    if      ( ic_p == 1 )
      {
      ptr = (p_etable+  ic_r - 1 )->ptr1;
      }
    else if ( ic_p == 2 )
      {
      ptr = (p_etable+  ic_r - 1 )->ptr2;
      }

    if ( ptr == -1 )                     /* Start or end point      */
      {
      no_all= no_all+1;                  /* Number of -1 records    */
      if ( no_all > 2*CBMAX )
        {
        sprintf(errbuf, "(no_all= %d> 2*CBMAX %d 1)%%sur922",
            (int)no_all,2*CBMAX );
        return(varkon_erpush("SU2993",errbuf));
        }
      a_s[no_all-1]= ic_r;               /* Record no to a_s        */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur922*cbranch   no_all= %d    a_s= %d\n",
       (int)no_all, (int)a_s[no_all-1]   );  
  }
#endif

      }

    }                                    /* End   loop u1,v1-u2,v2  */
nxtrec:;                                 /* Label: Equal iso-segm.  */
  }                                      /* End   loop EPOINT       */
/*!                                                                 */
/*    End points in record          ic_p= 1,2                       */
/* End records in EPOINT            ic_r= 1,2,3  ... , no_ep        */
/*                                                                 !*/

/*!                                                                 */
/* If no_all= 0 <==> The curve is closed.                           */
/* (This shall not occur. Refer to compare (uvdist and dist)        */
/*                                                                 !*/

   if ( no_all == 0 )
    {
    status=oneclo (p_etable,no_ep,&no_all, a_s );
    if (status<0) 
      {
      sprintf(errbuf,"oneclo%%sur922*compare");
      return(varkon_erpush("SU2943",errbuf));
      }
    }

/*!                                                                 */
/* 3. Select the curve branch start points                          */
/* _______________________________________                          */
/*                                                                  */
/* Loop all start/end pts in a_b    i_b= 1,2,3,4 ... ,no_all        */
/*                                                                 !*/


addclo:; /* label: Add closed branch  */

/* Initialize the number of output branches to zero                 */
   *p_no_br  = 0; 
   no_rectot = 0;
   i_b_s     = 1;

   if ( no_all > 2*CBMAX )
     {
     sprintf(errbuf, "(no_all= %d> 2*CBMAX %d 2)%%sur922",
       (int)no_all,2*CBMAX );
     return(varkon_erpush("SU2993",errbuf));
     }
   for (i_s= 1; i_s <= no_all; ++i_s ) 
     a_b[i_s-1] = a_s[i_s-1];

   for (i_r= 1; i_r <= no_ep;  ++i_r ) 
   (p_etable+i_r-1)->branch = I_UNDEF;  


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur922*cbranch Start or restart i_b_s %d no_all= %d \n", 
                      (int)i_b_s, (int)no_all );  
}
#endif


for (i_b= i_b_s; i_b <= no_all; ++i_b)   /* Start loop curve branch */
  {

/*!                                                                 */
/*  Retrieve record number i_r for start/end pt from b_a            */
/*  Let previous record number i_rprev= -1                          */

/*  Next record number if the retrieved point (record) is           */
/*  an end point of the curve branch:                               */
/*  Goto label endpt if i_r= -10                                    */
/*                                                                 !*/

  i_rprev= -1;                           /* Previous record number  */
  if ( i_b > 2*CBMAX )
     {
     sprintf(errbuf, "(i_b= %d> 2*CBMAX %d)%%sur922",(int)i_b,2*CBMAX );
     return(varkon_erpush("SU2993",errbuf));
     }
  i_r    = a_b[i_b-1];                   /* Current  record number  */
  if ( i_r == -10 )goto endpt;           /* Goto endpt for -10      */

/*!                                                                 */
/*  Retrieved point will be a curve branch start point.             */
/*  Increase number of output branches p_no_br                      */
/*  Curve branch start record i_r to output array cbsr              */

/*  Find the end point of this curve branch.                        */
/*                                                                 !*/

  *p_no_br= *p_no_br+1;                  /* Increase output branches*/
  if ( *p_no_br > CBMAX )
    {
    sprintf(errbuf, "(*p_no_br= %d> CBMAX %d)%%sur922*cbranch",
         (int)*p_no_br,CBMAX );
    return(varkon_erpush("SU2993",errbuf));
    }
  cbsr[*p_no_br-1]= i_r;                 /* Crv branch start record */
  no_recb = 0;                           /* No records in branch    */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur922*cbranch i_b= %d Crv branch= %d with start record (cbsr)= %d\n",
   (int)i_b, (int)*p_no_br, (int)cbsr[*p_no_br-1]   );  
}
#endif

/*!                                                                 */
newrec: /* Label: There is an additional record (pt) in the branch  */
/*                                                                 !*/

  no_recb = no_recb+1;                   /* No records in branch    */
  no_iter = no_iter+1;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur922*cbranch newrec: i_r %d no_recb %d no_iter= %d \n",
  (int)i_r,(int)no_recb,(int)no_iter);  
fflush(dbgfil(SURPAC)); 
}
#endif

  if ( i_r < 1 )
    {
#ifdef  DEBUG
    petable(p_etable,no_ep);       /* Printout of EPOINT      */
#endif
    sprintf(errbuf, "(i_r= %d < 1)%%sur922",(int)i_r);
    return(varkon_erpush("SU2993",errbuf));
    }

/*!                                                                 */
  if (no_iter > EPMAX )
    {
    sprintf(errbuf,
        "(no_iter= %d>EPMAX= %d)%%sur922",(int)no_iter,EPMAX);
    return(varkon_erpush("SU2993",errbuf));
    }

/*!                                                                 */
/*  Find record number for the next segment of the curve branch.    */
/*  Let i_rnext= ptr2 if i_rprev= ptr1 or otherwise                 */
/*  let i_rnext= ptr1  !!! H{r m}ste nog b}da avk{nnas !!!!!        */
/*                                                                 !*/

  if((p_etable+i_r-1)->ptr1==i_rprev)
    {
    i_rnext= (p_etable+i_r-1)->ptr2;     /* Next     record number  */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur922*cbranch u1= %f v1= %f     ptr1= %d i_rnext= %d\n",
     (p_etable+i_r-1)->u1, (p_etable+i_r-1)->v1,   
     (int)(p_etable+i_r-1)->ptr1,(int)i_rnext);
  fflush(dbgfil(SURPAC)); 
  }
#endif
    }
  else
    {
    i_rnext= (p_etable+i_r-1)->ptr1;     /* Next     record number  */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur922*cbranch u2= %f v2= %f     ptr2= %d i_rnext= %d\n",
     (p_etable+i_r-1)->u2, (p_etable+i_r-1)->v2,   
     (int)(p_etable+i_r-1)->ptr2,(int)i_rnext);
  fflush(dbgfil(SURPAC)); 
  }
#endif
    }

    if ( i_rnext > 0 )
        {
      if ( (p_etable+i_rnext-1)->branch > 0 )
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur922*cbranch i_b %d Current i_r %d branch= %d i_rnext= %d no_recb %d\n",
(int)i_b,(int)i_r,(int)(p_etable+i_r-1)->branch,(int)i_rnext,(int)no_recb);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur922*cbranch Record i_rnext %d is already used in branch= %d !\n",
(int)i_rnext, (int)(p_etable+i_rnext-1)->branch);
}
#endif
      if  ( i_rnext == ( p_etable+  i_r - 1 )->ptr1 )
        ( p_etable+  i_r - 1 )->ptr1 = -1;
      else
        ( p_etable+  i_r - 1 )->ptr2 = -1;

      no_all = no_all + 1;
      if ( no_all > 2*CBMAX )
        {
        sprintf(errbuf, "(no_all= %d> 2*CBMAX %d 3)%%sur922",
                   (int)no_all,2*CBMAX );
       return(varkon_erpush("SU2993",errbuf));
      }
      a_s[no_all-1] = i_r;

#ifdef  DEBUG
      petable(p_etable,no_ep);       /* Printout of EPOINT      */
#endif

      goto  addclo;

        }  /* End next record already used */
      }  /* i_rnext > 0                  */

/*!                                                                 */
/*  Branch number to current record, which is a flag that the       */
/*  record is "used" (initialization to negative values above)      */
/*                                                                 !*/
    (p_etable+i_r-1)->branch = *p_no_br;  

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur922*cbranch i_b %d Current i_r %d branch= %d i_rnext= %d no_recb %d\n",
(int)i_b,(int)i_r,(int)(p_etable+i_r-1)->branch,(int)i_rnext,(int)no_recb);
}
#endif

/*!                                                                 */
/*  Remove end point from b_a if ir_next= -1 (and continue)         */
/*  Let i_rprev= i_r, i_r= i_rnext and goto newrec if not -1        */
/*                                                                 !*/
  if ( i_rnext == -1 )
    {
    for (i_x=i_b; i_x <= no_all; ++i_x)  /* Start loop remove end pt*/
        {
        if ( i_x  > 2*CBMAX )
          {
          sprintf(errbuf, "(i_x= %d> 2*CBMAX %d)%%sur922*cbranch",
                      (int)i_x,2*CBMAX );
          return(varkon_erpush("SU2993",errbuf));
          }
        if ( a_b[i_x-1]==i_r )           /* End point <==> -10      */
             a_b[i_x-1]=  -10;
        }
    i_rprev =  -1;
    }
  else
    {
    i_rprev = i_r;
    i_r = i_rnext;
    goto  newrec;                        /* Next record in branch  */
    }

   no_rectot = no_rectot + no_recb;
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur922*cbranch Curve branch %d number of segments %d start record %d\n",
   (int)*p_no_br,   (int)no_recb, (int)cbsr[*p_no_br-1]   );  
}
#endif
/*!                                                                 */
endpt: ;  /* Label: Record in a_b was -10 (= end point)             */
/*                                                                 !*/

  }                                      /* End   loop curve branch */

/*!                                                                 */
/* End start/end points in b_a      i_b= 1,2,3,4 ... , no_all       */
/*                                                                 !*/


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur922*cbranch No of used records no_rectot %d Totally %d in EPOINT\n",
            (int)no_rectot, (int)no_ep );
}
#endif

/*!                                                                 */
/* Check if there is a closed curve left (if all records are used)  */
/*                                                                 !*/


  if  ( no_rectot == no_ep ) goto all_rec;

#ifdef  DEBUG
  if  ( no_rectot >  no_ep ) 
    {
    sprintf(errbuf, "(no_rectot>no_ep)%%sur922*cbranch");
    return(varkon_erpush("SU2993",errbuf)); 
    }
#endif

/*!                                                                 */
/* Find the first unused record and split curve                     */
/*                                                                 !*/

for (ic_r= 1; ic_r<= no_ep; ++ic_r)      /* Start loop EPOINT       */
  {

  if (  (p_etable + ic_r - 1 )->b_flag == -9) goto skip;

  if ( (p_etable+ic_r-1)->branch < 0 )
    {
    ic_s   = ic_r;
    ptr1_s = ( p_etable+  ic_s - 1 )->ptr1;
    ptr2_s = ( p_etable+  ic_s - 1 )->ptr2;
    ( p_etable+  ic_s - 1 )->ptr1 = -1;

  if ( ptr1_s < 1  ||  ptr2_s < 1  )
      {
#ifdef  DEBUG
petable(p_etable,no_ep);       /* Printout of EPOINT      */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur922*cbranch Closed crv start record ic_s %d with ptr1_s %d ptr2_s %d\n",
                  (int)ic_s, (int)ptr1_s,(int)ptr2_s);  
}
#endif
      sprintf(errbuf, "Error for ic_s %d: ptr1 or ptr2 < 1%%sur922",
                      (int)ic_s );
     return(varkon_erpush("SU2993",errbuf));
      }



    no_all = no_all + 1;
    if ( no_all > 2*CBMAX )
      {
      sprintf(errbuf, "(no_all= %d> 2*CBMAX %d 4)%%sur922",
                      (int)no_all,2*CBMAX );
      return(varkon_erpush("SU2993",errbuf));
      }

    a_s[no_all-1] = ic_s;

    ic_e = ptr1_s;
    no_all = no_all + 1;
    a_s[no_all-1] = ic_e;
    ptr1_e = ( p_etable+  ic_e - 1 )->ptr1;
    ptr2_e = ( p_etable+  ic_e - 1 )->ptr2;

    if        (  ic_s   == ptr1_e )
      ( p_etable+  ic_e - 1 )->ptr1 = -1;
    else
      ( p_etable+  ic_e - 1 )->ptr2 = -1;
   
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur922*cbranch Closed crv start record ic_s %d with ptr1_s %d ptr2_s %d\n",
                  (int)ic_s, (int)ptr1_s,(int)ptr2_s);  
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur922*cbranch Closed crv end   record ic_e %d with ptr1_e %d ptr2_e %d\n",
                      (int)ic_e, (int)ptr1_e,(int)ptr2_e);  
}
#endif

#ifdef  DEBUG
    petable(p_etable,no_ep);       /* Printout of EPOINT      */
#endif

    goto  addclo;

    }

skip:; /*! Label: b_flag= -9. Another identical record exists       */

  }                                      /* End   loop EPOINT       */

all_rec:; /* Label: All records are used   */




    return(SUCCED);

} /* End of function                                                */
/********************************************************************/



/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!********* Internal ** function ** oneclo  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function handles the case closed curve                       */

    static short oneclo (p_etable,no_ep, p_no_all, a_s )
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   EPOINT  *p_etable;    /* Entry/exit point table            (ptr) */
   DBint    no_ep;       /* Number of records in etable             */
   DBint   *p_no_all;    /* Number of start and end points          */
   DBint    a_s[];       /* All records with -1 pointers            */
/* Out:                                                             */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/

/*!-------------------- Flow chart ---------------------------------*/
/*                   ___________                                    */
/*                  !!         !!                                   */
/*                  !! oneclo  !!                                   */
/*                  !!_________!!                                   */
/*   _____________________!____                                     */
/*  !    ____!_____     _______!_________                           */
/* !1!  !    2     !   !       3         !                          */
/* !I!                                                              */
/* !n!                                                              */
/* !i!                                                              */
/* !t!                                                              */
/* !i!                                                              */
/* !a!                                                              */
/* !t!                                                              */
/* !i!                                                              */
/* !o!                                                              */
/* !n!                                                              */
/* !s!                                                              */
/* !_!                                                              */
/*                                                                  */
/*-----------------------------------------------------------------!*/
{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    ptr1_s;      /* Ptr ptr1 from first record in EPOINT    */
   DBint    ptr2_s;      /* Ptr ptr2 from first record in EPOINT    */
   DBint    ptr1_e;      /* Ptr ptr1 for end of curve               */
   DBint    ptr2_e;      /* Ptr ptr2 for end of curve               */
   DBint    ic_r;        /* Current record number      in EPOINT    */
/*-----------------------------------------------------------------!*/

#ifdef  DEBUG
   short    status;      /* Error code from a called function       */
   char     errbuf[80];  /* String for error message fctn erpush    */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initializations                                               */
/* ___________________                                              */
/*                                                                  */
/*                                                                 !*/

    ic_r = 1;
    ptr1_s = ( p_etable+  ic_r - 1 )->ptr1;
    ptr2_s = ( p_etable+  ic_r - 1 )->ptr2;


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur922*oneclo First record ptr1_s %d ptr2_s %d\n",(int)ptr1_s,(int)ptr2_s);  
}
#endif

/*!                                                                 */
/* 2. First record will be the start point                          */
/* _______________________________________                          */
/*                                                                  */
/*                                                                 !*/

    ic_r = 1;
    ( p_etable+  ic_r - 1 )->ptr1 = -1;

    *p_no_all = 1;                       /* Number of -1 records    */
    a_s[0]    = ic_r;                    /* Record no to a_s        */

/*!                                                                 */
/* 2. End of curve record is ptr1_s                                 */
/* ________________________________                                 */
/*                                                                  */
/*                                                                 !*/

    ic_r = ptr1_s;
    ptr1_e = ( p_etable+  ic_r - 1 )->ptr1;
    ptr2_e = ( p_etable+  ic_r - 1 )->ptr2;


    if        (  ptr1_e == 1 )
      ( p_etable+  ic_r - 1 )->ptr1 = -1;
    else
      ( p_etable+  ic_r - 1 )->ptr2 = -1;
   

    *p_no_all = 2;                       /* Number of -1 records    */
    a_s[1]    = ic_r;                    /* Record no to a_s        */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur922*oneclo Curve end record ic_r %d with ptr1_e %d ptr2_e %d\n",
                      (int)ic_r, (int)ptr1_e,(int)ptr2_e);  
}
#endif



/*!                                                                 */
/* 4. Printout of EPOINT table                                      */
/* ___________________________                                      */
/*                                                                  */
/* For DEBUG: Printout of the EPOINT table to SURPAC.DBG.           */
/*            Call of petable.                                      */
/*                                                                 !*/

#ifdef DEBUG
   status=petable(p_etable,no_ep);
   if (status<0) 
     {
     sprintf(errbuf,"petable%%sur922*oneclo");
     return(varkon_erpush("SU2943",errbuf));
     }
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/



/*!********* Internal ** function ** chelink ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function checks that all records in table EPOINT are linked  */

      static short chelink(p_etable,no_ep)
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
   DBint    f_link;      /* Eq. 0: All linked  Eq. -1: Error        */
   DBint    ic_r;        /* Loop index corresp. to record in EPOINT */
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Check ptr1 and ptr2                                           */
/*                                                                 !*/

f_link = 0;

for (ic_r= 1; ic_r<= no_ep; ++ic_r)      /* Start loop EPOINT       */
  {

  if (  (p_etable + ic_r - 1 )->b_flag == -9) goto skip;

  if     (   (p_etable+ic_r-1)->ptr1 == 0 ||
             (p_etable+ic_r-1)->ptr2 == 0    )
    {
    f_link = -1;
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur922*chelink Record number: %d is not linked !        ERROR!\n", 
                    (int)ic_r);
  fprintf(dbgfil(SURPAC),
  "EPOINT u1= %20.15f v1= %20.15f     ptr1= %d\n",
     (p_etable+ic_r-1)->u1, (p_etable+ic_r-1)->v1,   
     (int)(p_etable+ic_r-1)->ptr1);
  fprintf(dbgfil(SURPAC),
  "EPOINT u2= %20.15f v2= %20.15f     ptr2= %d\n",
     (p_etable+ic_r-1)->u2, (p_etable+ic_r-1)->v2,   
     (int)(p_etable+ic_r-1)->ptr2);
  fprintf(dbgfil(SURPAC),
  "EPOINT branch= %d b_flag= %d\n", (int)(p_etable+ic_r-1)->branch,
                                    (int)(p_etable+ic_r-1)->b_flag);
  }
#endif
  }    /* End: The record is not linked */

skip:; /*! Label: b_flag= -9. Another identical record exists       */

  }                                      /* End   loop EPOINT       */

    if  ( f_link == -1 )
    {
#ifdef  DEBUG
    petable(p_etable,no_ep);       /* Printout of EPOINT      */
#endif
    sprintf(errbuf, "Unlinked records%%sur922*chelink");
    return(varkon_erpush("SU2993",errbuf));
    }

    


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/



