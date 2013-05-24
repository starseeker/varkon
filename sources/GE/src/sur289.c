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
/*  Function: varkon_sur_poispline                 File: sur289.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function calculates tangents for a SUR_SPLARR surface.      */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-04-02   Originally written                                 */
/*  1996-05-28   Eliminated compilation warnings                    */
/*  1997-06-05   Bug: No values for p_dlag for case nocalc          */
/*  1999-12-13   Free source code modifications                     */
/*  2000-09-26   Argument changed metod->s_case                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_poispline  Spline tangents for SUR_SPLARR   */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_poiadr       * Address in POISUR for point            */
/* GE809                   * Create Stiffness spline                */
/* GE807                   * Create Ferguson spline                 */
/* GE808                   * Create Chordlength spline              */
/* GE109                   * Curve   evaluation routine             */
/* varkon_erpush           * Error message to terminal              */
/* varkon_erinit           * Initiate error message stack           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_poispline       */
/* SU2363 = Splining of U tangents not yet implemented              */
/* SU2993 = Severe program error (  ) in varkon_sur_poispline       */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/

     DBstatus         varkon_sur_poispline (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBint    s_case,       /* Spline case.                            */
                         /* Eq. 1: Stiffness                        */
                         /* Eq. 2: Chord length                     */
                         /* Eq. 3: Ferguson                         */
  DBint    iu_s,         /* Start U point                           */
  DBint    iv_s,         /* Start V point                           */
  DBint    iu_d,         /* Delta U point                           */
  DBint    iv_d,         /* Delta V point                           */
  DBint    iu_e,         /* End   U point                           */
  DBint    iv_e,         /* End   V point                           */
  DBint    f_p,          /* Flag ....    Eq:-1: No Eq 1: U Eq 2: V  */
  DBint    nu_poi,       /* Number of points in U direction   (ptr) */
  DBint    nv_poi,       /* Number of points in V direction   (ptr) */
/* In and out:                                                      */
/*                                                                  */
  DBint   *p_f_utan,     /* Eq. +1: U tangents defined Eq. -1: Und. */
                         /* Eq. +2: Some tangents are defined       */
  DBint   *p_f_vtan,     /* Eq. +1: V tangents defined Eq. -1: Und. */
                         /* Eq. +2: Some tangents are defined       */
  DBint   *p_f_twist,    /* Eq. +1: Twists     defined Eq. -1: Und. */
                         /* Eq. +2: Some tangents are defined       */
/* Out:                                                             */
/*                                                                  */
  SURPOI  *p_surpoi,     /* Point table SURPOI (alloc. area)  (ptr) */
  DBint   *p_dflag,      /* Eq. 1: Pt def. Eq. -1: Not def.   (ptr) */
  DBint   *p_iuu,        /* Point number U for undefined pt   (ptr) */
  DBint   *p_ivu )       /* Point number V for undefined pt   (ptr) */

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
  DBint   recno;         /* Current record in table SURPOI          */
  DBint   dflag;         /* Eq. 1: Point defined Eq. -1: Not defined*/

  EVALS  r00;            /* Corner point U=0 V=0                    */
  EVALS  r01;            /* Corner point U=0 V=1                    */
  EVALS  r10;            /* Corner point U=1 V=0                    */
  DBint  status;         /* Error code from called function         */

  DBint  np_max;         /* Maximum number of spline points         */
  DBint  np_spline;      /* Current number of spline points         */
  DBVector poi[300];     /* Points     for geo810                   */
  DBVector tan[300];     /* Tangents   for geo810                   */
  DBCurve cur;           /* Curve                                   */
  DBSeg *p_seg;          /* Segments for spline creted by geo810    */
  DBint  i_t;            /* Loop index tangent                      */
  DBfloat tan_len;       /* Tangent vector length                   */

  SURPOI *p_p;           /* Current record in SURPOI          (ptr) */
  DBint   i_p;           /* Loop index point in SURPOI (=npv)       */
   EVALC  xyz;           /* Coordinates and derivatives for crv pt  */

  char   errbuf[80]="U"; /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

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
"sur289 Enter***varkon_sur_poispline**  p_surpoi= %d\n", p_surpoi);
}
#endif
 
   if ( *p_f_utan == 1 && *p_f_vtan == 1 )
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur289 Tangents defined *p_f_utan %d *p_f_vtan %d (*p_f_twist %d)\n", 
                             *p_f_utan ,  *p_f_vtan ,   *p_f_twist);
}
#endif
      *p_dflag= 1; 
      *p_iuu  = 0;
      *p_ivu  = 0;
      goto nocalc;
      }

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
/* Check the spline case                                            */
/*                                                                 !*/

   if         ( 0  == s_case ||
                1  == s_case ||
                2  == s_case ||
                3  == s_case      ); /* OK */
   else
     {
     sprintf(errbuf,"Spline case not OK%%varkon_sur_poispline");
     return(varkon_erpush("SU2993",errbuf));
     }


/*!                                                                 */
/* 2. Allocate memory for spline                                    */
/* _____________________________                                    */
/*                                                                  */
/*                                                                 !*/

  if ( nu_poi > nv_poi ) np_max = nu_poi;
  else                   np_max = nv_poi;
     
  p_seg  = DBcreate_segments(np_max-1);

  if ( np_max > 300 )
     {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur289 Maximum size of spline is 300. Max of nu_poi %d nv_poi %d exceeds\n",
                   nu_poi , nv_poi ); 
#endif
    sprintf(errbuf,"nu_poi or nv_poi > 300%%varkon_sur_poispline");
    return(varkon_erpush("SU2993",errbuf));
     }

   for (i_t=1;i_t<=np_max;i_t=i_t+1)     /* Start loop tangents     */
     {
     poi[i_t-1].x_gm = F_UNDEF;
     poi[i_t-1].y_gm = F_UNDEF;
     poi[i_t-1].z_gm = F_UNDEF;
     tan[i_t-1].x_gm = 0.0;
     tan[i_t-1].y_gm = 0.0;
     tan[i_t-1].z_gm = 0.0;
     }

   if ( *p_f_vtan  == 1 )
      {
      goto  utcalc;
      }
/*!                                                                 */
/* 3. Calculate V tangents                                          */
/* ______________________                                           */
/*                                                                  */
/* Goto utcalc if V tangents are calculated.                        */
/*                                                                  */
/* Loop all V points ivp= iv_s to iv_e     +eu with step iv_d       */
/*  Loop all U points iup= iu_s to iu_e-iu_d+eu with step iu_d      */
/*                                                                 !*/

for (ivp=iv_s;ivp<=iv_e     +ev;         /* Start loop V points     */
                         ivp=ivp+iv_d)   /*                         */
 {                                       /*                         */
 np_spline = 0;                          /* Initialize no spline pts*/
 for (iup=iu_s;iup<=iu_e-iu_d+eu;        /* Start loop U points     */
                        iup=iup+iu_d)    /*                         */
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
  "sur289 varkon_sur_poiadr failed for r00\n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poispline");
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
  "sur289 p_r00->iu %d p_r00->iv %d iuc %d ivc %d\n",
     p_r00->iu,p_r00->iv, iuc, ivc  ); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur289 r00   %f %f %f Spline pt %d Iso V\n",
                   r00.r_x,r00.r_y,r00.r_z , np_spline+1); 
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur289 r00 U %f %f %f \n",
                   r00.u_x,r00.u_y,r00.u_z ); 
  fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur289 r00 V %f %f %f \n",
                   r00.v_x,r00.v_y,r00.v_z ); 
  fflush(dbgfil(SURPAC));
  }
#endif




/*                                                                  */
/*    Retrieve corner point P10 with derivatives from table SURPOI  */
/*                                                                  */

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
  "sur289 varkon_sur_poiadr failed for r10\n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poispline");
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
  "sur289 p_r10->iu %d p_r10->iv %d iuc %d ivc %d\n",
     p_r10->iu,p_r10->iv, iuc, ivc  ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur289 r10   %f %f %f \n",
                   r10.r_x,r10.r_y,r10.r_z ); 
  fflush(dbgfil(SURPAC)); 
  }
#endif


    if ( np_spline > np_max - 1 )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur289 Current number of spline pts np_splin %d exceeds np_max %d\n",
                   np_spline, np_max  ); 
#endif
       sprintf(errbuf,"nu_poi or nv_poi > 300%%varkon_sur_poispline");
       return(varkon_erpush("SU2993",errbuf));
       }

/*!                                                                 */
/*    Spline point r00 to array                                     */
/*                                                                 !*/

    poi[np_spline].x_gm = r00.r_x;
    poi[np_spline].y_gm = r00.r_y;
    poi[np_spline].z_gm = r00.r_z;
    if ( *p_f_vtan  == 2 && s_case > 0 )
	{
        tan[np_spline].x_gm = r00.u_x;
        tan[np_spline].y_gm = r00.u_y;
        tan[np_spline].z_gm = r00.u_z;
	}

    np_spline = np_spline + 1;           /* An additional spline pt */


  }                                      /* End   loop U points     */
/*!                                                                 */
/*  End  all U points iup= iu_s to iu_e-iu_d+eu with step iu_d      */
/*                                                                 !*/

/*!                                                                 */
/*  Spline point r10 to array                                       */
/*                                                                 !*/

    poi[np_spline].x_gm = r10.r_x;
    poi[np_spline].y_gm = r10.r_y;
    poi[np_spline].z_gm = r10.r_z;
    if ( *p_f_vtan  == 2 && s_case > 0 )
	{
        tan[np_spline].x_gm = r10.u_x;
        tan[np_spline].y_gm = r10.u_y;
        tan[np_spline].z_gm = r10.u_z;
	}
    np_spline = np_spline + 1;           /* Start pt shall be 1     */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur289 r10   %f %f %f Spline pt %d Iso V\n",
                   r10.r_x,r10.r_y,r10.r_z , np_spline); 
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur289 Isoparameter curve V= %d to be splined has %d pts \n",ivp,np_spline);
  fflush(dbgfil(SURPAC));
  }
#endif


   cur.hed_cu.type = CURTYP;
   status = I_UNDEF;
   if         ( 0  == s_case || 1  == s_case )
   status=GE809 (poi,tan,np_spline,&cur,p_seg);
   else if    ( 2  == s_case )
   status=GE808 (poi,tan,np_spline,&cur,p_seg);
   else if    ( 3  == s_case )
   status=GE807 (poi,tan,np_spline,&cur,p_seg);
   if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur289 geo810 failed V iso\n"); 
#endif
    sprintf(errbuf,"geo810%%varkon_sur_poispline");
    return(varkon_erpush("SU2943",errbuf));
    goto undef;
    }

    for ( i_p=1; i_p<=nu_poi; ++i_p )    /* Start loop i_p = iup    */
      {
/*!                                                                 */
/*    Calculate tangent on the splined V curve.                     */
/*    Call of varkon_GE109 (GE109).                               */
/*                                                                 !*/

      xyz.evltyp   = EVC_DR;
      xyz.t_global = (DBfloat)i_p; 

   status=GE109 ((DBAny *)&cur,p_seg,&xyz);
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_sur_poispline (sur289)");
        return(varkon_erpush("SU2943",errbuf));
        }

      tan_len =  SQRT(
                 xyz.drdt.x_gm*xyz.drdt.x_gm +
                 xyz.drdt.y_gm*xyz.drdt.y_gm +
                 xyz.drdt.z_gm*xyz.drdt.z_gm );
      if ( tan_len < 0.000000001 ) 
        {
        sprintf(errbuf,"GE109 tangent= 0%%varkon_sur_poispline");
        return(varkon_erpush("SU2993",errbuf));
        }


      p_p=p_surpoi+(i_p-1)*nv_poi+ivp-1; /* Ptr to  current SURPOI  */
      p_p->spt.u_x= xyz.drdt.x_gm/tan_len;  /* Vild test .... kanske v_ ? */
      p_p->spt.u_y= xyz.drdt.y_gm/tan_len;
      p_p->spt.u_z= xyz.drdt.z_gm/tan_len;
      }                                  /* End   loop i_p = i_v    */


  }                                      /* End   loop V patches    */


/*!                                                                 */
/* End  all V points ivp= iv_s to iv_e     +ev with step iv_d       */
/*                                                                 !*/

/*!                                                                 */
/* Let V tangent flag *p_f_vtan be +1                               */
/*                                                                 !*/

   *p_f_vtan = 1;

utcalc:; /*! Label utcalc: Calculate U tangents                    !*/

   if ( *p_f_utan  == 1 )
      {
      goto  udefin;
      }
/*!                                                                 */
/* 4. Calculate U tangents                                          */
/* _______________________                                          */
/*                                                                  */
/* Goto udefin if U tangents are calculated                         */
/*                                                                  */
/* Loop all U points iup= iu_s to iu_e     +eu with step iu_d       */
/*  Loop all V points ivp= iv_s to iv_e-iv_d+ev with step iv_d      */
/*                                                                 !*/

for (iup=iu_s;iup<=iu_e     +eu;         /* Start loop U points     */
                        iup=iup+iu_d)    /*                         */
 {                                       /*                         */

 np_spline = 0;                          /* Initialize no spline pts*/

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
  "sur289 varkon_sur_poiadr failed for r00\n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poispline");
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
  "sur289 p_r00->iu %d p_r00->iv %d iuc %d ivc %d\n",
     p_r00->iu,p_r00->iv, iuc, ivc  ); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur289 r00   %f %f %f Spline pt %d\n",
                   r00.r_x,r00.r_y,r00.r_z , np_spline+1); 
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur289 r00 U %f %f %f \n",
                   r00.u_x,r00.u_y,r00.u_z ); 
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur289 r00 V %f %f %f \n",
                   r00.v_x,r00.v_y,r00.v_z ); 
  fflush(dbgfil(SURPAC)); 
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
  "sur289 varkon_sur_poiadr failed for r01\n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_poispline");
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
  "sur289 p_r01->iu %d p_r01->iv %d iuc %d ivc %d\n",
     p_r01->iu,p_r01->iv, iuc, ivc  ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur289 r01   %f %f %f \n",
                   r01.r_x,r01.r_y,r01.r_z ); 
  fflush(dbgfil(SURPAC)); 
  }
#endif

    if ( np_spline > np_max - 1 )
       {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur289 Current number of spline pts np_splin %d exceeds np_max %d\n",
                   np_spline, np_max  ); 
#endif
       sprintf(errbuf,"nu_poi or nv_poi > 300%%varkon_sur_poispline");
       return(varkon_erpush("SU2993",errbuf));
       }

/*!                                                                 */
/*    Spline point r00 to array                                     */
/*                                                                 !*/

    poi[np_spline].x_gm = r00.r_x;
    poi[np_spline].y_gm = r00.r_y;
    poi[np_spline].z_gm = r00.r_z;
    if ( *p_f_utan  == 2 && s_case > 0 )
	{
        tan[np_spline].x_gm = r00.v_x;
        tan[np_spline].y_gm = r00.v_y;
        tan[np_spline].z_gm = r00.v_z;
	}

    np_spline = np_spline + 1;           /* An additional spline pt */

    }                                    /* End   loop U patches    */
/*!                                                                 */
/*  End  all V points ivp= iv_s to iv_e-iv_d+ev with step iv_d      */
/*                                                                 !*/

/*!                                                                 */
/*  Spline point r01 to array                                       */
/*                                                                 !*/

    poi[np_spline].x_gm = r01.r_x;
    poi[np_spline].y_gm = r01.r_y;
    poi[np_spline].z_gm = r01.r_z;
    if ( *p_f_utan  == 2 && s_case > 0 )
	{
        tan[np_spline].x_gm = r01.v_x;
        tan[np_spline].y_gm = r01.v_y;
        tan[np_spline].z_gm = r01.v_z;
	}

    np_spline = np_spline + 1;           /* Start pt shall be 1     */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur289 r01   %f %f %f Spline pt %d\n",
                   r01.r_x,r01.r_y,r01.r_z , np_spline); 
  fflush(dbgfil(SURPAC)); /* To file from buffer      */
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur289 Isoparameter curve U= %d to be splined has %d pts \n",
             iup,np_spline);
  fflush(dbgfil(SURPAC)); 
  }
#endif


   cur.hed_cu.type = CURTYP;

   status = I_UNDEF;
   if         ( 0  == s_case || 1  == s_case )
   status=GE809 (poi,tan,np_spline,&cur,p_seg);
   else if    ( 2  == s_case )
   status=GE808 (poi,tan,np_spline,&cur,p_seg);
   else if    ( 3  == s_case )
   status=GE807 (poi,tan,np_spline,&cur,p_seg);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur289 geo810 failed \n"); 
#endif
    sprintf(errbuf,"geo810%%varkon_sur_poispline");
    return(varkon_erpush("SU2943",errbuf));
    goto undef;
    }

    for ( i_p=1; i_p<=nv_poi; ++i_p )    /* Start loop i_p = ivp    */
      {
/*!                                                                 */
/*    Calculate tangent on the splined V curve.                     */
/*    Call of varkon_GE109 (GE109).                               */
/*                                                                 !*/

      xyz.evltyp   = EVC_DR;
      xyz.t_global = (DBfloat)i_p; 

   status=GE109 ((DBAny *)&cur,p_seg,&xyz);
   if (status<0) 
        {
        sprintf(errbuf,"GE109%%varkon_sur_poispline (sur289)");
        return(varkon_erpush("SU2943",errbuf));
        }

      tan_len =  SQRT(
                 xyz.drdt.x_gm*xyz.drdt.x_gm +
                 xyz.drdt.y_gm*xyz.drdt.y_gm +
                 xyz.drdt.z_gm*xyz.drdt.z_gm );
      if ( tan_len < 0.000000001 ) 
        {
        sprintf(errbuf,"GE109 tangent= 0%%varkon_sur_poispline");
        return(varkon_erpush("SU2993",errbuf));
        }


      p_p=p_surpoi+(iup-1)*nv_poi+i_p-1; /* Ptr to  current SURPOI  */
      p_p->spt.v_x= xyz.drdt.x_gm/tan_len; /* Wild test .. maybe u_ ? */
      p_p->spt.v_y= xyz.drdt.y_gm/tan_len;
      p_p->spt.v_z= xyz.drdt.z_gm/tan_len;
      }                                  /* End   loop i_p = i_v    */


  }                                      /* End   loop V patches    */


/*!                                                                 */
/* End  all U points iup= iu_s to iu_e     +eu with step iu_d       */
/*                                                                 !*/

/*!                                                                 */
/* Let U tangent flag *p_f_utan be +1                               */
/*                                                                 !*/

   *p_f_utan = 1;

/*                                                                  */
/* n. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                  */

udefin:; /*! Label udefin: U tangents already defined              !*/

undef:; /*! Label undef: Requested point is undefined or geo810    !*/

/*!                                                                 */
/* Deallocate memory for spline curve                               */
/*                                                                 !*/
    DBfree_segments(p_seg);

nocalc: /*! Label nocalc: Tangents already defined. No calculation !*/

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur289 Exit***varkon_sur_poispline ** \n");
  fflush(dbgfil(SURPAC));
  }
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/
