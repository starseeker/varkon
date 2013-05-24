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

#define DEBUG

#include "../../DB/include/DB.h"
#include "../include/GE.h"

#ifdef DEBUG
#include "../../IG/include/debug.h"
#endif

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_sur_rot                       File: sur850.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Create a rotational surface (SUR_ROT)                           */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-08-31   Originally written                                 */
/*  1996-11-10   V3MOME for curve headers, r_case, modif. start/end */
/*  1997-03-09   sur_type, typarr[]= 1 -->typarr[]= 3, criterion    */
/*               angles                                             */
/*  1997-03-18   rotcsy                                             */
/*  1997-03-19   rotcsy continued                                   */
/*  1997-03-21   rotcsy direction (projected pts)                   */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_rot        Create a rotational surface      */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_idpoint          * Identical points criterion             */
/* varkon_idangle          * Identical angles criterion             */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_ini_gmseg        * Initialize DBSeg                       */
/* varkon_ini_gmcur        * Initialize DBCurve                     */
/* GE109                   * Curve evaluation routine               */
/* GE133                   * Rational cubic with P-value            */
/* GEmktf_rs               * Create rotation    matrix              */
/* GEmktf_ts               * Create translation matrix              */
/* GE135                   * Trim a segment (reparam.)              */
/* varkon_angd             * Angle between vectors (degr.)          */
/* varkon_lin_ptclose      * Closest point on line                  */
/* varkon_sur_rotloft      * Create a LFT_SUR rotation              */
/* varkon_sur_creloft      * Create a LFT_SUR                       */
/* varkon_cur_print        * Printout of curve data                 */
/* varkon_erinit           * Initial. error message stack           */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short initial();       /* Check input data and initialize    */
static short rotcsys();       /* Coord. system & spine, sur_type= 1 */
static short lftcsys();       /* Coord. system & spine, sur_type= 2 */
static short checur ();       /* Check input curve                  */
static short modcur ();       /* Modify start and/or end of curve   */
static short angles ();       /* Angles and rotation matrices       */
static short tcurve ();       /* Tangent curve                      */
static short transf ();       /* Transformate all curves            */
static short midcrv ();       /* P-value mid curve creation         */
static short curfree();       /* Deallocate memory for curves       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat  idpoint;       /* Identical point criterion           */
static DBfloat  idangle;       /* Identical angle criterion           */
static DBfloat  comptol;       /* Computer tolerance                  */
static DBVector pexts;         /* External point, start of curve      */
static DBVector pcloses;       /* Nearest point on line to pexts      */
static DBVector pclosee;       /* Nearest point on line to pexte      */
static DBfloat  dists;         /* Distance line to pexts              */
static DBfloat  diste;         /* Distance line to pexte              */
static DBCurve  spine;         /* Spine                               */
static DBSeg  spineseg;      /* Spine segment                       */
static DBCurve  midpval;       /* Mid P-value curve                   */
static DBSeg  midpvalseg;    /* Mid P-value curve segment           */
static DBCurve limit1;        /* Limit curve 1                       */
static DBCurve limit2;        /* Limit curve 2                       */
static DBCurve  limit3;        /* Limit curve 3                       */
static DBCurve  limit4;        /* Limit curve 4                       */
static DBCurve  limit5;        /* Limit curve 5                       */
static DBCurve  tangs;         /* Start tangent curve                 */
static DBCurve  tang1;         /* Tangent curve 1                     */
static DBCurve  tang2;         /* Tangent curve 2                     */
static DBCurve  tang3;         /* Tangent curve 3                     */
static DBCurve  tang4;         /* Tangent curve 4                     */
static DBCurve  tang5;         /* Tangent curve 5                     */
static DBSeg *p_limit1_seg;  /* Limit curve 1 segments        (ptr) */
static DBSeg *p_limit2_seg;  /* Limit curve 2 segments        (ptr) */
static DBSeg *p_limit3_seg;  /* Limit curve 3 segments        (ptr) */
static DBSeg *p_limit4_seg;  /* Limit curve 4 segments        (ptr) */
static DBSeg *p_limit5_seg;  /* Limit curve 5 segments        (ptr) */
static DBSeg *p_tangs_seg;   /* Start tangent curve segments  (ptr) */
static DBSeg *p_tang1_seg;   /* Tangent curve 1 segments      (ptr) */
static DBSeg *p_tang2_seg;   /* Tangent curve 2 segments      (ptr) */
static DBSeg *p_tang3_seg;   /* Tangent curve 3 segments      (ptr) */
static DBSeg *p_tang4_seg;   /* Tangent curve 4 segments      (ptr) */
static DBSeg *p_tang5_seg;   /* Tangent curve 5 segments      (ptr) */
static DBTmat spine_csy;     /* Transformation matrix spine         */
static DBfloat  spine_length;  /* Spine length                        */
static DBint  no_seg;        /* Number of strips in the surface     */
static DBfloat  ang1_rot;      /* Rotation angle for  limit curve 1   */
static DBfloat  ang2_rot;      /* Rotation angle for  limit curve 2   */
static DBfloat  ang3_rot;      /* Rotation angle for  limit curve 3   */
static DBfloat  ang4_rot;      /* Rotation angle for  limit curve 4   */
static DBfloat  ang5_rot;      /* Rotation angle for  limit curve 5   */
static DBTmat ang1_tra;      /* Rotation matrix for limit curve 1   */
static DBTmat ang2_tra;      /* Rotation matrix for limit curve 2   */
static DBTmat ang3_tra;      /* Rotation matrix for limit curve 3   */
static DBTmat ang4_tra;      /* Rotation matrix for limit curve 4   */
static DBTmat ang5_tra;      /* Rotation matrix for limit curve 5   */
static DBTmat tang_tra;      /* Translation matrix for tangent crv  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_rot             */
/* SU2973 = Internal function () failed in varkon_sur_rot           */
/* SU2993 = Severe program error (  ) in varkon_sur_rot             */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/

   DBstatus           varkon_sur_rot (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve  *p_cur,      /* Rotation curve                    (ptr) */
   DBSeg    *p_seg,      /* Segment data for p_cur            (ptr) */
   DBint     r_case,     /* Rotation case                           */
                         /* Eq.  1: Rotation in positive direction  */
                         /* Eq. -1: Rotation in negative direction  */
   DBVector *p_p1,       /* Rotation axis point 1             (ptr) */
   DBVector *p_p2,       /* Rotation axis point 2             (ptr) */
   DBfloat   v0,         /* Start angle                             */
   DBfloat   v1,         /* End   angle                             */
   DBint     sur_type,   /* Output surface type                     */
                         /* Eq.  1: LFT_SUR, p_flag= 3              */
                         /* Eq.  2: LFT_SUR, p_flag= 2              */
   DBSurf   *p_surout,   /* Output surface                    (ptr) */
   DBPatch **pp_patout ) /* Alloc. area for topol. patch data (ptr) */
                         /* (pp_patout= NULL <==> not allocated)    */


/* Out:                                                             */
/*                                                                  */
/*      Data to p_surout and pp_patout in allocated memory area     */
/*                                                                  */
/*      Data to p_pltable                                           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBCurve  limcur[5];   /* All limit   curves                      */
   DBCurve  tancur[5];   /* All tangent curves                      */
   DBCurve  midcur[4];   /* All mid     curves                      */
   DBSeg   *limseg[5];   /* All limit   curve segments       (ptrs) */
   DBSeg   *tanseg[5];   /* All tangent curve segments       (ptrs) */
   DBSeg   *midseg[4];   /* All mid     curve segments       (ptrs) */
   DBint    typarr[4];   /* Type of mid curve. 1= P 2= M 3= R       */
/*                                                                  */
/*-----------------------------------------------------------------!*/


  DBint  status;         /* Error code from called function         */
  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur850 Enter**varkon_sur_rot p_cur %d p_seg %d sur_type %d\n",
           (int)p_cur,(int)p_seg,  (int)sur_type );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur850 r_case %d v0 %f v1 %f\n", (short)r_case, v0, v1  );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur850 p1 %f  %f  %f\n", p_p1->x_gm, p_p1->y_gm,p_p1->z_gm  );
fprintf(dbgfil(SURPAC),
"sur850 p2 %f  %f  %f\n", p_p2->x_gm, p_p2->y_gm,p_p2->z_gm  );
fflush(dbgfil(SURPAC));
}
#endif
 
/*!                                                                 */
/* Let pp_patout= NULL                                              */
/*                                                                 !*/
   *pp_patout = NULL;

    status= initial(  );
    if (status<0) 
      {
      sprintf(errbuf,"initial%%varkon_sur_rot (sur850)");
      return(varkon_erpush("SU2973",errbuf));
      }

/*!                                                                 */
/* 2. Create local coordinate system and spine                      */
/*                                                                 !*/

    if        ( sur_type == 1 )
    status= rotcsys   ( p_cur, p_seg, p_p1, p_p2 );
    else if   ( sur_type == 2 )
    status= lftcsys   ( p_cur, p_seg, p_p1, p_p2  );
    else
      {
      sprintf(errbuf,"sur_type%%varkon_sur_rot (sur850)");
      return(varkon_erpush("SU2993",errbuf));
      }

    if (status < 0) 
      {
      return(status);
      }

/*!                                                                 */
/* 3. Check input rotation curve                                    */
/*                                                                 !*/

    status= checur ( p_cur, p_seg, p_p1, p_p2 , sur_type );
    if (status<0) 
      {
      return(status);
      }

/*!                                                                 */
/* 4. Calculate rotation angles and matrices                        */
/*                                                                 !*/

    status= angles ( v0, v1, r_case );
    if (status<0) 
      {
      return(status);
      }

/*!                                                                 */
/* 5. Create tangent definition curve to the input curve            */
/*                                                                 !*/

    status= tcurve ( p_cur, p_seg );
    if (status<0) 
      {
      curfree ();
      sprintf(errbuf,"tcurve%%varkon_sur_rot (sur850)");
      return(varkon_erpush("SU2973",errbuf));
      }

/*!                                                                 */
/* 6. Create surface definition curves by transformation            */
/*                                                                 !*/

    status= transf ( p_cur, p_seg );
    if (status<0) 
      {
      curfree ();
      sprintf(errbuf,"transf%%varkon_sur_rot (sur850)");
      return(varkon_erpush("SU2973",errbuf));
      }

/*!                                                                 */
/* 7. Create mid P-value curve                                      */
/*                                                                 !*/

    status= midcrv ( );
    if (status<0) 
      {
      curfree ();
      sprintf(errbuf,"midcrv%%varkon_sur_rot (sur850)");
      return(varkon_erpush("SU2973",errbuf));
      }



/*!                                                                 */
/* 8. Create the surface. Call of varkon_sur_rotloft (sur851).      */
/*                                                                 !*/

  limcur[0] = limit1;
  limcur[1] = limit2;
  limcur[2] = limit3;
  limcur[3] = limit4;
  limcur[4] = limit5;
  tancur[0] = tang1;
  tancur[1] = tang2;
  tancur[2] = tang3;
  tancur[3] = tang4;
  tancur[4] = tang5;
  midcur[0] = midpval;
  midcur[1] = midpval;
  midcur[2] = midpval;
  midcur[3] = midpval;

  limseg[0] = p_limit1_seg;
  limseg[1] = p_limit2_seg;
  limseg[2] = p_limit3_seg;
  limseg[3] = p_limit4_seg;
  limseg[4] = p_limit5_seg;
  tanseg[0] = p_tang1_seg;
  tanseg[1] = p_tang2_seg;
  tanseg[2] = p_tang3_seg;
  tanseg[3] = p_tang4_seg;
  tanseg[4] = p_tang5_seg;
  midseg[0] = &midpvalseg;
  midseg[1] = &midpvalseg;
  midseg[2] = &midpvalseg;
  midseg[3] = &midpvalseg;

  if     ( sur_type == 1 )
    {
    typarr[0] = 3;
    typarr[1] = 3;
    typarr[2] = 3;
    typarr[3] = 3;
    }
  else if ( sur_type == 2 )
    {
    typarr[0] = 1;
    typarr[1] = 1;
    typarr[2] = 1;
    typarr[3] = 1;
    }

#ifdef  DEBUG

status = varkon_cur_print  (p_cur, p_seg, (DBint)2 );
   if (status<0) 
     {
     sprintf(errbuf,"sur799%%sur850");
     return(varkon_erpush("SU2943",errbuf));
    }

status = varkon_cur_print  (&spine,&spineseg, (DBint)2 );
   if (status<0) 
     {
     sprintf(errbuf,"sur799%%sur850");
     return(varkon_erpush("SU2943",errbuf));
    }

#endif


  if     ( sur_type == 1 )
    {
status = varkon_sur_rotloft
      (&spine,&spineseg,(DBint)no_seg,limcur,limseg,
        tancur,tanseg,midcur,midseg,typarr,p_surout,pp_patout);
     if (status<0) 
       {
       curfree ();
       sprintf(errbuf,"sur851%%sur850");
       return(varkon_erpush("SU2943",errbuf));
      }
    }        /*  End sur_type = 1  */

  else if ( sur_type == 2 )
    {
status = varkon_sur_creloft
      (&spine,&spineseg,(DBint)no_seg,limcur,limseg,
            tancur,tanseg,midcur,midseg,typarr,p_surout,pp_patout);
     if (status<0) 
       {
       curfree ();
       sprintf(errbuf,"sur800%%sur850");
       return(varkon_erpush("SU2943",errbuf));
      }
    }        /*  End sur_type = 2  */



/*!                                                                 */
/* 9. Exit (deallocate memory for created curves)                   */
/*                                                                 !*/

    curfree(); 

#ifdef  DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur850 pp_patout %d\n", (int)pp_patout );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur850 Exit*varkon_sur_rot Number of patches U= %d V= %d\n", 
 p_cur->ns_cu,(int)no_seg );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/


/*!********* Internal ** function ** initial ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function checks and initializes data                         */

   static short initial(  )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*initial* Enter ** \n");
  }
#endif

    varkon_ini_gmcur (&spine);
   spine.ns_cu       = (short)1; 
   spine.hed_cu.type = CURTYP;

    varkon_ini_gmcur (&midpval);
   midpval.ns_cu       = (short)1; 
   midpval.hed_cu.type = CURTYP;

/*!                                                                 */
/* Note that ......                                                 */
/* Copy curve headers from the input curve to transformated curves  */
/* which will be created in this function. The curve transformation */
/* assumes that this is done by the calling function !              */
/* The curve header (the copied data) will be changed if is a       */
/* planar curve. A new plane will be calculated by the curve trans- */
/* formation function GEtfCurve_to_local.                           */
/*                                                                 !*/

    varkon_ini_gmcur (&limit1);
    varkon_ini_gmcur (&limit2);
    varkon_ini_gmcur (&limit3);
    varkon_ini_gmcur (&limit4);
    varkon_ini_gmcur (&limit5);
    varkon_ini_gmcur (&tangs);
    varkon_ini_gmcur (&tang1);
    varkon_ini_gmcur (&tang2);
    varkon_ini_gmcur (&tang3);
    varkon_ini_gmcur (&tang4);
    varkon_ini_gmcur (&tang5);

/*!                                                                 */
/* Initialize segment data in spineseg and midpvalseg.              */
/* Calls of varkon_ini_gmseg  (sur779).                             */
/*                                                                 !*/

    varkon_ini_gmseg ( &spineseg );  

    varkon_ini_gmseg ( &midpvalseg );  

   idpoint   = varkon_idpoint();
   idangle   = varkon_idangle();
   comptol   = varkon_comptol();

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur850*initial idpoint %10.6f idangle %6.4f comptol %12.10f\n", 
idpoint, idangle, comptol  );
fflush(dbgfil(SURPAC)); 
}
#endif

   ang1_rot = F_UNDEF;
   ang2_rot = F_UNDEF;
   ang3_rot = F_UNDEF;
   ang4_rot = F_UNDEF;
   ang5_rot = F_UNDEF;

   p_limit1_seg = NULL;
   p_limit2_seg = NULL;
   p_limit3_seg = NULL;
   p_limit4_seg = NULL;
   p_limit5_seg = NULL;
   p_tangs_seg  = NULL;
   p_tang1_seg  = NULL;
   p_tang2_seg  = NULL;
   p_tang3_seg  = NULL;
   p_tang4_seg  = NULL;
   p_tang5_seg  = NULL;

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/








/*!********* Internal ** function ** lftcsys ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*      For a normal LFT_SUR, p_flag= 1 (sur_type= 2)               */
/* The function creates a local coordinate system with the          */
/* X axis equal to the rotation axis and the spine for the          */
/* surface.                                                         */
/*                                                                  */
/*                                                                  */

   static short lftcsys ( p_cur, p_seg, p_p1, p_p2  )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve  *p_cur;      /* Rotation curve                    (ptr) */
   DBSeg    *p_seg;      /* Segment data for p_cur            (ptr) */
   DBVector *p_p1;       /* Rotation axis point 1             (ptr) */
   DBVector *p_p2;       /* Rotation axis point 2             (ptr) */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBVector pextm;        /* External point, mid   of curve          */
  DBVector pexte;        /* External point, start of curve          */
  DBVector origin;       /* Origin for line coordinate  system      */
  DBVector xaxis;        /* X axis for spine coordinate  system     */
  DBVector yaxis;        /* Y axis for spine coordinate  system     */

/*----------------------------------------------------------------- */
  EVALC    xyz_c;        /* Point and derivatives for curve         */
  DBVector ldir;         /* Direction vector for the line           */
  DBVector pclosem;      /* Nearest point on line to pextm          */
  DBfloat  params;       /* Parameter for the nearest point pcloses */
  DBfloat  paramm;       /* Parameter for the nearest point pclosem */
  DBfloat  parame;       /* Parameter for the nearest point pclosee */
  DBfloat  distm;        /* Distance line to pextm                  */
  DBVector texts;        /* Start tangent for curve                 */
  DBVector textm;        /* Mid   tangent for curve                 */
  DBVector texte;        /* End   tangent for curve                 */
  DBfloat  dot_spine_s;  /* Scalar product start tangent - chord    */
  DBfloat  dot_spine_m;  /* Scalar product mid   tangent - chord    */
  DBfloat  dot_spine_e;  /* Scalar product end   tangent - chord    */
  DBfloat  dot_spine;    /* Scalar product start tangent - chord    */
  DBVector points[4];    /* points[0]    = start point              */
                         /* points[1]    = start tangent point      */
                         /* points[2]    = end   tangent point      */
                         /* points[3]    = end   point              */
  DBfloat  pvalue;       /* P value for the rational segment        */
  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*lftcsys* Enter **  \n");
  }
#endif

  dists       = F_UNDEF; 
  distm       = F_UNDEF; 
  diste       = F_UNDEF; 
  dot_spine   = F_UNDEF; 
  dot_spine_s = F_UNDEF; 
  dot_spine_m = F_UNDEF; 
  dot_spine_e = F_UNDEF; 

/*!                                                                 */
/* Get coordinates and tangents for start and end of curve.         */
/* Calls of GE109.                                                  */
/*                                                                 !*/

   xyz_c.evltyp   = EVC_DR; 

   xyz_c.t_global = 1.0;   /* Global parameter value start point    */


status=GE109 ((DBAny *)p_cur, p_seg, &xyz_c );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur850");
     return(varkon_erpush("SU2943",errbuf));
    }

  pexts.x_gm = xyz_c.r.x_gm;
  pexts.y_gm = xyz_c.r.y_gm;
  pexts.z_gm = xyz_c.r.z_gm;
  texts.x_gm = xyz_c.drdt.x_gm;
  texts.y_gm = xyz_c.drdt.y_gm;
  texts.z_gm = xyz_c.drdt.z_gm;


  xyz_c.t_global = (DBfloat)p_cur->ns_cu/2.0 + 1.0; /* Mid point  */

status=GE109 ((DBAny *)p_cur, p_seg, &xyz_c );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur850");
     return(varkon_erpush("SU2943",errbuf));
    }

  pextm.x_gm = xyz_c.r.x_gm;
  pextm.y_gm = xyz_c.r.y_gm;
  pextm.z_gm = xyz_c.r.z_gm;
  textm.x_gm = xyz_c.drdt.x_gm;
  textm.y_gm = xyz_c.drdt.y_gm;
  textm.z_gm = xyz_c.drdt.z_gm;

  xyz_c.t_global = (DBfloat)p_cur->ns_cu + 1.0; /* End point        */

status=GE109 ((DBAny *)p_cur, p_seg, &xyz_c );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur850");
     return(varkon_erpush("SU2943",errbuf));
    }

  pexte.x_gm = xyz_c.r.x_gm;
  pexte.y_gm = xyz_c.r.y_gm;
  pexte.z_gm = xyz_c.r.z_gm;
  texte.x_gm = xyz_c.drdt.x_gm;
  texte.y_gm = xyz_c.drdt.y_gm;
  texte.z_gm = xyz_c.drdt.z_gm;


/*!                                                                 */
/* Calculate closest point to the rotation axis.                    */
/* Calls of varkon_lin_ptclose (sur712).                            */
/*                                                                 !*/


  ldir.x_gm = p_p2->x_gm - p_p1->x_gm;
  ldir.y_gm = p_p2->y_gm - p_p1->y_gm;
  ldir.z_gm = p_p2->z_gm - p_p1->z_gm;


    status = varkon_lin_ptclose
   (p_p1, &ldir , &pexts, &pcloses, &params, &dists );
    if(status<0)
    {
    sprintf(errbuf,"sur712%%sur850");
    return(varkon_erpush("SU2943",errbuf));
    }

    status = varkon_lin_ptclose
   (p_p1, &ldir , &pextm, &pclosem, &paramm, &distm );
    if(status<0)
    {
    sprintf(errbuf,"sur712%%sur850");
    return(varkon_erpush("SU2943",errbuf));
    }

    status = varkon_lin_ptclose
   (p_p1, &ldir , &pexte, &pclosee, &parame, &diste );
    if(status<0)
    {
    sprintf(errbuf,"sur712%%sur850");
    return(varkon_erpush("SU2943",errbuf));
    }


/*!                                                                 */
/* Create spine line (= the rotation axis). Check spine length.     */
/* Call of GE133.                                                   */
/*                                                                 !*/

  spine_length = 
    SQRT ( (pcloses.x_gm-pclosee.x_gm)*(pcloses.x_gm-pclosee.x_gm) +
           (pcloses.y_gm-pclosee.y_gm)*(pcloses.y_gm-pclosee.y_gm) +
           (pcloses.z_gm-pclosee.z_gm)*(pcloses.z_gm-pclosee.z_gm) );

  if ( spine_length < 10.0*idpoint )
    {
    sprintf(errbuf,"%6.3f%%(sur850)",spine_length);
    varkon_erinit();
    return(varkon_erpush("SU8003",errbuf));
    }


/*   Create parabola                                                */

  dot_spine_s = texts.x_gm*(pclosee.x_gm-pcloses.x_gm) + 
                texts.y_gm*(pclosee.y_gm-pcloses.y_gm) +  
                texts.z_gm*(pclosee.z_gm-pcloses.z_gm);  
  dot_spine_m = textm.x_gm*(pclosee.x_gm-pcloses.x_gm) + 
                textm.y_gm*(pclosee.y_gm-pcloses.y_gm) +  
                textm.z_gm*(pclosee.z_gm-pcloses.z_gm);  
  dot_spine_e = texte.x_gm*(pclosee.x_gm-pcloses.x_gm) + 
                texte.y_gm*(pclosee.y_gm-pcloses.y_gm) +  
                texte.z_gm*(pclosee.z_gm-pcloses.z_gm);  

  if      (  fabs(dot_spine_s) >= fabs(dot_spine_m) &&
             fabs(dot_spine_s) >= fabs(dot_spine_e)     )
    dot_spine = dot_spine_s;
  else if (  fabs(dot_spine_m) >= fabs(dot_spine_s) &&
             fabs(dot_spine_m) >= fabs(dot_spine_e)     )
    dot_spine = dot_spine_m;
  else if (  fabs(dot_spine_e) >= fabs(dot_spine_s) &&
             fabs(dot_spine_e) >= fabs(dot_spine_m)     )
    dot_spine = dot_spine_e;
  else
    {
    sprintf(errbuf,"dot_spine%%sur850*lftcsys");
    return(varkon_erpush("SU2993",errbuf));
    }


  if ( dot_spine >  0.0 )
    {
    points[0].x_gm = pcloses.x_gm;
    points[0].y_gm = pcloses.y_gm;
    points[0].z_gm = pcloses.z_gm;
    points[1].x_gm = pclosee.x_gm;
    points[1].y_gm = pclosee.y_gm;
    points[1].z_gm = pclosee.z_gm;
    points[2].x_gm = pcloses.x_gm;
    points[2].y_gm = pcloses.y_gm;
    points[2].z_gm = pcloses.z_gm;
    points[3].x_gm = pclosee.x_gm;
    points[3].y_gm = pclosee.y_gm;
    points[3].z_gm = pclosee.z_gm;
    }
  else
    {
    points[0].x_gm = pclosee.x_gm;
    points[0].y_gm = pclosee.y_gm;
    points[0].z_gm = pclosee.z_gm;
    points[1].x_gm = pcloses.x_gm;
    points[1].y_gm = pcloses.y_gm;
    points[1].z_gm = pcloses.z_gm;
    points[2].x_gm = pclosee.x_gm;
    points[2].y_gm = pclosee.y_gm;
    points[2].z_gm = pclosee.z_gm;
    points[3].x_gm = pcloses.x_gm;
    points[3].y_gm = pcloses.y_gm;
    points[3].z_gm = pcloses.z_gm;
    }

    pvalue = 0.5;

status=GE133 (points,pvalue,&spineseg);
      if (status<0) 
         {
         sprintf(errbuf,"GE133%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }

   spine.al_cu = spine_length;
   spineseg.sl = spine_length;

    origin.x_gm = points[0].x_gm;
    origin.y_gm = points[0].y_gm;
    origin.z_gm = points[0].z_gm;
    xaxis.x_gm  = points[3].x_gm-points[0].x_gm;
    xaxis.y_gm  = points[3].y_gm-points[0].y_gm;
    xaxis.z_gm  = points[3].z_gm-points[0].z_gm;

    if       ( dists >= distm && dists >= diste  ) 
      {
      yaxis.x_gm  = pexts.x_gm-pcloses.x_gm;
      yaxis.y_gm  = pexts.y_gm-pcloses.y_gm;
      yaxis.z_gm  = pexts.z_gm-pcloses.z_gm;
      }
    else if  ( distm >= dists && distm >= diste  ) 
      {
      yaxis.x_gm  = pextm.x_gm-pclosem.x_gm;
      yaxis.y_gm  = pextm.y_gm-pclosem.y_gm;
      yaxis.z_gm  = pextm.z_gm-pclosem.z_gm;
      }
    else if  ( diste >= dists && diste >= distm  ) 
      {
      yaxis.x_gm  = pexte.x_gm-pclosee.x_gm;
      yaxis.y_gm  = pexte.y_gm-pclosee.y_gm;
      yaxis.z_gm  = pexte.z_gm-pclosee.z_gm;
      }
    else
      {
      sprintf(errbuf,"yaxis%%sur850");
      return(varkon_erpush("SU2993",errbuf));
      }


    status = GEmktf_3p (&origin,&xaxis,&yaxis,&spine_csy);
if (status<0) 
  {
  sprintf(errbuf,"sur690%%sur850");
  return(varkon_erpush("SU2943",errbuf));
  }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*lftcsys dists %f distm %f diste %f\n", dists, distm ,diste);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*lftcsys spine_length %f dot_spine %f\n", 
          spine_length, dot_spine);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*lftcsys dot_spine_s %f dot_spine_m %f, dot_spine_e %f\n", 
          dot_spine_s , dot_spine_m, dot_spine_e);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*lftcsys textm   %f  %f  %f\n", 
          textm.x_gm, textm.y_gm, textm.z_gm );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*lftcsys compare %f  %f  %f\n", 
pclosee.x_gm-pcloses.x_gm, pclosee.y_gm-pcloses.y_gm, 
pclosee.z_gm-pcloses.z_gm );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*lftcsys pclosee %f  %f  %f\n", 
          pclosee.x_gm, pclosee.y_gm, pclosee.z_gm );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*lftcsys pcloses %f  %f  %f\n", 
          pcloses.x_gm, pcloses.y_gm, pcloses.z_gm );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*lftcsys origin %f %f %f\n", 
                origin.x_gm, origin.y_gm, origin.z_gm );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*lftcsys xaxis  %f %f %f\n", 
                xaxis.x_gm, xaxis.y_gm, xaxis.z_gm );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*lftcsys g11 %f g12 %f g13 %f g14 %f\n", 
   spine_csy.g11, spine_csy.g12, spine_csy.g13, spine_csy.g14  );
  fprintf(dbgfil(SURPAC),
  "sur850*lftcsys g21 %f g22 %f g23 %f g24 %f\n", 
   spine_csy.g21, spine_csy.g22, spine_csy.g23, spine_csy.g24  );
  fprintf(dbgfil(SURPAC),
  "sur850*lftcsys g31 %f g32 %f g33 %f g34 %f\n", 
   spine_csy.g31, spine_csy.g32, spine_csy.g33, spine_csy.g34  );
  fprintf(dbgfil(SURPAC),
  "sur850*lftcsys g41 %f g42 %f g43 %f g44 %f\n", 
   spine_csy.g41, spine_csy.g42, spine_csy.g43, spine_csy.g44  );
  }
#endif


    return(SUCCED);

} /* End of function                                                */

/*********************************************************/







/*!********* Internal ** function ** angles  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Calculates rotation angles (number of surface strips) and        */
/* and rotation matrices.                                           */

   static short angles ( ang1, ang2, r_case )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBfloat  ang1;        /* Start angle                             */
   DBfloat  ang2;        /* End   angle                             */
   DBint    r_case;      /* Rotation case                           */
                         /* Eq.  1: Rotation in positive direction  */
                         /* Eq. -1: Rotation in negative direction  */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  ang1_m;      /* Modified ang1 = ang1 +/- n*360.0        */
   DBfloat  ang2_m;      /* Modified ang2 = ang2 +/- n*360.0        */
/*----------------------------------------------------------------- */
   DBfloat  sx,sy,sz;    /* Scaling factors                         */
   DBint    status;      /* Error code from called function         */
   char     errbuf[80];  /* String for error message fctn erpush    */
   char     errbuf2[80]; /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur850*angles* Enter ang1 %f ang2 %f r_case %d\n",
                      ang1,   ang2,(int)r_case);
}
#endif

/* Initializations                                                  */

   ang1_m = F_UNDEF;
   ang2_m = F_UNDEF;


/*!                                                                 */
/* Check input angles and rotation direction                        */
/*                                                                 !*/

  if ( ang1 > -360.0-idangle && ang1 < 360.0+idangle &&
       ang2 > -360.0-idangle && ang2 < 360.0+idangle    )
    {
    ; /* Angles are within definition area. Equality checked below */
    }
  else
    {
    sprintf(errbuf,"sur850%% ");
    varkon_erinit();
    return(varkon_erpush("SU8043",errbuf));
    }


  if ( r_case == -1 || r_case == 1 )
    {
    ; /* Rotation direction is OK */
    }
  else
    {
    sprintf(errbuf,"sur850%% ");
    varkon_erinit();
    return(varkon_erpush("SU8053",errbuf));
    }


/*!                                                                 */
/* Modify input angles with +/- n*360 degrees.                      */
/*                                                                 !*/


  if   ( r_case == 1 )
    {
    if ( ang1 <  0.0 )     ang1_m = ang1 + 360;
    else                   ang1_m = ang1;
    if ( ang2 <= 0.0 )     ang2_m = ang2 + 360;
    else                   ang2_m = ang2;
    if ( fabs(ang2_m-ang1_m) < 10.0*idangle ) /* Equal angles ?     */
      {
      sprintf(errbuf2,"%8.2f%%%8.2f",ang1, ang2);
      sprintf(errbuf,"%6.4f%%(sur850)",10.0*idangle);
      varkon_erinit();
      varkon_erpush("SU8063",errbuf);
      return(varkon_erpush("SU8083",errbuf2));
      } /* End equal angles */
    
    if ( ang2_m < ang1_m ) ang2_m = ang2_m + 360.0;
    }

/* The start rotation angle ang1_m shall be a negative angle        */
/* equal to 0.0 or less than -360.0                                 */
  else if   ( r_case == -1 )
    {

    if ( ang1 >  0.0 )     ang1_m = ang1 - 360.0;
    else                   ang1_m = ang1;
    if ( ang2 >= 0.0 )     ang2_m = ang2 - 360.0;
    else                   ang2_m = ang2;


    if ( fabs(ang2_m-ang1_m) < 10.0*idangle ) /* Equal angles ?     */
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur850*angles ang1_m %8.3f = ang2_m %8.3f\n", ang1_m, ang2_m);
}
#endif

      sprintf(errbuf2,"%8.2f%%%8.2f",ang1, ang2);
      sprintf(errbuf,"%6.4f%%(sur850)",10.0*idangle);
      varkon_erinit();
      varkon_erpush("SU8063",errbuf);
      return(varkon_erpush("SU8093",errbuf2));
      } /* End equal angles */
    
    if ( ang2_m > ang1_m ) ang2_m = ang2_m - 360.0;
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur850*angles ang1_m %8.3f ang2_m %8.3f\n", ang1_m, ang2_m);
}
#endif

/*!                                                                 */
/* Determine the number of segments and delta angles                */
/*                                                                 !*/

   if      ( ang2_m-ang1_m <=  90.0 + 2.0*idangle && r_case ==   1 ) 
     {
     no_seg   =    1;
     ang1_rot = ang1_m;
     ang2_rot = ang2_m;
     }
   else if ( ang1_m-ang2_m <=  90.0 + 2.0*idangle && r_case == - 1 ) 
     {
     no_seg   =    1;
     ang1_rot = ang1_m;
     ang2_rot = ang2_m;
     }
   else if ( ang2_m-ang1_m <= 180.0 + 2.0*idangle && r_case ==   1 ) 
     {
     no_seg   =    2;
     ang1_rot = ang1_m;
     ang2_rot = ang1_rot + (ang2_m-ang1_m)/2.0;
     ang3_rot = ang2_rot + (ang2_m-ang1_m)/2.0;
     }
   else if ( ang1_m-ang2_m <= 180.0 + 2.0*idangle && r_case == - 1 ) 
     {
     no_seg   =    2;
     ang1_rot = ang1_m;
     ang2_rot = ang1_rot - (ang1_m-ang2_m)/2.0;
     ang3_rot = ang2_rot - (ang1_m-ang2_m)/2.0;
     }
   else if ( ang2_m-ang1_m <= 270.0 + 2.0*idangle && r_case ==   1 ) 
     {
     no_seg   =    3;
     ang1_rot = ang1_m;
     ang2_rot = ang1_rot + (ang2_m-ang1_m)/3.0;
     ang3_rot = ang2_rot + (ang2_m-ang1_m)/3.0;
     ang4_rot = ang3_rot + (ang2_m-ang1_m)/3.0;
     }
   else if ( ang1_m-ang2_m <= 270.0 + 2.0*idangle && r_case == - 1 ) 
     {
     no_seg   =    3;
     ang1_rot = ang1_m;
     ang2_rot = ang1_rot - (ang1_m-ang2_m)/3.0;
     ang3_rot = ang2_rot - (ang1_m-ang2_m)/3.0;
     ang4_rot = ang3_rot - (ang1_m-ang2_m)/3.0;
     }
   else if ( ang2_m-ang1_m <= 360.0 + 2.0*idangle && r_case ==   1 ) 
     {
     no_seg   =    4;
     ang1_rot = ang1_m;
     ang2_rot = ang1_rot + (ang2_m-ang1_m)/4.0;
     ang3_rot = ang2_rot + (ang2_m-ang1_m)/4.0;
     ang4_rot = ang3_rot + (ang2_m-ang1_m)/4.0;
     ang5_rot = ang4_rot + (ang2_m-ang1_m)/4.0;
     }
   else if ( ang1_m-ang2_m <= 360.0 + 2.0*idangle && r_case == - 1 ) 
     {
     no_seg   =    4;
     ang1_rot = ang1_m;
     ang2_rot = ang1_rot - (ang1_m-ang2_m)/4.0;
     ang3_rot = ang2_rot - (ang1_m-ang2_m)/4.0;
     ang4_rot = ang3_rot - (ang1_m-ang2_m)/4.0;
     ang5_rot = ang4_rot - (ang1_m-ang2_m)/4.0;
     }
   else
    {
    sprintf(errbuf,"no_seg%%(sur850)");
    varkon_erinit();
    return(varkon_erpush("SU2993",errbuf));
    }


/*!                                                                 */
/* Calculate rotation matrices the limit curves.                    */
/* Calls of GEmktf_rs.                                              */
/*                                                                 !*/

   sx = 1.0;
   sy = 1.0;
   sz = 1.0;

status=GEmktf_rs
      (&pcloses,&pclosee,ang1_rot,sx,sy,sz,&ang1_tra);
      if (status<0) 
         {
         sprintf(errbuf,"GEmktf_rs(1)%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }


status=GEmktf_rs
      (&pcloses,&pclosee,ang2_rot,sx,sy,sz,&ang2_tra);
      if (status<0) 
         {
         sprintf(errbuf,"GEmktf_rs(2)%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }


if  ( no_seg >= 2 )
{
status=GEmktf_rs
      (&pcloses,&pclosee,ang3_rot,sx,sy,sz,&ang3_tra);
      if (status<0) 
         {
         sprintf(errbuf,"GEmktf_rs(3)%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }
}

if  ( no_seg >= 3 )
{
status=GEmktf_rs
      (&pcloses,&pclosee,ang4_rot,sx,sy,sz,&ang4_tra);
      if (status<0) 
         {
         sprintf(errbuf,"GEmktf_rs(4)%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }
}

if  ( no_seg >= 4 )
{
status=GEmktf_rs
      (&pcloses,&pclosee,ang5_rot,sx,sy,sz,&ang5_tra);
      if (status<0) 
         {
         sprintf(errbuf,"GEmktf_rs(5)%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }
}



#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  if ( no_seg == 1 )
  fprintf(dbgfil(SURPAC),
  "sur850*angles ang1_rot %8.3f ang2_rot %8.3f\n", ang1_rot, ang2_rot );
  if ( no_seg == 2 )
  fprintf(dbgfil(SURPAC),
  "sur850*angles ang1_rot %8.3f ang2_rot %8.3f ang3_rot %8.3f\n", 
                  ang1_rot, ang2_rot, ang3_rot );
  if ( no_seg == 3 )
  fprintf(dbgfil(SURPAC),
  "sur850*angles ang1_rot %8.3f ang2_rot %8.3f ang3_rot %8.3f ang4_rot %8.3f\n", 
                  ang1_rot, ang2_rot, ang3_rot, ang4_rot );
  if ( no_seg == 4 )
  fprintf(dbgfil(SURPAC),
  "sur850*angles ang1_rot %8.3f ang2_rot %8.3f ang3_rot %8.3f ang4_rot %8.3f ang5_rot %8.3f\n", 
                  ang1_rot, ang2_rot, ang3_rot, ang4_rot, ang5_rot );
  }
#endif


    return(SUCCED);

} /* End of function                                                */

/*********************************************************/






/*!********* Internal ** function ** tcurve  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Creates a tangent curve to the input profile curve               */

   static short tcurve ( p_cur, p_seg )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur;       /* Rotation curve                    (ptr) */
   DBSeg   *p_seg;       /* Segment data for p_cur            (ptr) */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  sx,sy,sz;    /* Scaling factors                         */
/*----------------------------------------------------------------- */
   DBVector pexts_l;     /* Curve start point in local system       */
   DBVector ptrans_l;    /* Start tang. point in local system       */
   DBVector ptrans;      /* Start tang. point in basic system       */
   DBint    status;      /* Error code from called function         */
   char     errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*tcurve * Enter ** \n");
  }
#endif

/*!                                                                 */
/* Calculate translation matrix for the tangent curve.              */
/* Call of GEtfpos_to_local, _GEtfpos_to_basic and _GEmktf_ts.      */
/*                                                                 !*/

status=GEtfpos_to_local
      (&pexts  ,&spine_csy, &pexts_l);
      if (status<0) 
         {
         sprintf(errbuf,"GEtfpos_to_local%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }

   ptrans_l.x_gm = pexts_l.x_gm;
   ptrans_l.y_gm = pexts_l.y_gm;
   ptrans_l.z_gm = pexts_l.z_gm + 0.25*spine_length;

status=GEtfpos_to_basic
      (&ptrans_l,&spine_csy, &ptrans);
      if (status<0) 
         {
         sprintf(errbuf,"GEtfpos_to_basic%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }
   sx = 1.0;
   sy = 1.0;
   sz = 1.0;

status=GEmktf_ts
      (&pexts  ,&ptrans ,sx,sy,sz,&tang_tra);
      if (status<0) 
         {
         sprintf(errbuf,"GEmktf_ts%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }

/*!                                                                 */
/* Allocate memory and create start tangent curve.                  */
/* Call of DBcreate_segments and GEtfCurve_to_local.                */
/*                                                                 !*/

    p_tangs_seg = DBcreate_segments(p_cur->ns_cu);
    V3MOME((char *)(p_cur),(char *)(&tangs),sizeof(DBCurve));

status=GEtfCurve_to_local
      (p_cur, p_seg,NULL, &tang_tra, &tangs , p_tangs_seg, NULL );
      if (status<0) 
         {
         sprintf(errbuf,"GEtfCurve_to_local(ts)%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*tcurve tangs.ns_cu %d p_tangs_seg %d \n", 
    tangs.ns_cu ,  (int)p_tangs_seg );
  }
#endif




    return(SUCCED);

} /* End of function                                                */

/*********************************************************/







/*!********* Internal ** function ** transf  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Creates a tangent curve to the input profile curve               */

   static short transf ( p_cur, p_seg )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur;       /* Rotation curve                    (ptr) */
   DBSeg   *p_seg;       /* Segment data for p_cur            (ptr) */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */
   DBint    status;      /* Error code from called function         */
   char     errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*transf * Enter ** no_seg %d\n", (int)no_seg );
  }
#endif

/*!                                                                 */
/* Allocate memory and create limit and tangent curves.             */
/* Calls of DBcreate_segments and GEtfCurve_to_local.               */
/*                                                                 !*/

    p_limit1_seg = DBcreate_segments(p_cur->ns_cu);
    V3MOME((char *)(p_cur),(char *)(&limit1),sizeof(DBCurve));
status=GEtfCurve_to_local
      (p_cur, p_seg, NULL, &ang1_tra, &limit1, p_limit1_seg, NULL );
      if (status<0) 
         {
         sprintf(errbuf,"GEtfCurve_to_local(l1)%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }

    p_tang1_seg  = DBcreate_segments(tangs.ns_cu);
    V3MOME((char *)(&tangs),(char *)(&tang1),sizeof(DBCurve));
status=GEtfCurve_to_local
   (&tangs, p_tangs_seg, NULL, &ang1_tra, &tang1 , p_tang1_seg, NULL );
     if (status<0) 
         {
         sprintf(errbuf,"GEtfCurve_to_local(t1)%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }



    p_limit2_seg = DBcreate_segments(p_cur->ns_cu);
    V3MOME((char *)(p_cur),(char *)(&limit2),sizeof(DBCurve));


status=GEtfCurve_to_local
      (p_cur, p_seg, NULL, &ang2_tra, &limit2, p_limit2_seg, NULL );
      if (status<0) 
         {
         sprintf(errbuf,"GEtfCurve_to_local(l2)%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }



    p_tang2_seg  = DBcreate_segments(tangs.ns_cu);
    V3MOME((char *)(&tangs),(char *)(&tang2),sizeof(DBCurve));
status=GEtfCurve_to_local
   (&tangs, p_tangs_seg, NULL, &ang2_tra, &tang2 , p_tang2_seg, NULL );
      if (status<0) 
         {
         sprintf(errbuf,"GEtfCurve_to_local(t2)%%sur850");
         return(varkon_erpush("SU2943",errbuf));
        }



  if  ( no_seg >= 2 )
    {
    p_limit3_seg = DBcreate_segments(p_cur->ns_cu);
    V3MOME((char *)(p_cur),(char *)(&limit3),sizeof(DBCurve));
status=GEtfCurve_to_local
      (p_cur, p_seg, NULL, &ang3_tra, &limit3, p_limit3_seg, NULL );
      if (status<0) 
         {
         sprintf(errbuf,"GEtfCurve_to_local(l3)%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }
    p_tang3_seg  = DBcreate_segments(tangs.ns_cu);
    V3MOME((char *)(&tangs),(char *)(&tang3),sizeof(DBCurve));
status=GEtfCurve_to_local
   (&tangs, p_tangs_seg, NULL, &ang3_tra, &tang3 , p_tang3_seg, NULL );
      if (status<0) 
         {
         sprintf(errbuf,"GEtfCurve_to_local(t3)%%sur850");
         return(varkon_erpush("SU2943",errbuf));
        }
     }


  if  ( no_seg >= 3 )
    {
    p_limit4_seg = DBcreate_segments(p_cur->ns_cu);
    V3MOME((char *)(p_cur),(char *)(&limit4),sizeof(DBCurve));
status=GEtfCurve_to_local
      (p_cur, p_seg, NULL, &ang4_tra, &limit4, p_limit4_seg, NULL );
      if (status<0) 
         {
         sprintf(errbuf,"GEtfCurve_to_local%%(l4)sur850");
         return(varkon_erpush("SU2943",errbuf));
         }
    p_tang4_seg  = DBcreate_segments(tangs.ns_cu);
    V3MOME((char *)(&tangs),(char *)(&tang4),sizeof(DBCurve));
status=GEtfCurve_to_local
   (&tangs, p_tangs_seg, NULL, &ang4_tra, &tang4 , p_tang4_seg, NULL );
      if (status<0) 
         {
         sprintf(errbuf,"GEtfCurve_to_local(t4)%%sur850");
         return(varkon_erpush("SU2943",errbuf));
        }
      }


  if  ( no_seg >= 4 )
    {
    p_limit5_seg = DBcreate_segments(p_cur->ns_cu);
    V3MOME((char *)(p_cur),(char *)(&limit5),sizeof(DBCurve));
status=GEtfCurve_to_local
      (p_cur, p_seg, NULL, &ang5_tra, &limit5, p_limit5_seg, NULL );
      if (status<0) 
         {
         sprintf(errbuf,"GEtfCurve_to_local(l5)%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }
    p_tang5_seg  = DBcreate_segments(tangs.ns_cu);
    V3MOME((char *)(&tangs),(char *)(&tang5),sizeof(DBCurve));
status=GEtfCurve_to_local
   (&tangs, p_tangs_seg, NULL, &ang5_tra, &tang5 , p_tang5_seg, NULL );
      if (status<0) 
         {
         sprintf(errbuf,"GEtfCurve_to_local(t5)%%sur850");
         return(varkon_erpush("SU2943",errbuf));
        }
      }




#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*transf p_limit1_seg %d p_limit2_seg %d no_seg %d\n", 
    (int)p_limit1_seg, (int)p_limit2_seg, (int)no_seg );
  fprintf(dbgfil(SURPAC),
  "sur850*transf p_limit3_seg %d p_limit4_seg %d p_limit5_seg %d\n", 
    (int)p_limit3_seg, (int)p_limit4_seg, (int)p_limit5_seg );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*transf p_tang1_seg %d p_tang2_seg %d no_seg %d\n", 
    (int)p_tang1_seg, (int)p_tang2_seg, (int)no_seg );
  fprintf(dbgfil(SURPAC),
  "sur850*transf p_tang3_seg %d p_tang4_seg %d p_tang5_seg %d\n", 
    (int)p_tang3_seg, (int)p_tang4_seg, (int)p_tang5_seg );
  }
#endif



    return(SUCCED);

} /* End of function                                                */

/*********************************************************/




/*!********* Internal ** function ** midcrv  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Creates the P-value mid curve                                    */

   static short midcrv (  )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  pvalmid;     /* P value corresponding to delta angle    */
/*----------------------------------------------------------------- */
   DBVector points[4];   /* points[0]    = start point              */
                         /* points[1]    = start tangent point      */
                         /* points[2]    = end   tangent point      */
                         /* points[3]    = end   point              */
   DBfloat  pvalue;      /* P value for the rational segment        */
   DBfloat  d_angle;     /* Delta angle for one V strip in radians  */
   DBfloat  denom;       /* Denominator P value                     */
   DBint    status;      /* Error code from called function         */
   char     errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*midcrv * Enter ** \n");
  }
#endif

/*!                                                                 */
/* Calculate the P-value for the V strips (all are equal since      */
/* the delta angles ang2_rot-ang1_rot, .. are equal)                */
/*                                                                 !*/

    d_angle = fabs(ang2_rot - ang1_rot)*PI/180.0; 

    denom   = SQRT(1.0 + tan(d_angle/2.0)*tan(d_angle/2.0)) - 
                   cos(d_angle/2.0);
    if  ( fabs(denom) < 100.0*comptol )
      {
      sprintf(errbuf,"denom%%sur850*midcrv");
      return(varkon_erpush("SU2993",errbuf));
      }

    pvalmid = (1.0 - cos(d_angle/2.0))/denom;

    if  ( pvalmid < 10.0*comptol )
      {
      sprintf(errbuf,"pval<0.0%%sur850*midcrv");
      return(varkon_erpush("SU2993",errbuf));
      }
    if  ( pvalmid > 0.85         )
      {
      sprintf(errbuf,"pval<0.85%%sur850*midcrv");
      return(varkon_erpush("SU2993",errbuf));
      }

/*!                                                                 */
/* Create the P-value (mid) curve                                   */
/*                                                                 !*/

    points[0].x_gm = 0.0;
    points[0].y_gm = pvalmid;
    points[0].z_gm = 0.0;
    points[1].x_gm = spine_length;
    points[1].y_gm = pvalmid;
    points[1].z_gm = 0.0;
    points[2].x_gm = 0.0;
    points[2].y_gm = pvalmid;
    points[2].z_gm = 0.0;
    points[3].x_gm = spine_length;
    points[3].y_gm = pvalmid;
    points[3].z_gm = 0.0;


    pvalue = 0.5;

status=GE133 (points,pvalue,&midpvalseg);
      if (status<0) 
         {
         sprintf(errbuf,"GE133%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }

   midpval.al_cu = spine_length;
   midpvalseg.sl = spine_length;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*midcrv * Exit** P-value %f for delta angle %f\n",
            pvalmid,d_angle*180.0/PI);
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/


/*!********* Internal ** function ** curfree ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Deallocation of memory for curves                                */

   static short curfree  (  )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

    if ( p_limit1_seg != NULL ) DBfree_segments(p_limit1_seg);
    if ( p_limit2_seg != NULL ) DBfree_segments(p_limit2_seg);
    if ( p_limit3_seg != NULL ) DBfree_segments(p_limit3_seg);
    if ( p_limit4_seg != NULL ) DBfree_segments(p_limit4_seg);
    if ( p_limit5_seg != NULL ) DBfree_segments(p_limit5_seg);
    if ( p_tangs_seg  != NULL ) DBfree_segments(p_tangs_seg);
    if ( p_tang1_seg  != NULL ) DBfree_segments(p_tang1_seg);
    if ( p_tang2_seg  != NULL ) DBfree_segments(p_tang2_seg);
    if ( p_tang3_seg  != NULL ) DBfree_segments(p_tang3_seg);
    if ( p_tang4_seg  != NULL ) DBfree_segments(p_tang4_seg);
    if ( p_tang5_seg  != NULL ) DBfree_segments(p_tang5_seg);

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*curfree* Memory temp. limit and tangent crvs deallocated\n");
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/





/*!********* Internal ** function ** checur  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Check input rotation curve                                       */

   static short checur   ( p_cur, p_seg, p_p1, p_p2 , sur_type  )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve  *p_cur;      /* Rotation curve                    (ptr) */
   DBSeg    *p_seg;      /* Segment data for p_cur            (ptr) */
   DBVector *p_p1;       /* Rotation axis point 1             (ptr) */
   DBVector *p_p2;       /* Rotation axis point 2             (ptr) */
   DBint     sur_type;   /* Output surface type                     */
                         /* Eq.  1: LFT_SUR, p_flag= 3              */
                         /* Eq.  2: LFT_SUR, p_flag= 2              */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint    i_seg;        /* Loop index segment in curve             */
  DBVector s_pt;         /* Start point   curve segment             */
  DBVector s_pt_l;       /* Start point   curve segment local syst. */
  DBVector e_pt;         /* End   point   curve segment             */
  DBVector e_pt_l;       /* End   point   curve segment local syst. */
  DBVector s_t;          /* Start tangent curve segment             */
  DBVector s_t_l;        /* Start tangent curve segment local syst. */
  DBVector e_t;          /* End   tangent curve segment             */
  DBVector e_t_l;        /* End   tangent curve segment local syst. */
  DBfloat  s_tan[3];     /* Start tangent curve segment local syst. */
  DBfloat  e_tan[3];     /* End   tangent curve segment local syst. */
  DBfloat  xaxis[3];     /* X axis                      local syst. */
  DBfloat  zaxis[3];     /* Z axis                      local syst. */
  DBfloat  s_xangle;     /* Start tangent angle with the X axis     */
  DBfloat  e_xangle;     /* End   tangent angle with the X axis     */
  DBfloat  s_zangle;     /* Start tangent angle with the Z axis     */
  DBfloat  e_zangle;     /* End   tangent angle with the Z axis     */
  EVALC    xyz;          /* Coordinates and derivatives for crv pt  */
  DBint    f_smod;       /* Modification flag =1: Start modified    */
  DBint    f_emod;       /* Modification flag =1: End   modified    */
/*----------------------------------------------------------------- */

  DBint  status;         /* Error code from called function         */
  char   errbuf[80];     /* String for error message fctn erpush    */
/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*checur * Enter ** p_cur %d p_seg %d p_cur->ns_cu %d\n", 
          (int)p_cur, (int)p_seg,  p_cur->ns_cu );
  }
#endif

   xyz.evltyp   = EVC_DR; 

   f_smod = 0;
   f_emod = 0;

   for ( i_seg = 1; i_seg <= p_cur->ns_cu; i_seg = i_seg + 1 )
      {

modseg:; /* Label: Start or end segment has been modified           */

/*!                                                                 */
/*    Curve evaluation at start and end of segment i_seg            */
/*    Calls of GE109.                                               */
/*                                                                 !*/

/*    Start of segment i_seg                                   */

      xyz.t_global = (DBfloat)i_seg + comptol;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur850*checur i_seg %d xyz.t_global %25.15f for point s_pt\n",
  (int)i_seg,xyz.t_global );
fflush(dbgfil(SURPAC));
}
#endif


      status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
      if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_sur_rot");
        return(erpush("SU2943",errbuf));
        }

      s_pt = xyz.r;
      s_t  = xyz.drdt;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur850*checur i_seg %d s_pt %10.2f %10.2f %10.2f\n",
  (int)i_seg,s_pt.x_gm,s_pt.y_gm,s_pt.z_gm );
fflush(dbgfil(SURPAC));
}
#endif


/*    End of current segment i_seg                                  */

      xyz.t_global = (DBfloat)i_seg + 1.0 - comptol;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur850*checur i_seg %d xyz.t_global %25.15f for point e_pt\n",
  (int)i_seg,xyz.t_global );
fflush(dbgfil(SURPAC));
}
#endif


      status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
      if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_sur_rot     ");
        return(erpush("SU2943",errbuf));
        }

      e_pt = xyz.r;
      e_t  = xyz.drdt;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur850*checur i_seg %d e_pt %10.2f %10.2f %10.2f\n",
  (int)i_seg,e_pt.x_gm,e_pt.y_gm,e_pt.z_gm );
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/* Transformation of point and vector to local system.              */
/* Call of GEtfpos_to_local and GEtfvec_to_local.                   */
/*                                                                 !*/

status=GEtfpos_to_local (&s_pt   ,&spine_csy, &s_pt_l);
      if (status<0) 
         {
         sprintf(errbuf,"GEtfpos_to_local%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }

status=GEtfpos_to_local (&e_pt   ,&spine_csy, &e_pt_l);
      if (status<0) 
         {
         sprintf(errbuf,"GEtfpos_to_local%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }


status=GEtfvec_to_local (&s_t   ,&spine_csy, &s_t_l);
      if (status<0) 
         {
         sprintf(errbuf,"GEtfvec_to_local%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }

status=GEtfvec_to_local (&e_t   ,&spine_csy, &e_t_l);
      if (status<0) 
         {
         sprintf(errbuf,"GEtfvec_to_local%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }

      s_tan[0]= s_t_l.x_gm;
      s_tan[1]= s_t_l.y_gm;
      s_tan[2]= s_t_l.z_gm;
      e_tan[0]= e_t_l.x_gm;
      e_tan[1]= e_t_l.y_gm;
      e_tan[2]= e_t_l.z_gm;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur850*checur i_seg %d s_pt_l %10.2f %10.2f %10.2f\n",
  (int)i_seg,s_pt_l.x_gm,s_pt_l.y_gm,s_pt_l.z_gm );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur850*checur i_seg %d e_pt_l %10.2f %10.2f %10.2f\n",
  (int)i_seg,e_pt_l.x_gm,e_pt_l.y_gm,e_pt_l.z_gm );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur850*checur i_seg %d Start_z %10.6f End_z %10.6f idpoint %10.6f\n",
  (int)i_seg,s_pt_l.z_gm,e_pt_l.z_gm,idpoint);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur850*checur s_tan %f %f %f \n", s_tan[0],s_tan[1],s_tan[2]   );
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur850*checur e_tan %f %f %f \n", e_tan[0],e_tan[1],e_tan[2]   );
}
#endif

/*!                                                                 */
/* Check that points are in the X/Y plane                           */
/*                                                                 !*/

   if  ( fabs(s_pt_l.z_gm) > idpoint ||  fabs(e_pt_l.z_gm) > idpoint )
         {
         sprintf(errbuf,"sur850%% ");
         varkon_erinit();
         return(varkon_erpush("SU8013",errbuf));
         }

/*!                                                                 */
/* Check that segments not ar of type UV, i.e. surface curve that   */
/* cannot be transformed (surface must be transformed for this case)*/
/*                                                                 !*/

      if  ( (p_seg+i_seg-1)->typ == UV_CUB_SEG )
         {
         sprintf(errbuf,"sur850%% ");
         varkon_erinit();
         return(varkon_erpush("SU8023",errbuf));
         }

/*!                                                                 */
/* Check that tangents are in the direction of the spine (X axis)   */
/*                                                                 !*/

  xaxis[0] = 1.0;
  xaxis[1] = 0.0;
  xaxis[2] = 0.0;

         status= varkon_angd (xaxis,s_tan,&s_xangle);
         if (status<0) 
           {
           sprintf(errbuf,"varkon_angd%%sur850");
           return(erpush("SU2943",errbuf));
           }

         status= varkon_angd (xaxis,e_tan,&e_xangle);
         if (status<0) 
           {
           sprintf(errbuf,"varkon_angd%%sur850");
           return(erpush("SU2943",errbuf));
           }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur850*checur i_seg %d Angle with X axis: Start %f End %f Criterion %f\n",
  (int)i_seg, s_xangle, e_xangle, 90.0 - idangle );
fflush(dbgfil(SURPAC));
}
#endif

     if ( sur_type == 2 )
      {
      if  ( (fabs(s_xangle-90.0) <= idangle || dists <= idpoint)  && 
            i_seg == 1 && f_smod == 0 )
        {
        status = modcur(p_cur, p_seg, i_seg, &s_t);
        f_smod = 1;
        goto modseg;
        }
      if  ( (fabs(e_xangle-90.0) <= idangle || diste <= idpoint)  && 
            i_seg == p_cur->ns_cu && f_emod == 0 )
        {
        status = modcur(p_cur, p_seg, i_seg, &e_t);
        f_emod = 1;
        goto modseg;
        }
       }  /* sur_type == 2 */



      if  ( sur_type == 2 )
        {
        if  ( s_xangle > 90.0 + idangle  || e_xangle > 90.0 + idangle )
           {
           sprintf(errbuf," %% ");
           varkon_erinit();
           return(varkon_erpush("SU8033",errbuf));
           }
         }


/*!                                                                 */
/* Check that tangents are in the X/Y plane                         */
/*                                                                 !*/

  zaxis[0] = 0.0;
  zaxis[1] = 0.0;
  zaxis[2] = 1.0;

         status= varkon_angd (zaxis,s_tan,&s_zangle);
         if (status<0) 
           {
           sprintf(errbuf,"varkon_angd%%sur850");
           return(erpush("SU2943",errbuf));
           }

         status= varkon_angd (zaxis,e_tan,&e_zangle);
         if (status<0) 
           {
           sprintf(errbuf,"varkon_angd%%sur850");
           return(erpush("SU2943",errbuf));
           }

   s_zangle = fabs(s_zangle-90.0);
   e_zangle = fabs(e_zangle-90.0);

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur850*checur i_seg %d Angle with Z axis: Start %f End %f Criterion %f\n",
  (int)i_seg, s_zangle, e_zangle, idangle );
fflush(dbgfil(SURPAC));
}
#endif

      if  ( s_zangle > idangle  || e_zangle > idangle )
         {
         sprintf(errbuf," %% ");
         varkon_erinit();
         return(varkon_erpush("SU8013",errbuf));
         }



      }  /* End loop curve segments i_seg= 1,2,3                    */

/*!                                                                 */
/* Create a new spine if curve has been modified                    */
/*                                                                 !*/

    if  ( f_smod == 1 || f_emod == 1 )
      {
      status= lftcsys   ( p_cur, p_seg, p_p1, p_p2 );
      if (status < 0) 
        {
        return(status);
        }
      }

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/


/*!********* Internal ** function ** modcur  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Modify start and/or end of curve                                 */

   static short modcur   ( p_cur, p_seg, i_seg, p_tan )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve  *p_cur;      /* Rotation curve                    (ptr) */
   DBSeg    *p_seg;      /* Segment data for p_cur            (ptr) */
   DBint     i_seg;      /* Start or end segment                    */
   DBVector *p_tan;      /* Start or end vector               (ptr) */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  t_leng;      /* Length of start or end tangent          */
   DBfloat  delta_u;     /* Delta U for the trimming                */
   DBfloat  u_s;         /* Start U for the trimming                */
   DBfloat  u_e;         /* End   U for the trimming                */
/*----------------------------------------------------------------- */
   DBSeg    trimseg;     /* Trimmed segment                         */
   DBSeg   *p_seg_c;     /* Current segment                   (ptr) */
   DBfloat  uextr[2];    /* Reparameterization parameter values     */
                         /* values for a curve segment              */ 
  DBint     status;      /* Error code from called function         */
  char      errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur850*modcur* Enter**p_cur %d p_seg %d p_cur->ns_cu %d i_seg %d\n", 
        (int)p_cur, (int)p_seg,  p_cur->ns_cu, (int)i_seg );
}
#endif

/*!                                                                 */
/* Initializations                                                  */
/*                                                                 !*/

   p_seg_c = p_seg + i_seg - 1;
   V3MOME((char *)(p_seg_c),(char *)(&trimseg),sizeof(DBSeg));

/*!                                                                 */
/* Calculate length of tangent                                      */
/*                                                                 !*/

   t_leng =  SQRT(p_tan->x_gm*p_tan->x_gm+ 
                  p_tan->y_gm*p_tan->y_gm+ 
                  p_tan->z_gm*p_tan->z_gm);

/*!                                                                 */
/* Calculate delta U for a 1.0*idpoint trimming of the curve        */
/*                                                                 !*/

   if  ( t_leng < 10.0*comptol)
     {
     sprintf(errbuf,"t_leng%%sur850*modcur");
     varkon_erinit();
     return(varkon_erpush("SU2993",errbuf));
     }  /* End t_leng= 0 */

    delta_u = 1.0*idpoint/t_leng; 

/*!                                                                 */
/* Trimming U values                                                */
/*                                                                 !*/

    if       (  i_seg ==    1         ) 
     {
     u_s = 0.0 + delta_u; 
     u_e = 1.0;
     }
    else if  (  i_seg == p_cur->ns_cu ) 
     {
     u_s = 0.0;
     u_e = 1.0 - delta_u;
     }
    else
     {
     sprintf(errbuf,"i_seg%%sur850*modcur");
     varkon_erinit();
     return(varkon_erpush("SU2993",errbuf));
     }  /* End i_seg not start or end segment */

/*!                                                                 */
/* Trim segment. Call of GE135.                                     */
/*                                                                 !*/


   uextr[0] = u_s;
   uextr[1] = u_e;

    status = GE135 (p_seg_c,uextr,&trimseg);     
    if(status<0)
    {
    sprintf(errbuf,"GE135%%(sur850*modcur)");
    return(varkon_erpush("SU2943",errbuf));
    }

   V3MOME((char *)(&trimseg),(char *)(p_seg_c),sizeof(DBSeg));


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur850*modcur idpoint %6.4f delta_u %8.4f u_s %8.4f u_e %8.4f\n", 
         idpoint,delta_u,u_s,u_e );
}
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/



/*!********* Internal ** function ** rotcsys ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*     For a rotation LFT_SUR, p_flag= 3 (sur_type= 1)              */
/* The function creates a local coordinate system with the          */
/* X axis equal to the rotation axis and the spine for the          */
/* surface.                                                         */
/*                                                                  */
/*                                                                  */

   static short rotcsys ( p_cur, p_seg, p_p1, p_p2 )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve   *p_cur;       /* Rotation curve                    (ptr) */
   DBSeg   *p_seg;       /* Segment data for p_cur            (ptr) */
   DBVector *p_p1;        /* Rotation axis point 1             (ptr) */
   DBVector *p_p2;        /* Rotation axis point 2             (ptr) */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBVector pextm;        /* External point, mid   of curve          */
  DBVector pexte;        /* External point, start of curve          */
  DBVector origin;       /* Origin for line coordinate  system      */
  DBVector xaxis;        /* X axis for spine coordinate  system     */
  DBVector yaxis;        /* Y axis for spine coordinate  system     */

/*----------------------------------------------------------------- */
  EVALC    xyz_c;        /* Point and derivatives for curve         */
  DBVector ldir;         /* Direction vector for the line           */
  DBVector pcloses_l;    /* Nearest point on line to pexts          */
  DBVector pclosem;      /* Nearest point on line to pextm          */
  DBVector pclosee_l;    /* Nearest point on line to pexte          */
  DBfloat  params;       /* Parameter for the nearest point pcloses */
  DBfloat  paramm;       /* Parameter for the nearest point pclosem */
  DBfloat  parame;       /* Parameter for the nearest point pclosee */
  DBfloat  distm;        /* Distance line to pextm                  */
  DBVector texts;        /* Start tangent for curve                 */
  DBVector textm;        /* Mid   tangent for curve                 */
  DBVector texte;        /* End   tangent for curve                 */
  DBfloat  dot_spine_s;  /* Scalar product start tangent - chord    */
  DBfloat  dot_spine_m;  /* Scalar product mid   tangent - chord    */
  DBfloat  dot_spine_e;  /* Scalar product end   tangent - chord    */
  DBfloat  dot_spine;    /* Scalar product start tangent - chord    */
  DBVector points[4];    /* points[0]    = start point              */
                         /* points[1]    = start tangent point      */
                         /* points[2]    = end   tangent point      */
                         /* points[3]    = end   point              */
  DBfloat  pvalue;       /* P value for the rational segment        */
  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*rotcsys* Enter **  \n");
  }
#endif

  dists       = F_UNDEF; 
  distm       = F_UNDEF; 
  diste       = F_UNDEF; 
  dot_spine   = F_UNDEF; 
  dot_spine_s = F_UNDEF; 
  dot_spine_m = F_UNDEF; 
  dot_spine_e = F_UNDEF; 

/*!                                                                 */
/* Get coordinates and tangents for start and end of curve.         */
/* Calls of GE109                                                   */
/*                                                                 !*/

   xyz_c.evltyp   = EVC_DR; 

   xyz_c.t_global = 1.0;   /* Global parameter value start point    */


status=GE109 ((DBAny *)p_cur, p_seg, &xyz_c );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur850");
     return(varkon_erpush("SU2943",errbuf));
    }

  pexts.x_gm = xyz_c.r.x_gm;
  pexts.y_gm = xyz_c.r.y_gm;
  pexts.z_gm = xyz_c.r.z_gm;
  texts.x_gm = xyz_c.drdt.x_gm;
  texts.y_gm = xyz_c.drdt.y_gm;
  texts.z_gm = xyz_c.drdt.z_gm;


  xyz_c.t_global = (DBfloat)p_cur->ns_cu/2.0 + 1.0; /* Mid point  */

status=GE109 ((DBAny *)p_cur, p_seg, &xyz_c );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur850");
     return(varkon_erpush("SU2943",errbuf));
    }

  pextm.x_gm = xyz_c.r.x_gm;
  pextm.y_gm = xyz_c.r.y_gm;
  pextm.z_gm = xyz_c.r.z_gm;
  textm.x_gm = xyz_c.drdt.x_gm;
  textm.y_gm = xyz_c.drdt.y_gm;
  textm.z_gm = xyz_c.drdt.z_gm;

  xyz_c.t_global = (DBfloat)p_cur->ns_cu + 1.0;  /* End point        */

status=GE109 ((DBAny *)p_cur, p_seg, &xyz_c );
   if (status<0) 
     {
     sprintf(errbuf,"GE109%%sur850");
     return(varkon_erpush("SU2943",errbuf));
    }

  pexte.x_gm = xyz_c.r.x_gm;
  pexte.y_gm = xyz_c.r.y_gm;
  pexte.z_gm = xyz_c.r.z_gm;
  texte.x_gm = xyz_c.drdt.x_gm;
  texte.y_gm = xyz_c.drdt.y_gm;
  texte.z_gm = xyz_c.drdt.z_gm;




/*!                                                                 */
/* Calculate closest point to the rotation axis.                    */
/* Calls of varkon_lin_ptclose (sur712).                            */
/*                                                                 !*/


  ldir.x_gm = p_p2->x_gm - p_p1->x_gm;
  ldir.y_gm = p_p2->y_gm - p_p1->y_gm;
  ldir.z_gm = p_p2->z_gm - p_p1->z_gm;


    status = varkon_lin_ptclose
   (p_p1, &ldir , &pexts, &pcloses_l, &params, &dists );
    if(status<0)
    {
    sprintf(errbuf,"sur712%%sur850");
    return(varkon_erpush("SU2943",errbuf));
    }

    status = varkon_lin_ptclose
   (p_p1, &ldir , &pextm, &pclosem, &paramm, &distm );
    if(status<0)
    {
    sprintf(errbuf,"sur712%%sur850");
    return(varkon_erpush("SU2943",errbuf));
    }

    status = varkon_lin_ptclose
   (p_p1, &ldir , &pexte, &pclosee_l, &parame, &diste );
    if(status<0)
    {
    sprintf(errbuf,"sur712%%sur850");
    return(varkon_erpush("SU2943",errbuf));
    }



/*!                                                                 */
/* Create spine line (= the rotation axis). Check spine length.     */
/* Call of GE133.                                                   */
/*                                                                 !*/

  spine_length = 
   SQRT ( (p_p1->x_gm-p_p2->x_gm)*(p_p1->x_gm-p_p2->x_gm) +
          (p_p1->y_gm-p_p2->y_gm)*(p_p1->y_gm-p_p2->y_gm) +
          (p_p1->z_gm-p_p2->z_gm)*(p_p1->z_gm-p_p2->z_gm) );

  if ( spine_length < 10.0*idpoint )
    {
    sprintf(errbuf,"%6.3f%%(sur850)",spine_length);
    varkon_erinit();
    return(varkon_erpush("SU8003",errbuf));
    }

  dot_spine_s = texts.x_gm*(p_p2->x_gm-p_p1->x_gm) + 
                texts.y_gm*(p_p2->y_gm-p_p1->y_gm) +  
                texts.z_gm*(p_p2->z_gm-p_p1->z_gm);  
  dot_spine_m = textm.x_gm*(p_p2->x_gm-p_p1->x_gm) + 
                textm.y_gm*(p_p2->y_gm-p_p1->y_gm) +  
                textm.z_gm*(p_p2->z_gm-p_p1->z_gm);  
  dot_spine_e = texte.x_gm*(p_p2->x_gm-p_p1->x_gm) + 
                texte.y_gm*(p_p2->y_gm-p_p1->y_gm) +  
                texte.z_gm*(p_p2->z_gm-p_p1->z_gm);  

  if      (  fabs(dot_spine_s) >= fabs(dot_spine_m) &&
             fabs(dot_spine_s) >= fabs(dot_spine_e)     )
    dot_spine = dot_spine_s;
  else if (  fabs(dot_spine_m) >= fabs(dot_spine_s) &&
             fabs(dot_spine_m) >= fabs(dot_spine_e)     )
    dot_spine = dot_spine_m;
  else if (  fabs(dot_spine_e) >= fabs(dot_spine_s) &&
             fabs(dot_spine_e) >= fabs(dot_spine_m)     )
    dot_spine = dot_spine_e;
  else
    {
    sprintf(errbuf,"dot_spine%%sur850*rotcsys");
    return(varkon_erpush("SU2993",errbuf));
    }



 /*
 ***A straight line segment rotated around an axis to which
 ***it is perpendicular will give a value of dotspine close to zero.
 ***The following original check produced random results in this case.
 *** if ( dot_spine >  0.0 ) ***
 ***To remove the random behaviour it was changed 30/1 2006 JK 
 */

  if ( ABS(dot_spine) > varkon_idangle() )
    {
    points[0].x_gm = p_p1->x_gm;
    points[0].y_gm = p_p1->y_gm;
    points[0].z_gm = p_p1->z_gm;
    points[1].x_gm = p_p2->x_gm;
    points[1].y_gm = p_p2->y_gm;
    points[1].z_gm = p_p2->z_gm;
    points[2].x_gm = p_p1->x_gm;
    points[2].y_gm = p_p1->y_gm;
    points[2].z_gm = p_p1->z_gm;
    points[3].x_gm = p_p2->x_gm;
    points[3].y_gm = p_p2->y_gm;
    points[3].z_gm = p_p2->z_gm;
/*  Not closest points. Used for transformations             */
    pcloses.x_gm   = p_p2->x_gm;
    pcloses.y_gm   = p_p2->y_gm;
    pcloses.z_gm   = p_p2->z_gm;
    pclosee.x_gm   = p_p1->x_gm;
    pclosee.y_gm   = p_p1->y_gm;
    pclosee.z_gm   = p_p1->z_gm;
    }
  else
    {
    points[0].x_gm = p_p2->x_gm;
    points[0].y_gm = p_p2->y_gm;
    points[0].z_gm = p_p2->z_gm;
    points[1].x_gm = p_p1->x_gm;
    points[1].y_gm = p_p1->y_gm;
    points[1].z_gm = p_p1->z_gm;
    points[2].x_gm = p_p2->x_gm;
    points[2].y_gm = p_p2->y_gm;
    points[2].z_gm = p_p2->z_gm;
    points[3].x_gm = p_p1->x_gm;
    points[3].y_gm = p_p1->y_gm;
    points[3].z_gm = p_p1->z_gm;
/*  Not closest points. Used for transformations             */
    pcloses.x_gm   = p_p1->x_gm;
    pcloses.y_gm   = p_p1->y_gm;
    pcloses.z_gm   = p_p1->z_gm;
    pclosee.x_gm   = p_p2->x_gm;
    pclosee.y_gm   = p_p2->y_gm;
    pclosee.z_gm   = p_p2->z_gm;
    }

  pvalue = 0.5;

status=GE133 (points,pvalue,&spineseg);
      if (status<0) 
         {
         sprintf(errbuf,"GE133%%sur850");
         return(varkon_erpush("SU2943",errbuf));
         }

    spine.al_cu = spine_length;
    spineseg.sl = spine_length;

    origin.x_gm = points[0].x_gm;
    origin.y_gm = points[0].y_gm;
    origin.z_gm = points[0].z_gm;
    xaxis.x_gm  = points[3].x_gm-points[0].x_gm;
    xaxis.y_gm  = points[3].y_gm-points[0].y_gm;
    xaxis.z_gm  = points[3].z_gm-points[0].z_gm;

/* Note that local closest points p_closes_l and p_closee_l    */
/* are used. Points p_closes and p_closee are used for the     */
/* calculation of the transformation (rotation) matrices       */
/* For LFT_SUR, type 3 may the projected points be equal       */

    if       ( dists >= distm && dists >= diste  ) 
      {
      yaxis.x_gm  = pexts.x_gm-pcloses_l.x_gm;
      yaxis.y_gm  = pexts.y_gm-pcloses_l.y_gm;
      yaxis.z_gm  = pexts.z_gm-pcloses_l.z_gm;
      }
    else if  ( distm >= dists && distm >= diste  ) 
      {
      yaxis.x_gm  = pextm.x_gm-pclosem.x_gm;
      yaxis.y_gm  = pextm.y_gm-pclosem.y_gm;
      yaxis.z_gm  = pextm.z_gm-pclosem.z_gm;
      }
    else if  ( diste >= dists && diste >= distm  ) 
      {
      yaxis.x_gm  = pexte.x_gm-pclosee_l.x_gm;
      yaxis.y_gm  = pexte.y_gm-pclosee_l.y_gm;
      yaxis.z_gm  = pexte.z_gm-pclosee_l.z_gm;
      }
    else
      {
      sprintf(errbuf,"yaxis%%sur850");
      return(varkon_erpush("SU2993",errbuf));
      }

    status = GEmktf_3p (&origin,&xaxis,&yaxis,&spine_csy);
if (status<0) 
  {
  sprintf(errbuf,"sur690%%sur850");
  return(varkon_erpush("SU2943",errbuf));
  }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*rotcsys dists %f distm %f diste %f\n", dists, distm ,diste);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*rotcsys spine_length %f dot_spine %f\n", 
          spine_length, dot_spine);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*rotcsys dot_spine_s %f dot_spine_m %f, dot_spine_e %f\n", 
          dot_spine_s , dot_spine_m, dot_spine_e);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*rotcsys textm   %f  %f  %f\n", 
          textm.x_gm, textm.y_gm, textm.z_gm );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*rotcsys compare %f  %f  %f\n", 
pclosee.x_gm-pcloses.x_gm, pclosee.y_gm-pcloses.y_gm, 
pclosee.z_gm-pcloses.z_gm );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*rotcsys pclosee %f  %f  %f\n", 
          pclosee.x_gm, pclosee.y_gm, pclosee.z_gm );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*rotcsys pcloses %f  %f  %f\n", 
          pcloses.x_gm, pcloses.y_gm, pcloses.z_gm );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*rotcsys origin %f %f %f\n", origin.x_gm, origin.y_gm, origin.z_gm );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*rotcsys xaxis  %f %f %f\n", xaxis.x_gm, xaxis.y_gm, xaxis.z_gm );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur850*rotcsys g11 %f g12 %f g13 %f g14 %f\n", 
   spine_csy.g11, spine_csy.g12, spine_csy.g13, spine_csy.g14  );
  fprintf(dbgfil(SURPAC),
  "sur850*rotcsys g21 %f g22 %f g23 %f g24 %f\n", 
   spine_csy.g21, spine_csy.g22, spine_csy.g23, spine_csy.g24  );
  fprintf(dbgfil(SURPAC),
  "sur850*rotcsys g31 %f g32 %f g33 %f g34 %f\n", 
   spine_csy.g31, spine_csy.g32, spine_csy.g33, spine_csy.g34  );
  fprintf(dbgfil(SURPAC),
  "sur850*rotcsys g41 %f g42 %f g43 %f g44 %f\n", 
   spine_csy.g41, spine_csy.g42, spine_csy.g43, spine_csy.g44  );
  }
#endif


    return(SUCCED);

} /* End of function                                                */

/*********************************************************/



