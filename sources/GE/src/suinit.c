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
#include "../../IG/include/env.h"
#include "../include/GE.h"

/********************************************************************/
/*!                                                                 */
/*  Function: suinit                                 File: suinit.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Initialization function for surpac.                             */
/*                                                                  */
/*  Tolerances for surpac are defined in this start-up              */
/*  function for surpac routines.                                   */
/*                                                                  */
/*  Also the dummy routine suexit is defined in this file.          */
/*                                                                  */
/*  Note that the dimension of the array def_tol_tab is defined     */
/*  is defined in this function. Adding of the default value for    */
/*  a new tolerance will also increase the dimension of the array   */
/*                                                                  */
/*                                                                  */
/*  ToDo .......... Read idknot and cgrapg from text file !!!!!!    */
/*                  Read linecrit          from text file !!!!!!    */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */ 
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-12-09   Originally written                                 */
/*  1996-01-08   & added                                            */
/*  1998-01-11   idknot, cgraph, short descriptions                 */
/*  1998-02-28   linecrit added                                     */
/*  1999-11-20   Only long names                                    */
/*  2000-01-25   Include env.h                                      */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr suinit                Initialization fctn for surpac   */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
extern short varkon_erpush();  /* Error message                     */
extern char *v3genv();         /* Get directory name for tolerances */
extern bool  v3ftst();         /* Determine if file exists          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/* ------------ Error messages and warnings ------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */


static DBfloat def_tol_tab[] = 
{
/*!------------ Default geometry tolerances ------------------------*/
/*                                                                  */
0.0000000000001,/*  0 Computer tolerance (accuracy)        comptol  */
0.0005         ,/*  1 Coordinate end calculation criterion ctol     */
0.0000004      ,/*  2 Angle      end calculation criterion ntol     */
0.01           ,/*  3 Identical point tolerance            idpoint  */
0.001          ,/*  4 Identical angle tolerance            idangle  */
1000000.0      ,/*  5 Infinite distance criterion      infinitetol  */
0.000000001    ,/*  6 Geodesic curvature criterion     geodesictol  */
1.00           ,/*  7 Identical radius  criterion         idradius  */
0.05           ,/*  8 Graphic ctol end  criterion         cgraph    */
0.000001       ,/*  9 Identical knot    criterion         idknot    */
9000.0          /* 10 Criterion line (radius)           linecrit    */
/*                                                                  */
/*-----------------------------------------------------------------!*/
};

/*!****************** Function * suinit *****************************/
/*                                                                  */
         DBstatus          suinit()   

/******************************************************************!*/

/* ------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
/*----------------------------------------------------------------- */

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
   char path[V3PTHLEN];        /* Path name to tolerance directory  */
   FILE   *f_tol;              /* Tolerance file SURPAC.TOL   (ptr) */
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */
   char   rad[133];      /* One line in the tolerance file          */
   char   errbuf[80];    /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* Create file name $VARKON_TOL SURPAC.TOL. Call of v3genv.         */
/*                                                                 !*/

   sprintf(path,"%sSURPAC.TOL",v3genv(VARKON_TOL));

/*!                                                                 */
/* Goto nofile (use default values) if there is no file.            */
/* Call of v3fst.                                                   */
/*                                                                 !*/

   if ( v3ftst(path) )
     {
     ;
     }
   else
     {
     goto nofile;
     }

/*!                                                                 */
/* Retrieve tolerances from the file SURPAC.TOL.                    */
/* Open file (C function fopen)                                     */
/*                                                                 !*/

if ( (f_tol= fopen( path ,"r")) == NULL ) 
 {                                      
 sprintf(errbuf, "(open)%%suinit"); 
 return(varkon_erpush("SU2993",errbuf)); 
 }                                  

/*!                                                                 */
/* Read first line with description and eight tolerances            */
/* Calls of C functions fgets and sscanf.                           */
/*                                                                 !*/

  fgets(rad,132,f_tol);                /* Read description line     */

  fgets(rad,132,f_tol);                /* comptol                   */
  sscanf(rad,"%lf",&def_tol_tab[0]);
  fgets(rad,132,f_tol);                /* ctol                      */
  sscanf(rad,"%lf",&def_tol_tab[1]);
  fgets(rad,132,f_tol);                /* ntol                      */
  sscanf(rad,"%lf",&def_tol_tab[2]);
  fgets(rad,132,f_tol);                /* idpoint                   */
  sscanf(rad,"%lf",&def_tol_tab[3]);
  fgets(rad,132,f_tol);                /* idangle                   */
  sscanf(rad,"%lf",&def_tol_tab[4]);
  fgets(rad,132,f_tol);                /* infinitetol               */
  sscanf(rad,"%lf",&def_tol_tab[5]);
  fgets(rad,132,f_tol);                /* geodesictol               */
  sscanf(rad,"%lf",&def_tol_tab[6]);
  fgets(rad,132,f_tol);                /* idradius                  */
  sscanf(rad,"%lf",&def_tol_tab[7]);

/*!                                                                 */
/* Close the file (C function fclose)                               */
/*                                                                 !*/

 fclose(f_tol); 

nofile:;       /*! Label nofile: There is no SURPAC.TOL            !*/

   return(0);

}

/****************** End suinit ***************************************/

/*!****************** Function * suexit *****************************/
/*                                                                  */
         DBstatus   suexit()   

/******************************************************************!*/

{
return(0);
}

/****************** End suexit ***************************************/

/*!****************** Function * comptol ****************************/
/*                                                                  */
         DBfloat      varkon_comptol()   

/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr sur753 varkon_comptol Computer accuracy     comptol    */
/*                                                              */
/*------------------------------------------------------------- */

{
  return(def_tol_tab[0]);
}

/****************** End comptol **************************************/

/*!****************** Function * ctol *******************************/
/*                                                                  */
         DBfloat      varkon_ctol   ()   

/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr sur751 varkon_ctol    End coord. criterion  ctol       */
/*                                                              */
/*------------------------------------------------------------- */

{
  return(def_tol_tab[1]);
}

/****************** End ctol *****************************************/

/*!****************** Function * ntol *******************************/
/*                                                                  */
         DBfloat      varkon_ntol   ()   

/******************************************************************!*/

/*!                                                                 */
/*  The tolerance ntol is the end criterion for a geometry          */
/*  calculation with angle between vectors as the objective         */
/*  function.                                                       */
/*                                                                  */
/*  One example is the closest point calculation. The line          */
/*  from an external point to the surface shall be parallell        */
/*  (zero angle) to the surface normal.                             */
/*                                                                  */
/*                                                                 !*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr sur752 varkon_ntol    End angle  criterion  ntol       */
/*                                                              */
/*------------------------------------------------------------- */

{
  return(def_tol_tab[2]);
}

/****************** End ntol *****************************************/

/*!****************** Function * idpoint ****************************/
/*                                                                  */
         DBfloat      varkon_idpoint()   

/******************************************************************!*/

/*!                                                                 */
/*  The tolerance idpoint is used to determine if (calculated)      */
/*  points can be considered identical, if two curves can be        */
/*  combined to one curve, etc.                                     */
/*                                                                  */
/*  The identical points criterion is also used to determine        */
/*  the number of branches for an intersect in a surface,           */
/*  for a surface silhouette curve, etc.                            */
/*                                                                  */
/*                                                                  */
/*                                                                 !*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr sur741 varkon_idpoint Identical points      idpoint    */
/*                                                              */
/*------------------------------------------------------------- */

{
  return(def_tol_tab[3]);
}

/****************** End idpoint **************************************/

/*!****************** Function * idangle ****************************/
/*                                                                  */
         DBfloat      varkon_idangle()   

/******************************************************************!*/

/*!                                                                 */
/*  The criterion is used to determine if (calculated) vectors      */
/*  can be considered identical.                                    */
/*                                                                  */
/*                                                                 !*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr sur742 varkon_idangle Identical vectors     idangle    */
/*                                                              */
/*------------------------------------------------------------- */


{
  return(def_tol_tab[4]);
}

/****************** End idangle **************************************/

/*!****************** Function * infinitetol ************************/
/*                                                                  */
         DBfloat    varkon_infinitetol()   
/******************************************************************!*/

/*!                                                                 */
/*  Infinite distance criterion                                     */
/*                                                                 !*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr sur754 v_infinitetol  Infinite distance     infinitetol*/
/*                                                              */
/*------------------------------------------------------------- */


{
  return(def_tol_tab[5]);
}

/****************** End infinitetol **********************************/

/*!****************** Function * geodesictol ************************/
/*                                                                  */
         DBfloat      varkon_geodesictol()   

/******************************************************************!*/

/*!                                                                 */
/*  The tolerance is the end criterion for a geometry               */
/*  calculation with geodesic curvature as the objective            */
/*  function.                                                       */
/*                                                                  */
/*  One example is the calculation of geodesic, i.e a shortest      */
/*  distance curve on a surface. The geodesic curvature is          */
/*  everywhere zero for such a curve.                               */
/*                                                                  */
/*                                                                  */
/*                                                                 !*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr sur755 _geodesictol   Geodesic criterion    geodesictol*/
/*                                                              */
/*------------------------------------------------------------- */


{
  return(def_tol_tab[6]);
}

/****************** End geodesictol **********************************/


/*!****************** Function * idradius ***************************/
/*                                                                  */
         DBfloat      varkon_idradius()   

/******************************************************************!*/

/*!                                                                 */
/*  The criterion is used to determine if (calculated) curvature    */
/*  radii can be considered identical.                              */
/*                                                                  */
/*                                                                 !*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr sur743 _idradius      Curvature r. criter.  idradius   */
/*                                                              */
/*------------------------------------------------------------- */


{
  return(def_tol_tab[7]);
}

/****************** End idradius *************************************/

/*!****************** Function * idknot   ***************************/
/*                                                                  */
         DBfloat      varkon_idknot  ()   

/******************************************************************!*/

/*!                                                                 */
/*  The criterion is used to determine if two knot (parameter)      */
/*  values are equal.                                               */
/*                                                                  */
/*                                                                 !*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr sur744 varkon_idknot  Identical knot values idknot     */
/*                                                              */
/*------------------------------------------------------------- */

{
  return(def_tol_tab[9]);
}

/****************** End idknot ***************************************/

/*!****************** Function * cgraph *****************************/
/*                                                                  */
         DBfloat      varkon_cgraph ()   

/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr sur756 varkon_cgraph  End graphic idpoint   cgraph     */
/*                                                              */
/*------------------------------------------------------------- */

{
  return(def_tol_tab[8]);
}

/****************** End cgraph ***************************************/


/*!****************** Function * cgraph *****************************/
/*                                                                  */
         DBfloat      varkon_linecrit ()   

/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr sur745 _linecrit      Criterion for straight line      */
/*                                                              */
/*------------------------------------------------------------- */

{
  return(def_tol_tab[10]);
}

/****************** End cgraph ***************************************/


