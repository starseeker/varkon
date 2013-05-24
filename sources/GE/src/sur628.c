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
/*  Function: varkon_pat_factra                    File: sur628.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Transforms a facet patch with the input transformation matrix   */
/*  (the input coordinate system).                                  */
/*                                                                  */
/*  The input patch will not be copied if the input and output      */
/*  patch addresses are equal.                                      */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-12-04   Originally written                                 */
/*  1999-11-28   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_factra     Transform a facet patch          */
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* GEtfpos_to_local       * Transform a point                       */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_pat_factra (sur628)  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_pat_factra (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  GMPATF *p_patin,       /* Input patch                       (ptr) */
  DBTmat *p_c,           /* Transformation matrix             (ptr) */
  GMPATF *p_patout )     /* Output patch                      (ptr) */

/* Out:                                                             */
/*        Transformed points to p_patout                            */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

   DBVector poi_in;      /* Input point for GEtfpos_to_local        */
   DBVector poi_tra;     /* Transformed point                       */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur628 Enter*varkon_pat_factra**\n");
  }
#endif
 
/*!                                                                 */
/*  Check transformation matrix p_c.  Error SU2993 if not defined   */
/*                                                                 !*/

if ( p_c == NULL )
 {               
 sprintf(errbuf, "(p_c)%%varkon_pat_factra (sur628"); 
 return(varkon_erpush("SU2993",errbuf));
 }

/* 2. Transform data                                                */
/* ___________________                                              */
/*                                                                 !*/

/* Point p1                                                         */

   poi_in = p_patin->p1;
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->p1 = poi_tra;
 
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC)," p_patin->p1 %f %f %f \n",
        poi_in.x_gm,      poi_in.x_gm,      poi_in.x_gm  );
  fprintf(dbgfil(SURPAC),"p_patout->p1 %f %f %f \n",
   p_patout->p1.x_gm, p_patout->p1.x_gm, p_patout->p1.x_gm  );
  }
#endif

/* Point p2                                                         */

   poi_in = p_patin->p2;
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->p2 = poi_tra;
 
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC)," p_patin->p2 %f %f %f \n",
        poi_in.x_gm,      poi_in.x_gm,      poi_in.x_gm  );
  fprintf(dbgfil(SURPAC),"p_patout->p2 %f %f %f \n",
   p_patout->p2.x_gm, p_patout->p2.x_gm, p_patout->p2.x_gm  );
  }
#endif

/* Point p3                                                         */

   poi_in = p_patin->p3;
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->p3 = poi_tra;
 
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC)," p_patin->p3 %f %f %f \n",
        poi_in.x_gm,      poi_in.x_gm,      poi_in.x_gm  );
  fprintf(dbgfil(SURPAC),"p_patout->p3 %f %f %f \n",
   p_patout->p3.x_gm, p_patout->p3.x_gm, p_patout->p3.x_gm  );
  }
#endif

/* Point p4                                                         */

   poi_in = p_patin->p4;
    GEtfpos_to_local (&poi_in, p_c, &poi_tra);
   p_patout->p4 = poi_tra;
 
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC)," p_patin->p4 %f %f %f \n",
        poi_in.x_gm,      poi_in.x_gm,      poi_in.x_gm  );
  fprintf(dbgfil(SURPAC),"p_patout->p4 %f %f %f \n",
   p_patout->p4.x_gm, p_patout->p4.x_gm, p_patout->p4.x_gm  );
  }
#endif

/* U,V values will be undefined (no pointer to the surface exists)  */
p_patout->u1  = F_UNDEF;
p_patout->v1  = F_UNDEF;
p_patout->u2  = F_UNDEF;
p_patout->v2  = F_UNDEF;
p_patout->u3  = F_UNDEF;
p_patout->v3  = F_UNDEF;
p_patout->u4  = F_UNDEF;
p_patout->v4  = F_UNDEF;
 
p_patout->triangles  = p_patin->triangles;
p_patout->defined    = p_patin->defined;
p_patout->p_att      = p_patin->p_att;

/*                                                                  */
/* 3. Exit                                                          */
/* ________                                                         */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur628 Exit ****** varkon_pat_factra ****** \n");
  }
#endif



    return(SUCCED);

  }

/*********************************************************/
