/**********************************************************************
*
*    gp28.c
*    ======
*
*    This file is part of the VARKON Graphics  Library.
*    URL: http://www.varkon.com
*
*    plycud();    Make dashed curve polyline
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
#include "../../GE/include/GE.h"
#include "../include/GP.h"
#include <math.h>

/*
***Beräknar nästa u-värde
*/
static short nextu(double ds);

/*
***Beräknar nästa position
*/
static short nextp(double ug, double x[], double y[], double z[]);

/*
***Beräknar nästa antal vektorer
*/
static short nextnv(double ug);

extern DBTmat actvym;
extern VY     actvy;
extern double curnog,gpgszx,k2x;
extern int    ncrdxy;
extern bool   pltflg,gpgenz;

/*
***Static-variabler.
*/

static double actl,actu,prevu,sl,sskala;
static int    k;
static int    nvec;
static GMSEG *segpek;
static GMCUR *curpek;

/*!******************************************************/

        short plycud(
        GMCUR *gmpost,
        GMSEG *segmnt,
        int   *n,
        double x[],
        double y[],
        double z[],
        char   a[])

/*      Bygger en streckad kurva i form av en 2D/3D-polyline.
 *
 *      In: 
 *         gmpost:      Pekare till curve-structure.
 *         segmnt:      Array med segment.
 *         n+1:         Offset till polylinjestart.
 *         x,y,z,a:     Arrayer för resultat.
 *
 *      Ut:
 *         n:           Offset till sista vektorn i polylinjen
 *         x,y,z,a:     Koordinater och status hos vektorerna
 *                      i polylinjen.
 *
 *      FV:   0 = Ok.
 *           -1 = För liten eller för stor strecklängd för att synas.
 *
 *      (C)microform ab 9/1/92 J. Kjellander
 *
 *      7/3/92   Uppsnabbning, J. Kjellander
 *      16/6/93  Dynamiska segment, J. Kjellander
 *      31/1/94  k = *n+1, J. Kjellander
 *      5/12/94  Font=2, J. Kjellander
 *      9/1/96   Bytt vydist->gpgenz, J. Kjellander
 *      19961104 nsgr, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    int     i;
    double  ltot,ds,mfakt,uinc,stopl;
    bool    offseg;
    GMSEG  *prjseg;
    DBTmat  m1,m2;

/*
***Div initiering.
*/
    curpek = gmpost;           /* Pekare till kurv-posten */
    k      = *n+1;             /* Offset i polylinjen */
    sl     = curpek->lgt_cu;   /* Strecklängd */
    actu   = 0.0;              /* Aktuellt globalt parametervärde */
    prevu  = 0.0;              /* Föregående globala parametervärde */
    actl   = 0.0;              /* Aktuell relativ längd */
    mfakt  = DASHRATIO;        /* Mellanrumsfaktor=0.25 */
    offseg = FALSE;            /* TRUE om något segm. har offset */
/*
***Skärmskala.
*/
    sskala = gpgszx/(actvy.vywin[2] - actvy.vywin[0]);
/*
***Om något av segmenten har offset är det en offset-kurva 
***och då skall kurvplanet transformeras till aktiv vy.
*/
    for ( i=0; i<curpek->nsgr_cu; ++i )
      {
      if ( (segmnt+i)->ofs != 0.0 )
        {
        if ( actvy.vy3d )
          {
          V3MOME(&curpek->csy_cu,&m1,sizeof(DBTmat));
          GEtform_inv(&actvym,&m2);
          GEtform_mult(&m1,&m2,&curpek->csy_cu);
          }
        offseg = TRUE;
        break;
        }
      }
/*
***Om 3D-vy, transformera segmenten till rätt vy och beräkna
***nya längder. Om inte, kolla om längd behövs och beräkna isåfall.
***Så är tex. fallet för en 3D-cirkel i 2D-vy då vi anropas av gpplar().
***Om projicering behövs görs detta på en kopia av segmenten så att
***om det visar att det räcker med heldraget (för stor eller liten
***streck-längd) så skall inte de ursprungliga segmenten projiceras
***en gång till av gpplcu(). För att segmentlängder skall bli rätt
***och därmed strecklängder måste alla Z-koefficienter sättas till
***0 dvs. äkta projicering. För en offset-kurva kan man dock inte
***göra på det viset eftersom man då förändrar kurvans form.
*/
    if ( actvy.vy3d )
      {
      prjseg = DBcreate_segments(curpek->nsgr_cu);
      for ( i=0; i < curpek->nsgr_cu; ++i )
        {
        GEtfseg_to_local(&segmnt[i],&actvym,&prjseg[i]);
        if ( !offseg  &&  !gpgenz )
          prjseg[i].c0z = prjseg[i].c1z = prjseg[i].c2z = prjseg[i].c3z = 0.0;
        }
      segpek = prjseg;
      }
    else
      {
      prjseg = NULL;
      segpek = segmnt;
      }
/*
***Beräkna hela kurvans längd. Om ingen projektion skett ovan och
***kurvan har samma grafiska som geometriska representation är detta
***viserligen redan gjort men hur ska vi veta det ???
*/
    GEarclength((DBAny *)curpek,segpek,&ltot);
/*
***Kommer strecken att synas på skärmen ? Om inte rita heldraget.
*/
    if ( ltot < (1.5+mfakt)*sl ) { status = -1; goto error; }
    if ( mfakt*sl*k2x < 1.0  &&  !pltflg ) { status = -1; goto error; }
/*
***Beräkna relativ steglängd. 0 > ds > 1.
*/
    ds = sl/ltot;
/*
***Finns det plats ?
*/
    if ( k+2 > PLYMXV )
      {
      status = -1;
      goto error;
      }
/*
***Börja med en släckt förflyttning till kurvans startposition.
***actu = 0.
*/
    nextp(actu,x,y,z);
    a[k++] = INVISIBLE;
/*
***Sedan ett första tänt delstreck. Beräkna först nytt actu
***och sedan antal vektorer genom ett nextnv()-anrop mitt på.
*/
    if ( (status=nextu(ds)) < 0 )
      {
      status = -1;
      goto error;
      }
    nextnv(actu/2.0);
    uinc = actu/nvec;
/*
***Finns det plats ?
*/
    if ( k+nvec+2 > PLYMXV )
      {
      --k;
      status = -1;
      goto error;
      }
/*
***Generera.
*/
    for ( i=0; i<nvec; ++i )
      {
      prevu += uinc;
      nextp(prevu,x,y,z);
      a[k++] = VISIBLE;
      }
/*
***Vi står nu i slutet på det första del-strecket med actl och actu.
***Generera resten utom sista. Detta fortsätter vi med så länge vi
***befinner oss på tryggt avstånd från slutet. Vad detta är beror på
***aktuell font. Med streckat är det (1 delstreck + 2 mellanrum)
***och med streckprickat är det (1 långt streck + 1 mellanrum + 1 kort
***streck + 2 mellanrum).
*/
    if ( curpek->fnt_cu == 1 ) stopl = 1.0 - (1.0+2*mfakt)*ds;
    else                       stopl = 1.0 - (1.0+4*mfakt)*ds;

    while ( actl < stopl )
      {
/*
***Om det är streckprickat lägger vi nu in ett mellanrum + ett
***kort streck.
*/
      if ( curpek->fnt_cu != 1 )
        {
        if ( (status=nextu((2.0*mfakt)*ds)) < 0 )
          {
          status = -1;
          goto error;
          }
/*
***Vi approximerar mellanrummet med ett motsvarande steg i u.
*/
        prevu += (actu-prevu)/2.0;
        nextp(prevu,x,y,z);
        a[k++] = INVISIBLE;
/*
***Vi står nu i början av en eller flera tända förflyttningar.
***Beräkna erfoderligt antal vektorer för delstrecket.
***Om de inte får plats i polylinjen avslutar vi utan att rita
***klart.
*/
        nextnv((actu+prevu)/2.0);

        if ( k+nvec+2 >= PLYMXV )
         {
       --k;
         status = -1;
         goto error;
         }
/*
***Generera nvec tända förflyttningar.
*/
        uinc = (actu-prevu)/nvec;

        for ( i=0; i<nvec; ++i )
          {
          prevu += uinc;
          nextp(prevu,x,y,z);
          a[k++] = VISIBLE;
          }
        }
/*
***Oavsett om det är streckat eller streckprickat är det nu dags att
***räkna upp actl och actu med (ett mellanrum + ett långt streck).
*/
      if ( (status=nextu((1.0+mfakt)*ds)) < 0 )
        {
        status = -1;
        goto error;
        }
/*
***Vi approximerar mellanrummet med ett motsvarande steg i u.
*/
      prevu = actu - 1.0/(1.0+mfakt)*(actu-prevu);
      nextp(prevu,x,y,z);
      a[k++] = INVISIBLE;
/*
***Vi står nu i början av en eller flera tända förflyttningar.
***Beräkna erfoderligt antal vektorer för delstrecket.
***Om de inte får plats i polylinjen avslutar vi utan att rita
***klart.
*/
      nextnv((actu+prevu)/2.0);

      if ( k+nvec+2 >= PLYMXV )
       {
     --k;
       status = -1;
       goto error;
       }
/*
***Generera nvec tända förflyttningar.
*/
      uinc = (actu-prevu)/nvec;

      for ( i=0; i<nvec; ++i )
        {
        prevu += uinc;
        nextp(prevu,x,y,z);
        a[k++] = VISIBLE;
        }
      }
/*
***Ett sista mellanrum.
*/
    if ( (status=nextu(ds*mfakt)) < 0 )
      {
      status = -1;
      goto error;
      }
    nextp(actu,x,y,z);
    a[k++] = INVISIBLE;
/*
***Nu gäller det att avsluta på ett vettigt sätt.
***Finns det tillräckligt med plats kvar gör vi ett
***till streck, annars förlänger vi föregående.
*/
    if ( 1.0 - actl < ds*mfakt )
      {
      k -= 1;
      actu = prevu;
      }
/*
***Hur många vektorer behövs för det som är kvar ?
*/
    nextnv((curpek->nsgr_cu + actu)/2.0);
    if ( k+nvec+2 >= PLYMXV )
      {
      --k;
      status = -1;
      goto error;
      }
/*
***Sista delstrecket.
*/
    uinc = (curpek->nsgr_cu - actu)/nvec;

    for ( i=0; i<nvec; ++i )
      {
      actu += uinc;
      nextp(actu,x,y,z);
      a[k++] = VISIBLE;
      }
/*
***När allt är klart har k räknats upp en gång för mycket.
*/
    k--;
/*
***Allt verkar ha gått bra.
*/
    status = 0;
/*
***Hur många vektorer blev det ?
*/
   *n = k;
    ncrdxy = k+1;
/*
***Endside.
*/
    for ( i=k/2+1; i<=k; ++i ) a[i] = a[i] | ENDSIDE;
/*
***Ev. perspektiv.
*/
    if ( actvy.vydist != 0.0 ) gppstr(x,y,z);
/*
***Felutgång.
*/
error:
    if ( prjseg != NULL ) DBfree_segments(prjseg);

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
    GE107((GMUNON *)curpek,segpek+intu,ug-intu,(short)0,out);
    x[k] = out[0]; y[k] = out[1]; z[k] = out[2];
/*
***Slut.
*/
   return(0);

 }

 /*******************************************************/
/*!******************************************************/

 static short nextnv(
        double ug)

/*      Beräknar nästa antal vektorer.
 *
 *      In: ug = Globalt u-värde.
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
    GE107((GMUNON *)curpek,segpek+intu,ug-intu,(short)3,out);
    kappa = out[15]/sskala;
/*
***Antal vektorer.
***Som mått på dsdu används sl. Se gp8.c.
***I gp8() används 0.2*SQRT(..... för en tredjedel av ett segment.
***Här är det då logiskt att ta 3 ggr. så mycket för ett streck.
*/
    nvec = (int)(0.6*SQRT(kappa)*sl*sskala*curnog + 0.5);
    if (nvec < 1 ) nvec = 1;
/*
***Slut.
*/
   return(0);

 }

 /*******************************************************/
