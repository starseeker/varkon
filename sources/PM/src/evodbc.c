/**********************************************************************
*
*    evodbc.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    evodco();      Evaluerar ODBC_Connect
*    evoded();      Evaluerar ODBC_Execdirect
*    evodfe();      Evaluerar ODBC_Fetch
*    evodgs();      Evaluerar ODBC_Getstring
*    evoddc();      Evaluerar ODBC_Disconnect
*    evoder();      Evaluerar ODBC_Error
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
#include "../../EX/include/EX.h"
#include <string.h>

extern PMPARVA *func_pv;  /* Access structure for MBS routines */
extern short    func_pc;  /* Number of actual parameters */
extern PMLITVA *func_vp;  /* Pekare till resultat. */

/*!******************************************************/

        short evodco()
 
/*      Interface-rutin för ODBC_CONNECT.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 1998-10-12 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   return(EXodco(func_pv[1].par_va.lit.str_va,
                 func_pv[2].par_va.lit.str_va,
                 func_pv[3].par_va.lit.str_va,
                &func_vp->lit.int_va));
  }

/********************************************************/
/*!******************************************************/

        short evoded()
 
/*      Interface-rutin för ODBC_EXECDIRECT.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *          Global *func_pc  => Number of actual parameters.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 1998-10-12 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *      2001-04-03 Optional parameter added, J.Kjellander
 *
 ******************************************************!*/


  {
   char sqlstr[2*V3STRLEN+1];

   strcpy(sqlstr,func_pv[1].par_va.lit.str_va);

   if ( func_pc == 2 ) strcat(sqlstr,func_pv[2].par_va.lit.str_va);

   return(EXoded(sqlstr, &func_vp->lit.int_va));
  }

/********************************************************/
/*!******************************************************/

        short evodfe()
 
/*      Interface-rutin för ODBC_FETCH.
 *
 *      In: 
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 1998-10-12 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   return(EXodfe(&func_vp->lit.int_va));
  }

/********************************************************/
/*!******************************************************/

        short evodgs()
 
/*      Interface-rutin för ODBC_GETSTRING.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 1998-10-12 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   short   status;
   PMLITVA litval;

/*
***Hämta värdet från ODBC.
*/
   status = EXodgs(litval.lit.str_va,&func_vp->lit.int_va);
   if ( status < 0 ) return(status);
/*
***Skriv till motsvarande MBS-variabel.
*/
   return(inwvar(func_pv[1].par_ty,func_pv[1].par_va.lit.adr_va,0,NULL,&litval));
  }

/********************************************************/
/*!******************************************************/

        short evoddc()
 
/*      Interface-rutin för ODBC_DISCONNECT.
 *
 *      In: 
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 1998-10-12 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   return(EXoddc(&func_vp->lit.int_va));
  }

/********************************************************/
/*!******************************************************/

        short evoder()
 
/*      Interface-rutin för ODBC_ERROR.
 *
 *      In: Global  
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 1998-10-12 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   return(EXoder(func_vp->lit.str_va));
  }

/********************************************************/
/********************************************************/
