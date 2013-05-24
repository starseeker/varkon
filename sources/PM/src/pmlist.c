/**********************************************************************
*
*    pmlist.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    pretty print routins for PM structures
*
*    short pmprmo()    print module
*
*    Rutinerna får en PM-pekare som indata och gör om den till en c-pekare 
*    till begärd nodstructur.
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

#define INDSTEP  4    /* indentation step */
#define MAXIND  50    /* maximum indentation */

static short pmprst(pm_ptr statla);
static short pmprsl(pm_ptr stlist);
static short pmprex(pm_ptr exprla);
static short pmprel(pm_ptr exlist);
static short pmprnl(pm_ptr nplist);
static short pmprnp(pm_ptr npla);
static short pmprpa(pm_ptr pala);
static short pmprpl(pm_ptr palist);
static short pmprcl(pm_ptr colist);
static short pmprco(pm_ptr cola);
static int   indent(short indlev);
static int   prsymb(pm_ptr symla);

static short level = 0;   /* indentation level for output */

/********************************************************/
/*!******************************************************/

        static short pmprst(
        pm_ptr statla)

/*      Print statement.
 *
 *      In:    statla =>  PM-pointer to statement
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMSTNO *np;        /* c-pointer to statement node */
   PMGEST *geop;      /* c-pointr to geo_stat node */
   PMPRST *procp;     /* c-pointer to procedure subnode */
   PMPAST *partp;     /* c-pointer to part-stat subnode */
   PMASST *assp;      /* c-pointer to assignment subnode */
   PMIFST *ifp;       /* c-pointer to if subnode */
   PMFOST *forp;      /* c-pointer to for subnode */
   PMGOST *gotop;     /* c-pointer to goto subnode */
   PMLAST *labstp;    /* c-pointer to labaled subnode */

   stidcl idclass;    /* identifier class */
   short status;
   string str;

   if ( statla == (pm_ptr)NULL )            /* empty statement */
      {
      indent( level );
      printf( "EMPTY_STAT \n" );
      return(0);
      }

   if ( ( status = pmgsta( statla, &np ) ) != 0 )
      {  
      return( status );                     /* Error */
      }

   if ( np->noclst != STAT )
      { 
      return( erpush( "PM2512", "" ) );     /* Not a statement node */
      }

   switch ( np->suclst )
      {
      case ASS_ST:
         assp = &(np->stsubc.ass_st);
         indent( level );
         printf( "ASSIGN_STAT \n" );

         level++;

         pmprex(assp->asvar);               /* print left expression */
         pmprex(assp->asexpr);              /* print right expression */

         level--;
         break;

      case IF_ST:
         ifp = &(np->stsubc.if_st);
         indent( level );
         printf( "IF_STAT \n" );

         level++;

         pmprcl(ifp->ifcond);               /* print condition list */
         pmprsl(ifp->ifstat);               /* print statement list */

         level--;
         break;

      case FOR_ST:
         forp = &(np->stsubc.for_st);
         indent( level );
         printf( "FOR_STAT \n" );

         level++;
         indent(level);
         printf( "FOR_VAR  " );
         prsymb(forp->fovar);
         printf( "\n" );

         indent(level);
         printf( "START_EXPR \n" );

         level++;
         pmprex(forp->fostar);
         level--;

         indent(level);
         printf( "TO_EXPR \n" );

         level++;
         pmprex(forp->foto);
         level--;

         indent(level);
         printf( "STEP_EXPR \n" );

         level++;
         pmprex(forp->fostep);
         level--;

         indent(level);
         printf( "DO_STAT_LIST \n" );

         level++;
         pmprsl(forp->fordo);
         level--;

         level--;

         break;

      case GO_ST:
         gotop = &(np->stsubc.gotost);
         indent( level );
         printf( "GOTO_STAT \n" );

         level++;

         indent( level );
         prsymb(gotop->golabe);          /* print label */
         printf( "\n" );
 
         level--;
         break;

      case PRO_ST:
         procp = &(np->stsubc.procst);
         indent( level );
         printf( "PROC_STAT  " );
         prsymb( procp->prname );       /* print routine name */ 
         printf( "\n" );

         level ++;

/*
***if routine "set" special treatment
***set_basic(), 4/6/91 J. Kjellander
*/

         if ((status = stratt( procp->prname, &idclass, &str )) != 0)
         return( status );

         if (strcmp(str,"SET") == 0  ||  strcmp(str,"SET_BASIC") == 0 )
            pmprnl( procp->pracva );    /* print named parameter list */
         else
            pmprel( procp->pracva );    /* print value parameter list */

         level --;

         break;

      case PART_ST:
         partp = &(np->stsubc.partst);
         indent( level );
         printf( "PART_STAT \n" );

         level ++;
      
         indent( level );
         printf( "#%d\n", partp->geident );
/*
***Fr.o.m. version 1.12 kan den anropade modulens namn
***representeras på två olika sätt. Om pm-pekaren till namnet
***är positiv gäller den gamla representationen med en PM-string
***och om pekaren är negativ pekar den på ett uttryck.
**/
         if ( partp->modname > 0 )
           {
           if ( ( status = pmgstr( partp->modname, &str ) ) < -1 )
              return( status );
           indent( level );
           printf( "NAME = %s \n", str );
           }
         else
           {
           indent(level);
           printf( "NAME = EXPRESSION\n");
           pmprex(-partp->modname);
           }

         level ++;

         pmprel( partp->modpara );

         level --;

         pmprel( partp->partpara );
         pmprnl( partp->partacna );

         level --;

         break;


      case GEO_ST:
         geop = &(np->stsubc.geo_st);
         indent( level );
         printf( "GEO_STAT  " );
         prsymb( geop->gename );        /* print routine name */ 
         printf( "\n" );

         level ++;
         indent( level );
         printf( "#%d\n", geop->geident );


         pmprel( geop->geacva );        /* print value parameter list */
         pmprnl( geop->geacna );        /* print named parameter list */
         level --;

         break;

      case LAB_ST:
         labstp = &(np->stsubc.labest);
         indent( level );
         printf( "LABELED_STAT \n" );

         level++;

         indent( level );
         prsymb(labstp->lalabe);        /* print label */
         printf("\n");

         pmprst(labstp->lastat);        /* print statement */
 
         level--;

         break;

      default:
         return( erpush( "PM2512", "" ) );
         break;

      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short pmprsl(
        pm_ptr  stlist)

/*      Print statement list.
 *
 *      In:    stlist =>  PM-pointer to statement list (tconc-node)
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;     /* PM-pointer to list node */ 
   pm_ptr nextla;     /* PM-pointer to next list node */
   pm_ptr statla;     /* PM-pointer to statment node */
   short status;

   indent( level );
   printf( "STAT-LIST \n" );

   level ++;

   if ( stlist == (pm_ptr)NULL )
      {
      indent( level );
      printf( "NULL\n" );
      level --;
      return( 0 );
      }

/*
***get first list node in list
*/
   if ( ( status = pmgfli( stlist, &listla ) ) != 0 )
      {  
      return( status );             /* Error */
      }

   while ( listla != (pm_ptr)NULL )
      { 
/*
***get contents of list node
*/
      if ( ( status = pmglin( listla, &nextla, &statla ) ) != 0 )
         return( status );

/*
***print statement
*/
      if ( ( status = pmprst( statla ) ) != 0 )
         return( status );

      listla = nextla;
      }

   level --;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short pmprex(
        pm_ptr exprla) 

/*      Print expression.
 *
 *      In:    exprla =>  PM-pointer to expression node
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMEXNO *np;          /* c-pointer to expression node */
   PMFUNCEX *funcp;     /* c-pointer to func_expr node */
   PMBINEX *binp;       /* c-pointer to bin_expr node */
   PMUNEX  *unp;        /* c-pointer to unary_exper node */
   PMLITEX *litp;       /* c-pointer to lit_expr node */
   PMREFEX *refp;       /* c-pointer to ref. value in PM */
   PMVECVA *vecp;       /* c-pointer to vector value */
   short status;
   string str;

   if ( exprla == (pm_ptr)NULL )         /* empty (default) expression */
      {
      indent( level );
      printf( "DEFAULT-EXPR \n" );
      return(0);
      }

   if ( ( status = pmgexp( exprla, &np ) ) != 0 )
      {  
      return( status );                  /* Error */
      } 

   if ( np->noclex != EXPR )
      {
      return( erpush( "PM2522", "" ) );   /* Not a expression node */
      }

   switch ( np->suclex )
      {
      case C_UN_EX:

         indent( level );
         printf( "UNARY_EXPR " );

         unp = &( np->ex_subcl.unop_ex );

         switch ( unp->unop )
            {
            case PM_NOT:
               printf( "NOT \n" );
               break;

            case PM_MINUS:
               printf( "- \n" );
               break;

            default:
               printf( "unknown unary operator \n" );
               break;

            }

         level ++;

         pmprex( unp->p_unex );          /* print expression */

         level --;

         break;

      case C_BIN_EX:

         indent( level );
         printf( "BIN_EXPR " );

         binp = &( np->ex_subcl.binop_ex );

         switch ( binp->binop )
            {
            case PM_GT:
               printf( "> \n" );
               break;

            case PM_GE:
               printf( ">= \n" );
               break;

            case PM_EQ:
               printf( "= \n" );
               break;

            case PM_NE:
               printf( "<> \n" );
               break;

            case PM_LT:
               printf( "< \n" );
               break;

            case PM_AND:
               printf( "AND \n" );
               break;

            case PM_OR:
               printf( "OR \n" );
               break;

            case PM_PLUS:
               printf( "+ \n" );
               break;

            case PM_MINUS:
               printf( "- \n" );
               break;

            case PM_DIV:
               printf( "/ \n" );
               break;

            case PM_MUL:
               printf( "* \n" );
               break;

            case PM_EXP:
               printf( "** \n" );
               break;

            default:
               printf( "unknown operator \n" );
               break;
            }

         level ++;

         pmprex( binp->p_bin_l );   /* print left expression */
         pmprex( binp->p_bin_r );   /* print right expression */

         level --;

         break;

      case C_LIT_EX:
         indent( level );
         printf( "LIT ");

         litp = &(np->ex_subcl.lit_ex);

         switch ( litp->lit_type )
            {
            case C_INT_VA:
               printf( "INT %d \n", litp->litex.int_li );
               break;

            case C_FLO_VA:
               printf( "FLOAT %f \n", litp->litex.float_li );
               break;

            case C_STR_VA:
               if ( ( status = pmgstr( litp->litex.str_li, &str ) ) != 0 )
                  return( status );
               printf( "STRING %s \n", str );
               break;

            case C_VEC_VA:
               printf( "VECTOR " );
               if ( ( status = pmgvec( litp->litex.vec_li, &vecp ) ) != 0 )
                  return( status );
               printf("( %f, %f, %f ) \n", vecp->x_val, vecp->y_val, vecp->z_val);
               break;

            case C_REF_VA:
               printf( "REF " );

               refp = &litp->litex.ref_li;

               printf( "#%d.%d", refp->seq_lit, refp->ord_lit );

               while ( refp->nextref != 0 )
                  {
                  if ( ( status = pmgref( refp->nextref, &refp ) ) != 0 )
                     return( status );

                   printf( "#%d.%d", refp->seq_lit, refp->ord_lit );
                   }
               printf( "\n" );
            
               break;
            }

         break;

      case C_ID_EX:
         indent( level );
         printf( "ID ");
         prsymb( np->ex_subcl.id_ex.p_id );
         printf( "\n" ); 
         break;

      case C_IND_EX:
         indent( level );
         printf( "INDX ");
         prsymb( np->ex_subcl.ind_ex.p_indid );
         printf( "\n" );
         level++;
         pmprel( np->ex_subcl.ind_ex.p_indli );
         level--;
         break;

      case C_COM_EX:
         indent( level );
         printf( "COMP ");
         switch(np->ex_subcl.comp_ex.p_cfield)
            {
            case PM_X:
               printf("X\n");
               break;
            case PM_Y:
               printf("Y\n");
               break;
            case PM_Z:
               printf("Z\n");
               break;
            }
         level++;
         pmprex( np->ex_subcl.comp_ex.p_comvar);
         level--;
         break;

      case C_FUN_EX:
         funcp = &(np->ex_subcl.func_ex);
         indent( level );
         printf( "FUNC_EXPR " );
         prsymb( funcp->funcid );            /* print routine name */ 
         printf( "\n" );

         level ++;

         pmprel( funcp->p_funcar );          /* print value parameter list */

         level --;

         break;

      default:
         return( erpush( "PM2522", "" ) );
         break;
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short pmprel(
        pm_ptr  exlist)

/*      Print expression list.
 *
 *      In:    exlist =>  PM-pointer to expression list 
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;     /* PM-pointer to list node */ 
   pm_ptr nextla;     /* PM-pointer to next list node */
   pm_ptr exprla;     /* PM-pointer to statment node */
   short status;

   indent( level );
   printf( "EXPR-LIST \n" );

   level ++;

   if ( exlist == (pm_ptr)NULL )
      {
      indent( level );
      printf( "NULL\n" );
      level --;
      return( 0 );
      }
/*
***get first list node in list
*/
   if ( ( status = pmgfli( exlist, &listla ) ) != 0 )
      {  
      return( status );            /* Error */
      }

   while ( listla != (pm_ptr)NULL )
      { 
/*
***get contents of list node
*/
      if ( ( status = pmglin( listla, &nextla, &exprla ) ) != 0 )
         return( status );
/*
***print expression
*/
      if ( ( status = pmprex( exprla ) ) != 0 )
         return( status );

      listla = nextla;
      }

   level --;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short pmprnl(
        pm_ptr nplist)

/*      Print named parameter list.
 *
 *      In:    nplist =>  PM-pointer to named parameter list (TCONC_NODE) 
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;     /* PM-pointer to list node */ 
   pm_ptr nextla;     /* PM-pointer to next list node */
   pm_ptr npla;       /* PM-pointer to named parameter node */
   short status;

   indent( level );
   printf( "NAME_PARAM-LIST \n" ); 

   level ++;

   if ( nplist == (pm_ptr)NULL )
      {
      indent( level );
      printf( "NULL\n" );
      level --;
      return( 0 );
      }
 
/*
***get first list node in list
*/
   if ( ( status = pmgfli( nplist, &listla ) ) != 0 )
      { 
      return( status );              /* Error */
      }

   while ( listla != (pm_ptr)NULL )
      { 
/*
***get contents of list node
*/
      if ( ( status = pmglin( listla, &nextla, &npla ) ) != 0 )
         return( status );

/*
***print named parameter
*/
      if ( ( status = pmprnp( npla ) ) != 0 )
         return( status );

      listla = nextla;
      }

   level --;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short pmprnp(
        pm_ptr npla)

/*      Print named parameter.
 *
 *      In:    npla   =>  PM-pointer to named parameter (PMNPNO) 
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMNPNO *npp;
   static char *namtab[] = { "ERROR",     /* NULL */
                             "PEN",       /* PMPEN */
                             "LEVEL",     /* PMLEVEL */
                             "LFONT",     /* PMLFONT */
                             "AFONT",     /* PMAFONT */
                             "LDASHL",    /* PMLDASHL */
                             "ADASHL",    /* PMADASHL */
                             "TSIZE",     /* PMTSIZE */
                             "TWIDTH",    /* PMTWIDTH */
                             "TSLANT",    /* PMTSLANT */
                             "DTSIZE",    /* PMDTSIZE */
                             "DASIZE",    /* PMDASIZE */
                             "DNDIG",     /* PMDNDIG */
                             "DAUTO",     /* PMDAUTO */
                             "XFONT",     /* PMXFONT */
                             "XDASHL",    /* PMXDASHL */
                             "BLANK",     /* PMBLANK */
                             "HIT",       /* PMHIT */
                             "SAVE",      /* PMSAVE */
                             "CFONT",     /* PMCFONT */
                             "CDASHL",    /* PMCDASHL */
                             "TFONT",     /* PMTFONT */
                             "SFONT",     /* PMSFONT */
                             "SDASHL",    /* PMSDASHL */
                             "NULINES",   /* PMNULIN */
                             "NVLINES",   /* PMNVLIN */
                             "WIDTH",     /* PMWIDTH */
                             "TPMODE",    /* PMTPMODE */
                             "MFONT"};    /* PMMFONT */


   short status;

   if ( ( status = pmgnpa( npla, &npp ) ) != 0 )
      {  
      return( status );                   /* Error */
      }

   if ( npp->noclnp != NAMEPA )
      {
      return( erpush( "PMXXX1", "" ) );   /*Error  Not a named parameter node */
      }

   indent( level );
   printf( "NAME_PARAM  " );
   if((npp->par_np < 1) || (npp->par_np > 28))
      printf( namtab[0] );
   else
      printf( namtab[(int)npp->par_np]);

   printf( "\n" );

   level ++;

   pmprex(npp->p_val_np);                 /* print named parameter */
                                          /* value expression */
   level--;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        static short pmprpa(
        pm_ptr pala)

/*      Print module parameter.
 *
 *      In:    pala   =>  PM-pointer to to module parameter node 
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMPANO *np;       /* c-pointer to module parameter node */
   string str;
   STVAR var;        /* interface struct for a parameter */
   short status;

   if ( ( status = pmgpar( pala, &np ) ) )
      {  
      return( status );                   /* Error */
      }

   if ( np->noclpa != PARAM )
      {  
      return( erpush( "PM2532", "" ) );   /*Error  Not an module parameter node */
      }

   indent( level );
   printf( "PARAM  " );
   prsymb( np->fopa_ );                   /* print formal parameter name */ 
   printf( "\n" );

   level ++;
/*
***get and print default value
*/
   indent( level );
   printf( "Default: \n" );
   level ++;
   if ( ( status = strvar( np->fopa_, &var ) ) != 0 )
      return( status );
   if ( ( status = pmprex( var.def_va ) ) != 0 )
      return( status );
   level --;

   indent( level );
   if ( ( status = pmgstr( np->ppro_, &str ) ) != 0 )
      return( status );
   printf( "Prompt: %s\n", str );

   level --;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short pmprpl(
        pm_ptr palist)

/*      Print module parameter list.
 *
 *      In:    palist =>  PM-pointer to to module parameter list
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;     /* PM-pointer to list node */ 
   pm_ptr nextla;     /* PM-pointer to next list node */
   pm_ptr parala;     /* PM-pointer to module parameter node */
   short status;

   indent( level );
   printf( "PARAM-LIST \n" );

   level ++;

   if ( palist == (pm_ptr)NULL )
      {
      indent( level );
      printf( "NULL\n" );
      level --;
      return( 0 );
      }
/*
***get first list node in list
*/
   if ( ( status = pmgfli( palist, &listla ) ) != 0 )
      {  
      return( status );          /* Error */
      }

   while ( listla != (pm_ptr)NULL )
      { 
/*
***get contents of list node
*/
      if ( ( status = pmglin( listla, &nextla, &parala ) ) != 0 )
          return( status );
/*
***print parameter
*/
      if ( ( status = pmprpa( parala ) ) != 0 )
          return( status );

      listla = nextla;
      }

   level --;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmprmo(
        pm_ptr mola)

/*      Print module.
 *
 *      In:    mola   =>  PM-pointer to base for module header
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMMONO *np;                  /* c-pointer to module header */
   string str;
   short status;
   pm_ptr prev=(pm_ptr)NULL;    /* for ST dump */
   stidcl idcl;                 /* for ST dump */
   char *name;                  /* for ST dump */

   if ( ( status = pmgmod((pm_ptr)0, &np ) ) != 0 )
      {  
      return( status );                   /* Error */
      }

   if ( np->monocl != MODULE )
      {  
      return( erpush( "PM2542", "" ) );   /*Error  Not a statement node */
      }

   indent( level );

   if ( ( status = pmgstr( np->mona_, &str ) ) != 0 )
      return( status );

   printf( "MODULE %s\n", str );

/*
***print module header
*/
   indent( level );
   printf( "Module type      : " );
   if ( np->moty_ == _2D )
      printf( "2D \n" );
   else if ( np->moty_ == _3D )
      printf( "3D \n" );
   else
      printf( "unknown type \n" );

   indent( level );
   printf( "Module attribute : " );
   if ( np->moat_ == LOCAL )
      printf( "LOCAL \n" );
   else if ( np->moat_ == GLOBAL )
      printf( "GLOBAL \n" );
   else if ( np->moat_ == BASIC )
      printf( "BASIC \n" );
   else if ( np->moat_ == MACRO )
      printf( "MACRO \n" );
   else
      printf( "unknown attribute \n" );

   indent( level );
   printf( "Highest id       : %d\n", (short) np->geidlev );

   indent( level );
   printf( "Data size        : %d\n", np->datasize );

   level ++;

   indent( level );
   printf( "SYMBOL-LIST\n" );
   level++;
   do
     {
     stgnid( prev, &idcl, &name, &prev );
     if ( prev != (pm_ptr)NULL )
        {
        indent(level);
        prsymb(prev);
        printf("\n");
        }
     } while( prev != (pm_ptr)NULL );

   level--;

   pmprpl( np->ppali_ );     /* print parameter list */

   pmprsl( np->pstl_ );      /* print statement list */

   level --;

  return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short pmprcl(
        pm_ptr colist)

/*      Print condition list.
 *
 *      In:    colist =>  PM-pointer to condition list (TCONC-NODE)
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;     /* PM-pointer to list node */ 
   pm_ptr nextla;     /* PM-pointer to next list node */
   pm_ptr cola;       /* PM-pointer to condition node */
   short status;

   indent( level );
   printf( "CONDITION-LIST \n" ); 

   level ++;

   if ( colist == (pm_ptr)NULL )
      {
      indent( level );
      printf( "NULL\n" );
      level --;
      return( 0 );
      }
/*
***get first list node in list
*/
   if ( ( status = pmgfli( colist, &listla ) ) != 0 )
      {  
      return( status );     /* Error */
      }

   while ( listla != (pm_ptr)NULL )
      { 
/*
***get contents of list node
*/
      if ( ( status = pmglin( listla, &nextla, &cola ) ) != 0 )
          return( status );
/*
***print condition
*/
      if ( ( status = pmprco( cola ) ) != 0 )
          return( status );

      listla = nextla;
      }

   level --;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short pmprco(
        pm_ptr cola)

/*      Print condition.
 *
 *      In:    cola   =>  PM-pointer to condition (COND_NODE)
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMCONO *cop;   /* c-pointer to condition (COND_NODE) */
   short status;

   indent( level );
   printf( "CONDITION\n" ); 

   if ( ( status = pmgcon( cola, &cop ) ) != 0 )
      {  
      return( status );                   /* Error */
      }

   if ( cop->noclco != COND )
      {  
      return( erpush( "PMXXX2", "" ) );   /*Error  Not a condition node */
      }

   level ++;

   pmprex(cop->p_cond);                   /* print cond. expr. */

   pmprsl(cop->p_stl_co);                 /* print statement list */

   level--;

  return(0);
  }

/********************************************************/
/*!******************************************************/

        static int indent(
        short indlev)

/*      Print indentation.
 *
 *      In:    indlev =>  Indentation level
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   int i;
   int nustep;

   nustep = indlev * INDSTEP;
   if ( nustep > MAXIND )
      nustep = MAXIND;

   for ( i = 1; i <= nustep; i++ ) 
      printf( " " );

   return(0);
  }

/********************************************************/
/*!******************************************************/

        static int prsymb(
        pm_ptr symla)

/*      Print symbol name.
 *
 *      In:    indlev =>  PM-pointer to symbol table
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   stidcl idclass;       /* identifier class */ 
   string symname;       /* string for symbol name */
   STVAR var;            /* st structure for a variable */
   STCONST konst;        /* st structure for a constant */
   STTYTBL typ;          /* MBS-type definition structure */
   short status,i;

   if ( ( status = stratt( symla, &idclass, &symname ) ) != 0 )
      return( status );

   switch ( idclass )
     {
     case ST_VAR:
        printf( "VAR " );

        if ( ( status = strvar( symla, &var ) ) != 0 )
           return( status );

        if ( var.type_va == (pm_ptr)NULL )
           {
           printf( "UNDEF " );
           return(0);
           }

        if ( ( status = strtyp( var.type_va, &typ ) ) != 0 )
           return( status );

        switch ( typ.kind_ty )
           {
           case ST_INT:
              printf( "INT " );
              break;

           case ST_FLOAT:
              printf( "FLOAT " );
              break;

           case ST_STR:
              printf( "STRING " );
              break;

           case ST_VEC:
              printf( "VECTOR " );
              break;

           case ST_REF:
              printf( "REF " );
              break;

           case ST_FILE:
              printf( "FILE " );
              break;

           case ST_ARR:
              printf( "ARR " );
              break;

           default:
              printf( "Unknown type: %d ", typ.kind_ty );
              break;
           }

        break;

     case ST_CONST:
        printf( "CONST " );

        if ( ( status = strcon( symla, &konst ) ) != 0 )
           return( status );

        if ( konst.type_co == (pm_ptr)NULL )
           {
           printf( "UNDEF " );
           return(0);
           }

        if ( ( status = strtyp( konst.type_co, &typ ) ) != 0 )
           return( status );

        switch ( typ.kind_ty )
           {
           case ST_INT:
              printf( "INT " );
              break;

           case ST_FLOAT:
              printf( "FLOAT " );
              break;

           case ST_STR:
              printf( "STRING " );
              break;

           case ST_VEC:
              printf( "VECTOR " );
              break;

           case ST_REF:
              printf( "REF " );
              break;

           default:
              printf( "Unknown type: %d ", typ.kind_ty );
              break;
           }

        break;

     case ST_LABEL:
        printf( "LABEL " );
        break;

     case ST_FUNC:
        printf( "FUNC " );
        break;

     case ST_PROC:
        printf( "PROC " );
        break;

     case ST_MOD:
        printf( "MOD " );
        break;

     default:
        printf( "Not a symbol. PM-adr: %d ", symla );
        break;
     }

   printf( "%s ", symname );     /* print symbol name */

   if (idclass == ST_CONST)
      {
      switch ( konst.valu_co.lit_type )
         {
         case C_INT_VA:
            printf( "= %d", konst.valu_co.lit.int_va );
            break;

         case C_FLO_VA:
            printf("= %f", konst.valu_co.lit.float_va );
            break;

         case C_STR_VA:
            printf("= \042%s\042", konst.valu_co.lit.str_va );
            break;

         case C_VEC_VA:
            printf("= ( %f, %f, %f )",
                    konst.valu_co.lit.vec_va.x_val,
                    konst.valu_co.lit.vec_va.y_val,
                    konst.valu_co.lit.vec_va.z_val);
            break;

         case ST_REF:
            printf("= #%d.%d",konst.valu_co.lit.ref_va[0].seq_val,
                              konst.valu_co.lit.ref_va[0].ord_val);
            i = 0;
            while ( konst.valu_co.lit.ref_va[i].p_nextre != NULL )
               {
               ++i;
               printf("#%d.%d",konst.valu_co.lit.ref_va[i].seq_val,
                                 konst.valu_co.lit.ref_va[i].ord_val);
               }
            break;

         default:
            printf("unknown value type");
            break;
         }
      }

   return(0);
  }

/********************************************************/
