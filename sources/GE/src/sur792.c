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
/*  Function: varkon_cur_analysis                  File: sur792.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function analysis a curve with respect to steps in          */
/*  in position, tangent and curvature.                             */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-10-29   Originally written                                 */
/*  1996-05-28   Elimination of compilation warnings                */
/*  1997-02-09   Elimination of compilation warnings                */
/*  1997-03-12   Check if iso-parameter is a point, fflush          */
/*  1999-12-18   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_analysis   Position, tangent, curvat. steps */
/*                                                              */
/*------------------------------------------------------------- */

/* --------------------- Theory ------------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol         * Retrieve computer tolerance             */
/* varkon_idpoint         * Identical points criterion              */
/* varkon_idangle         * Identical angles criterion              */
/* varkon_idradius        * Identical radii  criterion              */
/* GE109                  * Curve   evaluation routine              */
/* varkon_angd            * Angle between vectors (degr.)           */
/* varkon_erpush          * Error message to terminal               */
/* varkon_ini_evalc       * Initialize EVALS for Debug On           */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_cur_analysis         */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus           varkon_cur_analysis (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur,        /* Current curve                     (ptr) */
  DBSeg   *p_seg,        /* Coefficients for curve segments   (ptr) */
  DBint    istart,       /* Start segment ( < 0 == whole curve )    */
  DBint    iend,         /* End   segment                           */
  DBint    c_case,       /* Computation case:                       */
                         /* Eq. 1: Position   only                  */
                         /* Eq. 2: Derivative added                 */
                         /* Eq. 3: Curvature  added                 */
  DBfloat  pcrit,        /* Position criterion < 0 ==> system value */
  DBint   *p_np,         /* Number of steps in position             */
  DBint    piseg[SUANA], /* Segments (end pt) with position steps   */
  DBfloat  dcrit,        /* Derivat. criterion < 0 ==> system value */
  DBint   *p_nd,         /* Number of steps in derivative           */
  DBint    diseg[SUANA], /* Segments with derivative steps          */
  DBfloat  ccrit,        /* Radius   criterion < 0 ==> system value */
  DBint   *p_nc,         /* Number of steps in curvature            */
  DBint    ciseg[SUANA], /* Segments with radius of curvature steps */
  DBint   *p_ns,         /* Number of too short segments            */
  DBint    siseg[SUANA] )/* Segments with too short chord lengths   */

/* Out:                                                             */
/*        Values to p_np, piseg, p_nd, diseg, p_nc and ciseg        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/
  DBfloat  comptol;      /* Computer tolerance (accuracy)           */
  DBint    i_seg;        /* Loop index segment in curve             */
  DBVector s_pt;         /* Start point   curve (next) segment      */
  DBVector e_pt;         /* End   point   curve segment             */
  DBfloat  s_tan[3];     /* Start tangent curve (next) segment      */
  DBfloat  e_tan[3];     /* End   tangent curve segment             */
  DBVector s_pt_pre;     /* Start point   curve (prev) segment      */
  DBfloat  dist;         /* Distance                                */
  DBfloat  chord;        /* Chord distance of segment               */
  DBfloat  angdiff;      /* Angle difference                        */
  EVALC    xyz;          /* Coordinates and derivatives for crv pt  */
  DBfloat  idpoint;      /* Identical points criterion              */
  DBfloat  idangle;      /* Identical angles criterion              */
  DBfloat  idradius;     /* Identical radii  criterion              */
  DBint    status;       /* Error code from a called function       */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*------------end-of-declarations-----------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur792 Enter varkon_cur_analysis Position, derivative,.. steps  \n");
}

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur792 istart %d iend %d p_cur->ns_cu %d\n", 
        (int)istart, (int)iend, (int)p_cur->ns_cu );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur792 pcrit %12.8f dcrit %12.8f ccrit %12.8f \n",pcrit,dcrit,ccrit);
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/*  1. Check of input data and initiations                          */
/*  ______________________________________                          */
/*                                                                  */
/* Initiate output variables p_np = p_nd = p_nc = p_ns = 0          */
/*                                                                 !*/
      *p_np = 0;
      *p_nd = 0;
      *p_nc = 0;
      *p_ns = 0;

/*!                                                                 */
/* Retrieve identical points criterion if input pcrit < 0.          */
/* Call of varkon_idpoint (sur741).                                 */
/*                                                                 !*/

if ( pcrit < 0.0 )
   {
   idpoint   =varkon_idpoint();
   }
else
   {
   idpoint = pcrit;
   }

/*!                                                                 */
/* Retrieve identical angles criterion if input dcrit < 0.          */
/* Call of varkon_idangle (sur742).                                 */
/*                                                                 !*/

if ( dcrit < 0.0 && c_case >= 2 )
   {
   idangle   =varkon_idangle();
   }
else
   {
   idangle = dcrit;
   }

/*!                                                                 */
/* Retrieve identical radius criterion if input ccrit < 0.          */
/* Call of varkon_idradius (sur743).                                */
/*                                                                 !*/

if ( ccrit < 0.0 && c_case >= 3 )
   {
   idradius  =varkon_idradius();
   }
else
   {
   idradius = ccrit;
   }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur792 idpoint %12.8f idangle %12.8f idradius %12.8f\n", 
        idpoint  ,     idangle   ,    idradius     )  ;
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Retrieve computer tolerance criterion.                           */
/* Call of varkon_comptol (sur753).                                 */
/*                                                                 !*/

   comptol=varkon_comptol();

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur792 comptol %25.15f\n", comptol );
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* Initiate EVALC for DEBUG On                                      */
/* Call of varkon_ini_evalc (sur776).                               */
/*                                                                 !*/

#ifdef  DEBUG
      status=varkon_ini_evalc (&xyz);
#endif

/*!                                                                 */
/* Check of c_case                                                  */
/*                                                                 !*/

if      ( c_case == 1 )
   {
   xyz.evltyp   = EVC_R;
   }
else if ( c_case == 2 )
   {
   xyz.evltyp   = EVC_DR;
   }
else if ( c_case == 3 )
   {
   xyz.evltyp   = EVC_KAP;
/* Not yet implemented */
   sprintf(errbuf,"c_case %d%%varkon_cur_analysis(sur792)",
                     (int)c_case  );
        return(varkon_erpush("SU2993",errbuf));
   }
else
   {
   sprintf(errbuf,"(c_case= %d)%%varkon_cur_analysis(sur792)",
                     (int)c_case);
      return(varkon_erpush("SU2993",errbuf));
   }

  if ( istart < 0 )
     {
     istart = 1;
     iend   = p_cur->ns_cu;
     }

  if ( iend  == istart )
     {
     goto oneseg;
     }

  if ( istart > 0  &&  istart <= p_cur->ns_cu - 1 )
     {
     /*  OK  */
     }
     else
        {
        sprintf(errbuf,"(istart= %d)%%varkon_cur_analysis(sur792)",
                     (int)istart);
        return(varkon_erpush("SU2993",errbuf));
        }

  if ( iend   > 1  &&  iend <= p_cur->ns_cu )
     {
     /*  OK  */
     }
     else
        {
        sprintf(errbuf,"(iend= %d)%%varkon_cur_analysis(sur792)",
                     (int)iend  );
        return(varkon_erpush("SU2993",errbuf));
        }
  if ( iend   > istart )
     {
     /*  OK  */
     }
     else
        {
        sprintf(errbuf,"(istart= %d iend %d)%%sur792",
                     (int)istart, (int)iend);
        return(varkon_erpush("SU2993",errbuf));
        }

/*!                                                                 */
/* 2. Analyse the curve                                             */
/* ____________________                                             */
/*                                                                  */
/*  Start loop curve segments i_seg= istart, .... ,iend - 1         */
/*                                                                 !*/

    for ( i_seg = istart ; i_seg <= iend - 1; i_seg = i_seg + 1 )
      {
/*!                                                                 */
/*    Curve evaluation at end of segment i_seg and at start         */
/*    of segment i_seg+1.                                           */
/*    Calls of GE109.                                               */
/*                                                                 !*/

/*    End of current segment i_seg                                  */

      xyz.t_global = (DBfloat)i_seg + 1.0 - comptol;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur792 i_seg %d xyz.t_global %25.15f for point e_pt\n",
  (int)i_seg,xyz.t_global );
fflush(dbgfil(SURPAC)); 
}
#endif

      status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
      if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_cur_analysis");
        return(varkon_erpush("SU2943",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur792 After GE109 evltyp %d  point e_pt\n", (int)xyz.evltyp );
fflush(dbgfil(SURPAC)); 
}
#endif

      e_pt = xyz.r;
      if ( c_case >= 2 )
        {
        e_tan[0]= xyz.drdt.x_gm;
        e_tan[1]= xyz.drdt.y_gm;
        e_tan[2]= xyz.drdt.z_gm;
        }


      if  (  i_seg == istart )
        {

/*      Start of current segment i_seg                             */

        xyz.t_global = (DBfloat)i_seg + comptol;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur792 i_seg %d xyz.t_global %25.15f for point s_pt_pre\n",
  (int)i_seg,xyz.t_global );
fflush(dbgfil(SURPAC)); 
}
#endif


        status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
        if (status<0) 
          {
          sprintf(errbuf,"GE109 p_sp_pre%%varkon_cur_analysis");
          return(varkon_erpush("SU2943",errbuf));
          }
        s_pt_pre = xyz.r;

        }  /* Start point of current segment i_seg */
      else  s_pt_pre = s_pt; 

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur792 i_seg %d e_pt %10.2f %10.2f %10.2f\n",
  (int)i_seg,e_pt.x_gm,e_pt.y_gm,e_pt.z_gm );
fflush(dbgfil(SURPAC)); 
}
#endif

/*    Start of next segment i_seg + 1                               */

      xyz.t_global = (DBfloat)i_seg + 1.0 + comptol;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur792 i_seg %d xyz.t_global %25.15f for point s_pt\n",
  (int)i_seg,xyz.t_global );
fflush(dbgfil(SURPAC)); 
}
#endif


      status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
      if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_cur_analysis");
        return(varkon_erpush("SU2943",errbuf));
        }

      s_pt = xyz.r;
      if ( c_case >= 2 ) 
        {
        s_tan[0]= xyz.drdt.x_gm;
        s_tan[1]= xyz.drdt.y_gm;
        s_tan[2]= xyz.drdt.z_gm;
        }

     dist = SQRT( (s_pt.x_gm-e_pt.x_gm)*(s_pt.x_gm-e_pt.x_gm) +
                  (s_pt.y_gm-e_pt.y_gm)*(s_pt.y_gm-e_pt.y_gm) +
                  (s_pt.z_gm-e_pt.z_gm)*(s_pt.z_gm-e_pt.z_gm) );


     chord = SQRT( (s_pt_pre.x_gm-e_pt.x_gm)*(s_pt_pre.x_gm-e_pt.x_gm) +
                   (s_pt_pre.y_gm-e_pt.y_gm)*(s_pt_pre.y_gm-e_pt.y_gm) +
                   (s_pt_pre.z_gm-e_pt.z_gm)*(s_pt_pre.z_gm-e_pt.z_gm) );

#ifdef DEBUG
     if ( dist > idpoint || chord <= idpoint )
      {
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur792 i_seg %d dist %25.10f chord %25.10f idpoint %12.8f\n",
  (int)i_seg, dist, chord, idpoint);
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur792 i_seg %d s_pt %10.2f %10.2f %10.2f\n",
  (int)i_seg,s_pt.x_gm,s_pt.y_gm,s_pt.z_gm );
fprintf(dbgfil(SURPAC),
"sur792 i_seg %d e_pt %10.2f %10.2f %10.2f\n",
  (int)i_seg,e_pt.x_gm,e_pt.y_gm,e_pt.z_gm );
}
}
fflush(dbgfil(SURPAC)); 
      }
#endif

     if ( dist > idpoint )
      {
      if ( *p_np  <= SUANA )
      piseg[*p_np] = i_seg;
      *p_np = *p_np + 1;
      }

     if ( chord <= idpoint )
      {
      if ( *p_ns  <= SUANA )
      piseg[*p_ns] = i_seg;
      *p_ns = *p_ns + 1;
      }

      if ( c_case >= 2 )
         {
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur792 e_tan %f %f %f \n",
  e_tan[0],e_tan[1],e_tan[2]   );
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur792 e_tan %f %f %f \n",
  s_tan[0],s_tan[1],s_tan[2]   );
fflush(dbgfil(SURPAC)); 
}
#endif
         status= varkon_angd (e_tan,s_tan,&angdiff);
         if (status<0) 
           {
           sprintf(errbuf,"varkon_angd%%varkon_cur_analysis");
           return(varkon_erpush("SU2943",errbuf));
           }
         if ( angdiff > idangle )
          {
          if ( *p_np  <= SUANA )
          diseg[*p_nd] = i_seg;
          *p_nd = *p_nd + 1;
          }
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur792 i_seg %d angdiff %25.10f idangle %12.8f\n",
  (int)i_seg, angdiff, idangle);
fflush(dbgfil(SURPAC)); 
}
#endif
         }

      }  /* End loop curve segments i_seg= 1,2,3                    */


oneseg:; /*! Label oneseg: Only one segment in the curve           !*/

/*                                                                  */
/* 4. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur792 Exit _cur_analys Steps position %d angle %d\n", 
             (int)*p_np , (int)*p_nd       );
fflush(dbgfil(SURPAC)); 
}
#endif

    return(SUCCED);

} /* End of function */


/********************************************************************/
