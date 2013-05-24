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
/*  Function: varkon_pat_loftrev                   File: sur635.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*                                                                  */
/*  Reverse of parametric directions for a conic lofting patch.     */
/*                                                                  */
/*  The input patch will not be copied if the input and output      */
/*  patch adresses are equal.                                       */
/*                                                                  */
/*  Actually not necessary to reparameterize all segments.          */
/*  Sufficient with spine and P-value function.                     */
/*                                                                  */
/*  TODO                                                            */
/*  DBLine vs and ve should probably also be reparameterized ...    */
/*  Have forgotten the intention with this lines .....              */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-10-20   Originally written                                 */
/*  1999-12-12   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_loftrev    Reverse a conic lofting patch    */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short prepar();        /* Scaling of P-value function        */
#ifdef DEBUG       
static short pripat();        /* Printout of patch data             */
#endif
/*-----------------------------------------------------------------!*/

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* GE135                  * Reparameterize curve segment            */
/* GE120                  * Curve segment arclength                 */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_loftrev   */
/* SU2943 = Called   function xxxxxx failed in varkon_pat_loftrev   */
/* SU2803 = Spine must be straight line varkon_pat_loftrev.         */
/* SU2993 = Severe program error in varkon_pat_loftrev (sur635).    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus       varkon_pat_loftrev (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL *p_ipatl,      /* Input  lofting patch              (ptr) */
   DBint   rcase,        /* Reverse  case:                          */
                         /* Eq.  1: Reverse U                       */
                         /* Eq.  2: Reverse V                       */
                         /* Eq. 12: Reverse U and V                 */
   GMPATL *p_opatl )     /* Output lofting patch              (ptr) */

/* Out:                                                             */
/*       Data to p_opatl                                            */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   GMPATL  tpatl;        /* Temporary used conic lofting patch      */
   GMPATL  t2patl;       /* Temporary used conic lofting patch 2    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat u_trim[2];    /* Reparameterization U values             */
   DBCurve scur;         /* Curve header for segment (dummy) curve  */
   DBfloat uinterv[2];   /* Parameter interval for arclength calc.  */
   DBfloat s_len;        /* Spine segment arclength                 */
   char    errbuf[80];   /* String for error message fctn erpush    */
   DBint   status;       /* Error code from a called function       */

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
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur635 Enter *** varkon_pat_loftrev p_ipatl %d rcase %d\n", 
           (int)p_ipatl, (short)rcase);
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/* Printout of input conic lofting patch data  for DEBUG on.        */
/* Call of pripat.                                                  */
/*                                                                 !*/

/* Initialize for spine segment arclength computation.              */
   scur.hed_cu.type    = CURTYP;
   scur.ns_cu     = 1;      
   scur.plank_cu  = FALSE;      

#ifdef DEBUG      
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur635 Input conic lofting patch   p_ipatl= %d \n", (int)p_ipatl);
fflush(dbgfil(SURPAC)); 
}
   status=pripat (p_ipatl); 
   if (status<0) 
   {
   sprintf(errbuf,"pripat%%varkon_pat_loftrev (sur635)");
   return(varkon_erpush("SU2973",errbuf));
   }
#endif

/*!                                                                 */
/* 2. Reparameterize all rational curve segments                    */
/* _____________________________________________                    */
/*                                                                  */
/*  All data (incl. vs, ve ofs_pat) to tempory patch tpatl.         */
/*                                                                  */
/*  Reparameterization of all curve segments p_ipatl->lims, ->lime, */
/*  ->tans, etc. (modification of data in tpatl).                   */
/*  Calls of varkon_GE135 (GE135).                                */
/*                                                                  */
/*  Goto notrim if rcase= 2.                                        */
/*                                                                 !*/

tpatl = *p_ipatl;   /* All data to temporary patch                  */

  if         ( rcase == 1 || rcase == 12 )
    {
    u_trim[0] = 1.0;
    u_trim[1] = 0.0;
    }
  else if    ( rcase == 2 )
    {
    u_trim[0] = F_UNDEF;
    u_trim[1] = F_UNDEF;
    goto  notrim;
    }
  else
    {
    sprintf(errbuf,"rcase%%varkon_pat_loftrev (sur635)");
    return(varkon_erpush("SU2993",errbuf));
    }

/*  Reparameterization of spine segment                             */
status=GE135 (&p_ipatl->spine, u_trim , &tpatl.spine);
if (status<0) 
   {
   sprintf(errbuf,"GE135(spine)%%varkon_pat_loftrev (sur635)");
   return(varkon_erpush("SU2943",errbuf));
   }

uinterv[0] = 0.0;
uinterv[1] = 1.0;

status=GE120 ((DBAny *)&scur,&tpatl.spine,  uinterv , &s_len );
if (status<0) 
   {
   sprintf(errbuf,"GE120(spine)%%varkon_pat_loftrev (sur635)");
   return(varkon_erpush("SU2943",errbuf));
   }

tpatl.spine.sl = s_len;

/*  Reparameteris. of lims  segment                                 */

status=GE135 (&p_ipatl->lims, u_trim , &tpatl.lims);
if (status<0) 
   {
   sprintf(errbuf,"GE135(lims)%%varkon_pat_loftrev (sur635)");
   return(varkon_erpush("SU2943",errbuf));
   }

/*  Reparameteris. of lime  segment                                 */

status=GE135 (&p_ipatl->lime, u_trim , &tpatl.lime);
if (status<0) 
   {
   sprintf(errbuf,"GE135(lime)%%varkon_pat_loftrev (sur635)");
   return(varkon_erpush("SU2943",errbuf));
   }

/*  Reparameteris. of tans  segment                                 */

status=GE135 (&p_ipatl->tans, u_trim , &tpatl.tans);
if (status<0) 
   {
   sprintf(errbuf,"GE135(tans)%%varkon_pat_loftrev (sur635)");
   return(varkon_erpush("SU2943",errbuf));
   }

/*  Reparameteris. of tane  segment                                 */

status=GE135 (&p_ipatl->tane, u_trim , &tpatl.tane);
if (status<0) 
   {
   sprintf(errbuf,"GE135(tane)%%varkon_pat_loftrev (sur635)");
   return(varkon_erpush("SU2943",errbuf));
   }

/*  Reparameteris. of pval  segment if p_flag=1                     */


if ( p_ipatl->p_flag == 1 )
  {
   status=prepar (p_ipatl,u_trim,&tpatl); 
   if (status<0) 
     {
     sprintf(errbuf,"prepar%%varkon_pat_loftrev (sur635)");
     return(varkon_erpush("SU2973",errbuf));
     }   /* End error             */
}        /* End P value function  */

/*  Reparameteris. of midc  segment if p_flag= 2                    */

if ( p_ipatl->p_flag == 2 )
{
status=GE135 (&p_ipatl->midc, u_trim , &tpatl.midc);
if (status<0) 
   {
   sprintf(errbuf,"GE135(midc)%%varkon_pat_loftrev (sur635)");
   return(varkon_erpush("SU2943",errbuf));
   }
}


notrim:;  /* Label: No reparameterization in U direction */

/*!                                                                 */
/* 3. Reorder curve segments for rcase= 2 and 12                    */
/* _____________________________________________                    */
/*                                                                  */
/*  All data (incl. vs, ve ofs_pat) to tempory patch t2patl.        */
/*                                                                  */
/*                                                                 !*/

t2patl = tpatl;   /* All data to temporary patch 2                  */

  if         ( rcase == 2 || rcase == 12 )
    {
    V3MOME((char *)(&tpatl.lims),(char *)(&t2patl.lime),sizeof(DBSeg));
    V3MOME((char *)(&tpatl.lime),(char *)(&t2patl.lims),sizeof(DBSeg));
    V3MOME((char *)(&tpatl.tans),(char *)(&t2patl.tane),sizeof(DBSeg));
    V3MOME((char *)(&tpatl.tane),(char *)(&t2patl.tans),sizeof(DBSeg));
    }
  else
    {
    ; /* Do nothing more */
    }



#ifdef DEBUG      
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur635 Temporary conic lofting patch &t2patl= %d \n", (int)&t2patl);
}
   status=pripat (&t2patl); 
   if (status<0) 
   {
   sprintf(errbuf,"pripat%%varkon_pat_loftrev (sur635)");
   return(varkon_erpush("SU2973",errbuf));
   }
#endif

/*!                                                                 */
/* 3. Data to output variable                                       */
/* __________________________                                       */
/*                                                                  */
/*  Let output patch                                                */
  *p_opatl = t2patl;
/*                                                                 !*/

/*!                                                                 */
/*  Printout of output conic lofting patch data  for DEBUG on.      */
/*  Call of pripat.                                                 */
/*                                                                 !*/

#ifdef DEBUG      
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur635 Output conic lofting patch  p_opatl= %d \n", (int)p_opatl);
}
   status=pripat (p_opatl); 
   if (status<0) 
   {
   sprintf(errbuf,"pripat%%varkon_pat_loftrev     (sur635)");
   return(varkon_erpush("SU2973",errbuf));
   }
#endif


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur635 Exit *** varkon_pat_loftrev  ***  \n");
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/

/*!********* Internal ** function ** prepar *************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Reparameterization of P-value function                           */
/*                                                                  */
    static  short    prepar (p_ipatl, u_trim, p_tpatl)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL *p_ipatl;      /* Input     conic lofting patch     (ptr) */
   DBfloat  u_trim[2];     /* Reparameterization U values             */
   GMPATL *p_tpatl;      /* Temporary conic lofting patch     (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */
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
"sur635*prepar* Reparam. P-value p_ipatl= %d p_tpatl %d\n", 
        (int)p_ipatl, (int)p_tpatl );
}
#endif

#ifdef DEBUG      
if ( dbglev(SURPAC) == 1  )
{
fprintf(dbgfil(SURPAC),
"sur635*prepar Spine length In %f Out %f Difference %f\n"
       ,p_ipatl->spine.sl,p_tpatl->spine.sl,  
        p_ipatl->spine.sl - p_tpatl->spine.sl );
}
#endif

/*!                                                                 */
/* Transformation of P-value function.                              */
/* Call of varkon_GE135 (GE135).                                  */
/*                                                                 !*/

status=GE135 (&p_ipatl->pval , u_trim , &p_tpatl->pval );
if (status<0) 
   {
   sprintf(errbuf,"GE135(pval)%%varkon_pat_loftrev (sur635)");
   return(varkon_erpush("SU2943",errbuf));
   }

#ifdef DEBUG      
if ( dbglev(SURPAC) == 1  )
{
fprintf(dbgfil(SURPAC),
"sur635*prepar Spine input  start X %f end X %f\n"
       ,p_ipatl->spine.c0x,
        p_ipatl->spine.c0x+p_ipatl->spine.c1x+
        p_ipatl->spine.c2x+p_ipatl->spine.c3x);
fprintf(dbgfil(SURPAC),
"sur635*prepar Spine output start X %f end X %f\n"
       ,p_tpatl->spine.c0x,
        p_tpatl->spine.c0x+p_tpatl->spine.c1x+
        p_tpatl->spine.c2x+p_tpatl->spine.c3x);
fprintf(dbgfil(SURPAC),
"sur635*prepar Pval  input  start X %f end X %f Length %f\n"
       ,p_ipatl->pval.c0x,
        p_ipatl->pval.c0x+p_ipatl->pval.c1x+
        p_ipatl->pval.c2x+p_ipatl->pval.c3x, 
                          p_ipatl->pval.c1x+
        p_ipatl->pval.c2x+p_ipatl->pval.c3x);
fprintf(dbgfil(SURPAC),
"sur635*prepar Pval  output start X %f end X %f Length %f\n"
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
"sur635 *** pripat*Printout of patch data for p_patl= %d\n", (int)p_patl);

fprintf(dbgfil(SURPAC),
"sur635 Spine addresses &p_patl->spine= %d &p_patl->spine.c0y= %d\n"
     ,(int)&p_patl->spine,  (int)&p_patl->spine.c0y);

fprintf(dbgfil(SURPAC),
"sur635 Coefficients: p_patl->spine.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur635 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->spine.c0x,p_patl->spine.c1x,
        p_patl->spine.c2x,p_patl->spine.c3x );
fprintf(dbgfil(SURPAC),
"sur635 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->spine.c0y,p_patl->spine.c1y,
        p_patl->spine.c2y,p_patl->spine.c3y );
fprintf(dbgfil(SURPAC),
"sur635 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->spine.c0z,p_patl->spine.c1z,
        p_patl->spine.c2z,p_patl->spine.c3z );
fprintf(dbgfil(SURPAC),
"sur635 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->spine.c0 ,p_patl->spine.c1 ,
        p_patl->spine.c2 ,p_patl->spine.c3  );
fprintf(dbgfil(SURPAC),
"sur635 offset .ofs= %f  Type .typ= %d Lengt .sl= %f\n"
       ,p_patl->spine.ofs,p_patl->spine.typ,p_patl->spine.sl );
fprintf(dbgfil(SURPAC),
"sur635 Coefficients: p_patl->lims.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur635 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->lims.c0x,p_patl->lims.c1x,
        p_patl->lims.c2x,p_patl->lims.c3x );
fprintf(dbgfil(SURPAC),
"sur635 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->lims.c0y,p_patl->lims.c1y,
        p_patl->lims.c2y,p_patl->lims.c3y );
fprintf(dbgfil(SURPAC),
"sur635 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->lims.c0z,p_patl->lims.c1z,
        p_patl->lims.c2z,p_patl->lims.c3z );
fprintf(dbgfil(SURPAC),
"sur635 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->lims.c0 ,p_patl->lims.c1 ,
        p_patl->lims.c2 ,p_patl->lims.c3  );
fprintf(dbgfil(SURPAC),
"sur635 offset .ofs= %f  Type .typ= %d Lengt .sl= %f\n"
       ,p_patl->lims.ofs,p_patl->lims.typ,p_patl->lims.sl );
fprintf(dbgfil(SURPAC),
"sur635 Coefficients: p_patl->lime.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur635 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->lime.c0x,p_patl->lime.c1x,
        p_patl->lime.c2x,p_patl->lime.c3x );
fprintf(dbgfil(SURPAC),
"sur635 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->lime.c0y,p_patl->lime.c1y,
        p_patl->lime.c2y,p_patl->lime.c3y );
fprintf(dbgfil(SURPAC),
"sur635 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->lime.c0z,p_patl->lime.c1z,
        p_patl->lime.c2z,p_patl->lime.c3z );
fprintf(dbgfil(SURPAC),
"sur635 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->lime.c0 ,p_patl->lime.c1 ,
        p_patl->lime.c2 ,p_patl->lime.c3  );
fprintf(dbgfil(SURPAC),
"sur635 offset .ofs= %f  Type .typ= %d Lengt .sl= %f\n"
       ,p_patl->lime.ofs,p_patl->lime.typ,p_patl->lime.sl );
fprintf(dbgfil(SURPAC),
"sur635 Coefficients: p_patl->tans.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur635 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->tans.c0x,p_patl->tans.c1x,
        p_patl->tans.c2x,p_patl->tans.c3x );
fprintf(dbgfil(SURPAC),
"sur635 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->tans.c0y,p_patl->tans.c1y,
        p_patl->tans.c2y,p_patl->tans.c3y );
fprintf(dbgfil(SURPAC),
"sur635 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->tans.c0z,p_patl->tans.c1z,
        p_patl->tans.c2z,p_patl->tans.c3z );
fprintf(dbgfil(SURPAC),
"sur635 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->tans.c0 ,p_patl->tans.c1 ,
        p_patl->tans.c2 ,p_patl->tans.c3  );
fprintf(dbgfil(SURPAC),
"sur635 offset .ofs= %f  Type .typ= %d Lengt .sl= %f\n"
       ,p_patl->tans.ofs,p_patl->tans.typ,p_patl->tans.sl );
fprintf(dbgfil(SURPAC),
"sur635 Coefficients: p_patl->tane.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur635 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->tane.c0x,p_patl->tane.c1x,
        p_patl->tane.c2x,p_patl->tane.c3x );
fprintf(dbgfil(SURPAC),
"sur635 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->tane.c0y,p_patl->tane.c1y,
        p_patl->tane.c2y,p_patl->tane.c3y );
fprintf(dbgfil(SURPAC),
"sur635 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->tane.c0z,p_patl->tane.c1z,
        p_patl->tane.c2z,p_patl->tane.c3z );
fprintf(dbgfil(SURPAC),
"sur635 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->tane.c0 ,p_patl->tane.c1 ,
        p_patl->tane.c2 ,p_patl->tane.c3  );
fprintf(dbgfil(SURPAC),
"sur635 offset .ofs= %f  Type .typ= %d Lengt .sl= %f\n"
       ,p_patl->tane.ofs,p_patl->tane.typ,p_patl->tane.sl );
fprintf(dbgfil(SURPAC),
"sur635 Coefficients: p_patl->pval.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur635 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->pval.c0x,p_patl->pval.c1x,
        p_patl->pval.c2x,p_patl->pval.c3x );
fprintf(dbgfil(SURPAC),
"sur635 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->pval.c0y,p_patl->pval.c1y,
        p_patl->pval.c2y,p_patl->pval.c3y );
fprintf(dbgfil(SURPAC),
"sur635 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->pval.c0z,p_patl->pval.c1z,
        p_patl->pval.c2z,p_patl->pval.c3z );
fprintf(dbgfil(SURPAC),
"sur635 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->pval.c0 ,p_patl->pval.c1 ,
        p_patl->pval.c2 ,p_patl->pval.c3  );
fprintf(dbgfil(SURPAC),
"sur635 offset .ofs= %f  Type .typ= %d Lengt .sl= %f\n"
       ,p_patl->pval.ofs,p_patl->pval.typ,p_patl->pval.sl );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */


}                            /* End   if statement for whole fctn  */
    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
#endif

