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
/*  (C)Microform AB 2001, Gunnar Liden, gunnar@microform.se         */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"
#include <string.h>

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_sur_nurbswrite                  File: sur894.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Write NURBS data to a file                                      */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  2001-06-11   Originally written                                 */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_nurbswrite Write NURBS data to a file       */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* varkon_erinit          * Initialize error message stack          */
/* varkon_errmes          * Warning message to terminal             */
/* varkon_erpush          * Error   message to terminal             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

extern char  jobdir[];          /* Current job directory            */
extern char  jobnam[];          /* Current job name                 */

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in this function  (sur894)    */
/* SU2993 = Severe program error (  ) in this function  (sur894)    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_nurbswrite (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
  DBSurf    *p_sur,      /* Pointer to the output surface           */
  DBPatch   *p_pat,      /* Pointer to alloc. area for patch data   */
  char *     f_name_in,  /* Input file name                         */
  DBint      print_flag) /* 0: No description 1: With description   */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   char   f_name[133];   /* Full file name (with path)              */
   FILE  *f_dat;         /* Output file                       (ptr) */

   DBint    k_nu;        /* Number of values in U knot vector       */
   DBint    k_nv;        /* Number of values in V knot vector       */
   DBint    order_u;     /* Order in U direction                    */
   DBint    order_v;     /* Order in V direction                    */
   DBint    c_nuv;       /* Number of control points                */
   GMPATNU *p_patnu;     /* NURBS patch                       (ptr) */
  
/*                                                                  */
/*-----------------------------------------------------------------!*/
   DBfloat *p_uval;      /* U knot value                      (ptr) */
   DBfloat *p_vval;      /* V knot value                      (ptr) */
   DBfloat  uval;        /* U knot value                            */
   DBfloat  vval;        /* V knot value                            */
   DBHvector *p_pp;      /* Polygon point                     (ptr) */
   DBfloat  x_coord;     /* X coordinate                            */
   DBfloat  y_coord;     /* Y coordinate                            */
   DBfloat  z_coord;     /* Z coordinate                            */
   DBfloat  weight;      /* Weight                                  */
   DBint    npts_u;      /* Number of Control Points in U direction */
   DBint    npts_v;      /* Number of Control Points in V direction */
   DBint    k_iu;        /* Loop index value in U knot vector       */
   DBint    k_iv;        /* Loop index value in V knot vector       */   
   DBint    c_iuv;       /* Loop index control point                */
   DBint    nu;          /* Number of patches in U direction        */
   DBint    nv;          /* Number of patches in V direction        */
   DBint    i_u;         /* Index for Control Point in U direction  */
   DBint    i_v;         /* Index for Control Point in V direction  */
   DBint    surtype;     /* Type CUB_SUR, RAT_SUR, NURBS_SUR, or .. */
   char     c_styp[80];  /* Surface type                            */
   char     errbuf[80];  /* String for error message fctn erpush    */
   short    status;      /* Error code from called function         */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Initializations and checks                                    */
/* _____________________________                                    */
/*                                                                 !*/

/* Initialize internal variables                                    */

   k_nu      = I_UNDEF;
   k_nv      = I_UNDEF;
   order_u   = I_UNDEF;
   order_v   = I_UNDEF;
   c_nuv     = I_UNDEF;
   i_u       = I_UNDEF;
   i_v       = I_UNDEF;
   npts_u    = I_UNDEF;
   npts_v    = I_UNDEF;

   uval      = F_UNDEF;
   vval      = F_UNDEF;
   x_coord   = F_UNDEF;
   y_coord   = F_UNDEF;
   z_coord   = F_UNDEF;
   weight    = F_UNDEF;

   p_patnu    = NULL;
   p_uval     = NULL;
   p_vval     = NULL;
   p_pp       = NULL;

/*!                                                                 */
/* 2. Retrieve NURBS surface data                                   */
/* ______________________________                                   */
/*                                                                 !*/

/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/*                                                                 !*/

    status = varkon_sur_nopatch
    (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"sur230%%sur894");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

   sprintf(c_styp  ,"Undefined_surface_type");

   if ( surtype == NURB_SUR ) sprintf(c_styp,"NURB_SUR");
   else      
      { 
      sprintf(c_styp,"Surface code= %d",(int)surtype );
      }

    p_patnu= (GMPATNU *)p_pat->spek_c;
    k_nu    = (DBint)p_patnu->nk_u;
    k_nv    = (DBint)p_patnu->nk_v;

/*  Order in U and V direction                                      */
    order_u = (DBint)p_patnu->order_u;    
    order_v = (DBint)p_patnu->order_v;    

/*  Number of control points                                        */

    c_nuv =  (k_nu-order_u)*(k_nv-order_v);

/*!                                                                 */
/* Construct the full file name.                                    */
/* TODO Here is assumed that file name with extension is given      */
/*                                                                 !*/

   strcpy(f_name,jobdir);
   strcat(f_name,f_name_in);

/*!                                                                 */
/* Open file in write mode.                                         */
/*                                                                 !*/


   if ( (f_dat= fopen(f_name ,"w")) == NULL )
     {                     
     sprintf(errbuf,      
     "(open w)%%sur894"); 
     return(varkon_erpush("SU2993",errbuf));
     } 


   fprintf(f_dat, "%d ", order_u );  
   if ( 1 == print_flag ) 
      fprintf(f_dat, "      The bspline order in the U-direction \n");
   else
      fprintf(f_dat, " \n");

   fprintf(f_dat, "%d ", order_v );  
   if ( 1 == print_flag ) 
      fprintf(f_dat, "      The bspline order in the V-direction \n");
   else
      fprintf(f_dat, " \n");

   fprintf(f_dat, "%d ", k_nu );  
   if ( 1 == print_flag ) 
      fprintf(f_dat, "      Number of knots in U-direction \n");
   else
      fprintf(f_dat, " \n");

   fprintf(f_dat, "%d ", k_nv );  
   if ( 1 == print_flag ) 
      fprintf(f_dat, "      Number of knots in V-direction \n");
   else
      fprintf(f_dat, " \n");

    for ( k_iu = 1; k_iu <= k_nu; ++k_iu )
      {
      p_uval = p_patnu->kvec_u + k_iu - 1;
      uval   = *p_uval;
      fprintf(f_dat, "%f ", uval );  
      if ( 1 == print_flag ) 
         fprintf(f_dat, "      Uknot(%6d) \n",k_iu);
      else
         fprintf(f_dat, " \n");
      }

    for ( k_iv = 1; k_iv <= k_nv; ++k_iv )
      {
      p_vval  = p_patnu->kvec_v + k_iv - 1;
      vval   = *p_vval;
      fprintf(f_dat, "%f ", vval );  
      if ( 1 == print_flag ) 
         fprintf(f_dat, "      Vknot(%6d) \n",k_iv);
      else
         fprintf(f_dat, " \n");
     }


   npts_u    =  k_nu - order_u;
   npts_v    =  k_nv - order_v;
   i_u = 1;
   i_v = 0;
   for ( c_iuv = 0; c_iuv <  c_nuv; ++c_iuv )
   {
/*    Pointer to current polygon point                              */
      p_pp  = p_patnu->cpts + c_iuv;
      x_coord = p_pp->x_gm;
      y_coord = p_pp->y_gm;
      z_coord = p_pp->z_gm;
      weight  = p_pp->w_gm;
      i_v = i_v + 1;
      if ( i_v > npts_v ) 
      {
         i_u = i_u + 1;
         i_v = 1;
      }
      fprintf(f_dat, "%f %f %f %f", x_coord, y_coord, z_coord, weight );  
      if ( 1 == print_flag ) 
         fprintf(f_dat, "      ControlPoint(%6d,%6d) \n",i_u, i_v);
      else
         fprintf(f_dat, " \n");
   }



   if ( 1 == print_flag ) 
   {
     fprintf(f_dat, " \n");  
     fprintf(f_dat, " \n");  
     fprintf(f_dat, " \n");
     fprintf(f_dat, 
        "Number of Contol Points in U direction npts_u = nk_u - order_u = %d\n",
        npts_u);  
     fprintf(f_dat, 
        "Number of Contol Points in V direction npts_v = nk_v - order_v = %d\n",
        npts_v);  
     fprintf(f_dat, " \n");  
     fprintf(f_dat, "File: %s \n", f_name );  
     fprintf(f_dat, " \n");  
     fprintf(f_dat, "Job name:      %s \n", jobnam );  
     fprintf(f_dat, "Job directory: %s \n", jobdir );  
     fprintf(f_dat, " \n");  
     fprintf(f_dat,"Surface type: %s  \n", c_styp );
     fprintf(f_dat, " \n");  
/*
     fprintf(f_dat,"Number of patches nu= %d nv= %d\n",
                    (int)nu, (int)nv );
     fprintf(f_dat, " \n");  
*/
   }

/*!                                                                 */
/* Close the output file (C function fclose)                        */
/*                                                                 !*/

 fclose(f_dat); 




    return(SUCCED);

  }

/*********************************************************/

