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
/*  Function: varkon_sur_trim                      File: sur181.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function trims a surface.                                   */
/*                                                                  */
/*  The function corresponds to the MBS function SUR_TRIM.          */
/*                                                                  */
/*  Reverse of parameter directions, with or without trimming,      */
/*  is also made with this function.                                */
/*                                                                  */
/*  Interchange of parameter directions for CUB_SUR is also         */
/*  implemented in this function.                                   */
/*                                                                  */
/*  The function allocates memory for the surface. Memory that      */
/*  must be deallocated by the calling function!                    */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-10-22   Originally written                                 */
/*  1998-01-03   POL_SUR, P3, P5, P7, P9, P21 added                 */
/*  1998-08-22   Interchange rcase= 21 added                        */
/*  1999-12-12   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_trim       Trim a surface                   */
/*                                                              */
/*------------------------------------------------------------- */

/* -------------- Function calls (internal) ------------------------*/
/*                                                                  */
/*----------------------------------------------------------------- */

/* -- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*----------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_comptol          * Retrieve computer tolerance            */
/* varkon_sur_nopatch      * Retrieve number of patches             */
/* varkon_sur_postrim      * Trim surface, no reverse               */
/* varkon_sur_reverse      * Reverse surface                        */
/* varkon_sur_interch      * Interchange surface paramet.           */
/* varkon_ini_gmsur        * Initialize DBSurf                      */
/* DBfree_patches          * Deallocate surface memory              */
/* varkon_erinit           * Initial. error message stack           */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_trim            */
/* SU2993 = Severe program error (  ) in varkon_sur_trim            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/

         DBstatus     varkon_sur_trim (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf  *p_sur,        /* Input surface                     (ptr) */
  DBPatch *p_patin,      /* Alloc. area for topol. patch data (ptr) */
  DBfloat  s_lim[2][2],  /* Surface limitation U,V points           */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
                         /*                                         */
  DBint    tcase,        /* Trimming case:                          */
                         /* Eq.  1: Reparameterise                  */
                         /* Eq.  2: No reparameterisation           */
                         /* Eq. 11: As 1 but keep surface direction */
                         /* Eq. 12: As 2 but keep surface direction */
                         /* Eq. 69: Interchange with no trimming    */
  DBSurf  *p_surout,     /* Output surface                    (ptr) */

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
  DBint   f_trim;        /* Eq. 0: No trimming  Eq. 1: Trim surface */
  DBint   rcase;         /* Eq.  0: No reverse                      */
                         /* Eq.  1: Reverse U   Eq.  2: Reverse V   */
                         /* Eq. 12: Reverse     U and V             */
                         /* Eq. 21: Interchange U and V             */
  DBint   nu;            /* No patches in U direction input surface */
  DBint   nv;            /* No patches in V direction input surface */
  DBSurf  revsur;        /* Reversed surface                        */
  DBPatch *p_patrev;     /* Patch data for the reversed surface     */
  DBSurf  trimsur;       /* Trimmed  surface                        */
  DBPatch *p_pattrim;    /* Patch data for the trimmed  surface     */
  DBSurf  intsur;        /* Surface with interchanged parameters    */
  DBPatch *p_patint;     /* Patch data for the interchanged surface */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBint  tcase_pos;      /* Trim case is positive, i.e no reverse   */
  DBint  surtype;        /* Type CUB_SUR, RAT_SUR, or ...           */
  DBfloat  comptol;        /* Computer tolerance (accuracy)           */
  DBint  status;         /* Error code from called function         */
  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* ___________________________________________                      */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur181 Enter***varkon_sur_trim  tcase %d\n" , (short)tcase );
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif
 
/*!                                                                 */
/* Initialize pp_patout to NULL. Variable pp_patout is also a flag  */
/* to the calling function. Memory must be deallocated if           */
/* pp_patout not is equal to NULL (also for errors).                */
/*                                                                 !*/

*pp_patout = NULL; 

/*!                                                                 */
/* Check trim case                                                  */
/*                                                                 !*/

   if ( tcase == 1 || tcase == 11 )
      {
      ;
      }
   else if ( 69 == tcase )
      {
      ;
      }
   else
      {
      sprintf(errbuf, " = %d %% ", (short)tcase );
       varkon_erinit();
      return(varkon_erpush("SU2543",errbuf));
      }

/*!                                                                 */
/* Computer tolerance. Call of varkon_comptol (sur753).             */
/*                                                                 !*/

   comptol=varkon_comptol();

  f_trim  = I_UNDEF;
  rcase   = I_UNDEF;

/* Initialize surface data in revsur                                */
/* Call of varkon_ini_gmsur (sur769).                               */

    varkon_ini_gmsur (&revsur);

/* Initialize surface data in trimsur                               */
/* Call of varkon_ini_gmsur (sur769).                               */

    varkon_ini_gmsur (&trimsur);

/* Initialize surface data in intsur                                */
/* Call of varkon_ini_gmsur (sur769).                               */

    varkon_ini_gmsur (&intsur);

/* Initialize patch pointers                                        */
   p_patrev   = NULL;
   p_patint   = NULL;
   p_pattrim  = NULL;

/*!                                                                 */
/* 2. Determine if reverse/interchange and/or trim is requested     */
/* ____________________________________________________________     */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/*   Retrieve number of patches and surface type.                   */
/*   Call of varkon_sur_nopatch (sur230).                           */
/*   Check that the function can handle the surface type            */
/*                                                                 !*/

    status = varkon_sur_nopatch (p_sur, &nu, &nv, &surtype); 
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_nopatch%%varkon_sur_trim");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif


if ( surtype     ==  CUB_SUR ||
     surtype     ==  RAT_SUR ||
     surtype     ==  LFT_SUR ||
     surtype     ==  PRO_SUR ||
     surtype     ==  CON_SUR ||
     surtype     ==  POL_SUR ||
     surtype     ==   P3_SUR ||
     surtype     ==   P5_SUR ||
     surtype     ==   P7_SUR ||
     surtype     ==   P9_SUR ||
     surtype     ==  P21_SUR ||
     surtype     ==  MIX_SUR )
 ;


else if ( surtype     == NURB_SUR  )
  {
   sprintf(errbuf, "Surface trimming%% NURB_SUR (sur181)");
       varkon_erinit();
      return(varkon_erpush("SU3003",errbuf));
  }

 else
 {
   sprintf(errbuf, "Surface trimming%% type %d (sur181)", 
   (short)surtype );
       varkon_erinit();
      return(varkon_erpush("SU3003",errbuf));
 }



   if ( 69 != tcase )
     {
     /* All cases except interchange of parameters */
     rcase = 0;
     if ( s_lim[0][0] > s_lim[1][0] ) rcase = 1;
     if ( s_lim[0][1] > s_lim[1][1] ) 
       {
       if ( rcase == 1 ) rcase = 12;
       else              rcase =  2;
       }
      {
      ;
      }
     }
   else
     {
     rcase  = 21;
     if ( s_lim[0][0] > s_lim[1][0] ||
          s_lim[0][1] > s_lim[1][1]    ) 
       {
       sprintf(errbuf, "(sur181)%%");
       varkon_erinit();
       return(varkon_erpush("SU3603",errbuf));
       }
     }  /* End tcase = 69 */


  if       ( rcase   == 0 )
    {
    if ( fabs(s_lim[0][0]-   1.0       ) > comptol  ||
         fabs(s_lim[1][0]-(DBfloat)(nu+1)) > comptol  ||
         fabs(s_lim[0][1]-   1.0       ) > comptol  ||
         fabs(s_lim[1][1]-(DBfloat)(nv+1)) > comptol    )
      {
      f_trim = 1;
      }
    else
      {
      f_trim = 0;
      }
    } /* End rcase   == 0 */

  else if  ( rcase   == 1 )
    {
    if ( fabs(s_lim[1][0]-   1.0       ) > comptol  ||
         fabs(s_lim[0][0]-(DBfloat)(nu+1)) > comptol  ||
         fabs(s_lim[0][1]-   1.0       ) > comptol  ||
         fabs(s_lim[1][1]-(DBfloat)(nv+1)) > comptol    )
      {
      f_trim = 1;
      }
    else
      {
      f_trim = 0;
      }
    } /* End rcase   == 1 */

  else if  ( rcase   == 2 )
    {
    if ( fabs(s_lim[0][0]-   1.0       ) > comptol  ||
         fabs(s_lim[1][0]-(DBfloat)(nu+1)) > comptol  ||
         fabs(s_lim[1][1]-   1.0       ) > comptol  ||
         fabs(s_lim[0][1]-(DBfloat)(nv+1)) > comptol    )
      {
      f_trim = 1;
      }
    else
      {
      f_trim = 0;
      }
    } /* End rcase   == 2 */

  else if  ( rcase   == 12 )
    {
    if ( fabs(s_lim[1][0]-   1.0       ) > comptol  ||
         fabs(s_lim[0][0]-(DBfloat)(nu+1)) > comptol  ||
         fabs(s_lim[1][1]-   1.0       ) > comptol  ||
         fabs(s_lim[0][1]-(DBfloat)(nv+1)) > comptol    )
      {
      f_trim = 1;
      }
    else
      {
      f_trim = 0;
      }
    } /* End rcase   == 12 */

  else if  ( rcase   == 21 )
    {
    if ( fabs(s_lim[0][0]-   1.0       ) > comptol  ||
         fabs(s_lim[1][0]-(DBfloat)(nu+1)) > comptol  ||
         fabs(s_lim[0][1]-   1.0       ) > comptol  ||
         fabs(s_lim[1][1]-(DBfloat)(nv+1)) > comptol    )
      {
      f_trim = 1;
      }
    else
      {
      f_trim = 0;
      }
    } /* End rcase   == 21 */


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur181 f_trim %d rcase   %d \n", 
   (short)f_trim,(short)rcase  );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur181 Start U %f End U %f nu %d\n",
                s_lim[0][0],s_lim[1][0],(short)nu);
  fprintf(dbgfil(SURPAC),
  "sur181 Start V %f End V %f nv %d\n",
                s_lim[0][1],s_lim[1][1],(short)nv);
  }
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur181 nu %d nv %d surtype %d\n", 
   (short)nu,(short)nv,(short)surtype);
fflush(dbgfil(SURPAC)); 
}
#endif
 
  if  ( tcase == 11 && rcase != 0 ) 
    {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur181 Trim case %d Reverse rcase= %d changed to rcase= 0\n", 
   (short)tcase,(short)rcase);
fflush(dbgfil(SURPAC)); 
}
#endif
    rcase = 0;
    }

/*!                                                                 */
/* 3. Trim surface                                                  */
/* _______________                                                  */
/*                                                                  */
/*                                                                 !*/

  if      ( f_trim == 0 && rcase !=  0 && rcase != 21 ) goto o_rev;
  else if ( f_trim == 0 && rcase ==  0 ) goto o_copy;
  else if ( f_trim == 0 && rcase == 21 ) goto o_int;   
  else
   {
   ; /* Trim (continue) */
   }

o_copy:; /*! Label: Only copy surface                              !*/


tcase_pos = 11;

    status = varkon_sur_postrim 
    (p_sur,p_patin,s_lim,tcase_pos, &trimsur,&p_pattrim); 
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_postrim%%varkon_sur_trim");
    return(varkon_erpush("SU2943",errbuf));
    }

    V3MOME((char *)(&trimsur),(char *)(p_surout),sizeof(DBSurf));
    *pp_patout = p_pattrim;

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur181 p_pattrim %d *pp_patout %d tcase %d\n",
       (int)p_pattrim  ,(int)*pp_patout, (short)tcase );
fflush(dbgfil(SURPAC));
}
#endif


 

/*!                                                                 */
/* 4. Reverse surface                                               */
/* __________________                                               */
/*                                                                  */
/*                                                                 !*/

o_rev:;  /*! Label: Only reverse surface                           !*/




  if  ( f_trim == 0 && rcase != 0 && rcase != 21  ) 
    {
    status = varkon_sur_reverse 
    (p_sur,p_patin, rcase, &revsur ,&p_patrev); 
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_reverse%%varkon_sur_trim");
    return(varkon_erpush("SU2943",errbuf));
    }
    V3MOME((char *)(&revsur),(char *)(p_surout),sizeof(DBSurf));
    *pp_patout = p_patrev;
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur181 p_patrev  %d *pp_patout %d rcase %d f_trim %d\n",
(int)p_patrev   ,(int)*pp_patout, (short)rcase, (short)f_trim );
fflush(dbgfil(SURPAC));
}
#endif
    }
  else if  ( f_trim != 0 && rcase != 0 && rcase != 21  ) 
    {
    status = varkon_sur_reverse 
    (&trimsur,p_pattrim, rcase, &revsur ,&p_patrev); 
    if(status<0)
    {
    sprintf(errbuf,"varkon_sur_reverse%%varkon_sur_trim");
    return(varkon_erpush("SU2943",errbuf));
    }
    V3MOME((char *)(&revsur),(char *)(p_surout),sizeof(DBSurf));
    *pp_patout = p_patrev;

    /* Deallocate memory for the trimmed surface */
    DBfree_patches(&trimsur,p_pattrim);

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur181 p_patrev  %d *pp_patout %d rcase %d f_trim %d\n",
  (int)p_patrev,(int)*pp_patout,(short)rcase,(short)f_trim);
  fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur181 Memory deallocated for &trimsur %d p_pattrim %d\n",
  (int)&trimsur,(int)p_pattrim);
  fflush(dbgfil(SURPAC));
  }
#endif
    }


/*!                                                                 */
/* 5. Interchange parameters in surface                             */
/* ____________________________________                             */
/*                                                                  */
/*                                                                 !*/

o_int:;  /*! Label: Only interchange parameters in surface         !*/


  if  ( f_trim == 0 && 21 == rcase ) 
    {

    status = varkon_sur_interch 
    (p_sur,p_patin, rcase, &intsur ,&p_patint); 
    if(status<0)
      {
      sprintf(errbuf,"varkon_sur_interch%%varkon_sur_trim");
      return(varkon_erpush("SU2943",errbuf));
      }
    V3MOME((char *)(&intsur),(char *)(p_surout),sizeof(DBSurf));
    *pp_patout = p_patint;

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur181 p_patint  %d *pp_patout %d rcase %d f_trim %d\n",
(int)p_patint   ,(int)*pp_patout, (short)rcase, (short)f_trim );
fflush(dbgfil(SURPAC));
}
#endif
    }
  else if  ( f_trim != 0 && 21 ==  rcase ) 
    {

    status = varkon_sur_interch 
    (&trimsur,p_pattrim, rcase, &intsur ,&p_patint); 
    if(status<0)
      {
      sprintf(errbuf,"varkon_sur_interch%%varkon_sur_trim");
      return(varkon_erpush("SU2943",errbuf));
      }
    V3MOME((char *)(&intsur),(char *)(p_surout),sizeof(DBSurf));
    *pp_patout = p_patint;

    /* Deallocate memory for the trimmed surface */
    DBfree_patches(&trimsur,p_pattrim);

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur181 p_patint  %d *pp_patout %d rcase %d f_trim %d\n",
  (int)p_patrev,(int)*pp_patout,(short)rcase,(short)f_trim);
  fflush(dbgfil(SURPAC));
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur181 Memory deallocated for &trimsur %d p_pattrim %d\n",
  (int)&trimsur,(int)p_pattrim);
  fflush(dbgfil(SURPAC));
  }
#endif
    }


/*                                                                  */
/* 5. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur181 Exit***varkon_sur_trim p_surout->nu_su %d ->nv_su %d\n",
                 p_surout->nu_su, p_surout->nv_su );
fflush(dbgfil(SURPAC));
}
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/

