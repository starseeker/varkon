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
/*  Function: varkon_sur_patadr                    File: sur211.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the patch adress for a given            */
/*  global (topological) parameter (u,v) point on a surface.        */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                 !*/
/*  Revisions                                                       */
/*                                                                  */
/*  1994-04-01   Originally written                                 */
/*  1996-05-28   Elimination of compilation warnings                */
/*  1997-02-09   Elimination of compilation warnings                */
/*  1999-11-20   Free source code modifications                     */
/*                                                                  */
/********************************************************************/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_patadr     Patch adress for given UV point  */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/*           varkon_erpush          * Error message to terminal     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2953 = Patch iu,iv=  outside surface nu,nv=                    */
/* SU2993 = Severe program error in varkon_sur_patadr (sur211).     */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
   DBstatus varkon_sur_patadr (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBfloat  uglob,       /* Global u value                          */
   DBfloat  vglob,       /* Global v value                          */
   DBint    nu,          /* Number of patches in U direction        */
   DBint    nv,          /* Number of patches in V direction        */
   DBint   *p_iu,        /* Patch address    IU                     */
   DBint   *p_iv,        /*                  IV                     */
   DBfloat *p_u,         /* Patch (local) U parameter value         */
   DBfloat *p_v )        /* Patch (local) V parameter value         */

/* Out:                                                             */
/*       Data to p_iu, p_iv, p_u and p_v                            */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBfloat u_pat;        /* Patch (local) U parameter value         */
   DBfloat v_pat;        /* Patch (local) V parameter value         */
   DBint   iu,iv;        /* Patch number (adress)                   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Patch address iu,iv and patch parameter pt u_pat,v_pat        */
/* _________________________________________________________        */
/*                                                                  */
/* (Error SU2953 if parameter point is outside definition area)     */
/*                                                                 !*/

/*!                                                                 */
/*  Integer part iu of uglob and iv of vglob                        */
/*                                                                 !*/

    iu= (DBint)floor(uglob);
    iv= (DBint)floor(vglob);

/*!                                                                 */
/*  Check that patch iu exists  1 <= iu <= nu + 1                   */
/*                                                                 !*/

    if (iu > nu + 1 )
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur211 Patch iu= %d iv= %d outside definition area nu= %d nv= %d\n",
   (int)iu,(int)iv,(int)nu,(int)nv);
  }
#endif
        sprintf(errbuf,"%d %d%%%d %d varkon_sur_patadr (sur211)"
               ,(int)iu,(int)iv,(int)nu,(int)nv);
        return(varkon_erpush("SU2953",errbuf));
        }
    if (iu < 1 )
        {
        sprintf(errbuf,"%d %d%%%d %d varkon_sur_patadr (sur211)"
               ,(int)iu,(int)iv,(int)nu,(int)nv);
        return(varkon_erpush("SU2953",errbuf));
        }

/*!                                                                 */
/*  Calculate local parameter u_pat= decimal part of uglob          */
/*                                                                 !*/

    if (iu == nu+1 )
        {
        iu = iu - 1;
        u_pat  = uglob - (DBfloat)iu;     
        if ( u_pat > 1.0 + TOL4 )
            {
            sprintf(errbuf,"%d %d%%%d %d varkon_sur_patadr (sur211)"
               ,(int)nu+1,(int)iv,(int)nu,(int)nv);
            return(varkon_erpush("SU2953",errbuf));
            }
        }
    else                    
        {
        u_pat  = uglob - (DBfloat)iu;     
        }


/*!                                                                 */
/*  Check that patch iv exists  1 <= iv <= nv + 1                   */
/*                                                                 !*/

    if (iv > nv + 1 )
        {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur211 Patch iu= %d iv= %d outside definition area nu= %d nv= %d\n",
   (int)iu,(int)iv,(int)nu,(int)nv);
  }
#endif
        sprintf(errbuf,"%d %d%%%d %d varkon_sur_patadr (sur211)"
               ,(int)iu,(int)iv,(int)nu,(int)nv);
        return(varkon_erpush("SU2953",errbuf));
        }
    if (iv < 1 )
        {
        sprintf(errbuf,"%d %d%%%d %d varkon_sur_patadr (sur211)"
               ,(int)iu,(int)iv,(int)nu,(int)nv);
        return(varkon_erpush("SU2953",errbuf));
        }

/*!                                                                 */
/*  Calculate local parameter v_pat= decimal part of vglob          */
/*                                                                 !*/

    if (iv == nv+1 )
        {
        iv = iv - 1;
        v_pat = vglob - (DBfloat)iv;     
        if ( v_pat > 1.0 + TOL4 )
            {
            sprintf(errbuf,"%d %d%%%d %d varkon_sur_patadr (sur211)"
               ,(int)iu,(int)nv+1,(int)nu,(int)nv);
            return(varkon_erpush("SU2953",errbuf));
            }
        }
    else                    
        {
        v_pat  = vglob - (DBfloat)iv;     
        }

/*!                                                                 */
/* 2. Data to output variables                                      */
/* ___________________________                                      */
/*                                                                  */
/*                                                                 !*/

   *p_iu = iu;
   *p_iv = iv;
   *p_u  = u_pat;
   *p_v  = v_pat;

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur211 Exit*Patch address *p_iu %d *p_iv %d *p_u %f *p_v %f\n",
   (int)*p_iu,(int)*p_iv,*p_u,*p_v);
  fflush(dbgfil(SURPAC)); 
  }
#endif


    return(SUCCED);

} /* End of function                                                */

/********************************************************************/

