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
/*  Function: varkon_vda_rsur3                       File: sur553.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Retrieve all float values from one line                         */
/*                                                                  */
/*  Author: Gunnar Liden                                            */ 
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-11-01   Originally written                                 */
/*  1999-12-04   Free source code modifications                     */
/*  2001-06-04   Bug when line shorter that 72 (i_max added)        */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_vda_rsur3      Retrieve floats from VDA-FS line */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush       * Error message to terminal                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error (  ) in varkon_vda_rsur3  (sur553) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_vda_rsur3 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
  char     rad[],        /* One line in the VDA-FS file             */
  DBint   *p_n_flt,      /* Number of floats in VDA-FS line   (ptr) */
  DBfloat  all_flt[] )   /* All floats                        (ptr) */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
/*                                                                  */
/* Data to                                                          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

{ /* Start of function */

/*!New-Page--------------------------------------------------------!*/

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  n_c;           /* Number of characters in the line        */
   DBint  i_max;         /* Maximum number of characters for loop   */
   DBint  i_c;           /* Loop index character in line            */
   DBint  n_comma;       /* Number of commas in the string          */
   DBint  i_pre;         /* Previous position for comma             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint  b_flag;        /* Flag for space                          */
   DBint  i_f;           /* Loop index float for initialization     */
   char   c_c[2];        /* Current character                       */
   char   c_float[80];   /* Current character                       */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* Check of input data and initializations                          */
/*                                                                 !*/

  *p_n_flt = I_UNDEF;

  for ( i_f = 0; i_f < SMAX; ++i_f )
    {
    all_flt[i_f] = F_UNDEF;
    }

  i_max = I_UNDEF;

  sprintf(c_c , "%s","U\n");

/*!                                                                 */
/* Retrieve floats from the line                                    */
/*                                                                 !*/

  n_c     = strlen(rad);
  if (  n_c > 72 ) 
     i_max = 72;
  else
     i_max = n_c - 1;

  n_comma = 0;
  i_pre   = 1;
  b_flag  = 0;

  for ( i_c = 1; i_c <=  i_max; ++i_c )
    {
    V3MOME((char *)(&rad[i_c-1]),(char *)(&c_c[0]), 1 );
    if (  strcmp(c_c," \n")  != 0  ) b_flag = 1;
    if (  (strcmp(c_c,",\n")  == 0  && b_flag == 1) ||
          (    i_c == i_max         && b_flag == 1)    )
      {
      b_flag = 0;
      V3MOME((char *)(&rad[i_pre-1]),(char *)(&c_float[0]),i_c-i_pre);
      c_float[i_c-i_pre] = '\0';
      i_pre = i_c + 1;
      sscanf(c_float,"%lf",&all_flt[n_comma]);
      n_comma = n_comma + 1;
      if  ( n_comma >= SMAX )
        {
        sprintf(errbuf,"array size all_flt%%sur553");
        return(varkon_erpush("SU2993",errbuf));
        }

      }
    }


  *p_n_flt = n_comma;


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur553 Exit*varkon_vda_rsur3 * n_c %d *p_n_flt %d all_flt[0] %f\n", 
                  (short)n_c, (short)*p_n_flt, all_flt[0] );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

  }

/*********************************************************/
