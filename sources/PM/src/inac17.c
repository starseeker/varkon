/**********************************************************************
*
*    inac17.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    short inequa()   evaluate equal operator
*    short ingreq()   evaluate greater equal
*    short inleeq()   evaluate less equal
*    short inand()    evaluate AND
*    short inor()     evaluate OR
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

/* 
***standard types
*/
extern pm_ptr  stintp;
extern pm_ptr  stflop;
extern pm_ptr  ststrp;
extern pm_ptr  stvecp;
extern pm_ptr  strefp;
extern pm_ptr  stbolp;

/********************************************************/
/*!******************************************************/

        short evequa(
        PMLITVA  *val1p,
        pm_ptr    tyla1,
        PMLITVA  *val2p,
        pm_ptr    tyla2,
        PMLITVA  *valp,
        pm_ptr   *tylap)

/*      Evaluate Equal.
 *      The routine are also used for Not Equal == NOT Equal
 *
 *      In:   *val1p  =>  C-pointer to left expression value
 *             tyla1  =>  Type pointer for      - " -          
 *            *val2p  =>  C-pointer to right expression value
 *             tyla2  =>  Type pointer for       - " -   
 *
 *      Out:  *valp   =>   Result value
 *            *tylap  =>   Result type pointer
 *
 *      FV:   return - error severity code
 *
 *      Error codes:  IN 308 Type missmatch for right operand, INT or FLOAT desired
 *                    IN 310            - " -                , STRING desired
 *                    IN 311            - " -                , VECTOR  - " -
 *                    IN 312 Ilegal types for operator ......
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   bool x_bool;
   bool y_bool;
   bool z_bool;

   bool seq_bool;
   bool ord_bool;
   V2REFVA *ref1p;
   V2REFVA *ref2p;

  STTYTBL type1;
/*
***read type info 
*/
  strtyp( tyla1, &type1 );

   switch ( type1.kind_ty )
      { 
      case ST_INT:
         if ( ineqty( tyla1, tyla2 ) )                 /* equal types ? */
            {  
/*
***evaluate integer equal, result is boolean
*/
            valp->lit_type = C_BOL_VA;
            *tylap = stbolp;

            valp->lit.bool_va =(v2bool)(val1p->lit.int_va == 
                                        val2p->lit.int_va);

            }
         else if ( ineqty( tyla2, stflop ) )           /* if FLOAT */
            {  
/*
***evaluate integer EQ float, result is boolean
*/
            valp->lit_type = C_BOL_VA;
            *tylap = stbolp;

            valp->lit.bool_va = (v2bool)(val1p->lit.int_va == 
                                         val2p->lit.float_va);
            }
         else  
            { 
/*
***Type missmatch for right operand, INT or FLOAT desired
*/
            return( erpush( "IN3083", " =, <> " ) );
            }

         break;

      case ST_FLOAT:
         if ( ineqty( tyla1, tyla2 ) )                 /* equal types ? */
            {  
/*
***evaluate float equal, result is boolean
*/
            valp->lit_type = C_BOL_VA;
            *tylap = stbolp;

            valp->lit.bool_va = (v2bool)(val1p->lit.float_va == 
                                         val2p->lit.float_va);

            }
         else if ( ineqty( tyla2, stintp ) )           /* if INT */
            {  
/*
***evaluate float == integer, result is boolean
*/
            valp->lit_type = C_BOL_VA;
            *tylap = stbolp;

            valp->lit.bool_va = (v2bool)(val1p->lit.float_va == 
                                         val2p->lit.int_va);
            }
         else  
            { 
/*
***Type missmatch for right operand, INT or FLOAT desired
*/
            return( erpush( "IN3083", " =, <> " ) );
            }
        break;

      case ST_STR:
         if ( ineqty( tyla1, tyla2 ) )                 /* equal types ? */
            {  
/*
***evaluate string equal, result is boolean
*/
            valp->lit_type = C_BOL_VA;
            *tylap = stbolp;

            if ( strcmp( val1p->lit.str_va, val2p->lit.str_va ) == 0 )
                valp->lit.bool_va = 1;
            else
                valp->lit.bool_va = 0 ;
            }
         else 
/*
***Error. Type missmatch for right operand, STRING desired
*/
            {
            return( erpush( "IN3103", " =, <> " ) );
            }

         break;

      case ST_VEC:
         if ( ineqty( tyla1, tyla2 ) )                 /* equal types ? */
            {  
/*
***evaluate vector equal, result is boolean
*/
            valp->lit_type = C_BOL_VA;
            *tylap = stbolp;

            x_bool = val1p->lit.vec_va.x_val == val2p->lit.vec_va.x_val;
            y_bool = val1p->lit.vec_va.y_val == val2p->lit.vec_va.y_val;
            z_bool = val1p->lit.vec_va.z_val == val2p->lit.vec_va.z_val;

            valp->lit.bool_va = (v2bool)(x_bool && y_bool && z_bool);
            }
         else 
/*
***Error. Type missmatch for right operand, VECTOR desired 
*/
            {
            return( erpush( "IN3113", " =, <> " ) );
            }

         break;

      case ST_REF:
         if ( ineqty( tyla1, tyla2 ) )                 /* equal types ? */
            {
/*
***evaluate ref equal, result is boolean
*/
            ref1p = val1p->lit.ref_va;
            ref2p = val2p->lit.ref_va;

            valp->lit_type = C_BOL_VA;
            *tylap = stbolp;

            seq_bool = ( ref1p->seq_val == ref2p->seq_val );
            ord_bool = ( ref1p->ord_val == ref2p->ord_val );

            while ( seq_bool && ord_bool && (ref1p->p_nextre != NULL)
                                         && (ref2p->p_nextre != NULL) )
               {
               ref1p = ref1p->p_nextre;
               ref2p = ref2p->p_nextre;

               seq_bool = ( ref1p->seq_val == ref2p->seq_val );
               ord_bool = ( ref1p->ord_val == ref2p->ord_val );
               }

            valp->lit.bool_va = (v2bool)(seq_bool && ord_bool);
            }
         else
/*
***Error. Type missmatch for right operand, REF desired
*/
            {
            return( erpush( "IN3283", " =, <> " ) );
            }

         break;

      case ST_ARR:
         return( erpush( "IN3333", "equal arrays not implemented" ) );
         break;

      default:   
/*
***Error. Ilegal type for Equal operator
*/
         return( erpush( "IN3123", " =, <> " ) );
         break;

      }
   return( 0 );
}

/********************************************************/
/*!******************************************************/

        short evgreq(
        PMLITVA  *val1p,
        pm_ptr    tyla1,
        PMLITVA  *val2p,
        pm_ptr    tyla2,
        PMLITVA  *valp,
        pm_ptr   *tylap)

/*      Evaluate Greater Equal.
 *      The routine are also used for Less Then = NOT Greater Equal 
 *
 *      In:   *val1p  =>  C-pointer to left expression value
 *             tyla1  =>  Type pointer for      - " -          
 *            *val2p  =>  C-pointer to right expression value
 *             tyla2  =>  Type pointer for       - " -   
 *
 *      Out:  *valp   =>   Result value
 *            *tylap  =>   Result type pointer
 *
 *      FV:   return - error severity code
 *
 *      Error codes:  IN 308 Type missmatch for right operand, INT or FLOAT desired
 *                    IN 312 Ilegal types for operator ......
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STTYTBL type1;
/* 
***read type info
*/
   strtyp( tyla1, &type1 );

   switch ( type1.kind_ty )
      { 
      case ST_INT:
         if ( ineqty( tyla1, tyla2 ) )              /* equal types ? */
            {
/*
***evaluate integer GE, result is boolean
*/
            valp->lit_type = C_BOL_VA;
            *tylap = stbolp;

            valp->lit.bool_va = (v2bool)(val1p->lit.int_va >= 
                                         val2p->lit.int_va);
            }
         else if ( ineqty( tyla2, stflop ) )        /* if FLOAT */
            {
/*
***evaluate integer EQ float, result is boolean
*/
            valp->lit_type = C_BOL_VA;
            *tylap = stbolp;

            valp->lit.bool_va = (v2bool)(val1p->lit.int_va >= 
                                         val2p->lit.float_va);
            }
         else  
/*
***Type missmatch for right operand, INT or FLOAT desired
*/
            {
            return( erpush( "IN3083", " >=, < " ) );
            }

         break;

      case ST_FLOAT:
         if ( ineqty( tyla1, tyla2 ) )               /* equal types ? */
            {  
/*
***evaluate float equal, result is boolean
*/
            valp->lit_type = C_BOL_VA;
            *tylap = stbolp;

            valp->lit.bool_va = (v2bool)(val1p->lit.float_va >= 
                                         val2p->lit.float_va);
            }
         else if ( ineqty( tyla2, stintp ) )         /* if INT */
            {  
/*
***evaluate float >= integer, result is boolean
*/
            valp->lit_type = C_BOL_VA;
            *tylap = stbolp;

            valp->lit.bool_va = (v2bool)(val1p->lit.float_va >= 
                                         val2p->lit.int_va);
            }
         else  
/*
***Type missmatch for right operand, INT or FLOAT desired
*/
            { 
            return( erpush( "IN3083", " >=, < " ) );
            }

        break;

      case ST_STR:
          if ( ineqty( tyla1, tyla2 ) )             /* equal types ? */
             { 
/*
***evaluate string >= string, result is boolean
*/
             valp->lit_type = C_BOL_VA;
            *tylap = stbolp;

            if ( strcmp( val1p->lit.str_va, val2p->lit.str_va ) >= 0 )
                valp->lit.bool_va = 1;
            else
                valp->lit.bool_va = 0 ;

            }
         else 
/*
***Error. Type missmatch for right operand, STRING desired 
*/
            { 
            return( erpush( "IN3103", " =, <> " ) );
            }

         break;

      default:   
/*
***Error. Ilegal type for Greater Equal and Less Then operator 
*/
         return( erpush( "IN3123", " >=, < " ) );
         break;
      }

   return( 0 );
  }
/********************************************************/
/*!******************************************************/

        short evleeq(
        PMLITVA  *val1p,
        pm_ptr    tyla1,
        PMLITVA  *val2p,
        pm_ptr    tyla2,
        PMLITVA  *valp,
        pm_ptr   *tylap)

/*      Evaluate Less Equal.
 *      The routine are also used for Greater Then == NOT Less Equal 
 *
 *      In:   *val1p  =>  C-pointer to left expression value
 *             tyla1  =>  Type pointer for      - " -          
 *            *val2p  =>  C-pointer to right expression value
 *             tyla2  =>  Type pointer for       - " -   
 *
 *      Out:  *valp   =>   Result value
 *            *tylap  =>   Result type pointer
 *
 *      FV:   return - error severity code
 *
 *      Error codes:  IN 308 Type missmatch for right operand, INT or FLOAT desired
 *                    IN 312 Ilegal types for operator ......
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STTYTBL type1;
/*
***read type info
*/
   strtyp( tyla1, & type1 );

   switch ( type1.kind_ty )
      { 
      case ST_INT:

         if ( ineqty( tyla1, tyla2 ) )               /* equal types ? */
            {  
/*
***evaluate integer GE, result is boolean
*/

            valp->lit_type = C_BOL_VA;
            *tylap = stbolp;

            valp->lit.bool_va = (v2bool)(val1p->lit.int_va <= 
                                         val2p->lit.int_va);

            }
         else if ( ineqty( tyla2, stflop ) )          /* if FLOAT */
            {  
/*
***evaluate integer EQ float, result is boolean
*/
          
            valp->lit_type = C_BOL_VA;
            *tylap = stbolp;

            valp->lit.bool_va = (v2bool)(val1p->lit.int_va <= 
                                         val2p->lit.float_va);

            }
         else  
/*
***Type missmatch for right operand, INT or FLOAT desired
*/
            {
            return( erpush( "IN3083", " <=, > " ) );
            }

         break;

      case ST_FLOAT:
         if ( ineqty( tyla1, tyla2 ) )               /* equal types ? */
            {  
/*
***evaluate float equal, result is boolean
*/
            valp->lit_type = C_BOL_VA;
            *tylap = stbolp;

            valp->lit.bool_va = (v2bool)(val1p->lit.float_va <= 
                                         val2p->lit.float_va);

            }
         else if ( ineqty( tyla2, stintp ) )          /* if INT */
            {
/*
***evaluate float <= integer, result is boolean
*/
            valp->lit_type = C_BOL_VA;
            *tylap = stbolp;

            valp->lit.bool_va = (v2bool)(val1p->lit.float_va <= 
                                         val2p->lit.int_va);
            }
         else  
/*
***Type missmatch for right operand, INT or FLOAT desired
*/
            { 
            return( erpush( "IN3083", " <=, > " ) );
            }

        break;

      case ST_STR:
         if ( ineqty( tyla1, tyla2 ) )                  /* equal types ? */
            {  
/*
***evaluate string <= string, result is boolean
*/
            valp->lit_type = C_BOL_VA;
            *tylap = stbolp;

            if ( strcmp( val1p->lit.str_va, val2p->lit.str_va ) <= 0 )
                valp->lit.bool_va = 1;
            else
                valp->lit.bool_va = 0 ;
            }
         else 
/*
***Error. Type missmatch for right operand, STRING desired 
*/
            {
            return( erpush( "IN3103", " =, <> " ) );
            }

         break;

      default:  
/*
***Error. Ilegal type for Less Equal and Greater Then operator 
*/

         return( erpush( "IN3123", " <=, > " ) );
         break;
      }

   return( 0 );
  } 
/********************************************************/
/*!******************************************************/

        short evand(
        pm_ptr  rexpr,
        PMLITVA  *val1p,
        pm_ptr    tyla1,
        PMLITVA  *valp,
        pm_ptr   *tylap)

/*      Evaluate AND.
 *
 *      In:    rexpr  =>   PM-pointer to right expression 
 *            *val1p  =>   C-pointer to left expression value
 *             tyla1  =>   Type pointer for      - " -          
 *
 *      Out:  *valp   =>   Result value
 *            *tylap  =>   Result type pointer
 *
 *      FV:   return - error severity code
 *
 *      Error codes:  IN 324 Ilegal types for logical operator 
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *
 ******************************************************!*/
  
  {
   PMLITVA val2;
   pm_ptr tyla2;
/*
***check type
*/
   if ( ! ineqty( tyla1, stbolp ) )
       return( erpush( "IN3243", "" ) );

/*
***Följande rad måste vara fel. C_BOL_VA är en typ inte
***ett värde. Ändrat 921102 JK.
valp->lit.bool_va = C_BOL_VA;
*/
   valp->lit_type = C_BOL_VA;

   *tylap = stbolp;

   if ( val1p->lit.bool_va )
      {
      inevex( rexpr, &val2, &tyla2 );
/*
***check type
*/
      if ( ! ineqty( tyla2, stbolp ) )
           return( erpush( "IN3243", "" ) );
      valp->lit.bool_va = val2.lit.bool_va;
      }
   else
      valp->lit.bool_va = FALSE;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short evor(
        pm_ptr  rexpr,
        PMLITVA  *val1p,
        pm_ptr    tyla1,
        PMLITVA  *valp,
        pm_ptr   *tylap)

/*      Evaluate OR.
 *
 *      In:    rexpr  =>   PM-pointer to right expression 
 *            *val1p  =>   C-pointer to left expression value
 *             tyla1  =>   Type pointer for      - " -          
 *
 *      Out:  *valp   =>   Result value
 *            *tylap  =>   Result type pointer
 *
 *      FV:   return - error severity code
 *
 *      Error codes:  IN 324 Ilegal types for logical operator 
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMLITVA val2;
   pm_ptr tyla2;
/*
***check type
*/
   if ( ! ineqty( tyla1, stbolp ) )
      return( erpush( "IN3243", "" ) );
/*
***Följande rad måste vara fel. C_BOL_VA är en typ inte
***ett värde. Ändrat 921102 JK.
valp->lit.bool_va = C_BOL_VA;
*/
   valp->lit_type = C_BOL_VA;

   *tylap = stbolp;

   if ( val1p->lit.bool_va )
      { 
/*
***result is true
*/
      valp->lit.bool_va = TRUE;
      }
   else
      {
      inevex( rexpr, &val2, &tyla2 );
/*
***check type
*/
      if ( ! ineqty( tyla2, stbolp ) )
         return( erpush( "IN3243", "" ) );
      valp->lit.bool_va = val2.lit.bool_va;
      }

   return( 0 );
  }

/********************************************************/
