/********************************************************************/
/*                                                                  */
/*  This file is part of the VARKON Geometry Library.               */
/*  URL:  http://www.varkon.com                                     */
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
/*  Function: varkon_cur_segadr                    File: sur785.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the segment adress for a given          */
/*  global (topological) parameter (t) point on a curve.            */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-10-16   Originally written                                 */
/*  1996-05-28   Declaration of erpush                              */
/*  1997-02-09   Elimination f compiler warning                     */
/*  1999-11-27   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_segadr     Segment adress for given T value */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2953 = Segment iseg= ..  outside curve limit 1 - ...           */
/* SU2993 = Severe program error in varkon_cur_segadr (sur785).     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/
/*                                                                  */
    DBstatus     varkon_cur_segadr (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBfloat  tglob,       /* Global t value                          */
   DBint    nseg,        /* Number of curve segments                */
   DBint   *p_iseg,      /* Curve segment address iseg              */
   DBfloat *p_t )        /* Segment (local) parameter value         */

/* Out:                                                             */
/*       Data to p_iseg and p_t                                     */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat t_seg;        /* Segment local parameter value           */
   DBint   iseg;         /* Curve segment address                   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur785 Enter*varkon_cur_segadr: tglob= %15.10f nseg= %d\n", 
              tglob, (int)nseg  );
  }
#endif

/*!                                                                 */
/* 1. Segment address iseg and local parameter t_seg                */
/* _________________________________________________                */
/*                                                                  */
/*                                                                 !*/
/*  Integer part of tglob                                           */

    iseg= (DBint)floor(tglob);

/*  Check that the segment iseg exists  1 <= iseg <= nseg + 1       */

    if (iseg > nseg + 1 )
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur785 Segment iseg= %d outside definition area nseg= %d \n",
   (int)iseg,(int)nseg);
  }
#endif
        sprintf(errbuf,"%f %% %d varkon_cur_patadr (sur785)"
               ,tglob-1.0,(int)nseg);
        return(varkon_erpush("SU2523",errbuf));
        }


    if (iseg < 0 )
        {
        sprintf(errbuf,"%f %% %d varkon_cur_patadr (sur785)"
               ,tglob-1.0,(int)nseg);
        return(varkon_erpush("SU2523",errbuf));
        }

/*  Calculate local parameter t_seg= decimal part of tglob          */
/*  Note that extrapolation  0< tglob <1 and nseg< tglob <nseg+1    */
/*  is allowed.                                                     */

    if (iseg == nseg+1 )
        {
        iseg = iseg - 1;
        t_seg  = tglob    - (DBfloat)iseg;     
        }

    else if (iseg ==  0   )
        {
        iseg = iseg + 1;
        t_seg  = tglob    - (DBfloat)iseg;     
        }

    else                    
        {
        t_seg  = tglob    - (DBfloat)iseg;     
        }

/*!                                                                 */
/* 2. Data to output variables                                      */
/* ___________________________                                      */
/*                                                                  */
/*                                                                 !*/

   *p_iseg = iseg;
   *p_t    = t_seg;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur785 Local parameter point *p_t %15.10f\n",
   *p_t);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur785 Exit tglob %15.10f *p_iseg %d *p_t %15.10f\n",
   tglob,(int)*p_iseg,*p_t);
  fflush(dbgfil(SURPAC)); 
  }
#endif


    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

