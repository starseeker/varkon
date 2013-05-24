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
/*  Function: varkon_sur_epts2                     File: sur923.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function is a general function for the calculation          */
/*  of entry and exit points in patches defined by the              */
/*  input patch boundary table (PBOUND) which contains data         */
/*  from two surfaces.                                              */
/*                                                                  */
/*  The calulation in this function is one part of a curve          */
/*  creation calculation. Approximative points on the curve         */
/*  may be input to the function. These points will be used         */
/*  as start points for the calculation of the exact points.        */
/*  Approximative points are given in table APOINT.                 */
/*                                                                  */
/*  Input is also a pointer to a structure which defines the        */
/*  calculation criterion for the points (the objective             */
/*  function), the method for the numerical calculation,            */
/*  the number of restarts, the tolerances, etc.                    */
/*  This data is passed on to the general (curve creation)          */
/*  patch interrogation routine.                                    */
/*                                                                  */
/*  The output from the calculation are UV points (on a curve)      */
/*  on both surfaces, i.e the output is two entry/exit points       */
/*  tables. One for each surface.                                   */
/*                                                                  */
/*  So far is this function used only for surface/surface intersect */
/*  calculation. It is not likely that any other types of           */
/*  calculations will be implemented in the near future.....        */
/*                                                                  */
/*                                                                  */
/*  !! F|r att f} upp hastigheter borde PBOUND tabellen g|ras om    */
/*  !! Efter master patch (yta 1) borde alla slav patches (yta 2)   */
/*     som sk{r masterpatchen komma. Detta inneb{r att en patch     */
/*     fr}n slavytan kommer att finnas flera g}nger i patchen       */
/*     men det inneb{r att man vet vilka patches som ska            */
/*     fasetteras och antalet j{mf|releser blir betydligt mindre    */
/*     I denna rutin (om inte startpunkter finns) vet man om        */
/*     ska f|rs|ka g|ra sk{rning mellan tv} patches ....            */
/*     Kanske ska man inte dela upp i dessa faser utan g|ra         */
/*     EN huvudrutin d{r man loopar genom alla faser ....           */
/*     Enklare vore att g|ra en ytterligare tabell med heltal       */
/*     som talar om vilka patches (BOX'es) som kan t{nkas sk{ra     */
/*     varandra. DETTA ALTERNATIV SKA V[LJAS f|r att b{ttra         */
/*     prestanda !                                                  */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-02-06   Originally written                                 */
/*  1996-04-28   b_flag for sur926 (not used for the moment!)       */
/*  1998-09-04   Eliminate compilation warnings                     */
/*  1999-12-18   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_epts2      Entry/exit patch pts, 2 surfaces */
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
#ifdef  DEBUG
static short initial();       /* Initialization of variables        */
#endif
static short boundary();      /* Entry/exit points in one boundary  */
static short isoparam();      /* Create isoparametric boundary      */
static short retrieve();      /* Retrieve computation data          */
static short startpts();      /* Get start points from apts         */
#ifdef  DEBUG_TEMPORARY   
static short pripts();        /* Print entry/exit points            */
#endif
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBCurve iso_cur;           /* Isoparametric curve            */
static DBSeg   iso_seg;           /* Isoparametric curve segment    */
static ISURCUR curdat;            /* Isoparametric curve segment    */
static DBint   numpts;            /* Number of entry/exit records   */
static DBint   n_patch;           /* No entry/exit pts in one patch */
static PBOUND *p_cr_m;            /* Current master pt record (ptr) */
static PBOUND *p_cr_s;            /* Current slave  pt record (ptr) */
static DBint   rec_no_m;          /* Record PBOUND master surface   */
static DBint   rec_no_s;          /* Record PBOUND slave  surface   */
static DBfloat u_s_m;             /* U start point master surface   */
static DBfloat v_s_m;             /* V start point master surface   */
static DBfloat u_e_m;             /* U end   point master surface   */
static DBfloat v_e_m;             /* V end   point master surface   */
static DBfloat u_s_s;             /* U start point slave  surface   */
static DBfloat v_s_s;             /* V start point slave  surface   */
static DBfloat u_e_s;             /* U end   point slave  surface   */
static DBfloat v_e_s;             /* V end   point slave  surface   */
static EVALC   curpt_m[9];        /* All entry/exit curve points    */
static EVALS   xyzpt_m[9];        /* Derivatives corresp. to curpt  */
static DBfloat u1_m,v1_m;         /* Entry/exit point 1 Master sur. */
static DBfloat u2_m,v2_m;         /* Entry/exit point 2 Master sur. */
/*-----------------------------------------------------------------!*/

#ifdef DEBUG
static DBfloat u1_t_m,v1_t_m;     /* Entry/exit point 1 tangent     */
static DBfloat u2_t_m,v2_t_m;     /* Entry/exit point 2 tangent     */
static DBint   n_startpts;        /* Number of start points         */
static DBfloat u_sur[200];        /* U values intersect points      */
static DBfloat v_sur[200];        /* V values intersect points      */
static DBint   n_uvsur;           /* Number of intersect points     */
#endif

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_gmcur        * Initialize DBCurve                     */
/* varkon_ini_gmseg        * Initialize DBSeg                       */
/* varkon_ini_evalc        * Initialize EVALC                       */
/* varkon_ini_evals        * Initialize EVALS                       */
/* varkon_pat_segint       * Segment/patch intersect                */
/* varkon_sur_eval         * Surface evaluation routine             */
/* varkon_sur_uvtang       * Calculate UV tangents                  */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2783 = No solution (no entry/exit pts= 0) varkon_sur_epts2     */
/* SU2943 = Called function   () failed in varkon_sur_epts2(sur923) */
/* SU2973 = Internal function () failed in varkon_sur_epts2(sur923) */
/* SU2993 = Severe program error xxx    in varkon_sur_epts2(sur923) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus    varkon_sur_epts2 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur1,      /* Surface 1                         (ptr) */
   DBPatch *p_pat1,      /* Alloc. area for topol. patch data (ptr) */
   DBptr    p_gm1,       /* DB address to surface 1   (not a C ptr) */
   DBSurf  *p_sur2,      /* Surface 2                         (ptr) */
   DBPatch *p_pat2,      /* Alloc. area for topol. patch data (ptr) */
   DBptr    p_gm2,       /* DB address to surface 2   (not a C ptr) */
   IRUNON  *p_comp,      /* Computation data                  (ptr) */
   PBOUND  *p_pbtable,   /* Patch boundary table              (ptr) */
   DBint    pbn,         /* Number of records in pbtable            */
   APOINT  *p_apts,      /* Facets intersect (start) points   (ptr) */
   DBint    napts,       /* Number of records in apts               */
   EPOINT  *p_etable1,   /* Entry/exit point table, surface 1 (ptr) */
   DBint   *p_en1,       /* Number of records in etable1            */
   EPOINT  *p_etable2,   /* Entry/exit point table, surface 2 (ptr) */
   DBint   *p_en2 )      /* Number of records in etable2            */
/* Out:                                                             */
/*         Enter/exit points in EPOINT table and no records         */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  i_rm;          /* Loop index PBOUND record master surface */
   DBint  i_rs;          /* Loop index PBOUND record slave  surface */
   DBint  sur_no_m;      /* Master surface number                   */
   DBint  sur_no_s;      /* Slave  surface number                   */
   DBint  req_sur_no;    /* Requested (current/master) surface      */
   DBint  ctype;         /* Case of computation                     */
   BBOX   pbox_m;        /* Bounding box  for the (PBOUND) patch    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint  i_i;           /* Loop index record initialization        */
   EVALC  ecrec;         /* Record curve   point and derivatives    */
   EVALS  esrec;         /* Record surface point and derivatives    */
   DBint  bcase;         /* Eq. 1: u0 Eq. 2: u1 Eq. 3: v0 Eq. 4: v1 */
   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint  status;        /* Error code from a called function       */
#ifdef TEMPORARY  
   DBint  sflag;         /* Eq. -1: No solution Eq. 1: Solution     */
   DBint  ix1;           /* Temporarely used loop index             */
   short  b_flag;
#endif

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur923 Enter *** varkon_sur_epts2*Calculate entry/exit points*\n");
fflush(dbgfil(SURPAC)); 
}
n_uvsur = 0 ;           /* Number of intersect points     */
#endif


/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */

/*  Initialize output variables                 for DEBUG on.       */
/*  Call of initial, which also prints out the input data.          */
/*                                                                 !*/

/*  Printout of input data is in function initial                   */

#ifdef DEBUG
    n_startpts = 0;
    status=initial(p_comp,pbn,napts,p_en1,p_en2);
    if (status<0) 
        {
        sprintf(errbuf,"initial%%varkon_sur_epts2(sur923)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif

/* Initialize variabels                                             */
/* Calls of varkon_ini_evalc (sur776) _ini_evals (sur770)           */
/* varkon_ini_gmcur (sur778) and varkon_ini_gmseg (sur779)          */
/*                                                                 !*/

      varkon_ini_gmcur (&iso_cur);

      varkon_ini_gmseg (&iso_seg);

   for (i_i=1; i_i<=  9;   i_i= i_i+1)
     { 
      varkon_ini_evalc (&ecrec);
     curpt_m[i_i-1] = ecrec;
      varkon_ini_evals (&esrec);
     xyzpt_m[i_i-1] = esrec;
     }

/*!                                                                 */
/* Case of computation ctype from p_comp                            */
/*                                                                 !*/

   ctype   = p_comp->isu_un.ctype;

    *p_en1 = 0;
    *p_en2 = 0;

/*!                                                                 */
/* Curve and segment data used for all isoparameter boundary curves */
/*                                                                 !*/

   iso_cur.ns_cu       = (short)1;
   iso_cur.al_cu       = 0.0;        
   iso_cur.plank_cu    = FALSE;      
   iso_cur.hed_cu.type = CURTYP;

   iso_seg.subtyp      = 1;
/* Must be DB patch ptr la for iso crv */
   iso_seg.spek_gm     = DBNULL; 
   iso_seg.spek2_gm    = DBNULL;

   iso_seg.c2x         = 0.0;
   iso_seg.c3x         = 0.0;

   iso_seg.c2y         = 0.0;
   iso_seg.c3y         = 0.0;

   iso_seg.c0z         = 0.0;  
   iso_seg.c1z         = 0.0;
   iso_seg.c2z         = 0.0;
   iso_seg.c3z         = 0.0;

   iso_seg.c0          = 1.0;  
   iso_seg.c1          = 0.0;
   iso_seg.c2          = 0.0;
   iso_seg.c3          = 0.0;

   iso_seg.ofs          = 0.0;
   iso_seg.typ          = UV_CUB_SEG ;
   iso_seg.sl           = 0.0;     

/*!                                                                 */
/* Retrieve computation data. Call of retrieve.                     */
/*                                                                  */
/*                                                                 !*/

    status=retrieve (p_comp);
    if (status<0) 
        {
        sprintf(errbuf,"retrieve%%varkon_sur_epts2(sur923)");
        return(varkon_erpush("SU2973",errbuf));
        }

/*!                                                                 */
/* 2. Calculate all entry exit points                               */
/* __________________________________                               */
/*                                                                  */
/*                                                                 !*/

   req_sur_no = 1;       /* Requested (current/master) surface      */

/*!                                                                 */
/* Initiate loop variables:                                         */
   numpts = 0;
/* Loop all records in PBOUND table i_rm=0,1,....,pbn               */
/*                                                                 !*/

for ( i_rm=0; i_rm< pbn; ++i_rm )        /* Start loop PBOUND       */
  {                                      /* master surface          */

    n_patch = 0;                         /* Initialize number of    */
                                         /* entry/exit pts in the   */
                                         /* current (master) patch  */

/*!                                                                 */
/*  Master surface data from the PBOUND table:                      */
    p_cr_m= p_pbtable + i_rm;            /* Ptr to current record   */
    rec_no_m = p_cr_m->rec_no;           /* Record number           */
    u_s_m    = p_cr_m->us;               /* U start point           */
    v_s_m    = p_cr_m->vs;               /* V start point           */
    u_e_m    = p_cr_m->ue;               /* U end   point           */
    v_e_m    = p_cr_m->ve;               /* V end   point           */
    sur_no_m = p_cr_m->sur_no;           /* Surface number          */
    pbox_m   = p_cr_m->pbox;             /* BBOX                    */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur923 i_rm %d sur_no_m %d req_sur_no %d\n", 
       (int)i_rm, (int)sur_no_m , (int)req_sur_no );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur923 rec_no_m %d p_cr_m %d u_s_m %6.1f v_s_m %6.1f u_e_m %6.1f v_e_m %6.1f\n",
      (int)rec_no_m , (int)p_cr_m , u_s_m ,v_s_m ,u_e_m ,v_e_m );
  }
#endif

/*!                                                                 */
/*  Goto nxtregm if not the requested master surface.               */
/*                                                                 !*/

    if ( sur_no_m != req_sur_no ) goto nxtregm; /* Not master surface */

/* Loop all records in PBOUND table i_rs=0,1,....,pbn               */
/*                                                                 !*/

for ( i_rs=0; i_rs< pbn; ++i_rs )        /* Start loop PBOUND       */
  {                                      /* slave  surface          */


/*!                                                                 */
/*  Slave  surface data from the PBOUND table:                      */
    p_cr_s= p_pbtable + i_rs;            /* Ptr to current record   */
    rec_no_s = p_cr_s->rec_no;           /* Record number           */
    u_s_s    = p_cr_s->us;               /* U start point           */
    v_s_s    = p_cr_s->vs;               /* V start point           */
    u_e_s    = p_cr_s->ue;               /* U end   point           */
    v_e_s    = p_cr_s->ve;               /* V end   point           */
    sur_no_s = p_cr_s->sur_no;           /* Surface number          */
/*                                                                 !*/




    if ( sur_no_s == req_sur_no ) goto nxtregs; /* Not slave  surface */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur923 i_rm %d i_rs %d sur_no_m %d sur_no_s %d req_sur_no %d\n", 
       (int)i_rm, (int)i_rs, (int)sur_no_m, (int)sur_no_s, (int)req_sur_no );
  }
#endif

#ifdef DEBUG
/* Check u_s_m, .....                                               */
  if      (  req_sur_no == 1 )                 /* Surface number 1  */
    {
    if ( u_s_m  <     1.0         ||           /* u_s_m is not OK   */
         u_s_m  > (DBfloat)p_sur1->nu_su + 1.0 )
      {
      goto  pb_error;
      }
    if ( u_e_m  <     1.0         ||           /* u_e_m is not OK   */
         u_e_m  > (DBfloat)p_sur1->nu_su + 1.0 )
      {
      goto  pb_error;
      }
    if ( v_s_m  <     1.0         ||           /* v_s_m is not OK   */
         v_s_m  > (DBfloat)p_sur1->nv_su + 1.0 )
      {
      goto  pb_error;
      }
    if ( v_e_m  <     1.0         ||           /* v_e_m is not OK   */
         v_e_m  > (DBfloat)p_sur1->nv_su + 1.0 )
      {
      goto  pb_error;
      }
    }      /* End req_sur_no == 1                                   */
  else if (  req_sur_no == 2 )                 /* Surface number 2  */
    {
    if ( u_s_m  <     1.0         ||           /* u_s_m is not OK   */
         u_s_m  > (DBfloat)p_sur2->nu_su + 1.0 )
      {
      goto  pb_error;
      }
    if ( u_e_m  <     1.0         ||           /* u_e_m is not OK   */
         u_e_m  > (DBfloat)p_sur2->nu_su + 1.0 )
      {
      goto  pb_error;
      }
    if ( v_s_m  <     1.0         ||           /* v_s_m is not OK   */
         v_s_m  > (DBfloat)p_sur2->nv_su + 1.0 )
      {
      goto  pb_error;
      }
    if ( v_e_m  <     1.0         ||           /* v_e_m is not OK   */
         v_e_m  > (DBfloat)p_sur2->nv_su + 1.0 )
      {
      goto  pb_error;
      }
    }      /* End req_sur_no == 2                                   */


goto no_error;   /* No errors found for u_s_m, ......                */

pb_error:; /* There is an error for u_s_m, .....                     */

if ( dbglev(SURPAC) == 1 )
{
  fprintf(dbgfil(SURPAC),
  "sur923 Error u_s_m, v_sm, ...  nu1 %d nv1 %d nu2 %d nv2 %d\n",
   (int)p_sur1->nu_su, (int)p_sur1->nv_su, 
   (int)p_sur2->nu_su, (int)p_sur2->nv_su );
}
        sprintf(errbuf,"u_s_m, u_s ..not OK%%sur923");
        return(varkon_erpush("SU2993",errbuf));

no_error:; /* There is no error in the PBOUND table                  */


#endif

/*!                                                                 */
/*  Find solution pts on crv segment U= u_s_m for V= v_s_m to v_e_m */
/*  Call of startpts, isoparameter and boundary.                    */
/*  For the moment will the boundary calculation only be made if    */
/*  there is a start point for the calculation !!!!                 */
/*                                                                 !*/
    
    bcase      =  1;                   /* Boundary u= u_s_m         */

    status= startpts ( p_apts, napts, sur_no_m, bcase );
    if (status<0) 
        {
        sprintf(errbuf,"startpts u_s_m%%varkon_sur_epts2(sur923)");
        return(varkon_erpush("SU2973",errbuf));
        }


    if ( curdat.startuvpt.x_gm > 0.0 )
      {
      status= isoparam  (bcase);
      if (status<0) 
          {
          sprintf(errbuf,"isoparam u_s_m%%varkon_sur_epts2(sur923)");
          return(varkon_erpush("SU2973",errbuf));
          }

      status=boundary(p_sur1,p_pat1,p_gm1,p_sur2,p_pat2,p_gm2,bcase,sur_no_m);
      if (status<0) 
          {
          sprintf(errbuf,"boundary u0%%varkon_sur_epts2(sur923)");
          return(varkon_erpush("SU2973",errbuf));
          }
       }          /* End curdat.startuvpt.x_gm > 0.0  */


/*!                                                                 */
/*  Find solution pts on crv segment U= u_e_m for V= v_s_m to v_e_m */
/*  Call of startpts, isoparameter and boundary.                    */
/*  For the momemt will the boundary calculation only be made if    */
/*  there is a start point for the calculation !!!!                 */
/*                                                                 !*/
    

    bcase      =  2;                     /* Boundary u= u_e_m         */

    status= startpts (p_apts, napts, sur_no_m, bcase );
    if (status<0) 
        {
        sprintf(errbuf,"startpts u_e_m%%varkon_sur_epts2(sur923)");
        return(varkon_erpush("SU2973",errbuf));
        }

    if ( curdat.startuvpt.x_gm > 0.0 )
      {
      status= isoparam  (bcase);
      if (status<0) 
          {
          sprintf(errbuf,"isoparam u_e_m%%varkon_sur_epts2(sur923)");
          return(varkon_erpush("SU2973",errbuf));
          }

      status=boundary(p_sur1,p_pat1,p_gm1,p_sur2,p_pat2,p_gm2,bcase,sur_no_m);
      if (status<0) 
          {
          sprintf(errbuf,"boundary u_e_m%%varkon_sur_epts2(sur923)");
          return(varkon_erpush("SU2973",errbuf));
          }
       }

/*!                                                                 */
/*  Find solution pts on crv segment V= v_s_m for U= u_s_m to u_e_m */
/*  Call of startpts, isoparameter and boundary.                    */
/*  For the momemt will the boundary calculation only be made if    */
/*  there is a start point for the calculation !!!!                 */
/*                                                                 !*/
 
    bcase      =  3;                     /* Boundary v= v_s_m         */

    status= startpts ( p_apts, napts, sur_no_m, bcase );
    if (status<0) 
        {
        sprintf(errbuf,"startpts v_s_m%%varkon_sur_epts2(sur923)");
        return(varkon_erpush("SU2973",errbuf));
        }

    if ( curdat.startuvpt.x_gm > 0.0 )
      {
      status= isoparam  (bcase);
      if (status<0) 
          {
          sprintf(errbuf,"isoparam v_s_m%%varkon_sur_epts2(sur923)");
          return(varkon_erpush("SU2973",errbuf));
          }

      status=boundary(p_sur1,p_pat1,p_gm1,p_sur2,p_pat2,p_gm2,bcase,sur_no_m);
      if (status<0) 
          {
          sprintf(errbuf,"boundary v_s_m%%varkon_sur_epts2(sur923)");
          return(varkon_erpush("SU2973",errbuf));
          }
       }

/*!                                                                 */
/*  Find solution pts on crv segment V= v_e_m for U= u_s_m to u_e_m */
/*  Call of startpts, isoparameter and boundary.                    */
/*  For the momemt will the boundary calculation only be made if    */
/*  there is a start point for the calculation !!!!                 */
/*                                                                 !*/
    bcase      =  4;                     /* Boundary v= v_e_m         */

    status= startpts ( p_apts, napts, sur_no_m, bcase );
    if (status<0) 
        {
        sprintf(errbuf,"startpts v_e_m%%varkon_sur_epts2(sur923)");
        return(varkon_erpush("SU2973",errbuf));
        }

    if ( curdat.startuvpt.x_gm > 0.0 )
      {
      status= isoparam  (bcase);
      if (status<0) 
          {
          sprintf(errbuf,"isoparam v_e_m%%varkon_sur_epts2(sur923)");
          return(varkon_erpush("SU2973",errbuf));
          }

      status=boundary(p_sur1,p_pat1,p_gm1,p_sur2,p_pat2,p_gm2,bcase,sur_no_m);
      if (status<0) 
          {
          sprintf(errbuf,"boundary v_e_m%%varkon_sur_epts2(sur923)");
          return(varkon_erpush("SU2973",errbuf));
          }
       }


/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/


#ifdef  TILLSVIDARE
nosol:  /*!Label nosol:  There is no solution in the patch         !*/
#endif



/*!                                                                 */
/*  Printout of calculated entry/exit points         For Debug On   */
/*  Call of pripts (if n_patch > 2 or = 1)                          */
/*                                                                 !*/


#ifdef  DEBUG_TILLFALLIG  
  if ( n_patch >  2 ) pripts();
  if ( n_patch == 1 ) pripts();
#endif

#ifdef TILLFALLIGT

/*!                                                                 */
/*  Calculate all UV tangents                                       */
/*  Call of varkon_sur_uvtang (sur926).                             */
/*  Tangents from curpt_m to u1_t_m,v1_t_m,u2_t_m,v2_t_m.                     */
/*                                                                 !*/

b_flag = 1;  /*  1996-04-29 */
if ( n_patch > 0 )
{
status=varkon_sur_uvtang
   (n_patch,xyzpt_m,curpt_m,p_comp,b_flag);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_uvtang(sur926)%%varkon_sur_epts2");
        return(varkon_erpush("SU2943",errbuf));
        }
  u1_t_m=curpt_m[0].u_t; 
  v1_t_m=curpt_m[0].v_t; 
  u2_t_m=curpt_m[1].u_t; 
  v2_t_m=curpt_m[1].v_t; 
}

   if ( n_patch == 1 )
       {
#ifdef DEBUG
     if ( dbglev(SURPAC) == 1 )
       {
       fprintf(dbgfil(SURPAC),
       "sur923 Only one entry/exit pt u_s_m %f v_s_m %f u_e_m %f v_e_m %f\n",
           u_s_m ,v_s_m ,u_e_m ,v_e_m );
       }
#endif
       n_patch = 0; /* ????  1994-04-10  Ej hanterat !!! */
/* !!  goto  nosol;     ????  1994-04-10  Ej hanterat !!! */
       }

   if ( n_patch == 2 )
     {
/*!                                                                 */
    numpts = numpts+1;                   /* Patch with enter/exit pt*/
    (p_etable1+numpts-1)->rec_no=numpts; /* Record number           */
    (p_etable1+numpts-1)->u1=u1_m;         /* Point 1                 */
    (p_etable1+numpts-1)->v1=v1_m;         /* Point 1                 */
    (p_etable1+numpts-1)->u2=u2_m;         /* Point 2                 */
    (p_etable1+numpts-1)->v2=v2_m;         /* Point 2                 */
    (p_etable1+numpts-1)->u1_t=u1_t_m;     /* Tangent to point 1      */
    (p_etable1+numpts-1)->v1_t=v1_t_m;     /*                         */
    (p_etable1+numpts-1)->u2_t=u2_t_m;     /* Tangent to point 2      */
    (p_etable1+numpts-1)->v2_t=v2_t_m;     /*                         */
    (p_etable1+numpts-1)->ptr1=0;        /* Ptr to patch with eq. pt*/
    (p_etable1+numpts-1)->ptr2=0;        /* Ptr to patch with eq. pt*/
    (p_etable1+numpts-1)->us=u_s_m;      /* Patch limit start point */
    (p_etable1+numpts-1)->vs=v_s_m;      /* master ?????            */
    (p_etable1+numpts-1)->ue=u_e_m;      /* Patch limit end   point */
    (p_etable1+numpts-1)->ve=v_e_m;      /*                         */
    (p_etable1+numpts-1)->b_flag=1;      /* Boundary   flag         */
    (p_etable1+numpts-1)->d_flag=3;      /* Derivative flag         */
/*  Local curpt_m to EPOINT curpt_m for n_patch >= 2                    */
/*                                                                 !*/
     }            /* End n_patch == 2                               */

   if ( n_patch >= 2 )
     {
     for (ix1 = 1; ix1 <= n_patch; ix1 = ix1+1)
        {
        (p_etable1+numpts-1)->curpt_m[ix1-1]=curpt_m[ix1-1];
        }
     }


    if ( numpts > EPSMA )
      {
      sprintf(errbuf,                    /* Error SU2993 if maxim.  */
      "(EPSMA)%%varkon_sur_epts2");       /* number of points is     */
  return(varkon_erpush("SU2993",errbuf));/* exceeded                */
      }

#endif /*  TILLFALLIGT  */


nxtregs:; /* Label: Not the requested slave  surface */

  }                                      /* End  loop PBOUND slave  */
/*!                                                                 */
/* End  all records in PBOUND for slave  surface i_rs=0,1,....,pbn  */
/*                                                                 !*/

nxtregm:; /* Label: Not the requested master surface */

  }                                      /* End  loop PBOUND master */
/*!                                                                 */
/* End  all records in PBOUND for master surface i_rm=0,1,....,pbn  */
/*                                                                 !*/


/*!                                                                 */
/* 3. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/* Number of solutions to output variable                           */
   *p_en1   = numpts;     
/*                                                                  */
/* Exit with error if *p_en1= 0                                     */
/*                                                                 !*/

   if ( *p_en1 == 0 )
      {
      sprintf(errbuf," %d%%varkon_sur_epts2",(int)*p_en1);
      return(varkon_erpush("SU2783",errbuf));
      }



#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur923 Exit*varkon_sur_epts2*No records in EPOINT tables %d and %d (EPSMA %d)\n",
     (int)*p_en1, (int)*p_en2 , EPSMA  );
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
/* The function calculates entry/exit points for one of the patch   */
/* boundaries of the master surface. The patch boundary is defined  */
/* as a one-segment isoparameter curve.                             */
/*                                                                  */
/* The entry/exit points are the intersect points between the       */
/* the isoparameter curve segment and one given patch of the        */
/* slave surface. All possible (selected) patches in the slave      */
/* surface will be considered by the loop in the main program.      */
/* In this function will the calculation be made in only one        */
/* patch. The case that the isoparameter segment intersects         */
/* multiple patches of the slave surface is probably not taken      */
/* care of ....                                                     */
/*                                                                  */
/* There is normally only one intersect point between the patch     */
/* and the isoparameter curve. An intersect point is added to       */
/* two arrays. One for the master and one for the slave surface.    */
/* These two arrays hold all intersect points for the four          */
/* boundaries for a patch in the master surface (intersects from    */
/* four isoparameter segments). The majority of master surface      */
/* patches will have two entry/exit points (variable n_patch).      */
/*                                                                  */
/* The normal case for only one entry/exit point will be that       */
/* the surface/surface intersect points starts in a point inside    */
/* the master surface patch. This point will be calculated in the   */
/* second phase, when the master surface is the slave surface ...   */
/*                                                                  */
/* The tangents in the U,V planes (for the master and the slave     */
/* surface) will also be calculated before data is put in the       */
/* two arrays described above.                                      */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                                                                  */

static short boundary(p_sur1,p_pat1,p_gm1,p_sur2,p_pat2,p_gm2,bcase,sur_no_m)
/*                                                                  */
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur1;      /* Surface 1                         (ptr) */
   DBPatch *p_pat1;      /* Alloc. area for topol. patch data (ptr) */
   DBptr    p_gm1;       /* DB address to surface 1   (not a C ptr) */
   DBSurf  *p_sur2;      /* Surface 2                         (ptr) */
   DBPatch *p_pat2;      /* Alloc. area for topol. patch data (ptr) */
   DBptr    p_gm2;       /* DB address to surface 2   (not a C ptr) */
   DBint    bcase;       /* Eq. 1: u0 Eq. 2: u1 Eq. 3: v0 Eq. 4: v1 */
   DBint    sur_no_m;    /* Master surface number (1 or 2)          */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat u_int_m[SMAX];/* All intersect pts in master surface U   */
   DBfloat v_int_m[SMAX];/* All intersect pts in master surface V   */
   DBfloat u_int_s[SMAX];/* All intersect pts in slave  surface U   */
   DBfloat v_int_s[SMAX];/* All intersect pts in slave  surface V   */
   EVALS  xyz_ps_m[SMAX];/* All patch/segment points Master surface */
   EVALS  xyz_ps_s[SMAX];/* All patch/segment points Slave  surface */
   EVALC  xyz_psc[SMAX]; /* All patch/segment points Curve          */
   DBint  n_pspts;       /* Number of patch/segment points          */
/*-----------------------------------------------------------------!*/

   DBSurf  *p_sur_m;     /* Master surface                    (ptr) */
   DBPatch *p_pat_m;     /* Master surface topologic. patches (ptr) */
   DBSurf  *p_sur_s;     /* Slave  surface                    (ptr) */
   DBPatch *p_pat_s;     /* Slave  surface topologic. patches (ptr) */
   DBint    i_s;         /* Loop index solution point               */
   DBint    icase;       /* Calculation case for varkon_sur_eval    */
   DBint    status;      /* Error code from a called function       */
   char     errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */

/* 1. Initiatializations and checks                                 */
/*                                                                  */
/* (No checks for the moment)                                       */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur923*boundary  sur_no_m %d bcase %d p_gm1 %d  p_gm2 %d \n" , 
                (int)sur_no_m,  (int)bcase, (int)p_gm1, (int)p_gm2  );
fflush(dbgfil(SURPAC));
}
#endif

  if ( sur_no_m == 1 )
    {
    p_sur_s = p_sur2;
    p_pat_s = p_pat2;
    iso_seg.spek_gm = p_gm1; 
    p_sur_m = p_sur1;
    p_pat_m = p_pat1;
    }
  else
    {
    p_sur_s = p_sur1;
    p_pat_s = p_pat1;
    iso_seg.spek_gm = p_gm2;
    p_sur_m = p_sur2;
    p_pat_m = p_pat2;
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur923*boundary sur_no_m %d  p_gm1 %d  p_gm2 %d iso_seg.spek_gm %d\n" , 
    (int)sur_no_m , (int)p_gm1, (int)p_gm2, (int)iso_seg.spek_gm  );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 2. Calculate entry/exit points                                   */
/*                                                                  */
/* Call of  varkon_pat_segint (sur904).                             */
/*                                                                 !*/

    status = varkon_pat_segint
        (p_sur_s,p_pat_s,&iso_cur,&iso_seg,(IRUNON*)&curdat,
        &n_pspts,xyz_ps_s,xyz_psc);
if (status<0) 
  {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur923*boundary  sur904 failed Tillfalligt return(SUCCED) \n" );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
return(SUCCED);  /* !!!!!!!!  Tillfalligt !!!!!!!  */
#endif
  sprintf(errbuf,"varkon_pat_segint%%varkon_sur_epts2");
  return(varkon_erpush("SU2943",errbuf));
  }

  for (i_s = 1; i_s <= n_pspts; i_s = i_s+1)
    {
    if      ( bcase == 1 )
      {
      u_int_m[i_s-1] = u_s_m; 
      v_int_m[i_s-1] = v_s_m + xyz_psc[i_s-1].t_global - 1.0; 
      }
    else if ( bcase == 2 )
      {
      u_int_m[i_s-1] = u_e_m; 
      v_int_m[i_s-1] = v_s_m + xyz_psc[i_s-1].t_global - 1.0; 
      }
    else if ( bcase == 3 )
      {
      u_int_m[i_s-1] = v_s_m; 
      v_int_m[i_s-1] = u_s_m + xyz_psc[i_s-1].t_global - 1.0; 
      }
    else if ( bcase == 4 )
      {
      u_int_m[i_s-1] = v_e_m; 
      v_int_m[i_s-1] = u_s_m + xyz_psc[i_s-1].t_global - 1.0; 
      }
    u_int_s[i_s-1] = xyz_ps_s[i_s-1].u;  /* Not used for the moment */
    v_int_s[i_s-1] = xyz_ps_s[i_s-1].u;  /* Not used for the moment */
       icase = 3;  
       status=varkon_sur_eval
       (p_sur_m,p_pat_m,icase,u_int_m[i_s-1],v_int_m[i_s-1],&xyz_ps_m[i_s-1]);
       if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_eval%%sur923");
        return(varkon_erpush("SU2943",errbuf));
        }
    }

#ifdef DEBUG
for (i_s = 1; i_s <= n_pspts; i_s = i_s+1)
{
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur923*boundary  Intersect point %d X %10.2f Y %10.2f Z %10.2f (Master)\n" , 
     (int)i_s, xyz_ps_m[0].r_x , xyz_ps_m[0].r_y , xyz_ps_m[0].r_z ); 
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur923*boundary  Intersect point %d X %10.2f Y %10.2f Z %10.2f (Slave )\n" , 
     (int)i_s, xyz_ps_s[0].r_x , xyz_ps_s[0].r_y , xyz_ps_s[0].r_z ); 
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur923*boundary  Intersect point %d U %6.3f V %6.3f (Master)\n" , 
     (int)i_s, xyz_ps_m[0].u , xyz_ps_m[0].v ); 
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur923*boundary  Intersect point %d U %6.3f V %6.3f (Slave )\n" , 
     (int)i_s, xyz_ps_s[0].u , xyz_ps_s[0].v ); 
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
}
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 && n_pspts > 0 )
{
fprintf(dbgfil(SURPAC),
"sur923*boundary  Intersect point U %6.3f V %6.3f (n_pspts %d)\n" , 
     xyz_ps_s[0].u , xyz_ps_s[0].v,  (int)n_pspts ); 
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 1 && n_pspts == 0 )
{
fprintf(dbgfil(SURPAC),
"sur923*boundary  No intersect between curve segm. and surface patch \n"); 
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 1 && n_pspts == 0 )
{
fprintf(dbgfil(SURPAC),
"CUR_ISO(#101,s2_id,qUq,%8.5f,%8.5f,%8.5f:PEN=4); \n",
  u_s_s-1.0,v_s_s-1.0,v_e_s-1.0); 
fprintf(dbgfil(SURPAC),
"CUR_ISO(#102,s2_id,qUq,%8.5f,%8.5f,%8.5f:PEN=4); \n",
  u_e_s-1.0,v_s_s-1.0,v_e_s-1.0); 
fprintf(dbgfil(SURPAC),
"CUR_ISO(#103,s2_id,qVq,%8.5f,%8.5f,%8.5f:PEN=4); \n",
  v_s_s-1.0,u_s_s-1.0,u_e_s-1.0); 
fprintf(dbgfil(SURPAC),
"CUR_ISO(#104,s2_id,qVq,%8.5f,%8.5f,%8.5f:PEN=4); \n",
  v_e_s-1.0,u_s_s-1.0,u_e_s-1.0); 

if ( bcase == 1 )
fprintf(dbgfil(SURPAC),
"CUR_ISO(#105,s1_id,qUq,%8.5f,%8.5f,%8.5f:PEN=3); \n",
  u_s_m-1.0,v_s_m-1.0,v_e_m-1.0); 
if ( bcase == 2 )
fprintf(dbgfil(SURPAC),
"CUR_ISO(#106,s1_id,qUq,%8.5f,%8.5f,%8.5f:PEN=3); \n",
  u_e_m-1.0,v_s_m-1.0,v_e_m-1.0); 
if ( bcase == 3 )
fprintf(dbgfil(SURPAC),
"CUR_ISO(#107,s1_id,qVq,%8.5f,%8.5f,%8.5f:PEN=3); \n",
  v_s_m-1.0,u_s_m-1.0,u_e_m-1.0); 
if ( bcase == 4 )
fprintf(dbgfil(SURPAC),
"CUR_ISO(#108,s1_id,qVq,%8.5f,%8.5f,%8.5f:PEN=3); \n",
  v_e_m-1.0,u_s_m-1.0,u_e_m-1.0); 
fprintf(dbgfil(SURPAC),
"POI_FREE(#109,ON(#10?,%8.5f):PEN=3); \n",
    curdat.startupt.x_gm - 1.0  ); 
fprintf(dbgfil(SURPAC),
"POI_FREE(#110,ON(s2_id,VEC(%8.5f,%8.5f,0)):PEN=4); \n",
  curdat.startuvpt.x_gm - 1.0 , curdat.startuvpt.y_gm - 1.0 ); 

fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( n_pspts > 0 )
{
n_uvsur = n_uvsur + 1;            /* Number of intersect points     */
u_sur[n_uvsur-1] = xyz_ps_s[0].u;   /* U values intersect points      */
v_sur[n_uvsur-1] = xyz_ps_s[0].v;   /* V values intersect points      */
}
#endif


/*!                                                                 */
/* 3. Entry/exit points to the interrogation tables                 */
/*                                                                  */
/* No of solutions nu0, nu1, nv0 or nv1 to the PBOUND table         */
/*                                                                 !*/

   if      ( bcase ==  1 )          /* No of solutions to PBOUND    */
     {
     p_cr_m->nu0 = (short)n_pspts; 
     }
   else if ( bcase ==  2 )
     {
     p_cr_m->nu1 = (short)n_pspts; 
     }
   else if ( bcase ==  3 )
     {
     p_cr_m->nv0 = (short)n_pspts; 
     }
   else if ( bcase ==  4 )
     {
     p_cr_m->nv1 = (short)n_pspts; 
     }

   if ( n_pspts > 0 && n_patch == 0 ) 
     {
     n_patch    = n_patch+1;
     u1_m       = xyz_ps_m[n_pspts-1].u; 
     v1_m       = xyz_ps_m[n_pspts-1].v; 
     curpt_m[0].u = xyz_ps_m[n_pspts-1].u; 
     curpt_m[0].v = xyz_ps_m[n_pspts-1].v; 
     xyzpt_m[0]   = xyz_ps_m[n_pspts-1];
     n_pspts       = n_pspts - 1;
     }

   if ( n_pspts > 0 && n_patch == 1 ) 
     {
     n_patch    = n_patch+1;
     u2_m       = xyz_ps_m[n_pspts-1].u; 
     v2_m       = xyz_ps_m[n_pspts-1].v; 
     curpt_m[1].u = xyz_ps_m[n_pspts-1].u; 
     curpt_m[1].v = xyz_ps_m[n_pspts-1].v; 
     xyzpt_m[1]   = xyz_ps_m[n_pspts-1];
     n_pspts       = n_pspts - 1;
     }
   if ( n_pspts > 0 && n_patch == 2 ) 
     {
     n_patch    = n_patch+1;
     curpt_m[2].u = xyz_ps_m[n_pspts-1].u; 
     curpt_m[2].v = xyz_ps_m[n_pspts-1].v; 
     xyzpt_m[2]   = xyz_ps_m[n_pspts-1];
     n_pspts       = n_pspts - 1;
     }
   if ( n_pspts > 0 && n_patch == 3 ) 
     {
     n_patch    = n_patch+1;
     curpt_m[3].u = xyz_ps_m[n_pspts-1].u; 
     curpt_m[3].v = xyz_ps_m[n_pspts-1].v; 
     xyzpt_m[3]   = xyz_ps_m[n_pspts-1];
     n_pspts       = n_pspts - 1;
     }
   if ( n_pspts > 0 && n_patch == 4 ) 
     {
     n_patch    = n_patch+1;
     curpt_m[4].u = xyz_ps_m[n_pspts-1].u; 
     curpt_m[4].v = xyz_ps_m[n_pspts-1].v; 
     xyzpt_m[4]   = xyz_ps_m[n_pspts-1];
     n_pspts       = n_pspts - 1;
     }
   if ( n_pspts > 0 && n_patch == 5 ) 
     {
     n_patch    = n_patch+1;
     curpt_m[5].u = xyz_ps_m[n_pspts-1].u; 
     curpt_m[5].v = xyz_ps_m[n_pspts-1].v; 
     xyzpt_m[5]   = xyz_ps_m[n_pspts-1];
     n_pspts       = n_pspts - 1;
     }
   if ( n_pspts > 0 && n_patch == 6 ) 
     {
     n_patch    = n_patch+1;
     curpt_m[6].u = xyz_ps_m[n_pspts-1].u; 
     curpt_m[6].v = xyz_ps_m[n_pspts-1].v; 
     xyzpt_m[6]   = xyz_ps_m[n_pspts-1];
     n_pspts       = n_pspts - 1;
     }
   if ( n_pspts > 0 && n_patch == 7 ) 
     {
     n_patch    = n_patch+1;
     curpt_m[7].u = xyz_ps_m[n_pspts-1].u; 
     curpt_m[7].v = xyz_ps_m[n_pspts-1].v; 
     xyzpt_m[7]   = xyz_ps_m[n_pspts-1];
     n_pspts       = n_pspts - 1;
     }
   if ( n_pspts > 0 && n_patch == 8 ) 
     {
     n_patch    = n_patch+1;
     curpt_m[8].u = xyz_ps_m[n_pspts-1].u; 
     curpt_m[8].v = xyz_ps_m[n_pspts-1].v; 
     xyzpt_m[8]   = xyz_ps_m[n_pspts-1];
     n_pspts       = n_pspts - 1;
     }
   if ( n_pspts > 0 && n_patch >= 9 ) 
     {
     sprintf(errbuf,                    /* Error SU2993 if there   */
     "(n_patch>9)%%varkon_sur_epts2");   /* are more than nine      */
return(varkon_erpush("SU2993",errbuf)); /* solution points         */
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1  )
{
fprintf(dbgfil(SURPAC),
"sur923*boundary  Number of intersect points n_patch %d\n" , 
                  (int)n_patch);
fflush(dbgfil(SURPAC)); 
}
#endif


    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/




/*!New-Page--------------------------------------------------------!*/

/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
#ifdef DEBUG_TILLVIDARE
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
"sur923*pripts Only one entry/exit pt in patch %5.2f %5.2f %5.2f %5.2f\n"
      , u_s_m , u_e_m , v_s_m , v_e_m );
}
}


if ( dbglev(SURPAC) == 1 )
{
if (n_patch > 0 )
{
for (ix1 = 1; ix1 <= n_patch ; ix1 = ix1+1)
{
fprintf(dbgfil(SURPAC),
"sur923*pripts Point %d u %10.5f v %10.5f u_t %10.5f v_t %10.5f\n", 
(int)ix1,
curpt_m[ix1-1].u,
curpt_m[ix1-1].v,
curpt_m[ix1-1].u_t,
curpt_m[ix1-1].v_t );
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
/* static (common) variables to F_UNDEF and I_UNDEF.                */

static short initial(p_comp,pbn,napts,p_en1,p_en2)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   IRUNON  *p_comp;      /* Computation data                  (ptr) */
   DBint    pbn;         /* Number of records in pbtable            */
   DBint    napts;       /* Number of records in apts               */
   DBint   *p_en1;       /* Number of records in etable1            */
   DBint   *p_en2;       /* Number of records in etable2            */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint  ix1;           /* Temporarely used loop index             */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Printout of input variables                                   */
/*                                                                 !*/

/* For new types of calculation. Add check here and check that      */
/* current retrieved data (maxiter, ctype, nu, .. ) also are        */
/* defined in the new structure variable. If not add IF-THEN        */
/* statements for the different types of calculations               */

if      ( p_comp->icu_un.ctype == SURSUR  )
{
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur923*initial Case of computation SURSUR (ctype= %d)\n",
      (int)p_comp->isu_un.ctype);
  }
}
   else
       {
       sprintf(errbuf,"(ctype)%% varkon_sur_epts2 (sur923)");
       return(varkon_erpush("SU2993",errbuf));
       }

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur923*initial Case of comp. %d  Method %d  No restarts %d Max iter %d\n",
      (int)p_comp->isu_un.ctype,
      (int)p_comp->isu_un.method, 
      (int)p_comp->isu_un.nstart, 
      (int)p_comp->isu_un.maxiter);
  }

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur923*initial Number of records in tables pbtable %d and in apts %d\n",
      (int)pbn, (int)napts);
  }

/*!                                                                 */
/* 2. Initiate output variables and static (internal) variables     */
/*                                                                 !*/
     u1_m         = F_UNDEF;   
     v1_m         = F_UNDEF;   
     u1_t_m       = F_UNDEF;   
     v1_t_m       = F_UNDEF;   
     u2_m         = F_UNDEF;   
     v2_m         = F_UNDEF;   
     u2_t_m       = F_UNDEF;   
     v2_t_m       = F_UNDEF;   

    for (ix1 = 1; ix1 <=  9      ; ix1 = ix1+1)
      {
       curpt_m[ix1-1].u   = F_UNDEF;   
       curpt_m[ix1-1].v   = F_UNDEF;   
       curpt_m[ix1-1].u_t = F_UNDEF;   
       curpt_m[ix1-1].v_t = F_UNDEF;   
      }


    *p_en1       = I_UNDEF;     
    *p_en2       = I_UNDEF;     

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
#endif


/*!New-Page--------------------------------------------------------!*/

/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!********* Internal ** function ** isoparam ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates the current isoparameter boundary curve     */
/* on the master surface. The curve is a one-segment curve.         */
/* Note that the limits (curdat.usc= 1 and curdat.uec= 2) are       */
/* defined in startpts (should be here instead ???? )               */
/*                                                                  */
/* Note that u_s_m, v_s_m, u_e_m and v_e_m can be surface 1 or 2    */
/*                                                                  */

static short isoparam (bcase)
/*                                                                  */
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint    bcase;       /* Eq. 1: u0 Eq. 2: u1 Eq. 3: v0 Eq. 4: v1 */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */


/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initializations and checks                                    */
/*                                                                  */
/* (No checks for the moment)                                       */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur923*isoparam  bcase %d\n" , (int)bcase );
}
#endif

/*!                                                                 */
/* 2. Create straight line in the U,V plane                         */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Straight line:                                                   */
/*                                                                  */
/* U  = U0 + s*TU    (U0,V0) = Start point for line                 */
/* V  = V0 + s*TV    (TU,TV) = Direction for line                   */
/*                                                                 !*/

   if        ( bcase == 1 )              /* Boundary u= u_s_m       */
      {
      iso_seg.c0x= u_s_m;        
      iso_seg.c1x= 0.0;
      iso_seg.c0y= v_s_m;        
      iso_seg.c1y= v_e_m  - v_s_m  ;
      }
   else if   ( bcase == 2 )              /* Boundary u= u_e_m       */
      {
      iso_seg.c0x= u_e_m  ;      
      iso_seg.c1x= 0.0;
      iso_seg.c0y= v_s_m  ;      
      iso_seg.c1y= v_e_m  - v_s_m  ;
      }
   else if   ( bcase == 3 )              /* Boundary v= v_s_m       */
      {
      iso_seg.c0x= u_s_m  ;      
      iso_seg.c1x= u_e_m  - u_s_m  ;
      iso_seg.c0y= v_s_m  ;      
      iso_seg.c1y= 0.0;    
      }
   else if   ( bcase == 4 )              /* Boundary v= v_e_m       */
      {
      iso_seg.c0x= u_e_m  ;      
      iso_seg.c1x= u_e_m  - u_s_m  ;
      iso_seg.c0y= v_e_m  ;      
      iso_seg.c1y= 0.0;    
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur923*isoparam bcase %d c0x %5.1f c01 %5.1f c0y %5.1f c1y %5.1f\n" , 
    (int)bcase, iso_seg.c0x, iso_seg.c1x, iso_seg.c0y, iso_seg.c1y );
}
#endif

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/


/*!New-Page--------------------------------------------------------!*/

/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!********* Internal ** function ** retrieve ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates current isoparameter boundary curve         */

static short retrieve (p_comp)
/*                                                                  */
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   IRUNON  *p_comp;      /* Computation data                  (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */

/* 1. Initiations and checks                                        */
/*                                                                  */
/* (No checks for the moment)                                       */
/*                                                                 !*/

  if ( p_comp->isu_un.ctype == SURSUR )
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur923*retrieve  Computation case: SURSUR (ctype %d)\n" , 
              (int)p_comp->isu_un.ctype );
}
#endif
    }
  else
    {
    }

/*!                                                                 */
/* 2. Data to curdat                                                */
/*                                                                  */
/*                                                                 !*/

curdat.ctype    = SURCUR;         /* Type of calculation            */
curdat.method   = NEWTON;         /* Method to be used              */
curdat.nstart   =                 /* Number of restarts             */
          p_comp->iso_un.nstart; 
curdat.maxiter  = 20;             /* Maximum number of restarts     */
curdat.s_method = 1;              /* Select/search method           */
                                  /* Eq. 1: BBOX'es only            */

curdat.ctol     =                 /* Coordinate     tolerance       */
         p_comp->isu_un.ctol;
curdat.ntol     =                 /* Surface normal tolerance       */
         p_comp->isu_un.ntol;
curdat.idpoint  =                 /* Identical point criterion      */
         p_comp->isu_un.idpoint;
curdat.idangle  =                 /* Identical angle criterion      */
         p_comp->isu_un.idangle;
curdat.comptol  =                 /* Computer accuracy tolerance    */
         p_comp->isu_un.comptol;
curdat.factol   =                 /* Facets (B-planes) tolerance    */
         p_comp->isu_un.factol; 
curdat.d_che    =                 /* Maximum check R*3 distance     */
         p_comp->isu_un.d_che;  

                                  /* Density of the UV net:         */
curdat.u_add   =  1;              /* For surface 1 in U direction   */
curdat.v_add   =  1;              /* For surface 1 in V direction   */
                                  /* (Values 1, 2, 3, 4, .... )     */

                                  /* Patch limit pts (search area): */
curdat.us       = F_UNDEF;        /* U start surface                */
curdat.vs       = F_UNDEF;        /* V start surface                */
curdat.ue       = F_UNDEF;        /* U end   surface                */
curdat.ve       = F_UNDEF;        /* V end   surface                */

                                  /* Segment limits  (search area): */
curdat.usc      = F_UNDEF;        /* U start for curve              */
curdat.uec      = F_UNDEF;        /* U end   for curve              */

curdat.startuvpt.x_gm = F_UNDEF;  /* Start U,V point surface        */
curdat.startuvpt.y_gm = F_UNDEF; 
curdat.startuvpt.z_gm = F_UNDEF; 
curdat.startupt.x_gm  = F_UNDEF;  /* Start U   point curve          */
curdat.startupt.y_gm  = F_UNDEF; 
curdat.startupt.z_gm  = F_UNDEF; 


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur923*retrieve curdat.d_che %f\n" , curdat.d_che  );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/



/*!New-Page--------------------------------------------------------!*/

/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!********* Internal ** function ** startpts ***********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function retrieves start point for the intersect calculation */
/* between a curve segment and a surface patch intersect            */
/*                                                                  */
/* The criteria for a start point from the APOINT table is that:    */
/*                                                                  */
/* - The approximative point is the result from a calculation       */
/*   with patches (record numbers) rec_no_m and rec_no_s in         */
/*   table PBOUND.                                                  */
/*                                                                  */
/* - The approximative point is on the current boundary (bcase)     */
/*                                                                  */
/*                                                                  */

static short startpts ( p_apts, napts, sur_no_m, bcase )
/*                                                                  */
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   APOINT  *p_apts;      /* Facets intersect (start) points   (ptr) */
   DBint    napts;       /* Number of records in apts               */
   DBint    sur_no_m;    /* Master surface number (1 or 2)          */
   DBint    bcase;       /* Eq. 1: u0 Eq. 2: u1 Eq. 3: v0 Eq. 4: v1 */
                         /* (Defines the current boundary)          */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    i_p;         /* Loop index point in APOINT              */
/*-----------------------------------------------------------------!*/

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

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur923*startpts rec_no_m %d rec_no_s %d sur_no_m %d bcase %d napts %d\n" , 
 (int)rec_no_m,  (int)rec_no_s,  (int)sur_no_m, 
 (int)bcase, (int)napts );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 2. Definition of patch area and curve segment                    */
/*                                                                  */
/*                                                                 !*/

                              /* Patch limit pts (search area):     */
curdat.us   = u_s_s;          /* U start slave surface              */
curdat.vs   = v_s_s;          /* V start slave surface              */
curdat.ue   = u_e_s;          /* U end   slave surface              */
curdat.ve   = v_e_s;          /* V end   slave surface              */

                              /* Segment limits  (search area):     */
curdat.usc  = 1.0;            /* U start for curve master surface   */
curdat.uec  = 2.0;            /* U end   for curve master surface   */


/*!                                                                 */
/* 3. Search for curve start point for the current boundary         */
/*                                                                  */
/*                                                                 !*/

curdat.startuvpt.x_gm = F_UNDEF;                 
curdat.startuvpt.y_gm = F_UNDEF;                 
curdat.startuvpt.z_gm = F_UNDEF;                 
curdat.startupt.x_gm  = F_UNDEF;
curdat.startupt.y_gm  = F_UNDEF;
curdat.startupt.z_gm  = F_UNDEF;

for ( i_p=0; i_p< napts; ++i_p )       /* Start loop i_p in APOINT  */
  {
  if        ( rec_no_m == (p_apts+i_p)->rec_no_sur1 &&
              rec_no_s == (p_apts+i_p)->rec_no_sur2    )
    ; /* Approximative points created by rec_no_m and rec_no_s      */
  else if   ( rec_no_s == (p_apts+i_p)->rec_no_sur1 &&
              rec_no_m == (p_apts+i_p)->rec_no_sur2    )
    ; /* Approximative points created by rec_no_m and rec_no_s      */
  else   goto  nxtpoint;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur923* sur1 us %5.2f vs %5.2f ue %5.2f ve %5.2f rec_no_m %d rec_no_s %d i_p %d\n" , 
         (p_apts+i_p)->us_sur1, (p_apts+i_p)->vs_sur1, 
         (p_apts+i_p)->ue_sur1, (p_apts+i_p)->ve_sur1, 
         (int)rec_no_m, (int)rec_no_s,(int)i_p );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur923* sur2 us %5.2f vs %5.2f ue %5.2f ve %5.2f\n" , 
         (p_apts+i_p)->us_sur2, (p_apts+i_p)->vs_sur2, 
         (p_apts+i_p)->ue_sur2, (p_apts+i_p)->ve_sur2);
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur923* sur1 u1 %5.2f v1 %5.2f v2 %5.2f v2 %5.2f\n" , 
         (p_apts+i_p)->u1sur1 , (p_apts+i_p)->v1sur1 , 
         (p_apts+i_p)->v1sur1 , (p_apts+i_p)->v2sur1 );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur923* sur2 u1 %5.2f v1 %5.2f v2 %5.2f v2 %5.2f\n" , 
         (p_apts+i_p)->u1sur2 , (p_apts+i_p)->v1sur2 , 
         (p_apts+i_p)->v1sur2 , (p_apts+i_p)->v2sur2 );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif



  if      ( bcase == 1 && sur_no_m == 1 )  /* Boundary u= u_s_m     */
     {
     if      ( fabs(u_s_m-(p_apts+i_p)->u1sur1) <  0.03  )  
       {
       curdat.startupt.x_gm = (p_apts+i_p)->v1sur1- 
        floor((p_apts+i_p)->v1sur1) + (DBfloat)1.0;
       curdat.startuvpt.x_gm = (p_apts+i_p)->u1sur2;
       curdat.startuvpt.y_gm = (p_apts+i_p)->v1sur2;
       goto strpoint;
       }
     else if ( fabs(u_s_m-(p_apts+i_p)->u2sur1) <  0.03  )  
       {
       curdat.startupt.x_gm = (p_apts+i_p)->v2sur1- 
        floor((p_apts+i_p)->v2sur1) + (DBfloat)1.0;
       curdat.startuvpt.x_gm = (p_apts+i_p)->u2sur2;
       curdat.startuvpt.y_gm = (p_apts+i_p)->v2sur2;
       goto strpoint;
       }
     else
       {
       goto nxtpoint;
       }
     }                        /* Boundary u= u_s_m  sur_no_m= 1     */

  else if ( bcase == 2 && sur_no_m == 1 )  /* Boundary u= u_e_m     */
     {
     if      ( fabs(u_e_m-(p_apts+i_p)->u1sur1) <  0.03  )  
       {
       curdat.startupt.x_gm = (p_apts+i_p)->v1sur1- 
        floor((p_apts+i_p)->v1sur1) + (DBfloat)1.0;
       curdat.startuvpt.x_gm = (p_apts+i_p)->u1sur2;
       curdat.startuvpt.y_gm = (p_apts+i_p)->v1sur2;
       goto strpoint;
       }
     else if ( fabs(u_e_m-(p_apts+i_p)->u2sur1) <  0.03  )  
       {
       curdat.startupt.x_gm = (p_apts+i_p)->v2sur1- 
        floor((p_apts+i_p)->v2sur1) + (DBfloat)1.0;
       curdat.startuvpt.x_gm = (p_apts+i_p)->u2sur2;
       curdat.startuvpt.y_gm = (p_apts+i_p)->v2sur2;
       goto strpoint;
       }
     else
       {
       goto nxtpoint;
       }
     }                        /* Boundary u= u_e_m  sur_no_m= 1     */

  else if ( bcase == 3 && sur_no_m == 1 )  /* Boundary v= v_s_m     */
     {
     if      ( fabs(v_s_m-(p_apts+i_p)->v1sur1) <  0.03  )  
       {
       curdat.startupt.x_gm = (p_apts+i_p)->v1sur1- 
        floor((p_apts+i_p)->v1sur1) + (DBfloat)1.0;
       curdat.startuvpt.x_gm = (p_apts+i_p)->u1sur2;
       curdat.startuvpt.y_gm = (p_apts+i_p)->v1sur2;
       goto strpoint;
       }
     else if ( fabs(v_s_m-(p_apts+i_p)->v2sur1) <  0.03  )  
       {
       curdat.startupt.x_gm = (p_apts+i_p)->u2sur1- 
        floor((p_apts+i_p)->u2sur1) + (DBfloat)1.0;
       curdat.startuvpt.x_gm = (p_apts+i_p)->u2sur2;
       curdat.startuvpt.y_gm = (p_apts+i_p)->v2sur2;
       goto strpoint;
       }
     else
       {
       goto nxtpoint;
       }
     }                        /* Boundary u= v_s_m  sur_no_m= 1     */


  else if ( bcase == 4 && sur_no_m == 1 )  /* Boundary u= v_e_m     */
     {
     if      ( fabs(v_e_m-(p_apts+i_p)->v1sur1) <  0.03  )  
       {
       curdat.startupt.x_gm = (p_apts+i_p)->u1sur1- 
        floor((p_apts+i_p)->u1sur1) + (DBfloat)1.0;
       curdat.startuvpt.x_gm = (p_apts+i_p)->u1sur2;
       curdat.startuvpt.y_gm = (p_apts+i_p)->v1sur2;
       goto strpoint;
       }
     else if ( fabs(v_e_m-(p_apts+i_p)->v2sur1) <  0.03  )  
       {
       curdat.startupt.x_gm = (p_apts+i_p)->u2sur1- 
        floor((p_apts+i_p)->u2sur1) + (DBfloat)1.0;
       curdat.startuvpt.x_gm = (p_apts+i_p)->u2sur2;
       curdat.startuvpt.y_gm = (p_apts+i_p)->v2sur2;
       goto strpoint;
       }
     else
       {
       goto nxtpoint;
       }
     }                        /* Boundary u= u_s_m  sur_no_m= 1     */
   
     else
      {
      sprintf(errbuf,                  
      "(startpts switch)%%varkon_sur_epts2");   
  return(varkon_erpush("SU2993",errbuf));
      }


strpoint:;  /* A start point is found                               */

curdat.startuvpt.z_gm = 0.0;     
curdat.startupt.y_gm  = 0.0;     
curdat.startupt.z_gm  = 0.0;     

nxtpoint:;  /* Approximative pts not created by rec_no_m & rec_no_s */

  }                                    /* End   loop i_p in APOINT  */



#ifdef DEBUG
if ( dbglev(SURPAC) == 1 &&  curdat.startuvpt.x_gm > 0.0 )
{
fprintf(dbgfil(SURPAC),
"sur923*startpts curdat.startuvp %f %f curdat.startupt %f\n" , 
   curdat.startuvpt.x_gm , curdat.startuvpt.y_gm, curdat.startupt.x_gm );
}
if ( dbglev(SURPAC) == 1 && curdat.startuvpt.x_gm > 0.0)
{
fprintf(dbgfil(SURPAC),
"sur923*startpts curdat.us %6.3f vs %6.3f ue %6.3f ve %6.3f usc %6.3f uec %6.3f\n" , 
   curdat.us , curdat.ue, curdat.vs, curdat.ve , curdat.usc, curdat.uec );
}
if ( dbglev(SURPAC) == 2 && curdat.startuvpt.x_gm < 0.0)
{
fprintf(dbgfil(SURPAC),
"sur923*startpts There is no approx. start pt for rec_no_m %d rec_no_s %d\n" , 
   (int)rec_no_m, (int)rec_no_s );
}
#endif



    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/


