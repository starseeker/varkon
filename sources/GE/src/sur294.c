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
/*  Function: varkon_sur_splarr                    File: sur294.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function creates a surface from a grid of points,           */
/*  optionally with tangents. Input to the function is              */
/*  one array with points and one array with tangents.              */
/*                                                                  */
/*  This function corresponds to the SUR_SPLARR MBS function.       */
/*                                                                  */
/*  Tangents and twist vectors are considered to be undefined       */
/*  if all vectors in array are zero vectors or if the array        */
/*  pointers are NULL.                                              */
/*                                                                  */
/*  Order of points in input array poi:                             */
/*                                                                  */
/*   V                                                              */
/* !         np_u= 7  and  np_v= 3                                  */
/* !                                                                */
/* !                                                                */
/* !   3     6     9    12    15    18    21  -->- V isoparam 3     */
/* !                                                                */
/* !   2     5     8    11    14    17    20  -->- V isoparam 2     */
/* !                                                                */
/* !   1     4     7    10    13    16    19  -->- V isoparam 1     */
/* !                                                                */
/* !   !     !     !     !     !     !     !                        */
/* !   !     !     !     !     !     !     !                        */
/* !   v     v     v     v     v     v     v                        */
/* !   !     !     !     !     !     !     !                        */
/* !                                                                */
/* !   U     U     U     U     U     U     U                        */
/* !                                                                */
/* !   i     i     i     i     i     i     i                        */
/* !   s     s     s     s     s     s     s                        */
/* !   o     o     o     o     o     o     o                        */
/* !   p     p     p     p     p     p     p                        */
/* !   a     a     a     a     a     a     a                        */
/* !   m     m     m     m     m     m     m                        */
/* !                                                                */
/* !   1     2     3     4     5     6     7                        */
/* !                                                                */
/*  ------------------------------------------------------------> U */
/*                                                                  */
/*  The function allocates memory for the surface. Memory that      */
/*  must be deallocated by the calling function!                    */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-04-02   Originally written                                 */
/*  1995-09-15   Debug and comments                                 */
/*  1996-02-22   free -> v3free                                     */
/*  1997-02-09   Elimination of compilation warnings                */
/*  1997-06-05   Initialization local variables, Debug              */
/*  1999-12-01   Free source code modifications                     */
/*  2000-09-26   Tangent calculations with different spline types   */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_splarr     Create CUB_SUR (MBS SUR_SPLARR)  */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_ini_gmsur        * Initialize DBSurf                      */
/* varkon_ini_gmpat        * Initialize DBPatch                     */
/* varkon_ini_gmpatc       * Initialize GMPATC                      */
/* varkon_sur_readarr      * Points from arrays to SURPOI           */
/* varkon_sur_poiprint     * Printout of table     SURPOI           */
/* varkon_sur_poispli      * Spline tangents in SURPOI              */
/* varkon_sur_poitanl      * Tangent lengths                        */
/* varkon_sur_poitwist     * Twist vectors                          */
/* varkon_sur_poiadr       * Address in SURPOI for point            */
/* varkon_sur_biccre1      * Patch corner pts to CUB_PAT            */
/* varkon_sur_bound        * Bounding boxes and cones               */
/* varkon_pat_pribic       * Print bicubic patch data               */
/* v3free                  * Free allocated memory                  */
/* varkon_erinit           * Initialize error buffer                */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_splarr          */
/* SU2993 = Severe program error (  ) in varkon_sur_splarr          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/

     DBstatus         varkon_sur_splarr (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  char     *metod,       /* Method:                                 */
                         /* ZEROTWIST_1: Zero twist                 */
                         /* ADINI_1:     Adini twist calculation    */
  DBint     np_u,        /* Number of points in U direction         */
  DBint     np_v,        /* Number of points in V direction         */
  DBVector *p_poi,       /* Points                            (ptr) */
  DBVector *p_utan,      /* Tangents in U direction           (ptr) */
  DBVector *p_vtan,      /* Tangents in V direction           (ptr) */
  DBVector *p_twist,     /* Twist vectors                     (ptr) */
  DBSurf   *p_surout,    /* Output surface                    (ptr) */

/* Out:                                                             */
/*                                                                  */
  DBPatch **pp_patout )  /* Alloc. area for topol. patch data (ptr) */
/*                                                                  */
/*      Data to p_surout and pp_patout in allocated memory area     */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint   c_case;        /* Creation case                           */
                         /* Eq. 1: Zero twists                      */
                         /* Eq. 2: Adinis method                    */
  DBint   s_case;        /* Spline case.                            */
                         /* Eq. 0: Original version (stiffness)     */
                         /* Eq. 1: Stiffness                        */
                         /* Eq. 2: Chord length                     */
                         /* Eq. 3: Ferguson                         */
  DBint   iu_s;          /* Start U point                           */
  DBint   iv_s;          /* Start V point                           */
  DBint   iu_d;          /* Delta U point                           */
  DBint   iv_d;          /* Delta V point                           */
  DBint   iu_e;          /* End   U point                           */
  DBint   iv_e;          /* End   V point                           */
  DBint   f_p;           /* Flag ....    Eq:-1: No Eq 1: U Eq 2: V  */
  DBint   f_utan;        /* Eq. +1: U tangents defined Eq. -1: Und. */
                         /* Eq. +2: Some tangents are defined       */
  DBint   f_vtan;        /* Eq. +1: V tangents defined Eq. -1: Und. */
                         /* Eq. +2: Some tangents are defined       */
  DBint   f_twist;       /* Eq. +1: Twists     defined Eq. -1: Und. */
                         /* Eq. +2: Some tangents are defined       */
  SURPOI *p_surpoi;      /* Point table SURPOI (alloc. area)  (ptr) */
  DBint   nu_poi;        /* Number of points in U direction   (ptr) */
  DBint   nv_poi;        /* Number of points in V direction   (ptr) */

  DBint   nu;            /* No patches in U direction for   surface */
  DBint   nv;            /* No patches in V direction for   surface */
  DBint   eu;            /* Eq. to one if f_p= 1 else eq. to zero   */
  DBint   ev;            /* Eq. to one if f_p= 2 else eq. to zero   */
  DBint   maxnum;        /* Maximum number of patches (alloc. area) */
  DBPatch *p_frst;       /* Pointer to the first patch              */
  DBPatch *p_t;          /* Current topological patch         (ptr) */
  GMPATC *p_patcc;       /* Current geometric   patch         (ptr) */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint   i_s;           /* Loop index surface patch record         */
  DBint   iup,ivp;       /* Loop index for input limit points       */
  DBint   iuc,ivc;       /* Current address to corner point in file */
  SURPOI *p_r00;         /* Record for r00 in table SURPOI    (ptr) */
  SURPOI *p_r01;         /* Record for r01 in table SURPOI    (ptr) */
  SURPOI *p_r10;         /* Record for r10 in table SURPOI    (ptr) */
  SURPOI *p_r11;         /* Record for r11 in table SURPOI    (ptr) */
  DBint   recno;         /* Current record in table SURPOI          */
  DBint   dflag;         /* Eq. 1: Point defined Eq. -1: Not defined*/

  DBint   acc_box;       /* For sur911                              */
  DBint   iu,iv;         /* Patch address                           */
  DBfloat   comptol;       /* Computer tolerance (accuracy)           */
  char   *p_gpat;        /* Allocated area geom.  patch data  (ptr) */
  char   *p_tpat;        /* Allocated area topol. patch data  (ptr) */
  EVALS  r00;            /* Corner point U=0 V=0                    */
  EVALS  r01;            /* Corner point U=0 V=1                    */
  EVALS  r10;            /* Corner point U=1 V=0                    */
  EVALS  r11;            /* Corner point U=1 V=1                    */
  DBint  pat_no;         /* Patch number                            */
  DBint  status;         /* Error code from called function         */

  DBfloat  uleng;          /* Length of chord in U direction          */
  DBfloat  vleng;          /* Length of chord in U direction          */

  char   errbuf[80];     /* String for error message fctn erpush    */

#ifdef DEBUG
  DBint  i_p,j_p;        /* Temporarely used loop index             */
  DBint  s_record;       /* Start record for varkon_sur_poiprint    */
  DBint  e_record;       /* End   record for varkon_sur_poiprint    */
#endif

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
"sur294 Enter***varkon_sur_splarr    metod %s np_u %d np_v %d\n" , 
                                     metod, (int)np_u ,  (int)np_v );
}
if ( dbglev(SURPAC) == 1 )
{
for ( i_p=0; i_p<np_u; ++i_p )
    {
    fprintf(dbgfil(SURPAC),
         "Points which define U isoparameter curve %d\n",(int)i_p+1);
      for ( j_p=0; j_p<np_v; ++j_p )
        {
        fprintf(dbgfil(SURPAC),
           "poi(iu,iv)=poi(%d,%d)= (%g,%g,%g)\n", 
                     (int)i_p+1, (int)j_p+1,
                      (p_poi+np_v*i_p+j_p)->x_gm,
                      (p_poi+np_v*i_p+j_p)->y_gm,
                     (p_poi+np_v*i_p+j_p)->z_gm);
        if ( p_utan != NULL)
        fprintf(dbgfil(SURPAC),
         "Point number nv*iu+iv= %d Tangent utan= (%g,%g,%g)\n",
                               (int)(np_v*i_p+j_p+1),
                               (p_utan+np_v*i_p+j_p)->x_gm,
                             (p_utan+np_v*i_p+j_p)->y_gm,
                                 (p_utan+np_v*i_p+j_p)->z_gm);
        if ( p_vtan != NULL)
        fprintf(dbgfil(SURPAC),
          "Point number nv*iu+iv= %d Cross   vtan= (%g,%g,%g)\n",
                                      (int)(np_v*i_p+j_p+1),
                                  (p_vtan+np_v*i_p+j_p)->x_gm,
                                   (p_vtan+np_v*i_p+j_p)->y_gm,
                                 (p_vtan+np_v*i_p+j_p)->z_gm);
        }
    }
}


if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur294 Order of points in input array poi to function sur294:\n");
}
if ( dbglev(SURPAC) == 1 )
{
for ( i_p=0; i_p<np_u*np_v; ++i_p )
    {
    fprintf(dbgfil(SURPAC),"i_p %d p= (%g,%g,%g)\n", (int)i_p, 
                                      (p_poi+i_p)->x_gm,
                                      (p_poi+i_p)->y_gm,
                                      (p_poi+i_p)->z_gm);
    }
}

#endif
 
/*!                                                                 */
/* Initialize pp_patout to NULL. Variable pp_patout is also a flag  */
/* to the calling function. Memory must be deallocated if           */
/* pp_patout not is equal to NULL (also for errors).                */
/*                                                                 !*/

*pp_patout = NULL; 

/*!                                                                 */
/* Initialize local variables.                                      */
/*                                                                 !*/

  i_s     = I_UNDEF;
  iup     = I_UNDEF;
  ivp     = I_UNDEF;
  iuc     = I_UNDEF;
  ivc     = I_UNDEF;
  dflag   = I_UNDEF;
  recno   = I_UNDEF;
  f_utan  = I_UNDEF;
  f_vtan  = I_UNDEF;
  f_twist = I_UNDEF;
  s_case  = I_UNDEF;

/*!                                                                 */
/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

/*!                                                                 */
/* Initiate surface data in p_surout                                */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/
    varkon_ini_gmsur (p_surout);

  nu = I_UNDEF;
  nv = I_UNDEF;

/*!                                                                 */
/* Check input method                                               */
/*                                                                 !*/

   if      ( strcmp(metod,"ADINI_1")  == 0  || 
             strcmp(metod,"adini_1")  == 0    )
      {
      c_case = 2;
      s_case = 0;
      }
   else if ( strcmp(metod,"ZEROTWIST_1")  == 0  || 
             strcmp(metod,"zerotwist_1")  == 0    )
      {
      c_case = 1;
      s_case = 0;
      }
   else if   ( strcmp(metod,"ADINI_1/STIFFNESS")  == 0  || 
               strcmp(metod,"adini_1/stiffness")  == 0    )
      {
      c_case = 2;
      s_case = 1;
      }
   else if ( strcmp(metod,"ZEROTWIST_1/STIFFNESS")  == 0  || 
             strcmp(metod,"zerotwist_1/stiffness")  == 0    )
      {
      c_case = 1;
      s_case = 1;
      }
   else if   ( strcmp(metod,"ADINI_1/CHORD")  == 0  || 
               strcmp(metod,"adini_1/chord")  == 0    )
      {
      c_case = 2;
      s_case = 2;
      }
   else if ( strcmp(metod,"ZEROTWIST_1/CHORD")  == 0  || 
             strcmp(metod,"zerotwist_1/chord")  == 0    )
      {
      c_case = 1;
      s_case = 2;
      }
   else if   ( strcmp(metod,"ADINI_1/FERGUSON")  == 0  || 
               strcmp(metod,"adini_1/ferguson")  == 0    )
      {
      c_case = 2;
      s_case = 3;
      }
   else if ( strcmp(metod,"ZEROTWIST_1/FERGUSON")  == 0  || 
             strcmp(metod,"zerotwist_1/ferguson")  == 0    )
      {
      c_case = 1;
      s_case = 3;
      }
   else
      {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur294 Not a valid method metod= %s \n",metod); 
#endif
    sprintf(errbuf," %s %%varkon_sur_splarr   ",metod);
    varkon_erinit();
    return(varkon_erpush("SU2373",errbuf));
    }


/*!                                                                 */
/* Let iu_s= 1, iv_s= 1, iu_d= 1, iv_d= 1, iu_e= np_u, iv_e= np_v   */
/*                                                                 !*/

  iu_s= 1; iv_s= 1; iu_d= 1; iv_d=1; iu_e= np_u; iv_e= np_v;

/*!                                                                 */
/* Let eu= iu_d and ev=   0  for a U closed surface        (f_p=1)  */
/* Let eu=  0   and ev= iv_d for a V closed surface        (f_p=2)  */
/* Let eu=  0   and ev=  0   if surface not is closed               */
/*                                                                 !*/

  f_p= -1; /* TODO Method for closed surface           */

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
/* Let nu_poi= np_u and nv_poi= np_v                                */
/*                                                                 !*/

   nu_poi = np_u;
   nv_poi = np_v;

/*!                                                                 */
/* 2. Create table SURPOI.                                          */
/* _______________________                                          */
/*                                                                  */
/* Read points/derivatives from file, allocate memory and write     */
/* input (defined) data to point and derivative table SURPOI.       */
/* Call of varkon_sur_readarr (sur293).                             */
/*                                                                 !*/

   status= varkon_sur_readarr
(metod,np_u,np_v,p_poi,p_utan,p_vtan,p_twist,
  &f_utan,&f_vtan,&f_twist,&p_surpoi);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur294 varkon_sur_readarr failed \n"); 
#endif
    sprintf(errbuf,"varkon_sur_readarr%%varkon_sur_splarr   ");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1  )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 f_utan =  %d f_vtan %d  f_twist %d\n", 
            (int)f_utan,(int)f_vtan,(int)f_twist);
  fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 &&    f_utan ==  1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 Input tangents in U direction are defined f_utan =  %d\n", 
             (int)f_utan);
  fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 &&    f_utan == -1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 Input tangents in U direction not defined f_utan = %d\n",
                  (int)f_utan);
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1 &&    f_vtan ==  1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 Input tangents in V direction are defined f_vtan =  %d\n",
           (int)f_vtan);
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1 &&    f_vtan == -1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 Input tangents in V direction are not defined f_vtan = %d\n"
                       , (int)f_vtan);
  fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 &&    f_twist  ==  1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 Input twist vectors are defined f_twist=  %d\n", 
                       (int)f_twist );
  fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 &&    f_twist  == -1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 Input twist vectors are defined f_twist= %d\n", 
                       (int)f_twist );
  fflush(dbgfil(SURPAC));
  }
#endif

#ifdef  DEBUG      
s_record = -1;
e_record = -1;
   status= varkon_sur_poiprint
       ( nu_poi,nv_poi, p_surpoi, s_record,  e_record);
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_poiprint%%varkon_sur_splarr");
    varkon_erpush("SU2943",errbuf);
    }
#endif

/*!                                                                 */
/* 3. Calculate undefined tangents in SURPOI                        */
/* _________________________________________                        */
/*                                                                  */
/* Tangents are calculated with ...                                 */
/* Call of varkon_sur_poispli (sur289).                             */
/* Goto undef if function finds undefined point.                    */
/*                                                                 !*/

   status= varkon_sur_poispline
     (s_case, iu_s,iv_s,iu_d,iv_d,iu_e,iv_e,f_p,nu_poi,nv_poi,
       &f_utan,  &f_vtan,  &f_twist, p_surpoi, &dflag, &iuc, &ivc ); 
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur294 varkon_sur_poispli failed \n"); 
#endif
    sprintf(errbuf,"varkon_sur_poispli%%varkon_sur_splarr");
    varkon_erpush("SU2943",errbuf);
    goto undef;
    }

    if ( dflag  < 0   )                  /* Check that surface pt   */
         {                               /* is defined              */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur294 dflag= %d < 0 <==> Undefined point (from sur289) \n",
            (int)dflag); 
#endif
         goto undef;
         }

#ifdef  DEBUG
s_record = -1;
e_record = -1;
   status= varkon_sur_poiprint
       ( nu_poi,nv_poi, p_surpoi, s_record,  e_record);
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_poiprint%%varkon_sur_splarr");
    varkon_erpush("SU2943",errbuf);
    }
#endif

#ifdef WILD_TEST
  if ( f_p == -1 ) return(-123);  /* Temporary   !!!!!!  */
#endif

/*!                                                                 */
/* 3. Calculate tangent vector lengths in SURPOI                    */
/* _____________________________________________                    */
/*                                                                  */
/* Tangents are calculated with method c_case= ....                 */
/* Call of varkon_sur_poitanl (sur298).                             */
/* Goto undef if function finds undefined point.                    */
/*                                                                 !*/

   status= varkon_sur_poitanl
   (c_case,iu_s,iv_s,iu_d,iv_d,iu_e,iv_e,f_p,nu_poi,nv_poi,
    p_surpoi, &dflag, &iuc, &ivc ); 
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur294 varkon_sur_poitanl failed \n"); 
#endif
    sprintf(errbuf,"varkon_sur_poitanl%%varkon_sur_splarr");
    varkon_erpush("SU2943",errbuf);
    goto undef;
    }

    if ( dflag  < 0   )                  /* Check that surface pt   */
         {                               /* is defined              */
         goto undef;
         }

/*!                                                                 */
/* 4. Calculate twist vectors in SURPOI                             */
/* ___________________________________                              */
/*                                                                  */
/* Twists   are calculated with method c_case= ....                 */
/* Call of varkon_sur_poitwist (sur299).                            */
/* Goto undef if function finds undefined point.                    */
/*                                                                 !*/

   status= varkon_sur_poitwist
   (c_case,iu_s,iv_s,iu_d,iv_d,iu_e,iv_e,f_p,nu_poi,nv_poi,
    p_surpoi, &dflag, &iuc, &ivc ); 
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur294 varkon_sur_poitwist failed \n"); 
#endif
    sprintf(errbuf,"varkon_sur_poitwist%%varkon_sur_splarr");
    varkon_erpush("SU2943",errbuf);
    goto undef;
    }

    if ( dflag  < 0   )                  /* Check that surface pt   */
         {                               /* is defined              */
         goto undef;
         }


/*!                                                                 */
/* 5. Allocate memory area for output surface                       */
/* __________________________________________                       */
/*                                                                  */
/* Calculate the number of output patches in U and V direction      */
/* Input "limit values" iu_s, ... iv_e will also be checked         */
/*                                                                  */
/*                                                                  */
/*                                                                 !*/

nu = -1;                                 /* Initiate no U patches   */

for (iup=iu_s;iup<=iu_e;iup=iup+iu_d)    /* Start loop U points     */
 {
 nu = nu + 1;                            /* An additional U patch   */
 nv = -1;                                /* Initiate no V patches   */
 for (ivp=iv_s;ivp<=iv_e;ivp=ivp+iv_d)   /* Start loop V points     */
  {
  nv = nv + 1;                           /* An additional V patch   */
  if (iup <= nu_poi   && iup >= 0 &&     /* Check that point is     */
      ivp <= nv_poi   && ivp >= 0    )   /* within point grid limit */
        ;                                /*                         */
  else
    {                                    /*                        */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur294 Failure iup %d ivp %d nu %d nv %d\n",
     (int)iup,(int)ivp,(int)nu,(int)nv ); 
  }
#endif
 sprintf(errbuf, " %d,%d %% %d,%d",
           (int)iup ,(int)ivp ,(int)nu,(int)nv);
 return(varkon_erpush("SU2693",errbuf));
 }   /* QQQQ*/

  }
 }

/*!                                                                 */
/* Add one patch (nu= nu+1) in U for a U closed surface  (f_p=1)    */
/* Add one patch (nv= nv+1) in V for a V closed surface  (f_p=2)    */
/*                                                                 !*/

  if      ( f_p == 1 )
      {
      nu = nu+1;
      }
  else if ( f_p == 2 )
      {
      nv = nv+1;
      }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 Ptr to SURPOI table p_surpoi %d nu_poi %d nv_poi %d\n",
    (int)p_surpoi, (int)nu_poi, (int)nv_poi );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 No patches in output surf nu %d nv %d\n",
    (int)nu , (int)nv);
  }
#endif


/*!                                                                 */
/* Dynamic allocation of area (pp_pat) for the topological data.    */
/* Call of function DBcreate_patches.                               */
/*                                                                 !*/

   maxnum = nu*nv;

if((p_tpat=DBcreate_patches(TOP_PAT,maxnum))==NULL)
 { 
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 Allocation failed  nu %d nv %d maxnum %d\n",
    (int)nu, (int)nv, (int)maxnum );
  }
#endif
 sprintf(errbuf, "(alloc)%%varkon_sur_splarr");
 return(varkon_erpush("SU2993",errbuf));
 }

p_frst = (DBPatch*)p_tpat;               /* Pointer to first patch  */
*pp_patout = p_frst;                     /* Output pointer          */

/*!                                                                 */
/* Initialize patch data in pp_patout.                              */
/* Calls of varkon_ini_gmpat  (sur768).                             */
/*                                                                 !*/

   for (i_s=1; i_s<= maxnum ;i_s= i_s+1)
     {
     varkon_ini_gmpat (p_frst  +i_s-1);
     }

/*!                                                                 */
pat_no = 0;                              /* Patch number 1, 2, 3, ..*/
/*                                                                 !*/

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur294 Memory for %d patches has been allocated p_frst %d\n",
 (int)maxnum , (int)p_frst );
}
#endif

/*!                                                                 */
/* Surface type and number of patches:                              */
          p_surout->typ_su= CUB_SUR;
          p_surout->nu_su = (short)nu;  
          p_surout->nv_su = (short)nv;
/*                                                                 !*/
/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* 6. Create the bicubic surface                                    */
/* ______________________________                                   */
/*                                                                  */
/*                                                                 !*/

/* Loop all U points iup= iu_s to iu_e-iu_d+eu with step iu_d       */
/*  Loop all V points ivp= iv_s to iv_e-iv_d+ev with step iv_d      */
/*                                                                 !*/

iu = -1;                                 /* Initiate no U patches   */

for (iup=iu_s;iup<=iu_e-iu_d+eu;         /* Start loop U points     */
                        iup=iup+iu_d)    /*                         */
 {                                       /*                         */

 iu = iu + 1;                            /* An additional U patch   */
 iv = -1;                                /* Initiate no V patches   */

 for (ivp=iv_s;ivp<=iv_e-iv_d+ev;        /* Start loop V points     */
                         ivp=ivp+iv_d)   /*                         */
  {                                      /*                         */

  iv = iv + 1;                           /* An additional V patch   */

/*!                                                                 */
/*   Pointer to current patch p_t:                                  */
/*   Let pat_no= pat_no+1 (an additional patch)                     */
/*   Let p_t   = p_frst+pat_no-1 (p_frst pointer to first patch)    */
/*                                                                 !*/

     pat_no = pat_no + 1;                /* An additional patch     */
     p_t = p_frst + pat_no - 1;          /* Ptr to  current patch   */

/*!                                                                 */
/* Dynamic allocation of area for one geometric (bicubic) patch.    */
/* Call of function DBcreate_patches (pointer to patch is p_patcc). */
/*                                                                 !*/

if ( (p_gpat=DBcreate_patches(CUB_PAT, 1 ))==NULL)
 {
 sprintf(errbuf, "(allocg)%%varkon_sur_splarr");
 return(varkon_erpush("SU2993",errbuf));
 }

p_patcc= (GMPATC *)p_gpat;               /* Current geometric patch */

/*!                                                                 */
/*  Initialize patch data in p_patcc.                               */
/*  Call of varkon_ini_gmpatc (sur767).                             */
/*                                                                 !*/

    varkon_ini_gmpatc (p_patcc);

/*!                                                                 */
/*  Topological patch data to current patch p_t.                    */
/*                                                                 !*/

  p_t->styp_pat = CUB_PAT;               /* Type of secondary patch */
  p_t->spek_c   = p_gpat;                /* Secondary patch (C ptr) */
  p_t->su_pat   = 0;                     /* Topological adress      */
  p_t->sv_pat   = 0;                     /* secondary patch not def.*/
  p_t->iu_pat   = (short)(iu + 1);       /* Topological adress for  */
  p_t->iv_pat   = (short)(iv + 1);       /* current (this) patch    */
  p_t->us_pat= (DBfloat)iu+1.0+comptol;  /* Start U on geom. patch  */
  p_t->ue_pat= (DBfloat)iu+2.0-comptol;  /* End   U on geom. patch  */
  p_t->vs_pat= (DBfloat)iv+1.0+comptol;  /* Start V on geom. patch  */
  p_t->ve_pat= (DBfloat)iv+2.0-comptol;  /* End   V on geom. patch  */

  p_t->box_pat.xmin = 1.0;               /* BBOX  initiation        */
  p_t->box_pat.ymin = 2.0;               /*                         */
  p_t->box_pat.zmin = 3.0;               /*                         */
  p_t->box_pat.xmax = 4.0;               /*                         */
  p_t->box_pat.ymax = 5.0;               /*                         */
  p_t->box_pat.zmax = 6.0;               /*                         */
  p_t->box_pat.flag =  -1;               /* Not calculated          */
    
  p_t->cone_pat.xdir = 1.0;              /* BCONE initiation        */
  p_t->cone_pat.ydir = 2.0;              /*                         */
  p_t->cone_pat.zdir = 3.0;              /*                         */
  p_t->cone_pat.ang  = 4.0;              /*                         */
  p_t->cone_pat.flag =  -1;              /* Not calculated          */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 pat_no %d p_t %d p_patcc %d\n", 
           (int)pat_no, (int)p_t, (int)p_patcc ); 
  }
#endif

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
  "sur294 varkon_sur_poiadr failed for r00\n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_splarr");
    varkon_erpush("SU2943",errbuf);
    goto undef;
    }

    p_r00= p_surpoi + recno - 1;         /* Current SURPOI adress   */

    if ( dflag  < 0   )                  /* Check that surface pt   */
         {                               /* is defined              */
         goto undef;
         }

    r00       = p_r00->spt;              /* r00                     */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur294 p_r00->iu %d p_r00->iv %d iuc %d ivc %d\n",
     (int)p_r00->iu,(int)p_r00->iv, (int)iuc, (int)ivc  ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur294 r00   %f %f %f \n",
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
  "sur294 varkon_sur_poiadr failed for r01\n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_splarr");
    varkon_erpush("SU2943",errbuf);
    goto undef;
    }

    p_r01= p_surpoi + recno - 1;         /* Current SURPOI adress   */

    if ( dflag  < 0  )                   /* Check that surface pt   */
         {                               /* is defined              */
         goto undef;
         }
    r01       = p_r01->spt;              /* r01                     */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur294 p_r01->iu %d p_r01->iv %d iuc %d ivc %d\n",
     (int)p_r01->iu,(int)p_r01->iv, (int)iuc, (int)ivc  ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur294 r01   %f %f %f \n",
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
  "sur294 varkon_sur_poiadr failed for r10\n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_splarr");
    varkon_erpush("SU2943",errbuf);
    goto undef;
    }

    p_r10= p_surpoi + recno - 1;         /* Current SURPOI adress   */

    if ( dflag  < 0 )                    /* Check that surface pt   */
         {                               /* is defined              */
         goto undef;
         }

    r10       = p_r10->spt;              /* r10                     */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur294 p_r10->iu %d p_r10->iv %d iuc %d ivc %d\n",
     (int)p_r10->iu,(int)p_r10->iv, (int)iuc, (int)ivc  ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur294 r10   %f %f %f \n",
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
  "sur294 varkon_sur_poiadr failed \n"); 
#endif
    sprintf(errbuf,"varkon_sur_poiadr%%varkon_sur_splarr");
    varkon_erpush("SU2943",errbuf);
    goto undef;
    }

    p_r11= p_surpoi + recno - 1;         /* Current SURPOI adress   */

    if ( dflag  < 0 )                    /* Check that surface pt   */
         {                               /* is defined              */
         goto undef;
         }

    r11       = p_r11->spt;              /* r11                     */

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur294 p_r11->iu %d p_r11->iv %d iuc %d ivc %d\n",
     (int)p_r11->iu,(int)p_r11->iv, (int)iuc, (int)ivc  ); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "sur294 r11   %f %f %f \n",
                   r11.r_x,r11.r_y,r11.r_z ); 
  }
#endif

/*!                                                                 */
/*   Tangent vector lengths from SURPOI.                            */
/*   Let twists be zero.                                            */
/*                                                                 !*/


    uleng=SQRT((r00.r_x - r10.r_x)*(r00.r_x - r10.r_x) +
               (r00.r_y - r10.r_y)*(r00.r_y - r10.r_y) +
               (r00.r_z - r10.r_z)*(r00.r_z - r10.r_z));
    vleng=SQRT((r00.r_x - r01.r_x)*(r00.r_x - r01.r_x) +
               (r00.r_y - r01.r_y)*(r00.r_y - r01.r_y) +
               (r00.r_z - r01.r_z)*(r00.r_z - r01.r_z));

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && vleng > 0.0000001 && uleng > 0.00000001 )
  {
  fprintf(dbgfil(SURPAC), 
"sur294 r00 u_l3 %f v_l3 %f iu %d iv %d\n",
p_r00->u_l3/uleng,p_r00->v_l3/vleng,(int)p_r00->iu,(int)p_r00->iv); 
  }
if ( dbglev(SURPAC) == 1 && vleng > 0.0000001 && uleng > 0.00000001 )
  {
  fprintf(dbgfil(SURPAC), 
"sur294 r01 u_l3 %f v_l1 %f iu %d iv %d\n",
p_r01->u_l3/uleng,p_r01->v_l1/vleng,(int)p_r01->iu,(int)p_r01->iv); 
  }
if ( dbglev(SURPAC) == 1 && vleng > 0.0000001 && uleng > 0.00000001 )
  {
  fprintf(dbgfil(SURPAC), 
"sur294 r10 u_l1 %f v_l3 %f iu %d iv %d\n",
p_r10->u_l1/uleng,p_r10->v_l3/vleng,(int)p_r10->iu,(int)p_r10->iv); 
  }
if ( dbglev(SURPAC) == 1 && vleng > 0.0000001 && uleng > 0.00000001 )
  {
  fprintf(dbgfil(SURPAC), 
"sur294 r11 u_l1 %f v_l1 %f iu %d iv %d\n",
p_r11->u_l1/uleng,p_r11->v_l1/vleng,(int)p_r11->iu,(int)p_r11->iv); 
  }
#endif

/*!                                                                 */

    r00.uv_x  = p_r00->spt.uv_x;         /*                         */
    r00.uv_y  = p_r00->spt.uv_y;         /*                         */
    r00.uv_z  = p_r00->spt.uv_z;         /*                         */
    r01.uv_x  = p_r01->spt.uv_x;         /*                         */
    r01.uv_y  = p_r01->spt.uv_y;         /*                         */
    r01.uv_z  = p_r01->spt.uv_z;         /*                         */
    r10.uv_x  = p_r10->spt.uv_x;         /*                         */
    r10.uv_y  = p_r10->spt.uv_y;         /*                         */
    r10.uv_z  = p_r10->spt.uv_z;         /*                         */
    r11.uv_x  = p_r11->spt.uv_x;         /*                         */
    r11.uv_y  = p_r11->spt.uv_y;         /*                         */
    r11.uv_z  = p_r11->spt.uv_z;         /*                         */

    r00.v_x   = r00.v_x * p_r00->v_l3;   /* rv00 length is one      */
    r00.v_y   = r00.v_y * p_r00->v_l3;   /*                         */
    r00.v_z   = r00.v_z * p_r00->v_l3;   /*                         */
    r00.u_x   = r00.u_x * p_r00->u_l3;   /* ru00 length is one      */
    r00.u_y   = r00.u_y * p_r00->u_l3;   /*                         */
    r00.u_z   = r00.u_z * p_r00->u_l3;   /*                         */

    r01.v_x   = r01.v_x * p_r01->v_l1;   /* rv01 length is one      */
    r01.v_y   = r01.v_y * p_r01->v_l1;   /*                         */
    r01.v_z   = r01.v_z * p_r01->v_l1;   /*                         */
    r01.u_x   = r01.u_x * p_r01->u_l3;   /* ru01 length is one      */
    r01.u_y   = r01.u_y * p_r01->u_l3;   /*                         */
    r01.u_z   = r01.u_z * p_r01->u_l3;   /*                         */

    r10.v_x   = r10.v_x * p_r10->v_l3;   /* rv10 length is one      */
    r10.v_y   = r10.v_y * p_r10->v_l3;   /*                         */
    r10.v_z   = r10.v_z * p_r10->v_l3;   /*                         */
    r10.u_x   = r10.u_x * p_r10->u_l1;   /* ru10 length is one      */
    r10.u_y   = r10.u_y * p_r10->u_l1;   /*                         */
    r10.u_z   = r10.u_z * p_r10->u_l1;   /*                         */

    r11.v_x   = r11.v_x * p_r11->v_l1;   /* rv11 length is one      */
    r11.v_y   = r11.v_y * p_r11->v_l1;   /*                         */
    r11.v_z   = r11.v_z * p_r11->v_l1;   /*                         */
    r11.u_x   = r11.u_x * p_r11->u_l1;   /* ru11 length is one      */
    r11.u_y   = r11.u_y * p_r11->u_l1;   /*                         */
    r11.u_z   = r11.u_z * p_r11->u_l1;   /*                         */


/*!                                                                 */
/*   Convert Ferguson format to CUB_PAT format.                     */
/*   Call of varkon_sur_biccre1 (sur254).                           */
/*                                                                 !*/

   status= varkon_pat_biccre1 ( &r00,  &r10,  &r01,  &r11, p_patcc);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur294 varkon_pat_biccre1 failed \n"); 
#endif
    sprintf(errbuf,"varkon_pat_biccre1%%varkon_sur_splarr   ");
    return(varkon_erpush("SU2943",errbuf));
    }

    p_patcc->ofs_pat = 0.0;              /* Offset for the patch    */

/*!                                                                 */
/*   For Debug On: Printout of second output bicubic patch          */
/*                                                                 !*/

#ifdef DEBUG
if ( pat_no  > 1465         )
  {
    status = varkon_pat_pribic  (p_patcc); 
    if(status<0)
    {
    sprintf(errbuf,"varkon_pat_pribic%%varkon_sur_splarr   ");
    return(varkon_erpush("SU2943",errbuf));
    }
  }
#endif

    }                                    /* End   loop V patches    */
  }                                      /* End   loop U patches    */
/*!                                                                 */
/*  End  all V patches  iv=0,1,....,nv-1                            */
/* End  all U patches  iu=0,1,....,nu-1                             */
/*                                                                 !*/

/*!                                                                 */
/* 5. Calculate bounding boxes and cones                            */
/* _____________________________________                            */
/*                                                                  */
/* Bounding boxes and cones for the topological patches.            */
/* Call of varkon_sur_bound (sur911).                               */
/*                                                                 !*/

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 Calculate bounding boxes and cones p_surout %d p_frst %d\n"
         , (int)p_surout, (int)p_frst );
  }
#endif

   acc_box = 0;     /* Calculation of BBOX and BCONE                */

   status=varkon_sur_bound (p_surout ,p_frst ,acc_box );
    if (status<0) 
        {
        sprintf(errbuf,"varkon_sur_bound%%varkon_sur_splarr   ");
        return(varkon_erpush("SU2943",errbuf));
        }


/*                                                                  */
/* 6. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 Total number of patches %d \n", (int)pat_no );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 Exit***varkon_sur_splarr    ** Output nu %d nv %d type %d\n"
    , (int)p_surout->nu_su,(int)p_surout->nv_su, (int)p_surout->typ_su);
  }
#endif

/*!                                                                 */
/* Deallocate memory for point table SURPOI.                        */
/* Call of function v3free.                                         */
/*                                                                 !*/


 v3free(p_surpoi,"sur293"); /* sur293 since data is allocated there */


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 Memory for table SURPOI is deallocated (p_surpoi= %d)\n",
         (int)p_surpoi);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 Exit***varkon_sur_splarr    ** \n");
  }
#endif

    return(SUCCED);

undef:; /*! Label undef: The requested point is undefined          !*/

/*!                                                                 */
/* Deallocate memory for point table SURPOI for calculation failure */
/* Call of function v3free.                                         */
/*                                                                 !*/


 v3free(p_surpoi,"sur293"); /* sur293 since data is allocated there */


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 Undefined point. Memory for table SURPOI is deallocated \n");
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 Undefined point. iuc %d ivc %d \n",
         (int)iuc , (int)ivc );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur294 Exit***varkon_sur_splarr ** (calculation failed) dflag %d\n"
             , (int)dflag);
  }
#endif

if ( dflag < 0 )
  {
  sprintf(errbuf,"iu= %d iv= %d%%varkon_sur_splarr",(int)iuc,(int)ivc);
  varkon_erinit();
  return(varkon_erpush("SU2673",errbuf));
  }
else
  {
  sprintf(errbuf,"(... )%%varkon_sur_splarr");
  return(varkon_erpush("SU2993",errbuf));
  }


  } /* End of function */

/*********************************************************/
