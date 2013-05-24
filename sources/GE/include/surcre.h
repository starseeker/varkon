/**********************************************************************
*
*    surcre.h
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
*    (C)Microform AB 1984-1999, Gunnar Liden, gunnar@microform.se
*
***********************************************************************/


/*!********** Cases of surface creation *****************************/
                               /* Define a surface from:            */
#define  CRE_PTS      1        /* Points only                       */
#define  CRE_UD       2        /* Points + U tangents               */
#define  CRE_VD       3        /* Points + V tangents               */
#define  CRE_UVD      4        /* Points + U + V tangents           */
#define  CRE_UVWD     5        /* Points + U + V tangents + twist   */
/******************************************************************!*/

/*!*********************** SURPOI *********************************!*/
/*!                                                                 */
/*  Points and derivatives for surface creation                     */
/*  -------------------------------------------                     */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*      ! V                                                         */
/*      !           !                                               */
/*      !   Patch 2 ! Patch 4                                       */
/*      !   --------!---------                                      */
/*      !   Patch 1 ! Patch 3                                       */
/*      !           !                                               */
/*      !                                                           */
/*      !----------------------> U                                  */
/*                                                                 !*/
typedef struct
{
gmint iu,iv;                      /* Point (record) number          */
EVALS spt;                        /* Surface point & derivatives    */
gmflt u_l1 ,u_l2 ,u_l3 ,u_l4;     /* Lengths of U tangents          */
gmflt v_l1 ,v_l2 ,v_l3 ,v_l4;     /* Lengths of V tangents          */
gmflt uv_l1,uv_l2,uv_l3,uv_l4;    /* Lengths of twist vectors       */
gmflt u2_l1,u2_l2,u2_l3,u2_l4;    /* Lengths of d2r/du2 vectors     */
gmflt v2_l1,v2_l2,v2_l3,v2_l4;    /* Lengths of d2r/dv2 vectors     */
gmflt u_ratio;                    /* Vector ratio in U direction    */
gmflt v_ratio;                    /* Vector ratio in V direction    */
} SURPOI; 
/*!*********************** SURPOI *********************************!*/

/*!*********************** SURCOMP ********************************!*/
/*!                                                                 */
/*  Composite surface calculation data                              */
/*  -----------------------------------                             */
/*                                                                  */
/*                                                                 !*/
typedef struct surcomp
{
/*!                                                                 */
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
gmint che_add;                    /* No. of added points for check  */

gmint nu1;                        /* Number of U patches surface 1  */
gmint nv1;                        /* Number of U patches surface 1  */
gmint nu2;                        /* Number of V patches surface 2  */
gmint nv2;                        /* Number of V patches surface 2  */
gmint surtype1;                   /* Surface type        surface 1  */
gmint surtype2;                   /* Surface type        surface 2  */

gmint equal_case;                 /* Equal     boundary case        */
gmint comp_case;                  /* Composite boundary case        */

gmflt s_p_sur1;                   /* Start parameter surface 1      */
gmflt e_p_sur1;                   /* End   parameter surface 1      */
gmflt s_p_sur2;                   /* Start parameter surface 2      */
gmflt e_p_sur2;                   /* End   parameter surface 2      */

} SURCOMP; 

/*!*********************** SURCOMP ********************************!*/

/*!********** Cases for composite surfaces **************************/
                               /* Combine surfaces with boundaries: */
#define  S1US_S2UE    1        /* Surf. 1 start U Surf. 2 end   U   */
#define  S1UE_S2US    2        /* Surf. 1 end   U Surf. 2 start U   */
#define  S1VS_S2VE    3        /* Surf. 1 start V Surf. 2 end   V   */
#define  S1VE_S2VS    4        /* Surf. 1 end   V Surf. 2 start V   */
#define  S1VF_S2VF   99        /* Analysis failure                  */
                               /* Composite boundary case:          */
#define  BDY_IDENT  101        /* Boundaries are identical          */
#define  BDY_NULLP  102        /* Null patches must be added        */
#define  BDY_TRIMP  103        /* Patches must be trimmed           */
#define  BDY_AFAIL  199        /* Analysis failure                  */
/******************************************************************!*/
