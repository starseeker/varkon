/*!******************************************************************
*
*    exsugl.c
*    ========
*
*    EXsugl();     Interface function to MBS statement CUR_GL for SELEnn
*                  functions
*
*    This file is part of the VARKON Execute Library.
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
*    (C)Microform AB 1984-1998, Gunnar Liden, gunnar@microform.se
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
/*  Function: EXsugl                 SNAME: EXsugl File: exsugl.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Interface function to MBS statement CUR_GL for SELEnn functions */
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
/*  1994-12-10   Originally written                                 */
/*  1997-05-16   Elimination of compiler warnings                   */
/*  1998-12-18   surxxx->varkon_sur_yyyy                            */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr EXsugl                Interface CUR_GL for SELEnn      */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Externally defined variables ---------------------*/
extern DBTmat *lsyspk;             /* C  ptr active coord. system   */
extern DBTmat  lklsyi;             /* C  ptr active inverted matrix */
extern DBptr   amsyla;             /* DB ptr active coord. system   */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_creloft     * Create LFT_SUR from curves              */
/* varkon_sur_sele01      * Create SELE01 surface element           */
/* varkon_sur_closestpt   * Closest point on surface                */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short sele99();        /* SELE99 for a surface               */
static short sele01();        /* Create LFT_SUR from curves         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-- Static (common) variables for the functions in this file -----*/
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* EX1402 = Refererad storhet finns ej.                             */
/* EX1412 = Refererad storhet har fel typ.                          */
/* SU2943 = Called   function xxxxxx failed in EXsugl               */
/* SU2973 = Internal function xxxxxx failed in EXsugl               */
/* SU2993 = Severe program error ( ) in EXcucl                      */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/
/*                                                                  */
  short EXsugl(
  DBId    *id,           /* PM (?) ptr to input curve               */
  DBId    *rid,          /* PM (?) ptr to input surface             */
  char    *str,          /* String with input data                  */
  V2NAPA  *pnp)          /*    (?) ptr to attribute data            */


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  char     name[9];      /* Computation case                        */
  DBSurf   sur;          /* Input surface                           */
  DBPatch *p_pat;        /* Ptr to memory area for surface patches  */
/*-----------------------------------------------------------------!*/

  DBint    status;       /* Error code from a called function       */
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
"EXsugl Enter  str %s\n",str);
 fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */

/* Retrieve the first six characters (name) from the input string.  */
/* Call of function sscanf. Goto noalloc if name= SELE01            */
/*                                                                 !*/

   status = 0;

   sscanf(str,"%s",name); 
   if ( strcmp(name,"SELE01") == 0 ) goto noalloc;


   if ( strcmp(name,"SELE02") == 0  ||
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
      sprintf(errbuf,"(name)%%EXsugl");
      return(varkon_erpush("SU2993",errbuf));
      }

/*!                                                                 */
/* Get data for the referenced surface rid.                         */
/* Call of DBget_pointer and DBread_surface.                        */
/* Allocate memory for the surface patches.                         */
/* Call of DBread_patches.                                          */
/* Call of igidst if rid not is a surface.                          */
/*                                                                 !*/

    if ( DBget_pointer('I',rid,&la,&typ) < 0 ) 
               return(varkon_erpush("EX1402",""));

    switch ( typ )
      {
      case SURTYP:
      DBread_surface(&sur,la);
      DBread_patches(&sur,&p_pat);
      break;

      default:
      IGidst(rid,errbuf);
      return(varkon_erpush("EX1412",errbuf));
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
"EXsugl Computation case  name= %s\n", name );
}
#endif

/*!                                                                 */
/* Switch to the right internal function:                           */
/* Call of sele99 if name is SELE99                                 */
/*                                                                 !*/

   if      ( strcmp(name,"SELE99") == 0 ) 
     {
     status=sele99 (id,rid,str,pnp,&sur,p_pat);
     if (status < 0 )
        {
#ifdef DEBUG
        sprintf(errbuf,"sele99%%EXsugl");
        varkon_erpush("SU2973",errbuf);
#endif
        goto  err3;
        }
     }
   else if ( strcmp(name,"SELE01") == 0 ) 
     {
     status=sele01 (id,rid,str,pnp);
#ifdef DEBUG
     if (status < 0 )
        {
        sprintf(errbuf,"sele01%%EXsugl");
        varkon_erpush("SU2973",errbuf);
        goto  nodeall;
        }
#endif
     goto nodeall;
     }
   else
     {
     sprintf(errbuf,"(name)%%EXsugl");
     varkon_erpush("SU2993",errbuf);
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

/*!********* Internal ** function ** sele99 *************************/
/*                                                                  */
/* !!!  Bara beh}llen som mall f|r yta d} minne ska allokeras !!!!! */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Function SELE99 for a surface                                    */

   static short sele99 (id,rid,str,pnp,p_sur,p_pat)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBId    *id;           /* PM (?) ptr to input curve               */
  DBId    *rid;          /* PM (?) ptr to input surface             */
  char    *str;          /* String with input data (SELE99 .. )     */
  V2NAPA  *pnp;          /*    (?) ptr to attribute data            */
  DBSurf  *p_sur;        /* Pointer to surface                      */
  DBPatch *p_pat;        /* Pointer to patches for the surface      */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBVector extpt;        /* External point                          */
  DBVector s_uvpt;       /* Start U,V point.                        */
  DBint    ocase;        /* Computation case                        */
                         /* Eq. 1: Ordered according to R*3 distance*/
                         /* Eq. 2: Ordered according to UV  distance*/
  DBint    acc;          /* Computation accuracy (case)             */
                         /* Eq. 1: One "layer"  of patches          */
                         /* Eq. 2: Two "layers" of patches          */
  DBint    sol_no;       /* Requested solution number               */
  DBint    no_points;    /* Total number of solution points         */
  DBVector c_pt;         /* Closest point  (R*3 space)              */
  DBVector c_uvpt;       /* Closest point  (U,V space)              */
                         /* c_uvpt.z_gm= distance to surface        */
  char     refstr[50];   /* String with MBS reference (# id)        */
  DBPoint  poi;          /* Input (external) point                  */
  DBPoint  poi_p;        /* Point projected on a surface            */
  DBId     pid[MXINIV];  /* PM (?) ptr                              */
  short    igstid();     /* Convert string to DB identity           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBptr    la;           /* DB pointer                              */
  DBetype  typ;          /* Type of entity                          */
  char     name[9];      /* Name   (SELE99)                         */
  DBfloat    xe,ye,ze;     /* External point                          */
  DBfloat    u_s,v_s;      /* Start u,v point                         */
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
  "EXsugl*sele99 Enter** str= %s\n",str);
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

/*!                                                                 */
/* 2. Convert string to input data for surpac function              */
/* ___________________________________________________              */

/* Call of function sscanf.                                         */
/*                                                                 !*/

sscanf(str,"%s%lf%lf%lf%lf%lf%s",name,&xe,&ye,&ze,&u_s,&v_s,refstr); 

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXsugl xe= %f ye= %f ze= %f \n",xe,ye,ze);
  fprintf(dbgfil(EXEPAC),
  "EXsugl u_s %f v_s %f name %s\n",u_s,v_s,name);
  fprintf(dbgfil(EXEPAC),
  "EXsugl refstr  %s\n",refstr );
  }
#endif

/*!                                                                 */
/* Check that input string starts with (name=) SELE99.              */
/*                                                                 !*/

 if ( strcmp(name,"SELE99") != 0 )
 {
 sprintf(errbuf, "(not SELE99)%%gl_clo");
 return(varkon_erpush("SU2993",errbuf));
 }

/*!                                                                 */
/* The UV point to variable s_uvpt. Add 1.0 if point is defined.    */
/*                                                                 !*/

  s_uvpt.x_gm = u_s;
  s_uvpt.y_gm = v_s;
  if ( u_s >= 0 )
    {
    s_uvpt.x_gm =  s_uvpt.x_gm + 1.0;
    s_uvpt.y_gm =  s_uvpt.y_gm + 1.0;
    s_uvpt.z_gm =  0.0;
    }
  else
    {
    s_uvpt.z_gm = -0.1;
    }

/*!                                                                 */
/* The R*3 point from ref. (# id) to variable EXtpt.                */
/* Convert reference string to DB pointer. Call of igstid.          */
/* Retrieve point data from DB.                                     */
/* Call of DBget_pointer and DBread_point                           */
/*                                                                 !*/

    status = IGstid(refstr,pid);
    if ( status < 0 )
      {
      sprintf(errbuf,"igstid%%EXsugl*sele99");
      return(varkon_erpush("SU2943",errbuf));
      }

    if ( DBget_pointer('I',pid,&la,&typ) < 0 ) 
              return(varkon_erpush("EX1402",""));

    switch ( typ )
      {
      case POITYP:
      DBread_point(&poi,la);
      break;

      default:
      IGidst(pid,errbuf);
      return(varkon_erpush("EX1412",errbuf));
      }

#ifdef DEBUG
    if ( dbglev(EXEPAC) == 1 )
      {
      fprintf(dbgfil(EXEPAC),
"EXsugl*sele99 Pt %s coordinates %10.2f %10.2f %10.2f\n",
       refstr,poi.crd_p.x_gm,poi.crd_p.y_gm,poi.crd_p.z_gm );
      }
#endif

    extpt  = poi.crd_p;



    ocase  = 1;
    acc    = 2;
    sol_no = 1;

    status = varkon_sur_closestpt(p_sur,p_pat,&extpt,&s_uvpt,ocase,acc,
                   sol_no,&no_points,&c_pt ,&c_uvpt );
    if ( status < 0 )
      {
      sprintf(errbuf,"varkon_sur_closestpt(sur160)%%EXsugl*sele99");
      return(varkon_erpush("SU2943",errbuf));
      }

/*
*** Koordinater till projicerad punkt
*/

    poi_p.crd_p = c_pt;

/*
***Lagra i gm och rita.
*/
    status=EXepoi(id,&poi_p,pnp);
    if ( status < 0 )
      {
      sprintf(errbuf,"EXepoi%%EXsugl*sele99");
      return(varkon_erpush("SU2943",errbuf));
      }

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/


/*!********* Internal ** function ** sele01 *************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/* Create surface element of type SELE01                            */
/*                                                                  */
/*                                                                  */

   static short sele01 (id,rid,str,pnp)
/*                                                                 !*/
/********************************************************************/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBId    *id;           /* PM (?) ptr to input curve               */
  DBId    *rid;          /* PM (?) ptr to input spine curve         */
  char    *str;          /* String with input data (SELE01 .. )     */
  V2NAPA  *pnp;          /*    (?) ptr to attribute data            */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */

  DBId     pid[MXINIV];  /* PM (?) ptr                              */
  short    igstid();     /* Convert string to DB identity           */
/*                                                                  */
/*-----------------------------------------------------------------!*/

  DBCurve  spine;        /* Spine curve                             */
  DBSeg   *p_spineseg;   /* Spine segments                    (ptr) */
  DBCurve  lim_s;        /* Start limit curve                       */
  DBSeg   *p_limseg;     /* Start limit curve segments        (ptr) */
  DBfloat    l_s;          /* Start parameter value limit curve       */
  DBfloat    l_e;          /* End   parameter value limit curve       */
  DBCurve  gener1;       /* Generatrix 1 curve                      */
  DBSeg   *p_gener1seg;  /* Generatrix 1 curve segments       (ptr) */
  DBCurve  gener2;       /* Generatrix 2 curve                      */
  DBSeg   *p_gener2seg;  /* Generatrix 2 curve segments       (ptr) */
  DBint    e_case;       /* Case for element SELE01                 */
  DBSurf   surout;       /* Output surface                          */
  DBPatch *p_patout;     /* Patches for the output surface    (ptr) */

  char     s_lim_s[19];  /* String MBS reference to start limit crv */
  char     s_gener1[19]; /* String MBS reference to generatrix 1    */
  char     s_gener2[19]; /* String MBS reference to generatrix 2    */


  DBptr    la;           /* DB pointer                              */
  DBetype  typ;          /* Type of entity                          */
  char     name[9];      /* Name   (SELE01)                         */
  int      idum1;        /* Dummy integer                           */
  short    status;       /* Error code from called function         */
                         /* Obs short eftersom argument till return?*/
  char     errbuf[80];   /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */

/* 1. Retrieve spine data                                           */
/* ______________________                                           */
/*                                                                 !*/

#ifdef  DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXsugl*sele01 Enter** str= %s\n",str);
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

   status = SUCCED;     /* Initiate error code                      */

/*!                                                                 */
/* Get DB pointer for the spine (and type of entity).               */
/* Call of DBget_pointer.                                           */
/* Get, and allocate memory area, for the spine curve.              */
/* Call of DBread_curve.                                            */
/*                                                                 !*/

    if ( DBget_pointer('I',rid,&la,&typ) < 0 ) 
               return(varkon_erpush("EX1402",""));

    switch ( typ )
      {
      case (CURTYP):
      DBread_curve(&spine,NULL,&p_spineseg,la);
      break;

      default:
      IGidst(pid,errbuf);
      return(varkon_erpush("EX1412",errbuf));
      }

/*!                                                                 */
/* 2. Convert string to input data for surpac function              */
/* ___________________________________________________              */
/*                                                                  */
/* Retrieve name and number of strips. Call of function sscanf.     */
/*                                                                 !*/

   sscanf(str,"%s",name);

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXsugl*sele01 name %s\n",name );
  fprintf(dbgfil(EXEPAC),
  "EXsugl*sele01 &spine %d p_spineseg %d spine.ns_cu %d\n",
                 (int)&spine,   (int)p_spineseg,   spine.ns_cu );
  }
 fflush(dbgfil(EXEPAC)); /* To file from buffer      */
#endif

/*!                                                                 */
/* Check that input string starts with (name=) SELE01.              */
/*                                                                 !*/

 if ( strcmp(name,"SELE01") != 0 )
 {
 sprintf(errbuf, "(not SELE01)%%EXsugl*sele01");
 return(varkon_erpush("SU2993",errbuf));
 }

/*!                                                                 */
/* Retrieve MBS (#) references from string. Call sscanf.            */
/*                                                                 !*/


   sscanf(str,"%s%s%lf%lf%s%s%d", name, s_lim_s ,
        &l_s, &l_e, s_gener1, s_gener2, &idum1  ); 

   e_case = idum1;

/* To surpac parameter values                                       */

   l_s = l_s + 1.0;
   l_e = l_e + 1.0;


#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsugl*sele01 s_lim_s %s l_s %f l_e %f\n" ,s_lim_s , l_s , l_e  );
}
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsugl*sele01 s_gener1 %s s_gener2 %s\n" ,s_gener1 ,s_gener2  );
}
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXsugl*sele01 e_case %d \n" , (int)e_case  );
}
 fflush(dbgfil(EXEPAC)); /* To file from buffer      */
#endif

/*!                                                                 */
/* Convert reference string to DB pointer. Calls of igstid.         */
/* Retrieve curve data from DB.                                     */
/* Calls of DBget_pointer and DBread_curve.                         */
/*                                                                 !*/

/*!                                                                 */
/* Start limit curve s_lim_s    to lim_s  and p_limseg.             */

    status = IGstid(s_lim_s   ,pid);
    if ( status < 0 )
      {
      sprintf(errbuf,"igstid%%EXsugl*sele01");
      return(varkon_erpush("SU2943",errbuf));
      }

    if ( DBget_pointer('I',pid,&la,&typ) < 0 ) 
                     return(varkon_erpush("EX1402",""));

    switch ( typ )
      {
      case (CURTYP):
      DBread_curve(&lim_s,NULL,&p_limseg,la);
      break;

      default:

      IGidst(pid,errbuf);
      return(varkon_erpush("EX1412",errbuf));
      }

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXsugl*sele01 &lim_s %d p_limseg %d lim_s.ns_cu %d\n",
                 (int)&lim_s,   (int)p_limseg,   lim_s.ns_cu );
  }
 fflush(dbgfil(EXEPAC)); /* To file from buffer      */
#endif


/*!                                                                 */
/* Generatrix 1 s_gener1 to gener1 and p_gener1seg.                 */
/*Empty                                                            !*/

    status = IGstid(s_gener1 ,pid);
    if ( status < 0 )
      {
      sprintf(errbuf,"igstid%%EXsugl*sele01");
      return(varkon_erpush("SU2943",errbuf));
      }

    if ( DBget_pointer('I',pid,&la,&typ) < 0 ) 
                  return(varkon_erpush("EX1402",""));

    switch ( typ )
      {
      case (CURTYP):
      DBread_curve(&gener1,NULL,&p_gener1seg,la);
      break;

      default:

      IGidst(pid,errbuf);
      return(varkon_erpush("EX1412",errbuf));
      }

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXsugl*sele01 &gener1 %d p_gener1seg %d gener1.ns_cu %d\n",
                 (int)&gener1,   (int)p_gener1seg,   gener1.ns_cu );
  }
 fflush(dbgfil(EXEPAC)); /* To file from buffer      */
#endif


/*!                                                                 */
/* Generatrix 2 s_gener2 to gener1 and p_gener2seg.                 */
/*Empty                                                            !*/

    status = IGstid(s_gener2 ,pid);
    if ( status < 0 )
      {
      sprintf(errbuf,"igstid%%EXsugl*sele01");
      return(varkon_erpush("SU2943",errbuf));
      }

    if ( DBget_pointer('I',pid,&la,&typ) < 0 ) 
                      return(varkon_erpush("EX1402",""));

    switch ( typ )
      {
      case (CURTYP):
      DBread_curve(&gener2,NULL,&p_gener2seg,la);
      break;

      default:

      IGidst(pid,errbuf);
      return(varkon_erpush("EX1412",errbuf));
      }

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXsugl*sele01 &gener2 %d p_gener2seg %d gener2.ns_cu %d\n",
                 (int)&gener2,   (int)p_gener2seg,   gener2.ns_cu );
  }
 fflush(dbgfil(EXEPAC)); /* To file from buffer      */
#endif


/*!                                                                 */
/* 3. Create surface element                                        */
/* _________________________                                        */

/* Create surface element SELE01.                                   */
/* Call of varkon_sur_sele01 (sur801).                              */
/*                                                                 !*/

   status= varkon_sur_sele01
        (&spine,p_spineseg,&lim_s,p_limseg,l_s,l_e,
        &gener1,p_gener1seg, &gener2,p_gener2seg,e_case,
                        &surout,&p_patout);
    if ( status < 0 )
      {
      sprintf(errbuf,"varkon_sur_sele01(sur801)%%EXsugl*sele01");
      varkon_erpush("SU2943",errbuf);
      goto err2;
      }


#ifdef TILLSVIDARE
/* Start TILLSVIDARE  */
/*!                                                                 */
/* 3. Create conic lofting surface                                  */
/* _______________________________                                  */

/* Create surface for the given curves                              */
/* Call of varkon_sur_creloft (sur800).                             */
/*                                                                 !*/

   no_strips = 1;

   status= varkon_sur_creloft(&spine,p_spineseg,no_strips,lim,p_lseg,
    tan,p_tseg,mid,p_mseg,mflag,&surout,&p_patout);
    if ( status < 0 )
      {
      sprintf(errbuf,"varkon_sur_creloft(sur800)%%EXsugl*sele01");
      varkon_erpush("SU2943",errbuf);
      goto err2;
      }

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXsugl*sele01 &surout %d p_patout %d\n",
                 (int)&surout ,  (int)p_patout );
  }
 fflush(dbgfil(EXEPAC)); /* To file from buffer      */
#endif

/*
***Lagra yta i gm och rita.
*/

    status = EXesur(id,&surout,p_patout,pnp);
    if ( status < 0 )
      {
      sprintf(errbuf,"EXesur%%EXsugl*sele01");
      varkon_erpush("SU2943",errbuf);
      }

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "EXsugl*sele01 Surface filed in gm. \n");
  }
 fflush(dbgfil(EXEPAC)); /* To file from buffer      */
#endif

/*!                                                                 */
/* n. Deallocate memory                                             */
/* ____________________                                             */
/*                                                                  */
/* Deallocate memory for the surface.                               */
/* Call of DBfree_patches.                                                  */
/*                                                                 !*/

    DBfree_patches(&surout,p_patout);

    DBfree_segments(p_lseg[0]);
    DBfree_segments(p_tseg[0]);
    DBfree_segments(p_mseg[0]);
    DBfree_segments(p_lseg[1]);
    DBfree_segments(p_tseg[1]);
    if ( no_strips > 1 )
      {
      DBfree_segments(p_mseg[1]);
      DBfree_segments(p_lseg[2]);
      DBfree_segments(p_tseg[2]);
      }

/* Slut  TILLSVIDARE  */
#endif




/*!                                                                 */
err2:
/* Deallocate memory for curves: spine, limit and generatrices.     */
/* Calls of DBfree_segments.                                                 */
/*                                                                 !*/

    DBfree_segments(p_spineseg);
    DBfree_segments(p_limseg);
    DBfree_segments(p_gener1seg);
    DBfree_segments(p_gener2seg);


    return(status);

} /* End of function                                                */
/********************************************************************/

