/**********************************************************************
*
*    evshade.c
*    =========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*    evltvi();     Evaluerar LIGHT_VIEW
*    evlton();     Evaluerar LIGHT_ON
*    evltof();     Evaluerar LIGHT_OFF
*    evshvi();     Evaluerar SHADE_VIEW
*    evmtvi();     Evaluerar MATERIAL_VIEW
*    evcrco();     Evaluerar CRE_COLOR
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
#ifdef V3_X11
#include "../../WP/include/WP.h"
#endif

extern PMPARVA *proc_pv;  /* inproc.c *pv      Access structure for MBS routines */
extern short    proc_pc;  /* inproc.c parcount Number of actual parameters */

/*!******************************************************/

        short evltvi()

/*      Evaluerar LIGHT_VIEW.
 *
 *      In: extern pv   => Pekare till array med parametervärden
 *          extern pc   => Antal parametrar.
 *
 *      Felkod: 
 *
 *      (C)microform ab 1997-02-18 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   DBVector dir;
   DBfloat spotang,focus;


   if ( proc_pc == 2 )
     {
     dir.x_gm = 0.0;
     dir.y_gm = 0.0;
     dir.z_gm = 1.0;
     spotang  = 180.0;
     focus    = 0.0;
     }
   else if ( proc_pc == 3 )
     {
     dir.x_gm = proc_pv[3].par_va.lit.vec_va.x_val;
     dir.y_gm = proc_pv[3].par_va.lit.vec_va.y_val;
     dir.z_gm = proc_pv[3].par_va.lit.vec_va.z_val;
     spotang  = 180.0;
     focus    = 0.0;
     }
   else if ( proc_pc == 4 )
     {
     dir.x_gm = proc_pv[3].par_va.lit.vec_va.x_val;
     dir.y_gm = proc_pv[3].par_va.lit.vec_va.y_val;
     dir.z_gm = proc_pv[3].par_va.lit.vec_va.z_val;
     spotang  = proc_pv[4].par_va.lit.float_va;
     focus    = 0.0;
     }
   else
     {
     dir.x_gm = proc_pv[3].par_va.lit.vec_va.x_val;
     dir.y_gm = proc_pv[3].par_va.lit.vec_va.y_val;
     dir.z_gm = proc_pv[3].par_va.lit.vec_va.z_val;
     spotang  = proc_pv[4].par_va.lit.float_va;
     focus    = proc_pv[5].par_va.lit.float_va;
     }

   return(WPltvi(proc_pv[1].par_va.lit.int_va,
    (DBVector *)&proc_pv[2].par_va.lit.vec_va,
                &dir,spotang,focus));
 }

/********************************************************/
/*!******************************************************/

        short evlton()

/*      Evaluerar LIGHT_ON.
 *
 *      In: extern pv   => Pekare till array med parametervärden
 *
 *      (C)microform ab 1997-02-19 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   return(WPlton(proc_pv[1].par_va.lit.int_va,
                 proc_pv[2].par_va.lit.float_va,
                 TRUE));
 }

/********************************************************/
/*!******************************************************/

        short evltof()

/*      Evaluerar LIGHT_OFF.
 *
 *      In: extern pv   => Pekare till array med parametervärden
 *
 *      (C)microform ab 1997-02-19 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   return(WPlton(proc_pv[1].par_va.lit.int_va,
                 100.0,
                 FALSE));
 }

/********************************************************/
/*!******************************************************/

        short evshvi()

/*      Evaluerar SHADE_VIEW.
 *
 *      In: extern pv   => Pekare till array med parametervärden
 *
 *      Felkod: 
 *
 *      (C)microform ab 1997-02-18 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   return(WPshad(proc_pv[1].par_va.lit.int_va,
                 proc_pv[2].par_va.lit.int_va));
 }

/********************************************************/
/*!******************************************************/

        short evmtvi()

/*      Evaluerar MATERIAL_VIEW.
 *
 *      In: extern pv   => Pekare till array med parametervärden
 *
 *      Felkod: 
 *
 *      (C)microform ab 1997-02-18 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   return(WPmtvi(proc_pv[1].par_va.lit.int_va,
                 proc_pv[2].par_va.lit.float_va,
                 proc_pv[3].par_va.lit.float_va,
                 proc_pv[4].par_va.lit.float_va,
                 proc_pv[5].par_va.lit.float_va,
                 proc_pv[6].par_va.lit.float_va,
                 proc_pv[7].par_va.lit.float_va,
                 proc_pv[8].par_va.lit.float_va,
                 proc_pv[9].par_va.lit.float_va,
                 proc_pv[10].par_va.lit.float_va,
                 proc_pv[11].par_va.lit.float_va,
                 proc_pv[12].par_va.lit.float_va,
                 proc_pv[13].par_va.lit.float_va,
                 proc_pv[14].par_va.lit.float_va));
 }

/********************************************************/
/*!******************************************************/

        short evcrco()

/*      Evaluerar CRE_COLOR
 *
 *      In: extern pv   => Pekare till array med parametervärden
 *
 *      Felkod: 
 *
 *      (C)microform ab 1997-02-18 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
#ifdef V3_X11

   return(WPccol(proc_pv[1].par_va.lit.int_va,
                 proc_pv[2].par_va.lit.int_va,
                 proc_pv[3].par_va.lit.int_va,
                 proc_pv[4].par_va.lit.int_va));
#endif

#ifdef WIN32
extern int msccol();

   return(msccol(proc_pv[1].par_va.lit.int_va,
                 proc_pv[2].par_va.lit.int_va,
                 proc_pv[3].par_va.lit.int_va,
                 proc_pv[4].par_va.lit.int_va));
#endif

 }

/********************************************************/
