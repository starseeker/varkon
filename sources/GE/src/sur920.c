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
/*  Function: varkon_sur_epts                      File: sur920.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function is a general function for the calculation          */
/*  of entry and exit points in patches defined by the              */
/*  input patch boundary table (PBOUND).                            */
/*                                                                  */
/*  Input is a pointer to a structure which defines the             */
/*  calculation criterion for the points (the objective             */
/*  function), the method for the numerical calculation,            */
/*  the number of restarts, the tolerances, etc.                    */
/*  This data is passed on to the general (curve creation)          */
/*  patch interrogation routine.                                    */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1996-02-13   Debug                                              */
/*  1996-04-27   Debug, unreferenced parameters deleted             */
/*  1996-04-29   b_flag= 2 for iso-segments to sur926               */
/*  1996-11-15   Bug: cone data not retrieved from PBOUND           */
/*               Compilation warnings                               */
/*  1997-04-05   printf -> sprintf                                  */
/*  1998-09-26   n_patch > 2 Bug Data added to EPOINT for numpts=0  */
/*  1999-11-24   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_epts       Entry/exit points in patches     */
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short boundary();      /* Entry/exit points in one boundary  */
#ifdef  DEBUG
static short initial();       /* Initialization of variables        */
static short pripts();        /* Print entry/exit points            */
#endif
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBint   numpts;            /* Number of entry/exit records   */
static DBint   n_patch;           /* No entry/exit pts in one patch */
static PBOUND *p_cr;              /* Current point record     (ptr) */
static DBfloat u_s;               /* U start point                  */
static DBfloat v_s;               /* V start point                  */
static DBfloat u_e;               /* U end   point                  */
static DBfloat v_e;               /* V end   point                  */
static DBfloat t_in[2][2];        /* Iso-parameter curve limits     */
                                  /* (0,0): Start U  (0,1): Start V */
                                  /* (1,0): End   U  (1,1): End   V */
static EVALC   curpt[9];          /* All entry/exit curve points    */
static EVALS   xyzpt[9];          /* Derivatives corresp. to curpt  */
static DBfloat u1,v1;             /* Entry/exit point 1             */
static DBfloat u2,v2;             /* Entry/exit point 2             */
static DBfloat u1_t,v1_t;         /* Entry/exit point 1 tangent     */
static DBfloat u2_t,v2_t;         /* Entry/exit point 2 tangent     */
static DBint   b_anal;            /* Boundary point case:           */
                                  /* Eq. 0: No     points           */
                                  /* Eq. 1: Normal points           */
                                  /* Eq. 2: Curve coincides w. iso  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_ini_evalc        * Initiate EVALC               */
/*           varkon_ini_evals        * Initiate EVALS               */
/*           varkon_sur_num1         * Patch one parameter calculat */
/*           varkon_pat_chebound     * Check bounding box & cone    */
/*           varkon_sur_uvtang       * Calculate UV tangents        */
/*           varkon_erpush           * Error message to terminal    */
/*           varkon_erinit           * Initiate error message stack */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2783 = Number of entry/exit points is zero   varkon_sur_epts   */
/* SU2943 = Called function   () failed in varkon_sur_epts (sur920) */
/* SU2973 = Internal function () failed in varkon_sur_epts (sur920) */
/* SU2993 = Severe program error xxx    in varkon_sur_epts (sur920) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus     varkon_sur_epts (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur,       /* Surface                           (ptr) */
   DBPatch *p_pat,       /* Alloc. area for topol. patch data (ptr) */
   IRUNON  *p_comp,      /* Computation data                  (ptr) */
   PBOUND  *p_pbtable,   /* Patch boundary table              (ptr) */
   DBint    pbn,         /* Number of records in pbtable            */
   EPOINT  *p_etable,    /* Entry/exit point table            (ptr) */
   DBint   *p_en )       /* Number of records in etable             */
/* Out:                                                             */
/*         Enter/exit points in EPOINT table and no records         */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  ir;            /* Loop index corresp. to PBOUND record    */
   DBint  numrec;        /* Record number                           */
   DBint  ctype;         /* Case of computation                     */
   BBOX   pbox;          /* Bounding box  for the (PBOUND) patch    */
   BCONE  pcone;         /* Bounding cone for the (PBOUND) patch    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint  i_r;           /* Loop indec record                       */
   EVALC  ecrec;         /* Record curve   point and derivatives    */
   EVALS  esrec;         /* Record surface point and derivatives    */
   DBint  sflag;         /* Eq. -1: No solution Eq. 1: Solution     */
   DBint  bcase;         /* Eq. 1: u0 Eq. 2: u1 Eq. 3: v0 Eq. 4: v1 */
   DBint  all_b_anal[4]; /* Boundary point case for all boundaries  */
                         /* Eq. 0: No     points                    */
                         /* Eq. 1: Normal points                    */
                         /* Eq. 2: Curve coincides w. isoparameter  */
   short  b_flag;        /* Boundary point case                     */
   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint  ix1;           /* Temporarely used loop index             */
   DBint  n_select;      /* Number of selected patches              */
   DBint  status;        /* Error code from a called function       */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */

/*  Initialize output coordinates and derivatives for DEBUG on.     */
/*  Call of initial, which also prints out the input data.          */
/*                                                                 !*/

    b_flag = I_UNDEF;

/*  Printout of input data is in function initial                   */

#ifdef DEBUG
    status=initial(p_sur,p_pat,p_comp,p_pbtable,pbn,p_etable,p_en);
    if (status<0) 
        {
        sprintf(errbuf,"initial%%varkon_sur_epts (sur920)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif

/* Initialize variabels xyz_                                        */
/* Calls of varkon_ini_evalc (sur776) and _ini_evals (sur770)       */
/*                                                                 !*/

   for (i_r=1; i_r<=  9;   i_r= i_r+1)
     {
     varkon_ini_evalc (&ecrec);
     curpt[i_r-1] = ecrec;
     varkon_ini_evals (&esrec);
     xyzpt[i_r-1] = esrec; 
     }

   all_b_anal[0] = I_UNDEF;               /* Boundary point case    */
   all_b_anal[1] = I_UNDEF; 
   all_b_anal[2] = I_UNDEF;
   all_b_anal[3] = I_UNDEF;

/*!                                                                 */
/* Case of computation ctype from p_comp                            */
/*                                                                 !*/

   ctype   = p_comp->ipl_un.ctype;

/*!                                                                 */
/* 2. Calculate all entry exit points                               */
/* __________________________________                               */
/*                                                                  */
/*                                                                 !*/


/*!                                                                 */
/* Initiate loop variables:                                         */
   numpts = 0;
/* Loop all records in PBOUND table ir=0,1,....,pbn                 */
/*                                                                 !*/

n_select  = 0;           /* Number of selected patches              */
for ( ir=0; ir< pbn; ++ir )              /* Start loop PBOUND       */
  {

    n_patch = 0;                         /* No pts in one patch     */

/*!                                                                 */
/*  Data from the PBOUND table:                                     */
    p_cr= p_pbtable + ir;                /* Ptr to current record   */
    numrec=p_cr->rec_no;                 /* Record number           */
    u_s   =p_cr->us;                     /* U start point           */
    v_s   =p_cr->vs;                     /* V start point           */
    u_e   =p_cr->ue;                     /* U end   point           */
    v_e   =p_cr->ve;                     /* V end   point           */
    pbox  =p_cr->pbox;                   /* BBOX                    */
    pcone =p_cr->pcone;                  /* BCONE                   */
/*                                                                 !*/

/*!                                                                 */
/*  Check if there is a solution for the bounding box or cone.      */
/*  Call of varkon_pat_chebound (sur914).                           */
/*  Goto nosol if there is no solution in the patch.                */
/*                                                                 !*/

    status=varkon_pat_chebound
    (p_sur,p_pat,p_comp,u_s,u_e,v_s,v_e,&pbox,&pcone,&sflag);
    if (status<0) 
      {
      sprintf(errbuf,"varkon_pat_chebound(sur914)%%varkon_sur_epts");
      return(varkon_erpush("SU2943",errbuf));
      }

    if ( sflag == -99 )
       /* Surface patch is not OK */
       {
       p_cr->nu0 = 0;                    /* No solutions to PBOUND  */
       p_cr->nu1 = 0;
       p_cr->nv0 = 0;
       p_cr->nv1 = 0;
       goto  nosol;
       }

    if ( sflag == -1 )
       {
       p_cr->nu0 = 0;                    /* No solutions to PBOUND  */
       p_cr->nu1 = 0;
       p_cr->nv0 = 0;
       p_cr->nv1 = 0;
       goto  nosol;
       }

   n_select  = n_select + 1;      /* Number of selected patches     */

/*!                                                                 */
/*  Find solution points on curve segment U= u_s for V= v_s to v_e  */
/*  (u_s,v_s,u_e,v_e to the varkon_sur_num1 t_in variable)          */
/*  Call of boundary.                                               */
/*                                                                 !*/
    
    t_in[0][0] = u_s;                    /* Start U                 */
    t_in[0][1] = v_s;                    /* Start V                 */
    t_in[1][0] = u_s;                    /* End   U                 */
    t_in[1][1] = v_e;                    /* End   V                 */

    bcase      =  1;                     /* Boundary u= u_s         */

    status=boundary(p_sur,p_pat,p_comp,bcase);
    if (status<0) 
        {
        sprintf(errbuf,"boundary u0%%varkon_sur_epts (sur920)");
        return(varkon_erpush("SU2973",errbuf));
        }
    all_b_anal[bcase-1] = b_anal;         /* Boundary point case    */


/*!                                                                 */
/*  Find solution points on curve segment U= u_e for V= v_s to v_e  */
/*  (u_s,v_s,u_e,v_e to the varkon_sur_num1 t_in variable)          */
/*  Call of boundary.                                               */
/*                                                                 !*/
    

t_in[0][0] = u_e;        /* Start U                                 */
t_in[0][1] = v_s;        /* Start V                                 */
t_in[1][0] = u_e;        /* End   U                                 */
t_in[1][1] = v_e;        /* End   V                                 */

    bcase      =  2;                     /* Boundary u= u_e         */

    status=boundary(p_sur,p_pat,p_comp,bcase);
    if (status<0) 
        {
        sprintf(errbuf,"boundary u0%%varkon_sur_epts (sur920)");
        return(varkon_erpush("SU2973",errbuf));
        }
    all_b_anal[bcase-1] = b_anal;         /* Boundary point case    */

/*!                                                                 */
/*  No constant V for S_SILH calculation (goto novlin in this case) */
/*                                                                  */
/*  Find solution points on curve segment V= v_s for U= u_s to u_e  */
/*  (u_s,v_s,u_e,v_e to the varkon_sur_num1 t_in variable)          */
/*  Call of boundary.                                               */
/*                                                                 !*/
 
if ( ctype == S_SILH ) goto novlin;   

t_in[0][0] = u_s;        /* Start U                                 */
t_in[0][1] = v_s;        /* Start V                                 */
t_in[1][0] = u_e;        /* End   U                                 */
t_in[1][1] = v_s;        /* End   V                                 */

    bcase      =  3;                     /* Boundary v= v_s         */

    status=boundary(p_sur,p_pat,p_comp,bcase);
    if (status<0) 
        {
        sprintf(errbuf,"boundary v_s%%varkon_sur_epts (sur920)");
        return(varkon_erpush("SU2973",errbuf));
        }
    all_b_anal[bcase-1] = b_anal;         /* Boundary point case    */

/*!                                                                 */
/*  Find solution points on curve segment V= v_e for U= u_s to u_e  */
/*  (u_s,v_s,u_e,v_e to the varkon_sur_num1 t_in variable)          */
/*  Call of boundary.                                               */
/*                                                                 !*/
    

t_in[0][0] = u_s;        /* Start U                                 */
t_in[0][1] = v_e;        /* Start V                                 */
t_in[1][0] = u_e;        /* End   U                                 */
t_in[1][1] = v_e;        /* End   V                                 */

    bcase      =  4;                     /* Boundary v= v_e         */

    status=boundary(p_sur,p_pat,p_comp,p_etable,bcase);
    if (status<0) 
        {
        sprintf(errbuf,"boundary v_e%%varkon_sur_epts (sur920)");
        return(varkon_erpush("SU2973",errbuf));
        }
    all_b_anal[bcase-1] = b_anal;         /* Boundary point case    */


novlin:;/*!Label:novlin: No constant V lines                       !*/
nosol:  /*!Label nosol:  There is no solution in the patch         !*/

/*!                                                                 */
/*  Printout of calculated entry/exit points         For Debug On   */
/*  Call of pripts (if n_patch > 2 or = 1)                          */
/*                                                                 !*/


#ifdef  DEBUG
  if ( n_patch >  2 ) pripts();
  if ( n_patch == 1 ) pripts();
#endif

/*!                                                                 */
/*  Flag for no points in patch, normal points or two points for    */
/*  an iso-parametric segment (b_flag= 0, 1 and 2 resp.)            */
/*                                                                 !*/
    if         ( n_patch == 0 ) b_flag = 0;
    else if    ( n_patch == 1 ) b_flag = 1;
    else if    ( n_patch  > 2 ) b_flag = 1;
    else if    ( n_patch == 2 ) 
      {
      if (all_b_anal[0] == 2 ||            /*                         */
          all_b_anal[1] == 2 ||            /*                         */
          all_b_anal[2] == 2 ||            /*                         */
          all_b_anal[3] == 2    )          /*                         */
        b_flag = 2;
      else                                 /*                         */
        b_flag = 1;
      }

/*!                                                                 */
/*  Calculate all UV tangents                                       */
/*  Call of varkon_sur_uvtang (sur926).                             */
/*  Tangents from curpt to u1_t,v1_t,u2_t,v2_t.                     */
/*                                                                 !*/

if ( n_patch > 0 )
{
status=varkon_sur_uvtang
   (n_patch,xyzpt,curpt,p_comp,b_flag);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_uvtang(sur926)%%varkon_sur_epts");
        return(varkon_erpush("SU2943",errbuf));
        }
  u1_t=curpt[0].u_t; 
  v1_t=curpt[0].v_t; 
  u2_t=curpt[1].u_t; 
  v2_t=curpt[1].v_t; 
}


   if ( n_patch == 1 )
       {
#ifdef DEBUG
     if ( dbglev(SURPAC) == 1 )
       {
       fprintf(dbgfil(SURPAC),
       "sur920 Only one entry/exit pt u_s %f v_s %f u_e %f v_e %f\n",
           u_s ,v_s ,u_e ,v_e );
       }
#endif
       n_patch = 0; /* ????  1994-04-10  Not handled !!! */
/* !!  goto  nosol;     ????  1994-04-10  Not handled !!! */
       }

   if ( n_patch == 2 )
     {
/*!                                                                 */
    numpts = numpts+1;                   /* Patch with enter/exit pt*/
    (p_etable+numpts-1)->rec_no=numpts;  /* Record number           */
    (p_etable+numpts-1)->u1=u1;          /* Point 1                 */
    (p_etable+numpts-1)->v1=v1;          /* Point 1                 */
    (p_etable+numpts-1)->u2=u2;          /* Point 2                 */
    (p_etable+numpts-1)->v2=v2;          /* Point 2                 */
    (p_etable+numpts-1)->u1_t=u1_t;      /* Tangent to point 1      */
    (p_etable+numpts-1)->v1_t=v1_t;      /*                         */
    (p_etable+numpts-1)->u2_t=u2_t;      /* Tangent to point 2      */
    (p_etable+numpts-1)->v2_t=v2_t;      /*                         */
    (p_etable+numpts-1)->ptr1=0;         /* Ptr to patch with eq. pt*/
    (p_etable+numpts-1)->ptr2=0;         /* Ptr to patch with eq. pt*/
    (p_etable+numpts-1)->us=u_s;         /* Patch limit start point */
    (p_etable+numpts-1)->vs=v_s;         /*                         */
    (p_etable+numpts-1)->ue=u_e;         /* Patch limit end   point */
    (p_etable+numpts-1)->ve=v_e;         /*                         */
    (p_etable+numpts-1)->b_flag= b_flag; /* Boundary   flag         */
    (p_etable+numpts-1)->d_flag=3;       /* Derivative flag         */
/*  Local curpt to EPOINT curpt for n_patch == 2                    */
    for (ix1 = 1; ix1 <= n_patch; ix1 = ix1+1)
       {
       (p_etable+numpts-1)->curpt[ix1-1]=curpt[ix1-1];
       }
/*                                                                 !*/
     }            /* End n_patch == 2                               */

/*  Local curpt to EPOINT curpt for n_patch >  2   1996-09-26       */
   if ( n_patch >  2 )
     {
#ifdef DEBUG
     if ( dbglev(SURPAC) == 1 )
         {
         fprintf(dbgfil(SURPAC),
         "sur920 NOT_EXPECTED_ANY_MORE n_patch= %d numpts= %d\n",
            (int)n_patch, (int)numpts);
         }
#endif

/*  Should be handled in sur900                    1996-09-26       */
/*  The case n_patch > 2 is not handled !!!!!                       */
#ifdef NOT_HANDLED_CASE
     for (ix1 = 1; ix1 <= n_patch; ix1 = ix1+1)
        {
        (p_etable+numpts-1)->curpt[ix1-1]=curpt[ix1-1];
        }
#endif
     }


    if ( numpts > EPMAX )
      {
      sprintf(errbuf, "(EPMAX)%%varkon_sur_epts");
      return(varkon_erpush("SU2993",errbuf));
      }

  }                                      /* End   loop PBOUND       */
/*!                                                                 */
/* End  all records in PBOUND ir=0,1,....,pbn                       */
/*                                                                 !*/

/*!                                                                 */
/* 3. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/* Number of solutions to output variable                           */
   *p_en   = numpts;     
/*                                                                  */
/* Exit with error if *p_en= 0                                      */
/*                                                                 !*/

   if ( *p_en == 0 )
      {
      varkon_erinit();
      sprintf(errbuf,"varkon_sur_epts%% ");
      return(varkon_erpush("SU2783",errbuf));
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  if (numpts > 0 )
    {
    for (ix1 = 1; ix1 <= numpts ; ix1 = ix1+1)
      {
      fprintf(dbgfil(SURPAC),
      "sur920 %d p1 %15.10f %15.10f p2 %15.10f %15.10f\n", (int)ix1,
       (p_etable+ix1-1)->u1,
       (p_etable+ix1-1)->v1,
       (p_etable+ix1-1)->u2,
       (p_etable+ix1-1)->v2   );
      }
    }
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur920 Number of patches with possible solution n_select = %d\n",
     (int)n_select   );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur920 Exit *** varkon_sur_epts **No records in EPOINT*** = %d\n",
     (int)*p_en   );
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!********* Internal ** function ** boundary ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates entry/exit points in one patch           */
/* boundary.                                                        */

static short boundary(p_sur,p_pat,p_comp,bcase)
/*                                                                  */
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur;       /* Surface                           (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */
   IRUNON  *p_comp;      /* Computation data                  (ptr) */
   DBint    bcase;       /* Eq. 1: u0 Eq. 2: u1 Eq. 3: v0 Eq. 4: v1 */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  npts;          /* Number of output points                 */
   EVALS   xyz_a[SMAX];  /* Array with all solutions (R*3 and UV)   */
/*-----------------------------------------------------------------!*/

   DBint  status;        /* Error code from a called function       */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */

/* 1. Initializations and checks                                    */
/*                                                                  */
/* (No checks for the moment)                                       */
/*                                                                 !*/


/*!                                                                 */
/* 2. Calculate entry/exit points                                   */
/*                                                                  */
/* Call of  varkon_sur_num1 (sur900).                               */
/*                                                                 !*/

   status=varkon_sur_num1
   (p_sur,p_pat,t_in,p_comp,&npts,xyz_a,&b_anal);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_num1%%varkon_sur_epts");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/* 3. Entry/exit points to the interrogation tables                 */
/*                                                                  */
/* No solutions nu0, nu1, nv0 or nv1 to the PBOUND table            */
/*                                                                 !*/

   if      ( bcase ==  1 )             /* No solutions to PBOUND    */
     {
     p_cr->nu0 = (short)npts; 
     }
   else if ( bcase ==  2 )
     {
     p_cr->nu1 = (short)npts; 
     }
   else if ( bcase ==  3 )
     {
     p_cr->nv0 = (short)npts; 
     }
   else if ( bcase ==  4 )
     {
     p_cr->nv1 = (short)npts; 
     }

   if ( npts > 0 && n_patch == 0 ) 
     {
     n_patch    = n_patch+1;
     u1         = xyz_a[npts-1].u;
     v1         = xyz_a[npts-1].v; 
     curpt[0].u = xyz_a[npts-1].u; 
     curpt[0].v = xyz_a[npts-1].v; 
     xyzpt[0]   = xyz_a[npts-1];
     npts       = npts - 1;
     }

   if ( npts > 0 && n_patch == 1 ) 
     {
     n_patch    = n_patch+1;
     u2         = xyz_a[npts-1].u; 
     v2         = xyz_a[npts-1].v; 
     curpt[1].u = xyz_a[npts-1].u; 
     curpt[1].v = xyz_a[npts-1].v; 
     xyzpt[1]   = xyz_a[npts-1];
     npts       = npts - 1;
     }
   if ( npts > 0 && n_patch == 2 ) 
     {
     n_patch    = n_patch+1;
     curpt[2].u = xyz_a[npts-1].u; 
     curpt[2].v = xyz_a[npts-1].v; 
     xyzpt[2]   = xyz_a[npts-1];
     npts       = npts - 1;
     }
   if ( npts > 0 && n_patch == 3 ) 
     {
     n_patch    = n_patch+1;
     curpt[3].u = xyz_a[npts-1].u; 
     curpt[3].v = xyz_a[npts-1].v; 
     xyzpt[3]   = xyz_a[npts-1];
     npts       = npts - 1;
     }
   if ( npts > 0 && n_patch == 4 ) 
     {
     n_patch    = n_patch+1;
     curpt[4].u = xyz_a[npts-1].u; 
     curpt[4].v = xyz_a[npts-1].v; 
     xyzpt[4]   = xyz_a[npts-1];
     npts       = npts - 1;
     }
   if ( npts > 0 && n_patch == 5 ) 
     {
     n_patch    = n_patch+1;
     curpt[5].u = xyz_a[npts-1].u; 
     curpt[5].v = xyz_a[npts-1].v; 
     xyzpt[5]   = xyz_a[npts-1];
     npts       = npts - 1;
     }
   if ( npts > 0 && n_patch == 6 ) 
     {
     n_patch    = n_patch+1;
     curpt[6].u = xyz_a[npts-1].u; 
     curpt[6].v = xyz_a[npts-1].v; 
     xyzpt[6]   = xyz_a[npts-1];
     npts       = npts - 1;
     }
   if ( npts > 0 && n_patch == 7 ) 
     {
     n_patch    = n_patch+1;
     curpt[7].u = xyz_a[npts-1].u; 
     curpt[7].v = xyz_a[npts-1].v; 
     xyzpt[7]   = xyz_a[npts-1];
     npts       = npts - 1;
     }
   if ( npts > 0 && n_patch == 8 ) 
     {
     n_patch    = n_patch+1;
     curpt[8].u = xyz_a[npts-1].u; 
     curpt[8].v = xyz_a[npts-1].v; 
     xyzpt[8]   = xyz_a[npts-1];
     npts       = npts - 1;
     }
   if ( npts > 0 && n_patch >= 9 ) 
     {
     sprintf(errbuf, "(n_patch>9)%%varkon_sur_epts");
     return(varkon_erpush("SU2993",errbuf));
     }

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/




/*!New-Page--------------------------------------------------------!*/

/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
#ifdef DEBUG
/*!********* Internal ** function ** pripts *************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function prints out the calculated entry/exit points.        */

static short pripts()
/*                                                                 !*/
/********************************************************************/


/* ------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

/* Out:                                                             */
/*----------------------------------------------------------------- */

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   DBint  ix1;           /* Temporarely used loop index             */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Printout of entry exit points                                 */
/*                                                                 !*/

if ( n_patch == 1 )
  {
  if ( dbglev(SURPAC) == 1 )
    {
    fprintf(dbgfil(SURPAC),
    "sur920 Only one entry/exit pt in patch %5.2f %5.2f %5.2f %5.2f\n"
          , u_s , u_e , v_s , v_e );
    }
  }


if ( dbglev(SURPAC) == 1 )
  {
  if (n_patch > 0 )
    {
    for (ix1 = 1; ix1 <= n_patch ; ix1 = ix1+1)
      {
      fprintf(dbgfil(SURPAC),
      "sur920 Point %d u %10.5f v %10.5f u_t %10.5f v_t %10.5f\n", (int)ix1,
       curpt[ix1-1].u,
       curpt[ix1-1].v,
       curpt[ix1-1].u_t,
       curpt[ix1-1].v_t );
      }
    }
  }

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
#endif


#ifdef DEBUG
/*!********* Internal ** function ** initial ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function initializes the output variables and the            */
/* static (common) variables to 1.23456789 and 123456789.           */

static short initial(p_sur,p_pat,p_comp,p_pbtable,pbn,p_etable,p_en)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur;       /* Surface                           (ptr) */
   DBPatch *p_pat;       /* Alloc. area for topol. patch data (ptr) */
   IRUNON  *p_comp;      /* Computation data                  (ptr) */
   PBOUND  *p_pbtable;   /* Patch boundary table              (ptr) */
   DBint    pbn;         /* Number of records in pbtable            */
   EPOINT  *p_etable;    /* Entry/exit point table            (ptr) */
   DBint   *p_en;        /* Number of records in etable             */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   DBint  ix1;           /* Temporarely used loop index             */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Printout of input variables                                   */
/*                                                                 !*/

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur920 Case of comp. %d  Method %d  No restarts %d Max iter %d\n",
      (int)p_comp->ipl_un.ctype,
      (int)p_comp->ipl_un.method, 
      (int)p_comp->ipl_un.nstart, 
      (int)p_comp->ipl_un.maxiter);
  }

/*!                                                                 */
/* 2. Initiate output variables and static (internal) variables     */
/*                                                                 !*/
     u1         = F_UNDEF;  
     v1         = F_UNDEF;  
     u1_t       = F_UNDEF;  
     v1_t       = F_UNDEF;  
     u2         = F_UNDEF;  
     v2         = F_UNDEF;  
     u2_t       = F_UNDEF;  
     v2_t       = F_UNDEF;  

    for (ix1 = 1; ix1 <=  9      ; ix1 = ix1+1)
      {
       curpt[ix1-1].u   = F_UNDEF;  
       curpt[ix1-1].v   = F_UNDEF;  
       curpt[ix1-1].u_t = F_UNDEF;  
       curpt[ix1-1].v_t = F_UNDEF;  
      }


    *p_en       = I_UNDEF;

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
#endif

