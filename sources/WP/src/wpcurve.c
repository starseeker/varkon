/**********************************************************************
*
*    wpcurve.c
*    =========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPdrcu();      Draw curve
*    WPdlcu();      Delete curve
*    WPplcu();      Create 3D polyline
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
#include "../../GE/include/GE.h"
#include "../include/WP.h"
#include <math.h>

#define MINDS 1E-9   /* Minimum value accepted for ds/dt */

/*
***Statics for fast computations.
*/
static DBCurve *cp;                     /* C-ptr to current curve */
static DBSeg   *sp;                     /* C-ptr to current array of segments */
static double   actu,prevu,actl,sl;     /* Current state for vector generation */
static int      nvec;                   /* Current state for vector generation */
static double   cn;                     /* Current curve accuracy */

static int   drawcu(WPGWIN *gwinpt, DBCurve *curpek, DBSeg *segpek,
                    DBptr la, bool draw);
static int   mk_solid(double scale, int *n, double x[], double y[],
                      double z[], char a[]);
static int   mk_dash(double scale, int *n, double x[], double y[],
                     double z[], char a[]);
static int   nextu(double ds);
static void  nextp(double ug, int k, double x[], double y[], double z[]);
static void  nextnv(double scale, double ug);

/*
***External variables.
*/
extern int  actpen;

/*!******************************************************/

        short    WPdrcu(
        DBCurve *curpek,
        DBSeg   *segpek,
        DBptr    la,
        DBint    win_id)

/*      Display a curve in a window.
 *
 *      In: curpek => Pekare till cur-post.
 *          segpek => Pekare till DBSeg.
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
***Loopa igenom alla WPGWIN-fönster.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
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
         if ( WPnivt(gwinpt->nivtab,curpek->hed_cu.level) )
           {
/*
***Ja. Kolla att rätt färg är inställd.
*/
           if ( curpek->hed_cu.pen != actpen ) WPspen(curpek->hed_cu.pen);
           if ( curpek->wdt_cu != 0.0 ) WPswdt(gwinpt->id.w_id,curpek->wdt_cu);
/*
***Sen är det bara att rita.
*/
           drawcu(gwinpt,curpek,segpek,la,TRUE);
           if ( curpek->wdt_cu != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short    WPdlcu(
        DBCurve *curpek,
        DBSeg   *segpek,
        DBptr    la,
        DBint    win_id)

/*      Delete a curve from a window.
 *
 *      In: curpek => Pekare till cur-post.
 *          segpek => Pekare till DBSeg.
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
***Loopa igenom alla WPGWIN-fönster.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
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
         if ( WPfobj(gwinpt,la,CURTYP,&typ) ) WProbj(gwinpt);
/*
***Om den nu ligger på en släckt nivå eller är blankad gör vi
***inget mer. Annars får vi återskapa polylinjen och sudda från skärm.
*/
         if ( !WPnivt(gwinpt->nivtab,curpek->hed_cu.level)  ||
                             curpek->hed_cu.blank) return(0);

         if ( curpek->wdt_cu != 0.0 ) WPswdt(gwinpt->id.w_id,curpek->wdt_cu);
         drawcu(gwinpt,curpek,segpek,la,FALSE);
         if ( curpek->wdt_cu != 0.0 ) WPswdt(gwinpt->id.w_id,0.0);
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static int      drawcu(
        WPGWIN  *gwinpt,
        DBCurve *curpek,
        DBSeg   *segpek,
        DBptr    la,
        bool     draw)

/*      Draw a curve in a window and store in DF.
 *
 *      In: gwinpt => Pekare till fönster.
 *          curpek => Pekare till cur-post.
 *          segpek => Pekare till DBSeg.
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
   double x[PLYMXV],y[PLYMXV],z[PLYMXV],scale;
   char   a[PLYMXV];
   int    n;

/*
***Current scale factor in this window =
***(n_pixels * pixel_size) / model_window_size
*/
   scale = (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin) *
            gwinpt->geo.psiz_x /
           (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin);
/*
***Create graphical polyline representation.
*/
   n = -1;
   WPplcu(curpek,segpek,scale,&n,x,y,z,a);
/*
***Project on current view of window.
*/
   WPpply(gwinpt,n,x,y,z);
/*
***Clip the polyline to the window borders.
***Display or erase visible parts.
*/
   if ( curpek->fnt_cu == 3  ||  WPcply(&gwinpt->vy.modwin,(short)-1,&n,x,y,a) )
     {
     if ( draw  &&  curpek->hed_cu.hit )
       {
       if ( !WPsply(gwinpt,n,x,y,a,la,CURTYP) ) return(erpush("GP0012",""));
       if ( curpek->fnt_cu == 3 ) WPfply(gwinpt,n,x,y,draw);
       else WPdobj(gwinpt,TRUE);
       }
     else
       {
       if ( curpek->fnt_cu == 3 ) WPfply(gwinpt,n,x,y,draw);
       else WPdply(gwinpt,n,x,y,a,draw);
       }
     }

   return(0);
 }

/***********************************************************/
/*!******************************************************/

        short    WPplcu(
        DBCurve *curpek,
        DBSeg   *segmnt,
        double   scale,
        int     *n,
        double   x[],
        double   y[],
        double   z[],
        char     a[])

/*      Creates the graphical 3D polyline representation 
 *      for a curve. Note that graphical rep's of curves
 *      are linear/quadric or cubic rational or non rational
 *      segments with optional offset. NURBS rep. of curves
 *      cannot be used to create a polyline.
 *
 *      In:  curpek  =  C-ptr to DBCurve.
 *           segmnt  =  Ptr to array of curve segments.
 *           scale   =  Curve scale factor (ZOOM-status). Used to
 *                      determine step length.
 *           n+1     =  Offset to polyline start.
 *
 *      Out: n       =  Offset to polyline end.
 *           x,y,z,a =  Polyline coordinates and status
 *
 *      FV:   0 = Ok.
 *
 *      (C)2006-12-10 J. Kjellander
 *
 ******************************************************!*/

  {
   int    status;

/*
***Initialization of global variables.
*/
   cp = curpek;
   sp = segmnt;
   WPget_cacc(&cn);
/*
***What font ?.
*/
    switch ( curpek->fnt_cu )
      {
/*
***CFONT=Solid.
*/
      case 0:
      status = mk_solid(scale,n,x,y,z,a);
      break;
/*
***CFONT=Dashed. If this doesn't work, try solid instead.
*/
      case 1:
      case 2:
      status = mk_dash(scale,n,x,y,z,a);
      if ( status < 0 ) status = mk_solid(scale,n,x,y,z,a);
      break;
/*
***Unknown curve font.
*/
      default:
      status = -2;
      break;
      }

   return(status);
 }

/********************************************************/
/*!******************************************************/

 static int    mk_solid(
        double scale,
        int   *n,
        double x[],
        double y[],
        double z[],
        char   a[])

/*      Creates the graphical 3D polyline representation 
 *      for a solid curve.
 *
 *      In:  scale   = Curve scale factor (ZOOM-status).
 *           n+1     =  Offset to polyline start.
 *
 *      Out: n       =  Offset to polyline end.
 *           x,y,z,a =  Polyline coordinates and status.
 *
 *      Return:   0 = Ok.
 *
 *      (C)2006-12-16 J. Kjellander
 *
 *      2007-01-12 n_zero, J.Kjellander
 *
 ******************************************************!*/

  {
   int     i,j,k,nvec1,nvec2,nvec3,n_zero,nt,status;
   double  out[17],t;
   double  dt1,dt2,dt3,dsdt1,dsdt2,dsdt3,kap1,kap2,kap3;
   double  tv[15*100];    /* 15 = 3*5 = max vectors/seg */
                          /* 100 = max curnog */

/*
***Restart here with a lower value of cn if too
***many vectors are generated.
*/
loop:
    k = *n + 1;
/*
***Each curve segment is treated separately.
***The segment loop starts here.
*/
   for ( i=0; i<cp->nsgr_cu; ++i )
     {
/*
***The number of vectors for each segment is determined
***by calculating curvature and ds/dt in 3 places/segment,
***t=0.2, t=0.5 and t=0.8.
***kappa=dsdt=0 => Point or straight line.
*/
     GE107((DBAny *)cp,sp,(double)0.2,(short)3,out);
     dsdt1 = SQRT(out[3]*out[3] + out[4]*out[4] + out[5]*out[5]);
     if ( dsdt1 < MINDS ) dsdt1 = 0.0;
     kap1 = out[15];

     GE107((DBAny *)cp,sp,(double)0.5,(short)3,out);
     dsdt2 = SQRT(out[3]*out[3] + out[4]*out[4] + out[5]*out[5]);
     if ( dsdt2 < MINDS ) dsdt2 = 0.0;
     kap2 = out[15];

     GE107((DBAny *)cp,sp,(double)0.8,(short)3,out);
     dsdt3 = SQRT(out[3]*out[3] + out[4]*out[4] + out[5]*out[5]);
     if ( dsdt3 < MINDS ) dsdt3 = 0.0;
     kap3 = out[15];
/*
***Compensate for current output scale factor.
***Increasing scale means less curvature but higher ds/dt.
*/
     kap1 /= scale;  kap2 /= scale;  kap3 /= scale;
     dsdt1 *= scale; dsdt2 *= scale; dsdt3 *= scale;
/*
***Number of vectors (steps in R3) depends on curvature.
***To determine the number of steps in t we multiply by ds/dt.
***The rest of the formula below is the result of many years
***of trial and error.
*/
     nvec1 = (int)(0.3*SQRT(kap1)*dsdt1 + 0.5);
     nvec2 = (int)(0.3*SQRT(kap2)*dsdt2 + 0.5);
     nvec3 = (int)(0.3*SQRT(kap3)*dsdt3 + 0.5);
/*
***More than 15 vectors/seg with cn = 1.0 is no meaning.
*/
     if ( nvec1 > 5 ) nvec1 = 5;
     if ( nvec2 > 5 ) nvec2 = 5;
     if ( nvec3 > 5 ) nvec3 = 5;
/*
***Now it's time to adjust nvec for the global curve
***accuracy factor cn.
*/
     nvec1 = (int)(nvec1*cn);
     nvec2 = (int)(nvec2*cn);
     nvec3 = (int)(nvec3*cn);
/*
***Make sure nvec never becomes negative.
*/
     if ( nvec1 < 1 ) nvec1 = 0;
     if ( nvec2 < 1 ) nvec2 = 0;
     if ( nvec3 < 1 ) nvec3 = 0;
/*
***Check if any of the three nvec's < 0.
*/
     n_zero = 0;

     if ( nvec1 == 0 ) ++n_zero;
     if ( nvec2 == 0 ) ++n_zero;
     if ( nvec3 == 0 ) ++n_zero;
/*
***Calculate dt for all combinations of nvec so
***that nvec1*dt1 + nvec2*dt2 + nvec3*dt3 = 1.0
***and then remove the last vector. Always create
***at least 2 vectors / segment.
*/
     switch (n_zero)
       {
       case 3:
       nvec1 = 1;     /* All 3 are zero, set nvec1=1 and continue to case 2 */

       case 2:
       if      ( nvec1 > 0 ) {if ( nvec1 == 1 ) ++nvec1; dt1 = 1.0/nvec1; --nvec1;}
       else if ( nvec2 > 0 ) {if ( nvec2 == 1 ) ++nvec2; dt2 = 1.0/nvec2; --nvec2;}
       else                  {if ( nvec3 == 1 ) ++nvec3; dt3 = 1.0/nvec3; --nvec3;}
       break;

       case 1:
       if      ( nvec1 == 0 ) {dt2 = 0.5/nvec2; dt3 = 0.5/nvec3; --nvec3;}
       else if ( nvec2 == 0 ) {dt1 = 0.5/nvec1; dt3 = 0.5/nvec3; --nvec3;}
       else                   {dt1 = 0.5/nvec1; dt2 = 0.5/nvec2; --nvec2;}
       break;

       case 0:
       dt1 = 1.0/(3.0*nvec1);
       dt2 = 1.0/(3.0*nvec2);
       dt3 = 1.0/(3.0*nvec3);
     --nvec3;
       break;
       }
/*
***Calculate t-values. 
*/
     t     = 0.0;
     tv[0] = t;
     nt    = 1;

     for ( j=0; j<nvec1; ++j ) { t += dt1; tv[nt++] = t; }
     for ( j=0; j<nvec2; ++j ) { t += dt2; tv[nt++] = t; }
     for ( j=0; j<nvec3; ++j ) { t += dt3; tv[nt++] = t; }
/*
***If there is no room for more vectors, restart from first
***segment with lower value of cn or exit.
*/
     if ( k+nt+2 > PLYMXV )
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
***Calculate R3 coordinates.
*/
     for ( j=0; j<nt; ++j )
       {
       GE107((DBAny *)cp,sp,tv[j],(short)0,out);
       x[k] = out[0];
       y[k] = out[1];
       z[k] = out[2];
       ++k;
       }
   ++sp;
     }
/*
***The segment loop ends here. Now the last point on the last segment.
*/
     GE107((DBAny *)cp,sp-1,(double)1.0,(short)0,out);
     x[k] = out[0];
     y[k] = out[1];
     z[k] = out[2];
/*
***Everything seems to be ok.
*/
   status = 0;
/*
***No space for all vectors.
*/
end:
/*
***Status-information).
*/
   a[*n+1] = 0;
   for (i = *n+2; i <= k; ++i) a[i] = VISIBLE;
   for (i = k/2 +1; i <= k; ++i) a[i] = a[i] | ENDSIDE;
   *n = k;
/*
***End.
*/
   return(status);
 }

/*********************************************************/
/*!******************************************************/

 static int    mk_dash(
        double scale,
        int   *n,
        double x[],
        double y[],
        double z[],
        char   a[])

/*      Creates the graphical 3D polyline representation 
 *      for dashed curves, CFONT=1 or CFONT=2.
 *
 *      In:  scale   = Curve scale factor (ZOOM-status).
 *           n+1     =  Offset to polyline start.
 *
 *      Out: n       =  Offset to polyline end.
 *           x,y,z,a =  Polyline coordinates and status.
 *
 *      Return:   0 = Ok.
 *               -1 = Dash length > curve length or out of space
 *                    If this happens, try mk_solid() instead.
 *
 *      (C)2006-12-16 J. Kjellander
 *
 ******************************************************!*/

  {
    int     i,k,status;
    double  ltot,ds,mfakt,uinc,stopl;

/*
***Initializations.
*/
    k      = *n+1;             /* Polyline offset */
    sl     = cp->lgt_cu;       /* Dash length */
    ltot   = cp->al_cu;        /* Total curve length in R3 */
    prevu  = 0.0;              /* Previous t value */
    actl   = 0.0;              /* Current relative arc length */
    mfakt  = DASHRATIO;        /* Ratio of space/dash 0.25 */
/*
***If total curve length is not computed earlier we must do it now.
***One example is wireframe rep. of surfaces.
*/
    if ( ltot == 0.0 ) GEarclength((DBAny *)cp,sp,&ltot);
/*
***If dashlength is larger than curve length return error.
*/
    if ( ltot < (1.5+mfakt)*sl ) { status = -1; goto error; }
/*
***Calculate relative step length. 0 > ds > 1.
*/
    ds = sl/ltot;
/*
***Is there space enough ?
*/
    if ( k+2 > PLYMXV )
      {
      status = -1;
      goto error;
      }
/*
***Start with an invisible move to the start of the curve.
*/
    actu = 0.0;
    nextp(actu,k,x,y,z);
    a[k++] = INVISIBLE;
/*
***Calculate a new value for actu and then calculate the
***number of vectors for the dash with nextnv(). nextnv() uses
***the current scale and the curvature on the middle of the dash.
*/
    if ( (status=nextu(ds)) < 0 )
      {
      status = -1;
      goto error;
      }
    nextnv(scale,actu/2.0);
    uinc = actu/nvec;
/*
***Check for space.
*/
    if ( k+nvec+2 > PLYMXV )
      {
      --k;
      status = -1;
      goto error;
      }
/*
***Create vectors in R3.
*/
    for ( i=0; i<nvec; ++i )
      {
      prevu += uinc;
      nextp(prevu,k,x,y,z);
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
    if ( cp->fnt_cu == 1 ) stopl = 1.0 - (1.0+2*mfakt)*ds;
    else                   stopl = 1.0 - (1.0+4*mfakt)*ds;

    while ( actl < stopl )
      {
/*
***Om det är streckprickat lägger vi nu in ett mellanrum + ett
***kort streck.
*/
      if ( cp->fnt_cu != 1 )
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
        nextp(prevu,k,x,y,z);
        a[k++] = INVISIBLE;
/*
***Vi står nu i början av en eller flera tända förflyttningar.
***Beräkna erfoderligt antal vektorer för delstrecket.
***Om de inte får plats i polylinjen avslutar vi utan att rita
***klart.
*/
        nextnv(scale,(actu+prevu)/2.0);

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
          nextp(prevu,k,x,y,z);
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
      nextp(prevu,k,x,y,z);
      a[k++] = INVISIBLE;
/*
***Vi står nu i början av en eller flera tända förflyttningar.
***Beräkna erfoderligt antal vektorer för delstrecket.
***Om de inte får plats i polylinjen avslutar vi utan att rita
***klart.
*/
      nextnv(scale,(actu+prevu)/2.0);

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
        nextp(prevu,k,x,y,z);
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
    nextp(actu,k,x,y,z);
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
    nextnv(scale,(cp->nsgr_cu + actu)/2.0);
    if ( k+nvec+2 >= PLYMXV )
      {
      --k;
      status = -1;
      goto error;
      }
/*
***Sista delstrecket.
*/
    uinc = (cp->nsgr_cu - actu)/nvec;

    for ( i=0; i<nvec; ++i )
      {
      actu += uinc;
      nextp(actu,k,x,y,z);
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
/*
***Endside.
*/
    for ( i=k/2+1; i<=k; ++i ) a[i] = a[i] | ENDSIDE;
/*
***Felutgång.
*/
error:
    return(status);
 }

/*********************************************************/
/*!******************************************************/

 static int    nextu(
        double ds)

/*      Calculates the next u-value.
 *
 *      In: ds = Step size in relative arc length.
 *
 *      Out: Updates prevu, actu, actl.
 *
 *      FV:   0 = Ok.
 *           <0 = Probably ds/dt = 0.
 *
 *      (C)2006-12-16 J. Kjellander
 *
 *      2007-01-12 ns_geo, J.Kjellander
 *
 ******************************************************!*/

 {
   int   ns_geo;
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
***Beräkna ett nytt actu. Här anropar vi GE717() med den
***grafiska representationen som indata. GE717() tror att
***det är de geometriska segmenten vi skickar och tror
***därför att antal segment = cp->ns_cu. För NURBS-kurvor
***stämmer detta inte så vi sätter här tillfälligt antalet
***geometriska segment = antalet grafiska segment.
*/
   ns_geo = cp->ns_cu;
   cp->ns_cu = cp->nsgr_cu;
   status = GE717((DBAny *)cp,sp,NULL,actl,&actu);
   cp->ns_cu = ns_geo;

   if ( status < 0 ) return(status);

   actu -= 1.0;

   return(0);
 }

 /*******************************************************/
/*!******************************************************/

 static void   nextp(
        double ug,
        int    k,
        double x[],
        double y[],
        double z[])

/*      Calculates a position in R3.
 *
 *      In: ug     = Global u value.
 *          k      = Offset in polyline.
 *
 *      Out: x,y,z = Coordinates in R3.
 *
 *      (C)2006-12-16 J. Kjellander
 *
 ******************************************************!*/

 {
   int     intu;
   DBfloat out[17];

/*
***Välj rätt segment.
*/
    if ( (intu=(int)HEL(ug)) == cp->nsgr_cu ) intu -= 1;
/*
***Beräkna koordinater.
*/
    GE107((DBAny *)cp,sp+intu,ug-intu,(short)0,out);
    x[k] = out[0]; y[k] = out[1]; z[k] = out[2];
 }

 /*******************************************************/
/*!******************************************************/

 static void   nextnv(
        double scale,
        double ug)

/*      Calculates how many vectors to use for next step.
 *
 *      In: scale = Scale factor for output window.
 *          ug    = Global u value.
 *
 *      Out: Updates nvec.
 *
 *      (C)2006-12-16 J. Kjellander
 *
 ******************************************************!*/

 {
   int    intu;
   double kappa,out[17];

/*
***Select segment.
*/
    if ( (intu=(int)HEL(ug)) == cp->nsgr_cu ) intu -= 1;
/*
***Calculate curvature.
*/
    GE107((DBAny *)cp,sp+intu,ug-intu,(short)3,out);
    kappa = out[15]/scale;
/*
***Calculate the number of vectors needed.
*/
    nvec = (int)(0.6*SQRT(kappa)*sl*scale*cn + 0.5);
    if ( nvec < 1 ) nvec = 1;
 }

 /*******************************************************/

