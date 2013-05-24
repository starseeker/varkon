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
/*  Function: varkon_cur_usrche                    File: sur367.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function check a CUR_USRDEF curve on a surface              */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1998-03-29   Originally written                                 */
/*  1999-12-18   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_usrche     Check CUR_USRDEF on a surface    */
/*                                                              */
/*------------------------------------------------------------- */

/* --------------------- Theory ------------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_gmlin       * Initialize DBLine                       */
/* varkon_cur_bound       * Bounding box for a curve                */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* varkon_geo710          * Intersect line/curve                    */
/* varkon_cur_analysis    * Position continuity analysis            */
/* varkon_idknot          * Retrieve parameter tolerance            */
/* varkon_comptol         * Retrieve computer tolerance             */
/* varkon_erinit          * Initialize error message stack          */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_cur_usrche           */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus         varkon_cur_usrche (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Surface                           (ptr) */
  DBPatch *p_pat,        /* Alloc. area for topol. patch data (ptr) */
  DBCurve *p_cur,        /* Current curve                     (ptr) */
  DBSeg   *p_seg )       /* Coefficients for curve segments   (ptr) */

/* Out:                                                             */
/*        p_seg data will be modified                               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat  u_min;       /* Minimum U parameter value in surface    */
   DBfloat  u_max;       /* Maximum U parameter value in surface    */
   DBfloat  v_min;       /* Minimum V parameter value in surface    */
   DBfloat  v_max;       /* Maximum V parameter value in surface    */

   DBfloat  us_pat;      /* Patch boundary U start value            */
   DBfloat  ue_pat;      /* Patch boundary U end   value            */
   DBfloat  vs_pat;      /* Patch boundary V start value            */
   DBfloat  ve_pat;      /* Patch boundary V end   value            */

   DBint    i_pat;       /* Loop index topology patch               */

   DBVector p1;          /* Start point for line                    */
   DBVector p2;          /* End   point for line                    */
   DBLine   lin;         /* Line                                    */

   DBint    i_sol;       /* Loop index solution                     */
   DBfloat  u_lin;       /* Current parameter for intersect, line   */
   DBfloat  u_cur;       /* Current parameter for intersect, curve  */

   short   noint;        /* Number of intersects                    */
   DBfloat uout1[25*INTMAX]; /* Parameter values for intersect      */
   DBfloat uout2[25*INTMAX]; /* Parameter values for intersect      */

   DBint    int_part;    /* Integer part of curve parameter value   */
   DBfloat  dec_part;    /* Decimal part of curve parameter value   */
   DBint    bound_flag;  /* Flag =0: On boundary  =1: Not boundary  */
   BBOX     cur_box;     /* Bounding box for the curve              */
   DBint    nu;          /* Number of patches in U direction        */
   DBint    nv;          /* Number of patches in V direction        */
   DBint    surtype;     /* Type CUB_SUR, RAT_SUR, or .....         */
   DBint    i_seg;       /* Loop index segment in curve             */

                         /* For varkon_cur_analysis (sur792):       */
   DBint  c_case;        /* Computation case:                       */
                         /* Eq. 1: Position   only                  */
                         /* Eq. 2: Derivative added                 */
                         /* Eq. 3: Curvature  added                 */
  DBfloat  pcrit;        /* Position criterion < 0 ==> system value */
  DBint    n_p;          /* Number of steps in position             */
  DBint    piseg[SUANA]; /* Segments (end pt) with position steps   */
  DBfloat  dcrit;        /* Derivat. criterion < 0 ==> system value */
  DBint    n_d;          /* Number of steps in derivative           */
  DBint    diseg[SUANA]; /* Segments (end pt) with derivat. steps   */
  DBfloat  ccrit;        /* Radius   criterion < 0 ==> system value */
  DBint    n_c;          /* Number of steps in radius of curvature  */
  DBint    ciseg[SUANA]; /* Segments (end pt) with curvat.  steps   */
  DBint    n_s;          /* Number of too short segments            */
  DBint    siseg[SUANA]; /* Segments with too short chord lengths   */

/*                                                                  */
/*-----------------------------------------------------------------!*/
  DBfloat comptol;       /* Computer tolerance (accuracy)           */
  DBfloat idknot;        /* Identical knots criterion               */
  DBint   status;        /* Error code from a called function       */
  char    errbuf[80];    /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur367 Enter varkon_cur_usrche Check a surface CUR_USRDEF crv\n");
}

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur367 p_cur->ns_cu %d p_sur->typ_su %d\n", 
      (int)p_cur->ns_cu  , (int)p_sur->typ_su);
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/*  1. Initializations                                              */
/*  __________________                                              */
/*                                                                  */
/*                                                                 !*/

/* Initialize local variables                                       */
   u_min       = F_UNDEF;
   u_max       = F_UNDEF;
   v_min       = F_UNDEF;
   v_max       = F_UNDEF;
   us_pat      = F_UNDEF;
   ue_pat      = F_UNDEF;
   vs_pat      = F_UNDEF;
   ve_pat      = F_UNDEF;
   u_lin       = F_UNDEF;
   u_cur       = F_UNDEF;
   dec_part    = F_UNDEF;
   noint       = I_UNDEF;
   int_part    = I_UNDEF;
   bound_flag  = I_UNDEF;
   for(i_sol=0;i_sol< 25*INTMAX; ++i_sol)uout1[i_sol]= F_UNDEF;
   for(i_sol=0;i_sol< 25*INTMAX; ++i_sol)uout2[i_sol]= F_UNDEF;

/*!                                                                 */
/* The curve (segments) shall be handled like a R*3 curve in        */
/* this function: UV_CUB_SEG --> CUB_SEG                                */
/*                                                                 !*/

   for ( i_seg= 0; i_seg < p_cur->ns_cu; ++i_seg )
       (p_seg+i_seg)->typ = CUB_SEG;

/*!                                                                 */
/* Initialize DBLine. Call of varkon_ini_gmlin (sur764).            */
/*                                                                 !*/

   varkon_ini_gmlin(&lin);

/* Type of entity                                                   */
   lin.hed_l.type=  LINTYP;       

/*!                                                                 */
/* Retrieve computer tolerance criterion.                           */
/* Call of varkon_comptol (sur753).                                 */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* Identical knots criterion. Call of varkon_idknot (sur744).       */
/*                                                                 !*/

   idknot =varkon_idknot();

   idknot = 50.0*idknot; /* Don't really know the size ....         */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur367 comptol %25.15f idknot %f\n", comptol ,idknot);
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Calculate the bounding box for the curve.                        */
/* Call of varkon_cur_bound (sur915).                               */
/*                                                                 !*/

    status=varkon_cur_bound (p_cur,p_seg,&cur_box);
    if (status<0) 
      {
      sprintf(errbuf,"sur915%%sur367 ");
      return(erpush("SU2943",errbuf));
      }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur367 xmin  %7.4f xmax  %7.4f ymin  %7.4f ymax  %7.4f\n", 
        cur_box.xmin, cur_box.xmax, cur_box.ymin, cur_box.ymax );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur367 cur_box.zmin %20.12f _box.zmax %20.12f\n", 
        cur_box.zmin, cur_box.zmax );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/*  2. Check that the input curve is 2D curve                       */
/*  _________________________________________                       */
/*                                                                  */
/*                                                                 !*/


/*!                                                                 */
/* Check that the curve is in plane Z= 0.0                          */
/*                                                                 !*/

    if ( fabs(cur_box.zmin) > comptol ||  
         fabs(cur_box.zmax) > comptol    ) 
      {
      sprintf(errbuf,"sur367%% ");
      varkon_erinit();
      return(varkon_erpush("SU6803",errbuf));
      }

/*!                                                                 */
/*  3. Check that the curve is inside the surface definition area   */
/*  _____________________________________________________________   */
/*                                                                  */
/*                                                                 !*/


/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype);
    if(status<0)
    {
    sprintf(errbuf,"sur230%%sur367");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur367 nu %d nv %d surtype %d \n", 
      (int)nu, (int)nv, (int)surtype   );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Assume that surface types CUB_SUR, CON_SUR, .... have the        */
/* surface definition area defined by the number of patches         */
/* (which is the case 1998-03-29).                                  */
/* For a NURBS surface must the surface definition area be          */
/* retrieved ("calculated") from the data in the topological        */
/* patch. (In the future for all surface types .... )               */
/*                                                                 !*/

  if       ( surtype     ==  CUB_SUR || 
             surtype     ==  RAT_SUR ||           
             surtype     ==  CON_SUR ||           
             surtype     ==   P3_SUR ||           
             surtype     ==   P5_SUR ||           
             surtype     ==   P7_SUR ||           
             surtype     ==   P9_SUR ||           
             surtype     ==  P21_SUR ||           
             surtype     ==  LFT_SUR   )            
     {
     u_min       =           1.0;
     u_max       = (DBfloat)nu+1.0;
     v_min       =           1.0;
     v_max       = (DBfloat)nv+1.0;
     }


   else if ( surtype     == NURB_SUR   )
     {
     u_min       =  500000.0;
     u_max       = -500000.0;
     v_min       =  500000.0;
     v_max       = -500000.0;
     for  ( i_pat = 0; i_pat < nu*nv; ++i_pat )
       {
       if ((p_pat+i_pat)->us_pat < u_min )u_min= (p_pat+i_pat)->us_pat;
       if ((p_pat+i_pat)->us_pat > u_max )u_max= (p_pat+i_pat)->us_pat;
       if ((p_pat+i_pat)->ue_pat < u_min )u_min= (p_pat+i_pat)->ue_pat;
       if ((p_pat+i_pat)->ue_pat > u_max )u_max= (p_pat+i_pat)->ue_pat;

       if ((p_pat+i_pat)->vs_pat < v_min )v_min= (p_pat+i_pat)->vs_pat;
       if ((p_pat+i_pat)->vs_pat > v_max )v_max= (p_pat+i_pat)->vs_pat;
       if ((p_pat+i_pat)->ve_pat < v_min )v_min= (p_pat+i_pat)->ve_pat;
       if ((p_pat+i_pat)->ve_pat > v_max )v_max= (p_pat+i_pat)->ve_pat;
       }
     } /* Surface is of type NURB_SUR */

   else
     {
#ifdef DEBUG 
     if ( dbglev(SURPAC) == 1 )
       {
       fprintf(dbgfil(SURPAC),
       "sur367 Error surface type %d\n", (short)surtype );
       }
#endif
      sprintf(errbuf, "(surface type)%%(sur367");
      return(varkon_erpush("SU2993",errbuf));
     } /* End surface type is not OK */



/*!                                                                 */
/* Check that the curve is inside the surface definition area.      */
/* Tolerance is idknot                                              */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur367 u_min %7.4f u_max %7.4f v_min %7.4f v_max %7.4f\n", 
        u_min, u_max, v_min, v_max );
fflush(dbgfil(SURPAC)); 
}
#endif

    if ( cur_box.xmin < u_min - idknot      ||  
         cur_box.xmax > u_max + idknot      ||  
         cur_box.ymin < v_min - idknot      ||  
         cur_box.ymax > v_max + idknot        ) 
      {
      sprintf(errbuf,"sur367%% ");
      varkon_erinit();
      return(varkon_erpush("SU6813",errbuf));
      }


/*!                                                                 */
/*  4. Curve segment start and or end point on patch boundaries     */
/*  ___________________________________________________________     */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/*  Initialize flag to OK                                           */
/*  Start loop all topology patches in the surface                  */
/*                                                                 !*/

    bound_flag = 0;

    for  ( i_pat = 0; i_pat < nu*nv; ++i_pat )
      {
/*!                                                                 */
/*    Retrieve patch boundaries for the current patch               */
/*                                                                 !*/

      us_pat = (p_pat+i_pat)->us_pat;
      ue_pat = (p_pat+i_pat)->ue_pat;
      vs_pat = (p_pat+i_pat)->vs_pat;
      ve_pat = (p_pat+i_pat)->ve_pat;

/*!                                                                 */
/*    Create start U line, intersect and check                      */
/*                                                                 !*/

/*    Line start and end point                                      */
      p1.x_gm  = us_pat;
      p1.y_gm  = vs_pat;
      p1.z_gm  =    0.0;
      p2.x_gm  = us_pat;
      p2.y_gm  = ve_pat;
      p2.z_gm  =    0.0;

/*    Create line                                                   */

      status = GE201 (&p1, &p2, NULL, &lin);
      if( status < 0 )
      {
      sprintf(errbuf,"sur201 (ustart)%%sur367");
      return(varkon_erpush("SU2943",errbuf));
      }


/*    Calculate the intersect                                       */


      status = GE710
      ((DBAny *)&lin, NULL, (DBAny *)p_cur, p_seg, NULL ,
         &noint,uout1,uout2);
      if( status < 0 )
      {
      sprintf(errbuf,"sur710 (ustart)%%sur367");
      return(varkon_erpush("SU2943",errbuf));
      }

/*    Check intersect points                                        */

      for (i_sol=0;i_sol< noint; ++i_sol)
        {
/*      Parameter values for intersect points                       */
        u_lin  = uout1[i_sol];
        u_cur  = uout2[i_sol];
/*      Check only if within line limits                            */
        if  ( u_lin > 1.0 - idknot       && 
              u_lin < 2.0 + idknot         )
          {
/*        The parameter value for the curve must be 1.0, 2.0, ....  */
          int_part = (DBint)floor(u_cur+idknot        );
          dec_part = u_cur - (DBfloat)int_part;
          if  ( fabs(dec_part) > idknot ) bound_flag = bound_flag+1;
          } /* Within line limits */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur367 U start i_sol %d u_lin %f u_cur %f bound_flag %d\n", 
           (int)i_sol+1, u_lin,u_cur, (int)bound_flag);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur367 U start  int_part %d dec_part %f\n", (int)int_part, dec_part);
fflush(dbgfil(SURPAC)); 
}
#endif
        } /* End loop intersect points */



/*!                                                                 */
/*    Create end   U line, intersect and check                      */
/*                                                                 !*/

/*    Line start and end point                                      */
      p1.x_gm  = ue_pat;
      p1.y_gm  = vs_pat;
      p1.z_gm  =    0.0;
      p2.x_gm  = ue_pat;
      p2.y_gm  = ve_pat;
      p2.z_gm  =    0.0;

/*    Create line                                                   */

      status = GE201 (&p1, &p2, NULL, &lin);
      if( status < 0 )
      {
      sprintf(errbuf,"sur201 (uend)%%sur367");
      return(varkon_erpush("SU2943",errbuf));
      }


/*    Calculate the intersect                                       */


      status = GE710
      ((DBAny *)&lin, NULL, (DBAny *)p_cur, p_seg, NULL ,
                &noint,uout1,uout2);
      if( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur367 ERROR Intersect: U end  ue_pat  %f vs_pat %f ve_pat %f\n", 
         ue_pat, vs_pat, ve_pat );
fflush(dbgfil(SURPAC)); 
}
#endif
      sprintf(errbuf,"sur710 (uend)%%sur367");
      return(varkon_erpush("SU2943",errbuf));
      }

/*    Check intersect points                                        */

      for (i_sol=0;i_sol< noint; ++i_sol)
        {
/*      Parameter values for intersect points                       */
        u_lin  = uout1[i_sol];
        u_cur  = uout2[i_sol];
/*      Check only if within line limits                            */
        if  ( u_lin > 1.0 - idknot       && 
              u_lin < 2.0 + idknot         )
          {
/*        The parameter value for the curve must be 1.0, 2.0, ....  */
          int_part = (DBint)floor(u_cur+idknot        );
          dec_part = u_cur - (DBfloat)int_part;
          if  ( fabs(dec_part) > idknot ) bound_flag = bound_flag+1;
          } /* Within line limits */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur367 U end   i_sol %d u_lin %f u_cur %f bound_flag %d\n", 
           (int)i_sol+1, u_lin,u_cur, (int)bound_flag);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur367 U end    int_part %d dec_part %f\n", (int)int_part, dec_part);
fflush(dbgfil(SURPAC)); 
}
#endif
        } /* End loop intersect points */


/*!                                                                 */
/*    Create start V line, intersect and check                      */
/*                                                                 !*/

/*    Line start and end point                                      */
      p1.x_gm  = us_pat;
      p1.y_gm  = vs_pat;
      p1.z_gm  =    0.0;
      p2.x_gm  = ue_pat;
      p2.y_gm  = vs_pat;
      p2.z_gm  =    0.0;

/*    Create line                                                   */

      status = GE201 (&p1, &p2, NULL, &lin);
      if( status < 0 )
      {
      sprintf(errbuf,"sur201 (vstart)%%sur367");
      return(varkon_erpush("SU2943",errbuf));
      }


/*    Calculate the intersect                                       */


      status = GE710
      ((DBAny *)&lin, NULL, (DBAny *)p_cur, p_seg, NULL ,
         &noint,uout1,uout2);
      if( status < 0 )
      {
      sprintf(errbuf,"sur710 (vstart)%%sur367");
      return(varkon_erpush("SU2943",errbuf));
      }

/*    Check intersect points                                        */

      for (i_sol=0;i_sol< noint; ++i_sol)
        {
/*      Parameter values for intersect points                       */
        u_lin  = uout1[i_sol];
        u_cur  = uout2[i_sol];
/*      Check only if within line limits                            */
        if  ( u_lin > 1.0 - idknot       && 
              u_lin < 2.0 + idknot         )
          {
/*        The parameter value for the curve must be 1.0, 2.0, ....  */
          int_part = (DBint)floor(u_cur+idknot        );
          dec_part = u_cur - (DBfloat)int_part;
          if  ( fabs(dec_part) > idknot ) bound_flag = bound_flag+1;
          } /* Within line limits */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur367 V start i_sol %d u_lin %f u_cur %f bound_flag %d\n", 
           (int)i_sol+1, u_lin,u_cur, (int)bound_flag);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur367 V start  int_part %d dec_part %f\n", (int)int_part, dec_part);
fflush(dbgfil(SURPAC)); 
}
#endif
        } /* End loop intersect points */



/*!                                                                 */
/*    Create end   V line, intersect and check                      */
/*                                                                 !*/

/*    Line start and end point                                      */
      p1.x_gm  = us_pat;
      p1.y_gm  = ve_pat;
      p1.z_gm  =    0.0;
      p2.x_gm  = ue_pat;
      p2.y_gm  = ve_pat;
      p2.z_gm  =    0.0;


/*    Create line                                                   */

      status = GE201 (&p1, &p2, NULL, &lin);
      if( status < 0 )
      {
      sprintf(errbuf,"sur201 (vend)%%sur367");
      return(varkon_erpush("SU2943",errbuf));
      }


/*    Calculate the intersect                                       */


      status = GE710
      ((DBAny *)&lin, NULL, (DBAny *)p_cur, p_seg, NULL ,
         &noint,uout1,uout2);
      if( status < 0 )
      {
      sprintf(errbuf,"sur710 (vend)%%sur367");
      return(varkon_erpush("SU2943",errbuf));
      }

/*    Check intersect points                                        */

      for (i_sol=0;i_sol< noint; ++i_sol)
        {
/*      Parameter values for intersect points                       */
        u_lin  = uout1[i_sol];
        u_cur  = uout2[i_sol];
/*      Check only if within line limits                            */
        if  ( u_lin > 1.0 - idknot       && 
              u_lin < 2.0 + idknot         )
          {
/*        The parameter value for the curve must be 1.0, 2.0, ....  */
          int_part = (DBint)floor(u_cur+idknot        );
          dec_part = u_cur - (DBfloat)int_part;
          if  ( fabs(dec_part) > idknot ) bound_flag = bound_flag+1;
          } /* Within line limits */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur367 V end   i_sol %d u_lin %f u_cur %f bound_flag %d\n", 
           (int)i_sol+1, u_lin,u_cur, (int)bound_flag);
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur367 V end    int_part %d dec_part %f\n", (int)int_part, dec_part);
fflush(dbgfil(SURPAC)); 
}
#endif

        } /* End loop intersect points */




/*!                                                                 */
/*  End   loop all topology patches in the surface                  */
/*                                                                 !*/

   } /* End loop patches            */

#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 && bound_flag > 0 )
{
fprintf(dbgfil(SURPAC),
"sur367 Curve did not pass the patch boundary test bound_flag %d\n"
           ,(int)bound_flag);
fflush(dbgfil(SURPAC)); 
}
#endif


/*!                                                                 */
/* 5. Check continuity in position                                  */
/* ________________________________                                 */
/*                                                                  */
/* Analyse the curve with respect to steps in position.             */
/* Call of varkon_cur_analysis (sur792).                            */
/*                                                                 !*/

/*!                                                                 */
/* Change back to a surface curve: CUB_SEG --> UV_CUB_SEG               */
/*                                                                 !*/

   for ( i_seg= 0; i_seg < p_cur->ns_cu; ++i_seg )
       (p_seg+i_seg)->typ = UV_CUB_SEG;

   c_case = 1;
   pcrit  = -1.0;
   dcrit  = -1.0;
   ccrit  = -1.0;


    status= varkon_cur_analysis
    (p_cur,p_seg, 1 , p_cur->ns_cu ,c_case, 
      pcrit, &n_p,piseg,dcrit, &n_d, diseg,ccrit, &n_c,ciseg,
             &n_s,siseg);
    if(status<0)
    {
    sprintf(errbuf,"varkon_cur_analysis%%sur150");
    return(varkon_erpush("SU2943",errbuf));
    }

  if ( n_s > 0 )
    {
    sprintf(errbuf,"%d%%sur367",(short)n_s+1);
    varkon_erinit();
    return(varkon_erpush("SU2163",errbuf));
    }

  if ( n_p > 0 )
    {
    sprintf(errbuf,"%d%%sur367",(short)n_p+1);
    varkon_erinit();
    return(varkon_erpush("SU2563",errbuf));
    }


/*                                                                  */
/* 6. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/*                                                                  */

#ifdef DEBUG 
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur367 Exit The UV curve has been checked bound_flag %d\n"
           ,(int)bound_flag);
fflush(dbgfil(SURPAC)); 
}
#endif

    return(SUCCED);

} /* End of function */


/********************************************************************/
