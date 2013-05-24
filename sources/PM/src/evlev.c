/**********************************************************************
*
*    evlev.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*    evbllv();     Evaluates BLANK_LEVEL
*    evublv();     Evaluates UNBLANK_LEVEL
*    evgtlv();     Evaluates GET_LEVEL
*    evnmlv();     Evaluates NAME_LEVEL
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
#ifdef UNIX
#include "../../WP/include/WP.h"
#endif
#include "../../EX/include/EX.h"

extern PMPARVA *proc_pv;  /* inproc.c *pv      Access structure for MBS routines */
extern short    proc_pc;  /* inproc.c parcount Number of actual parameters */

extern PMLITVA *func_vp;   /* Pekare till resultat. */

/*!******************************************************/

        short evbllv()

/*      Evaluates procedure BLANK_LEVEL.
 *
 *      Error: IN5792 = Error when executing procedure %s
 *
 *      (C)microform ab 1/6/86 J. Kjellander
 *
 *      17/1-95  Multifönster, J. Kjellander
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *      2007-04-01 1,19, J.Kjellander
 *
 ******************************************************!*/

  {
    if ( EXblank_level(proc_pv[1].par_va.lit.int_va,
                       proc_pv[2].par_va.lit.int_va) < 0 )
      return(erpush("IN5792","BLANK_LEVEL"));
    else
      return(0);
  }

/********************************************************/
/*!******************************************************/

        short evublv()

/*      Evaluates procedure UNBLANK_LEVEL().
 *
 *      Error: IN5792 = Error when executing procedure %s
 *
 *      (C)microform ab 1/6/86 J. Kjellander
 *
 *      17/1-95  Multifönster, J. Kjellander
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *      2007-04-01 1,19, J.Kjellander
 *
 ******************************************************!*/

  {
    if ( EXunblank_level(proc_pv[1].par_va.lit.int_va,
                         proc_pv[2].par_va.lit.int_va) < 0 )
      return(erpush("IN5792","UNBLANK_LEVEL"));
    else
      return(0);
  }

/********************************************************/
/*!******************************************************/

        short evgtlv()

/*      Evaluates procedure GET_LEVEL().
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 1/6/86 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *      2007-04-01 1,19, J.Kjellander
 *
 ******************************************************!*/

  {
   bool    blank;
   char    name[V3STRLEN+1];
   short   status;
   PMLITVA litval;

/*
***Get level information.
*/
   if ( (status=EXget_level(proc_pv[1].par_va.lit.int_va,
                            proc_pv[2].par_va.lit.int_va,
                           &blank,name)) < 0 ) return(status);;
/*
***Level data to MBS variable.
*/
   if ( blank ) litval.lit.int_va = 1;
   else         litval.lit.int_va = 0;

   inwvar(proc_pv[3].par_ty,proc_pv[3].par_va.lit.adr_va,0,NULL,&litval);
/*
***Optional name to MBS variable.
*/
   if ( proc_pc == 4 )
     {
     strcpy(litval.lit.str_va,name);
     inwvar(proc_pv[4].par_ty,proc_pv[4].par_va.lit.adr_va,0,NULL,&litval);
     }
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short evnmlv()

/*      Evaluates procedure NAME_LEVEL().
 *
 *      Error: IN5792 = Error when executing procedure %s
 *
 *      (C)2007-03-31 J. Kjellander
 *
 ******************************************************!*/

  {
    if ( EXname_level(proc_pv[1].par_va.lit.int_va,
                      proc_pv[2].par_va.lit.str_va) < 0 )
      return(erpush("IN5792","NAME_LEVEL"));
    else
      return(0);
  }

/********************************************************/
