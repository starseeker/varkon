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
/*  Function: varkon_sur_interfacet  SNAME: sur107 File: sur107.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates the intersect curve between two FAC_SUR's. */
/*  Output is arrays with straight line start and end points.       */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-06   Originally written                                 */
/*  1996-01-24   New and old GMPATF                                 */
/*  1996-01-30   New GMPATF                                         */
/*  1998-03-21   Elimination of compilation warning                 */
/*  1999-12-12   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function ---------------*/
/*                                                            */
/*sdescr varkon_sur_interfacet Intersection facet surfaces    */
/*                                                            */
/*------------------------------------------------------------*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_idpoint         * Identical point criterion               */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* varkon_lin_2plane      * Plane/plane intersect                   */
/* varkon_erpush          * Error message to terminal               */
/* varkon_erinit          * Initiate error message stack            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2??3 = No intersection between the surfaces                    */
/* SU2943 = Called function .. failed in varkon_sur_interfacet      */
/* SU2993 = Severe program error ( ) in varkon_sur_interfacet       */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
    DBstatus    varkon_sur_interfacet (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur1,       /* Surface 1                         (ptr) */
  DBPatch *p_pat1,       /* Alloc. area for topol. patch data (ptr) */
  DBSurf  *p_sur2,       /* Surface 2                         (ptr) */
  DBPatch *p_pat2,       /* Alloc. area for topol. patch data (ptr) */
  DBint    a_size,       /* Array size (maximum number of lines)    */
  DBVector s_pts[],      /* Start points for lines                  */
  DBVector e_pts[],      /* End   points for lines                  */
  DBint   *p_nlines )    /* Number of output lines                  */

/* Out:                                                             */
/*     Line data to s_pts, e_pts and p_nlines                       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    nu1;         /* Number of patches in U direction p_sur1 */
   DBint    nv1;         /* Number of patches in V direction p_sur1 */
   DBint    nu2;         /* Number of patches in U direction p_sur2 */
   DBint    nv2;         /* Number of patches in V direction p_sur2 */


   DBint    i_pat1;      /* Loop index surface 1                    */
   DBint    i_pat2;      /* Loop index surface 2                    */
   DBint    pat1_no;     /* Current patch number in surface 1       */
   DBint    pat2_no;     /* Current patch number in surface 2       */
   DBPatch *p_t1;        /* Current topological patch  sur 1  (ptr) */
   DBPatch *p_t2;        /* Current topological patch  sur 1  (ptr) */
   GMPATF  *p_patbc1;    /* Current B_PLANE patch surface 1   (ptr) */
   GMPATF  *p_patbc2;    /* Current B_PLANE patch surface 2   (ptr) */
   DBBplane sur1_bpl1;   /* B_PLANE 1 upper side  surface 1         */
   DBBplane sur1_bpl2;   /* B_PLANE 2 upper side  surface 1         */
   DBBplane sur2_bpl1;   /* B_PLANE 1 upper side  surface 2         */
   DBBplane sur2_bpl2;   /* B_PLANE 2 upper side  surface 2         */


/*                                                                  */
/*-----------------------------------------------------------------!*/

  char     *p_gpat1;     /* Secondary patch surface 1         (ptr) */
  char     *p_gpat2;     /* Secondary patch surface 2         (ptr) */
  DBVector  ps_out;      /* Start point intersect line              */
  DBVector  pe_out;      /* End   point intersect line              */
  DBint     lin_no;      /* Requested output line                   */
                         /* Eq.  12: Common part inside polygons    */
                         /* Eq.   1: Intersect B-plane 1 polygon    */
                         /* Eq.   2: Intersect B-plane 2 polygon    */
                         /* Eq.  99: Line in unbounded planes       */
  DBint     nlin;        /* No of output lines 0 or 1               */

  DBint     surtype;     /* Type FAC_SUR                            */
  DBfloat   c_idpoint;   /* Identical point criterion               */
  DBint     status;      /* Error code from called function         */
  char      errbuf[80];  /* String for error message fctn erpush    */

/* -------------------- Flow chart ---------------------------------*/
/*                _______________________                           */
/*               !                       !                          */
/*               ! varkon_sur_interfacet !                          */
/*               !      (sur107)         !                          */
/*               !_______________________!                          */
/*       ___________________!________________________________       */
/*  ____!_____         ____!_____   ___!___   ____!_____  ___!____  */
/* !    1     !       !    2     ! !   3   ! !    4     !!   5    ! */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur107 Enter *** varkon_sur_interfacet **Facet surfaces inters.***\n");
}
#endif
 
/*!                                                                 */
/* Identical point criterion. Call of varkon_idpoint (sur741).      */
/*                                                                 !*/

   c_idpoint = varkon_idpoint();

/*!                                                                 */
/* Initialize output variables                                      */
/*                                                                 !*/

  *p_nlines   = 0;       /* Number of output lines                  */

  lin_no      = 12;

/*!                                                                 */
/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/* For Debug On: Check that the surfaces are of type FAC_SUR        */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur1, &nu1, &nv1, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_interfacet(sur107)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


#ifdef DEBUG
if ( surtype     == FAC_SUR )            /* Check surface type      */
 ;
 else
 {
 sprintf(errbuf, "(sur1 not FAC_SUR)%% (sur107");
 return(varkon_erpush("SU2993",errbuf));
 }
#endif

    status = varkon_sur_nopatch (p_sur2, &nu2, &nv2, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_interfacet(sur107)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

#ifdef DEBUG
if ( surtype     == FAC_SUR )            /* Check surface type      */
 ;
 else
 {
 sprintf(errbuf, "(sur2 not FAC_SUR)%% (sur107");
 return(varkon_erpush("SU2993",errbuf));
 }
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur107 No patches in U nu1= %d and V nv1= %d Surface 1\n", 
                (int)nu1,(int)nv1);
fprintf(dbgfil(SURPAC),
"sur107 No patches in U nu2= %d and V nv2= %d Surface 2\n", 
                (int)nu2,(int)nv2);
fprintf(dbgfil(SURPAC),
"sur107 Maximum number of output lines a_size %d \n", (int)a_size );
fprintf(dbgfil(SURPAC),
"sur107 Surface type= %d  \n", p_sur1->typ_su);
}
#endif

/*!                                                                 */
/* 2. Calculate intersects                                          */
/* _______________________                                          */
/*                                                                  */
/*                                                                 !*/


pat1_no = 0;   

/*!                                                                 */
/* Loop all patches in surface 1 i_pat1=1,2,....,nu1*nv1            */
/*                                                                 !*/

for (i_pat1=1; i_pat1<= nu1*nv1; ++i_pat1) /* Start loop surface 1  */
  {

  pat2_no = 0;   
/*!                                                                 */
/*   Pointer to current patch in surface 1                          */
/*                                                                 !*/

     pat1_no = pat1_no + 1;              /* Patch number 1, 2, 3, ..*/
     p_t1 = p_pat1   + pat1_no - 1;      /* Current topol. patch    */
     p_gpat1 = p_t1->spek_c;             /* Secondary patch (C ptr) */
     if (p_t1->styp_pat != FAC_PAT )     /* Type of secondary patch */
     {
     sprintf(errbuf,"(not FAC_PAT)%%sur107");
     return(varkon_erpush("SU2993",errbuf));
     }

     p_patbc1= (GMPATF *)p_gpat1;        /* Current geometric patch */

/*!                                                                 */
/*   Goto nxt_pat1 if patch in surface 1 not is defined             */
/*                                                                 !*/

     if (  p_patbc1->defined == FALSE )
       {
       goto nxt_pat1;
       }

     sur1_bpl1.crd1_bp= p_patbc1->p1;
     sur1_bpl1.crd2_bp= p_patbc1->p2;
     sur1_bpl1.crd3_bp= p_patbc1->p2;
     sur1_bpl1.crd4_bp= p_patbc1->p3;
     sur1_bpl2.crd1_bp= p_patbc1->p1;
     sur1_bpl2.crd2_bp= p_patbc1->p3;
     sur1_bpl2.crd3_bp= p_patbc1->p3;
     sur1_bpl2.crd4_bp= p_patbc1->p4;

     for (i_pat2=1; i_pat2<= nu2*nv2; ++i_pat2) /* Start   surface 2  */
       {
       pat2_no = pat2_no + 1;              /* Patch number surface 2  */
       p_t2 = p_pat2   + pat2_no - 1;      /* Current topol. patch    */
       p_gpat2 = p_t2->spek_c;             /* Secondary patch (C ptr) */
       if (p_t2->styp_pat != FAC_PAT )     /* Type of secondary patch */
       {
       sprintf(errbuf,"(not FAC_PAT sur 2)%%sur107");
       return(varkon_erpush("SU2993",errbuf));
       }

       p_patbc2= (GMPATF *)p_gpat2;        /* Current geometric patch */

/*!                                                                   */
/*     Goto nxt_pat2 if patch in surface 2 not is defined             */
/*                                                                   !*/

       if (  p_patbc2->defined == FALSE )
         {
         goto nxt_pat2;
         }

     sur2_bpl1.crd1_bp= p_patbc2->p1;
     sur2_bpl1.crd2_bp= p_patbc2->p2;
     sur2_bpl1.crd3_bp= p_patbc2->p2;
     sur2_bpl1.crd4_bp= p_patbc2->p3;
     sur2_bpl2.crd1_bp= p_patbc2->p1;
     sur2_bpl2.crd2_bp= p_patbc2->p3;
     sur2_bpl2.crd3_bp= p_patbc2->p3;
     sur2_bpl2.crd4_bp= p_patbc2->p4;

  if  ( *p_nlines > a_size - 5 )
        {
        sprintf(errbuf,"a_size %d %%sur107 ",(int)a_size );
        return(varkon_erpush("SU2993",errbuf));
        }

   status=varkon_lin_2plane 
       (&sur1_bpl1, &sur2_bpl1, lin_no, &ps_out, &pe_out,  &nlin );
       if (  nlin == 1 && status == 0  )
         {
         s_pts[*p_nlines] = ps_out;   /* Line end points            */
         e_pts[*p_nlines] = pe_out; 
         *p_nlines   = *p_nlines + 1; /* An additional line         */
         }

   status=varkon_lin_2plane 
       (&sur1_bpl1, &sur2_bpl2, lin_no, &ps_out, &pe_out,  &nlin );
       if (  nlin == 1 && status == 0  )
         {
         s_pts[*p_nlines] = ps_out;   /* Line end points            */
         e_pts[*p_nlines] = pe_out; 
         *p_nlines   = *p_nlines + 1; /* An additional line         */
         }

   status=varkon_lin_2plane 
       (&sur1_bpl2, &sur2_bpl1, lin_no, &ps_out, &pe_out,  &nlin );
       if (  nlin == 1 && status == 0  )
         {
         s_pts[*p_nlines] = ps_out;   /* Line end points            */
         e_pts[*p_nlines] = pe_out; 
         *p_nlines   = *p_nlines + 1; /* An additional line         */
         }

   status=varkon_lin_2plane 
       (&sur1_bpl2, &sur2_bpl2, lin_no, &ps_out, &pe_out,  &nlin );
       if (  nlin == 1 && status == 0  )
         {
         s_pts[*p_nlines] = ps_out;   /* Line end points            */
         e_pts[*p_nlines] = pe_out; 
         *p_nlines   = *p_nlines + 1; /* An additional line         */
         }


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur107 Current patches pat1_no= %d pat2_no= %d\n", 
          (int)pat1_no,(int)pat2_no);
}
#endif

nxt_pat2:;

       }  /*  End loop surface 2 */

nxt_pat1:;

  }  /*  End loop surface 1 */


/* 6. Exit                                                          */
/* -------                                                          */

  if  ( *p_nlines == 0 )
        {
        sprintf(errbuf,"p_lines= 0%%sur107 " );
        varkon_erinit();
        return(varkon_erpush("SU2993",errbuf));
        }



#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur107 Exit***varkon_sur_interfacet** *p_nlines %d\n", 
          (int)*p_nlines );
  }
#endif

    varkon_erinit();

    return(SUCCED);

  } /* End of function */

/*********************************************************/
