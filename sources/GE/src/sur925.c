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
/*  Function: varkon_sur_d3toduv                   File: sur925.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Calculate the UV tangent for a given R*3 tangent                */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-10-24   Originally written                                 */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_d3toduv    UV tangent for given R*3 tangent */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_idpoint         * Identical points criterion              */
/* varkon_idangle         * Identical angles criterion              */
/* varkon_angd            * Angle between vectors (degr.)           */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function xxxxxx failed in varkon_sur_d3toduv     */
/* SU2993 = Program error in varkon_sur_d3toduv (sur925). Report !  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*
*
*     Formulae
*     ========
*
*  Refererence: Computational Geometry for Design and Manufacture
*               Faux & Pratt,  page 107 - 108
*
*
*  r(t) = r(u(t),v(t)) is a curve on a surface with t as parameter
*
*  The tangent vector to this curve is:
*
*   dr/dt = (dx/dt,dy/dt,dzdt)
*
*   where (the chain rule):
*
*   dx/dt = dx/du*du/dt + dx/dv*dv/dt     (1)
*   dy/dt = dy/du*du/dt + dy/dv*dv/dt     (2)
*   dz/dt = dz/dt*du/dt + dz/dv*dv/dt     (3)
*
*  Two equations is enough to determine the u,v vector for a given
*  tangent on the surface . The third is used as a check that the
*  input tangent is on the surface.
*
*  Input vector (tx,ty,tz)
*
*  Equations (1) and (2)
*  .....................
*
*   tx = dx/du*du/dt + dx/dv*dv/dt
*   ty = dy/du*du/dt + dy/dv*dv/dt
*
*         <==>
*
*   dx/du*du/dt + dx/dv*dv/dt = tx
*   dy/du*du/dt + dy/dv*dv/dt = ty
*
*         <==>  (Alternative du/dt)
*
*   dx/du*dy/dv*du/dt + dx/dv*dy/dv*dv/dt = tx*dy/dv
*   dx/dv*dy/du*du/dt + dx/dv*dy/dv*dv/dt = ty*dx/dv
*
*         <==>
*
*   dx/du*dy/dv*du/dt + dx/dv*dy/dv*dv/dt = tx*dy/dv
*         (dx/du*dy/dv-dx/dv*dy/du)*du/dt = tx*dy/dv - ty*dx/dv
*
*         <==>
*
*   dx/du*dy/dv*du/dt + dx/dv*dy/dv*dv/dt = tx*dy/dv
*
*
*                                           tx*dy/dv - ty*dx/dv
*                                   du/dt = -----------------------
*                                           dx/du*dy/dv-dx/dv*dy/du
*         <==>
*   tx = dx/du*du/dt + dx/dv*dv/dt
*   ty = dy/du*du/dt + dy/dv*dv/dt
*
*
*
*           tx*dy/dv - ty*dx/dv
*  du/dt = -----------------------        (f1)
*           dx/du*dy/dv-dx/dv*dy/du
*
*           tx - dx/du*du/dt
*  dv/dt = ------------------             (f2)
*            dx/dv
*
*           ty - dy/du*du/dt
*  dv/dt = --------------------           (f3)
*            dy/dv
*
*
*         <==>  (Alternative dv/dt)
*
*
*   dx/du*dy/du*du/dt + dx/dv*dy/du*dv/dt = tx*dy/du
*   dx/du*dy/du*du/dt + dx/du*dy/dv*dv/dt = ty*dx/du
*
*
*         <==>
*
*   dx/du*dy/du*du/dt + dx/dv*dy/du*dv/dt = tx*dy/du
*       (dx/dv*dy/du - dx/du*dy/dv)*dv/dt = tx*dy/du - ty*dx/du
*
*
*         <==>
*   dx/du*dy/du*du/dt + dx/dv*dy/du*dv/dt = tx*dy/du
*
*                                              tx*dy/du - ty*dx/du
*                                   dv/dt = -------------------------
*                                           dx/dv*dy/du - dx/du*dy/dv
*         <==>
*
*             tx*dy/du - ty*dx/du
*  dv/dt = -------------------------      (f4)
*          dx/dv*dy/du - dx/du*dy/dv
*
*
*          tx - dx/dv*dv/dt
*  du/dt = --------------------           (f5)
*               dx/du
*
*               or
*
*          ty - dy/dv*dv/dt
*  du/dt = --------------------           (f6)
*              dy/du
*
*  Formulas using the equations (1) and (2)
*  .............................................
*
*
*             tx*dy/dv - ty*dx/dv
*  du/dt = -----------------------        (f1)
*          dx/du*dy/dv - dx/dv*dy/du
*
*           tx - dx/du*du/dt
*  dv/dt = ------------------             (f2)
*              dx/dv
*
*           ty - dy/du*du/dt
*  dv/dt = --------------------           (f3)
*              dy/dv
*
*
*             tx*dy/du - ty*dx/du
*  dv/dt = -------------------------      (f4)
*          dx/dv*dy/du - dx/du*dy/dv
*
*
*          tx - dx/dv*dv/dt
*  du/dt = --------------------           (f5)
*               dx/du
*
*               or
*
*          ty - dy/dv*dv/dt
*  du/dt = --------------------           (f6)
*              dy/du
*
*  Formulas using the equations (1) and (3)
*  .............................................
*  ( By similarity)
*
*             tx*dz/dv - tz*dx/dv
*  du/dt = -----------------------        (f7)
*          dx/du*dz/dv - dx/dv*dz/du
*
*           tx - dx/du*du/dt
*  dv/dt = ------------------             (f8)
*              dx/dv
*
*           tz - dz/du*du/dt
*  dv/dt = --------------------           (f9)
*              dz/dv
*
*
*             tx*dz/du - tz*dx/du
*  dv/dt = -------------------------      (f10)
*          dx/dv*dz/du - dx/du*dz/dv
*
*
*          tx - dx/dv*dv/dt
*  du/dt = --------------------           (f11)
*               dx/du
*
*               or
*
*          tz - dy/dv*dv/dt
*  du/dt = --------------------           (f12)
*              dy/du
*
*
*  Formulas using the equations (2) and (3)
*  .............................................
*  ( By similarity)
*
*
*             tz*dy/dv - ty*dz/dv
*  du/dt = -----------------------        (f13)
*          dz/du*dy/dv - dz/dv*dy/du
*
*           tz - dz/du*du/dt
*  dv/dt = ------------------             (f14)
*              dz/dv
*
*           ty - dy/du*du/dt
*  dv/dt = --------------------           (f15)
*              dy/dv
*
*
*             tz*dy/du - ty*dz/du
*  dv/dt = -------------------------      (f16)
*          dz/dv*dy/du - dz/du*dy/dv
*
*
*          tz - dz/dv*dv/dt
*  du/dt = --------------------           (f17)
*               dz/du
*
*               or
*
*          ty - dy/dv*dv/dt
*  du/dt = --------------------           (f18)
*              dy/du
*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus        varkon_sur_d3toduv (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   EVALS    *p_xyz,      /* Coordinates and derivatives       (ptr) */
   DBVector *p_r3tang,   /* R*3 tangent vector                (ptr) */
   DBVector *p_uvtang )  /* U,V tangent vector                (ptr) */
/*                                                                  */
/* Out:                                                             */
/*       Data to p_uvtang                                           */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat idpoint;       /* Identical point  tolerance             */
   DBfloat idangle;       /* Identical angle  tolerance             */
   DBfloat t3_in[3];      /* Dummy vector 1 for the test            */
   DBfloat t_che[3];      /* Dummy vector 2 for the test            */
   DBfloat dxdu,dydu,dzdu;/* Derivatives dx/du, dy/du, dz/du        */
   DBfloat dxdv,dydv,dzdv;/* Derivatives dx/dv, dy/dv, dz/dv        */
   DBfloat dxdt,dydt,dzdt;/* Derivatives dx/dt, dy/dt, dz/dt        */
   DBfloat dudt,dvdt;     /* Derivatives du/dt, dv/dt               */
   DBfloat dudt1,dvdt1;   /* du/dt, dv/dt using f1 and f4           */
   DBfloat dudt2,dvdt2;   /* du/dt, dv/dt using f7 and f10          */
   DBfloat dudt3,dvdt3;   /* du/dt, dv/dt using f13 and f16         */
   DBfloat s_normal[3];   /* Surface normal vector                  */
   DBfloat alpha1;        /* Angle deviations                       */
   DBfloat alpha2;        /* (f1,f4), (f7,f10) and (f13,f16)        */
   DBfloat alpha3;        /*                                        */
   DBfloat alpha;         /* The best of alpha1, alpha2 and alpha3  */

/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBfloat testang;       /* Test angle                             */
   DBfloat v_leng;        /* Length of a vector                     */
   DBfloat denom1;        /* Dummy denominator 1                    */
   DBfloat denom2;        /* Dummy denominator 2                    */
   DBfloat denom3;        /* Dummy denominator 3                    */
   char    errbuf[80];    /* String for error message fctn erpush   */
   DBint   status;        /* Error code from a called function      */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur925 Enter *** varkon_sur_d3toduv \n" );
  }
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* ___________________________________________                      */
/*                                                                  */
/*                                                                 !*/

   p_uvtang->x_gm = F_UNDEF;
   p_uvtang->y_gm = F_UNDEF;
   p_uvtang->z_gm = F_UNDEF;

   dudt1    = F_UNDEF;
   dvdt1    = F_UNDEF;
   dudt2    = F_UNDEF;
   dvdt2    = F_UNDEF;
   dudt3    = F_UNDEF;
   dvdt3    = F_UNDEF;
   dudt     = F_UNDEF;
   dvdt     = F_UNDEF;
   alpha1   = F_UNDEF;
   alpha2   = F_UNDEF;
   alpha3   = F_UNDEF;
   alpha    = F_UNDEF;

   idpoint   = varkon_idpoint();
   idangle   = varkon_idangle();

/*  2. Convert derivatives in XYZ to local variables dxdu,...,dzdv, */
/*     s_normal and dxdt,...                                        */

      dxdu = p_xyz->u_x;
      dydu = p_xyz->u_y;
      dzdu = p_xyz->u_z;
      dxdv = p_xyz->v_x;
      dydv = p_xyz->v_y;
      dzdv = p_xyz->v_z;

      s_normal[0] = p_xyz->n_x;
      s_normal[1] = p_xyz->n_y;
      s_normal[2] = p_xyz->n_z;

      dxdt = p_r3tang->x_gm;
      dydt = p_r3tang->y_gm;
      dzdt = p_r3tang->z_gm;

/*    Store input tangent in t3_in and normalize t3_in  */

      t3_in[0] = dxdt;    
      t3_in[1] = dydt;    
      t3_in[2] = dzdt;    
      v_leng = SQRT( t3_in[0]*t3_in[0] +
                     t3_in[1]*t3_in[1] +
                     t3_in[2]*t3_in[2]  );
      if (v_leng < idpoint ) 
        {
        sprintf(errbuf,"input V%%sur925");
        return(varkon_erpush("SU2993",errbuf));
        }
        t3_in[0] = t3_in[0]/v_leng;
        t3_in[1] = t3_in[1]/v_leng;
        t3_in[2] = t3_in[2]/v_leng;

/*    Check if the tangent is on the surface              */

      status= varkon_angd (s_normal,t3_in,&testang);
      if (status<0) 
        {
        sprintf(errbuf,"varkon_angd%%sur925");
        return(varkon_erpush("SU2943",errbuf));
        }
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur925 testang-90= %15.10f idangle %15.10f\n", 
      fabs(fabs(testang)-90),idangle );
  }
#endif
      testang = fabs(testang);  
      if (  fabs(testang-90.0) > idangle )
        {
        sprintf(errbuf,"V not on surface%%sur925");
        return(varkon_erpush("SU2993",errbuf));
        }

/*  The denominators to the formulas f1, f4, f7, f10, f13 and f16 */
/*  are computed and there is a check if all three are zero.      */

      denom1 = dxdu*dydv - dxdv*dydu;
      denom2 = dxdu*dzdv - dxdv*dzdu;
      denom3 = dzdu*dydv - dzdv*dydu;

      if(fabs(denom1) <  0.000001  &&
         fabs(denom2) <  0.000001  &&
         fabs(denom3) <  0.000001     )
        {
        sprintf(errbuf,"All denoms= 0%%sur925");
        return(varkon_erpush("SU2993",errbuf));
        }



/*  3. Compute the UV-tangents.                                      */
/*     In order to check if there is a numerical accuracy problem all*/
/*     formulas f1, f4, f7, f13 and f16 are used.                    */

      if(fabs(denom1) >  0.0000000001)
        {
        dudt1 =  (dxdt*dydv - dydt*dxdv)/denom1;
        dvdt1 = -(dxdt*dydu - dydt*dxdu)/denom1;
        }        
      if(fabs(denom2) >  0.0000000001)
        {
        dudt2 =  (dxdt*dzdv - dzdt*dxdv)/denom2;
        dvdt2 = -(dxdt*dzdu - dzdt*dxdu)/denom2;
        }        
      if(fabs(denom3) >  0.0000000001)
        {
        dudt3 =  (dzdt*dydv - dydt*dzdv)/denom3;
        dvdt3 = -(dzdt*dydu - dydt*dzdu)/denom3;
        }        

/*     Choose the U,V vector which gives the "best" R*3 vector.  */
/*     alpha1:                                                   */

      if(fabs(denom1) >  0.0000000001) 
        {
/*      Compute Z-component = dzdu*dudt1 + dzdv*dvdt1            */
        t_che[0] = dxdt;   
        t_che[1] = dydt;   
        t_che[2] = dzdu*dudt1 + dzdv*dvdt1;
        status= varkon_angd (t3_in,t_che,&alpha1);
         if (status<0) 
           {
           sprintf(errbuf,"varkon_angd%%sur925");
           return(varkon_erpush("SU2943",errbuf));
           }
         }
      else alpha1 = 500;

/*     alpha2:                                             */

      if (  fabs(denom2) >  0.0000000001  )
        {

/*      Compute Y-component = dydu*dudt2 + dydv*dvdt2       */

        t_che[0] = dxdt;    
        t_che[1] = dydu*dudt2 + dydv*dvdt2;
        t_che[2] = dzdt;    
        status= varkon_angd (t3_in,t_che,&alpha2);
         if (status<0) 
           {
           sprintf(errbuf,"varkon_angd%%sur925");
           return(varkon_erpush("SU2943",errbuf));
           }
         }

      else alpha2 = 500;


/*     alpha3:                                                 */

      if ( fabs(denom3) >  0.0000000001 )
        {
/*      Compute X-component = dxdu*dudt3 + dxdv*dvdt3          */
        t_che[0] = dxdu*dudt3 + dxdv*dvdt3;
        t_che[1] = dydt;   
        t_che[2] = dzdt;   
        status= varkon_angd (t3_in,t_che,&alpha3);
         if (status<0) 
           {
           sprintf(errbuf,"varkon_angd%%sur925");
           return(varkon_erpush("SU2943",errbuf));
           }
        }
      else alpha3 = 500;


/* Compare angles: The UV-vector which gives the smallest angle is */
/*                 output from the function.                       */

      if ( fabs(alpha1) <= fabs(alpha2)  && 
           fabs(alpha1) <= fabs(alpha3)      )
        {
        if(fabs(dudt1)  >  0.0  ||  fabs(dvdt1)  >   0.0 )
         {
          dudt = dudt1;
          dvdt = dvdt1;
          alpha = alpha1;
          }    
        }    

      if(fabs(alpha2) <= fabs(alpha1)  && 
         fabs(alpha2) <= fabs(alpha3)       )
        {
        if(fabs(dudt2)  >  0.0  ||  fabs(dvdt2)  >   0.0 )
          {
          dudt = dudt2;
          dvdt = dvdt2;
          alpha = alpha2;
          }    
        }    

      if(fabs(alpha3) <= fabs(alpha1)  && 
         fabs(alpha3) <= fabs(alpha2)       )
        {
        if(fabs(dudt3)  >  0.0  ||  fabs(dvdt3)  >   0.0 )
          {
          dudt = dudt3;
          dvdt = dvdt3;
          alpha = alpha3;
          }    
        }    

/* Check that the vector is on the surface within given tolerance. */

   if (fabs(alpha) >  idangle  )
     {
     sprintf(errbuf,"alpha>idangle%%sur925");
     return(varkon_erpush("SU2993",errbuf));
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur925  dudt1 %f  dvdt1 %f \n", dudt1, dvdt1  );
fprintf(dbgfil(SURPAC),
"sur925  dudt2 %f  dvdt2 %f \n", dudt2, dvdt2  );
fprintf(dbgfil(SURPAC),
"sur925  dudt3 %f  dvdt3 %f \n", dudt3, dvdt3  );
fprintf(dbgfil(SURPAC),
"sur925  dudt  %f  dvdt  %f \n", dudt , dvdt   );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur925  alpha1 %f  alpha2 %f alpha3 %f\n", alpha1, alpha2, alpha3 );
}
#endif


/* Result to output vector          */

   v_leng = SQRT(dudt*dudt+dvdt*dvdt);
   if (v_leng < 0.00000000001 ) 
     {
     sprintf(errbuf,"Output=0%%sur925");
     return(varkon_erpush("SU2993",errbuf));
     }

   dudt = dudt/v_leng;
   dvdt = dvdt/v_leng;

   p_uvtang->x_gm = dudt;
   p_uvtang->y_gm = dvdt;
   p_uvtang->z_gm = 0.0;    

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur925 Exit***varkon_sur_d3toduv: dsdu= %f dsdv= %f \n",
                      p_uvtang->x_gm, p_uvtang->y_gm );
}
#endif

    return(SUCCED);

} /* End of function                                                */

/*!****************************************************************!*/
