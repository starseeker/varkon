/**********************************************************************
*
*    evgrid.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    evgrid();     Evaluerar GRID_VIEW
*    evgrix();     Evaluerar GRIDX_VIEW
*    evgriy();     Evaluerar GRIDY_VIEW
*    evgrdx();     Evaluerar GRIDDX_VIEW
*    evgrdy();     Evaluerar GRIDDY_VIEW
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
#ifdef V3_X11
#include "../../WP/include/WP.h"
#endif
#include "../../EX/include/EX.h"

extern PMPARVA *proc_pv;  /* inproc.c *pv      Access structure for MBS routines */
extern short    proc_pc;  /* inproc.c parcount Number of actual parameters */

extern PMLITVA *func_vp;   /* Pekare till resultat. */

/*!******************************************************/

        short evgrid()

/*      Evaluerar proceduren GRID_VIEW.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      (C)microform ab 1996-02-12
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    extern bool  rstron;
    extern DBfloat rstrox,rstroy,rstrdx,rstrdy;
    extern short WPdrrs(),WPdlrs();

/*
***Rastret skall tändas. Om det redan var tänt gör vi ingenting.
***För att ändra ett raster skall man alltså först släcka det,
***sedan ändra det och sist tända det igen.
*/
    if ( proc_pv[1].par_va.lit.int_va == 1 )
      {
      if ( !rstron )
        {
        rstron = TRUE;
        WPdrrs(rstrox,rstroy,rstrdx,rstrdy);
        }
      }
/*
***Rastret skall släckas. Om det redan var släckt gör vi heller
***ingenting.
*/
    else if ( proc_pv[1].par_va.lit.int_va == 0 )
      {
      if ( rstron )
        {
        rstron = FALSE;
        WPdlrs(rstrox,rstroy,rstrdx,rstrdy);
        }
      }
/*
***Om parametern varken var 1 (Tänd) eller 0 (Släck) gör vi ingenting.
*/

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evgrix()

/*      Evaluerar proceduren GRIDX_VIEW.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      (C)microform ab 1996-02-12
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    extern DBfloat rstrox;

    rstrox = proc_pv[1].par_va.lit.float_va;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evgriy()

/*      Evaluerar proceduren GRIDY_VIEW.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      (C)microform ab 1996-02-12
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    extern DBfloat rstroy;

    rstroy = proc_pv[1].par_va.lit.float_va;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evgrdx()

/*      Evaluerar proceduren GRIDDX_VIEW.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      (C)microform ab 1996-02-12
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    extern DBfloat rstrdx;

    rstrdx = proc_pv[1].par_va.lit.float_va;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evgrdy()

/*      Evaluerar proceduren GRIDDY_VIEW.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      (C)microform ab 1996-02-12
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    extern DBfloat rstrdy;

    rstrdy = proc_pv[1].par_va.lit.float_va;

    return(0);
  }

/********************************************************/
