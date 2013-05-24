/*!******************************************************************
*
*    exsurcur.c 
*    ==========
*
*    EXscur();            SUR_CURVES
*
*    This file is part of the VARKON Execute  Library.
*    URL:  http://varkon.sourceforge.net
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
*    (C)Microform AB 2000-11-01, Gunnar Liden, gunnar@microform.se
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../include/EX.h"

#ifdef DEBUG
#include "../../IG/include/debug.h"
#endif

/******************************************************************!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */

/* varkon_ini_gmsur           * Initialize DBSurf                    */
/* varkon_ini_gmpat           * Initialize DBPatch                    */
/* varkon_sur_bound           * Bounding box & cone for surface     */
/* varkon_ini_gmpatc          * Initialize DBPatch_C                   */
/* varkon_ini_evals           * Initialize EVALS                    */
/* varkon_pat_biccre1         * Geometry to CUB_PAT                 */
/* varkon_comptol             * Retrieve computer tolerance         */
/* varkon_pat_pribic          * Print bicubic patch data For Debug  */
#ifdef  DEBUG
extern short igidst();          /* Convert identity to string       */
#endif
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!******************************************************************/
/*                                                                  */
/*  Function: EXscur                                                */
/*  =================                                               */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Function for the MBS statement SUR_CURVES.                      */
/*                                                                  */
/*  The function SUR_CURVES creates a surface. The input data       */
/*  is a set of curves                                              */
/*                                                                  */
/*                                                                  */
/*  The function varkon_sur_curves allocates memory for the output  */ 
/*  surface.                                                        */
/*                                                                  */
/*  The output surface is stored in the Geometry Memory (GM).       */
/*                                                                  */
/*  Memory for the surface is deallocated.                          */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  2000-11-01   Originally written                                 */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr EXscur                Function corresp. to SUR_CURVES  */
/*                                                              */
/*------------------------------------------------------------- */


/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called   function xxxxxx failed in EXscur               */
/* SU2993 = Severe program error (xxxxxxxx) in EXscur               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/
/*                                                                  */

   short EXscur(
  DBId    *sur_id,       /* Reference (# id.) for output surface    */
  DBint    nu_cur,       /* Requested number of U lines             */
  DBint    nv_cur,       /* Number of input V lines                 */
  DBId    *pVlines,      /* Input curves                            */
  char    *twistmethod,  /* Method for the twist calculation        */
  V2NAPA  *pnp)          /* Attribute data for surface sur_id       */


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

  DBSurf    sur;          /* Surface                                 */
  DBPatch   *p_pat;        /* Allocated area topol. patch data  (ptr) */
  DBCurve  cur[200];/* Input curves                            */
  DBSeg   *seg[200];/* Input segments                          */
  DBptr    la;           /*                                         */
  DBetype  typ;          /*                                         */
  DBint    i;            /* Loop index                              */
  DBint    status;       /* Error code from a called function       */
  char     errbuf[133];  /* Error message string                    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXscur Enter Function corresponding to MBS statement SUR_CURVES\n");
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXccur Input twist method %s\n", twistmethod );
fflush(dbgfil(EXEPAC)); 
}
#endif


/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

   status = 0;


/*!                                                                 */
/* 2. Get the curves                                                */
/* _________________                                                */
/*                                                                  */
/*                                                                  */

   for ( i=0; i<nv_cur; ++i )
     {
     if ( DBget_pointer('I',(pVlines+i),&la,&typ) < 0 ) 
             return(erpush("EX1402",""));
     IGidst((pVlines+i),errbuf); /* Temporary for Debug */
     if ( typ != CURTYP )
       {
       IGidst((pVlines+i),errbuf);
       return(erpush("EX1412",errbuf));
       }
     DBread_curve(&cur[i],NULL,&seg[i],la);
     }



/*!                                                                 */
/* 3. Create the surface                                            */
/* _____________________                                            */
/*                                                                  */
/*                                                                  */     
   status = varkon_sur_curves
          (nu_cur, nv_cur,cur,seg,twistmethod,&sur,&p_pat);

   if      ( status < 0 && p_pat == NULL ) goto error2;
   else if ( status < 0 )                  goto error1;


/*!                                                                 */
/* 3. Output surface                                                */
/* _________________                                                */
/*                                                                  */
/* Draw surface and store in the Geometry Memory (GM).              */
/* Call of EXesur.                                                  */
/*                                                                 !*/


   status = EXesur(sur_id,&sur,p_pat,NULL,NULL,pnp);
   if (status<0) 
     {
     sprintf(errbuf,"EXesur%%EXscur");
     erpush("SU2943",errbuf);
     goto error1;
     }


/*!                                                                 */
/* 8. Exit                                                          */
/* ________                                                         */
/*                                                                  */
/*                                                                 !*/


error1:

/*!                                                                 */
/* Deallocate memory for the output surface.                        */
/* Call of function DBfree_patches.                                 */
/*                                                                 !*/

    DBfree_patches(&sur,p_pat);

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXscur Area for patches has been deallocated \n");
fflush(dbgfil(EXEPAC)); 
}
#endif

error2:

/*!                                                                 */
/* Deallocate memory for the curve segments.                        */
/* Call of function DBfree_segments.                                */
/*                                                                  */
   for ( i=0; i<nv_cur; ++i ) DBfree_segments(seg[i]);


#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC), "EXscur Exit\n");
fflush(dbgfil(EXEPAC));
}
#endif

    return(status);

  }
  
/********************************************************************/
