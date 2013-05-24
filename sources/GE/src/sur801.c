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
/*  Function: varkon_sur_sele01                    File: sur801.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates a surface element of type SELE01           */
/*  (for the moment a surface with patches of type LFT_PAT).        */
/*                                                                  */
/*  TODO Much more to program                                       */
/*                                                                  */
/*  The function allocates memory area for the patch data. This     */
/*  area must be deallocated by the calling function.               */
/*  Deallocation must not be made for pointer pp_pat= NULL          */
/*  but in all other cases (also if there is an error).             */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-12-12   Originally written                                 */
/*  1999-12-18   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_sele01     Create surface element SELE01    */
/*                                                              */
/*--------------------------------------------------------------*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
#ifdef  DEBUG
static short initial();       /* Check and initiations For Debug On */
#endif
static short clopoi();        /* Closest point to a curve           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBfloat ctol;              /* Coordinate tolerance           */
static DBfloat comptol;           /* Computer tolerance (accuracy)  */
/*-----------------------------------------------------------------!*/


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol        * Retrieve computer tolerance              */
/* varkon_ctol           * Retrieve coordinate tolerance            */
/* GE109                 * Curve   evaluation routine               */
/* GE703                 * Closest point on a curve                 */
/* varkon_erpush         * Error message to terminal                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2973 = Internal function xxxxxx failed in varkon_sur_sele01    */
/* SU2993 = Severe program error ( ) in varkon_sur_sele01  (sur801) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
        DBstatus       varkon_sur_sele01 (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBCurve *p_spine,      /* Spine curve                       (ptr) */
  DBSeg   *p_spineseg,   /* Spine segments                    (ptr) */

  DBCurve *p_lim_s,      /* Start limit curve                 (ptr) */
  DBSeg   *p_limseg,     /* Start limit curve segments        (ptr) */
  DBfloat  l_s,          /* Start parameter value limit curve       */
  DBfloat  l_e,          /* End   parameter value limit curve       */
  DBCurve *p_gener1,     /* Generatrix 1 curve                (ptr) */
  DBSeg   *p_gener1seg,  /* Generatrix 1 curve segments       (ptr) */
  DBCurve *p_gener2,     /* Generatrix 2 curve                (ptr) */
  DBSeg   *p_gener2seg,  /* Generatrix 2 curve segments       (ptr) */
  DBint    e_case,       /* Case for element SELE01                 */

  DBSurf  *p_sur,        /* Output surface                    (ptr) */
  DBPatch **pp_pat )     /* Allocated area for patch data     (ptr) */

/* Out:                                                             */
/*                                                                  */
/*       Data to p_sur and coefficients to the pp_pat area          */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat  s_s;          /* Start parameter value spine curve       */
  DBfloat  s_e;          /* End   parameter value spine curve       */
  DBfloat  dist;         /* Distance between curve and point        */
  DBfloat  g_1;          /* Parameter on limit crv for generatrix 1 */
  DBfloat  g_2;          /* Parameter on limit crv for generatrix 2 */
  DBfloat  g_1_s;        /* Parameter on spine crv for generatrix 1 */
  DBfloat  g_2_s;        /* Parameter on spine crv for generatrix 2 */

/*-----------------------------------------------------------------!*/

   DBint  status;        /* Error code from called function         */
   char   errbuf[80];    /* String 1 for error message fctn erpush  */

/*--------------end-of-declarations---------------------------------*/

/*!-------------- Flow diagram -------------------------------------*/
/*                ____________________                              */
/*               !                    !                             */
/*               ! varkon_sur_sele01  !                             */
/*               !     (sur801)       !                             */
/*               !____________________!                             */
/*         _______________!________________________                 */
/*   _____!_____   ____________!_____________   ___!____            */
/*  !     1   o ! !            2             ! !   3    !           */
/*  ! Initiate  ! !                          ! !  Exit  !           */
/*  !___________! !__________________________! !________!           */
/*  Debug ! On                 !                                    */
/*   _____!_____               !                                    */
/*  ! initial   !              !                                    */
/*  !___________!              !                                    */
/*------------------------------------------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur801 Enter varkon_sur_sele01 : Create surface element SELE01\n");
}
#endif
 
/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/*  Initiate output and internal variables      For Debug On.       */
/*  Call of initial, which also prints out the input data.          */
/*                                                                 !*/

#ifdef DEBUG
    status=initial
       (p_spine,p_spineseg,p_lim_s,p_limseg,l_s,l_e,
        p_gener1,p_gener1seg, p_gener2,p_gener2seg,e_case,
        p_sur,pp_pat);
    if (status<0) 
        {
        sprintf(errbuf,"initial%%varkon_sur_sele01  (sur801)");
        return(varkon_erpush("SU2973",errbuf));
        }
#endif

/*!                                                                 */
/* Computer   tolerance. Call of varkon_comptol (sur753).           */
/* Coordinate tolerance. Call of varkon_ctol    (sur751).           */
/*                                                                 !*/

   comptol=varkon_comptol();
   ctol   =varkon_ctol();

/*!                                                                 */
/* 2. Create a trimmed spine                                        */
/* __________________________                                       */
/*                                                                  */
/*  Calculate closest points from limit curve end points.           */
/*  Calls of clopoi.                                                */
/*                                                                 !*/

    status=clopoi 
       (p_spine,p_spineseg,p_lim_s,p_limseg,l_s, &s_s, &dist);
    if (status<0) 
        {
        sprintf(errbuf,"clopoi%%varkon_sur_sele01  (sur801)");
        return(varkon_erpush("SU2973",errbuf));
        }
    status=clopoi 
       (p_spine,p_spineseg,p_lim_s,p_limseg,l_e, &s_e, &dist);
    if (status<0) 
        {
        sprintf(errbuf,"clopoi%%varkon_sur_sele01  (sur801)");
        return(varkon_erpush("SU2973",errbuf));
        }

#ifdef DEBUG  
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur801 Spine trimming parameters s_s %f s_e %f \n",s_s , s_e );
}
#endif


/*!                                                                 */
/*  Create trimmed spine.                                           */
/*  Call of geo???                                                  */
/*                                                                 !*/

/*!                                                                 */
/* 3. Parameter values on the limit curve and the                   */
/*    spine gor the generatrices.                                   */
/* _______________________________________________                  */
/*                                                                  */
/*  Calculate closest points from generatrix end points.            */
/*  Calls of clopoi.                                                */
/*                                                                 !*/

/*  Start point on generatrix 1 U= 1.0                              */
/*  Limit curve ...                                                 */
    status=clopoi 
       (p_lim_s,p_limseg,p_gener1,p_gener1seg, 1.0, &g_1, &dist);
    if (status<0) 
        {
        sprintf(errbuf,"clopoi%%varkon_sur_sele01  (sur801)");
        return(varkon_erpush("SU2973",errbuf));
        }
    if ( dist <= ctol ) 
        {
/*      Limit curve ...                                             */
        status=clopoi 
          (p_spine,p_spineseg,p_gener1,p_gener1seg, 1.0, &g_1_s, &dist);
        if (status<0) 
            {
            sprintf(errbuf,"clopoi%%varkon_sur_sele01  (sur801)");
            return(varkon_erpush("SU2973",errbuf));
            }
            goto gen1;
        } /* End dist <= ctol */
/*  End   point on generatrix 1 U= 2.0 if distance > ctol           */
    status=clopoi 
       (p_lim_s,p_limseg,p_gener1,p_gener1seg, 2.0, &g_1, &dist);
    if (status<0) 
        {
        sprintf(errbuf,"clopoi%%varkon_sur_sele01  (sur801)");
        return(varkon_erpush("SU2973",errbuf));
        }

    if ( dist > ctol ) 
        {
        sprintf(errbuf,"gener1%%varkon_sur_sele01 (sur801)");
        return(varkon_erpush("SU2993",errbuf));
        }

    status=clopoi 
       (p_spine,p_spineseg,p_gener1,p_gener1seg, 2.0, &g_1_s, &dist);
    if (status<0) 
        {
        sprintf(errbuf,"clopoi%%varkon_sur_sele01  (sur801)");
        return(varkon_erpush("SU2973",errbuf));
        }


gen1:; /*! Label gen1: Start point on generatrix is on limit curve  */


 g_2_s = -0.123456789; 

/*  Start point on generatrix 2 U= 1.0                              */
    status=clopoi 
       (p_lim_s,p_limseg,p_gener2,p_gener2seg, 1.0, &g_2, &dist);
    if (status<0) 
        {
        sprintf(errbuf,"clopoi%%varkon_sur_sele01  (sur801)");
        return(varkon_erpush("SU2973",errbuf));
        }
    if ( dist <= ctol )
        {
/*      Limit curve ...                                                 */
        status=clopoi 
           (p_spine,p_spineseg,p_gener2,p_gener2seg, 1.0, &g_2_s, &dist);
        if (status<0) 
            {
            sprintf(errbuf,"clopoi%%varkon_sur_sele01  (sur801)");
            return(varkon_erpush("SU2973",errbuf));
            }
            goto gen2;
        } /* End dist <= ctol */
/*  End   point on generatrix 2 U= 2.0 if distance > ctol           */
    status=clopoi 
       (p_lim_s,p_limseg,p_gener2,p_gener2seg, 2.0, &g_2, &dist);
    if (status<0) 
        {
        sprintf(errbuf,"clopoi%%varkon_sur_sele01  (sur801)");
        return(varkon_erpush("SU2973",errbuf));
        }

    if ( dist > ctol ) 
        {
        sprintf(errbuf,"gener2%%varkon_sur_sele01 (sur801)");
        return(varkon_erpush("SU2993",errbuf));
        }


    status=clopoi 
       (p_spine,p_spineseg,p_gener2,p_gener2seg, 2.0, &g_2_s, &dist);
    if (status<0) 
        {
        sprintf(errbuf,"clopoi%%varkon_sur_sele01  (sur801)");
        return(varkon_erpush("SU2973",errbuf));
        }


gen2:; /*! Label gen2: Start point on generatrix is on limit curve  */


#ifdef DEBUG  
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur801 Limit curve parameters for generatrices g_1   %f g_2   %f\n"
             ,g_1 , g_2 );
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur801 Spine curve parameters for generatrices g_1_s %f g_2_s %f\n"
             ,g_1_s,g_2_s);
}
#endif





/*                                                                  */
/* 7. Exit                                                          */
/* _______                                                          */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur801 Exit *** varkon_sur_sele01  \n");
}
#endif


    return(SUCCED);

  } /* End of function */

/*********************************************************/


/*!New-Page--------------------------------------------------------!*/

#ifdef DEBUG
/*!********* Internal ** function ** initial ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function checks the input data and it initializes the        */
/* output variables and the static (common) variables to the        */
/* values 1.23456789 and 123456789.                                 */

   static short initial
            (p_spine,p_spineseg,p_lim_s,p_limseg,l_s,l_e,
        p_gener1,p_gener1seg, p_gener2,p_gener2seg,e_case,
                        p_sur,pp_pat)
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBCurve *p_spine;      /* Spine curve                       (ptr) */
  DBSeg   *p_spineseg;   /* Spine segments                    (ptr) */


  DBCurve *p_lim_s;      /* Start limit curve                 (ptr) */
  DBSeg   *p_limseg;     /* Start limit curve segments        (ptr) */
  DBfloat  l_s;          /* Start parameter value limit curve       */
  DBfloat  l_e;          /* End   parameter value limit curve       */
  DBCurve *p_gener1;     /* Generatrix 1 curve                (ptr) */
  DBSeg   *p_gener1seg;  /* Generatrix 1 curve segments       (ptr) */
  DBCurve *p_gener2;     /* Generatrix 2 curve                (ptr) */
  DBSeg   *p_gener2seg;  /* Generatrix 2 curve segments       (ptr) */
  DBint    e_case;       /* Case for element SELE01                 */

  DBSurf  *p_sur;        /* Output surface                    (ptr) */
  DBPatch **pp_pat;      /* Allocated area for patch data     (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Printout of input variables                                   */
/* ==============================                                   */
/*                                                                 !*/

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur801*initial p_sur %d\n", p_sur );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur801*initial p_spine %d p_spineseg %d (*p_spine).ns_cu %d\n",
                  p_spine,   p_spineseg,   (*p_spine).ns_cu );
  fflush(dbgfil(SURPAC)); 
  }

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur801*initial p_lim_s %d p_limseg %d p_lim_s->ns_cu %d\n",
                 p_lim_s,   p_limseg,   p_lim_s->ns_cu );
  }

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur801*initial l_s %f l_e %f\n" , l_s , l_e  );
}

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur801*initial p_gener1 %d p_gener1seg %d p_gener1->ns_cu %d\n",
                 p_gener1,   p_gener1seg,   p_gener1->ns_cu );
  }


if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur801*initial p_gener2 %d p_gener2seg %d p_gener2->ns_cu %d\n",
                 p_gener2,   p_gener2seg,   p_gener2->ns_cu );
  }


if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur801*initial e_case %d \n" , e_case  );
}



/*!                                                                 */
/* 3. Initiate output variables and static (internal) variables     */
/* ============================================================     */
/*                                                                 !*/

    return(SUCCED);

} /* End of function                                                */
/*!****************************************************************!*/
#endif

/*!********* Internal ** function ** clopoi  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function calculates the closest points to all start and      */
/* segment points of a directrix curve. Output is an array of       */
/* spine parameter values.                                          */

   static short clopoi 
   (p_close,p_closeseg,p_cur,p_startseg,ucur,p_uclose,p_dist )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

  DBCurve *p_close;      /* Closest curve                     (ptr) */
  DBSeg   *p_closeseg;   /* Closest curve segments            (ptr) */
  DBCurve *p_cur;        /* Current curve                     (ptr) */
  DBSeg   *p_startseg;   /* Current start segment             (ptr) */

  DBfloat  ucur;         /* Current parameter value                 */
  DBfloat *p_uclose;     /* Parameter value for closest point (ptr) */
  DBfloat *p_dist;       /* Distance between points           (ptr) */

/* Out:                                                             */
/*                                                                  */

/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

   EVALC   xyz_c;        /* Point and derivatives on curve          */
   DBVector extpt;       /* External point                          */
   short   intnr;        /* Defines the output closest point        */
   DBfloat u_clo;        /* Output parameter value                  */

   DBint   status;       /* Error code from a called function       */

   char    errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                  */
/* 1. Initiations                                                   */
/* ==============                                                   */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur801*clopoi  p_close= %d  p_closeseg %d\n",p_close , p_closeseg );
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur801*clopoi  p_cur= %d  p_startseg %d ucur %f\n"
               ,p_cur , p_startseg , ucur);
}
#endif

/*!                                                                 */
/* 2. External point from curve p_cur                               */
/* ==================================                               */
/*                                                                  */
/* Coordinates for the given parameter value ucur.                  */
/* Call of GE109.                                                   */
/*                                                                 !*/

   xyz_c.t_global = ucur;     
   xyz_c.evltyp   = EVC_R;

   status=GE109 ((DBAny *)p_cur,p_startseg ,&xyz_c);
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_sur_sele01  (clopoi )");
        return(varkon_erpush("SU2943",errbuf));
        }

   extpt      =  xyz_c.r;

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur801*clopoi External point on p_cur  extpt %f %f %f\n",
  extpt.x_gm,extpt.y_gm,extpt.z_gm);
}
#endif

/*!                                                                 */
/* 3. Closest point on curve p_close                                */
/* ==================================                               */
/*                                                                  */
/* Calculate the output parameter value p_uclose.                   */
/* Call of varkon_GE703 (GE703).                                  */
/* !!!! GE703 has MBS global parameters starting with 0 (zero) !!! */
/*                                                                 !*/

   intnr = 1;

   status=GE703 ((DBAny *)p_close,p_closeseg,&extpt,intnr,&u_clo);
   if (status<0) 
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur801*clopoi No closest point for     extpt %f %f %f\n",
  extpt.x_gm,extpt.y_gm,extpt.z_gm);
}
#endif
        sprintf(errbuf,"GE703%%varkon_sur_sele01  (clopoi )");
        return(varkon_erpush("SU2943",errbuf));
        }

  u_clo = u_clo + 1.0; /* GE703 Should be changed to surpac paramet. */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur801*clopoi Parameter value for closest pt u_clo %f\n", u_clo);
}
#endif


  *p_uclose = u_clo; 


/*!                                                                 */
/* 4. Distance between points                                       */
/* ==========================                                       */
/*                                                                  */
/* Coordinates for the calculated parameter value p_uclose.         */
/* Call of GE109.                                                   */
/* Calculate output distance p_dist.                                */
/*                                                                 !*/

   xyz_c.t_global = u_clo;     
   xyz_c.evltyp   = EVC_R;

   status=GE109 ((DBAny *)p_close,p_closeseg ,&xyz_c);
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_sur_sele01  (clopoi )");
        return(varkon_erpush("SU2943",errbuf));
        }

   *p_dist = SQRT( 
    (extpt.x_gm-xyz_c.r.x_gm)*(extpt.x_gm-xyz_c.r.x_gm) +
    (extpt.y_gm-xyz_c.r.y_gm)*(extpt.y_gm-xyz_c.r.y_gm) +
    (extpt.z_gm-xyz_c.r.z_gm)*(extpt.z_gm-xyz_c.r.z_gm)  );

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur801*clopoi Point on p_close  %f %f %f\n",
  xyz_c.r.x_gm,xyz_c.r.y_gm,xyz_c.r.z_gm);
}
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur801*clopoi Distance between points p_dist=  %f\n", *p_dist );
}
#endif

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

