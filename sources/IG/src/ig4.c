/*!******************************************************************/
/*  File: ig4.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   igcrvp();   Create view from direction                         */
/*   igcrvc();   Create view from coordinate system                 */
/*   dlview();   Delete view                                        */
/*   liview();   List views                                         */
/*   chview();   Change view                                        */
/*   olview();   Previous view                                      */
/*   scroll();   Pan view                                           */
/*   scale();    Scale view                                         */
/*   igascl();   Autozoom                                           */
/*   igzoom();   Zoom view                                          */
/*   repagm();   Repaint view                                       */
/*   igcnog();   Set curve accuracy                                 */
/*   igcror();   Move grid                                          */
/*   igcrdx();   Set grid size X                                    */
/*   igcrdy();   Set grid size Y                                    */
/*   igtndr();   Turn grid on                                       */
/*   igslkr();   Turn grid off                                      */
/*   igcrsk();   Set drawingscale                                   */
/*   igcvyd();   Set perspective distance                           */
/*   igshd0();   Flat shading                                       */
/*   igshd1();   Smooth shading                                     */
/*   igrenw();   Dynamic shading                                    */
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
#include "../include/screen.h"
#include "../../GP/include/GP.h"
#include "../../EX/include/EX.h"
#include <string.h>

extern MNUALT smbind[];
extern VY     vytab[];
extern VY     actvy;
extern gmflt  rstrox,rstroy,rstrdx,rstrdy;
extern bool   rstron;
extern short  modtyp,igtrty,gptrty,menurx,menury,mant,mstack[];
extern double gpgszx,gpgszy;

/*!******************************************************/

        short igcrvp()

/*      Varkon-funktion för skapa vy med betraktelse-
 *      position.
 *
 *      FV:      0 = OK.
 *          REJECT = Avbruten operation
 *          GOMAIN = Huvudmenyn
 *
 *      (C)microform ab 19/1/85 J. Kjellander
 *
 *      6/9/85  Felhantering, R. Svedin
 *      25/9/85 2D-vyer, J. Kjellander
 *      6/10/86 GOMAIN, J. Kjellander
 *      10/10/86 Inga param för vyfönster till excrvi, R. Svedin
 *      1/2/89   Perspektiv, J. Kjellander
 *      24/10/91 Bytt namn till igcrvp(), J. Kjellander
 *
 ******************************************************!*/

  {
    char   newnam[GPVNLN+1];
    VYVEC  vyrikt;
    DBVector  tmp;
    short  status;

/*
***Läs in vynamn.
*/
    igptma(220,IG_INP);
    if ( (status=igssip(iggtts(267),newnam,"",GPVNLN)) < 0 ) goto end;
/*
**Om geometrimodul, läs in vy-riktning. Annars sätt den till (0,0,1).
*/
    if ( modtyp == 3 )
      {
      igptma(330,IG_MESS);
      status=genpov(&tmp);
      igrsma();
      if ( status < 0 ) goto end;
      }
    else
      {
      tmp.x_gm = 0.0;
      tmp.y_gm = 0.0;
      tmp.z_gm = 1.0;
      }
/*
***Skapa den nya vyn.
*/
    vyrikt.x_vy = tmp.x_gm;
    vyrikt.y_vy = tmp.y_gm;
    vyrikt.z_vy = tmp.z_gm;
    
    if (EXcrvp(newnam,&vyrikt) < 0 ) errmes();

/*
***Avslutning.
*/
end:
    igrsma();
    gphgal(0);
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igcrvc()

/*      Varkon-funktion för att skapa ny vy
 *      med koordinatsystem.
 *
 *      IN: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Avbruten operation
 *          GOMAIN = Huvudmenyn
 *
 *      (C)microform ab 24/10/91 J. Kjellander
 *
 ******************************************************!*/

  {
    char    newnam[GPVNLN+1];
    DBetype   typ;
    bool    end,right;
    short   status;
    DBId    idvek[MXINIV];

/*
***Vynamn.
*/
    igptma(220,IG_INP);
    if ( (status=igssip(iggtts(267),newnam,"",GPVNLN)) < 0 ) goto end;
/*
***Koordinatsystem.
*/
   igptma(271,IG_MESS);
   typ = CSYTYP;
   status = getidt(idvek,&typ,&end,&right,(short)0);
   igrsma();
   if ( status < 0 ) goto end;
/*
***Skapa den nya vyn.
*/
    if (EXcrvc(newnam,idvek) < 0 ) errmes();
/*
***Avslutning.
*/
end:
    igrsma();
    gphgal(0);
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short dlview()

/*      Varkon-funktion för ta bort vy.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Avbruten operation
 *          GOMAIN = Huvudmenyn
 *
 *      Felkod: IG3012 = Vyn %s finns ej.
 *              IG3052 = Vyn %s är aktiv.
 *
 *      (C)microform ab 10/1/84 J. Kjellander
 *
 *      6/9/85   Felhantering, R. Svedin
 *      13/3/86  Aktiv vy, J. Kjellander
 *      6/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

 {
    char  name[GPVNLN+1];
    short i,status;

/*
***Läs in vynamn.
*/
    igptma(220,IG_INP);
    if ( (status=igssip(iggtts(267),name,"",GPVNLN)) < 0 ) goto end;
/*
***Kolla om vyn är aktiv.
*/
    if ( strcmp(actvy.vynamn,name) == 0 )
      {
      erpush("IG3052",name);
      goto errend;
      }
/*
***Stryk vyn.
*/
    if ( (i=vyindx(name)) < 0 ) 
      {
      erpush("IG3012",name);
      goto errend;
      }
    else vytab[i].vynamn[0] = '\0';
/*
***Slut.
*/
end:
    igrsma();
    return(status);
/*
***Felutgång.
*/
errend:
    errmes();
    goto end;
 }

/********************************************************/
/*!******************************************************/

        short liview()

/*      Varkon-funktion för lista vyer.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:     0 = Ok.
 *         GOMAIN = Huvudmenyn.
 *
 *      (C)microform ab 10/1/84 J. Kjellander
 *
 *      6/10/86  GOMAIN och nytt listformat, J. Kjellander
 *      29/9/92  Nya vyer, J. Kjellander
 *      1999-01-08 actvy.vydist, J.Kjellander
 *
 ******************************************************!*/

  {
    short  i,status;
    char   strbuf[V3STRLEN+1];

/*
***Initiera listarean.
*/
    iginla(iggtts(TS_VLHEAD));
/*
***Uppdatera den vy i vytab som är aktiv med senaste
***vy-data från actvy.
*/
    if ( (i=vyindx(actvy.vynamn)) != -1 )
      {
      vytab[i].vywin[0] = actvy.vywin[0];
      vytab[i].vywin[1] = actvy.vywin[1];
      vytab[i].vywin[2] = actvy.vywin[2];
      vytab[i].vywin[3] = actvy.vywin[3];
      vytab[i].vydist   = actvy.vydist;
      }
/*
***Lista vyerna.
*/
    for ( i=0; i < GPMAXV; ++i)
      {
      if ( vytab[i].vynamn[0] != '\0' )
        {
        sprintf(strbuf,"%-15s%10g%10g%10g%10g   ",vytab[i].vynamn,
                                                  vytab[i].vywin[0],
                                                  vytab[i].vywin[1],
                                                  vytab[i].vywin[2],
                                                  vytab[i].vywin[3]);
        if ( strcmp(vytab[i].vynamn,actvy.vynamn) == 0 )
          strcat(strbuf,iggtts(224));
        if ( (status=igalla(strbuf,(short)1)) < 0 ) return(status);
/*
***Om 3D-modul, skriv även ut ev. vyriktning samt ev. vydist och
***en extra tom rad.
*/
        if ( modtyp == 3 )
          {
          if ( vytab[i].vytypp == TRUE )
            {
            sprintf(strbuf," %s(%g, %g, %g)",iggtts(49),
                                            vytab[i].vyrikt.x_vy,
                                            vytab[i].vyrikt.y_vy,
                                            vytab[i].vyrikt.z_vy);
            if ( (status=igalla(strbuf,(short)1)) < 0 ) return(status);
            }
          else
            {
            sprintf(strbuf," %s",iggtts(111));
            if ( (status=igalla(strbuf,(short)1)) < 0 ) return(status);
            sprintf(strbuf,"     %g, %g, %g",vytab[i].vymatr.v11,
                                             vytab[i].vymatr.v12,
                                             vytab[i].vymatr.v13);
            if ( (status=igalla(strbuf,(short)1)) < 0 ) return(status);
            sprintf(strbuf,"     %g, %g, %g",vytab[i].vymatr.v21,
                                             vytab[i].vymatr.v22,
                                             vytab[i].vymatr.v23);
            if ( (status=igalla(strbuf,(short)1)) < 0 ) return(status);
            sprintf(strbuf,"     %g, %g, %g",vytab[i].vymatr.v31,
                                             vytab[i].vymatr.v32,
                                             vytab[i].vymatr.v33);
            if ( (status=igalla(strbuf,(short)1)) < 0 ) return(status);
            }

          if ( vytab[i].vydist != 0.0 )
            {
            sprintf(strbuf," %s %g",iggtts(152),vytab[i].vydist);
            if ( (status=igalla(strbuf,(short)1)) < 0 ) return(status);
            }
          else
            { 
            sprintf(strbuf," %s",iggtts(154));
            if ( (status=igalla(strbuf,(short)1)) < 0 ) return(status);
            }
          if ( (status=igalla(" ",(short)1)) < 0 ) return(status);
          }
        }
      }
/*
***Avslutning.
*/
    return(igexla());
  }

/********************************************************/
/*!******************************************************/

        short chview()

/*      Varkon-funktion för aktivera ny vy.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *          GOMAIN = Huvudmenyn.
 *
 *      (C)microform ab 10/1/84 J. Kjellander
 *
 *      6/10/86  GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
    char   newnam[GPVNLN+1];
    short  status;

/*
***Läs in vynamn.
*/
    igptma(220,IG_INP);
    if ( (status=igssip(iggtts(267),newnam,"",GPVNLN)) < 0 ) goto end;
/*
***Aktivera vyn.
*/
    if ( EXacvi(newnam,0) < 0 )
      {
      errmes();
      igrsma();
      return(0);
      }
    repagm();
    igupsa();
/*
***Avslutning.
*/
end:
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short olview()

/*      Varkon-funktion för aktivera föregående bild.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *
 *      (C)microform ab 10/3/91 J. Kjellander
 *
 ******************************************************!*/

  {
    VY oldv;
/*
***Hämta föregående vy.
*/
    gpgovy(&oldv);
/*
***Aktivera.
*/
    gpstvi(&oldv);
    gpswin(&oldv);

    repagm();
    igupsa();
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short scroll()

/*      Varkon-funktion för scroll.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *          GOMAIN = Huvudmenyn.
 *
 *      Felkod: 
 *
 *      (C)microform ab 10/1/84 J. Kjellander
 *
 *      9/9/85   Utskrifter, R. Svedin
 *      30/12/85 Pekmärke, J. Kjellander
 *      6/10/86  GOMAIN, J. Kjellander
 *      9/10/86  GOMAIN, B. Doverud
 *      15/10/86 gpswin(), J. Kjellander
 *      18/10/86 gpgwin(), J. Kjellander
 *
 ******************************************************!*/

  {
    char    pektkn;
    double  x,y;
    VY      oldwin,newwin;
/*
***Läs in nytt centrum.
*/
loop:
    igptma(321,IG_MESS);
    gpgtmc(&pektkn,&x,&y,FALSE);
    igrsma();

    if ( pektkn == *smbind[1].str ) return(REJECT);
    if ( pektkn == *smbind[7].str ) return(GOMAIN);
    if ( pektkn == *smbind[8].str )
      {
      ighelp();
      goto loop;
      }

    if ( pektkn != ' ' )
      {
      igbell();
      goto loop;
      }
/*
***Beräkna nytt fönster.
*/
    gpgwin(&newwin);
    gpgwin(&oldwin);
    newwin.vywin[0] = x - (oldwin.vywin[2]-oldwin.vywin[0])/2.0;
    newwin.vywin[2] = x + (oldwin.vywin[2]-oldwin.vywin[0])/2.0;
    newwin.vywin[1] = y - (oldwin.vywin[3]-oldwin.vywin[1])/2.0;
    newwin.vywin[3] = y + (oldwin.vywin[3]-oldwin.vywin[1])/2.0;
/*
***Skapa det nya fönstret.
*/
    gpswin(&newwin); 
/*
***Generera ny bild.
*/
    repagm();
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short scale()

/*      Varkon-funktion för ändring av skala.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *          GOMAIN = Huvudmenyn.
 *
 *      Felkod: IG3042 = Kan ej minska skalan till begärt värde
 *              IG3032 = Kan ej skapa fönster
 *
 *      (C)microform ab 10/1/84 J. Kjellander
 *
 *       6/9/85   Utskrifter, R. Svedin
 *       13/2/86  Variabler för skärmstorlek, R. Svedin
 *       6/10/86  GOMAIN, J. Kjellander
 *       15/10/86 gpswin(), J. Kjellander
 *       18/10/86 gpgwin(), J. Kjellander
 *       29/9/87  genflv(), J. Kjellander
 *
 ******************************************************!*/

  {
    double skala;
    short  status;
    char   istr[V3STRLEN+1];
    VY     oldwin,newwin;

    static char dstr[V3STRLEN+1] = "1.0";

/*
***Läs in ny skala.
*/
    if ( (status=genflv(222,istr,dstr,&skala)) < 0 ) goto end;
    strcpy(dstr,istr);

    if ( skala <= 1e-10 )
      {
      erpush("IG3042","");
      errmes();
      return(0);
      }
/*
***Beräkna nytt fönster.
*/
    gpgwin(&oldwin);
    gpgwin(&newwin);

    newwin.vywin[0] = oldwin.vywin[0] + 
                     (oldwin.vywin[2]-oldwin.vywin[0])/2.0 -
                      gpgszx/2.0/skala;
    newwin.vywin[2] = oldwin.vywin[0] +
                     (oldwin.vywin[2]-oldwin.vywin[0])/2.0 + 
                      gpgszx/2.0/skala;
    newwin.vywin[1] = oldwin.vywin[1] +
                     (oldwin.vywin[3]-oldwin.vywin[1])/2.0 -
                      gpgszy/2.0/skala;
    newwin.vywin[3] = oldwin.vywin[1] +
                     (oldwin.vywin[3]-oldwin.vywin[1])/2.0 +
                      gpgszy/2.0/skala;
/*
***Skapa det nya fönstret.
*/
    if ( gpswin(&newwin) < 0 ) 
      {
      erpush("IG3032","");
      errmes();
      return(0);
      }
/*
***Uppdatera statusarean.
*/
    igupsa();
/*
***Generera ny bild.
*/
    repagm();
end:
    return(status);

  }

/********************************************************/
/*!******************************************************/

        short igascl()

/*      Varkon-funktion för auto-skala.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *
 *      (C)microform ab 15/1/84 J. Kjellander
 *
 *      8/2/90  Vänta..., J. Kjellander
 *      19/3/91 <CTRL>c, J. Kjellander
 *      30/6/92 X11, J. Kjellander
 *
 ******************************************************!*/

  {
    VY     minwin;

/*
***Beräkna nytt fönster, avbryt om interrupt från
***tangentbordet.
*/
    igptma(164,IG_MESS);
    gpgwin(&minwin);

    if ( iggtmx(&minwin) == AVBRYT )
      {
      igrsma();
      igwtma(168);
      return(0);
      }
/*
***Skapa det nya fönstret.
*/
    gpswin(&minwin);
/*
***Uppdatera statusfältet.
*/
    igupsa();
/*
***Generera ny bild.
*/
    igrsma();
    repagm();

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short igzoom()

/*      Varkon-funktion för ZOOM.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *          GOMAIN = Huvudmenyn.
 *
 *      Felkod: IG3042 = Kan ej minska skalan till begärt värde
 *
 *      (C)microform ab 10/1/84 J. Kjellander
 *
 *      6/9/85   Felhantering, R. Svedin
 *      30/12/85 Pekmärke, J. Kjellander
 *      6/10/86  GOMAIN, J. Kjellander
 *      9/10/86  GOMAIN, B. Doverud
 *      15/10/86 gpswin(), J. Kjellander
 *
 ******************************************************!*/

  {
    char   pektkn;
    double x1,y1,x2,y2;
    double tmp,k,dx,dy;
    VY     newwin;

/*
***Läs in nytt fönster.
*/
loop1:
    igptma(322,IG_MESS);
    gpgtmc(&pektkn,&x1,&y1,FALSE);
    igrsma();
    if ( pektkn == *smbind[1].str ) return(REJECT);
    if ( pektkn == *smbind[7].str ) return(GOMAIN);
    if ( pektkn == *smbind[8].str )
      {
      ighelp();
      goto loop1;
      }

    if ( pektkn != ' ' )
      {
      igbell();
      goto loop1;
      }
loop2:
    igptma(323,IG_MESS);
    gpgtmc(&pektkn,&x2,&y2,FALSE);
    igrsma();
    if ( pektkn == *smbind[1].str ) return(REJECT);
    if ( pektkn == *smbind[7].str ) return(GOMAIN);
    if ( pektkn == *smbind[8].str )
      {
      ighelp();
      goto loop2;
      }

    if ( pektkn != ' ' )
      {
      igbell();
      goto loop2;
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
      erpush("IG3042","igzoom");
      errmes();
      goto loop1;
      }
/*
***Skapa nytt fönster.
*/
    gpgwin(&newwin);

    if ((dy/dx) - (k=gpgszy/gpgszx) < 0)
      {
      newwin.vywin[0] = x1;
      newwin.vywin[2] = x2;
      newwin.vywin[1] = (y2+y1)/2.0 - k*dx/2.0;
      newwin.vywin[3] = (y2+y1)/2.0 + k*dx/2.0;
      }
    else
      {
      newwin.vywin[1] = y1;
      newwin.vywin[3] = y2;
      newwin.vywin[0] = (x2+x1)/2.0 - dy/k/2.0;
      newwin.vywin[2] = (x2+x1)/2.0 + dy/k/2.0;
      }
/*
***Skapa det nya fönstret.
*/
    gpswin(&newwin);
/*
***Uppdatera statusfältet.
*/
    igupsa();
/*
***Generera ny bild.
*/
    repagm();
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short repagm()

/*      Varkon-funktion för repaint från GM.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 10/1/84 J. Kjellander
 *
 *      28/10/88 CGI, J. Kjellander
 *      3/11/88  N220G, R. Svedin
 *
 ******************************************************!*/

  {
    short  m,status;

/*
***Rita om den grafiska skärmen.
*/
    gperal();
    gpdram();
    if ( rstron ) gpdrrs(rstrox,rstroy,rstrdx,rstrdy);
    status = EXdral(GWIN_MAIN);
/*
***Om VT100-bildminne saknas är skärmen nu tom på
***text. Uppdatera menyer och status utan onödig
***suddning, skärmen är ju suddad.
*/
    switch ( igtrty )
      {
      case CGI:
      case N220G:
      case MSCOLOUR:
      case MSMONO:
      if ( mant > 0 )
        {
        if ( (m=mstack[mant-1]) == 0 ) m = iggmmu();
        igpamu(menurx,menury,m);
        }
      iggnsa();
      break;
      }

    return(status);
  }

/********************************************************/
/*!******************************************************/

        short igcnog()

/*      Varkon-funktion för ändring av kurvnoggrannhet.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *          GOMAIN = Huvudmenyn.
 *
 *      (C)microform ab 15/2/84 J. Kjellander
 *
 *      18/10/85 Minvärde = 0.001 J. Kjellander
 *      6/10/86 GOMAIN, J. Kjellander
 *      39/9/87 genflv, J. Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    double dblval;
    char   knstr[V3STRLEN+1];
    char   istr[V3STRLEN+1];

    static char dstr[V3STRLEN+1] = "1.0";

    gpgtcn(&dblval);
    sprintf(knstr,"%s%g  %s",iggtts(247),dblval,iggtts(248));
    igplma(knstr,IG_INP);

loop:
    if ( (status=genflv(0,istr,dstr,&dblval)) < 0 ) goto end;
    strcpy(dstr,istr);

    if ( EXcavi(dblval) < 0 ) 
      {
      errmes();
      goto loop;
      }

end:
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short igcror()

/*      Huvudrutin för ändra läge på raster.
 *      Om rastret är tänt ritas det om.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *          GOMAIN = Huvudmenyn.
 *
 *      (C)microform ab 29/8/85 R. Svedin
 *
 *      6/9/85 R. Svedin   Inläsning via pos.menyn
 *      6/10/86 GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
    short status;
    DBVector pos;
    char  strbuf[80];

/*
***Skriv ut nuvarande värde.
*/
    sprintf(strbuf,"%s(%g,%g)  %s",iggtts(43),rstrox,rstroy,iggtts(44));
    igplma(strbuf,IG_MESS);
/*
***Hämta ett nytt värde för origo.
*/
    if ( (status=genpov(&pos)) < 0 ) goto end;
/*
***Om rastret är tänt rita om.
*/
    if ( rstron == FALSE ) 
       {
       rstrox = pos.x_gm;
       rstroy = pos.y_gm;
     goto end;
       }

    gpdlrs(rstrox,rstroy,rstrdx,rstrdy);    /* sudda det gamla rastret */

       rstrox = pos.x_gm;
       rstroy = pos.y_gm; 

    gpdrrs(rstrox,rstroy,rstrdx,rstrdy);    /* rita ett nytt raster */

end:
    gphgal(0);    
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short igcrdx()

/*      Huvudrutin för ändra delning i X-led för raster.
 *      Om rastret är tänt ritas det om.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *          GOMAIN = Huvudmenyn.
 *
 *
 *      (C)microform ab 29/8/85 R. Svedin
 *
 *      6/10/86 GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
    short status;
    gmflt dx;
    char  strbuf[80];
    char   istr[V3STRLEN+1];

    static char dstr[V3STRLEN+1] = "10.0";

/*
***Skriv ut nuvarande värde.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(43),rstrdx,iggtts(45));
    igplma(strbuf,IG_INP);
/*
***Läs in nytt delningsvärde i X-led.
*/
    if ( (status=genflv(0,istr,dstr,&dx)) < 0 ) goto end;
    strcpy(dstr,istr);
/*
***Om rastret är tänt rita om.
*/
    if ( rstron == FALSE ) 
       {
       rstrdx = dx;
       goto end;
       }

    gpdlrs(rstrox,rstroy,rstrdx,rstrdy);    /* sudda det gamla rastret */

    rstrdx = dx;
    gpdrrs(rstrox,rstroy,rstrdx,rstrdy);    /* rita ett nytt raster */

end:
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short igcrdy()

/*      Huvudrutin för ändra delning i Y-led för raster.
 *      Om rastret är tänt ritas det om.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *          GOMAIN = Huvudmenyn.
 *
 *      (C)microform ab 29/8/85 R. Svedin
 *
 *      6/10/86 GOMAIN, J. Kjellander
 *
 ******************************************************!*/

  {
    short status;
    gmflt dy;
    char  strbuf[80];
    char   istr[V3STRLEN+1];

    static char dstr[V3STRLEN+1] = "10.0";

/*
***Skriv ut nuvarande värde.
*/
    sprintf(strbuf,"%s%g  %s",iggtts(43),rstrdy,iggtts(45));
    igplma(strbuf,IG_INP);
/*
***Läs in nytt delningsvärde i Y-led.
*/
    if ( (status=genflv(0,istr,dstr,&dy)) < 0 ) goto end;
    strcpy(dstr,istr);
/*
***Om rastret är tänt rita om.
*/
    if ( rstron == FALSE ) 
       {
       rstrdy = dy;
       goto end;
       }

    gpdlrs(rstrox,rstroy,rstrdx,rstrdy);    /* sudda det gamla rastret */

    rstrdy = dy;
    gpdrrs(rstrox,rstroy,rstrdx,rstrdy);    /* rita ett nytt raster */

end:
    igrsma();
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short igtndr()

/*      Huvudrutin för att tända raster.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod: 
 *              
 *
 *      (C)microform ab 29/8/85 R. Svedin
 *
 ******************************************************!*/

  {

/*
***Om rastret är släckt rita ett.
*/
    if ( rstron == TRUE ) goto end;

    gpdrrs(rstrox,rstroy,rstrdx,rstrdy);    /* Tänd  rastret */

    rstron = TRUE;

end:
    return(0);
  }

/********************************************************/
/*!******************************************************/

       short igslkr()

/*      Huvudrutin för att släcka raster.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod: 
 *              
 *
 *      (C)microform ab 29/8/85 R. Svedin
 *
 ******************************************************!*/

  {

/*
***Om rastret är tänt släck det.
*/
    if ( rstron == FALSE ) goto end;

    gpdlrs(rstrox,rstroy,rstrdx,rstrdy);    /* Släck rastret */

    rstron = FALSE;

end:
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short igcrsk()

/*      Varkon-funktion för ändring av rit-skala.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *          GOMAIN = Huvudmenyn.
 *
 *      Felkod: IG3042 = Kan ej minska skalan till begärt värde
 *
 *      (C)microform ab 20/10/86 J. Kjellander
 *
 ******************************************************!*/

  {
    char   buf[80];
    double skala;
    short  status;
    char   istr[V3STRLEN+1];

    static char dstr[V3STRLEN+1] = "1.0";

/*
***Läs in ny skala.
*/
    gpgrsk(&skala);
    sprintf(buf,"%s%g%s",iggtts(43),skala,iggtts(362));
    igplma(buf,IG_INP);

    if ( (status=genflv(0,istr,dstr,&skala)) < 0 ) goto end;
    strcpy(dstr,istr);

    if ( skala <= 1e-10 )
      {
      erpush("IG3042","");
      errmes();
      goto end;
      }
/*
***Aktivera ny ritskala.
*/
    gpsrsk(skala);
/*
***Rita om skärmen.
*/
    repagm();
/*
***Slut.
*/
end:
    igrsma();
    return(status);

  }

/********************************************************/
/*!******************************************************/

        short igcvyd()

/*      Varkon-funktion för ändring av betraktelseavstånd.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          REJECT = Operationen avbruten.
 *          GOMAIN = Huvudmenyn.
 *
 *      (C)microform ab 1/2/89 J. Kjellander
 *
 ******************************************************!*/

  {
    char   buf[80];
    double dist;
    short  status;
    char   istr[V3STRLEN+1];

/*
***Läs in nytt betraktelseavstånd.
*/
    sprintf(buf,"%s%g %s",iggtts(43),actvy.vydist,iggtts(153));
    igplma(buf,IG_INP);
loop:
    if ( (status=genflv(0,istr,"0",&dist)) < 0 ) goto end;
/*
***Aktivera.
*/
    if ( (status=EXprvi(actvy.vynamn,dist)) < 0 )
      {
      errmes();
      goto loop;
      }
/*
***Rita om skärmen.
*/
    repagm();
/*
***Slut.
*/
end:
    igrsma();
    return(status);

  }

/********************************************************/
/*!******************************************************/

        short igshd0()

/*      Varkon-funktion för flat shading.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 1997-02-07 J. Kjellander
 *
 ******************************************************!*/

  {

#ifdef V3_X11
extern short wpshad();

   if ( wpshad(0,FALSE) < 0 ) errmes();
#endif

#ifdef WIN32
extern short wpshad();

   if ( wpshad(0,FALSE) < 0 ) errmes();
#endif

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short igshd1()

/*      Varkon-funktion för smooth shading.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 1997-02-07 J. Kjellander
 *
 ******************************************************!*/

  {

#ifdef V3_X11
extern short wpshad();

   if ( wpshad(0,TRUE) < 0 ) errmes();
#endif

#ifdef WIN32
extern short wpshad();

   if ( wpshad(0,TRUE) < 0 ) errmes();
#endif

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short igrenw()

/*      Varkon-funktion för renderingsfönstret.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 1997-12-21 J. Kjellander
 *
 ******************************************************!*/

  {

#ifdef V3_X11
extern short wprenw();

   if ( wprenw() < 0 ) errmes();
#endif

#ifdef WIN32
extern short msrenw();

   if ( msrenw() < 0 ) errmes();
#endif


   return(0);
  }

/********************************************************/
