/**********************************************************************
*
*    WPshade.c
*    =========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPinsh();    Init for shade
*    WPexsh();    Exit for shade
*    WPshad();    Generate shaded image
*    WPdrsh();    Draw shaded image
*    WPscsh();    Sets active color
*    WPltvi();    Servs LIGHT_VIEW()
*    WPmtvi();    Servs MATERIAL_VIEW() 
*    WPlton();    Servs LIGHT_ON/OFF()
*    WPconl();    Sets light parameters
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

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/WP.h"
#include <GL/glu.h>
#include <math.h>

static WPGWIN    *gwinpt; /* Fönsterpekare */
static GLXContext rc;     /* Motsvarande Rendering Context */


/*
***B-plan och SUR_FAC-facetter vytransformeras och sparas
***i dynamiskt block-allokerat minne. Nedanstående konstanter
***används även i GP för hide men får egna värden här.
*/
typedef struct
{
DBVector  p1,p2,p3,p4,nv;
short     pen;
tbool     triangle;
} WPBPL;

#undef  MAXPBL
#define MAXPBL  1000
#undef  PBLSIZ
#define PBLSIZ  200*sizeof(WPBPL)  /* Blockens storlek i bytes < 32K */
#undef  MAXPLN
#define MAXPLN 200000

static WPBPL  *pblpek[MAXPBL];
static WPBPL **ppek;
static int     pblant,pblofs,nplan;

static short   mk_psdata(WPGWIN *gwinpt);
static short   mk_plan(DBBplane *bpl, WPBPL *plan);
static short   mk_sur(DBSurf *sur, DBptr la, WPGWIN *gwinpt);
static short   tr_cpts(DBSurf *sur, GLfloat *p_cpts, WPGWIN *gwinpt);
static short   tz_bbox(DBSurf *sur, WPGWIN *gwinpt);

/*
***Ytor får ligga kvar i GM och vytransformeras först i pass 2.
***Under pass 1 sparar vi bara GM-pekare och kollar Z-max/min.
*/
#define MAXSUR 1000     /* Max antal ytor */
static DBptr *spek;
static int    nsur;

/*
***Den vytransformerade modellens storlek i Z-led.
*/
static double  zmax,zmin;

/*
***Kör vi RGB-mode eller ColorIndex ?
*/
static bool rgb_gl;

/*
***Ljuskällor, max 8 stycken.
*/
typedef struct wplight
{
GLfloat pos[4];            /* Läge/riktning */
GLfloat dir[3];            /* Spotriktning */
GLfloat ang;               /* Spotvinkel */
GLfloat focus;             /* Spotfocus (0-128) */
GLfloat intensity;         /* Ljusstyrka (0-1) */
bool    state;             /* På/av */
} WPLIGHT;

static WPLIGHT lt_tab[8] = { {{0,0,1,0},{0,0,-1},180,0,1,TRUE},
                             {{0,0,1,0},{0,0,-1},180,0,1,FALSE},
                             {{0,0,1,0},{0,0,-1},180,0,1,FALSE},
                             {{0,0,1,0},{0,0,-1},180,0,1,FALSE},
                             {{0,0,1,0},{0,0,-1},180,0,1,FALSE},
                             {{0,0,1,0},{0,0,-1},180,0,1,FALSE},
                             {{0,0,1,0},{0,0,-1},180,0,1,FALSE},
                             {{0,0,1,0},{0,0,-1},180,0,1,FALSE} };
/*
***Material, max 8 stycken.
*/
#define MTTABSIZ 8

typedef struct wpmaterial
{
GLfloat ambient[4];        /* Ambient RGB */
GLfloat diffuse[4];        /* Diffuse RGB */
GLfloat specular[4];       /* Specular RGB */
GLfloat emission[4];       /* Emitted RGB */
GLfloat shininess;         /* Blankhet */
bool    defined;           /* Skapat ja/nej */
} WPMATERIAL;

static WPMATERIAL mt_tab[MTTABSIZ] = {
                          {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},0,FALSE},
                          {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},0,FALSE},
                          {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},0,FALSE},
                          {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},0,FALSE},
                          {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},0,FALSE},
                          {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},0,FALSE},
                          {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},0,FALSE},
                          {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},0,FALSE} };

static double  x[PLYMXV],y[PLYMXV],z[PLYMXV],a[PLYMXV];

/*!******************************************************/

        short WPinsh(
        int    wid,
        double zmin,
        double zmax,
        bool   smooth)

/*      Initrutin för OpenGL-shading med X-Windows.
 *
 *      In: wid        = Fönsterid.
 *          zmin, zmax = Scenens storlek i Z-led.
 *          smooth     = FLAT/SMOOTH
 *
 *      Ut: Inget.
 *
 *      Felkoder: WP1623 = GLX-saknas.
 *                WP1633 = Ingen visual
 *                WP1643 = Ingen context
 *                WP1663 = Visual mismatch
 *
 *      (C)microform ab 1997-02-11 J. Kjellander
 *
 *      1997-06-11 Visualresurser, J.Kjellander
 *
 ******************************************************!*/

  {
   char              errbuf[V3STRLEN],tmpbuf[V3STRLEN];
   int               i,dummy;
   VisualID          vidwin,vidglx;
   XVisualInfo      *visinfo;
   XWindowAttributes wa;
   int               visatt[50];


/*
***Kolla att OpenGL supportas av X-servern.
*/
   if ( !glXQueryExtension(xdisp,&dummy,&dummy) )
     return(erpush("WP1623",""));
/*
***Fixa lämplig visual.
*/
   i = 0;

   if ( WPgrst("varkon.shade.rgba",tmpbuf)  &&
        strcmp(tmpbuf,"True") == 0 )
     {
     visatt[i++] = GLX_RGBA;
     visatt[i++] = GLX_RED_SIZE;
     visatt[i++] = 2;
     visatt[i++] = GLX_GREEN_SIZE;
     visatt[i++] = 2;
     visatt[i++] = GLX_BLUE_SIZE;
     visatt[i++] = 2;
     rgb_gl = TRUE;
     }
   else rgb_gl = FALSE;

   visatt[i++] = GLX_DEPTH_SIZE;
   visatt[i++] = 16;

   if ( WPgrst("varkon.shade.doublebuffer",tmpbuf)  &&
        strcmp(tmpbuf,"True") == 0 ) visatt[i++] = GLX_DOUBLEBUFFER;

   visatt[i]   = None;

   visinfo = glXChooseVisual(xdisp,xscr,visatt);

   if ( !visinfo )
     {
     if ( rgb_gl ) return(erpush("WP1633","RGB Mode"));
     else          return(erpush("WP1633","Color Index Mode"));
     }
/*
***Pekare till aktuellt grafiskt fönster.
*/
   if ( (gwinpt=(WPGWIN *)wpwtab[wid].ptr) == NULL ) return(-2);
/*
***Kolla att vald visual matchar det grafiska fönstrets visual.
*/
   XGetWindowAttributes(xdisp,gwinpt->id.x_id,&wa);

   vidwin = XVisualIDFromVisual(wa.visual);
   vidglx = XVisualIDFromVisual(visinfo->visual);

   if ( vidwin != vidglx )
     {
     sprintf(errbuf,"%d%%%d",(int)vidwin,(int)vidglx);
     return(erpush("WP1663",errbuf));
     }
/*
***Skapa rendering context.
*/
   if ( (rc=glXCreateContext(xdisp,visinfo,NULL,TRUE)) == NULL )
     return(erpush("WP1643",""));
/*
***Aktivera Rendering Contextet.
*/
   glXMakeCurrent(xdisp,gwinpt->id.x_id,rc);
/*
***Aktivera djuptest.
*/
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);
   glClearDepth((GLclampd)1.0);
/* 
***Transformation. I varkon kan zmin = zmax men OpenGL gillar
***det inte.
*/
   if ( zmax - zmin < 1e-3 )
     {
     zmax = zmax + 0.01;
     zmin = zmin - 0.01;
     }

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(gwinpt->vy.modwin.xmin,gwinpt->vy.modwin.xmax,
              gwinpt->vy.modwin.ymin,gwinpt->vy.modwin.ymax,
              -zmax,-zmin);
/*
***Viewport. Den del av det grafiska fönstret som får användas.
***Eventuella marginaler för knappar skall ej skrivas över.
*/
   glScissor((GLint)gwinpt->vy.scrwin.xmin,(GLint)gwinpt->vy.scrwin.ymin,
             (GLint)(gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin),
             (GLint)(gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin));
   glEnable(GL_SCISSOR_TEST);

   glViewport((GLint)gwinpt->vy.scrwin.xmin,(GLint)gwinpt->vy.scrwin.ymin,
              (GLint)(gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin),
              (GLint)(gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin));
/*
***Låt positiv normal definieras av moturs riktning precis
***som i Varkon.
*/
   glFrontFace(GL_CCW);
/*
***Configurera aktiva ljuskällor.
*/
   WPconl(gwinpt);
/*
***Normalisera ytnormaler så att färgstyrka
***beräknas på rätt sätt.
*/
   glEnable(GL_NORMALIZE);
/*
***Smooth shading kan ha betydelse om man använder
***spitlights.
*/
   if ( smooth ) glShadeModel(GL_SMOOTH);
   else          glShadeModel(GL_FLAT);
/*
***Sudda färgbuffert och djupbuffert.
*/ 
   glClearColor((GLclampf)1.0,(GLclampf)1.0,(GLclampf)1.0,(GLclampf)1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPexsh()

/*      Exitrutin för OpenGL-shading.
 *
 *      FV: 0.
 *
 *      (C)microform ab 1997-02-12 J. Kjellander
 *
 *      1997-04-09 Save-under, J.Kjellander
 *      1997-06-11 Dubbelbuffring, J.Kjellander
 *
 ******************************************************!*/

  {
   char tmpbuf[V3STRLEN];

/*
***Flusha OpenGL.
*/
   if ( WPgrst("varkon.shade.doublebuffer",tmpbuf)  &&
        strcmp(tmpbuf,"True") == 0 )
     {
     glXSwapBuffers(xdisp,gwinpt->id.x_id);
     }
   else
     {
     glFlush();
     }
/*
***Stäng OpenGL.
*/
   glXMakeCurrent(xdisp,None,NULL);
   glXDestroyContext(xdisp,rc);
/*
***Kopiera fönstrets innehåll till "save-under"-
***bitmappen.
*/
   XCopyArea(xdisp,gwinpt->id.x_id,gwinpt->savmap,gwinpt->win_gc,
             0,0,gwinpt->geo.dx,gwinpt->geo.dy,0,0);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPshad(
        int  wid,
        bool smooth)

/*      Huvudrutin för shade.
 *
 *      In: wid    = Fönsterid.
 *          smooth = FLAT eller SMOOTH shading
 *
 *      (C)microform ab 1997-02-10 J. Kjellander
 *
 *      Felkoder: WP1592 = Fönster %s finns ej
 *                WP1602 = Fönster %s ej grafiskt
 *                WP1612 = Kan ej initiera OpenGL
 *
 ******************************************************!*/

 {
   short   status;
   int     i;
   char    buf[80],errbuf[80];
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Pekare till aktuellt grafiskt fönster.
*/
   if ( (winptr=WPwgwp((wpw_id)wid)) == NULL )
     {
     sprintf(errbuf,"%d",wid);
     return(erpush("WP1592",errbuf));
     }

   if ( winptr->typ != TYP_GWIN )
     {
     sprintf(errbuf,"%d",wid);
     return(erpush("WP1602",errbuf));
     }

   gwinpt = (WPGWIN *)winptr->ptr;
/*
***Skapa plandata. Ett litet meddelande under tiden.
*/
   igptma(155,IG_MESS);
   status = mk_psdata(gwinpt);
   igrsma();
   if ( status < 0 ) return(status);
   sprintf(buf,"%s%d + %d",iggtts(156),nplan,nsur);
   igplma(buf,IG_MESS);
/*
***Först nu kan vi prova att öppna OpenGL eftersom
***det är först nu som vi vet modellens storlek i Z-led.
*/
   if ( nplan > 0  ||  nsur > 0 )
     {
#ifdef WIN32
     if ( msinsh(wid,zmin,zmax,smooth) < 0 ) return(erpush("WP1612",""));
#else
     if ( WPinsh(wid,zmin,zmax,smooth) < 0 ) return(erpush("WP1612",""));
#endif
/*
***Rita skuggat.
*/
     WPdrsh(gwinpt);
     }
/*
***Avallokera minne.
*/
   for ( i=0; i<pblant; ++i ) v3free(pblpek[i],"WPshad");
   v3free(spek,"WPshad");
/*
***Avslutning. Här utförs OS-beroende avslutning
***av OpenGL.
*/
   if ( nplan > 0  ||  nsur > 0 )
     {
#ifdef WIN32
     msexsh();
#else
     WPexsh();
#endif
     }
/*
***Slut.
*/
   igrsma();

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short WPdrsh(
        WPGWIN *gwinpt)

/*      Ritar storheter skuggade.
 *
 *      (C)microform ab 1997-03-06 J. Kjellander
 *
 ******************************************************!*/

 {
   short        curpen;
   int          i;
   double       cn;
   DBSurf       sur;
   GLfloat      tol;
   GLfloat     *p_kvu,*p_kvv,*p_cpts;
   GLUnurbsObj *yr;

/*
***Div. initiering.
*/
   curpen = -1;
/*
***Gå igenom planen.
*/
/*
glNewList(1,GL_COMPILE);
*/

   for ( i=0; i<nplan; ++i )
     {
     if ( (*(ppek+i))->pen != curpen )
       {
       curpen = (*(ppek+i))->pen;
       WPscsh((int)curpen);
       }
     if ( (*(ppek+i))->triangle )
       {
       glBegin(GL_TRIANGLES);
       glNormal3d((*(ppek+i))->nv.x_gm,(*(ppek+i))->nv.y_gm,(*(ppek+i))->nv.z_gm);
       glVertex3d((*(ppek+i))->p1.x_gm,(*(ppek+i))->p1.y_gm,(*(ppek+i))->p1.z_gm);
       glVertex3d((*(ppek+i))->p2.x_gm,(*(ppek+i))->p2.y_gm,(*(ppek+i))->p2.z_gm);
       glVertex3d((*(ppek+i))->p4.x_gm,(*(ppek+i))->p4.y_gm,(*(ppek+i))->p4.z_gm);
       glEnd();
       }
     else 
       {
       glBegin(GL_QUADS);
       glNormal3d((*(ppek+i))->nv.x_gm,(*(ppek+i))->nv.y_gm,(*(ppek+i))->nv.z_gm);
       glVertex3d((*(ppek+i))->p1.x_gm,(*(ppek+i))->p1.y_gm,(*(ppek+i))->p1.z_gm);
       glVertex3d((*(ppek+i))->p2.x_gm,(*(ppek+i))->p2.y_gm,(*(ppek+i))->p2.z_gm);
       glVertex3d((*(ppek+i))->p3.x_gm,(*(ppek+i))->p3.y_gm,(*(ppek+i))->p3.z_gm);
       glVertex3d((*(ppek+i))->p4.x_gm,(*(ppek+i))->p4.y_gm,(*(ppek+i))->p4.z_gm);
       glEnd();
       }
     }
/*
***Och ytorna.
*/
   if ( nsur > 0 )
     {
     glEnable(GL_AUTO_NORMAL);
     yr = gluNewNurbsRenderer();
     WPget_cacc(&cn);
     if ( (tol=50.0/cn) < 1.0 ) tol = 1.0;
     gluNurbsProperty(yr,GLU_SAMPLING_TOLERANCE,tol);
     gluNurbsProperty(yr,GLU_DISPLAY_MODE,GLU_FILL);

     for ( i=0; i<nsur; ++i )
       {
       DBread_surface(&sur,*(spek+i));
       if ( sur.hed_su.pen != curpen )
         {
         curpen = sur.hed_su.pen;
         WPscsh((int)curpen);
         }
       DBread_sur_grsur(&sur,&p_kvu,&p_kvv,&p_cpts);
/*
***Vytransformera styrpolygonen.
*/
       tr_cpts(&sur,p_cpts,gwinpt);
/*
***Rita.
*/
       gluBeginSurface(yr);
       gluNurbsSurface(yr,
                       sur.nku_su, p_kvu,
                       sur.nkv_su, p_kvv,
                      (sur.nkv_su-sur.vorder_su)*3, 3,
                       p_cpts,
                       sur.uorder_su, sur.vorder_su,                         
                       GL_MAP2_VERTEX_3);
       gluEndSurface(yr);
       }
     DBfree_sur_grsur(p_kvu,p_kvv,p_cpts);
     }
/*
***Slut.
*/

/*
glEndList();
glCallList(1);
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

    static short   mk_psdata(
           WPGWIN *gwinpt)

/*      Går igenom GM och preparerar data för skuggning.
 * 
 *      Felkoder: GP0072 = Plandata kräver för många block
 *                GP0082 = Fel från malloc() för plandata
 *                GP0092 = Fel från malloc() för planpekare
 *
 *      (C)microform ab 1997-02-11 J. Kjellander
 *
 ******************************************************!*/

 {
   short    status;
   int      i,j;
   DBetype  type;
   DBptr    la;
   DBBplane bpl;
   DBSurf   sur;
   DBPatch *patpek,*toppat;
   DBPatchF  *facpat;
   WPBPL    plan;

/*
***Div. initiering.
*/
   nplan  =  0;
   pblant =  0;
   pblofs =  100;
   nsur   =  0;
   zmax   = -1e20;
   zmin   =  1e20;
/*
***Allokera minne för plan-pekare. Max antal plan = MAXPLN.
*/
   if ( (ppek=(WPBPL **)v3mall(MAXPLN*sizeof(WPBPL *),"psdata")) == NULL )
     return(erpush("GP0092",""));
/*
***Allokera minne för ytor. Max antal ytor = MAXSUR.
*/
   if ( (spek=(DBptr *)v3mall(MAXSUR*sizeof(DBptr),"psdata")) == NULL )
     return(erpush("GP0092",""));
/*
***Leta upp alla plan och ytor i GM.
*/
   DBget_pointer('F',NULL,&la,&type);

   while ( DBget_pointer('N',NULL,&la,&type ) == 0 )
     {
     switch ( type )
       {
/*
***Ett B-plan.
*/
       case BPLTYP:
       DBread_bplane(&bpl,la);
       if ( bpl.hed_bp.blank  |  !WPnivt(gwinpt,bpl.hed_bp.level) ) break;
       if ( (status=mk_plan(&bpl,&plan)) < 0 ) return(status);
       break;
/*
***En yta, kanske en facett-yta.
*/
       case SURTYP:
       DBread_surface(&sur,la);
       if ( sur.typ_su == FAC_SUR )
         {
         if ( sur.hed_su.blank | !WPnivt(gwinpt,sur.hed_su.level) ) break;
         bpl.hed_bp.pen  = sur.hed_su.pen;
         DBread_patches(&sur,&patpek);
         toppat = patpek;
         for ( i=0; i<sur.nu_su; ++i )
           {
           for ( j=0; j<sur.nv_su; ++j )
             {
             if ( toppat->styp_pat == FAC_PAT )
               {
               facpat = (DBPatchF *)toppat->spek_c;
/*
***Antingen är facetten fyrsidig....
*/
               if ( !facpat->triangles )
                 {
                 V3MOME(&facpat->p1,&bpl.crd1_bp,sizeof(DBVector));
                 V3MOME(&facpat->p2,&bpl.crd2_bp,sizeof(DBVector));
                 V3MOME(&facpat->p3,&bpl.crd3_bp,sizeof(DBVector));
                 V3MOME(&facpat->p4,&bpl.crd4_bp,sizeof(DBVector));
                 if ( (status=mk_plan(&bpl,&plan)) < 0 ) return(status);
                 }
/*
***eller också består den av två trianglar.
*/
               else
                 {
                 V3MOME(&facpat->p1,&bpl.crd1_bp,sizeof(DBVector));
                 V3MOME(&facpat->p2,&bpl.crd2_bp,sizeof(DBVector));
                 V3MOME(&facpat->p2,&bpl.crd3_bp,sizeof(DBVector));
                 V3MOME(&facpat->p3,&bpl.crd4_bp,sizeof(DBVector));
                 if ( (status=mk_plan(&bpl,&plan)) < 0 ) return(status);
                 V3MOME(&facpat->p1,&bpl.crd1_bp,sizeof(DBVector));
                 V3MOME(&facpat->p3,&bpl.crd2_bp,sizeof(DBVector));
                 V3MOME(&facpat->p3,&bpl.crd3_bp,sizeof(DBVector));
                 V3MOME(&facpat->p4,&bpl.crd4_bp,sizeof(DBVector));
                 if ( (status=mk_plan(&bpl,&plan)) < 0 ) return(status);
                 }
               }
             ++toppat;
             }
           }
         DBfree_patches(&sur,patpek);
         }
/*
***Om det inte är en FAC_SUR har den B-spline approximation.
*/
       else
         {
         if ( (status=mk_sur(&sur,la,gwinpt)) < 0 ) return(status);
         }
       break;
       }
     }
/*
****Slut.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

static  short  mk_plan(
        DBBplane *bpl,
        WPBPL *plan)

/*      Processar ett DBBplane och skapar motsvarande
 *      WPBPL med data för skuggning.
 * 
 *      (C)microform ab 1997-02-11 J. Kjellander
 *
 ******************************************************!*/

 {
   int     k;
   double  d;
   DBVector   p21,p32,p41,prod;

/*
***Transformera till aktiv vy genom att anropa gpplbp.
*/
   k = -1;
/*   gpplbp(bpl,&k); !!!!!!!!!!!!!! Fixa senare !!!!!! */
/*
***Beräkna normalvektor vecn(vprod(p2-p1,p4-p1)).
*/
   p21.x_gm = x[1] - x[0];
   p21.y_gm = y[1] - y[0];
   p21.z_gm = z[1] - z[0];

   p41.x_gm = x[3] - x[0];
   p41.y_gm = y[3] - y[0];
   p41.z_gm = z[3] - z[0];

   prod.x_gm = p21.y_gm*p41.z_gm - p21.z_gm*p41.y_gm;
   prod.y_gm = p21.z_gm*p41.x_gm - p21.x_gm*p41.z_gm;
   prod.z_gm = p21.x_gm*p41.y_gm - p21.y_gm*p41.x_gm;

   d = prod.x_gm*prod.x_gm + prod.y_gm*prod.y_gm + prod.z_gm*prod.z_gm;

   if ( d > DTOL )
     {
     d = 1.0/SQRT(d);
     plan->nv.x_gm = prod.x_gm * d;
     plan->nv.y_gm = prod.y_gm * d;
     plan->nv.z_gm = prod.z_gm * d;
     }
   else return(0);
/*
***Om planet är vänt mot betraktaren, beräkna även övriga konstanter.
***Lagra resultatet i en WPBPL-post.
*/
   if ( plan->nv.z_gm > DTOL )
     {
/*
*** 4 hörnpositioner.
*/
     plan->p1.x_gm = x[0]; plan->p1.y_gm = y[0]; plan->p1.z_gm = z[0];
     plan->p2.x_gm = x[1]; plan->p2.y_gm = y[1]; plan->p2.z_gm = z[1];
     plan->p3.x_gm = x[2]; plan->p3.y_gm = y[2]; plan->p3.z_gm = z[2];
     plan->p4.x_gm = x[3]; plan->p4.y_gm = y[3]; plan->p4.z_gm = z[3];
/*
***Pennummer.
*/
     plan->pen = bpl->hed_bp.pen;
/*
***3- eller 4-sidigt ?.
*/
     p32.x_gm = x[2] - x[1];
     p32.y_gm = y[2] - y[1];
     p32.z_gm = z[2] - z[1];
     d = p32.x_gm*p32.x_gm + p32.y_gm*p32.y_gm + p32.z_gm*p32.z_gm;

     if ( d < DTOL ) plan->triangle = TRUE;
     else            plan->triangle = FALSE;
/*
***Z-max och min.
*/
     if      ( z[0] > zmax ) zmax = z[0];
     else if ( z[0] < zmin ) zmin = z[0];

     if      ( z[1] > zmax ) zmax = z[1];
     else if ( z[1] < zmin ) zmin = z[1];

          if ( z[2] > zmax ) zmax = z[2];
     else if ( z[2] < zmin ) zmin = z[2];

     if      ( z[3] > zmax ) zmax = z[3];
     else if ( z[3] < zmin ) zmin = z[3];
/*
***Plan-data för detta plan är nu klara. Allokera minne för plan-
***data och lagra.
*/
     if (  pblofs == 100 )
       {
       if ( pblant == MAXPBL ) 
         return(erpush("GP0072",""));
       if ( (pblpek[pblant]=(WPBPL *)v3mall(PBLSIZ,"wpgnpd")) == NULL )
         return(erpush("GP0082",""));
       else
         { ++pblant; pblofs = 0; }
       }

     *(ppek+nplan) = pblpek[pblant-1] + pblofs;
     V3MOME((char *)plan,(char *)*(ppek+nplan),sizeof(WPBPL));
     ++pblofs; ++nplan;
     }

  return(0);
 }

/********************************************************/
/*!******************************************************/

static  short  mk_sur(
        DBSurf  *sur,
        DBptr   la,
        WPGWIN *gwinpt)

/*      Processar en yta.
 *
 *      Felkoder: WP1653 = BBOX saknas.
 * 
 *      (C)microform ab 1997-03-06 J. Kjellander
 *
 ******************************************************!*/

 {

/*
***Finns NURBS-approximation ?
*/
   if ( sur->nku_su == 0    ||
        sur->nkv_su == 0    ||
        sur->uorder_su == 0 ||
        sur->vorder_su == 0 ||
        sur->pkvu_su ==  DBNULL  ||
        sur->pkvv_su ==  DBNULL  ||
        sur->pcpts_su == DBNULL ) return(0);
/*
***Tills vidare spar vi bara tända ytors adress i GM.
*/
   if ( nsur < MAXSUR )
     {
     if ( sur->hed_su.blank  |  !WPnivt(gwinpt,sur->hed_su.level) ) return(0);
    *(spek+nsur) = la;
   ++nsur;
/*
***Hur stor är ytan i Z-led ?
*/
     if ( sur->box_su.flag == -99 ) return(erpush("WP1653",""));
     tz_bbox(sur,gwinpt);
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

static short    tr_cpts(
       DBSurf   *sur,
       GLfloat *p_cpts,
       WPGWIN  *gwinpt)

/*      Vytransformerar styrpolygon.
 *
 *      (C)microform ab 1997-02-19 J. Kjellander
 *
 ******************************************************!*/

  {
   int      i,npts;
   GLfloat *px,*py,*pz;
   GLfloat  x,y,z;

/*
***Loopa igenom alla styrpunkter.
*/
   npts = (sur->nku_su - sur->uorder_su) *
          (sur->nkv_su - sur->vorder_su);

   px = p_cpts;
   py = px + 1;
   pz = px + 2;

   for ( i=0; i<npts; ++i )
     {
     x = *px;
     y = *py;
     z = *pz;
/*
***Transformera positionen.
*/
     *px = gwinpt->vy.vymat.k11 * x +
           gwinpt->vy.vymat.k12 * y +
           gwinpt->vy.vymat.k13 * z;
     *py = gwinpt->vy.vymat.k21 * x +
           gwinpt->vy.vymat.k22 * y +
           gwinpt->vy.vymat.k23 * z;
     *pz = gwinpt->vy.vymat.k31 * x +
           gwinpt->vy.vymat.k32 * y +
           gwinpt->vy.vymat.k33 * z;
     px += 3;
     py += 3;
     pz += 3;
     }

  return(0);
  }

/********************************************************/
/*!******************************************************/
/*
static short    tz_cpts(
       DBSurf   *sur,
       GLfloat *p_cpts,
       WPGWIN  *gwinpt)
*/

/*     Testar styrpolygonens Z-koordinater mot zmax och zmin.
 *
 *      (C)microform ab 1997-02-19 J. Kjellander
 *
 ******************************************************!*/
/*
  {
   int      i,npts;
   GLfloat *pz;

*
***Loopa igenom alla styrpunkter.
*
   npts = (sur->nku_su - sur->uorder_su) *
          (sur->nkv_su - sur->vorder_su);

   pz = p_cpts + 2;

   for ( i=0; i<npts; ++i )
     {
     if      ( *pz < zmin ) zmin = *pz;
     else if ( *pz > zmax ) zmax = *pz;
     pz += 3;
     }

  return(0);
  }
*/
/********************************************************/
/*!******************************************************/

static short    tz_bbox(
       DBSurf   *sur,
       WPGWIN  *gwinpt)

/*     Transformerar ytans BBOX till rätt vy och testar
 *     Z-koordinaterna mot zmax och zmin.
 *
 *      (C)microform ab 1997-06-17 J. Kjellander
 *
 ******************************************************!*/

  {
   int    i;
   DBfloat  boxz[8];

/*
***Transformera alla boxens Z-koordinater till fönstrets vy.
*/
   boxz[0] =  gwinpt->vy.vymat.k31 * sur->box_su.xmin +
              gwinpt->vy.vymat.k32 * sur->box_su.ymin +
              gwinpt->vy.vymat.k33 * sur->box_su.zmin;  

   boxz[1] =  gwinpt->vy.vymat.k31 * sur->box_su.xmax +
              gwinpt->vy.vymat.k32 * sur->box_su.ymin +
              gwinpt->vy.vymat.k33 * sur->box_su.zmin;  

   boxz[2] =  gwinpt->vy.vymat.k31 * sur->box_su.xmax +
              gwinpt->vy.vymat.k32 * sur->box_su.ymax +
              gwinpt->vy.vymat.k33 * sur->box_su.zmin;  

   boxz[3] =  gwinpt->vy.vymat.k31 * sur->box_su.xmin +
              gwinpt->vy.vymat.k32 * sur->box_su.ymax +
              gwinpt->vy.vymat.k33 * sur->box_su.zmin;  

   boxz[4] =  gwinpt->vy.vymat.k31 * sur->box_su.xmin +
              gwinpt->vy.vymat.k32 * sur->box_su.ymin +
              gwinpt->vy.vymat.k33 * sur->box_su.zmax;  

   boxz[5] =  gwinpt->vy.vymat.k31 * sur->box_su.xmax +
              gwinpt->vy.vymat.k32 * sur->box_su.ymin +
              gwinpt->vy.vymat.k33 * sur->box_su.zmax;  

   boxz[6] =  gwinpt->vy.vymat.k31 * sur->box_su.xmax +
              gwinpt->vy.vymat.k32 * sur->box_su.ymax +
              gwinpt->vy.vymat.k33 * sur->box_su.zmax;  

   boxz[7] =  gwinpt->vy.vymat.k31 * sur->box_su.xmin +
              gwinpt->vy.vymat.k32 * sur->box_su.ymax +
              gwinpt->vy.vymat.k33 * sur->box_su.zmax;  
/*
***Testa Z-koordinaterna mot zmax och zmin.
*/
   for ( i=0; i<8; ++i )
      {
      if      ( boxz[i] < zmin ) zmin = boxz[i];
      else if ( boxz[i] > zmax ) zmax = boxz[i];
      }
/*
***Slut.
*/

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPscsh(
        int pen)

/*      Sätter aktiv färg eller material. Pena 1 tom MTTABSIZ
 *      kan användas för att definiera material.
 *
 *      In: VARKON pennummer.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 1997-02-11 J. Kjellander
 *
 *      1997-04-13 WIN32, J.Kjellander
 *      1997-06-11 rgb_gl, J.Kjellander
 *
 ******************************************************!*/

  {
   GLfloat mat[4];

#ifdef V3_X11
   XColor  rgb;
   GLint   cind[3];
#endif
#ifdef WIN32
   COLORREF col;
#endif

/*
***Om material finns definierat tar vi det.
*/
   if ( pen > 0  &&  pen <= MTTABSIZ  &&  mt_tab[pen-1].defined )
     {
     glMaterialfv(GL_FRONT,GL_AMBIENT,mt_tab[pen-1].ambient);
     glMaterialfv(GL_FRONT,GL_DIFFUSE,mt_tab[pen-1].diffuse);
     glMaterialfv(GL_FRONT,GL_SPECULAR,mt_tab[pen-1].specular);
     glMaterialfv(GL_FRONT,GL_EMISSION,mt_tab[pen-1].emission);
     glMaterialf(GL_FRONT,GL_SHININESS,mt_tab[pen-1].shininess);
     return(0);
     }
/*
***Om inget material finns definierat för den aktuella
***pennan hämtar vi rgb-värden ur wp4:coltab[].
*/
#ifdef V3_X11
   else
     {
     if ( rgb_gl )
       {
       WPgrgb(pen,&rgb);
       mat[0] = rgb.red/65536.0;
       mat[1] = rgb.green/65536.0;
       mat[2] = rgb.blue/65536.0;
       mat[3] = 1.0;

       glMaterialfv(GL_FRONT,GL_AMBIENT,mat);
       glMaterialfv(GL_FRONT,GL_DIFFUSE,mat);
       glMaterialfv(GL_FRONT,GL_SPECULAR,mat);
       mat[0] = mat[1] = mat[2] = 0.0;
       glMaterialfv(GL_FRONT,GL_EMISSION,mat);
       glMaterialf(GL_FRONT,GL_SHININESS,(GLfloat)40.0);
       }
     else
       {
       cind[0] = cind[1] = cind[2] = (GLint)pen;
       glMaterialiv(GL_FRONT,GL_COLOR_INDEXES,cind);
       }
     }
#endif
/*
***eller om vi kör WIN32 
*/
#ifdef WIN32
   else
     {
     msgrgb(pen,&col);
     mat[0] = GetRValue(col)/255.0;
     mat[1] = GetGValue(col)/255.0;
     mat[2] = GetBValue(col)/255.0;
     mat[3] = 1.0;

     glMaterialfv(GL_FRONT,GL_AMBIENT,mat);
     glMaterialfv(GL_FRONT,GL_DIFFUSE,mat);
     glMaterialfv(GL_FRONT,GL_SPECULAR,mat);
     mat[0] = mat[1] = mat[2] = 0.0;
     glMaterialfv(GL_FRONT,GL_EMISSION,mat);
     glMaterialf(GL_FRONT,GL_SHININESS,(GLfloat)40.0);
     }
#endif

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short  WPltvi(
        DBint     ltnum,
        DBVector *pos1,
        DBVector *pos2,
        DBfloat   ang,
        DBfloat   focus)

/*      Sätter ljuskällas geometry.
 *
 *      In: ltnum = Ljuskälla
 *          pos1  = Position/riktning
 *          pos2  = Spotposition
 *          ang   = Spotvinkel
 *          focus = Spotfokus
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 1997-02-19 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Lite felkontroll.
*/
   if ( ltnum < 0  ||  ltnum > 7 ) return(0);
/*
***Om ang = 180 är det en ingen spotlight utan
***en vanlig lampa som lyser i alla riktningar.
*/
   if ( ang == 180.0 )
     {
     if ( pos1->x_gm == 0.0  &&
          pos1->y_gm == 0.0  &&
          pos1->z_gm == 0.0 ) return(0);

     lt_tab[ltnum].pos[0] = (GLfloat)pos1->x_gm;
     lt_tab[ltnum].pos[1] = (GLfloat)pos1->y_gm;
     lt_tab[ltnum].pos[2] = (GLfloat)pos1->z_gm;
     lt_tab[ltnum].pos[3] = (GLfloat)0.0;
     lt_tab[ltnum].ang    = 180.0;
     }
/*
***Om spotang <> 180 är det en spot. pos1 och pos2 får
***då inte vara lika för då kan vi inte beräkna nån riktning.
*/
   else
     {
     if ( pos2->x_gm == pos1->x_gm  &&
          pos2->y_gm == pos1->x_gm  &&
          pos2->z_gm == pos1->x_gm ) return(0);

     if ( ang > 90.0  ||  ang <= 0.0 ) return(0);
     if ( focus < 0  ||  focus > 100 ) return(0);

     lt_tab[ltnum].pos[0] = (GLfloat)pos1->x_gm;
     lt_tab[ltnum].pos[1] = (GLfloat)pos1->y_gm;
     lt_tab[ltnum].pos[2] = (GLfloat)pos1->z_gm;
     lt_tab[ltnum].pos[3] = (GLfloat)1.0;
     lt_tab[ltnum].dir[0] = (GLfloat)(pos2->x_gm - pos1->x_gm);
     lt_tab[ltnum].dir[1] = (GLfloat)(pos2->y_gm - pos1->y_gm);
     lt_tab[ltnum].dir[2] = (GLfloat)(pos2->z_gm - pos1->z_gm);
     lt_tab[ltnum].ang    = (GLfloat)ang;
     lt_tab[ltnum].focus  = (GLfloat)focus*1.28;
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short  WPmtvi(
        int    mtnum,
        DBfloat  ar,
        DBfloat  ag,
        DBfloat  ab,
        DBfloat  dr,
        DBfloat  dg,
        DBfloat  db,
        DBfloat  sr,
        DBfloat  sg,
        DBfloat  sb,
        DBfloat  er,
        DBfloat  eg,
        DBfloat  eb,
        DBfloat  sh)

/*      Sätter ett materials egenskaper.
 *
 *      In: mtnum    = Materialnummer 1 - MTTABSIZ
 *          ar,ag,ab = Ambient RGB   0-100
 *          dr,dg,db = Diffuse RGB   0-100
 *          sr,sg,sb = Specular RGB   0-100
 *          er,eg,eb = Emitted RGB   0-100
 *          sh       = Shininess   0-100
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 1997-02-20 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Lite felkontroll.
*/
   if ( mtnum < 1  ||  mtnum > MTTABSIZ  ) return(0);

   if ( ar < 0.0  ||  ar > 100.0 ) return(0);
   if ( ag < 0.0  ||  ag > 100.0 ) return(0);
   if ( ab < 0.0  ||  ab > 100.0 ) return(0);
   if ( dr < 0.0  ||  dr > 100.0 ) return(0);
   if ( dg < 0.0  ||  dg > 100.0 ) return(0);
   if ( db < 0.0  ||  db > 100.0 ) return(0);
   if ( sr < 0.0  ||  sr > 100.0 ) return(0);
   if ( sg < 0.0  ||  sg > 100.0 ) return(0);
   if ( sb < 0.0  ||  sb > 100.0 ) return(0);
   if ( er < 0.0  ||  er > 100.0 ) return(0);
   if ( eg < 0.0  ||  eg > 100.0 ) return(0);
   if ( eb < 0.0  ||  eb > 100.0 ) return(0);
   if ( sh < 0.0  ||  sh > 100.0 ) return(0);
/*
***Spara i mt_tab[].
*/
   mt_tab[mtnum-1].ambient[0]  = ar/100.0;
   mt_tab[mtnum-1].ambient[1]  = ag/100.0;
   mt_tab[mtnum-1].ambient[2]  = ab/100.0;
   mt_tab[mtnum-1].diffuse[0]  = dr/100.0;
   mt_tab[mtnum-1].diffuse[1]  = dg/100.0;
   mt_tab[mtnum-1].diffuse[2]  = db/100.0;
   mt_tab[mtnum-1].specular[0] = sr/100.0;
   mt_tab[mtnum-1].specular[1] = sg/100.0;
   mt_tab[mtnum-1].specular[2] = sb/100.0;
   mt_tab[mtnum-1].emission[0] = er/100.0;
   mt_tab[mtnum-1].emission[1] = eg/100.0;
   mt_tab[mtnum-1].emission[2] = eb/100.0;
   mt_tab[mtnum-1].shininess   = sh*1.28;
   mt_tab[mtnum-1].defined     = TRUE;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPlton(
        int   ltnum,
        DBfloat intensity,
        bool  state)

/*      Slår på och av olika ljuskällor.
 *
 *      In: ltnum = Ljuskälla
 *          state = På/Av.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 1997-02-19 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Lite felkontroll.
*/
   if ( ltnum < 0  ||  ltnum > 7 ) return(0);
   if ( intensity < 0.0  ||  intensity > 100.0 ) return(0);
/*
***Slå på/av.
*/
   if ( state )
     {
     lt_tab[ltnum].state = TRUE;
     lt_tab[ltnum].intensity = (GLfloat)(intensity/100.0);
     }
   else lt_tab[ltnum].state = FALSE;

   return(0);
  }

/********************************************************/
/*!******************************************************/

       short WPconl(
       WPGWIN *gwinpt)

/*      Konfigurerar aktiva ljuskällor.
 *
 *      In: gwinpt = Pekare till grafisk fönster
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 1997-02-19 J. Kjellander
 *
 ******************************************************!*/

  {
   int     i,ival;
   char    buf[V3STRLEN];
   GLfloat pos[4],dir[3],ambient[4],diffuse[4],specular[4];

   GLfloat scene_ambient[4]  = {0.2,0.2,0.2,1.0};
   GLfloat lt_ambient[4]  = {1.0,1.0,1.0,1.0};
   GLfloat lt_diffuse[4]  = {1.0,1.0,1.0,1.0};
   GLfloat lt_specular[4] = {1.0,1.0,1.0,1.0};
/*
***Slå på ljus.
*/
   glEnable(GL_LIGHTING);
/*
***Justera scenens eget bakgrundsljus. Default är
***enligt manualen (0.2,0.2,0.2).
*/
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT,scene_ambient);
/*
***Default bakgrundsljus för alla ljuskällor hämtar vi från
***resursfilen.
*/
#ifdef V3_X11
   if ( WPgrst("varkon.shade.ambient.red",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.AMBIENT.RED",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_ambient[0] = ival/100.0; 

#ifdef V3_X11
   if ( WPgrst("varkon.shade.ambient.green",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.AMBIENT.GREEN",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_ambient[1] = ival/100.0; 

#ifdef V3_X11
   if ( WPgrst("varkon.shade.ambient.blue",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.AMBIENT.BLUE",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_ambient[2] = ival/100.0; 
/*
***Default diffust ljus likaså.
*/
#ifdef V3_X11
   if ( WPgrst("varkon.shade.diffuse.red",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.DIFFUSE.RED",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_diffuse[0] = ival/100.0; 

#ifdef V3_X11
   if ( WPgrst("varkon.shade.diffuse.green",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.DIFFUSE.GREEN",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_diffuse[1] = ival/100.0; 

#ifdef V3_X11
   if ( WPgrst("varkon.shade.diffuse.blue",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.DIFFUSE.BLUE",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_diffuse[2] = ival/100.0; 
/*
***Och default reflekterat ljus.
*/
#ifdef V3_X11
   if ( WPgrst("varkon.shade.specular.red",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.SPECULAR.RED",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_specular[0] = ival/100.0; 

#ifdef V3_X11
   if ( WPgrst("varkon.shade.specular.green",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.SPECULAR.GREEN",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_specular[1] = ival/100.0; 

#ifdef V3_X11
   if ( WPgrst("varkon.shade.specular.blue",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.SPECULAR.BLUE",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_specular[2] = ival/100.0; 
/*
***Gå igenom lt_tab och sätt upp alla aktiva ljuskällor.
*/
   for ( i=0; i<8; ++i )
     {
     if ( lt_tab[i].state )
       {
       glEnable(GL_LIGHT0+i);
/*
***Transformera positionen.
*/
       pos[0] = gwinpt->vy.vymat.k11 * lt_tab[i].pos[0] +
                gwinpt->vy.vymat.k12 * lt_tab[i].pos[1] +
                gwinpt->vy.vymat.k13 * lt_tab[i].pos[2];
       pos[1] = gwinpt->vy.vymat.k21 * lt_tab[i].pos[0] +
                gwinpt->vy.vymat.k22 * lt_tab[i].pos[1] +
                gwinpt->vy.vymat.k23 * lt_tab[i].pos[2];
       pos[2] = gwinpt->vy.vymat.k31 * lt_tab[i].pos[0] +
                gwinpt->vy.vymat.k32 * lt_tab[i].pos[1] +
                gwinpt->vy.vymat.k33 * lt_tab[i].pos[2];
/*
***En vanlig ljuskälla har spotvinkel 180.0 och
***ingen avklingning, pos[3] = 0.0.
*/
       if ( lt_tab[i].ang == 180.0 )
         {
         pos[3] = 0.0;
         glLightfv(GL_LIGHT0+i,GL_POSITION,pos);
         glLightf(GL_LIGHT0+i,GL_SPOT_CUTOFF,(GLfloat)180.0);
         }
/*
***Om det är en spot skall även riktning transformeras
***och avklingning slås på.
*/
       if ( lt_tab[i].ang < 180.0 )
         {
         pos[3] = 1.0;
         dir[0] = gwinpt->vy.vymat.k11 * lt_tab[i].dir[0] +
                  gwinpt->vy.vymat.k12 * lt_tab[i].dir[1] +
                  gwinpt->vy.vymat.k13 * lt_tab[i].dir[2];
         dir[1] = gwinpt->vy.vymat.k21 * lt_tab[i].dir[0] +
                  gwinpt->vy.vymat.k22 * lt_tab[i].dir[1] +
                  gwinpt->vy.vymat.k23 * lt_tab[i].dir[2];
         dir[2] = gwinpt->vy.vymat.k31 * lt_tab[i].dir[0] +
                  gwinpt->vy.vymat.k32 * lt_tab[i].dir[1] +
                  gwinpt->vy.vymat.k33 * lt_tab[i].dir[2];
         glLightfv(GL_LIGHT0+i,GL_POSITION,pos);
         glLightfv(GL_LIGHT0+i,GL_SPOT_DIRECTION,dir);
         glLightf(GL_LIGHT0+i,GL_SPOT_CUTOFF,(GLfloat)lt_tab[i].ang);
         glLightf(GL_LIGHT0+i,GL_SPOT_EXPONENT,(GLfloat)lt_tab[i].focus);
         }
/*
***Bakgrundskomponentens styrka.
*/
       ambient[0] = lt_tab[i].intensity * lt_ambient[0];
       ambient[1] = lt_tab[i].intensity * lt_ambient[1];
       ambient[2] = lt_tab[i].intensity * lt_ambient[2];
       ambient[3] = 1.0;
       glLightfv(GL_LIGHT0+i,GL_AMBIENT,ambient);
/*
***Diffusa komponentens styrka.
*/
       diffuse[0] = lt_tab[i].intensity * lt_diffuse[0];
       diffuse[1] = lt_tab[i].intensity * lt_diffuse[1];
       diffuse[2] = lt_tab[i].intensity * lt_diffuse[2];
       diffuse[3] = 1.0;
       glLightfv(GL_LIGHT0+i,GL_DIFFUSE,diffuse);
/*
***Spekulära komponentens styrka.
*/
       specular[0] = lt_tab[i].intensity * lt_specular[0];
       specular[1] = lt_tab[i].intensity * lt_specular[1];
       specular[2] = lt_tab[i].intensity * lt_specular[2];
       specular[3] = 1.0;
       glLightfv(GL_LIGHT0+i,GL_SPECULAR,specular);
       }
     }
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
