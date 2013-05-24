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
/*  Function: varkon_sur_isoparam    SNAME: sur150 File: sur150.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates an iso-parametric curve on a surface.      */
/*                                                                  */
/*  Memory area is allocated for the UV curve segment coefficients. */
/*  This area must be deallocated by the calling function!          */
/*                                                                  */
/*                                                                  */
/*  TODO Not yet fully programmed. The following is missing:        */
/*  - Handling of NUL_PAT's ?????                                   */
/*                                                                  */
/*  - A U line in LFT_SUR is a planar curve (plank_cu= TRUE)        */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1994-10-31   Isoparameter in only one segment, comptol,         */
/*               CUR data, check position continuity                */
/*  1995-03-02   Initialize pp_seg= NULL                            */
/*  1995-05-16   Comments added                                     */
/*  1995-11-03   comptol for input check and reverse                */
/*  1996-02-01   v2mome --> V3MOME                                  */
/*  1996-02-23   CON_SUR added                                      */
/*  1996-05-12   comptol in order to be within patch.               */
/*  1996-11-03   POL_SUR, P3_SUR, P5_SUR, P7_SUR, P9_SUR, P21_SUR   */
/*  1997-02-09   start_dec, end_dec may be used uninitialized       */ 
/*  1997-03-12   Check of chord lengths                             */ 
/*  1997-12-07   NURB_SUR added , sur363 added                      */ 
/*  1999-12-18   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_isoparam   Isoparametric curve              */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol         * Retrieve computer tolerance             */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* varkon_ini_gmcur       * Initiate DBCurve                        */
/* varkon_ini_gmseg       * Initiate DBSeg                          */
/* GE135                  * Trim a segment (reparam.)               */
/* varkon_cur_uvmap       * Map UV curve                            */
/* varkon_cur_analysis    * Position continuity analysis            */
/* varkon_erpush          * Error message to stack                  */
/* varkon_erinit          * Initial. error message stack            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2893 = Isoparameter curve case .. is not implemented           */
/* SU2903 = Isoparameter start .. > end .. not yet implemented      */
/* SU2913 = Isoparameter value .. is outside surface limits nu nv   */
/* SU2993 = Severe program error ( ) in varkon_sur_isoparam (sur150)*/
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus           varkon_sur_isoparam (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Surface                           (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  char    *iso_c,        /* Iso-parametric case "U" or "V"          */
  DBfloat  iso_v,        /* Iso-parametric value                    */
  DBfloat  start_v_in,   /* Start parametric value for iso-param.   */
  DBfloat  end_v_in,     /* End   parametric value for iso-param.   */
  DBptr    la,           /* Surface adress in Geometry Memory       */
  DBCurve *p_cur,        /* Surface curve                     (ptr) */
  DBSeg  **pp_seg )      /* Alloc. area for UV curve segments (ptr) */

/* Out:                                                             */
/*        Surface curve data to to p_cur and pp_seg. Note that      */
/*        memory area is allocated for the segment data (pp_seg).   */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!New-Page--------------------------------------------------------!*/

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  start_v;     /* Start parametric value for reverse      */
   DBfloat  end_v;       /* End   parametric value for reverse      */
   DBint    f_reverse;   /* Eq. 0: No reverse Eq. 1: Reverse curve  */
   DBint    nu;          /* Number of patches in U direction        */
   DBint    nv;          /* Number of patches in V direction        */
   DBint    surtype;     /* Type CUB_SUR, RAT_SUR, LFT_SUR  ...     */
   DBint    no_seg;      /* Number of segments in the UV curve      */
   DBint    ir;          /* Loop index corresp. to curve segment    */
   DBint    i_start;     /* Start segment (patch)                   */
   DBint    i_end;       /* End   segment (patch)                   */
   DBfloat  start_dec;   /* Decimal part of start_v                 */
   DBfloat  end_dec;     /* Decimal part of end_v                   */
   DBSeg   *p_seg_r_in;  /* Segments which shall be  reversed (ptr) */
   DBSeg   *p_seg_r_out; /* Segments which have been reversed (ptr) */
   DBfloat  uextr[2];    /* Extrapolation parameter values          */
                         /* values for a curve segment              */ 


                         /* For varkon_cur_analysis (sur792):       */
   DBint   c_case;       /* Computation case:                       */
                         /* Eq. 1: Position   only                  */
                         /* Eq. 2: Derivative added                 */
                         /* Eq. 3: Curvature  added                 */
  DBfloat  pcrit;        /* Position criterion < 0 ==> system value */
  DBint    n_p;          /* Number of steps in position             */
  DBint    piseg[SUANA]; /* Segments (end pt) with position steps   */
  DBfloat  dcrit;        /* Derivat. criterion < 0 ==> system value */
  DBint    n_d;          /* Number of steps in derivative           */
  DBint    diseg[SUANA]; /* Segments (end pt) with derivat. steps   */
  DBfloat  ccrit;        /* Radius   criterion < 0 ==> system value */
  DBint    n_c;          /* Number of steps in radius of curvature  */
  DBint    ciseg[SUANA]; /* Segments (end pt) with curvat.  steps   */
  DBint    n_s;          /* Number of too short segments            */
  DBint    siseg[SUANA]; /* Segments with too short chord lengths   */

/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat comptol;       /* Computer tolerance (accuracy)           */
   DBint   i_s;           /* Loop index segment record               */
   DBint   no_alloc;      /* Number of allocated segments            */
   DBSeg   *parray;       /* Pointer                                 */
   DBint   status;        /* Error code from called function         */
   char    errbuf[80];    /* String for error message fctn erpush    */

/*!-------------- Flow diagram -------------------------------------*/
/*                                                                  */
/*                _____________________                             */
/*               !                     !                            */
/*               ! varkon_sur_isoparam !                            */
/*               !      (sur150)       !                            */
/*               !_____________________!                            */
/*         _______________!________________________                 */
/*   _____!_______  ____!______  _____!_____  _____!______          */
/*  !             !!           !!        *  !!            !         */
/*  !     1       !!    2      !!     3     !!     4      !         */
/*  ! Checks and  !!   Loop    !! Create UV !!   Check    !         */
/*  ! initiations !! variables !!  segments !! continuity !         */
/*  !_____________!!___________!!___________!!____________!         */
/*        !____________               !             !               */
/*   _____!______  ____!___      _____!_____  ______!________       */
/*  !            !!        !    !        o  !!               !      */
/*  ! _sur_nopat !!DBcreate!    !  Current  !! _cur_analysis !      */
/*  !  (sur230)  !!segments!    !  segment  !!   (sur792)    !      */
/*  !____________!!________!    !___________!!_______________!      */
/*    ____!_____   ____!_____         !                             */
/*   !          ! !          !        !                             */
/*   !_ini_gmcur! !_ini_gmseg!        !                             */
/*   ! (sur778) ! ! (sur779) !        !                             */
/*   !__________! !__________!        !                             */
/*    _____!____                      !                             */
/*   !          !                     !                             */
/*   ! _comptol !                     !                             */
/*   ! (sur753) !                     !                             */
/*   !__________!                     !                             */
/*                                    !                             */
/*                            ________!___________________          */
/*                   ________!______  ______!______  _____!_______  */
/*                  !               !!             !!             ! */
/*                  ! Start segment !! End segment !! Mid segment ! */
/*                  !_______________!!_____________!!_____________! */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur150 Enter *** varkon_sur_isoparam **Isoparametric curve***\n");
fflush(dbgfil(SURPAC)); 
}
#endif
 
/* Compiler complained ! TODO Investigate !!!!! */
  start_dec = F_UNDEF;
  end_dec   = F_UNDEF;

#ifdef  DEBUG
  start_v     = F_UNDEF;
  end_v       = F_UNDEF;
  f_reverse   = I_UNDEF;
  nu          = I_UNDEF;
  nv          = I_UNDEF;
  surtype     = I_UNDEF;
  no_seg      = I_UNDEF;
  ir          = I_UNDEF;
  i_start     = I_UNDEF;
  i_end       = I_UNDEF;
  p_seg_r_in  = NULL;
  p_seg_r_out = NULL;
  uextr[0]    = F_UNDEF;
  uextr[1]    = F_UNDEF;
#endif


/*!                                                                 */
/* Initialize curve data in p_cur.                                  */
/* Call of varkon_ini_gmcur  (sur778).                              */
/*                                                                 !*/
    varkon_ini_gmcur (p_cur);

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* Initialize pp_seg to NULL. Variable pp_seg is also a flag        */
/* to the calling function. Memory must be deallocated if           */
/* pp_seg not is equal to NULL (also for errors).                   */
/*                                                                 !*/

   *pp_seg = NULL;


/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/* For Debug On: Check that the surface is of type                  */
/*               CUB_SUR, RAT_SUR, LFT_SUR, CON_SUR or MIX_SUR      */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_isoparam (sur150)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


#ifdef DEBUG
if      ( surtype     ==  CUB_SUR ||   /* Check surface type      */
          surtype     ==  RAT_SUR ||           
          surtype     ==  CON_SUR ||           
          surtype     ==  POL_SUR ||           
          surtype     ==   P3_SUR ||           
          surtype     ==   P5_SUR ||           
          surtype     ==   P7_SUR ||           
          surtype     ==   P9_SUR ||           
          surtype     ==  P21_SUR ||           
          surtype     ==  MIX_SUR ||           
          surtype     == NURB_SUR ||           
          surtype     ==  LFT_SUR )            
 ;
else if ( surtype     == FAC_SUR )        /* Facetted surface not OK */
 {
 sprintf(errbuf, "(FAC_SUR)%%varkon_sur_isoparam (sur150");
 return(varkon_erpush("SU2993",errbuf));
 }
 else
 {
 sprintf(errbuf, "(type)%%varkon_sur_isoparam (sur150");
 return(varkon_erpush("SU2993",errbuf));
 }
#endif


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur150 Input surface type=%d\n",
                                (int)p_sur->typ_su);
fprintf(dbgfil(SURPAC),
  "sur150 No patches in direction U nu= %d and V nv= %d\n",
                                (short)nu,(short)nv);
fprintf(dbgfil(SURPAC),
   "sur150 Isoparametric curve %s  = %f start= %f end= %f\n",
             iso_c,iso_v,start_v_in,end_v_in );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Check that iso-parameter curve input data is within surface      */
/* limits.                                                          */
/*                                                                 !*/

 if ( start_v_in >= end_v_in )
    {
    f_reverse = 1;
    start_v =   end_v_in;
    end_v   = start_v_in;
    }
 else
    {
    f_reverse = 0;
    start_v = start_v_in;
    end_v   =   end_v_in;
    }

if ( strcmp(iso_c,"u") == 0 ) iso_c = "U";
if ( strcmp(iso_c,"v") == 0 ) iso_c = "V";

if ( strcmp(iso_c,"U") == 0 )
  {
  if ( iso_v >= 1.0 - comptol && iso_v < 1.0  )
    {
    iso_v = 1.0;
    }
  else if ( iso_v >  (DBfloat)nu + 1.0 && 
            iso_v <= (DBfloat)nu + 1.0 + comptol )
    {
    iso_v = (DBfloat)nu + 1.0;
    }
  else if ( iso_v >= 1 && iso_v <= nu + 1 )
    {
    ;
    }
  else
    {
    sprintf(errbuf,"U= %f%%(0-%d,0-%d)" ,
           iso_v-1.0, (short)nu, (short)nv);
    varkon_erinit();
    return(varkon_erpush("SU2913",errbuf));
    }
  if ( start_v >= 1.0 - comptol && start_v < 1.0  )
    {
    start_v = 1.0;
    }
  else if ( start_v >  (DBfloat)nv + 1.0 && 
            start_v <= (DBfloat)nv + 1.0 + comptol )
    {
    start_v = (DBfloat)nv + 1.0;
    }
  else if ( start_v >= 1 && start_v <= nv + 1 )
    {
    ;
    }
  else
    {
    sprintf(errbuf,"V= %f%%(0-%d,0-%d)" ,
          start_v-1.0, (short)nu, (short)nv);
    varkon_erinit();
    return(varkon_erpush("SU2913",errbuf));
    }
  if ( end_v >= 1.0 - comptol && end_v < 1.0  )
    {
    end_v = 1.0;
    }
  else if ( end_v >  (DBfloat)nv + 1.0 && 
            end_v <= (DBfloat)nv + 1.0 + comptol )
    {
    end_v = (DBfloat)nv + 1.0;
    }
  else if ( end_v >= 1 && end_v <= nv + 1 )
    {
    ;
    }
  else
    {
    sprintf(errbuf,"V= %f%%(0-%d,0-%d)" ,
         end_v-1.0, (short)nu, (short)nv);
    varkon_erinit();
    return(varkon_erpush("SU2913",errbuf));
    }
  }

else if ( strcmp(iso_c,"V") == 0 )
  {
  if ( iso_v >= 1.0 - comptol && iso_v < 1.0  )
    {
    iso_v = 1.0;
    }
  else if ( iso_v >  (DBfloat)nv + 1.0   && 
            iso_v <= (DBfloat)nv + 1.0 + comptol )
    {
    iso_v = (DBfloat)nv + 1.0;
    }
  else if ( iso_v >= 1 && iso_v <= nv + 1 )
    {
    ;
    }
  else
    {
    sprintf(errbuf,"V= %f%%(0-%d,0-%d)" ,
           iso_v-1.0, (short)nu, (short)nv);
    varkon_erinit();
    return(varkon_erpush("SU2913",errbuf));
    }
  if ( start_v >= 1.0 - comptol && start_v < 1.0  )
    {
    start_v = 1.0;
    }
  else if ( start_v >  (DBfloat)nu + 1.0 && 
            start_v <= (DBfloat)nu + 1.0 + comptol )
    {
    start_v = (DBfloat)nu + 1.0;
    }
  else if ( start_v >= 1 && start_v <= nu + 1 )
    {
    ;
    }
  else
    {
    sprintf(errbuf,"U= %f%%(0-%d,0-%d)" ,
              start_v-1.0, (short)nu, (short)nv);
    varkon_erinit();
    return(varkon_erpush("SU2913",errbuf));
    }
  if ( end_v >= 1.0 - comptol && end_v < 1.0  )
    {
    end_v = 1.0;
    }
  else if ( end_v >  (DBfloat)nu + 1.0 && 
            end_v <= (DBfloat)nu + 1.0 + comptol )
    {
    end_v = (DBfloat)nu + 1.0;
    }
  else if ( end_v >= 1 && end_v <= nu + 1 )
    {
    ;
    }
  else
    {
    sprintf(errbuf,"U= %f%%(0-%d,0-%d)" ,
              end_v-1.0, (short)nu, (short)nv);
    varkon_erinit();
    return(varkon_erpush("SU2913",errbuf));
    }
  }
else
  {
  sprintf(errbuf,"  %s  %% " ,iso_c );
  varkon_erinit();
  return(varkon_erpush("SU2893",errbuf));
  }

/*!                                                                 */
/* Dynamic allocation of memory for the curve data (pp_seg).        */
/* Let the size of the memory area be nv (rational segments)        */
/* if iso_c= U and nu (rational segments) if iso_c= V.              */
/* Call of DBcreate_segments.                                       */
/*                                                                 !*/

  if      ( strcmp(iso_c,"U") == 0 )
    {
    if ( f_reverse == 0 ) no_alloc =   nv;   /*                     */
    else                  no_alloc = 2*nv;   /*                     */
    if((*pp_seg = DBcreate_segments(no_alloc))==NULL)
      { 
      sprintf(errbuf, "(alloc)%%varkon_sur_isoparam(sur150");
      return(varkon_erpush("SU2993",errbuf));
      } 
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur150 no_alloc= %d segments nv= %d Start address *pp_seg %d\n",
                    (short)no_alloc, (short)nv, (int)*pp_seg  );
fflush(dbgfil(SURPAC)); 
}
#endif
    }
  else if ( strcmp(iso_c,"V") == 0 )
    {
    if ( f_reverse == 0 ) no_alloc =   nu;   /*                     */
    else                  no_alloc = 2*nu;   /*                     */
    if((*pp_seg = DBcreate_segments(no_alloc))==NULL)
      { 
      sprintf(errbuf, "(alloc)%%varkon_sur_isoparam(sur150");
      return(varkon_erpush("SU2993",errbuf));
      }


/*!                                                                 */
/* Initiate segment data in pp_seg.                                 */
/* Calls of varkon_ini_gmseg  (sur779).                             */
/*                                                                 !*/

   for (i_s=1; i_s<=no_alloc;i_s= i_s+1)
     {
     varkon_ini_gmseg ( *pp_seg+i_s-1);
     }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur150 no_alloc %d segments nu= %d Start address *pp_seg %d \n",
                       (short)no_alloc ,  (short)nu, (int)*pp_seg  );
fflush(dbgfil(SURPAC)); 
}
#endif
    }

/*!                                                                 */
/* 2. Loop variables                                                */
/* _________________                                                */
/*                                                                  */
/* Initiate the number of segments in the UV curve. Let no_seg=0.   */
/* Calculate start and end patch number (i_start and i_end) as      */
/* the integer part of start_v and end_v.                           */
/* Let the "decimal parts" of the isoparameter curve be             */
/* start_dec= start_v-istart and end_dec= end_v-i_end               */
/*                                                                 !*/
/* (special treatment if i_end= nv+1 or nu+1).                      */

   no_seg=0;    

    i_start= (DBint)floor(start_v);
    i_end  = (DBint)floor(  end_v);

   if ( strcmp(iso_c,"U") == 0 )
     {
     start_dec = start_v - (DBfloat)i_start;
     if (i_end   == nv+1 )
        {
        i_end = i_end - 1;
        end_dec  = end_v - (DBfloat)i_end;     
        if ( end_dec > 1.0 + comptol )
            {
            sprintf(errbuf,"v= %f%% varkon_sur_isoparam ",end_dec);
            return(varkon_erpush("SU2993",errbuf));
            }
        }
     else                    
        {
        end_dec  = end_v - (DBfloat)i_end;     
        if ( fabs(end_dec) < comptol )
            {
            i_end = i_end - 1;
            end_dec  = end_v - (DBfloat)i_end;     
            }
        }
   }
   if ( strcmp(iso_c,"V") == 0 )
     {
     start_dec = start_v - (DBfloat)i_start;
     if (i_end   == nu+1 )
        {
        i_end = i_end - 1;
        end_dec  = end_v - (DBfloat)i_end;     
        if ( end_dec > 1.0 + comptol )
            {
            sprintf(errbuf,"u= %f%% varkon_sur_isoparam ",end_dec);
            return(varkon_erpush("SU2993",errbuf));
            }
        }
     else                    
        {
        end_dec  = end_v - (DBfloat)i_end;     
        if ( fabs(end_dec) < comptol )
            {
            i_end = i_end - 1;
            end_dec  = end_v - (DBfloat)i_end;     
            }
        }
   }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur150 i_start %d start_dec= %f \n",
                        (short)i_start, start_dec );
fprintf(dbgfil(SURPAC),"sur150 i_end   %d   end_dec= %f \n",
                        (short)i_end,   end_dec );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 3. Definition of the curve in the UV plane                       */
/* __________________________________________                       */
/*                                                                  */
/* The curve is defined by a straight line in the UV plane.         */
/*                                                                  */
/* The line is defined by a composite rational curve with one       */
/* segment for each patch in the surface and one segment for        */
/* each "decimal part" of the isoparameter curve.                   */
/*                                                                 !*/

/*!                                                                 */
/* Start loop all segments in the curve ir=i_start, ... , i_end     */
/*                                                                 !*/

for ( ir=i_start; ir<= i_end; ++ir )     /* Start loop UV curve     */
  {

      parray = *pp_seg + ir-i_start;

/*!                                                                 */
/*    For a parameter curve in only one patch (i_start=i_end):      */
/*        i. For iso_c= U: Let c0x= iso_v    c1x=   0               */
/*                             c0y= start_v  c1y= end_dec-start_dec */
/*           For iso_c= V: Let c0x= start_v  c1x= end_dec-start_dec */
/*                             c0y= iso_v    c1y=   0               */
/*       ii. Let c0 = 1 and all other coefficients be zero          */
/*      iii. Let offset be zero, type be UV_CUB_SEG and                 */
/*           segment length be zero.                                */
/*       iv. Increase number of output segments (no_seg) with one.  */
/*        v. Goto oneseg.                                           */
/*                                                                 !*/

      if ( i_start == i_end )
         {
         if ( strcmp(iso_c,"U") == 0 )
           {
           (*parray).c0x= iso_v;
           (*parray).c1x= 0.0;
           }
         if ( strcmp(iso_c,"V") == 0 )
           {
           (*parray).c0x= start_v;
           (*parray).c1x= end_dec-start_dec;
           }
         (*parray).c2x= 0.0;
         (*parray).c3x= 0.0;

         if ( strcmp(iso_c,"U") == 0 )
           {
           (*parray).c0y= start_v;
           (*parray).c1y= end_dec-start_dec;
           }
         if ( strcmp(iso_c,"V") == 0 )
           {
           (*parray).c0y= iso_v;
           (*parray).c1y= 0.0;
           }
         (*parray).c2y= 0.0;
         (*parray).c3y= 0.0;

         (*parray).c0z= 0.0;  
         (*parray).c1z= 0.0;
         (*parray).c2z= 0.0;
         (*parray).c3z= 0.0;

         (*parray).c0 = 1.0;  
         (*parray).c1 = 0.0;
         (*parray).c2 = 0.0;
         (*parray).c3 = 0.0;

         (*parray).ofs = 0.0;
         (*parray).typ = UV_CUB_SEG ;
         (*parray).sl  = 0.0;     

         no_seg=no_seg+1;   /* Number of segments of UV curve       */
         goto oneseg;
         }

/*!                                                                 */
/*    Create "start decimal part" of iso-parameter curve.           */
/*    If ir= i_start and start_dec > 0:                             */
/*        i. For iso_c= U: Let c0x= iso_v       c1x=   0            */
/*                             c0y= start_v     c1y= 1-start_dec    */
/*           For iso_c= V: Let c0x= start_v     c1x= 1-start_dec    */
/*                             c0y= iso_v       c1y=   0            */
/*       ii. Let c0 = 1 and all other coefficients be zero          */
/*      iii. Let offset be zero, type be UV_CUB_SEG and                 */
/*           segment length be zero.                                */
/*       iv. Increase number of output segments (no_seg) with one.  */
/*        v. Goto eloop.                                            */
/*                                                                 !*/


      if ( ir==i_start && start_dec > comptol   )
         {
         if ( strcmp(iso_c,"U") == 0 )
           {
           (*parray).c0x= iso_v;
           (*parray).c1x= 0.0;
           }
         if ( strcmp(iso_c,"V") == 0 )
           {
           (*parray).c0x= start_v;
           (*parray).c1x= 1.0-start_dec;
           }
         (*parray).c2x= 0.0;
         (*parray).c3x= 0.0;

         if ( strcmp(iso_c,"U") == 0 )
           {
           (*parray).c0y= start_v;
           (*parray).c1y= 1.0-start_dec;
           }
         if ( strcmp(iso_c,"V") == 0 )
           {
           (*parray).c0y= iso_v;
           (*parray).c1y= 0.0;
           }
         (*parray).c2y= 0.0;
         (*parray).c3y= 0.0;

         (*parray).c0z= 0.0;  
         (*parray).c1z= 0.0;
         (*parray).c2z= 0.0;
         (*parray).c3z= 0.0;

         (*parray).c0 = 1.0;  
         (*parray).c1 = 0.0;
         (*parray).c2 = 0.0;
         (*parray).c3 = 0.0;

         (*parray).ofs = 0.0;
         (*parray).typ = UV_CUB_SEG ;
         (*parray).sl  = 0.0;     

         no_seg=no_seg+1;   /* Number of segments of UV curve       */
         goto eloop;
         }

/*!                                                                 */
/*    Create "end   decimal part" of iso-parameter curve.           */
/*    If ir= i_end   and   end_dec > 0:                             */
/*        i. For iso_c= U: Let c0x= iso_v       c1x=   0            */
/*                             c0y= i_end       c1y= 1-end_dec      */
/*           For iso_c= V: Let c0x= i_end       c1x= 1-end_dec      */
/*                             c0y= iso_v       c1y=   0            */
/*       ii. Let c0 = 1 and all other coefficients be zero          */
/*      iii. Let offset be zero, type be UV_CUB_SEG and                 */
/*           segment length be zero.                                */
/*       iv. Increase number of output segments (no_seg) with one.  */
/*        v. Goto eloop.                                            */
/*                                                                 !*/

      if ( ir==i_end   &&   end_dec > comptol )
         {
         if ( strcmp(iso_c,"U") == 0 )
           {
           (*parray).c0x= iso_v;
           (*parray).c1x= 0.0;
           }
         if ( strcmp(iso_c,"V") == 0 )
           {
           (*parray).c0x= (DBfloat)i_end;
           (*parray).c1x= end_dec;
           }
         (*parray).c2x= 0.0;
         (*parray).c3x= 0.0;

         if ( strcmp(iso_c,"U") == 0 )
           {
           (*parray).c0y= (DBfloat)i_end;
           (*parray).c1y= end_dec;
           }
         if ( strcmp(iso_c,"V") == 0 )
           {
           (*parray).c0y= iso_v;
           (*parray).c1y= 0.0;
           }
         (*parray).c2y= 0.0;
         (*parray).c3y= 0.0;

         (*parray).c0z= 0.0;  
         (*parray).c1z= 0.0;
         (*parray).c2z= 0.0;
         (*parray).c3z= 0.0;

         (*parray).c0 = 1.0;  
         (*parray).c1 = 0.0;
         (*parray).c2 = 0.0;
         (*parray).c3 = 0.0;

         (*parray).ofs = 0.0;
         (*parray).typ = UV_CUB_SEG ;
         (*parray).sl  = 0.0;     

         no_seg=no_seg+1;   /* Number of segments of UV curve       */
         goto eloop;
         }

/*!                                                                 */
/*    Create a "whole patch" segment of the iso-parameter curve:    */
/*        i. Increase number of output segments (no_seg) with one.  */
/*       ii. For iso_c= U: Let c0x= iso_v       c1x=   0            */
/*                             c0y= ir          c1y=   1            */
/*           For iso_c= V: Let c0x= ir          c1x=   1            */
/*                             c0y= iso_v       c1y=   0            */
/*      iii. Let c0 = 1 and all other coefficients be zero          */
/*       iv. Let offset be zero, type be UV_CUB_SEG and                 */
/*           segment length be zero.                                */
/*                                                                 !*/

      no_seg=no_seg+1;   /* Number of segments of UV curve        */

#ifdef DEBUG
      if ( no_seg > nu+nv   )                  /* Program check    */
       {                                       /* for Debug on     */
       sprintf(errbuf,                         /* Error SU2993 for */
       "(no_seg>nu+nv)%%varkon_sur_isoparam"); /* too many segments*/
       return(varkon_erpush("SU2993",errbuf)); /*                  */
       }
#endif

      if ( strcmp(iso_c,"U") == 0 )
        {
        (*parray).c0x= iso_v;
        (*parray).c1x= 0.0;
        }
      if ( strcmp(iso_c,"V") == 0 )
        {
        (*parray).c0x= (DBfloat)ir;
        (*parray).c1x= 1.0 - comptol;
        }
      (*parray).c2x= 0.0;
      (*parray).c3x= 0.0;

      if ( strcmp(iso_c,"U") == 0 )
        {
        (*parray).c0y= (DBfloat)ir;
        (*parray).c1y= 1.0 - comptol;
        }
      if ( strcmp(iso_c,"V") == 0 )
        {
        (*parray).c0y= iso_v;
        (*parray).c1y= 0.0;
        }
      (*parray).c2y= 0.0;
      (*parray).c3y= 0.0;

      (*parray).c0z= 0.0;  
      (*parray).c1z= 0.0;
      (*parray).c2z= 0.0;
      (*parray).c3z= 0.0;

      (*parray).c0 = 1.0;  
      (*parray).c1 = 0.0;
      (*parray).c2 = 0.0;
      (*parray).c3 = 0.0;

      (*parray).ofs = 0.0;
      (*parray).typ = UV_CUB_SEG ;
      (*parray).sl  = 0.0;     


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur150 Segment %d c0x= %f c1x= %f c0y= %f c1y= %f\n",
(short)no_seg,(*parray).c0x,(*parray).c1x,(*parray).c0y,(*parray).c1y);
fprintf(dbgfil(SURPAC),
"sur150 Segment %d parray= %d *pp_seg= %d ir= %d i_start= %d\n",
(short)no_seg,(int)parray,(int)*pp_seg, (short)ir,(short)i_start);
}
#endif

/*!                                                                 */
eloop:; /* Label:  Next segment of curve                            */
/* End   loop all segments in the curve ir=i_start, ... , i_end     */
/*                                                                 !*/
  }                                      /* End   loop UV curve     */

oneseg:; /*! Label oneseg: Only one segment in the curve           !*/

/*!                                                                 */
/* Number of segments no_seg, total arclength (=0) and              */
/* planar flag (FALSE) to p_cur.                                    */
/* Additional data to curve segments:                               */
/* - Subtype for segment                                            */
/* - Geometry Memory (GM) address for the surface (necessary        */
/*   for curve calculations below)                                  */
/*                                                                 !*/

  (p_cur)->ns_cu = (short)no_seg;
  (p_cur)->al_cu = 0.0;        
   p_cur->plank_cu  = FALSE;      
   p_cur->hed_cu.type = CURTYP;

    for ( i_s=0; i_s< no_seg; ++ i_s)
      {
      (*pp_seg+i_s)->subtyp   = 1;
      (*pp_seg+i_s)->spek_gm  = la;
      (*pp_seg+i_s)->spek2_gm = 0;
      }

/*!                                                                 */
/* 4. Reverse curve for case v_start_in > v_end_in                  */
/* _______________________________________________                  */
/*                                                                  */
/* Reverse all segments and copy to area after output curve         */
/* Call of varkon_GE135 (GE135).                                  */
/* Copy all segments in reversed order to output area               */
/*                                                                 !*/

   if ( f_reverse == 0 ) goto norev;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur150 Isoparameter will be reversed start_v_in %f > end_v_in %f\n",
start_v_in, end_v_in );
fflush(dbgfil(SURPAC)); 
}
#endif

   p_seg_r_in = *pp_seg + no_seg;        /* Segments to be reversed */
   p_seg_r_out= *pp_seg;                 /* Reversed segments       */

   uextr[0] = 1.0;
   uextr[1] = 0.0;

   for( i_s= 0; i_s < no_seg; ++i_s )    /* Start loop segments     */
     {

    status = GE135
    (p_seg_r_out+i_s,uextr,p_seg_r_in+i_s);     
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"GE135 (reverse)%%varkon_sur_isoparam (sur150)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur150 Segment %d p_seg_r_out+i_s= %d p_seg_r_in+i_s %d\n",
(short)i_s+1,(int)(p_seg_r_out+i_s),(int)(p_seg_r_in+i_s));
}
#endif
    }  /* End loop reparam. segments */


   for( i_s= 0; i_s < no_seg; ++i_s )    /* Start loop segments     */
     {
     V3MOME((char *)(p_seg_r_in+no_seg-1-i_s),
              (char *)(p_seg_r_out+i_s),sizeof(DBSeg));
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur150 Segment %d c0x= %f c1x= %f c0y= %f c1y= %f\n",
(short)i_s+1,(p_seg_r_out+i_s)->c0x,(p_seg_r_out+i_s)->c1x,
      (p_seg_r_out+i_s)->c0y,(p_seg_r_out+i_s)->c1y);
fprintf(dbgfil(SURPAC),
"sur150 Segment %d p_seg_r_in+no_seg-1-i_s= %d p_seg_r_out+i_s %d\n",
(short)i_s+1,(int)(p_seg_r_in+no_seg-1-i_s),(int)(p_seg_r_out+i_s));
}
#endif

     } /* End loop copy in reversed order */

norev:  /* Label No reverse of curve                                */

/*!                                                                 */
/* 5. Map UV curve to areas defined in the topology patches         */
/* _________________________________________________________        */
/*                                                                  */
/* Call of varkon_cur_uvmap (sur363).                               */
/*                                                                 !*/

    status= varkon_cur_uvmap (p_sur, p_pat, p_cur,*pp_seg );
    if(status<0)
    {
    sprintf(errbuf,"sur363%%sur150");
    return(varkon_erpush("SU2943",errbuf));
    }



/*!                                                                 */
/* 6. Check continuity in position                                  */
/* ________________________________                                 */
/*                                                                  */
/* Analyse the curve with respect to steps in position.             */
/* Call of varkon_cur_analysis (sur792).                            */
/*                                                                 !*/

   c_case = 1;
   pcrit  = -1.0;
   dcrit  = -1.0;
   ccrit  = -1.0;


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur150 Before analysis\n");
fflush(dbgfil(SURPAC)); 
}
#endif

    status= varkon_cur_analysis
    (p_cur,*pp_seg, 1 , no_seg ,c_case, 
      pcrit, &n_p,piseg,dcrit, &n_d, diseg,ccrit, &n_c,ciseg,
             &n_s,siseg);
    if(status<0)
    {
    sprintf(errbuf,"varkon_cur_analysis%%sur150");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur150 After  analysis\n");
fflush(dbgfil(SURPAC)); 
}
#endif
  if ( n_s > 0 )
    {
    sprintf(errbuf,"%d%%sur150",(short)n_s+1);
    varkon_erinit();
    return(varkon_erpush("SU2163",errbuf));
    }

  if ( n_p > 0 )
    {
    sprintf(errbuf,"%d%%varkon_sur_isoparam",(short)n_p+1);
    varkon_erinit();
    return(varkon_erpush("SU2563",errbuf));
    }



/*!                                                                 */
/* 5. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/*                                                                 !*/


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur150 Exit*varkon_sur_isoparam no_seg= %d Start adress *pp_seg %d\n"
            , (short)no_seg,  (int)*pp_seg );
fflush(dbgfil(SURPAC));
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
