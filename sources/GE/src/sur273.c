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
/*  Function: varkon_pat_coonscre4                 File: sur273.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates a Coons patch.                             */
/*                                                                  */
/*  Input data is one boundary curve with four corners.             */
/*                                                                  */
/*  Author:  Gunnar Liden                                           */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-05-28   Originally written                                 */
/*  1997-02-07   Elimination of compilation warnings                */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_coonscre4  Coons patch from 1 boundary crv  */
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
/* GE109                * Curve evaluation function                 */
/* varkon_ini_gmpatb    * Initialize GMPATB                         */
/* varkon_comptol       * Retrieve computer tolerance               */
/* varkon_angd          * Angle between vectors (degr.)             */
/* varkon_erinit        * Initial. error message stack              */
/* varkon_errmes        * Warning message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxx failed in varkon_pat_coonscre4    */
/* SU2993 = Severe program error         in varkon_pat_coonscre4    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
    DBstatus    varkon_pat_coonscre4 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur,        /* Composite boundary UV curve       (ptr) */
  DBSeg   *p_seg,        /* Segment data for p_cur            (ptr) */
  DBfloat  c_crit,       /* Criterion for a face boundary corner    */

   GMPATB *p_patb )      /* Coons patch                       (ptr) */

/* Out:                                                             */
/*       Data to p_patb                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    n_seg;       /* Number of segments in boundary curve    */
   DBint    i_seg;       /* Loop index segment in boundary curve    */

   DBVector s_pt;        /* Point   at start of curve segment       */
   DBfloat  e_tan[3];    /* Tangent at end   of curve segment       */
   DBfloat  s_tan[3];    /* Tangent at start of curve segment       */

   DBfloat  angdiff;     /* Angle difference                        */

   DBint    n_corner;    /* Number of corners in the boundary curve */

   DBCurve  cur_u0;      /* Curve U= 0                              */
   DBCurve  cur_u1;      /* Curve U= 1                              */
   DBCurve  cur_v0;      /* Curve V= 0                              */
   DBCurve  cur_v1;      /* Curve V= 1                              */

   DBint    n_u0;        /* Number of segments in cur_u0            */
   DBint    n_u1;        /* Number of segments in cur_u1            */
   DBint    n_v0;        /* Number of segments in cur_v0            */
   DBint    n_v1;        /* Number of segments in cur_v1            */

   DBSeg   *p_u0;        /* Limit segment U= 0                (ptr) */
   DBfloat  s_u0;        /* Start local parameter value             */
   DBfloat  e_u0;        /* End   local parameter value             */
   DBSeg   *p_u1;        /* Limit segment U= 1                (ptr) */
   DBfloat  s_u1;        /* Start local parameter value             */
   DBfloat  e_u1;        /* End   local parameter value             */
   DBSeg   *p_v0;        /* Limit segment V= 0                (ptr) */
   DBfloat  s_v0;        /* Start local parameter value             */
   DBfloat  e_v0;        /* End   local parameter value             */
   DBSeg   *p_v1;        /* Limit segment V= 1                (ptr) */
   DBfloat  s_v1;        /* Start local parameter value             */
   DBfloat  e_v1;        /* End   local parameter value             */
/*----------------------------------------------------------------- */

   EVALC    xyz_c;        /* Point and derivatives GE109            */
   DBfloat  comptol;     /* Computer tolerance (accuracy)           */
   char     errbuf[80];  /* String for error message fctn erpush    */
   DBint    status;      /* Error code from a called function       */

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!----------------- Theory ----------------------------------------*/
/*                                                                  */
/* Refer to the definition of Coons patch (GMPATB in surdef.h)      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur273 Enter*varkon_pat_coonscre4 p_patb %d c_crit %f\n", 
       (int)p_patb, c_crit );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*    ..  no checks implemented ....                                */
/*                                                                 !*/

/*!                                                                 */
/*  Initialization of GMPATB variables.                             */
/*  Call of varkon_ini_gmpatb (sur763).                             */
/*                                                                 !*/

   status= varkon_ini_gmpatb (p_patb);
   if (status<0) 
        {
        sprintf(errbuf,"sur763%%varkon_pat_coonscre4");
        return(varkon_erpush("SU2943",errbuf));
        }

   xyz_c.evltyp   = EVC_DR;  /* Coordinates and first derivatives   */

   p_u0 = NULL;     
   s_u0 = F_UNDEF; 
   e_u0 = F_UNDEF;
   p_u1 = NULL;  
   s_u1 = F_UNDEF;
   e_u1 = F_UNDEF;
   p_v0 = p_seg; 
   s_v0 = F_UNDEF;   
   e_v0 = F_UNDEF;  
   p_v1 = NULL;    
   s_v1 = F_UNDEF;
   e_v1 = F_UNDEF; 
   n_u0 = I_UNDEF;
   n_u1 = I_UNDEF;
   n_v0 = I_UNDEF;
   n_v1 = I_UNDEF;

/*!                                                                 */
/* Retrieve computer tolerance criterion.                           */
/* Call of varkon_comptol (sur753).                                 */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* 2. Find the corner points of the input boundary curve            */
/* _____________________________________________________            */
/*                                                                 !*/

/*!                                                                 */
/* Number of segments from DBCurve                                  */
/*                                                                 !*/

   n_seg  = p_cur->ns_cu;

   n_corner = 0;

/*!                                                                 */
/*  Start loop curve segments i_seg= 1, 2, ...... ,n_seg            */
/*                                                                 !*/

    for ( i_seg = 1 ; i_seg <= n_seg; i_seg = i_seg + 1 )
      {
/*!                                                                 */
/*    Curve evaluation at end of segment i_seg and at start         */
/*    of segment i_seg+1.                                           */
/*    Calls of GE109.                                               */
/*                                                                 !*/

/*    End of current segment i_seg                                  */

      xyz_c.t_global = (DBfloat)i_seg + 1.0 - comptol;

      status = GE109 ((DBAny *)p_cur ,p_seg ,&xyz_c  );
      if (status<0) 
        {
        sprintf(errbuf,"GE109%%sur273");
        return(varkon_erpush("SU2943",errbuf));
        }

      e_tan[0] = xyz_c.drdt.x_gm;
      e_tan[1] = xyz_c.drdt.y_gm;
      e_tan[2] = xyz_c.drdt.z_gm;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur273 i_seg %d xyz_c.t_global %25.15f for point \n",
  (int)i_seg,xyz_c.t_global );
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur273 Boundary curve   point %f %f %f\n", 
xyz_c.r.x_gm, xyz_c.r.y_gm, xyz_c.r.z_gm);
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur273 Boundary curve tangent %f %f %f\n", 
xyz_c.drdt.x_gm, xyz_c.drdt.y_gm, xyz_c.drdt.z_gm);
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur273 Boundary curve segment end   tangent %f %f %f\n", 
e_tan[0], e_tan[1], e_tan[2]);
fflush(dbgfil(SURPAC));
}
#endif

/*    Start of next segment i_seg + 1                               */

      if  ( i_seg  ==  n_seg  )
      xyz_c.t_global = 1.0 + comptol;
      else
      xyz_c.t_global = (DBfloat)i_seg + 1.0 + comptol;

      status = GE109 ((DBAny *)p_cur ,p_seg ,&xyz_c  );
      if (status<0) 
        {
        sprintf(errbuf,"GE109%%sur273");
        return(varkon_erpush("SU2943",errbuf));
        }

      s_pt     = xyz_c.r;
      s_tan[0] = xyz_c.drdt.x_gm;
      s_tan[1] = xyz_c.drdt.y_gm;
      s_tan[2] = xyz_c.drdt.z_gm;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur792 i_seg %d xyz_c.t_global %25.15f for point s_pt\n",
  (int)i_seg,xyz_c.t_global );
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur273 Boundary curve segment start tangent %f %f %f\n", 
s_tan[0], s_tan[1], s_tan[2]);
fflush(dbgfil(SURPAC));
}
#endif

      status = varkon_angd
      (e_tan,s_tan,&angdiff);
      if ( status < 0 ) 
        {
        sprintf(errbuf,"varkon_angd%%sur273");
        return(erpush("SU2943",errbuf));
        }

      if  ( fabs(angdiff) > c_crit )
        {
        n_corner = n_corner + 1;
        if        ( n_corner == 1 )
          {
          p_u0 = p_seg;     
          n_u0 = i_seg;
          s_u0 =     1.0         + comptol;
          e_u0 = (DBfloat)(n_u0+1) - comptol;
          p_patb->p_r01 = s_pt;              /* Corner point for s=0,t=1     */
          }
        else if   ( n_corner == 2 )
          {
          p_v1 = p_seg + n_u0;     
          n_v1 = i_seg - n_u0;
          s_v1 =     1.0         + comptol;
          e_v1 = (DBfloat)(n_v1+1) - comptol; 
          p_patb->p_r11 = s_pt;              /* Corner point for s=1,t=1     */
          }
        else if   ( n_corner == 3 )
          {
          p_u1 = p_seg + n_u0 + n_v1;     
          n_u1 = i_seg - n_u0 - n_v1;
          s_u1 = (DBfloat)(n_u1+1) - comptol;  /* Reversed */
          e_u1 =      1.0        + comptol;  /* Reversed */
          p_patb->p_r10 = s_pt;              /* Corner point for s=1,t=0     */
          }
        else if   ( n_corner == 4 )
          {
          p_v0 = p_seg + n_u0 + n_v1 + n_u1;     
          n_v0 = i_seg - n_u0 - n_v1 - n_u1;
          s_v0 = (DBfloat)(n_v0+1) - comptol;  /* Reversed */
          e_v0 =       1.0       + comptol;  /* Reversed */
          p_patb->p_r00 = s_pt;              /* Corner point for s=0,t=0     */
          }
        else
          {
          ; /* Do nothing, check below !! */
          }
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && fabs(angdiff) > c_crit  )
{
fprintf(dbgfil(SURPAC),
"sur273 n_seg %d i_seg %d angdiff %25.10f n_corner %d\n",
  (int)n_seg, (int)i_seg, angdiff, (int)n_corner);
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/*  End   loop curve segments i_seg= 1, 2, ...... ,n_seg            */
/*                                                                 !*/
     } /* End loop segments */


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur273 p_u0 %d s_u0 %f e_u0 %f n_u0 %d\n",
(int)p_u0,s_u0,e_u0,(int)n_u0);
fprintf(dbgfil(SURPAC),"sur273 p_u1 %d s_u1 %f e_u1 %f n_u1 %d\n",
(int)p_u1,s_u1,e_u1,(int)n_u1);
fprintf(dbgfil(SURPAC),"sur273 p_v0 %d s_v0 %f e_v0 %f n_v0 %d\n",
(int)p_v0,s_v0,e_v0,(int)n_v0);
fprintf(dbgfil(SURPAC),"sur273 p_v1 %d s_v1 %f e_v1 %f n_v1 %d\n",
(int)p_v1,s_v1,e_v1,(int)n_v1);
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

   if  ( n_corner != 4 )
     {
     sprintf(errbuf,"%d%% ",(int)n_corner);
     varkon_erinit();
     return(varkon_erpush("SU5003",errbuf));
     }

/*!                                                                 */
/*  Copy all data from p_cur to cur_u0, cur_u1, cur_v0 and cur_v0.  */
/*  Number of segments and curve arclength (=0) to DBCurve records. */
/*  All addresses (pointers) to DBCurve in GMPATB are egual to p_cur*/
/*                                                                 !*/

V3MOME((char *)(p_cur),(char *)(&cur_u0),sizeof(DBCurve));
V3MOME((char *)(p_cur),(char *)(&cur_u1),sizeof(DBCurve));
V3MOME((char *)(p_cur),(char *)(&cur_v0),sizeof(DBCurve));
V3MOME((char *)(p_cur),(char *)(&cur_v1),sizeof(DBCurve));

cur_u0.ns_cu = (short)n_u0;
cur_u1.ns_cu = (short)n_u1;
cur_v0.ns_cu = (short)n_v0;
cur_v1.ns_cu = (short)n_v1;

cur_u0.al_cu = 0.0;
cur_u1.al_cu = 0.0;
cur_v0.al_cu = 0.0;
cur_v1.al_cu = 0.0;

p_patb->cur_r0t = cur_u0;   /* Curve cur_r0t                (ptr) */
p_patb->cur_r1t = cur_u1;   /* Curve cur_r1t                (ptr) */
p_patb->cur_rs0 = cur_v0;   /* Curve cur_rs0                (ptr) */
p_patb->cur_rs1 = cur_v1;   /* Curve cur_rs1                (ptr) */


/*!                                                                 */
/*  Segment addresses p_u0, p_u1, p_v0, p_v1 to GMPATB              */
/*                                                                 !*/

p_patb->p_seg_r0t = p_u0;    /* Curve segments for cur_r0t    (ptr) */
p_patb->p_seg_r1t = p_u1;    /* Curve segments for cur_r1t    (ptr) */
p_patb->p_seg_rs0 = p_v0;    /* Curve segments for cur_rs0    (ptr) */
p_patb->p_seg_rs1 = p_v1;    /* Curve segments for cur_rs1    (ptr) */

p_patb->s_u0t= s_u0;         /* Start parameter value for cur_r0t   */
p_patb->e_u0t= e_u0;         /* End   parameter value for cur_r0t   */
p_patb->s_u1t= s_u1;         /* Start parameter value for cur_r1t   */
p_patb->e_u1t= e_u1;         /* End   parameter value for cur_r1t   */
p_patb->s_u0s= s_v0;         /* Start parameter value for cur_r0s   */
p_patb->e_u0s= e_v0;         /* End   parameter value for cur_r0s   */
p_patb->s_u1s= s_v1;         /* Start parameter value for cur_r1s   */
p_patb->e_u1s= e_v1;         /* End   parameter value for cur_r1s   */

/*!                                                                 */
/*  Computation case c_flag= 2 to GMPATB                            */
/*                                                                 !*/

p_patb->c_flag= 2;            /* Case flag:                         */
                              /* Eq. 1: Only one segment curves     */
                              /* Eq. 2: Multi-segment    curves     */

/*!                                                                 */
/*  Let offset for the patch be zero                                */
/*                                                                 !*/
    p_patb->ofs_pat = 0.0;        

/* 3. Exit                                                          */
/* _______                                                          */
/*                                                                  */


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur273 Exit*varkon_pat_coonscre4 cur_u0.ns_cu %d \n",
          (int)cur_u0.ns_cu );
fflush(dbgfil(SURPAC)); 
}
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/
