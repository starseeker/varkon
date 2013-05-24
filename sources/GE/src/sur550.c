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
/*  Function: varkon_vda_rsurm                       File: sur550.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Read data for a surface from a VDA-FS text file and create      */
/*  a POL_SUR, i.e. a surface with P3_PAT, P5_PAT, P7_PAT,          */
/*  P9_PAT and/or P21_PAT patches.                                  */
/*                                                                  */
/*  The function allocates memory area for the patch data. This     */
/*  area must be deallocated by the calling function.               */
/*                                                                  */
/*  Author: Gunnar Liden                                            */ 
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-11-01   Originally written                                 */
/*  1996-11-13   Error handling, comments added                     */
/*  1996-11-14   Surface type from sur554                           */
/*  1999-12-04   Free source code modifications                     */
/*  2001-07-12   rest line with possible parameter values added     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_vda_rsurm      From VDAFS surface to POL_SUR    */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_gmsur    * Initialize DBSurf                          */
/* varkon_ini_gmpat    * Initialize DBPatch                         */
/* varkon_vda_rsur1    * No. of patches in VDA-FS surface           */
/* varkon_vda_rsur2    * Parameter values  VDA-FS surface           */
/* varkon_vda_rsur4    * Coefficients for  VDA-FS surface           */
/* varkon_sur_bound    * Bound. boxes & cones for surface           */
/* varkon_comptol      * Retrieve computer tolerance                */
/* varkon_erpush       * Error message to terminal                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2861 = Patch iu= iv= of imported surface is not OK ( fctn .. ) */
/* SU2943 = Called function .. failed in varkon_vda_rsurm  (sur550) */
/* SU2993 = Severe program error (  ) in varkon_vda_rsurm  (sur550) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_vda_rsurm (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
  char     *infile,      /* Surface file with NMG F07 bicubic data  */
  DBint     sur_no,      /* Surface number in the VDA-FS file       */
  DBSurf   *p_sur,       /* Output surface                    (ptr) */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
  DBPatch **pp_pat,      /* Alloc. area for topol. patch data (ptr) */
                         /* This function allocates the area        */
  char      sur_name[] ) /* Surface name                            */
/*                                                                  */
/* Data to p_sur and pp_pat (topological and geometry patch data)   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!----- Description of the data in the VDA-FS file ----------------*/
/*                                                                  */
/*  The order of the patches:                                       */
/*  -------------------------                                       */
/*                                                                  */
/*  IU=1,IV=NV                              IU=NU,IV=NV             */
/*   -----------------------------------------                      */
/*   !     !      !      !      !     !      !                      */
/*   ! 13  ! 14   ! 15   ! 16   ! 17  ! 18   !                      */
/*   -----------------------------------------                      */
/*   !     !      !      !      !     !      !                      */
/*   !  7  !  8   !  9   ! 10   ! 11  ! 12   !                      */
/*   ----------------------------------------                       */
/*   !     !      !      !      !     !      !                      */
/*   !  1  !  2   !  3   !  4   !  5  !  6   !                      */
/*   ----------------------------------------                       */
/*                                                                  */
/*  IU=1,IV=1                                IU=NU,IV=1             */
/*                                                                  */
/* (Loop index iu= IU-1 and iv= IV-1 in program below)              */
/*                                                                  */
/*                                                                  */
/*  The order of the coefficients for a patch:                      */
/*  -----------------------------------------                       */
/*                                                                  */
/*  Line Coefficients   (    for    ) Coefficients   (    for    )  */
/*     .. to be added ...                                           */
/*                                                                  */
/*----------------------------------------------------------------- */

/*!----- Description of the VARKON data area for the surface -------*/
/*                                                                  */
/*  The order of the patches:                                       */
/*  -------------------------                                       */
/*                                                                  */
/*  IU=1,IV=NV                              IU=NU,IV=NV             */
/*   -----------------------------------------                      */
/*   !     !      !      !      !     !      !                      */
/*   !  3  !  6   !  9   ! 12   ! 15  ! 18   !                      */
/*   -----------------------------------------                      */
/*   !     !      !      !      !     !      !                      */
/*   !  2  !  5   !  8   ! 11   ! 14  ! 17   !                      */
/*   ----------------------------------------                       */
/*   !     !      !      !      !     !      !                      */
/*   !  1  !  4   !  7   ! 10   ! 13  ! 16   !                      */
/*   ----------------------------------------                       */
/*                                                                  */
/*  IU=1,IV=1                                IU=NU,IV=1             */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!New-Page--------------------------------------------------------!*/

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint   nu;           /* Number of patches in U direction        */
   DBint   nv;           /* Number of patches in V direction        */
   char   *p_tpat;       /* Allocated area topol. patch data  (ptr) */
   DBPatch *p_frst;      /* Pointer to the first patch              */
   FILE   *f_vda;        /* Input VDA-FS file                 (ptr) */
   char    rest[132];    /* Substring with possible parameter values*/
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint  ntot;          /* Total number of patches                 */
   DBint  i_s;           /* Loop index surface patch record         */
   DBint  sur_type;      /* Surface type                            */

   DBint  acc;           /* Calc. accuracy (case) for BBOX & BCONE  */
                         /* Eq. 0: BBOX and BCONE                   */
                         /* Eq. 1: Only BBOX                        */

   DBfloat comptol;       /* Computer tolerance (accuracy)           */
   char   errbuf[80];    /* String for error message fctn erpush    */

   DBint  status;        /* Error code from a called function       */


/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur550 Enter*varkon_vda_rsurm * infile %s \n", infile );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur550 Requested surface sur_no %d\n",
                         (short)sur_no );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

  *pp_pat = NULL;

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* Initialize surface data in p_sur.                                */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/
    varkon_ini_gmsur (p_sur);

    strcpy(rest,"Undefined");

/*!                                                                 */
/* Open the VDA-FS file (C function fopen)                          */
/*                                                                 !*/

if ( (f_vda= fopen(infile,"r")) == NULL )
 {
 sprintf(errbuf, "%s %% ",infile );
 return(varkon_erpush("SU5503",errbuf));
 }   

/*!                                                                 */
/* Get number of patches. Call varkon_vda_rsur1 (sur551).           */
/*                                                                 !*/

   status=varkon_vda_rsur1 (f_vda,sur_no,&nu,&nv,sur_name,rest);
    if (status < 0) return(status);

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur550 sur_name %s nu %d nv %d\n",sur_name,(short)nu,(short)nv);
}
#endif


/*!                                                                 */
/* Dynamic allocation of area (pp_pat) for the topological data.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

  ntot = nu*nv;

if ((p_tpat=DBcreate_patches(TOP_PAT,ntot))==NULL)
 { 
 sprintf(errbuf, "(alloc)%%sur550");
 return(varkon_erpush("SU2993",errbuf));
 }

p_frst = (DBPatch*)p_tpat;               /* Pointer to first patch  */
*pp_pat = p_frst;                        /* Output pointer          */

/*!                                                                 */
/* Initialize patch data in pp_pat.                                 */
/* Calls of varkon_ini_gmpat  (sur768).                             */
/*                                                                 !*/

   for (i_s=1; i_s<= ntot; i_s= i_s+1)
     {
     varkon_ini_gmpat (p_frst  +i_s-1);
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur550 Area has been allocated for ntot= %d patches\n",(short)ntot);
fflush(dbgfil(SURPAC)); 
}
#endif


/*!                                                                 */
/* Get parameter values.  Call varkon_vda_rsur2 (sur552).           */
/*                                                                 !*/

   status=varkon_vda_rsur2 ( f_vda, rest, nu,  nv, p_frst );
    if (status<0) 
        {
        sprintf(errbuf,"sur552%%sur550");
        return(varkon_erpush("SU2943",errbuf));
        }


/*!                                                                 */
/* Get coefficients and surface type.                               */
/* Call varkon_vda_rsur4 (sur554).                                  */
/*                                                                 !*/

   status=varkon_vda_rsur4 ( f_vda, nu,  nv, p_frst, &sur_type );
    if (status<0) 
        {
        sprintf(errbuf,"sur554%%sur550");
        return(varkon_erpush("SU2943",errbuf));
        }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur550 Coefficients retrieved \n");
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur550 Surface type %d\n",(int)sur_type);
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Close the VDA-FS  file (C function fclose)                       */
/*                                                                 !*/

 fclose(f_vda);


/*!                                                                 */
/* Surface type and number of patches in U and V to p_sur           */
/*                                                                 !*/

   p_sur->typ_su = (short)sur_type;
   p_sur->nu_su  = (short)nu; 
   p_sur->nv_su  = (short)nv;



#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur550 VDA-FS file closed \n");
fflush(dbgfil(SURPAC)); 
}
#endif


/*!                                                                 */
/*  Calculate bounding boxes and cones                              */
/*  Call of varkon_sur_bound (sur911).                              */
/*                                                                 !*/

   acc = 0;         /* Calculation of BBOX and BCONE                */

   status=varkon_sur_bound (p_sur,p_frst,acc);
    if (status<0) 
        {
        sprintf(errbuf,"sur911%%sur550");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur550 Bounding boxes and cones calculated\n");
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), "sur550 Exit \n");
fflush(dbgfil(SURPAC)); 
}
#endif


    return(SUCCED);

  }

/*********************************************************/
