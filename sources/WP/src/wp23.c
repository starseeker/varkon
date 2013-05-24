/**********************************************************************
*
*    wp23.c
*    ======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpdrcu();      Draw curve
*    wpdlcu();      Delete curve
*    wpmpcu();      Create 3D polyline
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
#include "../../GP/include/GP.h"
#include "../../GE/include/GE.h"
#include "../include/WP.h"
#include <math.h>

/*
***Static-variabler för snabb vektorgenerering.
*/

static double actl,actu,prevu,sl;
static short  k;
static int    nvec;
static GMSEG *segpek;
static GMCUR *curpek;

/*
***Externa deklarationer.
*/
extern short    actpen;

static short drawcu(WPGWIN *gwinpt, GMCUR *curpek, GMSEG *segpek,
                    DBptr la, bool draw);
static short mk_solid(double scale, int *npts, double x[], double y[],
                      double z[], char a[]);
static short mk_dash(double scale, int *npts, double x[], double y[],
                     double z[], char a[]);

/*!******************************************************/

        short wpdrcu(
        GMCUR  *curpek,
        GMSEG  *segpek,
        DBptr   la,
        v2int   win_id)

/*      Ritar en cur.
 *
 *      In: curpek => Pekare till cur-post.
 *          segpek => Pekare till GMSEG.
 *          la     => GM-adress.
 *          win_id => Fönster att rita i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 1/1/95 J. Kjellander
 *
 *      1997-12-27 Breda linjer, J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Om kurvan är blankad är det enkelt.
*/
   if ( curpek->hed_cu.blank) return(0);
/*
***Loopa igenom alla WPGWIN-fönster utom 1:a.
*/
   for ( i=1; i<WTABSIZ; ++i )
     {
     if ( (winptr=wpwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Skall vi rita i detta fönster ?
*/
       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Ja, ligger kurvan på en nivå som är tänd i detta fönster ?
*/
         if ( wpnivt(gwinpt,curpek->hed_cu.level) )
           {
/*
***Ja. Kolla att rätt färg är inställd.
*/
           if ( curpek->hed_cu.pen != actpen ) wpspen(curpek->hed_cu.pen);
           if ( curpek->wdt_cu != 0.0 ) wpswdt(gwinpt->id.w_id,curpek->wdt_cu);
/*
***Sen är det bara att rita.
*/
           drawcu(gwinpt,curpek,segpek,la,TRUE);
           if ( curpek->wdt_cu != 0.0 ) wpswdt(gwinpt->id.w_id,0.0);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short wpdlcu(
        GMCUR  *curpek,
        GMSEG  *segpek,
        DBptr   la,
        v2int   win_id)

/*      Suddar en kurva.
 *
 *      In: curpek => Pekare till cur-post.
 *          segpek => Pekare till GMSEG.
 *          la     => GM-adress.
 *          win_id => Fönster att sudda i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 31/12/94 J. Kjellander
 *
 *      1997-12-27 Breda linjer, J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   DBetype typ;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Loopa igenom alla WPGWIN-fönster utom 1:a.
*/
   for ( i=1; i<WTABSIZ; ++i )
     {
     if ( (winptr=wpwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN ) 
       {
       gwinpt = (WPGWIN *)winptr->ptr;
/*
***Skall vi sudda i detta fönster ?
*/
       if ( win_id == GWIN_ALL  ||  win_id == gwinpt->id.w_id )
         {
/*
***Ja. Stryk ur DF.
*/
         if ( wpfobj(gwinpt,la,CURTYP,&typ) ) wprobj(gwinpt);
/*
***Om den nu ligger på en släckt nivå eller är blankad gör vi
***inget mer. Annars får vi återskapa polylinjen och sudda från skärm.
*/
         if ( !wpnivt(gwinpt,curpek->hed_cu.level)  ||
                             curpek->hed_cu.blank) return(0);

         if ( curpek->wdt_cu != 0.0 ) wpswdt(gwinpt->id.w_id,curpek->wdt_cu);
         drawcu(gwinpt,curpek,segpek,la,FALSE);
         if ( curpek->wdt_cu != 0.0 ) wpswdt(gwinpt->id.w_id,0.0);
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        static short drawcu(
        WPGWIN *gwinpt,
        GMCUR  *curpek,
        GMSEG  *segpek,
        DBptr   la,
        bool    draw)

/*      Ritar/suddar en grafisk kurva i ett visst fönster.
 *      Vid ritning lagras objektet samtidigt i DF.
 *
 *      In: gwinpt => Pekare till fönster.
 *          curpek => Pekare till cur-post.
 *          segpek => Pekare till GMSEG.
 *          la     => GM-adress.
 *          draw   => TRUE = Rita, FALSE = Sudda
 *
 *      Ut:  Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 19/2/94 J. Kjellander
 *
 ******************************************************!*/

 {
   double x[PLYMXV],y[PLYMXV],z[PLYMXV];
   char   a[PLYMXV];
   int    k;

/*
***Ställ om grapac:s vy så att detta fönster gäller.
*/
   wpfixg(gwinpt);
/*
***Skapa grafisk representation, dvs. polylinje.
*/
   k = -1;
   gpplcu(curpek,segpek,&k,x,y,z,a);
/*
***Återställ grapac:s ursprungliga vy igen.
*/
   wpfixg(NULL);
/*
***Klipp polylinjen. Om den är synlig (helt eller delvis ),
***rita den.
*/
   if ( curpek->fnt_cu == 3  ||  wpcply(gwinpt,(short)-1,&k,x,y,a) )
     {
     if ( draw  &&  curpek->hed_cu.hit )
       {
       if ( !wpsply(gwinpt,k,x,y,a,la,CURTYP) ) return(erpush("GP0012",""));
       if ( curpek->fnt_cu == 3 ) wpfply(gwinpt,k,x,y,draw);
       else wpdobj(gwinpt,TRUE);
       }
     else
       {
       if ( curpek->fnt_cu == 3 ) wpfply(gwinpt,k,x,y,draw);
       else wpdply(gwinpt,k,x,y,a,draw);
       }
     }

   return(0);
 }

/***********************************************************/
/*!******************************************************/

        short wpmpcu(
        GMCUR *gmpost,
        GMSEG *segmnt,
        double scale,
        int   *npts,
        double x[],
        double y[],
        double z[],
        char   a[])

/*      Bygger en heldragen eller streckad arc/kurva
 *      i form av en 3D-polyline.
 *
 *      In: 
 *         gmpost   = Pekare till arc/curve-structure.
 *         segmnt   = Array med segment.
 *         scale    = Påverkar noggrannheten.
 *
 *      Ut:
 *         npts     = Antal genererade positioner
 *         x,y,z,a  = Koordinater och status hos vektorerna
 *                    i polylinjen.
 *
 *      FV:   0 = Ok.
 *
 *      (C)microform ab 1998-10-06J. Kjellander
 *
 ******************************************************!*/

  {
   short status;

/*
***Div. initiering.
*/
   curpek = (GMCUR *)gmpost;
   segpek = segmnt;
/*
***Vektorer genereras olika för heldragen och streckad kurva.
*/
    switch ( curpek->fnt_cu )
      {
      case 0:
      status = mk_solid(scale,npts,x,y,z,a);
      break;

      case 1:
      case 2:
      status = mk_dash(scale,npts,x,y,z,a);
      break;

      default:
      status = -2;
      break;
      }

   return(status);
 }

/*********************************************************/
/*!******************************************************/

 static short nextu(
        double ds)

/*      Beräknar nästa u-värde.
 *
 *      In: ds = Storleken på nästa steg, i relativ båglängd.
 *
 *      Ut: prevu, actu, actl.
 *
 *      FV:   0 = Ok.
 *
 *      (C)microform ab 10/3/92 J. Kjellander
 *
 ******************************************************!*/

 {
   short status;

/*
***Sätt föregående u = aktuellt u.
*/
   prevu = actu;
/*
***Räkna upp actl.
*/
   actl += ds;
/*
***Beräkna ett nytt actu.
*/
   status = GE717((DBAny *)curpek,segpek,NULL,actl,&actu);
   if ( status < 0 ) return(status);

   actu -= 1.0;

   return(0);
 }

 /*******************************************************/
/*!******************************************************/

 static short nextp(
        double ug,
        double x[],
        double y[],
        double z[])

/*      Beräknar nästa position.
 *
 *      In: ug    = Globalt u-värde.
 *          x,y,z = Arrayer för resultat.
 *
 *      Ut: x,y,z.
 *
 *      FV:   0 = Ok.
 *
 *      (C)microform ab 10/3/92 J. Kjellander
 *
 ******************************************************!*/

 {
   int   intu;
   gmflt out[16];

/*
***Välj rätt segment.
*/
    if ( (intu=(int)HEL(ug)) == curpek->nsgr_cu ) intu -= 1;
/*
***Beräkna koordinater.
*/
    GE107((DBAny *)curpek,segpek+intu,ug-intu,(short)0,out);
    x[k] = out[0]; y[k] = out[1]; z[k] = out[2];
/*
***Slut.
*/
   return(0);

 }

 /*******************************************************/
/*!******************************************************/

 static short nextnv(
        double ug,
        double sskala)

/*      Beräknar nästa antal vektorer.
 *
 *      In: ug     = Globalt u-värde.
 *          sskala = "Skärmskala"
 *
 *      Ut: x,y,z.
 *
 *      FV:   0 = Ok.
 *
 *      (C)microform ab 10/3/92 J. Kjellander
 *
 ******************************************************!*/

 {
   int   intu;
   gmflt kappa,out[16];

/*
***Välj rätt segment.
*/
    if ( (intu=(int)HEL(ug)) == curpek->nsgr_cu ) intu -= 1;
/*
***Beräkna krökning.
*/
    GE107((DBAny *)curpek,segpek+intu,ug-intu,(short)3,out);
    kappa = out[15]/sskala;
/*
***Antal vektorer.
***Som mått på dsdu används sl. Se gp8.c.
***I gp8() används 0.2*SQRT(..... för en tredjedel av ett segment.
***Här är det då logiskt att ta 3 ggr. så mycket för ett streck.
*/
    nvec = (int)(0.6*SQRT(kappa)*sl*sskala + 0.5);
    if (nvec < 1 ) nvec = 1;
/*
***Slut.
*/
   return(0);

 }

 /*******************************************************/
/*!******************************************************/

 static short mk_solid(
        double scale,
        int   *npts,
        double x[],
        double y[],
        double z[],
        char   a[])

/*      Genererar vektorer för heldragen kurva.
 *
 *      In: 
 *         scale    = Påverkar noggrannheten.
 *                    0 = minsta möjliga.
 *
 *      Ut:
 *         npts     = Antal positioner i polylinjen
 *         x,y,z,a  = Koordinater och status hos vektorerna
 *                    i polylinjen.
 *
 *      FV:   0 = Ok.
 *
 *      (C)microform ab 1998-10-06J. Kjellander
 *
 ******************************************************!*/

  {
   double  uinc,ustop;

/*
***Initiering av gemensamma static-variabler.
*/
   k    = 0;       /* Antal positioner  */
   actu = 0.0;     /* Aktuellt globalt parametervärde */
/*
***Börja med en släckt förflyttning till kurvans startposition.
***actu = 0.
*/
   nextp(actu,x,y,z);
   a[k++] = INVISIBLE;
/*
***Steglängd och stoppvillkor. Om scale = 0 genererar vi
***bara 2 vektorer/segment.
*/
   if ( scale > 0.0 ) uinc = 0.2/scale;
   else               uinc = 0.5;

   ustop = (double)curpek->nsgr_cu - uinc/10;
/*
***Tända förflyttningar inne på kurvan.
*/
   while ( (actu += uinc) < ustop )
     {
     nextp(actu,x,y,z);
     a[k++] = VISIBLE;
     if ( k > PLYMXV-3 ) return(-1);
     }
/*
***Å så den sista.
*/
   nextp((double)curpek->nsgr_cu,x,y,z);
   a[k++] = VISIBLE;
/*
***Hur många vektorer blev det ?
*/
  *npts = k;
/*
***Slut.
*/
   return(0);
 }

/*********************************************************/
/*!******************************************************/

 static short mk_dash(
        double scale,
        int   *n,
        double x[],
        double y[],
        double z[],
        char   a[])

/*      Bygger en heldragen eller streckad arc/kurva
 *      i form av en 3D-polyline.
 *
 *      In: 
 *         scale    = Påverkar noggrannheten.
 *         n+1      = Offset till polylinjestart.
 *         x,y,z,a  = Arrayer för resultat.
 *
 *      Ut:
 *         n        = Offset till sista vektorn i polylinjen
 *         x,y,z,a  = Koordinater och status hos vektorerna
 *                    i polylinjen.
 *
 *      FV:   0 = Ok.
 *
 *      (C)microform ab 1998-10-06J. Kjellander
 *
 ******************************************************!*/

  {
  return(-2);
  }

/********************************************************/
