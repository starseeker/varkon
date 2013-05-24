/**********************************************************************
*
*    surint.h
*    ========
*
*    This file is part of the VARKON Geometry Library.
*    URL: http://www.varkon.com
*
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Library General Public
*    License as published by the Free Software Foundation; either
*    version 2 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Library General Public License for more details.
*
*    You should have received a copy of the GNU Library General Public
*    License along with this library; if not, write to the Free
*    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*    (C)Microform AB 1984-1999, Gunar Liden, gunnar@microform.se
*
***********************************************************************/

/*!********** Cases of surface interrogations ***********************/
#define  F_SILH       1        /* Finite   eye point for silhouette */
#define  I_SILH       2        /* Infinite eye point for silhouette */
#define  F_ISOP       3        /* Finite   eye point for isophote   */
#define  I_ISOP       4        /* Infinite eye point for isophote   */
#define  SURPLAN      5        /* Surface/plane intersection        */
#define  CLOSEPT      6        /* Closest (projected) point         */
#define  CLOSECRV     7        /* Closest (projected) curve         */
#define  SURAPPR      8        /* Surface approximation             */
#define  S_SILH       9        /* Spine & 2D vector  for silhouette */
#define  BICAPPR     10        /* Approximation to CUB_SUR surface  */
#define  D_GEODES    11        /* Geodesic given start direction    */
#define  LINTSUR     12        /* Line/surface, trans. intersection */
#define  DEVELRUL    13        /* Developable ruling                */
#define  SURCUR      14        /* Surface/curve intersection        */
#define  SURSUR      15        /* Surface/surface intersection      */
#define  CURPLAN     20        /* Curve/plane intersection          */
/******************************************************************!*/

/*!********** Methods of computation ********************************/
#define  NEWTON       1        /* Newton-Rhapson                    */
#define  PARAB2       2        /* Two   pt parabola interpolation   */
#define  PARAB3       3        /* Three pt parabola interpolation   */
/******************************************************************!*/

/*!*********************** SILHOU *********************************!*/
/*!                                                                 */
/*  Silhouette computation data                                     */
/*  ---------------------------                                     */
/*                                                                  */
/*                                                                 !*/
typedef struct
{
gmint ctype;                      /* Type of calculation:           */
                                  /* F_SILH, I_SILH or S_SILH       */
gmint method;                     /* Method to be used              */
gmint nstart;                     /* Number of restarts             */
gmint maxiter;                    /* Maximum number of restarts     */
gmflt ctol;                       /* Coordinate     tolerance       */
gmflt ntol;                       /* Surface normal tolerance       */
gmflt idpoint;                    /* Identical point criterion      */
gmflt idangle;                    /* Identical angle criterion      */
gmflt comptol;                    /* Computer accuracy tolerance    */
gmflt d_che;                      /* Maximum check R*3 distance     */
gmflt  eye_x, eye_y, eye_z;       /* Eye point  for ctype F_SILH    */
gmflt eyev_x,eyev_y,eyev_z;       /* Eye vector for ctype I_SILH or */
                                  /* 2D vector  for ctype S_SILH    */
} SILHOU; 
/*!*********************** SILHOU *********************************!*/

/*!*********************** ISOPHO *********************************!*/
/*!                                                                 */
/*  Isophote   computation data                                     */
/*  ---------------------------                                     */
/*                                                                  */
/*                                                                 !*/
typedef struct
{
gmint ctype;                      /* Type of calculation            */
gmint method;                     /* Method to be used              */
gmint nstart;                     /* Number of restarts             */
gmint maxiter;                    /* Maximum number of restarts     */
gmflt ctol;                       /* Coordinate     tolerance       */
gmflt ntol;                       /* Surface normal tolerance       */
gmflt idpoint;                    /* Identical point criterion      */
gmflt idangle;                    /* Identical angle criterion      */
gmflt comptol;                    /* Computer accuracy tolerance    */
gmflt d_che;                      /* Maximum check R*3 distance     */
gmflt  eye_x, eye_y, eye_z;       /* Eye point                      */
gmflt eyev_x,eyev_y,eyev_z;       /* Eye vector                     */
gmflt angle;                      /* Angle with surface normal      */
} ISOPHO; 
/*!*********************** ISOPHO *********************************!*/

/*!*********************** IPLANE *********************************!*/
/*!                                                                 */
/*  Intersection plane computation data                             */
/*  -----------------------------------                             */
/*                                                                  */
/*                                                                 !*/
typedef struct
{
gmint ctype;                      /* Type of calculation            */
gmint method;                     /* Method to be used              */
gmint nstart;                     /* Number of restarts             */
gmint maxiter;                    /* Maximum number of restarts     */
gmflt ctol;                       /* Coordinate     tolerance       */
gmflt ntol;                       /* Surface normal tolerance       */
gmflt idpoint;                    /* Identical point criterion      */
gmflt idangle;                    /* Identical angle criterion      */
gmflt comptol;                    /* Computer accuracy tolerance    */
gmflt d_che;                      /* Maximum check R*3 distance     */
gmflt in_x,in_y,in_z,in_d;        /* Plane data: n*r=d              */
} IPLANE; 
/*!*********************** IPLANE *********************************!*/

/*!*********************** ISURSUR ********************************!*/
/*!                                                                 */
/*  Surface/surface intersection data                               */
/*  ---------------------------------                               */
/*                                                                  */
/*                                                                 !*/
typedef struct
{
gmint ctype;                      /* Type of calculation            */
gmint method;                     /* Method to be used              */
gmint nstart;                     /* Number of restarts             */
gmint maxiter;                    /* Maximum number of restarts     */

gmint s_method;                   /* Select/search method           */
                                  /* Eq. 1: BBOX'es only            */

gmflt ctol;                       /* Coordinate     tolerance       */
gmflt ntol;                       /* Surface normal tolerance       */
gmflt idpoint;                    /* Identical point criterion      */
gmflt idangle;                    /* Identical angle criterion      */
gmflt comptol;                    /* Computer accuracy tolerance    */
gmflt factol;                     /* Facets (B-planes) tolerance    */
gmflt d_che;                      /* Maximum check R*3 distance     */

                                  /* Density of the UV net:         */
gmint u1_add;                     /* For surface 1 in U direction   */
gmint v1_add;                     /* For surface 1 in V direction   */
gmint u2_add;                     /* For surface 2 in U direction   */
gmint v2_add;                     /* For surface 2 in V direction   */
                                  /* (Values 1, 2, 3, 4, .... )     */

                                  /* Patch limit pts (search area): */
gmflt us1;                        /* U start surface 1              */
gmflt vs1;                        /* V start surface 1              */
gmflt ue1;                        /* U end   surface 1              */
gmflt ve1;                        /* V end   surface 1              */
gmflt us2;                        /* U start surface 2              */
gmflt vs2;                        /* V start surface 2              */
gmflt ue2;                        /* U end   surface 2              */
gmflt ve2;                        /* V end   surface 2              */

                                  /* Segment limits  (search area): */
gmflt usc;                        /* U start for curve              */
gmflt uec;                        /* U end   for curve              */

DBVector startuvpt1;                 /* Start U,V point surface 1      */
DBVector startuvpt2;                 /* Start U,V point surface 2      */
} ISURSUR; 

/*!*********************** ISURSUR ********************************!*/

/*!*********************** ISURCUR ********************************!*/
/*!                                                                 */
/*  Surface/curve intersection data                                 */
/*  -------------------------------                                 */
/*                                                                  */
/*                                                                 !*/
typedef struct
{
gmint ctype;                      /* Type of calculation            */
gmint method;                     /* Method to be used              */
gmint nstart;                     /* Number of restarts             */
gmint maxiter;                    /* Maximum number of restarts     */

gmint s_method;                   /* Select/search method           */
                                  /* Eq. 1: BBOX'es only            */

gmflt ctol;                       /* Coordinate     tolerance       */
gmflt ntol;                       /* Surface normal tolerance       */
gmflt idpoint;                    /* Identical point criterion      */
gmflt idangle;                    /* Identical angle criterion      */
gmflt comptol;                    /* Computer accuracy tolerance    */
gmflt factol;                     /* Facets (B-planes) tolerance    */
gmflt d_che;                      /* Maximum check R*3 distance     */

                                  /* Density of the UV net:         */
gmint u_add;                      /* For surface 1 in U direction   */
gmint v_add;                      /* For surface 1 in V direction   */
                                  /* (Values 1, 2, 3, 4, .... )     */

                                  /* Patch limit pts (search area): */
gmflt us;                         /* U start surface                */
gmflt vs;                         /* V start surface                */
gmflt ue;                         /* U end   surface                */
gmflt ve;                         /* V end   surface                */

                                  /* Segment limits  (search area): */
gmflt usc;                        /* U start for curve              */
gmflt uec;                        /* U end   for curve              */

DBVector startuvpt;                  /* Start U,V point surface        */
DBVector startupt;                   /* Start U   point curve          */
} ISURCUR; 
/*!*********************** ISURSUR ********************************!*/

/*!*********************** PROJECT ********************************!*/
/*!                                                                 */
/*  Projected (closest) curve or point                              */
/*  ----------------------------------                              */
/*                                                                  */
/*                                                                 !*/
typedef struct
{
gmint ctype;                      /* Type of calculation            */
gmint method;                     /* Method to be used              */
gmint nstart;                     /* Number of restarts             */
gmint maxiter;                    /* Maximum number of restarts     */
gmflt ctol;                       /* Coordinate     tolerance       */
gmflt ntol;                       /* Surface normal tolerance       */
gmint ocase;                      /* Ordering case for solution pts */
                                  /* Eq. 1: According to R*3 dist.  */
                                  /* Eq. 2: According to UV  dist.  */
gmint acc;                        /* Computation accuracy (case)    */
                                  /* Eq. 1: All patches connected   */
                                  /*        to start UV pt s_uvpt   */
                                  /* Eq. 2: Next layer of patches.. */
gmint scase;             /* Solution selection case                 */
                         /* Eq. 1: All intersect points             */
                         /* Eq. 2: Intersects in shooting direction */
                         /* Eq. 3: As 2 but only intersect pts with */
                         /*        surface normals in the shooting  */
                         /*        direction will be output         */
                         /* Eq. 4: As 2 but only intersect pts with */
                         /*        surface normals in the opposite  */
                         /*        shooting direction will be output*/

short (*objfunc)();               /* Pointer to the function which  */
                                  /* for the given interrogation    */ 
                                  /* case (ctype) calculates:       */ 
                                  /* - Objective function value     */ 
                                  /* - Gradient vector and length   */ 
                                  /* - Hessian matrix               */ 
                                  /* (Not yet implemented !!!!!!)   */ 

DBTmat  pat_tra;                  /* Transformat. matrix for patch  */
} PROJECT;
/*!*********************** PROJECT ********************************!*/

/*!*********************** APPROX *********************************!*/
/*!                                                                 */
/*  Approximation computation data                                  */
/*  ------------------------------                                  */
/*                                                                  */
/*                                                                 !*/
typedef struct
{
gmint ctype;             /* Type of calculation                     */
gmint method;            /* Method to be used                       */
gmint nstart;            /* Number of restarts                      */
gmint maxiter;           /* Maximum number of restarts              */
gmint acase;             /* Approximation case:                     */
                         /* Eq. 1: Approximate to CUB_PAT patches   */
                         /* Eq. 2: Approximate to RAT_PAT patches   */
                         /* Eq. 3: Approximate to LFT_PAT patches   */
                         /* Eq. 4: Approximate to FAC_PAT patches   */
                         /* Add 100 if zero patches not is allowed  */
                         /* in the output surface.                  */
gmint acc;               /* Computation accuracy                    */
                         /* Eq. 1: Tolerance band +/- surface       */
                         /* Eq. 2: Tolerance band  +  surface only  */
                         /* Eq. 3: Tolerance band  -  surface only  */
gmflt offset;            /* Surface offset                          */
gmflt thick;             /* Surface thickness                       */

                         /* Approximation tolerances:               */
                         /* ( Tolerance < 0 <==> not defined )      */
gmflt ctol;              /* Surface coordinate  tolerance           */
gmflt ntol;              /* Surface normal      tolerance           */
                         /* (tolerance is in degrees)               */
gmflt rtol;              /* Radius of curvature tolerance           */
} APPROX; 
/*!*********************** APPROX *********************************!*/

/*!*********************** GEODES *********************************!*/
/*!                                                                 */
/*  Geodesic computation data                                       */
/*  -------------------------                                       */
/*                                                                  */
/*                                                                 !*/
typedef struct
{
gmint ctype;                      /* Type of calculation:           */
                                  /* D_GEODES, ..                   */
gmint method;                     /* Method to be used              */
gmint nstart;                     /* Number of restarts             */
gmint maxiter;                    /* Maximum number of restarts     */
gmflt ctol;                       /* Coordinate zero end criterion  */
gmflt gtol;                       /* Geodesic   zero end criterion  */
gmflt uv_x,uv_y;                  /* Current U,V point              */
gmflt tuv_x,tuv_y;                /* Current U,V tangent            */
} GEODES; 
/*!*********************** GEODES *********************************!*/

/*!*********************** APPCUR  ********************************!*/
/*!                                                                 */
/*  Curve approximation computation data                            */
/*  ------------------------------------                            */
/*                                                                  */
/*                                                                 !*/
typedef struct
{
gmint ctype;             /* Type of calculation                     */
                         /* Eq. 1: No of new segments per segment   */
                         /* Eq. 2: Total number of segments         */
                         /* Eq. 3: Add only segments                */
                         /* Eq. 4: Reduce number of segments        */
gmint method;            /* Method to be used                       */
                         /* Eq. 1: Least square                     */
                         /* Eq. 2: ...                              */
gmint nstart;            /* Number of restarts                      */
gmint maxiter;           /* Maximum number of iterations            */
gmint acase;             /* Approximation case:                     */
                         /* Eq. 1: Approximate to LINE   segments   */
                         /* Eq. 2: Approximate to CUBIC  segments   */
                         /* Eq. 3: Approximate to CIRCLE segments   */
                         /* Eq. 4: Approximate to CONIC  segments   */
                         /* Eq. 5: Approximate to RATCUB segments   */

gmint t_incur;           /* Type of input curve (what to use)       */
                         /* Eq. 1: Points (segment end points)      */
                         /* Eq. 2: Lines                            */
                         /* Eq. 3: Points and tangents              */
                         /* Eq. 4: Points, tangents and curvature   */

gmint n_req;             /* Requested no of segments for ctype= 1,2 */

gmint n_alloc;           /* Size of allocated area for output segm. */

                         /* Limits and criteria for approximation:  */
gmflt clmax;             /* Maximum chord length                    */
gmflt lcrit;             /* Curvature radius for straight line      */

gmflt d_che;             /* Arclength check distance                */

                         /* Approximation tolerances:               */
                         /* ( Tolerance < 0 <==> not defined )      */
gmint t_type;            /* Tolerance type                          */
                         /* Eq. 1: Tolerance band +/- curve         */
                         /* Eq. 2: Tolerance band  +  curve   only  */
                         /* Eq. 3: Tolerance band  -  curve   only  */
gmflt ctol;              /* Curve   coordinate  tolerance           */
gmflt ntol;              /* Curve   tangent     tolerance           */
                         /* (tolerance is in degrees)               */
gmflt rtol;              /* Radius of curvature tolerance           */

gmflt comptol;           /* Computer accuracy tolerance             */

gmint i_start;           /* Current start segm. no for  input crv   */
gmflt u_start;           /* Current parameter value in i_start      */
gmint i_end;             /* Current end   segm. no for  input crv   */
gmflt u_end;             /* Current parameter value in i_end        */
gmint i_outcur;          /* Current segment number for output crv   */
gmflt u_outcur;          /* Current parameter value in i_outcur     */
gmint i_delta;           /* No of segments approx by previous segm. */

EVALC p_in;              /* Current point from input  curve         */
EVALC p_out;             /* Current point from output curve         */
} APPCUR;
/*!*********************** APPCUR  ********************************!*/

/*!*********************** DEVRUL *********************************!*/
/*!                                                                 */
/*  Developable ruling computation data                             */
/*  -----------------------------------                             */
/*                                                                  */
/*                                                                 !*/
typedef struct
{
gmint ctype;                      /* Type of calculation            */
gmint method;                     /* Method to be used              */
gmint nstart;                     /* Number of restarts             */
gmint maxiter;                    /* Maximum number of restarts     */
gmflt ctol;                       /* Coordinate     tolerance       */
gmflt ntol;                       /* Surface normal tolerance       */
gmflt idpoint;                    /* Identical point criterion      */
gmflt idangle;                    /* Identical angle criterion      */
gmflt comptol;                    /* Computer accuracy tolerance    */
gmflt d_dist;                     /* Maximum delta distance         */
gmflt d_ang;                      /* Maximum delta angle            */
} DEVRUL; 

/*!*********************** DEVRUL *********************************!*/

/*!******* Union of interrogation calculation data ** IRUNON ******!*/
typedef union 
{
SILHOU  sil_un;
ISOPHO  iso_un;
IPLANE  ipl_un;
PROJECT pro_un;
APPROX  app_un;
GEODES  geo_un;
ISURSUR isu_un;
ISURCUR icu_un;
DEVRUL  dev_un;
}  IRUNON;
/*!********************* IRUNON ***********************************!*/

/* *********************** ISURF ********************************** */
/*                                                                  */
/* Interrogation surface                                            */
/* ----------------------                                           */
/*                                                                  */
/* Ej anv{nd eftersom jag ej fick ordning p} pekarna. Det blev      */
/* ist{llet en array med pekare som indata !!!  Gunnar              */
/*                                                                  */
/* Multiple surfaces are often part of a surface interrogation      */
/* calculation.                                                     */
/*                                                                  */
/* This structure variable is used for the definition of a set      */
/* of interrogation surfaces. The set of surfaces is referred       */
/* to as the ISURF table in the program documentation.              */
/*                                                                  */
/* Maximum number of records in the ISURF table is ISMAX.           */
/*                                                                  */
typedef struct
{
GMSUR  *psur;                     /* Ptr to interrogation surface   */
GMPAT  *pppat;                    /* Ptr to start patch of psur     */
} ISURF;     
/* *********************** ISURF ********************************** */

/*!*********************** PBOUND *********************************!*/
/*!                                                                 */
/* Patch boundary definition                                        */
/* -------------------------                                        */
/*                                                                  */
/* The true patch boundaries in a bicubic surface are defined by:   */
/* ubound= 1.0, 2.0, ... , nu and vbound= 1.0, 2.0, 3.0, ... , nv   */
/* This structure variable is used to define a table, which will    */
/* define the "accuracy of the enter/exit point search" phase       */
/* for all (curve generation) surface interrogation routines.       */
/*                                                                  */
/* Extra parameter values can be added in the table for instance:   */
/* ubound= 1.3, 1.75, ....... and vbound= 2.4, 3,1, 3.2, ........   */
/* All boundaries in the PBOUND table will be treated as real       */
/* boundaries. This implies for instance that there always will     */
/* be an output U,V curve segment start/end point for the given     */
/* boundary values.                                                 */
/*                                                                  */
/* The PBOUND table must be "well" defined for a successful         */
/* calculation. All curve generation surface interrogation          */
/* are based on one (1) entry    and one (1) exit point in          */
/* these fictitious patches.                                        */
/*                                                                  */
/* The PBOUND table can also be used to limit the surface. For      */
/* ubound= 2.0, 3.0, .., nu-1 and vbound= 3.0, 4.0, ... nv-2        */
/* will only U=2.0 to U=nu-1 and V=3 to V=nv-2 of the surface       */
/* be part of the calculation.                                      */
/*                                                                  */
/* The elements in the PBOUND table are the "adresses" to the       */
/* patches and there is one element for each fictitious patch.      */
/*                                                                  */
/* The total number of records in the table will be:                */
/* totnum = (nu_f-1)*(nv_f-1)  where                                */
/* nu_f= number of fictitious U values (nu= no of true patch bound.)*/
/* nv_f= number of fictitious V values (nv= no of true patch bound.)*/
/* Maximum number of records is defined by PBMAX. A value which     */
/* which some functions use. Normally though, will memory be        */
/* allocated .......                                                */
/*                                                                  */
/* All record data will be generated at the start of a calculation  */
/* except the number of solutions (nu0, nu1, nv0, nv1) for each     */
/* boundary. This information will be added during calculation and  */
/* it will be used to determine if there exist any solutions along  */
/* a common patch border. This information can for instance be used */
/* for a planar intersect calculation. It is not necessary to make  */
/* the curve plane intersection if there are no solutions in the    */
/* common curve segment. If there is an intersect point then can    */
/* the parameter value be used as a start point. It is necessary    */
/* to recompute since the parameterisation can be different. For    */
/* a conic lofting type surface can however the point be used       */
/* witout any recomputation.                                        */
/*                                                                  */
/*                                   nv1=0                          */
/*                     ----------=============                      */
/*                    !          !            !                     */
/*  Previous patch -> !   nu1=1  x nu0=1      ! <- Current patch    */
/*                    !          !            !                     */
/*                     ----------!============!                     */
/*                                   nv0=0                          */
/*                                                                 !*/
/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!                        PBOUND                    (continued)   !*/
/*!                                                                !*/
typedef struct
{
gmint  rec_no;                    /* Record number                  */
short  ptype;                     /* Patch type CUB_PAT, NUL_PAT... */
gmflt  us,vs;                     /* U and V for patch "start pt"   */
gmflt  ue,ve;                     /* U and V for patch "end   pt"   */
short  nu0;                       /* Number of solutions along u0   */
short  nu1;                       /* Number of solutions along u1   */
short  nv0;                       /* Number of solutions along v0   */
short  nv1;                       /* Number of solutions along v1   */
gmint  sur_no;                    /* Data from surface number 1 or 2*/
                                  /* (For surface/surface interr.)  */
BBOX   pbox;                      /* Bounding box  for patch        */
BCONE  pcone;                     /* Bounding cone for patch        */
} PBOUND; 
/*!*********************** PBOUND *********************************!*/

/*!*********************** EPOINT *********************************!*/
/*!                                                                 */
/* Patch boundary definition                                        */
/* -------------------------                                        */
/*                                                                  */
/* Entry    and exit points in a patch (defined by PBOUND) will     */
/* be stored in an EPOINT table.                                    */
/*                                                                  */
/* Maximum number of records is defined by EPMAX.                   */
/*                                                                  */
/*                                                                 !*/
typedef struct
{
gmint rec_no;                     /* Record number                  */
gmflt u1,v1;                      /* Exit/entry    UV point 1       */
gmflt u1_t,v1_t;                  /* Tangent for point u1,v1        */
gmflt u2,v2;                      /* Exit/entry    UV point 2       */
gmflt u2_t,v2_t;                  /* Tangent for point u2,v2        */
gmflt us,vs,ue,ve;                /* Patch limit points             */
gmint ptr1,ptr2;                  /* Pointers between points        */

gmint branch;                     /* Curve branch                   */


short b_flag;                     /* Boundary point flag            */
                                  /* Eq. 1: Two "normal" points     */
                                  /* Eq. 2: Curve coincides iso-p.  */
                                  /* Eq.-1: Points "used" in curve  */
                                  /* Eq.-9: Do not use record !     */
                                  /*        There are identical pts */
                                  /*        in another record !     */
short d_flag;                     /* Derivative point flag          */
                                  /* Eq. 0: Derivatives not  calc.  */
                                  /* Eq. 1: u1_t,v1_t only   calc.  */
                                  /* Eq. 2: u2_t,v2_t only   calc.  */
                                  /* Eq. 3: Both derivatives calc.  */

short no_ep;                      /* Number of exit/entry points    */
                                  /* (for the normal case two (2))  */
EVALC curpt[9];                   /* All entry/exit curve points    */
                                  /* ( Also R*3 coordinates and     */
                                  /*   tangents                     */
} EPOINT;
/*!*********************** EPOINT *********************************!*/

/*!*********************** SPOINT  ********************************!*/
/*!                                                                 */
/* Point on a surface                                               */
/* ------------------                                               */
/*                                                                  */
/* The record contains input and output data for the calculation    */
/* of a point on a surface.                                         */
/*                                                                  */
/* Input point is often a point from a surface and data from        */
/* from this surface is also saved since one very common use        */
/* of the SPOINT table is to compare two surfaces. The deviation    */
/* in distance and deviation in surface normal for a grid of        */
/* surface points.                                                  */
/*                                                                  */
/*                                                                  */
/* Maximum number of records is defined by SPMAX.                   */
/* (  .. men denna tabell borde allokeras dynamiskt ... !!!  )      */
/*                                                                  */
/*                                                                 !*/
typedef struct
{
/*!                                                                 */
DBVector extpt;                      /* External point                 */
DBVector extptnorm;                  /* External point normal          */
DBVector extuvpt;                    /* External point U,V value       */
GMSUR *pisur;                     /* Ptr to interrogation surface   */
GMPAT *pppat;                     /* Ptr to alloc. pisur patch area */
DBVector startuvpt;                  /* Start U,V point                */
gmflt us,vs,ue,ve;                /* Patch limit points             */
DBVector extseg;                     /* External curve segment.        */
gmflt extseg_t;                   /* External curve segment param.  */
DBVector proj;                       /* Projection vector              */
                                  /* Eq. (0,0,0) <=> closest crv/pt */

EVALS spt;                        /* Surface point with derivatives */

short pflag;                      /* Point flag                     */
                                  /* Eq. 0: No calculation is made  */
                                  /* Eq. 1: Only one solution       */
                                  /* Eq. 2: Multiple solutions      */
                                  /* Eq.-1: There was no solution.  */
gmint ptrs;                       /* Ptr to next record for pflag=2 */
} SPOINT ;
/*!*********************** SPOINT  ********************************!*/

/*!*********************** APLANE  ********************************!*/
/*!                                                                 */
/* Approximative plane                                              */
/* -------------------                                              */
/*                                                                  */
/* Approximative planes for two intersecting surfaces are           */
/* calculated. The intersects between the planes are calculated     */
/* and the intersect points are saved ...                           */
/*                                                                  */
/* Maximum number of records is defined by PLMAX.                   */
/* (  .. men denna tabell borde allokeras dynamiskt ... !!!  )      */
/*                                                                  */
/*                                                                 !*/
typedef struct
{
GMBPL bpl;                        /* Triangular B-plane             */
gmflt u1bpl;                      /* U value for B-plane point 1    */
gmflt v1bpl;                      /* V value for B-plane point 1    */
gmflt u2bpl;                      /* U value for B-plane point 2    */
gmflt v2bpl;                      /* V value for B-plane point 2    */
gmflt u4bpl;                      /* U value for B-plane point 4    */
gmflt v4bpl;                      /* V value for B-plane point 4    */

gmint rec_no;                     /* Record number in PBOUND        */
gmflt us,vs,ue,ve;                /* Patch (PBOUND) limit values    */
gmint sur_no;                     /* Data from surface number 1 or 2*/
                                  /* (For surface/surface interr.)  */
} APLANE ;
/*!*********************** APLANE  ********************************!*/

/*!*********************** APOINT  ********************************!*/
/*!                                                                 */
/* Intersect APLANE points                                          */
/* -----------------------                                          */
/*                                                                  */
/* This table contains the result of an intersection calculation    */
/* between two APLANE surfaces.                                     */
/*                                                                  */
/* Maximum number of records is defined by APMAX.                   */
/* (  .. men denna tabell borde allokeras dynamiskt ... !!!  )      */
/*                                                                  */
/*                                                                 !*/
typedef struct
{
                                  /* Intersect points R*3:          */
DBVector p1;                         /* Intersect point 1              */
DBVector p2;                         /* Intersect point 2              */

                                  /* Interpolated UV values:        */
gmflt u1sur1;                     /* U value for p1 in surface 1    */
gmflt v1sur1;                     /* V value for p1 in surface 1    */
gmflt u2sur1;                     /* U value for p2 in surface 1    */
gmflt v2sur1;                     /* V value for p2 in surface 1    */
gmflt u1sur2;                     /* U value for p1 in surface 2    */
gmflt v1sur2;                     /* V value for p1 in surface 2    */
gmflt u2sur2;                     /* U value for p2 in surface 2    */
gmflt v2sur2;                     /* V value for p2 in surface 2    */
                                  /* U and V < 0 <==> No intersect  */

gmint rec_no_sur1;                /* PBOUND record number surface 1 */
gmint rec_no_sur2;                /* PBOUND record number surface 2 */
gmflt us_sur1,vs_sur1;            /* Patch (PBOUND) limit values    */
gmflt ue_sur1,ve_sur1;            /* for surface 1                  */
gmflt us_sur2,vs_sur2;            /* Patch (PBOUND) limit values    */
gmflt ue_sur2,ve_sur2;            /* for surface 2                  */

gmint branch;                     /* Curve branch                   */
gmint ip1;                        /* Ordered p1 point number        */
gmint ip2;                        /* Ordered p2 point number        */
} APOINT ;
/*!*********************** APOINT  ********************************!*/

/*!*********************** UVPT   *********************************!*/
/*!                                                                 */
/*  Patch UV point and tangent for segment of solution branch.      */
/*  ----------------------------------------------------------      */
/*                                                                  */
/*                                                                 !*/
typedef struct   
{
gmint f;                          /* Flag for the UV point          */
                                  /* <   0: Not defined             */
                                  /* Eq. 1: Defined                 */
                                  /* Eq. 2: Used as start point     */
                                  /* Eq. 3: Used as end   point     */
gmflt l_interv;                   /* Curve "parameter"              */
gmflt u;                          /* U value                        */
gmflt v;                          /* V value                        */
gmflt tu;                         /* Tangent U                      */
gmflt tv;                         /* Tangent V                      */
} UVPT;   
/*!*********************** UVPT ***********************************!*/

/*!*********************** SURWARN *********************************!*/
/*!                                                                 */
/*  Surface quality warning/error                                   */
/*  -----------------------------                                   */
/*                                                                  */
/*  Texts for warnings/errors are defined in file SU.ERM            */
/*                                                                  */
/*                                                                  */
/*                                                                 !*/
typedef struct
{
gmint warning_n1;                 /* Error/warning number       (1) */
char  warning_c1[80];             /* Warning                    (1) */
gmint warning_n2;                 /* Error/warning number       (2) */
char  warning_c2[80];             /* Warning                    (2) */
gmflt u_mbs;                      /* MBS U parameter value          */
gmflt v_mbs;                      /* MBS V parameter value          */
gmint i_u;                        /* Patch address in U direction   */
gmflt i_v;                        /* Patch address in V direction   */
} SURWARN;   
/*!*********************** SURWARN ********************************!*/

/*!********** Definition of constants *******************************/
/*!********** Dimensions of interrogation tables (arrays) ***********/
                               /* Maximum number of:                */
#define  PBMAX      6400       /* Records in table PBOUND           */
#define  PBSMA       500       /* Records in table PBOUND Small     */
#define  PLMAX      2000       /* Records in table APLANE           */
#define  BPLMAX     2000       /* Elements in B-plane array         */
#define  APMAX      1000       /* Records in table APOINT           */
#define  EPMAX       800       /* Records in table EPOINT           */
#define  EPSMA       100       /* Records in table EPOINT Small     */
#define  SPMAX       800       /* Records in table SPOINT           */
#define  ISMAX         5       /* Records in table ISURF            */
#define  SMAX         10       /* Solution points                   */
#define  CBMAX        20       /* Curve branches                    */
#define  INTMAX       50       /* Intersection points               */
#define  DMAX         36       /* Times that a patch can be divided */
#define  STRIP        50       /* Strips in a lofting surface       */
#define  SPINE       100       /* Segments in a spine               */
#define  SUANA        50       /* Position, derivative, .. analysis */
/* 1997-01-29#define UVPTMAX 60 *//* Segment/check pts for one patch*/
#define  UVPTMAX     200       /* Segment/check pts for one patch   */
#define  SFMAX        20       /* Input surfaces to SUR_AFACE       */
#define  NUMAX       500       /* U patches in a surface            */
#define  NVMAX       500       /* V patches in a surface            */
#define  SWMAX       100       /* Maximum number of SURWARN         */
/******************************************************************!*/

