/*!******************************************************************/
/*  igmodule.c                                                      */
/*  ==========                                                      */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*    IGevlp();        Evaluate parameters of active module         */
/*    IGrun_active();  Run active module interactively              */
/*    IGream();        Run active module from macro                 */
/*    IGedit_active(); Edit module in MBS mode                      */
/*    IGprtm();        Decompile to file                            */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://varkon.sourceforge.net                             */
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
#include "../include/isch.h"
#include "../../EX/include/EX.h"
#include "../../WP/include/WP.h"
#include <string.h>

#ifdef DEBUG
#include "../include/debug.h"
#endif

extern pm_ptr  actmod;
extern DBptr   lsysla;
extern char    jobdir[],jobnam[];
extern bool    igbflg;
extern V3MDAT  sydata;

/*!******************************************************/

        short IGevlp()

/*      Går igenom modulens parameterlista och anropar
 *      för varje parameter rutinen inevpa().
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod:    IG2263 => Kan ej evaluera parameterlistan
 *
 *      (C)microform ab 3/9/85 J. Kjellander
 *
 *      26/5/86  Nytt anrop till pmrpap(), J. Kjellander
 *
 ******************************************************!*/

  {
  pm_ptr parptr;

/*
***Initiera parameterpekare.
*/
  if ( pmrpap((pm_ptr)0) < 0 ) goto errend;
/*
***Gå igenom parameterlistan.
*/
  for (;;)
      {
      if ( pmgpad(&parptr) != 0 ) goto errend;
      if ( parptr == (pm_ptr)NULL) return(0);    /* Slut på parametrar ? */
      if ( inevpa(parptr,NULL,NULL,0) < 0 ) goto errend;
      }
/*
***Felutgång.
*/
errend:
  return(erpush("IG2263",""));
  }

/********************************************************/
/*!******************************************************/

        short IGrun_active()

/*      Interaktive function for "Run active module".
 *
 *      (C)microform ab 10/6/92 J. Kjellander
 *
 *      8/10/92  inclpp(), J. Kjellander
 *      1996-12-05 Demo, J.Kjellander
 *      2007-07-29 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short status;
    char  mesbuf[V3STRLEN];

/*
***Init parameter-pool.
*/
    inclpp();
/*
***Clear RTS.
*/
    incrts();
/*
***Execute the currently active module.
*/
    status = IGream();
/*
***Informational message.
*/
    if ( status == 0 )
      {
      sprintf(mesbuf,"%s %s",jobnam,IGgtts(62));
      WPaddmess_mcwin(mesbuf,WP_MESSAGE);
      }
/*
***The end.
*/
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGream()

/*      Reexecute active module. RTS and parameter pool
 *      are not cleared (to save time). In batch mode
 *      display is not updated.
 *
 *      (C)2007-12-17 2.0, J.Kjellander
 *
 ******************************************************!*/

  {
    short status;

/*
***This function is time critical in Varkon so
***code for time monitoring is included here.
*
#ifdef DEBUG
   v3time(V3_TIMER_RESET,0,"");
   v3time(V3_TIMER_ON,0,"");
#endif
*
***Erase graphical windows.
*/
    if ( !igbflg )
      {
      WPergw(GWIN_ALL);
      WPerrw(RWIN_ALL);
      WPwait(GWIN_MAIN,TRUE);
      }
/*
***Write time for erase.
*
#ifdef DEBUG
   v3time(V3_TIMER_OFF,0,"");
   v3time(V3_TIMER_WRITE,0,"Erase all WPGWIN and WPRWIN");
   v3time(V3_TIMER_RESET,0,"");
   v3time(V3_TIMER_ON,0,"");
#endif
*
***Reset DB.
*/
    DBreset();
/*
***Write time for DB reset.
*
#ifdef DEBUG
   v3time(V3_TIMER_OFF,0,"");
   v3time(V3_TIMER_WRITE,0,"DB reset");
   v3time(V3_TIMER_RESET,0,"");
   v3time(V3_TIMER_ON,0,"");
#endif
*
***Execute the module.
*/
    EXmoba();
    inrdnp();
    status=inmod(actmod);
/*
***Write time for module execution.
*
#ifdef DEBUG
   v3time(V3_TIMER_OFF,0,"");
   v3time(V3_TIMER_WRITE,0,"Execute module");
   v3time(V3_TIMER_RESET,0,"");
   v3time(V3_TIMER_ON,0,"");
#endif
*
***Highlight possible active local csys.
*/
    if ( !igbflg )
      {
      IGupcs(lsysla,V3_CS_ACTIVE);
/*
***Update the current settings of PEN, LEVEL
***and CSY in the menu window.
*/
      WPupdate_menu();
/*
***Show grid maybe ?
*/
      WPdraw_grid(GWIN_ALL);
/*
***No more wait.
*/
      WPwait(GWIN_MAIN,FALSE);
/*
***Update WPRWIN's.
*/
      WPrepaint_RWIN(RWIN_ALL,FALSE);
      }
/*
***Write final time.
*
#ifdef DEBUG
   v3time(V3_TIMER_OFF,0,"");
   v3time(V3_TIMER_WRITE,0,"Update cs + grid on + WPuprw(RWIN_ALL)");
#endif
*
***Any errors ?
*/
    if ( status < 0 ) errmes();
/*
***The end.
*/
    return(status);
  }

/********************************************************/
/********************************************************/

       short  IGedit_active()

/*     Edit active module in MBS mode.
*
*      (C)2007-11-20 J. Kjellander.
*
*******************************************************!*/

 {
/*
***Call WP.
*/
   return(WPamod());
 }

/******************************************************!*/
/*!******************************************************/

        short IGprtm()

/*      Varkonfunktion för att skriva ut modul till fil.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkod:  IG0212 = Otillåten skyddskod
 *               IG0222 = Kan ej skapa fil
 *
 *      (C)microform ab 18/4/87 J. Kjellander
 *
 *      15/9/87    Ej dekomp. om mpcode = 0, J. Kjellander
 *      23/9/87    Fixat bug. fclose isf. close, R. Svedin
 *      11/11/88   Ny felhant, J. Kjellander
 *      14/2/92    Filen finns redan, J. Kjellander
 *      1996-01-26 ser_crypt, J. Kjellander
 *
 ******************************************************!*/

  {
    char      buf[80];
    short     status;
    FILE     *mbsfp;

    static char fnam[V3PTHLEN+1]="";

/*
***Fråga efter filnamn.
*/
    strcpy(fnam,jobdir);
    strcat(fnam,jobnam);
    strcat(fnam,MBSEXT);
    status = IGssip(IGgtts(376),IGgtts(267),fnam,fnam,V3PTHLEN);
    if ( status < 0 ) return(status);
/*
***Finns redan en fil med samma namn ?
*/
    if ( (mbsfp=fopen(fnam,"r")) != NULL )
      {
      fclose(mbsfp);
      if ( !IGialt(440,67,68,TRUE) ) return(0);
      }
/*
***Gammal fil finns inte eller skall skrivas över.
*/
    if ( (mbsfp=fopen(fnam,"w")) == NULL )
      {
      erpush("IG0222",fnam);
      errmes();
      return(0);
      }
/*
***Dekompilera.
*/
    pprmo(PPFILE,mbsfp);
    fclose(mbsfp);
/*
***Avslutande meddelande.
*/
    strcpy(buf,fnam);
    strcat(buf,IGgtts(374));
    WPaddmess_mcwin(buf,WP_MESSAGE);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
