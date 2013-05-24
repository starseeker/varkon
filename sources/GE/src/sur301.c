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
/*  Function: varkon_pat_sarea                     File: sur301.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Calculation of surface area, center of gravity and moments      */
/*  of inertia for a part of a patch.                               */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-09-16   Originally written                                 */
/*  1999-12-18   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_pat_sarea      Patch area c.o.g. m.o. inertia   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_eval       * Surface evaluation routine               */
/* varkon_erpush         * Error message to terminal                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_pat_sarea  (sur301) */
/* SU2993 = Severe program error (  ) in varkon_pat_sarea  (sur301) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_pat_sarea (

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf   *p_surin,     /* Input surface                     (ptr) */
  DBPatch  *p_patin,     /* Alloc. area for topol. patch data (ptr) */
  DBTmat   *p_csys,      /* Coordinate system for m.o.i.      (ptr) */
  DBint     c_case,      /* Calculation case:                       */
                         /* Eq. 1: ..                               */
                         /* Eq. 2: ..                               */
                         /* Eq. 3: ..                               */
   DBfloat  u_s,         /* Start U for rectangle in UV plane       */
   DBfloat  v_s,         /* Start V for rectangle in UV plane       */
   DBfloat  u_e,         /* End   U for rectangle in UV plane       */
   DBfloat  v_e,         /* End   V for rectangle in UV plane       */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
  DBfloat  *p_patarea,   /* Patch   area                      (ptr) */
  DBVector *p_patcog,    /* Center of gravity                 (ptr) */
  DBVector *p_patmoi )   /* Moments of inertia (jx,jy,jz)     (ptr) */
                         /* w.r.t to coordinate system p_csys       */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------- Theory --------------------------------------------*/
/*                                                                  */
/* Refer to Faux & Pratt .....                                      */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat  u_m;          /* Mid   U for rectangle in UV plane       */
  DBfloat  v_m;          /* Mid   V for rectangle in UV plane       */
  DBfloat  u_step;       /* Step  U for rectangle in UV plane       */
  DBfloat  v_step;       /* Step  V for rectangle in UV plane       */
  EVALS    p_1;          /* Point for u_s,v_s                       */
  EVALS    p_2;          /* Point for u_e,v_s                       */
  EVALS    p_3;          /* Point for u_s,v_e                       */
  EVALS    p_4;          /* Point for u_e,v_e                       */
  EVALS    p_12;         /* Point for u_m,v_s                       */
  EVALS    p_24;         /* Point for u_e,v_m                       */
  EVALS    p_43;         /* Point for u_m,v_e                       */
  EVALS    p_31;         /* Point for u_s,v_m                       */
  EVALS    p_mid;        /* Point for u_m,v_m                       */
  DBfloat  a_1;          /* Function value corresponding to p_1     */
  DBfloat  a_2;          /* Function value corresponding to p_2     */
  DBfloat  a_3;          /* Function value corresponding to p_3     */
  DBfloat  a_4;          /* Function value corresponding to p_4     */
  DBfloat  a_12;         /* Function value corresponding to p_12    */
  DBfloat  a_24;         /* Function value corresponding to p_24    */
  DBfloat  a_43;         /* Function value corresponding to p_43    */
  DBfloat  a_31;         /* Function value corresponding to p_31    */
  DBfloat  a_mid;        /* Function value corresponding to p_mid   */
  DBfloat  sum_all;      /* Sum of all function values              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

#ifdef  DEBUG
  DBfloat  dot1;         /* Dummy                                   */
  DBfloat  dot2;         /* Dummy                                   */
  DBfloat  darea;        /* Dummy                                   */
#endif
  DBint    icase;        /* Case for varkon_sur_eval                */
  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 Enter*varkon_pat_sarea * p_surin %d c_case %d\n",
          (int)p_surin, (int)c_case );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 p_patin %d p_csys %d \n", (int)p_patin, (int)p_csys );
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 u_s %6.3f v_s %6.3f u_e %6.3f v_e %6.3f \n", 
         u_s,v_s,u_e,v_e );
  }
#endif
 
#ifdef DEBUG
  *p_patarea       = F_UNDEF;       /* Surface area                 */
  p_patcog->x_gm   = F_UNDEF;       /* Center of gravity            */
  p_patcog->y_gm   = F_UNDEF;       /*                              */
  p_patcog->z_gm   = F_UNDEF;       /*                              */
  p_patmoi->x_gm   = F_UNDEF;       /* Moments of inertia           */
  p_patmoi->y_gm   = F_UNDEF;       /*                              */
  p_patmoi->z_gm   = F_UNDEF;       /*                              */
#endif

/*!                                                                 */
/* Calculate points and derivatives                                 */
/* Calls of varkon_sur_eval (sur210).                               */
/*                                                                 !*/

   u_m    = u_s + (u_e-u_s)/2.0;
   v_m    = v_s + (v_e-v_s)/2.0;
   u_step = u_e-u_s;
   v_step = v_e-v_s;

   icase = 3;

   status= varkon_sur_eval (p_surin,p_patin,icase,u_s,v_s,&p_1);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
 fprintf(dbgfil(SURPAC),
"sur301 varkon_sur_eval (sur210) failed for u_s %f v_s %f\n",u_s,v_s); 
#endif
    sprintf(errbuf,"varkon_sur_eval%%varkon_pat_sarea");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 u_s %8.4f v_s %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     u_s, v_s, p_1.r_x, p_1.r_y, p_1.r_z); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 u_s %8.4f v_s %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     u_s, v_s, p_1.u_x, p_1.u_y, p_1.u_z); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 u_s %8.4f v_s %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     u_s, v_s, p_1.v_x, p_1.v_y, p_1.v_z); 
  }
   dot1 =     ( p_1.u_x* p_1.u_x+ p_1.u_y* p_1.u_y+ p_1.u_z* p_1.u_z)*
              ( p_1.v_x* p_1.v_x+ p_1.v_y* p_1.v_y+ p_1.v_z* p_1.v_z);
   dot2 =     ( p_1.u_x* p_1.v_x+ p_1.u_y* p_1.v_y+ p_1.u_z* p_1.v_z )*
              ( p_1.u_x* p_1.v_x+ p_1.u_y* p_1.v_y+ p_1.u_z* p_1.v_z );
   darea = u_step*v_step*SQRT(dot1-dot2);
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 u_step %8.4f v_step %8.4f D1 %10.2f D2 %10.2f A %10.2f\n",
     u_step, v_step, dot1, dot2, darea); 
  }
#endif



   status= varkon_sur_eval (p_surin,p_patin,icase,u_e,v_s,&p_2);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur301 varkon_sur_eval (sur210) failed for u_e %f v_s %f\n",u_e,v_s); 
#endif
    sprintf(errbuf,"varkon_sur_eval%%varkon_pat_sarea");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 u_e %8.4f v_s %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     u_e, v_s, p_2.r_x, p_2.r_y, p_2.r_z); 
  }
#endif



   status= varkon_sur_eval (p_surin,p_patin,icase,u_s,v_e,&p_3);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur301 varkon_sur_eval (sur210) failed for u_s %f v_e %f\n",u_s,v_e); 
#endif
    sprintf(errbuf,"varkon_sur_eval%%varkon_pat_sarea");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 u_s %8.4f v_e %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     u_s, v_e, p_3.r_x, p_3.r_y, p_3.r_z); 
  }
#endif



   status= varkon_sur_eval (p_surin,p_patin,icase,u_e,v_e,&p_4);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur301 varkon_sur_eval (sur210) failed for u_e %f v_e %f\n",u_e,v_e); 
#endif
    sprintf(errbuf,"varkon_sur_eval%%varkon_pat_sarea");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 u_e %8.4f v_e %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     u_e, v_e, p_4.r_x, p_4.r_y, p_4.r_z); 
  }
#endif


   status= varkon_sur_eval (p_surin,p_patin,icase,u_m,v_s,&p_12);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur301 varkon_sur_eval (sur210) failed for u_m %f v_s %f\n"
         ,u_m,v_s); 
#endif
    sprintf(errbuf,"varkon_sur_eval%%varkon_pat_sarea");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 u_m %8.4f v_s %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     u_m, v_s, p_12.r_x, p_12.r_y, p_12.r_z); 
  }
#endif


   status= varkon_sur_eval (p_surin,p_patin,icase,u_e,v_m,&p_24);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur301 varkon_sur_eval (sur210) failed for u_e %f v_m %f\n",u_e,v_m); 
#endif
    sprintf(errbuf,"varkon_sur_eval%%varkon_pat_sarea");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 u_e %8.4f v_m %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     u_e, v_m, p_24.r_x, p_24.r_y, p_24.r_z); 
  }
#endif


   status= varkon_sur_eval (p_surin,p_patin,icase,u_m,v_e,&p_43);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur301 varkon_sur_eval (sur210) failed for u_m %f v_e %f\n",u_m,v_e); 
#endif
    sprintf(errbuf,"varkon_sur_eval%%varkon_pat_sarea");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 u_m %8.4f v_e %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     u_m, v_e, p_43.r_x, p_43.r_y, p_43.r_z); 
  }
#endif


   status= varkon_sur_eval (p_surin,p_patin,icase,u_s,v_m,&p_31);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur301 varkon_sur_eval (sur210) failed for u_s %f v_m %f\n",u_s,v_m); 
#endif
    sprintf(errbuf,"varkon_sur_eval%%varkon_pat_sarea");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 u_s %8.4f v_m %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     u_s, v_m, p_31.r_x, p_31.r_y, p_31.r_z); 
  }
#endif


   status= varkon_sur_eval (p_surin,p_patin,icase,u_m,v_m,&p_mid);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur301 varkon_sur_eval (sur210) failed for u_m %f v_m %f\n",u_m,v_m); 
#endif
    sprintf(errbuf,"varkon_sur_eval%%varkon_pat_sarea");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 u_m %8.4f v_m %8.4f X %10.2f Y %10.2f Z %10.2f\n",
     u_m, v_m, p_mid.r_x, p_mid.r_y, p_mid.r_z); 
  }
#endif

   a_1 = SQRT(( p_1.u_x* p_1.u_x+ p_1.u_y* p_1.u_y+ p_1.u_z* p_1.u_z)*
              ( p_1.v_x* p_1.v_x+ p_1.v_y* p_1.v_y+ p_1.v_z* p_1.v_z)-
              ( p_1.u_x* p_1.v_x+ p_1.u_y* p_1.v_y+ p_1.u_z* p_1.v_z )*
              ( p_1.u_x* p_1.v_x+ p_1.u_y* p_1.v_y+ p_1.u_z* p_1.v_z ));

   a_2 = SQRT(( p_2.u_x* p_2.u_x+ p_2.u_y* p_2.u_y+ p_2.u_z* p_2.u_z)*
             ( p_2.v_x* p_2.v_x+ p_2.v_y* p_2.v_y+ p_2.v_z* p_2.v_z)-
             ( p_2.u_x* p_2.v_x+ p_2.u_y* p_2.v_y+ p_2.u_z* p_2.v_z )*
             ( p_2.u_x* p_2.v_x+ p_2.u_y* p_2.v_y+ p_2.u_z* p_2.v_z ));

   a_3 = SQRT(( p_3.u_x* p_3.u_x+ p_3.u_y* p_3.u_y+ p_3.u_z* p_3.u_z)*
              ( p_3.v_x* p_3.v_x+ p_3.v_y* p_3.v_y+ p_3.v_z* p_3.v_z)-
              ( p_3.u_x* p_3.v_x+ p_3.u_y* p_3.v_y+ p_3.u_z* p_3.v_z )*
              ( p_3.u_x* p_3.v_x+ p_3.u_y* p_3.v_y+ p_3.u_z* p_3.v_z ));

   a_4 = SQRT(( p_4.u_x* p_4.u_x+ p_4.u_y* p_4.u_y+ p_4.u_z* p_4.u_z)*
              ( p_4.v_x* p_4.v_x+ p_4.v_y* p_4.v_y+ p_4.v_z* p_4.v_z)-
              ( p_4.u_x* p_4.v_x+ p_4.u_y* p_4.v_y+ p_4.u_z* p_4.v_z )*
              ( p_4.u_x* p_4.v_x+ p_4.u_y* p_4.v_y+ p_4.u_z* p_4.v_z ));

  a_12 = SQRT((p_12.u_x*p_12.u_x+p_12.u_y*p_12.u_y+p_12.u_z*p_12.u_z)*
              (p_12.v_x*p_12.v_x+p_12.v_y*p_12.v_y+p_12.v_z*p_12.v_z)-
              (p_12.u_x*p_12.v_x+p_12.u_y*p_12.v_y+p_12.u_z*p_12.v_z )*
              (p_12.u_x*p_12.v_x+p_12.u_y*p_12.v_y+p_12.u_z*p_12.v_z ));

  a_24 = SQRT((p_24.u_x*p_24.u_x+p_24.u_y*p_24.u_y+p_24.u_z*p_24.u_z)*
              (p_24.v_x*p_24.v_x+p_24.v_y*p_24.v_y+p_24.v_z*p_24.v_z)-
              (p_24.u_x*p_24.v_x+p_24.u_y*p_24.v_y+p_24.u_z*p_24.v_z )*
              (p_24.u_x*p_24.v_x+p_24.u_y*p_24.v_y+p_24.u_z*p_24.v_z ));

  a_43 = SQRT((p_43.u_x*p_43.u_x+p_43.u_y*p_43.u_y+p_43.u_z*p_43.u_z)*
              (p_43.v_x*p_43.v_x+p_43.v_y*p_43.v_y+p_43.v_z*p_43.v_z)-
              (p_43.u_x*p_43.v_x+p_43.u_y*p_43.v_y+p_43.u_z*p_43.v_z )*
              (p_43.u_x*p_43.v_x+p_43.u_y*p_43.v_y+p_43.u_z*p_43.v_z ));

  a_31 = SQRT((p_31.u_x*p_31.u_x+p_31.u_y*p_31.u_y+p_31.u_z*p_31.u_z)*
              (p_31.v_x*p_31.v_x+p_31.v_y*p_31.v_y+p_31.v_z*p_31.v_z)-
              (p_31.u_x*p_31.v_x+p_31.u_y*p_31.v_y+p_31.u_z*p_31.v_z )*
              (p_31.u_x*p_31.v_x+p_31.u_y*p_31.v_y+p_31.u_z*p_31.v_z ));

 a_mid= SQRT(
         (p_mid.u_x*p_mid.u_x+p_mid.u_y*p_mid.u_y+p_mid.u_z*p_mid.u_z)*
         (p_mid.v_x*p_mid.v_x+p_mid.v_y*p_mid.v_y+p_mid.v_z*p_mid.v_z)-
         (p_mid.u_x*p_mid.v_x+p_mid.u_y*p_mid.v_y+p_mid.u_z*p_mid.v_z )*
         (p_mid.u_x*p_mid.v_x+p_mid.u_y*p_mid.v_y+p_mid.u_z*p_mid.v_z));

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 a_1  %8.1f a_2  %8.1f a_3  %8.1f a_4  %8.1f  a_12 %8.1f\n",
     a_1, a_2, a_3, a_4, a_12 ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 a_24 %8.1f a_43 %8.1f a_31 %8.1f a_mid %8.1f \n",
     a_24, a_43, a_31, a_mid ); 
  }
#endif

  *p_patarea      =                 /* Patch part area              */
                 (u_step*v_step/36.0)*
             (a_1+a_2+a_3+a_4+4.0*(a_12+a_24+a_43+a_31)+16.0*a_mid);

/*!                                                                 */
/* 3. Center of gravity                                             */
/* ______________________                                           */
/*                                                                  */
/*                                                                 !*/

  sum_all = a_1 + a_2 + a_3 + a_4 + a_12 + a_24 + a_43 + a_31 + a_mid;

  if ( sum_all < 0.0001 )
     {
     p_patcog->x_gm =  0.0; 
     p_patcog->y_gm =  0.0; 
     p_patcog->z_gm =  0.0; 
     goto zero;
     }

  p_patcog->x_gm = a_1  /sum_all*p_1.r_x + 
                   a_2  /sum_all*p_2.r_x + 
                   a_3  /sum_all*p_3.r_x + 
                   a_4  /sum_all*p_4.r_x + 
                   a_12 /sum_all*p_12.r_x + 
                   a_24 /sum_all*p_24.r_x + 
                   a_43 /sum_all*p_43.r_x + 
                   a_31 /sum_all*p_31.r_x + 
                   a_mid/sum_all*p_mid.r_x  ;
  p_patcog->y_gm = a_1  /sum_all*p_1.r_y + 
                   a_2  /sum_all*p_2.r_y + 
                   a_3  /sum_all*p_3.r_y + 
                   a_4  /sum_all*p_4.r_y + 
                   a_12 /sum_all*p_12.r_y + 
                   a_24 /sum_all*p_24.r_y + 
                   a_43 /sum_all*p_43.r_y + 
                   a_31 /sum_all*p_31.r_y + 
                   a_mid/sum_all*p_mid.r_y  ;
  p_patcog->z_gm = a_1  /sum_all*p_1.r_z + 
                   a_2  /sum_all*p_2.r_z + 
                   a_3  /sum_all*p_3.r_z + 
                   a_4  /sum_all*p_4.r_z + 
                   a_12 /sum_all*p_12.r_z + 
                   a_24 /sum_all*p_24.r_z + 
                   a_43 /sum_all*p_43.r_z + 
                   a_31 /sum_all*p_31.r_z + 
                   a_mid/sum_all*p_mid.r_z  ;
#ifdef TEST_1
  p_patcog->x_gm =    1.0/9.0*   p_1.r_x + 
                      1.0/9.0*   p_2.r_x + 
                      1.0/9.0*   p_3.r_x + 
                      1.0/9.0*   p_4.r_x + 
                      1.0/9.0*   p_12.r_x + 
                      1.0/9.0*   p_24.r_x + 
                      1.0/9.0*   p_43.r_x + 
                      1.0/9.0*   p_31.r_x + 
                      1.0/9.0*   p_mid.r_x  ;
  p_patcog->y_gm =    1.0/9.0*   p_1.r_y + 
                      1.0/9.0*   p_2.r_y + 
                      1.0/9.0*   p_3.r_y + 
                      1.0/9.0*   p_4.r_y + 
                      1.0/9.0*   p_12.r_y + 
                      1.0/9.0*   p_24.r_y + 
                      1.0/9.0*   p_43.r_y + 
                      1.0/9.0*   p_31.r_y + 
                      1.0/9.0*   p_mid.r_y  ;
  p_patcog->z_gm =    1.0/9.0*   p_1.r_z + 
                      1.0/9.0*   p_2.r_z + 
                      1.0/9.0*   p_3.r_z + 
                      1.0/9.0*   p_4.r_z + 
                      1.0/9.0*   p_12.r_z + 
                      1.0/9.0*   p_24.r_z + 
                      1.0/9.0*   p_43.r_z + 
                      1.0/9.0*   p_31.r_z + 
                      1.0/9.0*   p_mid.r_z  ;
#endif  /* End   TEST_1   */

zero:;

/* 6. Exit                                                          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 p_patcog %9.2f v_s %9.2f u_e %9.2f \n",
               p_patcog->x_gm,p_patcog->y_gm,p_patcog->z_gm   );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur301 Exit u_s %6.3f v_s %6.3f u_e %6.3f v_e %6.3f Area %f\n",
               u_s,      v_s,      u_e,      v_e,   *p_patarea);
  fflush(dbgfil(SURPAC)); 
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
