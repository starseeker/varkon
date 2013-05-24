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

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_seg_eval                      File: sur782.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  TODO This function should be replaced by GE109 !!!!            */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*  The function calculates coordinates and derivatives for         */
/*  a given parameter ( u ) point on a given curve segment.         */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-01-15   Originally written                                 */
/*  1994-10-12   Mer av parametrarna i EVALC ifyllda G Liden        */
/*  1996-05-28   Deleted ix1                                        */
/*  1997-02-09   Elimination of compiler warnings                   */
/*  1999-11-27   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_seg_eval       Curve segment evaluation fctn    */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
#ifdef  DEBUG
static short initial();       /* Initialization of variables        */
#endif
/*                                                                  */
/*-----------------------------------------------------------------!*/

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------- Function calls (external) ------------------------*/
/*                                                                  */
/*                                                                  */
/*------------------------------------------------------------------*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxxx failed in varkon_seg_eval        */
/* SU2983 = sur782 Illegal computation case=  for varkon_seg_eval   */
/* SU2973 = Internal function xxxxxx failed in varkon_seg_eval      */
/* SU2993 = Program error in varkon_seg_eval    (sur782). Report !  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus          varkon_seg_eval (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur,       /* Curve                             (ptr) */
   DBSeg   *p_seg,       /* Curve segment                     (ptr) */
   DBint    icase,       /* Calculation case:                       */
                         /* Eq. 0: Coordinates only                 */
                         /* Eq. 1: First  derivative added          */
                         /* Eq. 2: Second derivative added          */
                         /* Eq. 3: Frenet vectors added and         */
                         /*        curvature added                  */
   DBfloat  u_seg,       /* Curve segment (local) parameter value   */
   EVALC   *p_xyz )      /* Pointer to coordinates and derivatives  */
                         /* for a point on a curve                  */
/* Out:                                                             */
/*       Data to p_xyz                                              */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat  out[16];     /* Coordinates and derivatives for crv pt  */
   short    rcode;       /* Flag for output coordinates/derivatives */
                         /* Eq. 0: Coordinates only        out[0-2] */
                         /* Eq. 1: First  derivative added out[3-5] */
                         /* Eq. 2: Second derivative added out[6-8] */
                         /* Eq. 3: Frenet vectors added    out[9-14]*/
                         /*        curvature added         out[ 15 ]*/
   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint  status;        /* Error code from a called function       */

/* ----------------- Theory ----------------------------------------*/
/*                                                                  */
/* The coordinates and derivatives  ........................        */
/*                                                                  */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur782 Enter *** varkon_seg_eval   : u_seg= %f p_seg= %d\n",
          u_seg,(int)p_seg );
 fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */

/*  Initialize output coordinates and derivatives for DEBUG On.     */
/*  Call of initial, which also prints out the input data.          */
/*                                                                 !*/

/*  Printout of input data is in function initial                   */

#ifdef DEBUG
   initial(icase,u_seg,p_xyz); 
#endif

/*! Check computation case.               Error SU2983 if not OK.  !*/
    if (icase > 3 ) 
        {
        sprintf(errbuf,"%d%% varkon_seg_eval    (sur782)",(int)icase);
        return(varkon_erpush("SU2983",errbuf));
        }


/*!                                                                 */
/* 2. Calculate coordinates and derivatives                         */
/* _________________________________________                        */

/*  Switch to the right curve segment evaluation routine:           */
/*  Call of varkon_GE107 (GE107).                                 */
/*                                                                 !*/

   rcode = (short)icase;
   status=GE107 ((GMUNON *)p_cur,p_seg,u_seg,rcode,out);
   if (status<0) 
        {
        sprintf(errbuf,"GE107%%varkon_seg_eval (sur782)");
        return(varkon_erpush("SU2943",errbuf));
        }


/*!                                                                 */
/* 3. Calculated data to output variables                           */
/* ______________________________________                           */
/*                                                                  */
/* Coordinates, derivatives and curvature to p_xyz                  */
/*                                                                 !*/

    p_xyz->r_x= out[0];
    p_xyz->r_y= out[1];
    p_xyz->r_z= out[2];

    if ( icase > 0 )
       {
       p_xyz->u_x= out[3];
       p_xyz->u_y= out[4];
       p_xyz->u_z= out[5];
       }

    if ( icase > 1 )
       {
       p_xyz->u2_x= out[6];
       p_xyz->u2_y= out[7];
       p_xyz->u2_z= out[8];
       }

    if ( icase > 2 )
       {
       p_xyz->pn_x=  out[9];
       p_xyz->pn_y=  out[10];
       p_xyz->pn_z=  out[11];
       p_xyz->bn_x=  out[12];
       p_xyz->bn_y=  out[13];
       p_xyz->bn_z=  out[14];
       p_xyz->kappa= out[15];
       }

    p_xyz->offset= p_seg->ofs;

    if ( p_seg->ofs != 0.0 )    p_xyz->offseg = TRUE;
    else                        p_xyz->offseg = FALSE;

    if ( p_seg->typ == UV_SEG ) p_xyz->surpat = TRUE;
    else                        p_xyz->surpat = FALSE;

    if ( p_seg->c0 == 1.0  &&  
         p_seg->c1 == 0.0  &&
         p_seg->c2 == 0.0  &&  
         p_seg->c3 == 0.0 )     p_xyz->ratseg = FALSE;
    else                        p_xyz->ratseg = TRUE;

    if ( p_seg->c3x == 0.0  &&  
         p_seg->c3y == 0.0  &&
         p_seg->c3z == 0.0  &&  
         p_seg->c3  == 0.0 )    p_xyz->cubseg = FALSE;
    else                        p_xyz->cubseg = TRUE;

    if ( p_seg->c3x == 0.0  &&  
         p_seg->c3y == 0.0  &&
         p_seg->c3z == 0.0  &&  
         p_seg->c3  == 0.0  && 
         p_seg->c2x == 0.0  &&  
         p_seg->c2y == 0.0  &&
         p_seg->c2z == 0.0  &&  
         p_seg->c2  == 0.0 )    p_xyz->straight = TRUE; 
    else                        p_xyz->straight = FALSE;

    p_xyz->c_anal= TRUE;  

    p_xyz->t_local= u_seg; 
   
    p_xyz->r.x_gm = out[0];
    p_xyz->r.y_gm = out[1];
    p_xyz->r.z_gm = out[2];
   
    p_xyz->drdt.x_gm = out[3];
    p_xyz->drdt.y_gm = out[4];
    p_xyz->drdt.z_gm = out[5];
   
    p_xyz->d2rdt2.x_gm = out[6];
    p_xyz->d2rdt2.y_gm = out[7];
    p_xyz->d2rdt2.z_gm = out[8];
   
    p_xyz->p_norm.x_gm = out[9];
    p_xyz->p_norm.y_gm = out[10];
    p_xyz->p_norm.z_gm = out[11];
   
    p_xyz->b_norm.x_gm = out[12];
    p_xyz->b_norm.y_gm = out[13];
    p_xyz->b_norm.z_gm = out[14];

    p_xyz->kappa = out[15];

    p_xyz->surpat= FALSE;  
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur782 r_x %f r_y %f r_z %f \n",
   p_xyz->r_x,p_xyz->r_y,p_xyz->r_z);
  fprintf(dbgfil(SURPAC),
  "sur782 u_x %f u_y %f u_z %f \n",
   p_xyz->u_x,p_xyz->u_y,p_xyz->u_z);
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur782 u2_x %f u2_y %f u2_z %f \n",
   p_xyz->u2_x,p_xyz->u2_y,p_xyz->u2_z);
  fprintf(dbgfil(SURPAC),
  "sur782 Exit *** varkon_seg_eval    ******* \n");
  }
#endif

    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/

#ifdef  DEBUG
/*!********* Internal ** function ** initial ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function initializes the output variables and the            */
/* static (common) variables to 1.23456789 and 123456789.           */

   static short initial(icase,u_seg,p_xyz)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint   icase;        /* Calculation case                        */
                         /* Eq. 0: Coordinates only                 */
                         /* Eq. 1: First  derivative added          */
                         /* Eq. 2: Second derivative added          */
                         /* Eq. 3: Frenet vectors added and         */
                         /*        curvature added                  */
   DBfloat   u_seg;        /* Patch (local) U parameter value         */
   EVALC  *p_xyz;        /* Pointer to coordinates and derivatives  */
                         /* for a point on a curve                  */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Printout of input variables                                   */
/*                                                                 !*/

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur782 *** initial: icase= %d p_xyz= %d\n", (int)icase, (int)p_xyz);
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }

/*!                                                                 */
/* 2. Initialize output variables EVALC and static variables        */
/*                                                                 !*/

/*  Coordinates          r(u)                                       */
    p_xyz->r_x= 1.23456789;
    p_xyz->r_y= 1.23456789;
    p_xyz->r_z= 1.23456789;

/*  Tangent             dr/du                                       */
    p_xyz->u_x= 1.23456789;
    p_xyz->u_y= 1.23456789;
    p_xyz->u_z= 1.23456789;

/*  Second derivative  d2r/du2                                      */
    p_xyz->u2_x= 1.23456789;
    p_xyz->u2_y= 1.23456789;
    p_xyz->u2_z= 1.23456789;

/*  Principal normal                                                */
    p_xyz->pn_x= 1.23456789;
    p_xyz->pn_y= 1.23456789;
    p_xyz->pn_z= 1.23456789;

/*  Binormal                                                        */
    p_xyz->bn_x= 1.23456789;
    p_xyz->bn_y= 1.23456789;
    p_xyz->bn_z= 1.23456789;

/*  Curvature                                                       */
    p_xyz->kappa= 1.23456789;

/*  Offset                                                          */
    p_xyz->offset= 1.23456789;

/*  Evaluation case                                                 */
    p_xyz->e_case= 123456789;

/*  Parameter point for a surface curve                             */
    p_xyz->u= 1.23456789;
    p_xyz->v= 1.23456789;

/*  Parameter derivatives for a surface curve                       */
    p_xyz->u_t=  1.23456789;
    p_xyz->v_t=  1.23456789;
    p_xyz->u_t2= 1.23456789;
    p_xyz->v_t2= 1.23456789;

/*  Flag for defined boolean parameters                             */
    p_xyz->c_anal=  FALSE;      

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/
#endif /*   DEBUG   */
