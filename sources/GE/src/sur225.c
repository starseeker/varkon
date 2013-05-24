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
/*  Function: varkon_pat_coonseval                 File: sur225.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter ( u,v ) point on a given Coons patch.         */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-10-24   Originally written                                 */
/*  1996-05-28   Declaration of GE107                               */
/*  1999-12-18   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_coonseval  Coons patch evaluation fctn      */
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
/* varkon_pat_norm           * Normal with derivatives              */
/* varkon_erpush             * Error message to terminal            */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxxx failed in varkon_pat_coonseval   */
/* SU2962 = sur225 Surface normal is a zero vector in u= , v=       */
/* SU2993 = Severe program error in varkon_pat_coonseval sur225.    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus     varkon_pat_coonseval (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATB  *p_patb,      /* Current rational patch            (ptr) */
   DBint   icase,        /* Calculation case:                       */
                         /* Eq. 0: Only coordinates                 */
                         /* Eq. 1: Coordinates and dr/du derivative */
                         /* Eq. 2: Coordinates and dr/dv derivative */
                         /* Eq.  : All  derivatives                 */
   DBfloat u_pat,        /* Patch (local) U parameter value         */
   DBfloat v_pat,        /* Patch (local) V parameter value         */
   EVALS   *p_xyz )      /* Coordinates and derivatives       (ptr) */
/* Out:                                                             */
/*       Data to p_xyz                                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBSeg  *p_u0;           /* Limit segment U= 0              (ptr) */
   DBSeg  *p_u1;           /* Limit segment U= 1              (ptr) */
   DBSeg  *p_v0;           /* Limit segment V= 0              (ptr) */
   DBSeg  *p_v1;           /* Limit segment V= 1              (ptr) */
   DBfloat r_x,r_y,r_z;    /* Coordinates          r(u)             */
   DBfloat u_x,u_y,u_z;    /* Tangent             dr/du             */
   DBfloat v_x,v_y,v_z;    /* Tangent             dr/dv             */
   DBfloat u2_x,u2_y,u2_z; /* Second derivative  d2r/du2            */
   DBfloat v2_x,v2_y,v2_z; /* Second derivative  d2r/dv2            */
   DBfloat uv_x,uv_y,uv_z; /* Twist vector       d2r/dudv           */
/*                                                                  */
/*----------------------------------------------------------------- */

   DBfloat  s_l;         /* Local parameter value s                 */
   DBfloat  t_l;         /* Local parameter value t                 */
   DBfloat  out_s0[16];  /* Coordinates and derivatives for rs0     */
   DBfloat  out_s1[16];  /* Coordinates and derivatives for rs1     */
   DBfloat  out_0t[16];  /* Coordinates and derivatives for r0t     */
   DBfloat  out_1t[16];  /* Coordinates and derivatives for r1t     */
   DBVector r00;         /* Corner point s= 0  t= 0                 */
   DBVector r01;         /* Corner point s= 0  t= 1                 */
   DBVector r10;         /* Corner point s= 1  t= 0                 */
   DBVector r11;         /* Corner point s= 1  t= 1                 */

   DBCurve  scur;        /* Curve header for segment (dummy) curve  */
   short    rcode;       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors added    out[9-14]*/
   char     errbuf[80];  /* String for error message fctn erpush    */

   DBint    status;      /* Error code from a called function       */

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

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/*  Check that c_flag is one (1), which is the only implem. case.   */
/*                                                                 !*/

   if ( p_patb->c_flag == 1 )
      {
      ;
      }
   else
      {
      sprintf(errbuf,"c_flag%%varkon_pat_coonseval (sur225)");
      return(varkon_erpush("SU2993",errbuf));
      }

    p_xyz->r_x= F_UNDEF;    
    p_xyz->r_y= F_UNDEF;    
    p_xyz->r_z= F_UNDEF;    

    p_xyz->u_x= F_UNDEF;    
    p_xyz->u_y= F_UNDEF;    
    p_xyz->u_z= F_UNDEF;    

    p_xyz->v_x= F_UNDEF;    
    p_xyz->v_y= F_UNDEF;    
    p_xyz->v_z= F_UNDEF;    

    p_xyz->u2_x= F_UNDEF;    
    p_xyz->u2_y= F_UNDEF;    
    p_xyz->u2_z= F_UNDEF;    

    p_xyz->v2_x= F_UNDEF;    
    p_xyz->v2_y= F_UNDEF;    
    p_xyz->v2_z= F_UNDEF;    

    p_xyz->uv_x= F_UNDEF;    
    p_xyz->uv_y= F_UNDEF;    
    p_xyz->uv_z= F_UNDEF;    

/*!                                                                 */
/*  Segment addresses from GMPATB to p_u0, p_u1, p_v0, p_v1         */
/*                                                                 !*/

p_u0= p_patb->p_seg_r0t;     /* Curve segments for cur_r0t    (ptr) */
p_u1= p_patb->p_seg_r1t;     /* Curve segments for cur_r1t    (ptr) */
p_v0= p_patb->p_seg_rs0;     /* Curve segments for cur_rs0    (ptr) */
p_v1= p_patb->p_seg_rs1;     /* Curve segments for cur_rs1    (ptr) */

/*!                                                                 */
/*  Corner points to local parameters r00, r01, r10 and r11.        */
/*                                                                 !*/

r00 = p_patb->p_r00;         /* Corner point s= 0  t= 0             */
r01 = p_patb->p_r01;         /* Corner point s= 0  t= 1             */
r10 = p_patb->p_r10;         /* Corner point s= 1  t= 0             */
r11 = p_patb->p_r11;         /* Corner point s= 1  t= 1             */


/*!                                                                 */
/* 2. Evaluate the curves for the given parameter values            */
/* _____________________________________________________            */
/*                                                                 !*/

/*!                                                                 */
/* Evaluate coordinates and derivatives for s_l and t_l.            */
/* Calls of varkon_GE107 (GE107).                                 */
/*                                                                 !*/
    rcode = 3;
    scur.hed_cu.type    = CURTYP;
    scur.ns_cu     = 1;      
    scur.plank_cu  = FALSE;      

#ifdef THINKING_IF_
p_patb->e_u0t= e_u0 + 1.0;   /* End   parameter value for cur_r0t   */
p_patb->s_u1t= s_u1 + 1.0;   /* Start parameter value for cur_r1t   */
p_patb->e_u1t= e_u1 + 1.0;   /* End   parameter value for cur_r1t   */
p_patb->e_u0s= e_v0 + 1.0;   /* End   parameter value for cur_r0s   */
p_patb->s_u1s= s_v1 + 1.0;   /* Start parameter value for cur_r1s   */
p_patb->e_u1s= e_v1 + 1.0;   /* End   parameter value for cur_r1s   */
#endif

s_l= p_patb->s_u0s - 1.0 + u_pat*(p_patb->e_u0s-p_patb->s_u0s);

   status=GE107 ((DBAny  *)&scur,p_v0,s_l,rcode,out_s0);
   if (status<0) 
        {
        sprintf(errbuf,"GE107 rs0%%varkon_pat_coonseval (sur225)");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur225 rs0 %15.8f %15.8f %15.8f s_l %10.8f\n",
       out_s0[0], out_s0[1],out_s0[2] ,s_l);
fflush(dbgfil(SURPAC));
}
#endif

s_l= p_patb->s_u1s - 1.0 + u_pat*(p_patb->e_u1s-p_patb->s_u1s);

   status=GE107 ((DBAny  *)&scur,p_v1,s_l,rcode,out_s1);
   if (status<0) 
        {
        sprintf(errbuf,"GE107 rs1%%varkon_pat_coonseval (sur225)");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur225 rs1 %15.8f %15.8f %15.8f s_l %10.8f\n",
       out_s1[0], out_s1[1],out_s1[2] ,s_l);
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

t_l= p_patb->s_u0t - 1.0 + v_pat*(p_patb->e_u0t-p_patb->s_u0t);

   status=GE107 ((DBAny  *)&scur,p_u0,t_l,rcode,out_0t);
   if (status<0) 
        {
        sprintf(errbuf,"GE107 r0t%%varkon_pat_coonseval (sur225)");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur225 r0t %15.8f %15.8f %15.8f t_l %10.8f\n",
       out_0t[0], out_0t[1],out_0t[2] ,t_l);
fflush(dbgfil(SURPAC));
}
#endif

t_l= p_patb->s_u1t - 1.0 + v_pat*(p_patb->e_u1t-p_patb->s_u1t);

   status=GE107 ((DBAny  *)&scur,p_u1,t_l,rcode,out_1t);
   if (status<0) 
        {
        sprintf(errbuf,"GE107 r1t%%varkon_pat_coonseval (sur225)");
        return(varkon_erpush("SU2943",errbuf));
        }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur225 rt1 %15.8f %15.8f %15.8f t_l %10.8f\n",
       out_1t[0], out_1t[1],out_1t[2] ,t_l);
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* 3. Calculate surface coordinates and derivatives                 */
/* _____________________________________________________            */
/*                                                                 !*/

/*!                                                                 */
/*  Local parameter s_l= u_pat and t_l= v_pat                       */
/*                                                                 !*/
    s_l= u_pat;
    t_l= v_pat;

   r_x= (1.0-s_l)*out_0t[0] + s_l*out_1t[0] + 
         out_s0[0]*(1.0-t_l) + out_s1[0]*t_l -  
         ((1.0-s_l)*r00.x_gm + s_l*r10.x_gm)*(1.0-t_l) - 
         ((1.0-s_l)*r01.x_gm + s_l*r11.x_gm)*t_l;      

   r_y= (1.0-s_l)*out_0t[1] + s_l*out_1t[1] + 
         out_s0[1]*(1.0-t_l) + out_s1[1]*t_l -  
         ((1.0-s_l)*r00.y_gm + s_l*r10.y_gm)*(1.0-t_l) - 
         ((1.0-s_l)*r01.y_gm + s_l*r11.y_gm)*t_l;      

   r_z= (1.0-s_l)*out_0t[2] + s_l*out_1t[2] + 
         out_s0[2]*(1.0-t_l) + out_s1[2]*t_l -  
         ((1.0-s_l)*r00.z_gm + s_l*r10.z_gm)*(1.0-t_l) - 
         ((1.0-s_l)*r01.z_gm + s_l*r11.z_gm)*t_l;      


   u_x= -out_0t[0] +out_1t[0] + out_s0[3]*(1.0-t_l)+ out_s1[3]*t_l-    
        (r10.x_gm-r00.x_gm)*(1.0-t_l) - (r11.x_gm-r01.x_gm)*t_l;

   u_y= -out_0t[1] +out_1t[1] + out_s0[4]*(1.0-t_l)+ out_s1[4]*t_l-    
        (r10.y_gm-r00.y_gm)*(1.0-t_l) - (r11.y_gm-r01.y_gm)*t_l;

   u_z= -out_0t[2] +out_1t[2] + out_s0[5]*(1.0-t_l)+ out_s1[5]*t_l-    
        (r10.z_gm-r00.z_gm)*(1.0-t_l) - (r11.z_gm-r01.z_gm)*t_l;


   v_x= (1.0-s_l)*out_0t[3] + s_l*out_1t[3] - out_s0[0] + out_s1[0]  + 
        ((1.0-s_l)*r00.x_gm+s_l*r10.x_gm) - 
        ((1-s_l)*r01.x_gm+s_l*r11.x_gm);

   v_y= (1.0-s_l)*out_0t[4] + s_l*out_1t[4] - out_s0[1] + out_s1[1]  + 
        ((1.0-s_l)*r00.y_gm+s_l*r10.y_gm) - 
        ((1-s_l)*r01.y_gm+s_l*r11.y_gm);

   v_z= (1.0-s_l)*out_0t[5] + s_l*out_1t[5] - out_s0[2] + out_s1[2]  + 
        ((1.0-s_l)*r00.z_gm+s_l*r10.z_gm) - 
        ((1-s_l)*r01.z_gm+s_l*r11.z_gm);

   u2_x=  out_s0[6]*(1.0-t_l)+ out_s1[6]*t_l; 
   u2_y=  out_s0[7]*(1.0-t_l)+ out_s1[7]*t_l; 
   u2_z=  out_s0[8]*(1.0-t_l)+ out_s1[8]*t_l; 

   v2_x= (1.0-s_l)*out_0t[6] + s_l*out_1t[6];
   v2_y= (1.0-s_l)*out_0t[7] + s_l*out_1t[7];
   v2_z= (1.0-s_l)*out_0t[8] + s_l*out_1t[8];

   uv_x= -out_0t[3] + out_1t[3] - out_s0[3] + out_s1[3] -   
           r00.x_gm + r10.x_gm + r01.x_gm - r11.x_gm;

   uv_y= -out_0t[4] + out_1t[4] - out_s0[4] + out_s1[4] -   
           r00.y_gm + r10.y_gm + r01.y_gm - r11.y_gm;

   uv_z= -out_0t[5] + out_1t[5] - out_s0[5] + out_s1[5] -   
           r00.z_gm + r10.z_gm + r01.z_gm - r11.z_gm;

/* 3. Exit                                                          */
/* _______                                                          */
/*                                                                  */


/*!                                                                 */
/* 3. Coordinates and derivatives to output variable p_xyz          */
/* ______________________________________________________           */
/*                                                                  */
/* Coordinates, derivatives to p_xyz                                */
/*                                                                 !*/

    p_xyz->r_x= r_x;
    p_xyz->r_y= r_y;
    p_xyz->r_z= r_z;

    p_xyz->u_x= u_x;
    p_xyz->u_y= u_y;
    p_xyz->u_z= u_z;

    p_xyz->v_x= v_x;
    p_xyz->v_y= v_y;
    p_xyz->v_z= v_z;

    p_xyz->u2_x= u2_x;
    p_xyz->u2_y= u2_y;
    p_xyz->u2_z= u2_z;

    p_xyz->v2_x= v2_x;
    p_xyz->v2_y= v2_y;
    p_xyz->v2_z= v2_z;

    p_xyz->uv_x= uv_x;
    p_xyz->uv_y= uv_y;
    p_xyz->uv_z= uv_z;

/*!                                                                 */
/* 4. Surface normal and surface normal derivatives                 */
/* ________________________________________________                 */
/*                                                                  */
/* Calculate surface normal and derivatives w.r.t u and v.          */
/* Error SU2963 for a zero length surface normal.                   */
/* Call of varkon_pat_norm (sur240).                                */
/*                                                                 !*/

   status=varkon_pat_norm (icase,p_xyz);
   if (status < 0 )
        {
        sprintf(errbuf,"%f%%%f",u_pat,v_pat);
        return(varkon_erpush("SU2962",errbuf));
        }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur225 r_x %15.8f r_y %15.8f r_z %15.8f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur225 u_x %15.8f u_y %15.8f u_z %15.8f \n",
   p_xyz->u_x,p_xyz->u_y,p_xyz->u_z);
  fprintf(dbgfil(SURPAC),
  "sur225 v_x %15.8f v_y %15.8f v_z %15.8f \n",
   p_xyz->v_x,p_xyz->v_y,p_xyz->v_z);
   }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur225 u2_x %15.8f u2_y %15.8f u2_z %15.8f \n",
   p_xyz->u2_x,p_xyz->u2_y,p_xyz->u2_z);
  fprintf(dbgfil(SURPAC),
  "sur225 v2_x %15.8f v2_y %15.8f v2_z %15.8f \n",
   p_xyz->v2_x,p_xyz->v2_y,p_xyz->v2_z);
  fprintf(dbgfil(SURPAC),
  "sur225 uv_x %15.8f uv_y %15.8f uv_z %15.8f \n",
   p_xyz->uv_x,p_xyz->uv_y,p_xyz->uv_z);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur225 Exit x,y,z= %15.8f %15.8f %15.8f\n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/
