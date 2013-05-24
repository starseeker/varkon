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

/*!******************************************************************/
/*                                                                  */
/*  Function: varkon_pat_priconloft                File: sur223.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function prints out patch data to Debug File.               */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-03   Originally written                                 */
/*  1994-11-22   Debug                                              */
/*  1995-03-07   Debug                                              */
/*  1996-05-28   Elimination of compilation warning                 */
/*  1997-02-08   Elimination of compilation warning                 */
/*  1999-11-27   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_priconloft Printout of conic lofting patch  */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short pripat();        /* Printout of patch data             */
/*-----------------------------------------------------------------!*/

/*--- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*------------------------------------------------------------------*/

/*--------------- Function calls (external) ------------------------*/
/*                                                                  */
/*------------------------------------------------------------------*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_pat_priconloft*/
/* SU2993 = Severe program error in varkon_pat_priconloft (sur223). */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
    DBstatus   varkon_pat_priconloft (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL *p_patl )      /* Conic lofting patch               (ptr) */
/* Out:                                                             */
/*       Only printout of data to Debug file                        */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint  status;        /* Error code from a called function       */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur223 Enter *** varkon_pat_priconloft p_patl %d \n", (int)p_patl);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 1. Printout of patch data                                        */
/* __________________________                                       */
/*                                                                  */
/*  Printout of conic lofting patch data.                           */
/*  Call of pripat.                                                 */
/*                                                                 !*/

   status=pripat (p_patl); 
   if (status<0) 
   {
   sprintf(errbuf,"pripat%%varkon_pat_priconloft (sur223)");
   return(varkon_erpush("SU2973",errbuf));
   }

    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/

/*!********* Internal ** function ** pripat *************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Printout of the conic lofting patch data.                        */

   static short pripat (p_patl)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   GMPATL *p_patl;       /* Pointer to the conic lofting patch      */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
#ifdef DEBUG
   DBCurve  scur;        /* Curve header for segment (dummy) curve  */
   DBfloat  out[16];     /* Coordinates and derivatives for crv pt  */
   DBfloat  t_l;         /* Local parameter value                   */
   short    rcode;       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors added    out[9-14]*/
/*-----------------------------------------------------------------!*/
   char   errbuf[80];    /* String for error message fctn erpush    */
   short  status;        /* Error code from a called function       */


/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 2. Initiations                                                   */

   rcode = 3;
   scur.hed_cu.type    = CURTYP;
/*                                                                 !*/
   scur.ns_cu     = 1;      
   scur.plank_cu  = FALSE;      
/* Input data to the curve evaluation routine GE107 is DBCurve(scur)*/
/* Only scur.hed_cu.type is used, but all are defined in case ..... */

/*!                                                                 */
/* 2. Patch address                                                 */
/*                                                                 !*/
if ( dbglev(SURPAC) == 1 ) 
  {
fprintf(dbgfil(SURPAC),
"sur223 Printout of patch data for p_patl= %d\n", (int)p_patl);
  }

/*!                                                                 */
/* 3. Spine data                                                    */
/*                                                                 !*/

if ( dbglev(SURPAC) == 1 ) 
  {
fprintf(dbgfil(SURPAC),
"sur223 Spine addresses &p_patl->spine= %d &p_patl->spine.c0y= %d\n"
     ,(int)&p_patl->spine,  (int)&p_patl->spine.c0y);

fprintf(dbgfil(SURPAC),
"sur223 Coefficients: p_patl->spine.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur223 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->spine.c0x,p_patl->spine.c1x,
        p_patl->spine.c2x,p_patl->spine.c3x );
fprintf(dbgfil(SURPAC),
"sur223 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->spine.c0y,p_patl->spine.c1y,
        p_patl->spine.c2y,p_patl->spine.c3y );
fprintf(dbgfil(SURPAC),
"sur223 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->spine.c0z,p_patl->spine.c1z,
        p_patl->spine.c2z,p_patl->spine.c3z );
fprintf(dbgfil(SURPAC),
"sur223 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->spine.c0 ,p_patl->spine.c1 ,
        p_patl->spine.c2 ,p_patl->spine.c3  );
fprintf(dbgfil(SURPAC),
"sur223 offset .ofs= %f  Type .typ= %d Length .sl= %f\n"
       ,p_patl->spine.ofs,(int)p_patl->spine.typ,p_patl->spine.sl );
}
if ( dbglev(SURPAC) == 1 ) 
  {
fprintf(dbgfil(SURPAC),
"sur223 Coefficients: p_patl->lims.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur223 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->lims.c0x,p_patl->lims.c1x,
        p_patl->lims.c2x,p_patl->lims.c3x );
fprintf(dbgfil(SURPAC),
"sur223 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->lims.c0y,p_patl->lims.c1y,
        p_patl->lims.c2y,p_patl->lims.c3y );
fprintf(dbgfil(SURPAC),
"sur223 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->lims.c0z,p_patl->lims.c1z,
        p_patl->lims.c2z,p_patl->lims.c3z );
fprintf(dbgfil(SURPAC),
"sur223 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->lims.c0 ,p_patl->lims.c1 ,
        p_patl->lims.c2 ,p_patl->lims.c3  );
fprintf(dbgfil(SURPAC),
"sur223 offset .ofs= %f  Type .typ= %d Length .sl= %f\n"
       ,p_patl->lims.ofs,(int)p_patl->lims.typ,p_patl->lims.sl );
fprintf(dbgfil(SURPAC),
"sur223 .spek_gm %d .spek2_gm %d .subtyp= %d\n"
       ,(int)p_patl->lims.spek_gm,(int)p_patl->lims.spek2_gm,(int)p_patl->lims.subtyp );
}
if ( dbglev(SURPAC) == 1 ) 
  {
fprintf(dbgfil(SURPAC),
"sur223 Coefficients: p_patl->lime.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur223 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->lime.c0x,p_patl->lime.c1x,
        p_patl->lime.c2x,p_patl->lime.c3x );
fprintf(dbgfil(SURPAC),
"sur223 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->lime.c0y,p_patl->lime.c1y,
        p_patl->lime.c2y,p_patl->lime.c3y );
fprintf(dbgfil(SURPAC),
"sur223 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->lime.c0z,p_patl->lime.c1z,
        p_patl->lime.c2z,p_patl->lime.c3z );
fprintf(dbgfil(SURPAC),
"sur223 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->lime.c0 ,p_patl->lime.c1 ,
        p_patl->lime.c2 ,p_patl->lime.c3  );
fprintf(dbgfil(SURPAC),
"sur223 offset .ofs= %f  Type .typ= %d Length .sl= %f\n"
       ,p_patl->lime.ofs,(int)p_patl->lime.typ,p_patl->lime.sl );
fprintf(dbgfil(SURPAC),
"sur223 .spek_gm %d .spek2_gm %d .subtyp= %d\n"
       ,(int)p_patl->lime.spek_gm,(int)p_patl->lime.spek2_gm,(int)p_patl->lime.subtyp );
  }
if ( dbglev(SURPAC) == 1 ) 
  {
fprintf(dbgfil(SURPAC),
"sur223 Coefficients: p_patl->tans.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur223 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->tans.c0x,p_patl->tans.c1x,
        p_patl->tans.c2x,p_patl->tans.c3x );
fprintf(dbgfil(SURPAC),
"sur223 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->tans.c0y,p_patl->tans.c1y,
        p_patl->tans.c2y,p_patl->tans.c3y );
fprintf(dbgfil(SURPAC),
"sur223 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->tans.c0z,p_patl->tans.c1z,
        p_patl->tans.c2z,p_patl->tans.c3z );
fprintf(dbgfil(SURPAC),
"sur223 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->tans.c0 ,p_patl->tans.c1 ,
        p_patl->tans.c2 ,p_patl->tans.c3  );
fprintf(dbgfil(SURPAC),
"sur223 offset .ofs= %f  Type .typ= %d Length .sl= %f\n"
       ,p_patl->tans.ofs,(int)p_patl->tans.typ,p_patl->tans.sl );
fprintf(dbgfil(SURPAC),
"sur223 .spek_gm %d .spek2_gm %d .subtyp= %d\n"
       ,(int)p_patl->tans.spek_gm,(int)p_patl->tans.spek2_gm,(int)p_patl->tans.subtyp );
  }
if ( dbglev(SURPAC) == 1 ) 
  {
fprintf(dbgfil(SURPAC),
"sur223 Coefficients: p_patl->tane.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur223 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->tane.c0x,p_patl->tane.c1x,
        p_patl->tane.c2x,p_patl->tane.c3x );
fprintf(dbgfil(SURPAC),
"sur223 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->tane.c0y,p_patl->tane.c1y,
        p_patl->tane.c2y,p_patl->tane.c3y );
fprintf(dbgfil(SURPAC),
"sur223 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->tane.c0z,p_patl->tane.c1z,
        p_patl->tane.c2z,p_patl->tane.c3z );
fprintf(dbgfil(SURPAC),
"sur223 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->tane.c0 ,p_patl->tane.c1 ,
        p_patl->tane.c2 ,p_patl->tane.c3  );
fprintf(dbgfil(SURPAC),
"sur223 offset .ofs= %f  Type .typ= %d Length .sl= %f\n"
       ,p_patl->tane.ofs,(int)p_patl->tane.typ,p_patl->tane.sl );
fprintf(dbgfil(SURPAC),
"sur223 .spek_gm %d .spek2_gm %d .subtyp= %d\n"
       ,(int)p_patl->tane.spek_gm,(int)p_patl->tane.spek2_gm,(int)p_patl->tane.subtyp );
  }
if ( dbglev(SURPAC) == 1 ) 
  {
fprintf(dbgfil(SURPAC),
"sur223 Coefficients: p_patl->pval.c0x ... c3z ... c3  \n");
fprintf(dbgfil(SURPAC),
"sur223 c0x %f c1x %f c2x %f c3x %f \n"
       ,p_patl->pval.c0x,p_patl->pval.c1x,
        p_patl->pval.c2x,p_patl->pval.c3x );
fprintf(dbgfil(SURPAC),
"sur223 c0y %f c1y %f c2y %f c3y %f \n"
       ,p_patl->pval.c0y,p_patl->pval.c1y,
        p_patl->pval.c2y,p_patl->pval.c3y );
fprintf(dbgfil(SURPAC),
"sur223 c0z %f c1z %f c2z %f c3z %f \n"
       ,p_patl->pval.c0z,p_patl->pval.c1z,
        p_patl->pval.c2z,p_patl->pval.c3z );
fprintf(dbgfil(SURPAC),
"sur223 c0  %f c1  %f c2  %f c3  %f \n"
       ,p_patl->pval.c0 ,p_patl->pval.c1 ,
        p_patl->pval.c2 ,p_patl->pval.c3  );
fprintf(dbgfil(SURPAC),
"sur223 offset .ofs= %f  Type .typ= %d Length .sl= %f\n"
       ,p_patl->pval.ofs,(int)p_patl->pval.typ,p_patl->pval.sl );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }

if ( dbglev(SURPAC) == 1 ) 
  {
t_l = 0.0;
   status=GE107(&scur,&p_patl->spine,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_priconloft (pripat)");
        return(varkon_erpush("SU2943",errbuf));
        }
fprintf(dbgfil(SURPAC),
"sur223 Spine start point   %f %f %f \n",
        out[0], out[1], out[2]  );
fprintf(dbgfil(SURPAC),
"sur223 Spine start tangent %f %f %f \n",
        out[3], out[4], out[5]  );
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
 

t_l = 1.0;
   status=GE107(&scur,&p_patl->spine,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_priconloft (pripat)");
        return(varkon_erpush("SU2943",errbuf));
        }

fprintf(dbgfil(SURPAC),
"sur223 Spine  end   point   %f %f %f\n",
        out[0], out[1], out[2] );
#ifdef TILLF
fprintf(dbgfil(SURPAC),
"sur223 Spine  end   tangent %f %f %f \n",
        out[3], out[4], out[5]  );
#endif

}          

if ( dbglev(SURPAC) == 1 ) 
  {
t_l = 0.0;
   status=GE107(&scur,&p_patl->pval ,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_priconloft (pripat)");
        return(erpush("SU2943",errbuf));
        }
fprintf(dbgfil(SURPAC),
"sur223 Pvalue start point   %f %f %f \n",
        out[0], out[1], out[2]  );
#ifdef TILLF
fprintf(dbgfil(SURPAC),
"sur223 Pvalue start tangent %f %f %f \n",
        out[3], out[4], out[5]  );
#endif
 

t_l = 1.0;
   status=GE107(&scur,&p_patl->pval ,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_pat_priconloft (pripat)");
        return(varkon_erpush("SU2943",errbuf));
        }

fprintf(dbgfil(SURPAC),
"sur223 Pvalue end   point   %f %f %f\n",
        out[0], out[1], out[2] );
#ifdef TILLF
fprintf(dbgfil(SURPAC),
"sur223 Pvalue end   tangent %f %f %f \n",
        out[3], out[4], out[5]  );
#endif

}          

#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

