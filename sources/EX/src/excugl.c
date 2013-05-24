/*!******************************************************************
*
*    excugl.c
*    ========
*
*    EXcugl();    Interface function to MBS statement CUR_GL. 
*
*    This file is part of the VARKON Execute  Library.
*    URL:  http://varkon.sourceforge.net
*
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Library General Public
*    License as published by the Free Software Foundation; either
*    version 2 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Library General Public License for more details.
*
*    You should have received a copy of the GNU Library General Public
*    License along with this library; if not, write to the Free
*    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*    (C)Microform AB 1984-1999, Gunnar Liden,  gunnar@microform.se
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../include/EX.h"

#ifdef DEBUG
#include "../../IG/include/debug.h"
#endif

/******************************************************************!*/
/*                                                                  */
/*  Function: EXcugl                 SNAME: EXcugl File: excugl.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Interface function to MBS statement CUR_GL.                     */
/*                                                                  */
/*  The function decodes the first six characters of the input      */
/*  string, which defines computation case (the surpac function     */
/*  that shall be called).                                          */
/*                                                                  */
/*  There is one internal function for each computation case.       */
/*  The rest of the input string is decoded in the internal         */
/*  function, data from DB is retrieved, the selected surpac        */
/*  function called and DB entities is created.                     */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-05-28   Originally written                                 */
/*  1996-01-01   Erase of not used CUR_GL functions. Note           */
/*               that there is a copy on /u/bibliotek/EXE G. Liden  */
/*  1996-01-24   DBPatchF bortkommenterad tills vidare                */
/*  1996-05-13   Gary's rutiner bortkommenterade                    */
/*  1996-06-08   AFACE added                                        */
/*  1996-08-22   SUR_ROT added                                      */
/*  1996-09-06   SUR_RULED added                                    */
/*  1997-01-20   MATCH     added                                    */
/*  1997-01-25   BICPAT    added                                    */
/*  1997-02-07   IPK Conditional Compilation                        */
/*  1997-03-23   SUR_USRDEF (SUDEF/EXsuru) added                    */
/*  1997-03-26   SUR_ROT --> SUR_CYL                                */
/*  1997-05-16   EXsuru (SUR_USRDEF) erased                         */
/*  1998-03-21   Erased lines between TABORT                        */
/*  1999-12-18   curxxx->varkon_sur_yyyyy, sarea deleted            */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr EXcugl                Interface function to CUR_GL     */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Externally defined variables ---------------------*/
extern DBTmat *lsyspk;             /* C  ptr active coord. system   */
extern DBTmat  lklsyi;             /* C  ptr active inverted matrix */
extern DBptr   amsyla;             /* DB ptr active coord. system   */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_wferguson   * Write Ferguson surface                  */
/* varkon_sur_rferguson   * Read  Ferguson surface                  */
/* varkon_sur_wnmgcatia   * Write NMG->CATIA CUB_SUR                */
/* varkon_sur_bictra      * Transformate bicubic surface            */
/* varkon_sur_aface       * Create an approximate face              */
/* varkon_ini_gmsur       * Initialize DBSurf                       */
/* varkon_ini_gmcur       * Initialize DBCurve                      */
/* varkon_ini_evals       * Initialize EVALS                        */
/* varkon_pat_biccre1     * Create a bicubic patch                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short wricatia();      /* Write NMG->Catia cubic surface     */
static short wriferg();       /* Write Ferguson surface             */
static short reaferg();       /* Read  Ferguson surface             */
static short aface();         /* Create an approximate face         */
static short bicpat();        /* Create surface with 1 bicubic patch*/
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* EX1402 = Refererad storhet finns ej.                             */
/* EX1412 = Refererad storhet har fel typ.                          */
/* SU2943 = Called   function xxxxxx failed in EXcugl               */
/* SU2973 = Internal function xxxxxx failed in EXcugl               */
/* SU2993 = Severe program error ( ) in EXcucl                      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/
/*                                                                  */
  short EXcugl(
  DBId    *id,           /* PM (?) ptr to input curve               */
  DBId    *rid,          /* PM (?) ptr to input surface             */
  char    *str,          /* String with input data                  */
  V2NAPA  *pnp)          /*    (?) ptr to attribute data            */

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  char     name[9];      /* Computation case                        */
  DBSurf   sur;          /* Input surface                           */
  DBPatch *p_pat;        /* Surface patches                   (ptr) */
  DBCurve  cur;          /* Input curve                             */
  DBSeg   *p_seg;        /* Curve segments                    (ptr) */
/*-----------------------------------------------------------------!*/

  short    status;       /* Error code from a called function       */
  DBptr    la;           /* DB pointer                              */
  DBetype  typ;          /* Type of entity                          */
  char     errbuf[133];  /* Error message string                    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXcugl Enter  str %s\n",str);
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* ______________________________________                           */

/* Retrieve the first six characters (name) from the input string.  */
/* Call of function sscanf.                                         */
/* Call of function EXsugl if name= SELEnn, nn= 01, 02, ...., 10    */
/* Call of function EXgary if name= OPTFnn, nn= 01, 02, ...., 10    */
/* Call of function EXipkm if name= MATCH                           */
/* Goto noalloc if name=         RFERGU, AFACE                      */
/*                                                                 !*/

   status = 0;
   varkon_ini_gmsur(&sur);         /* Initialize DBSurf             */
   varkon_ini_gmcur(&cur);         /* Initialize DBCurve            */
   p_pat  = NULL;
   p_seg  = NULL;

   sscanf(str,"%s",name); 

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXcugl CUR_GL name= %s \n", name );
fflush(dbgfil(EXEPAC)); 
}
#endif

   if ( strcmp(name,"SELE01") == 0  ||
        strcmp(name,"SELE02") == 0  ||
        strcmp(name,"SELE03") == 0  ||
        strcmp(name,"SELE04") == 0  ||
        strcmp(name,"SELE05") == 0  ||
        strcmp(name,"SELE06") == 0  ||
        strcmp(name,"SELE07") == 0  ||
        strcmp(name,"SELE08") == 0  ||
        strcmp(name,"SELE09") == 0  ||
        strcmp(name,"SELE10") == 0  ||
        strcmp(name,"SELE99") == 0     )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXcugl Computation case  name= %s Surface element (switch to EXsugl)\n", name );
}
#endif

      status= EXsugl(id,rid,str,pnp);
      if ( status < 0 )
        {
        sprintf(errbuf,"exsugl%%EXcugl");
        return(erpush("SU2943",errbuf));
        }
      return(SUCCED);
      }

   if ( strcmp(name,"OPTF01") == 0  ||
        strcmp(name,"OPTF02") == 0  ||
        strcmp(name,"OPTF03") == 0  ||
        strcmp(name,"OPTF04") == 0  ||
        strcmp(name,"OPTF05") == 0  ||
        strcmp(name,"OPTF06") == 0  ||
        strcmp(name,"OPTF07") == 0  ||
        strcmp(name,"OPTF08") == 0  ||
        strcmp(name,"OPTF09") == 0  ||
        strcmp(name,"OPTF10") == 0  ||
        strcmp(name,"OPTF99") == 0     )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXcugl Optimization computation case  name= %s (switch to EXgary)\n", name );
}
#endif

/*1996-05-13      status= EXgary(id,rid,str,pnp);  */
      status = I_UNDEF;
      if ( status < 0 )
        {
        sprintf(errbuf,"exgary%%EXcugl");
        return(erpush("SU2943",errbuf));
        }
      return(SUCCED);
      }

#ifdef  IPK

   if ( strcmp(name,"MATCH" ) == 0   )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXcugl Model Match computation name= %s (switch to EXipkm)\n", name );
}
#endif

      status= EXipkm(id,rid,str,pnp);
      if ( status < 0 )
        {
        sprintf(errbuf,"EXipkm%%EXcugl");
        return(erpush("SU2943",errbuf));
        }
      return(SUCCED);
      }
#endif  /*  IPK  */


   if ( strcmp(name,"RFERGU") == 0 ) goto noalloc;
   if ( strcmp(name,"BICPAT") == 0 ) goto noalloc;

/*!                                                                 */
/* Get data for the referenced surface or curve rid.                */
/* Call of DBget_pointer and DBread_surface.                                       */
/* Allocate memory for the surface patches.                         */
/* Call of DBread_patches.                                                  */
/* Call of igidst if rid not is a surface.                          */
/*                                                                 !*/

    if ( DBget_pointer('I',rid,&la,&typ) < 0 ) return(erpush("EX1402",""));

    switch ( typ )
      {
      case SURTYP:
      DBread_surface(&sur,la);
      DBread_patches(&sur,&p_pat);
      break;

      case CURTYP:
      DBread_curve(&cur,NULL,&p_seg,la);
      break;

      default:
      IGidst(rid,errbuf);
      return(erpush("EX1412",errbuf));
      }

/*!                                                                 */
/* 2. Make the calculation for the given case                       */
/* __________________________________________                       */

/*                                                                 !*/

noalloc:  /* Label: No allocation of surface memory                 */

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXcugl Computation case  name= %s\n", name );
}
#endif

/*!                                                                 */
/* Switch to the right internal function:                           */
/*                                                                 !*/



   if ( strcmp(name,"WFERGU") == 0 ) 
     {
     status=wriferg(str,&sur,p_pat);
     if (status < 0 )
        {
#ifdef DEBUG
        sprintf(errbuf,"wriferg%%EXcugl");
        erpush("SU2973",errbuf);
#endif
        goto  err3;
        }
     }
   else if ( strcmp(name,"WCATIA") == 0 ) 
     {
     status=wricatia(str,&sur,p_pat);
     if (status < 0 )
        {
#ifdef DEBUG
        sprintf(errbuf,"wricatia%%EXcugl");
        erpush("SU2973",errbuf);
#endif
        goto  err3;
        }
     }
   else if ( strcmp(name,"RFERGU") == 0 ) 
     {
     status=reaferg(id,str,pnp);
#ifdef DEBUG
     if (status < 0 )
        {
        sprintf(errbuf,"reaferg%%EXcugl");
        erpush("SU2973",errbuf);
        }
#endif
     goto  nodeall;
     }

   else if ( strcmp(name,"AFACE" ) == 0 ) 
     {
     status= aface (id,str,pnp,&cur,p_seg );
#ifdef DEBUG
     if (status < 0 )
        {
        sprintf(errbuf," aface %%EXcugl");
        erpush("SU2973",errbuf);
        }
#endif
     DBfree_segments(p_seg);  /* Deallocate area for curve segments */
     goto  nodeall;  /* No deallocation of any surface     */
     }




   else if ( strcmp(name,"BICPAT") == 0 ) 
     {
     status=bicpat (id,rid,str,pnp);
#ifdef DEBUG
     if (status < 0 )
        {
        sprintf(errbuf,"bicpat%%EXcugl");
        erpush("SU2973",errbuf);
        }
#endif
     goto  nodeall;
     }


   else
     {
     sprintf(errbuf,"%s%%EXcugl",name);
     status= erpush("SU8903",errbuf);
     goto  err3;
     }

/*!                                                                 */
/* 3. Deallocation of memory for surface patches                    */
/* _____________________________________________                    */

err3:
/* Call of function DBfree_patches.                                         */
/*                                                                 !*/

    DBfree_patches(&sur,p_pat);

nodeall:  /* Label: No deallocation of memory                       */

    return(status);
  }
  
/********************************************************************/



/*!********* Internal ** function ** wriferg ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Function writes a CUB_SUR surface to file in Ferguson format     */

   static short wriferg(str,p_sur,p_pat)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  char    *str;          /* String with input data                  */
  DBSurf   *p_sur;        /* Pointer to surface                      */
  DBPatch   *p_pat;        /* Pointer to patches for the surface      */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

  char     name[9];      /* Name   (WFERGU)                         */
  char     fname[80];    /* Output file name                        */
  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Printout of input variables                                   */
/*                                                                 !*/

#ifdef  DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXcugl*wriferg Enter** Input string with all data for sur290:\n");
  fprintf(dbgfil(EXEPAC),
  "EXcugl*wriferg str= %s\n",str);
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

/*!                                                                 */
/* 2. Convert string to input data for surpac function              */
/* ___________________________________________________              */

/* Call of function sscanf.                                         */
/*                                                                 !*/

sscanf(str,"%s%s",name,fname); 

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXcugl*wriferg name %s fname %s\n", name, fname );
  }
#endif

/*!                                                                 */
/* Check that input string starts with (name=) WFERGU.              */
/*                                                                 !*/

 if ( strcmp(name,"WFERGU") != 0 )
 {
 sprintf(errbuf,                         /* Error SU2993 if not     */
 "(not WFERGU)%%wriferg");               /* WFERGU                  */
 return(erpush("SU2993",errbuf));        /*                         */
 }

/*!                                                                 */
/* 3. Printout of Ferguson surface                                  */
/* _______________________________                                  */
/*                                                                  */
/* Write surfcae data to file in Ferguson format                    */
/* Call of varkon_sur_wferguson (sur290).                           */
/*                                                                 !*/

  IGrsma();      /* Clear message zone                              */

  IGplma("Writing Ferguson surface data to file",IG_MESS);

  status = varkon_sur_wferguson (p_sur,p_pat,fname); 
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*wriferg sur290 failed \n");
  }
#endif

      sprintf(errbuf,"sur290%%EXcugl*wriferg");
      return(erpush("SU2943",errbuf));
      }

  IGrsma();      /* Clear message zone                              */

  return(SUCCED);

} /* End of function                                                */
/********************************************************************/







/*!********* Internal ** function ** reaferg ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Function creates a CUB_SUR surface from file in Ferguson format  */

   static short reaferg(id,str,pnp)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBId    *id;           /* PM (?) ptr to input curve               */
  char    *str;          /* String with input data                  */
  V2NAPA  *pnp;          /*    (?) ptr to attribute data            */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBPatch   *p_patout;     /* Pointer to alloc. area for patch data   */
  DBSurf    surout;       /* Output surface                          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  char     name[9];      /* Name   (RFERGU)                         */
  char     fname[80];    /* Output file name                        */
  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Printout of input variables                                   */
/*                                                                 !*/

#ifdef  DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXcugl*reaferg Enter** Input string with all data for sur291:\n");
  fprintf(dbgfil(EXEPAC),
  "EXcugl*reaferg str= %s\n",str);
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

/*!                                                                 */
/* 2. Convert string to input data for surpac function              */
/* ___________________________________________________              */

/* Call of function sscanf.                                         */
/*                                                                 !*/

sscanf(str,"%s%s",name,fname); 

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXcugl*reaferg name %s fname %s\n", name, fname );
 fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

/*!                                                                 */
/* Check that input string starts with (name=) RFERGU.              */
/*                                                                 !*/

 if ( strcmp(name,"RFERGU") != 0 )
 {
 sprintf(errbuf,                         /* Error SU2993 if not     */
 "(not RFERGU)%%reaferg");               /* RFERGU                  */
 return(erpush("SU2993",errbuf));        /*                         */
 }

/*!                                                                 */
/* 3. Convert Ferguson surface to CUB_SUR                           */
/* _______________________________________                          */
/*                                                                  */
/* Read surface Ferguson data from file and create bicubic surface  */
/* Call of varkon_sur_rferguson (sur291).                           */
/*                                                                 !*/

  IGrsma();      /* Clear message zone                              */

  IGplma("Converting Ferguson surface from file to CUB_SUR",IG_MESS);

  status = varkon_sur_rferguson (fname,&surout,&p_patout); 
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*reaferg sur291 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

      sprintf(errbuf,"sur291%%EXcugl*reaferg");
      return(erpush("SU2943",errbuf));
      }

/*!                                                                 */
/* Transformate surface if coordinate system lklsyi is defined      */
/* Call of varkon_sur_bictra (sur600).                              */
/*                                                                 !*/

      if ( lsyspk != NULL )
        { 
        if ( (status=varkon_sur_bictra(&surout,p_patout,&lklsyi)) < 0 )
          {
          sprintf(errbuf,"varkon_sur_bictra(sur600)%%EXcugl*reaferg");
          return(erpush("SU2943",errbuf));
          }
        }

/*!                                                                 */
/* Store converted surface in Geometry Memory (DB).                 */
/* Call of EXesur.                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*reaferg File and draw \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

  status = EXesur(id,&surout,p_patout,NULL,NULL,pnp);
  if ( status < 0 )
      {
      sprintf(errbuf,"EXesur%%EXcugl*reaferg");
      return(erpush("SU2943",errbuf));
      }


/*!                                                                 */
/* Deallocation of memory for the bicubic  surface                  */
/* Call of function DBfree_patches.                                         */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXcugl*reaferg Deallocate memory &surout %d p_patout %d\n",
           (int)&surout , (int)p_patout );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

  DBfree_patches(&surout,p_patout);

  IGrsma();      /* Clear message zone                              */

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXcugl*reaferg Memory is deallocated \n" );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif


  return(SUCCED);

} /* End of function                                                */
/********************************************************************/







/*!********* Internal ** function ** wricatia  **********************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Function writes a CUB_SUR surface to file in NMG->Catia          */
/* interface format.                                                */

   static short wricatia (str,p_sur,p_pat)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  char    *str;          /* String with input data                  */
  DBSurf   *p_sur;        /* Pointer to surface                      */
  DBPatch   *p_pat;        /* Pointer to patches for the surface      */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/* --------------- Internal variables ------------------------------*/
/*                                                                  */
/*                                                                  */
/*----------------------------------------------------------------- */

  char     name[9];      /* Name   (WCATIA)                         */
  char     fname[80];    /* Output file name                        */
  char     s_model[80];  /* Catia model name                        */
  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Printout of input variables                                   */
/*                                                                 !*/

#ifdef  DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXcugl*wricatia Enter** Input string with all data for sur292:\n");
  fprintf(dbgfil(EXEPAC),
  "EXcugl*wricatia str= %s\n",str);
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

/*!                                                                 */
/* 2. Convert string to input data for surpac function              */
/* ___________________________________________________              */

/* Call of function sscanf.                                         */
/*                                                                 !*/

sscanf(str,"%s%s%s",name,fname,s_model); 

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXcugl*wricatia name %s fname %s\n", name, fname );
  }
#endif

/*!                                                                 */
/* Check that input string starts with (name=) WCATIA.              */
/*                                                                 !*/

 if ( strcmp(name,"WCATIA") != 0 )
 {
 sprintf(errbuf,                         /* Error SU2993 if not     */
 "(not WCATIA)%%wricatia");              /* WCATIA                  */
 return(erpush("SU2993",errbuf));        /*                         */
 }

/*!                                                                 */
/* 3. Printout of cubic surface                                     */
/* _______________________________                                  */
/*                                                                  */
/* Write surface data to file in NMG->Catia format                  */
/* Call of varkon_sur_wnmgcatia (sur292).                           */
/*                                                                 !*/

  IGrsma();      /* Clear message zone                              */

  IGplma("Writing NMG->Catia surface data to file",IG_MESS);

  status = varkon_sur_wnmgcatia (p_sur,p_pat,fname,s_model); 
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*wricatia sur292 failed \n");
  }
#endif

      sprintf(errbuf,"sur292%%EXcugl*wriferg");
      return(erpush("SU2943",errbuf));
      }

  IGrsma();      /* Clear message zone                              */

  return(SUCCED);

} /* End of function                                                */
/********************************************************************/

/*!********* Internal ** function ** aface   ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Function creates an approximate face                             */

   static short aface  (id,str,pnp,p_cur,p_seg)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBId    *id;           /* PM (?) ptr to input curve               */
  char    *str;          /* String with input data                  */
  V2NAPA  *pnp;          /*    (?) ptr to attribute data            */
  DBCurve *p_cur;        /* Composite boundary UV curve       (ptr) */
  DBSeg   *p_seg;        /* Segment data for p_cur            (ptr) */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint    s_typ;        /* Output surface type:                    */
                         /* Eq. 1: Approximate to CUB_PAT patches   */
                         /* Eq. 2: Approximate to RAT_PAT patches   */
                         /* Eq. 3: Approximate to LFT_PAT patches   */
                         /* Eq. 4: Approximate to BPL_PAT patches   */
                         /* Eq. 5: Approximate to CON_PAT patches   */
  DBint    a_case;       /* Case for interrogation of surface:      */
                         /* Eq. 1: Use Coons (no interrogation)     */
                         /* Eq. 2: Line,   error   for no intersect */
                         /* Eq. 3: Line,   warning for no intersect */
                         /* Eq. 4: Line,   use Coons for failure    */
                         /* Eq. 5: Closest error   for no intersect */
                         /* Eq. 6: Closest warning for no intersect */
                         /* Eq. 7: Closest use Coons for failure    */
  DBint    method;       /* Method for approximation:               */
                         /* Eq. 1: Arclength Coons with given       */
                         /*        number of output U and V lines   */
  DBint    acc;          /* Computation accuracy                    */
                         /* Eq. 1: Tolerance band +/- surface       */
                         /* Eq. 2: Tolerance band  +  surface only  */
                         /* Eq. 3: Tolerance band  -  surface only  */
  DBint    n_ulin;       /* Number of U lines in output surface     */
  DBint    n_vlin;       /* Number of V lines in output surface     */
  DBfloat  c_crit;       /* Criterion for a face boundary corner    */
  DBfloat  ctol;         /* Coordinate          tolerance           */
  DBfloat  ntol;         /* Surface normal      tolerance           */
  DBfloat  rtol;         /* Radius of curvature tolerance           */
  DBPatch *p_patout;     /* Allocated area for patch data     (ptr) */
  DBSurf   surout;       /* Output surface                          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  char     name[9];      /* Name   (AFACE)                          */
  char     surtyp[20];   /* Output surface type                     */
  char     metod[80];    /* Method for approximation                */
  char     sacase[20];   /* Case a_case as input string             */
                         /* Eq. COONS                               */
                         /* Eq. LIN_E                               */
                         /* Eq. LIN_W                               */
                         /* Eq. LIN_C                               */
                         /* Eq. CLO_E                               */
                         /* Eq. CLO_W                               */
                         /* Eq. CLO_C                               */
  int      idum1,idum2;  /* Dummy integer(s) for sscanf             */
  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Printout of input variables                                   */
/*                                                                 !*/

#ifdef  DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXcugl*aface   Enter** Input string with all data for sur190:\n");
  fprintf(dbgfil(EXEPAC),
  "EXcugl*aface   str= %s\n",str);
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

  status = SUCCED;

  a_case = I_UNDEF;

/*!                                                                 */
/* 2. Convert string to input data for surpac function              */
/* ___________________________________________________              */

/* Call of function sscanf.                                         */
/*                                                                 !*/

sscanf(str,"%s%s%s%s%lf%d%d",name,surtyp,metod,sacase,
                            &c_crit,&idum1 ,&idum2 ); 
n_ulin = idum1;
n_vlin = idum2;

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXcugl*aface   name %s metod %s\n", name, metod );
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXcugl*aface n_ulin %d n_vlin %d c_crit %f\n", 
(int)n_ulin, (int)n_vlin, c_crit );
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* Check that input string starts with (name=) AFACE.               */
/*                                                                 !*/

 if ( strcmp(name,"AFACE" ) != 0 )
 {
 sprintf(errbuf,                         /* Error SU2993 if not     */
 "(not AFACE )%%aface  ");               /* AFACE                   */
 return(erpush("SU2993",errbuf));        /*                         */
 }

/*!                                                                 */
/* 3. Create an approximate face                                    */
/* _____________________________                                    */
/*                                                                  */
/* Create face defined by a composite UV (surface) curve            */
/* Call of varkon_sur_aface (sur190).                               */
/*                                                                 !*/

  IGrsma();      /* Clear message zone                              */

  IGplma("Processing SUR_AFACE",IG_MESS);

 if ( strcmp(surtyp,"CUB_SUR" ) == 0 )
   {
   s_typ = 1;
   }
 else
   {
   sprintf(errbuf,                    
   "(not CUB_SUR )%%aface  ");       
   return(erpush("SU2993",errbuf)); 
   }

 if       ( strcmp(metod,"ARCL_NUNV" ) == 0 )
   {
   method = 1;
   }
 else if ( strcmp(metod,"ARCL_AUTO" ) == 0 )
   {
   sprintf(errbuf,                    
   "(ARCL_AUTO not impl.)%%aface  ");       
   return(erpush("SU2993",errbuf)); 
   }
 else
   {
   sprintf(errbuf,"%s %%EXcugl*aface" ,metod );       
   return(erpush("SU5023",errbuf)); 
   }

 if       ( strcmp(sacase,"COONS" ) == 0 )
   {
   a_case = 1;
   }
 else if  ( strcmp(sacase,"LIN_E" ) == 0 )
   {
   a_case = 2;
   }
 else if  ( strcmp(sacase,"LIN_W" ) == 0 )
   {
   a_case = 3;
   }
 else if  ( strcmp(sacase,"LIN_C" ) == 0 )
   {
   a_case = 4;
   }
 else if  ( strcmp(sacase,"CLO_E" ) == 0 )
   {
   a_case = 5;
   }
 else if  ( strcmp(sacase,"CLO_W" ) == 0 )
   {
   a_case = 6;
   }
 else if  ( strcmp(sacase,"CLO_C" ) == 0 )
   {
   a_case = 7;
   }
 else
   {
   sprintf(errbuf,"%s %%EXcugl*aface" ,sacase);       
   return(erpush("SU5033",errbuf)); 
   }


  acc   = 1;
  ctol  = F_UNDEF;
  ntol  = F_UNDEF;
  rtol  = F_UNDEF;

  status = varkon_sur_aface
  (p_cur,p_seg,s_typ,method,a_case,acc,n_ulin, n_vlin, c_crit,
              ctol,ntol,rtol,&surout,&p_patout); 
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*aface sur190 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      if  (  p_patout == NULL )
        {
        goto  err2;
        }
       else
        {
        goto  err1;
        }
      }

/*!                                                                 */
/* Transformate surface if coordinate system lklsyi is defined      */
/* Call of varkon_sur_bictra (sur600).                              */
/*                                                                 !*/

      if ( lsyspk != NULL )
        { 
        if ( (status=varkon_sur_bictra(&surout,p_patout,&lklsyi)) < 0 )
          {
          sprintf(errbuf,"varkon_sur_bictra(sur600)%%EXcugl*aface  ");
          return(erpush("SU2943",errbuf));
          }
        }

/*!                                                                 */
/* Store converted surface in Geometry Memory (DB).                 */
/* Call of EXesur.                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*aface File and draw \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

  status = EXesur(id,&surout,p_patout,NULL,NULL,pnp);
  if ( status < 0 ) goto  err1;


/*!                                                                 */
/* Deallocation of memory for the surface                           */
/* Call of function DBfree_patches.                                         */
/*                                                                 !*/

err1:;

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXcugl*aface   Deallocate memory &surout %d p_patout %d\n",
           (int)&surout , (int)p_patout );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

  DBfree_patches(&surout,p_patout);

err2:;

  IGrsma();      /* Clear message zone                              */

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXcugl*aface   Memory is deallocated \n" );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif


  return(status);

} /* End of function                                                */
/********************************************************************/


/*!********* Internal ** function ** bicpat    ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Create a surface with one (1) bicubic patch                      */

   static short bicpat   (id,rid,str,pnp)
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBId    *id;           /* PM (?) ptr to input                     */
  DBId    *rid;          /* PM (?) ptr to input part                */
  char    *str;          /* String with input data                  */
  V2NAPA  *pnp;          /*    (?) ptr to attribute data            */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   EVALS   r00;          /* Corner point U=0 , V=0                  */
   EVALS   r10;          /* Corner point U=1 , V=0                  */
   EVALS   r01;          /* Corner point U=0 , V=1                  */
   EVALS   r11;          /* Corner point U=1 , V=1                  */

   DBPatchC patbic;      /* Bicubic patch                           */
   DBPatch  patout;      /* Topological patch                       */
   DBSurf   surout;      /* Output surface                          */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   short    pnum;        /* Parameter number                        */
   PMLITVA  vecval;      /* Vector                                  */


  char     name[9];      /* Name   (BICPAT)                         */
  int      idum1;        /* Dummy integer for sscanf                */
  DBint    r_case;       /* Bicubic creation case                   */
  DBint    status;       /* Error code from called function         */
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Printout of input variables                                   */
/*                                                                 !*/

#ifdef  DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXcugl*bicpat    Enter** Input string with all data for sur254:\n");
  fprintf(dbgfil(EXEPAC),
  "EXcugl*bicpat  str= %s\n",str);
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

  status = SUCCED;

/*!                                                                 */
/* 2. Convert string to input data for surpac function              */
/* ___________________________________________________              */

/* Call of function sscanf.                                         */
/*                                                                 !*/

sscanf(str,"%s%d", name,&idum1); 

r_case = idum1;

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXcugl*bicpat    name %s \n", name );
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXcugl*bicpat  r_case %d\n", (int)r_case );
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* Check that input string starts with (name=) BICPAT.              */
/*                                                                 !*/

 if ( strcmp(name,"BICPAT" ) != 0 )
 {
 sprintf(errbuf,                         /* Error SU2993 if not     */
 "(not BICPAT)%%bicpat");                /* BICPAT                  */
 return(erpush("SU2993",errbuf));        /*                         */
 }

/*!                                                                 */
/* 3. Create a surface with one bicubic patch                       */
/* __________________________________________                       */
/*                                                                  */
/* Call of varkon_pat_biccre1  (sur254).                            */
/*                                                                 !*/

  IGrsma();      /* Clear message zone                              */

  IGplma("Processing BICPAT ",IG_MESS);

/* Initialize EVALS variables                                       */
  varkon_ini_evals(&r00);
  varkon_ini_evals(&r10);
  varkon_ini_evals(&r01);
  varkon_ini_evals(&r11);
/* Initialize DBSurf variable                                       */
  varkon_ini_gmsur(&surout);

  surout.typ_su = CUB_SUR;
  surout.nu_su  = 1;
  surout.nv_su  = 1;


  pnum = 1;
  status = EXgvec(rid,pnum,&vecval);
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat EXgvec pnum=1 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      goto  err1;
      }
  r00.r_x = vecval.lit.vec_va.x_val;
  r00.r_y = vecval.lit.vec_va.y_val;
  r00.r_z = vecval.lit.vec_va.z_val;

  pnum = 2;
  status = EXgvec(rid,pnum,&vecval);
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat EXgvec pnum=2 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      goto  err1;
      }
  r10.r_x = vecval.lit.vec_va.x_val;
  r10.r_y = vecval.lit.vec_va.y_val;
  r10.r_z = vecval.lit.vec_va.z_val;

  pnum = 3;
  status = EXgvec(rid,pnum,&vecval);
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat EXgvec pnum=3 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      goto  err1;
      }
  r01.r_x = vecval.lit.vec_va.x_val;
  r01.r_y = vecval.lit.vec_va.y_val;
  r01.r_z = vecval.lit.vec_va.z_val;

  pnum = 4;
  status = EXgvec(rid,pnum,&vecval);
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat EXgvec pnum=4 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      goto  err1;
      }
  r11.r_x = vecval.lit.vec_va.x_val;
  r11.r_y = vecval.lit.vec_va.y_val;
  r11.r_z = vecval.lit.vec_va.z_val;

  pnum = 5;
  status = EXgvec(rid,pnum,&vecval);
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat EXgvec pnum=5 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      goto  err1;
      }
  r00.u_x = vecval.lit.vec_va.x_val;
  r00.u_y = vecval.lit.vec_va.y_val;
  r00.u_z = vecval.lit.vec_va.z_val;

  pnum = 6;
  status = EXgvec(rid,pnum,&vecval);
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat EXgvec pnum=6 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      goto  err1;
      }
  r10.u_x = vecval.lit.vec_va.x_val;
  r10.u_y = vecval.lit.vec_va.y_val;
  r10.u_z = vecval.lit.vec_va.z_val;

  pnum = 7;
  status = EXgvec(rid,pnum,&vecval);
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat EXgvec pnum=7 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      goto  err1;
      }
  r01.u_x = vecval.lit.vec_va.x_val;
  r01.u_y = vecval.lit.vec_va.y_val;
  r01.u_z = vecval.lit.vec_va.z_val;

  pnum = 8;
  status = EXgvec(rid,pnum,&vecval);
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat EXgvec pnum=8 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      goto  err1;
      }
  r11.u_x = vecval.lit.vec_va.x_val;
  r11.u_y = vecval.lit.vec_va.y_val;
  r11.u_z = vecval.lit.vec_va.z_val;

  pnum = 9;
  status = EXgvec(rid,pnum,&vecval);
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat EXgvec pnum=9 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      goto  err1;
      }
  r00.v_x = vecval.lit.vec_va.x_val;
  r00.v_y = vecval.lit.vec_va.y_val;
  r00.v_z = vecval.lit.vec_va.z_val;

  pnum = 10;
  status = EXgvec(rid,pnum,&vecval);
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat EXgvec pnum=10 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      goto  err1;
      }
  r10.v_x = vecval.lit.vec_va.x_val;
  r10.v_y = vecval.lit.vec_va.y_val;
  r10.v_z = vecval.lit.vec_va.z_val;

  pnum = 11;
  status = EXgvec(rid,pnum,&vecval);
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat EXgvec pnum=11 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      goto  err1;
      }
  r01.v_x = vecval.lit.vec_va.x_val;
  r01.v_y = vecval.lit.vec_va.y_val;
  r01.v_z = vecval.lit.vec_va.z_val;

  pnum = 12;
  status = EXgvec(rid,pnum,&vecval);
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat EXgvec pnum=12 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      goto  err1;
      }
  r11.v_x = vecval.lit.vec_va.x_val;
  r11.v_y = vecval.lit.vec_va.y_val;
  r11.v_z = vecval.lit.vec_va.z_val;

  pnum = 13;
  status = EXgvec(rid,pnum,&vecval);
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat EXgvec pnum=13 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      goto  err1;
      }
  r00.uv_x = vecval.lit.vec_va.x_val;
  r00.uv_y = vecval.lit.vec_va.y_val;
  r00.uv_z = vecval.lit.vec_va.z_val;

  pnum = 14;
  status = EXgvec(rid,pnum,&vecval);
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat EXgvec pnum=14 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      goto  err1;
      }
  r10.uv_x = vecval.lit.vec_va.x_val;
  r10.uv_y = vecval.lit.vec_va.y_val;
  r10.uv_z = vecval.lit.vec_va.z_val;

  pnum = 15;
  status = EXgvec(rid,pnum,&vecval);
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat EXgvec pnum=15 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      goto  err1;
      }
  r01.uv_x = vecval.lit.vec_va.x_val;
  r01.uv_y = vecval.lit.vec_va.y_val;
  r01.uv_z = vecval.lit.vec_va.z_val;

  pnum = 16;
  status = EXgvec(rid,pnum,&vecval);
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat EXgvec pnum=16 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      goto  err1;
      }
  r11.uv_x = vecval.lit.vec_va.x_val;
  r11.uv_y = vecval.lit.vec_va.y_val;
  r11.uv_z = vecval.lit.vec_va.z_val;


  status = varkon_pat_biccre1 (&r00, &r10, &r01, &r11, &patbic);
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat sur254 failed \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
      goto  err1;
      }

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),"EXcugl*bicpat Coefficients as in F07\n");
fprintf(dbgfil(EXEPAC),
"%f %f %f %f %f %f\n", patbic.a00x, patbic.a00y, patbic.a00z,
                       patbic.a10x, patbic.a10y, patbic.a10z );
fprintf(dbgfil(EXEPAC),
"%f %f %f %f %f %f\n", patbic.a20x, patbic.a20y, patbic.a20z,
                       patbic.a30x, patbic.a30y, patbic.a30z );
fprintf(dbgfil(EXEPAC),
"%f %f %f %f %f %f\n", patbic.a01x, patbic.a01y, patbic.a01z,
                       patbic.a11x, patbic.a11y, patbic.a11z );
fprintf(dbgfil(EXEPAC),
"%f %f %f %f %f %f\n", patbic.a21x, patbic.a21y, patbic.a21z,
                       patbic.a31x, patbic.a31y, patbic.a31z );
fflush(dbgfil(EXEPAC));
}
#endif

/*!                                                                 */
/*  Topological patch data                                          */
/*                                                                 !*/

  patout.styp_pat = CUB_PAT;               /* Type of secondary patch */
  patout.spek_c   = (char *) &patbic;      /* Secondary patch (C ptr) */
  patout.su_pat   = 0;                     /* Topological adress      */
  patout.sv_pat   = 0;                     /* secondary patch not def.*/
  patout.iu_pat   =           1;           /* Topological adress for  */
  patout.iv_pat   =           1;           /* current (this) patch    */
  patout.us_pat   =           1.0;         /* Start U on geom. patch  */
  patout.ue_pat   =           2.0;         /* End   U on geom. patch  */
  patout.vs_pat   =           1.0;         /* Start V on geom. patch  */
  patout.ve_pat   =           2.0;         /* End   V on geom. patch  */

  patout.box_pat.xmin = 1.0;               /* BBOX  initiation        */
  patout.box_pat.ymin = 2.0;               /*                         */
  patout.box_pat.zmin = 3.0;               /*                         */
  patout.box_pat.xmax = 4.0;               /*                         */
  patout.box_pat.ymax = 5.0;               /*                         */
  patout.box_pat.zmax = 6.0;               /*                         */
  patout.box_pat.flag = -1;                /* Not calculated          */
    
  patout.cone_pat.xdir = 1.0;              /* BCONE initiation        */
  patout.cone_pat.ydir = 2.0;              /*                         */
  patout.cone_pat.zdir = 3.0;              /*                         */
  patout.cone_pat.ang  = 4.0;              /*                         */
  patout.cone_pat.flag = -1;               /* Not calculated          */


/*  #ifdef TILLSVIDARE  */

/*!                                                                 */
/* Store converted surface in Geometry Memory (DB).                 */
/* Call of EXesur.                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"EXcugl*bicpat  File and draw \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

  status = EXesur(id,&surout,&patout,NULL,NULL,pnp);
  if ( status < 0 ) goto  err1;

/*  #endif */  /* TILLSVIDARE */


err1:;

  IGrsma();      /* Clear message zone                              */

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXcugl*bicpat Exit \n" );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif


  return(status);

} /* End of function                                                */
/********************************************************************/


