/**********************************************************************
*
*    evstr.c
*    =======
* 
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes the following routines:
*
*    evrstr();   evaluate RSTR
*    evrval();   evaluate RVAL
*    evival();   evaluate IVAL
*    evfval();   evaluate FVAL
*    evfins();   evaluate FINDS
*
*    evchr()     evaluate CHR function
*    evasci()    evaluate ASCII function
*    evstr()     evaluate STR function
*    evleng()    evaluate LENGTH function
*    evsubs()    evaluate SUBSTR function
* 
*    evglor()    evaluate GLOBAL_REF function
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
#include "../../GE/include/GE.h"
#include "../include/indef.h"
#include <ctype.h>

/*
***standard types
*/
extern pm_ptr  stintp;
extern pm_ptr  stflop;
extern pm_ptr  stvecp;
extern pm_ptr  strefp;
extern pm_ptr  ststrp;

/*
***Global variables for arguments
*/
extern PMPARVA *func_pv;
extern short    func_pc;
extern PMLITVA *func_vp;
extern pm_ptr  *func_ty;

#ifndef ANALYZER

/*!******************************************************/

        short evrstr()

/*      Evaluerar funktionen RSTR.
 *
 *      In: Global func_pv  => Pekare till array med parametervärden.
 *          Global func_vp  => Pekare till resultat.
 *
 *      Ut: Global*func_vp  = Referens.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 1/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    return(EXrstr(  &func_pv[1].par_va.lit.ref_va[0],
              (short)func_pv[2].par_va.lit.int_va,
                     func_vp->lit.str_va));

  }

/********************************************************/
/*!******************************************************/

        short evrval()

/*      Evaluerar funktionen RVAL.
 *
 *      In: Global func_pv  => Pekare till array med parametervärden.
 *          Global func_vp  => Pekare till resultat.
 *
 *      Ut: Global*func_vp  = Referens.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 1/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXrval( func_pv[1].par_va.lit.str_va,
                  &func_vp->lit.ref_va[0]));
  }

/********************************************************/
/*!******************************************************/

        short evival()

/*      Evaluerar funktionen IVAL.
 *
 *      In: Global func_pv  => Pekare till array med parametervärden.
 *          Global func_vp  => Pekare till resultat.
 *          Global func_pc  => Antal parametrar.
 *
 *      Ut: Global*func_vp  = Referens.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 1998-09-04 J.Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   int     status,n,tmp;
   char   *s;
   PMLITVA litval;

/*
***Prova att konvertera med sscanf().
*/
   if ( sscanf(func_pv[1].par_va.lit.str_va,"%d",&tmp) == 1 )
     {
     func_vp->lit.int_va = tmp;
     status = 0;
     }
   else
     {
     func_vp->lit.int_va = 0;
     status = -1;
     }
/*
***Returnera ev. status.
*/
   if ( func_pc > 1 )
     {
     litval.lit.int_va = status;
     inwvar(func_pv[2].par_ty, func_pv[2].par_va.lit.adr_va,
             0, NULL, &litval);
     }
/*
***Returnera antal tecken.
*/
   if ( func_pc > 2 )
     {
     n = 0;
     s = func_pv[1].par_va.lit.str_va;
     while ( *s == ' ' ) ++n, ++s;
     if ( *s == '-'  ||  *s == '+' ) ++n, ++s;
     while ( isdigit((int)*s) ) ++n, ++s;
     litval.lit.int_va = n;
     inwvar(func_pv[3].par_ty, func_pv[3].par_va.lit.adr_va,
             0, NULL, &litval);
     }
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short evfval()

/*      Evaluerar funktionen FVAL.
 *
 *      In: Global func_pv  => Pekare till array med parametervärden.
 *          Global func_vp  => Pekare till resultat.
 *          Global func_pc  => Antal parametrar.
 *
 *      Ut: Global*func_vp  = Referens.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 1998-09-04 J.Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   int     status,n;
   char   *s;
   PMLITVA litval;

/*
***Prova att konvertera med sscanf().
*/
   if ( sscanf(  func_pv[1].par_va.lit.str_va,"%lf",
	            &func_vp->lit.float_va) == 1 )
     status = 0;
   else
     {
     func_vp->lit.int_va = 0;
     status = -1;
     }
/*
***Returnera ev. status.
*/
   if ( func_pc > 1 )
     {
     litval.lit.int_va = status;
     inwvar(func_pv[2].par_ty, func_pv[2].par_va.lit.adr_va,
             0, NULL, &litval);
     }
/*
***Returnera antal tecken.
*/
   if ( func_pc > 2 )
     {
     n = 0;
     s = func_pv[1].par_va.lit.str_va;

     while ( *s == ' ' ) ++n, ++s;
     if ( *s == '-'  ||  *s == '+' ) ++n, ++s;
     if ( *s == '.' ) ++n, ++s;
     while ( isdigit((int)*s) ) ++n, ++s;
     if ( *s == '.' ) ++n, ++s;
     while ( isdigit((int)*s) ) ++n, ++s;
     if ( *s == 'e' ||  *s == 'E' )
       if ( *(s+1) == '-'  || *(s+1) == '+'  ||  isdigit(*(s+1)) ) n+=2, s+=2;
     while ( isdigit((int)*s) ) ++n, ++s;

     litval.lit.int_va = n;
     inwvar(func_pv[3].par_ty, func_pv[3].par_va.lit.adr_va,
             0, NULL, &litval);
     }
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short evfins()

/*      Evaluerar funktionen FINDS.
 *
 *      In: Global func_pv  => Pekare till array med parametervärden.
 *          Global func_vp  => Pekare till resultat.
 *
 *      Ut: Global*func_vp  = Referens.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 13/4/87  R. Svedin.
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    short status;
    short i;

    status=(EXfins(func_pv[1].par_va.lit.str_va,
                   func_pv[2].par_va.lit.str_va,&i));

    func_vp->lit.int_va = i;

    return(status);

  }

/********************************************************/
#endif

/*!******************************************************/

        short evchr()

/*      Evaluate function CHR.
 *
 *      In: Global  func_pv  => Parameter value array
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: Return - error severity code 
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   register DBint code;

   code = func_pv[1].par_va.lit.int_va;

/*
***check value for code , >= 0 and < 256 , 880128-JK
*/
   if ( ( code < 0 ) || ( code > 255 ) )
       {  
       return( erpush( "IN2092", "CHR" ) );  /* Ilegal value on partameter to CHR */
       }
/*
***execute CHR , result type STRING
*/
   func_vp->lit.str_va[0] = (char)code;
   func_vp->lit.str_va[1] = '\0';

   return( 0 );
  }  

/********************************************************/
/*!******************************************************/

        short evasci()

/*      Evaluate function ASCII.
 *
 *      In: Global  func_pv  => Parameter value array
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: Return - error severity code 
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
/*
***execute ASCII , result type INT
*/
/*
***unsigned char 1999-03-05, JK
*/
   func_vp->lit.int_va = (DBint)(unsigned char)func_pv[1].par_va.lit.str_va[0];

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short evstr()

/*      Evaluate function STR.
 *
 *      In: Global  func_pv  => Parameter value array
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: Return - error severity code 
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   int format1;                 /*       - " -          */
   int format2;                 /*       - " -          */
   char formstr[ IOFSTRL ];     /* the c:s format string */

   format1 = (int)func_pv[ 2 ].par_va.lit.int_va;
   format2 = (int)func_pv[ 3 ].par_va.lit.int_va;
/*
***make c:s format string
*/
   mkform( C_FLO_VA, format1, format2, formstr );
/*
***execute STR, result type STRING
*/
   sprintf( func_vp->lit.str_va, formstr, func_pv[1].par_va.lit.float_va );

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short evleng()

/*      Evaluate function LENGTH.
 *
 *      In: Global  func_pv  => Parameter value array
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: Return - error severity code 
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
/*
***execute LENGTH , result type INT
*/
   func_vp->lit.int_va = strlen( func_pv[ 1 ].par_va.lit.str_va );

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short evsubs()

/*      Evaluate function SUBSTR.
 *
 *      In: Global  func_pv  => Parameter value array
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: Return - error severity code 
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   register int   nr_char;
   register char *dest,*source;
   register int   sp,sl,nr_poss;

/*
***Se till att startposition >= 1.
*/
   if ( (sp=func_pv[2].par_va.lit.int_va) < 1 ) sp = 1;
/*
***Kolla att startposition ligger inom strängen.
*/
   if ( sp > (sl=strlen(func_pv[1].par_va.lit.str_va)) )
     {
     func_vp->lit.str_va[0] = '\0';
     return(0);
     }
/*
***Beräkna antal tecken att kopiera.
*/
   nr_char = func_pv[3].par_va.lit.int_va;
   nr_poss = sl - sp + 1;

   if ( nr_char > nr_poss || nr_char < 1 ) nr_char = nr_poss;
/*
***Kopiera nr_char tecken.
*/
   dest = func_vp->lit.str_va;
   source = &func_pv[1].par_va.lit.str_va[sp-1];

   for ( ; nr_char>0; ++dest,++source,--nr_char ) *dest = *source;
/*
***Null sist.
*/
   *dest = '\0';

   return(0);
  }  

/********************************************************/
#ifndef ANALYZER
/*!******************************************************/

        short evglor()

/*      Evaluerar funktionen GLOBAL_REF.
 *
 *      In: Global func_pv  => Pekare till array med parametervärden.
 *          Global func_vp  => Pekare till resultat.
 *
 *      Ut: Global*func_vp  = Referens.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 21/12/86 J. Kjellander
 *
 *      2001-03-05 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXglor(  &func_pv[1].par_va.lit.ref_va[0],
              (short)func_pv[2].par_va.lit.int_va,
                    &func_vp->lit.ref_va[0]));
  }

/********************************************************/
#endif
