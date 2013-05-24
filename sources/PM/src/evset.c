/**********************************************************************
*
*    evset.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    short evset()     evaluate procedure SET 
*    short evsetb()    evaluate procedure SET_BASIC
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

extern V2NAPA  defnap;      /* Gloabal area for named parameter values */

extern PMPARVA *proc_pv;    /* inproc.c *pv      Access structure for MBS routines */
extern short    proc_pc;    /* inproc.c parcount Number of actual parameters */
extern pm_ptr   proc_argl;  /* inproc.c arglist  Pekare till aktuell parameterlista */


/*!******************************************************/

        short evset()


/*      Evaluate procedure SET.
 *
 *      In:   extern proc_argl => Pekare till aktuell parameterlista
 *
 *      Out: 
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *      2001-02-12 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   V2NAPA npblock;           /* named parameter block */
   short status;
/*
***evaluate named parameters
*/
   if ( ( status = inevnl( proc_argl, &npblock ) ) < -1 )
      return( status );
/*
***copy new values to "defnap" 
*/
   V3MOME((char *)&npblock,(char *)&defnap,sizeof(V2NAPA));

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short evsetb()  

/*      Evaluate procedure SET_BASIC.
 *
 *      In:   extern proc_argl => Pekare till aktuell parameterlista
 *
 *      Out: 
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1991-06-06 J. Kjellander
 *
 *      1999-11-12 Rewritten, R. Svedin
 *      2001-02-12 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   short status;
/* 
***evaluate named parameters and update defnap + stacked blocks 
*/
   if ( ( status = insetb( proc_argl ) ) < -1 )
      return( status );

   return( 0 );
  }

/********************************************************/
