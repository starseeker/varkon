/**********************************************************************
*
*    wphide.c
*    ========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    WPhide();  Display hidden line image
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
#include "../../WP/include/WP.h"
#include "../../GE/include/GE.h"
#include <math.h>


/*
***Defines for Hide.
*/

#define MAXPBL  1000               /* Max antal minnesblock för Plan-data */
#define PBLSIZ  100*sizeof(GPBPL)  /* Blockens storlek i bytes < 32K */
#define MAXPLN 100000              /* Max antal plan = PBLSIZ/sizeof(GPBPL) */
                                   /* gånger max antal block */

#define SPLMAX   100               /* Max antal split-delar */

#define SYNLIG     0               /* Kod för synlig vektor */
#define OSYNLIG    1               /* Kod för osynlig vektor */
#define SPLIT2     2               /* Kod för 2D-delad vektor */
#define SPLIT3     3               /* Kod för 3D-delad vektor */
#define KLIPPT     4               /* Kod för klippt vektor */
#define UTANFR     0               /* Pos 2D-utanför plan */
#define INUTI      1               /* Pos 2D-inuti plan */
#define HITOM      1               /* Kod för 3D-hitom */
#define BAKOM      0               /* Kod för 3D-bakom */
#define NOEDGE     0               /* Ingen rand */
#define EDGE1      1               /* Rand 1 */
#define EDGE2      2               /* Rand 2 */
#define EDGE3      3               /* Rand 3 */
#define EDGE4      4               /* Rand 4 */


/*
***A hiding plane.
*/
typedef struct gpbpl
{
DBVector p1,p2,p3,p4,nv;
double   dx1,dx2,dx3,dx4;
double   dy1,dy2,dy3,dy4;
double   l1,l2,l3,l4;
double   k1,k2,k3,k4;
double   xmin,xmax,ymin,ymax,zmax;
short    pen;
tbool    blank;
tbool    triangle;
} GPBPL;


/*
***Function prototypes.
*/
static short gpdrhd();
static void  hidply();
static short gpgnpd();
static short gpcl2d(double *xv, double *yv, GPBPL *pl);
static void  gpsvp2(GPBPL *pl, int end);
static void  gpspl1(GPBPL *pl, int end);
static short gpspl2(GPBPL *pl);
static void  gpsvp3(GPBPL *pl, DBVector *ps);
static void  gpspl3(DBVector *ps, int sida1);
static void  gpsvp4(GPBPL *plr, GPBPL *plt, DBVector *ps);
static void  pp_cut(int i);
static void  pp_clip(GPBPL *pl2);
static void  add_bnd(GPBPL *pl1, GPBPL *pl2);
static int   clip(int ip);
static void  rita();
static bool  clipsc(int iv);
static short mk_gpbpl(GMBPL *bpl, GPBPL *plan);
static void  gen2db(GPBPL *pln);
static void  sortz(int np, GPBPL **ppek);
static void  cutpl(GPBPL *pl, int *n, double u[]);
static short klptst(double *v, double *w, double *t1, double *t2);
static short gpplbp(GMBPL *bplpek, int *n);

/*
***Common variables.
*/

static GPBPL *pblpek[MAXPBL];
static int    pblant,pblofs;

/*
***MAXPBL är max antal primärminnesblock a c:a 32000 bytes
***som kan malloceras av gpgnpd() för Plan-data.
***pblpek är pekare till dessa block och pblant är antal
***block som allokerats. pblofs är antal använda.
*/

static GPBPL **ppek;
static int     np;

/*
***ppek är en pekare till en array av plan-pekare. Minne
***för denna malloceras av gpgnpd(). Varje plan-pekare
***pekar på plan-data för ett GPBPL-plan.
***np är antal plan-pekare, dvs. antal plan.
*/


static double  clipv[6];
static double *px1 = &clipv[0];
static double *px2 = &clipv[1];
static double *py1 = &clipv[2];
static double *py2 = &clipv[3];
static double *pz1 = &clipv[4];
static double *pz2 = &clipv[5];

/*
***I clipv lagras den vektor som just nu skickats till clip().
***px1,px2,py1,py2,pz1 och pz2 är pekare till de 6 koordinaterna.
*/


static double *spx,*spy,*spz;
static int     nsplit;
static int     spp[SPLMAX];

/*
***Pekare till koordinater för delar av vektor som
***blivit över efter split. nsplit = antal split.
***Minne allokeras av WPhide(). spp innehåller pekare
***till det plan där split-delen skall börja testas igen.
*/


static int      nsk,edge;
static int      edsk[2];
static GPBPL   *plsk[2];
static DBVector psk[2];

/*
***När ett plans rand skär igenom ett annat plan lagras
***skärningspunktens koordinater i psk, planets C-pekare
***i plsk, randens ordningsnummer i edsk och antal
***skärningar i nsk. edge är aktiv vektors edge-ordnings-
***nummer.
*/

static double  inutol;                         /* Tol for 2D in/out classification */
static bool    invbpl;                         /* Visibility of last plane */
static bool    screen,gksfil;                  /* Output to window/file */
static METADEF md;                             /* GKS definition record */
static FILE   *gksfp;                          /* GKS metafile */
static double  x[PLYMXV],y[PLYMXV],z[PLYMXV];  /* Polyline coordinates */
static char    a[PLYMXV];                      /* and status */
static int     ncrdxy;                         /* Antal vektorer i polylinje */
static WPGWIN *actwin;                         /* Output window */
static WPVIEW  pltvy;                          /* Plot window */

/*!******************************************************/

        short     WPhide(
        WPGWIN   *gwinpt,
        bool      flag1,
        bool      flag2,
        FILE     *pfil,
        DBVector *origo)

/*      Display model with hidden lines removed.
 *
 *      In: gwinpt = Ptr to WPGWIN
 *          flag1  = TRUE if image should be displayed in window.
 *          flag2  = TRUE if image should be written to disc.
 *          pfil   = If flag2 = TRUE, ptr to open file.
 *          origo  = Image origin relative to window.
 *
 *      Felkoder: GP0102 = Kan ej allokera minne för split
 *
 *      (C)microform ab 29/1/89 J. Kjellander
 *
 *      29/5/91 origo, J. Kjellander
 *      2006-12-28 Removed GP, J.Kjellander
 *
 ******************************************************!*/

 {
   char   buf[80];
   char   metarec[MAXMETA];
   short  status;
   int    i;

/*
***Initiering.
*/
   screen = flag1; gksfil = flag2;
   gksfp  = pfil;
   actwin = gwinpt;
   pltvy.modwin.xmin = gwinpt->vy.modwin.xmin;
   pltvy.modwin.ymin = gwinpt->vy.modwin.ymin;
   pltvy.modwin.xmax = gwinpt->vy.modwin.xmax;
   pltvy.modwin.ymax = gwinpt->vy.modwin.ymax;
/*
***Börja med att skapa Plan-data.
*/
   IGptma(155,IG_MESS); status = gpgnpd(); IGrsma();
   if ( status < 0 ) return(status);
   sprintf(buf,"%s%d",IGgtts(156),np); IGplma(buf,IG_MESS);
/*
***Allokera minne för SPLMAX split-delar. 2 koordinater per del.
*/
   if ( (spx=(double *)v3mall(2*SPLMAX*sizeof(double),"WPhide")) == NULL )
     return(erpush("GP0010",""));
   if ( (spy=(double *)v3mall(2*SPLMAX*sizeof(double),"WPhide")) == NULL )
     return(erpush("GP0010",""));
   if ( (spz=(double *)v3mall(2*SPLMAX*sizeof(double),"WPhide")) == NULL )
     return(erpush("GP0010",""));
/*
***Om det skall ritas i fönster, sudda först.
*/
   if ( screen ) WPergw(gwinpt->id.w_id);
/*
***Om det skall plottas, initiera GKS-metafil.
*/
   if ( gksfil )
     {
     if ( (status=WPgksm_header(&md,gksfp,metarec)) < 0 ) goto end;
     if ( (status=WPgksm_clear(&md,gksfp,metarec)) < 0 ) goto end;
     if ( (status=WPgksm_window(&md,gksfp,metarec,&pltvy,origo)) < 0 ) goto end;
     }
/*
***Rita storheter skymt.
*/
   status = gpdrhd();
/*
***Avsluta ev. plottning.
*/
   if ( gksfil ) WPgksm_end(&md,gksfp,metarec);
/*
***Lämna tillbaks allokerat minne.
*/
end:
   v3free(spx,"WPhide"); v3free(spy,"WPhide");
   v3free(spz,"WPhide"); v3free(ppek,"WPhide");
   for ( i=0; i<pblant; ++i ) v3free(pblpek[i],"WPhide");
/*
***Slut.
*/
   return(status);
 }

/********************************************************/
/*!******************************************************/

 static short gpdrhd()

/*      Ritar storheter med skymda delar borttagna.
 *      Plan-data har genererats tidigare.
 *
 *      FV:  0     = Ok.
 *          AVBRYT = Avbrott från tangentbordet.
 *
 *      (C)microform ab 29/1/89 J. Kjellander
 *
 *      21/3/91    intrup, J. Kjellander
 *      10/1/92    Streckade linjer, J. Kjellander
 *      14/3/92    GPBPL.pen, J. Kjellander
 *      1999-11-24 Text, J.Kjellander
 *      2007-01-09 pborder, piso,   Sören L
 *
 ******************************************************!*/

 {
   short     curpen;
   int       i,k,nbl;
   char      str[V3STRLEN+1],metarec[MAXMETA];
   double    size,scale;
   DBetype   type;
   DBptr     la;
   DBfloat   xhcrds[4*GMXMXL];
   DBAny     gmpost;
   DBTmat    pmat;
   DBSeg    *segptr,arcseg[4];
   DBSegarr *pborder, *piso;

/*
***Div. initiering.
*/
   curpen = -1;
/*
***Gå igenom GM efter 3D-trådobjekt. B-plan väntar vi med eftersom det går
***fortare att ta dom direkt från ppek[] och denna dessutom bara innehåller
***hitåtvända plan.
*/
   DBget_pointer('F',NULL,&la,&type);
loop:
   while ( DBget_pointer('N',NULL,&la,&type ) == 0 )
     {
     k = -1;
     ncrdxy = 0;
     switch ( type )
       {
/*
***Point, size = 1.5% of window size.
*/
       case POITYP:
       DBread_point(&gmpost.poi_un,la);
       if ( !gmpost.hed_un.blank  &&  WPnivt(actwin->nivtab,gmpost.hed_un.level) )
         {
         size  = (double)0.015*(pltvy.modwin.xmax - pltvy.modwin.xmin);
         WPplpt(&gmpost.poi_un,size,&k,x,y,z,a);
         }
       break;
/*
***Line.
*/
       case LINTYP:
       DBread_line(&gmpost.lin_un,la);
       if ( !gmpost.hed_un.blank  &&  WPnivt(actwin->nivtab,gmpost.hed_un.level) )
         {
         WPplli(&gmpost.lin_un,&k,x,y,z,a);
         }
       break;
/*
***Arc.
*/
       case ARCTYP:
       DBread_arc(&gmpost.arc_un,arcseg,la);
       if ( !gmpost.hed_un.blank  &&  WPnivt(actwin->nivtab,gmpost.hed_un.level) )
         {
         WPplar(&gmpost.arc_un,arcseg,scale,&k,x,y,z,a);
         }
       break;
/*
***Curve.
*/
       case CURTYP:
       DBread_curve(&gmpost.cur_un,&segptr,NULL,la);
       if ( !gmpost.hed_un.blank  &&  WPnivt(actwin->nivtab,gmpost.hed_un.level) )
         {
         WPplcu(&gmpost.cur_un,segptr,scale,&k,x,y,z,a);
         }
       DBfree_segments(segptr);
       break;
/*
***Surface. Facet surfaces are processed as B_planes.
*/
       case SURTYP:
       DBread_surface(&gmpost.sur_un,la);
       if ( !gmpost.hed_un.blank  &&  WPnivt(actwin->nivtab,gmpost.hed_un.level) )
         {
         if ( gmpost.sur_un.typ_su != FAC_SUR )
           {
           DBread_sur_grwire(&gmpost.sur_un,&pborder,&piso);
           WPplsu(&gmpost.sur_un,pborder,piso,scale,&k,x,y,z,a);
           DBfree_sur_grwire(&gmpost.sur_un,pborder,piso);
           }
         }
       break;
/*
***Coordinate system. Size should be approx 10% of output media.
*/
       case CSYTYP:
       DBread_csys(&gmpost.csy_un,&pmat,la);
       if ( !gmpost.hed_un.blank  &&  WPnivt(actwin->nivtab,gmpost.hed_un.level) )
         {
         size = (double)0.1*(pltvy.modwin.xmax - pltvy.modwin.xmin);
         WPplcs(&gmpost.csy_un,size,V3_CS_NORMAL,&k,x,y,z,a);
         }
       break;
/*
***B_planes are handled separately.
*/
       case BPLTYP:
       goto loop;
/*
***Mesh ???
*/
       case MSHTYP:
       DBread_mesh(&gmpost.msh_un,la,MESH_HEADER+MESH_VERTEX+MESH_HEDGE);
       if ( !gmpost.hed_un.blank  &&  WPnivt(actwin->nivtab,gmpost.hed_un.level) )
         {
         size  = (double)0.0075*(pltvy.modwin.xmax - pltvy.modwin.xmin);
         WPplms(&gmpost.msh_un,size,&k,x,y,z,a);
         }
       break;
/*
***Text.
*/
       case TXTTYP:
       DBread_text(&gmpost.txt_un,str,la);
       if ( !gmpost.hed_un.blank  &&  WPnivt(actwin->nivtab,gmpost.hed_un.level) )
         {
         WPpltx(&gmpost.txt_un,str,&k,x,y,z,a);
         }
       break;
/*
***Linear dimension.
*/
       case LDMTYP:
       DBread_ldim(&gmpost.ldm_un,la);
       if ( !gmpost.hed_un.blank  &&  WPnivt(actwin->nivtab,gmpost.hed_un.level) )
         {
         WPplld(&gmpost.ldm_un,&k,x,y,z,a);
         }
       break;
/*
***Diameter dimension.
*/
       case CDMTYP:
       DBread_cdim(&gmpost.cdm_un,la);
       if ( !gmpost.hed_un.blank  &&  WPnivt(actwin->nivtab,gmpost.hed_un.level) )
         {
         WPplcd(&gmpost.cdm_un,&k,x,y,z,a);
         }
       break;
/*
***Radius dimension.
*/
       case RDMTYP:
       DBread_rdim(&gmpost.rdm_un,la);    
       if ( !gmpost.hed_un.blank  &&  WPnivt(actwin->nivtab,gmpost.hed_un.level) )
         {
         WPplrd(&gmpost.rdm_un,&k,x,y,z,a);
         }
       break;
/*
***Angular dimension.
*/
       case ADMTYP:
       DBread_adim(&gmpost.adm_un,la);
       if ( !gmpost.hed_un.blank  &&  WPnivt(actwin->nivtab,gmpost.hed_un.level) )
         {
         WPplad(&gmpost.adm_un,scale,&k,x,y,z,a);
         }
       break;
/*
***Hatch.
*/
       case XHTTYP:
       DBread_xhatch(&gmpost.xht_un,xhcrds,la);
       if ( !gmpost.hed_un.blank  &&  WPnivt(actwin->nivtab,gmpost.hed_un.level) )
         {
         WPplxh(&gmpost.xht_un,xhcrds,&k,x,y,z,a);
         }
       break;
/*
***Part and group.
*/
       case PRTTYP:
       case GRPTYP:
       goto loop;
/*
***Unknown (not yet? supported) entity.
*/
       default:
       goto loop;
       }
/*
***Sätt rätt penna och rita storheten.
*/
     ncrdxy = k+1;

     if ( ncrdxy > 0 )
       {
       if ( gmpost.hed_un.pen != curpen )
         {
         curpen = gmpost.hed_un.pen;
         if ( screen ) WPspen(curpen);
         if ( gksfil ) WPgksm_pen(&md,gksfp,curpen,metarec);
         }
       WPpply(actwin,k,x,y,z);
       hidply();
       }
     }
/*
***ppek[] innehåller alla hitåtvända och ej blankade plan. Dom
***kan dock fortfarande ligga på en släckt nivå och skall då
***inte visas.
*/
   a[0] = 0; a[1] = a[2] = a[3] = a[4] = VISIBLE;

   for ( i=0; i<np; ++i )
     {
     if ( (*(ppek+i))->blank == FALSE )
       {
/*
***Sätt rätt penna. Flusha först så sist ritade vektor kommer ut
***i rätt färg. OBS WPspen() borde göra gpflsh() innan pennan ändras!!!!!!!!!
***Hur funkar detta för trådstorheter ovan ????? Kanske bug !!
***OBS åxå att rita-rutinen kan göra direkta anrop till (X-)Windows.
***Borde gå lite fortare.
*/
       if ( (*(ppek+i))->pen != curpen )
         {
         curpen = (*(ppek+i))->pen;
         if ( screen ) WPspen(curpen);
         if ( gksfil ) WPgksm_pen(&md,gksfp,curpen,metarec);
         }
/*
***Bygg polylinje.
*/
       ncrdxy = 0;

       x[ncrdxy]   = (*(ppek+i))->p1.x_gm; y[ncrdxy] = (*(ppek+i))->p1.y_gm;
       z[ncrdxy++] = (*(ppek+i))->p1.z_gm;

       x[ncrdxy]   = (*(ppek+i))->p2.x_gm; y[ncrdxy] = (*(ppek+i))->p2.y_gm;
       z[ncrdxy++] = (*(ppek+i))->p2.z_gm;

       if ( (*(ppek+i))->l2 > 0.0 )
         {
         x[ncrdxy]   = (*(ppek+i))->p3.x_gm; y[ncrdxy] = (*(ppek+i))->p3.y_gm;
         z[ncrdxy++] = (*(ppek+i))->p3.z_gm;
         }

       if ( (*(ppek+i))->l3 > 0.0 )
         {
         x[ncrdxy]   = (*(ppek+i))->p4.x_gm; y[ncrdxy] = (*(ppek+i))->p4.y_gm;
         z[ncrdxy++] = (*(ppek+i))->p4.z_gm;
         }

       x[ncrdxy]   = (*(ppek+i))->p1.x_gm; y[ncrdxy] = (*(ppek+i))->p1.y_gm;
       z[ncrdxy++] = (*(ppek+i))->p1.z_gm;
/*
***Rita skymt. Innan vi ritar planet sätter sätter vi en flagga.
***Om någon del av planet är synlig kommer flaggan vid återkomsten
***från hidply att vara sänkt.
*/       
       invbpl = TRUE;
       hidply();
/*
***Om planet är helt osynligt kan det inte heller skymma något annat
***plan (eller trådgeometri heller för den delen) eller ge upphov till
***några synliga ränder så då kan vi lika gärna betrakta det som släckt 
***i fortsättningen. Diverse prov visar dock att detta inte spar någon
***tid eftersom det praktiskt taget aldrig inträffar med Z-sortering
***och 2D-box-test.
*/
       if ( invbpl )
         {
         (*(ppek+i))->blank = TRUE;
         }
       }
     }
/*
 ***Ett litet test.
 */
    nbl = 0;
    for ( i=0; i<np; ++i )
      {
      if ( (*(ppek+i))->blank ) ++nbl;
      }
/*
***Skapa plan/plan-skärningar och rita skymt. Alla plan
***utom det sista testas mot övriga. Det sista blir ju
***ändå testat av alla andra.
*/
   for ( i=0; i<np-1; ++i )
     {
/*
***Om ppek->blank = FALSE är planet tänt (tänd nivå etc.). Bortvända
***och blankade (BLANK=1) plan är ju redan borsorterade. Det kan
***också ha konstaterats osynligt i loopen ovanför.
*/
     if ( (*(ppek+i))->blank == FALSE )
       {
/*
***Sätt rätt penna.
*/
       if ( (*(ppek+i))->pen != curpen )
         {
         curpen = (*(ppek+i))->pen;
         if ( screen ) WPspen(curpen);
         if ( gksfil ) WPgksm_pen(&md,gksfp,curpen,metarec);
         }
/*
***Beräkna alla skärningar mellan detta plans rand och andra plan.
***Under denna beräkning kommer nya ränder att testas med hidply()
***som ju tittar på ncrdxy och aÄÅ. För att slippa sätt dom till
***rätt värde för varenda rand gör vi det en gång för alla här.
*/
       a[0] = 0; a[1] = VISIBLE;
       ncrdxy = 2;
       pp_cut(i);
       }
     }
/*
***Slut.
*/
   XFlush(xdisp);

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static void pp_cut(
        int  i)

/*      Testar vilka av alla plan som ett visst plans rand
 *      skär igenom. Beräknar och lagrar skärningarna.
 *      
 *      In: i = Plan att testa.
 *
 *      (C)microform ab 1996-01-25 J. Kjellander
 *
 ******************************************************!*/

 {
   int    start,stop,ip;
   GPBPL *pl1,*pl2;

/*
***Planet i behöver bara testas mot planen i+1, i+2 osv.
***eftersom det redan testats mot tidigare plan i-1, i-2 etc.
*/
   start = i+1;
/*
***Sista planet att testa får tills vidare bli det sista vi
***har men Z-test kan minska denna siffra. För närvarande lagras
***bara planens Z-max, ej Z-min så en test här skulle ta lika
***lång tid som att beräkna Z-min redan från början. Det gör vi
***nån annan gång.
*/
   stop = np;
 /*
 ***Planet i skall nu testas mot planen start till stop.
 ***Planet stop testas ej eftersom detta ej finns.
 */
   start = 0; stop = np;

   for ( ip=start; ip<stop; ++ip )
     {
     pl1 = ppek[i];
     pl2 = ppek[ip];
/*
***Plan som är blankade kan inte ge upphov till några
***nya ränder. Båda måste ju vara synliga.
*/
     if ( pl2->blank ) ;
/*
***Gör 2D-box test.
*/
     else if ( pl1->xmax <= pl2->xmin ) ; 
     else if ( pl1->xmin >= pl2->xmax ) ;
     else if ( pl1->ymax <= pl2->ymin ) ;
     else if ( pl1->ymin >= pl2->ymax ) ;
/*
***Risk för skärning finns, testa ränderna var för sig.
***Globala variabeln nsk håller antalet skärningar.
*/
     else
       {
       nsk = 0;

      *px1 = pl1->p1.x_gm; *py1 = pl1->p1.y_gm; *pz1 = pl1->p1.z_gm;
      *px2 = pl1->p2.x_gm; *py2 = pl1->p2.y_gm; *pz2 = pl1->p2.z_gm;
       edge = EDGE1;
       pp_clip(pl2);

       if ( pl1->l2 > 0.0 )
         {
        *px1 = *px2; *py1 = *py2; *pz1 = *pz2;
        *px2 = pl1->p3.x_gm; *py2 = pl1->p3.y_gm; *pz2 = pl1->p3.z_gm;
         edge = EDGE2;
         pp_clip(pl2);
         }

       if ( pl1->l3 > 0.0 )
         {
        *px1 = *px2; *py1 = *py2; *pz1 = *pz2;
        *px2 = pl1->p4.x_gm; *py2 = pl1->p4.y_gm; *pz2 = pl1->p4.z_gm;
         edge = EDGE3;
         pp_clip(pl2);
         }

      *px1 = *px2; *py1 = *py2; *pz1 = *pz2;
      *px2 = pl1->p1.x_gm; *py2 = pl1->p1.y_gm; *pz2 = pl1->p1.z_gm;
       edge = EDGE4;
       pp_clip(pl2);
/*
***Om någon eller några (1 eller 2) av pl1:s ränder skär
***pl2 skapar vi en ny rand.
*/
       if ( nsk > 0 ) add_bnd(pl1,pl2);
       }
     }
 }

/********************************************************/
/*!******************************************************/

 static void   pp_clip(
        GPBPL *pl2)

/*      Testar ett plans ränder mot ett annat plan och
 *      beräknar ev. skärningar. Denna rutin är en variant
 *      av rutinen clip() som används för trådgeometri.
 *
 *      In: pl1 = Pekare till planet vars ränder skall testas.
 *          pl2 = Pekare till plan att testa emot.
 *
 *      (C)microform ab 1996-01-25 J. Kjellander
 *
 ******************************************************!*/

 {
#define HITTOL -0.0015    /* Tolerens för HITOM/BAKOM */
#define BAKTOL  0.0015    /* Tolerens för HITOM/BAKOM */
#define INTOL  -0.0015    /* Tolerens för INUTI */
#define UTTOL   0.0015    /* Tolerens för UTANFR */

  int    sida1,sida2,state;
  DBVector  ps;

/*
***3D-klassificering! Se rutinen cut().
*/
  if ( (*px1 - pl2->p1.x_gm)*pl2->nv.x_gm +
       (*py1 - pl2->p1.y_gm)*pl2->nv.y_gm +
       (*pz1 - pl2->p1.z_gm)*pl2->nv.z_gm  < HITTOL ) sida1 = BAKOM;
  else sida1 = HITOM;

  if ( (*px2 - pl2->p1.x_gm)*pl2->nv.x_gm +
       (*py2 - pl2->p1.y_gm)*pl2->nv.y_gm +
       (*pz2 - pl2->p1.z_gm)*pl2->nv.z_gm  < HITTOL ) sida2 = BAKOM;
  else sida2 = HITOM;
/*
***Om båda ändpunkterna är hitom kan inte vektorn skära planet.
*/
  if ( sida1 == HITOM  &&  sida2 == HITOM ) return;
/*
***Om istället båda ligger bakom kan vektorn inte heller skära
***genom planet.
*/
  else if ( sida1 == BAKOM  &&  sida2 == BAKOM ) return;
/*
***Det verkar som om vektorns ändpunkter ligger på var
***sin sida om planet. För att vara riktigt säkra testar
***vi HITOM-punkten igen men mot ett plan som ligger strax
***hitom det verkliga planet. Om punkten då hamnar bakom
***har vi kontakt och hela vektorn kan anses ligga bakom.
*/
  else
    {
    if ( sida1 == HITOM )
      {
      if ( (*px1 - pl2->p1.x_gm)*pl2->nv.x_gm +
           (*py1 - pl2->p1.y_gm)*pl2->nv.y_gm +
           (*pz1 - pl2->p1.z_gm)*pl2->nv.z_gm  < BAKTOL ) return;
      }
    else
      {
      if ( (*px2 - pl2->p1.x_gm)*pl2->nv.x_gm +
           (*py2 - pl2->p1.y_gm)*pl2->nv.y_gm +
           (*pz2 - pl2->p1.z_gm)*pl2->nv.z_gm  < BAKTOL ) return;
      }
/*
***Vektorns ändpunkter ligger klart på var sin sida om planet.
***Alltså finns det en punkt på vektorn som skär igenom
***planets oändliga plan, beräkna denna.
*/
    gpsvp3(pl2,&ps);
/*
***Om 2D-klassning av skärningspunkten visar att denna ligger
***inuti planet har vi sann skärning. Lagra skärningen och räkna
***upp nsk.
*/
    inutol = INTOL;
    state = gpcl2d(&ps.x_gm,&ps.y_gm,pl2);
    if ( state == INUTI )
      {
      if ( nsk < 2 )
        {
        edsk[nsk] = edge;
        plsk[nsk] = pl2;
        V3MOME(&ps,&psk[nsk],sizeof(DBVector));
      ++nsk;
        }
      }
/*
***Vektorn skär inte igenom planet.
*/
    else return;
    }
 }

/********************************************************/
/*!******************************************************/

 static void   add_bnd(
        GPBPL *pl1,
        GPBPL *pl2)

/*      Genererar plan/plan-skärningar.
 *
 *      In: pl1,pl2 = Pekare till planen.
 *
 *      FV: Void.
 *
 *      (C)microform ab 20/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
   int    ed;
   double dx,dy,xt,yt,tt;
   DBVector  ps1,ps;
   DBVector *pa,*pb;


/*
***Om det är två skärningar i planet är det enkelt, förbind
***skärningarna.
*/
   if ( nsk == 2 )
     { 
     x[0] = psk[0].x_gm; y[0] = psk[0].y_gm; z[0] = psk[0].z_gm;
     x[1] = psk[1].x_gm; y[1] = psk[1].y_gm; z[1] = psk[1].z_gm;
     hidply();
     }
/*
***En ensam skärning. Då blir det värre.
*/
   else
     {
/*
***Beräkna en skärning till, ps.
*/
     gpsvp4(pl1,pl2,&ps);
/*
***Använd ps till att skapa en oändligt lång linje ps1-ps
***riktad längs skärningen mellan planen och in mot rit-planet.
*/
     ed = edsk[0];

     if      ( ed == EDGE1 )
       { pa = &pl1->p1; pb = &pl1->p2; }
     else if ( ed == EDGE2 )
       { pa = &pl1->p2; pb = &pl1->p3; }
     else if ( ed == EDGE3 )
       { pa = &pl1->p3; pb = &pl1->p4; }
     else
       { pa = &pl1->p4; pb = &pl1->p1; }

     dx = pb->y_gm - pa->y_gm; dy = pa->x_gm - pb->x_gm;
     xt = ps.x_gm - pa->x_gm;  yt = ps.y_gm - pa->y_gm;
     tt = (dx*xt + dy*yt)/SQRT(dx*dx + dy*dy);

     V3MOME(&psk[0],&ps1,sizeof(DBVector));
     ps.x_gm -= ps1.x_gm; ps.y_gm -= ps1.y_gm; ps.z_gm -= ps1.z_gm;
     GEnormalise_vector3D(&ps,&ps);

     if ( tt < 0.0 )
       {
       ps.x_gm = ps1.x_gm + 1e8*ps.x_gm;
       ps.y_gm = ps1.y_gm + 1e8*ps.y_gm;
       ps.z_gm = ps1.z_gm + 1e8*ps.z_gm;
       }
     else
       {
       ps.x_gm = ps1.x_gm - 1e8*ps.x_gm;
       ps.y_gm = ps1.y_gm - 1e8*ps.y_gm;
       ps.z_gm = ps1.z_gm - 1e8*ps.z_gm;
       }
/*
***Klipp skärningslinjen mot testplanet.
*/
     *px1 = ps1.x_gm; *py1 = ps1.y_gm; *pz1 = ps1.z_gm;
     *px2 = ps.x_gm; *py2 = ps.y_gm; *pz2 = ps.z_gm;
     x[0] = *px1; y[0] = *py1; z[0] = *pz1;
     gpsvp2(pl2,1);
     x[1] = *px1; y[1] = *py1; z[1] = *pz1;
     hidply();
     }
/*
***Slut.
*/
   return;
 }

/********************************************************/
/*!******************************************************/

 static void hidply()

/*      Ritar polyline skymt.
 *
 *      (C)microform ab 6/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
   register int ip,iv,st;
   int nt,nv,state;

/*
***Testa alla polylinjens Z-koordinater mot alla plans
***Z-max och avgör hur många plan - nt, som polylinjen
***måste klippas mot.
*/
   nt = 0;
   for ( iv=0; iv<ncrdxy; ++iv )
     {
     ip = 0;
     while ( ip<np  &&  z[iv] < (*(ppek+ip))->zmax ) ++ip;
     if ( ip > nt ) nt = ip;
     }
/*
***Testa polylinjens vektorer mot de nt första
***planen. Börja med 2D-boxar.
*/
   nv = ncrdxy - 1;
   for ( iv=0; iv<nv; ++iv )
     {
     if ( (a[iv+1] & VISIBLE) == VISIBLE  &&  clipsc(iv) )
       {
       *px1 = x[iv]; *px2 = x[iv+1];
       *py1 = y[iv]; *py2 = y[iv+1];
       *pz1 = z[iv]; *pz2 = z[iv+1];
       state = SYNLIG; nsplit = 0; st = 0;
start:
       for ( ip=st; ip<nt; ++ip )
         {
         if ( *px1 <= (*(ppek+ip))->xmin  &&
                     *px2 <= (*(ppek+ip))->xmin ) ;
         else if ( *px1 >= (*(ppek+ip))->xmax  &&
                     *px2 >= (*(ppek+ip))->xmax ) ;
         else if ( *py1 <= (*(ppek+ip))->ymin  &&
                     *py2 <= (*(ppek+ip))->ymin ) ;
         else if ( *py1 >= (*(ppek+ip))->ymax  &&
                     *py2 >= (*(ppek+ip))->ymax ) ;
/*
***En vektor i polylinjen skyms av en 2D-box. Gör fullständig
***klipp-test.
*/
         else
           {
           state = clip(ip); 
           switch ( state )
             {
             case OSYNLIG:
             goto nxtvec;

             case SPLIT2:
             spp[nsplit] = ip+1;
             ++nsplit;
             state = SYNLIG;
             break;

             case SPLIT3:
             spp[nsplit] = ip;
             ++nsplit;
             state = SYNLIG;
             break;
             }
           }
         }
/*
***Vektorn har testats mot alla plan, skall det ritas något ?
*/
      if ( state == SYNLIG ) rita();
/*
***Finns det split-delar att testa.
*/
nxtvec:
       if ( nsplit > 0 )
         {
         --nsplit;
         *px1 = *(spx+2*nsplit); *px2 = *(spx+2*nsplit+1);
         *py1 = *(spy+2*nsplit); *py2 = *(spy+2*nsplit+1);
         *pz1 = *(spz+2*nsplit); *pz2 = *(spz+2*nsplit+1);
         st = spp[nsplit];
         state = SYNLIG;
         goto start;
         }
       }
     }
 }

/********************************************************/
/*!******************************************************/

 static int clip(
        int ip)

/*      Klipper vektor mot plan.
 *
 *      In:  ip = Offset till planets adress i ppek.
 *
 *      (C)microform ab 6/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
#define HITTOL -0.0015    /* Tolerens för HITOM/BAKOM */
#define BAKTOL  0.0015    /* Tolerens för HITOM/BAKOM */
#define INTOL  -0.0015    /* Tolerens för INUTI */
#define UTTOL   0.0015    /* Tolerens för UTANFR */

  int    sida1,sida2,end1,end2,ends;
  DBVector  ps;
  GPBPL *pl;

/*
***Planets adress.
*/
   pl = *(ppek+ip);
/*
***3D-klassificering! På vilken sida om planet ligger
***vektorns ändpunkter. Med HITTOL testar vi egentligen
***mot ett plan som ligger strax bakom det verkliga planet,
***detta för att säkert klassa positioner i planets yta som
***HITOM.
*/
  if ( (*px1 - pl->p1.x_gm)*pl->nv.x_gm +
       (*py1 - pl->p1.y_gm)*pl->nv.y_gm +
       (*pz1 - pl->p1.z_gm)*pl->nv.z_gm  < HITTOL ) sida1 = BAKOM;
  else sida1 = HITOM;

  if ( (*px2 - pl->p1.x_gm)*pl->nv.x_gm +
       (*py2 - pl->p1.y_gm)*pl->nv.y_gm +
       (*pz2 - pl->p1.z_gm)*pl->nv.z_gm  < HITTOL ) sida2 = BAKOM;
  else sida2 = HITOM;
/*
***Om båda ändpunkterna är hitom kan planet inte skymma vektorn.
*/
start:
  if ( sida1 == HITOM  &&  sida2 == HITOM ) return(SYNLIG);
/*
***Om istället båda ligger bakom kan inte vektorn skära igenom
***planet och problemet kan lösas i 2 dimensioner. Börja med
***att 2D-klassa vektorns ändpunkter mot ett plan som är aningen
***större än det verkliga. För att en punkt skall anses ligga
***utanför måste den alltså ligga säkert utanför, inte på randen tex.
*/
  else if ( sida1 == BAKOM  &&  sida2 == BAKOM )
    {
    inutol = UTTOL;
    end1 = gpcl2d(px1,py1,pl);
    end2 = gpcl2d(px2,py2,pl);
/*
***Om båda ändarna ligger bakom och dom dessutom ligger approx.
***inuti planets 2D-projektion kan inte vektorn vara synlig.
*/
    if ( end1 == INUTI  &&  end2 == INUTI ) return(OSYNLIG);
/*
***Om båda ändarna ligger utanför är vektorn antingen synlig
***eller också är det frågan om ett split.
*/
    else if ( end1 == UTANFR  &&  end2 == UTANFR )
      return(gpspl2(pl));
/*
***Om bara ena änden ligger inuti, klassa den igen mot ett mindre plan
***för att säkert konstatera att den ligger inuti. Om så är fallet
***ligger ena punkten säkert utanför och andra punkten säkert inuti
***planet och klippning borde gå bra.
*/
    else
      {
      inutol = INTOL;
      if ( end1 == INUTI )
        {
        end1 = gpcl2d(px1,py1,pl);
        if ( end1 == INUTI ) gpsvp2(pl,1);
        else gpspl1(pl,1);
        }
      else
        {
        end2 = gpcl2d(px2,py2,pl);
        if ( end2 == INUTI ) gpsvp2(pl,2);
        else gpspl1(pl,2);
        }
      return(SYNLIG);
      }
    } 
/*
***Det verkar som om vektorns ändpunkter ligger på var
***sin sida om planet. För att vara riktigt säkra testar
***vi HITOM-punkten igen men mot ett plan som ligger strax
***hitom det verkliga planet. Om punkten då hamnar bakom
***har vi kontakt och hela vektorn kan anses ligga bakom.
*/
  else
    {
    if ( sida1 == HITOM )
      {
      if ( (*px1 - pl->p1.x_gm)*pl->nv.x_gm +
           (*py1 - pl->p1.y_gm)*pl->nv.y_gm +
           (*pz1 - pl->p1.z_gm)*pl->nv.z_gm  < BAKTOL )
        { sida1 = BAKOM; goto start; }
      }
    else
      {
      if ( (*px2 - pl->p1.x_gm)*pl->nv.x_gm +
           (*py2 - pl->p1.y_gm)*pl->nv.y_gm +
           (*pz2 - pl->p1.z_gm)*pl->nv.z_gm  < BAKTOL )
        { sida2 = BAKOM; goto start; }
      }
/*
***Vektorns ändpunkter ligger klart på var sin sida om planet.
***Alltså finns det en punkt på vektorn som skär igenom
***planets oändliga plan, beräkna denna.
*/
    gpsvp3(pl,&ps);
/*
***Om 2D-klassning av skärningspunkten visar att denna ligger
***inuti planet delar vi vektorn i två delar, en som ligger helt
***framför och en som ligger helt bakom planet. Delen som ligger
***framför är helt synlig och delen som ligger bakom får behandlas
***igen. Lagra den som en split-del.
*/
    inutol = INTOL;
    ends = gpcl2d(&ps.x_gm,&ps.y_gm,pl);
    if ( ends == INUTI )
      {
      gpspl3(&ps,sida1);
      return(SPLIT3);
      }
/*
***Vektorn skär inte igenom planet. Om HITOM-änden ligger inuti
***planet är hela vektorn synlig.
*/
    else
      {
      if ( sida1 == HITOM ) end1 = gpcl2d(px1,py1,pl);
      else end1 = gpcl2d(px2,py2,pl);
      if ( end1 == INUTI ) return(SYNLIG);
/*
***Om HITOM-änden ligger utanför planet kan en del av vektorn
***skymmas av planet. Enda lösningen då är att göra spli, rita
***den del som ligger framför och testa bakom-delen igen.
*/
      else
        {
        gpspl3(&ps,sida1);
        return(SPLIT3);
        }
      }
    }
 }

/********************************************************/
/*!******************************************************/

 static void rita()

/*      Ritar aktiv vektor.
 *
 *      (C)microform ab 26/2/89 J. Kjellander
 *
 *      1996-01-25 invbpl, J. Kjellander
 *      2006-12-28 Removed GP, J.Kjellander
 *
 ******************************************************!*/

 {
   char metarec[MAXMETA];
   char av[2];
   int  ix1,iy1,ix2,iy2;

/*
***Sätt flaggan för osynligt b-plan till false.
*/
   invbpl = FALSE;
/*
***Rita på skärmen.
*/
   if ( screen )
     {
     ix1 = actwin->vy.k1x + actwin->vy.k2x*(*px1);
     iy1 = actwin->geo.dy - (actwin->vy.k1y + actwin->vy.k2y*(*py1));
     ix2 = actwin->vy.k1x + actwin->vy.k2x*(*px2);
     iy2 = actwin->geo.dy - (actwin->vy.k1y + actwin->vy.k2y*(*py2));

#ifdef UNIX
     XDrawLine(xdisp,actwin->id.x_id,actwin->win_gc,ix1,iy1,ix2,iy2);
     XDrawLine(xdisp,actwin->savmap,actwin->win_gc,ix1,iy1,ix2,iy2);
#endif

#ifdef WIN32
       Polyline(gwinpt->dc,ip,np);
       Polyline(gwinpt->bmdc,ip,np);
#endif
     }
/*
***Och till fil.
*/
   if ( gksfil )
     {
     av[0] = 0; av[1] = VISIBLE;
     WPgksm_polyline(&md,gksfp,(short)1,&clipv[0],&clipv[2],av,&pltvy,metarec);
     }
 }

/********************************************************/
/*!******************************************************/

 static bool clipsc(
        int  iv)

/*      Klipper vektorn i x,y,z mot skärmens kanter.
 *
 *      In: iv = Index till vektorns startpos i X,Y,Z.
 *
 *      (C)microform ab 6/4/89 J. Kjellander
 *
 ******************************************************!*/

 {
   double v[4],t1,t2,z1,z2;

   v[0] = x[iv]; v[1] = y[iv];
   v[2] = x[iv+1]; v[3] = y[iv+1];

   switch ( klptst(v,&pltvy.modwin.xmin,&t1,&t2) )
     {
     case -1:
     return(FALSE);

     case  0:
     return(TRUE);

     case  1:
     x[iv] = v[0]; y[iv] = v[1];
     z[iv] += t1*(z[iv+1] - z[iv]);
     return(TRUE);

     case  2:
     x[iv+1] = v[2]; y[iv+1] = v[3];
     z[iv+1] -= t2*(z[iv+1] - z[iv]);
     return(TRUE);

     case  3:
     x[iv] = v[0]; y[iv] = v[1];
     x[iv+1] = v[2]; y[iv+1] = v[3];
     z1 = z[iv] + t1*(z[iv+1] - z[iv]);
     z2 = z[iv] + (1.0-t2)*(z[iv+1] - z[iv]);
     z[iv] = z1;
     z[iv+1] = z2;
     return(TRUE);

     default:
     return(FALSE);
     }
 }

/********************************************************/
/*!******************************************************/

 static short gpgnpd()

/*      Går igenom GM och skapar GPBPL-plan. Ett för
 *      varje B_PLANE och många för varje FAC_SUR och MESH.
 * 
 *      Felkoder: GP0072 = Plandata kräver för många block
 *                GP0082 = Fel från malloc() för plandata
 *                GP0092 = Fel från malloc() för planpekare
 *
 *      (C)microform ab 23/1/89 J. Kjellander
 *
 *      13/3/92  Penna, J. Kjellander
 *      10/1/96  Ytor, J. Kjellander
 *      10/7/04  Mesh, J.Kjellander, Örebro university
 *
 ******************************************************!*/

 {
   short      status;
   int        i,j,edge_1,edge_2,edge_3,
              vertex_1,vertex_2,vertex_3;
   bool       niv_status;
   DBetype    type;
   DBptr      la;
   DBBplane   bpl;
   DBSurf     sur;
   DBMesh     mesh;
   GMPAT     *patpek,*toppat;
   GMPATF    *facpat;
   GPBPL      plan;

/*
***Div. initiering.
*/
   np = 0;
   pblant = 0;
   pblofs = 100;
/*
***Allokera minne för plan-pekare. Max antal plan = MAXPLN.
*/
   if ( (ppek=(GPBPL **)v3mall(MAXPLN*sizeof(GPBPL *),"gpgnpd")) == NULL )
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
***Om det är ett B_PLANE som ligger på en tänd nivå skall det ritas.
***Detta kollas nu och lagras som blank TRUE/FALSE i GPBPL-posten.
***När all trådgeometri klippts och planen skall ritas testas
***plan.blank. Om TRUE ritas planet inte.
***Spara även pennummer så att planet kan ritas med rätt
***penna om det är synligt.
*/
       case BPLTYP:
       DBread_bplane(&bpl,la);
       if ( !bpl.hed_bp.blank )
         {
         niv_status = WPnivt(actwin->nivtab,(short)sur.hed_su.level);
         plan.blank = !niv_status;
         plan.pen   = bpl.hed_bp.pen;
         if ( (status=mk_gpbpl(&bpl,&plan)) < 0 ) return(status);
         }
       break;
/*
***Om det är en yta med facetter behandlar vi dessa som
***B-plan.
*/
       case SURTYP:
       DBread_surface(&sur,la);
       if ( sur.typ_su == FAC_SUR  &&  !sur.hed_su.blank )
         {
         niv_status = WPnivt(actwin->nivtab,(short)sur.hed_su.level);
         DBread_patches(&sur,&patpek);
         toppat = patpek;
         for ( i=0; i<sur.nu_su; ++i )
           {
           for ( j=0; j<sur.nv_su; ++j )
             {
             if ( toppat->styp_pat == FAC_PAT )
               {
               facpat = (GMPATF *)toppat->spek_c;
               plan.blank = !niv_status;
               plan.pen   = sur.hed_su.pen;
/*
***Antingen är facetten fyrsidig....
*/
               if ( !facpat->triangles )
                 {
                 V3MOME(&facpat->p1,&bpl.crd1_bp,sizeof(DBVector));
                 V3MOME(&facpat->p2,&bpl.crd2_bp,sizeof(DBVector));
                 V3MOME(&facpat->p3,&bpl.crd3_bp,sizeof(DBVector));
                 V3MOME(&facpat->p4,&bpl.crd4_bp,sizeof(DBVector));
                 if ( (status=mk_gpbpl(&bpl,&plan)) < 0 ) return(status);
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
                 if ( (status=mk_gpbpl(&bpl,&plan)) < 0 ) return(status);
                 V3MOME(&facpat->p1,&bpl.crd1_bp,sizeof(DBVector));
                 V3MOME(&facpat->p3,&bpl.crd2_bp,sizeof(DBVector));
                 V3MOME(&facpat->p3,&bpl.crd3_bp,sizeof(DBVector));
                 V3MOME(&facpat->p4,&bpl.crd4_bp,sizeof(DBVector));
                 if ( (status=mk_gpbpl(&bpl,&plan)) < 0 ) return(status);
                 }
               }
             ++toppat;
             }
           }
         DBfree_patches(&sur,patpek);
         }
       break;
/*
***Mesh.
*/
       case MSHTYP:
       DBread_mesh(&mesh,la,MESH_ALL);
       if ( !mesh.hed_m.blank  &&  (mesh.font_m == 0 || mesh.font_m == 4) )
         {
         niv_status = WPnivt(actwin->nivtab,(short)sur.hed_su.level);
         plan.blank = !niv_status;
         plan.pen   = mesh.hed_m.pen;
/*
***Copy all (triangular) faces to DBBplane.
***TODO !!!! What if a face is not triangular ?
*/
         for ( i=0; i<mesh.nf_m; ++i )
           {
           edge_1 = mesh.pf_m[i].i_hedge;
           edge_2 = mesh.ph_m[edge_1].i_nhedge;
           edge_3 = mesh.ph_m[edge_2].i_nhedge;

           vertex_1 = mesh.ph_m[edge_1].i_evertex;
           vertex_2 = mesh.ph_m[edge_2].i_evertex;
           vertex_3 = mesh.ph_m[edge_3].i_evertex;

           V3MOME(&(mesh.pv_m[vertex_1].p),&bpl.crd1_bp,sizeof(DBVector));
           V3MOME(&(mesh.pv_m[vertex_2].p),&bpl.crd2_bp,sizeof(DBVector));
           V3MOME(&(mesh.pv_m[vertex_2].p),&bpl.crd3_bp,sizeof(DBVector));
           V3MOME(&(mesh.pv_m[vertex_3].p),&bpl.crd4_bp,sizeof(DBVector));
           if ( (status=mk_gpbpl(&bpl,&plan)) < 0 ) return(status);
           }
         }
       break;
       }
     }
/*
***Alla plan i GM är genomgångna, gör Z-sortering.
*/
   if ( np > 0 ) sortz(np,ppek);
/*
****Slut.
*/

   return(0);
 }

/********************************************************/
/*!******************************************************/

static  short  mk_gpbpl(
        GMBPL *bpl,
        GPBPL *plan)

/*      Processar ett GMBPL och skapar motsvarande
 *      GPBPL.
 * 
 *      (C)microform ab 10/1/96 J. Kjellander
 *
 ******************************************************!*/

 {
   int      k;
   double   d;
   DBVector p21,p41,prod;

/*
***Transformera till aktiv vy genom att anropa gpplbp.
*/
   k = -1;
   gpplbp(bpl,&k);
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
   else
     {
     plan->nv.x_gm = 0.0;
     plan->nv.y_gm = 0.0;
     plan->nv.z_gm = 0.0;
     }
/*
***Om planet är vänt mot betraktaren, beräkna även övriga konstanter.
***Lagra resultatet i en GPBPL-post.
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
***Min och Max i X- och Y-led samt Max i Z-led.
*/
     gen2db(plan);
/*
*** dx och dy för de fyra sidorna.
*/
     plan->dx1 = p21.x_gm;
     plan->dy1 = p21.y_gm;
     plan->dx2 = plan->p3.x_gm - plan->p2.x_gm;
     plan->dy2 = plan->p3.y_gm - plan->p2.y_gm;
     plan->dx3 = plan->p4.x_gm - plan->p3.x_gm;
     plan->dy3 = plan->p4.y_gm - plan->p3.y_gm;
     plan->dx4 = -p41.x_gm;
     plan->dy4 = -p41.y_gm;
/*
***Beräkna längden i kvadrat för de fyra sidorna.
***Om någon har längden 0 sätt k till -1.
***Annars, sätt k till 0 och beräkna  1/(verklig längd). 
*/
     plan->l1 = plan->dx1*plan->dx1 + plan->dy1*plan->dy1;
     if ( plan->l1 > DTOL ) { plan->l1 = 1.0/SQRT(plan->l1); plan->k1 = 0.0; }
     else { plan->l1 = 0.0; plan->k1 = -1.0; }
  
     plan->l2 = plan->dx2*plan->dx2 + plan->dy2*plan->dy2;
     if ( plan->l2 > DTOL ) { plan->l2 = 1.0/SQRT(plan->l2); plan->k2 = 0.0; }
     else { plan->l2 = 0.0; plan->k2 = -1.0; }

     plan->l3 = plan->dx3*plan->dx3 + plan->dy3*plan->dy3;
     if ( plan->l3 > DTOL ) { plan->l3 = 1.0/SQRT(plan->l3); plan->k3 = 0.0; }
     else { plan->l3 = 0.0; plan->k3 = -1.0; }

     plan->l4 = plan->dx4*plan->dx4 + plan->dy4*plan->dy4;
     if ( plan->l4 > DTOL ) { plan->l4 = 1.0/SQRT(plan->l4); plan->k4 = 0.0; }
     else { plan->l4 = 0.0; plan->k4 = -1.0; }
/*
***Plan-data för detta plan är nu klara. Allokera minne för plan-
***data och lagra.
*/
     if (  pblofs == 100 )
       {
       if ( pblant == MAXPBL ) 
         return(erpush("GP0072",""));
       if ( (pblpek[pblant]=(GPBPL *)v3mall(PBLSIZ,"gpgnpd")) == NULL )
         return(erpush("GP0082",""));
       else
         { ++pblant; pblofs = 0; }
       }
     *(ppek+np) = pblpek[pblant-1] + pblofs;
     V3MOME((char *)plan,(char *)*(ppek+np),sizeof(GPBPL));
     ++pblofs; ++np;
     }

  return(0);
 }

/********************************************************/
/*!******************************************************/

 static void   gen2db(
        GPBPL *pln)

/*      Genererar ett plans 2D-box.
 *
 *      In: p = Pekare till plan.
 * 
 *      (C)microform ab 26/1/89 J. Kjellander
 *
 *      14/3/92 Tolerenser, J. Kjellander
 *
 ******************************************************!*/

 {
#define UTTOL 0.0015  /* Så mycket för stor som 2D-boxen görs */

/*
***Min och Max i X-led.
*/
   if ( pln->p1.x_gm < pln->p2.x_gm )
     {
     if ( pln->p1.x_gm < pln->p3.x_gm )
       {
       if ( pln->p1.x_gm < pln->p4.x_gm )
         {
/*
***p1 < p2,p3,p4.
*/
         pln->xmin = pln->p1.x_gm;
         if ( pln->p4.x_gm > pln->p3.x_gm )
           { if ( pln->p4.x_gm > pln->p2.x_gm ) pln->xmax = pln->p4.x_gm;
           else pln->xmax = pln->p2.x_gm; }
         else
           { if ( pln->p3.x_gm > pln->p2.x_gm ) pln->xmax = pln->p3.x_gm;
           else pln->xmax = pln->p2.x_gm; }
         }
       else
/*
***p4 < p1 < p2,p3.
*/
         { pln->xmin = pln->p4.x_gm;
         if ( pln->p2.x_gm > pln->p3.x_gm ) pln->xmax = pln->p2.x_gm;
         else pln->xmax = pln->p3.x_gm; }
       }
     else
       {
       if ( pln->p3.x_gm < pln->p4.x_gm )
/*
***p3 < p1 < p2 och p3 < p4.
*/
         { pln->xmin = pln->p3.x_gm;
         if ( pln->p2.x_gm > pln->p4.x_gm ) pln->xmax = pln->p2.x_gm;
         else pln->xmax = pln->p4.x_gm; }
       else
/*
***p4 < p3 < p2 < p1.
*/
         { pln->xmin = pln->p4.x_gm; pln->xmax = pln->p2.x_gm; }
       }
     }
   else
     {
/*
***p2 < p1,p3,p4.
*/
     if ( pln->p2.x_gm < pln->p3.x_gm )
       {
       if ( pln->p2.x_gm < pln->p4.x_gm )
         {
/*
***p2 < p1,p3,p4.
*/
         pln->xmin = pln->p2.x_gm;
         if ( pln->p4.x_gm > pln->p3.x_gm )
           { if ( pln->p4.x_gm > pln->p1.x_gm ) pln->xmax = pln->p4.x_gm;
           else pln->xmax = pln->p1.x_gm; }
         else
           { if ( pln->p3.x_gm > pln->p1.x_gm ) pln->xmax = pln->p3.x_gm;
           else pln->xmax = pln->p1.x_gm; }
         }
       else
/*
***p4 < p2 < p1,p3.
*/
         { pln->xmin = pln->p4.x_gm;
         if ( pln->p1.x_gm > pln->p3.x_gm ) pln->xmax = pln->p1.x_gm;
         else pln->xmax = pln->p3.x_gm; }
       }
     else
       {
       if ( pln->p3.x_gm < pln->p4.x_gm )
/*
***p3 < p2 < p1 och p3 < p4.
*/
         { pln->xmin = pln->p3.x_gm;
         if ( pln->p1.x_gm > pln->p4.x_gm ) pln->xmax = pln->p1.x_gm;
         else pln->xmax = pln->p4.x_gm; }
       else
/*
***p4 < p3 < p2 < p1.
*/
         { pln->xmin = pln->p4.x_gm; pln->xmax = pln->p1.x_gm; }
       }
     }
/*
***Min och Max i Y-led.
*/
   if ( pln->p1.y_gm < pln->p2.y_gm )
     {
     if ( pln->p1.y_gm < pln->p3.y_gm )
       {
       if ( pln->p1.y_gm < pln->p4.y_gm )
         {
/*
***p1 < p2,p3,p4.
*/
         pln->ymin = pln->p1.y_gm;
         if ( pln->p4.y_gm > pln->p3.y_gm )
           { if ( pln->p4.y_gm > pln->p2.y_gm ) pln->ymax = pln->p4.y_gm;
           else pln->ymax = pln->p2.y_gm; }
         else
           { if ( pln->p3.y_gm > pln->p2.y_gm ) pln->ymax = pln->p3.y_gm;
           else pln->ymax = pln->p2.y_gm; }
         }
       else
/*
***p4 < p1 < p2,p3.
*/
         { pln->ymin = pln->p4.y_gm;
         if ( pln->p2.y_gm > pln->p3.y_gm ) pln->ymax = pln->p2.y_gm;
         else pln->ymax = pln->p3.y_gm; }
       }
     else
       {
       if ( pln->p3.y_gm < pln->p4.y_gm )
/*
***p3 < p1 < p2 och p3 < p4.
*/
         { pln->ymin = pln->p3.y_gm;
         if ( pln->p2.y_gm > pln->p4.y_gm ) pln->ymax = pln->p2.y_gm;
         else pln->ymax = pln->p4.y_gm; }
       else
/*
***p4 < p3 < p2 < p1.
*/
         { pln->ymin = pln->p4.y_gm; pln->ymax = pln->p2.y_gm; }
       }
     }
   else
     {
/*
***p2 < p1,p3,p4.
*/
     if ( pln->p2.y_gm < pln->p3.y_gm )
       {
       if ( pln->p2.y_gm < pln->p4.y_gm )
         {
/*
***p2 < p1,p3,p4.
*/
         pln->ymin = pln->p2.y_gm;
         if ( pln->p4.y_gm > pln->p3.y_gm )
           { if ( pln->p4.y_gm > pln->p1.y_gm ) pln->ymax = pln->p4.y_gm;
           else pln->ymax = pln->p1.y_gm; }
         else
           { if ( pln->p3.y_gm > pln->p1.y_gm ) pln->ymax = pln->p3.y_gm;
           else pln->ymax = pln->p1.y_gm; }
         }
       else
/*
***p4 < p2 < p1,p3.
*/
         { pln->ymin = pln->p4.y_gm;
         if ( pln->p1.y_gm > pln->p3.y_gm ) pln->ymax = pln->p1.y_gm;
         else pln->ymax = pln->p3.y_gm; }
       }
     else
       {
       if ( pln->p3.y_gm < pln->p4.y_gm )
/*
***p3 < p2 < p1 och p3 < p4.
*/
         { pln->ymin = pln->p3.y_gm;
         if ( pln->p1.y_gm > pln->p4.y_gm ) pln->ymax = pln->p1.y_gm;
         else pln->ymax = pln->p4.y_gm; }
       else
/*
***p4 < p3 < p2 < p1.
*/
         { pln->ymin = pln->p4.y_gm; pln->ymax = pln->p1.y_gm; }
       }
     }
/*
***Bestäm planets Z-max.
*/
   if ( pln->p1.z_gm > pln->p2.z_gm )
     {
     if ( pln->p1.z_gm > pln->p3.z_gm )
       {
       if ( pln->p1.z_gm > pln->p4.z_gm ) pln->zmax = pln->p1.z_gm;
       else pln->zmax = pln->p4.z_gm;
       }
     else
       {
       if ( pln->p3.z_gm > pln->p4.z_gm ) pln->zmax = pln->p3.z_gm;
       else pln->zmax = pln->p4.z_gm;
       }
     }
   else
     {
     if ( pln->p2.z_gm > pln->p3.z_gm )
       {
       if ( pln->p2.z_gm > pln->p4.z_gm ) pln->zmax = pln->p2.z_gm;
       else pln->zmax = pln->p4.z_gm;
       }
     else
       {
       if ( pln->p3.z_gm > pln->p4.z_gm ) pln->zmax = pln->p3.z_gm;
       else pln->zmax = pln->p4.z_gm;
       }
     }
/*
***Gör planets 2D-box lite större. Detta för att inte linjer
***som råkar ligga exakt på planets 2D-boxrand skall klassas
***som synliga. Om 2D-boxranden är identisk med planets rand,
***tex. en rektangel med räta hörn innebär detta att testen
***mot 2D-boxen resulterar att linjen klassas som inuti 2D-boxen
***och får testas mot planet självt. Linjen kommer då att
***klassas som osynlig. Använd tolerensen 0.0015, samma som i
***clip().
*/
    pln->xmin -= UTTOL;  pln->xmax += UTTOL;  
    pln->ymin -= UTTOL;  pln->ymax += UTTOL;  

    return;

 }

/********************************************************/
/*!******************************************************/

 static void    sortz(
        int     np,
        GPBPL **ppek)

/*      Sorterar ppek i fallande Z-ordning.
 *
 *      In:   np    = Antal plan.
 *            ppek  = C-pekare till array av GPBPL-pekare.
 * 
 *      (C)microform ab 26/1/89 J. Kjellander
 *
 ******************************************************!*/

 {
   bool shift;
   double zmax;
   int    start,end,izmax,i;
   GPBPL *tpek;

   start = 0;
   end = np-1;

loop:
   shift = FALSE;
   zmax = (*(ppek+start))->zmax; izmax = start;

   for ( i=start; i<end; ++i )
     {
     if ( (*(ppek+i+1))->zmax > (*(ppek+i))->zmax )
       {
       shift = TRUE;
       tpek = *(ppek+i); *(ppek+i) = *(ppek+i+1); *(ppek+i+1) = tpek;
       if ( (*(ppek+i))->zmax > zmax )
         { zmax = (*(ppek+i))->zmax; izmax = i; }
       }
     }

   if ( shift )
     {
     tpek = *(ppek+izmax); *(ppek+izmax) = *(ppek+start);
    *(ppek+start) = tpek; ++start; --end; goto loop;
     }
 }

/********************************************************/
/*!******************************************************/

 static short   gpcl2d(
        double *xv,
        double *yv,
        GPBPL  *pl)

/*      Klassar en vektor mot ett plan, 2D.
 *
 *      In: xv,yv = Adresser till vektorns startpos.
 *          pl    = Planets adress.
 *
 *      FV: INUTI/UTANFR
 *
 *      (C)microform ab 6/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
  double tt;

  tt = (pl->dy1 * (*xv - pl->p1.x_gm) -
        pl->dx1 * (*yv - pl->p1.y_gm)) * pl->l1 + pl->k1;
  if ( tt < inutol )
    {
    tt = (pl->dy2 * (*xv - pl->p2.x_gm) -
          pl->dx2 * (*yv - pl->p2.y_gm)) * pl->l2 + pl->k2;
    if ( tt < inutol )
      {
      tt = (pl->dy3 * (*xv - pl->p3.x_gm) -
            pl->dx3 * (*yv - pl->p3.y_gm)) * pl->l3 + pl->k3;
      if ( tt < inutol )
        {
        tt = (pl->dy4 * (*xv - pl->p4.x_gm) -
             pl->dx4 * (*yv - pl->p4.y_gm)) * pl->l4 + pl->k4;
        if ( tt < inutol ) return(INUTI);
        }
      }
    }

   return(UTANFR);
 }

/********************************************************/
/*!******************************************************/

 static void   gpsvp2(
        GPBPL *pl,
        int    end)

/*      Klipper aktiv vektor mot ett plan 2D.
 *      Vektorns ena ändpunkt ligger säkert inuti planet och
 *      den andra ligger säkert utanför.
 *
 *      In: pl   = Planets adress.
 *          end  = 1/2 Vilken av ändarna som ligger inuti.
 *
 *      FV: void
 *
 *      (C)microform ab 10/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
   double dx12,dy12,dz12,uv,up,q,upmin,upmax;

/*
***Initiering.
*/
   dx12 = *px1 - *px2; dy12 = *py1 - *py2; dz12 = *pz1 - *pz2;
   upmin = 0.0; upmax = 1.0;
/*
***Planets 1:a sida.
*/
start:
   q = dx12 * pl->dy1 - dy12 * pl->dx1;

   if ( ABS(q) > 1e-5 )
     {
     uv = ((*px1-pl->p1.x_gm)*pl->dy1 - (*py1-pl->p1.y_gm)*pl->dx1)/q;
     if ( uv > 0.0  &&  uv < 1.0 )
       {
       up = ((pl->p1.x_gm-*px1)*dy12 - (pl->p1.y_gm-*py1)*dx12)/q;
       if ( up > upmin  &&  up < upmax )
         {
         if ( end == 1 )
           { *px1 = *px1 - uv*dx12; *py1 = *py1 - uv*dy12;
             *pz1 = *pz1 - uv*dz12;  return; }
         else
           { *px2 = *px1 - uv*dx12; *py2 = *py1 - uv*dy12;
             *pz2 = *pz1 - uv*dz12;  return; }
         }
       }
     }
/*
***Planets 2:a sida.
*/
   q = dx12 * pl->dy2 - dy12 * pl->dx2;

   if ( ABS(q) > 1e-5 )
     {
     uv = ((*px1-pl->p2.x_gm)*pl->dy2 - (*py1-pl->p2.y_gm)*pl->dx2)/q;
     if ( uv > 0.0  &&  uv < 1.0 )
       {
       up = ((pl->p2.x_gm-*px1)*dy12 - (pl->p2.y_gm-*py1)*dx12)/q;
       if ( up > upmin  &&  up < upmax )
         {
         if ( end == 1 )
           { *px1 = *px1 - uv*dx12; *py1 = *py1 - uv*dy12;
             *pz1 = *pz1 - uv*dz12;  return; }
         else
           { *px2 = *px1 - uv*dx12; *py2 = *py1 - uv*dy12;
             *pz2 = *pz1 - uv*dz12;  return; }
         }
       }
     }
/*
***Planets 3:e sida.
*/
   q = dx12 * pl->dy3 - dy12 * pl->dx3;

   if ( ABS(q) > 1e-5 )
     {
     uv = ((*px1-pl->p3.x_gm)*pl->dy3 - (*py1-pl->p3.y_gm)*pl->dx3)/q;
     if ( uv > 0.0  &&  uv < 1.0 )
       {
       up = ((pl->p3.x_gm-*px1)*dy12 - (pl->p3.y_gm-*py1)*dx12)/q;
       if ( up > upmin  &&  up < upmax )
         {
         if ( end == 1 )
           { *px1 = *px1 - uv*dx12; *py1 = *py1 - uv*dy12;
             *pz1 = *pz1 - uv*dz12;  return; }
         else
           { *px2 = *px1 - uv*dx12; *py2 = *py1 - uv*dy12;
             *pz2 = *pz1 - uv*dz12;  return; }
         }
       }
     }
/*
***Planets 4:e sida.
*/
   q = dx12 * pl->dy4 - dy12 * pl->dx4;

   if ( ABS(q) > 1e-5 )
     {
     uv = ((*px1-pl->p4.x_gm)*pl->dy4 - (*py1-pl->p4.y_gm)*pl->dx4)/q;
     if ( uv > 0.0  &&  uv < 1.0 )
       {
       up = ((pl->p4.x_gm-*px1)*dy12 - (pl->p4.y_gm-*py1)*dx12)/q;
       if ( up > upmin  &&  up < upmax )
         {
         if ( end == 1 )
           { *px1 = *px1 - uv*dx12; *py1 = *py1 - uv*dy12;
             *pz1 = *pz1 - uv*dz12;  return; }
         else
           { *px2 = *px1 - uv*dx12; *py2 = *py1 - uv*dy12;
             *pz2 = *pz1 - uv*dz12;  return; }
         }
       }
     }
/*
***Kommer vi hit ner har inte vektorn befunnits skära någon
***av planets sidor trots att ena änden säkert ligger innanför
***och andra änden utanför. Förklaringen måste då vara att den
***slunkit ut genom ett hörn. Justera upmin och upmax och prova igen.
*/
   upmin -=  0.001; upmax += 0.001; goto start;

 }

/********************************************************/
/*!******************************************************/

 static void   gpspl1(
        GPBPL *pl,
        int    end)

/*      Splittar aktiv vektor mot ett plan 2D.
 *      Vektorns ena ände har kontakt med planet och
 *      den andra ligger säkert utanför.
 *
 *      In: pl   = Planets adress.
 *          end  = 1/2 Vilken ände som har kontakt.
 *
 *      FV: void
 *
 *      (C)microform ab 10/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
   register int  i;
   int    ns;
   double u[4],dx12,dy12,dz12,umin,umax;

/*
***Beräkna skärningar.
*/
   cutpl(pl,&ns,u);
/*
***Har vi 0 skärningar är allt ok! Vektorn kan omöjligt
***skära planet.
*/
   if ( ns == 0 ) return;
/*
***Har vi 1 skärning går det att klippa vektorn.
*/
   else if ( ns == 1 )
     {
     gpsvp2(pl,end);
     return;
     }
/*
***Har vi två skärningar kan de vara identiska eller också är
***den ena kontakten med planet och den andra en sann skärning.
***Om vi har mer än 2 skärningar måste några vara identiska.
***Leta upp största och minsta.
*/
   else
     {
     umin = 2.0; umax = -1.0;
     for ( i=0; i<ns; ++i )
       {
       if ( u[i] < umin ) umin = u[i];
       if ( u[i] > umax ) umax = u[i];
       }
     u[0] = umin;
     u[1] = umax;
     }
/*
***Nu har vi exakt 2 skärningar. Om de är identiska är det
***fråga om en vektor som har kontakt med ett hörn.
*/
   if ( ABS(u[0] - u[1]) < 1e-5 ) return;
/*
***Om skärningarna inte är identiska skall vi klippa mot
***den som ligger längst bort från kontaktänden.
*/
   dx12 = *px1 - *px2;
   dy12 = *py1 - *py2;
   dz12 = *pz1 - *pz2;

   if ( end == 1 )
     {
     if ( u[0] > u[1] )
       { *px1 = *px1 - u[0]*dx12; *py1 = *py1 - u[0]*dy12;
       *pz1 = *pz1 - u[0]*dz12; }
     else
       { *px1 = *px1 - u[1]*dx12; *py1 = *py1 - u[1]*dy12;
       *pz1 = *pz1 - u[1]*dz12; }
     }
   else
     {
     if ( u[0] < u[1] )
       { *px2 = *px1 - u[0]*dx12; *py2 = *py1 - u[0]*dy12;
       *pz2 = *pz1 - u[0]*dz12; }
     else
       { *px2 = *px1 - u[1]*dx12; *py2 = *py1 - u[1]*dy12;
       *pz2 = *pz1 - u[1]*dz12; }
     }

 }

/********************************************************/
/*!******************************************************/

 static short  gpspl2(
        GPBPL *pl)

/*      Splittar aktiv vektor mot ett plan 2D.
 *      Vektorns båda ändpunkter ligger säkert
 *      utanför planet.
 *
 *      In: pl   = Planets adress.
 *
 *      FV: void
 *
 *      (C)microform ab 10/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
   register int i;
   int    ns;
   double u[4],dx12,dy12,dz12,umin,umax;

/*
***Beräkna skärningar.
*/
   cutpl(pl,&ns,u);
/*
***Har vi nu 0 skärningar är allt ok! Vektorn kan omöjligt
***skära planet. Har vi 1 skärning är det lite skumt. Troligen
***passerar vektorn förbi planet på dess utsida väldigt nära
***ett hörn.
*/
   if ( ns < 2 ) return(SYNLIG);
/*
***Om ns = 2 eller mera finns ett största u och ett minsta.
***Om vi har mer än 2 skärningar måste några vara identiska.
***Leta upp största och minsta.
*/
   else
     {
     umin = 2.0; umax = -1.0;
     for ( i=0; i<ns; ++i )
       {
       if ( u[i] < umin ) umin = u[i];
       if ( u[i] > umax ) umax = u[i];
       }
     u[0] = umin;
     u[1] = umax;
     }
/*
***Nu har vi exakt 2 skärningar. Om de är identiska är det
***fråga om en vektor som passerar planet på utsidan nära ett
***hörn.
*/
   if ( ABS(u[0] - u[1]) < 1e-5 ) return(SYNLIG);
/*
***Om skärningarna inte är identiska är det dags för split.
***Börja med att skapa en split-del.
*/
   if ( nsplit < SPLMAX ) 
     {
     dx12 = *px1 - *px2;
     dy12 = *py1 - *py2;
     dz12 = *pz1 - *pz2;
     *(spx+2*nsplit) = *px1 - u[1]*dx12;
     *(spy+2*nsplit) = *py1 - u[1]*dy12;
     *(spz+2*nsplit) = *pz1 - u[1]*dz12;
     *(spx+2*nsplit+1) = *px2;
     *(spy+2*nsplit+1) = *py2;
     *(spz+2*nsplit+1) = *pz2;
/*
***Beräkna sedan ny slutpunkt för den ursprungliga
***nu synliga vektorn.
*/
     *px2 = *px1 - u[0]*dx12;
     *py2 = *py1 - u[0]*dy12;
     *pz2 = *pz1 - u[0]*dz12;
     return(SPLIT2);
     }
/*
***Om nsplit = SPLMAX....
*/
   return(OSYNLIG);

 }

/********************************************************/
/*!******************************************************/

 static void   cutpl(
        GPBPL *pl,
        int   *n,
        double u[])

/*      Skär aktiv vektor mot ett tätt plan.
 *
 *      In: pl = Planets adress.
 *
 *      Ut: n  = Antal skärningar 0 - 4.
 *          u  = Skärningarnas u-värden.
 *
 *      FV: void
 *
 *      (C)microform ab 15/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
   double dx12,dy12,uv,up,q,upmin,upmax;
   int    ns;

/*
***Initiering. Tätt plan.
*/
   dx12 = *px1 - *px2; dy12 = *py1 - *py2;
   upmin = -0.001; upmax = 1.001; ns = 0;
/*
***Planets 1:a sida.
*/
   q = dx12 * pl->dy1 - dy12 * pl->dx1;

   if ( ABS(q) > 1e-5 )
     {
     uv = ((*px1-pl->p1.x_gm)*pl->dy1 - (*py1-pl->p1.y_gm)*pl->dx1)/q;
     if ( uv > 0.0  &&  uv < 1.0 )
       {
       up = ((pl->p1.x_gm-*px1)*dy12 - (pl->p1.y_gm-*py1)*dx12)/q;
       if ( up > upmin  &&  up < upmax ) u[ns++] = uv;
       }
     }
/*
***Planets 2:a sida.
*/
   q = dx12 * pl->dy2 - dy12 * pl->dx2;

   if ( ABS(q) > 1e-5 )
     {
     uv = ((*px1-pl->p2.x_gm)*pl->dy2 - (*py1-pl->p2.y_gm)*pl->dx2)/q;
     if ( uv > 0.0  &&  uv < 1.0 )
       {
       up = ((pl->p2.x_gm-*px1)*dy12 - (pl->p2.y_gm-*py1)*dx12)/q;
       if ( up > upmin  &&  up < upmax ) u[ns++] = uv;
       }
     }
/*
***Planets 3:e sida.
*/
   q = dx12 * pl->dy3 - dy12 * pl->dx3;

   if ( ABS(q) > 1e-5 )
     {
     uv = ((*px1-pl->p3.x_gm)*pl->dy3 - (*py1-pl->p3.y_gm)*pl->dx3)/q;
     if ( uv > 0.0  &&  uv < 1.0 )
       {
       up = ((pl->p3.x_gm-*px1)*dy12 - (pl->p3.y_gm-*py1)*dx12)/q;
       if ( up > upmin  &&  up < upmax ) u[ns++] = uv;
       }
     }
/*
***Planets 4:e sida.
*/
   q = dx12 * pl->dy4 - dy12 * pl->dx4;

   if ( ABS(q) > 1e-5 )
     {
     uv = ((*px1-pl->p4.x_gm)*pl->dy4 - (*py1-pl->p4.y_gm)*pl->dx4)/q;
     if ( uv > 0.0  &&  uv < 1.0 )
       {
       up = ((pl->p4.x_gm-*px1)*dy12 - (pl->p4.y_gm-*py1)*dx12)/q;
       if ( up > upmin  &&  up < upmax ) u[ns++] = uv;
       }
     }
/*
***Antal skärningar.
*/
   *n = ns;

 }

/********************************************************/
/*!******************************************************/

 static void      gpsvp3(
        GPBPL    *pl,
        DBVector *ps)

/*      Beräknar skärning mellan aktiv vektor och ett
 *      oändligt plan 3D.
 *
 *      In: pl   = Planets adress.
 *
 *      Ut: ps   = Skärningspunkt.
 *
 *      FV: Void.
 *
 *      (C)microform ab 17/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
#define NOLLTOL 1e-12

/*
***Denna tolerens används för att testa hurvida ett plan
***är parallellt med något av de tre huvudplanen. Den måste
***vara så liten så att en vektor vars ena ändpunkt ligger
***i planet ( tex. en splitdel ) klassas säkert som liggande
***helt på ena eller andra sidan.
*/

   double t,l,n1x,n1y,n1z,n2x,n2y,n2z,dx,dy,dz,k1,k2,k3,k4,
          k7,ka,kb;

/*
***Beräkna normalvektorn (n1x,n1y,n1z) längs planets första sida.
*/
   n1x = pl->dx1; n1y = pl->dy1; n1z = pl->p2.z_gm - pl->p1.z_gm;
   l = SQRT(n1x*n1x + n1y*n1y + n1z*n1z);
   n1x /= l; n1y /= l; n1z /= l;
/*
***Beräkna normalvektorn n2 längs planets sista sida.
*/
   n2x = -pl->dx4; n2y = -pl->dy4; n2z = pl->p4.z_gm - pl->p1.z_gm;
   l = SQRT(n2x*n2x + n2y*n2y + n2z*n2z);
   n2x /= l; n2y /= l; n2z /= l;
/*
***Beräkna linjens normalvektor (dx,dy,dz).
*/
   dx = *px2 - *px1; dy = *py2 - *py1; dz = *pz2 - *pz1;
   l = SQRT(dx*dx + dy*dy + dz*dz);
   dx /= l; dy /= l; dz /= l;
/*
***Om n1x = 0, prova att vrida sida 1 mot sida 4.
***Om n1x fortfarande är 0 ligger planet i ett XY-plan.
***Eftersom vektorn tidigare konstaterats inte vara parallell
***med planet kan skärning enkelt beräknas.
***Om n1x efter vridning inte = 0 kan inte snabb metod användas,
***fortsätt då istället och testa n1y och n1z på samma sätt.
*/
   if ( ABS(n1x) < NOLLTOL )
     {
     n1x += 0.1*n2x; n1y += 0.1*n2y; n1z += 0.1*n2z;
     l = SQRT(n1x*n1x + n1y*n1y + n1z*n1z);
     n1x /= l; n1y /= l; n1z /= l;
     if ( ABS(n1x) < NOLLTOL )
       { t = (pl->p1.x_gm - *px1)/dx; goto end; }
     }
/*
***Samma för y.
*/
   if ( ABS(n1y) < NOLLTOL )
     {
     n1x += 0.1*n2x; n1y += 0.1*n2y; n1z += 0.1*n2z;
     l = SQRT(n1x*n1x + n1y*n1y + n1z*n1z);
     n1x /= l; n1y /= l; n1z /= l;
     if ( ABS(n1y) < NOLLTOL )
       { t = (pl->p1.y_gm - *py1)/dy; goto end; }
     }
/*
***Samma för z.
*/
   if ( ABS(n1z) < NOLLTOL )
     {
     n1x += 0.1*n2x; n1y += 0.1*n2y; n1z += 0.1*n2z;
     l = SQRT(n1x*n1x + n1y*n1y + n1z*n1z);
     n1x /= l; n1y /= l; n1z /= l;
     if ( ABS(n1z) < NOLLTOL )
       { t = (pl->p1.z_gm - *pz1)/dz; goto end; }
     }
/*
***Planet är inte parallellt med något av XY, ZY eller ZX-
***planen och ingen av n1:s komponenter = 0. Beräkna då
***skärning med JK:s metod.
*/
   k1 = n2x/n1x;
   k2 = -(dx/n1x);
   k3 = (*px1 - pl->p1.x_gm)/n1x;
   k4 = n2y/n1y;
   k7 = n2z/n1z;
   ka = (k4 - k1)*((*pz1 - pl->p1.z_gm)/n1z - k3) +
        (k1 - k7)*((*py1 - pl->p1.y_gm)/n1y - k3);
   kb = (k1 - k4)*(dz/n1z + k2) + (k7 - k1)*(dy/n1y + k2);
   t = ka/kb;
/*
***t har nu beräknats och punkten ps kan därmed beräknas.
*/
end:
   ps->x_gm = *px1 + t*dx;
   ps->y_gm = *py1 + t*dy;
   ps->z_gm = *pz1 + t*dz;

 }

/********************************************************/
/*!******************************************************/

 static void      gpspl3(
        DBVector *ps,
        int       sida1)

/*      Delar aktiv vektor i två delar vid ps.
 *      Änden som är hitom planet lagras i (px,py,pz)
 *      och andra änden lagras som en split-del.
 *
 *      In: ps    = Delningspunkt.
 *          sida1 = HITOM/BAKOM för vektorns startpunkt.
 *
 *      FV: void.
 *
 *      (C)microform ab 17/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
   if ( sida1 == HITOM )
     {
     *(spx+2*nsplit) = ps->x_gm;
     *(spy+2*nsplit) = ps->y_gm;
     *(spz+2*nsplit) = ps->z_gm;
     *(spx+2*nsplit+1) = *px2;
     *(spy+2*nsplit+1) = *py2;
     *(spz+2*nsplit+1) = *pz2;
     *px2 = ps->x_gm;
     *py2 = ps->y_gm;
     *pz2 = ps->z_gm;
     }
   else
     {
     *(spx+2*nsplit) = ps->x_gm;
     *(spy+2*nsplit) = ps->y_gm;
     *(spz+2*nsplit) = ps->z_gm;
     *(spx+2*nsplit+1) = *px1;
     *(spy+2*nsplit+1) = *py1;
     *(spz+2*nsplit+1) = *pz1;
     *px1 = *px2;
     *py1 = *py2;
     *pz1 = *pz2;
     *px2 = ps->x_gm;
     *py2 = ps->y_gm;
     *pz2 = ps->z_gm;
     }
 }

/********************************************************/
/*!******************************************************/

 static void      gpsvp4(
        GPBPL    *plr,
        GPBPL    *plt,
        DBVector *ps)

/*      Beräknar skärning plan/plan.
 *
 *      FV: Void.
 *
 *      (C)microform ab 20/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
   double n,k1,k2,k3;
   DBVector  p2a,p2b,p3a,p3b,p3d,u3,u12,u23,u31,v1,v2;

/*
***Bilda ett tredje plan vinkelrätt mot test-planet
***och innehållande dess 1:a rand.
*/
   p2a.x_gm = plt->p1.x_gm;
   p2a.y_gm = plt->p1.y_gm;
   p2a.z_gm = plt->p1.z_gm;
   p2b.x_gm = plt->p2.x_gm;
   p2b.y_gm = plt->p2.y_gm;
   p2b.z_gm = plt->p2.z_gm;

start:
   p3a.x_gm = p2a.x_gm;
   p3a.y_gm = p2a.y_gm;
   p3a.z_gm = p2a.z_gm;

   p3b.x_gm = p2b.x_gm;
   p3b.y_gm = p2b.y_gm;
   p3b.z_gm = p2b.z_gm;

   p3d.x_gm = p2a.x_gm + plt->nv.x_gm;
   p3d.y_gm = p2a.y_gm + plt->nv.y_gm;
   p3d.z_gm = p2a.z_gm + plt->nv.z_gm;
/*
***Beräkna det tredje planets normal.
*/
   v1.x_gm = p3b.x_gm - p3a.x_gm; v1.y_gm = p3b.y_gm - p3a.y_gm;
   v1.z_gm = p3b.z_gm - p3a.z_gm;
   v2.x_gm = p3d.x_gm - p3a.x_gm; v2.y_gm = p3d.y_gm - p3a.y_gm;
   v2.z_gm = p3d.z_gm - p3a.z_gm;
   GEvector_product(&v1,&v2,&u3);
   GEnormalise_vector3D(&u3,&u3);
/*
***Beräkna de tre kryssprodukterna i skärningen.
*/
   GEvector_product(&(plt->nv),&u3,&u23);
   GEvector_product(&u3,&(plr->nv),&u31);
   GEvector_product(&(plr->nv),&(plt->nv),&u12);
/*
***Kolla om skärning finns. Om inte, prova med sidan p4-p1
***istället.
*/
   n = plr->nv.x_gm*u23.x_gm + plr->nv.y_gm*u23.y_gm + plr->nv.z_gm*u23.z_gm;
   if ( ABS(n) < 1e-10 ) 
     {
     p2a.x_gm = plt->p4.x_gm; p2a.y_gm = plt->p4.y_gm;
     p2a.z_gm = plt->p4.z_gm; p2b.x_gm = plt->p1.x_gm;
     p2b.y_gm = plt->p1.y_gm; p2b.z_gm = plt->p1.z_gm;
     goto start;
     }
/*
***Beräkna skärningspunkten plr-plt-p3.
*/
   k1 = plr->p1.x_gm*plr->nv.x_gm + plr->p1.y_gm*plr->nv.y_gm +
        plr->p1.z_gm*plr->nv.z_gm;
   k2 = p2a.x_gm*plt->nv.x_gm + p2a.y_gm*plt->nv.y_gm +
        p2a.z_gm*plt->nv.z_gm;
   k3 = p3a.x_gm*u3.x_gm + p3a.y_gm*u3.y_gm + p3a.z_gm*u3.z_gm;

   u23.x_gm *= k1; u23.y_gm *= k1; u23.z_gm *= k1;
   u31.x_gm *= k2; u31.y_gm *= k2; u31.z_gm *= k2;
   u12.x_gm *= k3; u12.y_gm *= k3; u12.z_gm *= k3;

   ps->x_gm = (u23.x_gm + u31.x_gm + u12.x_gm )/n;
   ps->y_gm = (u23.y_gm + u31.y_gm + u12.y_gm )/n;
   ps->z_gm = (u23.z_gm + u31.z_gm + u12.z_gm )/n;
 }

/********************************************************/
/*!******************************************************/

 static short   klptst( 
        double *v,
        double *w,
        double *t1,
        double *t2)

/*      Klipper en vektor mot ett fönster samt ger ett mått på hur
*       mycket som klippts bort.
*
*       IN:
*            v: Vektor.
*            w: Fönster.
*
*
*       UT:
*            v: Vektor enl FV.
*            t1: Anger hur mycket som klippts bort vid punkt 1
*                0.0 < t1 < 1.0.
*            t2: Anger hur mycket som klippts bort vid punkt 2
*                0.0 < t2 < 1.0.
*         
*       FV:
*            -1: Vektorn är oklippt och ligger utanför fönstret
*             0: Vektorn är oklippt och ligger innanför fönstret.
*             1: Vektorn är klippt i punkt 1.
*             2: Vektorn är klippt i punkt 2.
*             3: Vektorn är klippt i både punkt 1 och punkt 2.
*
*
*       (c) Microform AB 1984 M. Nelson
*
*
*       REVIDERAD:
*
*       15/7/85  Bug, Ulf Johansson
*       29/10/86 *p1 == *p2, J. Kjellander
*
********************************************************!*/

  {
    register double *p1,*p2,*win;
    short sts1,sts2;
    double d1,d2;

        sts1 = sts2 = 0;
        p1 = v;
        p2 = p1 + 2;
        win = w;

        /* Om punkt 1 ligger utanför fönstret, klipp till fönsterkanten */

        if (*p1 < *win) {

            if (*p2 < *win)
                return(-1);          /* Hela vektorn vänster om fönstret */
            d1 = (*win - *p1)/(*p2 - *p1);
            *(p1+1) += (*(p2+1) - *(p1+1))*d1;
            *p1 = *win;
            sts1 = 1;
        } else if (*p1 > *(win+2)) {

            if (*p2 > *(win+2))
                return(-1);               /* Hela vektorn höger om fönstret */
            d1 = (*(win+2) - *p1)/(*p2 - *p1);
            *(p1+1) += (*(p2+1) - *(p1+1))*d1;
            *p1 = *(win+2);
            sts1 = 1; 
        }

        if (*(p1+1) < *(win+1)) {

            if (*(p2+1) < *(win+1))
                return(-1);               /* Hela vektorn nedanför fönstret */
            d2 = (*(win+1) - *(p1+1))/(*(p2+1) - *(p1+1));
            *p1 += (*p2 - *p1)*d2;
            *(p1+1) = *(win+1);
            sts1 += 2;
        } else if (*(p1+1) > *(win+3)) {

            if (*(p2+1) > *(win+3))
                return(-1);               /* Hela vektor ovanför fönstret */
            d2 = (*(win+3) - *(p1+1))/(*(p2+1) - *(p1+1));
            *p1 += (*p2 - *p1)*d2;
            *(p1+1) = *(win+3);
            sts1 += 2;
        }

        if (sts1 != 0) {                   /* Punkt 1 klippt */
            if ((*p1 < *win) ||
                (*p1 > *(win+2)) ||
                (*(p1+1) < *(win+1)) ||
                (*(p1+1) > *(win+3)))
                 return(-1);              /* Hela vektorn utanför fönstret */
            if ( *p1 == *p2 && *(p1+1) == *(p2+1) ) return(-1); /*861029JK */
        
            if (sts1 == 1)
                 *t1 = d1;
            else if (sts1 == 2)
                 *t1 = d2;
            else
                 *t1 = d1 + d2 - d1*d2;  /* Kompensation för tvåstegsklipp */

            sts1 = 1;
        }


       /* Punkt 1 ligger innanför fönstret, klipp punkt 2 om utanför. */ 

        if (*p2 < *win) {
            d1 = (*win - *p2)/(*p1 - *p2);
            *(p2+1) -= (*(p2+1) - *(p1+1))*d1;
            *p2 = *win;
            sts2 = 1;
        } else if (*p2 > *(win+2)) {
            d1 = (*(win+2) - *p2)/(*p1 - *p2);
            *(p2+1) -= (*(p2+1) - *(p1+1))*d1;
            *p2 = *(win+2);
            sts2 = 1;
        }

        if (*(p2+1) < *(win+1)) {
            d2 = (*(win+1) - *(p2+1))/(*(p1+1) - *(p2+1));
            *p2 -= (*p2 - *p1)*d2;
            *(p2+1) = *(win+1);
            sts2 += 2;
        } else if (*(p2+1) > *(win+3)) {
            d2 = (*(win+3) - *(p2+1))/(*(p1+1) - *(p2+1));
            *p2 -= (*p2 - *p1)*d2;
            *(p2+1) = *(win+3);
            sts2 += 2;
        }

        if (sts2 != 0) {                   /* Punkt 2 klippt */
        
            if ( *p1 == *p2 && *(p1+1) == *(p2+1) ) return(-1); /*861029JK */
            if (sts2 == 1)
                 *t2 = d1;
            else if (sts2 == 2)
                 *t2 = d2;
            else
                 *t2 = d1 + d2 - d1*d2;  /* Kompensation för tvåstegsklipp */

            sts2 = 2;

            if (sts1 != 0)
                 *t2 *= (1.0 - *t1);     /* kompensation för punkt 1-klipp */
        }

        return(sts1 + sts2);
  }

/********************************************************/
/*!******************************************************/

 static short   gpplbp(
        GMBPL  *bplpek,
        int    *n)
      
/*      Bygger ett B-plan i form av en polylinje.
 *
 *      In: bplpek =>  Pekare till plan-structure
 *          n+1    =>  Offset till planets startposition
 *
 *      Ut: n      =>  Offset till polylinjens slutposition
 *          x,y,z,a=>  Polylinjens koordinater och status
 *
 *      FV: 0
 *
 *      (C)microform ab 27/8/87 J. Kjellander
 *
 *      6/2/89   hide, J. Kjellander
 *      2006-12-29 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
   int k;

/*
***Initiering.
*/
   k = *n+1;
/*
***We could have used WPplbp() here but hide
***want's a 4-sided plane in this case.
*/
   x[k]   = bplpek->crd1_bp.x_gm;
   y[k]   = bplpek->crd1_bp.y_gm;
   z[k]   = bplpek->crd1_bp.z_gm;
   a[k++] = 0;

   x[k]   = bplpek->crd2_bp.x_gm;
   y[k]   = bplpek->crd2_bp.y_gm;
   z[k]   = bplpek->crd2_bp.z_gm;
   a[k++] = VISIBLE;

   x[k]   = bplpek->crd3_bp.x_gm;
   y[k]   = bplpek->crd3_bp.y_gm;
   z[k]   = bplpek->crd3_bp.z_gm;
   a[k++] = VISIBLE;

   x[k]   = bplpek->crd4_bp.x_gm;
   y[k]   = bplpek->crd4_bp.y_gm;
   z[k]   = bplpek->crd4_bp.z_gm;
   a[k++] = VISIBLE;

   x[k]   = bplpek->crd1_bp.x_gm;
   y[k]   = bplpek->crd1_bp.y_gm;
   z[k]   = bplpek->crd1_bp.z_gm;
   a[k] = VISIBLE;
/*
***Project on current view of window.
*/
   WPpply(actwin,k,x,y,z);
/*
***The end.
*/
  *n = k;

   return(0);
  }

/********************************************************/
