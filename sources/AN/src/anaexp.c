/**********************************************************************
*
*    anaexp.c
*    ========
*
*    This file is part of the VARKON Analyzer Library.
*    URL: http://www.varkon.com
*
*    This file includes the following internal routines:
*
*    anaexp();        V3/MBS analyser access function - interactiv Varkon.
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
**********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/AN.h"

struct ANSYREC sy;                 /* scanner interface structure */

/*!******************************************************/

        short anaexp(
        char   *expr,
        bool    tmpref,
        pm_ptr *rptr,
        sttycl *typ)

/*      V3/MBS analyser access function - interactiv V3.
 *
 *      In:  *expr  => The expression string - null term.
 *           tmpref => TRUE - use expression temp. value.
 *
 *      Out: *rptr  => PM-pointer to expr. node.
 *           *typ   => Simple type.
 *
 *      (C)microform ab 1986-04-14 Mats Nelson
 * 
 *       1999-04-15 Rewritten, R. Svedin
 *
 ******************************************************!*/
 
  {
   pm_ptr lptr;                               /* local de. */
   ANFSET set;
   PMLITVA valp;
   pm_ptr valt;
   short status;
   ANATTR attr;                               /* out - expression
                                                 result attributes. */
/*
***Analyse the expression string
*/
   anlogi();                                  /* init error logging */
   status=asinit(expr,ANRDSTR);               /* init. the scanner */
   if ( status != 0)
      return(status);
    ancset(&set,NULL,0,0,0,0,0,0,0,0,0);      /* create empty set */
    anascan (&sy);                            /* get first token */
    pmmark();                                 /* save PM-status */
    anarex(&lptr,&attr,&set );                /* parse arith. expr. */
    asexit();                                 /* exit from scanner */
    if ( sy.sytype != ANSYEOF )               /* at EOF ? */
      anperr("AN9172","<EOF>",NULL,
      sy.sypos.srclin,sy.sypos.srccol);
/*
***Check for syntax/type and runtime error/s
*/
   if ( anyerr() )                            /* syntax/type errors */
     {
     pmrele();                                /* delete incorrect expr. */
     return(erpush("IG2212",""));
     }
   if ( inevex(lptr,&valp,&valt) != 0)        /* runtime errors */
     {
     pmrele();                                /* delete incorrect expr. */
     return(erpush("IG2222",""));
     }
/*
***Prepare output
*/
   switch( tmpref )
     {
     case TRUE:                               /* temp. reference : */
     pmrele();                                /* delete PM-expr. */
     pmclie(&valp,rptr);                      /* PM.. create lit. expr. */
     *typ = angtcl(valt);
     break;

     case FALSE:                              /* fast reference */
     *rptr = lptr;
     *typ = angtcl(attr.type);
     break;
     }

    return(0);
  }

/**********************************************************************/
