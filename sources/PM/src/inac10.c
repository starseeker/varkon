/**********************************************************************
*
*    inac10.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes the following routines:
*
*    short inevex() -  evaluate expression ( can retun an address )
*    short inevev() -  evaluate expression value
*    short evplus() -  evaluate plus oparator
*    short evminu() -  evaluate minus oparator
*    short evunmi() -  evaluate unary minus
*    short evmul()  -  evaluate multiply oparator
*    short evdiv()  -  evaluate division oparator
*    short evexp()  -  evaluate exponential oparator
*    
*    internal routines:
*       short calind()  -  calculate indexed offset address
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

#ifdef DEBUG
#include "../../IG/include/debug.h"
#endif

#include <signal.h>
#include <setjmp.h>
#include <math.h>
#include <string.h>

static short evplus(PMLITVA *val1p, pm_ptr tyla1, PMLITVA *val2p,
                    pm_ptr tyla2, PMLITVA *valp, pm_ptr *tylap);
static short evminu(PMLITVA *val1p, pm_ptr tyla1, PMLITVA *val2p,
                    pm_ptr tyla2, PMLITVA *valp, pm_ptr *tylap);
static short evunmi( PMLITVA *valp, pm_ptr *tylap);
static short evmul(PMLITVA *val1p, pm_ptr tyla1, PMLITVA *val2p,
                    pm_ptr tyla2, PMLITVA *valp, pm_ptr *tylap);
static short evdiv(PMLITVA *val1p, pm_ptr tyla1, PMLITVA *val2p,
                    pm_ptr tyla2, PMLITVA *valp, pm_ptr *tylap);
static short evexp(PMLITVA *val1p, pm_ptr tyla1, PMLITVA *val2p,
                    pm_ptr tyla2, PMLITVA *valp, pm_ptr *tylap);

#ifndef ANALYZER
static DBint calind(pm_ptr indlist, pm_ptr *typelap);
static void  fptrap(int sigval);
#endif

jmp_buf env;                  /* environment-buffer för setjump och longjump */

/*
***standard types
*/
extern pm_ptr  stintp;
extern pm_ptr  stflop;
extern pm_ptr  ststrp;
extern pm_ptr  stvecp;
extern pm_ptr  strefp;
extern pm_ptr  stbolp;

extern short modtyp;

/*!******************************************************/
/*!******************************************************/

        short inevex(
        pm_ptr exprla, 
        PMLITVA *valp,
        pm_ptr *tylap)

/*      Evaluate expression.
 *
 *
 *      In:  exprla =>    PM-pointer to expression node
 *
 *      Out: *valp  =>    C-pointer to an literal structure
 *           *tylap =>    C-pointer to an type pointer
 *
 *      FV:  return - error severity code.
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-11 Rewritten, R. Svedin
 *      2007-04-04 Bugfix, J.Kjellander
 *
 ******************************************************!*/

  {
   PMLITVA val1, val2;       /* calculated values */
   pm_ptr tyla1, tyla2;      /* types for calculated values */

   PMEXNO   *np;             /* c-pointer to expression node */
   PMFUNCEX *funcp;          /* c-pointr to func_exper node */
   PMUNEX   *unp;            /* c-pointer to unary op exper node */
   PMBINEX  *binp;           /* c-pointer to binop_exper node */
   PMLITEX  *litp;           /* c-pointer to lit_exper node */
   PMIDEX   *idp;            /* c-pointer to id_expr node */
   PMCOMPEX *comp;           /* c-pointer to comp_expr node */
   PMREFEX  *refp;           /* c-pointer to ref. value in PM */
   V2REFVA  *valrefp, *rp;
   PMVECVA  *vecp;           /* c-pointer to vector value */
   short     status;
   char     *str;
   int       i,k;            /* index variable */
   stidcl    idkind;         /* attributer from symbol table */
   char     *idname;         /* char pointer for stratt() */
   STCONST   konst;          /* interface struct for variable info */

#ifndef ANALYZER
   PMINDEX *indp;            /* c-pointer to index_exper node */
   STVAR    var;             /*      - " -       variable */
   DBint    arroffs;         /* array offset */
   DBint    arrbase=0;       /* RTS offset for array base */
   PMLITVA  arrval;          /* used for array value */
   pm_ptr   typela;          /* PM-pointer to type info in Symbol table */
   v2addr   compsize;
   v2addr   compoffs=0;
#endif
/*
*** begining of inevex()
*/

   np = (PMEXNO *)pmgadr(exprla);

/*
***Error Not a expression node
*/
  if ( np->noclex != EXPR )
     {
     return( erpush( "IN2522", "" ) );
     }

switch ( np->suclex )
  {
  case C_UN_EX:

      unp = &(np->ex_subcl.unop_ex);
/* 
***evaluate expression value 
*/
      if ( ( status = inevev( unp->p_unex, valp, tylap ) ) < -1 )
          return( status );

      switch ( unp->unop )
        {
        case PM_NOT:
/* 
***check type
*/
            if ( *tylap != stbolp )
                return( erpush( "IN3243", "" ) );
            valp->lit.bool_va = ! valp->lit.bool_va;

            break;

        case PM_MINUS:
/*
***evaluate unary minus
*/
            if ((status = evunmi( valp, tylap )) < -1)
                return( status );

            break;
/*
***Error
*/
        default:  

            return( erpush( "IN3093", "" ) );
            break;

        }

        break;

  case C_BIN_EX:

      binp = &(np->ex_subcl.binop_ex);

/*
***evaluate left expression value
*/
      if ( ( status = inevev( binp->p_bin_l, &val1, &tyla1 ) ) < -1 )
          return( status );

#ifndef ANALYZER

      if ( binp->binop == PM_AND )
          {
          if ((status = evand(binp->p_bin_r, &val1, tyla1, valp, tylap)) < -1)
              return( status );
          }
      else if ( binp->binop == PM_OR )
          {
          if ((status = evor(binp->p_bin_r, &val1, tyla1, valp, tylap)) < -1)
              return( status );
          }
      else

#endif
          {
/*
***evaluate right expression value
*/
          if ( ( status = inevev( binp->p_bin_r, &val2, &tyla2 ) ) < -1 )
              return( status );


          switch ( binp->binop )
            {
#ifndef ANALYZER

            case PM_GT:
/*
***evaluat GT ( NOT LE )
*/
                if ((status= evleeq(&val1,tyla1,&val2,tyla2,valp,tylap))< -1)
                    return( status );
                valp->lit.bool_va = ! valp->lit.bool_va;
                break;

            case PM_GE:
/*
***evaluat GE
*/
                if ((status= evgreq(&val1,tyla1,&val2,tyla2,valp,tylap))< -1)
                    return( status );
                break;

            case PM_EQ:
/*
***evaluat EQ
*/
                if ((status= evequa(&val1,tyla1,&val2,tyla2,valp,tylap))< -1)
                    return( status );
                break;

            case PM_NE:
/*
***evaluat NE ( NOT EQ )
*/
                if ((status= evequa(&val1,tyla1,&val2,tyla2,valp,tylap))< -1)
                    return( status );
                valp->lit.bool_va = ! valp->lit.bool_va;
                break;

            case PM_LT:
/*
evaluat LT ( NOT GE )
*/
                if ((status= evgreq(&val1,tyla1,&val2,tyla2,valp,tylap))< -1)
                    return( status );
                valp->lit.bool_va = ! valp->lit.bool_va;
                break;

            case PM_LE:
/*
***evaluat LE
*/
                if ((status= evleeq(&val1,tyla1,&val2,tyla2,valp,tylap))< -1)
                    return( status );
                break;

#endif

/*
***Följande operationer, ( + - * / ** ) ändrade map. felhantering.
***862310-JK
*/
/*
***Plus.
*/
            case PM_PLUS:
#ifdef TA_BORT_KANSKE
            signal(SIGFPE,fptrap);
            if ( setjmp(env) < 0 ) return(erpush("IN2202",""));
#endif
                if ((status= evplus(&val1,tyla1,&val2,tyla2,valp,tylap)) <-1)
                    return( status );
#ifdef TA_BORT_KANSKE
            signal(SIGFPE,SIG_DFL);
#endif
            break;
/*
***Minus.
*/
            case PM_MINUS:
#ifdef TA_BORT_KANSKE
            signal(SIGFPE,fptrap);
            if ( setjmp(env) < 0 ) return(erpush("IN2212",""));
#endif
                if ((status= evminu(&val1,tyla1,&val2,tyla2,valp,tylap)) <-1)
                    return( status );
#ifdef TA_BORT_KANSKE
            signal(SIGFPE,SIG_DFL);
#endif
            break;
/*
***Multiplikation.
*/            
            case PM_MUL:
#ifdef TA_BORT_KANSKE
            signal(SIGFPE,fptrap);
            if ( setjmp(env) < 0 ) return(erpush("IN2222",""));
#endif
                if ((status= evmul(&val1,tyla1,&val2,tyla2,valp,tylap)) < -1)
                    return( status );
#ifdef TA_BORT_KANSKE
            signal(SIGFPE,SIG_DFL);
#endif
            break;
/*
***Division.
*/            
            case PM_DIV:
#ifdef TA_BORT_KANSKE
            signal(SIGFPE,fptrap);
            if ( setjmp(env) < 0 ) return(erpush("IN2232",""));
#endif
            if ( (status=evdiv(&val1,tyla1,&val2,tyla2,valp,tylap)) < -1)
                  return( status );
#ifdef TA_BORT_KANSKE
            signal(SIGFPE,SIG_DFL);
#endif
            break;
/*
***Exponentiering.
*/            
            case PM_EXP:
#ifdef TA_BORT_KANSKE
            signal(SIGFPE,fptrap);
            if ( setjmp(env) < 0 ) return(erpush("IN2242",""));
#endif
                if ((status= evexp(&val1,tyla1,&val2,tyla2,valp,tylap)) < -1)
                    return( status );
#ifdef TA_BORT_KANSKE
            signal(SIGFPE,SIG_DFL);
#endif
            break;
/*
***Okänd operation.
*/            
            default:  /* Error */
            return( erpush( "IN3093", "" ) );
            break;
            }
          }
       break;

  case C_LIT_EX:

      litp = &(np->ex_subcl.lit_ex);

      switch ( litp->lit_type )
        {
        case C_INT_VA:
            *tylap = stintp;

            valp->lit_type = C_INT_VA;
            valp->lit.int_va = litp->litex.int_li;
            break;

        case C_FLO_VA:
            *tylap = stflop;

            valp->lit_type = C_FLO_VA;
            valp->lit.float_va = litp->litex.float_li;
            break;

        case C_STR_VA:
            str = pmgadr(litp->litex.str_li);
           *tylap = ststrp;
            valp->lit_type = C_STR_VA;
            strcpy( valp->lit.str_va, str );
            break;

        case C_VEC_VA:
            vecp = (PMVECVA *)pmgadr(litp->litex.vec_li);
           *tylap = stvecp;
            valp->lit_type = C_VEC_VA;
            valp->lit.vec_va.x_val = vecp->x_val;
            valp->lit.vec_va.y_val = vecp->y_val;
            valp->lit.vec_va.z_val = vecp->z_val;
            break;

        case C_REF_VA:

            i = 0;

            refp = &litp->litex.ref_li;

            *tylap = strefp;

            valp->lit_type = C_REF_VA;

            valrefp = &valp->lit.ref_va[ i ];

            valrefp->seq_val = refp->seq_lit;
            valrefp->ord_val = refp->ord_lit;

            while ( refp->nextref != 0 )
                {
                refp = (PMREFEX *)pmgadr(refp->nextref);
/*
***Get pointer to next ref. in "valp" and check chain length
*/
                if ( ++i >= MXINIV )
                    return( erpush( "IN0074", "inevex()" ) );
                rp = &valp->lit.ref_va[ i ];

               
                valrefp->p_nextre = rp;

                valrefp = rp;

                valrefp->seq_val = refp->seq_lit;
                valrefp->ord_val = refp->ord_lit;

                }

            valrefp->p_nextre = NULL;
            
            break;
        }

      break;

  case C_ID_EX:

      idp = &( np->ex_subcl.id_ex );
      
/*
***read variable attributes
*/
      stratt( idp->p_id, &idkind, &idname );

/*
***check kind of ID
*/
      switch( idkind )
        {

#ifndef ANALYZER      
/*
***Don't compile if analyser
*/

        case ST_VAR:
     
/*
***read variable/parameter info
*/
        strvar( np->ex_subcl.id_ex.p_id, &var );

/*
***set return type for variable/parameter
*/
        *tylap = var.type_va;

/*
***check variable class
*/
        switch( var.kind_va )
          {
          case ST_LOCVA:
          case ST_VPAVA:

/*
***Calculate the offset address in  RTS and set return value
*/
              valp->lit_type = C_ADR_VA;
              valp->lit.adr_va = var.addr_va;
            
              break;

          case ST_RPAVA:

/*
***get the address value from RTS
*/
              ingval( var.addr_va, *tylap , TRUE, valp );

              break;

          case ST_GLOVA:

/*
***Calculate the offset address in  RTS and set return value
*/
              valp->lit_type = C_ADR_VA;
              valp->lit.adr_va = inggrb() - ingrtb() + var.addr_va;
            
              break;

          }

        break;

#endif

        case ST_CONST:
            strcon( idp->p_id, &konst );

/*
***set return type
*/
            *tylap = konst.type_co;

/*
***get value from symbol table
*/
            V3MOME((char *)&konst.valu_co,(char *)valp,sizeof(PMLITVA));
/*
***Om det är en referens måste pekarna sättas rätt.
*/
            if ( valp->lit_type == C_REF_VA )
              {
              k = 0;
              while ( valp->lit.ref_va[k].p_nextre != NULL )
                {
                ++k;
/*
***I samband med byte av OS till ODT 3.0 slutade följande rad att 
***fungera.
*
     valp->lit.ref_vaÄkÅ.p_nextre = &valp->lit.ref_vaÄ++kÅ;
*
***Tjusig kanske men inte helt entydig och kanske svår för optimizern ?
***Ersattes med följande.
*/
                valp->lit.ref_va[k-1].p_nextre = &valp->lit.ref_va[k];
                }
              }
 
            break;

        default:
            return( erpush( "IN0024", "" ) );
            break;
      
        }

      break;

#ifndef ANALYZER
/*
***Don't compile if analyser
*/

  case C_IND_EX:

      indp = &( np->ex_subcl.ind_ex );

/*
***read variable info.
*/
      strvar( indp->p_indid, &var );

/*
***check variable class and get RTS offset for array base
*/
      switch( var.kind_va )
        {
        case ST_LOCVA:
        case ST_VPAVA:

            arrbase = var.addr_va;
            break;

        case ST_RPAVA:
    
            ingval( var.addr_va, var.type_va, TRUE, &arrval );

            if ( arrval.lit_type != C_ADR_VA )
/*
***Error
*/
                { 
                erpush( "IN0034", "" ); /* array base not address */
                }

            arrbase = arrval.lit.adr_va;
            break;

        }

/*
***calculate array offset from index and get return type
*/
      typela = var.type_va;
      if ( ( arroffs = calind( indp->p_indli, &typela ) ) < 0 )
/*
***Error
*/
          {
          switch( arroffs )
            {
            case -1: /* array index out of bound */
                return( erpush( "IN3223", "" ) );
                break;
            case -2: /* error calculating array offset */
                return( erpush( "IN0044", "" ) );
                break;
            }
          }

/*
***Read return type info and set return type
*/
      *tylap = typela;

/*
***Calculate the offset address in RTS and set return value
*/
      valp->lit_type = C_ADR_VA;
      valp->lit.adr_va = arrbase + arroffs;

      break;

#endif

  case C_COM_EX:
      comp = &(np->ex_subcl.comp_ex);

/*
***Don't compile if analyser
*/
#ifndef ANALYZER

/*
***Evaluate component variable. Status check, bugfix 2007-04-04 J.Kjellander
*/
      if ( (status=inevex(comp->p_comvar,&val1,&tyla1)) < 0 ) return(status);
/*
***check if address
*/
      if ( val1.lit_type != C_ADR_VA )

/*
***Ilegal component variable address
*/

          {
          return(erpush("IN0084","inevex()"));
          }  

/*
***calculate address to component and set return value and type
*/
      compsize = v2vecsz / 3;
      switch( comp->p_cfield )
        {
        case PM_X:
            compoffs = 0;
            break;
        case PM_Y:
            compoffs = 1;
            break;
        case PM_Z:
            compoffs = 2;
            break;
        }
      valp->lit_type = C_ADR_VA;
      valp->lit.adr_va = val1.lit.adr_va + compsize * compoffs;
      *tylap = stflop;

#endif
      
      break;

  case C_FUN_EX:
      funcp = &(np->ex_subcl.func_ex);

/*
***evaluate MBS function
*/
      if ((status = inevfu(funcp->funcid, funcp->p_funcar, valp, tylap)) != 0 )
          return( status );

      break;

  default:
      return( erpush( "IN2522", "" ) );
      break;

  }

  return( 0 );
  }  

/********************************************************/
/*!******************************************************/

        short inevev(
        pm_ptr exprla,       
        PMLITVA *valp,       
        pm_ptr *tylap)       

/*      Evaluate expression value.
 *      This routine calls inevex() and if the result is an address also 
 *      calls ingval(). The routine always returns a value in PMLITVA.
 *
 *      In:  exprla =>    PM-pointer to expression node
 *
 *      Out: *valp  =>    C-pointer to an literal structure
 *           *tylap =>    C-pointer to an type pointer
 *
 *      FV:  return - error severity code.
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-11 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short status;

   if ( ( status = inevex( exprla, valp, tylap ) ) < -1 )
      return( status );

#ifndef ANALYZER

   if ( valp->lit_type == C_ADR_VA )
      ingval( valp->lit.adr_va, *tylap, FALSE,  valp );

#endif

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short evplus(
        PMLITVA  *val1p,
        pm_ptr   tyla1,
        PMLITVA  *val2p,     
        pm_ptr   tyla2,    
        PMLITVA  *valp,      
        pm_ptr   *tylap)     

/*      Evaluate Plus.
 *
 *      In:  *val1p =>    C-pointer to left expression value
 *            tyla1 =>    Type pointer for     - " -      
 *           *val2p =>    C-pointer to right expression value 
 *            tyla2 =>    Type pointer for       - " -          
 *
 *      Out: *valp  =>    Result value
 *           *tylap =>    Result type pointer
 *
 *      FV:  return - error severity code.
 *
 *      Error codes:  IN 308 Type missmatch for right operand, INT or FLOAT desired
 *                    IN 310            - " -                , STRING desired
 *                    IN 311            - " -                , VECTOR  - " -
 *                    IN 312 Ilegal types for operator ......
 *
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-11 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
/*
***temp string for string concatenate
*/
   char  tmpstr[ 2 * V3STRLEN + 2 ];

   STTYTBL type1;

   strtyp( tyla1, &type1 );

   switch ( type1.kind_ty )
      { 
      case ST_INT:
/*
***equal types ? evaluate integer plus, result is integer
*/
         if ( ineqty( tyla1, tyla2 ) )
            {  
            valp->lit_type = C_INT_VA;
            *tylap = stintp;

            valp->lit.int_va = val1p->lit.int_va + val2p->lit.int_va;
            }
/*
***if FLOAT evaluate integer + float, result is float
*/
         else if ( ineqty( tyla2, stflop ) )
            {  
            valp->lit_type = C_FLO_VA;
            *tylap = stflop;

            valp->lit.float_va = val1p->lit.int_va + val2p->lit.float_va;
            }
/*
***Type missmatch for right operand, INT or FLOAT desired
*/
          else  
            { 
            return( erpush( "IN3083", " + " ) );
            }
         break;

      case ST_FLOAT:

/*
***equal types ? evaluate float plus, result is float
*/
         if ( ineqty( tyla1, tyla2 ) ) 
            {  
            valp->lit_type = C_FLO_VA;
            *tylap = stflop;

            valp->lit.float_va = val1p->lit.float_va + val2p->lit.float_va;
            }
/*
***if INT, evaluate float + integer, result is float
*/
         else if ( ineqty( tyla2, stintp ) )
            {          
            valp->lit_type = C_FLO_VA;
            *tylap = stflop;

            valp->lit.float_va = val1p->lit.float_va + val2p->lit.int_va;
            }
/*
***Type missmatch for right operand, INT or FLOAT desired
*/
         else  
            { 
            return( erpush( "IN3083", " + " ) );
            }

        break;

      case ST_STR:

/*
***evaluate string concatenate
*/
         if ( ineqty( tyla1, tyla2 ) )
            {  
            valp->lit_type = C_STR_VA;
            *tylap = ststrp;

            strcpy( tmpstr, val1p->lit.str_va );
            strcat( tmpstr, val2p->lit.str_va );
            *( tmpstr+( sizeof( valp->lit.str_va ) - 1 ) ) = '\0';
            strcpy( valp->lit.str_va, tmpstr );
            }
         else
            return( erpush( "IN3103", " + " ) );

         break;	

      case ST_VEC:
/*
***equal types ? evaluate vector plus, result is vector
*/
         if ( ineqty( tyla1, tyla2 ) )  
            {  
            valp->lit_type = C_VEC_VA;
            *tylap = stvecp;

             valp->lit.vec_va.x_val=val1p->lit.vec_va.x_val+val2p->lit.vec_va.x_val;
             valp->lit.vec_va.y_val=val1p->lit.vec_va.y_val+val2p->lit.vec_va.y_val;

             if ( modtyp == 3 ) valp->lit.vec_va.z_val=val1p->lit.vec_va.z_val+
                                                            val2p->lit.vec_va.z_val;
             else valp->lit.vec_va.z_val = 0.0;
            }
/*
***Type missmatch for right operand, VECTOR desired
*/
         else 
            {  
            return( erpush( "IN3113", " + " ) );
            }

         break;

      default:
/*
***Ilegal type for Plus operator
*/
         return( erpush( "IN3123", " + " ) ); 
         break;
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short evunmi(
        PMLITVA *valp,       
        pm_ptr *tylap)       

/*      Evaluate Unary Minus.
 *
 *      In:  *valp =>     PM-pointer to expression value
 *           *tylap =>    type info for      - " -      
 *
 *      Out: *valp  =>    result value                 
 *           *tylap =>    result type
 *
 *      FV:  return - error severity code.
 *
 *      Error codes:  IN 312 Ilegal types for operator ......
 *
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-11 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STTYTBL type;
/* 
***reade type info
*/
   strtyp( *tylap, &type );

   switch ( type.kind_ty )
      { 
      case ST_INT:
/*
***evaluate unary integer minus
*/
         valp->lit.int_va = - ( valp->lit.int_va );
         break;

      case ST_FLOAT:
/*
***evaluate unary float minus
*/
         valp->lit.float_va = - ( valp->lit.float_va );
         break;

      case ST_VEC:
 /*
***evaluate unary vector minus
*/
         valp->lit.vec_va.x_val = - ( valp->lit.vec_va.x_val );
         valp->lit.vec_va.y_val = - ( valp->lit.vec_va.y_val );

         if ( modtyp == 3 ) valp->lit.vec_va.z_val = - ( valp->lit.vec_va.z_val );
         else valp->lit.vec_va.z_val = 0.0;

         break;

      default:  
 /*
***Ilegal type for Minus operator
*/
         return( erpush( "IN3123", " - " ) );
         break;
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short evminu(
        PMLITVA  *val1p,
        pm_ptr   tyla1,
        PMLITVA  *val2p,     
        pm_ptr   tyla2,    
        PMLITVA  *valp,      
        pm_ptr   *tylap)     

/*      Evaluate Minus .
 *
 *      In:  *val1p =>    C-pointer to left expression value
 *            tyla1 =>    Type pointer for     - " -      
 *           *val2p =>    C-pointer to right expression value 
 *            tyla2 =>    Type pointer for       - " -          
 *
 *      Out: *valp  =>    Result value
 *           *tylap =>    Result type pointer
 *
 *      FV:  return - error severity code.
 *
 *      Error codes:  IN 308 Type missmatch for right operand, INT or FLOAT desired
 *                    IN 311            - " -                , VECTOR  - " -
 *                    IN 312 Ilegal types for operator ......
 *
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-11 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STTYTBL type1;

#ifdef DEBUG
   if ( dbglev(PMPAC) == 10 )
      {
      fprintf(dbgfil(PMPAC),"***Start-evminu***\n");
      fflush(dbgfil(PMPAC));
      }
#endif

/* 
***read type info
*/
   strtyp( tyla1, &type1 );

   switch ( type1.kind_ty )
      { 
      case ST_INT:
/*
***equal types ? evaluate integer minus, result is integer
*/
         if ( ineqty( tyla1, tyla2 ) )  
            {
            valp->lit_type = C_INT_VA;
            *tylap = stintp;

            valp->lit.int_va = val1p->lit.int_va - val2p->lit.int_va;

#ifdef DEBUG
            if ( dbglev(PMPAC) == 10 )
               {
               fprintf(dbgfil(PMPAC),"INT - INT, %d - %d\n",val1p->lit.int_va,
                                                            val2p->lit.int_va);
                fprintf(dbgfil(PMPAC),"Resultat = %d\n",valp->lit.int_va);
                fflush(dbgfil(PMPAC));
               }
#endif
            }
/*
***if FLOAT, evaluate integer - float, result is float
*/
         else if ( ineqty( tyla2, stflop ) )  
            {
            valp->lit_type = C_FLO_VA;
           *tylap = stflop;
            valp->lit.float_va = val1p->lit.int_va - val2p->lit.float_va;
            }
/*
***Type missmatch for right operand, INT or FLOAT desired
*/
         else  
            { 
            return( erpush( "IN3083", " - " ) );
            }

         break;

      case ST_FLOAT:
/*
***equal types ? evaluate float minus, result is float
*/
         if ( ineqty( tyla1, tyla2 ) ) 
            {  
            valp->lit_type = C_FLO_VA;
            *tylap = stflop;

            valp->lit.float_va = val1p->lit.float_va - val2p->lit.float_va;

#ifdef DEBUG
            if ( dbglev(PMPAC) == 10 )
               {
               fprintf(dbgfil(PMPAC),"FLOAT - FLOAT, %g - %g\n",val1p->lit.float_va,
                                                                val2p->lit.float_va);
               fprintf(dbgfil(PMPAC),"Resultat = %g\n",valp->lit.float_va);
               fflush(dbgfil(PMPAC));
               }
#endif
            }
/*
***if INT , evaluate float - integer, result is float
*/
         else if ( ineqty( tyla2, stintp ) )  
            {  
            valp->lit_type = C_FLO_VA;
            *tylap = stflop;

            valp->lit.float_va = val1p->lit.float_va - val2p->lit.int_va;

#ifdef DEBUG
            if ( dbglev(PMPAC) == 10 )
               {
               fprintf(dbgfil(PMPAC),"FLOAT - INT, %g - %d\n",val1p->lit.float_va,
                                                              val2p->lit.int_va);
               fprintf(dbgfil(PMPAC),"Resultat = %g\n",valp->lit.float_va);
               fflush(dbgfil(PMPAC));
               }
#endif
            }
/*
***Type missmatch for right operand, INT or FLOAT desired
*/
         else  
            {
            return( erpush( "IN3083", " - " ) );
            }

         break;

      case ST_VEC:
/*
***equal types ? evaluate vector minus, result is vector 
*/
         if ( ineqty( tyla1, tyla2 ) )  
            {  
            valp->lit_type = C_VEC_VA;
            *tylap = stvecp;

             valp->lit.vec_va.x_val=val1p->lit.vec_va.x_val-val2p->lit.vec_va.x_val;
             valp->lit.vec_va.y_val=val1p->lit.vec_va.y_val-val2p->lit.vec_va.y_val;

             if ( modtyp == 3 ) valp->lit.vec_va.z_val=val1p->lit.vec_va.z_val-
                                                        val2p->lit.vec_va.z_val;
             else valp->lit.vec_va.z_val = 0.0;
            }
/*
***Type missmatch for right operand, INT or FLOAT desired
*/
         else 
            {
            return( erpush( "IN3113", " - " ) );
            }

         break;

      default:
/*
***Ilegal type for Minus operator
*/
         return( erpush( "IN3123", " - " ) );
         break;
      }

#ifdef DEBUG
    if ( dbglev(PMPAC) == 10 )
      {
      fprintf(dbgfil(PMPAC),"Resultatets typ = %d\n",valp->lit_type);
      fprintf(dbgfil(PMPAC),"***Slut-evminu***\n");
      fflush(dbgfil(PMPAC));
      }
#endif

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short evmul(
        PMLITVA  *val1p,
        pm_ptr   tyla1,
        PMLITVA  *val2p,     
        pm_ptr   tyla2,    
        PMLITVA  *valp,      
        pm_ptr   *tylap)     


/*      Evaluate Multiply .
 *
 *      In:  *val1p =>    C-pointer to left expression value
 *            tyla1 =>    Type pointer for     - " -      
 *           *val2p =>    C-pointer to right expression value 
 *            tyla2 =>    Type pointer for       - " -          
 *
 *      Out: *valp  =>    Result value
 *           *tylap =>    Result type pointer
 *
 *      FV:  return - error severity code.
 *
 *      Error codes:  IN 308 Type missmatch for right operand, INT or FLOAT desired
 *                    IN 311            - " -                , VECTOR  - " -
 *                    IN 312 Ilegal types for operator ......
 *                    IN 313 Type missmatch for right operand, INT, FLOAT or VECTOR
 *                                                                          desired
 *
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-11 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STTYTBL type1;

   strtyp( tyla1, &type1 );

switch ( type1.kind_ty )
  { 
  case ST_INT:

/*
***equal types ? evaluate integer multiply, result is integer
*/

      if ( ineqty( tyla1, tyla2 ) )  
          {  
          valp->lit_type = C_INT_VA;
          *tylap = stintp;

          valp->lit.int_va = val1p->lit.int_va * val2p->lit.int_va;
          }

/*
***if FLOAT, evaluate integer * float, result is float
*/
      else if ( ineqty( tyla2, stflop ) )  
          {
                    valp->lit_type = C_FLO_VA;
          *tylap = stflop;

          valp->lit.float_va = val1p->lit.int_va * val2p->lit.float_va;
          }

/*
***if VECTOR, evaluate integer * vector, result is vector
*/
      else if ( ineqty( tyla2, stvecp ) )  
          {  
          valp->lit_type = C_VEC_VA;
          *tylap = stvecp;

            valp->lit.vec_va.x_val=val1p->lit.int_va * val2p->lit.vec_va.x_val;
            valp->lit.vec_va.y_val=val1p->lit.int_va * val2p->lit.vec_va.y_val;

            if ( modtyp == 3 ) valp->lit.vec_va.z_val=val1p->lit.int_va *
                                                       val2p->lit.vec_va.z_val;
            else valp->lit.vec_va.z_val = 0.0;
          }

/*
***Type missmatch for right operand, INT, FLOAT or VECTOR desired
*/
      else  
          { 
          return( erpush( "IN3133", " * " ) );
          }

      break;

  case ST_FLOAT:

/*
***equal types ? evaluate float multiply, result is float
*/

      if ( ineqty( tyla1, tyla2 ) ) 
          {  
          valp->lit_type = C_FLO_VA;
          *tylap = stflop;

          valp->lit.float_va = val1p->lit.float_va * val2p->lit.float_va;
          }

/*
***if INT, evaluate float * integer, result is float
*/
      else if ( ineqty( tyla2, stintp ) ) 
          { 
          valp->lit_type = C_FLO_VA;
          *tylap = stflop;

          valp->lit.float_va = val1p->lit.float_va * val2p->lit.int_va;
          }

/*
***if VECTOR , evaluate float * vector, result is vector
*/
      else if ( ineqty( tyla2, stvecp ) )  
          {  
          valp->lit_type = C_VEC_VA;
          *tylap = stvecp;

          valp->lit.vec_va.x_val=val1p->lit.float_va * val2p->lit.vec_va.x_val;
          valp->lit.vec_va.y_val=val1p->lit.float_va * val2p->lit.vec_va.y_val;

          if ( modtyp == 3 ) valp->lit.vec_va.z_val=val1p->lit.float_va *
                                                       val2p->lit.vec_va.z_val;
          else valp->lit.vec_va.z_val = 0.0;
          }
/*
***Type missmatch for right operand, INT, FLOAT or VECTOR desired
*/
      else  
          { 
          return( erpush( "IN3083", " * " ) );
          }

     break;

  case ST_VEC:

/*
***if INT , evaluate vector * integer , result is vector
*/
      if ( ineqty( tyla2, stintp ) )  
          {  
          valp->lit_type = C_VEC_VA;
          *tylap = stvecp;

            valp->lit.vec_va.x_val=val1p->lit.vec_va.x_val * val2p->lit.int_va;
            valp->lit.vec_va.y_val=val1p->lit.vec_va.y_val * val2p->lit.int_va;

            if ( modtyp == 3 ) valp->lit.vec_va.z_val=val1p->lit.vec_va.z_val*
                                                             val2p->lit.int_va;
            else valp->lit.vec_va.z_val = 0.0;
          }

/*
***if FLOAT , evaluate vector * float , result is vector
*/
      else if ( ineqty( tyla2, stflop ) )  
          {  
          valp->lit_type = C_VEC_VA;
          *tylap = stvecp;

          valp->lit.vec_va.x_val=val1p->lit.vec_va.x_val * val2p->lit.float_va;
          valp->lit.vec_va.y_val=val1p->lit.vec_va.y_val * val2p->lit.float_va;

          if ( modtyp == 3 ) valp->lit.vec_va.z_val=val1p->lit.vec_va.z_val *
                                                           val2p->lit.float_va;
          else valp->lit.vec_va.z_val = 0.0;
          }

/*
***if VECTOR , evaluate vector * vector , result is float
*/
      else if ( ineqty( tyla2, stvecp ) )  
          {  
          valp->lit_type = C_FLO_VA;
          *tylap = stflop;

          if ( modtyp == 2 )
            valp->lit.float_va = 
                  val1p->lit.vec_va.x_val * val2p->lit.vec_va.x_val +
                  val1p->lit.vec_va.y_val * val2p->lit.vec_va.y_val;
          else
            valp->lit.float_va = 
                  val1p->lit.vec_va.x_val * val2p->lit.vec_va.x_val +
                  val1p->lit.vec_va.y_val * val2p->lit.vec_va.y_val +
                  val1p->lit.vec_va.z_val * val2p->lit.vec_va.z_val;
          }

/*
***Type missmatch for right operand, INT, FLOAT or VECTOR desired
*/
      else 
          {  
          return( erpush( "IN3083", " * " ) );
          }

      break;

/*
***Ilegal type for Mul operator
*/
  default:   
 
      return( erpush( "IN3123", " * " ) ); 
      break;
  }

  return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short evdiv(
        PMLITVA  *val1p,
        pm_ptr   tyla1,
        PMLITVA  *val2p,     
        pm_ptr   tyla2,    
        PMLITVA  *valp,      
        pm_ptr   *tylap)     


/*      Evaluate Division.
 *
 *      In:  *val1p =>    C-pointer to left expression value
 *            tyla1 =>    Type pointer for     - " -      
 *           *val2p =>    C-pointer to right expression value 
 *            tyla2 =>    Type pointer for       - " -          
 *
 *      Out: *valp  =>    Result value
 *           *tylap =>    Result type pointer
 *
 *      FV:  return - error severity code.
 *
 *      Error codes:  IN 308 Type missmatch for right operand, INT or FLOAT desired
 *                    IN 312 Ilegal types for operator ......
 *
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-11 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STTYTBL type1;
/*
***preper result type info., result type allways FLOAT
*/
   valp->lit_type = C_FLO_VA;
   *tylap = stflop;
/* 
***read type info
*/
   strtyp( tyla1, &type1 );

switch ( type1.kind_ty )
  { 
  case ST_INT:

/*
***equal types ? evaluate integer/integer , result is float
*/
      if ( ineqty( tyla1, tyla2 ) )  
          { 
       valp->lit.float_va=(double)val1p->lit.int_va/(double)val2p->lit.int_va;
          }

/*
***if FLOAT , evaluate integer / float, result is float
*/
      else if ( ineqty( tyla2, stflop ) )  
          {  
          valp->lit.float_va = val1p->lit.int_va / val2p->lit.float_va;
          }

/*
***Type missmatch for right operand, INT or FLOAT desired
*/
      else  
          { 
          return( erpush( "IN3083", " / " ) );
          }

      break;

  case ST_FLOAT:

/*
***equal types ? evaluate float division, result is float
*/
      if ( ineqty( tyla1, tyla2 ) ) 
          {  
          valp->lit.float_va = val1p->lit.float_va / val2p->lit.float_va;
          }

/*
***if INT, evaluate float / integer, result is float
*/
      else if ( ineqty( tyla2, stintp ) )  
          {  
          valp->lit.float_va = val1p->lit.float_va / val2p->lit.int_va;
          }

/*
***Type missmatch for right operand, INT or FLOAT desired
*/
      else  
          { 
          return( erpush( "IN3083", " / " ) );
          }

     break;

/*
***Ilegal type for Div operator
*/
  default:
 
      return( erpush( "IN3123", " / " ) );
      break;
  }

  return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short evexp(
        PMLITVA  *val1p,
        pm_ptr   tyla1,
        PMLITVA  *val2p,     
        pm_ptr   tyla2,    
        PMLITVA  *valp,      
        pm_ptr   *tylap)     

/*      Evaluate power  .
 *
 *      In:  *val1p =>    C-pointer to left expression value
 *            tyla1 =>    Type pointer for     - " -      
 *           *val2p =>    C-pointer to right expression value 
 *            tyla2 =>    Type pointer for       - " -          
 *
 *      Out: *valp  =>    Result value
 *           *tylap =>    Result type pointer
 *
 *      FV:  return - error severity code.
 *
 *      Error codes:  IN 308 Type missmatch for right operand, INT or FLOAT desired
 *                    IN 312 Ilegal types for operator ......
 *
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-11 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   STTYTBL type1;
/*
***preper result type info., result type allways FLOAT
*/
   valp->lit_type = C_FLO_VA;
   *tylap = stflop;
/*
***read type info
*/
   strtyp( tyla1, &type1 );

   switch ( type1.kind_ty )
      { 
      case ST_INT:

/*
***equal types ? evaluate integer ** integer , result is float
*/
         if ( ineqty( tyla1, tyla2 ) )  
            {  
            valp->lit.float_va=pow((double)val1p->lit.int_va,(double)val2p->lit.int_va );
            }
/*
***if FLOAT , evaluate integer ** float, result is float
*/
         else if ( ineqty( tyla2, stflop ) )  
            {  
/*
***mantissa positiv ?
*/
            if ( val1p->lit.int_va >= 0 )
               valp->lit.float_va=pow((double)val1p->lit.int_va,val2p->lit.float_va);
/*
***Ilegal exponent type for negative mantiss, INT desired
*/
            else
               return( erpush( "IN3153", "" ) );
            }

/*
***Ilegal type of exponent, INT or FLOAT desired
*/
         else  
            { 
            return( erpush( "IN3163", "" ) );
            }

         break;

      case ST_FLOAT:
/*
***equal types ? evaluate float ** float, result is float
*/
         if ( ineqty( tyla1, tyla2 ) ) 
            {  
/*
***mantissa positiv ?
*/
            if ( val1p->lit.float_va >= 0 )  
               valp->lit.float_va=pow(val1p->lit.float_va, val2p->lit.float_va);
/*
***Ilegal exponent type for negative mantiss, INT desired
*/
            else  
               return( erpush( "IN3153", "" ) );
            }
/*
***if INT, evaluate float ** integer, result is float
*/
        else if ( ineqty( tyla2, stintp ) )  
           {            
           valp->lit.float_va=pow(val1p->lit.float_va,(double)val2p->lit.int_va);
           }
/*
***Ilegal type of exponent, INT or FLOAT desired
*/
        else  
           { 
           return( erpush( "IN3163", "" ) );
           }

       break;
/*
***Ilegal type for Exp operator
*/
      default:   
 
         return( erpush( "IN3123", " ** " ) ); 
        break;
    }
   return( 0 );
  }  

/********************************************************/
/*********************************************************
*     Internal routines
*********************************************************/

#ifndef ANALYZER

/*!******************************************************/

        static DBint calind(
        pm_ptr    indlist,   
        pm_ptr    *typelap)  

/*      Calculate indexed offset. 
 *      Calculates array offset from an index list (expr-list).
 *      The routine dose not check if it is to many index.
 *
 *      In:   indlist =>  PM-pointer to index list, expr-list
 *            typelap =>  PM-pointer to type info. in symbol table
 *
 *      Out: *typelap =>  PM-pointer to type for indexed component
 *
 *      FV:  return - Calculated array offset, negativ value
 *                    indicates error status:
 *                    -1  array index out of bound range
 *                    -2  error during index calculation
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-11 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;            /* PM-pointer to list node */ 
   pm_ptr nextla;            /* PM-pointer to next list node */
   pm_ptr exprla;            /* PM-pointer to expression node */
   short status;

   STTYTBL typtbl;           /* type record */
   STARRTY arrtype;          /* array type */
   PMLITVA indval;           /* index value */
   pm_ptr  indtyla;          /* type pointer for index value */
   v2int index;
   DBint arroffs;            /* array offset */

   arroffs = 0;

   if ( indlist == (pm_ptr)NULL )
      {
      return( 0 );
      }
/* 
***get first list node in list
*/
   if ( ( status = pmgfli( indlist, &listla ) ) != 0 )
/*
***Error
*/
      { 
      return( status );
      }
/*
***read type info for the total array
*/
   strtyp( *typelap, &typtbl );

   while ( listla != (pm_ptr)NULL )
      { 
/*
***get contents of list node
*/
      if ( ( status = pmglin( listla, &nextla, &exprla ) ) != 0 )
         return( status );

/*
***evaluate index value
*/
      inevev( exprla, &indval, &indtyla );
      index = indval.lit.int_va;
/*
***check type on index
*/
      if ( ! ineqty( stintp, indtyla ) )
         {  /* errror */
         return( -2 );
         }
/*
read array info.
*/
      if ( typtbl.arr_ty != (pm_ptr)NULL )
         strarr( typtbl.arr_ty, &arrtype );
/*
***error
*/
      else
         return( -2 );
/*
***read array component type ( next type table )
*/
      strtyp( arrtype.base_arr, &typtbl );
/*
***check array bounds
*/
      if ( ( index < arrtype.low_arr ) || ( index > arrtype.up_arr ) )
/*
***error
*/
         { 
         return( -1 ); /* array index out of bound */
         }
/*
***calculate array offset
*/
      arroffs = ( index - arrtype.low_arr ) * typtbl.size_ty + arroffs;

      listla = nextla;
      }
/*
***set pm_pointer to return type
*/
   *typelap = arrtype.base_arr;

   return( arroffs );
  }

/*!******************************************************/

        static void fptrap(
        int sigval)

/*      Trap-rutin för flyttalsfel.
 *
 *      In:   
 *
 *      Out:
 *
 *      FV:  
 *
 *      (C)microform ab 1986-10-23 J. Kjellander
 *
 *      1999-11-11 Rewritten, R. Svedin
 *
 ******************************************************!*/

   { 
    if ( sigval == SIGFPE ) longjmp(env,-1);
   }

/********************************************************/

#endif

/********************************************************/
