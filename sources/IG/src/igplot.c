/********************************************************************
*   igplot.c
*   ========
*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGcgkm();  Create PLT-file GKS formatted                        */
/*  IGcdxf();  Create DXF-file                                      */
/*  IGshll();  Shell command                                        */
/*  IGhid1();  Hide on screen                                       */
/*  IGhid2();  Hide to PLT-file                                     */
/*  IGhid3();  Hide on screen and to file                           */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*                                                                  */
/*  This library is free software; you can redistribute it and/or   */
/*  modify it under the terms of the GNU Library General Public     */
/*  License as published by the Free Software Foundation; either    */
/*  version 2 of the License, or (at your option) any later         */
/*  version.                                                        */
/*                                                                  */
/*  This library is distributed in the hope that it will be         */
/*  useful, but WITHOUT ANY WARRANTY; without even the implied      */
/*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR         */
/*  PURPOSE.  See the GNU Library General Public License for more   */
/*  details.                                                        */
/*                                                                  */
/*  You should have received a copy of the GNU Library General      */
/*  Public License along with this library; if not, write to the    */
/*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge,        */
/*  MA 02139, USA.                                                  */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/IG.h"
#include "../../WP/include/WP.h"
#include "../../EX/include/EX.h"
#include "../../GE/include/GE.h"
#include <string.h>

extern char   jobdir[];
extern char   jobnam[];
extern bool   tmpref;
extern DBTmat lklsyi,*lsyspk;
extern MNUALT smbind[];

char speed[MAXTXT+1];
char pltyp[MAXTXT+1];
char port[MAXTXT+1];

static short igcplf(char *typ);
static short ighidf(bool screen);

/*!******************************************************/

       short IGcgkm()

/*      Skapa plotfil på GKS-format.
 *
 *      (C)microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 { return(igcplf(PLTEXT)); }

/********************************************************/
/*!******************************************************/

       short IGcdxf()

/*      Skapa plotfil på DXF-format.
 *
 *      (C)microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 { return(igcplf(DXFEXT)); }

/********************************************************/
/*!******************************************************/

static short igcplf(char *typ)

/*      Skapa plotfil.
 *
 *      In: typ = ".PLT eller ".DXF".
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      Felkoder: IG0103 = Okänt alt. i plot-menyn.
 *                IG3012 = Vyn %s finns ej.
 *                IG3062 = Kan ej skapa plotfil %s, fel från OS
 *
 *      (C)microform ab 9/8/85 J. Kjellander
 *
 *      19/11-85 GKS-format, Ulf Johansson
 *      18/10/86 Plot-meny mm, J. Kjellander
 *      4/11/86  gpstvi(), J. Kjellander
 *      5/11/86  Flagga för hela modellen, R. Svedin
 *      25/11/86 Återställning av flaggan, J. Kjellander
 *      7/5/87   Plotorigo, J. Kjellander
 *      4/8/87   Filen finns... J. Kjellander
 *      17/2/87  Plotvy, J. Kjellander
 *      21/5/89  tmpref, J. Kjellander
 *      27/3/91  DXF, J. Kjellander
 *      5/8/91   3D-origo, J. Kjellander
 *      2/10/91  Bug DXF-origo, J. Kjellander
 *      4/12/91  Bug GEtfpos_to_local(), J. Kjellander
 *      1/3/94   Snabbval, J. Kjellander
 *      2006-12-27 WPmkpf(), J.Kjellander
 *
 ******************************************************!*/

  {
     char     fnam[JNLGTH+1];
     char     path[V3PTHLEN+1];
     char     pektkn;
     double   x1,y1,x2,y2;
     double   tmp,dx,dy;
     short    status,alttyp,tsnr;
     bool     tmptrf;
     wpw_id   grw_id;
     FILE    *filpek;
     MNUALT  *pmualt;
     WPVIEW   plotvy;
     DBVector origo;
     WPWIN   *winptr;
     WPGWIN  *gwinpt;
     WPGRPT   projpos;

     static char dstr[JNLGTH+1] = "";

/*
***Fixa en pekare till huvudfönstret.
*/
    winptr = WPwgwp((wpw_id)GWIN_MAIN);
    gwinpt = (WPGWIN *)winptr->ptr;
/*
***Plot-menyn.
*/
#ifdef WIN32
    if ( strcmp(typ,PLTEXT) == 0 ) msshmu(146);
    else                           msshmu(147);
#else
    if ( strcmp(typ,PLTEXT) == 0 ) IGaamu(146);
    else                           IGaamu(147);   
#endif
/*
***Läs in svar, snabbval ej tillåtet.
*/
loop:
    IGptma(359,IG_MESS);
    IGgalt(&pmualt,&alttyp);

    if ( pmualt == NULL )
      {
      switch ( alttyp )
        {
        case SMBRETURN:
        IGrsma();
#ifdef WIN32
        mshdmu();
#else
        IGsamu();
#endif
        return(REJECT);

        case SMBMAIN:
        goto gomain;
        }
      }
/*
***Väl alternativ.
*/
    switch(pmualt->actnum)
      {
/*
***Plotta hela modellen.
*/
      case 1:
      WPmsiz(gwinpt,&plotvy);
      break;
/*
***Plotta allt på skärmen.
*/
      case 2:
      plotvy.modwin.xmin = gwinpt->vy.modwin.xmin;
      plotvy.modwin.ymin = gwinpt->vy.modwin.ymin;
      plotvy.modwin.xmax = gwinpt->vy.modwin.xmax;
      plotvy.modwin.ymax = gwinpt->vy.modwin.ymax;
      break;
/*
***Plotta del av skärmen.
*/
      case 3:
/*
***Läs in plot-fönster.
*/
l1:
      IGptma(322,IG_MESS);
      WPgmc2(FALSE,&pektkn,&x1,&y1,&grw_id);
      IGrsma();
      if ( pektkn == *smbind[1].str ) goto reject;
      if ( pektkn == *smbind[7].str ) goto gomain;
      if ( pektkn == *smbind[8].str )
        {
        IGhelp();
        goto l1;
        }
      if ( pektkn != ' ' )
        {
        WPbell();
        goto l1;
        }
l2:
      IGptma(323,IG_MESS);
      WPgmc2(FALSE,&pektkn,&x2,&y2,&grw_id);
      IGrsma();
      if ( pektkn == *smbind[1].str ) goto reject;
      if ( pektkn == *smbind[7].str ) goto gomain;
      if ( pektkn == *smbind[8].str )
        {
        IGhelp();
        goto l2;
        }
      if ( pektkn != ' ' )
        {
        WPbell();
        goto l2;
        }
/*
***Sortera.
*/
      if (x1 - x2 > 0.0) 
        {
        tmp=x1;
        x1=x2;
        x2=tmp;
        }

      if (y1 - y2 > 0.0) 
        {
        tmp=y1;
        y1=y2;
        y2=tmp;
        }
/*
***Felkontroll.
*/
      dx=x2 - x1;
      dy=y2 - y1;
      if ( dx < 1e-10 || dy < 1e-10 )
        {
        erpush("IG3042","");
        goto error;
        }
/*
***Sätt upp nytt fönster.
*/
      plotvy.modwin.xmin = x1;
      plotvy.modwin.ymin = y1;
      plotvy.modwin.xmax = x2;
      plotvy.modwin.ymax = y2;
      break;
/*
***Plotta annan vy. OBS !!!!!!!!! Detta alternativ bör utgå !!!!!!!!
*
      case 4:
      IGptma(220,IG_INP);
      status=IGssip(IGgtts(267),vynam,"",GPVNLN);
      IGrsma();
      if ( status == REJECT ) goto reject;
      if ( status == GOMAIN ) goto gomain;

      if ( (vynum=vyindx(vynam)) == -1 )
        {
        erpush("IG3012",vynam);
        goto error;
        }

      V3MOME(&vytab[vynum],&plotvy,sizeof(VY));
      vypek = &vytab[vynum];
      break;
*
***Okänt alternativ.
*/
      default:
      erpush("IG0103","");
      goto error;
      }
/*
***Sudda "Vad vill du plotta ?"
*/
     IGrsma();
/*
***Fråga efter plotorigo.
*/
     if ( IGialt(380,67,68,FALSE) )
       {
       origo.x_gm = origo.y_gm = origo.z_gm = 0.0;
       }
     else
       {
       IGptma(381,IG_MESS); tmptrf = tmpref; tmpref = TRUE;
       status=IGcpov(&origo);
       tmpref = tmptrf; IGrsma(); WPerhg();
       if ( status == GOMAIN ) goto gomain;
       if ( status == REJECT ) goto reject;
/*
***Transformera till BASIC och sedan till aktiv vy.
*/
       if ( lsyspk != NULL ) GEtfpos_to_local(&origo,&lklsyi,&origo);

       WPppos(gwinpt,&origo,&projpos);
       origo.x_gm = projpos.x;
       origo.y_gm = projpos.y;
       origo.z_gm = 0.0;
/*
***Om det är en GKS-fil som skall skapas skall origo beräknas
***annorlunda än om det är en DXF-fil.
*/
       if ( strcmp(typ,PLTEXT) == 0 )
         {
         origo.x_gm -= plotvy.modwin.xmin;
         origo.y_gm -= plotvy.modwin.ymin;
         }
       }
/*
***Läs in filnamn.
*/
getnam:
     if ( strcmp(dstr,"") == 0 )    strcpy(dstr,jobnam);
     if ( strcmp(typ,PLTEXT) == 0 ) IGptma(215,IG_INP);
     else                           IGptma(173,IG_INP);
     status=IGssip(IGgtts(267),fnam,dstr,JNLGTH);
     IGrsma();
     if ( status == GOMAIN ) goto gomain;
     if ( status == REJECT ) goto reject;
     strcpy(dstr,fnam);
/*
***Skapa filnamn.
*/
     strcpy(path,jobdir);
     strcat(path,fnam);
     strcat(path,typ);
/*
***Kolla om filen finns.
*/
     if ( (filpek=fopen(path,"r")) != NULL )
       {
       fclose(filpek);
       if ( strcmp(typ,PLTEXT) == 0 ) tsnr = 382; else tsnr = 383;
       if ( IGialt(tsnr,67,68,TRUE) ) goto opfil;
       else goto getnam;
       }
/*
***Öppna filen.
*/
opfil:
    if ( (filpek=fopen(path,"w+")) == NULL )
      {
      erpush("IG3062",path);
      goto error;
      }
/*
***Generera plotfil.
*/
     if ( strcmp(typ,PLTEXT) == 0 )
       {
       IGptma(221,IG_MESS);
       status = WPmkpf(gwinpt,filpek,&plotvy.modwin,&origo);
       }
     else
       {
       IGptma(172,IG_MESS);
       status = WPdxf_out(gwinpt,filpek,&plotvy,&origo);
       }

     fclose(filpek);
/*
***Avbrott.
*/
     if ( status == AVBRYT ) 
       {
       IGrsma();
#ifdef WIN32
        mshdmu();
#else
        IGsamu();
#endif
       WPaddmess_mcwin(IGgtts(171),WP_ERROR);
       return(REJECT);
       }
/*
***Sudda "Plotfil skapas" och återvänd till Plot-menyn.
*/
     IGrsma();
     goto loop;
/*
***Fel.
*/
error:
     errmes();
/*
***REJECT.
*/
reject:
     IGrsma();
     goto loop;
/*
***GOMAIN.
*/
gomain:
     return(GOMAIN);
  }

/********************************************************/
/*!******************************************************/

       short IGshll()

/*      Kommando till shell.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 20/8/85 J. Kjellander
 *
 *      22/11/85 v2cmos, J. Kjellander
 *      4/8/87   Fixa skärmen, J. Kjellander
 *      5/3/88   IGcmos(), J. Kjellander
 *
 ******************************************************!*/

  {
     char    shcmd[81];
     short   status;

     static char dstr[81] = "";

/*
***Läs in kommando.
*/
     if ( (status=IGssip(IGgtts(302),shcmd,dstr,80)) < 0 ) return(status);
     strcpy(dstr,shcmd);
/*
***Utför.
*/
     return(IGcmos(shcmd));

  }
/*********************************************************************/
/*!******************************************************/

        short IGhid1()

/*      Huvudrutin för hide mot skärm.
 *
 *      (C)microform ab 29/2/89 J. Kjellander
 *
 *      22/2/94 Bug origo.z=0, J. Kjellander
 *
 ******************************************************!*/

 {
   short status;
   DBVector origo;

   origo.x_gm = origo.y_gm = origo.z_gm = 0.0;
/*
***Det här kan ta tid.
*/
#ifdef UNIX
   WPwait(GWIN_MAIN,TRUE);
#endif

   status = EXhdvi("xy",TRUE,FALSE,NULL,&origo);

#ifdef UNIX
   WPwait(GWIN_MAIN,FALSE);
#endif

   if ( status == AVBRYT )
     {
     WPaddmess_mcwin(IGgtts(170),WP_ERROR);
     return(0);
     }
   else return(status);
 }

/********************************************************/
/*!******************************************************/

        short IGhid2()

/*      Huvudrutin för hide mot fil.
 *
 *      (C)microform ab 29/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
   return(ighidf(FALSE));
 }

/********************************************************/
/*!******************************************************/

        short IGhid3()

/*      Huvudrutin för hide både mot skärm och fil.
 *
 *      (C)microform ab 29/2/89 J. Kjellander
 *
 ******************************************************!*/

 {
   return(ighidf(TRUE));
 }

/********************************************************/
/*!******************************************************/

static short ighidf(bool screen)

/*      Hide mot fil.
 *
 *      In: screen = Skärm TRUE/FALSE.
 *
 *      Felkoder: IG3062 = Kan ej skapa plotfil %s, fel från OS
 *      
 *      (C)microform ab 29/1/89 J. Kjellander
 *
 *      5/8/91   3D-origo, J. Kjellander
 *
 ******************************************************!*/

 {
   short  status;
   char   fnam[JNLGTH+1];
   char   path[V3PTHLEN+1];
   bool   tmptrf;
   DBVector  origo,*nollp;
   FILE  *filpek;

   static char dstr[JNLGTH+1] = "";

/*
***Fråga efter plotorigo.
*/
     if ( IGialt(380,67,68,FALSE) )
       nollp = NULL;
     else
       {
       IGptma(381,IG_MESS); tmptrf = tmpref; tmpref = TRUE;
       status=IGcpov(&origo); nollp = &origo;
       tmpref = tmptrf; IGrsma(); WPerhg();
       if ( status < 0 ) return(status); 
       }
/*
***Läs in filnamn.
*/
getnam:
   if ( strcmp(dstr,"") == 0 ) strcpy(dstr,jobnam);
   IGptma(215,IG_INP);
   status = IGssip(IGgtts(267),fnam,dstr,JNLGTH);
   IGrsma();
   if ( status < 0 ) return(status);
   strcpy(dstr,fnam);
/*
***Skapa komplett filnamn med path.
*/
   strcpy(path,jobdir);
   strcat(path,fnam);
   strcat(path,PLTEXT);
/*
***Kolla om filen finns.
*/
   if ( (filpek=fopen(path,"r")) != NULL )
     {
     fclose(filpek);
     if ( IGialt(382,67,68,TRUE) ) goto opfil;
     else                          goto getnam;
     }
/*
***Öppna filen för skrivning.
*/
opfil:
   if ( (filpek=fopen(path,"w+")) == NULL )
     return(erpush("IG3062",path));
/*
***Gör hide. Hide kan ta tid så här slår vi på vänta-
***hanteringen.
*/
#ifdef UNIX
   WPwait(GWIN_MAIN,TRUE);
#endif

   status = EXhdvi("xy",screen,TRUE,filpek,nollp);
   if ( status == AVBRYT )
     {
     WPaddmess_mcwin(IGgtts(170),WP_ERROR);
     status = 0;
     }

#ifdef UNIX
   WPwait(GWIN_MAIN,FALSE);
#endif
/*
***Stäng filen.
*/
   fclose(filpek);

   return(status);
 }

/********************************************************/
