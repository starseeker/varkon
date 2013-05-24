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
/*  Function: varkon_sur_transf                    File: sur610.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Transforms a surface with the input transformation              */
/*  matrix (the input coordinate system).                           */
/*                                                                  */
/*  The input surface will not be copied. The input surface         */
/*  (patch) coefficients will be modified. Also the bounding        */
/*  box and cone data will be modified.                             */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-06-06   Originally written                                 */
/*  1996-01-04   Consurf added                                      */
/*  1996-10-29   Polynomial surfaces added                          */
/*  1997-12-04   Facet and NURBS added                              */
/*  1999-11-30   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_transf     Transform a surface              */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_bictra      * Transform bicubic     surface           */
/* varkon_sur_rbictra     * Transform rational    surface           */
/* varkon_sur_lofttra     * Transform conic loft. surface           */
/* varkon_sur_contra      * Transform Consurf     surface           */
/* varkon_sur_factra      * Transform facet       surface           */
/* varkon_sur_nurbtra     * Transform NURBS       surface           */
/* varkon_sur_poltra      * Transform polynomial  surface           */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_transf (sur620) */
/* SU2993 = Severe program error (  ) in varkon_sur_transf (sur620) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_transf (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Input/output surface              (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  DBTmat  *p_c )         /* Transform. matrix (local system)  (ptr) */

/* Out:                                                             */
/*        Coefficients of surface is modified                       */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

   DBint  status;        /* Error code from called function         */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/* Return if input system p_c not is defined (p_c= NULL)            */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur610 Enter*varkon_sur_transf** p_pat %d\n",(int)p_pat);
  fprintf(dbgfil(SURPAC),"sur610 Input system p_c=%d\n",(int)p_c);
 fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
#endif
 
if ( p_c == NULL ) return(SUCCED);       /* No transformation if    */
                                         /* system is undefined     */

/*!                                                                 */
/* Transformation of the surface:                                   */
/* Call of varkon_sur_bictra  (sur600) for CUB_SUR                  */
/* Call of varkon_sur_rbitra  (sur602) for RAT_SUR                  */
/*   .                                                              */
/*   .                                                              */
/*   .                                                              */
/*                                                                 !*/

    switch ( p_sur->typ_su )
      {
      case CUB_SUR:
      status = varkon_sur_bictra (p_sur,p_pat ,p_c);
      break;

      case RAT_SUR:
      status = varkon_sur_rbictra (p_sur,p_pat ,p_c);
      break;

      case LFT_SUR:
      status = varkon_sur_lofttra (p_sur,p_pat ,p_c);
      break;

      case CON_SUR:
      status = varkon_sur_contra (p_sur,p_pat ,p_c);
      break;

      case FAC_SUR:
      status = varkon_sur_factra (p_sur,p_pat ,p_c);
      break;

      case NURB_SUR:
      status = varkon_sur_nurbtra (p_sur,p_pat ,p_c);
      break;

      case POL_SUR:
      status = varkon_sur_poltra (p_sur,p_pat ,p_c);
      break;

      default:
      sprintf(errbuf,"Type %d",p_sur->typ_su);
      return(varkon_erpush("SU2993",errbuf));
      break;
      }

#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_....sur%%varkon_sur_transf (sur610)");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


/* 3. Exit                                                          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur610 Exit ****** varkon_sur_transf ********\n");
 fflush(dbgfil(SURPAC)); 
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
