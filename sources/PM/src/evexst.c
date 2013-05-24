/**********************************************************************
*
*    evexst.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    evexst();         evaluate EXESTAT
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

extern PMLITVA *func_vp;    /* Pekare till resultat. */

/*!******************************************************/

        short evexst()

/*      Evaluerar funktionen EXESTAT.
 *
 *      Ut: Global*func_vp  = 
 *
 *          0 = Kör aktiv modul, f5.
 *          1 = Skapa part, actfun=-2.
 *          2 = Ändra part, f7.
 *          3 = Uppdatera part, f109.
 *
 *      FV: 0.
 *
 *      (C)microform ab 23/7/90 J. Kjellander
 *
 *      1996-05-28 f170, J.Kjellander
 *      2001-03-06 Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    extern short actfun;

    switch (actfun)
      {
      case 5:
      func_vp->lit.int_va = 0;
      break;

      case -2:
      func_vp->lit.int_va = 1;
      break;

      case 7:
      case 170:
      func_vp->lit.int_va = 2;
      break;

      case 109:
      func_vp->lit.int_va = 3;
      break;

      default:
      func_vp->lit.int_va = 999;
      }

    return(0);

  }

/********************************************************/
