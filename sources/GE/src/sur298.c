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
/*  Function: varkon_sur_poitanl                   File: sur298.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates the tangent lengths.                    */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-22   Originally written                                 */
/*  1995-04-02   Debug                                              */
/*  1996-05-28   Eliminated compilation warnings                    */
/*  1997-02-09   Eliminated compilation warnings                    */
/*  1999-12-13   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_poitanl    Corner tangent lengths for surf. */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_poiadr       * Address in POISUR for point            */
/* GEmktf_3p               * Local coordinate system                */
/* varkon_erinit           * Initialize error stack                 */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_poitanl         */
/* SU2663 = Tangent for point iu,iv is not normalized               */
/* SU2993 = Severe program error (  ) in varkon_sur_poitanl         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/

      DBstatus        varkon_sur_poitanl (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBint    c_case,       /* Creation case                           */
  DBint    iu_s,         /* Start U point                           */
  DBint    iv_s,         /* Start V point                           */
  DBint    iu_d,         /* Delta U point                           */
  DBint    iv_d,         /* Delta V point                           */
  DBint    iu_e,         /* End   U point                           */
  DBint    iv_e,         /* End   V point                           */
  DBint    f_p,          /* Flag ....    Eq:-1: No Eq 1: U Eq 2: V  */
  DBint    nu_poi,       /* Number of points in U direction   (ptr) */
  DBint    nv_poi,       /* Number of points in V direction   (ptr) */
  SURPOI  *p_surpoi,     /* Point table SURPOI (alloc. area)  (ptr) */
  DBint   *p_dflag,      /* Eq. 1: Pt def. Eq. -1: Not def.   (ptr) */
  DBint   *p_iuu,        /* Point number U for undefined pt   (ptr) */
  DBint   *p_ivu )       /* Point number V for undefined pt   (ptr) */

/* Out:                                                             */
/*                                                                  */
/*      Data is modified in table POISUR                            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint   eu;            /* Eq. to iu_d if f_p= 1 else eq. to zero  */
  DBint   ev;            /* Eq. to iv_d if f_p= 2 else eq. to zero  */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint   iup,ivp;       /* Loop index for input limit points       */
  DBint   iuc,ivc;       /* Current address to corner point in file */
  SURPOI *p_r00;         /* Record for r00 in table SURPOI    (ptr) */
  SURPOI *p_r01;         /* Record for r01 in table SURPOI    (ptr) */
  SURPOI *p_r10;         /* Record for r10 in table SURPOI    (ptr) */
  SURPOI *p_r11;         /* Record for r11 in table SURPOI    (ptr) */
  DBint   recno;         /* Current record in table SURPOI          */
  DBint   dflag;         /* Eq. 1: Point defined Eq. -1: Not defined*/

  EVALS  r00;            /* Corner point U=0 V=0                    */
  EVALS  r01;            /* Corner point U=0 V=1                    */
  EVALS  r10;            /* Corner point U=1 V=0                    */
  EVALS  r11;            /* Corner point U=1 V=1                    */
  DBint  status;         /* Error code from called function         */

  DBfloat uleng;         /* Length of chord in U direction          */
  DBfloat vleng;         /* Length of chord in U direction          */
  DBfloat ulenge;        /* Length of chord in U direction end pat  */
  DBfloat vlenge;        /* Length of chord in U direction end pat  */

  DBTmat csys;           /* Coordinate system                       */
  DBVector origin;       /* Origin for coordinate system            */
  DBVector vecx;         /* X axis for coordinate system            */
  DBVector vecy;         /* Close to Y axis for coordinate system   */

  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur298 Enter***varkon_sur_poitanl**  c_case %d\n" , (int)c_case  );
}
#endif
 
   *p_dflag= 1;                          /* Initiate p_dflag        */
   *p_iuu  = 0;                          /* Initiate undef. pt no   */
   *p_ivu  = 0;                          /* Initiate undef. pt no   */
   dflag = 0;                            /*                         */

/*!                                                                 */
/* Let eu= iu_d and ev=   0  for a U closed surface        (f_p=1)  */
/* Let eu=  0   and ev= iv_d for a V closed surface        (f_p=2)  */
/* Let eu=  0   and ev=  0   if surface not is closed               */
/*                                                                 !*/

  if      ( f_p == 1 )
      {
      eu = iu_d;
      ev = 0;
      }
  else if ( f_p == 2 )
      {
      eu = 0;
      ev = iv_d;
      }
  else
      {
      eu = 0;
      ev = 0;
      }

/*!                                                                 */
/* 4. Modify tangent vector lengths                                 */
/* ________________________________                                 */
/*                                                                  */
/*                                                                  */
/* Loop all U points iup= iu_s to iu_e-iu_d+eu with step iu_d       */
/*  Loop all V points ivp= iv_s to iv_e-iv_d+ev with step iv_d      */
/*                                                                 !*/

for (iup=iu_s;iup<=iu_e-iu_d+eu;         /* Start loop U points     */
                        iup=iup+iu_d)    /*                         */
 {                                       /*                         */

 for (ivp=iv_s;ivp<=iv_e-iv_d+ev;        /* Start loop V points     */
                         ivp=ivp+iv_d)   /*                         */
  {                                      /*                         */

/*!                                                                 */
/*    Retrieve corner point P00 with derivatives from table SURPOI  */
/*                                                                 !*/

      iuc = iup;                         /* Point address in file   */
      ivc = ivp;                         /*                         */
   status= varkon_sur_poiadr 
   (iuc,ivc,p_surpoi,nu_poi,nv_poi,
    iu_s,iv_s,iu_e,iv_e,f_p,&recno,&dflag);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur298 varkon_sur_poiadr failed for r00\n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poitanl");
    return(varkon_erpush("SU2943",errbuf));
    }

    p_r00 = p_surpoi + recno - 1;        /* Current SURPOI adress   */

    if ( dflag  < 0   )                  /* Check that surface pt   */
       {                                 /* is defined              */
       *p_dflag= -1;                     /* Flag for undef. pt      */
       *p_iuu  = iuc;                    /* Undef pt number         */
       *p_ivu  = ivc;                    /*                         */
       goto undef;                       /*                         */
       }                                 /*                         */

    r00       = p_r00->spt;              /* r00                     */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur298 p_r00->iu %d p_r00->iv %d iuc %d ivc %d\n",
     (int)p_r00->iu,(int)p_r00->iv, (int)iuc, (int)ivc  ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur298 r00   %f %f %f \n",
                   r00.r_x,r00.r_y,r00.r_z ); 
  }
#endif

/*!                                                                 */
/*    Retrieve corner point P01 with derivatives from table SURPOI  */
/*    Point adress: iup , ivp + iv_d                                */
/*                                                                 !*/

      iuc = iup;                         /* Point address in file   */
      ivc = ivp + iv_d;                  /*                         */
   status= varkon_sur_poiadr 
   (iuc,ivc,p_surpoi,nu_poi,nv_poi,
    iu_s,iv_s,iu_e,iv_e,f_p,&recno,&dflag);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur298 varkon_sur_poiadr failed for r01\n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poitanl");
    return(varkon_erpush("SU2943",errbuf));
    }

    p_r01 = p_surpoi + recno - 1;        /* Current SURPOI adress   */

    if ( dflag  < 0  )                   /* Check that surface pt   */
       {                                 /* is defined              */
       *p_dflag= -1;                     /* Flag for undef. pt      */
       *p_iuu  = iuc;                    /* Undef pt number         */
       *p_ivu  = ivc;                    /*                         */
       goto undef;                       /*                         */
       }                                 /*                         */

    r01       = p_r01->spt;              /* r01                     */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur298 p_r01->iu %d p_r01->iv %d iuc %d ivc %d\n",
     (int)p_r01->iu,(int)p_r01->iv, (int)iuc, (int)ivc  ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur298 r01   %f %f %f \n",
                   r01.r_x,r01.r_y,r01.r_z ); 
  }
#endif

/*!                                                                 */
/*    Retrieve corner point P10 with derivatives from table SURPOI  */
/*                                                                 !*/

      iuc = iup + iu_d;                  /* Point address in file   */
      ivc = ivp;                         /*                         */
   status= varkon_sur_poiadr 
   (iuc,ivc,p_surpoi,nu_poi,nv_poi,
    iu_s,iv_s,iu_e,iv_e,f_p,&recno,&dflag);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur298 varkon_sur_poiadr failed for r10\n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poitanl");
    return(varkon_erpush("SU2943",errbuf));
    }

    p_r10 = p_surpoi + recno - 1;        /* Current SURPOI adress   */

    if ( dflag  < 0 )                    /* Check that surface pt   */
       {                                 /* is defined              */
       *p_dflag= -1;                     /* Flag for undef. pt      */
       *p_iuu  = iuc;                    /* Undef pt number         */
       *p_ivu  = ivc;                    /*                         */
       goto undef;                       /*                         */
       }                                 /*                         */

    r10       = p_r10->spt;              /* r10                     */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur298 p_r10->iu %d p_r10->iv %d iuc %d ivc %d\n",
     (int)p_r10->iu,(int)p_r10->iv, (int)iuc, (int)ivc  ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur298 r10   %f %f %f \n",
                   r10.r_x,r10.r_y,r10.r_z ); 
  }
#endif

/*!                                                                 */
/*    Retrieve corner point P11 with derivatives from table SURPOI  */
/*                                                                 !*/

      iuc = iup + iu_d;                  /* Point address in file   */
      ivc = ivp + iv_d;                  /*                         */
   status= varkon_sur_poiadr 
   (iuc,ivc,p_surpoi,nu_poi,nv_poi,
    iu_s,iv_s,iu_e,iv_e,f_p,&recno,&dflag);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur298 varkon_sur_poiadr failed \n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poitanl");
    return(varkon_erpush("SU2943",errbuf));
    goto undef;
    }

    p_r11 = p_surpoi + recno - 1;        /* Current SURPOI adress   */

    if ( dflag  < 0 )                    /* Check that surface pt   */
       {                                 /* is defined              */
       *p_dflag= -1;                     /* Flag for undef. pt      */
       *p_iuu  = iuc;                    /* Undef pt number         */
       *p_ivu  = ivc;                    /*                         */
       goto undef;                       /*                         */
       }                                 /*                         */

    r11       = p_r11->spt;              /* r11                     */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur298 p_r11->iu %d p_r11->iv %d iuc %d ivc %d\n",
     (int)p_r11->iu,(int)p_r11->iv, (int)iuc, (int)ivc  ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur298 r11   %f %f %f \n",
                   r11.r_x,r11.r_y,r11.r_z ); 
  }
#endif

/*!                                                                 */
/*   Calculate tangent vector lengths.                              */
/*                                                                 !*/

/*!                                                                 */
/*   Tangent vector lengths equal to the chords:                    */
/*   uleng=  !r00 - r10! and vleng=  !r00 - r01!                    */
/*   ulenge= !r01 - r11! and vlenge= !r10 - r11!                    */
/*                                                                 !*/


    uleng= SQRT((r00.r_x - r10.r_x)*(r00.r_x - r10.r_x) +
                (r00.r_y - r10.r_y)*(r00.r_y - r10.r_y) +
                (r00.r_z - r10.r_z)*(r00.r_z - r10.r_z));
    vleng= SQRT((r00.r_x - r01.r_x)*(r00.r_x - r01.r_x) +
                (r00.r_y - r01.r_y)*(r00.r_y - r01.r_y) +
                (r00.r_z - r01.r_z)*(r00.r_z - r01.r_z));
    ulenge=SQRT((r01.r_x - r11.r_x)*(r01.r_x - r11.r_x) +
                (r01.r_y - r11.r_y)*(r01.r_y - r11.r_y) +
                (r01.r_z - r11.r_z)*(r01.r_z - r11.r_z));
    vlenge=SQRT((r10.r_x - r11.r_x)*(r10.r_x - r11.r_x) +
                (r10.r_y - r11.r_y)*(r10.r_y - r11.r_y) +
                (r10.r_z - r11.r_z)*(r10.r_z - r11.r_z));

/*!                                                                 */
/*   Degenerate patch, i.e. two corner points are equal             */
/*   So far is only r00=r01 programmed ......                       */
/*                                                                 !*/

    if ( uleng  < 0.000001 )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur298 Degenerate patch r00 = r10 not yet implemented\n"); 
#endif
sprintf(errbuf,"(r00=r10 not impl.)%%varkon_sur_poitanl");
       return(varkon_erpush("SU2993",errbuf));
       }
    if ( ulenge < 0.000001 )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur298 Degenerate patch r10 = r11 not yet implemented\n"); 
#endif
sprintf(errbuf,"(r10=r11 not impl.)%%varkon_sur_poitanl");
       return(varkon_erpush("SU2993",errbuf));
       }
    if ( vlenge < 0.000001 )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur298 Degenerate patch r01 = r11 not yet implemented\n"); 
#endif
sprintf(errbuf,"(r01=r11 not impl.)%%varkon_sur_poitanl");
       return(varkon_erpush("SU2993",errbuf));
       }

    if ( vleng < 0.000001 )
       {
       origin.x_gm = r00.r_x;
       origin.y_gm = r00.r_y;
       origin.z_gm = r00.r_z;
       vecx.x_gm   = r00.u_x;
       vecx.y_gm   = r00.u_y;
       vecx.z_gm   = r00.u_z;
       vecy.x_gm   = r01.u_x;
       vecy.y_gm   = r01.u_y;
       vecy.z_gm   = r01.u_z;
       status = GEmktf_3p (&origin,&vecx,&vecy,&csys);
       if(status<0)
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur298 GEmktf_3p failed\n"); 
#endif
sprintf(errbuf,"GEmktf_3p%%varkon_sur_poitanl");
       return(varkon_erpush("SU2943",errbuf));
       }
       p_r00->spt.v_x = csys.g21;
       p_r00->spt.v_y = csys.g22;
       p_r00->spt.v_z = csys.g23;
       p_r01->spt.v_x = csys.g21;
       p_r01->spt.v_y = csys.g22;
       p_r01->spt.v_z = csys.g23;
       r00.v_x = csys.g21;
       r00.v_y = csys.g22;
       r00.v_z = csys.g23;
       r01.v_x = csys.g21;
       r01.v_y = csys.g22;
       r01.v_z = csys.g23;
       vleng = 0.0000001; /* TODO Implement triang. in sur210, 220.*/
#ifdef STRANGE_ERROR
       /* Reason that next patch not will be triangular ?  */
       p_r00->spt.r_x = r00.r_x - 0.00001*csys.g21;
       p_r00->spt.r_y = r00.r_y - 0.00001*csys.g22;
       p_r00->spt.r_z = r00.r_z - 0.00001*csys.g23;
       vleng = SQRT( (p_r01->spt.r_x-p_r00->spt.r_x)*
                     (p_r01->spt.r_x-p_r00->spt.r_x)  +
                     (p_r01->spt.r_y-p_r00->spt.r_y)*
                     (p_r01->spt.r_y-p_r00->spt.r_y)  +
                     (p_r01->spt.r_z-p_r00->spt.r_z)*
                     (p_r01->spt.r_z-p_r00->spt.r_z)  );
       p_r00->spt.r_x = r00.r_x + 0.0001*r00.u_x; 
       p_r00->spt.r_y = r00.r_y + 0.0001*r00.u_y; 
       p_r00->spt.r_z = r00.r_z + 0.0001*r00.u_z; 
       p_r01->spt.r_x = r01.r_x + 0.0001*r01.u_x; 
       p_r01->spt.r_y = r01.r_y + 0.0001*r01.u_y; 
       p_r01->spt.r_z = r01.r_z + 0.0001*r01.u_z; 
       p_r00->spt.r_x = r00.r_x - 0.000000005*csys.g21;
       p_r00->spt.r_y = r00.r_y - 0.000000005*csys.g22;
       p_r00->spt.r_z = r00.r_z - 0.000000005*csys.g23;
       p_r01->spt.r_x = r01.r_x + 0.000000005*csys.g21;
       p_r01->spt.r_y = r01.r_y + 0.000000005*csys.g22;
       p_r01->spt.r_z = r01.r_z + 0.000000005*csys.g23;
#endif

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur298 vecx r00.u_x %f r00.u_y %f r00.u_z %f  \n",
  r00.u_x,r00.u_y,r00.u_z );      
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur298 vecy r01.u_x %f r01.u_y %f r01.u_z %f  \n",
  r01.u_x,r01.u_y,r01.u_z );      
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur298 tria r00.v_x %f r00.v_y %f r00.v_z %f  \n",
  r00.v_x,r00.v_y,r00.v_z );      
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur298 tria r01.v_x %f r01.v_y %f r01.v_z %f  \n",
  r01.v_x,r01.v_y,r01.v_z );      
}
#endif
       }   /* End vleng = 0 */


    p_r00->u_l3 = uleng;                 /* U length for patch 3    */
    p_r00->u_l4 = uleng;                 /* U length for patch 4    */
    p_r10->u_l1 = uleng;                 /* U length for patch 1    */
    p_r10->u_l2 = uleng;                 /* U length for patch 2    */
    p_r00->v_l3 = vleng;                 /* V length for patch 3    */
    p_r00->v_l4 = vleng;                 /* V length for patch 4    */
    p_r01->v_l1 = vleng;                 /* V length for patch 1    */
    p_r01->v_l2 = vleng;                 /* V length for patch 2    */

    if ( iup == iu_s )
       {
       p_r00->u_l1 = 0.0;                /* U length for patch 1    */
       p_r00->u_l2 = 0.0;                /* U length for patch 2    */
       }
    if ( ivp == iv_s )
       {
       if ( f_p == 2 )
       {
       p_r00->v_l3 = vleng;              /* V l!!! lite fel !!!3    */
       p_r00->v_l4 = vleng;              /* V length for patch 4    */
       }
       else
       {
       p_r00->v_l1 = 0.0;                /* V length for patch 1    */
       p_r00->v_l2 = 0.0;                /* V length for patch 2    */
       }
       }
    if ( iup == iu_e - iu_d + eu )
       {
       p_r10->u_l3 = 0.0;                /* U length for patch 3    */
       p_r10->u_l4 = 0.0;                /* U length for patch 4    */
       p_r10->v_l3 = vlenge;             /* V length for patch 3    */
       p_r10->v_l4 = vlenge;             /* V length for patch 4    */
       p_r11->v_l1 = vlenge;             /* V length for patch 1    */
       p_r11->v_l2 = vlenge;             /* V length for patch 2    */
       }
    if ( ivp == iv_e - iv_d + ev )
       {
       if ( f_p == 2 )
       {
       p_r01->v_l3 = vlenge;             /* V  !! Lite fel !!!!     */
       p_r01->v_l4 = vlenge;             /* V length for patch 4    */
       } 
       else
       {
       p_r01->v_l3 = 0.0;                /* V length for patch 3    */
       p_r01->v_l4 = 0.0;                /* V length for patch 4    */
       }
       p_r01->u_l3 = ulenge;             /* U length for patch 3    */
       p_r01->u_l4 = ulenge;             /* U length for patch 4    */
       p_r11->u_l1 = ulenge;             /* U length for patch 1    */
       p_r11->u_l2 = ulenge;             /* U length for patch 2    */
       }
    if ( iup == iu_e - iu_d + eu &&
         ivp == iv_e - iv_d + ev )
       {
       p_r11->u_l1 = ulenge;             /* U length for patch 1    */
       p_r11->u_l2 = ulenge;             /* U length for patch 2    */
       p_r11->v_l1 = vlenge;             /* V length for patch 1    */
       p_r11->v_l2 = vlenge;             /* V length for patch 2    */
       p_r11->u_l3 = 0.0;                /* U length for patch 3    */
       p_r11->u_l4 = 0.0;                /* U length for patch 4    */
       if ( f_p == 2 )
       {
       p_r11->v_l3 = vlenge;             /* V l!!! lite fel !!!3    */
       p_r11->v_l4 = vlenge;             /* V length for patch 4    */
       }
       else
       {
       p_r11->v_l3 = 0.0;                /* V length for patch 3    */
       p_r11->v_l4 = 0.0;                /* V length for patch 4    */
       }
       }
    if ( iup == iu_s  &&
         ivp == iv_e - iv_d + ev )
       {
       p_r01->u_l1 = 0.0;                /* U length for patch 1    */
       p_r01->u_l2 = 0.0;                /* U length for patch 2    */
       p_r01->u_l3 = uleng;              /* U length for patch 3    */
       p_r01->u_l4 = uleng;              /* U length for patch 4    */
       if ( f_p == 2 )
       {
       p_r01->v_l3 = vlenge;             /* V l!!! lite fel !!!3    */
       p_r01->v_l4 = vlenge;             /* V length for patch 4    */
       }
       else
       {
       p_r01->v_l3 = 0.0;                /* V length for patch 3    */
       p_r01->v_l4 = 0.0;                /* V length for patch 4    */
       }
       }
    if ( iup == iu_e - iu_d + eu &&
         ivp == iv_s )
       {
       p_r10->u_l3 = 0.0;                /* U length for patch 3    */
       p_r10->u_l4 = 0.0;                /* U length for patch 4    */
       p_r10->v_l3 = vleng;              /* V length for patch 3    */
       p_r10->v_l4 = vleng;              /* V length for patch 4    */
       if ( f_p == 2 )
       {
       p_r10->v_l1 = vleng;              /* V l!!! lite fel !!!3    */
       p_r10->v_l1 = vleng;              /* V length for patch 4    */
       }
       else
       {
       p_r10->v_l1 = 0.0;                /* V length for patch 1    */
       p_r10->v_l2 = 0.0;                /* V length for patch 2    */
       }
       }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur298 iup %d ivp %d (End iu_e-iu_d+eu %d iv_e-iv_d+ev %d\n",
  (int)iup,(int)ivp, (int)(iu_e-iu_d+eu) ,(int)(iv_e-iv_d+ev)  );      
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur298 r00 u_l1 %8.4f u_l3 %8.4f v_l1 %8.4f v_l3 %8.4f iu %d iv %d\n",
          p_r00->u_l1,p_r00->u_l3,p_r00->v_l1,p_r00->v_l3, 
          (int)p_r00->iu  ,(int)p_r00->iv   );      
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur298 r01 u_l1 %8.4f u_l3 %8.4f v_l1 %8.4f v_l3 %8.4f iu %d iv %d\n",
          p_r01->u_l1,p_r01->u_l3,p_r01->v_l1,p_r01->v_l3, 
          (int)p_r01->iu  ,(int)p_r01->iv   );      
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur298 r10 u_l1 %8.4f u_l3 %8.4f v_l1 %8.4f v_l3 %8.4f iu %d iv %d\n",
          p_r10->u_l1,p_r10->u_l3,p_r10->v_l1,p_r10->v_l3, 
          (int)p_r10->iu  ,(int)p_r10->iv   );      
}
if ( iup == iu_e - iu_d + eu &&
     ivp == iv_e - iv_d + ev )
{
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur298 r11 u_l1 %8.4f u_l3 %8.4f v_l1 %8.4f v_l3 %8.4f iu %d iv %d\n",
          p_r11->u_l1,p_r11->u_l3,p_r11->v_l1,p_r11->v_l3, 
          (int)p_r11->iu  ,(int)p_r11->iv   );      
}
}      
#endif

/*!                                                                 */
/*   Normalise the tangent in EVALS ...                             */
/*   .....  l{gg till !!!!                                          */
/*                                                                 !*/


    uleng=SQRT(r00.u_x*r00.u_x + 
               r00.u_y*r00.u_y +
               r00.u_z*r00.u_z);
    vleng=SQRT(r00.v_x*r00.v_x + 
               r00.v_y*r00.v_y +
               r00.v_z*r00.v_z);
    if      ( fabs(uleng-1.0) > 0.01 ||
              fabs(vleng-1.0) > 0.01    )
         {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur298 r00.u_x %f r00.u_y %f r00.u_z %f uleng %f\n",
          r00.u_x ,  r00.u_y ,  r00.u_z ,  uleng);      
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur298 r00.v_x %f r00.v_y %f r00.v_z %f vleng %f\n",
          r00.v_x ,  r00.v_y ,  r00.v_z ,  vleng);      
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur298 Error: All tangent vectors shall be normalised  \n");
  }
#endif
         sprintf(errbuf,"%5d %5d%%varkon_sur_poitanl"
          ,(int)p_r00->iu,(int)p_r00->iv); 
         varkon_erinit();
         return(varkon_erpush("SU2663",errbuf));
         }

    uleng=SQRT(r01.u_x*r01.u_x + 
               r01.u_y*r01.u_y +
               r01.u_z*r01.u_z);
    vleng=SQRT(r01.v_x*r01.v_x + 
               r01.v_y*r01.v_y +
               r01.v_z*r01.v_z);
    if      ( fabs(uleng-1.0) > 0.01 ||
              fabs(vleng-1.0) > 0.01    )
         {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur298 r01.u_x %f r01.u_y %f r01.u_z %f uleng %f\n",
          r01.u_x ,  r01.u_y ,  r01.u_z ,  uleng);      
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur298 r01.v_x %f r01.v_y %f r01.v_z %f vleng %f\n",
          r01.v_x ,  r01.v_y ,  r01.v_z ,  vleng);      
  }
#endif
         sprintf(errbuf,"%5d %5d%%varkon_sur_poitanl"
          ,(int)p_r01->iu,(int)p_r01->iv); 
         varkon_erinit();
         return(varkon_erpush("SU2663",errbuf));
         }

    uleng=SQRT(r10.u_x*r10.u_x + 
               r10.u_y*r10.u_y +
               r10.u_z*r10.u_z);
    vleng=SQRT(r10.v_x*r10.v_x + 
               r10.v_y*r10.v_y +
               r10.v_z*r10.v_z);
    if      ( fabs(uleng-1.0) > 0.01 ||
              fabs(vleng-1.0) > 0.01    )
         {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur298 r10.u_x %f r10.u_y %f r10.u_z %f uleng %f\n",
          r10.u_x ,  r10.u_y ,  r10.u_z ,  uleng);      
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur298 r10.v_x %f r10.v_y %f r10.v_z %f vleng %f\n",
          r10.v_x ,  r10.v_y ,  r10.v_z ,  vleng);      
  }
#endif
         sprintf(errbuf,"%5d %5d%%varkon_sur_poitanl"
          ,(int)p_r10->iu,(int)p_r10->iv); 
         varkon_erinit();
         return(varkon_erpush("SU2663",errbuf));
         }

    uleng=SQRT(r11.u_x*r11.u_x + 
               r11.u_y*r11.u_y +
               r11.u_z*r11.u_z);
    vleng=SQRT(r11.v_x*r11.v_x + 
               r11.v_y*r11.v_y +
               r11.v_z*r11.v_z);
    if      ( fabs(uleng-1.0) > 0.01 ||
              fabs(vleng-1.0) > 0.01    )
         {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur298 r11.u_x %f r11.u_y %f r11.u_z %f uleng %f\n",
          r11.u_x ,  r11.u_y ,  r11.u_z ,  uleng);      
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur298 r11.v_x %f r11.v_y %f r11.v_z %f vleng %f\n",
          r11.v_x ,  r11.v_y ,  r11.v_z ,  vleng);      
  }
#endif
         sprintf(errbuf,"%5d %5d%%varkon_sur_poitanl"
          ,(int)p_r11->iu,(int)p_r11->iv); 
         varkon_erinit();
         return(varkon_erpush("SU2663",errbuf));
         }



    }                                    /* End   loop V patches    */
  }                                      /* End   loop U patches    */


/*!                                                                 */
/*  End  all V points ivp= iv_s to iv_e-iv_d+ev with step iv_d      */
/* End  all U points iup= iu_s to iu_e-iu_d+eu with step iu_d       */
/*                                                                 !*/

/*                                                                  */
/* n. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                  */

undef:; /*! Label undef: Requested point is undefined              !*/

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur298 Exit***varkon_sur_poitanl ** \n");
  }
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/
