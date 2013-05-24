/**********************************************************************
*
*    inproc.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    inevpr();     Interpreterar procedur
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
#include "../include/mbsrout.h"
#include "../include/evfuncs.h"

PMPARVA *proc_pv;    /* Gamla *pv       Access structure for MBS routines */
short    proc_pc;    /* Gamla parcount  Number of actual parameters */
pm_ptr   proc_argl;  /* Gamla arglist   Pekare till aktuell parameterlista. */

/*!******************************************************/

        short inevpr(
        pm_ptr routla,
        pm_ptr arglist)

/*      Central rutin för interpretering av ordinära
 *      procedurer.
 *
 *      In: routla  => Pekare till rutin.
 *          arglist => Pekare till aktuell parameterlista.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-12 Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

 {
   STPROC rout;           /* Access structure for MBS routines */
   short status;

/*
***Läs rutin-info från symboltabell.
*/
/*
***Initiera.
*/
   proc_argl = arglist;

   if ( ( status = strrou( routla, &rout ) ) < -1 )
        return( status );
/*
***Interpretera rutinens parameterlista. Special för SET och SET_BASIC.
*/
   if ( rout.kind_pr != VSET  &&  rout.kind_pr != VSETB )
     {
     if ( ( status = inapar( proc_argl, routla, &rout, &proc_pv, &proc_pc ) ) != 0 )
       {
       inrpar();
       return(status);
       }
     }
/*
***Exekvera proceduren. Special för SET och SET_BASIC.
*/
   switch ( rout.kind_pr )
     {
     case VSET:
     if ( ( status = evset() ) < -1 ) return( status);
     else return(0);
     break;

     case VSETB:
     if ( ( status = evsetb() ) < -1 ) return( status);
     else return(0);

     default:
     status = (*functab[rout.kind_pr])();
     }
/*
***Frigör parameter-pool utrymme.
*/
  inrpar();
/*
***Felkontroll.
*/
  if ( status == 0 ) return(0);
  else if ( status == -1 ) return(0);
  else return(status);
}

/********************************************************/

