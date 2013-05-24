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
/*  Function: varkon_sur_graphic                   File: sur962.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates six isoparametric curves for the           */
/*  graphical representation of a surface.                          */
/*                                                                  */
/*  Remark 1:                                                       */
/*  An output isoparametric curve pointer may be NULL, which        */
/*  means that there will be a graphical representation even        */
/*  if the surface not is OK. The tangent may for instance be       */
/*  a zero vector in one point of the surface. A missing iso-       */
/*  parametric is however an indication of an error. There is       */
/*  a bug in the routine which created the surface!                 */
/*  The function will exit withe error if the calculation           */
/*  failed for all isoparametrics (all pointers equal NULL).        */
/*                                                                  */
/*  Remark 2:                                                       */
/*  Note that the calling function must deallocate memory           */
/*  for the created graphical curves (for curves with               */
/*  pointers not equal to NULL).                                    */
/*                                                                  */
/*  Author: Johan Kjellander & Gunnar Liden                         */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-03-27   Originally written                                 */
/*  1994-10-29   pgm_sur input to sur150                            */
/*  1996-02-23   Deallocate sur150 memory also when sur150 fails    */
/*               CON_SUR added, unused variables                    */
/*  1997-03-07   Temporary fix for LFT_SUR rotation surface         */
/*  1997-03-12   Check of chord lengths replaces temp. fix          */
/*               Check of surface type                              */
/*  1997-11-28   NURB_SUR added                                     */
/*  1997-12-13   Comments                                           */
/*  1999-12-18   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_graphic    Surface graphical representation */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* varkon_sur_isoparam    * Isoparametric (U,V) curve               */
/* varkon_sur_scur_gra    * Graphical     (R*3) curve               */
/* DBfree_segments        * Deallocate memory                       */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_sur_graphic (sur962) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus         varkon_sur_graphic (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Surface                       (C  ptr)  */
  DBPatch *p_pat,        /* Allocated area for patch data (C  ptr)  */
  DBptr    pgm_sur,      /* Surface                       (DB ptr)  */
  DBint    g_case,       /* Graphical case (not yet used)           */
  DBSeg   *p_gseg[6] )   /* Graphical curve segments      (C  ptrs) */

/* Out:                                                             */
/*        Segment data for six curves in p_gseg(i), i=1,2,3,..,6    */
/*        (arclengths are not calculated for the segments)          */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   nu;           /* Number of patches in U direction        */
   DBint   nv;           /* Number of patches in V direction        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint   surtype;      /* Type CUB_SUR, RAT_SUR, LFT_SUR  ...     */
   DBfloat iso_v;        /* Iso-parametric value                    */
   DBfloat start_v;      /* Start parametric value for iso-param.   */
   DBfloat end_v;        /* End   parametric value for iso-param.   */
   DBCurve cur;          /* Curve                                   */
   DBSeg  *p_geoseg;     /* Geometric (U,V) curve (isoparametric)   */
   DBSeg  *p_graseg;     /* Graphical (R*3) curve (isoparametric)   */
   DBint   status;       /* Error code from called function         */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*!-------------- Flow diagram -------------------------------------*/
/*                                                                  */
/*                _____________________                             */
/*               !                     !                            */
/*               ! varkon_sur_graphic  !                            */
/*               !      (sur962)       !                            */
/*               !_____________________!                            */
/*         _______________!________________________ ..._            */
/*   _____!_______  ____!______  _____!_____       _____!______     */
/*  !             !!           !!           !     !            !    */
/*  !     1       !!    2      !!     3     !     !     8      !    */
/*  ! Checks and  !! Curve V=1 !! Curve ... !     !   Exit     !    */
/*  ! initiations !!           !!           !     !            !    */
/*  !_____________!!___________!!___________!     !____________!    */
/*        !              !                                          */
/*   _____!______   _____!_________                                 */
/*  !            ! !               !                                */
/*  ! _sur_nopat ! ! _sur_isoparam !                                */
/*  !  (sur230)  ! !  (sur150)     !                                */
/*  !____________! !_______________!                                */
/*                        !                                         */
/*                  ______!________                                 */
/*                 !            o  !                                */
/*                 ! _sur_scur_gra !                                */
/*                 !   (sur960)    !                                */
/*                 !_______________!                                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur962 Enter varkon_sur_graphic** Surface graphical representation\n");
fflush(dbgfil(SURPAC)); 
}
#endif
 
/*!                                                                 */
/* Initialize output pointers p_gseg to NULL.                       */
/*                                                                 !*/

  p_gseg[0]= NULL; 
  p_gseg[1]= NULL; 
  p_gseg[2]= NULL; 
  p_gseg[3]= NULL; 
  p_gseg[4]= NULL; 
  p_gseg[5]= NULL; 

/*!                                                                 */
/* Initialize output number of surface graphical segments (ngseg_su)*/
/*                                                                 !*/

  p_sur->ngseg_su[0] = 0;             
  p_sur->ngseg_su[1] = 0;             
  p_sur->ngseg_su[2] = 0;             
  p_sur->ngseg_su[3] = 0;             
  p_sur->ngseg_su[4] = 0;             
  p_sur->ngseg_su[5] = 0;             

/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/*               Check that the surface is of type                  */
/*               CUB_SUR, RAT_SUR, LFT_SUR, CON_SUR or MIX_SUR      */
/*               POL_SUR,  P3_SUR,  P7_SUR,  P9_SUR or P21_SUR      */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);

#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_graphic (sur962)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

if ( surtype     ==  CUB_SUR ||       /* Check surface type      */
     surtype     ==  RAT_SUR ||           
     surtype     ==  CON_SUR ||           
     surtype     ==  MIX_SUR ||           
     surtype     ==  POL_SUR ||           
     surtype     ==   P3_SUR ||           
     surtype     ==   P5_SUR ||           
     surtype     ==   P7_SUR ||           
     surtype     ==   P9_SUR ||           
     surtype     ==  P21_SUR ||           
     surtype     == NURB_SUR ||           
     surtype     ==  LFT_SUR )            
 ;
else if ( surtype == FAC_SUR )           /* Facetted surface not OK */
 {
 sprintf(errbuf, "(FAC_SUR)%%sur962");
 return(varkon_erpush("SU2993",errbuf));
 }
else if ( surtype == BOX_SUR )
 {
 sprintf(errbuf, "(BOX_SUR)%%sur962");
 return(varkon_erpush("SU2993",errbuf));
 }
 else
 {
 sprintf(errbuf, "(type)%%varkon_sur_graphic  (sur962");
 return(varkon_erpush("SU2993",errbuf));
 }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur962 Input surface type=%d\n", p_sur->typ_su);
fprintf(dbgfil(SURPAC),
  "sur962 No patches in direction U nu= %d and V nv= %d\n",
                        (int)nu,(int)nv);
fprintf(dbgfil(SURPAC),
   "sur962 Calculation case %d \n", (int)g_case );
fflush(dbgfil(SURPAC)); 
}
#endif



/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* 2. Start V curve                                                 */
/* ________________                                                 */
/*                                                                 !*/

/*!                                                                 */
/* Create isoparametric curve V= 1 (Debug printout for failure).    */
/* Call of varkon_sur_isoparam (sur150).                            */
/* For all segments in the output (U,V) curve:                      */
/*     i. Let segment subtype (subtyp) be 1                         */
/*    ii. Let DB pointer to surface 1 (spek_gm)  be pgm_sur.        */
/*   iii. Let DB pointer to surface 2 (spek2_gm) be NULL.           */
/*                                                                  */
/* Create graphical (R*3) curve, i.e output C pointer p_gseg(0)     */
/* Call of varkon_sur_scur_gra (sur960).                            */
/* Deallocate memory for U,V curve. Call of DBfree_segments.        */
/* Number of segments to surface header (p_sur->ngseg_su(0))        */
/*                                                                 !*/

   iso_v   = 1.0;
   start_v = 1.0;
   end_v   = (DBfloat)nu+1.0;

    status = varkon_sur_isoparam 
    (p_sur,p_pat,"V",iso_v,start_v,end_v,pgm_sur,&cur,&p_geoseg);

#ifdef DEBUG
if(status<0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur962 Failure calculat. isoparametric crv V= %f start= %f end= %f\n",
             iso_v,start_v,end_v );
fflush(dbgfil(SURPAC)); 
}
}
if(status == 0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur962 Iso curve V= %f start= %f end= %f is calculated\n",
             iso_v,start_v,end_v );
fflush(dbgfil(SURPAC)); 
}
}
#endif

    if ( status == 0 )
      {
       status = varkon_sur_scur_gra (&cur,p_geoseg,&p_graseg);
      if ( status == 0 )
        {
        DBfree_segments(p_geoseg);
        p_sur->ngseg_su[0] = cur.nsgr_cu; 
        p_gseg[0] = p_graseg;
        }
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
   "sur962 Graphical curve p_gseg[0]= %d \n", (int)p_gseg[0] );
fflush(dbgfil(SURPAC)); 
}
#endif


/*!                                                                 */
/* 3. End U curve                                                   */
/* ______________                                                   */
/*                                                                 !*/

/*!                                                                 */
/* Create isoparametric curve U= nu+1 (Debug printout for failure). */
/* Call of varkon_sur_isoparam (sur150).                            */
/* For all segments in the output (U,V) curve:                      */
/*     i. Let segment subtype (subtyp) be 1                         */
/*    ii. Let DB pointer to surface 1 (spek_gm)  be pgm_sur.        */
/*   iii. Let DB pointer to surface 2 (spek2_gm) be NULL.           */
/*                                                                  */
/* Create graphical (R*3) curve, i.e output C pointer p_gseg(1)     */
/* Call of varkon_sur_scur_gra (sur960).                            */
/* Deallocate memory for U,V curve. Call of DBfree_segments.        */
/* Number of segments to surface header (p_sur->ngseg_su(1))        */
/*                                                                 !*/

   iso_v   = (DBfloat)nu + 1.0;
   start_v = 1.0;
   end_v   = (DBfloat)nv+1.0;

    status = varkon_sur_isoparam 
    (p_sur,p_pat,"U",iso_v,start_v,end_v,pgm_sur,&cur,&p_geoseg);

#ifdef DEBUG
if(status<0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur962 Failure calculat. isoparametric crv U= %f start= %f end= %f\n",
             iso_v,start_v,end_v );
fflush(dbgfil(SURPAC)); 
}
}
if(status == 0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur962 Iso curve U= %f start= %f end= %f is calculated\n",
             iso_v,start_v,end_v );
fflush(dbgfil(SURPAC)); 
}
}
#endif

    if ( status == 0 )
      {
       status = varkon_sur_scur_gra (&cur,p_geoseg,&p_graseg);
      if ( status == 0 )
        {
        DBfree_segments(p_geoseg);
        p_sur->ngseg_su[1] = cur.nsgr_cu; 
        p_gseg[1] = p_graseg;
        }
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
   "sur962 Graphical curve p_gseg[1]= %d \n", (int)p_gseg[1] );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 4. End V curve                                                   */
/* ______________                                                   */
/*                                                                 !*/

/*!                                                                 */
/* Create isoparametric curve V= nv+1 (Debug printout for failure). */
/* Call of varkon_sur_isoparam (sur150).                            */
/* For all segments in the output (U,V) curve:                      */
/*     i. Let segment subtype (subtyp) be 1                         */
/*    ii. Let DB pointer to surface 1 (spek_gm)  be pgm_sur.        */
/*   iii. Let DB pointer to surface 2 (spek2_gm) be NULL.           */
/*                                                                  */
/* Create graphical (R*3) curve, i.e output C pointer p_gseg(2)     */
/* Call of varkon_sur_scur_gra (sur960).                            */
/* Deallocate memory for U,V curve. Call of DBfree_segments.        */
/* Number of segments to surface header (p_sur->ngseg_su(2))        */
/*                                                                 !*/

   iso_v   = (DBfloat)nv + 1.0;
   start_v = 1.0;
   end_v   = (DBfloat)nu+1.0;

    status = varkon_sur_isoparam 
    (p_sur,p_pat,"V",iso_v,start_v,end_v,pgm_sur,&cur,&p_geoseg);

#ifdef DEBUG
if(status<0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur962 Failure calculat. isoparametric crv V= %f start= %f end= %f\n",
             iso_v,start_v,end_v );
fflush(dbgfil(SURPAC)); 
}
}
if(status == 0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur962 Iso curve V= %f start= %f end= %f is calculated\n",
             iso_v,start_v,end_v );
fflush(dbgfil(SURPAC)); 
}
}
#endif

    if ( status == 0 )
      {
       status = varkon_sur_scur_gra (&cur,p_geoseg,&p_graseg);
      if ( status == 0 )
        {
        DBfree_segments(p_geoseg);
        p_sur->ngseg_su[2] = cur.nsgr_cu; 
        p_gseg[2] = p_graseg;
        }
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
   "sur962 Graphical curve p_gseg[2]= %d \n", (int)p_gseg[2] );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 5. Start U curve                                                 */
/* ________________                                                 */
/*                                                                 !*/

/*!                                                                 */
/* Create isoparametric curve U= 1    (Debug printout for failure). */
/* Call of varkon_sur_isoparam (sur150).                            */
/* For all segments in the output (U,V) curve:                      */
/*     i. Let segment subtype (subtyp) be 1                         */
/*    ii. Let DB pointer to surface 1 (spek_gm)  be pgm_sur.        */
/*   iii. Let DB pointer to surface 2 (spek2_gm) be NULL.           */
/*                                                                  */
/* Create graphical (R*3) curve, i.e output C pointer p_gseg(3)     */
/* Call of varkon_sur_scur_gra (sur960).                            */
/* Deallocate memory for U,V curve. Call of DBfree_segments.        */
/* Number of segments to surface header (p_sur->ngseg_su(3))        */
/*                                                                 !*/

   iso_v   = 1.0;
   start_v = 1.0;
   end_v   = (DBfloat)nv+1.0;

    status = varkon_sur_isoparam 
    (p_sur,p_pat,"U",iso_v,start_v,end_v,pgm_sur,&cur,&p_geoseg);

#ifdef DEBUG
if(status<0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur962 Failure calculat. isoparametric crv U= %f start= %f end= %f\n",
             iso_v,start_v,end_v );
fflush(dbgfil(SURPAC)); 
}
}
if(status == 0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur962 Iso curve U= %f start= %f end= %f is calculated\n",
             iso_v,start_v,end_v );
fflush(dbgfil(SURPAC)); 
}
}
#endif

    if ( status == 0 )
      {
       status = varkon_sur_scur_gra (&cur,p_geoseg,&p_graseg);
      if ( status == 0 )
        {
        DBfree_segments(p_geoseg);
        p_sur->ngseg_su[3] = cur.nsgr_cu; 
        p_gseg[3] = p_graseg;
        }
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
   "sur962 Graphical curve p_gseg[3]= %d \n", (int)p_gseg[3] );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* 6. Mid U curve                                                   */
/* ______________                                                   */
/*                                                                 !*/

/*!                                                                 */
/* Create isoparametric curve U=nu/2+1 (Debug printout for failure) */
/* Call of varkon_sur_isoparam (sur150).                            */
/* For all segments in the output (U,V) curve:                      */
/*     i. Let segment subtype (subtyp) be 1                         */
/*    ii. Let DB pointer to surface 1 (spek_gm)  be pgm_sur.        */
/*   iii. Let DB pointer to surface 2 (spek2_gm) be NULL.           */
/*                                                                  */
/* Create graphical (R*3) curve, i.e output C pointer p_gseg(4)     */
/* Call of varkon_sur_scur_gra (sur960).                            */
/* Deallocate memory for U,V curve. Call of DBfree_segments.        */
/* Number of segments to surface header (p_sur->ngseg_su(4))        */
/*                                                                 !*/

   iso_v   = (DBfloat)nu/2.0 + 1.0;
   start_v = 1.0;
   end_v   = (DBfloat)nv+1.0;

    status = varkon_sur_isoparam 
    (p_sur,p_pat,"U",iso_v,start_v,end_v,pgm_sur,&cur,&p_geoseg);

#ifdef DEBUG
if(status<0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur962 Failure calculat. isoparametric crv U= %f start= %f end= %f\n",
             iso_v,start_v,end_v );
fflush(dbgfil(SURPAC)); 
}
}
if(status == 0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur962 Iso curve U= %f start= %f end= %f is calculated\n",
             iso_v,start_v,end_v );
fflush(dbgfil(SURPAC)); 
}
}
#endif

    if ( status == 0 )
      {
       status = varkon_sur_scur_gra (&cur,p_geoseg,&p_graseg);
      if ( status == 0 )
        {
        DBfree_segments(p_geoseg);
        p_sur->ngseg_su[4] = cur.nsgr_cu; 
        p_gseg[4] = p_graseg;
        }
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
   "sur962 Graphical curve p_gseg[4]= %d \n", (int)p_gseg[4] );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 7. Mid V curve                                                   */
/* ______________                                                   */
/*                                                                 !*/

/*!                                                                 */
/* Create isoparametric curve V=nv/2+1 (Debug printout for failure) */
/* Call of varkon_sur_isoparam (sur150).                            */
/* For all segments in the output (U,V) curve:                      */
/*     i. Let segment subtype (subtyp) be 1                         */
/*    ii. Let DB pointer to surface 1 (spek_gm)  be pgm_sur.        */
/*   iii. Let DB pointer to surface 2 (spek2_gm) be NULL.           */
/*                                                                  */
/* Create graphical (R*3) curve, i.e output C pointer p_gseg(5)     */
/* Call of varkon_sur_scur_gra (sur960).                            */
/* Deallocate memory for U,V curve. Call of DBfree_segments.        */
/* Number of segments to surface header (p_sur->ngseg_su(5))        */
/*                                                                 !*/

   iso_v   = (DBfloat)nv/2.0 + 1.0;
   start_v = 1.0;
   end_v   = (DBfloat)nu+1.0;

    status = varkon_sur_isoparam 
    (p_sur,p_pat,"V",iso_v,start_v,end_v,pgm_sur,&cur,&p_geoseg);

#ifdef DEBUG
if(status<0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur962 Failure calculat. isoparametric crv V= %f start= %f end= %f\n",
             iso_v,start_v,end_v );
fflush(dbgfil(SURPAC)); 
}
}
if(status == 0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur962 Iso curve V= %f start= %f end= %f is calculated\n",
             iso_v,start_v,end_v );
fflush(dbgfil(SURPAC)); 
}
}
#endif

    if ( status == 0 )
      {
       status = varkon_sur_scur_gra (&cur,p_geoseg,&p_graseg);
      if ( status == 0 )
        {
        DBfree_segments(p_geoseg);
        p_sur->ngseg_su[5] = cur.nsgr_cu; 
        p_gseg[5] = p_graseg;
        }
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
   "sur962 Graphical curve p_gseg[5]= %d \n", (int)p_gseg[5] );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 8. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/* Check that at least one graphical curve is created for Debug On  */
/*                                                                 !*/

#ifdef DEBUG                             /* Debug check of output   */
 if (p_gseg[0] == NULL &&
     p_gseg[1] == NULL &&
     p_gseg[2] == NULL &&
     p_gseg[3] == NULL &&
     p_gseg[4] == NULL &&
     p_gseg[5] == NULL    )
{
 sprintf(errbuf, "(no output)%%varkon_sur_graphic (sur962");
 return(varkon_erpush("SU2993",errbuf));
}
#endif

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur962 Exit  *** varkon_sur_graphic \n");
fflush(dbgfil(SURPAC)); 
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
