/**********************************************************************
*
*    gp8.c
*    =====
*
*    This file is part of the VARKON Graphics Library.
*    URL: http://www.varkon.com
*
*    gpdrcu();     Draw curve
*    gpdlcu();     Erase curve
*    gpplcu();     Make polyline
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
***Beräknar krökning och dsdu
*/
static short calcur(gmflt u, gmflt *kappa, gmflt *dsdu);

/*
***Justerar för offset 
*/
static short addofs(int k, gmflt u, double x[], double y[], double z[]);

/*
***Villkoret för ett degenererat segment är att dsdu < MINDS.
***Ett lämpligt värde på MINDS är 1E-9 eftersom detta
***inte spräcker VAX:ens talområde även efter upphöjning
***till 3, se calcur()
*/

#define MINDS 1E-9

/*
***Följande static-variabler initieras av gpplcu och
***används sedan i calcur och plyofs. Orsak : prestanda.
*/
static bool  ratseg,kubseg;
static gmflt c0x,c1x,c2x,c3x,c0y,c1y,c2y,c3y,
             c0z,c1z,c2z,c3z,c0,c1,c2,c3,offset,px,py,pz;
/*
***Externa variabler.
*/
extern DBTmat actvym;
extern VY     actvy;
extern tbool  nivtb1[];
extern double curnog,gpgszx;
extern double x[],y[],z[];
extern short  actpen;
extern int    ncrdxy;
extern char   a[];
extern bool   gpgenz;

/*!******************************************************/

        short gpdrcu(
        GMCUR *curpek,
        GMSEG *segmnt,
        DBptr  la,
        short  drmod)

/*      Ritar en kurva.
 *
 *      IN:  curpek => Pekare till curve-structure.
 *           segmnt => Array med segment.
 *           la     => Logisk adress till curve i GM.
 *           drmod  => Ritmode.
 *
 *      UT: Inget.
 *
 *      FV:      0 => Ok.
 *          GP0012 => Displayfilen full.
 *
 *      (C)microform ab 18/1/85 J. Kjellander
 *
 *      REVIDERAD:
 *
 *      3/9-85   Div, Ulf Johansson
 *      27/1/86  Penna, B. Doverud
 *      29/9/86  Ny nivåhant. R. Svedin
 *      15/10/86 drmod, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      1997-12-29 Breda linjer, J.Kjellander
 *      1999-12-15 Fill, J.Kjellander
 *
 ******************************************************!*/

  {
    int   k;
    bool  visible;

/*
***Kanske kurvan ligger på en släckt nivå eller är blankad ?
*/
    if ( nivtb1[curpek->hed_cu.level] ||
          curpek->hed_cu.blank) return(0);
/*
***Generera vektorer.
*/
    k = -1;
    gpplcu(curpek,segmnt,&k,x,y,z,a);
    ncrdxy = k+1;
/*
***Optional clipping. Don't clip CFONT 3.
*/
    if ( drmod > GEN )
      {
      ncrdxy = 0;
      if ( curpek->fnt_cu == 3 ) visible = TRUE;
      else                       visible = klpply(-1,&k,x,y,a);
      if ( visible )
        {
        ncrdxy = k+1;
/*
***Ritning och lagring i df.
*/
        if ( drmod == DRAW )
          {
          if ( curpek->hed_cu.pen != actpen ) gpspen(curpek->hed_cu.pen);
          if ( curpek->wdt_cu != 0.0 ) gpswdt(curpek->wdt_cu);
          if ( curpek->hed_cu.hit )
            {
            if ( !stoply(k,x,y,a,la,CURTYP) ) return(erpush("GP0012",""));
            if ( curpek->fnt_cu == 3 ) fillply(k,x,y,curpek->hed_cu.pen);
            else                       drwobj(TRUE);
            }
          else
            {
            if ( curpek->fnt_cu == 3 ) fillply(k,x,y,curpek->hed_cu.pen);
            else                       drwply(k,x,y,a,TRUE);
            }
          if ( curpek->wdt_cu != 0.0 ) gpswdt(0.0);
          }
        }
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short gpdlcu(
        GMCUR *curpek,
        GMSEG *segmnt,
        DBptr la)

/*      Suddar en kurva.
 *
 *      In: la  => Kurvans GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:     0 => Ok.
 *
 *      (C)microform ab 2/2-85 J. Kjellander
 *
 *      REVIDERAD:
 *
 *      3/9-85   Bug, Ulf Johansson
 *      27/12/86 hit, J. Kjellander
 *      24/3/87  Släckt nivå, J. Kjellander
 *      1997-12-29 Breda linjer, J.Kjellander
 *      1999-12-13 Fill, J.Kjellander
 *
 ******************************************************!*/

  {
     DBetype typ;
     int     k;

/*
***Sudda ur DF.
*/
     if (fndobj(la,CURTYP,&typ))
       {
       gphgen(la,HILIINVISIBLE);
       remobj();
       }
/*
***Och från skärm.
*/
     if ( nivtb1[curpek->hed_cu.level] || curpek->hed_cu.blank)
       {
       return(0);
       }
     else
       {
       k = -1;
       gpplcu(curpek,segmnt,&k,x,y,z,a);
       if (klpply(-1,&k,x,y,a))
         {
         if ( curpek->fnt_cu == 3 )
           {
           fillply(k,x,y,(short)0);
           }
         else
           {
           if ( curpek->wdt_cu != 0.0 ) gpswdt(curpek->wdt_cu);
           drwply(k,x,y,a,FALSE);
           if ( curpek->wdt_cu != 0.0 ) gpswdt(0.0);
           }
         }
       }

     return(0);
  }

/********************************************************/
/*!******************************************************/

        short gpplcu(
        GMCUR *curpek,
        GMSEG *segmnt,
        int   *n,
        double x[],
        double y[],
        double z[],
        char   a[])

/*      Bygger en GMCUR eller 3D-GMARC i form av en polyline.
 *      Resultatet lagras i globala arrayer x,y,z och a.
 *
 *      In: curpek   = Pekare till curve-structure.
 *          segmnt   = Array med segment.
 *          n+1      = Offset till polylinjen start.
 *          x,y,z,a  = Arrayer för resultat.
 *
 *      Ut: *n      = Offset till sista vektorn i polylinjen
 *          x,y,z,a = Koordinater med status.
 *
 *      FV:  0 => Ok.
 *
 *      (C)microform ab 18/1/85 J. Kjellander
 *
 *      1/7/85   Div, Ulf Johansson
 *      17/10/85 dsdu = 0.0, J. Kjellander
 *      23/1/87  Förbättrad algoritm, J. Kjellander
 *      24/4/87  Ännu bättre, J. Kjellander
 *      3/8/87   Skitbra, J. Kjellander
 *      25/5/89  hide, J. Kjellander
 *      5/11/91  rationella segment, J. Kjellander
 *      1/2/94   Ytors grafiska repr. J. Kjellander
 *      26/11/94 Sneda kurvplan i XY-vyn, J. Kjellander
 *      961106   ns_cu -> nsgr_cu, J. Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    int      i,j,k,nu,nvec1,nvec2,nvec3;
    gmflt    u,dsdu1,dsdu2,dsdu3,w,du1,du2,du3,kap1,kap2,kap3,s,cn;
    bool     offseg=FALSE;
    DBTmat   m1,m2,m3;
    GMSEG   *sp=NULL;
   
    gmflt    uv[15*100];  /* 15 = 3*5 = max antal vektorer/seg */
                          /* 100 = maxvärde för curnog */

/*
***Om det är en streckad kurva provar vi att använda
***plycud(). Om det inte gick bra ritar vi heldraget i alla fall.
*/
    if ( curpek->fnt_cu == 1  ||  curpek->fnt_cu == 2 )
      {
      status = plycud(curpek,segmnt,n,x,y,z,a);
      if ( status == 0 ) return(status);
      }
/*
***Skärmskala = s.
*/
    s = gpgszx/(actvy.vywin[2]-actvy.vywin[0]);
    if ( s > 1000.0 ) s = 1000.0;
/*
***Till att börja med sätter vi cn = curnog, men om antalet
***genererade vektorer inte får plats minskar vi cn succesivt.
*/
    cn = curnog;
/*
***Om det är en offset-kurva skall kurvplanet transformeras
***till aktiv vy.
*/
    for ( i=0; i<curpek->nsgr_cu; ++i )
      {
      if ( (segmnt+i)->ofs != 0.0 )
        {
/*
***Alla offsetkurvors kurvplan måste alltid transformeras även
***i 2D. En kurva i ett snett plan i xy-vyn tex.
*/
        V3MOME(&curpek->csy_cu,&m1,sizeof(DBTmat));
        GEtform_inv(&actvym,&m2);
        GEtform_mult(&m1,&m2,&m3);
        px = m3.g31; py = m3.g32; pz = m3.g33;
        break;
        }
      }
/*
***Prova att generara vektorer. Om det blir för många, >PLYMXV,
***hoppar vi hit igen och provar på nytt med minskad kurvnoggrannhet.
*/
loop:
    k = *n + 1;
/*
***Här börjar loopen för alla segment. Först klassificering.
***ratseg = TRUE om rationellt segment.
***cubseg = TRUE om kubiskt segment.
***offseg = TRUE om offset-segment.
***offset = segmentets offset.
*/
    for ( i=0; i<curpek->nsgr_cu; ++i )
      {
      sp = (segmnt+i);
      if ( sp->c0 == 1.0  &&  sp->c1 == 0.0
                          &&  sp->c2 == 0.0  &&  sp->c3 == 0.0 )
        ratseg = FALSE;
      else
        ratseg = TRUE;

      if ( sp->c3x == 0.0  &&  sp->c3y == 0.0
                           &&  sp->c3z == 0.0  &&  sp->c3 == 0.0 )
        kubseg = FALSE;
      else
        kubseg = TRUE;

      if ( sp->ofs != 0.0 )
        {
        offseg = TRUE;
        offset = sp->ofs;
        }
      else offseg = FALSE;
/*
***Transformera till aktiv vy.
*/
      if ( actvy.vy3d )
        {
        c0x = actvym.g11*sp->c0x + actvym.g12*sp->c0y + actvym.g13*sp->c0z;
        c1x = actvym.g11*sp->c1x + actvym.g12*sp->c1y + actvym.g13*sp->c1z;
        c2x = actvym.g11*sp->c2x + actvym.g12*sp->c2y + actvym.g13*sp->c2z;

        c0y = actvym.g21*sp->c0x + actvym.g22*sp->c0y + actvym.g23*sp->c0z;
        c1y = actvym.g21*sp->c1x + actvym.g22*sp->c1y + actvym.g23*sp->c1z;
        c2y = actvym.g21*sp->c2x + actvym.g22*sp->c2y + actvym.g23*sp->c2z;

        if ( kubseg )
          {
          c3x = actvym.g11*sp->c3x + actvym.g12*sp->c3y + actvym.g13*sp->c3z;
          c3y = actvym.g21*sp->c3x + actvym.g22*sp->c3y + actvym.g23*sp->c3z;
          }
        else c3x = c3y = c3z = 0.0;

        if ( gpgenz  ||  offseg )
          {
          c0z = actvym.g31*sp->c0x + actvym.g32*sp->c0y + actvym.g33*sp->c0z;
          c1z = actvym.g31*sp->c1x + actvym.g32*sp->c1y + actvym.g33*sp->c1z;
          c2z = actvym.g31*sp->c2x + actvym.g32*sp->c2y + actvym.g33*sp->c2z;
          c3z = actvym.g31*sp->c3x + actvym.g32*sp->c3y + actvym.g33*sp->c3z;
          }
        }
/*
***Om 2D-vy, kopiera utan transformation.
*/
      else
        {
        c0x = sp->c0x; c1x = sp->c1x; c2x = sp->c2x; c3x = sp->c3x;
        c0y = sp->c0y; c1y = sp->c1y; c2y = sp->c2y; c3y = sp->c3y;
        if ( gpgenz  ||  offseg )
          { c0z = sp->c0z; c1z = sp->c1z; c2z = sp->c2z; c3z = sp->c3z; }
        }
/*
***Oavsett vy kopieras w-termerna otransformerade.
*/
      c0 = sp->c0; c1 = sp->c1; c2 = sp->c2; c3 = sp->c3;
/*
***Antalet vektorer bestäms genom att för varje segment
***beräkna krökningen och dsdu för u=0.2, u=0.5 och u=0.8.
***Om segmentet lokalt är en rät linje eller en punkt
***returnerar calcur() kappa=0 och dsdu=0.
*/
      calcur(0.2,&kap1,&dsdu1);
      calcur(0.5,&kap2,&dsdu2);
      calcur(0.8,&kap3,&dsdu3);
/*
***Skalan s är här detsamma som i statusarean på skärmen.
***Ju större skala ju mer minskar krökningen. Å andra
***sidan behövs fler vektorer eftersom vinkelfelet skall
***minska när storleken ökar.
*/
      kap1 /= s;  kap2 /= s;  kap3 /= s;
      dsdu1 *= s; dsdu2 *= s; dsdu3 *= s;

/*
fprintf(f,"ÖnEfter skalning, kap = kap/s och sddu = dsdu*sÖn");
fprintf(f,"Kap1=%g,  Kap2=%g,  Kap3=%gÖn",kap1,kap2,kap3);
fprintf(f,"dsdu1=%g,  dsdu2=%g,  dsdu3=%gÖn",dsdu1,dsdu2,dsdu3);
*/

/*
***Antal vektorer beräknas som dsdu (ett mått på segmentets längd)
***multiplicerat med SQRT(krökningen) (i denna vy). 0.5 medför korrekt
***avrundning från flyttal till heltal. Negativt nvec kan uppstå
***vid overflow. 0.2*SQRT(kappa)*dsdu är en bra formel som jag
***uppfunnit och provat ut under många år.
*/
      nvec1 = (int)(0.2*SQRT(kap1)*dsdu1 + 0.5);
      nvec2 = (int)(0.2*SQRT(kap2)*dsdu2 + 0.5);
      nvec3 = (int)(0.2*SQRT(kap3)*dsdu3 + 0.5);
/*
***Under alla omständigheter är det knappast lönsamt att ödsla
***mer än 5 streck/tredjedel av segmentet. nvec kan bli ett stort
***tal om kurvan är stor, dvs. dsdu är ett stort tal. Så är tex.
***fallet vid en kraftig ZOOM:ning. För att slippa generera
***mängder med vektorer som ändå ligger utanför skärmen är
***en maxgräns lämplig att ha.
*/
      if ( nvec1 > 5 ) nvec1 = 5;
      if ( nvec2 > 5 ) nvec2 = 5;
      if ( nvec3 > 5 ) nvec3 = 5;
/*
***Nu är det dags att korrigera nvec för kurvnoggrannhet.
***Om man vill ha många vektorer skall man få det !
***För mycket små segment är det särskilt viktigt att se
***till att inte nvec = 0 när man sätter upp curnog.
*/
      nvec1 = (int)(nvec1*cn);
      nvec2 = (int)(nvec2*cn);
      nvec3 = (int)(nvec3*cn);

      if ( cn > 1.0 )
        {
        if ( nvec1 < 1 ) nvec1 = (int)(cn/25.0);
        if ( nvec2 < 1 ) nvec2 = (int)(1 + cn/35.0); 
        if ( nvec3 < 1 ) nvec3 = (int)(cn/25.0); 
        }
/*
***Om nvec nu är lika med 1 eller mera kan du beräknas.
***Om nvec < 1 dvs. 0 eller negativ (overflow ovan) sätts
***nvec till 0 varvid inga extra vektorer genereras.
*/
      if ( nvec1 < 1 ) nvec1 = 0; else du1 = 1.0/(3.0*nvec1);
      if ( nvec2 < 1 ) nvec2 = 0; else du2 = 1.0/(3.0*nvec2);
      if ( nvec3 < 1 ) nvec3 = 0; else du3 = 1.0/(3.0*nvec3);
/*
***Generera u-värden, om det blir för många börjar vi
***om med mindre skalfaktor.
*/
      nu = 0; u = 0.0;

      for ( j=0; j<nvec1; ++j ) { u += du1; uv[nu++] = u; }

      if ( nvec1 == 0  && nvec2 > 0 )
        { u = 1.0/3.0 - du2; nvec2++; }
      for ( j=0; j<nvec2; ++j ) { u += du2; uv[nu++] = u; }

      if ( nvec2 == 0  && nvec3 > 0 )
        { u = 2.0/3.0 - du3;nvec3++; }
      for ( j=0; j<nvec3-1; ++j ) { u += du3; uv[nu++] = u; }
/*
***Får vi plats med dessa ytterligare vektorer i x,y,z och a ?
***Om inte, prova att minska kurvnoggrannheten. Om inte detta
***går avslutar vi med status = -1.
*/
      if ( k+nu+2 > PLYMXV )
        {
        cn /= 2.0;

        if ( cn < 0.1 )
          {
         --k;
          status = -1;
          goto end;
          }
        else
          goto loop;
        }
/*
***1:a punkten på segmentet.
*/
      if ( ratseg )
        { x[k] = c0x / sp->c0; y[k] = c0y / sp->c0;
          if ( gpgenz ) z[k] = c0z / sp->c0; }
      else
        { x[k] = c0x; y[k] = c0y;
          if ( gpgenz ) z[k] = c0z; }
      if ( offseg  ) addofs(k,0.0,x,y,z);
      k++;
/*
***nu st. punkter inne på segmentet.
*/
      for ( j=0; j<nu; ++j )
        {
        u = uv[j];
/*
fprintf(f,"u=%gÖn",u);
*/
        if ( ratseg )
          {
          if ( kubseg )
            {
            w = sp->c0 + u*(sp->c1 + u*(sp->c2 + u*sp->c3));
            x[k] = (c0x + u*(c1x + u*(c2x + u*c3x)))/w;
            y[k] = (c0y + u*(c1y + u*(c2y + u*c3y)))/w;
            if ( gpgenz ) z[k] = (c0z + u*(c1z + u*(c2z + u*c3z)))/w;
            }
          else
            {
            w = sp->c0 + u*(sp->c1 + u*(sp->c2));
            x[k] = (c0x + u*(c1x + u*c2x))/w;
            y[k] = (c0y + u*(c1y + u*c2y))/w;
            if ( gpgenz ) z[k] = (c0z + u*(c1z + u*c2z))/w;
            }
          }
        else 
          {
          if ( kubseg )
            {
            x[k] = c0x + u*(c1x + u*(c2x + u*c3x));
            y[k] = c0y + u*(c1y + u*(c2y + u*c3y));
            if ( gpgenz ) z[k] = c0z + u*(c1z + u*(c2z + u*c3z));
            }
          else
            {
            x[k] = c0x + u*(c1x + u*c2x);
            y[k] = c0y + u*(c1y + u*c2y);
            if ( gpgenz ) z[k] = c0z + u*(c1z + u*c2z);
            }
          }
/*
***Justera eventuellt för offset och räkna upp index-variabeln k.
*/
          if ( offseg ) addofs(k,u,x,y,z);
          ++k;
        }
      }
/*
***Sista punkten på sista segmentet, u=1.
*/
    if ( ratseg )
      {
      if ( kubseg )
        {
        w = sp->c0 + sp->c1 + sp->c2 + sp->c3;
        x[k] = (c0x + c1x + c2x + c3x)/w;
        y[k] = (c0y + c1y + c2y + c3y)/w;
        if ( gpgenz ) z[k] = (c0z + c1z + c2z + c3z)/w;
        }
      else
        {
        w = sp->c0 + sp->c1 + sp->c2;
        x[k] = (c0x + c1x + c2x)/w;
        y[k] = (c0y + c1y + c2y)/w;
        if ( gpgenz ) z[k] = (c0z + c1z + c2z)/w;
        }
      }
    else
      {
      if ( kubseg )
        {
        x[k] = c0x + c1x + c2x + c3x;
        y[k] = c0y + c1y + c2y + c3y;
        if ( gpgenz ) z[k] = c0z + c1z + c2z + c3z;
        }
      else
        {
        x[k] = c0x + c1x + c2x;
        y[k] = c0y + c1y + c2y;
        if ( gpgenz ) z[k] = c0z + c1z + c2z;
        }
      }
    if ( offseg ) addofs(k,1.0,x,y,z);
/*
***Allt verkar ha gått bra.
*/
   status = 0;
/*
***Hit kommer vi om det inte gick bra.
*/
end:
/*
***Status-information.
*/
   a[*n+1] = 0;
   for (i = *n+2; i <= k; ++i) a[i] = VISIBLE;
   for (i = k/2 +1; i <= k; ++i) a[i] = a[i] | ENDSIDE;
   *n = k;
/*
***Ev. perspektivprojektion.
*/
   ncrdxy = k+1;
   if ( actvy.vydist != 0.0 ) gppstr(x,y,z);
/*
***Slut.
*/
   return(status);
  }

/********************************************************/
/*!******************************************************/

        static short calcur(
        gmflt  u,
        gmflt *kappa,
        gmflt *dsdu)

/*      Beräknar kappa och dsdu för ett givet u på ett
 *      2D-kurvsegment.
 *      Globala variabler c0x, c0y osv. samt ratseg och kubseg
 *      används.
 *      Om segmentet lokalt är en rät linje eller en punkt
 *      returnerar calcur() kappa=0 och dsdu=0.
 *
 *      (C)microform ab 8/12/91 J. Kjellander
 *
 *      20/4/92  MINDS, J. Kjellander
 *
 ******************************************************!*/

  {
    gmflt xw,yw,w,dxdu,dydu,d2xdu2,d2ydu2,w2,w4,
          dwdu,dxwdu,dywdu,d2xwdu2,d2ywdu2,d2wdu2,tmp;

/*
***Koordinater (homogena) behövs bara om det är ett rationellt segment.
*/
      if ( ratseg )
        {
        xw = c0x + u*(c1x + u*c2x);
        yw = c0y + u*(c1y + u*c2y);
        w  = c0  + u*(c1  + u*c2);
        if ( kubseg )
          {
          xw += u*u*u*c3x;
          yw += u*u*u*c3y;
          w  += u*u*u*c3;
          }
        }
/*
***1:a derivatan.
*/
      dxwdu = c1x + u*(2.0*c2x + 3.0*u*c3x);
      dywdu = c1y + u*(2.0*c2y + 3.0*u*c3y);
      if ( ratseg ) dwdu = c1 + u*(2.0*c2 + 3.0*u*c3);
/*
***2:a derivatan.
*/
      d2xwdu2 = 2.0*c2x + 6.0*u*c3x;
      d2ywdu2 = 2.0*c2y + 6.0*u*c3y;
      if ( ratseg ) d2wdu2 = 2.0*c2 + 6.0*u*c3;
/*
***Om det är ett icke rationellt segment kan vi nu beräkna
***resultatet direkt. Om dsdu < MINDS har vi troligen ett
***segment som degenererat till en punkt. Isåfall returnerar
***vi 0.
*/
      if ( !ratseg )
        {
        tmp = SQRT(dxwdu*dxwdu + dywdu*dywdu);
        if ( tmp < MINDS )
          {
          *kappa = *dsdu = 0.0;
          }
        else
          {
          *kappa = ABS((dxwdu*d2ywdu2 - dywdu*d2xwdu2)/(tmp*tmp*tmp));
          *dsdu = tmp;
          }
        }
/*
***Rationella segment kräver lite mera.
*/
      else
        {
        w2 = w*w; w4 = w2*w2;

        dxdu = (w*dxwdu - dwdu*xw)/w2;
        dydu = (w*dywdu - dwdu*yw)/w2;

        d2xdu2 = (w*d2xwdu2 - dwdu*dxwdu)/w2 -
                 ((d2wdu2*xw + dwdu*dxwdu)*w2 - 2.0*w*xw*dwdu*dwdu)/w4;
        d2ydu2 = (w*d2ywdu2 - dwdu*dywdu)/w2 -
                 ((d2wdu2*yw + dwdu*dywdu)*w2 - 2.0*w*yw*dwdu*dwdu)/w4;

        tmp = SQRT(dxdu*dxdu + dydu*dydu);
        if ( tmp < MINDS )
          {
          *kappa = *dsdu = 0.0;
          }
        else
          {
          *kappa = ABS((dxdu*d2ydu2 - dydu*d2xdu2)/(tmp*tmp*tmp));
          *dsdu = tmp;
          }
        }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        static short addofs(
        int    k,
        gmflt  u,
        double x[],
        double y[],
        double z[])

/*      Adderar offset till en punkt i x,y och z.
 *
 *      In: 
 *         k   =  Index i grafik-arrayerna x,y,z och a.
 *         u   =  u-värde.
 *
 *      Ut:
 *         x,y,z:       X-,y-koordinater uppdaterade
 *                      Om gpgenz även Z-koordinaten.
 *
 *      (C)microform ab 15/11/91 J. Kjellander
 *
 ******************************************************!*/

  {
    gmflt xw,yw,zw,w,dxdu,dydu,dzdu,dsdu,w2,
          dwdu,dxwdu,dywdu,dzwdu,tx,ty,tz;

/*
***Om det är ett rationellt segment måste
***homogena koordinater beräknas.
*/
    if ( ratseg )
      {
      xw = c0x + u*(c1x + u*(c2x + u*c3x));
      yw = c0y + u*(c1y + u*(c2y + u*c3y));
      zw = c0z + u*(c1z + u*(c2z + u*c3z));
      w = c0 + u*(c1 + u*(c2 + u*c3));
      }
/*
***1:a derivatan i punkten.
*/
    dxwdu = c1x + u*(2.0*c2x + 3.0*u*c3x);
    dywdu = c1y + u*(2.0*c2y + 3.0*u*c3y);
    dzwdu = c1z + u*(2.0*c2z + 3.0*u*c3z);
    if ( ratseg ) dwdu = c1 + u*(2.0*c2 + 3.0*u*c3);
/*
***För ett icke rationellt segment kan tangenten beräknas
***direkt.
*/
    if ( !ratseg )
      {
      dsdu = SQRT(dxwdu*dxwdu + dywdu*dywdu + dzwdu*dzwdu);
      if ( dsdu < 1e-14 ) dsdu = 1e-10;
      tx = dxwdu/dsdu; ty = dywdu/dsdu; tz = dzwdu/dsdu;
      }
/*
***Rationella segment kräver lite mera.
*/
    else
      {
      w2 = w*w;

      dxdu = (w*dxwdu - dwdu*xw)/w2;
      dydu = (w*dywdu - dwdu*yw)/w2;
      dzdu = (w*dzwdu - dwdu*zw)/w2;

      dsdu = SQRT(dxdu*dxdu + dydu*dydu + dzdu*dzdu);
      if ( dsdu < 1e-14 ) dsdu = 1e-10;
      tx = dxdu/dsdu; ty = dydu/dsdu; tz = dzdu/dsdu;
      }
/*
***Normalen fås som tangenten X planets normal och sen
***är det bara att lägga till "offsettet".
*/
    x[k] += offset*(ty*pz - py*tz);
    y[k] += offset*(tz*px - pz*tx);
    if ( gpgenz ) z[k] += offset*(tx*py - px*ty);

   return(0);
  }

/********************************************************/
