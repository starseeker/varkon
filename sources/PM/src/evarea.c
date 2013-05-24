/**********************************************************************
*
*    evarea.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*    evarea();      Evaluerar AREA
*    evsuar();      Evaluerar SURFACE_AREA
*    evsear();      Evaluerar SECTION_AREA
*    evsecg();      Evaluerar SECTION_CGRAV
*    evcgra();      Evaluerar CGRAV
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
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../EX/include/EX.h"

extern PMPARVA *func_pv;   /* Access structure for MBS routines */
extern short    func_pc;   /* Number of actual parameters */
extern PMLITVA *func_vp;   /* Pekare till resultat. */

/*!******************************************************/

       short evarea()

/*      Evaluerar geometri-funktionen AREA.
 *
 *      In: extern *func_pv  => Pekare till array med parametervärden.
 *          extern *func_pc  => Pointer to number of actual parameters.
 *
 *      Ut: extern *func_vp  = Beräknad area.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 23/7/90 J. Kjellander
 *
 *      2001-03-05 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   short  nref = 0; /* Number of referenses */ 
   short  i;        /* loop variable */
   short  status;
   DBfloat  area;
   DBVector  tp;
   V2REFVA refarr[GMMXXH + 1]; /* reference array */

/*
***Skapa refarr.
*/
   for ( i = 2; i <= func_pc ; i++ )
     {
     refarr[ nref ].seq_val = func_pv[i].par_va.lit.ref_va[ 0 ].seq_val;
     refarr[ nref ].ord_val = func_pv[i].par_va.lit.ref_va[ 0 ].ord_val;
     refarr[ nref ].p_nextre = func_pv[i].par_va.lit.ref_va[ 0 ].p_nextre;
     nref++;
    }
/*
***Execute.
*/
   status = EXarea(refarr,nref,func_pv[1].par_va.lit.float_va,&area,&tp);
   func_vp->lit.float_va = area;

   return(status);
 }

/********************************************************/
/*!******************************************************/

       short evsuar()

/*      Evaluerar geometri-funktionen SURFACE_AREA.
 *
 *      In: extern *func_pv  => Pekare till array med parametervärden.
 *          extern *func_vp
 *
 *      Ut: extern *func_vp  = Beräknad area.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 1997-12-17 J. Kjellander
 *
 *      2001-03-05 In-Param changed to Global variables, R Svedin

 ******************************************************!*/

{
   short  status;
   DBfloat  area;

/*
***Execute.
*/
   status = EXsuar(&func_pv[1].par_va.lit.ref_va[0],
                    func_pv[2].par_va.lit.float_va,
                   &area);

   func_vp->lit.float_va = area;
   
   return(status);
 }

/********************************************************/
/*!******************************************************/

       short evsear()

/*      Evaluerar geometri-funktionen SECTION_AREA.
 *
 *      In: extern *func_pv  => Pekare till array med parametervärden.
 *          extern *func_vp
 *
 *      Ut: extern *func_vp  = Beräknad area.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 1997-12-17 J. Kjellander
 *
 *      2001-03-05 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   short  status;
   DBfloat  area;

/*
***Execute.
*/
   status = EXsear(&func_pv[1].par_va.lit.ref_va[0],
                    func_pv[2].par_va.lit.float_va,
                   &area);

   func_vp->lit.float_va = area;
   
   return(status);
 }

/********************************************************/
/*!******************************************************/

       short evsecg()

/*      Evaluerar geometri-funktionen SECTION_CGRAV.
 *
 *      In: extern *func_pv  => Pekare till array med parametervärden.
 *          extern *func_vp
 *
 *      Ut: extern *func_vp  = Beräknad tyngdpunkt.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 2000-03-31 J. Kjellander
 *
 *      2001-03-05 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   short    status;
   DBVector cgrav;

/*
***Execute.
*/
   status = EXsecg(&func_pv[1].par_va.lit.ref_va[0],
                    func_pv[2].par_va.lit.float_va,
                   &cgrav);

   func_vp->lit.vec_va.x_val = cgrav.x_gm;
   func_vp->lit.vec_va.y_val = cgrav.y_gm;
   func_vp->lit.vec_va.z_val = cgrav.z_gm;
   
   return(status);
 }

/********************************************************/
/*!******************************************************/

       short evcgra()

/*      Evaluerar geometri-funktionen CGRAV.
 *
 *      In: extern *func_pv  => Pointer to array with parametervalues.
 *          extern *func_pc  => Pointer to number of actual parameters.
 *
 *      Ut: extern *func_vp  = Beräknad area.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 23/7/90 J. Kjellander
 *
 *      2001-03-05 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   short  nref = 0; /* Number of referenses */ 
   short  i;        /* loop variable */
   short  status;
   DBfloat  area;
   DBVector  tp;
   V2REFVA refarr[GMMXXH + 1]; /* reference array */

/*
***Skapa refarr.
*/
   for ( i = 2; i <= func_pc ; i++ )
     {
     refarr[ nref ].seq_val = func_pv[i].par_va.lit.ref_va[ 0 ].seq_val;
     refarr[ nref ].ord_val = func_pv[i].par_va.lit.ref_va[ 0 ].ord_val;
     refarr[ nref ].p_nextre = func_pv[i].par_va.lit.ref_va[ 0 ].p_nextre;
     nref++;
    }
/*
***Execute.
*/
   status = EXarea(refarr,nref,func_pv[1].par_va.lit.float_va,&area,&tp);
   func_vp->lit.vec_va.x_val = tp.x_gm;
   func_vp->lit.vec_va.y_val = tp.y_gm;
   func_vp->lit.vec_va.z_val = 0.0;

   return(status);
 }

/********************************************************/
