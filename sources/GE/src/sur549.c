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
/*  Function: varkon_vda_nosur                       File: sur549.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Retrieve number of surfaces in a VDA-FS file.                   */
/*                                                                  */
/*  Remark: The code in this function is almost identical to        */
/*          varkon_sur_rsur1 (sur551).                              */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */ 
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-04-19   Originally written                                 */
/*  1997-04-21   100000->500000 lines                               */
/*  1998-03-31   Bug: Assumed SURF / and not SURF/ also             */
/*  1999-12-04   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_vda_nosur      No. of surfaces in a VDA-FS file */
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
/* SU2993 = Severe program error (  ) in varkon_vda_nosur  (sur549) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
        DBstatus  varkon_vda_nosur (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
  char    *infile,       /* Full name of file with VDA-FS data      */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
  DBint   *p_no_sur )    /* Number of surfaces in infile      (ptr) */
/*                                                                  */
/* Data to p_no_sur                                                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

{ /* Start of function */

/*!New-Page--------------------------------------------------------!*/

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  i_sur;         /* Number of surfaces found in VDA-FS file */
   DBint  max_l;         /* Maximum number of lines in input file   */
   DBint  i_l;           /* Loop index line in input file           */
   FILE  *f_vda;         /* Input VDA-FS file                 (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char  *n_surf;        /* Position for SURF in a string     (ptr) */
   char  *n_equal;       /* Position for  =   in a string     (ptr) */
   char  *n_line;        /* Position for  /   in a string     (ptr) */
   char   sur_name[80];  /* Surface name                            */
   char   w_1[80];       /* Dummy word 1 from line                  */
   char   w_2[80];       /* Dummy word 2 from line                  */
   char   w_3[80];       /* Dummy word 3 from line                  */
   char   w_4[80];       /* Dummy word 4 from line                  */
   char   w_5[80];       /* Dummy word 5 from line                  */
   char   w_6[80];       /* Dummy word 6 from line                  */
   char   rad[133];      /* One line in the VDA-FS file             */
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
fprintf(dbgfil(SURPAC),
"sur549 Enter*varkon_vda_nosur * infile %s\n", infile );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Initialize output variable                                       */
/*                                                                 !*/
  *p_no_sur = I_UNDEF;

/*!                                                                 */
/* Open the VDA-FS file (C function fopen)                          */
/*                                                                 !*/

if ( (f_vda= fopen(infile,"r")) == NULL )
 {
 sprintf(errbuf, "%s %% ",infile );
 varkon_erinit();
 return(varkon_erpush("SU5503",errbuf));
 }   

/*!                                                                 */
/* Initialize local variables                                       */
/*                                                                 !*/

  sprintf(sur_name, "%s","Undefined\n");
  sprintf(w_1     , "%s","Undefined 1\n");
  sprintf(w_2     , "%s","Undefined 2\n");
  sprintf(w_3     , "%s","Undefined 3\n");
  sprintf(w_4     , "%s","Undefined 4\n");
  sprintf(w_5     , "%s","Undefined 5\n");
  sprintf(w_6     , "%s","Undefined 6\n");

/*!                                                                 */
/* Loop for all lines in the input VDA-FS file. There is a maximum  */
/* of max_l= 100000 as a program check.                             */
/*                                                                 !*/

max_l   = 500000;
i_l     =      0;
n_surf  =   NULL;
n_equal =   NULL;
n_line  =   NULL;
i_sur   =      0;

while ( fgets(rad,132,f_vda)  != NULL )
  {
  i_l = i_l + 1;
  n_surf = strstr(rad,"SURF");
  n_equal= strstr(rad,"=");
  n_line = strstr(rad,"/");
  
  if ( n_surf != NULL && n_equal != NULL && n_line != NULL ) 
    {
    i_sur = i_sur + 1;
    }
  }

/*!                                                                 */
/* Close the VDA-FS  file (C function fclose)                       */
/*                                                                 !*/

 fclose(f_vda);

/*!                                                                 */
/* Check if the maximum number of lines in the file is exceeded     */
/*                                                                 !*/

  if ( i_l >= max_l ) 
    {
    sprintf(errbuf,"No. of lines > %d%%sur549",(int)max_l);
    varkon_erinit();
    return(varkon_erpush("SU2993",errbuf));
    }

/*!                                                                 */
/* Number of surfaces to output variable                            */
/*                                                                 !*/

  *p_no_sur = i_sur;


#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur549 w_1 %s w_2 %s w_3 %s w_4 %s w_5 %s w_6 %s\n", 
 w_1, w_2, w_3, w_4, w_5, w_6 );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur549 &rad %d n_equal %d n_surf %d i_l %d\n", 
 (int)&rad[0], (int)n_equal , (int)n_surf, (short)i_l  );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur549 Exit Number of surfaces *p_no_sur= %d in %s\n", 
                  (short)*p_no_sur, infile );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

  }

/*********************************************************/
