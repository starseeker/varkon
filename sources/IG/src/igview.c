/*!******************************************************************/
/*  igview.c                                                        */
/*  ========                                                        */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   igcrvp();   Create view from direction                         */
/*   igcrvc();   Create view from coordinate system                 */
/*   dlview();   Delete view                                        */
/*   igcnog();   Set curve accuracy                                 */
/*   igcror();   Move grid                                          */
/*   igcrdx();   Set grid size X                                    */
/*   igcrdy();   Set grid size Y                                    */
/*   igtndr();   Turn grid on                                       */
/*   igslkr();   Turn grid off                                      */
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
#include "../../WP/include/WP.h"
#include "../../EX/include/EX.h"
#include <string.h>

extern MNUALT  smbind[];
extern VY      vytab[];
extern VY      actvy;
extern DBfloat  rstrox,rstroy,rstrdx,rstrdy;
extern bool    rstron;
extern short   modtyp,igtrty,mant,mstack[];

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
    WPerhg();
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
    WPerhg();
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

    WPget_cacc(&dblval);
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

    WPdlrs(rstrox,rstroy,rstrdx,rstrdy);    /* sudda det gamla rastret */

       rstrox = pos.x_gm;
       rstroy = pos.y_gm; 

    WPdrrs(rstrox,rstroy,rstrdx,rstrdy);    /* rita ett nytt raster */

end:
    WPerhg();    
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
    DBfloat dx;
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

    WPdlrs(rstrox,rstroy,rstrdx,rstrdy);    /* sudda det gamla rastret */

    rstrdx = dx;
    WPdrrs(rstrox,rstroy,rstrdx,rstrdy);    /* rita ett nytt raster */

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
    DBfloat dy;
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

    WPdlrs(rstrox,rstroy,rstrdx,rstrdy);    /* sudda det gamla rastret */

    rstrdy = dy;
    WPdrrs(rstrox,rstroy,rstrdx,rstrdy);    /* rita ett nytt raster */

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

    WPdrrs(rstrox,rstroy,rstrdx,rstrdy);    /* Tänd  rastret */

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

    WPdlrs(rstrox,rstroy,rstrdx,rstrdy);    /* Släck rastret */

    rstron = FALSE;

end:
    return(0);
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
extern short WPshad();

   if ( WPshad(0,FALSE) < 0 ) errmes();
#endif

#ifdef WIN32
extern short WPshad();

   if ( WPshad(0,FALSE) < 0 ) errmes();
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
extern short WPshad();

   if ( WPshad(0,TRUE) < 0 ) errmes();
#endif

#ifdef WIN32
extern short WPshad();

   if ( WPshad(0,TRUE) < 0 ) errmes();
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
extern short WPrenw();

   if ( WPrenw() < 0 ) errmes();
#endif

#ifdef WIN32
extern short msrenw();

   if ( msrenw() < 0 ) errmes();
#endif


   return(0);
  }

/********************************************************/
