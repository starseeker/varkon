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
/*  Function: varkon_cur_print                     File: sur799.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function prints out curve data to the debug file            */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-08-30   Originally written                                 */
/*  1997-02-09   Elimination of compilation warning                !*/
/*  1999-12-01   Free source code modifications                     */
/*                                                                 !*/
/********************************************************************/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_cur_print      Debug printout of curve data     */
/*                                                              */
/*------------------------------------------------------------- */

/* --------------------- Theory ------------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol         * Retrieve computer tolerance             */
/* varkon_ini_evalc       * Initialize EVALC                        */
/* GE109                  * Curve   evaluation routine              */
/* varkon_erpush          * Error message to terminal               */
/*                                                                  */
/*-----------------------------------------------------------------!*/



/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error ( ) in varkon_cur_print            */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus           varkon_cur_print (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBCurve *p_cur,        /* Current curve                     (ptr) */
  DBSeg   *p_seg,        /* Coefficients for curve segments   (ptr) */
  DBint    p_case )      /* Printout case:                          */
                         /* Eq. 1: Coefficients                     */
                         /* Eq. 2: Points+tangents as MBS           */
                         /* Eq. 3:                                  */
/* Out:                                                             */
/*        - No output -                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint    istart;       /* Start segment ( < 0 == whole curve )    */
  DBint    iend;         /* End   segment                           */
  DBint    iadd;         /* Number of points in segment             */
/*                                                                  */
/*-----------------------------------------------------------------!*/
  DBfloat  comptol;      /* Computer tolerance (accuracy)           */
  DBint    i_seg;        /* Loop index segment in curve             */
  DBVector s_pt;         /* Start point   curve segment             */
  DBVector e_pt;         /* End   point   curve segment             */
  DBfloat  s_tan[3];     /* Start tangent curve segment             */
  DBfloat  e_tan[3];     /* End   tangent curve segment             */
  EVALC    xyz;          /* Coordinates and derivatives for crv pt  */
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
"sur799 Enter varkon_cur_print Printout of curve data p_case %d\n"
         , (int)p_case );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/*  1. Check of input data and initializations                      */
/*  __________________________________________                      */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Retrieve computer tolerance criterion.                           */
/* Call of varkon_comptol (sur753).                                 */
/*                                                                 !*/

   comptol=varkon_comptol();

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 comptol %25.15f\n", comptol );
fflush(dbgfil(SURPAC));
}
#endif

  istart = I_UNDEF;
  iend   = I_UNDEF;
  iadd   = I_UNDEF;

/*!                                                                 */
/* Initialize EVALC                                                 */
/* Call of varkon_ini_evalc (sur776).                               */
/*                                                                 !*/

      status=varkon_ini_evalc (&xyz);

/*!                                                                 */
/* Calculate points and tangents. Let evltyp= EVC_DR.               */
/*                                                                 !*/

   xyz.evltyp   = EVC_DR;

/*!                                                                 */
/* Check of p_case                                                  */
/*                                                                 !*/

if      ( p_case == 1 )
   {
   ;
   }
else if ( p_case == 2 )
   {
   ;
   }
else
   {
   sprintf(errbuf,"(p_case= %d)%%varkon_cur_print (sur799)",
                     (int)p_case);
      return(varkon_erpush("SU2993",errbuf));
   }

  if ( istart < 0 )
     {
     istart = 1;
     iend   = p_cur->ns_cu;
     }

  if ( istart > 0  &&  istart <= p_cur->ns_cu )
     {
     /*  OK  */
     }
     else
        {
        sprintf(errbuf,"(istart= %d)%%varkon_cur_print   (sur799)",
                     (int)istart);
        return(varkon_erpush("SU2993",errbuf));
        }

  if ( iend   >= 1  &&  iend <= p_cur->ns_cu )
     {
     /*  OK  */
     }
     else
        {
        sprintf(errbuf,"(iend= %d)%%varkon_cur_print   (sur799)",
                     (int)iend  );
        return(varkon_erpush("SU2993",errbuf));
        }
  if ( iend   >=  istart )
     {
     /*  OK  */
     }
     else
        {
        sprintf(errbuf,"(istart= %d iend %d)%%sur799",
                     (int)istart, (int)iend);
        return(varkon_erpush("SU2993",errbuf));
        }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 istart %d iend %d p_cur->ns_cu %d iadd %d\n", 
        (int)istart, (int)iend, (int)p_cur->ns_cu,  (int)iadd );
fflush(dbgfil(SURPAC));
}
#endif

/*!                                                                 */
/* 2. Printout of curve data and segments data      for p_case= 1   */
/* _______________________________________________________________  */
/*                                                                  */
/*                                                                 !*/

 if ( p_case == 1 )
    {

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 p_cur->hed_cu.type %d \n", p_cur->hed_cu.type );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 p_cur->hed_cu.p_ptr %d \n", (int)p_cur->hed_cu.p_ptr);
fflush(dbgfil(SURPAC));
}

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 p_cur->fnt_cu %d \n", (int)p_cur->fnt_cu );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 p_cur->lgt_cu %f \n", p_cur->lgt_cu );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 p_cur->al_cu %f \n", p_cur->al_cu );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 p_cur->ns_cu %d \n", (int)p_cur->ns_cu );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 p_cur->plank_cu %d \n", (int)p_cur->plank_cu );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 p_cur->pcsy_cu %d \n", (int)p_cur->pcsy_cu );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 p_cur->nsgr_cu %d \n", (int)p_cur->nsgr_cu );
fflush(dbgfil(SURPAC));
}
#endif


/*!                                                                 */
/*  Start loop curve segments i_seg= istart, .... ,iend             */
/*                                                                 !*/

    for ( i_seg = istart ; i_seg <= iend; i_seg = i_seg + 1 )
      {

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 p_seg->c0x %f c1x %f c2x %f c3x %f\n", 
p_seg->c0x, p_seg->c1x, p_seg->c2x, p_seg->c3x );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 p_seg->c0y %f c1y %f c2y %f c3y %f\n", 
p_seg->c0y, p_seg->c1y, p_seg->c2y, p_seg->c3y );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 p_seg->c0z %f c1z %f c2z %f c3z %f\n", 
p_seg->c0z, p_seg->c1z, p_seg->c2z, p_seg->c3z );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 p_seg->c0  %f c1  %f c2  %f c3  %f\n", 
p_seg->c0 , p_seg->c1 , p_seg->c2 , p_seg->c3  );
fflush(dbgfil(SURPAC));
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 p_seg->nxt_seg %d ofs %f sl %f typ %d\n", 
(int)p_seg->nxt_seg, p_seg->ofs, p_seg->sl, (int)p_seg->typ );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 p_seg->subtyp %d spek_gm %d spek2_gm %d \n", 
(int)p_seg->subtyp, (int)p_seg->spek_gm, (int)p_seg->spek2_gm );
fflush(dbgfil(SURPAC));
}
#endif


      }  /* End loop curve segments i_seg= 1,2,3                    */

   }  /* End p_case = 1                                             */

/*!                                                                 */
/* 3. Printout of points and tangents in MBS format for p_case= 2   */
/* _______________________________________________________________  */
/*                                                                  */
/*  Start loop curve segments i_seg= istart, .... ,iend             */
/*                                                                 !*/

 if ( p_case == 2 )
    {
    for ( i_seg = istart ; i_seg <= iend; i_seg = i_seg + 1 )
      {
/*!                                                                 */
/*    Curve evaluation at start and end of segment i_seg            */
/*    Calls of GE109.                                               */
/*                                                                 !*/

/*    Start of segment i_seg                                   */

      xyz.t_global = (DBfloat)i_seg + comptol;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 i_seg %d xyz.t_global %25.15f for point s_pt\n",
  (int)i_seg,xyz.t_global );
fflush(dbgfil(SURPAC));
}
#endif


      status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
      if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_cur_print   ");
        return(varkon_erpush("SU2943",errbuf));
        }

      s_pt = xyz.r;
      s_tan[0]= xyz.drdt.x_gm;
      s_tan[1]= xyz.drdt.y_gm;
      s_tan[2]= xyz.drdt.z_gm;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 i_seg %d s_pt %10.2f %10.2f %10.2f\n",
  (int)i_seg,s_pt.x_gm,s_pt.y_gm,s_pt.z_gm );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 e_tan %f %f %f \n",
  s_tan[0],s_tan[1],s_tan[2]   );
}
#endif


/*    End of current segment i_seg                                  */

      xyz.t_global = (DBfloat)i_seg + 1.0 - comptol;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 i_seg %d xyz.t_global %25.15f for point e_pt\n",
  (int)i_seg,xyz.t_global );
fflush(dbgfil(SURPAC));
}
#endif


      status=GE109 ((DBAny *)p_cur,p_seg,&xyz);
      if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_cur_print   ");
        return(erpush("SU2943",errbuf));
        }

      e_pt = xyz.r;
      e_tan[0]= xyz.drdt.x_gm;
      e_tan[1]= xyz.drdt.y_gm;
      e_tan[2]= xyz.drdt.z_gm;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 i_seg %d e_pt %10.2f %10.2f %10.2f\n",
  (int)i_seg,e_pt.x_gm,e_pt.y_gm,e_pt.z_gm );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 e_tan %f %f %f \n",
  e_tan[0],e_tan[1],e_tan[2]   );
}
#endif

      }  /* End loop curve segments i_seg= 1,2,3                    */
   }  /* End p_case = 2                                             */


/*                                                                  */
/* 4. Exit                                                          */
/* _______                                                          */
/*                                                                  */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur799 Exit varkon_cur_print  \n");
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

} /* End of function */


/********************************************************************/
