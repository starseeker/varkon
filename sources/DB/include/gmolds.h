/**********************************************************************
*
*    gmolds.h 
*    ========
*
*    This file is part of the VARKON Database Library.
*    URL:  http://www.varkon.com
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
***********************************************************************/

/*
***This file defines all earlier versions of GM structures
***for backward compatibility. Never change any of these.
*/

/*
***Curve segments.
*/
typedef struct
{
gmflt c0x,c0y,c0z,c0;
gmflt c1x,c1y,c1z,c1;
gmflt c2x,c2y,c2z,c2;
gmflt c3x,c3y,c3z,c3;
DBptr nxt_oc;
} GMSEG0;

typedef struct
{
gmflt c0x,c0y,c0z,c0;
gmflt c1x,c1y,c1z,c1;
gmflt c2x,c2y,c2z,c2;
gmflt c3x,c3y,c3z,c3;
DBptr nxt_oc;
gmflt ofs_oc;
gmflt sl_oc;
} GMSEG1;

/*
***Points.
*/
typedef struct
{
GMRECH hed_p;
DBVector  crd_p;
} GMPOI0;

typedef struct
{
GMRECH hed_p;
DBVector  crd_p;
DBptr  pcsy_p;
} GMPOI1;

typedef struct
{
DBHeader   hed_p;
DBVector   crd_p;
DBptr      pcsy_p;
DBfloat    wdt_p;
} GMPOI2;
/*
***Lines.
*/
typedef struct
{
GMRECH  hed_l;
short   fnt_l;
gmflt   lgt_l;
DBVector   crd1_l;
DBVector   crd2_l;
} GMLIN0;

typedef struct
{
GMRECH  hed_l;
short   fnt_l;
gmflt   lgt_l;
DBVector   crd1_l;
DBVector   crd2_l;
DBptr   pcsy_l;
} GMLIN1;

/*
***Arcs/Circles.
*/
typedef struct
{
GMRECH  hed_a;
short   fnt_a;
gmflt   lgt_a;
gmflt   x_a;
gmflt   y_a;
gmflt   r_a;
gmflt   v1_a;
gmflt   v2_a;
gmflt   o_a;
short   ns_a;
DBptr   sptr_a;
} GMARC0;

typedef struct
{
GMRECH  hed_a;
short   fnt_a;
gmflt   lgt_a;
gmflt   x_a;
gmflt   y_a;
gmflt   r_a;
gmflt   v1_a;
gmflt   v2_a;
gmflt   o_a;
short   ns_a;
DBptr   sptr_a;
DBptr   pcsy_a;
} GMARC1;

/*
***Curves.
*/
typedef struct
{
GMRECH  hed_oc;
gmflt   o_oc;
short   ns_oc;
DBptr   sptr_oc;
} GMCUR0;

typedef struct
{
GMRECH  hed_oc;
short   fnt_oc;
gmflt   lgt_oc;
gmflt   al_oc;
short   ns_oc;
DBptr   sptr_oc;
DBptr   cptr_oc;
DBptr   pcsy_oc;
} GMCUR1;

typedef struct
{
GMRECH  hed_oc;
short   fnt_oc;
gmflt   lgt_oc;
gmflt   al_oc;
short   nges_oc;
DBptr   pges_oc;
DBptr   cptr_oc;
DBptr   pcsy_oc;
short   ngrs_oc;
DBptr   pgrs_oc;
} GMCUR2;

/*
***Surfaces.
*/
typedef struct
{
GMRECH  hed_su;
short   typ_su;
short   fnt_su;
gmflt   lgt_su;
short   nu_su;
short   nv_su;
short   ngu_su;
short   ngv_su;
DBptr   ptab_su;
DBptr   pcsy_su;
short   ngseg_su[6];
DBptr   pgseg_su[6];
} GMSUR0;

typedef struct
{
DBHeader hed_su;
short    typ_su;
short    fnt_su;
gmflt    lgt_su;
short    nu_su;
short    nv_su;
short    ngu_su;
short    ngv_su;
DBptr    ptab_su;
DBptr    pcsy_su;
short    ngseg_su[6];
DBptr    pgseg_su[6];
int      uorder_su;
int      vorder_su;
int      nku_su;
int      nkv_su;
DBptr    pkvu_su;
DBptr    pkvv_su;
DBptr    pcpts_su;
BBOX     box_su;
} GMSUR1;

/*
***Coordinatesystems.
*/

typedef struct
{
GMRECH  hed_pl;
char    name_pl[10+1];
DBTmat  mat_pl;
} GMCSY0;

/*
***B_planes.
*/
typedef struct
{
GMRECH hed_bp;
DBVector  crd1_bp;
DBVector  crd2_bp;
DBVector  crd3_bp;
DBVector  crd4_bp;
} GMBPL0;

/*
***Text.
*/
typedef struct
{
GMRECH  hed_tx;
DBVector   crd_tx;
short   fnt_tx;
gmflt   h_tx;
gmflt   b_tx;
gmflt   l_tx;
gmflt   v_tx;
short   nt_tx;
DBptr   tptr_tx;
} GMTXT0;

typedef struct
{
GMRECH  hed_tx;
DBVector   crd_tx;
short   fnt_tx;
gmflt   h_tx;
gmflt   b_tx;
gmflt   l_tx;
gmflt   v_tx;
short   nt_tx;
DBptr   tptr_tx;
DBptr   pcsy_tx;
} GMTXT1;

typedef struct
{
GMRECH  hed_tx;
DBVector   crd_tx;
short   fnt_tx;
gmflt   h_tx;
gmflt   b_tx;
gmflt   l_tx;
gmflt   v_tx;
short   nt_tx;
DBptr   tptr_tx;
DBptr   pcsy_tx;
gmflt   wdt_tx;
} GMTXT2;
/*
***Xhatch.
*/
typedef struct
{
GMRECH  hed_xh;
short   fnt_xh;
gmflt   lgt_xh;
gmflt   dist_xh;
gmflt   ang_xh;
short   nlin_xh;
DBptr   lptr_xh;
} GMXHT0;

/*
***Dimensions.
*/
typedef struct
{
GMRECH  hed_ld;
DBVector   p1_ld;
DBVector   p2_ld;
DBVector   p3_ld;
short   dtyp_ld;
gmflt   asiz_ld;
gmflt   tsiz_ld;
short   ndig_ld;
tbool   auto_ld;
} GMLDM0;

typedef struct
{
GMRECH  hed_cd;
DBVector   p1_cd;
DBVector   p2_cd;
DBVector   p3_cd;
short   dtyp_cd;
gmflt   asiz_cd;
gmflt   tsiz_cd;
short   ndig_cd;
tbool   auto_cd;
} GMCDM0;

typedef struct
{
GMRECH  hed_rd;
DBVector   p1_rd;
DBVector   p2_rd;
DBVector   p3_rd;
gmflt   r_rd;
gmflt   asiz_rd;
gmflt   tsiz_rd;
short   ndig_rd;
tbool   auto_rd;
} GMRDM0;

typedef struct
{
GMRECH  hed_ad;
DBVector   pos_ad;
gmflt   r_ad;
gmflt   v1_ad;
gmflt   r1_ad;
gmflt   v2_ad;
gmflt   r2_ad;
gmflt   tv_ad;
gmflt   asiz_ad;
gmflt   tsiz_ad;
short   ndig_ad;
tbool   auto_ad;
} GMADM0;
