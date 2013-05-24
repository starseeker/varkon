/********************************************************************/
/*                                                                  */
/*  This file is part of the VARKON Geometry Library.               */
/*  URL:  http://varkon.sourceforge.net                             */
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
/*  (C)Microform AB 1984-1999, Gunnar Liden, gunnar@microform.se    */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_sur_epts_mbs                  File: sur921.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Create MBS code for check of table EPOINT.                      */
/*                                                                  */
/*  Author                                                          */
/*                                                                  */
/*  Gunnar Liden                                                    */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-02   Originally written                                 */
/*  1996-05-28   Elimination of compilation warnings                */
/*  1999-11-25   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_epts_mbs   MBS code (lines) of table EPOINT */
/*                                                              */
/*--------------------------------------------------------------*/

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_eval     * Surface evaluation routine                 */
/* varkon_erpush       * Error message to terminal                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxxx failed in varkon_sur_epts        */
/* SU2993 = Severe program error in varkon_sur_epts_mbs (sur921).   */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus   varkon_sur_epts_mbs (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur,       /* Surface                           (ptr) */
   DBPatch *p_pat,       /* Alloc. area for topol. patch data (ptr) */
   EPOINT  *p_etable,    /* Entry/exit point table            (ptr) */
   DBint    no_ep )      /* Number of records in etable             */
/* Out:                                                             */
/*         - None -                                                 */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   ir;           /* Loop index corresp. to EPOINT record    */
   DBfloat x1,y1,z1;     /* Entry/exit point 1                      */
   DBfloat x2,y2,z2;     /* Entry/exit point 2                      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   EVALS  xyz;           /* Coordinates and derivatives             */
                         /* for a point on a surface                */
   DBint  icase;         /* Calculation case for varkon_sur_eval    */
   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint  status;        /* Error code from a called function       */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Create MBS code in file SURPAC.DBG                            */
/* _____________________________________                            */

/*                                                                 !*/


/*!                                                                 */
/* Write LIN_FREE for U,V points                                    */
/*                                                                  */
/* Loop all records in EPOINT table ir=0,1,....,no_ep               */
/*                                                                 !*/
#ifdef DEBUG
for ( ir=1; ir<= no_ep; ++ir )           /* Start loop EPOINT       */
  {

      fprintf(dbgfil(SURPAC),
      "LIN_FREE(#%d,VEC(%f,%f,0),VEC(%f,%f,0));\n", ir,
       (p_etable+ir -1)->u1,
       (p_etable+ir -1)->v1,
       (p_etable+ir -1)->u2,
       (p_etable+ir -1)->v2   );

  }                                      /* End   loop EPOINT       */
#endif
/*!                                                                 */
/* End  all records in EPOINT ir=  1,....,no_ep                     */
/*                                                                 !*/


/*!                                                                 */
/* Write LIN_FREE for R*3 points. Calls of varkon_sur_eval          */
/*                                                                  */
/* Loop all records in EPOINT table ir=0,1,....,no_ep               */
/*                                                                 !*/

for ( ir=1; ir<= no_ep; ++ir )           /* Start loop EPOINT       */
  {

icase=3;

status=varkon_sur_eval
(p_sur,p_pat,icase,(p_etable+ir-1)->u1,(p_etable+ir-1)->v1,&xyz);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_eval%%varkon_sur_epts_mbs");
        return(varkon_erpush("SU2943",errbuf));
        }

      x1 = xyz.r_x;
      y1 = xyz.r_y;
      z1 = xyz.r_z;

status=varkon_sur_eval
(p_sur,p_pat,icase,(p_etable+ir-1)->u2,(p_etable+ir-1)->v2,&xyz);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_eval%%varkon_sur_epts_mbs");
        return(varkon_erpush("SU2943",errbuf));
        }

      x2 = xyz.r_x;
      y2 = xyz.r_y;
      z2 = xyz.r_z;
#ifdef DEBUG
      fprintf(dbgfil(SURPAC),
      "LIN_FREE(#%d,VEC(%f,%f,%f),VEC(%f,%f,%f));\n", ir,
       x1,y1,z1,x2,y2,z2 );
#endif

  }                                      /* End   loop EPOINT       */


/* 3. Exit                                                          */
/* _______                                                          */
 


    return(SUCCED);

} /* End of function                                                */

/********************************************************************/
