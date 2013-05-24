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
/*  Function: varkon_sur_fanga                     File: sur120.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates FANGA surface analysis curves.          */
/*                                                                  */
/*  Remark 1:                                                       */
/*  An output FANGA curve pointer may be NULL, which means          */
/*  that the FANGA calculation failed. This will be the case        */
/*  when for instance the surface strip is a ruled surface.         */
/*  Only the chord angle curve will be output in this case.         */
/*                                                                  */
/*  Remark 2:                                                       */
/*  Note that the calling function must deallocate memory           */
/*  for the created FANGA curves (for curves with pointers          */
/*  not equal to NULL).                                             */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1994-12-03   ivstart= -1 ==> all strips, error messages         */
/*  1995-06-17   Pointers to vectors for directions                 */
/*  1996-10-23   pgm_sur added as argument in sur100                */
/*  1999-12-07   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_fanga      FANGA surface analysis curves    */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_nopatch       * Retrieve number of patches            */
/* varkon_sur_silhouette    * Silhouette curve                      */
/* varkon_cur_2dchord       * Create chord curve                    */
/* DBfree_segments          * Deallocate memory                     */
/* varkon_erpush            * Error message to terminal             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2913 = Isoparameter value U= .. is outside surface limit 1-nu  */
/* SU2753 = Conic strip .. is outside surface limit  1 - nv         */
/* SU2763 = Calculation FANGA only for a surface with a spine       */
/* SU2453 = FANGA Creation of start (end or mid) silhouette failed  */
/* SU2943 = Called function ( ) failed in varkon_sur_fanga (sur120) */
/* SU2993 = Severe program error ( ) in varkon_sur_fanga   (sur120) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus         varkon_sur_fanga (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf   *p_sur,       /* Surface                       (C  ptr)  */
  DBPatch  *p_pat,       /* Allocated area for patch data (C  ptr)  */
  DBptr     pgm_sur,     /* Surface                       (DB ptr)  */
  DBVector *p_sdir1,     /* Start direction for fanga curve         */
  DBVector *p_sdir2,     /* End   direction for fanga curve         */
  DBfloat   ustart,      /* Start U value (spine parameter)         */
  DBfloat   uend,        /* End   U value (spine parameter)         */
  DBint     ivstart,     /* Start V line in the surface (strip no)  */
  DBint     ivend,       /* End   V line in the surface             */
  DBint     f_case,      /* Graphical case (not yet used)           */
  DBCurve  *p_cur,       /*  ...  Temporary  !!!  */
  DBSeg    *p_fseg[6] )  /* FANGA curve segments          (C  ptrs) */

/*  DBCurve *p_fcur[6];*//* FANGA curves TODO             (C  ptrs) */
/* Out:                                                             */
/*        Segment data for six curves in p_fseg(i), i=1,2,3,..,6    */
/*        (arclengths are not calculated for the segments)          */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBVector sdir1;       /* Start direction (equal to p_sdir1)      */
   DBVector sdir2;       /* End   direction (equal to p_sdir1)      */
   DBint    nu;          /* Number of patches in U direction        */
   DBint    nv;          /* Number of patches in V direction        */
   DBVector sdirm;       /* Mid   direction for fanga curve         */
   DBCurve  cur1;        /* Curve 1                                 */
   DBCurve  cur2;        /* Curve 2                                 */
   DBCurve  curm;        /* Curve mid                               */
   DBint    nobranch;    /* Total number of branches for silhoutte  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint  surtype;       /* Type CUB_SUR, RAT_SUR, LFT_SUR  ...     */


  DBfloat  eyedata[9];   /* Silhouette definition data.             */
                         /* For case F_SILH 3-5: Eye point          */
                         /* For case I_SILH 0-2: Eye vector         */
                         /* For case S_SILH 0-2: U curve tangent    */
                         /*                  3 : Start U for spine  */
                         /*                  4 : Start V            */
                         /*                  5 : End   U for spine  */
                         /*                  6 : End   V            */
                         /*                  (All 0: Whole surface) */
                         /* For case F_ISOP 3-5: Eye point          */
                         /*                  6 : Cosine value       */
                         /* For case I_ISOP 0-2: Eye vector         */
                         /*                  6 : Cosine value       */
  DBint    rtype;        /* Computation case                        */
                         /* Eq. 1: Silhouette F_SILH (finite)       */
                         /* Eq. 2: Silhouette I_SILH (infinite)     */
                         /* Eq. 3: Silhouette S_SILH (spine)        */
                         /* Eq. 4: Isophote   F_ISOP (finite)       */
                         /* Eq. 5: Isophote   I_ISOP (infinite)     */
  DBint    acc;          /* Computation accuracy (case)             */
                         /* Eq. 1: Original patch boundary curves   */
                         /* Eq. 2: Double the number of patches     */
                         /* Eq. 3: Triple the number of patches     */
                         /* Eq. 4:  ....                            */
  DBint    cur_no;       /* Requested output curve branch number    */





   DBSeg   *p_geoseg1;   /* Geometric (U,V) curve (silhouette 1 )   */
   DBSeg   *p_geoseg2;   /* Geometric (U,V) curve (silhouette 2 )   */
   DBSeg   *p_geosegm;   /* Geometric (U,V) curve (silhouette m )   */
   DBSeg   *p_choseg;    /* Chord curve R*3 segments                */
   DBSeg   *p_angseg;    /* Angle curve R*3 segments                */
   DBCurve *p_spine;     /* Spine curve                             */
   DBSeg   *p_spiseg;    /* Spine segments                          */
   DBint   status;       /* Error code from called function         */
   char    errbuf[80];   /* String for error message fctn erpush    */

/*!-------------- Flow diagram -------------------------------------*/
/*                                                                  */
/*                _____________________                             */
/*               !                     !                            */
/*               ! varkon_sur_fanga    !                            */
/*               !      (sur120)       !                            */
/*               !_____________________!                            */
/*         _______________!_________________________________        */
/*   _____!_______  ________!_________  ______!_______  ____!_____  */
/*  !             !!                  !!              !!          ! */
/*  !     1       !!        2         !!      3       !!    4     ! */
/*  ! Checks and  !! Spine silhouette !! Fanga curves !!   Exit   ! */
/*  ! initiations !!    curves        !!              !!          ! */
/*  !_____________!!__________________!!______________!!__________! */
/*        !                !                  !             !       */
/*   _____!______   _______!_______     ______!_______   ___!____   */
/*  !            ! !             * !   !            * ! !        !  */
/*  ! _sur_nopat ! !_sur_silhoutte !   ! _cur_2dchord ! ! DBfree_!  */
/*  !            ! !               !   !              ! !segments!  */
/*  !  (sur230)  ! !  (sur100)     !   !   (sur122)   ! !        !  */
/*  !____________! !_______________!   !______________! !________!  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur120 Enter varkon_sur_fanga * FANGA surface analysis curves\n");
fflush(dbgfil(SURPAC)); /* From buffer to file */
}
#endif
 
/*!                                                                 */
/* Initiate output pointers p_fseg to NULL.                         */
/*                                                                 !*/

  p_fseg[0]= NULL; 
  p_fseg[1]= NULL; 
  p_fseg[2]= NULL; 
  p_fseg[3]= NULL; 
  p_fseg[4]= NULL; 
  p_fseg[5]= NULL; 

  p_spine  = NULL;
  p_spiseg = NULL;

/*!                                                                 */
/* Initiate output number of surface graphical segments (ngseg_su)  */
/*                                                                 !*/

  p_sur->ngseg_su[0] = 0;             
  p_sur->ngseg_su[1] = 0;             
  p_sur->ngseg_su[2] = 0;             
  p_sur->ngseg_su[3] = 0;             
  p_sur->ngseg_su[4] = 0;             
  p_sur->ngseg_su[5] = 0;             

/*!                                                                 */
/* Direction vector for mid silhouette curve sdirm                  */
/*                                                                 !*/

  sdir1 = *p_sdir1;      /* 1995-06-17 Snabbaste s{ttet att byta    */
  sdir2 = *p_sdir2;      /* till pekare som indata                  */
  sdirm.x_gm = 0.5*(sdir1.x_gm + sdir2.x_gm);
  sdirm.y_gm = 0.5*(sdir1.y_gm + sdir2.y_gm);
  sdirm.z_gm = 0.5*(sdir1.z_gm + sdir2.z_gm);

/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/* Check that the surface is of type LFT_SUR                        */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);

#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_fanga   (sur120)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


if ( surtype     == LFT_SUR )            /* Check surface type      */
 ;
else 
 {
 sprintf(errbuf, "FANGA%%varkon_sur_fanga");
 return(varkon_erpush("SU2763",errbuf));
 }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur120 Input surface type=%d\n", p_sur->typ_su);
fprintf(dbgfil(SURPAC),
  "sur120 No patches in direction U nu= %d and V nv= %d\n",
           (short)nu,(short)nv);
fprintf(dbgfil(SURPAC),
   "sur120 Calculation case %d \n", (short)f_case );
fprintf(dbgfil(SURPAC),
  "sur120 ustart= %f uend= %f\n",ustart, uend);
fprintf(dbgfil(SURPAC),
  "sur120 ivstart= %d ivend= %d\n",(short)ivstart, (short)ivend);
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Let ivstart=1 and ivend= nv (all strips) if ivstart < 0          */
/* Check strip definition values ivstart and ivend.                 */
/* Check parameter values ustart  and uend.                         */
/*                                                                 !*/

  if ( ivstart < 0 )
    {
    ivstart =  1;
    ivend   = nv;
    }

  if ( ivstart < 1 || ivstart > nv )
    {
    sprintf(errbuf,"%d %% %d (varkon_sur_fanga)",
               (short)ivstart,(short)nv);
    return(varkon_erpush("SU2753",errbuf));
    }
  if ( ivend   < 1 || ivend   > nv )
    {
    sprintf(errbuf,"%d %% %d (varkon_sur_fanga)",
                 (short)ivend  ,(short)nv);
    return(varkon_erpush("SU2753",errbuf));
    }


  if (  ustart > 0 && (ustart < 1 || ustart > nu) )
    {
    sprintf(errbuf,"U= %f %% 1 - %d (varkon_sur_fanga)",
                 ustart,(short)nu);
    return(varkon_erpush("SU2913",errbuf));
    }
  if (  uend   > 0 && (uend   < 1 || uend   > nu) )
    {
    sprintf(errbuf,"U= %f %% 1 - %d (varkon_sur_fanga)",
                 uend  ,(short)nu);
    return(varkon_erpush("SU2913",errbuf));
    }




/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/

/*!                                                                 */
/* 2. Spine silhouette curves                                       */
/* __________________________                                       */
/*                                                                 !*/

/*!                                                                 */
/* Create spine silhouette curve for sdir1.                         */
/* Call of varkon_sur_silhouette (sur100).                          */
/*                                                                 !*/

/* For all segments in the output (U,V) curve:  TABORT              */
/*     i. Let segment subtype (subtyp) be 1                         */
/*    ii. Let DB pointer to surface 1 (spek_gm)  be pgm_sur.        */
/*   iii. Let DB pointer to surface 2 (spek2_gm) be NULL. TABORT    */

   eyedata[0] = sdir1.x_gm;
   eyedata[1] = sdir1.y_gm;
   eyedata[2] = sdir1.z_gm;

   if ( ustart < 0 ) eyedata[3] = 1.0;        
   else              eyedata[3] = ustart;
   eyedata[4] = (DBfloat)ivstart;
   if ( uend   < 0 ) eyedata[5] = (DBfloat)nu;
   else              eyedata[5] = uend;   
   eyedata[6] = (DBfloat)ivend;   

   eyedata[7] = -0.123456789;
   eyedata[8] = -0.123456789;

   rtype  = 3;
   acc    = 3;
/*   acc    = 20; */  /* Tillf{lligt */
   cur_no = 1;

    status = varkon_sur_silhouette
    (p_sur,p_pat,pgm_sur,eyedata,rtype,acc,cur_no,&nobranch,
      &cur1,&p_geoseg1);

#ifdef DEBUG
if(status<0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur120 Failure calculat. silhouette start curve eyedata %f %f %f\n",
             eyedata[0],eyedata[1],eyedata[2]);
fflush(dbgfil(SURPAC)); 
}
}
if(status == 0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur120 Start silhouette calculated eyedata %f %f %f\n",
             eyedata[0],eyedata[1],eyedata[2]);
fflush(dbgfil(SURPAC));
}
}
#endif

    if ( status < 0 )
     {
     if ( p_geoseg1 != NULL ) DBfree_segments(p_geoseg1);
     sprintf(errbuf, "start%%");
     return(varkon_erpush("SU2453",errbuf));
     }

/*!                                                                 */
/* Create spine silhouette curve for sdir2.                         */
/* Call of varkon_sur_silhouette (sur100).                          */
/* For all segments in the output (U,V) curve:                      */
/*     i. Let segment subtype (subtyp) be 1                         */
/*    ii. Let DB pointer to surface 1 (spek_gm)  be pgm_sur.        */
/*   iii. Let DB pointer to surface 2 (spek2_gm) be NULL.           */
/*                                                                  */
/*                                                                 !*/

   eyedata[0] = sdir2.x_gm;
   eyedata[1] = sdir2.y_gm;
   eyedata[2] = sdir2.z_gm;

    status = varkon_sur_silhouette
    (p_sur,p_pat,pgm_sur,eyedata,rtype,acc,cur_no,&nobranch,
      &cur2,&p_geoseg2);

#ifdef DEBUG
if(status<0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur120 Failure calculat. silhouette end   curve eyedata %f %f %f\n",
             eyedata[0],eyedata[1],eyedata[2]);
fflush(dbgfil(SURPAC)); 
}
}
if(status == 0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur120 End   silhouette calculated eyedata %f %f %f\n",
             eyedata[0],eyedata[1],eyedata[2]);
fflush(dbgfil(SURPAC)); 
}
}
#endif

    if ( status < 0 )
     {
     if ( p_geoseg2 != NULL ) DBfree_segments(p_geoseg2);
     sprintf(errbuf,"end%%");
     return(varkon_erpush("SU2453",errbuf));
     }

/*!                                                                 */
/* Create spine silhouette curve for sdirm.                         */
/* Call of varkon_sur_silhouette (sur100).                          */
/* For all segments in the output (U,V) curve:                      */
/*     i. Let segment subtype (subtyp) be 1                         */
/*    ii. Let DB pointer to surface 1 (spek_gm)  be pgm_sur.        */
/*   iii. Let DB pointer to surface 2 (spek2_gm) be NULL.           */
/*                                                                  */
/*                                                                 !*/

   eyedata[0] = sdirm.x_gm;
   eyedata[1] = sdirm.y_gm;
   eyedata[2] = sdirm.z_gm;

    status = varkon_sur_silhouette
    (p_sur,p_pat,pgm_sur,eyedata,rtype,acc,cur_no,&nobranch,
       &curm,&p_geosegm);

#ifdef DEBUG
if(status<0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur120 Failure calculat. silhouette mid   curve eyedata %f %f %f\n",
             eyedata[0],eyedata[1],eyedata[2]);
fflush(dbgfil(SURPAC));
}
}
if(status == 0)
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur120 Mid   silhouette calculated eyedata %f %f %f\n",
             eyedata[0],eyedata[1],eyedata[2]);
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur120 Mid   silhouette curm.ns_cu %d pgm_sur %d\n",
             curm.ns_cu,  (short)pgm_sur );
fflush(dbgfil(SURPAC));
}
}
#endif

    if ( status < 0 )
     {
     if ( p_geosegm != NULL ) DBfree_segments(p_geosegm);
     sprintf(errbuf,"mid%%");
     return(varkon_erpush("SU2453",errbuf));
     }

/*!                                                                 */
/* 3. FANGA curves                                                  */
/* _______________                                                  */
/*                                                                 !*/


/*!                                                                 */
/* Create chord curve sdir1-sdir2   ,i.e output C pointer p_fseg(0) */
/* Call of varkon_cur_2dchord  (sur122).                            */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
   "sur120 &cur1 %d &cur2 %d p_geoseg1 %d p_geoseg2 %d\n", 
    (int)&cur1,(int)&cur2,(int)p_geoseg1,(int)p_geoseg2 );
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
   "sur120 p_cur %d &p_choseg %d &p_angseg %d\n", 
     (int)p_cur ,  (int)&p_choseg ,  (int)&p_angseg );
fflush(dbgfil(SURPAC));
}
#endif

    status = varkon_cur_2dchord  
        (&cur1,p_geoseg1,&cur2,p_geoseg2,p_spine,p_spiseg,sdir1,
          p_cur,&p_choseg, p_cur,&p_angseg);
    if ( status < 0 )
     {
     sprintf(errbuf,                    
     "varkon_sur_silhouette%%varkon_sur_fanga");
     return(varkon_erpush("SU2943",errbuf));
     }

    if ( status == 0 )
        {
        p_fseg[0] = p_choseg;
        p_fseg[3] = p_angseg;
        }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
   "sur120 Chord curve p_fseg[0]= %d p_fseg[3] %d\n", 
     (int)p_fseg[0],(int)p_fseg[3]  );
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/* Create chord curve sdir1-sdirm   ,i.e output C pointer p_fseg(1) */
/* Call of varkon_cur_2dchord  (sur122).                            */
/*                                                                 !*/


    status = varkon_cur_2dchord  
        (&cur1,p_geoseg1,&curm,p_geosegm,p_spine,p_spiseg,sdir1,
         p_cur,&p_choseg, p_cur,&p_angseg);
    if ( status < 0 )
     {
     sprintf(errbuf,                    
     "varkon_sur_silhouette%%varkon_sur_fanga");
     return(varkon_erpush("SU2943",errbuf));
     }

    if ( status == 0 )
        {
        p_fseg[1] = p_choseg;
        p_fseg[4] = p_angseg;
        }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
   "sur120 Chord curve p_fseg[1]= %d p_fseg[4] %d\n", 
          (int)p_fseg[1], (int)p_fseg[4]);
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/* Create chord curve sdirm-sdir2   ,i.e output C pointer p_fseg(3) */
/* Call of varkon_cur_2dchord  (sur122).                            */
/*                                                                 !*/


    status = varkon_cur_2dchord  
        (&curm,p_geosegm,&cur2,p_geoseg2,p_spine,p_spiseg,sdir1,
         p_cur,&p_choseg, p_cur,&p_angseg);
    if ( status < 0 )
     {
     sprintf(errbuf,                    
     "varkon_sur_silhouette%%varkon_sur_fanga");
     return(varkon_erpush("SU2943",errbuf));
     }

    if ( status == 0 )
        {
        p_fseg[2] = p_choseg;
        p_fseg[5] = p_angseg;
        }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
   "sur120 Chord curve p_fseg[2]= %d p_fseg[5] %d\n", 
            (int)p_fseg[2], (int)p_fseg[5] );
fflush(dbgfil(SURPAC));
}
#endif



/*!                                                                 */
/* 4. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/* Deallocate memory for silhouette curves.                         */
/* Calls of DBfree_segments.                                                 */
/*                                                                  */
/* Check that at least one FANGA curve is created                   */
/*                                                                 !*/

#ifdef  DEBUG
/* Programming check for Debug On                                   */
 if ( p_geoseg1 == NULL ||
      p_geoseg2 == NULL ||
      p_geosegm == NULL )
    {
    sprintf(errbuf, "(silhouette ptr)%%varkon_sur_fanga");
    return(varkon_erpush("SU2993",errbuf));
    }
#endif

 DBfree_segments(p_geoseg1);
 DBfree_segments(p_geoseg2);
 DBfree_segments(p_geosegm);

 if (p_fseg[0] == NULL &&
     p_fseg[1] == NULL &&
     p_fseg[2] == NULL &&
     p_fseg[3] == NULL &&
     p_fseg[4] == NULL &&
     p_fseg[5] == NULL    )
{
 sprintf(errbuf, "(no output crv)%%varkon_sur_fanga");
 return(varkon_erpush("SU2993",errbuf));
}

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur120 Exit  *** varkon_sur_fanga   \n");
fflush(dbgfil(SURPAC));
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
