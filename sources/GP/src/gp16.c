/**********************************************************************
*
*    gp16.c
*    ======
*
*    This file is part of the VARKON Graphics  Library.
*    URL: http://www.varkon.com
*
*    gpdrsu();      Draw surf
*    gpdlsu();      Erase surf
*    gpplsu();      Make polyline
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
#include "../include/GP.h"
#include <math.h>

/*
***Bygger FAC_SUR
*/
static short plyfac(GMSUR *surpek, int *n, double x[], double y[],
             double z[], char a[]);

/*
***Transformerar punkt till vy
*/
static short vy_trf(DBVector *p, int k, double x[], double y[], double z[]);


/*
***Externa variabler.
*/
extern tbool  nivtb1[];
extern double x[],y[],z[],curnog;
extern short  actpen;
extern int    ncrdxy;
extern char   a[];
extern bool   gpgenz;
extern VY     actvy;
extern DBTmat actvym;

/*!******************************************************/

        short gpdrsu(
        GMSUR *surpek,
        GMSEG *sptarr[],
        DBptr  la,
        short  drmod)

/*      Ritar en yta.
 *
 *      IN:  surpek => Pekare till yt-structure.
 *           sptarr => Pekare till grafiska segment.
 *           la     => Logisk adress till yta i GM.
 *           drmod  => Ritmode.
 *
 *      UT: Inget.
 *
 *      FV:      0 => Ok.
 *          GP0012 => Displayfilen full.
 *
 *      (C)microform ab 31/1/94 J. Kjellander
 *
 ******************************************************!*/

  {
    int k;

/*
***Kanske ytan ligger på en släckt nivå eller är blankad ?
*/
    if ( nivtb1[surpek->hed_su.level] ||
          surpek->hed_su.blank) return(0);
/*
***Generera vektorer.
*/
    k = -1;
    gpplsu(surpek,sptarr,&k,x,y,z,a);
    ncrdxy = k+1;
/*
***Ev. klippning.
*/
    if ( drmod > GEN )
      {
      ncrdxy = 0;
      if (klpply(-1,&k,x,y,a))
        {
        ncrdxy = k+1;
/*
***Ritning och lagring i df.
*/
        if ( drmod == DRAW )
          {
          if ( surpek->hed_su.pen != actpen ) gpspen(surpek->hed_su.pen);
          if ( surpek->hed_su.hit )
            {
            if ( stoply(k,x,y,a,la,SURTYP) ) drwobj(TRUE);
            else return(erpush("GP0012",""));
            }
          else
            {
            drwply(k,x,y,a,TRUE);
            }
          }
        }
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short gpdlsu(
        GMSUR *surpek,
        GMSEG *sptarr[],
        DBptr la)

/*      Suddar en yta.
 *
 *      IN:  surpek => Pekare till yt-structure.
 *           sptarr => Pekare till grafiska segment.
 *           la     => Logisk adress till yta i GM.
 *
 *      Ut: Inget.
 *
 *      FV:     0 => Ok.
 *
 *      (C)microform ab 31/1/94 J. Kjellander
 *
 ******************************************************!*/

  {
       DBetype typ;
       int     k;

/*
***Om ytan finns i DF, sudda snabbt.
*/
       if (fndobj(la,SURTYP,&typ))
         {
         gphgen(la,HILIINVISIBLE);
         drwobj(FALSE);
         remobj();
         }
/*
***Om inte sudda långsamt såvida inte släckt.
*/
       else
         {
         if ( nivtb1[surpek->hed_su.level] || surpek->hed_su.blank)
           {
           return(0);
           }
         else
           {
           k = -1;
           gpplsu(surpek,sptarr,&k,x,y,z,a);
           if (klpply(-1,&k,x,y,a)) drwply(k,x,y,a,FALSE);
           }
         }

       return(0);
  }

/********************************************************/
/*!******************************************************/

        short gpplsu(
        GMSUR *surpek,
        GMSEG *sptarr[],
        int   *n,
        double x[],
        double y[],
        double z[],
        char   a[])

/*      Bygger en GMSUR i form av en polyline.
 *      Resultatet lagras i globala arrayer x,y,z och a.
 *
 *      In: surpek =  Pekare till yt-structure.
 *          sptarr =  Array med pekare till grafiska segment.
 *
 *      Ut: n      =  Offset till sista vektorn i polylinjen
 *
 *      FV: 0 => Ok.
 *
 *      (C)microform ab 31/1/94 J. Kjellander
 *
 *      5/12/94  kurvlängd=0, J. Kjellander
 *      9/1/96   FAC_SUR, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    double  cn_org,vd_org;
    GMCUR   cur;

/*
***Tills vidare har vi bara en font.
*/
    if ( surpek->fnt_su != 0 ) return(0);
/*
***Facettytor har annan grafisk representation.
*/
   if ( surpek->typ_su == FAC_SUR )
     return( plyfac(surpek,n,x,y,z,a));
/*
***Initiera div data i en kurv-post så att vi kan 
***använda den som indata till gpplcu().
*/
    cur.hed_cu.type = CURTYP;
    cur.plank_cu    = FALSE;
/*
***Vi börjar med den kurvnoggrannhet som användaren begärt, men
***om det blir för många vektorer minskar vi succesivt.
***För att kunna återställa curnog sparar vi det ursprungliga 
***värdet här. För att inte gpplcu() skall perspektivtransformera
***hela polylinjen vi varje anrop stänger vi tillfälligt av och
***slår på först på slutet igen.
*/
    cn_org = curnog;
    vd_org = actvy.vydist;
    actvy.vydist = 0.0;

start:
    *n = -1;
/*
***Randkurva 1. V=0.
*/
    cur.fnt_cu = 0;
    cur.lgt_cu = 0;
    cur.al_cu  = 0.0;

    cur.ns_cu = cur.nsgr_cu = surpek->ngseg_su[0];
    if ( cur.ns_cu > 0 )
      {
      status = gpplcu(&cur,sptarr[0],n,x,y,z,a);
      if ( status == -1 )
        {
        curnog /= 2.0;
        if ( curnog < 0.1 ) goto end;
        else                goto start;
        }
      }
/*
***Randkurva 2. U=1.
*/
    cur.al_cu = 0.0;
    cur.ns_cu = cur.nsgr_cu = surpek->ngseg_su[1];

    if ( cur.ns_cu > 0 )
      {
      status = gpplcu(&cur,sptarr[1],n,x,y,z,a);
      if ( status == -1 )
        {
        curnog /= 2.0;
        if ( curnog < 0.1 ) goto end;
        else                goto start;
        }
      }
/*
***Randkurva 3. V=1.
*/
    cur.al_cu = 0.0;
    cur.ns_cu = cur.nsgr_cu = surpek->ngseg_su[2];

    if ( cur.ns_cu > 0 )
      {
      status = gpplcu(&cur,sptarr[2],n,x,y,z,a);
      if ( status == -1 )
        {
        curnog /= 2.0;
        if ( curnog < 0.1 ) goto end;
        else                goto start;
        }
      }
/*
***Randkurva 4. U=0.
*/
    cur.al_cu = 0.0;
    cur.ns_cu = cur.nsgr_cu = surpek->ngseg_su[3];

    if ( cur.ns_cu > 0 )
      {
      status = gpplcu(&cur,sptarr[3],n,x,y,z,a);
      if ( status == -1 )
        {
        curnog /= 2.0;
        if ( curnog < 0.1 ) goto end;
        else                goto start;
        }
      }
/*
***Mittkurva 1. U="mitten".
*/
    cur.al_cu  = 0.0;
    cur.fnt_cu = 1;
    cur.lgt_cu = surpek->lgt_su;
    cur.ns_cu  = cur.nsgr_cu = surpek->ngseg_su[4];

    if ( cur.ns_cu > 0 )
      {
      status = gpplcu(&cur,sptarr[4],n,x,y,z,a);
      if ( status == -1 )
        {
        curnog /= 2.0;
        if ( curnog < 0.1 ) goto end;
        else                goto start;
        }
      }
/*
***Mittkurva 2. V="mitten".
*/
    cur.al_cu = 0.0;
    cur.ns_cu = cur.nsgr_cu = surpek->ngseg_su[5];

    if ( cur.ns_cu > 0 )
      {
      status = gpplcu(&cur,sptarr[5],n,x,y,z,a);
      if ( status == -1 )
        {
        curnog /= 2.0;
        if ( curnog < 0.1 ) goto end;
        else                goto start;
        }
      }
/*
***Slut, återställ curnog.
*/
end:
    curnog = cn_org;

/*
***Ev. perspektivprojektion.
*/
    ncrdxy = *n + 1;

    actvy.vydist = vd_org;
    if ( actvy.vydist != 0.0 ) gppstr(x,y,z);

    return(0);
  }

/********************************************************/
/*!******************************************************/

 static short plyfac(
        GMSUR *surpek,
        int   *n,
        double x[],
        double y[],
        double z[],
        char   a[])

/*      Ritar facett-yta.
 *
 *      In: surpek =  Pekare till yt-structure.
 *          sptarr =  Array med pekare till grafiska segment.
 *
 *      Ut: n      =  Offset till sista vektorn i polylinjen
 *
 *      FV: 0 => Ok.
 *
 *      (C)microform ab 1996-01-28 J. Kjellander
 *
 ******************************************************!*/

  {
   int     k,i,j,nvec,ustop,vstop;
   GMPAT  *patpek,*toppat;
   GMPATF *facpat;

/*
***Hämta ytans patchar från GM.
*/
   DBread_patches(surpek,&patpek);
   toppat = patpek;
/*
***Hur många positioner kan det bli som mest ? (6/patch)
*/
   nvec = surpek->nu_su*surpek->nv_su*6;
/*
***För att detta skall få plats med tanke på PLYMXV krävs det lite
***kontroller. Om det inte får plats ritar vi inte hela ytan.
*/
   if ( nvec < PLYMXV )
     {
     ustop = surpek->nu_su;
     vstop = surpek->nv_su;
     }
   else
     {
     if ( surpek->nu_su == 1 )
       {
       ustop = 1;
       vstop = (int)HEL((double)PLYMXV/(double)nvec*(double)surpek->nv_su);
       }
     else if ( surpek->nv_su == 1 )
       {
       ustop = (int)HEL((double)PLYMXV/(double)nvec*(double)surpek->nu_su);
       vstop = 1;
       }
     else
       {
       ustop = 2;
       vstop = 2;
       }
     }
/*
***Loopa igenom alla patchar och lagra ränder i x,y,z.
*/
   k = 0;

   for ( i=0; i<ustop; ++i )
     {
     for ( j=0; j<vstop; ++j )
       {
       if ( toppat->styp_pat == FAC_PAT )
         {
         facpat = (GMPATF *)toppat->spek_c;
/*
***Under alla omständigheter skall de fyra yttre ränderna visas.
*/
         vy_trf(&facpat->p1,k,x,y,z);
         a[k++] = 0;
         vy_trf(&facpat->p2,k,x,y,z);
         a[k++] = VISIBLE;
         vy_trf(&facpat->p3,k,x,y,z);
         a[k++] = VISIBLE;
         vy_trf(&facpat->p4,k,x,y,z);
         a[k++] = VISIBLE;
         vy_trf(&facpat->p1,k,x,y,z);
         a[k++] = VISIBLE;
/*
***Om det är 2 trianglar ritar vi även en diagonal.
*/
         if ( facpat->triangles )
           {
           vy_trf(&facpat->p3,k,x,y,z);
           a[k++] = VISIBLE;
           }
         }
       ++toppat;
       }
     }
   DBfree_patches(surpek,patpek);
/*
***Hur många vektorer blev det ?
*/
  *n = k-1;
/*
***Ev. perspektivprojektion.
*/
    ncrdxy = *n + 1;

    if ( actvy.vydist != 0.0 ) gppstr(x,y,z);

   return(0);
  }

/********************************************************/
/*!******************************************************/

 static short vy_trf(
        DBVector *p,
        int    k,
        double x[],
        double y[],
        double z[])

/*      Parallell-projicerar en 3D-punkt till aktuellt
 *      vyplan. Kopia av gppltr men jobbar på x,y,z som
 *      indata istället för globala x,y,z. Detta är nöd-
 *      vändigt för att multifönster skall funka.
 *
 *      In: p => 3D punkt.
 *
 *      Ut: Projizerade koordinater i xÄkÅ och yÄkÅ samt
 *          om gpgenz == TRUE även i zÄkÅ.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 1996-01-29 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Om det är en 3D-vy måste åtminstone X och Y-koordinaten
***transformeras.
*/
    if ( actvy.vy3d )
      {
      x[k] = actvym.g11 * p->x_gm +
             actvym.g12 * p->y_gm +
             actvym.g13 * p->z_gm;

      y[k] = actvym.g21 * p->x_gm +
             actvym.g22 * p->y_gm +
             actvym.g23 * p->z_gm;

      if ( gpgenz )
        {
        z[k] = actvym.g31 * p->x_gm +
               actvym.g32 * p->y_gm +
               actvym.g33 * p->z_gm;
        }
      }
/*
***Om det inte är en 3D-vy behövs ingen transformation.
*/
    else
      {
      x[k] = p->x_gm; y[k] = p->y_gm;
      if ( gpgenz ) z[k] = p->z_gm;
      }

    return(0);
  }

/********************************************************/
