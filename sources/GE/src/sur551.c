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
#include <string.h>

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_vda_rsur1                       File: sur551.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Retrieve number of patches for the VDA-FS surface.              */
/*                                                                  */
/*  Author: Gunnar Liden                                            */ 
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-11-01   Originally written                                 */
/*  1997-04-19   max_l 10000->100000                                */
/*  1997-04-21   max_l 100000->500000                               */
/*  1998-03-31   Bug: Assumed SURF / and not SURF/ also             */
/*  1999-12-03   Free source code modifications                     */
/*  2001-07-12   rest line with possible parameter values added     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_vda_rsur1      No. of patches in VDA-FS surface */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erinit       * Initialize error message stack             */
/* varkon_erpush       * Error message to terminal                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error (  ) in varkon_vda_rsur1  (sur551) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_vda_rsur1 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
   FILE   *f_vda,        /* Input VDA-FS file                 (ptr) */
   DBint   sur_no,       /* Surface number in the VDA-FS file       */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
   DBint  *p_nu,         /* Number of patches in U direction  (ptr) */
   DBint  *p_nv,         /* Number of patches in V direction  (ptr) */
   char    sur_name[],   /* Surface name                            */
   char    rest[] )      /* Substring with possible parameter values*/

/*                                                                  */
/* Data to p_nu, p_nv and sur_name                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  i_sur;         /* Number of surfaces found in VDA-FS file */
   DBint  max_l;         /* Maximum number of lines in input file   */
   DBint  i_l;           /* Loop index line in input file           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char  *n_surf;        /* Position for SURF in a string     (ptr) */
   char  *n_equal;       /* Position for  =   in a string     (ptr) */
   char  *n_line;        /* Position for  /   in a string     (ptr) */
   char  *n_comma;       /* Position for  ,   in a string     (ptr) */
   char   rad[133];      /* One line in the VDA-FS file             */
   char   substr[133];   /* Part of rad after /                     */
   int    idum1,idum2;   /* Dummy int's for sscanf                  */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* Check of input data and initializations                          */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur551*Enter* Requested surface sur_no %d\n",
                         (short)sur_no );
fflush(dbgfil(SURPAC));
}
#endif

  *p_nu = I_UNDEF; 
  *p_nv = I_UNDEF;
  sprintf(sur_name, "%s","Undefined\n");
  sprintf(substr  , "%s","Undefined 7\n");



/*!                                                                 */
/* Loop for all lines in the input VDA-FS file. There is a maximum  */
/* of max_l= 500000 as a program check.                             */
/*                                                                 !*/

max_l   = 500000;
i_l     =      0;
n_surf  =   NULL;
n_equal =   NULL;
n_line  =   NULL;
n_comma =   NULL;
i_sur   =      0;

while ( fgets(rad,132,f_vda)  != NULL )
  {
  i_l = i_l + 1;
  n_surf = strstr(rad,"SURF");
  n_equal= strstr(rad,"=");
  n_line = strstr(rad,"/");
  
  if ( n_surf != NULL && n_equal != NULL && n_line != NULL ) 
    {
    sscanf(rad,"%s",sur_name);
    strcpy(substr,n_line+1);
    sscanf(substr,"%d", &idum1 );
    n_comma = strstr(substr,",");
    strcpy(substr,n_comma+1);
    sscanf(substr,"%d", &idum2 );
    n_comma = strstr(substr,",");
    strcpy(rest,n_comma+1);

    *p_nu = idum1;
    *p_nv = idum2;


    i_sur = i_sur + 1;
    if ( i_sur == sur_no ) goto found;
    }
  }

/*!                                                                 */
/* No surface number sur_no in the file                             */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur551 Error: Requested surface no. %d is not in the VDA-FS file\n", 
 (int)sur_no );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur551 Error: The number of surfaces in the VDA-FS file is %d\n", 
 (int)i_sur );
fflush(dbgfil(SURPAC));
}
#endif

   varkon_erinit();
   sprintf(errbuf,"%d%%(sur551)",(int)sur_no);
   return(varkon_erpush("SU5513",errbuf));

found:;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur551 *p_nu %d *p_nv %d \n", 
  (short)*p_nu,  (short)*p_nv );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur551 &rad %d n_equal %d n_surf %d \n", 
 (int)&rad[0], (int)n_equal , (int)n_surf  );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur551 Exit*varkon_vda_rsur1 * sur_name %s i_l %d \n", 
                  sur_name, (short)i_l );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

  }

/*********************************************************/
