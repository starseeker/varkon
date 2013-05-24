/**********************************************************************
*
*    ingpar.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    short inapar()    Evaluate actual parameter list for selected routine
*    short inrpar()    Release pool space allocated by inapar()
*    short inrerr()    Routine error handler
*    void  inclpp()    Clear parameter pool
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
*    (C)Microform AB 1984-2000, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"

static PMPARVA ppool[V2NPAMAX];     /* parameter value pool, const in common.h */
static short polusrs = 0;           /* number of pool users */
static short usridx[V2NFUMAX];      /* pool indexes,const in common.h */
static short ffidx = 0;             /* first free index */

static short inrerr(char *estr, pm_ptr routla);

/********************************************************/
/*!******************************************************/

        short inapar(
        pm_ptr   actplist,
        pm_ptr   routla,
        STPROC  *rout,
        PMPARVA **pv,
        short   *nopar)

/*      Evaluate actual parameter list for selected routine.
 *      This routine evaluates the actual parameter list and put the parameter
 *      values in an array of PMLITVA:s. Elemet 0 in the array are reserved for 
 *      function return value.
 *
 *      In:   actplist  =>  PM-pointer to actual parameter list in PM 
 *             routla   =>  PM-pointer to routine  for error hand
 *            *rout     =>  Routine data 
 *
 *      Out:  **pv      =>  Pointer to array with actual parameter values 
 *            *nopar    =>  Number of parameter in actual list 
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {

/*
***local declarations
*/
   pm_ptr listla;      /* PM-pointer to list node */
   pm_ptr nextla;      /* PM-pointer to next list node */
   pm_ptr exprla;      /* PM-pointer to expression node, actual parameter value */
   int parnr = 1;      /* current parameter number */
   STPARDAT par;       /* structure for formal parameter info. */
   short status;
   short dummy;
   int   i;            /* loop variable */
   PMLINO *np;         /* c-pointer to an list node */

/*
***allocate memory for the new user (caller)
*/
   usridx[polusrs] = ffidx;            /* give first free index to user */
   polusrs++;                          /* increse user count */
   *pv = &(ppool[ ffidx ]);            /* select callers pool area */
   ffidx++;                            /* for special first element of parameter array */
   if( polusrs > V2NFUMAX )
       return( erpush("IN3303","") );  /* function nest level overflow */

   if ( actplist == (pm_ptr)NULL )
       { 
       listla = (pm_ptr)NULL;
       } 
   else
       {
/*
***get first list node in list
*/
       if ( ( status = pmgfli( actplist, &listla ) ) != 0 )
           {  
           return( status );            /* Error */
           }
       }

/*
***Evaluate actual parameters
*/
   while ( listla != (pm_ptr)NULL )
       {
/*
***Do action depending on parameter
***get contents of list node
*/
       np = (PMLINO *)pmgadr(listla);
       nextla = np->p_nex_li;
       exprla = np->p_no_li;

/*
***evaluate expression
*/
       if ( exprla != (pm_ptr)NULL )
           {  
           if ((status = inevex( exprla, &((*pv)[ parnr ].par_va),
                                         &((*pv)[ parnr ].par_ty) )) != 0 )
               return( status );
           }
       else
/*
***indicate no value
*/
           {  
           (*pv)[ parnr ].par_va.lit_type = C_NO_VA; 
           }
/*
***next parameter
*/
       listla = nextla;
       parnr ++;
       ffidx ++;
       }

/*
***Select formal parameter list and do type check/conversion
***select formal paramter list for routine
*/
   stsrou(rout->kind_pr );

/*
***get info. on first formal paramter
*/
   stgpar( (short)1, &par );

   for ( i = 1; i < parnr; i++ )
       {
/*
***check if parameter is ilegal
*/
       if ( par.valid == ST_ILL )
           {  
           erpush("IN3043","ST_ILL");      /* error, missmatch in parameters to routine */
           return( inrerr( "IN3043", routla ) ); 
           }
/*
***check if it excist an actual value
*/
       if ( (*pv)[ i ].par_va.lit_type != C_NO_VA )
           {
/*
***evaluate expression
*/
           if ( par.rvclass != ST_VARCL )
/*
***evaluate expression value
*/
              { 
#ifndef ANALYZER
               if ( (*pv)[ i ].par_va.lit_type == C_ADR_VA )
                   ingval( (*pv)[ i ].par_va.lit.adr_va, (*pv)[ i ].par_ty,
                   FALSE,&((*pv)[ i ].par_va) );
#endif
               }

/*
***check types
***if not equal types do conversion
*/
           if ( ! ineqty( par.type, (*pv)[ i ].par_ty ) ) 
/*
***conver type
*/ 
               { 
               if ((status = incoty(&((*pv)[ i ].par_va), 
                                    &((*pv)[ i ].par_ty), par.type)) < -1 )
                   return( inrerr( "IN3053", routla ) );     /* ilegal data type for parameter to */
               }
           }
       else
/*
***use default value
*/
           {
           if ( par.rvclass == ST_DEFCL )
               {
               V3MOME( (char *)&par.defval, (char *)&((*pv)[ i ].par_va),
                                                               sizeof( PMLITVA ));

               (*pv)[ i ].par_ty = par.type;
               }
           else
               {  
               erpush("IN3043","ST_DEFCL");       /* error, missmatch in parameters to routine */
               return( inrerr( "IN3043", routla ) );  
               }
           }
/*
***Get info. on next formal parameter
*/
       stgpar( (short)(i+1), &par );
       }

   if ( par.valid == ST_FOR )                    /* if FORCED */
       {  
       erpush("IN3043","ST_FOR");                /* error, missmatch in parameters to routine  */
       return( inrerr( "IN3043", routla ) );    
       }

/*
***Get default values for Optional parameter(s)
*/
   while( ( par.valid == ST_OPT ) && ( par.defval.lit_type != C_NO_VA ) )

/*
***use default value if excist
*/
       {
       V3MOME( (char *)&par.defval, (char *)&((*pv)[ parnr ].par_va),
                                                               sizeof( PMLITVA ));

       (*pv)[ parnr ].par_ty = par.type;

       parnr ++;
       ffidx ++;
       stgpar( (short)parnr, &par );
       }

   *nopar = (short)(parnr - 1);

/*
***Set return type for functions
*/
   stgret( &((*pv)[0].par_ty), &((*pv)[0].par_va.lit_type), &dummy );

   return( 0 );

  }

/********************************************************/
/*!******************************************************/

        void inrpar()

/*      Release pool space.
 *
 *      In:   
 *
 *      Out:  
 *
 *      FV:   
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   polusrs -= 1;                  /* decrease number of users */
   ffidx = usridx[ polusrs ];     /* backup first free pointer */
   return;
  }

/********************************************************/
/*!******************************************************/

        void inclpp()

/*      Clear parameter pool.
 *
 *      In:   
 *
 *      Out:  
 *
 *      FV:   
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/
  
  {
   polusrs = 0;              
   ffidx = 0;
   return;
  }

/********************************************************/
/*!******************************************************/

        static short inrerr(
        string  estr,   
        pm_ptr  routla)

/*      Routine error handler.
 *
 *      In:   estr      =>   Error code string 
 *            routla    =>   PM-pointer to routine in symbol table 
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   stidcl kind;
   char *rname;
/*
***read attributes from symbol table
*/
   stratt( routla, &kind, &rname );

   return( erpush( estr, rname ) );
  }

/********************************************************/
