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
/*  Function: varkon_sur_poitwist                  File: sur299.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates twist vectors.                          */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-09   Originally written                                 */
/*  1996-05-28   Eliminated compilation warnings                    */
/*  1999-12-13   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_poitwist   Twist vector calculation         */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_poiadr       * Address in POISUR for point            */
/* GEmktf_3p               * Local coordinate system                */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_poitwist        */
/* SU2993 = Severe program error (  ) in varkon_sur_poitwist        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/

     DBstatus         varkon_sur_poitwist (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBint    c_case,       /* Creation case                           */
                         /* Eq. 1: Zero twists                      */
                         /* Eq. 2: Adinis method                    */
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
  SURPOI *p_r02;         /* Record for r02 in table SURPOI    (ptr) */
  SURPOI *p_r12;         /* Record for r12 in table SURPOI    (ptr) */
  SURPOI *p_r22;         /* Record for r22 in table SURPOI    (ptr) */
  SURPOI *p_r20;         /* Record for r20 in table SURPOI    (ptr) */
  SURPOI *p_r21;         /* Record for r21 in table SURPOI    (ptr) */
  DBint   recno;         /* Current record in table SURPOI          */
  DBint   dflag;         /* Eq. 1: Point defined Eq. -1: Not defined*/

  EVALS  r00;            /* Corner point U=0 V=0                    */
  EVALS  r01;            /* Corner point U=0 V=1                    */
  EVALS  r10;            /* Corner point U=1 V=0                    */
  EVALS  r11;            /* Corner point U=1 V=1                    */
  EVALS  r02;            /* Corner point U=0 V=2                    */
  EVALS  r12;            /* Corner point U=1 V=2                    */
  EVALS  r22;            /* Corner point U=2 V=2                    */
  EVALS  r20;            /* Corner point U=2 V=0                    */
  EVALS  r21;            /* Corner point U=2 V=1                    */
  DBint  status;         /* Error code from called function         */

  DBfloat uleng;         /* Length of chord in U direction          */
  DBfloat vleng;         /* Length of chord in U direction          */
  DBfloat ulenge;        /* Length of chord in U direction end pat  */
  DBfloat vlenge;        /* Length of chord in U direction end pat  */

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
"sur299 Enter***varkon_sur_poitwist*  c_case %d\n" , c_case  );
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

   if ( c_case == 1 ) goto zero;

/*!                                                                 */
/* 4. Calculate twist vectors with Adinis method                    */
/* _____________________________________________                    */
/*                                                                  */
/*                                                                  */
/* Loop all U points iup= iu_s to iu_e-2*iu_d+eu with step iu_d     */
/*  Loop all V points ivp= iv_s to iv_e-2*iv_d+ev with step iv_d    */
/*                                                                 !*/

for (iup=iu_s;iup<=iu_e-2*iu_d+eu;       /* Start loop U points     */
                        iup=iup+iu_d)    /*                         */
 {                                       /*                         */

 for (ivp=iv_s;ivp<=iv_e-2*iv_d+ev;      /* Start loop V points     */
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
  "sur299 varkon_sur_poiadr failed for r00\n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poitwist");
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
  "sur299 p_r00->iu %d p_r00->iv %d iuc %d ivc %d\n",
     p_r00->iu,p_r00->iv, iuc, ivc  ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur299 r00   %f %f %f \n",
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
  "sur299 varkon_sur_poiadr failed for r01\n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poitwist");
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
  "sur299 p_r01->iu %d p_r01->iv %d iuc %d ivc %d\n",
     p_r01->iu,p_r01->iv, iuc, ivc  ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur299 r01   %f %f %f \n",
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
  "sur299 varkon_sur_poiadr failed for r10\n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poitwist");
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
  "sur299 p_r10->iu %d p_r10->iv %d iuc %d ivc %d\n",
     p_r10->iu,p_r10->iv, iuc, ivc  ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur299 r10   %f %f %f \n",
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
  "sur299 varkon_sur_poiadr failed \n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poitwist");
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
  "sur299 p_r11->iu %d p_r11->iv %d iuc %d ivc %d\n",
     p_r11->iu,p_r11->iv, iuc, ivc  ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur299 r11   %f %f %f \n",
                   r11.r_x,r11.r_y,r11.r_z ); 
  }
#endif

/*!                                                                 */
/*    Retrieve corner point P02 with derivatives from table SURPOI  */
/*                                                                 !*/

      iuc = iup;                         /* Point address in file   */
      ivc = ivp + iv_d + iv_d;           /*                         */
   status= varkon_sur_poiadr 
   (iuc,ivc,p_surpoi,nu_poi,nv_poi,
    iu_s,iv_s,iu_e,iv_e,f_p,&recno,&dflag);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur299 varkon_sur_poiadr failed \n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poitwist");
    return(varkon_erpush("SU2943",errbuf));
    goto undef;
    }

    p_r02 = p_surpoi + recno - 1;        /* Current SURPOI adress   */

    if ( dflag  < 0 )                    /* Check that surface pt   */
       {                                 /* is defined              */
       *p_dflag= -1;                     /* Flag for undef. pt      */
       *p_iuu  = iuc;                    /* Undef pt number         */
       *p_ivu  = ivc;                    /*                         */
       goto undef;                       /*                         */
       }                                 /*                         */

    r02       = p_r02->spt;              /* r02                     */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur299 p_r02->iu %d p_r02->iv %d iuc %d ivc %d\n",
     p_r02->iu,p_r02->iv, iuc, ivc  ); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur299 r02   %f %f %f \n",
                   r02.r_x,r02.r_y,r02.r_z ); 
  }
#endif

/*!                                                                 */
/*    Retrieve corner point P12 with derivatives from table SURPOI  */
/*                                                                 !*/

      iuc = iup + iu_d;                  /* Point address in file   */
      ivc = ivp + iv_d + iv_d;           /*                         */
   status= varkon_sur_poiadr 
   (iuc,ivc,p_surpoi,nu_poi,nv_poi,
    iu_s,iv_s,iu_e,iv_e,f_p,&recno,&dflag);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur299 varkon_sur_poiadr failed \n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poitwist");
    return(varkon_erpush("SU2943",errbuf));
    goto undef;
    }

    p_r12 = p_surpoi + recno - 1;        /* Current SURPOI adress   */

    if ( dflag  < 0 )                    /* Check that surface pt   */
       {                                 /* is defined              */
       *p_dflag= -1;                     /* Flag for undef. pt      */
       *p_iuu  = iuc;                    /* Undef pt number         */
       *p_ivu  = ivc;                    /*                         */
       goto undef;                       /*                         */
       }                                 /*                         */

    r12       = p_r12->spt;              /* r12                     */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur299 p_r12->iu %d p_r12->iv %d iuc %d ivc %d\n",
     p_r12->iu,p_r12->iv, iuc, ivc  ); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur299 r12   %f %f %f \n",
                   r12.r_x,r12.r_y,r12.r_z ); 
  }
#endif

/*!                                                                 */
/*    Retrieve corner point P20 with derivatives from table SURPOI  */
/*                                                                 !*/

      iuc = iup + iu_d + iu_d;           /* Point address in file   */
      ivc = ivp;                         /*                         */
   status= varkon_sur_poiadr 
   (iuc,ivc,p_surpoi,nu_poi,nv_poi,
    iu_s,iv_s,iu_e,iv_e,f_p,&recno,&dflag);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur299 varkon_sur_poiadr failed \n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poitwist");
    return(varkon_erpush("SU2943",errbuf));
    goto undef;
    }

    p_r20 = p_surpoi + recno - 1;        /* Current SURPOI adress   */

    if ( dflag  < 0 )                    /* Check that surface pt   */
       {                                 /* is defined              */
       *p_dflag= -1;                     /* Flag for undef. pt      */
       *p_iuu  = iuc;                    /* Undef pt number         */
       *p_ivu  = ivc;                    /*                         */
       goto undef;                       /*                         */
       }                                 /*                         */

    r20       = p_r20->spt;              /* r20                     */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur299 p_r20->iu %d p_r20->iv %d iuc %d ivc %d\n",
     p_r20->iu,p_r20->iv, iuc, ivc  ); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur299 r20   %f %f %f \n",
                   r20.r_x,r20.r_y,r20.r_z ); 
  }
#endif

/*!                                                                 */
/*    Retrieve corner point P21 with derivatives from table SURPOI  */
/*                                                                 !*/

      iuc = iup + iu_d + iu_d;           /* Point address in file   */
      ivc = ivp + iv_d;                  /*                         */
   status= varkon_sur_poiadr 
   (iuc,ivc,p_surpoi,nu_poi,nv_poi,
    iu_s,iv_s,iu_e,iv_e,f_p,&recno,&dflag);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur299 varkon_sur_poiadr failed \n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poitwist");
    return(varkon_erpush("SU2943",errbuf));
    goto undef;
    }

    p_r21 = p_surpoi + recno - 1;        /* Current SURPOI adress   */

    if ( dflag  < 0 )                    /* Check that surface pt   */
       {                                 /* is defined              */
       *p_dflag= -1;                     /* Flag for undef. pt      */
       *p_iuu  = iuc;                    /* Undef pt number         */
       *p_ivu  = ivc;                    /*                         */
       goto undef;                       /*                         */
       }                                 /*                         */

    r21       = p_r21->spt;              /* r21                     */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur299 p_r21->iu %d p_r21->iv %d iuc %d ivc %d\n",
     p_r21->iu,p_r21->iv, iuc, ivc  ); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur299 r21   %f %f %f \n",
                   r21.r_x,r21.r_y,r21.r_z ); 
  }
#endif

/*!                                                                 */
/*    Retrieve corner point P22 with derivatives from table SURPOI  */
/*                                                                 !*/

      iuc = iup + iu_d + iu_d;           /* Point address in file   */
      ivc = ivp + iv_d + iv_d;           /*                         */
   status= varkon_sur_poiadr 
   (iuc,ivc,p_surpoi,nu_poi,nv_poi,
    iu_s,iv_s,iu_e,iv_e,f_p,&recno,&dflag);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur299 varkon_sur_poiadr failed \n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poitwist");
    return(varkon_erpush("SU2943",errbuf));
    goto undef;
    }

    p_r22 = p_surpoi + recno - 1;        /* Current SURPOI adress   */

    if ( dflag  < 0 )                    /* Check that surface pt   */
       {                                 /* is defined              */
       *p_dflag= -1;                     /* Flag for undef. pt      */
       *p_iuu  = iuc;                    /* Undef pt number         */
       *p_ivu  = ivc;                    /*                         */
       goto undef;                       /*                         */
       }                                 /*                         */

    r22       = p_r22->spt;              /* r22                     */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur299 p_r22->iu %d p_r22->iv %d iuc %d ivc %d\n",
     p_r22->iu,p_r22->iv, iuc, ivc  ); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur299 r22   %f %f %f \n",
                   r22.r_x,r22.r_y,r22.r_z ); 
  }
#endif

/*!                                                                 */
/*   Calculate twist vector directions and lengths.                 */
/*                                                                 !*/

/*!                                                                 */
/*   Tangent vector lengths equal to the chords:                    */
/*   uleng=  !r00 - r10! and vleng=  !r00 - r01!                    */
/*   ulenge= !r01 - r11! and vlenge= !r10 - r11!                    */
/*                                                                 !*/

    r11.uv_x = (r21.v_x - r01.v_x)/2.0 +
               (r12.u_x - r10.u_x)/2.0 -
       (r22.r_x-r02.r_x-r20.r_x+r00.r_x)/4.0;
    r11.uv_y = (r21.v_y - r01.v_y)/2.0 +
               (r12.u_y - r10.u_y)/2.0 -
       (r22.r_y-r02.r_y-r20.r_y+r00.r_y)/4.0;
    r11.uv_z = (r21.v_z - r01.v_z)/2.0 +
               (r12.u_z - r10.u_z)/2.0 -
       (r22.r_z-r02.r_z-r20.r_z+r00.r_z)/4.0;


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
#ifdef  TILLSVIDARE
/*!                                                                 */
/*   Degenerate patch, i.e. two corner points are equal             */
/*   Ska vara andra avst}nd .....                                   */
/*                                                                 !*/

    if ( uleng  < 0.000001 )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur299 Degenerate patch r00 = r10 not yet implemented\n"); 
#endif
sprintf(errbuf,"(r00=r10 not impl.)%%varkon_sur_poitwist");
       return(varkon_erpush("SU2993",errbuf));
       }
    if ( ulenge < 0.000001 )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur299 Degenerate patch r10 = r11 not yet implemented\n"); 
#endif
sprintf(errbuf,"(r10=r11 not impl.)%%varkon_sur_poitwist");
       return(varkon_erpush("SU2993",errbuf));
       }
    if ( vlenge < 0.000001 )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
fprintf(dbgfil(SURPAC),
"sur299 Degenerate patch r01 = r11 not yet implemented\n"); 
#endif
sprintf(errbuf,"(r01=r11 not impl.)%%varkon_sur_poitwist");
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
"sur299 GEmktf_3p failed\n"); 
#endif
sprintf(errbuf,"GEmktf_3p%%varkon_sur_poitwist");
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
       vleng = vlenge;
       }   /* End vleng = 0 */
/* TILLSVIDARE slut  */
#endif


    p_r11->spt.uv_x = r11.uv_x;
    p_r11->spt.uv_y = r11.uv_y;
    p_r11->spt.uv_z = r11.uv_z;

    p_r11->uv_l1 = 1.0;    
    p_r11->uv_l2 = 1.0;    
    p_r11->uv_l3 = 1.0;    
    p_r11->uv_l4 = 1.0;    

    if ( iup == iu_s )
       {
       p_r00->spt.uv_x = r11.uv_x;
       p_r00->spt.uv_y = r11.uv_y;
       p_r00->spt.uv_z = r11.uv_z;
       p_r00->uv_l1    = 1.0;    
       p_r00->uv_l2    = 1.0;    
       p_r00->uv_l3    = 1.0;    
       p_r00->uv_l4    = 1.0;    
       p_r01->spt.uv_x = r11.uv_x;
       p_r01->spt.uv_y = r11.uv_y;
       p_r01->spt.uv_z = r11.uv_z;
       p_r01->uv_l1    = 1.0;    
       p_r01->uv_l2    = 1.0;    
       p_r01->uv_l3    = 1.0;    
       p_r01->uv_l4    = 1.0;    
       p_r02->spt.uv_x = r11.uv_x;
       p_r02->spt.uv_y = r11.uv_y;
       p_r02->spt.uv_z = r11.uv_z;
       p_r02->uv_l1    = 1.0;    
       p_r02->uv_l2    = 1.0;    
       p_r02->uv_l3    = 1.0;    
       p_r02->uv_l4    = 1.0;    
       }

    if ( ivp == iv_s )
       {
       p_r00->spt.uv_x = r11.uv_x;
       p_r00->spt.uv_y = r11.uv_y;
       p_r00->spt.uv_z = r11.uv_z;
       p_r00->uv_l1    = 1.0;    
       p_r00->uv_l2    = 1.0;    
       p_r00->uv_l3    = 1.0;    
       p_r00->uv_l4    = 1.0;    
       p_r10->spt.uv_x = r11.uv_x;
       p_r10->spt.uv_y = r11.uv_y;
       p_r10->spt.uv_z = r11.uv_z;
       p_r10->uv_l1    = 1.0;    
       p_r10->uv_l2    = 1.0;    
       p_r10->uv_l3    = 1.0;    
       p_r10->uv_l4    = 1.0;    
       p_r20->spt.uv_x = r11.uv_x;
       p_r20->spt.uv_y = r11.uv_y;
       p_r20->spt.uv_z = r11.uv_z;
       p_r20->uv_l1    = 1.0;    
       p_r20->uv_l2    = 1.0;    
       p_r20->uv_l3    = 1.0;    
       p_r20->uv_l4    = 1.0;    
       }



    if ( iup == iu_e - 2*iu_d + eu )
       {
       p_r20->spt.uv_x = r11.uv_x;
       p_r20->spt.uv_y = r11.uv_y;
       p_r20->spt.uv_z = r11.uv_z;
       p_r20->uv_l1    = 1.0;    
       p_r20->uv_l2    = 1.0;    
       p_r20->uv_l3    = 1.0;    
       p_r20->uv_l4    = 1.0;    
       p_r21->spt.uv_x = r11.uv_x;
       p_r21->spt.uv_y = r11.uv_y;
       p_r21->spt.uv_z = r11.uv_z;
       p_r21->uv_l1    = 1.0;    
       p_r21->uv_l2    = 1.0;    
       p_r21->uv_l3    = 1.0;    
       p_r21->uv_l4    = 1.0;    
       p_r22->spt.uv_x = r11.uv_x;
       p_r22->spt.uv_y = r11.uv_y;
       p_r22->spt.uv_z = r11.uv_z;
       p_r22->uv_l1    = 1.0;    
       p_r22->uv_l2    = 1.0;    
       p_r22->uv_l3    = 1.0;    
       p_r22->uv_l4    = 1.0;    
       }



    if ( ivp == iv_e - 2*iv_d + ev )
       {
       p_r02->spt.uv_x = r11.uv_x;
       p_r02->spt.uv_y = r11.uv_y;
       p_r02->spt.uv_z = r11.uv_z;
       p_r02->uv_l1    = 1.0;    
       p_r02->uv_l2    = 1.0;    
       p_r02->uv_l3    = 1.0;    
       p_r02->uv_l4    = 1.0;    
       p_r12->spt.uv_x = r11.uv_x;
       p_r12->spt.uv_y = r11.uv_y;
       p_r12->spt.uv_z = r11.uv_z;
       p_r12->uv_l1    = 1.0;    
       p_r12->uv_l2    = 1.0;    
       p_r12->uv_l3    = 1.0;    
       p_r12->uv_l4    = 1.0;    
       p_r22->spt.uv_x = r11.uv_x;
       p_r22->spt.uv_y = r11.uv_y;
       p_r22->spt.uv_z = r11.uv_z;
       p_r22->uv_l1    = 1.0;    
       p_r22->uv_l2    = 1.0;    
       p_r22->uv_l3    = 1.0;    
       p_r22->uv_l4    = 1.0;    
       }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur299 iup %d ivp %d (End iu_e-2*iu_d+eu %d iv_e-2*iv_d+ev %d\n",
  iup,ivp, iu_e-2*iu_d+eu ,iv_e-2*iv_d+ev  );      
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"r11 Twist %f %f %f iu %d iv %d\n",
          p_r11->spt.uv_x,p_r11->spt.uv_y,p_r11->spt.uv_z,
          p_r11->iu  ,p_r11->iv   );      
}
#endif

    }                                    /* End   loop V points     */
  }                                      /* End   loop U points     */


/*!                                                                 */
/*  End  all V points ivp= iv_s to iv_e-2*iv_d+ev with step iv_d    */
/* End  all U points iup= iu_s to iu_e-2*iu_d+eu with step iu_d     */
/*                                                                 !*/

   if ( c_case > 1 ) goto nomore;

zero:; /*! Label zero: Zero twist vectors                          !*/

/*!                                                                 */
/* 4. Zero twist vector                                             */
/* _____________________                                            */
/*                                                                  */
/*                                                                  */
/* Loop all U points iup= iu_s to iu_e-2*iu_d+eu with step iu_d     */
/*  Loop all V points ivp= iv_s to iv_e-2*iv_d+ev with step iv_d    */
/*                                                                 !*/

for (iup=iu_s;iup<=iu_e       +eu;       /* Start loop U points     */
                        iup=iup+iu_d)    /*                         */
 {                                       /*                         */

 for (ivp=iv_s;ivp<=iv_e       +ev;      /* Start loop V points     */
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
  "sur299 varkon_sur_poiadr failed for r00\n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poitwist");
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

    p_r00->spt.uv_x = 0.0;               /* r00 twist zero          */
    p_r00->spt.uv_y = 0.0;               /*                         */
    p_r00->spt.uv_z = 0.0;               /*                         */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur299 p_r00->iu %d p_r00->iv %d iuc %d ivc %d\n",
     p_r00->iu,p_r00->iv, iuc, ivc  ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur299 r00   %f %f %f \n",
                   r00.r_x,r00.r_y,r00.r_z ); 
  }
#endif
  }                                      /* End   loop V points     */
 }                                       /* End   loop U points     */


/*!                                                                 */
/*  End  all V points ivp= iv_s to iv_e-2*iv_d+ev with step iv_d    */
/* End  all U points iup= iu_s to iu_e-2*iu_d+eu with step iu_d     */
/*                                                                 !*/

/*                                                                  */
/* n. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                  */

nomore:;/*! Label nomore: Twists are calculated                    !*/
undef:; /*! Label undef:  Requested point is undefined             !*/

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur299 Exit***varkon_sur_poitwist ** \n");
  }
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/
