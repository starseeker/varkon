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
/*  Function: varkon_vda_rsur4                       File: sur554.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Retrieve coefficients for the VDA-FS surface.                   */
/*                                                                  */
/*  Author: Gunnar Liden                                            */ 
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-11-02   Originally written                                 */
/*  1996-11-13   idum1, idum2 for sscanf                            */
/*  1996-11-14   Surface type                                       */
/*  1999-12-04   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_vda_rsur4      Coefficients for VDA-FS surface  */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_vda_rp3      * Get  P3_PAT data from VDA-FS file          */
/* varkon_vda_rp5      * Get  P5_PAT data from VDA-FS file          */
/* varkon_vda_rp7      * Get  P7_PAT data from VDA-FS file          */
/* varkon_vda_rp9      * Get  P9_PAT data from VDA-FS file          */
/* varkon_vda_rp21     * Get P21_PAT data from VDA-FS file          */
/* varkon_erpush       * Error message to terminal                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error (  ) in varkon_vda_rsur4  (sur554) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_vda_rsur4 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
   FILE    *f_vda,       /* Input VDA-FS file                 (ptr) */
   DBint    nu,          /* Number of patches in U direction  (ptr) */
   DBint    nv,          /* Number of patches in V direction  (ptr) */
   DBPatch *p_pat,       /* Topological patch data            (ptr) */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
   DBint   *p_sur_type ) /* Surface type                      (ptr) */
/*                                                                  */
/* Data to                                                          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

{ /* Start of function */

/*!New-Page--------------------------------------------------------!*/

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    iu;          /* Loop index U patch                      */
   DBint    iv;          /* Loop index V patch                      */
   DBint    n_degu;      /* Degree in U direction                   */
   DBint    n_degv;      /* Degree in V direction                   */
   DBPatch *p_t;         /* Current patch                     (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char     w_1[80];     /* Dummy word 1 from line                  */
   char     w_2[80];     /* Dummy word 2 from line                  */
   char     rad[133];    /* One line in the VDA-FS file             */
   int      idum1,idum2; /* Dummy int's for sscanf                  */
   DBint    sur_type;    /* Surface type                            */
   DBint    status;      /* Error code from a called function       */
   char     errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* Check of input data and initializations                          */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),"sur554*Enter*  \n");
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

  n_degu   = I_UNDEF;
  n_degv   = I_UNDEF;
  sur_type = I_UNDEF;

  sprintf(w_1, "%s","Undefined 1\n");
  sprintf(w_2, "%s","Undefined 2\n");

/*!                                                                 */
/*  Loop for all patches in the surface                             */
/*                                                                 !*/


for ( iv=0; iv<nv; ++iv )                /* Start loop V patches    */
  {
  for ( iu=0; iu<nu; ++iu )              /* Start loop U patches    */
    {

/*!                                                                 */
/*      i. Current patch pointer p_t                                */
/*                                                                 !*/

    p_t           = p_pat  + iu*nv + iv; 

/*!                                                                 */
/*     ii. Retrieve degree of patch                                 */
/*                                                                 !*/

    if ( fgets(rad,132,f_vda)  != NULL )
      {
      sscanf(rad,"%d%s%d%s", &idum1 , w_1, &idum2 , w_2 );
      n_degu = idum1;
      n_degv = idum2;
      }

    if ( strcmp(w_1,",") == 0 && strcmp(w_2,",") == 0 )
      {
      ; /*  OK  */
      }
    else
      {
      sprintf(errbuf,"n_degu/n_degv%%sur554");
      return(varkon_erpush("SU2993",errbuf));
      }

    n_degu = n_degu - 1;
    n_degv = n_degv - 1;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur554 iu %d iv %d n_degu %d n_degv %d\n",
   (short)iu, (short)iv, (short)n_degu, (short)n_degv);
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/*    iii. Get coefficients                                         */
/*         Call varkon_vda_rp3   (sur555) if degree is  3           */
/*         Call varkon_vda_rp5   (sur556) if degree is  5           */
/*         Call varkon_vda_rp7   (sur557) if degree is  7           */
/*         Call varkon_vda_rp9   (sur558) if degree is  9           */
/*         Call varkon_vda_rp21  (sur559) if degree is 21           */
/*                                                                 !*/

    if      (  n_degu == 3 && n_degv == 3 )
      {
      if ( sur_type == I_UNDEF ) 
        {
        sur_type = P3_SUR;
        }
      if (sur_type != I_UNDEF && sur_type !=  P3_SUR)sur_type = POL_SUR;
      
      status=varkon_vda_rp3 ( f_vda, n_degu, n_degv, p_t );
       if (status<0) 
           {
           sprintf(errbuf,"sur555%%sur554");
           return(varkon_erpush("SU2943",errbuf));
           }

        }   /*  Degree 3 */
    else if (  n_degu <= 5 && n_degv <= 5 )
      {
      if ( sur_type == I_UNDEF ) 
        {
        sur_type = P5_SUR;
        }
      if (sur_type != I_UNDEF && sur_type !=  P5_SUR)sur_type = POL_SUR;

      status=varkon_vda_rp5 ( f_vda, n_degu, n_degv, p_t );
       if (status<0) 
           {
           sprintf(errbuf,"sur556%%sur554");
           return(varkon_erpush("SU2943",errbuf));
           }

        }   /*  Degree 5 */
    else if (  n_degu <= 7 && n_degv <= 7 )
      {
      if ( sur_type == I_UNDEF ) 
        {
        sur_type = P7_SUR;
        }
      if (sur_type != I_UNDEF && sur_type !=  P7_SUR)sur_type = POL_SUR;

      status=varkon_vda_rp7 ( f_vda, n_degu, n_degv, p_t );
       if (status<0) 
           {
           sprintf(errbuf,"sur557%%sur554");
           return(varkon_erpush("SU2943",errbuf));
           }

        }   /*  Degree 7 */
    else if (  n_degu <= 9 && n_degv <= 9 )
      {
      if ( sur_type == I_UNDEF ) 
        {
        sur_type = P9_SUR;
        }
      if (sur_type != I_UNDEF && sur_type !=  P9_SUR)sur_type = POL_SUR;

      status=varkon_vda_rp9 ( f_vda, n_degu, n_degv, p_t );
       if (status<0) 
           {
           sprintf(errbuf,"sur558%%sur554");
           return(varkon_erpush("SU2943",errbuf));
           }

        }   /*  Degree 9 */
    else if (  n_degu <= 21 && n_degv <= 21 )
      {
      if ( sur_type == I_UNDEF ) 
        {
        sur_type = P21_SUR;
        }
      if (sur_type != I_UNDEF && sur_type != P21_SUR)sur_type = POL_SUR;

      status=varkon_vda_rp21 ( f_vda, n_degu, n_degv, p_t );
       if (status<0) 
           {
           sprintf(errbuf,"sur559%%sur554");
           return(varkon_erpush("SU2943",errbuf));
           }

        }   /*  Degree 21 */


      else
        {
        sprintf(errbuf,"Degree not impl.%%sur554");
        return(varkon_erpush("SU2993",errbuf));
        }


     }     /* End iu */
  }        /* End iv */

  *p_sur_type = sur_type;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && sur_type ==  P3_SUR )
{
fprintf(dbgfil(SURPAC),
"sur554 Exit Surface is of type P3_SUR (%d)\n", (int)sur_type);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 && sur_type ==  P5_SUR )
{
fprintf(dbgfil(SURPAC),
"sur554 Exit Surface is of type P5_SUR (%d)\n", (int)sur_type);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 && sur_type ==  P7_SUR )
{
fprintf(dbgfil(SURPAC),
"sur554 Exit Surface is of type P7_SUR (%d)\n", (int)sur_type);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 && sur_type ==  P9_SUR )
{
fprintf(dbgfil(SURPAC),
"sur554 Exit Surface is of type P9_SUR (%d)\n", (int)sur_type);
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 && sur_type == P21_SUR )
{
fprintf(dbgfil(SURPAC),
"sur554 Exit Surface is of type P21_SUR (%d)\n", (int)sur_type);
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

  }

/*********************************************************/
