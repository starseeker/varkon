/*!******************************************************************/
/*  igview.c                                                        */
/*  ========                                                        */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   IGset_cacc();      Set curve accuracy                          */
/*   IGcreate_gwin();   Create a new WPGWIN                         */
/*   IGcreate_rwin();   Create a new WPRWIN                         */
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
#include "../../WP/include/WP.h"
#include "../../EX/include/EX.h"
#include <string.h>

extern MNUALT  smbind[];
extern short   mant,mstack[];

/*!******************************************************/

        short IGset_cacc()

/*      Set current curve accuracy factor.
 *
 *      Return:  0 = OK.
 *          REJECT = Cancel
 *          GOMAIN = Main menu
 *
 *      (C)microform ab 15/2/84 J. Kjellander
 *
 *      18/10/85 Minvärde = 0.001 J. Kjellander
 *      6/10/86 GOMAIN, J. Kjellander
 *      39/9/87 IGcflv, J. Kjellander
 *      2007-11-15 2.0, J.Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    double old_cacc,new_cacc;
    char   mstr[V3STRLEN+1],istr[V3STRLEN+1];

    static char dstr[V3STRLEN+1] = "1.0";

/*
***Get current cacc.
*/
    WPget_cacc(&old_cacc);
/*
***Ask for new.
*/
    sprintf(mstr,"%s%g  %s",IGgtts(247),old_cacc,IGgtts(248));
    WPaddmess_mcwin(mstr,WP_MESSAGE);

loop:
    if ( (status=IGcflv(0,istr,dstr,&new_cacc)) < 0 ) goto end;
    strcpy(dstr,istr);
/*
***Execute.
*/
    if ( new_cacc != old_cacc )
      {
      if ( EXcavi(new_cacc) < 0 ) 
        {
        errmes();
        goto loop;
        }
      else
        {
        WPrepaint_GWIN(GWIN_ALL);
        WPrepaint_RWIN(RWIN_ALL,FALSE);
        sprintf(mstr,"%s%g",IGgtts(247),new_cacc);
        WPaddmess_mcwin(mstr,WP_MESSAGE);
        }
      }
/*
***The end.
*/
end:
    return(status);
  }

/********************************************************/
/********************************************************/

        short IGcreate_gwin()

/*      Create a new WPGWIN.
 *
 *      (C)2007-11-14 J.Kjellander
 *
 ******************************************************!*/

  {
/*
***This goes directly to WP.
*/
   if ( WPneww() < 0 ) errmes();
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/********************************************************/

        short IGcreate_rwin()

/*      Create a new WPRWIN.
 *
 *      (C)2007-11-14 J.Kjellander
 *
 ******************************************************!*/

  {
/*
***This goes directly to WP.
*/
   if ( WPrenw() < 0 ) errmes();
/*
***The end.
*/
   return(0);
  }

/********************************************************/
