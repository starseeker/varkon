/**********************************************************************
*
*    gp12.c
*    ======
*
*    This file is part of the VARKON Graphics  Library.
*    URL: http://www.varkon.com
*
*    gpdrcs();    Draw coordinate system
*    gpdlcs();    Erase coordinate system
*    gpupcs();    Uppdate coordinate system
*    gpplcs();    Make polyline
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

extern VY     actvy;
extern tbool  nivtb1[];
extern double x[],y[],z[];
extern char   a[];    
extern short  actpen;
extern int    ncrdxy;

static int csmode = V3_CS_NORMAL;

/*!******************************************************/

        short gpdrcs(
        GMCSY  *csypek,
        DBTmat *pmat,
        DBptr   la,
        short   drmod)

/*      Ritar ett plan.
 *
 *      In: csypek => Pekare till plan-structure
 *          pmat   => Planets matris.
 *          la     => Logisk adress till plan i GM.
 *          drmod  => Ritmode.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          GP0012 => Display-filen full.
 *
 *      (C)microform ab 16/2/85 J. Kjellander
 *
 *      REVIDERAD:
 *
 *      3/9-85   Div, Ulf Johansson
 *      27/1/86  Penna, B. Doverud
 *      29/9/86  Ny nivåhant. R. Svedin
 *      15/10/86 drmod, J. Kellander
 *      27/12/86 hit, J. Kjellander
 *      1997-03-11 csmode, J.Kjellander
 *
 ******************************************************!*/

{
    int   k,ival;
    char  buf[80];

/*
***Kanske planet ligger på en släckt nivå ? 
*/
    if ( nivtb1[csypek->hed_pl.level] ||
          csypek->hed_pl.blank) return(0);
/*
***Generera vektorer.
*/
    k = -1;
    gpplcs(csypek,pmat,&k,x,y,a);
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
/*
***Aktivt koordinatsystem kan behöva ritas med annan penna.
*/
          if ( csypek->hed_pl.pen != actpen ) gpspen(csypek->hed_pl.pen);
#ifdef V3_X11
          if ( csmode == V3_CS_ACTIVE )
            {
            if ( wpgrst("varkon.act_csys_pen",buf)  &&
            sscanf(buf,"%d",&ival) == 1 ) gpspen((short)ival);
            }
#endif

#ifdef WIN32
          if ( csmode == V3_CS_ACTIVE )
            {
            if ( msgrst("ACT_CSYS_PEN",buf)  &&
            sscanf(buf,"%d",&ival) == 1 ) gpspen((short)ival);
            }
#endif
          if ( csypek->hed_pl.hit )
            {
            if ( stoply(k,x,y,a,la,CSYTYP) ) drwobj(TRUE);
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

        short gpdlcs(
        GMCSY  *csypek,
        DBTmat *pmat,
        DBptr la)

/*      Stryker ett koordinatsystem med adress la ur display-
 *      filen och från skärmen.
 *
 *      In: la  => Planets GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:     0 => Ok.
 *
 *      (C)microform ab 5/5-85 J. Kjellander
 *
 *      REVIDERAD:
 *
 *      3/9/85   Bug, Ulf Johansson
 *      27/12/86 hit, J. Kjellander
 *      23/3/87  Sudda ej om på släckt nivå, J. Kjellander
 *
 ******************************************************!*/

  {
     DBetype typ;
     int     k;

/*
***Om koordinatsystemet finns i DF finns det på
***skärmen, alltså sudda ur DF och från skärm.
*/
     if (fndobj(la,CSYTYP,&typ))
       {
       gphgen(la,HILIINVISIBLE);
       drwobj(FALSE);
       remobj();
       }
/*
***Om inte kan det finnas på skärmen ändå. Såvida inte
***blankat eller på släckt nivå, sudda från skärmen. 
*/
     else
       {
       if ( nivtb1[csypek->hed_pl.level] || csypek->hed_pl.blank) 
         {
         return(0);
         }
       else
         {
         k = -1;
         gpplcs(csypek,pmat,&k,x,y,a);
         if (klpply(-1,&k,x,y,a)) drwply(k,x,y,a,FALSE);
         }
       }

     return(0);
  }

/********************************************************/
/*!******************************************************/

        short gpupcs(
        GMCSY  *csypek,
        DBTmat *pmat,
        DBptr   la,
        int     mode)

/*      Uppdaterar ett koordinatsystem.
 *
 *      In: csypek = Pekare till GM-post
 *          pmat   = Transformationsmatris
 *          la     = Planets GM-adress.
 *          mode   = V3_CS_NORMAL eller V3_CS_ACTIVE
 *
 *      Ut: Inget.
 *
 *      FV:     0 => Ok.
 *
 *      (C)microform ab 1997-03-11 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Sätt mode.
*/
   csmode = mode;
/*
***Sudda.
*/
   gpdlcs(csypek,pmat,la);
/*
***Rita igen.
*/
   gpdrcs(csypek,pmat,la,DRAW);
/*
***Återställ mode.
*/
   csmode = V3_CS_NORMAL;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short gpplcs(
        GMCSY  *csypek,
        DBTmat *pmat,
        int    *n,
        double  x[],
        double  y[],
        char    a[])

/*      Bygger ett koordinatsystem i form av en polylinje.
 *
 *      IN:
 *         csypek:     Pekare till plan-structure
 *         n+1:        Offset till planets startposition
 *
 *      UT:
 *         n:          Offset till polylinjens slutposition
 *         x,y,a:      Polylinjens koordinater och status
 *
 *      FV: 0
 *
 *      (C)microform ab 16/2/85 J. Kjellander
 *
 *       5/9-85     Div., Ulf Johansson
 *       6/2/89     hide, J. Kjellander
 *       3/11/92    Z-axeln i 3D, J. Kjellander
 *       1997-03-11 csmode, J.Kjellander
 *
 ******************************************************!*/

  {
    int      k;
    gmflt    lgd,lgdz;
    DBVector pb,pl;

/*
***Initiering.
*/
    k = *n + 1;
/*
***Axlarnas längd 15% av skärmens bredd. Aktivt
***koordinatsystem gör vi lite större.
*/
    if ( csmode == V3_CS_ACTIVE )
      lgd=0.30*(actvy.vywin[2]-actvy.vywin[0]);
    else
      lgd=0.15*(actvy.vywin[2]-actvy.vywin[0]);
/*
***En pilspets på X-axeln.
*/
    pl.x_gm = 0.9*lgd;
    pl.y_gm = 0.05*lgd;
    pl.z_gm = 0.0;
    GEtfpos_to_basic( &pl, pmat, &pb);
    gppltr(&pb, x+k, y+k, z+k);
    a[ k++ ] = 0;

    pl.x_gm = lgd;
    pl.y_gm = 0.0;
    GEtfpos_to_basic( &pl, pmat, &pb);
    gppltr(&pb, x+k, y+k, z+k);
    a[ k++ ] = VISIBLE;

    pl.x_gm =  0.9*lgd;
    pl.y_gm = -0.05*lgd;
    GEtfpos_to_basic( &pl, pmat, &pb);
    gppltr(&pb, x+k, y+k, z+k);
    a[ k++ ] = VISIBLE;

    if ( csmode == V3_CS_ACTIVE )
      {
      pl.x_gm = 0.9*lgd;
      pl.y_gm = 0.05*lgd;
      pl.z_gm = 0.0;
      GEtfpos_to_basic( &pl, pmat, &pb);
      gppltr(&pb, x+k, y+k, z+k);
      a[ k++ ] = VISIBLE;
      }
/*
***Y-axeln.
*/
    pl.x_gm = 0.0;
    pl.y_gm = lgd;
    GEtfpos_to_basic( &pl, pmat, &pb);
    gppltr(&pb, x+k, y+k, z+k);
    a[ k++ ] = 0;

    if ( csmode == V3_CS_NORMAL)
      {
      pl.x_gm = 0.0;
      pl.y_gm = 0.59*lgd;
      GEtfpos_to_basic( &pl, pmat, &pb);
      gppltr(&pb, x+k, y+k, z+k);
      a[ k++ ] = VISIBLE;

      pl.x_gm = 0.0;
      pl.y_gm = 0.51*lgd;
      GEtfpos_to_basic( &pl, pmat, &pb);
      gppltr(&pb, x+k, y+k, z+k);
      a[ k++ ] = 0;
      }

    pl.x_gm = 0.0;
    pl.y_gm = 0.0;
    GEtfpos_to_basic( &pl, pmat, &pb);
    gppltr(&pb, x+k, y+k, z+k); 
    a[ k++ ] = VISIBLE;
/*
***X-axeln.
*/
    if ( csmode == V3_CS_NORMAL)
      {
      pl.x_gm = 0.51*lgd;
      pl.y_gm = 0.0;
      GEtfpos_to_basic( &pl, pmat, &pb);
      gppltr(&pb, x+k, y+k, z+k); 
      a[ k++ ] = VISIBLE;

      pl.x_gm = 0.59*lgd;
      pl.y_gm = 0.0;
      GEtfpos_to_basic( &pl, pmat, &pb);
      gppltr(&pb, x+k, y+k, z+k); 
      a[ k++ ] = 0;
      }

    pl.x_gm = lgd;
    pl.y_gm = 0.0;
    GEtfpos_to_basic( &pl, pmat, &pb);
    gppltr(&pb, x+k, y+k, z+k); 
    a[ k++ ] = VISIBLE;
/*
***Z-axel med pilspets ritas bara om det är en sned vy
***eller ett snett koordinatsystem.
*/
    if ( actvy.vy3d  ||  pmat->g11 != 1.0  ||
                         pmat->g22 != 1.0  ||
                         pmat->g33 != 1.0 )
      {
      lgdz = lgd/3.0;
      pl.x_gm = 0.0;
      pl.y_gm = 0.0;
      GEtfpos_to_basic( &pl, pmat, &pb);
      gppltr(&pb, x+k, y+k, z+k); 
      a[ k++ ] = 0;

      pl.z_gm = lgdz;
      GEtfpos_to_basic( &pl, pmat, &pb);
      gppltr(&pb, x+k, y+k, z+k); 
      a[ k++ ] = VISIBLE;

      pl.y_gm = 0.05*lgdz;
      pl.z_gm = 0.9*lgdz;
      GEtfpos_to_basic( &pl, pmat, &pb);
      gppltr(&pb, x+k, y+k, z+k);
      a[ k++ ] = 0;

      pl.y_gm = 0.0;
      pl.z_gm = lgdz;
      GEtfpos_to_basic( &pl, pmat, &pb);
      gppltr(&pb, x+k, y+k, z+k);
      a[ k++ ] = VISIBLE;

      pl.y_gm = -0.05*lgdz;
      pl.z_gm =  0.9*lgdz;
      GEtfpos_to_basic( &pl, pmat, &pb);
      gppltr(&pb, x+k, y+k, z+k);
      a[ k++ ] = VISIBLE;

      if ( csmode == V3_CS_ACTIVE )
        {
        pl.y_gm = 0.05*lgdz;
        pl.z_gm = 0.9*lgdz;
        GEtfpos_to_basic( &pl, pmat, &pb);
        gppltr(&pb, x+k, y+k, z+k);
        a[ k++ ] = VISIBLE;
        }
      }
/*
***En pilspets på Y-axeln.
*/
    pl.z_gm =  0.0;
    pl.x_gm = -0.05*lgd;
    pl.y_gm =  0.9*lgd;
    GEtfpos_to_basic( &pl, pmat, &pb);
    gppltr(&pb, x+k, y+k, z+k);
    a[ k++ ] = 0;

    pl.x_gm = 0.0;
    pl.y_gm = lgd;
    GEtfpos_to_basic( &pl, pmat, &pb);
    gppltr(&pb, x+k, y+k, z+k);
    a[ k++ ] = VISIBLE;

    pl.x_gm = 0.05*lgd;
    pl.y_gm = 0.9*lgd;
    GEtfpos_to_basic( &pl, pmat, &pb);
    gppltr(&pb, x+k, y+k, z+k);
    a[ k ] = VISIBLE;

    if ( csmode == V3_CS_ACTIVE )
      {
    ++k;
      pl.x_gm = -0.05*lgd;
      pl.y_gm =  0.9*lgd;
      GEtfpos_to_basic( &pl, pmat, &pb);
      gppltr(&pb, x+k, y+k, z+k);
      a[ k ] = VISIBLE;
      }
/*
***Ev. perspektiv.
*/
    *n = k;
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
