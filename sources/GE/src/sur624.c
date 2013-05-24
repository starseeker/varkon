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
/*  Function: varkon_pat_lofttra                   File: sur624.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*                                                                  */
/*  Transforms a conic lofting patch with the input transformation  */
/*  matrix (the input coordinate system).                           */
/*                                                                  */
/*  The input patch will not be copied if the input and output      */
/*  patch adresses are equal.                                       */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-14   Originally written                                 */
/*  1997-05-02   p_flag= 5                                          */
/*  1999-11-27   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_lofttra    Transform a conic lofting patch  */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short pscale();        /* Scaling of P-value function        */
#ifdef DEBUG       
static short pripat();        /* Printout of patch data             */
#endif
/*-----------------------------------------------------------------!*/

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* GEtfseg_to_local       * Transform rational segment              */
/* GE120                  * Curve segment arclength                 */
/* GEtfvec_to_local       * Transformation of a vector              */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_lofttra   */
/* SU2943 = Called   function xxxxxx failed in varkon_pat_lofttra   */
/* SU2803 = Spine must be straight line varkon_pat_lofttra.         */
/* SU2993 = Severe program error in varkon_pat_lofttra (sur624).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus       varkon_pat_lofttra (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL *p_ipatl,      /* Input  lofting patch              (ptr) */
   DBTmat *p_c,          /* Local coordinate system           (ptr) */
   GMPATL *p_opatl )     /* Output lofting patch              (ptr) */

/* Out:                                                             */
/*       Data to p_opatl                                            */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   GMPATL tpatl;         /* Temporary used conic lofting patch      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBCurve  scur;        /* Curve header for segment (dummy) curve  */
   DBfloat  uinterv[2];  /* Parameter interval for arclength calc.  */
   DBfloat  s_len;       /* Spine segment arclength                 */
   DBVector yaxis;       /* Fixed direction defining the Y axis     */
   char     errbuf[80];  /* String for error message fctn erpush    */
   DBint    status;      /* Error code from a called function       */

/* ----------------- Theory ----------------------------------------*/
/*                                                                  */
/* The coordinates and derivatives  ........................        */
/*                                                                  */
/*----------------------------------------------------------------- */

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
"sur624 Enter *** varkon_pat_lofttra     p_ipatl %d \n", p_ipatl);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/* Printout of input conic lofting patch data  for DEBUG on.        */
/* Call of pripat.                                                  */
/*                                                                 !*/

/* Initiate for spine segment arclength computation.                */
   scur.hed_cu.type    = CURTYP;
   scur.ns_cu     = 1;      
   scur.plank_cu  = FALSE;      

#ifdef DEBUG      
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur624 Input     conic lofting patch   p_ipatl= %d \n", p_ipatl);
}
   status=pripat (p_ipatl); 
   if (status<0) 
   {
   sprintf(errbuf,"pripat%%varkon_pat_lofttra     (sur624)");
   return(varkon_erpush("SU2973",errbuf));
   }
#endif

/*!                                                                 */
/* 2. Transform all rational curve segments                         */
/* ________________________________________                         */
/*                                                                  */
/*  All data (incl. vs, ve ofs_pat) to tempory patch tpatl.         */
/*                                                                  */
/*  Transformation of all curve segments p_ipatl->lims, ->lime,     */
/*  ->tans, etc. (modification of data in tpatl).                   */
/*  Calls of GEtfseg_to_local.                                      */
/*                                                                  */
/*  Calculate spine length for output spine segment.                */
/*  Call of GE120.                                                  */
/*                                                                  */
/*  Transform (scale) P-value function if input spine               */
/*  length not is equal to output spine length. (No                 */
/*  transformation of P-value fctn if they are equal).              */
/*  Call of internal function pscale.                               */
/*                                                                 !*/

tpatl = *p_ipatl;   /* All data to temporary patch                  */

/*  Transformation of spine segment                                 */
status=GEtfseg_to_local
(&p_ipatl->spine, p_c , &tpatl.spine);
if (status<0) 
 {
 sprintf(errbuf,"GEtfseg_to_local(spine)%%varkon_pat_lofttra (sur624)");
 return(varkon_erpush("SU2943",errbuf));
 }

uinterv[0] = 0.0;
uinterv[1] = 1.0;

status=GE120 ((DBAny *)&scur,&tpatl.spine,  uinterv , &s_len );
if (status<0) 
   {
   sprintf(errbuf,"GE120(spine)%%varkon_pat_lofttra (sur624)");
   return(varkon_erpush("SU2943",errbuf));
   }

tpatl.spine.sl = s_len;

/*  Transformation of lims  segment                                 */
/* Not programmed for p_flag= 5 and scaling matrix !!!!             */
if ( p_ipatl->p_flag != 5 )
  {
status=GEtfseg_to_local (&p_ipatl->lims, p_c , &tpatl.lims);
if (status<0) 
 {
 sprintf(errbuf,"GEtfseg_to_local(lims)%%varkon_pat_lofttra (sur624)");
 return(varkon_erpush("SU2943",errbuf));
 }
 }  /* Not p_flag = 5  */

/*  Transformation of lime  segment                                 */

status=GEtfseg_to_local (&p_ipatl->lime, p_c , &tpatl.lime);
if (status<0) 
 {
 sprintf(errbuf,"GEtfseg_to_local(lime)%%varkon_pat_lofttra (sur624)");
 return(varkon_erpush("SU2943",errbuf));
 }

/*  Transformation of tans  segment                                 */

status=GEtfseg_to_local (&p_ipatl->tans, p_c , &tpatl.tans);
if (status<0) 
 {
 sprintf(errbuf,"GEtfseg_to_local(tans)%%varkon_pat_lofttra (sur624)");
 return(varkon_erpush("SU2943",errbuf));
 }

/*  Transformation of tane  segment                                 */

status=GEtfseg_to_local (&p_ipatl->tane, p_c , &tpatl.tane);
if (status<0) 
 {
 sprintf(errbuf,"GEtfseg_to_local(tane)%%varkon_pat_lofttra (sur624)");
 return(varkon_erpush("SU2943",errbuf));
 }

/*  Transformation of pval  segment if p_flag=1 and                 */
/*  differing spine segment lengths.                                */


if ( p_ipatl->p_flag == 1 )
  {
  /* A 2D function shall only be transformed (scaled) if          */
  /* if the surface patch is scaled.                              */
  if ( fabs(p_ipatl->spine.sl - tpatl.spine.sl) > 0.01 )
   {
   status=pscale (p_ipatl,p_c,&tpatl); 
   if (status<0) 
     {
     sprintf(errbuf,"pscale%%varkon_pat_lofttra     (sur624)");
     return(varkon_erpush("SU2973",errbuf));
     }   /* End error             */
   }     /* End scaling           */
}        /* End P value function  */

/*  Transformation of midc  segment if p_flag= 2                    */

if ( p_ipatl->p_flag == 2 )
{
status=GEtfseg_to_local (&p_ipatl->midc, p_c , &tpatl.midc);
if (status<0) 
 {
 sprintf(errbuf,"GEtfseg_to_local(midc)%%varkon_pat_lofttra (sur624)");
 return(varkon_erpush("SU2943",errbuf));
 }
}

if ( p_ipatl->p_flag == 5 )
  {
  yaxis.x_gm = p_ipatl->pval.c0x;     
  yaxis.y_gm = p_ipatl->pval.c0y;     
  yaxis.z_gm = p_ipatl->pval.c0z;     
  status=GEtfvec_to_local (&yaxis , p_c , &yaxis);
  tpatl.pval.c0x = yaxis.x_gm;     
  tpatl.pval.c0y = yaxis.y_gm;     
  tpatl.pval.c0z = yaxis.z_gm;     
  }

#ifdef DEBUG      
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur624 Temporary conic lofting patch    &tpatl= %d \n", &tpatl);
}
   status=pripat (&tpatl); 
   if (status<0) 
   {
   sprintf(errbuf,"pripat%%varkon_pat_lofttra     (sur624)");
   return(varkon_erpush("SU2973",errbuf));
   }
#endif

/*!                                                                 */
/* 3. Data to output variable                                       */
/* __________________________                                       */
/*                                                                  */
/*  Let output patch                                                */
  *p_opatl = tpatl;
/*                                                                 !*/

/*!                                                                 */
/*  Printout of output conic lofting patch data  for DEBUG on.      */
/*  Call of pripat.                                                 */
/*                                                                 !*/

#ifdef DEBUG      
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur624 Output    conic lofting patch  p_opatl= %d \n", p_opatl);
}
   status=pripat (p_opatl); 
   if (status<0) 
   {
   sprintf(errbuf,"pripat%%varkon_pat_lofttra     (sur624)");
   return(varkon_erpush("SU2973",errbuf));
   }
#endif


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur624 Exit *** varkon_pat_lofttra  ***  \n");
  fflush(dbgfil(SURPAC)); 
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!********* Internal ** function ** pscale *************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Scaling of P-value function                                      */
/*                                                                  */
    static  short    pscale (p_ipatl, p_c, p_tpatl)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL *p_ipatl;      /* Input     conic lofting patch     (ptr) */
   DBTmat *p_c;          /* Local coordinate system           (ptr) */
   GMPATL *p_tpatl;      /* Temporary conic lofting patch     (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat scale;        /* Scale factor for P-value function       */
   DBTmat  smat;         /* Scale matrix for P-value function       */
/*----------------------------------------------------------------- */
   DBfloat c_sum;        /* Sum of coefficients                     */
   DBint  status;        /* Error code from a called function       */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG       
if ( dbglev(SURPAC) == 2 ) 
{
fprintf(dbgfil(SURPAC),
"sur624*pscale*Scale P-value p_ipatl= %d p_tpatl %d\n", 
        p_ipatl, p_tpatl );
}
#endif

#ifdef DEBUG      
if ( dbglev(SURPAC) == 1  )
{
fprintf(dbgfil(SURPAC),
"sur624*pscale Spine length In %f Out %f Difference %f\n"
       ,p_ipatl->spine.sl,p_tpatl->spine.sl,  
        p_ipatl->spine.sl - p_tpatl->spine.sl );
}
if ( dbglev(SURPAC) == 2  )
{
  fprintf(dbgfil(SURPAC),"sur624 Input transformation matrix \n");
  fprintf(dbgfil(SURPAC),"sur624 g11 %f g12 %f g13 %f g14 %f \n",
                  p_c->g11,p_c->g12,p_c->g13,p_c->g14);
  fprintf(dbgfil(SURPAC),"sur624 g21 %f g22 %f g23 %f g24 %f \n",
                  p_c->g21,p_c->g22,p_c->g23,p_c->g24);
  fprintf(dbgfil(SURPAC),"sur624 g31 %f g32 %f g33 %f g34 %f \n",
                  p_c->g31,p_c->g32,p_c->g33,p_c->g34);
  fprintf(dbgfil(SURPAC),"sur624 g41 %f g42 %f g43 %f g44 %f \n",
                  p_c->g41,p_c->g42,p_c->g43,p_c->g44);
}
#endif

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/* Check that the spine is a straight line if scaling               */
/* factors in X, Y and Z are different.                             */
/* !! ... check of scaling factors not yet implemented .. !!!!!!!   */
/*                                                                 !*/

   c_sum = fabs(p_ipatl->spine.c2x)+
           fabs(p_ipatl->spine.c3x)+
           fabs(p_ipatl->spine.c2y)+
           fabs(p_ipatl->spine.c3y)+
           fabs(p_ipatl->spine.c2z)+
           fabs(p_ipatl->spine.c3z)+
           fabs(p_ipatl->spine.c1 )+
           fabs(p_ipatl->spine.c2 )+
           fabs(p_ipatl->spine.c3 );

   if ( c_sum > 0.000001 )
      {
      sprintf(errbuf,"(scale of patch)%%varkon_pat_lofttra");
      return(varkon_erpush("SU2803",errbuf));
      }

/*!                                                                 */
/* Scaling factor scale=(output spine length)/(input spine length)  */
/*                                                                  */
/* Scale matrix smat:                                               */
/*  g11= scale  g12= 0    g13= 0   g14= 0                           */
/*  g21=  0     g22= 1    g23= 0   g24= 0                           */
/*  g31=  0     g32= 0    g33= 1   g34= 0                           */
/*  g41=  0     g42= 0    g43= 0   g44= 1                           */
/*                                                                 !*/

   if ( fabs(p_ipatl->spine.sl) > 0.0001 )
     {
     scale=  p_tpatl->spine.sl / p_ipatl->spine.sl ;  
     }
   else
     {
     sprintf(errbuf,"(spine length)%%varkon_pat_lofttra (sur624)");
     return(varkon_erpush("SU2993",errbuf));
     }

   smat.g11 = scale;
   smat.g12 = 0.0;   
   smat.g13 = 0.0;   
   smat.g14 = 0.0;   

   smat.g21 = 0.0;  
   smat.g22 = 1.0;   
   smat.g23 = 0.0;   
   smat.g24 = 0.0;   

   smat.g31 = 0.0;  
   smat.g32 = 0.0;   
   smat.g33 = 1.0;   
   smat.g34 = 0.0;   

   smat.g41 = 0.0;  
   smat.g42 = 0.0;   
   smat.g43 = 0.0;   
   smat.g44 = 1.0;   

/*!                                                                 */
/* Scaling of P-value function.                                     */
/* Call of GEtfseg_to_local.                                        */
/*                                                                 !*/

status=GEtfseg_to_local (&p_ipatl->pval , &smat , &p_tpatl->pval );
if (status<0) 
 {
 sprintf(errbuf,"GEtfseg_to_local(pval)%%varkon_pat_lofttra (sur624)");
 return(varkon_erpush("SU2943",errbuf));
 }

#ifdef DEBUG      
if ( dbglev(SURPAC) == 1  )
{
fprintf(dbgfil(SURPAC),
"sur624*pscale Scale factor scale=  %f\n", scale );
fprintf(dbgfil(SURPAC),
"sur624*pscale Spine input  start X %f end X %f\n"
       ,p_ipatl->spine.c0x,
        p_ipatl->spine.c0x+p_ipatl->spine.c1x+
        p_ipatl->spine.c2x+p_ipatl->spine.c3x);
fprintf(dbgfil(SURPAC),
"sur624*pscale Spine output start X %f end X %f\n"
       ,p_tpatl->spine.c0x,
        p_tpatl->spine.c0x+p_tpatl->spine.c1x+
        p_tpatl->spine.c2x+p_tpatl->spine.c3x);
fprintf(dbgfil(SURPAC),
"sur624*pscale Pval  input  start X %f end X %f Length %f\n"
       ,p_ipatl->pval.c0x,
        p_ipatl->pval.c0x+p_ipatl->pval.c1x+
        p_ipatl->pval.c2x+p_ipatl->pval.c3x, 
                          p_ipatl->pval.c1x+
        p_ipatl->pval.c2x+p_ipatl->pval.c3x);
fprintf(dbgfil(SURPAC),
"sur624*pscale Pval  output start X %f end X %f Length %f\n"
       ,p_tpatl->pval.c0x,
        p_tpatl->pval.c0x+p_tpatl->pval.c1x+
        p_tpatl->pval.c2x+p_tpatl->pval.c3x, 
                          p_tpatl->pval.c1x+
        p_tpatl->pval.c2x+p_tpatl->pval.c3x);
}
#endif


   return(SUCCED);

} /* End of function                                                */
/********************************************************************/
/*!                                                                 */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG       
/*!********* Internal ** function **Defined*Only*For*Debug*On********/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Printout of the conic lofting patch data.                        */
/*                                                                  */
   static short pripat (p_patl)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL *p_patl;       /* Pointer to the conic lofting patch      */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

if ( dbglev(SURPAC) == 2 )    /* Note: if statement for whole fctn  */
  {
/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* Printout of all data in the conic lofting patch.                 */
/*                                                                 !*/

fprintf(dbgfil(SURPAC),
"sur624 *** pripat*Printout of patch data for p_patl= %d\n", p_patl);

fprintf(dbgfil(SURPAC),
"sur624 Spine addresses &p_patl->spine= %d &p_patl->spine.c0y= %d\n"
     ,&p_patl->spine,  &p_patl->spine.c0y);

fprintf(dbgfil(SURPAC),
"sur624 Coefficients: p_patl->spine.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur624 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->spine.c0x,p_patl->spine.c1x,
        p_patl->spine.c2x,p_patl->spine.c3x );
fprintf(dbgfil(SURPAC),
"sur624 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->spine.c0y,p_patl->spine.c1y,
        p_patl->spine.c2y,p_patl->spine.c3y );
fprintf(dbgfil(SURPAC),
"sur624 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->spine.c0z,p_patl->spine.c1z,
        p_patl->spine.c2z,p_patl->spine.c3z );
fprintf(dbgfil(SURPAC),
"sur624 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->spine.c0 ,p_patl->spine.c1 ,
        p_patl->spine.c2 ,p_patl->spine.c3  );
fprintf(dbgfil(SURPAC),
"sur624 offset .ofs= %f  Type .typ= %d Lengt .sl= %f\n"
       ,p_patl->spine.ofs,p_patl->spine.typ,p_patl->spine.sl );
fprintf(dbgfil(SURPAC),
"sur624 Coefficients: p_patl->lims.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur624 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->lims.c0x,p_patl->lims.c1x,
        p_patl->lims.c2x,p_patl->lims.c3x );
fprintf(dbgfil(SURPAC),
"sur624 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->lims.c0y,p_patl->lims.c1y,
        p_patl->lims.c2y,p_patl->lims.c3y );
fprintf(dbgfil(SURPAC),
"sur624 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->lims.c0z,p_patl->lims.c1z,
        p_patl->lims.c2z,p_patl->lims.c3z );
fprintf(dbgfil(SURPAC),
"sur624 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->lims.c0 ,p_patl->lims.c1 ,
        p_patl->lims.c2 ,p_patl->lims.c3  );
fprintf(dbgfil(SURPAC),
"sur624 offset .ofs= %f  Type .typ= %d Lengt .sl= %f\n"
       ,p_patl->lims.ofs,p_patl->lims.typ,p_patl->lims.sl );
fprintf(dbgfil(SURPAC),
"sur624 Coefficients: p_patl->lime.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur624 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->lime.c0x,p_patl->lime.c1x,
        p_patl->lime.c2x,p_patl->lime.c3x );
fprintf(dbgfil(SURPAC),
"sur624 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->lime.c0y,p_patl->lime.c1y,
        p_patl->lime.c2y,p_patl->lime.c3y );
fprintf(dbgfil(SURPAC),
"sur624 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->lime.c0z,p_patl->lime.c1z,
        p_patl->lime.c2z,p_patl->lime.c3z );
fprintf(dbgfil(SURPAC),
"sur624 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->lime.c0 ,p_patl->lime.c1 ,
        p_patl->lime.c2 ,p_patl->lime.c3  );
fprintf(dbgfil(SURPAC),
"sur624 offset .ofs= %f  Type .typ= %d Lengt .sl= %f\n"
       ,p_patl->lime.ofs,p_patl->lime.typ,p_patl->lime.sl );
fprintf(dbgfil(SURPAC),
"sur624 Coefficients: p_patl->tans.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur624 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->tans.c0x,p_patl->tans.c1x,
        p_patl->tans.c2x,p_patl->tans.c3x );
fprintf(dbgfil(SURPAC),
"sur624 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->tans.c0y,p_patl->tans.c1y,
        p_patl->tans.c2y,p_patl->tans.c3y );
fprintf(dbgfil(SURPAC),
"sur624 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->tans.c0z,p_patl->tans.c1z,
        p_patl->tans.c2z,p_patl->tans.c3z );
fprintf(dbgfil(SURPAC),
"sur624 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->tans.c0 ,p_patl->tans.c1 ,
        p_patl->tans.c2 ,p_patl->tans.c3  );
fprintf(dbgfil(SURPAC),
"sur624 offset .ofs= %f  Type .typ= %d Lengt .sl= %f\n"
       ,p_patl->tans.ofs,p_patl->tans.typ,p_patl->tans.sl );
fprintf(dbgfil(SURPAC),
"sur624 Coefficients: p_patl->tane.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur624 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->tane.c0x,p_patl->tane.c1x,
        p_patl->tane.c2x,p_patl->tane.c3x );
fprintf(dbgfil(SURPAC),
"sur624 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->tane.c0y,p_patl->tane.c1y,
        p_patl->tane.c2y,p_patl->tane.c3y );
fprintf(dbgfil(SURPAC),
"sur624 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->tane.c0z,p_patl->tane.c1z,
        p_patl->tane.c2z,p_patl->tane.c3z );
fprintf(dbgfil(SURPAC),
"sur624 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->tane.c0 ,p_patl->tane.c1 ,
        p_patl->tane.c2 ,p_patl->tane.c3  );
fprintf(dbgfil(SURPAC),
"sur624 offset .ofs= %f  Type .typ= %d Lengt .sl= %f\n"
       ,p_patl->tane.ofs,p_patl->tane.typ,p_patl->tane.sl );
fprintf(dbgfil(SURPAC),
"sur624 Coefficients: p_patl->pval.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur624 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->pval.c0x,p_patl->pval.c1x,
        p_patl->pval.c2x,p_patl->pval.c3x );
fprintf(dbgfil(SURPAC),
"sur624 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->pval.c0y,p_patl->pval.c1y,
        p_patl->pval.c2y,p_patl->pval.c3y );
fprintf(dbgfil(SURPAC),
"sur624 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->pval.c0z,p_patl->pval.c1z,
        p_patl->pval.c2z,p_patl->pval.c3z );
fprintf(dbgfil(SURPAC),
"sur624 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->pval.c0 ,p_patl->pval.c1 ,
        p_patl->pval.c2 ,p_patl->pval.c3  );
fprintf(dbgfil(SURPAC),
"sur624 offset .ofs= %f  Type .typ= %d Lengt .sl= %f\n"
       ,p_patl->pval.ofs,p_patl->pval.typ,p_patl->pval.sl );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */


}                            /* End   if statement for whole fctn  */
    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
#endif

