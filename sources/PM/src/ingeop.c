/**********************************************************************
*
*    ingeop.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    inevgp();   Interpret geometric procedure
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
*    (C)Microform AB 1984-2001, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/evfuncs.h"
#include <math.h>

V2REFVA *geop_id;   /* Entity ID */
PMPARVA *geop_pv;   /* Access structure for MBS routines */
short    geop_pc;   /* Number of actual parameters */
V2NAPA	*geop_np;   /* Ptr to named parameter block.*/

/*!******************************************************/

        short inevgp(
        V2REFVA *identp,
        pm_ptr    routla,
        pm_ptr    arglist,
        V2NAPA  *npblockp)

/*      Generic routine for evaluation of geometric procedures.
 *
 *      In: identp   => Entity ID.
 *          routla   => Ptr to routine.
 *          arglist  => Ptr to actual parameterlist.
 *          npblockp => Ptr to name parameter block.
 *
 *      Out: None.
 *
 *      FV: Returns status of called routines.
 *
 *      (C)microform ab 23/3/86 J. Kjellander
 *
 *      2001-02-02 Param utbytta till Globla variabler, R Svedin
 *      2007-03-31 1.19, J.Kjellander
 *
 ******************************************************!*/

{
   STPROC rout;
   short  status;

 /*
 ***´Set global variables.
 */
   geop_id = identp;
   geop_np = npblockp;
/*
***Get routine info from symboltable.
*/
   if ( (status=strrou(routla,&rout)) < -1 ) return(status);
/*
***Interprete routine parameter list.
*/
   if ( (status=inapar(arglist,routla,&rout,&geop_pv,&geop_pc) ) != 0 )
     {
     inrpar();
     return(status);
     }
/*
***Execute.
*/
   status = (*functab[rout.kind_pr])();
/*
***Free parameter-pool memory.
*/
   inrpar();
/*
***Error check.
*/
   if      ( status == 0 )  return(0);
   else if ( status == -1 ) return(0);
   else                     return(status);
}

/************************************************************************/
