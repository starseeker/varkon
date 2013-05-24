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
/*  Function: varkon_sur_cylind                    File: sur869.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Create a cylinder surface (SUR_CYL)                             */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-04-27   Originally written                                 */
/*  1997-04-29   Reverse, average plane                             */
/*  1997-05-10   SUCCED->status                                     */
/*  1997-05-12   Debug                                              */
/*  1997-12-19   Center of gravity sur680                           */
/*  1999-12-18   Free source code modifications                     */
/*  2007-01-10   call to varkon_ini_gmsur(), Sören L                */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_cylind     Create cylinder surface SUR_CYL  */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ruled_l          * Ruled, lofting surface                 */
/* GEmktf_ts               * Create translation matrix              */
/* GE817                   * Trim a curve                           */
/* varkon_cur_averplan     * Average plane for a curve              */
/* varkon_ini_gmcur        * Initialize DBCurve                     */
/* varkon_idpoint          * Identical points criterion             */
/* varkon_idangle          * Identical angles criterion             */
/* varkon_comptol          * Retrieve computer tolerance            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short initial();       /* Check input data and initialize    */
static short cyldir();        /* Sweeping direction for cylinder    */
static short cyltra();        /* Create translation matrix          */
static short curtra();        /* Create translated curve            */
static short curmod();        /* Reverse curve directions           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
static DBCurve  cur2;        /* Boundary curve 2                    */
static DBCurve *p_cur2;      /* Boundary curve 2              (ptr) */
static DBSeg   *p_seg2;      /* Segment data for p_cur2       (ptr) */
static DBCurve  tricur1;     /* Reversed boundary curve 1           */
static DBCurve  tricur2;     /* Reversed boundary curve 1           */
static DBSeg   *p_triseg1;   /* Segment data trimmed crv 1    (ptr) */
static DBSeg   *p_triseg2;   /* Segment data trimmed crv 2    (ptr) */
static DBVector s_dir;       /* Sweeping direction                  */
static DBTmat   tang_tra;    /* Translation matrix for section crv  */
static DBCurve *p_cur1_mod;  /* Boundary curve 1  modified    (ptr) */
static DBSeg   *p_seg1_mod;  /* Segment data for p_cur2 modif (ptr) */
static DBCurve *p_cur2_mod;  /* Boundary curve 1  modified    (ptr) */
static DBSeg   *p_seg2_mod;  /* Segment data for p_cur2 modif (ptr) */
static DBint    r_l_case;    /* Case for varkon_ruled_l (sur863)    */
static DBfloat  idpoint;     /* Identical point criterion           */
static DBfloat  idangle;     /* Identical angle criterion           */
static DBfloat  comptol;     /* Computer tolerance                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_cylind          */
/* SU2973 = Internal function () failed in varkon_sur_cylind        */
/* SU2993 = Severe program error (  ) in varkon_sur_cylind          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/

     DBstatus         varkon_sur_cylind (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve  *p_cur1,     /* Boundary curve 1                  (ptr) */
   DBSeg    *p_seg1,     /* Segment data for p_cur1           (ptr) */
   DBfloat   dist,       /* Sweeping distance                       */
   DBVector *p_dir,      /* Sweeping direction                (ptr) */
   DBint     s_case,     /* Cylinder surface case                   */
                         /* Eq.  1: Section curve is V= 0           */
                         /*         No reverse of input curve       */
                         /* Eq. -1: Section curve is V= 0           */
                         /*         Reverse of input curve          */
                         /* Eq.  2: Section curve is V= 1           */
                         /*         No reverse of input curve       */
                         /* Eq.  3: Section curve is V= 1           */
                         /*         Reverse of input curve          */
   DBSurf   *p_surout,   /* Output surface                    (ptr) */
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
/*                                                                  */
/*-----------------------------------------------------------------!*/


  short  status;         /* Error code from called function         */
  char   errbuf[80];     /* String for error message fctn erpush    */

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
"sur869 Enter***varkon_sur_cylinder s_case %d\n", (int)s_case );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur869 p_cur1 %d p_seg1 %d\n", (int)p_cur1,  (int)p_seg1 );
fflush(dbgfil(SURPAC)); 
}
#endif
 
/*!                                                                 */
/* Let pp_patout= NULL                                              */
/*                                                                 !*/
   *pp_patout = NULL;

    status= initial( dist );
    if (status<0) 
      {
#ifdef  DEBUG
      sprintf(errbuf,"initial%%sur869");
      varkon_erpush("SU2973",errbuf);
#endif
      return(status);
      }

/*!                                                                 */
/* Initialize surface data in p_sur.                                */
/* Call of varkon_ini_gmsur (sur769).                               */
/*                                                                 !*/
    varkon_ini_gmsur (p_surout);


/*!                                                                 */
/* 2. Sweeping direction                                            */
/*                                                                 !*/

    status= cyldir ( p_cur1, p_seg1, p_dir );
    if (status<0) 
      {
#ifdef DEBUG
      sprintf(errbuf,"cyldir%%sur869");
      varkon_erpush("SU2973",errbuf);
#endif
      return(status);
      }

/*!                                                                 */
/* 3. Create translation matrix                                     */
/*                                                                 !*/

    status= cyltra ( dist );
    if (status<0) 
      {
#ifdef  DEBUG
      sprintf(errbuf,"cyltra%%sur869");
      varkon_erpush("SU2973",errbuf);
#endif
      return(status);
      }

/*!                                                                 */
/* 4. Create translated section curve                               */
/*                                                                 !*/

    status= curtra ( p_cur1, p_seg1 );
    if (status<0) 
      {
      sprintf(errbuf,"curtra%%sur869");
      varkon_erpush("SU2973",errbuf);
      goto err2;
      }

/*!                                                                 */
/* 5. Create surface                                                */
/*                                                                 !*/

    status= curmod ( p_cur1, p_seg1, s_case );
    if (status<0) 
      {
      sprintf(errbuf,"curmod%%sur869");
      varkon_erpush("SU2973",errbuf);
      goto err2;
      }


/*!                                                                 */
/* 6. Create the ruled, lofted surface                              */
/*                                                                 !*/

    r_l_case = I_UNDEF; 
    status = varkon_sur_ruled_l
    ( p_cur1_mod,p_seg1_mod,p_cur2_mod,p_seg2_mod,r_l_case,
      p_surout,pp_patout);
    if (status<0) 
      {
      sprintf(errbuf,"sur863%%sur869");
      return(varkon_erpush("SU2943",errbuf));
      }



/*!                                                                 */
/* 7. Deallocation of curve memory                                  */
/*                                                                 !*/

err2:;

    if ( p_seg2    != NULL ) DBfree_segments(p_seg2);
    if ( p_triseg1 != NULL ) DBfree_segments(p_triseg1);
    if ( p_triseg2 != NULL ) DBfree_segments(p_triseg2);

#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur869 *pp_patout %d\n", (int)*pp_patout );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 && p_surout->typ_su == LFT_SUR )
{
fprintf(dbgfil(SURPAC),
"sur869 Exit No. of patches in U %d in V %d in the LFT_SUR surface\n", 
          p_surout->nu_su ,  p_surout->nv_su  );
fflush(dbgfil(SURPAC));
}
#endif

    return(status);

  } /* End of function */

/*********************************************************/





/*!********* Internal ** function ** initial ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function checks and initializes data                         */

   static short initial(dist )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBfloat  dist;        /* Sweeping distance                       */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */
  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur869*initial*Enter dist %f \n", dist);
}
#endif

    varkon_ini_gmcur (&cur2);

 p_cur2     = NULL;  
 p_seg2     = NULL; 
 p_triseg1  = NULL; 
 p_triseg2  = NULL; 
 s_dir.x_gm = F_UNDEF;
 s_dir.y_gm = F_UNDEF;
 s_dir.z_gm = F_UNDEF;
 r_l_case   = I_UNDEF;      /* Case for varkon_ruled_l (sur863)    */
 p_cur1_mod = NULL;  
 p_seg1_mod = NULL; 
 p_cur2_mod = NULL;  
 p_seg2_mod = NULL; 

/* TODO Add initialization of tang_tra !!!!!!!                     */


   idpoint   = varkon_idpoint();
   idangle   = varkon_idangle();
   comptol   = varkon_comptol();

  if ( ABS(dist) < 10.0*idpoint )
    {
    varkon_erinit();
    sprintf(errbuf,"%7.5f%%sur869*initial", 10.0*idpoint);
    return(varkon_erpush("SU8203",errbuf));
    }



#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur869*initial idpoint %10.6f idangle %6.4f comptol %12.10f\n", 
idpoint, idangle, comptol  );
fflush(dbgfil(SURPAC)); 
}
#endif

    return(SUCCED);

} /* End of function                                                */

/*********************************************************/



/*!********* Internal ** function ** cyldir  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function determines the sweeping direction for cylinder      */

   static short cyldir ( p_cur1, p_seg1, p_dir )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur1;      /* Boundary curve 1                  (ptr) */
   DBSeg   *p_seg1;      /* Segment data for p_cur1           (ptr) */
   DBVector *p_dir;      /* Sweeping direction                (ptr) */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat  p_dir_l;      /* Length of p_dir vector                  */
  DBfloat  s_dir_l;      /* Length of s_dir vector                  */
  DBint    n_add;        /* Number of points per curve segment      */
  DBfloat  pcrit;        /* Position criterion < 0 ==> system value */
  DBfloat  dcrit;        /* Derivat. criterion < 0 ==> system value */
  DBVector c_o_g;        /* Center of gravity                       */
  DBfloat  maxdev;       /* Maximum deviation curv/plane            */
  DBfloat  a_plane[4];   /* Average plane                           */

/*----------------------------------------------------------------- */
  short    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur869*cyldir*Enter *p_dir %f %f %f \n", 
            p_dir->x_gm, p_dir->y_gm, p_dir->z_gm );
}
#endif

/*!                                                                 */
/* Initializations                                                  */
/*                                                                 !*/

    p_dir_l = F_UNDEF;
    s_dir_l = F_UNDEF;

    n_add      = 5; 
    pcrit      = F_UNDEF;    
    dcrit      = F_UNDEF;   
    maxdev     = F_UNDEF; 
    c_o_g.x_gm = F_UNDEF;
    c_o_g.y_gm = F_UNDEF;
    c_o_g.z_gm = F_UNDEF;
    a_plane[0] = F_UNDEF;  
    a_plane[1] = F_UNDEF;  
    a_plane[2] = F_UNDEF;  
    a_plane[3] = F_UNDEF;  

/*!                                                                 */
/* Calculate the length of the input direction vector *p_dir        */
/*                                                                 !*/

    p_dir_l = SQRT(
    p_dir->x_gm*p_dir->x_gm + 
    p_dir->y_gm*p_dir->y_gm + 
    p_dir->z_gm*p_dir->z_gm );

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur869*cyldir* Length of input vector *p_dir %f \n", p_dir_l );
}
#endif

/*!                                                                 */
/* Let s_dir= p_dir if p_dir is defined (not a zero vector)         */
/*                                                                 !*/

    if ( p_dir_l > idpoint )
      {
      s_dir.x_gm = p_dir->x_gm/p_dir_l;
      s_dir.y_gm = p_dir->y_gm/p_dir_l;
      s_dir.z_gm = p_dir->z_gm/p_dir_l;
      goto dirdef;
      }

/*!                                                                 */
/* Retrieve curve plane                                             */
/*                                                                 !*/

  if  ( p_cur1->plank_cu == FALSE )
    {
    status = varkon_cur_averplan
       (p_cur1,p_seg1,pcrit,dcrit, n_add,&c_o_g, &maxdev, a_plane );
    if ( status < 0 )
      {
      varkon_erinit();
      sprintf(errbuf,"sur869*cyldir%%");
      return(varkon_erpush("SU8223",errbuf));
      }
    s_dir.x_gm =  a_plane[0];
    s_dir.y_gm =  a_plane[1];
    s_dir.z_gm =  a_plane[2];
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur869*cyldir* An average plane has been calculated \n");
}
#endif
    goto  dirclc;
    }


/*!                                                                 */
/* Retrieve curve plane                                             */
/*                                                                 !*/

  s_dir.x_gm =  p_cur1->csy_cu.g31;
  s_dir.y_gm =  p_cur1->csy_cu.g32;
  s_dir.z_gm =  p_cur1->csy_cu.g33;

  s_dir_l = SQRT(
    s_dir.x_gm*s_dir.x_gm + 
    s_dir.y_gm*s_dir.y_gm + 
    s_dir.z_gm*s_dir.z_gm );

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur869*cyldir* A planar curve! Length of *s_dir %f \n", s_dir_l );
}
#endif


  if ( ABS(s_dir_l-1.0) > idpoint )
    {
    sprintf(errbuf,"zaxis not 1%%sur869*cyldir");
    return(varkon_erpush("SU2993",errbuf));
    }



dirclc:; /* Label: Direction vector is calculated                   */
dirdef:; /* Label: Direction vector is defined                      */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur869*cyldir*Exit s_dir %f %f %f \n", 
            s_dir.x_gm, s_dir.y_gm, s_dir.z_gm );
}
#endif


    return(SUCCED);

} /* End of function                                                */

/*********************************************************/



/*!********* Internal ** function ** cyltra  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates the translation matrix for the section      */

   static short cyltra ( dist  )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBfloat  dist;        /* Sweeping distance                       */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */
   DBfloat  sx,sy,sz;    /* Scaling factors                         */
   DBVector origin;      /* Origin= (0,0,0)                         */
   DBVector ptransl;     /* Translation point                       */
   short    status;      /* Error code from called function         */
   char     errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur869*cyltra*Enter \n" );
}
#endif

/*!                                                                 */
/* Create the translation matrix.                                   */
/* Call of GEmktf_ts.                                               */
/*                                                                 !*/


   origin.x_gm  = 0.0;
   origin.y_gm  = 0.0;
   origin.z_gm  = 0.0;

   ptransl.x_gm = dist*s_dir.x_gm;
   ptransl.y_gm = dist*s_dir.y_gm;
   ptransl.z_gm = dist*s_dir.z_gm;

   sx = 1.0;
   sy = 1.0;
   sz = 1.0;

   status=GEmktf_ts (&origin ,&ptransl, sx, sy, sz, &tang_tra);
      if (status<0) 
         {
         sprintf(errbuf,"GEmktf_ts%%sur869*cyltra");
         return(varkon_erpush("SU2943",errbuf));
         }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur869*cyltra*Exit ptransl %f %f %f \n", 
            ptransl.x_gm, ptransl.y_gm, ptransl.z_gm );
}
#endif


    return(SUCCED);

} /* End of function                                                */

/*********************************************************/







/*!********* Internal ** function ** curtra  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function creates the translated section curve                */

   static short curtra ( p_cur1, p_seg1  )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur1;      /* Boundary curve 1                  (ptr) */
   DBSeg   *p_seg1;      /* Segment data for p_cur1           (ptr) */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */
   short    status;      /* Error code from called function         */
   char     errbuf[80];  /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur869*curtra*Enter \n" );
}
#endif

/*!                                                                 */
/* Allocate memory and create translated section curve              */
/* Call of DBcreate_segments and GEtfCurve_to_local.                */
/*                                                                 !*/

    p_seg2 = DBcreate_segments(p_cur1->ns_cu);
/*  Copy data like planar curve, .. Check if necessary, meaningful .. */
    V3MOME((char *)(p_cur1),(char *)(&cur2),sizeof(DBCurve));
    p_cur2 = &cur2;

status=GEtfCurve_to_local
      (p_cur1, p_seg1, NULL, &tang_tra, p_cur2 , p_seg2, NULL );
      if (status<0) 
         {
         sprintf(errbuf,"GEtfCurve_to_local%%sur869*curtra");
         return(varkon_erpush("SU2943",errbuf));
         }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur869*curtra*Exit \n" );
}
#endif


    return(SUCCED);

} /* End of function                                                */

/*********************************************************/



/*!********* Internal ** function ** curmod  ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* The function modifies curves with respect to case s_case         */

   static short curmod ( p_cur1, p_seg1, s_case  )
/*                                                                 !*/
/********************************************************************/

/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
   DBCurve *p_cur1;      /* Boundary curve 1                  (ptr) */
   DBSeg   *p_seg1;      /* Segment data for p_cur1           (ptr) */
   DBint    s_case;      /* Cylinder surface case                   */
                         /* Eq.  1: Section curve is V= 0           */
                         /*         No reverse of input curve       */
                         /* Eq. -1: Section curve is V= 0           */
                         /*         Reverse of input curve          */
                         /* Eq.  2: Section curve is V= 1           */
                         /*         No reverse of input curve       */
                         /* Eq.  3: Section curve is V= 1           */
                         /*         Reverse of input curve          */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*                                                                  */
  DBfloat  uglobs1;      /* Global u start value, curve 1           */
  DBfloat  uglobe1;      /* Global u end   value, curve 1           */
  DBfloat  uglobs2;      /* Global u start value, curve 2           */
  DBfloat  uglobe2;      /* Global u end   value, curve 2           */
/*----------------------------------------------------------------- */
  short    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur869*curmod*Enter \n" );
}
#endif

  if (  s_case == -1 || s_case == 3 )
    {
    p_triseg1 = DBcreate_segments(p_cur1->ns_cu);
    uglobs1   = (DBfloat)p_cur1->ns_cu+1.0;
    uglobe1   =          1.0;
   status=GE817
    ((DBAny*)p_cur1, p_seg1,&tricur1,p_triseg1,uglobs1,uglobe1);
   if (status<0) 
        {
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur869*curmod GE817 failed uglobs1 %f uglobe1 %f \n",
                         uglobs1,uglobe1);
fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"GE817%%sur870*curmod");
        return(varkon_erpush("SU2943",errbuf));
        }
    p_triseg2 = DBcreate_segments(p_cur2->ns_cu);
    uglobs2   = (DBfloat)p_cur2->ns_cu+1.0;
    uglobe2   =          1.0;

   status=GE817
    ((DBAny*)p_cur2, p_seg2,&tricur2,p_triseg2,uglobs2,uglobe2);
   if (status<0) 
        {
#ifdef  DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC), 
"sur869*curmod GE817 failed uglobs2 %f uglobe2 %f \n",
                         uglobs2,uglobe2);
fflush(dbgfil(SURPAC));
}
#endif
        sprintf(errbuf,"GE817%%sur870*curmod");
        return(varkon_erpush("SU2943",errbuf));
        }
    }



  if      (  s_case == 1 )
    {
    p_cur1_mod = p_cur1;  
    p_seg1_mod = p_seg1; 
    p_cur2_mod = p_cur2;  
    p_seg2_mod = p_seg2; 
    }
  else if (  s_case == 2 )
    {
    p_cur1_mod = p_cur2;  
    p_seg1_mod = p_seg2; 
    p_cur2_mod = p_cur1;  
    p_seg2_mod = p_seg1; 
    }
  else if (  s_case == -1 )
    {
    p_cur1_mod = &tricur1;  
    p_seg1_mod = p_triseg1; 
    p_cur2_mod = &tricur2;  
    p_seg2_mod = p_triseg2; 
    }
  else if (  s_case == 3 )
    {
    p_cur1_mod = &tricur2;  
    p_seg1_mod = p_triseg2; 
    p_cur2_mod = &tricur1;  
    p_seg2_mod = p_triseg1; 
    }

  else
    {
    varkon_erinit();
    sprintf(errbuf,"%d%%sur869*curmod", (int)s_case);
    return(varkon_erpush("SU8213",errbuf));
    }


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur869*curmod*Exit \n" );
}
#endif


    return(SUCCED);

} /* End of function                                                */

/*********************************************************/

