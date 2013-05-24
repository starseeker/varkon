/**********************************************************************
*
*    gp23.c
*    ======
*
*    This file is part of the VARKON Graphics  Library.
*    URL: http://www.varkon.com
*
*    gpdrbp();    Draw B_plane
*    gpdlbp();    Erase B_plane
*    gpplbp();    Make polyline
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

extern tbool  nivtb1[];
extern bool   gpgenz;
extern double x[],y[],z[];
extern char   a[];    
extern short  actpen;
extern int    ncrdxy;
extern VY     actvy;

/*!******************************************************/

        short gpdrbp(
        GMBPL  *bplpek,
        DBptr   la,
        short   drmod)

/*      Ritar ett B-plan.
 *
 *      In: bplpek => Pekare till Bplan-structure
 *          la     => Logisk adress till plan i GM.
 *          drmod  => Ritmode.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          GP0012 => Display-filen full.
 *
 *      (C)microform ab 27/8/87 J. Kjellander
 *
 *      REVIDERAD:
 *
 ******************************************************!*/

{
    int k;

/*
***Kanske planet ligger på en släckt nivå ? 
*/
    if ( nivtb1[bplpek->hed_bp.level] ||
          bplpek->hed_bp.blank) return(0);
/*
***Generera vektorer.
*/
    k = -1;
    gpplbp(bplpek,&k);
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
          if ( bplpek->hed_bp.pen != actpen ) gpspen(bplpek->hed_bp.pen);
          if ( bplpek->wdt_bp != 0.0 ) gpswdt(bplpek->wdt_bp);
          if ( bplpek->hed_bp.hit )
            {
            if ( stoply(k,x,y,a,la,BPLTYP) ) drwobj(TRUE);
            else return(erpush("GP0012",""));
            }
          else
            {
            drwply(k,x,y,a,TRUE);
            }
          if ( bplpek->wdt_bp != 0.0 ) gpswdt(0.0);
          }
        }
      }

    return(0);
}

/********************************************************/
/*!******************************************************/

        short gpdlbp(
        GMBPL  *bplpek,
        DBptr la)

/*      Stryker ett B-plan med adress la ur display-
 *      filen och från skärmen.
 *
 *      In: bplpek => GM-structure.
 *          la     => B-planets GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:     0 => Ok.
 *
 *      (C)microform ab 27/8/87 J. Kjellander
 *
 *      REVIDERAD:
 *
 ******************************************************!*/

  {
     DBetype typ;
     int     k;

/*
***Om B-planet finns i DF finns det på
***skärmen, alltså sudda ur DF och från skärm.
*/
     if (fndobj(la,BPLTYP,&typ))
       {
       gphgen(la,HILIINVISIBLE);
       if ( bplpek->wdt_bp != 0.0 ) gpswdt(bplpek->wdt_bp);
       drwobj(FALSE);
       if ( bplpek->wdt_bp != 0.0 ) gpswdt(0.0);
       remobj();
       }
/*
***Om inte kan det finnas på skärmen ändå. Såvida inte
***blankat eller på släckt nivå, sudda från skärmen. 
*/
     else
       {
       if ( nivtb1[bplpek->hed_bp.level] || bplpek->hed_bp.blank) 
         {
         return(0);
         }
       else
         {
         k = -1;
         gpplbp(bplpek,&k);
         if ( bplpek->wdt_bp != 0.0 ) gpswdt(bplpek->wdt_bp);
         if (klpply(-1,&k,x,y,a)) drwply(k,x,y,a,FALSE);
         if ( bplpek->wdt_bp != 0.0 ) gpswdt(0.0);
         }
       }

     return(0);
  }

/********************************************************/
/*!******************************************************/

        short gpplbp(
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
 *
 ******************************************************!*/

  {
    int k;

/*
***Initiering.
*/
    k = *n+1;
/*
***Transformera de fyra positionerna till aktiv vy
***och lagra i x,y.
*/
    gppltr(&bplpek->crd1_bp,x+k, y+k, z+k);
    a[k++] = 0;

    gppltr(&bplpek->crd2_bp,x+k, y+k, z+k);
    a[k++] = VISIBLE;

    gppltr(&bplpek->crd3_bp,x+k, y+k, z+k);
    a[k++] = VISIBLE;

    gppltr(&bplpek->crd4_bp,x+k, y+k, z+k);
    a[k++] = VISIBLE;
/*
***En sista tänd förflyttning till startpunkten igen.
*/
    *(x+k) = *(x+k-4);
    *(y+k) = *(y+k-4);
    if ( gpgenz ) *(z+k) = *(z+k-4);
    a[k] = VISIBLE;

    *n = k;
/*
***Ev. perspektiv.
*/
   if ( actvy.vydist != 0.0 )
     {
     ncrdxy = k+1;
     gppstr(x,y,z);
     }
/*
***Slut.
*/
   return(0);

  }

/********************************************************/
