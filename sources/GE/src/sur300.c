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
/*  Function: varkon_sur_sarea                     File: sur300.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Calculation of surface area, center of gravity and moments      */
/*  of inertia for a surface.                                       */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-09-16   Originally written                                 */
/*  1997-12-21   Input end criterion                                */
/*  1999-12-01   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_sarea      Surface area c.o.g. m.o. inertia */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_nopatch    * Retrieve number of patches               */
/* varkon_pat_sarea      * Patch area, c.o.g., m.o.i                */
/* varkon_erpush         * Error message to terminal                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_sarea  (sur300) */
/* SU2993 = Severe program error (  ) in varkon_sur_sarea  (sur300) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_sarea (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf   *p_surin,     /* Input surface                     (ptr) */
  DBPatch  *p_patin,     /* Alloc. area for topol. patch data (ptr) */
  DBTmat   *p_csys,      /* Coordinate system for m.o.i.      (ptr) */
  DBint     acc,         /* Calculation start accuracy value:       */
                         /* Eq. 1: Whole patch                      */
                         /* Eq. 2: Patch divided  4 times           */
                         /* Eq. 3: Patch divided  9 times           */
                         /* Eq. 4:  .....                           */
  DBfloat   a_crit_in,   /* End area calculation criterion          */
                         /* <= 0.0 ==> 0.01 = 1 %                   */
  DBint     c_case,      /* Calculation case:                       */
                         /* Eq. 1: Use only acc and not a_crit      */
                         /* Eq. 2: Use acc as start and a_crit      */
                         /* Eq. 3: ..                               */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
  DBfloat  *p_sarea,     /* Surface area                      (ptr) */
  DBVector *p_cog,       /* Center of gravity                 (ptr) */
  DBVector *p_moi,       /* Moments of inertia (jx,jy,jz)     (ptr) */
                         /* w.r.t to coordinate system p_csys       */
  DBVector *p_axis1,     /* Principal axis 1                  (ptr) */
  DBVector *p_axis2,     /* Principal axis 2                  (ptr) */
  DBVector *p_axis3 )    /* Principal axis 3                  (ptr) */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  a_crit;      /* End area calculation criterion          */
   DBint    iu;          /* Loop index corresponding to u line      */
   DBint    iv;          /* Loop index corresponding to v line      */
   DBint    iu_acc;      /* Loop index corresponding to acc in u dir*/
   DBint    iv_acc;      /* Loop index corresponding to acc in v dir*/
   DBint    nu;          /* Number of patches in U direction        */
   DBint    nv;          /* Number of patches in V direction        */
   DBint    surtype;     /* Type of surface                         */
   DBfloat  d_uv;        /* Delta parameter value defined by acc    */
   DBfloat  u_s;         /* Start U for rectangle in UV plane       */
   DBfloat  v_s;         /* Start V for rectangle in UV plane       */
   DBfloat  u_e;         /* End   U for rectangle in UV plane       */
   DBfloat  v_e;         /* End   V for rectangle in UV plane       */
   DBfloat  patarea;     /* Patch part area                         */
   DBVector patcog;      /* Center of gravity patch part            */
   DBVector patmoi;      /* Moments of inertia patch part           */
   DBfloat  surfarea;    /* Surface    area                         */
   DBfloat  startarea;   /* Surface start area                      */
   DBfloat  a_all[10];   /* Surface area from all iterations        */
   DBint    a_max;       /* Maximum number of area calc. iterations */
   DBint    a_cur;       /* Current number of area calc. iterations */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint    status;      /* Error code from called function         */
   char     errbuf[80];  /* String for error message fctn erpush    */

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
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur300 Enter*varkon_sur_sarea * p_surin %d acc %d c_case %d\n",
          (int)p_surin, (int)acc, (int)c_case );
  fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur300 p_patin %d p_csys %d \n", (int)p_patin, (int)p_csys );
  }
#endif
 
#ifdef DEBUG
  *p_sarea      = F_UNDEF;        /* Surface area                   */
  p_cog->x_gm   = F_UNDEF;        /* Center of gravity              */
  p_cog->y_gm   = F_UNDEF;        /*                                */
  p_cog->z_gm   = F_UNDEF;        /*                                */
  p_moi->x_gm   = F_UNDEF;        /* Moments of inertia             */
  p_moi->y_gm   = F_UNDEF;        /*                                */
  p_moi->z_gm   = F_UNDEF;        /*                                */
  p_axis1->x_gm = F_UNDEF;        /* Principal axis                 */
  p_axis1->y_gm = F_UNDEF;        /*                                */
  p_axis1->z_gm = F_UNDEF;        /*                                */
  p_axis2->x_gm = F_UNDEF;        /* Principal axis                 */
  p_axis2->y_gm = F_UNDEF;        /*                                */
  p_axis2->z_gm = F_UNDEF;        /*                                */
  p_axis3->x_gm = F_UNDEF;        /* Principal axis                 */
  p_axis3->y_gm = F_UNDEF;        /*                                */
  p_axis3->z_gm = F_UNDEF;        /*                                */
  a_crit        = F_UNDEF;
#endif

/*!                                                                 */
/* End criterion a_crit. Default 1% if not defined                  */
/*                                                                 !*/

  if   ( a_crit_in < 0.0000000001 ) a_crit =  0.01;


/*!                                                                 */
/* Maximum number of area calculations is 10 for c_case= 2          */
/*                                                                 !*/

  if      ( c_case == 1 ) a_max =  1;
  else if ( c_case == 2 ) a_max = 10;
  else 
    {
    sprintf(errbuf,"c_case%%sur300");
    return(varkon_erpush("SU2993",errbuf));
    }

  a_cur     = 0;
  a_all[0]  = F_UNDEF;
  a_all[1]  = F_UNDEF;
  a_all[2]  = F_UNDEF;
  a_all[3]  = F_UNDEF;
  a_all[4]  = F_UNDEF;
  a_all[5]  = F_UNDEF;
  a_all[6]  = F_UNDEF;
  a_all[7]  = F_UNDEF;
  a_all[8]  = F_UNDEF;
  a_all[9]  = F_UNDEF;

/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/* For Debug On: Check the surface type                             */
/*                                                                 !*/


    status = varkon_sur_nopatch (p_surin, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"sur230%%sur300");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


#ifdef DEBUG
if      ( surtype     ==  CUB_SUR ||   /* Check surface type      */
          surtype     ==  RAT_SUR ||           
          surtype     ==  CON_SUR ||           
          surtype     ==  POL_SUR ||           
          surtype     ==   P3_SUR ||           
          surtype     ==   P5_SUR ||           
          surtype     ==   P7_SUR ||           
          surtype     ==   P9_SUR ||           
          surtype     ==  P21_SUR ||           
          surtype     ==  MIX_SUR ||           
          surtype     == NURB_SUR ||           
          surtype     ==  LFT_SUR )            
 ;
else if ( surtype     == FAC_SUR )        /* Facetted surface not OK */
 {
 sprintf(errbuf,    
"(FAC_SUR)%%sur300");
 return(varkon_erpush("SU2993",errbuf)); 
 }
 else
 {
 sprintf(errbuf,  
 "(type)%%sur300");
 return(varkon_erpush("SU2993",errbuf));
 }
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur300 Surface type=%d\n",
p_surin->typ_su);
fprintf(dbgfil(SURPAC),"sur300 No_patches in U nu=%d\n",
(int)nu);
fprintf(dbgfil(SURPAC),"sur300 No_patches in V nv=%d\n",
(int)nv);
}
#endif

/*!                                                                 */
/* Calculate delta parameter d_uv for the given accuracy acc        */
/*                                                                 !*/

nxt_area:; /* Label: Next area calculation                          */

  if  ( acc > 0 && acc < 45 )
    {
    d_uv = 1.0/(DBfloat)acc;
    }

/*!                                                                 */
/* 3. Calculate mass data                                           */
/* ______________________                                           */
/*                                                                  */
/* Loop all V patches  iv=0,1,....,nv-1                             */
/*  Loop all U patches  iu=0,1,....,nu-1                            */
/*   Loop extra V        iv_acc=0,1,....,acc                        */
/*    Loop extra U        iu_acc=0,1,....,acc                       */
/*                                                                 !*/

  surfarea = 0.0;                        /* Initialize total area   */
  p_cog->x_gm= 0.0;                      /* Initialize c.o.g        */
  p_cog->y_gm= 0.0;
  p_cog->z_gm= 0.0; 

for ( iv=0; iv< nv; ++iv )               /* Start loop V patches    */
  {
  for ( iu=0; iu< nu; ++iu )             /* Start loop U patches    */
    {

    for ( iv_acc=0;iv_acc< acc; ++iv_acc )  /* Extra in V           */
      {
      for ( iu_acc=0;iu_acc< acc; ++iu_acc )/* Extra in U           */
        {
/*!                                                                 */
/*      Limits in UV plane for rectangular area                     */
/*                                                                 !*/
        u_s = (DBfloat)(iu+1) + (DBfloat)(iu_acc)*d_uv; 
        v_s = (DBfloat)(iv+1) + (DBfloat)(iv_acc)*d_uv; 
        u_e = (DBfloat)(iu+1) + (DBfloat)(iu_acc+1)*d_uv; 
        v_e = (DBfloat)(iv+1) + (DBfloat)(iv_acc+1)*d_uv; 

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur300 u_s %6.3f v_s %6.3f u_e %6.3f v_e %6.3f \n",u_s,v_s,u_e,v_e);
}
#endif

    status = varkon_pat_sarea  
  (p_surin,p_patin, p_csys, c_case, u_s, v_s, u_e, v_e,
                &patarea, &patcog, &patmoi );
    if(status<0)
    {
    sprintf(errbuf,"varkon_pat_sarea%%varkon_sur_sarea (sur300)");
    return(varkon_erpush("SU2943",errbuf));
    }

         /* Center of gravity */
         if ( ABS(surfarea + patarea) > 0.000000001 )
            {
            p_cog->x_gm= (surfarea*p_cog->x_gm+patarea*patcog.x_gm)/
                              (surfarea + patarea); 
            p_cog->y_gm= (surfarea*p_cog->y_gm+patarea*patcog.y_gm)/
                              (surfarea + patarea);
            p_cog->z_gm= (surfarea*p_cog->z_gm+patarea*patcog.z_gm)/
                              (surfarea + patarea);
             }

         surfarea = surfarea + patarea;        /* Total area        */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur300 p_cog %10.2f %10.2f %10.2f surfarea %f\n",
       p_cog->x_gm, p_cog->y_gm, p_cog->z_gm, surfarea  );
  fflush(dbgfil(SURPAC)); /* From buffer to file */
  }
#endif


        }                                /* Extra in U              */
      }                                  /* Extra in V              */
    }                                    /* End   loop U patches    */
  }                                      /* End   loop V patches    */
/*!                                                                 */
/*    End  extra U        iu_acc=0,1,....,acc                       */
/*   End  extra V        iv_acc=0,1,....,acc                        */
/*  End  all U patches  iu=0,1,....,nu-1                            */
/* End  all V patches  iv=0,1,....,nv-1                             */
/*                                                                 !*/

  a_all[a_cur]  = surfarea;
  a_cur         = a_cur + 1;


  if  ( a_cur == 1 )
    {
    startarea = surfarea;
    if  ( fabs(startarea) < 0.000001 )
      {
       sprintf(errbuf, "(startarea=0)%%sur300");
       return(varkon_erpush("SU2993",errbuf)); 
       }
    }




#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 && a_cur == 1 )
{
fprintf(dbgfil(SURPAC),
"sur300 a_cur %d surfarea %25.10f\n",(int)a_cur, surfarea);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 && a_cur >= 2 )
{
fprintf(dbgfil(SURPAC),
"sur300 a_cur %d surfarea %25.10f Difference %f acc %d\n",
    (int)a_cur, surfarea,
  (a_all[a_cur-1]-a_all[a_cur-2])/startarea, (int)acc );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Check if the requested accuracy is reached                       */
/* Recalculate if not ..                                            */
/*                                                                 !*/

  if       (   a_cur == 1 && a_max == 1 ) 
    {
    ; /* End of calculation */
    }
  else if  (   a_cur >= 10  ) 
    {
    ; /* End of calculation */
    }
  else if  (   acc >= 45  ) 
    {
    ; /* End of calculation */
    }
  else if  (   a_cur < 2 ) 
    {
    acc = acc + 1;
    goto nxt_area;
    }
  else if  ( fabs(a_all[a_cur-1]-a_all[a_cur-2])/startarea < a_crit  ) 
    {
    ; /* End of calculation */
    }
  else
    {
    acc = acc + 1;
    goto nxt_area;
    }



  *p_sarea = surfarea;

  
/* 6. Exit                                                          */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur300 p_cog %f %f %f\n",p_cog->x_gm, p_cog->y_gm, p_cog->z_gm  );
  fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur300 Exit varkon_sur_sarea *p_sarea %25.10f\n",*p_sarea);
  fflush(dbgfil(SURPAC)); 
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
