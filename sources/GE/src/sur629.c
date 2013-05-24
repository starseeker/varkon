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
/*  Function: varkon_pat_nurbtra                   File: sur629.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Transforms a NURBS patch with the input transformation matrix   */
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
/*sdescr varkon_pat_nurbtra    Transform a NURBS patch          */
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_poi_nurbs       * Get address to a NURBS node             */
/* GEtfpos_to_local       * Transform a point                       */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_pat_nurbtra (sur629) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_pat_nurbtra (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  GMPATNU *p_patin,      /* Input patch                       (ptr) */
  DBTmat  *p_c,          /* Transformation matrix             (ptr) */
  GMPATNU *p_patout )    /* Output patch                      (ptr) */

/* Out:                                                             */
/*        Transformed points to p_patout                            */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  order_u;       /* Order  for surface in U direction       */
   DBint  order_v;       /* Order  for surface in V direction       */
   DBint  deg_u;         /* Degree for surface in U direction       */
   DBint  deg_v;         /* Degree for surface in V direction       */
   DBint  nk_u;          /* Number of nodes    in U direction       */
   DBint  nk_v;          /* Number of nodes    in V direction       */
   DBint  m_u;           /* Maximum node index value in U direction */
   DBint  m_v;           /* Maximum node index value in V direction */
/*                                                                  */
/*----------------------------------------------------------------- */

                         /* For varkon_poi_nurbs (sur243)           */
   DBint     rwcase;     /* Read/write case:                        */
                         /* Eq. 0: Read  node                       */
                         /* Eq. 1: Write node                       */
                         /* Eq. 2: Initialize values                */
                         /* Eq. 3: Printout of polygon points       */
                         /* Eq. 4: Check NURBS data                 */
   DBint     i_up;       /* Point address in U direction            */
   DBint     i_vp;       /* Point address in V direction            */
   DBHvector node;       /* Polygon point                           */

   DBfloat   weight;     /* Weight for a node point                 */


  DBint      i_u;        /* Loop index U knot values                */
  DBint      j_v;        /* Loop index V knot values                */
  DBfloat   *p_uval;     /* U knot value                      (ptr) */
  DBfloat   *p_vval;     /* V knot value                      (ptr) */
  DBfloat    u_knot;     /* U knot value                            */
  DBfloat    v_knot;     /* V knot value                            */


   DBVector  poi_in;     /* Input point for GEtfpos_to_local        */
   DBVector  poi_tra;    /* Transformed point                       */

   short    status;      /* Error code from a called function       */
   char     errbuf[80];  /* String for error message fctn erpush    */

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
  "sur629 Enter*varkon_pat_nurbtra**\n");
  }
#endif
 
/*!                                                                 */
/*  Check transformation matrix p_c.  Error SU2993 if not defined   */
/*                                                                 !*/

if ( p_c == NULL )
 {               
 sprintf(errbuf, "(p_c)%%sur629"); 
 return(varkon_erpush("SU2993",errbuf));
 }

/*!                                                                 */
/* Get number of knot values and copy to output patch               */
/*                                                                 !*/

    nk_u           = p_patin->nk_u;
    nk_v           = p_patin->nk_v;
    p_patout->nk_u = nk_u;
    p_patout->nk_v = nk_v;

/*!                                                                 */
/* Get the order (degree) of the basis functions and                */
/* copy to the output patch.                                        */
/*                                                                 !*/

    order_u           = p_patin->order_u;
    order_v           = p_patin->order_v;
    p_patout->order_u = order_u;
    p_patout->order_v = order_v;
    deg_u             = order_u - 1;
    deg_v             = order_v - 1;

/*!                                                                 */
/* Calculate the number of nodes                                    */
/*                                                                 !*/

    m_u     = nk_u - 1 - deg_u - 1;
    m_v     = nk_v - 1 - deg_v - 1;

/*!                                                                 */
/* Copy the rest of the NURBS patch header data                     */
/*                                                                 !*/

   p_patin->kvec_u   = p_patout->kvec_u;
   p_patin->kvec_v   = p_patout->kvec_v;
   p_patin->cpts     = p_patout->cpts;
   p_patin->pku      = p_patout->pku;
   p_patin->pkv      = p_patout->pkv;
   p_patin->pcpts    = p_patout->pcpts;
   p_patin->ofs_pat  = p_patout->ofs_pat;

/*!                                                                 */
/* 2. Transform node data                                           */
/* ______________________                                           */
/*                                                                 !*/

  for  ( i_up = 0; i_up <=  m_u; i_up++)
    {
      for  ( i_vp = 0; i_vp <=  m_v; i_vp++)
      {



/*    Retrieve polygon point                                        */

      rwcase = 0;
      status= varkon_poi_nurbs 
      (p_patin, rwcase, i_up, i_vp, &node);
     if  ( status < 0 ) 
       {
       sprintf(errbuf, "sur243 Read%%sur629");
     return(varkon_erpush("SU2943",errbuf));
     }

       poi_in.x_gm  =  node.x_gm;
       poi_in.y_gm  =  node.y_gm;
       poi_in.z_gm  =  node.z_gm;
       weight       =  node.w_gm;

/*    Transformate polygon point                                    */

       GEtfpos_to_local (&poi_in, p_c, &poi_tra);
 
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC)," Input  node %f %f %f \n",
        poi_in.x_gm,      poi_in.x_gm,      poi_in.x_gm  );
  fprintf(dbgfil(SURPAC)," Transf node %f %f %f \n",
        poi_tra.x_gm,      poi_tra.x_gm,      poi_tra.x_gm  );
  }
#endif


/*    Write    polygon point                                        */

       node.x_gm = poi_tra.x_gm;
       node.y_gm = poi_tra.y_gm;
       node.z_gm = poi_tra.z_gm;
       node.w_gm = weight;

      rwcase = 1;
      status= varkon_poi_nurbs (p_patout, rwcase, i_up, i_vp, &node);
     if  ( status < 0 ) 
       {
       sprintf(errbuf, "sur243 Write%%sur629");
     return(varkon_erpush("SU2943",errbuf));
     }


      } /* End loop i_vp */
    }   /* End loop i_up */

/*!                                                                 */
/* 3. Copy the knot vector                                          */
/* _______________________                                          */
/*                                                                 !*/

   for ( i_u = 0; i_u <  nk_u; ++i_u )
     {
      p_uval = p_patin->kvec_u+i_u;
      u_knot = *p_uval;
      p_uval = p_patout->kvec_u+i_u;
      (*p_uval) = u_knot;
     }

   for ( j_v = 0; j_v <  nk_v; ++j_v )
     {
      p_vval = p_patin->kvec_v+j_v;
      v_knot = *p_vval;
      p_vval = p_patout->kvec_v+j_v;
      (*p_vval) = v_knot;
     }

#ifdef  DEBUG
for ( i_u  = 1; i_u  <= nk_u; ++i_u  )
  {
  p_uval = p_patout->kvec_u + i_u  - 1;
  u_knot = *p_uval;
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur629 Index (span) %4d U knot value u_knot= %8.2f \n",
   (int)i_u -1, u_knot);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
  }

for ( j_v  = 1; j_v  <= nk_v; ++j_v  )
  {
  p_vval = p_patout->kvec_v + j_v  - 1;
  v_knot = *p_vval;
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur629 Index (span) %4d V knot value v_knot= %8.2f \n",
   (int)j_v -1, v_knot);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
  }

#endif
/*                                                                  */
/* 4. Exit                                                          */
/* ________                                                         */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur629 Exit ****** varkon_pat_nurbtra ****** \n");
  }
#endif



    return(SUCCED);

  }

/*********************************************************/
