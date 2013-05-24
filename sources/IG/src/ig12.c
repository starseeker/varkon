/*!******************************************************************/
/*  File: ig12.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  igcgkm();  Create PLT-file GKS formatted                        */
/*  igcdxf();  Create DXF-file                                      */
/*  igshll();  Shell command                                        */
/*  ighid1();  Hide on screen                                       */
/*  ighid2();  Hide to PLT-file                                     */
/*  ighid3();  Hide on screen and to file                           */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.varkon.com                                     */
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
/*  (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/IG.h"
#include "../../WP/include/WP.h"
#include "../../GP/include/GP.h"
#include "../../EX/include/EX.h"
#include "../../GE/include/GE.h"
#include <string.h>

extern char   jobdir[];
extern char   jobnam[];
extern bool   tmpref;
extern DBTmat lklsyi,*lsyspk;
extern MNUALT smbind[];
extern VY     actvy,vytab[];
extern short  rmarg,bmarg,gptrty;

char speed[ MAXTXT+1 ];
char pltyp[ MAXTXT+1 ];
char port[ MAXTXT+1 ];

static short igcplf(char *typ);
static short ighidf(bool screen);

/*!******************************************************/

       short igplot()

/*      Starta plottning.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 25/2/85 J. Kjellander
 *
 *      20/11-85 Plotparametrar, Ulf Johansson
 *      24/11/85 Finjustering, J. Kjellander
 *      1/4/86   Mod. för MF:s plott-program R. Svedin
 *      7/11/86  GOMAIN, VAX, J. Kjellander
 *      29/9/87  Default filnamn, J. Kjellander
 *      15/4/92  EXos(), J. Kjellander
 *
 ******************************************************!*/

  {

     char  *ps[ 4 ];
     char  *is[ 4 ];
     char  *ds[ 4 ];
     short ml[ 4 ];
     char  str[ 4*(MAXTXT+1) ];
     short state,status;

     static char filen[ MAXTXT+1 ] = "";
     static char scale[ MAXTXT+1 ] = "1.0";
     static char vridn[ MAXTXT+1 ] = "0.0";
     static char xmin[ MAXTXT+1 ] = "0.0";
     static char ymin[ MAXTXT+1 ] = "0.0";

/*
***Initiering.
*/
     is[ 0 ] = str;                         /* Input strings */
     is[ 1 ] = &str[ MAXTXT+1 ];
     is[ 2 ] = &str[ 2*(MAXTXT+1) ];
     is[ 3 ] = &str[ 3*(MAXTXT+1) ];
/*
***Läs in plotparametrar.
*/
     state = 0;
     do 
        {
        switch (state) 
           {
/*
***Fil och  hastighet.
*/
           case 0:
/*
***Promtsträngar.
*/
           ps[ 0 ] = iggtts(342);
           ps[ 1 ] = iggtts(335);
/*
***Defaultvärden.
*/
           if ( strcmp(filen,"") == 0 ) ds[0] = jobnam;
           else ds[0] = filen;
           ds[1] = speed;
/*
***Infältens maxlängd.
*/
           ml[ 0 ] = ml[ 1 ] = MAXTXT;
/*
***Läs in fil och hastighet.
*/
           igptma(347,IG_INP);
           status = igmsip(ps,is,ds,ml,2);
           if ( status == GOMAIN ) goto gomain;
           if ( status == REJECT ) state--;
           else
              {
              strcpy(filen,is[ 0 ]);
              strcpy(speed,is[ 1 ]);
              state++;
              } 
           igrsma();
           break;
/*
***Plotter och port.
*/
           case 1:
/*
***Promtsträngar.
*/
           ps[ 0 ] = iggtts(340);
           ps[ 1 ] = iggtts(344);
/*
***Defaultvärden.
*/
           ds[ 0 ] = pltyp;
           ds[ 1 ] = port;
/*
***Infältens maxlängd.
*/
           ml[ 0 ] = ml[ 1 ] = MAXTXT;
/*
***Läs in Plottertyp och port.
*/
           igptma(347,IG_INP);
           status = igmsip(ps,is,ds,ml,2);
           if ( status == GOMAIN ) goto gomain;
           if ( status == REJECT ) state--;
           else
              {
              strcpy(pltyp,is[ 0 ]);
              strcpy(port,is[ 1 ]);
              state++;
              } 
           igrsma();
           break;
/*
***Skala, vridning, Xmin och Ymin.
*/
           case 2:
/*
***Promtsträngar.
*/
           ps[ 0 ] = iggtts(338);
           ps[ 1 ] = iggtts(336);
           ps[ 2 ] = iggtts(339);
           ps[ 3 ] = iggtts(341);
/*
***Defaultvärden.
*/
           ds[ 0 ] = scale;
           ds[ 1 ] = vridn;
           ds[ 2 ] = xmin;
           ds[ 3 ] = ymin;
/*
***Infältens maxlängd.
*/
           ml[ 0 ] = ml[ 1 ] = ml[ 2 ] = ml[ 3 ] = MAXTXT;
/*
***Läs in skala, vridning, Xmin och Ymin.
*/
           igptma(337,IG_INP);
           status = igmsip(ps,is,ds,ml,4);
           if ( status == GOMAIN ) goto gomain;
           if ( status == REJECT ) state--;
           else
              {
              strcpy(scale,is[ 0 ]);
              strcpy(vridn,is[ 1 ]);
              strcpy(xmin,is[ 2 ]);
              strcpy(ymin,is[ 3 ]);
              state++;
              } 
           igrsma();
           break;
/*
***Starta upp plottning.
*/
           case 3:
           state = -1;
/*
***Bygg kommandosträng.
*/
           strcpy(str,pltyp);                   /* plotprogram */
           strcat(str," ");
           strcat(str,jobdir);                  /* plotfil */
           strcat(str,filen);
           strcat(str,PLTEXT);

           if (strlen(str) > 256) break;
 
           if (strcmp(scale,"1.0") != 0)        /* skala */
              {
              strcat(str," -s");
              strcat(str,scale);
              }

           if (strcmp(speed,iggtts(334)) != 0)  /* hastighet */
              {
              strcat(str," -h");
              strcat(str,speed);
              }

           if (strcmp(vridn,"0.0") != 0)        /* vridning */
              {
              strcat(str," -v");
              strcat(str,vridn);
              }

           if (strlen(str) > 256) break;

           if (strcmp(xmin,"0.0") != 0)        /* Xmin */
              {
              strcat(str," -x");
              strcat(str,xmin);
              }

           if (strcmp(ymin,"0.0") != 0)        /* Ymin */
              {
              strcat(str," -y");
              strcat(str,ymin);
              }

           if (strlen(str) > 256) break;

           strcat(str,port);                   /* plotport */

           if (strlen(str) > 256) break;
/*
***Skicka kommando till OS. Använd EXos() med mode = 2 för
***asynkron exekvering utan wait.
*/
           igplma(str,IG_MESS);
           EXos(str,(short)2);
           igrsma();
           break;

           default:
               state = -1;
               break;
           }

        }
        while (state >= 0);
/*
***Slut.
*/
        return(0);
/*
***GOMAIN.
*/
gomain:
        igrsma();
        return(GOMAIN);
  }

/********************************************************/
/*!******************************************************/

       short igcgkm()

/*      Skapa plotfil på GKS-format.
 *
 *      (C)microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 { return(igcplf(PLTEXT)); }

/********************************************************/
/*!******************************************************/

       short igcdxf()

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
 *
 ******************************************************!*/

  {
     char    vynam[GPVNLN+1];
     char    fnam[JNLGTH+1];
     char    path[V3PTHLEN+1];
     char    pektkn;
     double  x1,y1,x2,y2;
     double  tmp,dx,dy;
     short   status,alttyp,vynum,tsnr;
     bool    tmptrf;
     FILE   *filpek;
     MNUALT *pmualt;
     VY      tmpavy,plotvy;
     VY     *vypek;
     DBVector   origo;

     static char dstr[JNLGTH+1] = "";

/*
***Lagra actvy i temporär variabel.
*/
    gpgwin(&tmpavy);
/*
***Plot-menyn.
*/
#ifdef WIN32
    if ( strcmp(typ,PLTEXT) == 0 ) msshmu(146);
    else                           msshmu(147);
#else
    if ( strcmp(typ,PLTEXT) == 0 ) igaamu(146);
    else                           igaamu(147);   
#endif
/*
***Läs in svar, snabbval ej tillåtet.
*/
loop:
    igptma(359,IG_MESS);
    iggalt(&pmualt,&alttyp);

    if ( pmualt == NULL )
      {
      switch ( alttyp )
        {
        case SMBRETURN:
        igrsma();
#ifdef WIN32
        mshdmu();
#else
        igsamu();
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
      gpgwin(&plotvy);
      if ( iggtmx(&plotvy) ==AVBRYT )
        {
        igrsma();
        igerpl();
        igwtma(171);
#ifdef WIN32
        mshdmu();
#else
        igsamu();
#endif
        return(REJECT);
        }
      vypek = NULL;
      break;
/*
***Plotta allt på skärmen.
*/
      case 2:
      gpgwin(&plotvy);
      vypek = NULL;
      break;
/*
***Plotta del av skärmen.
*/
      case 3:
/*
***Läs in plot-fönster.
*/
l1:
      igptma(322,IG_MESS);
      gpgtmc(&pektkn,&x1,&y1,FALSE);
      igrsma();
      if ( pektkn == *smbind[1].str ) goto reject;
      if ( pektkn == *smbind[7].str ) goto gomain;
      if ( pektkn == *smbind[8].str )
        {
        ighelp();
        goto l1;
        }
      if ( pektkn != ' ' )
        {
        igbell();
        goto l1;
        }
l2:
      igptma(323,IG_MESS);
      gpgtmc(&pektkn,&x2,&y2,FALSE);
      igrsma();
      if ( pektkn == *smbind[1].str ) goto reject;
      if ( pektkn == *smbind[7].str ) goto gomain;
      if ( pektkn == *smbind[8].str )
        {
        ighelp();
        goto l2;
        }
      if ( pektkn != ' ' )
        {
        igbell();
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
      gpgwin(&plotvy);
      plotvy.vywin[0] = x1;
      plotvy.vywin[1] = y1;
      plotvy.vywin[2] = x2;
      plotvy.vywin[3] = y2;
      vypek = NULL;
      break;
/*
***Plotta annan vy.
*/
      case 4:
      igptma(220,IG_INP);
      status=igssip(iggtts(267),vynam,"",GPVNLN);
      igrsma();
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
/*
***Okänt alternativ.
*/
      default:
      erpush("IG0103","");
      goto error;
      }
/*
***Sudda "Vad vill du plotta ?"
*/
     igrsma();
/*
***Fråga efter plotorigo.
*/
     if ( igialt(380,67,68,FALSE) )
       {
       origo.x_gm = origo.y_gm = origo.z_gm = 0.0;
       }
     else
       {
       igptma(381,IG_MESS); tmptrf = tmpref; tmpref = TRUE;
       status=genpov(&origo);
       tmpref = tmptrf; igrsma(); gphgal(0);
       if ( status == GOMAIN ) goto gomain;
       if ( status == REJECT ) goto reject;
/*
***Transformera till BASIC och sedan till aktiv vy.
*/
       if ( lsyspk != NULL ) GEtfpos_to_local(&origo,&lklsyi,&origo);
       gptrpv(&origo);
/*
***Om det är en GKS-fil som skall skapas skall origo beräknas
***annorlunda än om det är en DXF-fil.
*/
       if ( strcmp(typ,PLTEXT) == 0 )
         {
         origo.x_gm -= plotvy.vywin[0];
         origo.y_gm -= plotvy.vywin[1];
         }
       }
/*
***Läs in filnamn.
*/
getnam:
     if ( strcmp(dstr,"") == 0 )    strcpy(dstr,jobnam);
     if ( strcmp(typ,PLTEXT) == 0 ) igptma(215,IG_INP);
     else                           igptma(173,IG_INP);
     status=igssip(iggtts(267),fnam,dstr,JNLGTH);
     igrsma();
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
       if ( igialt(tsnr,67,68,TRUE) ) goto opfil;
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
     gpswin(&plotvy);
     if ( vypek != NULL ) gpstvi(vypek);

#ifdef V3_X11
   if ( gptrty == X11 ) wpwait(GWIN_MAIN,TRUE);
#endif

     if ( strcmp(typ,PLTEXT) == 0 )
       {
       igptma(221,IG_MESS);
       status = gpmeta(filpek,&plotvy,&origo);
       }
     else
       {
       igptma(172,IG_MESS);
       status = dxfout(filpek,&plotvy,&origo);
       }

#ifdef V3_X11
   if ( gptrty == X11 ) wpwait(GWIN_MAIN,FALSE);
#endif

     gpswin(&tmpavy);
     gpstvi(&tmpavy);
     fclose(filpek);
/*
***Avbrott.
*/
     if ( status == AVBRYT ) 
       {
       igrsma();
       igerpl();
#ifdef WIN32
        mshdmu();
#else
        igsamu();
#endif
       igwtma(171);
       return(REJECT);
       }
/*
***Sudda "Plotfil skapas" och återvänd till Plot-menyn.
*/
     igrsma();
     igerpl();
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
     igrsma();
     igerpl();
     goto loop;
/*
***GOMAIN.
*/
gomain:
     return(GOMAIN);
  }

/********************************************************/
/*!******************************************************/

       short igshll()

/*      Kommando till shell.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 20/8/85 J. Kjellander
 *
 *      22/11/85 v2cmos, J. Kjellander
 *      4/8/87   Fixa skärmen, J. Kjellander
 *      5/3/88   igcmos(), J. Kjellander
 *
 ******************************************************!*/

  {
     char    shcmd[81];
     short   status;

     static char dstr[81] = "";

/*
***Läs in kommando.
*/
     if ( (status=igssip(iggtts(302),shcmd,dstr,80)) < 0 ) return(status);
     strcpy(dstr,shcmd);
/*
***Utför.
*/
     return(igcmos(shcmd));

  }
/*********************************************************************/
/*!******************************************************/

        short ighid1()

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
#ifdef V3_X11
   if ( gptrty == X11 ) wpwait(GWIN_MAIN,TRUE);
#endif

   status = EXhdvi(actvy.vynamn,TRUE,FALSE,NULL,&origo);

#ifdef V3_X11
   if ( gptrty == X11 ) wpwait(GWIN_MAIN,FALSE);
#endif

   if ( status == AVBRYT )
     {
     igwtma(170);
     return(0);
     }
   else return(status);
 }

/********************************************************/
/*!******************************************************/

        short ighid2()

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

        short ighid3()

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
     if ( igialt(380,67,68,FALSE) )
       nollp = NULL;
     else
       {
       igptma(381,IG_MESS); tmptrf = tmpref; tmpref = TRUE;
       status=genpov(&origo); nollp = &origo;
       tmpref = tmptrf; igrsma(); gphgal(0);
       if ( status < 0 ) return(status); 
       }
/*
***Läs in filnamn.
*/
getnam:
   if ( strcmp(dstr,"") == 0 ) strcpy(dstr,jobnam);
   igptma(215,IG_INP);
   status = igssip(iggtts(267),fnam,dstr,JNLGTH);
   igrsma();
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
     if ( igialt(382,67,68,TRUE) ) goto opfil;
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
#ifdef V3_X11
   if ( gptrty == X11 ) wpwait(GWIN_MAIN,TRUE);
#endif

   status = EXhdvi(actvy.vynamn,screen,TRUE,filpek,nollp);
   if ( status == AVBRYT )
     {
     igwtma(170);
     status = 0;
     }

#ifdef V3_X11
   if ( gptrty == X11 ) wpwait(GWIN_MAIN,FALSE);
#endif
/*
***Stäng filen.
*/
   fclose(filpek);

   return(status);
 }

/********************************************************/
