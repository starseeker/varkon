/*!******************************************************************/
/*  igset.c                                                         */
/*  =======                                                         */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   IGset_active_attribute(); Creates SET(attribute=expr)          */
/*   IGslvl();                 Creates SET(level=expr)              */
/*   IGspen();                 Creates SET(pen=expr)                */
/*   IGswdt();                 Creates SET(width=expr)              */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.tech.oru.se/cad/varkon                         */
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
#include "../../AN/include/AN.h"

extern V2NAPA defnap;

static short iggset(short  param, pm_ptr exnpt);

/*!******************************************************/

       short IGset_active_attribute(
       int   attribute,
       char *value)

/*      Creates a SET(attribute=expression)...statement.
 *
 *      In: attribute => PMPFONT, PMPSIZE .....
 *          value     => Expression string.
 *
 *      Return: Any error is reported here and -1 is returned.
 *
 *      (C)2007-03-23 J.Kjellander
 *       
 ******************************************************!*/

  {
    pm_ptr  exnpt,arglst,valtyp,namparam,dummy;
    sttycl  type;
    PMLITVA val;

/*
***Create the expression.
*/
   pmmark();

   if (anaexp(value,FALSE,&exnpt,&type) != 0)
     {
     pmrele();
     errmes();
     return(-1);
     }
/*
***Evaluate expression.
*/
   inevev(exnpt,&val,&valtyp);
/*
***Create parameter list for SET function.
*/
    pmcnpa(attribute,exnpt,&namparam);
    pmtcon(namparam,(pm_ptr)NULL,&arglst,&dummy);
/*
***Create the statement, execute and add to end of active module.
*/
    if ( IGcprs("SET",arglst) < 0) goto error;
/*
***The end.
*/
    return(0);
/*
***Error exit.
*/
error:
    errmes();
    pmrele();
    return(-1);
  }
  
/********************************************************/
/*!******************************************************/

       short IGslvl()

/*      Creates SET(LEVEL=integer_expression)....statement
 *
 *      Return: 0      = OK
 *              REJECT = Cancel operation
 *              GOMAIN = Back to main menu
 *
 *      (C)2007-03-17 J. Kjellander
 *
 ******************************************************!*/

  {
   short   status;
   char    istr[V3STRLEN+1],dstr[V3STRLEN+1],errbuf[V3STRLEN];
   pm_ptr  exnpt,valtyp;
   PMLITVA val;

/*
***What level is active now ? Create default string.
*/
   sprintf(dstr,"%d",defnap.level);
/*
***Create the new level number expression.
*/
   pmmark();
   if ( (status=IGcint(227,dstr,istr,&exnpt)) < 0 ) return(status);
/*
***Evaluate and check if this level is already active.
*/
   inevev(exnpt,&val,&valtyp);
   if (  val.lit.int_va == defnap.level )
     {
     pmrele();
     sprintf(errbuf,"%d",defnap.level);
     erpush("IG2332",errbuf);
     errmes();
     return(0);
     }
/*
***Create the SET(LEVEL=n); statement and execute.
*/
   iggset(PMLEVEL,exnpt);
/*
***The end.
*/
   return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short IGspen()

/*      Creates SET(PEN=integer_expression)....statement
 *
 *      Return: 0      = OK
 *              REJECT = Cancel operation
 *              GOMAIN = Back to main menu
 *
 *      (C)2007-03-17 J. Kjellander
 *
 ******************************************************!*/

  {
   short   status;
   char    istr[V3STRLEN+1],dstr[V3STRLEN+1],errbuf[V3STRLEN];
   pm_ptr  exnpt,valtyp;
   PMLITVA val;

/*
***What pen is active now ? Create default string.
*/
   sprintf(dstr,"%d",defnap.pen);
/*
***Create the new pen number expression.
*/
   pmmark();
   if ( (status=IGcint(16,dstr,istr,&exnpt)) < 0 ) return(status);
/*
***Evaluate and check if this pen is already active.
*/
   inevev(exnpt,&val,&valtyp);
   if (  val.lit.int_va == defnap.pen )
     {
     pmrele();
     sprintf(errbuf,"%d",defnap.pen);
     erpush("IG2332",errbuf);
     errmes();
     return(0);
     }
/*
***Create the SET(PEN=n); statement and execute.
*/
   iggset(PMPEN,exnpt);
/*
***The end.
*/
   return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short IGswdt()

/*      Creates SET(WIDTH=float_expression)....statement
 *
 *      Return: 0      = OK
 *              REJECT = Cancel operation
 *              GOMAIN = Back to main menu
 *
 *      (C)2007-03-25 J. Kjellander
 *
 ******************************************************!*/

  {
   short   status;
   char    istr[V3STRLEN+1],dstr[V3STRLEN+1],errbuf[V3STRLEN];
   pm_ptr  exnpt,valtyp;
   double  width;
   PMLITVA val;

/*
***What width is active now ? Create default string.
*/
   sprintf(dstr,"%g",defnap.width);
/*
***Create the new width expression.
*/
   pmmark();
   if ( (status=IGcflt(70,dstr,istr,&exnpt)) < 0 ) return(status);
/*
***Evaluate and check if this width is already active.
*/
   inevev(exnpt,&val,&valtyp);

   if      ( val.lit_type == C_INT_VA) width = val.lit.int_va;
   else if ( val.lit_type == C_FLO_VA) width = val.lit.float_va;

   if ( width == defnap.width )
     {
     pmrele();
     sprintf(errbuf,"%g",defnap.width);
     erpush("IG2332",errbuf);
     errmes();
     return(0);
     }
/*
***Create the SET(WIDTH=n); statement and execute.
*/
   iggset(PMWIDTH,exnpt);
/*
***The end.
*/
   return(0);
  }
  
/********************************************************/
/*!******************************************************/

static short  iggset(
       short  param,
       pm_ptr exnpt)

/*      Create set(param=expression) statement.
 *
 *      In: param  => Namngiven parameter tex. level
 *          exnpt  => Pekare till uttryck
 *
 *      Ut: Inget.
 *
 *      Felkoder:  IG5012 = Kan ej skapa namnparameterlista
 *                 IG5022 = Kan ej skapa set-sats
 *
 *      (C)microform ab 1/7/85 J. Kjellander
 *
 *      18/3/86 Anrop pmtcon B. Doverud
 *       
 ******************************************************!*/

  {
    pm_ptr  namparam,arglst,dummy;

/*
***Skapa namnparameternod och gör en lista av den.
*/
    if ( pmcnpa(param,exnpt,&namparam) < 0 ) goto error1;

    pmtcon(namparam,(pm_ptr)NULL,&arglst,&dummy);
/*
***Skapa satsen, interpretera och länka in i modulen.
*/
    if ( IGcprs("SET",arglst) < 0) goto error2;

    return(0);

/*
***Felutgångar.
*/
error1:
    erpush("IG5012","iggset");
    goto errend;

error2:
    erpush("IG5022","SET");

errend:
    errmes();
    return(0);
  }
  
/********************************************************/
