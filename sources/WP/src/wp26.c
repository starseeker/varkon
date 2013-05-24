/**********************************************************************
*
*    wp26.c
*    ======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpdrxh();    Draw xhatch
*    wpdlxh();    Delete xhatch
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
#include "../include/WP.h"

extern short    actpen;

static short drawxh(WPGWIN *gwinpt, GMXHT *xhtpek, gmflt crdvek[],
                    DBptr la, bool draw);

/*!******************************************************/

        short wpdrxh(
        GMXHT  *xhtpek,
        gmflt   crdvek[],
        DBptr   la,
        v2int   win_id)

/*      Ritar en xht.
 *
 *      In: xhtpek => Pekare till xht-post.
 *          crdvek => Pekare till koordinater.
 *          la     => GM-adress.
 *          win_id => Fönster att rita i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 26/1/95 J. Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Om kurvan är blankad är det enkelt.
*/
   if ( xhtpek->hed_xh.blank) return(0);
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
***Ja, ligger snittet på en nivå som är tänd i detta fönster ?
*/
         if ( wpnivt(gwinpt,xhtpek->hed_xh.level) )
           {
/*
***Ja. Kolla att rätt färg är inställd.
*/
           if ( xhtpek->hed_xh.pen != actpen ) wpspen(xhtpek->hed_xh.pen);
/*
***Sen är det bara att rita.
*/
           drawxh(gwinpt,xhtpek,crdvek,la,TRUE);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short wpdlxh(
        GMXHT  *xhtpek,
        gmflt   crdvek[],
        DBptr   la,
        v2int   win_id)

/*      Suddar ett snitt.
 *
 *      In: xhtpek => Pekare till xht-post.
 *          crdvek => Pekare till koordinater.
 *          la     => GM-adress.
 *          win_id => Fönster att sudda i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C) microform ab 26/1/95 J. Kjellander
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
***Ja. Om den finns i DF och det är heldraget kan vi sudda snabbt.
*/
         if ( wpfobj(gwinpt,la,XHTTYP,&typ) )
           {
           if ( xhtpek->fnt_xh == 0 ) wpdobj(gwinpt,FALSE);
           else
             {
             wprobj(gwinpt);
             if ( !wpnivt(gwinpt,xhtpek->hed_xh.level)  ||
                                 xhtpek->hed_xh.blank) return(0);
             drawxh(gwinpt,xhtpek,crdvek,la,FALSE);
             }
           }
/*
***Om den nu ligger på en släckt nivå eller är blankad gör vi
***inget mer. Annars får vi återskapa polylinjen och sudda långsamt.
*/
         else
           {
           if ( !wpnivt(gwinpt,xhtpek->hed_xh.level)  ||
                               xhtpek->hed_xh.blank) return(0);
           drawxh(gwinpt,xhtpek,crdvek,la,FALSE);
           }
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        static short drawxh(
        WPGWIN *gwinpt,
        GMXHT  *xhtpek,
        gmflt   crdvek[],
        DBptr   la,
        bool    draw)

/*      Ritar/suddar ett snitt ett visst fönster.
 *      Vid ritning lagras objektet samtidigt i DF.
 *
 *      In: gwinpt => Pekare till fönster.
 *          xhtpek => Pekare till xht-post.
 *          crdvek => Pekare till koordinater.
 *          la     => GM-adress.
 *          draw   => TRUE = Rita, FALSE = Sudda
 *
 *      Ut:  Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 26/1/95 J. Kjellander
 *
 ******************************************************!*/

 {
   int    i,k,n;
   double x[PLYMXV],y[PLYMXV],tmpx[PLYMXV],tmpy[PLYMXV];
   char   a[PLYMXV],tmpa[PLYMXV];
   GMLIN  lin;


/*
***Vi börjar med att skapa en polylinje av hela snittet
***med heldragna snittlinjer. Detta får duga som DF-representation.
***Om snittet är streckat får vi generera ny grafisk representation.
*/
   k = -1;
   i =  0;
   n =  4*xhtpek->nlin_xh;

   while ( i < n )
     {
     x[++k] = crdvek[i++];
     y[k]   = crdvek[i++];
     a[k]   = 0;
     x[++k] = crdvek[i++];
     y[k]   = crdvek[i++];
     a[k]   = (VISIBLE | ENDSIDE);
     }
/*
***Klipp polylinjen.
*/
   if ( wpcply(gwinpt,-1,&k,x,y,a) )
     {
     if ( draw  &&  xhtpek->hed_xh.hit )
       {
       if ( wpsply(gwinpt,k,x,y,a,la,XHTTYP) )
         {
/*
***Ett heldraget snitt kan ritas direkt.
*/
         if ( xhtpek->fnt_xh == 0 ) wpdobj(gwinpt,TRUE);
/*
***Är snittet streckat tar vi vektorerna i polylinjen en och en
***ock gör dom streckade samt ritar dem.
*/
         else
           {
           i = 0;
           n = k;
           lin.fnt_l = xhtpek->fnt_xh;
           lin.lgt_l = xhtpek->lgt_xh;
           wpfixg(gwinpt);

           while ( i<n )
             {
             tmpx[0] = x[i];   tmpy[0] = y[i];   tmpa[0] = a[i];
             tmpx[1] = x[i+1]; tmpy[1] = y[i+1]; tmpa[1] = a[i+1];

             if ( tmpa[0] == 0  &&  (tmpa[1]&VISIBLE) == VISIBLE )
               {
               k = -1;
               gpplli(&lin,&k,tmpx,tmpy,tmpa); 
               wpdply(gwinpt,k,tmpx,tmpy,tmpa,TRUE);
               }
             i++;
             }
           wpfixg(NULL);
           }
         }
       else return(erpush("GP0012",""));
       }
     else if ( xhtpek->fnt_xh == 0 ) wpdply(gwinpt,k,x,y,a,draw);
     else
       {
       i =  0;
       n =  k;
       lin.fnt_l = xhtpek->fnt_xh;
       lin.lgt_l = xhtpek->lgt_xh;
       wpfixg(gwinpt);

       while ( i<n )
         {
         tmpx[0] = x[i];   tmpy[0] = y[i];   tmpa[0] = a[i];
         tmpx[1] = x[i+1]; tmpy[1] = y[i+1]; tmpa[1] = a[i+1];

         if ( tmpa[0] == 0  &&  (tmpa[1]&VISIBLE) == VISIBLE )
           {
           k = -1;
           gpplli(&lin,&k,tmpx,tmpy,tmpa); 
           wpdply(gwinpt,k,tmpx,tmpy,tmpa,draw);
           }
         i++;
         }
       wpfixg(NULL);
       }
     }

   return(0);
 }

/***********************************************************/
