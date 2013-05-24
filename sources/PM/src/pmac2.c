/**********************************************************************
*
*    pmac2.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    Internal structure fetching routines
*
*    pmgmod()    ->   Ger en c-pekare till en modul-node.
*    pmgpar()    ->             - " -         parameternod.
*    pmgsta()    ->             - " -         sats-node.
*    pmgexp()    ->             - " -         uttrycks-nod.
*    pmgnpa()    ->             - " -         namnparameter-nod.
*    pmgcon()    ->             - " -         villkors-nod.
*    pmglin()    ->   Ger PM-pekare till en generisk node och en list-nod.
*    pmgfli()    ->   Ger en PM-pekare till första list noden i en lista.
*    pmgstr()    ->   Ger en c_pekare till en textsträng i PM.
*    pmgvec()    ->             - " -      ett vektorvärde i PM.
*    pmgref()    ->             - " -      ett referensvärde i PM.
*
*    Rutinerna får en PM-pekare som indata och gör om den till en c-pekare 
*    till begärd nodstructur.
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
#include "../../DB/include/DBintern.h"
#include "../../IG/include/IG.h"

/********************************************************/
/*!******************************************************/

        short pmgmod(
        pm_ptr pmla,
        PMMONO **retp)

/*      Get module node.
 *      Convert an PM-pointer to an c-pointer pointing on an module node.
 *
 *      In:    pmla   =>  PM-pointer to the module node
 *
 *      Out: **retp   =>  C-pointer to the module node 
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   *retp = (PMMONO *)pmgadr( pmla );
   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmgpar(
        pm_ptr   pmla,
        PMPANO **retp)

/*      Get module param node.
 *      Convert an PM-pointer to an c-pointer pointing on an module param node.
 *
 *      In:    pmla   =>  PM-pointer to the module paramerter node
 *
 *      Out: **retp   =>  C-pointer to the module node 
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  { 
   *retp = (PMPANO *)pmgadr( pmla );
   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmgsta(
        pm_ptr   pmla,
        PMSTNO **retp)

/*      Get statemant node.
 *      Convert PM-pointer to c-pointer pointing on an statement node in PM.
 *
 *      In:    pmla   =>  PM-pointer to the statement node
 *
 *      Out: **retp   =>  C-pointer to the statement node 
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   *retp = (PMSTNO *)pmgadr( pmla );
   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmgexp(
        pm_ptr   pmla,
        PMEXNO **retp)

/*      Get expression node.
 *      Convert PM-pointer to C-pointer pointing on an expression node in PM.
 *
 *      In:    pmla   =>  PM-pointer to the expression node
 *
 *      Out: **retp   =>  C-pointer to the expression node 
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   *retp = (PMEXNO *)pmgadr( pmla );
   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmgnpa(
        pm_ptr   pmla,
        PMNPNO **retp)

/*      Get named param node.
 *      Convert PM-pointer to C-pointer pointing on an named param node.
 *
 *      In:    pmla   =>  PM-pointer to the named param node
 *
 *      Out: **retp   =>  C-pointer to the named param node 
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   *retp = (PMNPNO *)pmgadr( pmla );
   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmgcon(
        pm_ptr   pmla,
        PMCONO **retp)

/*      Get condition node.
 *      Convert PM-pointer to an c-pointer pointing on an condition node.
 *
 *      In:    pmla   =>  PM-pointer to the condition node
 *
 *      Out: **retp   =>  C-pointer to the condition node 
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   *retp = (PMCONO *)pmgadr( pmla );
   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmglin(
        pm_ptr   pmla,
        pm_ptr  *nextla,
        pm_ptr  *genola)

/*      Get list element.
 *      Read list node, return PM address for next list node and the generic node.
 *
 *      In:    pmla    =>  PM-pointer to the list node
 *
 *      Out:  *nextla  =>  PM-pointer to next list node 
 *            *genola  =>  PM-pointer to the generic node 
 *
 *      FV:    return  -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMLINO *np;             /* c-pointer to an list node */

   np = (PMLINO *)pmgadr( pmla );

   *nextla = np->p_nex_li;
   *genola = np->p_no_li;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmgfli(
        pm_ptr   pmla,
        pm_ptr  *retla)

/*      Get first list node.
 *      Returns the first list node from a generic list. 
 *      Header pointer in an tconc-node.
 *
 *      In:    pmla   =>  PM-pointer to the tconc node
 *
 *      Out:  *retla  =>  PM-pointer to first list node in a list 
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMTCNO *np;                  /* c-pointer to an tconc node */

   if ( pmla != (pm_ptr)NULL )
      {
      np = (PMTCNO *)pmgadr( pmla );
      *retla = np->p_head;
      }
   else  
      *retla = (pm_ptr)NULL;    /* error */

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmgstr(
        pm_ptr   pmla,
        char   **retp)

/*      Get string from PM.
 *      Convert a PM-pointer to an c-pointer pointing to a string in PM.
 *
 *      In:    pmla   =>  PM-pointer to the statement node
 *
 *      Out: **retp   =>  C-pointer to the character string 
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   *retp = (char *)pmgadr( pmla );
   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmgvec(
        pm_ptr    pmla,
        PMVECVA **retp)

/*      Get vector value.
 *      Convert a PM-pointer to an c-pointer pointing to an vector in PM.
 *
 *      In:    pmla   =>  PM-pointer to the vector value in PM
 *
 *      Out: **retp   =>  C-pointer to the vector value in PM 
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   *retp = (PMVECVA *)pmgadr( pmla );
   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmgref(
        pm_ptr    pmla,
        PMREFEX **retp)

/*      Get one referens value in chain.
 *      Convert a PM-pointer to an c-pointer pointing to an referens value
 *      struct in PM.
 *
 *      In:    pmla   =>  PM-pointer to one referens value struct in PM
 *
 *      Out: **retp   =>  C-pointer to one referens value struct in PM 
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   *retp = (PMREFEX *)pmgadr( pmla );
   return( 0 );
  }

/********************************************************/
