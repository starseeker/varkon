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
/*  Function: varkon_pat_coonscre1                 File: sur270.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates a Coons patch.                             */
/*                                                                  */
/*  Input data is four boundary curve segments.                     */
/*                                                                  */
/*  Author:  Gunnar Liden                                           */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-10-24   Originally written                                 */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_coonscre1  Coons patch from 4 curve segments*/
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
/* GE107                * Rational curve segment evaluation         */
/* varkon_ini_gmpatb    * Initialize GMPATB variables               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxx failed in varkon_pat_coonscre1    */
/* SU2993 = Severe program error         in varkon_pat_coonscre1    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus   varkon_pat_coonscre1 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSeg    *p_u0,       /* Limit segment U= 0                (ptr) */
   DBfloat   s_u0,       /* Start local parameter value             */
   DBfloat   e_u0,       /* End   local parameter value             */
   DBSeg    *p_u1,       /* Limit segment U= 1                (ptr) */
   DBfloat   s_u1,       /* Start local parameter value             */
   DBfloat   e_u1,       /* End   local parameter value             */
   DBSeg    *p_v0,       /* Limit segment V= 0                (ptr) */
   DBfloat   s_v0,       /* Start local parameter value             */
   DBfloat   e_v0,       /* End   local parameter value             */
   DBSeg    *p_v1,       /* Limit segment V= 1                (ptr) */
   DBfloat   s_v1,       /* Start local parameter value             */
   DBfloat   e_v1,       /* End   local parameter value             */
   GMPATB   *p_patb )    /* Coons patch                       (ptr) */

/* Out:                                                             */
/*       Data to p_patb                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

#ifdef DEBUG
   DBVector r00;         /* Corner point for checking               */
   DBVector r10;         /* Corner point for checking               */
   DBVector r01;         /* Corner point for checking               */
   DBVector r11;         /* Corner point for checking               */
   DBfloat  dist;        /* Distance for checking                   */
#endif
   DBfloat  out[16];     /* Coordinates and derivatives for crv pt  */
   DBCurve  scur;        /* Curve header for segment (dummy) curve  */
   DBfloat  t_l;         /* Local parameter value                   */
   short    rcode;       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors added    out[9-14]*/
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
"sur270 Enter *** varkon_pat_coonscre1 p_patb %d \n", p_patb );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/*    ..  no checks implemented ....                                */
/*                                                                 !*/

/*!                                                                 */
/*  Initialisation of GMPATB variables.                             */
/*  Call of varkon_ini_gmpatb (sur763).                             */
/*                                                                 !*/

   status= varkon_ini_gmpatb (p_patb);
   if (status<0) 
        {
        sprintf(errbuf,"sur763%%varkon_pat_coonscre1");
        return(varkon_erpush("SU2943",errbuf));
        }

/*!                                                                 */
/*  Segment addresses p_u0, p_u1, p_v0, p_v1 to GMPATB              */
/*  (DBCurve are initialised to NULL)                               */
/*                                                                 !*/
p_patb->p_seg_r0t = p_u0;    /* Curve segments for cur_r0t    (ptr) */
p_patb->p_seg_r1t = p_u1;    /* Curve segments for cur_r1t    (ptr) */
p_patb->p_seg_rs0 = p_v0;    /* Curve segments for cur_rs0    (ptr) */
p_patb->p_seg_rs1 = p_v1;    /* Curve segments for cur_rs1    (ptr) */

p_patb->s_u0t= s_u0 + 1.0;   /* Start parameter value for cur_r0t   */
p_patb->e_u0t= e_u0 + 1.0;   /* End   parameter value for cur_r0t   */
p_patb->s_u1t= s_u1 + 1.0;   /* Start parameter value for cur_r1t   */
p_patb->e_u1t= e_u1 + 1.0;   /* End   parameter value for cur_r1t   */
p_patb->s_u0s= s_v0 + 1.0;   /* Start parameter value for cur_r0s   */
p_patb->e_u0s= e_v0 + 1.0;   /* End   parameter value for cur_r0s   */
p_patb->s_u1s= s_v1 + 1.0;   /* Start parameter value for cur_r1s   */
p_patb->e_u1s= e_v1 + 1.0;   /* End   parameter value for cur_r1s   */

/*!                                                                 */
/*  Computation case c_flag= 1 to GMPATB                            */
/*  (DBCurve are initialised to NULL)                               */
/*                                                                 !*/

p_patb->c_flag= 1;            /* Case flag:                         */
                              /* Eq. 1: Only one segment curves     */

/*!                                                                 */
/*  Let offset for the patch be zero                                */
/*                                                                 !*/
    p_patb->ofs_pat = 0.0;        

/*!                                                                 */
/* 2. Calculate corner points                                       */
/* __________________________                                       */
/*                                                                 !*/

/*!                                                                 */
/* Evaluate coordinates for segment p_u0 and p_u1                   */
/* Calls of GE107.                                                  */
/* Corner points to GMPATB                                          */
/*                                                                 !*/
    rcode = 3;
    scur.hed_cu.type    = CURTYP;
    scur.ns_cu     = 1;      
    scur.plank_cu  = FALSE;      


    t_l = s_u0;
   status=GE107 ((DBAny  *)&scur,p_u0,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107 r00 %%varkon_pat_coonscre1 (sur270)");
        return(varkon_erpush("SU2943",errbuf));
        }

p_patb->p_r00.x_gm= out[0];      /* Corner point        for s=0,t=0 */
p_patb->p_r00.y_gm= out[1];      /*                                 */
p_patb->p_r00.z_gm= out[2];      /*                                 */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur270 r00 %15.8f %15.8f %15.8f u0 %9.6f\n",
p_patb->p_r00.x_gm,p_patb->p_r00.y_gm,p_patb->p_r00.z_gm,t_l);
fflush(dbgfil(SURPAC)); 
}
#endif

    t_l = e_u0; 
   status=GE107 ((DBAny  *)&scur,p_u0,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107 r01 %%varkon_pat_coonscre1 (sur270)");
        return(varkon_erpush("SU2943",errbuf));
        }

p_patb->p_r01.x_gm= out[0];      /* Corner point        for s=0,t=1 */
p_patb->p_r01.y_gm= out[1];      /*                                 */
p_patb->p_r01.z_gm= out[2];      /*                                 */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur270 r01 %15.8f %15.8f %15.8f u0 %9.6f\n",
p_patb->p_r01.x_gm,p_patb->p_r01.y_gm,p_patb->p_r01.z_gm,t_l);
fflush(dbgfil(SURPAC)); 
}
#endif


    t_l = s_u1;
   status=GE107 ((DBAny  *)&scur,p_u1,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107 r10 %%varkon_pat_coonscre1 (sur270)");
        return(varkon_erpush("SU2943",errbuf));
        }

p_patb->p_r10.x_gm= out[0];      /* Corner point        for s=1,t=0 */
p_patb->p_r10.y_gm= out[1];      /*                                 */
p_patb->p_r10.z_gm= out[2];      /*                                 */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur270 r10 %15.8f %15.8f %15.8f u0 %9.6f\n",
p_patb->p_r10.x_gm,p_patb->p_r10.y_gm,p_patb->p_r10.z_gm,t_l);
fflush(dbgfil(SURPAC));
}
#endif



    t_l = e_u1;
   status=GE107 ((DBAny  *)&scur,p_u1,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107 r11 %%varkon_pat_coonscre1 (sur270)");
        return(varkon_erpush("SU2943",errbuf));
        }

p_patb->p_r11.x_gm= out[0];      /* Corner point        for s=1,t=1 */
p_patb->p_r11.y_gm= out[1];      /*                                 */
p_patb->p_r11.z_gm= out[2];      /*                                 */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur270 r11 %15.8f %15.8f %15.8f u0 %9.6f\n",
p_patb->p_r11.x_gm,p_patb->p_r11.y_gm,p_patb->p_r11.z_gm,t_l);
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* 3. Check input curves for Debug On                               */
/* __________________________________                               */
/*                                                                 !*/

#ifdef DEBUG

    t_l = s_v0;
   status=GE107 ((DBAny  *)&scur,p_v0,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107 r00 with p_v0%%sur270");
        return(varkon_erpush("SU2943",errbuf));
        }

r00.x_gm = out[0];
r00.y_gm = out[1];
r00.z_gm = out[2];

dist = (r00.x_gm-p_patb->p_r00.x_gm)*(r00.x_gm-p_patb->p_r00.x_gm)+ 
       (r00.y_gm-p_patb->p_r00.y_gm)*(r00.y_gm-p_patb->p_r00.y_gm)+ 
       (r00.z_gm-p_patb->p_r00.z_gm)*(r00.z_gm-p_patb->p_r00.z_gm); 

dist = SQRT(dist);

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur270 r00 %15.8f %15.8f %15.8f v0 %9.6f\n",
r00.x_gm,r00.y_gm,r00.z_gm,t_l);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur270 Distance r00 points %25.10f\n", dist );
}
fflush(dbgfil(SURPAC)); /* To file from buffer      */

if ( dist > 0.001 ) /* [ndra till ctol n}gon g}ng  */
   {
   sprintf(errbuf,"r00 dist > 0.001%%sur270");
   return(varkon_erpush("SU2993",errbuf));
   }

    t_l = e_v0;
   status=GE107 ((DBAny  *)&scur,p_v0,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107 r10 with p_v0%%sur270");
        return(varkon_erpush("SU2943",errbuf));
        }

r10.x_gm = out[0];
r10.y_gm = out[1];
r10.z_gm = out[2];

dist = (r10.x_gm-p_patb->p_r10.x_gm)*(r10.x_gm-p_patb->p_r10.x_gm)+ 
       (r10.y_gm-p_patb->p_r10.y_gm)*(r10.y_gm-p_patb->p_r10.y_gm)+ 
       (r10.z_gm-p_patb->p_r10.z_gm)*(r10.z_gm-p_patb->p_r10.z_gm); 

dist = SQRT(dist);

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur270 r10 %15.8f %15.8f %15.8f v0 %9.6f\n",
r10.x_gm,r10.y_gm,r10.z_gm,t_l);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur270 Distance r01 points %25.10f\n", dist );
}
fflush(dbgfil(SURPAC)); /* To file from buffer      */

if ( dist > 0.001 ) /* [ndra till ctol n}gon g}ng  */
   {
   sprintf(errbuf,"r10 dist > 0.001%%sur270");
   return(varkon_erpush("SU2993",errbuf));
   }

    t_l = s_v1;
   status=GE107 ((DBAny  *)&scur,p_v1,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107 r01 with p_v1%%sur270");
        return(varkon_erpush("SU2943",errbuf));
        }

r01.x_gm = out[0];
r01.y_gm = out[1];
r01.z_gm = out[2];

dist = (r01.x_gm-p_patb->p_r01.x_gm)*(r01.x_gm-p_patb->p_r01.x_gm)+ 
       (r01.y_gm-p_patb->p_r01.y_gm)*(r01.y_gm-p_patb->p_r01.y_gm)+ 
       (r01.z_gm-p_patb->p_r01.z_gm)*(r01.z_gm-p_patb->p_r01.z_gm); 

dist = SQRT(dist);

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur270 r01 %15.8f %15.8f %15.8f v1 %9.6f\n",
r01.x_gm,r01.y_gm,r01.z_gm,t_l);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur270 Distance r01 points %25.10f\n", dist );
}
fflush(dbgfil(SURPAC)); /* To file from buffer      */

if ( dist > 0.001 ) /* [ndra till ctol n}gon g}ng  */
   {
   sprintf(errbuf,"r01 dist > 0.001%%sur270");
   return(varkon_erpush("SU2993",errbuf));
   }


    t_l = e_v1;
   status=GE107 ((DBAny  *)&scur,p_v1,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107 r11 with p_v1%%sur270");
        return(varkon_erpush("SU2943",errbuf));
        }

r11.x_gm = out[0];
r11.y_gm = out[1];
r11.z_gm = out[2];

dist = (r11.x_gm-p_patb->p_r11.x_gm)*(r11.x_gm-p_patb->p_r11.x_gm)+ 
       (r11.y_gm-p_patb->p_r11.y_gm)*(r11.y_gm-p_patb->p_r11.y_gm)+ 
       (r11.z_gm-p_patb->p_r11.z_gm)*(r11.z_gm-p_patb->p_r11.z_gm); 

dist = SQRT(dist);

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur270 r11 %15.8f %15.8f %15.8f v1 %9.6f\n",
r11.x_gm,r11.y_gm,r11.z_gm,t_l);
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur270 Distance r11 points %25.10f\n", dist );
}
fflush(dbgfil(SURPAC));

if ( dist > 0.001 ) /* TODO Change to system tolernce */
   {
   sprintf(errbuf,"r11 dist > 0.001%%sur270");
   return(varkon_erpush("SU2993",errbuf));
   }


/* End DEBUG checking  */
#endif


/* 3. Exit                                                          */
/* _______                                                          */
/*                                                                  */


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur270 Exit *** varkon_pat_coonscre1  \n");
  fflush(dbgfil(SURPAC));
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/
