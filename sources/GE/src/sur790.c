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
/*  Function: varkon_cur_reparam                   File: sur790.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function reparameterises a rational cubic curve.            */
/*                                                                  */
/*  Note that there is an option to let the input pointer           */
/*  be equal to the output pointer. The input segment               */
/*  coefficients will be modified if this is the case.              */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-01-16   Originally written                                 */
/*  1994-10-22   Check of result for Debug On                       */
/*  1994-11-23   Check of result for Debug On                       */
/*  1995-03-07   No arclength calculation for offset segment        */
/*               Temporary fix !! p_cur must be input !!!!!         */
/*  1999-11-24   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_reparam    Reparameterise a rational cubic  */
/*                                                              */
/*------------------------------------------------------------- */

/*!--------------------- Theory ------------------------------------*/
/*                                                                  */
/*  Input cubic rational segment (homogenus coordinates):           */
/*                                                                  */
/*              2   3                                               */
/*  P(u)= (1,u,u , u ) * ! C0 !                                     */
/*                       ! C1 !                                     */
/*                       ! C2 !                                     */
/*                       ! C3 !                                     */
/*                                                                  */
/*  Reparameterise u = k0 + k1*t                                    */
/*                                                                  */
/*  For u= us_in shall t be t= us_out and                           */
/*  for u= ue_in shall t be t= ue_out                               */
/*                                                                  */
/*  us_in = k0 + k1*us_out                                          */
/*  ue_in = k0 + k1*ue_out                                          */
/*                                                                  */
/*  k0 + k1*us_out = us_in                                          */
/*  k0 + k1*ue_out = ue_in                                          */
/*                                                                  */
/*  k0 + k1*    us_out      = us_in                                 */
/*       k1*(ue_out-us_out) = ue_in - us_in                         */
/*                                                                  */
/*  k0 + k1*    us_out      = us_in                                 */
/*       k1                 = (ue_in - us_in)/(ue_out-us_out)       */
/*                                                                  */
/*       k0   = us_in-(ue_in-us_in)/(ue_out-us_out)*us_out          */
/*       k1   =       (ue_in-us_in)/(ue_out-us_out)                 */
/*                                                                  */
/*  Output cubic rational segment (homogenus coordinates):          */
/*                                                                  */
/*                             2         3                          */
/*  P(t)= (1,k0+k1*t,(k0+k1*t) ,(k0+k1*t) ) * ! C0 !                */
/*                                            ! C1 !                */
/*                                            ! C2 !                */
/*                                            ! C3 !                */
/*                                                                  */
/*  P(t)= C0 + C1*(k0+k1*t)    +                                    */
/*             C2*(k0+k1*t)**2 +                                    */
/*             C3*(k0+k1*t)**3                                      */
/*                                                                  */
/*  P(t)= C0 + C1*k0+C1*k1*t   +                                    */
/*             C2*k0**2+C2*2*k0*k1*t+C2*(k1*t)**2                   */
/*             C3*(k0**2+2*k0*k1*t+(k1*t)**2)*(k0+k1*t)             */
/*                                                                  */
/*  P(t)= C0 + C1*k0+C1*k1*t   +                                    */
/*             C2*k0**2+C2*2*k0*k1*t+C2*(k1*t)**2                   */
/*             C3*(k0**3+3*k0*k0*k1*t+3*k0*k1*t**2+(k1*t)**3        */
/*                                                                  */
/*  P(t)= C0 + C1*k0 +   C2*k0*k0 +  C3*k0*k0*k0        +           */
/*            (C1*k1 + 2*C2*k0*k1 + 3*C3*k0*k0*k1)*t    +           */
/*                      (C2*k1*k1 + 3*C3*k0*k1*k1)*t**2 +           */
/*                                   (C3*k1*k1*k1)*t**3             */
/*                                                                  */
/*                                                                  */
/*              2   3                                               */
/*  P(t)= (1,t,t , t ) * ! C0 + C1*k0 + C2*k0*k0 + C3*k0*k0*k0 !    */
/*                       !  C1*k1 + 2*C2*k0*k1 + 3*C3*k0*k0*k1 !    */
/*                       !       C2*k1*k1 + 3*C3*k0*k1*k1      !    */
/*                       !           C3*k1*k1*k1               !    */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* GE120                  * Arclength for a rational segment        */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/



/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Program error ( ) in varkon_cur_reparam         Report! */
/* SU2943 = Called fctn ... failed  in varkon_cur_reparam           */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus  varkon_cur_reparam (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSeg   *prin,         /* Pointer to input  rational segment      */
  DBfloat  us_in,        /* Parameter value for input  start point  */
  DBfloat  ue_in,        /* Parameter value for input  end   point  */
  DBfloat  us_out,       /* Parameter value for output start point  */
  DBfloat  ue_out,       /* Parameter value for output end   point  */
  DBSeg   *prout )       /* Pointer to output rational segment      */

/* Out:                                                             */
/*        Coefficients and other segment data in prout              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat k0,k1;         /* Reparameterisation coefficients         */
  DBSeg   segl;          /* Temporary used rational segment         */
/*                                                                  */
/*-----------------------------------------------------------------!*/
  DBint   status;        /* Error code for called function          */
  DBCurve cur;           /* Curve for GE120                        */
  DBfloat interv[2];     /* Parameter values                        */
  DBfloat delta;         /* Arc length                              */
  char    errbuf[80];    /* String for error message fctn erpush    */
#ifdef DEBUG
  DBfloat denom;         /* Denominator                             */
  DBfloat x_us_in;       /* X coordinate for us_in in input  segm.  */
  DBfloat y_us_in;       /* Y coordinate for us_in in input  segm.  */
  DBfloat z_us_in;       /* Z coordinate for us_in in input  segm.  */
  DBfloat x_ue_in;       /* X coordinate for ue_in in input  segm.  */
  DBfloat y_ue_in;       /* Y coordinate for ue_in in input  segm.  */
  DBfloat z_ue_in;       /* Z coordinate for ue_in in input  segm.  */
  DBfloat x_us_out;      /* X coordinate for us_in in output segm.  */
  DBfloat y_us_out;      /* Y coordinate for us_in in output segm.  */
  DBfloat z_us_out;      /* Z coordinate for us_in in output segm.  */
  DBfloat x_ue_out;      /* X coordinate for ue_in in output segm.  */
  DBfloat y_ue_out;      /* Y coordinate for ue_in in output segm.  */
  DBfloat z_ue_out;      /* Z coordinate for ue_in in output segm.  */
#endif

/*------------end-of-declarations-----------------------------------*/


/*! Algorithm                                                      !*/
/*! =========                                                      !*/

/*!                                                                 */
/*  1. Check of input data and initializations                      */
/*  __________________________________________                      */
/*                                                                 !*/

/*  For Debug On check:                                             */
/*  that parameter interval ! ue_out - us_out ! > TOL4   and        */
/*  that parameter interval ! ue_in  - us_in  ! > TOL4              */
/*                                                                 !*/

#ifdef DEBUG
    if ( fabs(ue_out-us_out) < TOL4 )
      {
      sprintf(errbuf,                        
      "out parameters%%varkon_cur_reparam");
      return(varkon_erpush("SU2993",errbuf));      
      }

    if ( fabs(ue_in -us_in ) < TOL4 )
      {
      sprintf(errbuf,                        
      "in  parameters%%varkon_cur_reparam");
      return(varkon_erpush("SU2993",errbuf));      
      }
#endif

#ifdef DEBUG
  denom      = (*prin).c0          + 
               (*prin).c1 *us_in      + 
               (*prin).c2 *us_in*us_in   +
               (*prin).c3 *us_in*us_in*us_in;
  x_us_in    = (*prin).c0x         + 
               (*prin).c1x*us_in      + 
               (*prin).c2x*us_in*us_in   +
               (*prin).c3x*us_in*us_in*us_in;
  y_us_in    = (*prin).c0y         + 
               (*prin).c1y*us_in      + 
               (*prin).c2y*us_in*us_in   +
               (*prin).c3y*us_in*us_in*us_in;
  z_us_in    = (*prin).c0z         + 
               (*prin).c1z*us_in      + 
               (*prin).c2z*us_in*us_in   +
               (*prin).c3z*us_in*us_in*us_in;
  if ( fabs(denom) > 0.0000001 )
      {
      x_us_in = x_us_in/denom;
      y_us_in = y_us_in/denom;
      z_us_in = z_us_in/denom;
      }
  denom      = (*prin).c0          + 
               (*prin).c1 *ue_in      + 
               (*prin).c2 *ue_in*ue_in   +
               (*prin).c3 *ue_in*ue_in*ue_in;
  x_ue_in    = (*prin).c0x         + 
               (*prin).c1x*ue_in      + 
               (*prin).c2x*ue_in*ue_in   +
               (*prin).c3x*ue_in*ue_in*ue_in;
  y_ue_in    = (*prin).c0y         + 
               (*prin).c1y*ue_in      + 
               (*prin).c2y*ue_in*ue_in   +
               (*prin).c3y*ue_in*ue_in*ue_in;
  z_ue_in    = (*prin).c0z         + 
               (*prin).c1z*ue_in      + 
               (*prin).c2z*ue_in*ue_in   +
               (*prin).c3z*ue_in*ue_in*ue_in;
  if ( fabs(denom) > 0.0000001 )
      {
      x_ue_in = x_ue_in/denom;
      y_ue_in = y_ue_in/denom;
      z_ue_in = z_ue_in/denom;
      }
#endif

/*!                                                                 */
/*  2. Calculate reparameterisation coefficients                    */
/*  ____________________________________________                    */

/*  Let  k0   = us_in-(ue_in-us_in)/(ue_out-us_out)*us_out          */
/*  Let  k1   =       (ue_in-us_in)/(ue_out-us_out)                 */
/*                                                                 !*/

    k0   = us_in-(ue_in-us_in)/(ue_out-us_out)*us_out;
    k1   =       (ue_in-us_in)/(ue_out-us_out); 

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
  fprintf(dbgfil(SURPAC),
  "sur790 k0    %f k1    %f \n",
    k0 , k1                       ) ;
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/*  3. Reparameterise the rational cubic                            */
/*  ____________________________________                            */
/*                                                                 !*/

/*!                                                                 */
/*  Output_C0 to  segl=    C0 + C1*k0 + C2*k0*k0 + C3*k0*k0*k0      */
/*                                                                 !*/

    segl.c0x = (*prin).c0x         + 
               (*prin).c1x*k0      + 
               (*prin).c2x*k0*k0   +
               (*prin).c3x*k0*k0*k0 ;

    segl.c0y = (*prin).c0y         + 
               (*prin).c1y*k0      + 
               (*prin).c2y*k0*k0   +
               (*prin).c3y*k0*k0*k0 ;

    segl.c0z = (*prin).c0z         + 
               (*prin).c1z*k0      + 
               (*prin).c2z*k0*k0   +
               (*prin).c3z*k0*k0*k0 ;

    segl.c0  = (*prin).c0          + 
               (*prin).c1 *k0      + 
               (*prin).c2 *k0*k0   +
               (*prin).c3 *k0*k0*k0 ;

/*!                                                                 */
/*  Output_C1 to  segl=     C1*k1 + 2*C2*k0*k1 + 3*C3*k0*k0*k1      */
/*                                                                 !*/

    segl.c1x = (*prin).c1x*k1                + 
               (*prin).c2x*2.0*k0*k1         +
               (*prin).c3x*3.0*k0*k0*k1;

    segl.c1y = (*prin).c1y*k1                + 
               (*prin).c2y*2.0*k0*k1         +
               (*prin).c3y*3.0*k0*k0*k1;

    segl.c1z = (*prin).c1z*k1                + 
               (*prin).c2z*2.0*k0*k1         +
               (*prin).c3z*3.0*k0*k0*k1;

    segl.c1  = (*prin).c1 *k1                + 
               (*prin).c2 *2.0*k0*k1         +
               (*prin).c3 *3.0*k0*k0*k1;

/*!                                                                 */
/*  Output_C2 to  segl=          C2*k1*k1 + 3*C3*k0*k1*k1           */
/*                                                                 !*/


    segl.c2x = (*prin).c2x*    k1*k1              + 
               (*prin).c3x*3.0*k0*k1*k1; 

    segl.c2y = (*prin).c2y*    k1*k1              + 
               (*prin).c3y*3.0*k0*k1*k1; 

    segl.c2z = (*prin).c2z*    k1*k1              + 
               (*prin).c3z*3.0*k0*k1*k1; 

    segl.c2  = (*prin).c2 *    k1*k1              + 
               (*prin).c3 *3.0*k0*k1*k1; 


/*!                                                                 */
/*  Output_C3 to  segl=              C3*k1*k1*k1                    */
/*                                                                 !*/

    segl.c3x = (*prin).c3x*k1*k1*k1;                 

    segl.c3y = (*prin).c3y*k1*k1*k1;                 

    segl.c3z = (*prin).c3z*k1*k1*k1;                 

    segl.c3  = (*prin).c3 *k1*k1*k1;                 


/*!                                                                 */
/*  4. Data to output segment prout                                 */
/*  _______________________________                                 */
/*                                                                  */
/*  Coefficients to prout (= segl).                                 */
/*                                                                 !*/

    prout->c0x = segl.c0x;
    prout->c0y = segl.c0y;
    prout->c0z = segl.c0z;
    prout->c0  = segl.c0 ;

    prout->c1x = segl.c1x;
    prout->c1y = segl.c1y;
    prout->c1z = segl.c1z;
    prout->c1  = segl.c1 ;

    prout->c2x = segl.c2x;
    prout->c2y = segl.c2y;
    prout->c2z = segl.c2z;
    prout->c2  = segl.c2 ;

    prout->c3x = segl.c3x;
    prout->c3y = segl.c3y;
    prout->c3z = segl.c3z;
    prout->c3  = segl.c3 ;

/*!                                                                 */
/*  Let output offset be equal to input offset.                     */
/*  Let output type   be equal to input type.                       */
/*  Calculate output segment length. Call of varkon_GE120 (GE120) */
/*                                                                 !*/

    prout->ofs = prin->ofs;

    prout->typ = prin->typ;
  
    prout->sl  = 0.0;

    if ( fabs(prout->ofs) > 0.00001 ) goto tmpfix;

   cur.hed_cu.type = CURTYP;
   cur.ns_cu       = 1;      
   cur.plank_cu    = FALSE;      

   interv[0]= 0.0;
   interv[1]= 1.0;

   status=GE120 ((DBAny *)&cur,prout,interv,&delta);
   if (status<0) 
        {
        sprintf(errbuf,"GE120%%varkon_cur_reparam");
        return(varkon_erpush("SU2943",errbuf));
        }

    prout->sl  = delta;
  
#ifdef DEBUG
  denom      = prout->c0          + 
               prout->c1 *us_out      + 
               prout->c2 *us_out*us_out   +
               prout->c3 *us_out*us_out*us_out;
  x_us_out   = prout->c0x         + 
               prout->c1x*us_out      + 
               prout->c2x*us_out*us_out   +
               prout->c3x*us_out*us_out*us_out;
  y_us_out   = prout->c0y         + 
               prout->c1y*us_out      + 
               prout->c2y*us_out*us_out   +
               prout->c3y*us_out*us_out*us_out;
  z_us_out   = prout->c0z         + 
               prout->c1z*us_out      + 
               prout->c2z*us_out*us_out   +
               prout->c3z*us_out*us_out*us_out;
  if ( fabs(denom) > 0.0000001 )
      {
      x_us_out= x_us_out/denom;
      y_us_out= y_us_out/denom;
      z_us_out= z_us_out/denom;
      }

  denom      = prout->c0          + 
               prout->c1 *ue_out      + 
               prout->c2 *ue_out*ue_out   +
               prout->c3 *ue_out*ue_out*ue_out;
  x_ue_out   = prout->c0x         + 
               prout->c1x*ue_out      + 
               prout->c2x*ue_out*ue_out   +
               prout->c3x*ue_out*ue_out*ue_out;
  y_ue_out   = prout->c0y         + 
               prout->c1y*ue_out      + 
               prout->c2y*ue_out*ue_out   +
               prout->c3y*ue_out*ue_out*ue_out;
  z_ue_out   = prout->c0z         + 
               prout->c1z*ue_out      + 
               prout->c2z*ue_out*ue_out   +
               prout->c3z*ue_out*ue_out*ue_out;
  if ( fabs(denom) > 0.0000001 )
      {
      x_ue_out= x_ue_out/denom;
      y_ue_out= y_ue_out/denom;
      z_ue_out= z_ue_out/denom;
      }
if ( dbglev(SURPAC) == 1 )
{
  fprintf(dbgfil(SURPAC),
  "sur790 p_us_in   %f %f %f\n",
   x_us_in , y_us_in , z_us_in   ) ;
  fprintf(dbgfil(SURPAC),
  "sur790 p_us_out  %f %f %f\n",
   x_us_out , y_us_out, z_us_out  ) ;
}
if ( dbglev(SURPAC) == 1 )
{
  fprintf(dbgfil(SURPAC),
  "sur790 p_ue_in   %f %f %f\n",
   x_ue_in , y_ue_in , z_ue_in   ) ;
  fprintf(dbgfil(SURPAC),
  "sur790 p_ue_out  %f %f %f\n",
   x_ue_out , y_ue_out, z_ue_out  ) ;
fflush(dbgfil(SURPAC)); /* From buffer to file */
}
/*  ???? Too tired to investigate TODO                      */
if ( fabs(y_us_out-y_us_in) > 0.001  )
    {
    sprintf(errbuf,"Y check%%varkon_cur_reparam");
    return(varkon_erpush("SU2993",errbuf));
    }
if ( fabs(y_ue_out-y_ue_in) > 0.001  )
    {
    sprintf(errbuf,"Y check%%varkon_cur_reparam");
    return(varkon_erpush("SU2993",errbuf));
    }
#endif

tmpfix: /* Label temporary fix */

    return(SUCCED);

} /* End of function */


/********************************************************************/
