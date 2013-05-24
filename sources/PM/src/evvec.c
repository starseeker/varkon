/**********************************************************************
*
*    evvec.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*    evvec()     evaluate vec function 
*    evvecp()    evaluate vecp function
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
#include "../../GE/include/GE.h"

#include <stdlib.h>
#include <math.h>
#include <ctype.h>

extern short  modtyp;      /* indicate module type, from GM-part */

extern PMPARVA *func_pv;   /* Access structure for MBS routines */
extern short    func_pc;   /* Number of actual parameters */
extern PMLITVA *func_vp;   /* Pekare till resultat. */

/*!******************************************************/

        short evvec()

/*      Evaluate function VEC.
 *
 *      In: Global func_pv  => Parameter value array
 *
 *      Ut: Global*func_vp  = Referens.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *      2001-03-05 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
/*
***assign VEC , result type VEC
*/
   func_vp->lit.vec_va.x_val = func_pv[ 1 ].par_va.lit.float_va;
   func_vp->lit.vec_va.y_val = func_pv[ 2 ].par_va.lit.float_va;

   if ( modtyp == 2 )
       func_vp->lit.vec_va.z_val = 0.0;
   else
       func_vp->lit.vec_va.z_val = func_pv[ 3 ].par_va.lit.float_va;

   return( 0 );
  }  

/*!******************************************************/
/*!******************************************************/

        short evvecp()

/*      Evaluate function VECP.
 *
 *      In: Global func_pv  => Parameter value array
 *
 *      Ut: Global*func_vp  = Referens.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *      2001-03-05 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   v2float  radius;       /* parameters for VECP */
   v2float  angel;        /*      - " -          */
   v2float  zcomp;        /*      - " -          */

   radius = func_pv[ 1 ].par_va.lit.float_va;
   angel  = func_pv[ 2 ].par_va.lit.float_va;
   if ( modtyp == _2D )
       zcomp  = 0.0;
   else
       zcomp  = func_pv[ 3 ].par_va.lit.float_va;

/*
***calculate VECP , result type VEC
*/
   func_vp->lit.vec_va.x_val = radius * COS(angel*DGTORD);
   func_vp->lit.vec_va.y_val = radius * SIN(angel*DGTORD);
   func_vp->lit.vec_va.z_val = zcomp;

   return( 0 );
  }  

/*!******************************************************/

