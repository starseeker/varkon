/**********************************************************************
*
*    surdef.h
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
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*                               Gunnar Liden, gunnar@microform.se
*
***********************************************************************/

/*!********** Definition of constants *******************************/

#define  MXSCOMP   25          /* Max surfaces in a SUR_COMP        */
#define  MAX_NURBD 21          /* Maximum NURBS polynomial degreee  */
#define  F_UNDEF  -0.123456789 /* Undefined value for a  float      */
#define  I_UNDEF  -12345       /* Undefined value for an integer    */

/*! Basic surface types, DBSurf.typ_su                             !*/

#define MIX_SUR   0            /* Mixed patch type surface          */
#define CUB_SUR   1            /* All patches CUB_PAT               */
#define RAT_SUR   2            /* All patches RAT_PAT               */
#define LFT_SUR   3            /* All patches LFT_PAT               */
#define FAC_SUR   4            /* All patches FAC_PAT               */
#define CON_SUR   5            /* All patches CON_PAT               */
#define POL_SUR   6            /* A mix of P3_PAT,.,P21_PAT patches */
#define  P3_SUR   7            /* All patches of type  P3_PAT       */
#define  P5_SUR   8            /* All patches of type  P5_PAT       */
#define  P7_SUR   9            /* All patches of type  P7_PAT       */
#define  P9_SUR  10            /* All patches of type  P9_PAT       */
#define P21_SUR  11            /* All patches of type P21_PAT       */
#define NURB_SUR 12            /* Patch       of type NURB_PAT      */
#define PRO_SUR  20            /* All patches of type PRO_PAT       */
#define BOX_SUR  88            /* A "bounding boxes surface"        */

/*! Geometrical patch types, DBPatch.styp_pat                      !*/

#define NUL_PAT   0   /* No patch                                   */
#define CUB_PAT   1   /* Geometrical cubic patch, DBPatch_C         */
#define RAT_PAT   2   /* Geometrical rational patch, DBPatchR       */
#define LFT_PAT   3   /* Geometrical conic lofting  patch, DBPatchL */
#define FAC_PAT   4   /* Geometrical facet patch, DBPatchF          */
#define TOP_PAT   5   /* Topological patch, DBPatch                 */
#define PRO_PAT   6   /* Geometrical procedure type patch, DBPatchP */
#define COO_PAT   7   /* Geometrical Coon's patch, DBPatchB         */
#define CON_PAT   8   /* Geometrical Consurf patch, DBPatchN        */
#define P3_PAT    9   /* Geometrical Polynomial patch, DBPatchP3    */
#define P5_PAT   10   /* Geometrical Polynomial patch, DBPatchP5    */
#define P7_PAT   11   /* Geometrical Polynomial patch, DBPatchP7    */
#define P9_PAT   12   /* Geometrical Polynomial patch, DBPatchP9    */
#define P21_PAT  13   /* Geometrical Polynomial patch, DBPatchP21   */
#define NURB_PAT 14   /* Geometrical NURBS-patch(surface),DBPatchNU */
#define BR3_PAT  15   /* Geometrical Bezier     patch, DBPatchBR3   */

/*!*********************** BBOX ***********************************!*/
/*!                                                                !*/
/*! Bounding box for a geometry object                             !*/
/*! -----------------------------------                            !*/
/*!                                                                !*/
/*! The bounding boxes are used by intersection functions          !*/
/*! to determine if an intersect between objects exist.            !*/
/*!                                                                !*/
/*!                                                                !*/

typedef struct
{
gmflt xmin;         /*! xmin is the minimum x value for the object !*/
gmflt ymin;         /*! ymin is the minimum y value for the object !*/
gmflt zmin;         /*! zmin is the minimum z value for the object !*/
gmflt xmax;         /*! xmax is the maximum x value for the object !*/
gmflt ymax;         /*! ymax is the maximum y value for the object !*/
gmflt zmax;         /*! zmax is the maximum z value for the object !*/
short flag;         /*! Eq.   1: Regular (normal) patch            !*/
                    /*! Eq.   2: Triangular patch                  !*/
                    /*! Eq.   3: U boundaries equal (Null Tile)    !*/
                    /*! Eq.   4: V boundaries equal (Null Tile)    !*/
                    /*! Eq.   5: Patch is a point                  !*/
                    /*! Eq. -99: Points could not be calculated    !*/
                    /*!          (Patch is not OK)                 !*/
} BBOX;
/*!*********************** BBOX ***********************************!*/

/*!*********************** BCONE **********************************!*/
/*!                                                                !*/
/*! Bounding cone for normals or tangents                          !*/
/*! --------------------------------------                         !*/
/*!                                                                !*/
/*! The bounding cones are used by surface (or curve) calculation  !*/
/*! functions to determine if a solution exists. One example is    !*/
/*! the silhouette calculation. The BCONE is used to find out      !*/
/*! if none of the surface normals in a patch will be perpen-      !*/
/*! dicular to the viewing direction (vector).                     !*/
/*!                                                                !*/
/*! The geometry objects are surface patches and ........          !*/
/*! ... Johan ! Ska ytor, kurvor och kurvsegment också             !*/
/*!             ha boxar ????                                      !*/
/*!                                                                !*/
typedef struct
{
gmflt xdir;         /*! The center of the cone: x component        !*/
gmflt ydir;         /*!                         y component        !*/
gmflt zdir;         /*!                         z component        !*/
gmflt ang;          /*! The angle for the cone in degrees          !*/
short flag;         /*! Eq.   1: Normals are calculated            !*/
                    /*! Eq.  -1: Normals are not calculated        !*/
                    /*! Eq. -99: Normals are not well defined      !*/
                    /*!          (Patch derivatives are not OK)    !*/
} BCONE;
/*!*********************** BCONE **********************************!*/

/*!*********************** DBSurf *********************************!*/

typedef struct                 /* GMPOSTV2 2007-01-01, J.Kjellander */
{
DBHeader hed_su;               /* Header                            */
short    typ_su;               /* CUB_SUR/RAT_SUR/LFT_SUR, etc.     */
short    fnt_su;               /* Font, always = 0                  */
gmflt    lgt_su;               /* Dash length for midcurves         */
short    nu_su;                /* Number of patches in U direction  */
short    nv_su;                /* Number of patches in V direction  */
short    ngu_su;               /* Number of U-lines in wire rep     */
short    ngv_su;               /* Number of V-lines in wire rep     */
DBptr    ptab_su;              /* DBptr to patch table              */
DBptr    pcsy_su;              /* DBptr to active csys              */
short    ngseg_su[6];          /* Not used                          */
DBptr    pgseg_su[6];          /* Not used                          */
int      uorder_su;            /* NURBS order in U direction        */
int      vorder_su;            /* NURBS order in V direction        */
int      nku_su;               /* Number of knots in U dir.         */
int      nkv_su;               /* Number of knots in V dir.         */
DBptr    pkvu_su;              /* DBptr to U knots                  */
DBptr    pkvv_su;              /* DBptr to V knots                  */
DBptr    pcpts_su;             /* DBPtr to control points           */
BBOX     box_su;               /* Surface bounding box              */
DBint    ntrim_su;             /* Number of trim curves 0=Untrimmed */
DBptr    getrim_su;            /* DBPtr to geometrical trimcurves   */
DBint    ngrwborder_su;        /* Number of graph. border curves    */
DBptr    grwborder_su;         /* DBptr to graph. border curves     */
DBint    ngrwiso_su;           /* Number of graph. iso mid curves   */
DBptr    grwiso_su;            /* DBptr to graph. iso mid curves    */
DBint    vertextype_su;        /* GL_MAP2_VERTEX_3 / GLMAP2_VERTEX_4*/
DBptr    grstrim_su;           /* DBptr to graphical trim curves    */
DBfloat  nustep_su;            /* Tesselation steps in U dir        */
DBfloat  nvstep_su;            /* Tesselation steps in V dir        */
DBfloat  wdt_su;               /* Linewidth for wireframe rep       */
} DBSurf;
/*!*********************** DBSurf *********************************!*/

/*!*********************** DBPatch ********************************!*/
/*!                                                                !*/
/*! Topological surface patch                                      !*/
/*! -------------------------                                      !*/
/*!                                                                !*/
/*! A surface may consist of many types of geometric patches.      !*/
/*! There must be position continuity between the geometric        !*/
/*! patches, but only parts of the patches need to be in contact.  !*/
/*! This means for instance that a conic lofting surface can be    !*/
/*! combined with a bicubic surface to one (1) surface provided    !*/
/*! that both surfaces have a .................                    !*/
/*!                                                                !*/
/*! For a user (c or MBS programmer) however, the surface will     !*/
/*! consist of a rectangular grid of topological patches of        !*/
/*! type DBPatch. Each DBPatch patch has a pointer to a secondary  !*/
/*! patch. The secondary patch is normally a geometric patch       !*/
/*! type DBPatchC, DBPatchR etc. but can also be another DBPatch if  !*/
/*! more than one DBPatch share the same geometric patch.          !*/
/*! The pointer may also be zero in the case of a NULL-patch.      !*/
/*!                                                                !*/
/*!                   _________                                    !*/
/*!                  !         !                                   !*/
/*!                  ! Surface !                                   !*/
/*!                  ! DBSurf  !                                   !*/
/*!                  !_________!                                   !*/
/*!                       !                                        !*/
/*!        _______________!_____________________________  . . . .  !*/
/*!   ____!_____    _____!____     _____!____     _____!____       !*/
/*!  !          !  !          !   !          !   !          !      !*/
/*!  ! Topolog. !  ! Topolog. !   ! Topolog. !   ! Topolog. !      !*/
/*!  !  Patch   !  !  Patch   !   !  Patch   !   !  Patch   !      !*/
/*!  !  DBPatch !  !  DBPatch !   !  DBPatch !   !  DBPatch !      !*/
/*!  !IU=1  IV=1!  !IU=1  IV=2!   !IU=1  IV=3!   !IU=2  IV=1!      !*/
/*!  !__________!  !__________!   !__________!   !__________!      !*/
/*!       !              !              !              !           !*/
/*!   ____!_____    _____!_____    _____!_____    _____!____       !*/
/*!  !          !  !           !  !           !  !          !      !*/
/*!  !  NULL-   !  ! Geometric !  ! Geometric !  ! Topolog. !      !*/
/*!  !  patch   !  !  patch    !  !  patch    !  !  patch   !      !*/
/*!  !          !  !  DBPatch_C   !  !  DBPatchL   !  !  GMPAT   !      !*/
/*!  !__________!  !___________!  !___________!  !__________!      !*/
/*!                                                                !*/
/*!                                                                !*/
/*!  The topological (DBPatch) u,v plane for a surface:            !*/
/*!                                                                !*/
/*!                                                                !*/
/*! IU=1,IV=NV                              IU=NU,IV=NV            !*/
/*!  ------------------------------------------                    !*/
/*!  !      !      !      !      !     !      !                    !*/
/*!  !  3   !  6   !  9   ! 12   ! 15  ! 18   !                    !*/
/*!  ------------------------------------------                    !*/
/*!  !      !      !      !      !     !      !                    !*/
/*!  !  2   !  5   !  8   ! 11   ! 14  ! 17   !                    !*/
/*!  ------------------------------------------                    !*/
/*!  !      !      !       !      !     !     !                    !*/
/*!  !  1   !  4   !   7   ! 10   ! 13  ! 16  !                    !*/
/*!  ------------------------------------------                    !*/
/*!                                                                !*/
/*! IU=1,IV=1      increasing U --->         IU=NU,IV=1            !*/
/*!                                                                !*/
/*!                                                                !*/

typedef struct
{
short  styp_pat;           /* Type of secondary patch               */
DBptr  spek_gm;            /* DB-pointer to secondary patch         */
char  *spek_c;             /* C-pointer to secondary patch          */
short  su_pat,sv_pat;      /* Topological adress  secondary patch   */
short  iu_pat,iv_pat;      /* Topological address this patch        */
gmflt  us_pat;             /* Start U parameter on geom. patch      */
gmflt  ue_pat;             /* End   U parameter on geom. patch      */
gmflt  vs_pat;             /* Start V parameter on geom. patch      */
gmflt  ve_pat;             /* End   V parameter on geom. patch      */
BBOX   box_pat;            /* Bounding box  for the (DBPatch) patch */
BCONE  cone_pat;           /* Bounding cone for the (DBPatch) patch */
} DBPatch;
/*!*********************** DBPatch ********************************!*/

/*!*********************** DBPatch_C *********************************!*/
/*!                                                                !*/
/*! A non-rational cubic patch                                     !*/
/*! --------------------------                                     !*/
/*!                                                                !*/
/*!                         ! _   _   _   _   ! ! 1  !             !*/
/*!                         ! a00 a01 a02 a03 ! !    !             !*/
/*!                         ! _   _   _   _   ! ! v  !             !*/
/*!                  2  3   ! a10 a11 a12 a13 ! !    !             !*/
/*! R(u,v)= ( 1  u  u  u  ) ! _   _   _   _   ! !  2 !             !*/
/*!                         ! a20 a21 a22 a23 ! ! v  !             !*/
/*!                         ! _   _   _   _   ! !    !             !*/
/*!                         ! a30 a31 a32 a33 ! !  3 !             !*/
/*!                         !                 ! ! v  !             !*/
/*!                                                                !*/
/*!                                                                !*/
/*! The number of coefficients per patch is 48.                    !*/
/*! An offset value can be defined for the patch.                  !*/

typedef struct
{
gmflt a00x,a01x,a02x,a03x;
gmflt a10x,a11x,a12x,a13x;
gmflt a20x,a21x,a22x,a23x;
gmflt a30x,a31x,a32x,a33x;
gmflt a00y,a01y,a02y,a03y;
gmflt a10y,a11y,a12y,a13y;
gmflt a20y,a21y,a22y,a23y;
gmflt a30y,a31y,a32y,a33y;
gmflt a00z,a01z,a02z,a03z;
gmflt a10z,a11z,a12z,a13z;
gmflt a20z,a21z,a22z,a23z;
gmflt a30z,a31z,a32z,a33z;
gmflt ofs_pat;               /* Offset for the patch         */
} DBPatchC;
/*!*********************** DBPatch_C *********************************!*/

/*!*********************** DBPatchR *********************************!*/
/*!                                                                !*/
/*! A rational cubic patch                                         !*/
/*! ----------------------                                         !*/
/*!                                                                !*/
/*!                         ! _   _   _   _   ! ! 1  !             !*/
/*!                         ! a00 a01 a02 a03 ! !    !             !*/
/*!                         ! _   _   _   _   ! ! v  !             !*/
/*!                  2  3   ! a10 a11 a12 a13 ! !    !             !*/
/*! R(u,v)= ( 1  u  u  u  ) ! _   _   _   _   ! !  2 !             !*/
/*!                         ! a20 a21 a22 a23 ! ! v  !             !*/
/*!                         ! _   _   _   _   ! !    !             !*/
/*!                         ! a30 a31 a32 a33 ! !  3 !             !*/
/*!                         !                 ! ! v  !             !*/
/*!                                                                !*/
/*!                                                                !*/
/*! The number of coefficients per patch is 65.                    !*/
/*! An offset value can be defined for the patch.                  !*/

typedef struct
{
gmflt a00x,a01x,a02x,a03x;
gmflt a10x,a11x,a12x,a13x;
gmflt a20x,a21x,a22x,a23x;
gmflt a30x,a31x,a32x,a33x;
gmflt a00y,a01y,a02y,a03y;
gmflt a10y,a11y,a12y,a13y;
gmflt a20y,a21y,a22y,a23y;
gmflt a30y,a31y,a32y,a33y;
gmflt a00z,a01z,a02z,a03z;
gmflt a10z,a11z,a12z,a13z;
gmflt a20z,a21z,a22z,a23z;
gmflt a30z,a31z,a32z,a33z;
gmflt a00 ,a01 ,a02 ,a03 ;
gmflt a10 ,a11 ,a12 ,a13 ;
gmflt a20 ,a21 ,a22 ,a23 ;
gmflt a30 ,a31 ,a32 ,a33 ;
gmflt ofs_pat;               /* Offset for the patch         */
} DBPatchR;
/*!*********************** DBPatchR *********************************!*/

/*!*********************** DBPatchP3 ********************************!*/
/*!                                                                !*/
/*! A third degree polynomial patch                                !*/ 
/*! -------------------------------                                !*/
/*!                                                                !*/
/*! This patch is exactly the same as DBPatch_C but members are       !*/
/*! named diferently in order to allow for patches of high         !*/
/*! degree where more digits are needed for row- and column-       !*/
/*! numbering.                                                     !*/
/*! Number of coefficients per patch is 49.                        !*/
/*! Space required = 392 bytes.                                    !*/
/*!                                                                !*/

typedef struct
{
gmflt c0000x,c0001x,c0002x,c0003x;
gmflt c0100x,c0101x,c0102x,c0103x;
gmflt c0200x,c0201x,c0202x,c0203x;
gmflt c0300x,c0301x,c0302x,c0303x;

gmflt c0000y,c0001y,c0002y,c0003y;
gmflt c0100y,c0101y,c0102y,c0103y;
gmflt c0200y,c0201y,c0202y,c0203y;
gmflt c0300y,c0301y,c0302y,c0303y;

gmflt c0000z,c0001z,c0002z,c0003z;
gmflt c0100z,c0101z,c0102z,c0103z;
gmflt c0200z,c0201z,c0202z,c0203z;
gmflt c0300z,c0301z,c0302z,c0303z;
gmflt ofs_pat;
} DBPatchP3;
/*!*********************** DBPatchP3 ********************************!*/

/*!*********************** DBPatchP5 ********************************!*/
/*!                                                                !*/
/*! A fifth degree polynomial patch                                !*/ 
/*! -------------------------------                                !*/
/*!                                                                !*/
/*! Number of coefficients per patch is 109.                       !*/
/*! Space required = 872 bytes.                                    !*/
/*!                                                                !*/

typedef struct
{
gmflt c0000x,c0001x,c0002x,c0003x,c0004x,c0005x;
gmflt c0100x,c0101x,c0102x,c0103x,c0104x,c0105x;
gmflt c0200x,c0201x,c0202x,c0203x,c0204x,c0205x;
gmflt c0300x,c0301x,c0302x,c0303x,c0304x,c0305x;
gmflt c0400x,c0401x,c0402x,c0403x,c0404x,c0405x;
gmflt c0500x,c0501x,c0502x,c0503x,c0504x,c0505x;

gmflt c0000y,c0001y,c0002y,c0003y,c0004y,c0005y;
gmflt c0100y,c0101y,c0102y,c0103y,c0104y,c0105y;
gmflt c0200y,c0201y,c0202y,c0203y,c0204y,c0205y;
gmflt c0300y,c0301y,c0302y,c0303y,c0304y,c0305y;
gmflt c0400y,c0401y,c0402y,c0403y,c0404y,c0405y;
gmflt c0500y,c0501y,c0502y,c0503y,c0504y,c0505y;

gmflt c0000z,c0001z,c0002z,c0003z,c0004z,c0005z;
gmflt c0100z,c0101z,c0102z,c0103z,c0104z,c0105z;
gmflt c0200z,c0201z,c0202z,c0203z,c0204z,c0205z;
gmflt c0300z,c0301z,c0302z,c0303z,c0304z,c0305z;
gmflt c0400z,c0401z,c0402z,c0403z,c0404z,c0405z;
gmflt c0500z,c0501z,c0502z,c0503z,c0504z,c0505z;
gmflt ofs_pat;
} DBPatchP5;
/*!*********************** DBPatchP5 ********************************!*/

/*!*********************** DBPatchP7 ********************************!*/
/*!                                                                !*/
/*! A seventh degree polynomial patch                              !*/ 
/*! ---------------------------------                              !*/
/*!                                                                !*/
/*! Number of coefficients per patch is 193.                       !*/
/*! Space required = 1544 bytes.                                   !*/
/*!                                                                !*/

typedef struct
{
gmflt c0000x,c0001x,c0002x,c0003x,c0004x,c0005x,c0006x,c0007x;
gmflt c0100x,c0101x,c0102x,c0103x,c0104x,c0105x,c0106x,c0107x;
gmflt c0200x,c0201x,c0202x,c0203x,c0204x,c0205x,c0206x,c0207x;
gmflt c0300x,c0301x,c0302x,c0303x,c0304x,c0305x,c0306x,c0307x;
gmflt c0400x,c0401x,c0402x,c0403x,c0404x,c0405x,c0406x,c0407x;
gmflt c0500x,c0501x,c0502x,c0503x,c0504x,c0505x,c0506x,c0507x;
gmflt c0600x,c0601x,c0602x,c0603x,c0604x,c0605x,c0606x,c0607x;
gmflt c0700x,c0701x,c0702x,c0703x,c0704x,c0705x,c0706x,c0707x;

gmflt c0000y,c0001y,c0002y,c0003y,c0004y,c0005y,c0006y,c0007y;
gmflt c0100y,c0101y,c0102y,c0103y,c0104y,c0105y,c0106y,c0107y;
gmflt c0200y,c0201y,c0202y,c0203y,c0204y,c0205y,c0206y,c0207y;
gmflt c0300y,c0301y,c0302y,c0303y,c0304y,c0305y,c0306y,c0307y;
gmflt c0400y,c0401y,c0402y,c0403y,c0404y,c0405y,c0406y,c0407y;
gmflt c0500y,c0501y,c0502y,c0503y,c0504y,c0505y,c0506y,c0507y;
gmflt c0600y,c0601y,c0602y,c0603y,c0604y,c0605y,c0606y,c0607y;
gmflt c0700y,c0701y,c0702y,c0703y,c0704y,c0705y,c0706y,c0707y;

gmflt c0000z,c0001z,c0002z,c0003z,c0004z,c0005z,c0006z,c0007z;
gmflt c0100z,c0101z,c0102z,c0103z,c0104z,c0105z,c0106z,c0107z;
gmflt c0200z,c0201z,c0202z,c0203z,c0204z,c0205z,c0206z,c0207z;
gmflt c0300z,c0301z,c0302z,c0303z,c0304z,c0305z,c0306z,c0307z;
gmflt c0400z,c0401z,c0402z,c0403z,c0404z,c0405z,c0406z,c0407z;
gmflt c0500z,c0501z,c0502z,c0503z,c0504z,c0505z,c0506z,c0507z;
gmflt c0600z,c0601z,c0602z,c0603z,c0604z,c0605z,c0606z,c0607z;
gmflt c0700z,c0701z,c0702z,c0703z,c0704z,c0705z,c0706z,c0707z;
gmflt ofs_pat;
} DBPatchP7;
/*!*********************** DBPatchP7 ********************************!*/

/*!*********************** DBPatchP9 ********************************!*/
/*!                                                                !*/
/*! A ninth degree polynomial patch                                !*/ 
/*! -------------------------------                                !*/
/*!                                                                !*/
/*! Number of coefficients per patch is 301.                       !*/
/*! Space required = 2408 bytes.                                   !*/
/*!                                                                !*/

typedef struct
{
gmflt c0000x,c0001x,c0002x,c0003x,c0004x,c0005x,c0006x,c0007x,
      c0008x,c0009x;
gmflt c0100x,c0101x,c0102x,c0103x,c0104x,c0105x,c0106x,c0107x,
      c0108x,c0109x;
gmflt c0200x,c0201x,c0202x,c0203x,c0204x,c0205x,c0206x,c0207x,
      c0208x,c0209x;
gmflt c0300x,c0301x,c0302x,c0303x,c0304x,c0305x,c0306x,c0307x,
      c0308x,c0309x;
gmflt c0400x,c0401x,c0402x,c0403x,c0404x,c0405x,c0406x,c0407x,
      c0408x,c0409x;
gmflt c0500x,c0501x,c0502x,c0503x,c0504x,c0505x,c0506x,c0507x,
      c0508x,c0509x;
gmflt c0600x,c0601x,c0602x,c0603x,c0604x,c0605x,c0606x,c0607x,
      c0608x,c0609x;
gmflt c0700x,c0701x,c0702x,c0703x,c0704x,c0705x,c0706x,c0707x,
      c0708x,c0709x;
gmflt c0800x,c0801x,c0802x,c0803x,c0804x,c0805x,c0806x,c0807x,
      c0808x,c0809x;
gmflt c0900x,c0901x,c0902x,c0903x,c0904x,c0905x,c0906x,c0907x,
      c0908x,c0909x;

gmflt c0000y,c0001y,c0002y,c0003y,c0004y,c0005y,c0006y,c0007y,
      c0008y,c0009y;
gmflt c0100y,c0101y,c0102y,c0103y,c0104y,c0105y,c0106y,c0107y,
      c0108y,c0109y;
gmflt c0200y,c0201y,c0202y,c0203y,c0204y,c0205y,c0206y,c0207y,
      c0208y,c0209y;
gmflt c0300y,c0301y,c0302y,c0303y,c0304y,c0305y,c0306y,c0307y,
      c0308y,c0309y;
gmflt c0400y,c0401y,c0402y,c0403y,c0404y,c0405y,c0406y,c0407y,
      c0408y,c0409y;
gmflt c0500y,c0501y,c0502y,c0503y,c0504y,c0505y,c0506y,c0507y,
      c0508y,c0509y;
gmflt c0600y,c0601y,c0602y,c0603y,c0604y,c0605y,c0606y,c0607y,
      c0608y,c0609y;
gmflt c0700y,c0701y,c0702y,c0703y,c0704y,c0705y,c0706y,c0707y,
      c0708y,c0709y;
gmflt c0800y,c0801y,c0802y,c0803y,c0804y,c0805y,c0806y,c0807y,
      c0808y,c0809y;
gmflt c0900y,c0901y,c0902y,c0903y,c0904y,c0905y,c0906y,c0907y,
      c0908y,c0909y;

gmflt c0000z,c0001z,c0002z,c0003z,c0004z,c0005z,c0006z,c0007z,
      c0008z,c0009z;
gmflt c0100z,c0101z,c0102z,c0103z,c0104z,c0105z,c0106z,c0107z,
      c0108z,c0109z;
gmflt c0200z,c0201z,c0202z,c0203z,c0204z,c0205z,c0206z,c0207z,
      c0208z,c0209z;
gmflt c0300z,c0301z,c0302z,c0303z,c0304z,c0305z,c0306z,c0307z,
      c0308z,c0309z;
gmflt c0400z,c0401z,c0402z,c0403z,c0404z,c0405z,c0406z,c0407z,
      c0408z,c0409z;
gmflt c0500z,c0501z,c0502z,c0503z,c0504z,c0505z,c0506z,c0507z,
      c0508z,c0509z;
gmflt c0600z,c0601z,c0602z,c0603z,c0604z,c0605z,c0606z,c0607z,
      c0608z,c0609z;
gmflt c0700z,c0701z,c0702z,c0703z,c0704z,c0705z,c0706z,c0707z,
      c0708z,c0709z;
gmflt c0800z,c0801z,c0802z,c0803z,c0804z,c0805z,c0806z,c0807z,
      c0808z,c0809z;
gmflt c0900z,c0901z,c0902z,c0903z,c0904z,c0905z,c0906z,c0907z,
      c0908z,c0909z;
gmflt ofs_pat;
} DBPatchP9;
/*!*********************** DBPatchP9 ********************************!*/

/*!*********************** DBPatchP21 *******************************!*/
/*!                                                                !*/
/*! A 21:st degree polynomial patch                                !*/ 
/*! -------------------------------                                !*/
/*!                                                                !*/
/*! Number of coefficients per patch is 1453.                      !*/
/*! Space required = 11616 bytes.                                  !*/
/*!                                                                !*/

typedef struct
{
gmflt c0000x,c0001x,c0002x,c0003x,c0004x,c0005x,c0006x,c0007x,
      c0008x,c0009x,c0010x,c0011x,c0012x,c0013x,c0014x,c0015x,
      c0016x,c0017x,c0018x,c0019x,c0020x,c0021x;
gmflt c0100x,c0101x,c0102x,c0103x,c0104x,c0105x,c0106x,c0107x,
      c0108x,c0109x,c0110x,c0111x,c0112x,c0113x,c0114x,c0115x,
      c0116x,c0117x,c0118x,c0119x,c0120x,c0121x;
gmflt c0200x,c0201x,c0202x,c0203x,c0204x,c0205x,c0206x,c0207x,
      c0208x,c0209x,c0210x,c0211x,c0212x,c0213x,c0214x,c0215x,
      c0216x,c0217x,c0218x,c0219x,c0220x,c0221x;
gmflt c0300x,c0301x,c0302x,c0303x,c0304x,c0305x,c0306x,c0307x,
      c0308x,c0309x,c0310x,c0311x,c0312x,c0313x,c0314x,c0315x,
      c0316x,c0317x,c0318x,c0319x,c0320x,c0321x;
gmflt c0400x,c0401x,c0402x,c0403x,c0404x,c0405x,c0406x,c0407x,
      c0408x,c0409x,c0410x,c0411x,c0412x,c0413x,c0414x,c0415x,
      c0416x,c0417x,c0418x,c0419x,c0420x,c0421x;
gmflt c0500x,c0501x,c0502x,c0503x,c0504x,c0505x,c0506x,c0507x,
      c0508x,c0509x,c0510x,c0511x,c0512x,c0513x,c0514x,c0515x,
      c0516x,c0517x,c0518x,c0519x,c0520x,c0521x;
gmflt c0600x,c0601x,c0602x,c0603x,c0604x,c0605x,c0606x,c0607x,
      c0608x,c0609x,c0610x,c0611x,c0612x,c0613x,c0614x,c0615x,
      c0616x,c0617x,c0618x,c0619x,c0620x,c0621x;
gmflt c0700x,c0701x,c0702x,c0703x,c0704x,c0705x,c0706x,c0707x,
      c0708x,c0709x,c0710x,c0711x,c0712x,c0713x,c0714x,c0715x,
      c0716x,c0717x,c0718x,c0719x,c0720x,c0721x;
gmflt c0800x,c0801x,c0802x,c0803x,c0804x,c0805x,c0806x,c0807x,
      c0808x,c0809x,c0810x,c0811x,c0812x,c0813x,c0814x,c0815x,
      c0816x,c0817x,c0818x,c0819x,c0820x,c0821x;
gmflt c0900x,c0901x,c0902x,c0903x,c0904x,c0905x,c0906x,c0907x,
      c0908x,c0909x,c0910x,c0911x,c0912x,c0913x,c0914x,c0915x,
      c0916x,c0917x,c0918x,c0919x,c0920x,c0921x;
gmflt c1000x,c1001x,c1002x,c1003x,c1004x,c1005x,c1006x,c1007x,
      c1008x,c1009x,c1010x,c1011x,c1012x,c1013x,c1014x,c1015x,
      c1016x,c1017x,c1018x,c1019x,c1020x,c1021x;
gmflt c1100x,c1101x,c1102x,c1103x,c1104x,c1105x,c1106x,c1107x,
      c1108x,c1109x,c1110x,c1111x,c1112x,c1113x,c1114x,c1115x,
      c1116x,c1117x,c1118x,c1119x,c1120x,c1121x;
gmflt c1200x,c1201x,c1202x,c1203x,c1204x,c1205x,c1206x,c1207x,
      c1208x,c1209x,c1210x,c1211x,c1212x,c1213x,c1214x,c1215x,
      c1216x,c1217x,c1218x,c1219x,c1220x,c1221x;
gmflt c1300x,c1301x,c1302x,c1303x,c1304x,c1305x,c1306x,c1307x,
      c1308x,c1309x,c1310x,c1311x,c1312x,c1313x,c1314x,c1315x,
      c1316x,c1317x,c1318x,c1319x,c1320x,c1321x;
gmflt c1400x,c1401x,c1402x,c1403x,c1404x,c1405x,c1406x,c1407x,
      c1408x,c1409x,c1410x,c1411x,c1412x,c1413x,c1414x,c1415x,
      c1416x,c1417x,c1418x,c1419x,c1420x,c1421x;
gmflt c1500x,c1501x,c1502x,c1503x,c1504x,c1505x,c1506x,c1507x,
      c1508x,c1509x,c1510x,c1511x,c1512x,c1513x,c1514x,c1515x,
      c1516x,c1517x,c1518x,c1519x,c1520x,c1521x;
gmflt c1600x,c1601x,c1602x,c1603x,c1604x,c1605x,c1606x,c1607x,
      c1608x,c1609x,c1610x,c1611x,c1612x,c1613x,c1614x,c1615x,
      c1616x,c1617x,c1618x,c1619x,c1620x,c1621x;
gmflt c1700x,c1701x,c1702x,c1703x,c1704x,c1705x,c1706x,c1707x,
      c1708x,c1709x,c1710x,c1711x,c1712x,c1713x,c1714x,c1715x,
      c1716x,c1717x,c1718x,c1719x,c1720x,c1721x;
gmflt c1800x,c1801x,c1802x,c1803x,c1804x,c1805x,c1806x,c1807x,
      c1808x,c1809x,c1810x,c1811x,c1812x,c1813x,c1814x,c1815x,
      c1816x,c1817x,c1818x,c1819x,c1820x,c1821x;
gmflt c1900x,c1901x,c1902x,c1903x,c1904x,c1905x,c1906x,c1907x,
      c1908x,c1909x,c1910x,c1911x,c1912x,c1913x,c1914x,c1915x,
      c1916x,c1917x,c1918x,c1919x,c1920x,c1921x;
gmflt c2000x,c2001x,c2002x,c2003x,c2004x,c2005x,c2006x,c2007x,
      c2008x,c2009x,c2010x,c2011x,c2012x,c2013x,c2014x,c2015x,
      c2016x,c2017x,c2018x,c2019x,c2020x,c2021x;
gmflt c2100x,c2101x,c2102x,c2103x,c2104x,c2105x,c2106x,c2107x,
      c2108x,c2109x,c2110x,c2111x,c2112x,c2113x,c2114x,c2115x,
      c2116x,c2117x,c2118x,c2119x,c2120x,c2121x;

gmflt c0000y,c0001y,c0002y,c0003y,c0004y,c0005y,c0006y,c0007y,
      c0008y,c0009y,c0010y,c0011y,c0012y,c0013y,c0014y,c0015y,
      c0016y,c0017y,c0018y,c0019y,c0020y,c0021y;
gmflt c0100y,c0101y,c0102y,c0103y,c0104y,c0105y,c0106y,c0107y,
      c0108y,c0109y,c0110y,c0111y,c0112y,c0113y,c0114y,c0115y,
      c0116y,c0117y,c0118y,c0119y,c0120y,c0121y;
gmflt c0200y,c0201y,c0202y,c0203y,c0204y,c0205y,c0206y,c0207y,
      c0208y,c0209y,c0210y,c0211y,c0212y,c0213y,c0214y,c0215y,
      c0216y,c0217y,c0218y,c0219y,c0220y,c0221y;
gmflt c0300y,c0301y,c0302y,c0303y,c0304y,c0305y,c0306y,c0307y,
      c0308y,c0309y,c0310y,c0311y,c0312y,c0313y,c0314y,c0315y,
      c0316y,c0317y,c0318y,c0319y,c0320y,c0321y;
gmflt c0400y,c0401y,c0402y,c0403y,c0404y,c0405y,c0406y,c0407y,
      c0408y,c0409y,c0410y,c0411y,c0412y,c0413y,c0414y,c0415y,
      c0416y,c0417y,c0418y,c0419y,c0420y,c0421y;
gmflt c0500y,c0501y,c0502y,c0503y,c0504y,c0505y,c0506y,c0507y,
      c0508y,c0509y,c0510y,c0511y,c0512y,c0513y,c0514y,c0515y,
      c0516y,c0517y,c0518y,c0519y,c0520y,c0521y;
gmflt c0600y,c0601y,c0602y,c0603y,c0604y,c0605y,c0606y,c0607y,
      c0608y,c0609y,c0610y,c0611y,c0612y,c0613y,c0614y,c0615y,
      c0616y,c0617y,c0618y,c0619y,c0620y,c0621y;
gmflt c0700y,c0701y,c0702y,c0703y,c0704y,c0705y,c0706y,c0707y,
      c0708y,c0709y,c0710y,c0711y,c0712y,c0713y,c0714y,c0715y,
      c0716y,c0717y,c0718y,c0719y,c0720y,c0721y;
gmflt c0800y,c0801y,c0802y,c0803y,c0804y,c0805y,c0806y,c0807y,
      c0808y,c0809y,c0810y,c0811y,c0812y,c0813y,c0814y,c0815y,
      c0816y,c0817y,c0818y,c0819y,c0820y,c0821y;
gmflt c0900y,c0901y,c0902y,c0903y,c0904y,c0905y,c0906y,c0907y,
      c0908y,c0909y,c0910y,c0911y,c0912y,c0913y,c0914y,c0915y,
      c0916y,c0917y,c0918y,c0919y,c0920y,c0921y;
gmflt c1000y,c1001y,c1002y,c1003y,c1004y,c1005y,c1006y,c1007y,
      c1008y,c1009y,c1010y,c1011y,c1012y,c1013y,c1014y,c1015y,
      c1016y,c1017y,c1018y,c1019y,c1020y,c1021y;
gmflt c1100y,c1101y,c1102y,c1103y,c1104y,c1105y,c1106y,c1107y,
      c1108y,c1109y,c1110y,c1111y,c1112y,c1113y,c1114y,c1115y,
      c1116y,c1117y,c1118y,c1119y,c1120y,c1121y;
gmflt c1200y,c1201y,c1202y,c1203y,c1204y,c1205y,c1206y,c1207y,
      c1208y,c1209y,c1210y,c1211y,c1212y,c1213y,c1214y,c1215y,
      c1216y,c1217y,c1218y,c1219y,c1220y,c1221y;
gmflt c1300y,c1301y,c1302y,c1303y,c1304y,c1305y,c1306y,c1307y,
      c1308y,c1309y,c1310y,c1311y,c1312y,c1313y,c1314y,c1315y,
      c1316y,c1317y,c1318y,c1319y,c1320y,c1321y;
gmflt c1400y,c1401y,c1402y,c1403y,c1404y,c1405y,c1406y,c1407y,
      c1408y,c1409y,c1410y,c1411y,c1412y,c1413y,c1414y,c1415y,
      c1416y,c1417y,c1418y,c1419y,c1420y,c1421y;
gmflt c1500y,c1501y,c1502y,c1503y,c1504y,c1505y,c1506y,c1507y,
      c1508y,c1509y,c1510y,c1511y,c1512y,c1513y,c1514y,c1515y,
      c1516y,c1517y,c1518y,c1519y,c1520y,c1521y;
gmflt c1600y,c1601y,c1602y,c1603y,c1604y,c1605y,c1606y,c1607y,
      c1608y,c1609y,c1610y,c1611y,c1612y,c1613y,c1614y,c1615y,
      c1616y,c1617y,c1618y,c1619y,c1620y,c1621y;
gmflt c1700y,c1701y,c1702y,c1703y,c1704y,c1705y,c1706y,c1707y,
      c1708y,c1709y,c1710y,c1711y,c1712y,c1713y,c1714y,c1715y,
      c1716y,c1717y,c1718y,c1719y,c1720y,c1721y;
gmflt c1800y,c1801y,c1802y,c1803y,c1804y,c1805y,c1806y,c1807y,
      c1808y,c1809y,c1810y,c1811y,c1812y,c1813y,c1814y,c1815y,
      c1816y,c1817y,c1818y,c1819y,c1820y,c1821y;
gmflt c1900y,c1901y,c1902y,c1903y,c1904y,c1905y,c1906y,c1907y,
      c1908y,c1909y,c1910y,c1911y,c1912y,c1913y,c1914y,c1915y,
      c1916y,c1917y,c1918y,c1919y,c1920y,c1921y;
gmflt c2000y,c2001y,c2002y,c2003y,c2004y,c2005y,c2006y,c2007y,
      c2008y,c2009y,c2010y,c2011y,c2012y,c2013y,c2014y,c2015y,
      c2016y,c2017y,c2018y,c2019y,c2020y,c2021y;
gmflt c2100y,c2101y,c2102y,c2103y,c2104y,c2105y,c2106y,c2107y,
      c2108y,c2109y,c2110y,c2111y,c2112y,c2113y,c2114y,c2115y,
      c2116y,c2117y,c2118y,c2119y,c2120y,c2121y;

gmflt c0000z,c0001z,c0002z,c0003z,c0004z,c0005z,c0006z,c0007z,
      c0008z,c0009z,c0010z,c0011z,c0012z,c0013z,c0014z,c0015z,
      c0016z,c0017z,c0018z,c0019z,c0020z,c0021z;
gmflt c0100z,c0101z,c0102z,c0103z,c0104z,c0105z,c0106z,c0107z,
      c0108z,c0109z,c0110z,c0111z,c0112z,c0113z,c0114z,c0115z,
      c0116z,c0117z,c0118z,c0119z,c0120z,c0121z;
gmflt c0200z,c0201z,c0202z,c0203z,c0204z,c0205z,c0206z,c0207z,
      c0208z,c0209z,c0210z,c0211z,c0212z,c0213z,c0214z,c0215z,
      c0216z,c0217z,c0218z,c0219z,c0220z,c0221z;
gmflt c0300z,c0301z,c0302z,c0303z,c0304z,c0305z,c0306z,c0307z,
      c0308z,c0309z,c0310z,c0311z,c0312z,c0313z,c0314z,c0315z,
      c0316z,c0317z,c0318z,c0319z,c0320z,c0321z;
gmflt c0400z,c0401z,c0402z,c0403z,c0404z,c0405z,c0406z,c0407z,
      c0408z,c0409z,c0410z,c0411z,c0412z,c0413z,c0414z,c0415z,
      c0416z,c0417z,c0418z,c0419z,c0420z,c0421z;
gmflt c0500z,c0501z,c0502z,c0503z,c0504z,c0505z,c0506z,c0507z,
      c0508z,c0509z,c0510z,c0511z,c0512z,c0513z,c0514z,c0515z,
      c0516z,c0517z,c0518z,c0519z,c0520z,c0521z;
gmflt c0600z,c0601z,c0602z,c0603z,c0604z,c0605z,c0606z,c0607z,
      c0608z,c0609z,c0610z,c0611z,c0612z,c0613z,c0614z,c0615z,
      c0616z,c0617z,c0618z,c0619z,c0620z,c0621z;
gmflt c0700z,c0701z,c0702z,c0703z,c0704z,c0705z,c0706z,c0707z,
      c0708z,c0709z,c0710z,c0711z,c0712z,c0713z,c0714z,c0715z,
      c0716z,c0717z,c0718z,c0719z,c0720z,c0721z;
gmflt c0800z,c0801z,c0802z,c0803z,c0804z,c0805z,c0806z,c0807z,
      c0808z,c0809z,c0810z,c0811z,c0812z,c0813z,c0814z,c0815z,
      c0816z,c0817z,c0818z,c0819z,c0820z,c0821z;
gmflt c0900z,c0901z,c0902z,c0903z,c0904z,c0905z,c0906z,c0907z,
      c0908z,c0909z,c0910z,c0911z,c0912z,c0913z,c0914z,c0915z,
      c0916z,c0917z,c0918z,c0919z,c0920z,c0921z;
gmflt c1000z,c1001z,c1002z,c1003z,c1004z,c1005z,c1006z,c1007z,
      c1008z,c1009z,c1010z,c1011z,c1012z,c1013z,c1014z,c1015z,
      c1016z,c1017z,c1018z,c1019z,c1020z,c1021z;
gmflt c1100z,c1101z,c1102z,c1103z,c1104z,c1105z,c1106z,c1107z,
      c1108z,c1109z,c1110z,c1111z,c1112z,c1113z,c1114z,c1115z,
      c1116z,c1117z,c1118z,c1119z,c1120z,c1121z;
gmflt c1200z,c1201z,c1202z,c1203z,c1204z,c1205z,c1206z,c1207z,
      c1208z,c1209z,c1210z,c1211z,c1212z,c1213z,c1214z,c1215z,
      c1216z,c1217z,c1218z,c1219z,c1220z,c1221z;
gmflt c1300z,c1301z,c1302z,c1303z,c1304z,c1305z,c1306z,c1307z,
      c1308z,c1309z,c1310z,c1311z,c1312z,c1313z,c1314z,c1315z,
      c1316z,c1317z,c1318z,c1319z,c1320z,c1321z;
gmflt c1400z,c1401z,c1402z,c1403z,c1404z,c1405z,c1406z,c1407z,
      c1408z,c1409z,c1410z,c1411z,c1412z,c1413z,c1414z,c1415z,
      c1416z,c1417z,c1418z,c1419z,c1420z,c1421z;
gmflt c1500z,c1501z,c1502z,c1503z,c1504z,c1505z,c1506z,c1507z,
      c1508z,c1509z,c1510z,c1511z,c1512z,c1513z,c1514z,c1515z,
      c1516z,c1517z,c1518z,c1519z,c1520z,c1521z;
gmflt c1600z,c1601z,c1602z,c1603z,c1604z,c1605z,c1606z,c1607z,
      c1608z,c1609z,c1610z,c1611z,c1612z,c1613z,c1614z,c1615z,
      c1616z,c1617z,c1618z,c1619z,c1620z,c1621z;
gmflt c1700z,c1701z,c1702z,c1703z,c1704z,c1705z,c1706z,c1707z,
      c1708z,c1709z,c1710z,c1711z,c1712z,c1713z,c1714z,c1715z,
      c1716z,c1717z,c1718z,c1719z,c1720z,c1721z;
gmflt c1800z,c1801z,c1802z,c1803z,c1804z,c1805z,c1806z,c1807z,
      c1808z,c1809z,c1810z,c1811z,c1812z,c1813z,c1814z,c1815z,
      c1816z,c1817z,c1818z,c1819z,c1820z,c1821z;
gmflt c1900z,c1901z,c1902z,c1903z,c1904z,c1905z,c1906z,c1907z,
      c1908z,c1909z,c1910z,c1911z,c1912z,c1913z,c1914z,c1915z,
      c1916z,c1917z,c1918z,c1919z,c1920z,c1921z;
gmflt c2000z,c2001z,c2002z,c2003z,c2004z,c2005z,c2006z,c2007z,
      c2008z,c2009z,c2010z,c2011z,c2012z,c2013z,c2014z,c2015z,
      c2016z,c2017z,c2018z,c2019z,c2020z,c2021z;
gmflt c2100z,c2101z,c2102z,c2103z,c2104z,c2105z,c2106z,c2107z,
      c2108z,c2109z,c2110z,c2111z,c2112z,c2113z,c2114z,c2115z,
      c2116z,c2117z,c2118z,c2119z,c2120z,c2121z;
gmflt ofs_pat;
} DBPatchP21;
/*!*********************** DBPatchP21 *******************************!*/

/*!*********************** DBPatchL *********************************!*/
/*!                                                                !*/
/*! A patch for a Conic Lofting Surface                            !*/
/*! -----------------------------------                            !*/
/*!                                                                !*/
/*! The patch is used for the definition of many types of          !*/
/*! procedural surfaces, where the surface is defined by           !*/
/*! longitudinal curves (also called directrices or limit          !*/
/*! curves) and a generatrix curve (section curve).                !*/
/*!                                                                !*/
/*! This patch holds data for five kinds of patches with           !*/
/*! different evaluation routines (procedures) for each            !*/
/*! type of patch. Also the transformations are different          !*/
/*! for the different patch types.                                 !*/
/*!                                                                !*/
/*! The description of the curve segments below are only valid     !*/
/*! for the first two types of patches:                            !*/
/*!                                                                !*/
/*! 1. SUR_CONIC with P-value function                             !*/
/*!    See description below                                       !*/
/*!                                                                !*/
/*! 2. SUR_CONIC with mid curve                                    !*/
/*!    See description below                                       !*/
/*!                                                                !*/
/*! 3. SUR_ROT                                                     !*/
/*!    spine                     Rotation line (straight line)     !*/
/*!    lims                      Start rotation curve segment      !*/
/*!    lime                      End   rotation curve segment      !*/
/*!    tans                      Not used                          !*/
/*!    tane                      Not used                          !*/
/*!    midc                      Not used                          !*/
/*!    pval                      Not used                          !*/
/*!    vs                        Not used                          !*/
/*!    ve                        Not used                          !*/
/*!                                                                !*/
/*!                                                                !*/
/*! 4. SUR_RULED arclength ruling (including SUR_CYL)              !*/
/*!    spine                     Not used (but = lims)             !*/
/*!    lims                      Start boundary curve              !*/
/*!    lime                      End   boundary curve              !*/
/*!    tans                      Not used                          !*/
/*!    tane                      Not used                          !*/
/*!    midc                      Not used                          !*/
/*!    pval                      Not used                          !*/
/*!    vs                        Not used                          !*/
/*!    ve                        Not used                          !*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*! 5. SUR_SWEEP                                                   !*/
/*!    spine                     Sweeping curve segment            !*/
/*!    lims                      Section curve segment             !*/
/*!    lime                      Not used                          !*/
/*!    tans                      Not used                          !*/
/*!    tane                      Not used                          !*/
/*!    midc                      Not used                          !*/
/*!    pval                      Constant vector for Y axis        !*/
/*!    vs                        Not used                          !*/
/*!    ve                        Not used                          !*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/

typedef struct
{
DBSeg spine;                 /* Spine         curve segment         */
DBSeg lims;                  /* Start limit   curve segment         */
DBSeg lime;                  /* End   limit   curve segment         */
DBSeg tans;                  /* Start tangent curve segment         */
DBSeg tane;                  /* End   tangent curve segment         */
DBSeg midc;                  /* Mid           curve segment         */
DBSeg pval;                  /* P value function                    */
short p_flag;                /* Patch flag:                         */
                             /* Eq. 1: P-value function             */
                             /* Eq. 2: Mid curve                    */
                             /* Eq. 3: Rotation surface             */
                             /* Eq. 4: Ruled surface                */
                             /* Eq. 5: Swept surface                */
DBLine vs;                   /* V start line                        */
DBLine ve;                   /* V end   line                        */
gmflt ofs_pat;               /* Offset for the patch                */
} DBPatchL;
/*!*********************** DBPatchL *********************************!*/

/*!*********************** DBPatchF ***********************************/
/*                                                                  */
/* A plane facet patch                                              */
/* -------------------                                              */
/*                                                                  */
/* The patch   is defined by four points ...                        */
/*                                                                  */
/*       p4  _______ p3                                             */
/*          !      /!                                               */
/*   pl1->  !    /  !                                               */
/*          !  /    ! <-pl2                                         */
/*          !/______!                                               */
/*       p1        p2                                               */
/*                                                                  */
/*                                                                  */

typedef struct
{
DBVector p1;                    /* Point p1                            */
DBVector p2;                    /* Point p2                            */
DBVector p3;                    /* Point p3                            */
DBVector p4;                    /* Point p4                            */
gmflt u1;                    /* U value for point 1                 */
gmflt v1;                    /* V value for point 1                 */
gmflt u2;                    /* U value for point 2                 */
gmflt v2;                    /* V value for point 2                 */
gmflt u3;                    /* U value for point 3                 */
gmflt v3;                    /* V value for point 3                 */
gmflt u4;                    /* U value for point 4                 */
gmflt v4;                    /* V value for point 4                 */
bool  triangles;             /* Eq.  TRUE: Two triangles (B_PLANE's)*/
bool  defined;               /* Eq.  TRUE: Patch is defined         */
DBptr p_att;                 /* Pointer to additional attributes    */
} DBPatchF;
/*!*********************** DBPatchF ***********************************/

/*!*********************** DBPatchP *********************************!*/
/*!                                                                !*/
/*! A general procedural type of surface patch                     !*/
/*! ------------------------------------------                     !*/
/*!                                                                !*/
/*! Curves with many segments...................                   !*/
/*! A big "patch" used by many DBPatch patches.                    !*/
/*!  .. Egentligen vill jag ha DBCurve .. elements ........        !*/

typedef struct
{
DBSeg spine[5];              /* Spine         curve                 */
short n_spine;               /* Number of curve segments: spine     */
DBSeg curve01[5];            /* Curve 01                            */
short n_c01;                 /* Number of curve segments: curve01   */
DBSeg curve02[5];            /* Curve 02                            */
short n_c02;                 /* Number of curve segments: curve02   */
DBSeg curve03[5];            /* Curve 03                            */
short n_c03;                 /* Number of curve segments: curve03   */
DBSeg curve04[5];            /* Curve 04                            */
short n_c04;                 /* Number of curve segments: curve04   */
DBSeg curve05[5];            /* Curve 05                            */
short n_c05;                 /* Number of curve segments: curve05   */
DBSeg curve06[5];            /* Curve 06                            */
short n_c06;                 /* Number of curve segments: curve06   */
DBSeg curve07[5];            /* Curve 07                            */
short n_c07;                 /* Number of curve segments: curve07   */
DBSeg curve08[5];            /* Curve 08                            */
short n_c08;                 /* Number of curve segments: curve08   */
DBSeg curve09[5];            /* Curve 09                            */
short n_c09;                 /* Number of curve segments: curve09   */
DBSeg curve10[5];            /* Curve 10                            */
short n_c10;                 /* Number of curve segments: curve10   */
gmflt p_flt1;                /* Patch float 1                       */
gmflt p_flt2;                /* Patch float 2                       */
short p_flag1;               /* Patch flag 1                        */
short p_flag2;               /* Patch flag 2                        */
gmflt ofs_pat;               /* Offset            for the patch     */
} DBPatchP;
/*!*********************** DBPatchP *********************************!*/

/*!*********************** DBPatchB *********************************!*/
/*!                                                                !*/
/*! Coons patch with linear blending functions                     !*/
/*! ------------------------------------------                     !*/
/*!                                                                !*/
/*! Refererence: Faux & Pratt p 199 , Figure 7.1 and p 200  (7.4)  !*/
/*!                                                                !*/
/*! The surface patch is defined by four boundary curves which     !*/
/*! are blended together with blending functions which are         !*/
/*! linear.                                                        !*/
/*!                                                                !*/
/*! This surface patch may have many segments in the boundary      !*/
/*! curves. The parameter of the curves may be the input           !*/
/*! curve parameters (u,v) but the relative arclength for          !*/
/*! curves may also be the parameter (s,t).                        !*/
/*!                                                                !*/
/*!                                                                !*/
/*!          p_r01                               p_r11             !*/
/*!         s=0,t=1                             s=1,t=1            !*/
/*!           _______________________________________              !*/
/*!          /          r(s,1)= rs1 --->             !             !*/
/*!         /   /                               !    !             !*/
/*!        / r(0,t)=                         r(1,t)= !             !*/
/*!       /  = r0t                           = r1t   !             !*/
/*!      /            r(s,0)= rs0 --->               !             !*/
/*!     /____________________________________________!             !*/
/*!    s=0,t=0                                   s=1,t=0           !*/
/*!     p_r00                                     p_r10            !*/
/*!                                                                !*/
/*!                       _      _                        _   _    !*/
/*!           _       _  ! _      !    _              _  !     !   !*/
/*! _        !         ! ! r(0,t) !   ! _       _      ! ! 1-t !   !*/
/*! r(s,t) = ! (1-s) s !*! _      ! + ! r(s,0)  r(s,1) !*!     ! - !*/
/*!          !_       _! ! r(1,t) !   !_              _! !  t  !   !*/
/*!                      !_      _!                      !_   _!   !*/
/*!                                                                !*/
/*!                         _              _   _   _               !*/
/*!                        ! _       _      ! !     !              !*/
/*!                        ! r(0,0)  r(0,1) ! ! 1-t !              !*/
/*!          - ! (1-s) s !*! _       _      !*!     !              !*/
/*!                        ! r(1,0)  r(1,1) ! !     !              !*/
/*!                        !_              _! !  t  !              !*/
/*!                                           !_   _!              !*/
/*!                                                                !*/
/*!                       _   _                  _   _             !*/
/*!           _       _  !     !    _        _  !     !            !*/
/*! _        !         ! ! r0t !   !          ! ! 1-t !            !*/
/*! r(s,t) = ! (1-s) s !*!     ! + ! rs0  rs1 !*!     !  -         !*/
/*!          !_       _! ! r1t !   !_        _! !  t  !            !*/
/*!                      !_   _!                !_   _!            !*/
/*!                                                                !*/
/*!                         _              _   _   _               !*/
/*!                        !                ! !     !              !*/
/*!                        ! p_r00   p_r01  ! ! 1-t !              !*/
/*!          - ! (1-s) s !*!                !*!     !              !*/
/*!                        ! p_r10   p_r11  ! !     !              !*/
/*!                        !_              _! !  t  !              !*/
/*!                                           !_   _!              !*/
/*!                                                                !*/
/*! x(s,t)= (1-s)*  x0t + s*x1t + xs0*(1-t) + xs1*t -              !*/
/*!                  - ((1-s)*p_x00 + s*p_x10)*(1-t) -             !*/
/*!                  - ((1-s)*p_x01 + s*p_x11)*t                   !*/
/*! y(s,t)= (1-s)*  y0t + s*y1t + ys0*(1-t) + ys1*t -              !*/
/*!                  - ((1-s)*p_y00 + s*p_y10)*(1-t)  -            !*/
/*!                  - ((1-s)*p_y01 + s*p_y11)*t                   !*/
/*! z(s,t)= (1-s)*  z0t + s*z1t + zs0*(1-t) + zs1*t -              !*/
/*!                  - ((1-s)*p_z00 + s*p_z10)*(1-t)  -            !*/
/*!                  - ((1-s)*p_z01 + s*p_z11)*t                   !*/
/*!                                                                !*/
/*!  where                                                         !*/
/*!  r(0,t) = r0t = (x0t,y0t,z0t)                                  !*/
/*!  r(1,t) = r1t = (x1t,y1t,z1t)                                  !*/
/*!  r(s,0) = rs0 = (xs0,ys0,zs0)                                  !*/
/*!  r(s,1) = rs1 = (xs1,ys1,zs1)                                  !*/
/*!  p_r00  = (p_x00,p_y00,p_z00)                                  !*/
/*!  p_r01  = (p_x01,p_y01,p_z01)                                  !*/
/*!  p_r10  = (p_x10,p_y10,p_z10)                                  !*/
/*!  p_r11  = (p_x11,p_y11,p_z11)                                  !*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*!   Differentiation of r(s,t) with respect to s:                 !*/
/*!                                                                !*/
/*!                    _   _                        _   _          !*/
/*!          _     _  !     !    _              _  !     !         !*/
/*!   _     !       ! ! r0t !   !                ! ! 1-t !         !*/
/*!  dr/ds= ! -1  1 !*!     ! + ! drs0/ds drs1/ds!*!     !  -      !*/
/*!         !_     _! ! r1t !   !_              _! !  t  !         !*/
/*!                   !_   _!                      !_   _!         !*/
/*!                                                                !*/
/*!                       _             _   _    _                 !*/
/*!             _     _  !               ! !     !                 !*/
/*!            !       ! ! p_r00  p_r01  ! ! 1-t !                 !*/
/*!          - ! -1  1 !*!               !*!     !                 !*/
/*!            !_     _! ! p_r10  p_r11  ! !     !                 !*/
/*!                      !_             _! !  t  !                 !*/
/*!                                        !_   _!                 !*/
/*!                                                                !*/
/*!  dr/ds= -r0t +r1t + drs0/ds*(1-t)+ drs1/ds*t-                  !*/
/*!                                                                !*/
/*!             (-p_r00+p_r10)*(1-t) - (-p_r01+p_r11)*t            !*/
/*!                                                                !*/
/*!                                                                !*/
/*!  dx/ds= -x0t +x1t + dxs0/ds*(1-t)+ dxs1/ds*t-                  !*/
/*!                                                                !*/
/*!             (-p_x00+p_x10)*(1-t) - (-p_x01+p_x11)*t            !*/
/*!                                                                !*/
/*!  dy/ds= -y0t +y1t + dys0/ds*(1-t)+ dys1/ds*t-                  !*/
/*!                                                                !*/
/*!             (-p_y00+p_y10)*(1-t) - (-p_y01+p_y11)*t            !*/
/*!                                                                !*/
/*!  dz/ds= -z0t +z1t + dzs0/ds*(1-t)+ dzs1/ds*t-                  !*/
/*!                                                                !*/
/*!             (-p_z00+p_z10)*(1-t) - (-p_z01+p_z11)*t            !*/
/*!                                                                !*/
/*!  where                                                         !*/
/*!  dr(s,0)/ds = drs0/ds = (dxs0/ds,dys0/ds,dzs0/ds)              !*/
/*!  dr(s,1)/ds = drs1/ds = (dxs1/ds,dys1/ds,dzs1/ds)              !*/
/*!                                                                !*/
/*!                                                                !*/
/*!   Differentiation of r(s,t) with respect to t:                 !*/
/*!                                                                !*/
/*!                     _      _                  _   _            !*/
/*!         _       _  !        !    _        _  !     !           !*/
/*!   _    !         ! ! dr0t/dt!   !          ! ! -1  !           !*/
/*!  dr/dt=! (1-s) s !*!        ! + ! rs0  rs1 !*!     !  -        !*/
/*!        !_       _! ! dr1t/dt!   !_        _! !  1  !           !*/
/*!                    !_      _!                !_   _!           !*/
/*!                                                                !*/
/*!                         _              _   _   _               !*/
/*!             _       _  !                ! !     !              !*/
/*!            !         ! ! p_r00   p_r01  ! ! -1  !              !*/
/*!          - ! (1-s) s !*!                !*!     !              !*/
/*!            !_       _! ! p_r10   p_r11  ! !     !              !*/
/*!                        !_              _! !  1  !              !*/
/*!                                           !_   _!              !*/
/*!   _                                                            !*/
/*!  dr/dt=(1-s)*dr0t/dt + s*dr1t/dt - rs0 + rs1  -                !*/
/*!                                                                !*/
/*!        ((1-s)*p_r00+s*p_r10)*(-1) - ((1-s)*p_r01+s*p_r11)      !*/
/*!                                                                !*/
/*!                                                                !*/
/*!   _                                                            !*/
/*!  dx/dt=(1-s)*dx0t/dt + S*dx1t/dt - xs0 + xs1  -                !*/
/*!                                                                !*/
/*!        ((1-s)*p_x00+s*p_x10)*(-1) - ((1-s)*p_x01+s*p_x11)      !*/
/*!                                                                !*/
/*!   _                                                            !*/
/*!  dy/dt=(1-s)*dy0t/dt + S*dy1t/dt - ys0 + ys1  -                !*/
/*!                                                                !*/
/*!        ((1-s)*p_y00+s*p_y10)*(-1) - ((1-s)*p_y01+s*p_y11)      !*/
/*!                                                                !*/
/*!   _                                                            !*/
/*!  dz/dt= (1-s)*dz0t/dt + S*dz1t/dt - zs0 + zs1  -               !*/
/*!                                                                !*/
/*!        ((1-s)*p_z00+s*p_z10)*(-1) - ((1-s)*p_z01+s*p_z11)      !*/
/*!                                                                !*/
/*!  where                                                         !*/
/*!  dr(0,t)/dt = dr0t/dt = (dx0t/dt,dy0t/dt,dz0t/dt)              !*/
/*!  dr(1,t)/dt = dr1t/dt = (dx1t/dt,dy1t/dt,dz1t/dt)              !*/
/*!                                                                !*/
/*!                                                                !*/
/*!   Differentiation of dr(s,t)/ds with respect to s:             !*/
/*!                                                                !*/
/*!                    _   _                              _   _    !*/
/*!            _   _  !     !    _                    _  !     !   !*/
/*!    _      !     ! ! r0t !   !                      ! ! 1-t !   !*/
/*!  d2r/ds2= ! 0 0 !*!     ! + ! d2rs0/ds2 d2rs1/ds2  !*!     ! - !*/
/*!           !_   _! ! r1t !   !_                    _! !  t  !   !*/
/*!                   !_   _!                            !_   _!   !*/
/*!                                                                !*/
/*!                       _             _   _    _                 !*/
/*!             _     _  !               ! !     !                 !*/
/*!            !       ! ! p_r00  p_r01  ! ! 1-t !                 !*/
/*!          - !  0  0 !*!               !*!     !                 !*/
/*!            !_     _! ! p_r10  p_r11  ! !     !                 !*/
/*!                      !_             _! !  t  !                 !*/
/*!                                        !_   _!                 !*/
/*!                                                                !*/
/*!  d2r/ds2=  dr2s0/ds2*(1-t)+ d2rs1/ds2*t                        !*/
/*!                                                                !*/
/*!                                                                !*/
/*!  d2x/ds2=  dx2s0/ds2*(1-t)+ d2xs1/ds2*t                        !*/
/*!  d2y/ds2=  dy2s0/ds2*(1-t)+ d2ys1/ds2*t                        !*/
/*!  d2y/ds2=  dy2s0/ds2*(1-t)+ d2ys1/ds2*t                        !*/
/*!                                                                !*/
/*!                                                                !*/
/*!   Differentiation of dr(s,t)/dt with respect to t:             !*/
/*!                                                                !*/
/*!                       _         _               _ _            !*/
/*!           _       _  !           !  _       _  !   !           !*/
/*!     _    !         ! ! d2r0t/dt2 ! !         ! ! 0 !           !*/
/*!  d2r/dt2=! (1-s) s !*!           !+! rs0 rs1 !*!   !  -        !*/
/*!          !_       _! ! d2r1t/dt2 ! !_       _! ! 0 !           !*/
/*!                      !_         _!             !_ _!           !*/
/*!                                                                !*/
/*!                         _              _   _   _               !*/
/*!             _       _  !                ! !     !              !*/
/*!            !         ! ! p_r00   p_r01  ! !  0  !              !*/
/*!          - ! (1-s) s !*!                !*!     !              !*/
/*!            !_       _! ! p_r10   p_r11  ! !     !              !*/
/*!                        !_              _! !  0  !              !*/
/*!                                           !_   _!              !*/
/*!    _                                                           !*/
/*!  d2r/dt2= (1-s)*d2r0t/dt2 + s*d2r1t/dt2                        !*/
/*!                                                                !*/
/*!  d2x/dt2= (1-s)*d2x0t/dt2 + s*d2x1t/dt2                        !*/
/*!  d2y/dt2= (1-s)*d2y0t/dt2 + s*d2y1t/dt2                        !*/
/*!  d2z/dt2= (1-s)*d2z0t/dt2 + s*d2z1t/dt2                        !*/
/*!                                                                !*/
/*!                                                                !*/
/*!   Differentiation of dr(s,t)/dt with respect to s:             !*/
/*!                                                                !*/
/*!                     _      _                  _   _            !*/
/*!            _     _  !        !    _        _  !     !          !*/
/*!    _      !       ! ! dr0t/dt!   !          ! ! -1  !          !*/
/*!  d2r/dsdt=! -1  1 !*!        ! + ! rs0  rs1 !*!     !  -       !*/
/*!           !_     _! ! dr1t/dt!   !_        _! !  1  !          !*/
/*!                     !_      _!                !_   _!          !*/
/*!                                                                !*/
/*!                         _              _   _   _               !*/
/*!             _       _  !                ! !     !              !*/
/*!            !         ! ! p_r00   p_r01  ! ! -1  !              !*/
/*!          - !   -1  1 !*!                !*!     !              !*/
/*!            !_       _! ! p_r10   p_r11  ! !     !              !*/
/*!                        !_              _! !  1  !              !*/
/*!                                           !_   _!              !*/
/*!   _                                                            !*/
/*!  d2r/dsdt= -dr0t/dt + dr1t/dt - drs0/dt + drs1/dt              !*/
/*!          - p_r00 + p_r10 + p_r01 - p_r11                       !*/
/*!                                                                !*/
/*!  d2x/dsdt= -dx0t/dt + dx1t/dt - dxs0/dt + dxs1/dt              !*/
/*!          - p_x00 + p_x10 + p_x01 - p_x11                       !*/
/*!                                                                !*/
/*!  d2y/dsdt= -dy0t/dt + dy1t/dt - dys0/dt + dys1/dt              !*/
/*!          - p_y00 + p_y10 + p_y01 - p_y11                       !*/
/*!                                                                !*/
/*!  d2z/dsdt= -dz0t/dt + dz1t/dt - dzs0/dt + dzs1/dt              !*/
/*!          - p_z00 + p_z10 + p_z01 - p_z11                       !*/
/*!                                                                !*/
/*!                                                                !*/

typedef struct
{
DBCurve cur_r0t;              /* Boundary curve r0t                  */
DBSeg *p_seg_r0t;            /* Curve segments for cur_r0t    (ptr) */
gmflt  s_u0t;                /* Start parameter value for cur_r0t   */
gmflt  e_u0t;                /* End   parameter value for cur_r0t   */
                             /* Reversed curve: e_u0t < s_u0t       */
DBCurve cur_r1t;              /* Boundary curve r1t                  */
DBSeg *p_seg_r1t;            /* Curve segments for cur_r1t    (ptr) */
gmflt  s_u1t;                /* Start parameter value for cur_r1t   */
gmflt  e_u1t;                /* End   parameter value for cur_r1t   */
                             /* Reversed curve: e_u1t < s_u1t       */
DBCurve cur_rs0;              /* Boundary curve r0s                  */
DBSeg *p_seg_rs0;            /* Curve segments for cur_rs0    (ptr) */
gmflt  s_u0s;                /* Start parameter value for cur_rs0   */
gmflt  e_u0s;                /* End   parameter value for cur_rs0   */
                             /* Reversed curve: e_u0s < s_u0s       */
DBCurve cur_rs1;              /* Boundary curve r1s                  */
DBSeg *p_seg_rs1;            /* Curve segments for cur_rs1    (ptr) */
gmflt  s_u1s;                /* Start parameter value for cur_rs1   */
gmflt  e_u1s;                /* End   parameter value for cur_rs1   */
                             /* Reversed curve: e_u1s < s_u1s       */

DBVector  p_r00;                /* Corner point            for s=0,t=0 */
DBVector  p_r01;                /* Corner point            for s=0,t=1 */
DBVector  p_r10;                /* Corner point            for s=1,t=0 */
DBVector  p_r11;                /* Corner point            for s=1,t=1 */

gmint  c_flag;               /* Case flag:                          */
                             /* Eq. 1: Only one segment curves      */
                             /*        Parameterized from 0 to 1    */
                             /* Eq. 2: Multi-segment curves.        */
                             /*        Relative arclengt param.     */
gmflt  ofs_pat;              /* Offset for the patch                */
} DBPatchB;
/*!*********************** DBPatchB *********************************!*/

/*!*********************** DBPatchN *********************************!*/
/*!                                                                !*/
/*! Consurf patch                                                  !*/
/*! -------------                                                  !*/
/*!                                                                !*/
/*! Refer to Alan Ball's paper describing Consurf                  !*/
/*!                                                                !*/
/*! An offset value can be defined for the patch.                  !*/

typedef struct
{
DBVector  v1;                   /* Tile corner point 1                 */
gmflt  w11;                  /* Weight for v1                       */
gmflt  t11;                  /* P value function value              */
DBVector  v2;                   /* Tile corner point 2                 */
gmflt  w14;                  /* Weight for v2                       */
gmflt  t14;                  /* P value function value              */
DBVector  u1;                   /* Cross slope defining point for v1   */
gmflt  w12;                  /* Weight for u1                       */
gmflt  t12;                  /* P value function value              */
DBVector  u2;                   /* Cross slope defining point for v2   */
gmflt  w13;                  /* Weight for u2                       */
gmflt  t13;                  /* P value function value              */
DBVector  dv1;                  /* Fore/aft tangent point for v1       */
gmflt  w21;                  /* Weight for dv1                      */
gmflt  t21;                  /* P value function value              */
DBVector  dv2;                  /* Fore/aft tangent point for v2       */
gmflt  w24;                  /* Weight for dv2                      */
gmflt  t24;                  /* P value function value              */
DBVector  du1;                  /* Fore/aft tangent point for u1       */
gmflt  w22;                  /* Weight for du1                      */
gmflt  t22;                  /* P value function value              */
DBVector  du2;                  /* Fore/aft tangent point for u2       */
gmflt  w23;                  /* Weight for du2                      */
gmflt  t23;                  /* P value function value              */
DBVector  v3;                   /* Tile corner point 3                 */
gmflt  w31;                  /* Weight for v3                       */
gmflt  t31;                  /* P value function value              */
DBVector  v4;                   /* Tile corner point 4                 */
gmflt  w34;                  /* Weight for v4                       */
gmflt  t34;                  /* P value function value              */
DBVector  u3;                   /* Cross slope defining point for v3   */
gmflt  w32;                  /* Weight for u3                       */
gmflt  t32;                  /* P value function value              */
DBVector  u4;                   /* Cross slope defining point for v4   */
gmflt  w33;                  /* Weight for u4                       */
gmflt  t33;                  /* P value function value              */
DBVector  dv3;                  /* Fore/aft tangent point for v3       */
gmflt  w41;                  /* Weight for dv3                      */
gmflt  t41;                  /* P value function value              */
DBVector  dv4;                  /* Fore/aft tangent point for v4       */
gmflt  w44;                  /* Weight for dv4                      */
gmflt  t44;                  /* P value function value              */
DBVector  du3;                  /* Fore/aft tangent point for u3       */
gmflt  w42;                  /* Weight for du3                      */
gmflt  t42;                  /* P value function value              */
DBVector  du4;                  /* Fore/aft tangent point for u4       */
gmflt  w43;                  /* Weight for du4                      */
gmflt  t43;                  /* P value function value              */
gmflt  lambda_0;             /* Lambda  U=  0                       */
gmflt  lambda_13;            /* Lambda  U= 1/3                      */
gmflt  lambda_23;            /* Lambda  U= 2/3                      */
gmflt  lambda_1;             /* Lambda  U=  1                       */
gmflt  mu_0;                 /* Mu      U=  0                       */
gmflt  mu_13;                /* Mu      U= 1/3                      */
gmflt  mu_23;                /* Mu      U= 2/3                      */
gmflt  mu_1;                 /* Mu      U=  1                       */
gmflt  dlambda_0;            /* dLambda/dU   U=  0                  */
gmflt  dlambda_13;           /* dLambda/dU   U= 1/3                 */
gmflt  dlambda_23;           /* dLambda/dU   U= 2/3                 */
gmflt  dlambda_1;            /* dLambda/dU   U=  1                  */
gmflt  dmu_0;                /* dMu/dU       U=  0                  */
gmflt  dmu_13;               /* dMu/dU       U= 1/3                 */
gmflt  dmu_23;               /* dMu/dU       U= 2/3                 */
gmflt  dmu_1;                /* dMu/dU       U=  1                  */
gmflt  p1;                   /* Flag 1                              */
gmflt  p2;                   /* Flag 2                              */
gmflt  p3;                   /* Flag 3                              */
DBVector  cl_p;                 /* Core Line Point                     */
DBVector  cl_v;                 /* Core Line Vector                    */
gmflt ofs_pat;               /* Offset for the patch                */
} DBPatchN;
/*!*********************** DBPatchN *********************************!*/

/*!*********************** DBPatchNU ********************************!*/
/*!                                                                !*/
/*! NURBS patch (surface)                                          !*/
/*! ---------------------                                          !*/
/*!                                                                !*/
/*!                                                                !*/

typedef struct
{
gmint   order_u;              /* Ordning i U-led                    */
gmint   order_v;              /* Ordning i V-led                    */
gmint   nk_u;                 /* Antal knutpunkter i U-led          */
gmint   nk_v;                 /* Antal knutpunkter i V-led          */
gmflt  *kvec_u;               /* C-pekare till U-knutvektor         */
gmflt  *kvec_v;               /* C-pekare till V-knutvektor         */
DBHvector *cpts;                 /* C-pekare till kontrollpunkter      */
DBptr   pku;                  /* GM-pekare till U-knutvektor        */
DBptr   pkv;                  /* GM-pekare till V-knutvektor        */
DBptr   pcpts;                /* GM-pekare till kontrollpunkter     */
gmflt ofs_pat;                /* Offset for the patch               */
} DBPatchNU;
/*!*********************** DBPatchNU ********************************!*/

/************************* DBPatchBR3 *********************************/
/*!                                                                !*/
/*! A rational Bezier cubic patch                                  !*/
/*! -----------------------------                                  !*/
/*!                                                                !*/
/*!                           ! _   _   _   _   !    ! 1  !        !*/
/*!                           ! r00 r01 r02 r03 !    !    !        !*/
/*!                           ! _   _   _   _   !    ! v  !        !*/
/*!                  2  3     ! r10 r11 r12 r13 !  T !    !        !*/
/*! R(u,v)= ( 1  u  u  u  ) M ! _   _   _   _   ! M  !  2 !        !*/
/*!                           ! r20 r21 r22 r23 !    ! v  !        !*/
/*!                           ! _   _   _   _   !    !    !        !*/
/*!                           ! r30 r31 r32 r33 !    !  3 !        !*/
/*!                           !                 !    ! v  !        !*/
/*!                                                                !*/
/*!                                                                !*/
/*!       !  1  0  0  0  !                                         !*/
/*!       !              !                                         !*/
/*!       !  0  0  0  1  !                                         !*/
/*!  M =  !              !                                         !*/
/*!       ! -3  3  0  0  !                                         !*/
/*!       !              !                                         !*/
/*!       !  0  0 -3  3  !                                         !*/
/*!                                                                !*/
/*!                                                                !*/
/*!                                                                !*/
/*! The number of coefficients per patch is 65.                    !*/
/*! An offset value can be defined for the patch.                  !*/

typedef struct
{
gmflt r00x,r01x,r02x,r03x;
gmflt r10x,r11x,r12x,r13x;
gmflt r20x,r21x,r22x,r23x;
gmflt r30x,r31x,r32x,r33x;
gmflt r00y,r01y,r02y,r03y;
gmflt r10y,r11y,r12y,r13y;
gmflt r20y,r21y,r22y,r23y;
gmflt r30y,r31y,r32y,r33y;
gmflt r00z,r01z,r02z,r03z;
gmflt r10z,r11z,r12z,r13z;
gmflt r20z,r21z,r22z,r23z;
gmflt r30z,r31z,r32z,r33z;
gmflt r00 ,r01 ,r02 ,r03 ;
gmflt r10 ,r11 ,r12 ,r13 ;
gmflt r20 ,r21 ,r22 ,r23 ;
gmflt r30 ,r31 ,r32 ,r33 ;
gmflt ofs_pat;               /* Offset for the patch         */
} DBPatchBR3;
/************************* DBPatchBR3 *********************************/

/*!*********************** DBPatchX ***********************************/
/*                                                                  */
/* The union of all other patches                                   */
/* ------------------------------                                   */
/*                                                                  */
/* To be used when allocating a variable or dynamic memory big      */
/* enough to hold any type of patch.                                */
/*                                                                  */

typedef union
{
DBPatch    toppat;
DBPatchC   cubpat;
DBPatchR   ratpat;
DBPatchL   lftpat;
DBPatchF   facpat;
DBPatchP   propat;
DBPatchB   blnpat;
DBPatchN   conpat; 
DBPatchP3  p3pat;
DBPatchP5  p5pat;
DBPatchP7  p7pat;
DBPatchP9  p9pat;
DBPatchP21 p21pat;
DBPatchNU  nurbpat;
} DBPatchX;
/*!*********************** DBPatchX ***********************************/

/*!*********************** EVALS **********************************!*/
/*!                                                                !*/
/*! Evaluated coordinates and derivatives for a point on a surface !*/
/*! -------------------------------------------------------------- !*/
/*!                                                                !*/
/*!                                                                !*/

typedef struct
{
gmflt r_x,r_y,r_z;                /* Coordinates          r(u)      */
gmflt u_x,u_y,u_z;                /* Tangent             dr/du      */
gmflt v_x,v_y,v_z;                /* Tangent             dr/dv      */
gmflt u2_x,u2_y,u2_z;             /* Second derivative  d2r/du2     */
gmflt v2_x,v2_y,v2_z;             /* Second derivative  d2r/dv2     */
gmflt uv_x,uv_y,uv_z;             /* Twist vector       d2r/dudv    */
gmflt n_x,n_y,n_z;                /* Surface normal       r(u)      */
gmflt nu_x,nu_y,nu_z;             /* Normal derivative   dn/du      */
gmflt nv_x,nv_y,nv_z;             /* Normal derivative   dn/dv      */
gmflt kmax;                       /* Maximum principal curvature    */
gmflt kmin;                       /* Minimum principal curvature    */
gmflt kmax_x,kmax_y,kmax_z;       /* Direction in R*3 for kmax      */
gmflt kmin_x,kmin_y,kmin_z;       /* Direction in R*3 for kmin      */
gmflt kmax_u,kmax_v;              /* Direction in u,v for kmax      */
gmflt kmin_u,kmin_v;              /* Direction in u,v for kmin      */
gmflt u,v;                        /* Parameter value on surface     */
gmflt offset;                     /* Offset value for   surface     */
gmint e_case;                     /* Evaluation case                */
                                  /* Eq. 0: Coordinates only        */
                                  /* Eq. 1: First derivatives added */
                                  /* Eq.  :                         */
 
gmflt sp_x,sp_y,sp_z;             /* Coordinates       for spine    */
gmflt spt_x,spt_y,spt_z;          /* Tangent           for spine    */
gmflt spt2_x,spt2_y,spt2_z;       /* Second derivative for spine    */

bool  s_anal;                     /* Eq. TRUE: Analysis is made     */
                                  /* (Parameters below have values) */
bool  cubpat;                     /* Eq. TRUE: Cubic    patch    pt */
bool  ratpat;                     /* Eq. TRUE: Rational patch    pt */
bool  lftpat;                     /* Eq. TRUE: Lofting  patch    pt */
bool  offpat;                     /* Eq. TRUE: Offset   patch    pt */
bool  planar;                     /* Eq. TRUE: Planar            pt */
bool  sphere;                     /* Eq. TRUE: Spherical         pt */
bool  devel;                      /* Eq. TRUE: Developable       pt */
bool  saddle;                     /* Eq. TRUE: Saddle            pt */
int   ordnum;                     /* Surface number, used for       */
                                  /* ordering of solutions from     */
                                  /* multiple surfaces              */
                                  /* Also for ordering ....         */
DBSurf *p_sur;                    /* Surface                  (ptr) */
DBPatch *p_pat;                   /* Patches for p_sur        (ptr) */
} EVALS;
/*!*********************** EVALS **********************************!*/

/*!*********************** EVALC **********************************!*/
/*!                                                                !*/
/*! Evaluated coordinates and derivatives for a point on a curve   !*/
/*! -------------------------------------------------------------- !*/
/*!                                                                !*/
/*!                                                                !*/

/*! Koder för olika typer av evalueringar                          !*/

#define EVC_R       1             /* Position */
#define EVC_DR      2             /* 1:a derivata */
#define EVC_D2R     4             /* 2:a derivata */
#define EVC_PN      8             /* Principalnormal */
#define EVC_BN     16             /* Binormal */
#define EVC_KAP    32             /* Kr|kning */
#define EVC_NOR    64             /* Normal kr|kning */
#define EVC_GEO   128             /* Geodesisk kr|kning */
#define EVC_TOR   256             /* Torsion            */

/*! Interfacestruktur för kurvevaluering                           !*/

typedef struct
{
gmint evltyp;                     /* Typ av evaluering              */
gmflt t_local;                    /* Lokalt parameterv{rde          */
gmflt t_global;                   /* Globalt parameterv{rde         */
DBVector r;                          /* Position                       */
DBVector drdt;                       /* 1:a derivata                   */
DBVector d2rdt2;                     /* 2:a derivata                   */
DBVector p_norm;                     /* Principalnormal                */
DBVector b_norm;                     /* Binormal                       */
gmflt kappa;                      /* Kr|kning                       */
gmflt nkappa;                     /* Normal kr|kning                */
gmflt geodesic;                   /* Tangential kr|kning            */
gmflt torsion;                    /* Torsion                        */

gmint iseg;                       /* Kurv segment nummer            */

                                  /* For a surface curve u(t),v(t): */
gmflt u,v;                        /* Parameter point  u,v           */
gmflt u_t,v_t;                    /* Derivative   du/dt,dv/dt       */
gmflt u_t2,v_t2;                  /* Derivative d2u/dt2,dv2/dt2     */

gmflt u_t2_geod0,v_t2_geod0;      /* Derivative for geodesic= 0     */



gmflt u_global;       /* Ta bort*//* Curve parameter value (input)  */

gmflt r_x,r_y,r_z;    /* Ta bort*//* Coordinates          r(u)      */
gmflt u_x,u_y,u_z;    /* Ta bort*//* Tangent             dr/du      */
gmflt u2_x,u2_y,u2_z; /* Ta bort*//* Second derivative  d2r/du2     */
gmflt pn_x,pn_y,pn_z; /* Ta bort*//* Principal normal               */
gmflt bn_x,bn_y,bn_z; /* Ta bort*//* Binormal                       */

gmflt offset;                     /* Offset value for curve segment */
gmint e_case;         /* Ta bort*//* Evaluation case                */
                                  /* Eq. 0: Coordinates only        */
                                  /* Eq. 1: First derivatives added */
                                  /* Eq.  :                         */

bool  c_anal;                     /* Eq. TRUE: Analysis is made     */
                                  /* (Parametrs below have values)  */
bool  ratseg;                     /* Eq. TRUE: Rational segment  pt */
bool  cubseg;                     /* Eq. TRUE: Cubic    segment  pt */
bool  offseg;                     /* Eq. TRUE: Offset   segment  pt */
bool  surpat;                     /* Eq. TRUE: Surface  patch    pt */
bool  straight;                   /* Eq. TRUE: Curve is line     pt */
} EVALC;
/*!*********************** EVALC **********************************!*/

/*!*********************** EVALCH *********************************!*/
/*!                                                                !*/
/*! Homogenous coordinates and derivatives for a point on a curve  !*/
/*! -------------------------------------------------------------- !*/
/*!                                                                !*/
/*!                                                                !*/

typedef struct
{
gmflt R_X,R_Y,R_Z,R_w;            /* Coordinates          R(u)      */
gmflt U_X,U_Y,U_Z,U_w;            /* First derivative    dR/du      */
gmflt U2_X,U2_Y,U2_Z,U2_w;        /* Second derivative  d2R /du2    */
} EVALCH;
/*!*********************** EVALCH *********************************!*/
