/**********************************************************************
*
*    gp24.c
*    ======
*
*    This file is part of the VARKON Graphics Library.
*    URL: http://www.varkon.com
*
*    gphide();  Display hidden line image
*    gpdrhd();  Draw entities with hidden lines removed
*    hidply();  Draw polyline with hidden lines removed
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
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../WP/include/WP.h"
#include "../../GE/include/GE.h"
#include "../include/GP.h"
#include <math.h>
static int clip(int ip);

/*
***Skapar nya ränder vid plan/plan
*/
static void pp_cut(int i);

/*
***Används av pp_cut()
*/
static void pp_clip(GPBPL *pl2);

/*
***Skapar ny rand
*/
static void add_bnd(GPBPL *pl1, GPBPL *pl2);

/*
***Klipper vektor mot plan
*/
static int clip(int ip);

/*
***Ritar klippt vektor
*/
static void rita();

/*
***Klipper vektor mot skärm
*/
static bool clipsc(int iv);


GPBPL *pblpek[MAXPBL];
int    pblant,pblofs;

/*
***MAXPBL är max antal primärminnesblock a c:a 32000 bytes
***som kan malloceras av gpgnpd() för Plan-data.
***pblpek är pekare till dessa block och pblant är antal
***block som allokerats. pblofs är antal använda.
*/


GPBPL  **ppek;
int    np;

/*
***ppek är en pekare till en array av plan-pekare. Minne
***för denna malloceras av gpgnpd(). Varje plan-pekare
***pekar på plan-data för ett GPBPL-plan.
***np är antal plan-pekare, dvs. antal plan.
*/


double  clipv[6];
double *px1 = &clipv[0];
double *px2 = &clipv[1];
double *py1 = &clipv[2];
double *py2 = &clipv[3];
double *pz1 = &clipv[4];
double *pz2 = &clipv[5];

/*
***I clipv lagras den vektor som just nu skickats till clip().
***px1,px2,py1,py2,pz1 och pz2 är pekare till de 6 koordinaterna.
*/


double *spx,*spy,*spz;
int     nsplit;
int     spp[SPLMAX];

/*
***Pekare till koordinater för delar av vektor som
***blivit över efter split. nsplit = antal split.
***Minne allokeras av gphide(). spp innehåller pekare
***till det plan där split-delen skall börja testas igen.
*/


int      nsk,edge;
int      edsk[2];
GPBPL   *plsk[2];
DBVector psk[2];

/*
***När ett plans rand skär igenom ett annat plan lagras
***skärningspunktens koordinater i psk, planets C-pekare
***i plsk, randens ordningsnummer i edsk och antal
***skärningar i nsk. edge är aktiv vektors edge-ordnings-
***nummer.
*/


double inutol;

/*
***inutol är villkoret för att en position skall anses ligga
***inuti ett plan 2D.
*/

static bool invbpl;

/*
***invbpl är en flagga som håller reda på hurvida sist behandlade
***plan var osynligt eller ej.
*/

static bool screen,gksfil;
METADEF  md;
FILE    *gksfp;

/*
***Styr hurvida klippt geometri ritas på skärm resp. till
***GKS-metafil.
*/


extern bool   gpgenz;      /* För att gpdrxx-rutiner skall generera
                              även Z-koordinater. */
extern double x[],y[],z[]; /* Polylinjens koordinater lagras
                              av gpdrxx-rutiner här */
extern char   a[];           /* Status för polylinje */
extern int    ncrdxy;        /* Antal vektorer i polylinje */
extern double k1x,k2x,k1y,k2y; /* Skärmtransformation */
extern bool   intrup;
extern VY     actvy;

/*
***Med X11 har vi egen avbrottshantering.
*/
#ifdef V3_X11
extern short gptrty;
#endif

/*!******************************************************/

        short gphide(
        bool   flag1,
        bool   flag2,
        FILE  *pfil,
        DBVector *origo)

/*      Huvudrutin för hide.
 *
 *      In: flag1 = TRUE om bilden skall ritas på skärmen.
 *          flag2 = TRUE om bilden skall skrivas ut till disk.
 *          pfil  = Pekare till öppen fil.
 *          origo = Bildens nollpunkt rel. aktiv vy
 *
 *      Felkoder: GP0102 = Kan ej allokera minne för split
 *
 *      (C)microform ab 29/1/89 J. Kjellander
 *
 *      29/5/91 origo, J. Kjellander
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
   gksfp = pfil;
/*
***Börja med att skapa Plan-data.
*/
   igptma(155,IG_MESS); status = gpgnpd(); igrsma();
   if ( status < 0 ) return(status);
   sprintf(buf,"%s%d",iggtts(156),np); igplma(buf,IG_MESS);
/*
***Allokera minne för SPLMAX split-delar. 2 koordinater per del.
*/
   if ( (spx=(double *)v3mall(2*SPLMAX*sizeof(double),"gphide")) == NULL )
     return(erpush("GP0010",""));
   if ( (spy=(double *)v3mall(2*SPLMAX*sizeof(double),"gphide")) == NULL )
     return(erpush("GP0010",""));
   if ( (spz=(double *)v3mall(2*SPLMAX*sizeof(double),"gphide")) == NULL )
     return(erpush("GP0010",""));
/*
***Om det skall ritas på skärmen, sudda skärm och df.
*/
   if ( screen )
     {
     gperal();
     gpdram();
     }
/*
***Om det skall plottas, initiera GKS-metafil.
*/
   if ( gksfil )
     {
     if ( (status=methdr(&md,gksfp,metarec)) < 0 ) goto end;
     if ( (status=metclr(&md,gksfp,metarec)) < 0 ) goto end;
     if ( (status=metwin(&md,gksfp,metarec,&actvy,origo)) < 0 ) goto end;
     }
/*
***Rita storheter skymt.
*/
   status = gpdrhd();
   igrsma();
/*
***Återställ skärmen.
*/
   if ( screen )
     {
     igupmu();
     iggnsa();
     }
/*
***Avsluta ev. plottning.
*/
   if ( gksfil ) metend(&md,gksfp,metarec);
/*
***Lämna tillbaks allokerat minne.
*/
end:
   v3free(spx,"gphide"); v3free(spy,"gphide");
   v3free(spz,"gphide"); v3free(ppek,"gphide");
   for ( i=0; i<pblant; ++i ) v3free(pblpek[i],"gphide");
/*
***Slut.
*/
   return(status);
 }

/********************************************************/
/*!******************************************************/

        short gpdrhd()

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
 *
 ******************************************************!*/

 {
   short   curpen;
   int     i,k,nbl;
   char    str[V3STRLEN+1],metarec[MAXMETA];
   DBetype type;
   DBptr   la;
   GMUNON  gmpost;
   DBTmat  pmat;
   GMSEG  *segptr,arcseg[4],*sptarr[6];

/*
***Div. initiering.
*/
   gpgenz = TRUE;
   curpen = -1;
/*
***Gå igenom GM efter 3D-trådobjekt. B-plan väntar vi med eftersom det går
***fortare att ta dom direkt från ppekÄÅ och denna dessutom bara innehåller
***hitåtvända plan.
*/
   DBget_pointer('F',NULL,&la,&type);
loop:
   while ( DBget_pointer('N',NULL,&la,&type ) == 0 )
     {
     if ( intrup ) return(AVBRYT);

#ifdef V3_X11
     if ( gptrty == X11  &&  wpintr() ) return(AVBRYT);
#endif

     ncrdxy = 0;
     switch ( type )
       {
       case POITYP:
       DBread_point(&gmpost.poi_un,la);
       gpdrpo(&gmpost.poi_un,la,GEN);
       break;
/*
***Linjer kan from. V1.11 vara streckade. Streckad linjes polylinje
***måste skapas här med visst trix eftersom gpdrcu med GEN som rit-
***mode bara skapar heldragna linjer.
*/
       case LINTYP:
       DBread_line(&gmpost.lin_un,la);
       gpdrli(&gmpost.lin_un,la,GEN);
       if ( gmpost.lin_un.fnt_l > 0 )
         {
         k = -1;
         gpplli(&gmpost.lin_un,&k,x,y,a);
         ncrdxy = k+1;
         }
       break;
        
       case ARCTYP:
       DBread_arc(&gmpost.arc_un,arcseg,la);
       gpdrar(&gmpost.arc_un,arcseg,la,GEN);
       break;
        
       case CURTYP:
       DBread_curve(&gmpost.cur_un,&segptr,NULL,la);
       gpdrcu(&gmpost.cur_un,segptr,la,GEN);
       DBfree_segments(segptr);
       break;

       case SURTYP:
       DBread_surface(&gmpost.sur_un,la);
       if ( gmpost.sur_un.typ_su != FAC_SUR )
         {
         DBread_srep_curves(&gmpost.sur_un,sptarr);
         gpdrsu(&gmpost.sur_un,sptarr,la,GEN);
         DBfree_srep_curves(sptarr);
         }
       break;
        
       case CSYTYP:
       DBread_csys(&gmpost.csy_un,&pmat,la);
       gpdrcs(&gmpost.csy_un,&pmat,la,GEN);
       break;

       case TXTTYP:
       DBread_text(&gmpost.txt_un,str,la);
       gpdrtx(&gmpost.txt_un,str,la,GEN);
       break;
 
       default:
       goto loop;
       }
/*
***Sätt rätt penna och rita storheten.
*/
     if ( ncrdxy > 0 )
       {
       if ( gmpost.hed_un.pen != curpen )
         {
         curpen = gmpost.hed_un.pen;
         if ( screen ) gpspen(curpen);
         if ( gksfil ) metpen(&md,gksfp,curpen,metarec);
         }
       hidply();
       }
     }
/*
***ppekÄÅ innehåller alla hitåtvända och ej blankade plan. Dom
***kan dock fortfarande ligga på en släckt nivå och skall då
***inte visas.
*/
   a[0] = 0; a[1] = a[2] = a[3] = a[4] = VISIBLE;

   for ( i=0; i<np; ++i )
     {
#ifdef V3_X11
     if ( gptrty == X11  &&  wpintr() ) return(AVBRYT);
#endif
     if ( (*(ppek+i))->blank == FALSE )
       {
/*
***Sätt rätt penna. Flusha först så sist ritade vektor kommer ut
***i rätt färg. OBS gpspen() borde göra gpflsh() innan pennan ändras!!!!!!!!!
***Hur funkar detta för trådstorheter ovan ????? Kanske bug !!
***OBS åxå att rita-rutinen kan göra direkta anrop till (X-)Windows.
***Borde gå lite fortare.
*/
       if ( (*(ppek+i))->pen != curpen )
         {
         curpen = (*(ppek+i))->pen;
         if ( screen )
           {
           gpflsh();
           gpspen(curpen);
           }
         if ( gksfil ) metpen(&md,gksfp,curpen,metarec);
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
         if ( screen )
           {
           gpflsh();
           gpspen(curpen);
           }
         if ( gksfil ) metpen(&md,gksfp,curpen,metarec);
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
   gpflsh();
   if ( actvy.vydist == 0.0 ) gpgenz = FALSE;

   return(0);
 }

/********************************************************/
/*!******************************************************/

        static void pp_cut(
        int i)

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

        static void pp_clip(
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

        static void add_bnd(
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

        void hidply()

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
 *
 ******************************************************!*/

 {
   char metarec[MAXMETA];
   char av[2];

/*
***Sätt flaggan för osynligt b-plan till false.
*/
   invbpl = FALSE;
/*
***Rita på skärmen.
*/
   if ( screen )
     {
     gpmvsc((short)(k1x + *px1*k2x),(short)(k1y + *py1*k2y));
     gpdwsc((short)(k1x + *px2*k2x),(short)(k1y + *py2*k2y));
     }
/*
***Och till fil.
*/
   if ( gksfil )
     {
     av[0] = 0; av[1] = VISIBLE;
     metply(&md,gksfp,(short)1,&clipv[0],&clipv[2],av,&actvy,metarec);
     }
/*
***Slut.
*/
   return;
 }

/********************************************************/
/*!******************************************************/

        static bool clipsc(
        int    iv)

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

   switch ( klptst(v,actvy.vywin,&t1,&t2) )
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
