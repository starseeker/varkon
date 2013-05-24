/**********************************************************************
*
*    DB.h
*    ====
*
*    This file is part of the VARKON Database Library.
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
*    Author: Johan Kjellander, Örebro university, Sweden
*
*    Latest change: Mesh added, 040703, J. Kjellander
*
***********************************************************************/

/*
***First of all define the current version of the DB library.
***These constants are used to ensure that the DB is not called
***by code that is not compatible with the current DB and that
***DB files are not loaded that are created by later versions
***of the DB.
***
***DBlib level 16F = 32 bit pagenumbers
***DBlib level 18A = 32 bit sequence- and ordernumbers
*/
#define DB_LIBVERSION    1     /* DBlib version */
#define DB_LIBREVISION  18     /* DBlib revision */
#define DB_LIBLEVEL     'A'    /* DBlib level 16F = 32 bit pagenumbers */

/*
***Now we need to define the size of some
***basic datatypes. These definitions may have to be
***changed when the system is ported to a new platform.
*/
typedef          char   DBchar;    /* Always 8-bit  signed char */
typedef          short  DBshort;   /* Always 16-bit signed integer */
typedef unsigned short  DBushort;  /* Always 16-bit unsigned int */
#ifdef _CRAYT3E
typedef          short  DBint;     /* Always 32-bit signed integer */
#else
typedef          int    DBint;     /* Always 32-bit signed integer */
#endif
typedef unsigned int    DBuint;    /* Always 32-bit unsigned int */
typedef          double DBfloat;   /* Always 64-bit float */

/*
***Now some basic DB datatypes.
***Added #ifdef for bool in C++ 13/8 2000 JK.
*/

typedef DBint   DBptr;        /* A DB virtual address (pointer) may be < 0 */
#define DBNULL (DBptr)0       /* A DB NULL pointer */
typedef DBshort DBstatus;     /* A DB function return value */
typedef DBint   DBseqnum;     /* A DB ID sequence value */
typedef DBint   DBordnum;     /* A DB ID order value */
#ifndef __cplusplus
typedef DBint   bool;         /* A boolean value */
#endif
typedef DBchar  tbool;        /* A tiny boolean */
typedef DBint   DBpagnum;     /* A page number, changed to 32 bit 020222, JK */

/*
***Some of the DB types depend on Varkon types defined in
***common.h.
*/
#include "common.h"

/*
***Surface graphical NURBS representations are based on datatypes
***defined in OpenGL. On unix systems gl.h is included explicitly.
***On WIN32 platforms gl.h is included by windows.h
*/
#ifdef UNIX
#include <GL/gl.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

/*
***A DB entity ID (Same as V2REFVA in common.h).
*/

typedef V2REFVA DBId;

/*
***VARKON entity typecodes.
*/

typedef DBuint DBetype;       /* A GM entity type, POITYP, LINTYP etc.. short->int 040720, JK */

#define TRFTYP (DBetype)0     /* Transformation */
#define POITYP (DBetype)1     /* Point */
#define LINTYP (DBetype)2     /* Line */
#define ARCTYP (DBetype)4     /* Arc */
#define CURTYP (DBetype)8     /* Curve */
#define SURTYP (DBetype)16    /* Surface */                             
#define TXTTYP (DBetype)32    /* Text */
#define XHTTYP (DBetype)64    /* Hatch */
#define LDMTYP (DBetype)128   /* Linear dimension */
#define CDMTYP (DBetype)256   /* Diameter dimension */
#define RDMTYP (DBetype)512   /* Radius dimension */
#define ADMTYP (DBetype)1024  /* Angular dimension */
#define MSHTYP (DBetype)2048  /* Mesh */
#define GRPTYP (DBetype)4096  /* Group */
#define CSYTYP (DBetype)8192  /* Coordinatesystem */
#define PRTTYP (DBetype)16384 /* Part */
#define BPLTYP (DBetype)32768 /* Bplane */
#define ALLTYP (DBetype)65535 /* Mask for all types */

/*
***A 2D/3D-vector (Compatible with V2VECVA in common.h).
*/

typedef struct
{
DBfloat x_gm;                 /* X or U coordinate */
DBfloat y_gm;                 /* Y or V coordiante */
DBfloat z_gm;                 /* Optional Z coordinate */
} DBVector;

/*
***A 4D-vector.
*/

typedef struct
{
DBfloat x_gm;                 /* X-coordinate */
DBfloat y_gm;                 /* Y-coordinate */
DBfloat z_gm;                 /* Z-coordinate */
DBfloat w_gm;                 /* W-coordinate */
} DBHvector;

/*
***A 4X4 Transformation matrix.
*/

typedef struct
{
DBfloat g11,g12,g13,g14;      /* First  row */
DBfloat g21,g22,g23,g24;      /* Second row */
DBfloat g31,g32,g33,g34;      /* Third  row */
DBfloat g41,g42,g43,g44;      /* Last   row */
} DBTmat;

/*
***A mesh vertex.
*/
typedef struct
{
DBVector  p;                  /* Vertex position */
DBint     i_ehedge;           /* Emanating edge index */
} DBVertex;

/*
***A mesh (half)edge.
*/
typedef struct
{
DBint   i_evertex;            /* End vertex index */
DBint   i_ohedge;             /* Opposite halfedge index */
DBint   i_nhedge;             /* Next halfedge index */
DBint   i_pface;              /* Parent face index */
} DBHedge;

/*
***A mesh face.
*/
typedef struct
{
DBint   i_hedge;              /* Edge index */
} DBFace;

/*
***A bounding box.
*/
typedef struct
{
DBfloat xmin;                 /* Minimum x value */
DBfloat ymin;                 /* Minimum y value */
DBfloat zmin;                 /* Minimum z value */
DBfloat xmax;                 /* Maximum x value */
DBfloat ymax;                 /* Maximum y value */
DBfloat zmax;                 /* Maximum z value */
} DBBbox;

/*
***A curve segment.
*/

#define CUB_SEG      1          /* Rational cubic (offset-)segment */
#define UV_CUB_SEG   2          /* Rational cubic segment on a surface */
#define NURB_SEG     3          /* NURBS-span */
#define UV_NURB_SEG  4          /* NURBS-span on a surface */

typedef struct
{
DBfloat    c0x,c0y,c0z,c0;    /* Segment-coefficients. */
DBfloat    c1x,c1y,c1z,c1;    /* UV_CUB_SEG only use 12. */
DBfloat    c2x,c2y,c2z,c2;    /* NURB_SEG and UV_NURB_SEG use none */
DBfloat    c3x,c3y,c3z,c3;
DBptr      nxt_seg;           /* DB-Pointer to next segment */
DBfloat    ofs;               /* Optional offset */
DBfloat    sl;                /* Arclength in R3 */
DBshort    typ;               /* Segment type CUB_SEG, UV_CUB_SEG or NURB_SEG */
DBshort    subtyp;            /* Sub type */
DBptr      spek_gm;           /* Optional DB-pointer to surface */
DBptr      spek2_gm;          /* Optional DB-pointer to surface */
DBptr      cpts_db;           /* DB-pointer to 4D controlpoints */
DBHvector *cpts_c;            /* C-pointer to 4D controlpoints */
DBint      ncpts;             /* Number of cpts */
DBint      offset_cpts;       /* Offset for this span */
DBptr      knots_db;          /* DB-pointer to knotvector */
DBfloat   *knots_c;           /* C-pointer to knotvector */
DBint      nknots;            /* Number of knots */
DBint      offset_knots;      /* Offset for this span */
DBshort    nurbs_degree;      /* Degree (order-1) for NURB span */
} DBSeg;

/*
***An array of curve segments.
*/

typedef struct
{
DBint      nseg;             /* Number of segments in array */
DBptr      segptr_db;        /* DBptr to array of segments */
DBSeg     *segptr_c;         /* C-ptr to array of segments */
} DBSegarr;

/*
***A graphical NURBS surface trim curve.
*/

typedef struct
{
DBint      order;            /* Curve NURBS order */
DBint      nknots;           /* Number of knots */
DBptr      knots_db;         /* DBptr to array of knots */
GLfloat   *knots_c;          /* C-ptr to array of knots */
GLenum     vertextype;       /* GL_MAP2_VERTEX_3 or GLMAP2_VERTEX_4 */
DBptr      cpts_db;          /* DBptr to array of control points */
GLfloat   *cpts_c;           /* C-ptr to array of control points */
} DBGrstrimcurve;

/*
***All Varkon entities have a common entity record header.
*/

typedef struct
{
DBetype  type;               /* Entity type */
DBptr    p_ptr;              /* Pointer to parent part */
DBptr    n_ptr;              /* Pointer to previous instance */
DBseqnum seknr;              /* Sequence number */
DBordnum ordnr;              /* Order number */
DBshort  vers;               /* Version number */
DBptr    g_ptr[3];           /* Group pointers */
tbool    blank;              /* Visible TRUE/FALSE */
tbool    hit;                /* Hitable TRUE/FALSE */
DBshort  pen;                /* Pen/Color number */
DBshort  level;              /* Level/Layor number */
} DBHeader;

/*
***And now the actual Varkon native entities.
***A Transformation, Type = 0.
*/

typedef struct
{
DBHeader  hed_tf;            /* Header */
DBTmat    mat_tf;            /* 4X4 Transformation matrix */
DBptr     pcsy_tf;           /* DB-pointer to coordinate system */
} DBTform;

/*
***A 2D or 3D point. Type = 1.
*/

typedef struct
{
DBHeader   hed_p;            /* Header */
DBVector   crd_p;            /* Coordinates */
DBptr      pcsy_p;           /* DB-pointer to coordinate system */
DBfloat    wdt_p;            /* Linewidth */
} DBPoint;

/*
***A 2D or 3D line. Type = 2.
*/

typedef struct
{
DBHeader hed_l;              /* Header */
DBshort  fnt_l;              /* Font */
DBfloat  lgt_l;              /* Dash length */
DBVector crd1_l;             /* Start position */
DBVector crd2_l;             /* End position */
DBptr    pcsy_l;             /* DB-pointer to coordinatesystem */
DBfloat  wdt_l;              /* Linewidth */
} DBLine;

/*
***A 2D or 3D Circle/Arc. Type = 4.
*/

typedef struct
{
DBHeader hed_a;              /* Header */
DBshort  fnt_a;              /* Font */
DBfloat  lgt_a;              /* Dash length */
DBfloat  x_a;                /* Centre X-coordinate */
DBfloat  y_a;                /* Centre Y-coordinate */
DBfloat  r_a;                /* Radius */
DBfloat  v1_a;               /* Start angle */
DBfloat  v2_a;               /* End angle */
DBfloat  al_a;               /* Arclength in R3 */
DBshort  ns_a;               /* Number of segments, maximum 4 */
DBptr    sptr_a;             /* DB-pointer to 1:st segment */
DBptr    pcsy_a;             /* DB-pointer to coordinatesystem */
DBfloat  wdt_a;              /* Linewidth */
} DBArc;

/*
***A 2D or 3D curve. Type = 8.
*/

typedef struct
{
DBHeader hed_cu;             /* Header */
DBshort  fnt_cu;             /* Font */
DBfloat  lgt_cu;             /* Dash length */
DBfloat  al_cu;              /* Arclength in R3 */
DBshort  ns_cu;              /* Number of geometric segments */
DBTmat   csy_cu;             /* Optional curve plane */
tbool    plank_cu;           /* TRUE if curve is planar */
DBptr    pcsy_cu;            /* DB-pointer to coordinate system */
DBshort  nsgr_cu;            /* Number of graphical segments */
DBfloat  wdt_cu;             /* Linewidth */
} DBCurve;

/*
***A surface. Type = 16. Most of the surface related
***datatypes are defined in surdef.h
*/

#include "surdef.h"

/*
***A 2D or 3D Text. Type = 32.
*/

typedef struct
{
DBHeader hed_tx;             /* Header */
DBVector crd_tx;             /* Start position */
DBshort  fnt_tx;             /* Font */
DBfloat  h_tx;               /* Text height */
DBfloat  b_tx;               /* Text width */
DBfloat  l_tx;               /* Text slant */
DBfloat  v_tx;               /* Text direction */
DBshort  nt_tx;              /* Number of characters */
DBptr    tptr_tx;            /* DB-pointer to text string */
DBptr    pcsy_tx;            /* DB-pointer to coordinate system */
DBfloat  wdt_tx;             /* Linewidth */
DBshort  pmod_tx;            /* 0=2D, 1=3D screen mode, 2=3D true */
DBVector lpos_tx;            /* Position in local system */
DBfloat  lang_tx;            /* Angle in local system */
} DBText;

/*
***A 2D hatch. Type = 64.
*/

#define GMXMXL 1000          /* Max number of hatchlines in one DBHatch */

typedef struct
{
DBHeader hed_xh;             /* Header */
DBshort  fnt_xh;             /* Font */
DBfloat  lgt_xh;             /* Dash length */
DBfloat  dist_xh;            /* Distance between lines */
DBfloat  ang_xh;             /* Line angle */
DBshort  nlin_xh;            /* Number of lines, max GMXMXL */
DBptr    lptr_xh;            /* DB-pointer to lines */
DBptr    pcsy_xh;            /* DB-pointer to coordinate system */
} DBHatch;

/*
***A 2D Linear dimension. Type = 128.
*/

typedef struct
{
DBHeader hed_ld;             /* Header */
DBVector p1_ld;              /* Position 1 */
DBVector p2_ld;              /* Position 2 */
DBVector p3_ld;              /* Position 3 */
DBshort  dtyp_ld;            /* Type, horizontal/vertical/parallell */
DBfloat  asiz_ld;            /* Arrow size */
DBfloat  tsiz_ld;            /* Text size */
DBshort  ndig_ld;            /* Number of decimals */
tbool    auto_ld;            /* Auto text TRUE/FALSE */
DBptr    pcsy_ld;            /* DB-pointer to coordinate system */
} DBLdim;

/*
***A 2D Circular dimension. Type = 256.
*/

typedef struct
{
DBHeader hed_cd;             /* Header */
DBVector p1_cd;              /* Position 1 */
DBVector p2_cd;              /* Position 2 */
DBVector p3_cd;              /* Position 3 */
DBshort  dtyp_cd;            /* Type, horizontal/vertical/parallell */
DBfloat  asiz_cd;            /* Arrow size */
DBfloat  tsiz_cd;            /* Text size */
DBshort  ndig_cd;            /* Number of decimals */
tbool    auto_cd;            /* Auto text TRUE/FALSE */
DBptr    pcsy_cd;            /* DB-pointer to coordinate system */
} DBCdim;

/*
***A 2D Radius dimension. Type = 512.
*/

typedef struct
{
DBHeader hed_rd;             /* Header */
DBVector p1_rd;              /* Position 1 */
DBVector p2_rd;              /* Position 2 */
DBVector p3_rd;              /* Position 3 */
DBfloat  r_rd;               /* Radius */
DBfloat  asiz_rd;            /* Arrow size */
DBfloat  tsiz_rd;            /* Text size */
DBshort  ndig_rd;            /* Number of decimals */
tbool    auto_rd;            /* Auto text TRUE/FALSE */
DBptr    pcsy_rd;            /* DB-pointer to coordinate system */
} DBRdim;

/*
***An 2D Angular dimension. Type = 1024.
*/

typedef struct
{
DBHeader hed_ad;             /* Header */
DBVector pos_ad;             /* Position */
DBfloat  r_ad;               /* Radius */
DBfloat  v1_ad;              /* Start angle */
DBfloat  r1_ad;              /* Start radius */
DBfloat  v2_ad;              /* End angle */
DBfloat  r2_ad;              /* End radius */
DBfloat  tv_ad;              /* Text direction */
DBfloat  asiz_ad;            /* Arrow size */
DBfloat  tsiz_ad;            /* Text size */
DBshort  ndig_ad;            /* Number of decimals */
tbool    auto_ad;            /* Auto text TRUE/FALSE */
DBptr    pcsy_ad;            /* DB-pointer to coordinate system */
} DBAdim;

/*
***A Mesh. Type = 2048.
*/
#define MESH_HEADER  0
#define MESH_VERTEX  1
#define MESH_HEDGE   2
#define MESH_FACE    4
#define MESH_XFLOAT  8
#define MESH_ALL     1+2+4

typedef struct
{
DBHeader  hed_m;             /* Header */
DBint     font_m;            /* Graphical representation */
DBint     nv_m;              /* Number of vertices */
DBVertex *pv_m;              /* C-pointer to array of vertices */
DBptr     pvarr_m;           /* DB-pointer to array of vertices */
DBint     nh_m;              /* Number of halfedges */
DBHedge  *ph_m;              /* C-Pointer to array of halfedges */
DBptr     pharr_m;           /* DB-pointer to array of halfedges */
DBint     nf_m;              /* Number of faces */
DBFace   *pf_m;              /* C-pointer to array faces */
DBptr     pfarr_m;           /* DB-pointer to array of faces */
DBint     nx_m;              /* Number of extra floats */
DBfloat  *px_m;              /* C-pointer to array extra floats */
DBptr     pxarr_m;           /* DB-pointer to array of extra floats */
DBBbox    bbox_m;            /* Mesh bounding box */
DBptr     pcsy_m;            /* DB-pointer to coordinate system */
DBfloat   wdt_m;             /* Linewidth */
} DBMesh;

/*
***A Group. Type = 4096.
*/

#define GMMXGP 100           /* Max number of members */

typedef struct
{
DBHeader hed_gp;             /* Header */
char     name_gp[JNLGTH+1];  /* Name + '\0' */
DBshort  nmbr_gp;            /* Number of members, max GMMXGP */
DBptr    mptr_gp;            /* DB-pointer to member table */
} DBGroup;

/*
***A 2D or 3D coordinate system. Type = 8192.
*/

typedef struct
{
DBHeader hed_pl;             /* Header */
char     name_pl[JNLGTH+1];  /* Name + '\0' */
DBTmat   mat_pl;             /* Matrix */
DBptr    pcsy_pl;            /* DB-pointer to coordinate system */
} DBCsys;

/*
***A Part. Type = 16384.
*/

typedef struct
{
DBHeader hed_pt;             /* Header */
char     name_pt[JNLGTH+1];  /* Name + '\0' */
DBshort  dummy_pt;           /* Not used after V1.2 */
DBshort  dts_pt;             /* sizeof(GMPDAT) */
DBptr    dtp_pt;             /* DB-pointer to part-data */
DBptr    itp_pt;             /* DB-pointer to index table */
DBseqnum its_pt;             /* Size of indextable */
} DBPart;

typedef struct
{
DBshort mtyp_pd;             /* Module type, 2D/3D */
DBshort matt_pd;             /* Module attribute */
DBptr   csp_pd;              /* DB-pointer to coordinate system */
DBshort npar_pd;             /* Number of parameters */
DBptr   typp_pd;             /* DB-pointer to type array */
DBptr   valp_pd;             /* DB-pointer to value array */
DBshort vsiz_pd;             /* Size of value array */
DBptr   attp_pd;             /* DB-pointer to graphical attributes */
DBshort asiz_pd;             /* Size of graphical attributes */
} DBPdat;

/*
***A 2D or 3D Bounded plane. Type = 32768.
*/

typedef struct
{
DBHeader hed_bp;             /* Header */
DBVector crd1_bp;            /* Vertex 1 */
DBVector crd2_bp;            /* Vertex 2 */
DBVector crd3_bp;            /* Vertex 3 */
DBVector crd4_bp;            /* Vertex 4 */
DBptr    pcsy_bp;            /* DB-pointer to coordinate system */
DBfloat  wdt_bp;             /* Linewidth */
} DBBplane;

/*
***Any entity.
*/

typedef union
{
DBHeader hed_un;
DBTform  trf_un;
DBPoint  poi_un;
DBLine   lin_un;
DBArc    arc_un;
DBCurve  cur_un;
DBSurf   sur_un;
DBText   txt_un;
DBHatch  xht_un;
DBLdim   ldm_un;
DBCdim   cdm_un;
DBRdim   rdm_un;
DBAdim   adm_un;
DBMesh   msh_un;
DBGroup  grp_un;
DBCsys   csy_un;
DBPart   prt_un;
DBBplane bpl_un;
} DBAny;

/*
***GM database meta data.
*/

typedef struct
{
DBshort  pagsiz;             /* Pagesize (PAGSIZ) */
DBpagnum logmax;             /* Currently allocated logical pages */
DBpagnum logusd;             /* Currently used logical pages */
DBpagnum fysmax;             /* Currently allocated RAM pages */
DBpagnum fysusd;             /* Currently used RAM pages */
DBint    pfsiz;              /* Size of pagefile */
DBint    pfcnt;              /* Current number of pagefaults */
DBint    wrcnt;              /* Number of pages written */
DBint    rdcnt;              /* Number of pages read */
DBpagnum ipgnum;             /* Current ID page */
DBpagnum dpgnum;             /* Current data page */
DBshort  datofs;             /* Current offset within data page */
} GMMDAT;

/*
***Function prototypes for the public DB API.
*/
#ifdef __cplusplus
extern "C" {
#endif

/* Database init, load, save and clear */

DBstatus DBinit(char *filename, DBint cache_size,
                DBint version, DBint revision, char level);
DBstatus DBload(char *filename, DBint cache_size,
                DBint version, DBint revision, char level);
DBstatus DBexit();
DBstatus DBreset();

/* DB entity header read and update */

DBstatus DBread_header(DBHeader *hedptr, DBptr la);
DBstatus DBupdate_header(DBHeader *hedptr, DBptr la);

/* Transformation insert, read, update and delete */

DBstatus DBinsert_tform(DBTform *trfptr, DBId *idptr, DBptr *laptr);
DBstatus DBread_tform(DBTform *trfptr, DBptr la);
DBstatus DBupdate_tform(DBTform *trfptr, DBptr la);
DBstatus DBdelete_tform(DBptr la);

/* Point insert, read, update and delete */

DBstatus DBinsert_point(DBPoint *poiptr, DBId *idptr, DBptr *laptr);
DBstatus DBread_point(DBPoint *poiptr, DBptr la);
DBstatus DBupdate_point(DBPoint *poiptr, DBptr la);
DBstatus DBdelete_point(DBptr la);

/* Line insert, read, update and delete */

DBstatus DBinsert_line(DBLine *linptr, DBId *idptr, DBptr *laptr);
DBstatus DBread_line(DBLine *linptr, DBptr la); 
DBstatus DBupdate_line(DBLine *linptr, DBptr la);
DBstatus DBdelete_line(DBptr la); 

/* Circle/arc insert, read, update and delete */

DBstatus DBinsert_arc(DBArc *arcptr,DBSeg *geoseg,DBId  *idptr,DBptr *laptr);
DBstatus DBread_arc(DBArc *arcptr, DBSeg *geoseg, DBptr la);
DBstatus DBupdate_arc(DBArc *arcptr, DBSeg *geoseg, DBptr la);
DBstatus DBdelete_arc(DBptr la);

/* Curve insert, read, update and delete etc. */

DBstatus DBinsert_curve(DBCurve *curptr, DBSeg *graseg, DBSeg *geoseg,
                        DBId  *idptr,  DBptr *laptr);
DBstatus DBread_curve(DBCurve *curptr, DBSeg **graseg, DBSeg **geoseg, DBptr la);
DBstatus DBupdate_curve(DBCurve *curptr, DBSeg *geoseg, DBptr la);
DBstatus DBdelete_curve(DBptr la);
DBSeg   *DBcreate_segments(DBint nseg);
DBSeg   *DBadd_segments(DBSeg *segptr, DBint nseg);
DBstatus DBfree_segments(DBSeg *segptr);
DBstatus DBwrite_nurbs(DBHvector *cpts, DBint ncpts, DBfloat *knots,
                       DBint nknots, DBptr *cpts_la, DBptr *knots_la);
DBstatus DBread_nurbs(DBHvector *cpts, DBint ncpts, DBfloat *knots,
                             DBint nknots, DBptr cpts_la, DBptr knots_la);
DBstatus DBfree_nurbs(DBfloat  *knots, DBHvector *cpts);                             
DBstatus DBdelete_nurbs(DBSeg *seg);                  
                       
                       
/* Surface insert, read, update, deltete etc. */

DBstatus  DBinsert_surface(DBSurf *surptr, DBPatch *tpptr, DBSegarr *ptrim,
                           DBId *idptr, DBptr *laptr);
DBstatus  DBread_surface(DBSurf *surptr, DBptr la);
DBstatus  DBupdate_surface(DBSurf *surptr, DBptr la);
DBstatus  DBdelete_surface(DBptr la);
DBstatus  DBread_patches(DBSurf *surptr, DBPatch **ppatpt);
DBstatus  DBread_one_patch(DBSurf *surptr, DBPatch *patptr, DBshort iu, DBshort iv);
char     *DBcreate_patches(DBint pattyp, DBint numpat);
DBstatus  DBcreate_NURBS(DBPatchNU *patptr);
DBstatus  DBfree_patches(DBSurf *surptr, DBPatch *patptr);
DBstatus  DBadd_sur_grwire(DBSurf *surptr, DBSeg *sptarr[]);
DBstatus  DBread_sur_grwire(DBSurf *surptr, DBSeg *sptarr[]);
DBstatus  DBdelete_sur_grwire(DBSurf *surptr);
DBstatus  DBfree_sur_grwire(DBSeg *sptarr[]);
DBstatus  DBadd_sur_grsur(DBSurf *surptr, GLfloat *ku,
                           GLfloat *kv, GLfloat *cpts);
DBstatus  DBread_sur_grsur(DBSurf *surptr, GLfloat **ku,
                            GLfloat **kv, GLfloat **cpts);
DBstatus  DBdelete_sur_grsur(DBSurf *surptr);
DBstatus  DBfree_sur_grsur(GLfloat *ku, GLfloat *kv, GLfloat *cpts);
DBstatus  DBread_getrimcurves(DBSurf *surpek, DBSegarr **ppgetrimcurves);
DBstatus  DBfree_getrimcurves(DBSurf *surpek, DBSegarr *pgetrimcurves);
DBSegarr *DBcreate_segarrs(DBint nsegarr);

/* Coordinate system insert, read, update and delete */

DBstatus DBinsert_csys(DBCsys *csyptr, DBTmat *csymat,
                       DBId  *idptr,  DBptr *laptr);
DBstatus DBread_csys(DBCsys *csyptr, DBTmat *csymat, DBptr la);
DBstatus DBupdate_csys(DBCsys *csyptr, DBTmat *csymat, DBptr la);
DBstatus DBdelete_csys(DBptr la);

/* B_plane insert, read, update and delete */

DBstatus DBinsert_bplane(DBBplane *bplptr, DBId *idptr, DBptr *laptr);
DBstatus DBread_bplane(DBBplane *bplptr, DBptr la);
DBstatus DBupdate_bplane(DBBplane *bplptr, DBptr la);
DBstatus DBdelete_bplane(DBptr la);

/* Text insert, read, update and delete */

DBstatus DBinsert_text(DBText *txtptr, char *strptr,
                       DBId  *idptr,  DBptr *laptr);
DBstatus DBread_text(DBText *txtptr, char *strptr, DBptr la);
DBstatus DBupdate_text(DBText *txtptr, char *strptr, DBptr la);
DBstatus DBdelete_text(DBptr la);

/* Crosshatch insert, read, update and delete */

DBstatus DBinsert_xhatch(DBHatch *xhtptr, gmflt *crdptr,
                         DBId  *idptr,  DBptr *laptr);
DBstatus DBread_xhatch(DBHatch *xhtptr, gmflt *crdptr, DBptr la);
DBstatus DBupdate_xhatch(DBHatch *xhtptr, gmflt *crdptr, DBptr la);
DBstatus DBdelete_xhatch(DBptr la);

/* Linear dimension insert, read, update and delete */

DBstatus DBinsert_ldim(DBLdim *ldmptr, DBId *idptr, DBptr *laptr);
DBstatus DBread_ldim(DBLdim *ldmptr, DBptr la);
DBstatus DBupdate_ldim(DBLdim *ldmptr, DBptr la);
DBstatus DBdelete_ldim(DBptr la);

/* Circular dimension insert, read, update and delete */

DBstatus DBinsert_cdim(DBCdim *cdmptr, DBId *idptr, DBptr *laptr);
DBstatus DBread_cdim(DBCdim *cdmptr, DBptr la);
DBstatus DBupdate_cdim(DBCdim *cdmptr, DBptr la);
DBstatus DBdelete_cdim(DBptr la);

/* Radius dimension insert, read, update and delete */

DBstatus DBinsert_rdim(DBRdim *rdmptr, DBId *idptr, DBptr *laptr);
DBstatus DBread_rdim(DBRdim *rdmptr, DBptr la);
DBstatus DBupdate_rdim(DBRdim *rdmptr, DBptr la);
DBstatus DBdelete_rdim(DBptr la);

/* Angular dimension insert, read, update and delete */

DBstatus DBinsert_adim(DBAdim *admptr, DBId *idptr, DBptr *laptr);
DBstatus DBread_adim(DBAdim *admptr, DBptr la);
DBstatus DBupdate_adim(DBAdim *admptr, DBptr la);
DBstatus DBdelete_adim(DBptr la);

/* Mesh insert, read, alloc, free and delete */

DBstatus DBinsert_mesh(DBMesh *mshptr, DBId *idptr, DBptr *laptr);
DBstatus DBread_mesh(DBMesh *mshptr, DBptr la, DBint mode);
DBstatus DBdelete_mesh(DBptr la);
DBstatus DBalloc_mesh(DBMesh *mshptr);
DBstatus DBfree_mesh(DBMesh *mshptr);

/* Group insert, read, update, delete, member add and delete  */

DBstatus DBinsert_group(DBGroup *grpptr, DBptr *laarr,
                        DBId  *idptr,  DBptr *laptr);
DBstatus DBread_group(DBGroup *grpptr, DBptr *laarr, DBptr la);
DBstatus DBupdate_group(DBGroup *grpptr, DBptr *laarr, DBptr la);
DBstatus DBdelete_group(DBptr la);
DBstatus DBadd_group_member(DBptr la, DBptr memla);
DBstatus DBdelete_group_member(DBptr la, DBptr memla);
DBstatus gmagpp(DBptr la, DBptr memla);

/* Part open, close, read, update and delete */

DBstatus DBinsert_part(DBPart *prtptr, DBPdat *datptr, V2PATLOG typarr[],
                       char valarr[], DBId  *idptr, V2NAPA  *npptr,
                       DBptr *laptr);
DBstatus DBclose_part();
DBstatus DBread_part(DBPart *prtptr, DBptr la);
DBstatus DBread_part_parameters(DBPdat *datptr, V2PATLOG typarr[],
                                char vlarr[], DBptr la, DBshort size);
DBstatus DBupdate_part_parameter(DBPart *prtptr, DBshort parnum,
                                 V2LITVA *newval);
DBstatus DBread_part_attributes(char *attptr, DBint attsiz, DBptr la);
DBstatus DBdelete_part(DBptr la);

/* Named data insert, read and delete */

DBstatus DBinsert_named_data(char *key, DBint type, DBint size,
                             DBint count, char *datptr);
DBstatus DBread_named_data(char *key, DBint *type, DBint *size,
                           DBint *count, char **datptr);
DBstatus DBdelete_named_data(char *key);

/* DBId to DBptr etc. */

DBstatus DBget_pointer(char fcode, DBId *id, DBptr *laptr, DBetype *typptr);
DBstatus DBget_id(DBptr la, DBId *idptr);
DBstatus DBget_highest_id(DBseqnum *numptr);
DBstatus DBget_free_id(DBseqnum *numptr);

/* GM search and traversal */

DBstatus DBset_root_id(DBId *id);
DBstatus DBget_next_ptr(DBint trmode, DBetype typmsk, char *name,
                        char *group, DBptr *laptr);

/* Generic entity delete */

DBstatus DBdelete_entity(DBptr la, DBetype type);

/* Get DB metadata */

DBstatus gmgtmd(GMMDAT *datptr);

#ifdef __cplusplus
}
#endif

/* Typedefs for the DBxxx namescheme of public datatypes */

typedef V3MDAT DBMetadata;
typedef V3MSIZ DBSystemsize;

/*
***Typedefs to ensure comatibility with old nameschemes.
***To be removed when all old code is updated
*/
typedef DBHeader     GMRECH;
typedef DBTform      GMTRF;
typedef DBPoint      GMPOI;
typedef DBLine       GMLIN;
typedef DBArc        GMARC;
typedef DBCurve      GMCUR;
typedef DBSeg        GMSEG;
typedef DBSurf       GMSUR;
typedef DBPatch      GMPAT;
typedef DBPatchC     GMPATC;
typedef DBPatchR     GMPATR;
typedef DBPatchL     GMPATL;
typedef DBPatchF     GMPATF;
typedef DBPatchP     GMPATP;
typedef DBPatchB     GMPATB;
typedef DBPatchN     GMPATN; 
typedef DBPatchP3    GMPATP3;
typedef DBPatchP5    GMPATP5;
typedef DBPatchP7    GMPATP7;
typedef DBPatchP9    GMPATP9;
typedef DBPatchP21   GMPATP21;
typedef DBPatchNU    GMPATNU;
typedef DBPatchBR3   GMPATBR3;
typedef DBPatchX     GMPATX;
typedef DBBplane     GMBPL;
typedef DBText       GMTXT;
typedef DBCsys       GMCSY;
typedef DBLdim       GMLDM;
typedef DBCdim       GMCDM;
typedef DBRdim       GMRDM;
typedef DBAdim       GMADM;
typedef DBHatch      GMXHT;
typedef DBGroup      GMGRP;
typedef DBPart       GMPRT;
typedef DBPdat       GMPDAT;
typedef DBAny        GMUNON;
