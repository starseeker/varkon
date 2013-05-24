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
/*  Function: varkon_sur_selpati                   File: sur918.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Ist{llet f|r 12 och 30 grader borde varierbara gr{nser          */
/*  inf|ras. I testexemplet sursur1 blir cone_angle 11.627          */
/*  f|r alla patches fr}n yta 2 (ellipsoiden). F|r yta 1            */
/*  varierar vinkeln men ingen {r |ver 12 grader.                   */
/*                                                                  */
/*                                                                  */
/*  The function select the patches in two surfaces which may       */
/*  intersect. The output is one (BBOUND) table with records        */
/*  (patches) for both surfaces.                                    */
/*                                                                  */
/*  The output table also defines the iso-parametric curves, which  */
/*  shall be the "patch boundaries" for the surface/surface         */
/*  interrogation (intersect) function. The output UV net defines   */
/*  the accuracy in the calculation search phases. The density of   */
/*  the UV curve net may for instance determine how many of the     */
/*  curve branches that will be found in a surface/surface          */
/*  intersect calculation.                                          */
/*                                                                  */
/*  Note that the output are "irregular" nets of UV rectangles      */
/*  which are mixed in the PBOUND table.                            */
/*                                                                  */
/*           Regular UV net                   Irregular UV net      */
/*        ___________________                  ________________     */
/*       !         !         !                !    !     !     !    */
/*       !         !         !                !    !     !     !    */
/*       !    7    !    8    !                ! 14 ! 15  ! 16  !    */
/*       !_________!_________!              __!____!_____!_____!    */
/*       !         !         !             !  10   !_12__!     !    */
/*       !    5    !    6    !             !_______! 11  ! 13  !    */
/*       !_________!_________!              _______!_____!_____!    */
/*       !         !         !             !    !  !     !          */
/*       !         !         !             !    !  ! 8   !_____     */
/*       !    3    !    4    !             !    !  !_____!     !    */
/*       !         !         !             ! 5  !6 ! 7   !  9  !    */
/*       !_________!_________!             !____!__!_____!_____!    */
/*       !         !         !             !  !    !     !     !    */
/*       !    1    !    2    !             !1 ! 2  ! 3   !  4  !    */
/*       !_________!_________!             !__!____!_____!_____!    */
/*                                                                  */
/*                                                                  */
/*!New-Page--------------------------------------------------------!*/
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-16   Originally written                                 */
/*  1996-01-27   errbuf-DEBUG error fixed                           */
/*  1996-01-29   numrec not initialized                             */
/*  1996-02-11   New cone_angle limit 10 -> 12 degrees              */
/*  1996-06-15   Debug                                              */
/*  1999-11-21   Free source code modifications                     */
/*                                                                  */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_selpati    Select intersecting patches      */
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short addrecs();       /* Divide patch, add PBOUND records   */
static short sur2pat();       /* Add surface 2 patch address        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static BBOX    box1;              /* Patch box  from surface 1      */
static BBOX    box2;              /* Patch box  from surface 2      */
static BCONE   cone1;             /* Patch cone from surface 1      */
static BCONE   cone2;             /* Patch cone from surface 2      */
static DBint   sur2u[500];        /* Patch adresses surface 2 (IU)  */
static DBint   sur2v[500];        /* Patch adresses surface 2 (IV)  */
static DBint   sur2_tot;          /* Total no of surface 2 patches  */
static DBint   numrec;            /* Record number  in PBOUND table */
static DBfloat comptol;           /* Computer tolerance (accuracy)  */
/*-----------------------------------------------------------------!*/




/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush       * Error message to terminal                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_sur_selpati   */
/* SU2993 = Severe program error ( ) in varkon_sur_selpati (sur918) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus      varkon_sur_selpati (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBSurf  *p_sur1,       /* Surface 1                         (ptr) */
  DBPatch *p_pat1,       /* Alloc. area for topol. patch data (ptr) */
  DBint   s_type1,       /* Type CUB_SUR, RAT_SUR, or .....         */
  DBint   nu1,           /* Number of patches in U direction        */
  DBint   nv1,           /* Number of patches in V direction        */
  DBfloat s_lim1[2][2],  /* Surface limitation U,V points           */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
  DBSurf  *p_sur2,       /* Surface 2                         (ptr) */
  DBPatch *p_pat2,       /* Alloc. area for topol. patch data (ptr) */
  DBint   s_type2,       /* Type CUB_SUR, RAT_SUR, or .....         */
  DBint   nu2,           /* Number of patches in U direction        */
  DBint   nv2,           /* Number of patches in V direction        */
  DBfloat s_lim2[2][2],  /* Surface limitation U,V points           */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
  IRUNON *p_comp,        /* Computation data                  (ptr) */
  PBOUND *p_pbtable,     /* Pointer to patch boundary table         */
  DBint  *p_nr )         /* Number of records in pbtable            */

/* Out:                                                             */
/*        Table with ..                                             */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   s_iu1;        /* Start loop value in   U direction       */
   DBint   s_iv1;        /* Start loop value in   V direction       */
   DBint   e_iu1;        /* End   loop value in   U direction       */
   DBint   e_iv1;        /* End   loop value in   V direction       */
   DBint   iu1;          /* Loop index U line in surface 1          */
   DBint   iv1;          /* Loop index V line in surface 1          */
   DBPatch *p_t1;        /* Current topological patch         (ptr) */
   DBint   s_iu2;        /* Start loop value in   U direction       */
   DBint   s_iv2;        /* Start loop value in   V direction       */
   DBint   e_iu2;        /* End   loop value in   U direction       */
   DBint   e_iv2;        /* End   loop value in   V direction       */
   DBint   iu2;          /* Loop index U line in surface 2          */
   DBint   iv2;          /* Loop index V line in surface 2          */
   DBPatch *p_t2;        /* Current topological patch         (ptr) */
   DBint   n2_in;        /* No of box2 boxes intersecting box1      */
   DBint   f_in;         /* = -1: Outside box1 = 1: Intersect box1  */
   DBint   sur_no;       /* Surface number 1 or 2                   */
   DBint   ctype;        /* Type of computation SURSUR, ..          */
                         /* Density of the UV net:                  */
   DBint   u1_add;       /* For surface 1 in U direction            */
   DBint   v1_add;       /* For surface 1 in V direction            */
   DBint   u2_add;       /* For surface 2 in U direction            */
   DBint   v2_add;       /* For surface 2 in V direction            */
                         /* (Values 1, 2, 3, 4, .... )              */
/*-----------------------------------------------------------------!*/

   DBint   i_p;          /* Loop index intersecting patch surf. 2   */
   DBint   status;       /* Error code from called function         */
   char    errbuf[80];   /* String for error message fctn erpush    */
#ifdef DEBUG
   DBint   i_r;          /* Loop index record in PBOUND             */
   PBOUND *p_cr;         /* Current record in PBOUND          (ptr) */
#endif

/*--------------end-of-declarations---------------------------------*/

/*!-------------- Flow diagram -------------------------------------*/
/*                ____________________                              */
/*               !                    !                             */
/*               ! varkon_sur_selpati !                             */
/*               !     (sur918)       !                             */
/*               !____________________!                             */
/*         _______________!________________________                 */
/*   _____!_____   ____________!_____________   ___!____            */
/*  !     1   o ! !            2             ! !   3    !           */
/*  ! Initial.  ! ! Data to the PBOUND table ! !  Exit  !           */
/*  !___________! !__________________________! !________!           */
/*  Debug ! On                 !                                    */
/*   _____!_____               !                                    */
/*  ! initial   !              !                                    */
/*  !___________!              !                                    */
/*       ______________________!_____________                       */
/*  ____!_____   __________!__________    ___!___                   */
/* !          ! !                   * !  !       !                  */
/* ! Initial. ! ! U direction patches !  !varkon_!                  */
/* !   loop   ! !_____________________!  ! normv !                  */
/* ! variabl. !  __________!__________   !_______!                  */
/* !__________! !                   * !                             */
/*              ! V direction patches !                             */
/*              !_____________________!                             */
/*       __________________!                                        */
/*  ____!____    __________!__________                              */
/* !         !  !                   * !                             */
/* ! addsur2 !  ! Within one patch    !                             */
/* !_________!  !_____________________!                             */
/*                     ____!___                                     */
/*                    !        !                                    */
/*                    ! addrec !                                    */
/*                    !________!                                   !*/
/*------------------------------------------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur918 Enter varkon_sur_selpati: Select intersecting patches \n");
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
 
/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/* Retrieve computation data p_comp                                 */
/*                                                                 !*/

   ctype   = p_comp->isu_un.ctype;
   if  (  ctype == SURSUR )
     {
     comptol  = p_comp->isu_un.comptol;
     u1_add   = p_comp->isu_un.u1_add;
     v1_add   = p_comp->isu_un.v1_add;
     u2_add   = p_comp->isu_un.u2_add;
     v2_add   = p_comp->isu_un.v2_add;
     }
#ifdef  DEBUG
   else
     {
     sprintf(errbuf, "(ctype)%%varkon_sur_selpati"); 
      return(varkon_erpush("SU2993",errbuf));
     }
#endif  /* End DEBUG  */


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur918 u1_add %d v1_add %d u2_add %d v2_add %d comptol %15.10g\n",
 u1_add,v1_add,u2_add,v2_add , comptol );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
 
   u1_add = I_UNDEF;
   v1_add = I_UNDEF;
   u2_add = I_UNDEF;
   v2_add = I_UNDEF;

   sur2_tot = 0;         /* Total number of surface 2 patches       */

  *p_nr = I_UNDEF;       /* Number of records in pbtable            */
  numrec = 0;

/*!                                                                 */
/* 2. Data to PBOUND table pbtable                                  */
/* _______________________________                                  */
/*                                                                  */
/*                                                                 !*/

/* Initiate loop variables:                                         */



   s_iu1  = (DBint)s_lim1[0][0] - 1; /* Start loop value U          */
   s_iv1  = (DBint)s_lim1[0][1] - 1; /* Start loop value V          */
   e_iu1  = (DBint)s_lim1[1][0] - 1; /* End   loop value U          */
   e_iv1  = (DBint)s_lim1[1][1] - 1; /* End   loop value V          */

   if ( s_iu1 < 0 || s_iv1 < 0 || e_iu1 > nu1 || e_iv1 > nv1 )
      {
      sprintf(errbuf,                    /* Error SU2993 for invalid*/
      "(s_iu1...)%%varkon_sur_selpati"); /* type                    */
      return(varkon_erpush("SU2993",errbuf));
      }
   s_iu2  = (DBint)s_lim2[0][0] - 1; /* Start loop value U          */
   s_iv2  = (DBint)s_lim2[0][1] - 1; /* Start loop value V          */
   e_iu2  = (DBint)s_lim2[1][0] - 1; /* End   loop value U          */
   e_iv2  = (DBint)s_lim2[1][1] - 1; /* End   loop value V          */

   if ( s_iu2 < 0 || s_iv2 < 0 || e_iu2 > nu2 || e_iv2 > nv2 )
      {
      sprintf(errbuf,                    /* Error SU2993 for invalid*/
      "(s_iu2...)%%varkon_sur_selpati"); /* type                    */
      return(varkon_erpush("SU2993",errbuf));
      }


/*!                                                                 */
/* Loop all V patches  iv1= s_iu1, ...., e_iu1                      */
/*  Loop all U patches  iu1= s_iv1, ..., e_iv1                      */
/*                                                                 !*/

for ( iu1=s_iu1; iu1<= e_iu1; ++iu1 )    /* Start loop U patches    */
  {
  for ( iv1=s_iv1; iv1<= e_iv1; ++iv1 )  /* Start loop V patches    */
    {


/*!                                                                 */
/*   Pointer to current topological patch                           */
     p_t1 = p_pat1 + iu1*nv1 + iv1; 
/*   Retrieve BBOX for the current patch in surface 1.              */
/*                                                                 !*/

     box1  =  p_t1->box_pat;             /* Copy BBOX  to box1      */
     cone1 =  p_t1->cone_pat;            /* Copy BCONE to cone1     */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
 fprintf(dbgfil(SURPAC),
   "sur914 box1 Min %8.1f %8.1f %8.1f Max %8.1f %8.1f %8.1f\n",
         box1.xmin,   box1.ymin,   box1.zmin,
         box1.xmax,   box1.ymax,   box1.zmax );
}
#endif

     n2_in = 0;               /* No of box2 boxes intersecting box1 */
/*!                                                                 */
/*   Loop all V patches  iv2= s_iu2, ...., e_iu2                    */
/*    Loop all U patches  iu2= s_iv2, ..., e_iv2                    */
/*                                                                 !*/

      for ( iu2=s_iu2; iu2<= e_iu2; ++iu2 )   /* Start loop U      */
        {
        for ( iv2=s_iv2; iv2<= e_iv2; ++iv2 ) /* Start loop V      */
          {


/*!                                                                 */
/*        Pointer to current topological patch                      */
          p_t2 = p_pat2 + iu2*nv2 + iv2; 
/*        Retrieve BBOX for the current patch in surface 2.         */
/*                                                                 !*/

          box2  =  p_t2->box_pat;        /* Copy BBOX  to box2      */
          cone2 =  p_t2->cone_pat;       /* Copy BCONE to cone2     */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
 fprintf(dbgfil(SURPAC),
   "sur914 box2 Min %8.1f %8.1f %8.1f Max %8.1f %8.1f %8.1f\n",
         box2.xmin,   box2.ymin,   box2.zmin,
         box2.xmax,   box2.ymax,   box2.zmax );
}
#endif
          f_in = -1;

          /*   X  values                                             */
          if        ( box1.xmin >  box2.xmin &&
                      box1.xmin >  box2.xmax &&
                      box1.xmax >  box2.xmin &&
                      box1.xmax >  box2.xmax    )  goto nointers;
          if        ( box1.xmin <  box2.xmin &&
                      box1.xmin <  box2.xmax &&
                      box1.xmax <  box2.xmin &&
                      box1.xmax <  box2.xmax    )  goto nointers;
          /*   Y  values                                             */
          if        ( box1.ymin >  box2.ymin &&
                      box1.ymin >  box2.ymax &&
                      box1.ymax >  box2.ymin &&
                      box1.ymax >  box2.ymax    )  goto nointers;
          if        ( box1.ymin <  box2.ymin &&
                      box1.ymin <  box2.ymax &&
                      box1.ymax <  box2.ymin &&
                      box1.ymax <  box2.ymax    )  goto nointers;
          /*   Z  values                                             */
          if        ( box1.zmin >  box2.zmin &&
                      box1.zmin >  box2.zmax &&
                      box1.zmax >  box2.zmin &&
                      box1.zmax >  box2.zmax    )  goto nointers;
          if        ( box1.zmin <  box2.zmin &&
                      box1.zmin <  box2.zmax &&
                      box1.zmax <  box2.zmin &&
                      box1.zmax <  box2.zmax    )  goto nointers;

          n2_in = n2_in + 1;       /* Possible intersect with box2   */

          f_in =  1;  /* Not a necessary variable  */



          if  (  f_in  ==  1 )
            {
            status=sur2pat (iu2,iv2);
            if (status<0) 
             {
             sprintf(errbuf,"sur2pat%%varkon_sur_selpati (sur918)");
             return(varkon_erpush("SU2973",errbuf));
             }
            }
       
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 && f_in == 1 )
{
  fprintf(dbgfil(SURPAC),
  "sur918 iu1= %d iv1= %d iu2= %d iv2= %d f_in %d n2_in %d sur2_tot %d numrec %d\n", 
          iu1,    iv1,    iu2,    iv2,    f_in,   n2_in, sur2_tot, numrec );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

nointers:;

       }                        /* End   loop U patches surface 1   */
     }                          /* End   loop V patches surface 1   */
/*!                                                                 */
/*     End  all U patches  iu2= s_iu2, ..., e_iu2                   */
/*   End  all V patches  iv2= s_iv2, ..., e_iv2                     */
/*   Add records in PBOUND table if n2_in > 0                       */
/*                                                                 !*/

     if  (  n2_in  >  0 )
       {
       sur_no = 1;
       status=addrecs (p_pbtable,sur_no,u1_add,v1_add,iu1,iv1,s_type1);
       if (status<0) 
        {
        sprintf(errbuf,"addrecs%%varkon_sur_selpati (sur918)");
        return(varkon_erpush("SU2973",errbuf));
        }
       }

    }                           /* End   loop U patches surface 2   */
  }                             /* End   loop V patches surface 2   */
/*!                                                                 */
/*  End  all U patches  iu1= s_iu1, ..., e_iu1                      */
/* End  all V patches  iv1= s_iv1, ..., e_iv1                       */
/*                                                                 !*/


/*!                                                                 */
/* Surface 2 patches to the PBOUND table                            */
/*                                                                 !*/


/*!                                                                 */
/* Loop all intersection surface 2 patches  i_p= 1, ... , sur2_tot  */
/*                                                                 !*/

sur_no = 2;
for ( i_p= 1; i_p<= sur2_tot; ++i_p )    /* Start loop patches      */
  {
    iu2 = sur2u[i_p-1];
    iv2 = sur2v[i_p-1];
    status=addrecs (p_pbtable,sur_no,u2_add,v2_add,iu2,iv2,s_type2);
    if (status<0) 
      {
      sprintf(errbuf,"addrecs(2)%%varkon_sur_selpati (sur918)");
      return(varkon_erpush("SU2973",errbuf));
      }

  }  




/*!                                                                 */
/* 3. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/* Total number of records to output variable                       */
   *p_nr = numrec;
/*                                                                  */
/* Check that U and V values are within surface for Debug On        */
/*                                                                 !*/

#ifdef DEBUG
      if ( *p_nr == 0 ) 
        {
        sprintf(errbuf,"*p_nr=0%%varkon_sur_selpati (sur918)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif


#ifdef DEBUG
/* Check that U and V values are within surface for Debug On        */
for ( i_r = 1; i_r  <= *p_nr; ++i_r  ) /* Start loop pbound records */
  {
  p_cr= p_pbtable + i_r - 1;             /* Ptr to current record   */
  if      (  p_cr->sur_no == 1 )         /* Surface number 1        */
    {
    if ( p_cr->us <     1.0         ||   /* us is not OK            */
         p_cr->us > (DBfloat)nu1 + 1.0 )
      {
      goto  pb_error;
      }
    if ( p_cr->vs <     1.0         ||   /* vs is not OK            */
         p_cr->vs > (DBfloat)nv1 + 1.0 )
      {
      goto  pb_error;
      }
    if ( p_cr->ue <     1.0         ||   /* ue is not OK            */
         p_cr->ue > (DBfloat)nu1 + 1.0 )
      {
      goto  pb_error;
      }
    if ( p_cr->ve <     1.0         ||   /* ve is not OK            */
         p_cr->ve > (DBfloat)nv1 + 1.0 )
      {
      goto  pb_error;
      }
    }
  else if (  p_cr->sur_no == 2 )         /* Surface number 2        */
    {
    if ( p_cr->us <     1.0         ||   /* us is not OK            */
         p_cr->us > (DBfloat)nu2 + 1.0 )
      {
      goto  pb_error;
      }
    if ( p_cr->vs <     1.0         ||   /* vs is not OK            */
         p_cr->vs > (DBfloat)nv2 + 1.0 )
      {
      goto  pb_error;
      }
    if ( p_cr->ue <     1.0         ||   /* ue is not OK            */
         p_cr->ue > (DBfloat)nu2 + 1.0 )
      {
      goto  pb_error;
      }
    if ( p_cr->ve <     1.0         ||   /* ve is not OK            */
         p_cr->ve > (DBfloat)nv2 + 1.0 )
      {
      goto  pb_error;
      }
    }
  else                                   /* Surface number not OK   */
    {
    goto pb_error;
    }

  }              /* End loop records in PBOUND                       */

goto no_error;   /* No errors found in PBOUND table                  */

pb_error:; /* There is an error in the PBOUND table                  */

if ( dbglev(SURPAC) == 1 )
{
  fprintf(dbgfil(SURPAC),
  "sur918 Error PBOUND table nu1 %d nv1 %d nu2 %d nv2 %d\n",
   nu1, nv1, nu2, nv2 );
}
        sprintf(errbuf,"PBOUND table not OK%%sur918");
        return(varkon_erpush("SU2993",errbuf));

no_error:; /* There is no error in the PBOUND table                  */


#endif

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
  fprintf(dbgfil(SURPAC),
  "sur918 s_iu1= %d s_iv1= %d e_iu1= %d e_iv1= %d\n",
   s_iu1,s_iv1,e_iu1,e_iv1);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur918 Exit*varkon_sur_selpati*No of PBOUND records %d (max PBMAX %d)\n"
     , *p_nr , PBMAX );
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/



/*!********* Internal ** function ** addrecs ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates UV net for Bicubic surface approximation    */

   static short addrecs (p_pbtable,sur_no,np_u_in,np_v_in,iu,iv,surtype)
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  PBOUND  *p_pbtable;    /* Patch boundary table              (ptr) */
  DBint    sur_no;       /* Surface number 1 or 2                   */
  DBint    np_u_in;      /* No of addition. patches per patch in U  */
  DBint    np_v_in;      /* No of addition. patches per patch in V  */
  DBint    iu;           /* Current patch IU                        */
  DBint    iv;           /* Current patch IV                        */
  DBint    surtype;      /* Type CUB_SUR, RAT_SUR, LFT_SUR, ....    */
/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   DBint   np_u;         /* No of addition. patches per patch in U  */
   DBint   np_v;         /* No of addition. patches per patch in V  */
   DBfloat us_p;         /* Start U value for the patch             */
   DBfloat vs_p;         /* Start V value for the patch             */
   DBfloat ue_p;         /* End   U value for the patch             */
   DBfloat ve_p;         /* End   V value for the patch             */
   DBfloat u_delta;      /* Delta U value                           */
   DBfloat v_delta;      /* Delta V value                           */
   DBint   iu_d;         /* Loop index for the U patch splitting    */
   DBint   iv_d;         /* Loop index for the V patch splitting    */
   DBint   nu_d;         /* Loop end for iu_d                       */
   DBint   nv_d;         /* Loop end for iv_d                       */
   DBfloat cone_angle;   /* Bounding cone angle                     */
   PBOUND *p_cr;         /* Current record in PBOUND          (ptr) */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initialize and check                                          */
/* ==========================                                       */
/*                                                                 !*/

/*!                                                                 */
/* Goto d_def if the number of additional patches is defined.       */
/* Retrieve BCONE angle for the current patch                       */
/* Determine the number of times the patch shall be divided         */
/* Let no of patches be  one  if       cone_angle < 12 degrees      */
/* Let no of patches be four  if 12 <= cone_angle < 30 degrees      */
/* Let no of patches be nine  if 30 <= cone_angle                   */
/* The cone angle is not calculated for LFT_SUR !!! tillf{lligt !   */
/* Let no of patches be nine  for a surface of type LFT_SUR         */
/*                                                                 !*/

   if ( np_u_in > 0 && np_v_in > 0 ) 
     {
     np_u = np_u_in;
     np_v = np_v_in;
#ifdef DEBUG
if ( np_u <= 1 || np_u > 30 ||
     np_v <= 1 || np_v > 30    )
   {
   sprintf(errbuf,"addrecs %d or %d > 30%%varkon_sur_selpati",
                    np_u , np_v  );
   return(varkon_erpush("SU2993",errbuf));
   }
#endif
     goto d_def;
     }

   if( sur_no == 1 ) cone_angle= cone1.ang; /* BCONE angle  */
   else              cone_angle= cone2.ang;
   if      ( surtype == LFT_SUR || surtype == CON_SUR )
     {
     np_u = 5;
     np_v = 5;
     }
   else if ( fabs(cone_angle) < 12.0 )
     {
     np_u = 1;
     np_v = 1;
     }
   else if ( fabs(cone_angle) >= 12.0 && fabs(cone_angle) < 30.0 )
     {
     np_u = 2;
     np_v = 2;
     }
   else 
     {
     np_u = 3;
     np_v = 3;
     }


d_def:; /* Label: np_u and np_v is input to the function            */

nu_d    = np_u - 1;                      /* End loop iu_d           */
nv_d    = np_v - 1;                      /* End loop iv_d           */

  us_p    =(DBfloat)iu+1.0+comptol;        /* U start point           */
  ue_p    =(DBfloat)iu+2.0-comptol;        /* U end   point           */
  u_delta =(ue_p - us_p)/(DBfloat)np_u;    /* Delta U                 */
  for ( iu_d= 0; iu_d <= nu_d; ++iu_d )  /* Start loop extra U lines*/
    {
      vs_p    =(DBfloat)iv+1.0+comptol;    /* V start point           */
      ve_p    =(DBfloat)iv+2.0-comptol;    /* V end   point           */
      v_delta =(ve_p - vs_p)/(DBfloat)np_v;/* Delta V                 */

      for ( iv_d= 0; iv_d <= nv_d; ++iv_d )
        {
        numrec = numrec + 1;             
        if ( numrec > PBMAX )
          {
          sprintf(errbuf,"PBMAX %d 0%%varkon_sur_selpati", PBMAX );
          return(varkon_erpush("SU2993",errbuf));
          }
        p_cr= p_pbtable + numrec - 1;     /* Ptr to current record   */
        p_cr->rec_no= numrec;             /* Record number           */
        p_cr->us=                         /* U start point           */
               us_p+ iu_d   *u_delta;
        p_cr->vs=                         /* V start point           */
               vs_p+ iv_d   *v_delta; 
        p_cr->ue=                         /* U end   point           */
               us_p+(iu_d+1)*u_delta;
        p_cr->ve=                         /* V end   point           */
               vs_p+(iv_d+1)*v_delta; 
        p_cr->sur_no = sur_no;            /* Surface number 1 or 2   */

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur918*addrecs np_u %d np_v %d cone_angle %f\n",
   np_u ,  np_v ,  cone_angle);
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur918*addrecs %f %f %f %f\n",
  p_cr->us,p_cr->vs,p_cr->ue,p_cr->ve );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
       }
   } /* End loop            */


    return(SUCCED);

} /* End of function                                                */


/*!********* Internal ** function ** sur2pat ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Add patch adress for an intersecting patch from surface 2        */

   static short sur2pat (iu2,iv2)
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBint    iu2;          /* Current patch IU                        */
  DBint    iv2;          /* Current patch IV                        */
/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint    i_a;          /* Loop index element in array             */
/*----------------------------------------------------------------- */

   char    errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* Check if the surface 2 patch already is defined                  */
/*                                                                 !*/

  for ( i_a= 0; i_a < sur2_tot; ++i_a )  
    {
    if  (  iu2 == sur2u[i_a] && iv2 == sur2v[i_a] )
       {
       goto exist;
       }
    }



if ( sur2_tot >= 499  )
   {
   sprintf(errbuf,"sur2_tot %d 0%%varkon_sur_selpati", sur2_tot  );
   return(varkon_erpush("SU2993",errbuf));
   }

   sur2u[sur2_tot] = iu2;
   sur2v[sur2_tot] = iv2;
   sur2_tot = sur2_tot + 1;



exist:;  /* Patch is already in the array */


    return(SUCCED);

} /* End of function                                                */






