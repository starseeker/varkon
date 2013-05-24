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
/*  Function: varkon_sur_readarr                   File: sur293.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function retrieves points, tangents and twist vectors       */
/*  from input arrays.                                              */
/*                                                                  */
/*  Tangents and twist vectors are considered to be undefined       */
/*  if all vectors in array are zero vectors or if the array        */
/*  pointers are NULL.                                              */
/*  In other words. If there is at least one tangent or twist       */
/*  vector that is greater than zero will the array be considered   */
/*  to be defined. Non-zero vector values will be kept and zero     */
/*  vectors will be splined (in another function)                   */
/*                                                                  */
/*  The function allocates memory for the point data. Memory that   */
/*  must be deallocated by the calling function!                    */
/*  Memory shall not be deallocated if this function fails. It      */
/*  will be done here.                                              */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-04-02   Originally written                                 */
/*  1996-02-22   malloc -> v3mall                                   */
/*  1999-12-13   Free source code modifications                     */
/*  2000-09-26   p_f_utan and p_f_vtan = 2 implemented              */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_readarr    Retrieve pts/deriv. from arrays  */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_surpoi       * Initialize SURPOI                      */
/**v3mall                  * Allocate memory                        */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_readarr         */
/* SU2993 = Severe program error (  ) in varkon_sur_readarr         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/

       DBstatus       varkon_sur_readarr (

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
/* Out:                                                             */
/*                                                                  */
  DBint    *p_f_utan,    /* Eq. +1: U tangents defined Eq. -1: Und. */
                         /* Eq. +2: Some tangents are defined       */
  DBint    *p_f_vtan,    /* Eq. +1: V tangents defined Eq. -1: Und. */
                         /* Eq. +2: Some tangents are defined       */
  DBint    *p_f_twist,   /* Eq. +1: Twists     defined Eq. -1: Und. */
                         /* Eq. +2: Some tangents are defined       */
  SURPOI  **pp_ptsout )  /* Allocated area for point data     (ptr) */
                         /* Number of records in SURPOI table       */
                         /* is np_u*np_v.                           */
/*                                                                  */
/*      Data to pp_ptsout in allocated memory area                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint   maxnum;        /* Maximum number of points (=np_u*np_v)   */
  DBint   i_u;           /* Loop index U point                      */
  DBint   i_v;           /* Loop index V point                      */
  SURPOI *p_frst;        /* First   record in table SURPOI    (ptr) */
  SURPOI *p_p;           /* Current record in table SURPOI    (ptr) */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBVector *p_array;     /* Current pointer to array          (ptr) */
  DBint     i_array;     /* Current array record number             */
  DBfloat  vec_length;   /* Length of a vector                      */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initialisations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur293 Enter***varkon_sur_readarr metod %s  np_u %d np_v %d\n" , 
                                   metod ,   np_u ,  np_v  );
fflush(dbgfil(SURPAC)); 
}
#endif
 
*pp_ptsout = NULL;                       /* Init. output pointer    */

/*!                                                                 */
/* 2. Determine if tangents and twists are defined                  */
/* _______________________________________________                  */
/*                                                                  */
/*                                                                 !*/

  *p_f_utan = -1;        /* Initialization of vector flags          */
  *p_f_vtan = -1; 
  *p_f_twist= -1;

  if ( p_utan == NULL )
    {
    *p_f_utan = -1; 
    }
  else       /* Start p_utan not equal NULL */
    {
    for ( i_u=1; i_u<= np_u;    ++i_u ) 
      {       
      for ( i_v=1; i_v<= np_v;    ++i_v ) 
        {
        p_array= p_utan+np_v*(i_u-1)+i_v-1;
        vec_length = SQRT(
         (p_array)->x_gm*(p_array)->x_gm+
         (p_array)->y_gm*(p_array)->y_gm+
         (p_array)->z_gm*(p_array)->z_gm);
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur293 U tangent %f %f %f\n",
        (p_array)->x_gm,(p_array)->y_gm,(p_array)->z_gm);
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur293 i_u %d i_v %d Tangent length %f \n", 
              i_u ,  i_v ,  vec_length);
  fflush(dbgfil(SURPAC));
  }
#endif
        if ( vec_length > 0.01 )
           {
           *p_f_utan =  1; 
           }  /* End vector is defined     */
        if ( vec_length < 0.00000001 && 1 == *p_f_utan )
           {
           *p_f_utan =  2; 
           goto  not_all_u;
           }  /* End vector is defined     */
        }     /* End V points              */
      }       /* End U points              */
    }         /* End p_utan not equal NULL */
not_all_u:;
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 && *p_f_utan ==  1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur293 Tangents in U direction are     defined *p_f_utan= %d\n",
              *p_f_utan);
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1 && *p_f_utan == -1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur293 Tangents in U direction are not defined *p_f_utan= %d\n",
                 *p_f_utan);
  fflush(dbgfil(SURPAC)); 
  }
#endif

  if ( p_vtan == NULL )
    {
    *p_f_vtan = -1; 
    }
  else       /* Start p_vtan not equal NULL */
    {
    for ( i_u=1; i_u<= np_u;    ++i_u ) 
      {       
      for ( i_v=1; i_v<= np_v;    ++i_v ) 
        {
        p_array= p_vtan+np_v*(i_u-1)+i_v-1;
        vec_length = SQRT(
         (p_array)->x_gm*(p_array)->x_gm+
         (p_array)->y_gm*(p_array)->y_gm+
         (p_array)->z_gm*(p_array)->z_gm);
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur293 V tangent %f %f %f\n",(p_array)->x_gm,
                 (p_array)->y_gm,(p_array)->z_gm);
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur293 i_u %d i_v %d Tangent length %f \n",i_u ,i_v ,vec_length);
  fflush(dbgfil(SURPAC)); 
  }
#endif
        if ( vec_length > 0.01 )
           {
           *p_f_vtan =  1; 
           }  /* End vector is defined     */
        if ( vec_length < 0.00000001 && 1 == *p_f_vtan )
           {
           *p_f_vtan =  2; 
           goto  not_all_v;
           }  /* End vector is defined     */
        }     /* End V points              */
      }       /* End U points              */
    }         /* End p_vtan not equal NULL */

not_all_v:;

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 && *p_f_vtan ==  1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur293 Tangents in V direction are     defined *p_f_vtan= %d\n",
                    *p_f_vtan);
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1 && *p_f_vtan == -1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur293 Tangents in V direction are not defined *p_f_vtan= %d\n",
                    *p_f_vtan);
  fflush(dbgfil(SURPAC)); 
  }
#endif


  if ( p_twist == NULL )
    {
    *p_f_twist= -1; 
    }
  else       /* Start p_twist not equal NULL */
    {
    for ( i_u=1; i_u<= np_u;    ++i_u ) 
      {       
      for ( i_v=1; i_v<= np_v;    ++i_v ) 
        {
        p_array= p_twist+np_v*(i_u-1)+i_v-1;
        vec_length = SQRT(
         (p_array)->x_gm*(p_array)->x_gm+
         (p_array)->y_gm*(p_array)->y_gm+
         (p_array)->z_gm*(p_array)->z_gm);
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur293 Twist     %f %f %f\n",(p_array)->x_gm,(p_array)->y_gm,
                  (p_array)->z_gm);
  fflush(dbgfil(SURPAC)); 
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur293 i_u %d i_v %d Tangent length %f \n",i_u,i_v,vec_length);
  fflush(dbgfil(SURPAC));
  }
#endif
        if ( vec_length > 0.01 )
           {
           *p_f_twist=  1; 
           }  /* End vector is defined     */
        if ( vec_length < 0.00000001 && 1 == *p_f_twist )
           {
           *p_f_twist =  2; 
           goto  not_all_w;
           }  /* End vector is defined     */
        }     /* End V points              */
      }       /* End U points              */
    }         /* End p_twist not equal NULL*/
not_all_w:;

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 && *p_f_twist  ==  1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur293 Twist vectors are defined *p_f_twist = %d\n", *p_f_twist );
  fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 && *p_f_twist  == -1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur293 Twist vectors are not defined *p_f_twist = %d\n", 
                     *p_f_twist );
  fflush(dbgfil(SURPAC)); 
  }
#endif


/*!                                                                 */
/* 3. Allocate memory area for output points                        */
/* _________________________________________                        */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Dynamic allocation of area (pp_ptsout) for the point data.       */
/* Call of v3mall.                                                  */
/*                                                                 !*/

   maxnum = np_u*np_v;

if((p_frst=(SURPOI *)
       v3mall((unsigned)(maxnum*sizeof(SURPOI)),"sur923"))==NULL) 
 {                                       /* memory for SURPOI       */
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur293 Allocation failed  np_u %d np_v %d maxnum %d\n",
    np_u, np_v, maxnum );
  }
#endif
 sprintf(errbuf, "(alloc)%%varkon_sur_readarr  ");
 return(varkon_erpush("SU2993",errbuf));
 }

*pp_ptsout = p_frst;                     /* Output pointer          */


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur293 Table SURPOI memory allocation for np_u*np_v %d pts\n",
    maxnum);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur293 Ptr to first point p_frst  = %d in table SURPOI\n",
    p_frst );
  fflush(dbgfil(SURPAC)); 
  }
#endif

/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/

/*!                                                                 */
/* 4. Initialise data in table SURPOI                               */
/* __________________________________                               */
/*                                                                  */
/* Let all point addresses ( i_u, i_v) = ( -1 , -1 )                */
/* Calls of varkon_ini_surpoi (sur171).                             */
/*                                                                 !*/

for ( i_u=1; i_u<= np_u;    ++i_u )      /* Start loop U pts        */
  {                                      /*                         */
  for ( i_v=1; i_v<= np_v;    ++i_v )    /* Start loop V pts        */
      {                                  /*                         */
      p_p=p_frst+(i_u- 1)*np_v+i_v- 1;   /* Ptr to  current point   */
      varkon_ini_surpoi (p_p);
      }                                  /* End   loop V pts        */
  }                                      /* End   loop U pts        */

/*!                                                                 */
/*!                                                                 */
/* 5. Retrieve point data and write in table SURPOI                 */
/* ________________________________________________                 */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Loop all U points i_u= 1,2,....,np_u                             */
/*   Loop all V points i_v= 1,2,....,np_v                           */
/*                                                                 !*/

for ( i_u=1; i_u<=np_u; ++i_u )
    {
      for ( i_v=1; i_v<=np_v; ++i_v )
        {
/*!                                                                 */
/*    Pointer to current pt record p_p= p_first+(i_u-1)*np_v+i_v-1  */
/*                                                                 !*/

      p_p=p_frst+(i_u-1)*np_v+i_v-1;     /* Ptr to  current point   */


/*!                                                                 */
/*    Point address iu,iv to table SURPOI                           */
/*                                                                 !*/

      p_p->iu     = i_u;                  /* Point number           */
      p_p->iv     = i_v;                  /*                        */

/*!                                                                 */
/*    Point               to table SURPOI                           */
/*                                                                 !*/
      i_array= np_v*(i_u-1)+i_v-1;          /* Current array record */
      p_array= p_poi +np_v*(i_u-1)+i_v-1;   /* Current array ptr    */
      p_p->spt.r_x= p_array->x_gm;          /* Point                */
      p_p->spt.r_y= p_array->y_gm;       
      p_p->spt.r_z= p_array->z_gm;      

/*!                                                                 */
/*    U tangent to        to table SURPOI if flag *p_f_utan = +1    */
/*                                                                 !*/
      if ( *p_f_utan > 0 )
        {
        p_array= p_utan+np_v*(i_u-1)+i_v-1; /* Current array ptr    */
        p_p->spt.v_x= p_array->x_gm;        /* U tangent            */
        p_p->spt.v_y= p_array->y_gm;      /* Vild test Ska vara u_ */
        p_p->spt.v_z= p_array->z_gm;     
        }

/*!                                                                 */
/*    V tangent to        to table SURPOI if flag *p_f_vtan = +1    */
/*                                                                 !*/
      if ( *p_f_vtan > 0 )
        {
        p_array= p_vtan+np_v*(i_u-1)+i_v-1; /* Current array ptr    */
        p_p->spt.u_x= p_array->x_gm;        /* V tangent            */
        p_p->spt.u_y= p_array->y_gm;      /* Vild test Ska vara v_ */
        p_p->spt.u_z= p_array->z_gm;     
        }

/*!                                                                 */
/*    Twist vector        to table SURPOI if flag *p_f_twist= +1    */
/*                                                                 !*/
      if ( *p_f_twist > 0 )
        {
        p_array= p_twist+np_v*(i_u-1)+i_v-1;/* Current array ptr    */
        p_p->spt.uv_x= p_array->x_gm;       /* Twist vector         */
        p_p->spt.uv_y= p_array->y_gm;     
        p_p->spt.uv_z= p_array->z_gm;     
        }

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),
  "sur293 Point p_p->iu %d p_p->iv %d Current ptr p_p = %d SURPOI\n", 
                p_p->iu ,  p_p->iv ,                  p_p );
  fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC),"iu %d iv %d i_array %d p= (%g,%g,%g)\n",
                      i_u, i_v,
                      i_array ,  (p_poi+np_v*(i_u-1)+i_v-1)->x_gm,
                                 (p_poi+np_v*(i_u-1)+i_v-1)->y_gm,
                                 (p_poi+np_v*(i_u-1)+i_v-1)->z_gm);
  fflush(dbgfil(SURPAC)); 
  }
#endif

    }                                    /* End   loop V point      */
  }                                      /* End   loop U point      */
/*!                                                                 */
/*   End  V point             i_v= 1,2,3,....,np_v                  */
/* End  U point             i_u= 1,2,3,....,np_u                    */
/*                                                                 !*/


/*                                                                  */
/* 6. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur293 Exit***varkon_sur_readarr ** \n" );
  fflush(dbgfil(SURPAC));
  }
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/
