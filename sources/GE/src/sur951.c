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
/*  Function: varkon_sur_scur_mbs                  File: sur951.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Create MBS code for a surface curve. Only for testing !         */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1993-09-18   Originally written                                 */
/*  1996-01-18   Missing end for comment                            */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_scur_mbs   MBS code for curve as debug      */
/*                                                              */
/*------------------------------------------------------------- */

/*--------------- Function calls (external) ------------------------*/
/*                                                                  */
/*                                                                  */
/*------------------------------------------------------------------*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Program error in varkon_sur_scur_mbs (sur951). Report ! */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
      DBstatus varkon_sur_scur(

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *pcur,        /* Pointer to curve header                 */
   DBSeg   *pseg )       /* Pointer to a rational cubic UV segment  */
                         /* curve segment (a 2D curve segment)      */
/* Out:                                                             */
/*  File SURPAC.DBG with MBS source code                            */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat t_l;          /* Local parameter value                   */
   DBint   ir;           /* Loop index corresp. to curve segment    */
   DBint   no_seg;       /* Number of segments in the UV curve      */
/*-----------------------------------------------------------------!*/
   short   rcode;        /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors added    out[9-14]*/
   DBfloat out[15];      /* Array with coordinates and derivatives  */
   DBfloat x1,y1,z1;     /* Segment start point   1                 */
   DBfloat x2,y2,z2;     /* Segment end   point   2                 */
   DBfloat tx1,ty1,tz1;  /* Segment start tangent 1                 */
   DBfloat tx2,ty2,tz2;  /* Segment end   tangent 2                 */
   char    errbuf[80];   /* String for error message fctn erpush    */
   short   status;       /* Error code from a called function       */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "Enter ***** varkon_sur_scur_mbs (sur951) ********\n");
  }
#endif

/*!                                                                 */
/* 1. Initializations                                               */
/* __________________                                               */

/* Number of segments:                                              */
  no_seg= (pcur)->ns_cu;
/*                                                                 !*/


/*!                                                                 */
/* 2. Printout of MBS statements in SURPAC.DBG file                 */
/* ________________________________________________                 */
/*                                                                 !*/

/*!                                                                 */
/* Loop all segments in the curve   ir=0,1,....,no_seg-1            */
/*   Call GE107 and write LIN_FREE on debug file.                  */
/*                                                                 !*/

for ( ir=0; ir<= no_seg-1; ++ir )        /* Start loop EPOINT       */
  {

rcode=3;

t_l = 0.0;

status=GE107 ((DBAny*)pcur,pseg+ir,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107            %%varkon_sur_scur_mbs");
        return(varkon_erpush("SU2943",errbuf));
        }

      x1  = out[0]; 
      y1  = out[1]; 
      z1  = out[2]; 
      tx1 = out[3]; 
      ty1 = out[4]; 
      tz1 = out[5]; 

t_l = 1.0;

status=GE107 ((DBAny*)pcur,pseg+ir,t_l,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107            %%varkon_sur_scur_mbs");
        return(varkon_erpush("SU2943",errbuf));
        }

      x2  = out[0]; 
      y2  = out[1]; 
      z2  = out[2]; 
      tx2 = out[3]; 
      ty2 = out[4]; 
      tz2 = out[5]; 
#ifdef DEBUG
      fprintf(dbgfil(SURPAC),
      "LIN_FREE(#%d,VEC(%f,%f,%f),\n", ir+1,
       x1,y1,z1 );
      fprintf(dbgfil(SURPAC),
      "                  VEC(%f,%f,%f));\n",
       x2,y2,z2 );

      fprintf(dbgfil(SURPAC),
      "! Tangents %f,%f,%f    %f,%f,%f\n",
       tx1,ty1,tz1,tx2,ty2,tz2 );
#endif
/*  fel:;  */

  }                                      /* End   loop EPOINT       */



#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "Exit ****** varkon_sur_scur_mbs (sur951) ******* no_seg= %d\n",
     no_seg );
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/
