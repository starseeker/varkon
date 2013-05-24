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
/*  Function: varkon_sur_scur_uv                   File: sur970.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Create a scaled UV surface curve. Scale factor is 100.          */
/*                                                                  */
/*                                                                  */
/*  The function allocates memory area for the curve segments.      */
/*  This area must be deallocated by the calling function.          */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1996-05-28   Eliminated compilation warning                     */
/*  1999-12-18   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_scur_uv    Scaled UV curve in the UV plane  */
/*                                                              */
/*------------------------------------------------------------- */

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_gmseg       * Initialize DBSeg                        */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error in varkon_sur_scur_uv  (sur970).   */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus     varkon_sur_scur_uv (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur,       /* Curve                             (ptr) */
   DBSeg   *p_seg,       /* Allocated area for UV segments    (ptr) */
                         /* (curve on a surface)                    */
   DBSeg  **pp_graseg )  /* Allocated area for scaled segment (ptr) */
                         /* (area is allocated in the function)     */
/* Out:                                                             */
/* Rational cubic curve (coefficients) in allocated area pp_graseg    */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  no_seg;        /* No of segments in (UV)  surface curve   */
   DBint  ir;            /* Loop index: Segment in  surface curve   */
/*                                                                  */
/*-----------------------------------------------------------------!*/
   DBint  i_s;           /* Loop index segment record               */
   DBSeg  *parray;       /* Current input  segment            (ptr) */
   DBSeg  *pdummy;       /* Current output segment            (ptr) */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur970 Enter varkon_sur_scur_uv  *Scaled UV curve in UV plane*\n");
fflush(dbgfil(SURPAC)); /* From buffer to file */
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur970 Ptr to UV curve segments p_seg= %d Curve header p_cur= %d\n",
        p_seg, p_cur );
fprintf(dbgfil(SURPAC),
"sur970 p_seg->c0x %f c1x %f c2x %f c3x %f\n",
        p_seg->c0x,p_seg->c1x,p_seg->c2x,p_seg->c3x );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 1. Initiations                                                   */
/* ______________                                                   */
/*                                                                  */
/* Number of segments in the input UV curve:                        */
  no_seg= p_cur->ns_cu;
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur970 Prior to allocation *pp_graseg= %d no_seg= %d\n",
    *pp_graseg, no_seg );
   fflush(dbgfil(SURPAC)); 
   }
#endif

/*!                                                                 */
/* Allocation of memory for no_seg segments.                        */
/* Call of DBcreate_segments.                                       */
/*                                                                 !*/
if((*pp_graseg = DBcreate_segments(no_seg))==NULL)
 {
 sprintf(errbuf, "(alloc)%%varkon_sur_csur_uv (sur970");
 return(varkon_erpush("SU2993",errbuf));
 }

/*!                                                                 */
/* Initialize segment data in pp_graseg.                            */
/* Calls of varkon_ini_gmseg  (sur779).                             */
/*                                                                 !*/

   for (i_s=1; i_s<= no_seg; i_s= i_s+1)
     {
     varkon_ini_gmseg ( *pp_graseg+i_s-1); 
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur970 Start adress for allocated area *pp_graseg= %d no_seg= %d\n",
    *pp_graseg, no_seg );
   fflush(dbgfil(SURPAC)); 
   }
#endif

/*!                                                                 */
/* 2. Create the scaled output UV curve                             */
/* ____________________________________                             */
/*                                                                 !*/

/*!                                                                 */
/*                                                                  */
/* Loop all segments in the curve   ir=0,1,....,no_seg-1            */
/*                                                                 !*/

for ( ir=0; ir<= no_seg-1; ++ir )        /* Start loop UV curve     */
  {

/*!                                                                 */
/*    Let the seg_gra segments be equal to input segments p_seg     */
/*    except that they are scaled with 100.                         */
/*    Let offset be zero and type be CUB_SEG.                       */
/*                                                                 !*/

      pdummy = *pp_graseg + ir;
      parray =  p_seg     + ir;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur970 DBSeg ptr pdummy= %d and parray= %d for segment %d,\n",
   pdummy, parray , ir+1 );
   }
#endif


       parray = p_seg + ir;
       (*pdummy).c0x = 100.0*(*parray).c0x;
       (*pdummy).c1x = 100.0*(*parray).c1x;
       (*pdummy).c2x = 100.0*(*parray).c2x;
       (*pdummy).c3x = 100.0*(*parray).c3x;

       (*pdummy).c0y = 100.0*(*parray).c0y;
       (*pdummy).c1y = 100.0*(*parray).c1y;
       (*pdummy).c2y = 100.0*(*parray).c2y;
       (*pdummy).c3y = 100.0*(*parray).c3y;

       (*pdummy).c0z = 100.0*(*parray).c0z;
       (*pdummy).c1z = 100.0*(*parray).c1z;
       (*pdummy).c2z = 100.0*(*parray).c2z;
       (*pdummy).c3z = 100.0*(*parray).c3z;

       (*pdummy).c0  = (*parray).c0;
       (*pdummy).c1  = (*parray).c1;
       (*pdummy).c2  = (*parray).c2;
       (*pdummy).c3  = (*parray).c3;

       (*pdummy).ofs = 0.0;
       (*pdummy).typ = CUB_SEG;
       (*pdummy).sl  = 0.0;     


  }                                      /* End   loop UV curve     */


/*!                                                                 */
/* 3. Exit                                                          */
/* _______                                                          */
/*                                                                 !*/
 

/*!                                                                 */
/* Number of segments in the output scaled UV curve:                */
   p_cur->nsgr_cu = (short)no_seg;
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur970 Exit varkon_sur_scur_uv No of scaled UV curve segments%d\n",
   no_seg );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/
