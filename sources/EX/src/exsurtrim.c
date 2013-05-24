/*********************************************************
*
*    exsurtrim.c
*    ===========
*
*    This file is part of the VARKON Execute Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    EXstrimusrd();  Create a user defined trimmed surface
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
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../include/EX.h"

#ifdef DEBUG
#include "../../IG/include/debug.h"
#endif

/*****************************************************************************/

        short EXstrimusrd(
        DBId    *sur_id,          /* Reference (# id.) for output surface    */
        DBId    *oldsur_id,       /* Input untrimmed surface                 */
        DBint    ncur,            /* Number of input curves                  */
        DBId    *pcurves,         /* Input curves                            */
        V2NAPA  *pnp)             /* Name parameter block pointer            */


/*      Creates a trimmed surface
 *
 *
 *
 *      (C) 2007-01-10 Sören L. , Örebro University
 *
 ****************************************************************************!*/

{
  DBSurf    sur;             /* Surface                                 */
  DBPatch  *ppat;            /* Allocated area topol. patch data  (ptr) */
  DBCurve   cur[NTRIMMAX];   /* Input curves                            */
  DBSeg    *seg[NTRIMMAX];   /* Input segments                          */
  DBSeg    *graseg[NTRIMMAX];/* Input graphical segments                */
  DBSegarr *pgetrimcvs;      /* Ptr to array of dbseggarr's  (geom trim)*/
  DBSegarr *pborder;         /* Ptr to arr of dbseggarr's  (gr. w. trim)*/
  DBptr     sur_la,la;       /*                                         */
  DBetype   typ;             /*                                         */
  DBint     i;               /* Loop index                              */
  DBint     status;          /* Error code from a called function       */
  char      errbuf[133];     /* Error message string                    */

/*
*** Start of function
*/
  status = 0;
/*
*** Get the old surface 
*/
   if ( DBget_pointer('I',oldsur_id,&sur_la,&typ) < 0 ) return(erpush("EX1402",""));
   if ( typ != SURTYP )
     {
     IGidst(oldsur_id,errbuf);
     return(erpush("EX1412",errbuf));
     }
   if ( (status=DBread_surface(&sur,sur_la)) < 0 ) return(status);
   if ( (status=DBread_patches(&sur,&ppat)) < 0 ) return(status);
/*
*** Get the trim curves 
*/
   for ( i=0; i<ncur; ++i )
     {
     if ( DBget_pointer('I',(pcurves+i),&la,&typ) < 0 )
       {
       IGidst((pcurves+i),errbuf);
       return(erpush("EX1402",errbuf));
       }
     if ( typ != CURTYP )
       {
       IGidst((pcurves+i),errbuf);
       return(erpush("EX1412",errbuf));
       }
     DBread_curve(&cur[i],&graseg[i],&seg[i],la);
     }
/*
*** Create trimmed surface (sur will get contents renewed)
*** old patches are not changed and can be reused in EXesur()
*/
   status = GEsur_trim_usrdef(ncur,cur,seg,graseg,&sur,sur_la,ppat,
                         &pborder,&pgetrimcvs);
   if (status < 0 ) goto error1;
/*
*** Draw surface and store in DB
*/
   status = EXesur(sur_id,&sur,ppat,pgetrimcvs,pborder,pnp);
   if (status<0) 
     {
     sprintf(errbuf,"EXesur%%EXscur");
     erpush("SU2943",errbuf);
     goto error1;
     }

error1:
/*
*** The input curve segments should not be deallocated since
*** they have been deallocated by EXesur => DBfree_sur_grwire. Unused
*** input graphical segments have been dealocated by GEsur_trim_usrdef()
*/


/*
*** Deallocate the patches
*/
   DBfree_patches(&sur,ppat);

   return(status);
  }

/********************************************************************/

