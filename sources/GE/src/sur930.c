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
/*  Function: varkon_sur_curbr                     File: sur930.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates one of the curve branches in table EPOINT. */
/*                                                                  */
/*  Author:  Gunnar Liden                                           */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-04   Originally written                                 */
/*  1994-10-14   varkon_erinit                                      */
/*  1994-12-06   p_cur->hed_cu.type= CURTYP                         */
/*  1996-05-26   1,234 -> F_UNDEF Note , !!?                        */
/*               start_seg, max_seg                                 */
/*  1996-06-09   Debug                                              */
/*  1996-11-15   Compilation warnings                               */
/*  1997-04-04   Debug                                              */
/*  1999-11-24   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_curbr      Create one crv branch in EPOINT  */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short nextreg();       /* Retrieves next record in EPOINT    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBint   i_r;           /* Current  record in table EPOINT    */
static DBint   i_rprev;       /* Previous record in table EPOINT    */
static DBfloat ustart,vstart; /* Start U,V point for curve (part)   */
static DBfloat uend  ,vend;   /* End   U,V point for curve (part)   */
static DBfloat us_t,vs_t;     /* Start U,V tangent to ustart,vstart */
static DBfloat ue_t,ve_t;     /* End   U,V tangent to uend  ,vend   */
static DBfloat uls,vls;       /* Patch limit start point            */
static DBfloat ule,vle;       /* Patch limit end   point            */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_pat_uvcur      * Create UV curve in one patch   */
/*           varkon_sur_eval       * Surface evaluation routine     */
/*           varkon_vec_projpla    * Project vector onto plane      */
/*           GEmktf_3p();          * Create a local coord. system   */
/*           varkon_erinit         * Initiate error message stack   */
/*           varkon_erpush         * Error message to terminal      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2933 = Requested crv branch %s does not exist (fctn sur930)    */
/* SU2943 = Called function xxxxxx failed in varkon_sur_curbr       */
/* SU2993 = Severe program error in varkon_sur_curbr (sur930).      */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_curbr (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBSurf  *p_sur,       /* Surface                           (ptr) */
   DBPatch *p_pat,       /* Alloc. area for topol. patch data (ptr) */
   IRUNON  *p_comp,      /* Computation data                  (ptr) */
   EPOINT  *p_etable,    /* Enter/exit point table            (ptr) */
   DBint    no_ep,       /* Number of records        in EPOINT      */
   DBint    no_br,       /* Number of curve branches in EPOINT      */
   DBint    cbsr[CBMAX], /* Curve branches start records            */
   DBint    cur_no,      /* Requested output curve branch number    */
   DBCurve *p_cur,       /* Surface curve                     (ptr) */
   DBSeg   *p_seg )      /* Allocated area for UV segments    (ptr) */
/* Out:                                                             */
/*      Header data and coefficients to DBCurve and DBSeg           */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!New-Page--------------------------------------------------------!*/

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  no_all;        /* Total number of U,V curve segments      */
   DBint  nseg;          /* Number of U,V curve segments from patch */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint  start_seg;     /* Start segment number for curve in patch */
   DBint  max_seg;       /* Maximum number of segments for curve    */
   DBVector origin;      /* Origin for intersect coord. system      */
   DBVector vecx;        /* X axis for intersect coord. system      */
   DBVector vecy;        /* Y axis for intersect coord. system      */
   DBTmat icsys;         /* Intersect coordinate system             */
   EVALS  xyz;           /* Coordinates and derivatives             */
                         /* for a point on a surface                */
   DBfloat  u_o,v_o;     /* U,V for origin point                    */
   DBfloat  u_x,v_x;     /* U,V for X axis point                    */
   DBfloat  pi_x,pi_y;   /* Intersecting plane                      */
   DBfloat  pi_z,pi_d;   /*                                         */
   DBVector plane;       /* Plane  normal                           */
   DBint  ocase;         /* Output case. Eq. 1: Normalised vector   */
   

   char   errbuf[80];    /* String for error message fctn erpush    */
   DBint  no_iter;       /* Number of iterations                    */
   DBint  status;        /* Error code from a called function       */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
   i_r    = cbsr[cur_no-1];      /* Current  record number          */
   i_rprev= -1;                  /* Previous record number          */
   no_all= 0;                    /* Total number of segments        */
/*                                                                 !*/

   if ( cur_no > 0 && cur_no <= no_br )
     {
     ;
     }
   else
   {
     sprintf(errbuf,"%d%%sur930",(int)cur_no);
     varkon_erinit();
     return(varkon_erpush("SU2933",errbuf));
   }

   u_o = F_UNDEF;            /* U,V for origin point                */
   v_o = F_UNDEF;         
   u_x = F_UNDEF;            /* U,V for X axis point                */
   v_x = F_UNDEF;      

   start_seg = I_UNDEF;

/*!                                                                 */
/* Size of area allocated for curve to max_seg from p_cur->ns_su    */
/*                                                                 !*/
   max_seg = (DBint)(p_cur)->ns_cu; 

   no_iter = 0;

/*!                                                                 */
/* 2. Calculate coefficients for the whole curve                    */
/* ______________________________________________                   */
/*                                                                 !*/


/*!                                                                 */
newrec:  /* Label: Next record from EPOINT                          */
/*                                                                 !*/

  no_iter=no_iter+1;
  if (no_iter > EPMAX )
    {
    sprintf(errbuf, "(iter)%%varkon_sur_curbr(sur930");
    return(varkon_erpush("SU2993",errbuf));
    }

/*!                                                                 */
/* Retrieve the start and end U,V point from EPOINT                 */
/* Call of nextreg.                                                 */
/*                                                                 !*/

   status=nextreg(p_etable); 
   if (status<0) 
     {
     sprintf(errbuf,"nextreg%%varkon_sur_curbr (sur920)");
     return(varkon_erpush("SU2943",errbuf));
     }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur930 i_r= %d ustart= %f vstart= %f uend= %f vend= %f\n", 
 (int)i_r, ustart , vstart, uend,vend);            
  }
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur930 i_r= %d us_t %f vs_t %f ue_t %f ve_t %f\n", 
  (int)i_r, us_t , vs_t , ue_t , ve_t );            
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* Calculate curve segments in the current patch                    */
/* Call of varkon_pat_uvcur  (sur940).                              */
/*                                                                 !*/

  start_seg = no_all;  /* For warning message in sur940             */

  status=varkon_pat_uvcur 
   (p_sur,p_pat,p_comp,uls,vls,ule,vle,
    ustart,vstart,uend,vend,us_t,vs_t,ue_t,ve_t,
    start_seg,max_seg,p_cur,p_seg+no_all,&nseg);
   if (status<0) 
        {
        sprintf(errbuf,"varkon_pat_uvcur%%varkon_sur_curbr ");
        return(varkon_erpush("SU2943",errbuf));
        }

   if ( no_all == 0 && p_comp->ipl_un.ctype == SURPLAN)
      {
      u_o = ustart;          /* U,V for origin point                */
      v_o = vstart;         
      u_x = uend;            /* U,V for X axis point                */
      v_x = vend;         
      }

/*!                                                                 */
/* Add the number of output segments from the patch to the total    */
   no_all = no_all + nseg;
/*                                                                 !*/

   if ( no_all >= max_seg     )  /* Programming check              */
    {
    sprintf(errbuf, "(no_all> 500  )%%varkon_sur_curbr");
    return(varkon_erpush("SU2993",errbuf));
    }

/*!                                                                 */
/* Continue (goto newrec) if there are additional records.          */
/* Goto allrec for end of records (i_r = -1).                       */
/*                                                                 !*/


   if ( i_r == -1 ) 
       {
       goto allrec;
       }
   else
       {
       goto newrec;
       }


/*!                                                                 */
allrec:           /* Label: All records used for the curve branch   */

/* 3. Exit                                                          */
/* _______                                                          */
/*                                                                  */

/* Data (header) for the curve                                      */
/* Number of segments:                                              */
  (p_cur)->ns_cu = (short)no_all;
/* Entity is a curve (CURTYP)                                       */
/*                                                                 !*/

  p_cur->hed_cu.type= CURTYP;/* 1994-12-06       G. Liden           */

/*!                                                                 */
/* Create coordinate system for a planar curve (SURPLAN type).      */
/* Calls of varkon_sur_eval (sur210) and GEmktf_3p.                 */
/*                                                                 !*/

   p_cur->plank_cu  = FALSE;       /* Initiate planar flag          */

   if ( p_comp->ipl_un.ctype == SURPLAN)
      {
      status=varkon_sur_eval
         (p_sur,p_pat,(DBint)1,u_o ,v_o,&xyz);
      if (status<0) 
      {
        sprintf(errbuf,"varkon_sur_eval%%varkon_cur_curbr ");
        return(varkon_erpush("SU2943",errbuf));
      }

      origin.x_gm = xyz.r_x;               
      origin.y_gm = xyz.r_y;               
      origin.z_gm = xyz.r_z;               

      status=varkon_sur_eval
        (p_sur,p_pat,(DBint)1,u_x ,v_x,&xyz);
      if (status<0) 
      {
        sprintf(errbuf,"varkon_sur_eval%%varkon_cur_curbr ");
        return(varkon_erpush("SU2943",errbuf));
      }

      vecx.x_gm= xyz.r_x;
      vecx.y_gm= xyz.r_y;
      vecx.z_gm= xyz.r_z;

      vecx.x_gm= vecx.x_gm - origin.x_gm;
      vecx.y_gm= vecx.y_gm - origin.y_gm;
      vecx.z_gm= vecx.z_gm - origin.z_gm;

      pi_x = p_comp->ipl_un.in_x;
      pi_y = p_comp->ipl_un.in_y;
      pi_z = p_comp->ipl_un.in_z;
      pi_d = p_comp->ipl_un.in_d;
      plane.x_gm = pi_x;      /* Plane  normal                      */
      plane.y_gm = pi_y;
      plane.z_gm = pi_z;

      ocase = 1;

      status= varkon_vec_projpla (vecx,plane,ocase,&vecx);
      if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_eval%%varkon_cur_curbr ");
        return(varkon_erpush("SU2943",errbuf));
        }

      vecy.x_gm = pi_y*vecx.z_gm - pi_z*vecx.y_gm;  
      vecy.y_gm = pi_z*vecx.x_gm - pi_x*vecx.z_gm;  
      vecy.z_gm = pi_x*vecx.y_gm - pi_y*vecx.x_gm;  

     status = GEmktf_3p (&origin,&vecx,&vecy,&icsys);
     if (status<0) 
     {
        sprintf(errbuf,"GEmktf_3p%%varkon_sur_curbr");
        return(varkon_erpush("SU2943",errbuf));
     }

     p_cur->plank_cu  = TRUE;       
     p_cur->csy_cu    = icsys;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2  )
{
  fprintf(dbgfil(SURPAC),"sur930 Intersect coordinate system\n");
  fprintf(dbgfil(SURPAC),"sur930 g11 %f g12 %f g13 %f g14 %f \n",
                  icsys.g11,icsys.g12,icsys.g13,icsys.g14);
  fprintf(dbgfil(SURPAC),"sur930 g21 %f g22 %f g23 %f g24 %f \n",
                  icsys.g21,icsys.g22,icsys.g23,icsys.g24);
  fprintf(dbgfil(SURPAC),"sur930 g31 %f g32 %f g33 %f g34 %f \n",
                  icsys.g31,icsys.g32,icsys.g33,icsys.g34);
  fprintf(dbgfil(SURPAC),"sur930 g41 %f g42 %f g43 %f g44 %f \n",
                  icsys.g41,icsys.g42,icsys.g43,icsys.g44);
}
#endif
      }  /* End SURPLAN  */




#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur930 Exit *** varkon_sur_curbr ** Number of output segments %d\n"
   ,  (int)no_all );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/

/*!********* Internal ** function ** nextreg ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function retrieves start and end point for the current       */
/* record in EPOINT.                                                */

     static short   nextreg(p_etable)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   EPOINT  *p_etable;    /* Pointer to enter/exit point table       */
/* Out:                                                             */
/*     Points ustart,vstart and uend,vend and next record no i_r    */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint    i_rnext;     /* Next     record                         */
/*-----------------------------------------------------------------!*/


/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Patch limit points from EPOINT                                */
/*                                                                 !*/

  uls   = (p_etable+i_r-1)->us;
  vls   = (p_etable+i_r-1)->vs;
  ule   = (p_etable+i_r-1)->ue;
  vle   = (p_etable+i_r-1)->ve;


/*!                                                                 */
/* 2. Start and end points from EPOINT                              */
/*                                                                 !*/

  if((p_etable+i_r-1)->ptr1==i_rprev)
    {
    i_rnext= (p_etable+i_r-1)->ptr2;     /* Next     record number  */
    ustart= (p_etable+i_r-1)->u1;
    vstart= (p_etable+i_r-1)->v1;   
    uend  = (p_etable+i_r-1)->u2;
    vend  = (p_etable+i_r-1)->v2;   
    us_t  = (p_etable+i_r-1)->u1_t;
    vs_t  = (p_etable+i_r-1)->v1_t;   
    ue_t  = (p_etable+i_r-1)->u2_t;
    ve_t  = (p_etable+i_r-1)->v2_t;   
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "nextreg u1= %f v1= %f     ptr1= %d i_rnext= %d\n",
     (p_etable+i_r-1)->u1, (p_etable+i_r-1)->v1,   
     (int)(p_etable+i_r-1)->ptr1,(int)i_rnext);
  }
#endif
    }
  else
    {
    i_rnext= (p_etable+i_r-1)->ptr1;     /* Next     record number  */
    ustart= (p_etable+i_r-1)->u2;
    vstart= (p_etable+i_r-1)->v2;   
    uend  = (p_etable+i_r-1)->u1;
    vend  = (p_etable+i_r-1)->v1;   
    us_t  = (p_etable+i_r-1)->u2_t;
    vs_t  = (p_etable+i_r-1)->v2_t;   
    ue_t  = (p_etable+i_r-1)->u1_t;
    ve_t  = (p_etable+i_r-1)->v1_t;   
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "nextreg u2= %f v2= %f     ptr2= %d i_rnext= %d\n",
     (p_etable+i_r-1)->u2, (p_etable+i_r-1)->v2,   
     (int)(p_etable+i_r-1)->ptr2,(int)i_rnext);
  }
#endif
    }

  i_rprev= i_r;

  i_r    = i_rnext;


    return(SUCCED);

} /* End of function                                                */
/********************************************************************/
