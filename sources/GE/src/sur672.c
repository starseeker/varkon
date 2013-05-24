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

/*!******************************************************************/
/*                                                                  */
/*  Function: varkon_poi_3plane                    File: sur672.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Calculate point as the intersect between three planes           */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-01-05   Originally written                                 */
/*  1999-11-26   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_poi_3plane     Three plane intersect point      */
/*                                                              */
/*------------------------------------------------------------- */

/*!--------------------- Theory ------------------------------------*/
/*                                                                  */
/*  Reference: Faux & Pratt p 67                                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush      * Error message to terminal                   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2322 =  No three plane intersect point                         */
/* SU2993 = Severe program error ( ) in varkon_poi_3plane           */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus  varkon_poi_3plane (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBVector  u_1,         /* Plane 1 normal                          */
  DBfloat   d_1,         /* Plane 1 D value                         */
  DBVector  u_2,         /* Plane 2 normal                          */
  DBfloat   d_2,         /* Plane 2 D value                         */
  DBVector  u_3,         /* Plane 3 normal                          */
  DBfloat   d_3,         /* Plane 3 D value                         */
  DBVector *p_pint )     /* Intersect point                   (ptr) */

/* Out:                                                             */
/*        Point  data to *p_pint                                    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBVector vcross23;     /* Cross product vector u_2 X u_3          */
  DBVector vcross31;     /* Cross product vector u_3 X u_1          */
  DBVector vcross12;     /* Cross product vector u_1 X u_2          */
  DBfloat  denom;        /* Denominator                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

#ifdef DEBUG
  DBfloat  length;       /* Length of input plane vectors           */
#endif
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur672 Enter *** varkon_poi_3plane  ** Three plane intersect point*\n");
}
if ( dbglev(SURPAC) == 2 )
{
  fprintf(dbgfil(SURPAC),
  "sur672  u_1 %10.6g %10.6g %10.6g d_1 %15.8g\n",u_1.x_gm,u_1.y_gm,u_1.z_gm,d_1);
  fprintf(dbgfil(SURPAC),
  "sur672  u_2 %10.6g %10.6g %10.6g d_2 %15.8g\n",u_2.x_gm,u_2.y_gm,u_2.z_gm,d_2);
  fprintf(dbgfil(SURPAC),
  "sur672  u_3 %10.6g %10.6g %10.6g d_3 %15.8g\n",u_3.x_gm,u_3.y_gm,u_3.z_gm,d_3);
}
#endif
 

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/*                                                                  */
/* Check lengths of input plane normals for Debug On                */
/*                                                                 !*/

#ifdef DEBUG
   length = u_1.x_gm*u_1.x_gm + u_1.y_gm*u_1.y_gm + u_1.z_gm*u_1.z_gm;  
   if ( length < 0.000001 )
     {
     sprintf(errbuf,"(u_1 zero)%%varkon_poi_3plane  (sur672)");
     return(varkon_erpush("SU2993",errbuf));
     }

   length = u_2.x_gm*u_2.x_gm + u_2.y_gm*u_2.y_gm + u_2.z_gm*u_2.z_gm;  
   if ( length < 0.000001 )
     {
     sprintf(errbuf,"(u_2 zero)%%varkon_poi_3plane  (sur672)");
     return(varkon_erpush("SU2993",errbuf));
     }

   length = u_3.x_gm*u_3.x_gm + u_3.y_gm*u_3.y_gm + u_3.z_gm*u_3.z_gm;  
   if ( length < 0.000001 )
     {
     sprintf(errbuf,"(u_3 zero)%%varkon_poi_3plane  (sur672)");
     return(varkon_erpush("SU2993",errbuf));
     }
#endif  /* End DEBUG   */
 
  p_pint->x_gm = 12345.6789;       /* Initialize output point       */
  p_pint->y_gm = 12345.6789;
  p_pint->z_gm = 12345.6789; 

/*!                                                                 */
/* 2. Calculate intersect point                                     */
/*                                                                  */
/*                                                                 !*/

   vcross23.x_gm =   u_2.y_gm * u_3.z_gm -  u_2.z_gm * u_3.y_gm;  
   vcross23.y_gm =   u_2.z_gm * u_3.x_gm -  u_2.x_gm * u_3.z_gm;  
   vcross23.z_gm =   u_2.x_gm * u_3.y_gm -  u_2.y_gm * u_3.x_gm;  

   vcross31.x_gm =   u_3.y_gm * u_1.z_gm -  u_3.z_gm * u_1.y_gm;  
   vcross31.y_gm =   u_3.z_gm * u_1.x_gm -  u_3.x_gm * u_1.z_gm;  
   vcross31.z_gm =   u_3.x_gm * u_1.y_gm -  u_3.y_gm * u_1.x_gm;  

   vcross12.x_gm =   u_1.y_gm * u_2.z_gm -  u_1.z_gm * u_2.y_gm;  
   vcross12.y_gm =   u_1.z_gm * u_2.x_gm -  u_1.x_gm * u_2.z_gm;  
   vcross12.z_gm =   u_1.x_gm * u_2.y_gm -  u_1.y_gm * u_2.x_gm;  

   denom = u_1.x_gm*vcross23.x_gm +  
           u_1.y_gm*vcross23.y_gm +  
           u_1.z_gm*vcross23.z_gm;   

   if ( fabs(denom) < 0.000000001 )
     {
     sprintf(errbuf," %%varkon_poi_3plane (sur672)");
     return(varkon_erpush("SU2322",errbuf));
     }
 
  p_pint->x_gm = (d_1*vcross23.x_gm+d_2*vcross31.x_gm+d_3*vcross12.x_gm)/denom;  
  p_pint->y_gm = (d_1*vcross23.y_gm+d_2*vcross31.y_gm+d_3*vcross12.y_gm)/denom;  
  p_pint->z_gm = (d_1*vcross23.z_gm+d_2*vcross31.z_gm+d_3*vcross12.z_gm)/denom;  

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur672 Exit*varkon_poi_3plane  p_pint %f %f %f\n",
      p_pint->x_gm,  p_pint->y_gm,  p_pint->z_gm);
fflush(dbgfil(SURPAC)); 
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/
