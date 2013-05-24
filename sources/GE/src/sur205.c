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
/*  (C)Microform AB 2000-11-01, Gunnar Liden, gunnar@microform.se   */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"
#include "../../IG/include/debug.h"

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_sur_curves                    File: sur205.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Create a bicubic surface from a set of curves (SUR_CURVES)      */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  2000-11-05   Originally written                                 */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_curves      Create surface SUR_CURVES       */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_idpoint          * Identical points criterion             */
/* varkon_idangle          * Identical angles criterion             */
/* varkon_comptol          * Retrieve computer tolerance            */
/* GE717                   * Global U for relative arclength        */
/* GE109                   * Curve   evaluation routine             */
/* GEnormalise_vector3D    * Normalize 3D vector                    */
/* varkon_erinit           * Initial. error message stack           */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short initial();       /* Check input data and initialize    */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat  idpoint;     /* Identical point criterion           */
static DBfloat  idangle;     /* Identical angle criterion           */
static DBfloat  comptol;     /* Computer tolerance                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_curves          */
/* SU2973 = Internal function () failed in varkon_sur_curves        */
/* SU2993 = Severe program error (  ) in varkon_sur_curves          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/

       DBstatus       varkon_sur_curves (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint    nu_cur,      /* Number of U lines                       */
   DBint    nv_cur,      /* Number of V lines, i.e. input curves    */
   DBCurve  vcur[STRIP], /* V lines                                 */
   DBSeg   *p_seg[STRIP],/* V Lines segments                  (ptr) */
   char    *metod,       /* Method:                                 */
                         /* ZEROTWIST_1: Zero twist                 */
                         /* ADINI_1:     Adini twist calculation    */
   DBSurf  *p_surout,    /* Output surface                    (ptr) */
   DBPatch **pp_patout ) /* Alloc. area for topol. patch data (ptr) */
                         /* (pp_patout= NULL <==> not allocated)    */


/* Out:                                                             */
/*                                                                  */
/*      Data to p_surout and pp_patout in allocated memory area     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint     np_u;              /* Number of points in U direction   */
  DBint     np_v;              /* Number of points in V direction   */
  DBVector  poi[SPINE*STRIP];  /* Points                            */
  DBVector  vtan[SPINE*STRIP]; /* Tangents in V direction           */
  DBVector *p_poi;             /* Points                      (ptr) */
  DBVector *p_utan;            /* Tangents in U direction     (ptr) */
  DBVector *p_vtan;            /* Tangents in V direction     (ptr) */
  DBVector *p_twist;           /* Twist vectors               (ptr) */
  DBCurve  *p_cur;             /* Current curve               (ptr) */
  DBSeg    *p_startseg;        /* Current start segment       (ptr) */
  DBfloat   rel_leng;          /* Relative arclength                */
  DBfloat   u_global;          /* Global U parameter                */
  EVALC     xyz;               /* Coordinates/derivatives for crv pt*/
  DBVector  poi_cur;           /* Current curve point               */
  DBVector  tan_cur;           /* Current curve tangent             */
  DBVector  tan_norm_cur;      /* Current curve tangent, normalized */
  DBint     i_array;           /* Current array record number       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint     ip_u;        /* Loop index point point in U direction   */
  DBint     ip_v;        /* Loop index point point in V direction   */
  DBint     status;      /* Error code from called function         */
  char      errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur205 Enter***varkon_sur_curves n_cur %d nv_cur %d\n", 
          (int)nu_cur, (int)nv_cur );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
 
/*!                                                                 */
/* Let pp_patout= NULL                                              */
/*                                                                 !*/
   *pp_patout = NULL;

    status= initial(nu_cur, nv_cur, vcur, p_seg, metod );
    if (status<0) 
      {
      sprintf(errbuf,"initial%%varkon_sur_curves (sur205)");
      return(varkon_erpush("SU2973",errbuf));
      }
/* Initialization of local variables for DEBUG On                   */
#ifdef DEBUG
   np_u              =  I_UNDEF;
   np_v              =  I_UNDEF;
   rel_leng          =  F_UNDEF;
   u_global          =  F_UNDEF;
   poi_cur.x_gm      = F_UNDEF;
   poi_cur.y_gm      = F_UNDEF;
   poi_cur.z_gm      = F_UNDEF;
   tan_cur.x_gm      = F_UNDEF;
   tan_cur.y_gm      = F_UNDEF;
   tan_cur.z_gm      = F_UNDEF;
   tan_norm_cur.x_gm = F_UNDEF;
   tan_norm_cur.y_gm = F_UNDEF;
   tan_norm_cur.z_gm = F_UNDEF;
   i_array           = 0;
   for ( ip_u=0; ip_u<SPINE; ++ip_u )
   {
      for ( ip_v=0; ip_v<STRIP; ++ip_v )
	  {
      poi[i_array].x_gm  =  F_UNDEF;
      poi[i_array].y_gm  =  F_UNDEF;
      poi[i_array].z_gm  =  F_UNDEF;
      vtan[i_array].x_gm =  F_UNDEF;
      vtan[i_array].y_gm =  F_UNDEF;
      vtan[i_array].z_gm =  F_UNDEF;
      i_array = i_array + 1;
	  }
   }
   p_poi          =  (DBVector*)NULL;
   p_utan         =  (DBVector*)NULL;
   p_vtan         =  (DBVector*)NULL;
   p_twist        =  (DBVector*)NULL;
   p_cur          =  (DBCurve*)NULL;
   p_startseg     =  (DBSeg*)NULL;
   i_array        =  I_UNDEF;
#endif

/*!                                                                 */
/* 2. Points and tangents at equal relative arclengths to arrays    */
/*                                                                 !*/


/* Start loop all input curves                                      */
   for ( ip_v=1; ip_v<=nv_cur; ++ip_v )
   {
/*     Current curve                                                */
       p_cur          = &vcur[ip_v-1];
       p_startseg     = p_seg[ip_v-1];
/*     Start loop all points                                        */
       for ( ip_u=1; ip_u<=nu_cur; ++ip_u )
	   {
/*          Current relative arclength                              */
            rel_leng = (DBfloat)(ip_u-1)/(DBfloat)(nu_cur-1);
/*          Calculate global U value for the relative arclength     */
            status=GE717
              ((DBAny *)p_cur,p_startseg, NULL ,rel_leng,&u_global);
            if (status<0) 
			{
              sprintf(errbuf,"GE717%%varkon_sur_curves (sur205)");
              return(varkon_erpush("SU2943",errbuf));
			}
/*          Calculate point and tangent for the current U value     */
            xyz.evltyp   = EVC_DR;
            xyz.t_global = u_global;
            status=GE109 ((DBAny *)p_cur,p_startseg,&xyz);
            if (status<0) 
			{
               sprintf(errbuf,"GE109%%varkon_sur_curves (sur205)");
               return(varkon_erpush("SU2943",errbuf));
			}
            poi_cur = xyz.r;
            tan_cur = xyz.drdt;
            if ( GEnormalise_vector3D(&tan_cur,&tan_norm_cur) < 0 )
			{
               sprintf(errbuf,"Zero tangent%%varkon_sur_curves");
               return(varkon_erpush("SU2993",errbuf));
			}
/*          To arrays for the surface creation function             */
            i_array= nv_cur*(ip_u-1)+ip_v-1;/* Current array record */
            poi [i_array]  =  poi_cur; 
            vtan[i_array]  =  tan_norm_cur; 

	   }
   }

/*!                                                                 */
/* 3. Create the surface                                            */
/*                                                                 !*/
   np_u     =  nu_cur;
   np_v     =  nv_cur;
   p_poi    =  &poi[0];
   p_utan   =  (DBVector*)NULL;
   p_vtan   =  &vtan[0];
   p_twist  =  (DBVector*)NULL;

   status = varkon_sur_splarr (metod, np_u,np_v, p_poi, p_utan, 
                               p_vtan, p_twist, 
                               p_surout, pp_patout);
   if (status<0) 
   {
       sprintf(errbuf,"varkon_sur_splarr%%varkon_sur_curves");
       return(varkon_erpush("SU2943",errbuf));
   }


/*!                                                                 */
/* 4. Exit                                                          */
/*                                                                 !*/



#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur205 *pp_patout %d\n", (int)*pp_patout );
fflush(dbgfil(SURPAC)); 
}
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/


/*!********* Internal ** function ** initial ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function checks and initializes data                         */

   static short initial( nu_cur, nv_cur, vcur, p_seg, metod )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBint    nu_cur;      /* Number of U lines                       */
   DBint    nv_cur;      /* Number of V lines, i.e. input curves    */
   DBCurve  vcur[STRIP]; /* V lines                                 */
   DBSeg   *p_seg[STRIP];/* V Lines segments                  (ptr) */
   char    *metod;       /* Method:                                 */
                         /* ZEROTWIST_1: Zero twist                 */
                         /* ADINI_1:     Adini twist calculation    */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  char   errbuf[80];     /* String for error message fctn erpush    */
/*----------------------------------------------------------------- */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur205*initial* Enter ** \n");
  }
#endif

  if (nu_cur < 2 ) 
        {
        sprintf(errbuf,"nu_cur < 2 %%varkon_sur_curves (sur205)");
        return(varkon_erpush("SU2993",errbuf));
        }
  if (nu_cur > SPINE ) 
        {
        sprintf(errbuf,"nu_cur > SPINE %%varkon_sur_curves (sur205)");
        return(varkon_erpush("SU2993",errbuf));
        }

  if (nv_cur < 2 ) 
        {
        sprintf(errbuf,"nv_cur < 2 %%varkon_sur_curves (sur205)");
        return(varkon_erpush("SU2993",errbuf));
        }

  if (nv_cur > STRIP ) 
        {
        sprintf(errbuf,"nv_cur > STRIP %%varkon_sur_curves (sur205)");
        return(varkon_erpush("SU2993",errbuf));
        }

   idpoint   = varkon_idpoint();
   idangle   = varkon_idangle();
   comptol   = varkon_comptol();


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur205*initial idpoint %10.6f idangle %6.4f comptol %12.10f\n", 
idpoint, idangle, comptol  );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/


