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
/*  Function: varkon_sur_interaplan                File: sur108.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/* TODO Add a table to hold input data that define the patches      */
/*     (BBOXs) that can intersect each other. Unnecessary           */
/*     intersect calculations with triangles can be avoided         */
/*     this way.                                                    */
/*                                                                  */
/*  The function calculates the intersect lines between two         */
/*  facet surfaces, defined by table APLANE.                        */
/*                                                                  */
/*  Output are intersect points, with interpolated UV values        */
/*  which are stored in the APLANE tables.                          */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-02-10   Originally written                                 */
/*  1999-11-21   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function ---------------*/
/*                                                            */
/*sdescr varkon_sur_interaplan Intersection APLANE surfaces   */
/*                                                            */
/*------------------------------------------------------------*/

/* -------------------- Theory -------------------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_lin_2plane      * Plane/plane intersect                   */
/* varkon_bpl_barycen     * Barycentric coordinates                 */
/* varkon_erpush          * Error message to terminal               */
/* varkon_erinit          * Initiate error message stack            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2??3 = No intersection between the surfaces                    */
/* SU2943 = Called function .. failed in varkon_sur_interaplan      */
/* SU2993 = Severe program error ( ) in varkon_sur_interaplan       */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
     DBstatus   varkon_sur_interaplan (

/* ------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  APLANE  *p_asur1,      /* Approximation of surface 1        (ptr) */
  DBint    nasur1,       /* Number of records in p_asur1            */
  APLANE  *p_asur2,      /* Approximation of surface 2        (ptr) */
  DBint    nasur2,       /* Number of records in p_asur2            */
  APOINT  *p_apts,       /* Intersect R*3 and UV points       (ptr) */
  DBint   *p_napts )     /* Number of records in p_napts      (ptr) */

/* Out:                                                             */
/*     Intersection points (R*3 and U,V) to p_apts and p_napts      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   DBint  i_fac1;        /* Loop index facet in surface 1           */
   DBint  i_fac2;        /* Loop index facet in surface 2           */
   DBBplane sur1_bpl;    /* B_PLANE (facet) from  surface 1         */
   DBBplane sur2_bpl;    /* B_PLANE (facet) from  surface 2         */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBVector  ps_out;      /* Start point intersect line              */
  DBVector  pe_out;      /* End   point intersect line              */
  DBint  lin_no;         /* Requested output line                   */
                         /* Eq.  12: Common part inside polygons    */
                         /* Eq.   1: Intersect B-plane 1 polygon    */
                         /* Eq.   2: Intersect B-plane 2 polygon    */
                         /* Eq.  99: Line in unbounded planes       */
  DBint  nlin;           /* No of output lines 0 or 1               */

  DBfloat alpha;         /* Barycentric coordinate for p1           */
  DBfloat beta;          /* Barycentric coordinate for p2           */
  DBfloat gamma;         /* Barycentric coordinate for p4           */

  DBint  no_int;         /* Number of intersects                    */

  DBint   status;        /* Error code from called function         */
  char    errbuf[80];    /* String for error message fctn erpush    */
#ifdef DEBUG
  DBint  i_p;            /* Loop index record in p_apts             */
  DBfloat dist;          /* Distance between intersect points       */
#endif

/* -------------------- Flow chart ---------------------------------*/
/*                _______________________                           */
/*               !                       !                          */
/*               ! varkon_sur_interaplan !                          */
/*               !      (sur108)         !                          */
/*               !_______________________!                          */
/*       ___________________!________________________________       */
/*  ____!_____         ____!_____   ___!___   ____!_____  ___!____  */
/* !    1     !       !    2     ! !   3   ! !    4     !!   5    ! */
/*----------------------------------------------------------------- */

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

/*!                                                                 */
/* Initialize variables                                             */
/*                                                                 !*/

  lin_no      = 12;
  no_int      =  0;
  *p_napts    =  0;        

/*!                                                                 */
/* 2. Calculate intersects                                          */
/* _______________________                                          */
/*                                                                  */
/*                                                                 !*/


/*!                                                                 */
/* Loop all patches in surface 1 i_fac1=1,2,...., nasur1            */
/*                                                                 !*/

for (i_fac1=1; i_fac1<= nasur1;  ++i_fac1) /* Start loop surface 1  */
  {

/*!                                                                 */
/*   Get B-plane (facet) i_fac1 from table p_asur1 (surface 1)      */
/*                                                                 !*/

     sur1_bpl = (p_asur1+i_fac1-1)->bpl; 

     for (i_fac2=1; i_fac2<= nasur2;  ++i_fac2) /* Start   surface 2  */
       {
/*!                                                                   */
/*     Get B-plane (facet) i_fac2 from table p_asur2 (surface 2)      */
/*                                                                   !*/

       sur2_bpl = (p_asur2+i_fac2-1)->bpl; 

status=varkon_lin_2plane 
       (&sur1_bpl , &sur2_bpl , lin_no, &ps_out, &pe_out,  &nlin );
       if (  nlin == 1 && status == 0  )
         {
         ;
         }
       else
         {
         goto no_int;  
         }

#ifdef DEBUG
  dist = SQRT( (ps_out.x_gm-pe_out.x_gm) * (ps_out.x_gm-pe_out.x_gm) + 
               (ps_out.y_gm-pe_out.y_gm) * (ps_out.y_gm-pe_out.y_gm) + 
               (ps_out.z_gm-pe_out.z_gm) * (ps_out.z_gm-pe_out.z_gm) ); 
       if ( dist < 0.5 ) goto no_int;
#endif

       no_int = no_int + 1;       /* An additional record in APOINT */
       if ( no_int > APMAX )
         {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  fprintf(dbgfil(SURPAC),
  "sur178 APMAX must be increased in surint.h !!! \n"); 
#endif
    sprintf(errbuf,"APMAX too small%%varkon_sur_aplane");
    return(varkon_erpush("SU2993",errbuf));
         }


       (p_apts+no_int-1)->p1 = ps_out;      /* R*3 points to APOINT */
       (p_apts+no_int-1)->p2 = pe_out; 


status=varkon_bpl_barycen
       ( &sur1_bpl, &ps_out , &alpha, &beta, &gamma );
       if ( status < 0  )
         {
         (p_apts+no_int-1)->u1sur1 = -0.987654321;
         (p_apts+no_int-1)->v1sur1 = -0.987654321;
         }
       else
         {
         /* U,V for p1 surface 1 to APOINT */
         (p_apts+no_int-1)->u1sur1 = 
         alpha*(p_asur1+i_fac1-1)->u1bpl+ 
         beta *(p_asur1+i_fac1-1)->u2bpl+ 
         gamma*(p_asur1+i_fac1-1)->u4bpl; 
         (p_apts+no_int-1)->v1sur1 = 
         alpha*(p_asur1+i_fac1-1)->v1bpl+ 
         beta *(p_asur1+i_fac1-1)->v2bpl+ 
         gamma*(p_asur1+i_fac1-1)->v4bpl; 
         }

status=varkon_bpl_barycen
       ( &sur1_bpl, &pe_out , &alpha, &beta, &gamma );
       if ( status < 0  )
         {
         (p_apts+no_int-1)->u2sur1 = -0.987654321;
         (p_apts+no_int-1)->v2sur1 = -0.987654321;
         }
       else
         {
         /* U,V for p2 surface 1 to APOINT */
         (p_apts+no_int-1)->u2sur1 = 
         alpha*(p_asur1+i_fac1-1)->u1bpl+ 
         beta *(p_asur1+i_fac1-1)->u2bpl+ 
         gamma*(p_asur1+i_fac1-1)->u4bpl; 
         (p_apts+no_int-1)->v2sur1 = 
         alpha*(p_asur1+i_fac1-1)->v1bpl+ 
         beta *(p_asur1+i_fac1-1)->v2bpl+ 
         gamma*(p_asur1+i_fac1-1)->v4bpl; 
         }

status=varkon_bpl_barycen
       ( &sur2_bpl, &ps_out , &alpha, &beta, &gamma );
       if ( status < 0  )
         {
         (p_apts+no_int-1)->u1sur2 = -0.987654321;
         (p_apts+no_int-1)->v1sur2 = -0.987654321;
         }
       else
         {
         /* U,V for p1 surface 1 to APOINT */
         (p_apts+no_int-1)->u1sur2 = 
         alpha*(p_asur2+i_fac2-1)->u1bpl+ 
         beta *(p_asur2+i_fac2-1)->u2bpl+ 
         gamma*(p_asur2+i_fac2-1)->u4bpl; 
         (p_apts+no_int-1)->v1sur2 = 
         alpha*(p_asur2+i_fac2-1)->v1bpl+ 
         beta *(p_asur2+i_fac2-1)->v2bpl+ 
         gamma*(p_asur2+i_fac2-1)->v4bpl; 
         }

status=varkon_bpl_barycen
       ( &sur2_bpl, &pe_out , &alpha, &beta, &gamma );
       if ( status < 0  )
         {
         (p_apts+no_int-1)->u2sur2 = -0.987654321;
         (p_apts+no_int-1)->v2sur2 = -0.987654321;
         }
       else
         {
         /* U,V for p2 surface 1 to APOINT */
         (p_apts+no_int-1)->u2sur2 = 
         alpha*(p_asur2+i_fac2-1)->u1bpl+ 
         beta *(p_asur2+i_fac2-1)->u2bpl+ 
         gamma*(p_asur2+i_fac2-1)->u4bpl; 
         (p_apts+no_int-1)->v2sur2 = 
         alpha*(p_asur2+i_fac2-1)->v1bpl+ 
         beta *(p_asur2+i_fac2-1)->v2bpl+ 
         gamma*(p_asur2+i_fac2-1)->v4bpl; 
         }

       (p_apts+no_int-1)->rec_no_sur1  = (p_asur1+i_fac1-1)->rec_no;  
       (p_apts+no_int-1)->us_sur1      = (p_asur1+i_fac1-1)->us;       
       (p_apts+no_int-1)->vs_sur1      = (p_asur1+i_fac1-1)->vs;     
       (p_apts+no_int-1)->ue_sur1      = (p_asur1+i_fac1-1)->ue;     
       (p_apts+no_int-1)->ve_sur1      = (p_asur1+i_fac1-1)->ve;     

       (p_apts+no_int-1)->rec_no_sur2  = (p_asur2+i_fac2-1)->rec_no; 
       (p_apts+no_int-1)->us_sur2      = (p_asur2+i_fac2-1)->us;       
       (p_apts+no_int-1)->vs_sur2      = (p_asur2+i_fac2-1)->vs;     
       (p_apts+no_int-1)->ue_sur2      = (p_asur2+i_fac2-1)->ue;     
       (p_apts+no_int-1)->ve_sur2      = (p_asur2+i_fac2-1)->ve;     


#ifdef DEBUG
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"sur108 u1 %6.2f u2 %6.2f u4 %6.2f u1sur1 %6.2f u2sur1 %6.2f\n", 
     (p_asur1+i_fac1-1)->u1bpl, (p_asur1+i_fac1-1)->u2bpl, 
     (p_asur1+i_fac1-1)->u4bpl, 
     (p_apts+no_int-1)->u1sur1,  (p_apts+no_int-1)->u2sur1 );
fprintf(dbgfil(SURPAC),
"sur108 v1 %6.2f v2 %6.2f v4 %6.2f v1sur1 %6.2f v2sur1 %6.2f\n", 
     (p_asur1+i_fac1-1)->v1bpl, (p_asur1+i_fac1-1)->v2bpl, 
     (p_asur1+i_fac1-1)->v4bpl, 
     (p_apts+no_int-1)->v1sur1,  (p_apts+no_int-1)->v2sur1 );
}
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"sur108 u1 %6.2f u2 %6.2f u4 %6.2f u1sur2 %6.2f u2sur2 %6.2f\n", 
     (p_asur2+i_fac2-1)->u1bpl, (p_asur2+i_fac2-1)->u2bpl, 
     (p_asur2+i_fac2-1)->u4bpl, 
     (p_apts+no_int-1)->u1sur2,  (p_apts+no_int-1)->u2sur2 );
fprintf(dbgfil(SURPAC),
"sur108 v1 %6.2f v2 %6.2f v4 %6.2f v1sur2 %6.2f v2sur2 %6.2f\n", 
     (p_asur2+i_fac2-1)->v1bpl, (p_asur2+i_fac2-1)->v2bpl, 
     (p_asur2+i_fac2-1)->v4bpl, 
     (p_apts+no_int-1)->v1sur2,  (p_apts+no_int-1)->v2sur2 );
}
#endif


#ifdef DEBUG
if ( dbglev(SURPAC) == 2  )
{
fprintf(dbgfil(SURPAC),
"sur108 Current patches i_fac1= %d i_fac2= %d nlin= %d no_int= %d\n", 
                        i_fac1 ,i_fac2, nlin , no_int );
}
#endif

no_int:;  /* Label: No intersect between sur1_bpl and sur2_bpl       */


       }  /*  End loop surface 2 */

  }  /*  End loop surface 1 */


/* 6. Exit                                                          */
/* -------                                                          */

  *p_napts = no_int;

#ifdef DEBUG                             /* Debug printout          */

if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "GLOBAL GEOMETRY MODULE sursur2(); \n" );
  fprintf(dbgfil(SURPAC), "BEGINMODULE  \n" );
  }

for (i_p =1; i_p  <= *p_napts; ++i_p )    /* Start loop apts record */
{
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
" LIN_FREE(#%d, VEC(%8.2f,%8.2f,0.0),VEC(%8.2f,%8.2f,0.0):PEN=2); \n", 
  i_p, 100*(p_apts+i_p-1)->u1sur1,100*(p_apts+i_p-1)->v1sur1,
       100*(p_apts+i_p-1)->u2sur1,100*(p_apts+i_p-1)->v2sur1);
fprintf(dbgfil(SURPAC),
" LIN_FREE(#%d, VEC(%8.2f,%8.2f,0.0),VEC(%8.2f,%8.2f,0.0):PEN=4); \n", 
  i_p+400, 100*(p_apts+i_p-1)->u1sur2,100*(p_apts+i_p-1)->v1sur2,
           100*(p_apts+i_p-1)->u2sur2,100*(p_apts+i_p-1)->v2sur2);
fprintf(dbgfil(SURPAC),
" LIN_FREE(#%d, VEC(%9.1f,%9.1f,%9.1f),VEC(%9.1f,%9.1f,%9.1f):PEN=3); \n", 
  i_p+800, (p_apts+i_p-1)->p1.x_gm,(p_apts+i_p-1)->p1.y_gm,
           (p_apts+i_p-1)->p1.z_gm ,
           (p_apts+i_p-1)->p2.x_gm,(p_apts+i_p-1)->p2.y_gm,
           (p_apts+i_p-1)->p2.z_gm );
}
}
if ( dbglev(SURPAC) == 2 )
  {
  fprintf(dbgfil(SURPAC), "ENDMODULE    \n" );
  }

if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur108 Exit**varkon_sur_interaplan** APOINT records %d APMAX %d\n", 
              *p_napts , APMAX );
}
#endif

       varkon_erinit();

    return(SUCCED);

  } /* End of function */

/*********************************************************/
