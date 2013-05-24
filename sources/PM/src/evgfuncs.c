/**********************************************************************
*
*    evgfuncs.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes the following routines:
*
*    evon()      evaluate on function
*    evend()     evaluate end function
*    evstrt()    evaluate start function
*    evion()     evaluate inv_on function
*    evtang()    evaluate tang function
*    evitan()    evaluate inv_tang function
*    evcurv()    evaluate curv function
*    evicur()    evaluate inv_curv function
*    evarcl()    evaluate arcl function
*    eviarc()    evaluate inv_arcl function
*    evcent()    evaluate center function
*    evnorm()    evaluate norm function
*    evsect()    evaluate intersect function
*    evnsec()    evaluate n_intersect function
*
*    evrefc();   evaluate REFC
*    evvecl();   evaluate VECL
*    evvecn();   evaluate VECN
*    evcrpr();   evaluate VPROD
*    evangl();   evaluate ANGLE
*
*    evions();     Evaluerar INV_ONSUR
*    eveval();     Evaluerar EVAL
*    evncui();     Evaluerar N_CUR_INT
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

#ifndef ANALYZER           /* don't compile if analyser */
extern DBTmat  *lsyspk;
#endif

extern short  modtyp;      /* indicate module type, from GM-part */

extern PMPARVA *func_pv;   /* Access structure for MBS routines */
extern short    func_pc;   /* Number of actual parameters */
extern PMLITVA *func_vp;   /* Pekare till resultat. */


#ifndef ANALYZER           /* don't compile if analyser */

/*!******************************************************/

        short evarcl()

/*      Evaluate function ARCL.
 *
 *      In: Global func_pv  =>Parameter value array
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
***execute ARCL , result type FLOAT
*/
   return(EXarcl( &func_pv[1].par_va.lit.ref_va[0],
                  &func_vp->lit.float_va));
  }  

/*!******************************************************/
/*!******************************************************/

        short eviarc()

/*      Evaluate function INV_ARCL.
 *
 *      In: Global func_pv  => Parameter value array
 *
 *      Ut: Global *func_vp  = Referens.
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
   return(EXiarc(&func_pv[1].par_va.lit.ref_va[0],
                  func_pv[2].par_va.lit.float_va,
                 &func_vp->lit.float_va));

  }  

/*!******************************************************/
/*!******************************************************/

        short evtang()

/*      Evaluate function TANG.
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
   return(EXtang(&func_pv[1].par_va.lit.ref_va[0],
                  func_pv[2].par_va.lit.float_va,
                  lsyspk, (DBVector *)&func_vp->lit.vec_va));
  }  

/*!******************************************************/
/*!******************************************************/

        short evitan()

/*      Evaluate function INV_TANG.
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
   return(EXitan(      &func_pv[1].par_va.lit.ref_va[0],
           (DBVector *)&func_pv[2].par_va.lit.vec_va,
                 (short)func_pv[3].par_va.lit.int_va,
                       &func_vp->lit.float_va));

  }  

/*!******************************************************/
/*!******************************************************/

        short evcurv()

/*      Evaluate function CURV.
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
***execute CURV , result type FLOAT
*/
  return(EXcurv( &func_pv[1].par_va.lit.ref_va[0],
                  func_pv[2].par_va.lit.float_va,
                 &func_vp->lit.float_va ));

  }

/*!******************************************************/
/*!******************************************************/

        short evicur()

/*      Evaluate function INV_CURV.
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
   return(EXicur(&func_pv[1].par_va.lit.ref_va[0],
                  func_pv[2].par_va.lit.float_va,
           (short)func_pv[3].par_va.lit.int_va,
                 &func_vp->lit.float_va));

  }

/*!******************************************************/
/*!******************************************************/

        short evcen()

/*      Evaluate function CENTRE.
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
   short status;
/*
***execute CENTRE , result type VEC
*/
   if ( ( status = EXcen( &func_pv[1].par_va.lit.ref_va[0],
                           func_pv[2].par_va.lit.float_va,
                           lsyspk, (DBVector *)&func_vp->lit.vec_va ) ) != 0 )
          return( status );

   return( 0 );
  }

/*!******************************************************/
/*!******************************************************/

        short evnorm()

/*      Evaluate function NORM.
 *
 *      In: Global func_pv  => Parameter value array
 *          Global func_pc  => Number of parameters
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
   DBfloat u,v;

   if ( func_pc == 1 )
     {
     u = 0.0;
     v = 0.0;
     }
   else
     {
     switch ( func_pv[2].par_va.lit_type )
        {
        case C_INT_VA:
        u = (DBfloat)func_pv[2].par_va.lit.int_va;
        v = 0.0;
        break;

        case C_FLO_VA:
        u = (DBfloat)func_pv[2].par_va.lit.float_va;
        v = 0.0;
        break;

        case C_VEC_VA:
        u = (DBfloat)func_pv[2].par_va.lit.vec_va.x_val;
        v = (DBfloat)func_pv[2].par_va.lit.vec_va.y_val;
        break;
 
        default:
        return(erpush("IN5092","NORM"));
        }
     }

   return(EXnorm( &func_pv[1].par_va.lit.ref_va[0],u,v,(DBVector *)
	              &func_vp->lit.vec_va)); 
  } 

/*!******************************************************/
/*!******************************************************/

        short evstrt()

/*      Evaluate function START.
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
   return(EXstrt( &func_pv[1].par_va.lit.ref_va[0],(DBVector *)
	              &func_vp->lit.vec_va));
  }  

/*!******************************************************/
/*!******************************************************/

        short evend()

/*      Evaluate function END.
 *
 *      In: Global func_pv  => Parameter value array
 *
 *      Ut: Global*func_vp  => C-pointer to result value.
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
   return(EXend( &func_pv[1].par_va.lit.ref_va[0],(DBVector *)
	             &func_vp->lit.vec_va));
  }  

/*!******************************************************/
/*!******************************************************/

        short evon()

/*      Evaluate function ON.
 *
 *      In: Global func_pv  => Parameter value array
 *          Global func_pc  => Number of parameters
 *
 *      Ut: Global*func_vp  => C-pointer to result value.
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
   DBfloat u,v;

   if ( func_pc == 1 )
     {
     u = 0.0;
     v = 0.0;
     }
   else
     {
     switch ( func_pv[2].par_va.lit_type )
        {
        case C_INT_VA:
        u = (DBfloat)func_pv[2].par_va.lit.int_va;
        v = 0.0;
        break;

        case C_FLO_VA:
        u = (DBfloat)func_pv[2].par_va.lit.float_va;
        v = 0.0;
        break;

        case C_VEC_VA:
        u = (DBfloat)func_pv[2].par_va.lit.vec_va.x_val;
        v = (DBfloat)func_pv[2].par_va.lit.vec_va.y_val;
        break;
 
        default:
        return(erpush("IN5092","ON"));
        }
     }

   return(EXon( &func_pv[1].par_va.lit.ref_va[0],u,v,(DBVector *)
	            &func_vp->lit.vec_va));
  }  

/*!******************************************************/
/*!******************************************************/

        short evion()

/*      Evaluate function INV_ON.
 *
 *      In: Global func_pv  => Parameter value array
 *
 *      Ut: Global*func_vp  => C-pointer to result value.
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
   return(EXion(      &func_pv[1].par_va.lit.ref_va[0],
          (DBVector *)&func_pv[2].par_va.lit.vec_va,
                (short)func_pv[3].par_va.lit.int_va,
                      &func_vp->lit.float_va));
  }  

/*!******************************************************/
/*!******************************************************/

        short evsect()

/*      Evaluate function intersect .
 *
 *      In:   Global param. func_pv[] =>Parameter value array
 *
 *      Out:  Global param. *valp  =>   C-pointer to result value
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
   return(EXsect(&func_pv[1].par_va.lit.ref_va[0],
                 &func_pv[2].par_va.lit.ref_va[0],
                  func_pv[3].par_va.lit.int_va,
                  func_pv[4].par_va.lit.int_va,
        (DBVector *)&func_vp->lit.vec_va));

  }

/*!******************************************************/
/*!******************************************************/

        short evnsec()

/*      Evaluate function n_intersect.
 *
 *      In:   Global param. func_pv[] =>Parameter value array
 *
 *      Out:  Global param. *valp  => C-pointer to result value
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
   short numint,status;

   status = EXnsec(&func_pv[1].par_va.lit.ref_va[0],
                   &func_pv[2].par_va.lit.ref_va[0],
             (short)func_pv[3].par_va.lit.int_va,
                    lsyspk, &numint);

   if ( status < 0 ) return(status);

   func_vp->lit.int_va = (DBint)numint;
   return(0);
  }  

/*!******************************************************/

#endif

/*!******************************************************/

        short evrefc()

/*      Evaluerar funktionen REFC.
 *
 *      In: Global func_pv  => Pekare till array med parametervärden.
 *          Global func_vp  => Pekare till resultat.
 *          Global func_pc  => Antal parametrar.
 *
 *      Ut: Global*func_vp  = Referens.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 7/10/86 R. Svedin
 *
 *      19/10/86 Bug, J. Kjellander
 *      2001-03-05 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    short pvi,vai;

/*
***Sekvens och ordningsnummer.
*/
    for ( pvi=1, vai=0; pvi <= func_pc; pvi += 2, vai++ )
      {
      func_vp->lit.ref_va[vai].seq_val = func_pv[pvi].par_va.lit.int_va;
      func_vp->lit.ref_va[vai].ord_val = func_pv[pvi+1].par_va.lit.int_va;
      }
/*
***Pekare.
*/
    func_vp->lit.ref_va[0].p_nextre = NULL;

    for ( vai=0; vai < ((func_pc+1)/2)-1; vai++ )
      {
      func_vp->lit.ref_va[vai].p_nextre = &func_vp->lit.ref_va[vai+1];
      func_vp->lit.ref_va[vai+1].p_nextre = NULL;
      }

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evvecl()

/*      Evaluerar funktionen VECL.
 *
 *      In: Global func_pv  => Pekare till array med parametervärden.
 *          Global func_vp  => Pekare till resultat.
 *
 *      Ut: Global*func_vp  = Referens.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 13/4/87  R. Svedin.
 *
 *      1999-03-03 GExxx, J.Kjellander
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    if ( modtyp == 2 ) func_vp->lit.float_va =
      GEvector_length2D((DBVector *)&func_pv[1].par_va.lit.vec_va);
    else func_vp->lit.float_va =
      GEvector_length3D((DBVector *)&func_pv[1].par_va.lit.vec_va);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evvecn()

/*      Evaluerar funktionen VECN.
 *
 *      In: Global func_pv  => Pekare till array med parametervärden.
 *          Global func_vp  => Pekare till resultat.
 *
 *      Ut: Global*func_vp  = Referens.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 13/4/87  R. Svedin.
 *
 *      1999-03-03 GExxx, J.Kjellander
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    if ( modtyp == 2 )
      GEnormalise_vector2D((DBVector *)&func_pv[1].par_va.lit.vec_va,
                           (DBVector *)&func_vp->lit.vec_va);
    else
       GEnormalise_vector3D((DBVector *)&func_pv[1].par_va.lit.vec_va,
                            (DBVector *)&func_vp->lit.vec_va);
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evcrpr()

/*      Evaluerar funktionen VPROD.
 *
 *      In: Global func_pv  => Pekare till array med parametervärden.
 *          Global func_vp  => Pekare till resultat.
 *
 *      Ut: Global*func_vp  = Referens.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 13/4/87  R. Svedin.
 *
 *      1999-03-03 GExxx, J.Kjellander
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(GEvector_product((DBVector *)&func_pv[1].par_va.lit.vec_va,
                            (DBVector *)&func_pv[2].par_va.lit.vec_va,
                            (DBVector *)&func_vp->lit.vec_va));
  }

/********************************************************/
/*!******************************************************/

        short evangl()

/*      Evaluerar funktionen ANGLE.
 *
 *      In: Global func_pv  => Pekare till array med parametervärden.
 *          Global func_vp  => Pekare till resultat.
 *
 *      Ut: Global*func_vp  = Referens.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 13/4/87  R. Svedin.
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(GExy_angled( func_pv[1].par_va.lit.float_va,
                        func_pv[2].par_va.lit.float_va,
                       &func_vp->lit.float_va));
  }

/********************************************************/
/*!******************************************************/

        short evions()

/*      Evaluerar funktionen INV_ON_SUR.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *          Global  func_pc  => Number of parameters 
 *
 *      Ut: Global *func_vp  => Pointer to result value.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 25/4/94 J. Kjellander
 *
 *      2001-03-05 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
    DBVector uv_start;

/*
***Har startpunkt getts ? Om inte skicka (0,0,-1),
***detta tolkas av surpac-rutinen som "hela ytan".
*/
    if ( func_pc == 5 )
      {
      uv_start.x_gm = func_pv[5].par_va.lit.vec_va.x_val;
      uv_start.y_gm = func_pv[5].par_va.lit.vec_va.y_val;
      uv_start.z_gm = 0;
      }
    else
      {
      uv_start.x_gm =  0;
      uv_start.y_gm =  0;
      uv_start.z_gm = -1;
      }
/*
***Exekvera.
*/
    return(EXions(   &func_pv[1].par_va.lit.ref_va[0],
         (DBVector *)&func_pv[2].par_va.lit.vec_va,
               (DBint)func_pv[3].par_va.lit.int_va,
               (DBint)func_pv[4].par_va.lit.int_va,
                     &uv_start,
         (DBVector *)&func_vp->lit.vec_va));
}

/********************************************************/
/*!******************************************************/

        short eveval()

/*      Evaluerar funktionen EVAL.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Funktion result value.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 25/4/94 J. Kjellander
 *
 *      2001-03-05 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera.
*/
    return(EXeval(   &func_pv[1].par_va.lit.ref_va[0],
                      func_pv[2].par_va.lit.str_va,
         (DBVector *)&func_pv[3].par_va.lit.vec_va,
         (DBVector *)&func_vp->lit.vec_va));
}

/********************************************************/
/*!******************************************************/

        short evncui()

/*      Evaluerar funktionen N_CUR_INT.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Funktion result value.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 25/4/94 J. Kjellander
 *
 *      2001-03-05 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   return(EXncui(&func_pv[1].par_va.lit.ref_va[0],
                 &func_pv[2].par_va.lit.ref_va[0],
                 &func_vp->lit.int_va));
}

/********************************************************/
