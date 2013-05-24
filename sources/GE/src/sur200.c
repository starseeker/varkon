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
/*  Function: varkon_sur_importtype                  File: sur200.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function reads data from a file and tries to determine      */
/*  which type of surface it is.                                    */
/*                                                                  */
/*  Author: Gunnar Liden                                            */ 
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-11-26   Originally written                                 */
/*  1996-05-28   Erase of unused variables                          */
/*  1997-02-09   Elimination of compilation warnings                */
/*  1997-04-01   rad1, rad2, rad3 --> lin1, lin2, lin3              */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_importtype Determine type of import surface */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush       * Error message to terminal                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error (  ) in varkon_sur_importtype      */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_importtype (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
  char    *infile,       /* Surface file with NMG F07 bicubic data  */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
  DBint   *p_type,       /* Type code for surface:                  */
                         /* Eq.  1: Bicubic from NMG                */
                         /* Eq.  2: Consurf from NMG                */
                         /* Eq.  3: Rational bicubic Varkon format  */
                         /* Eq. -1: Unknown type                    */
  DBint   *p_nu,         /* Number of patches in U direction        */
  DBint   *p_nv )        /* Number of patches in V direction        */
                         /* (if data is available, otherwise = 0 )  */
/*                                                                  */
/* Data to p_type                                                   */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!--- Description of headers for different surface types ---------!*/
/*                                                                  */
/*  Bicubic surface from NMG                                        */
/*  ------------------------                                        */
/*                                                                  */
/*  Surface name  .. ... .. .. .                           (Line 1) */
/*  dum1 No coeff. Patch no. dum4 Type dum6 dum7 nu-1 nv-1 (Line 2) */
/*       ... empty line (no data) ...                      (Line 3) */
/*                                                                  */
/*  NMG Consurf in section format                                   */
/*  -----------------------------                                   */
/*                                                                  */
/* LINE  MOG1/U1                                                    */
/*    SECTION  5 POINTS                                             */
/*    NORMAL  4700.0000  0.0000 2260.0000 1.00000 0.00000 0.00000   */
/*----------------------------------------------------------------- */

{ /* Start of function */

/*!New-Page--------------------------------------------------------!*/

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   char   lin1[133];     /* First  line in the file                 */
   char   lin2[133];     /* Second line in the file                 */
   char   lin3[133];     /* Third  line in the file                 */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   int    nu;            /* Number of patches in U direction        */
   int    nv;            /* Number of patches in V direction        */
   int    nmgtype;       /* NMG entity type (bicubic=1001)          */
   int    ncoeff;        /* Number of coefficients in patch (48)    */
   int    ipatch;        /* Patch number in surface 1, 2, .., ntot  */
   char   errbuf[80];    /* String for error message fctn erpush    */

   char   lin_type[132]; /* The word LINE    for Consurf section    */
   char   sec_type[132]; /* The word SECTION for Consurf section    */
   char   nor_type[132]; /* The word NORMAL  for Consurf section    */

   FILE   *f;            /* Pointer to the input ....F07 file       */

   int    dum1,dum4;     /* Dummy integers                          */
   int    dum6,dum7;     /* Dummy integers                          */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initialisations                       */
/* ______________________________________                           */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur200 Enter*varkon_sur_importtype* infile %s \n", infile );
  }
 fflush(dbgfil(SURPAC)); /* To file from buffer      */
#endif

/*!                                                                 */
/* Let output type be -1 (unknown) and number of patches 0,0        */
/*                                                                 !*/

   *p_type = -1;
   *p_nu   =  0;
   *p_nv   =  0;

/*!                                                                 */
/* 2. Retrieve first lines (header) of file                         */
/* ________________________________________                         */
/*                                                                 !*/

/*!                                                                 */
/* Open  the input file (C function fopen)                          */
/*                                                                 !*/

/* (Error SU2463 for open failure)                                  */

if ( (f= fopen(infile,"r")) == NULL )
 {
 sprintf(errbuf, "%s %%",infile);
 return(varkon_erpush("SU2463",errbuf));
 }

/*!                                                                 */
/* Read first three lines                                           */
/*                                                                 !*/

fgets(lin1,132,f);                       /* Read line 1             */
fgets(lin2,132,f);                       /* Read line 2             */
fgets(lin3,132,f);                       /* Read line 3             */


/*!                                                                 */
/* Close the input file (C function fclose)                         */
/*                                                                 !*/

 fclose(f);                              /* Close the input file    */


/*!                                                                 */
/* 3. Determine the type of input surface                           */
/* ________________________________________                         */
/*                                                                 !*/

/*!                                                                 */
/* Is it a NMG bicubic surface ?                                    */
/* Retrieve no of patches (nu,nv) and NMG surface type from line 2. */
/* The NMG surface type shall be 1001 for a bicubic surface.        */
/*                                                                 !*/

if (sscanf(lin2,"%d%d%d%d%d%d%d%d%d",    /* NMG surface type and    */
 &dum1,&ncoeff,&ipatch,&dum4,&nmgtype,   /* number of U and V lines */
 &dum6,&dum7,&nu,&nv) == EOF )           /* (patch no and no coeff) */
  {                                      /* EOF if not NMG F07      */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur200 Not a NMG F07 file with coefficients\n");
  }
 fflush(dbgfil(SURPAC)); 
#endif
  goto notnmgbic;
  }                                      /*                         */


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur200 Surface type is 1001 for a NMG bicubic\n");
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur200 NMG surface type (code)     =%d\n",(int)nmgtype);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur200 Number of coefficients (48) =%d\n",(int)ncoeff);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur200 Patch number in NMG         =%d\n",(int)ipatch);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur200 No patches in U direction nu=%d\n",(int)nu);
  fflush(dbgfil(SURPAC));
  fprintf(dbgfil(SURPAC),
  "sur200 No patches in V direction nv=%d\n",(int)nv);
  fflush(dbgfil(SURPAC));
  }
#endif


if ( nmgtype == 1001 )                   /* NMG bicubic is 1001     */
 {                                       /*                         */
 *p_type = 1;                            /* Output surface type     */
 *p_nu = nu - 1;                         /* Number of U patches nu  */
 *p_nv = nv - 1;                         /* Number of V patches nv  */
 goto nomore;                            /* End of analysis         */
 }
else
 {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur200 Not a NMG bicubic 1001 type\n");
  fflush(dbgfil(SURPAC)); 
  }
#endif
 goto notnmgbic;
 }


notnmgbic:;     /*! Label notnmgbic: Not NMG bicubic               !*/


if(sscanf(lin1,"%s",lin_type)==EOF)      /* Read the word LINE      */
   goto notnmglft;                       /*                         */
if(sscanf(lin2,"%s",sec_type)==EOF)      /* Read the word SECTION   */
   goto notnmglft;                       /*                         */
if(sscanf(lin3,"%s",nor_type)==EOF)      /* Read the word NORMAL    */
   goto notnmglft;                       /*                         */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur200 Surface type Consurf section lines ?\n");
  fprintf(dbgfil(SURPAC),
  "sur200 lin_type %s sec_type %s nor_type %s\n",
          lin_type ,  sec_type ,  nor_type );
  fflush(dbgfil(SURPAC)); 
  }
#endif

if (strcmp(lin_type,"LINE")    == 0  &&  /* NMG Consurf ?           */
    strcmp(sec_type,"SECTION") == 0  &&  /*                         */
    strcmp(nor_type,"NORMAL")  == 0    ) /*                         */
    {
    *p_type = 2;                         /* Output surface type     */
    goto nomore;                         /* End of analysis         */
    }


notnmglft:;     /*! Label notnmglft: Not NMG Consurf               !*/



nomore:;        /*! Label nomore:    Surface type is determined    !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur200 No patches nu= %d nv= %d \n",(int)*p_nu,(int)*p_nv);
  fprintf(dbgfil(SURPAC),
  "sur200 Exit*varkon_sur_importtype* *p_type= %d\n",(int)*p_type);
  fflush(dbgfil(SURPAC));
  }
#endif


    return(SUCCED);

  }

/*********************************************************/
