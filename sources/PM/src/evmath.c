/**********************************************************************
*    evmath.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*    short evacos() -  Evaluate arithmetic function ARCCOS
*    short evasin() -  Evaluate arithmetic function ARCSIN
*    short evatan() -  Evaluate arithmetic function ARCTAN
*    short evcos()  -  Evaluate arithmetic function COS
*    short evsin()  -  Evaluate arithmetic function SIN
*    short evtan()  -  Evaluate arithmetic function TAN
*    short evnlog() -  Evaluate arithmetic function LN
*    short evlogn() -  Evaluate arithmetic function LOG
*    short evsqrt() -  Evaluate arithmetic function SQRT
*    short evabs()  -  Evaluate arithmetic function ABS for INT and FLOAT
*    short evfrac() -  Evaluate arithmetic function FRAC
*    short evroud() -  Evaluate arithmetic function ROUND
*    short evtrnc() -  Evaluate arithmetic function TRUNC
*    short evrand() -  Evaluate arithmetic function RANDOM
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

#include <math.h>
#include <stdlib.h>
#include <string.h>

extern PMPARVA *func_pv;   /* Access structure for MBS routines */
extern PMLITVA *func_vp;   /* Pekare till resultat. */
extern short    func_pc;   /* Number of actual parameters */
extern pm_ptr  *func_ty;   /* Pekare till resultattyp. */
extern pm_ptr   func_al;   /* Pekare till aktuell parameterlista. */
/*
***standard types
*/
extern pm_ptr  stintp;
extern pm_ptr  stflop;


/*!******************************************************/

        short evacos()

/*      Evaluate ARCCOS. Returns the arc cosine 
 *
 *      FV: Returnerar status.
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-03-01 Moved to own function, R Svedin
 *
 ******************************************************!*/

 {

   func_vp->lit.float_va = RDTODG*(v2float)acos(func_pv[1].par_va.lit.float_va);
    
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evasin()

/*      Evaluate ARCCOS Returns the arc sine.
 *
 *      FV: Returnerar status.
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-03-02 Moved to own function, R Svedin
 *
 ******************************************************!*/

 {

   func_vp->lit.float_va = RDTODG*(v2float)asin( func_pv[1].par_va.lit.float_va);
    
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evatan()

/*      Evaluate ARCTANG Returns the arc tangent.
 *
 *      FV: Returnerar status.
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-03-02 Moved to own function, R Svedin
 *
 ******************************************************!*/

 {

   func_vp->lit.float_va = RDTODG*(v2float)atan( func_pv[1].par_va.lit.float_va);
    
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evcos()

/*      Evaluate COS Returns the cosine.
 *
 *      FV: Returnerar status.
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-03-02 Moved to own function, R Svedin
 *
 ******************************************************!*/

 {

   func_vp->lit.float_va = (v2float)cos(func_pv[1].par_va.lit.float_va*DGTORD);
    
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evsin()

/*      Evaluate SIN Returns the sine.
 *
 *      FV: Returnerar status.
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-03-02 Moved to own function, R Svedin
 *
 ******************************************************!*/

 {

   func_vp->lit.float_va = (v2float)sin(func_pv[1].par_va.lit.float_va*DGTORD);
    
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evtan()

/*      Evaluate TAN Returns the tangent.
 *
 *      FV: Returnerar status.
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-03-02 Moved to own function, R Svedin
 *
 ******************************************************!*/

 {

   func_vp->lit.float_va = (v2float)tan(func_pv[1].par_va.lit.float_va*DGTORD);
    
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evnlog()

/*      Evaluate LN Returns the natural logarithm.
 *
 *      FV: Returnerar status.
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-03-02 Moved to own function, R Svedin
 *
 ******************************************************!*/

 {

   func_vp->lit.float_va = (v2float)log( func_pv[1].par_va.lit.float_va );
    
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evlogn()

/*      Evaluate LOG Returns the base-10 logarithm.
 *
 *      FV: Returnerar status.
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-03-02 Moved to own function, R Svedin
 *
 ******************************************************!*/

 {

   func_vp->lit.float_va = (v2float)log10( func_pv[1].par_va.lit.float_va );
    
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evsqrt()

/*      Evaluate SQRT Returns the square root.
 *
 *      FV: Returnerar status.
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-03-02 Moved to own function, R Svedin
 *
 ******************************************************!*/

 {

   func_vp->lit.float_va = (v2float)sqrt( func_pv[1].par_va.lit.float_va );
    
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evabs()
     

/*      Evaluate arithmetic function ABS
 *      Returns the absolute (positive) value .
 *
 *      FV:  return - error severity code.
 *
 *      Error codes:  IN 317  missmatch in argument list for arithmetic function
 *                    IN 318  Type missmatch in parameter to         - " -
 *
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-11 Rewritten, R. Svedin
 *      2001-03-05 Global parameters, R Svedin
 *      2003-04-03 Bugfix, Johan/Sören, Örebro University
 *
 ******************************************************!*/

  {
   
   PMPARVA *cur_pv;
   short    cur_pc;
   PMLITVA *cur_vp;
   pm_ptr  *cur_ty;
   pm_ptr   cur_al;					
   pm_ptr   listla;              /* PM-pointer to list node */
   pm_ptr   nextla;              /* PM-pointer next list node */
   pm_ptr   exprla;              /* PM-pointer to expression list */
   short    status;
   PMLITVA  val;                 /* value structure */
   pm_ptr   tyla;                /* and it's type */

 /*
 ***Save global params so that recursive calls to
 ***inevfu() will work correctly.
 */
   cur_pv = func_pv;
   cur_pc = func_pc;
   cur_vp = func_vp;
   cur_ty = func_ty;
   cur_al = func_al;						   				   
/* 
***Get first list node in list
*/
   if ( ( status = pmgfli( func_al, &listla ) ) < -1 )
      {
      return( status );        /* errror */
      }
/*
***Error, missmatch in parameters to arithmetic function 
*/
   if ( listla == (pm_ptr)NULL )
      {  
      return( erpush( "IN3173", "" ) );
      }   
/*
***Get contents of list node
*/
   if ( ( status = pmglin( listla, &nextla, &exprla ) ) < -1 )
      return( status );
/*
***Evaluate expression value 
*/
   if ( ( status = inevev( exprla, &val, &tyla ) ) < -1 )
      return( status );
/*
***Reset global variables.
*/
   func_pv = cur_pv;
   func_pc = cur_pc;
   func_vp = cur_vp;
   func_ty = cur_ty;
   func_al = cur_al;
/*
***if FLOAT
*/
   if ( ineqty( stflop, tyla ) )
      { 
      func_vp->lit.float_va = (v2float)fabs( val.lit.float_va );
      func_vp->lit_type = C_FLO_VA;
      *func_ty = stflop;
      }
/*
***if INT 
*/
   else if ( ineqty( stintp, tyla ) )
      {
      func_vp->lit.int_va = (v2int)abs( val.lit.int_va );
      func_vp->lit_type = C_INT_VA;
      *func_ty = stintp;
      }  
/*
***Error, Type missmatch in parameter for arithmetic function
*/
   else  
      {
      return( erpush( "IN3183", "" ) );
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short evfrac()

/*      Evaluate FRAC Returns the fractional part of an argument.
 *
 *      FV: Returnerar status.
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-03-02 Moved to own function, R Svedin
 *
 ******************************************************!*/
 {
   double p;           /* dummy argument for c-function modf() */

   func_vp->lit.float_va = (v2float)modf( func_pv[1].par_va.lit.float_va, &p );
    
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evroud()

/*      Evaluate ROUND 
 *      Rounds an argument to the nearest (lower or higher) integer.
 *
 *      FV: Returnerar status.
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-03-02 Moved to own function, R Svedin
 *
 ******************************************************!*/

 {

   if ( func_pv[1].par_va.lit.float_va < 0.0 )
        func_vp->lit.int_va = (v2int)( func_pv[1].par_va.lit.float_va - 0.5 );
   else
        func_vp->lit.int_va = (v2int)( func_pv[1].par_va.lit.float_va + 0.5 );
    
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evtrnc()

/*      Evaluate TRUNC.
 *      Returns the integer part of an argument (truncates).
 *
 *      FV: Returnerar status.
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-03-02 Moved to own function, R Svedin
 *
 ******************************************************!*/

 {

   func_vp->lit.int_va = (v2int) func_pv[1].par_va.lit.float_va ;
    
   return(0);
 }

/********************************************************/

#ifndef ANALYZER           /* don't compile if analyser */

/*!******************************************************/

        short evrand()

/*      Evaluate function RANDOM.
 *      Generates a pseudo random number in the range 
 *      0.0 <= number <= 1.0.
 *
 *      FV: Returnerar status.
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *      2001-03-05 Global parameters, R Svedin
 *
 ******************************************************!*/

  {
   int    range;
   double seed;

/*
***SCO/UNIX returnerar slumptal i intervallet från och med 0
***till och med RAND_MAX = 2**15 - 1 = 32767.
***I LINUX är RAND_MAX = 2**31 - 1 = 2147483647. 1996-04-18 JK.
***VAX/VMS returnerar från 0 till och med 2**31 - 1 = 2147483647.
*/
#ifdef UNIX
    range = RAND_MAX;
#endif

/*
***Samma för WIN32.
*/
#ifdef WIN32
    range = RAND_MAX;
#endif

#ifdef VMS
    range = 2147483647;
#endif
/*
***Utan parameter använder vi C-funktionen rand().
***Med parameter använder vi srand(seed) som återställer
***slumptalsgeneratorn till det läge i serien som anges
***av seed.
*/
   if ( func_pc == 1 )
     {
     seed = func_pv[1].par_va.lit.float_va;
     if ( seed < 0.0  ||  seed > 1.0 ) return(erpush("IN5242",""));
     else srand((int)(seed*range));
     }

   func_vp->lit.float_va = (double)rand()/(double)range;

   return(0);
  }

/*!******************************************************/
#endif
