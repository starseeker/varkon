/**********************************************************************
*
*    pretty.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    Pretty print routins for PM structures to MBS source format
*
*    short ppinit()    Initiera pretty-printern
*    short pprsts()    print statement to string
*    short pprst()     print statement
*    short pprsl()     print statement list
*    short pprexs()    print expression to string
*    short pprex()     print expression
*    short pprel()     print expression list
*
*    short pprmo();    print modul
*
*    short pprpa()     print module parameter
*    short pprpl()     print module parameter list
*    short pprnp()     print named parameter 
*    short pprnl()      print named parameter list
*    short pprco()     print condition 
*    short pprcl()     print condition list
*    void  pprint()    print string to Pretty's line buffer
*    void  pprlin()    output Prtty's line buffer to MBS-file
*    void  ppkeyw()    print key word to line buffer
*    short ppccol()    returns current column
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
#include <string.h>
#include <ctype.h>

static short ppinit(char  mode, FILE *fp);
static short pprsl(pm_ptr stlist);
static short pprel(pm_ptr exlist);
static short pprnl(pm_ptr nplist);
static short pprnp(pm_ptr npla);
static short pprpa(pm_ptr pala);
static short pprpl(pm_ptr palist);
static short pprcl(pm_ptr colist);
static short pprco(pm_ptr cola);
static int   indent(short indlev);
static int   ppdecl(pm_ptr symla);
static int   prsymb(pm_ptr symla);
static int   pparr(STTYTBL *typep, char *symname);
static void  pprint(char *str);
static short ppccol();
static void  pprlin();
static void  ppkeyw(char *keyw);
static void  fixsli(char str[]);
static void  fixflt(double f, char *str);

static FILE *mbsfp = NULL;      /* MBS-file pointer */
static int  ppmode = PPSTRING;  /* Output-mode for pretty */
static short ppstat = 0;        /* Output-status for pretty */
static short level = 0;         /* indentation level for output */
static char pplin[ PPLINLEN ];  /* line buffer */
static char *pplinp = pplin;    /* pointer to current position in pplin */
static pmmoty modtyp = _3D;     /* for vec(x,y,ÄzÅ)  */
static short ppindl = DEFINDL;

/********************************************************/
/*!******************************************************/

        static short ppinit(
        char  mode,
        FILE *fp)

/*      Init pretty.
 *
 *      In:    
 *
 *      FV:    return -   status 
 *
 *      (C)microform ab 1989-03-21 J. Kjellander
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
/*
***Sätt ppmode rätt.
*/
   if ( (ppmode=mode) == PPFILE ) mbsfp = fp;
/*
***Nollställ output-buffert mm.
*/
   pplinp = pplin;
   level = 0;
   ppstat = 0;

   return(0);
}

/********************************************************/
/*!******************************************************/

        short pprsts(
        pm_ptr statla,
        short  geotyp,
        char  *str,
        int    ntkn)

/*      Print statement to string.
 *
 *      In:    statla  =>  Satsens adress i PM
 *             geotyp  =>  2D eller 3D
 *            *str     =>  Buffert all lägga resultatet i
 *             ntkn    =>  Max antal tecken som får genereras
 *
 *      FV:    return -   status 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short status;
/*
***Initiera.
*/
   ppinit(PPSTRING,NULL);
   if ( geotyp == 2 ) modtyp = _2D;
   else               modtyp = _3D;
/*
***Dekompilera. Negativ status från pprst() innebär att
***modulen är konstig på något sätt.
*/
   status = pprst(statla);
/*
***Negativ ppstat innebär att raden inte fick plats i pp:s
***buffert, dvs. var längre än PPLINLEN.
*/
   if ( ppstat < 0 ) return(erpush("PM2492",""));
/*
***En rad om max PPLINLEN tecken finns nu i pplin.
***Den får dock inte vara längre än ntkn för att anropande
***rutiner skall kunna ta hand om den.
*/
   if ( strlen(pplin) < ntkn ) strcpy(str,pplin);
   else return(erpush("PM2492",""));

   return(status);
 }

/********************************************************/
/*!******************************************************/

        short pprst(
        pm_ptr statla)

/*      Print statement.
 *
 *      In:    statla  =>  PM-pointer to statement
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
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

/*   string valstrÄ PPVALSLEN Å;   Är detta rätt ??? jk */

   char valstr[ PPVALSLEN ];

   if ( statla == (pm_ptr)NULL )         /* empty statement */
      {
      indent( level );
      pprint( ";" );
      return(0);                         /* 1996-04-30 JK */
      }

/*
***BLOCK COMMENT
*/
/*
 ! ppblock( statla );  
*/

   if ( ( status = pmgsta( statla, &np ) ) != 0 )
      {  
      return( status );                  /* Error */
      }

   if ( np->noclst != STAT )
      {
      return( erpush( "PM2512", "" ) );  /*Error Not a statement node */
      }

   switch ( np->suclst )
     {
     case ASS_ST:
        assp = &(np->stsubc.ass_st);
        indent( level );

        pprex(assp->asvar, PP_NOPRI);    /* print left expression */
        pprint( ":=");
        pprex(assp->asexpr, PP_NOPRI);   /* print right expression */

        break;

     case IF_ST:
        ifp = &(np->stsubc.if_st);
        pprlin();
        indent( level );

        pprcl(ifp->ifcond);              /* print condition list */
        if ( ifp->ifstat != (pm_ptr)NULL )
           {
           indent( level );
           ppkeyw( "ELSE " );
/**           pplisc( ifp->stat ); **/
           pprlin();
           pprsl(ifp->ifstat);           /* print statement list */
           }
 
        indent( level );
        ppkeyw( "ENDIF" );

        break;

     case FOR_ST:
        forp = &(np->stsubc.for_st);
        pprlin();
        indent( level );
        ppkeyw( "FOR " );

        prsymb(forp->fovar);
        pprint( " := " );
        pprex(forp->fostar, PP_NOPRI);

        ppkeyw( " TO " );
        pprex(forp->foto, PP_NOPRI);

        if ( forp->fostep != (pm_ptr)NULL )
           {
           ppkeyw( " STEP " );
           pprex(forp->fostep, PP_NOPRI);
           }

        ppkeyw( " DO " );
/*
***LIST COMMENT
*/
/*
   ! pplisc(forp->fordo);
*/

        pprlin();

        pprsl( forp->fordo );

        indent(level);
        ppkeyw( "ENDFOR" );

        break;

     case GO_ST:
        gotop = &(np->stsubc.gotost);
        indent( level );
        ppkeyw( "GOTO " );

        prsymb(gotop->golabe);            /* print label */

        break;

     case PRO_ST:
        procp = &(np->stsubc.procst);
        indent( level );

        prsymb( procp->prname );          /* print routine name */ 
        pprint( "(" );

/*
***if routine "set" special treatment
*/
/*
***SET_BASIC(), 4/6/91, J. Kjellander
*/
        if ((status = stratt( procp->prname, &idclass, &str )) != 0)
           return( status );

        if(strcmp(str,"SET") == 0  ||  strcmp(str,"SET_BASIC") == 0 )
           pprnl( procp->pracva );        /* print named parameter list */
        else 
           pprel( procp->pracva );        /* print value parameter list */

        pprint( ")" );

        break;

     case PART_ST:
        partp = &(np->stsubc.partst);
        indent( level );
        ppkeyw( "part" );
        pprint( "(" );

        sprintf( valstr, "#%d", partp->geident );
        pprint( valstr );
        pprint( "," );
/*
***Olika representation av den anropade modulens namn from. V1.12.
*/
        if ( partp->modname > 0 )
           {
           if ( (status=pmgstr(partp->modname,&str)) < -1 ) return( status );
           pprint(str);
           }
        else
           {
           pprex(-partp->modname,PP_NOPRI);
           pprint(",");
           }

        pprint( "(" );
        pprel( partp->modpara );
        pprint( ")" );

        if ( partp->partpara != (pm_ptr)NULL )
           {
           pprint( "," );
           pprel( partp->partpara );
           }
        if ( partp->partacna != (pm_ptr)NULL )
           {
           pprint( ":" );
           pprnl( partp->partacna );
           }

        pprint( ")" );

        break;

     case GEO_ST:
        geop = &(np->stsubc.geo_st);
        indent( level );

        prsymb( geop->gename );            /* print routine name */ 
        pprint( "(" );
        sprintf( valstr, "#%d", geop->geident );
        pprint( valstr );

        if ( geop->geacva != (pm_ptr)NULL )
           {
           pprint( "," );
           pprel( geop->geacva );          /* print value parameter list */
           }
        if ( geop->geacna != (pm_ptr)NULL )
           {
           pprint( ":" );
           pprnl( geop->geacna );          /* print named parameter list */
           }

        pprint( ")" );

        break;

     case LAB_ST:
        labstp = &(np->stsubc.labest);
        level --;
        indent( level );

        prsymb(labstp->lalabe);            /* print label */
        pprint( ": " );
        level ++;
        pprst(labstp->lastat);             /* print statement */

        break;

     default:
        return( erpush( "PM2512", "" ) );
        break;
     }

   if ( np->suclst != LAB_ST )
      {
      pprint( ";" );
      pprlin();
      }

   switch( np->suclst )
      {
      case IF_ST:

      case FOR_ST:
         pprlin();

         break;
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short pprsl(
        pm_ptr stlist)

/*      Print statement list .
 *
 *      In:    stlist  =>  PM-pointer to statement list (tconc-node)
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;     /* PM-pointer to list node */ 
   pm_ptr nextla;     /* PM-pointer to next list node */
   pm_ptr statla;     /* PM-pointer to statment node */
   short status;

   level ++;

   if ( stlist == (pm_ptr)NULL )
      {
      level --;
      return( 0 );
      }
/*
***get first list node in list
*/
   if ( ( status = pmgfli( stlist, &listla ) ) != 0 )
      {  
      return( status );         /* Error */
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
      if ( ( status = pprst( statla ) ) != 0 )
         return( status );

      listla = nextla;
      }

   level --;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pprexs(
        pm_ptr exprla,
        short  geotyp,
        char  *str,
        int    ntkn)

/*      Print expression to string.
 *
 *      In:    exprla  =>  Uttryckets adress i PM
 *             geotyp  =>  2D eller 3D
 *            *str     =>  Buffert all lägga resultatet i
 *             ntkn    =>  Max antal tecken som får genereras
 *
 *      FV:    return  -   status
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short status;
/*
***Initiera.
*/
   ppinit(PPSTRING,NULL);
   if ( geotyp == 2 ) modtyp = _2D;
   else               modtyp = _3D;
/*
***Dekompilera. Negativ status från pprex() innebär att
***modulen är konstig på något sätt.
*/
   status = pprex(exprla,PP_NOPRI);
/*
***Negativ ppstat innebär att raden inte fick plats i pp:s
***buffert, dvs. var längre än PPLINLEN.
*/
   if ( ppstat < 0 ) return(erpush("PM2492",""));
/*
***En rad om max PPLINLEN tecken finns nu i pplin.
***Den får dock inte vara längre än ntkn för att anropande
***rutiner skall kunna ta hand om den.
*/
   if ( strlen(pplin) < ntkn ) strcpy(str,pplin);
   else return(erpush("PM2492",""));

   return(status);
  }

/********************************************************/
/*!******************************************************/

        short pprex(
        pm_ptr exprla,
        ppopri inpri)

/*      Print expression.
 *
 *      In:    exprla  =>  PM-pointer to expression node
 *             inpri   =>  in - callers priority
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMEXNO *np;       /* c-pointer to expression node */
   PMFUNCEX *funcp;  /* c-pointer to func_expr node */
   PMBINEX *binp;    /* c-pointer to bin_expr node */
   PMUNEX  *unp;     /* c-pointer to unary_exper node */
   PMLITEX *litp;    /* c-pointer to lit_expr node */
   PMREFEX *refp;    /* c-pointer to ref. value in PM */
   PMVECVA *vecp;    /* c-pointer to vector value */
   short status;
   string str;
   char valstr[ PPVALSLEN ];
   char tmpstr[ PPLINLEN ];

   bool parflg = FALSE;
   ppopri currpri = PP_NOPRI;                   /* current priority */

   if ( exprla == (pm_ptr)NULL )                /* empty (default) expression */
      {
      return(0);
      }

   if ( ( status = pmgexp( exprla, &np ) ) != 0 )
      {  
      return( status );                         /* Error */
      }

   if ( np->noclex != EXPR )
      {  
      return( erpush( "PM2522", "" ) );         /*Error  Not a expression node */
      }

   switch ( np->suclex )
      {
      case C_UN_EX:
         unp = &( np->ex_subcl.unop_ex );

         switch ( unp->unop )
            {
            case PM_NOT:
               currpri = PP_NOT;
               break;

            case PM_MINUS:
               currpri = PP_ADDOP;
               break;
            }

/*
***if callers priority are higher then current, use parenteses
*/
         if ( inpri > currpri )
            {
            parflg = TRUE;
            pprint( "(" );
            }

         switch ( unp->unop )
            {
            case PM_NOT:
               ppkeyw( "NOT " );
               break;

            case PM_MINUS:
               pprint( "-" );
               break;

            default:
               printf( "unknown unary operator \n" );
               break;
            }

         pprex( unp->p_unex, currpri + 1 );   /* print expression */

         if ( parflg )
            pprint( ")" );

         break;

      case C_BIN_EX:

         binp = &( np->ex_subcl.binop_ex );
/*
***set current priority
*/
         switch ( binp->binop )
            {
            case PM_OR:
               currpri = PP_OR;
               break;

            case PM_AND:
               currpri = PP_AND;
               break;

            case PM_GT:
            case PM_GE:
            case PM_EQ:
            case PM_NE:
            case PM_LT:
            case PM_LE:
               currpri = PP_RELOP;
               break;

            case PM_PLUS:
            case PM_MINUS:
               currpri = PP_ADDOP;
               break;

            case PM_DIV:
            case PM_MUL:
               currpri = PP_MULOP;
               break;

            case PM_EXP:
               currpri = PP_EXP;
               break;
            }
/*
***if callers priority are higher then current, use parenteses
*/
         if ( inpri > currpri )
            {
            parflg = TRUE;
            pprint( "(" );
            }

         pprex( binp->p_bin_l, currpri );   /* print left expression */

         switch ( binp->binop )
            {
            case PM_GT:
               pprint( " > " );
               break;

            case PM_GE:
               pprint( " >= " );
               break;

            case PM_EQ:
               pprint( " = " );
               break;

            case PM_NE:
               pprint( " <> " );
               break;

            case PM_LT:
               pprint( " < " );
               break;

            case PM_LE:
               pprint( " <= " );
               break;

            case PM_AND:
               ppkeyw( " AND " );
               break;

            case PM_OR:
               ppkeyw( " OR " );
               break;

            case PM_PLUS:
               pprint( " + " );
               break;

            case PM_MINUS:
               pprint( " - " );
               break;

            case PM_DIV:
               pprint( "/" );
               break;

            case PM_MUL:
               pprint( "*" );
               break;

            case PM_EXP:
               pprint( "**" );
               break;

            default:
               printf( "unknown operator \n" );
               break;
            }

         pprex( binp->p_bin_r, currpri + 1 );   /* print right expression */

         if ( parflg )
            pprint( ")" );

         break;

      case C_LIT_EX:

         litp = &(np->ex_subcl.lit_ex);

         switch ( litp->lit_type )
            {
            case C_INT_VA:
               sprintf(valstr, "%d", litp->litex.int_li );
               pprint( valstr );
               break;

            case C_FLO_VA:
               fixflt( litp->litex.float_li,valstr );
               pprint( valstr );
               break;

            case C_STR_VA:
               if ( ( status = pmgstr( litp->litex.str_li, &str ) ) != 0 )
                   return( status );
               strcpy( tmpstr, str );
               fixsli( tmpstr );
               pprint( tmpstr );
               break;

            case C_VEC_VA:
               if ( ( status = pmgvec( litp->litex.vec_li, &vecp ) ) != 0 )
                   return( status );
               strcpy(valstr,"vec(");
               fixflt(vecp->x_val,tmpstr);
               strcat(valstr,tmpstr);
               strcat(valstr,",");
               fixflt(vecp->y_val,tmpstr);
               strcat(valstr,tmpstr);

               if ( modtyp == _3D )
                  {
                  strcat(valstr,",");
                  fixflt(vecp->z_val,tmpstr);
                  strcat(valstr,tmpstr);
                  }

               strcat( valstr,")" );
               pprint( valstr );
               break;

            case C_REF_VA:
               refp = &litp->litex.ref_li;
/*
***Om 1:a sekvensnumret < 0 är det en global referens. I så fall
***skall inte bara dubbla "#" skrivas ut utan också minustecknet
***strippas. 3/11-94 J. Kjellander
*/
               if ( refp->ord_lit == 1 )
                  if ( refp->seq_lit < 0 )
                     sprintf(valstr,"##%d",-(refp->seq_lit));
                  else
                     sprintf(valstr,"#%d",refp->seq_lit);
               else
                  if ( refp->seq_lit < 0 )
                     sprintf(valstr,"##%d.%d",-(refp->seq_lit),refp->ord_lit);
                  else
                    sprintf(valstr,"#%d.%d",refp->seq_lit,refp->ord_lit);

               pprint( valstr );

               while ( refp->nextref != 0 )
                  {
                  if ( ( status = pmgref( refp->nextref, &refp ) ) != 0 )
                      return( status );

                  if ( refp->ord_lit == 1 )
                     sprintf( valstr, "#%d", refp->seq_lit );
                  else
                     sprintf( valstr, "#%d.%d", refp->seq_lit, refp->ord_lit );
                  pprint( valstr );
                  }
               break;
            }

         break;

      case C_ID_EX:
         prsymb( np->ex_subcl.id_ex.p_id );
         break;

      case C_IND_EX:
         prsymb( np->ex_subcl.ind_ex.p_indid );
         pprint( "(" );
         pprel( np->ex_subcl.ind_ex.p_indli );
         pprint( ")" );
         break;

      case C_COM_EX:
         pprex( np->ex_subcl.comp_ex.p_comvar, PP_NOPRI);

         switch(np->ex_subcl.comp_ex.p_cfield)
            {
            case PM_X:
               pprint(".x");
               break;
            case PM_Y:
               pprint(".y");
               break;
            case PM_Z:
               pprint(".z");
               break;
            }

         break;

      case C_FUN_EX:
         funcp = &(np->ex_subcl.func_ex);
         prsymb( funcp->funcid );           /* print routine name */ 
         pprint( "(" );
         pprel( funcp->p_funcar );          /* print value parameter list */  
         pprint( ")" );

         break;

      default:
         return( erpush( "PM2522", "" ) );
         break;
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short pprel(
        pm_ptr exlist)

/*      Print expression list.
 *
 *      In:    exprla  =>  PM-pointer to expression list 
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;     /* PM-pointer to list node */ 
   pm_ptr nextla;     /* PM-pointer to next list node */
   pm_ptr exprla;     /* PM-pointer to statment node */
   short status;

   if ( exlist == (pm_ptr)NULL )
      {
      return( 0 );
      }
/*
***get first list node in list
*/
   if ( ( status = pmgfli( exlist, &listla ) ) != 0 )
      {  
      return( status );          /* Error */
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
      if ( ( status = pprex( exprla, PP_NOPRI ) ) != 0 )
          return( status );

      listla = nextla;

      if ( listla != (pm_ptr)NULL )
         pprint( ", " );
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short pprnl(
        pm_ptr nplist)

/*      Print named parameter list.
 *
 *      In:    nplist  =>  PM-pointer to named parameter list (TCONC_NODE) 
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;     /* PM-pointer to list node */ 
   pm_ptr nextla;     /* PM-pointer to next list node */
   pm_ptr npla;       /* PM-pointer to named parameter node */
   short status;

   if ( nplist == (pm_ptr)NULL )
      {
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
      if ( ( status = pprnp( npla ) ) != 0 )
          return( status );

      listla = nextla;

      if ( listla != (pm_ptr)NULL )
         pprint( "," );
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short pprnp(
        pm_ptr npla)

/*      Print named parameter.
 *
 *      In:    npla    =>  PM-pointer to named parameter (PMNPNO) 
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 *****************************************************!*/

  {
   PMNPNO *npp;
   static char *namtab[] = { "ERROR",   /* NULL */
                             "PEN",     /* PMPEN */
                             "LEVEL",   /* PMLEVEL */
                             "LFONT",   /* PMLFONT */
                             "AFONT",   /* PMAFONT */
                             "LDASHL",  /* PMLDASHL */
                             "ADASHL",  /* PMADASHL */
                             "TSIZE",   /* PMTSIZE */
                             "TWIDTH",  /* PMTWIDTH */
                             "TSLANT",  /* PMTSLANT */
                             "DTSIZ",   /* PMDTSIZE */
                             "DASIZ",   /* PMDASIZE */
                             "DNDIG",   /* PMDNDIG */
                             "DAUTO",   /* PMDAUTO */
                             "XFONT",   /* PMXFONT */
                             "XDASHL",  /* PMXDASHL */
                             "BLANK",   /* PMBLANK */
                             "HIT",     /* PMHIT */
                             "SAVE",    /* PMSAVE */
                             "CFONT",   /* PMCFONT */
                             "CDASHL",  /* PMCDASHL */
                             "TFONT",   /* PMTFONT */
                             "SFONT",   /* PMSFONT */
                             "SDASHL",  /* PMSDASHL */
                             "NULINES", /* PMNULIN */
                             "NVLINES", /* PMNVLIN */
                             "WIDTH",   /* PMWIDTH */
                             "TPMODE",  /* PMTPMODE */
                             "MFONT",   /* PMMFONT */
                             "PFONT",   /* PMPFONT */
                             "PSIZE"};  /* PMPSIZE */

   short status;

   if ( ( status = pmgnpa( npla, &npp ) ) != 0 )
      {  
      return( status );                   /* Error */
      }

   if ( npp->noclnp != NAMEPA )
      {  
      return( erpush( "PMXXX1", "" ) );   /*Error  Not a named parameter node */
      }

   if ((npp->par_np < PMPEN) || (npp->par_np > PMPSIZE))
      erpush( "PP1014" ,"" );
   else
      pprint( namtab[(int)npp->par_np] );

   pprint( "=" );
   pprex(npp->p_val_np, PP_NOPRI);        /* print named parameter */
                                          /* value expression */
   return(0);
  }

/********************************************************/
/*!******************************************************/

        static short pprpa(
        pm_ptr pala)

/*      Print module parameter.
 *
 *      In:    nplist  =>  PM-pointer to module parameter node 
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMPANO *np;             /* c-pointer to module parameter node */
   string str;
   char tmpstr[ PPLINLEN ];
   STVAR var;              /* interface struct for a parameter */
   short status;

   if ( ( status = pmgpar( pala, &np ) ) )
      {  
      return( status );    /* Error */
      }

   if ( np->noclpa != PARAM )
      {  
      return( erpush( "PM2532", "" ) );   /*Error  Not an module parameter node */
      }

   ppdecl( np->fopa_ );                   /* print formal parameter name */ 

/*
***get and print default value
*/
   if ( ( status = strvar( np->fopa_, &var ) ) != 0 )
      return( status );

   if ( var.def_va != (pm_ptr)NULL )
      {
      pprint( ":=" );
      pprex( var.def_va, PP_NOPRI );
      }

   if ( ( status = pmgstr( np->ppro_, &str ) ) != 0 )
      return( status );

   if ( strlen( str ) > 0 )
      {
      pprint( " > " );
      strcpy( tmpstr, str );
      fixsli( tmpstr );
      pprint( tmpstr );
      }

  return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short pprpl(
        pm_ptr palist)

/*      Print module parameter list.
 *
 *      In:    palist  =>  PM-pointer to module parameter list 
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;     /* PM-pointer to list node */ 
   pm_ptr nextla;     /* PM-pointer to next list node */
   pm_ptr parala;     /* PM-pointer to module parameter node */
   short status;

   if ( palist == (pm_ptr)NULL )
      {
      return( 0 );
      }
/*
***get first list node in list
*/
   if ( ( status = pmgfli( palist, &listla ) ) != 0 )
      {  
      return( status );                      /* Error */
      }

   if ( listla != (pm_ptr)NULL ) pprlin();   /* Ny rad före 1:a parametern. */

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
      if ( ( status = pprpa( parala ) ) != 0 )
         return( status );

      listla = nextla;

      if ( listla != (pm_ptr)NULL )
         {
         pprint( ";" );
         pprlin();
         }
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pprmo(
        char   mode,
        FILE  *filpek)

/*      Print module.
 *
 *      In:    palist  =>  Output-mode 
 *            *filpek  =>  MBS-filpekare
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMMONO *np;                  /* c-pointer to module header */
   string str;
   short status;
   pm_ptr prev=(pm_ptr)NULL;    /* for ST dump */
   stidcl idcl;                 /* for ST dump */
   char *name;                  /* for ST dump */
   STVAR var;
/*
***Initiera pretty.
*/
   ppinit(mode,filpek);

   if ( ( status = pmgmod( (pm_ptr)0, &np ) ) != 0 )
      {  
      return( status );         /* Error */
      }

   if ( np->monocl != MODULE )
      {  
      return( erpush( "PM2542", "" ) );   /*Error  Not a module node */
      }

   indent( level );
/*
***Print module heade
*/
   if      ( np->moat_ == LOCAL )  pprint( "LOCAL " );
   else if ( np->moat_ == GLOBAL ) pprint( "GLOBAL " );
   else if ( np->moat_ == BASIC )  pprint( "BASIC " );
   else if ( np->moat_ == MACRO )  pprint( "MACRO " );

   modtyp = np->moty_;

   if ( np->moty_ == _2D ) ppkeyw( "DRAWING " );
   else if ( np->moty_ == _3D ) ppkeyw( "GEOMETRY " );

   if ( ( status = pmgstr( np->mona_, &str ) ) != 0 ) return( status );

   ppkeyw( "MODULE " );
   pprint( str );
   pprint( "(" );

   level = ppccol();             /* set level to start of module parameter */
   pprpl( np->ppali_ );          /* print parameter list */
   level = 0;

   pprint( ");" );
   pprlin();
   pprlin();
/*
***print declaration part
*/
   do
     {
     stgnid( prev, &idcl, &name, &prev );
     if ( prev != (pm_ptr)NULL )
        {
        switch ( idcl )
           {
           case ST_VAR:
              strvar( prev, &var );
              if ( var.kind_va != ST_LOCVA )
                  goto nextsym;
/*
***will not be terminated with "break"
*/
           case ST_CONST:
/*
***print declaretion local variable or constant
*/
/*            ppbloc( prev ); */
              indent( level );
              ppdecl(prev);
              pprint( "; " );
/*            ppstac( prev ); */
              pprlin();
              break;

           default:
              break;
           }
        }
     nextsym:   ;
     } while( prev != (pm_ptr)NULL );

   pprlin();
   ppkeyw( "BEGINMODULE" );
   pprlin();
   pprlin();

   pprsl( np->pstl_ );           /* print statement list */

   pprlin();
   ppkeyw( "ENDMODULE" );
  pprlin();

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short pprcl(
        pm_ptr colist)

/*      Print condition list.
 *
 *      In:    colist  =>  PM-pointer to condition list (TCONC-NODE) 
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;     /* PM-pointer to list node */ 
   pm_ptr nextla;     /* PM-pointer to next list node */
   pm_ptr cola;       /* PM-pointer to condition node */
   short status;

   if ( colist == (pm_ptr)NULL )
      {
      return( 0 );
      }
/*
***get first list node in list
*/
   if ( ( status = pmgfli( colist, &listla ) ) != 0 )
      {  
      return( status );            /* Error */
      }

   ppkeyw( "IF " );

/*
***get contents of list node
*/
   if ( ( status = pmglin( listla, &nextla, &cola ) ) != 0 )
      return( status );

/*
***print condition
*/
   if ( ( status = pprco( cola ) ) != 0 )
      return( status );

   listla = nextla;

   while ( listla != (pm_ptr)NULL )
      { 
/*
***get contents of list node
*/
      if ( ( status = pmglin( listla, &nextla, &cola ) ) != 0 )
         return( status );

      indent(level);          /*JK*/
      ppkeyw( "ELIF " );

/*
***print condition
*/
      if ( ( status = pprco( cola ) ) != 0 )
         return( status );

      listla = nextla;
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        static short pprco(
        pm_ptr cola)

/*      Print condition.
 *
 *      In:    cola    =>  PM-pointer to condition (COND_NODE) 
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMCONO *cop;          /* c-pointer to condition (COND_NODE) */
   short status;

   if ( ( status = pmgcon( cola, &cop ) ) != 0 )
      {  
      return( status );                   /* Error */
      }

   if ( cop->noclco != COND )
      {  
      return( erpush( "PMXXX2", "" ) );   /*Error  Not a condition node */
      }

   pprex(cop->p_cond, PP_NOPRI);          /* print cond. expr. */

   ppkeyw( " THEN " );

/*
!   pplisc( cola );
*/

   pprlin();

   pprsl(cop->p_stl_co);                  /* print statement list */

   return(0);
  }

/********************************************************/
/*!******************************************************/

        static int indent(
        short indlev)

/*      Print indentation.
 *
 *      In:    indlev  =>  Indentation level 
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   int i;
   int nustep;

   nustep = indlev * ppindl;
   if ( nustep > MAXIND )
      nustep = MAXIND;

   for ( i = 1; i <= nustep; i++ ) 
      pprint( " " );

   return(0);
  }

/********************************************************/
/*!******************************************************/

        static int ppdecl(
        pm_ptr symla)

/*      Print declaration.
 *
 *      In:    symla   =>  PM-pointer to symbol table 
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   stidcl idclass;         /* identifier class */
   string symname;         /* string for symbol name */
   STVAR var;              /* st structure for a variable */
   STCONST konst;          /* st structure for a constant */
   STTYTBL typ;            /* MBS-type definition structure */
   char tmpstr[ PPLINLEN ];
   pm_ptr pmval;           /* PM pointer to value for constant declaration */
   short status;

   if ( ( status = stratt( symla, &idclass, &symname ) ) != 0 )
      return( status );

   switch ( idclass )
      {
      case ST_VAR:
         if ( ( status = strvar( symla, &var ) ) != 0 )
            return( status );
      
         if ( var.kind_va == ST_RPAVA )
            ppkeyw( "VAR " );

         if ( var.type_va == (pm_ptr)NULL )
            {
            pprint( "UNDEF " );
            return(0);
            }

         if ( ( status = strtyp( var.type_va, &typ ) ) != 0 )
            return( status );

         switch ( typ.kind_ty )
            {
            case ST_INT:
               ppkeyw( "INT " );
               pprint( symname );
               break;
    
            case ST_FLOAT:
               ppkeyw( "FLOAT " );
               pprint( symname );
               break;
   
            case ST_STR:
               ppkeyw( "STRING " );
               pprint( symname );
              break;
    
            case ST_VEC:
               ppkeyw( "VECTOR " );
               pprint( symname );
               break;
    
            case ST_REF:
               ppkeyw( "REF " );
               pprint( symname );
               break;
     
            case ST_FILE:
               ppkeyw( "FILE " );
               pprint( symname );
              break;

            case ST_ARR:
               pparr( &typ, symname );
               break;
 
            default:
               printf( "Unknown type: %d ", typ.kind_ty );
               break;
            }
        
         if ( typ.kind_ty == ST_STR )
            {
            pprint( "*" );
            sprintf( tmpstr, "%d", typ.size_ty - 1 );
            pprint( tmpstr );
            }
         break;

      case ST_CONST:
         pprint( "CONSTANT " );

         if ( ( status = stgcon( symla, &konst, &pmval ) ) != 0 )
            return( status );

         if ( konst.type_co == (pm_ptr)NULL )
            {
            pprint( "UNDEF " );
            return(0);
            }

         if ( ( status = strtyp( konst.type_co, &typ ) ) != 0 )
            return( status );

         switch ( typ.kind_ty )
            {
            case ST_INT:
               ppkeyw( "INT " );
               break;

            case ST_FLOAT:
               ppkeyw( "FLOAT " );
               break;

            case ST_STR:
               ppkeyw( "STRING " );
               break;

            case ST_VEC:
               ppkeyw( "VECTOR " );
               break;

            case C_REF_VA:
               ppkeyw( "REF " );
               break;

            default:
               printf( "Unknown type: %d ", typ.kind_ty );
               break;
            }
      
         pprint( symname );          /* print symbol name */

         pprint( "=" );

         pprex( pmval, PP_NOPRI );   /* Print constant value definition */
         break;

      case ST_LABEL:
      case ST_FUNC:
      case ST_PROC:
      case ST_MOD:

      break;


      default:
         printf( "Not a symbol. PM-adr: %d ", symla );
         break;
      }
   return(0);
  }

/********************************************************/
/*!******************************************************/

        static int prsymb(
        pm_ptr symla)

/*      Print symbol name.
 *
 *      In:    symla   =>  PM-pointer to symbol table 
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   stidcl idclass;        /* identifier class */
   string symname;        /* string for symbol name */
   short  status;
   short  i=0;
   char   tmpstr[80];     /* symbolnamn */
   int    c;

   if ( ( status = stratt( symla, &idclass, &symname ) ) != 0 )
      return( status );

   strcpy(tmpstr,symname);
/*
***Små bokstäver för funktioner och procedurer
*/
   if ( idclass == ST_PROC || idclass == ST_FUNC )
      {
      for ( i=0; i<strlen(tmpstr); ++i )
        {
        c = (int)tmpstr[i];
        tmpstr[i] = (char)tolower(c);
        }
      }

   pprint(tmpstr);   /* print symbol name */

   return(0);
  }  

/********************************************************/
/*!******************************************************/

        static int pparr(
        STTYTBL *typep,
        char    *symname)

/*      Print array declaration.
 *
 *      In:   *typep   =>  Pointer to type descriptor 
 *            *symname =>  Pointer to symbol name 
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char dimstr[ PPLINLEN ];
   char tmpstr[ PPVALSLEN ];
   STARRTY arr;

   dimstr[ 0 ] = '\0';

   strarr( typep->arr_ty, &arr );

   if ( arr.low_arr == 1 )
      sprintf( tmpstr, "%d", arr.up_arr );
   else
      sprintf( tmpstr, "%d:%d", arr.low_arr, arr.up_arr );

   strcat( dimstr, tmpstr );

   strtyp( arr.base_arr, typep );

   while( typep->kind_ty == ST_ARR )
      {
      strcat( dimstr, "," );

      strarr( typep->arr_ty, &arr );

      if ( arr.low_arr == 1 )
         sprintf( tmpstr, "%d", arr.up_arr );
      else
         sprintf( tmpstr, "%d:%d", arr.low_arr, arr.up_arr );

      strcat( dimstr, tmpstr );

      strtyp( arr.base_arr, typep );
      }

   switch ( typep->kind_ty )
     {
     case ST_INT:
        ppkeyw( "INT " );
        break;
     case ST_FLOAT:
        ppkeyw( "FLOAT " );
        break;
     case ST_STR:
        ppkeyw( "STRING " );
        break;
     case ST_VEC:
        ppkeyw( "VECTOR " );
        break;
     case ST_REF:
        ppkeyw( "REF " );
        break;
     case ST_FILE:
        ppkeyw( "FILE " );
        break;
     }

   pprint( symname );
   pprint( "(" ); 
   pprint( dimstr );
   pprint( ")" );

   return(0);
  }  

/********************************************************/
/*!******************************************************/

        static void pprint(
        char    *str)

/*      Print string to line buffer.
 *
 *      In:    *str    =>  String to be printed in pplin 
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char *newlinp;      /* new line pointer into pplin */
   short newlinl;      /* new line length */

   newlinp = pplinp + strlen( str );
   newlinl = (short)(newlinp - pplin );
/*
***Skriver vi till en sträng, skall allt hamna på en rad,
***annars max PPMXLINL tecken/rad.
*/
   if ( ppmode != PPSTRING  &&  newlinl > PPMXLINL )
      {
/*
***line to long, print first current content
*/
      pprlin();

      level ++;
      indent( level );
      newlinp = pplinp + strlen( str );
      newlinl = (short)(newlinp - pplin );
      if ( newlinl > PPMXLINL )
         {
/*
***Item to long to fit in line limit, print long line
*/
         strcpy( pplinp, str );
         pplinp = newlinp;
         pprlin();
         indent( level );
         }
      else 
         {
         strcpy( pplinp, str );
         pplinp = newlinp;
         }
      level --;
      }
/*
***Om inte allt får plats i pp:s buffert men allt borde få
***plats där (lång kurva till sträng) sätter vi ppstat = -1
***utan att lagra någon text.
*/
   else
      {
      if ( newlinl < PPLINLEN )
         {
         strcpy(pplinp,str);
         pplinp = newlinp;
         }
      else ppstat = -1;
      }

   return;
  }

/********************************************************/
/*!******************************************************/

        static short ppccol()

/*      Get current column in line buffer.
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short clinlen;
   short indcol;               /* indentation column */

   clinlen = (short)(pplinp - pplin);
   indcol = clinlen / ppindl;

   if ( ( clinlen % ppindl ) != 0 )
      {  
      indcol = indcol + 1;     /* not indentation alignment */
      }

   return( indcol );
  }

/********************************************************/
/*!******************************************************/

        static void pprlin()

/*      Print line buffer to MBS-file and reset line buffer.
 *
 *      FV:    return  -   error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   if ( ppmode == PPFILE ) fprintf( mbsfp, "%s\n", pplin );
   else if ( ppmode == PPIGLIST ) WPalla(pplin,(short)1);

   if ( ppmode != PPSTRING )
      {
      pplinp = pplin;
      pplin[ 0 ] = '\0';
      }

   return;
  }

/********************************************************/
/*!******************************************************/

        static void ppkeyw(
        char    *keyw)

/*      Print key word into line buffer.
 *      This routine can be changed to print the key words 
 *      in specified case ( lower, uper etc. )
 *
 *      In:    *keyw   =>  String with key word 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pprint( keyw );
   return;
  }

/********************************************************/
/*!******************************************************/

        static void fixsli(
        char str[])

/*      Fix string to be MBS-syntax.
 *
 *      In:     str[]   =>  String to be fixed 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   int i, j;
   char tmpstr[ PPLINLEN ];

   i = 0;
   j = 1;

   tmpstr[ 0 ] = '\"';

   while ( str[ i ] != '\0' )
      {
      tmpstr[ j ] = str[ i ];
      if ( str[ i ] == '\"' )
         {
         j = j + 1;
         tmpstr[ j ] = '\"';
         }

      j = j + 1;
      i = i + 1;
      }

   tmpstr[ j ] = '\"';

   j = j + 1;

   tmpstr[ j ] = '\0';

   strcpy( str, tmpstr );

   return;
  }

/********************************************************/
/*!******************************************************/

        static void fixflt(
        double  f,
        char   *str)

/*      Formattera flyttal.
 *
 *      In:     f    =>  Flyttal  
 *             *str  =>  Utbuffer 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
/*
***Här gäller det att skriva ut med full precision utan
***att generera onödiga nollor. Dock skall alltid decimal-
***punkt och en nolla skrivas ut. Vi börjar med 14 signifikanta
***siffror och G-format.
*/
   sprintf(str,"%.14G",f);
/*
***Om talet är mycket stort eller litet kan detta resultera
***i exponentform. Om inte kollar vi att avslutande decimalpunkt
***och minst en ytterligare siffra finns med.
*/
   if ( (strchr(str,(int)'E') == (pm_ptr)NULL)  &&
        (strchr(str,(int)'e') == (pm_ptr)NULL) )
      {
      if ( strchr(str,(int)'.') == (pm_ptr)NULL )
        {
        strcat(str,".0");
        }
      }
  }

/********************************************************/
