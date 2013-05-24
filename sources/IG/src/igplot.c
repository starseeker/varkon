/********************************************************************/
/*  igplot.c                                                        */
/*  ========                                                        */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGexport_GKSM();  Export plotfile on GKS Metafile format        */
/*  IGexport_DXF2D(); Export plotfile on DXF format                 */
/*  IGshell(); Shell command                                        */
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

       short IGexport_GKSM()

/*      Create plotfile, GKS-format.
 *
 *      (C)microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 { return(igcplf(PLTEXT)); }

/********************************************************/
/*!******************************************************/

       short IGexport_DXF2D()

/*      Create plotfile, DXF-format.
 *
 *      (C)microform ab 27/3/91 J. Kjellander
 *
 ******************************************************!*/

 { return(igcplf(DXFEXT)); }

/********************************************************/
/*!******************************************************/

static short igcplf(char *type)

/*      Create 2D plotfile GKS or DXF formatted.
 *
 *      In: type = ".PLT or ".DXF".
 *
 *      Error: IG3042 = Plot window too small.
 *             IG3062 = Can't create file %s, OS error
 *             IG5373 = Cant create plotdata %s
 *
 *      (C)2007-11-28 J.Kjellander
 *
 ******************************************************!*/

  {
   short    status,tsnr;
   char     plotfile[V3PTHLEN],newname[JNLGTH],newpath[V3PTHLEN],
            wc_str[6],mesbuf[2*V3PTHLEN];
   int      i,ix1,iy1,ix2,iy2,dx,dy,tmp;
   double   x1,y1,x2,y2;
   wpw_id   grw_id;
   DBVector origin;
   WPVIEW   plotvy;
   WPWIN   *winptr;
   WPGWIN  *gwinpt;
   FILE    *fp;

/*
***Select plot area (and window).
*/
   WPaddmess_mcwin(IGgtts(359),WP_PROMPT);

   if ( (status=WPgtsw(&grw_id,&ix1,&iy1,&ix2,&iy2,WP_RUB_RECT,TRUE)) < 0 )
     return(status);
/*
***Sort coordinates.
*/
   if ( ix2 < ix1 ) { tmp = ix1; ix1 = ix2; ix2 = tmp; }
   if ( iy2 < iy1 ) { tmp = iy1; iy1 = iy2; iy2 = tmp; }
/*
***Get a C ptr to the window. Currently only WPGWIN
***supported.
*/
   if ( (winptr=WPwgwp((wpw_id)grw_id)) != NULL  &&
         winptr->typ != TYP_GWIN ) return(0);

   gwinpt = (WPGWIN *)winptr->ptr;
/*
***Transform to 2D model coordinates.
*/
   x1 = gwinpt->vy.modwin.xmin + (ix1 - gwinpt->vy.scrwin.xmin)*
                     (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin)/
                     (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
   y1 = gwinpt->vy.modwin.ymin + (iy1 - gwinpt->vy.scrwin.ymin)*
                     (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin)/
                     (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);

   x2 = gwinpt->vy.modwin.xmin + (ix2 - gwinpt->vy.scrwin.xmin)*
                     (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin)/
                     (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
   y2 = gwinpt->vy.modwin.ymin + (iy2 - gwinpt->vy.scrwin.ymin)*
                     (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin)/
                     (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);
/*
***Error check.
*/
   dx = x2 - x1;
   dy = y2 - y1;
   if ( dx < 1e-10 || dy < 1e-10 )
     {
     erpush("IG3042","");
     errmes();
     return(0);
     }
/*
***Set up the plot window.
*/
   plotvy.modwin.xmin = x1;
   plotvy.modwin.ymin = y1;
   plotvy.modwin.xmax = x2;
   plotvy.modwin.ymax = y2;
/*
***Set plot origin. DXF and PLT have different conventions.
*/
   origin.x_gm = origin.y_gm = origin.z_gm = 0.0;

   if ( strcmp(type,PLTEXT) == 0 )
     {
     origin.x_gm -= plotvy.modwin.xmin;
     origin.y_gm -= plotvy.modwin.ymin;
     }
/*
***Get the name and path to use for the new plotfile.
*/
   strcpy(wc_str,"*");
   strcat(wc_str,type);
   strcpy(newpath,jobdir);
   strcpy(plotfile,jobnam);
   strcat(plotfile,type);
   if ( strcmp(type,PLTEXT) == 0 ) tsnr = 186; else tsnr = 187;
start:
   status = WPfile_selector(IGgtts(tsnr),newpath,TRUE,plotfile,"*",newname);
   if ( status == 0 )
     {
     if ( IGcmpw(wc_str,newname) )
       {
       i = strlen(newname) - 4;
       newname[i] = '\0';
       }
     }
   else return(status);
/*
***Create full plotfile name with path.
*/
   strcpy(plotfile,newpath);
   strcat(plotfile,newname);
   strcat(plotfile,type);
/*
***Owerwrite existing file ?
*/
   if ( strcmp(type,PLTEXT) == 0 ) tsnr = 382; else tsnr = 383;
   if ( IGftst(plotfile) && !IGialt(tsnr,67,68,TRUE) ) goto start;
/*
***Open the new file.
*/
   if ( (fp=fopen(plotfile,"w+")) == NULL )
     {
     erpush("IG3062",plotfile);
     errmes();
     return(0);
     }
/*
***Create the PLT/DXF file.
*/
   if ( strcmp(type,PLTEXT) == 0 ) status = WPmkpf(gwinpt,fp,&plotvy.modwin,&origin);
   else                            status = WPdxf_out(gwinpt,fp,&plotvy,&origin);

   fclose(fp);

   if ( status < 0 )
     {
     erpush("IG5373",plotfile);
     errmes();
     return(0);
     }
/*
***Message.
*/
   strcpy(mesbuf,plotfile);
   strcat(mesbuf,IGgtts(221));
   WPaddmess_mcwin(mesbuf,WP_MESSAGE);
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

       short IGshell()

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
     if ( (status=IGssip("",IGgtts(302),shcmd,dstr,80)) < 0 ) return(status);
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
   status = IGssip(IGgtts(215),IGgtts(267),fnam,dstr,JNLGTH);
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
