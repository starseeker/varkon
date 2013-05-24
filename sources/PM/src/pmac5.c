/**********************************************************************
*
*    pmac5.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    editing routines for pm structure
*
*    short pmdges();    Delete geometric statement 
*    short pmglst();    Return ID of last geometric statement 
*    short pmdlst();    Delete last geometric statement 
*    short pmchpa();    Change parameter in part-call 
*    short pmchnp();    Change named parameter in part- and geo-statement 
*    short pmlges();    Locate geometric statement 
*    short pmrgps();    Replace geometric- and part-statement 
*    short pmgpla();    Get la of all parameters in geo- and part-statem. 
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

/*!******************************************************/

        short pmdges(
        V2REFVA *ref)

/*      Delete geometric & part statement.
 *
 *      In:    ref    =>  Reference to geometric statement
 *
 *      Out:
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
   pm_ptr prevla;     /* PM-pointer to prev list node */
   pm_ptr statla;     /* PM-pointer to statment node */
   short status;

   PMMONO *np;        /* c-pointer to module node */
   pm_ptr stlla;      /* statement list PM-pointer */
   PMSTNO *stp;       /* c-pointer to statement node */
   PMLINO *prevlip;   /* c-pointer to previous list node */
   PMTCNO *tconcp;    /* c-pointer to tconc node */

/*
***Get PM-pointer to module statement list
*/
   np = (PMMONO *)pmgadr((pm_ptr)0 );
   stlla = np->pstl_;

   if ( stlla == (pm_ptr)NULL )
      {  
      return( erpush( "PM1021", "STAT" ) );     /* empty list ref not in PM */
      }

   tconcp = (PMTCNO *)pmgadr( stlla );

/*
***get first list node in list
*/
   if ( ( status = pmgfli( stlla, &listla ) ) != 0 )
      {  
      return( status );                         /* Error */
      }

/*
***check if first statement in list match
***get contents of list node
*/
   if ( ( status = pmglin( listla, &nextla, &statla ) ) != 0 )
      return( status );

/*
***get statement
*/
   if ( ( status = pmgsta( statla, &stp ) ) != 0 )
      return( status );

/*
***check GEO-id , abs() added 1998-04-06, JK
*/
   if ( (stp->noclst==STAT) && ((stp->suclst==GEO_ST) || (stp->suclst==PART_ST)) )
      {
      if ( stp->stsubc.geo_st.geident == abs(ref->seq_val) )
         {
         if ( nextla == (pm_ptr)NULL )
            {
/*
***delete statement list from module node
*/
            np->pstl_ = (pm_ptr)NULL;
            }
         else
            {
            tconcp->p_head = nextla;
            }
         return( 0 );
         }
      }

   prevla = listla;
   listla = nextla;

   while ( listla != (pm_ptr)NULL )
      { 
/*
***get contents of list node
*/
      if ( ( status = pmglin( listla, &nextla, &statla ) ) != 0 )
         return( status );

/*
***get statement
*/
      if ( ( status = pmgsta( statla, &stp ) ) != 0 )
         return( status );

/**
***check GEO-id
*/
      if ( stp->noclst==STAT && ( stp->suclst==GEO_ST || stp->suclst==PART_ST ) )
         {
         if ( stp->stsubc.geo_st.geident == abs(ref->seq_val) )
            {
/*
***delete statement from list
*/
            prevlip = (PMLINO *)pmgadr( prevla );
            prevlip->p_nex_li = nextla;
/*
***Update tail pointer in tconc-node if last statement in list
*/
            if ( nextla == (pm_ptr)NULL )
               tconcp->p_tail = prevla;
            break;
            }
         }

      prevla = listla;
      listla = nextla;
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmglst(V2REFVA *id)

/*      Return ID of last geometric statement.
 *
 *      FV:    return -   error severity code 
 *
 *      (C) 011010 J.Kjellander
 *
 ******************************************************!*/

  {
   pm_ptr listla;     /* PM-pointer to list node */ 
   pm_ptr nextla;     /* PM-pointer to next list node */
   pm_ptr prevla;     /* PM-pointer to prev list node */
   pm_ptr statla;     /* PM-pointer to statment node */
   short status;


   PMMONO *np;        /* c-pointer to module node */
   pm_ptr stlla;      /* statement list PM-pointer */
   PMTCNO *tconcp;    /* c-pointer to tconc node */
   PMSTNO *statptr;   /* c-pointer to statement node */

/*
***Get PM-pointer to module statement list
*/
   np = (PMMONO *)pmgadr((pm_ptr)0 );
   stlla = np->pstl_;

   if ( stlla == (pm_ptr)NULL )
      {  
      return( erpush( "PM1021", "STAT" ) );    /* empty list ref not in PM */
      }

   tconcp = (PMTCNO *)pmgadr( stlla );
/*
***Get first list node in list
*/
   if ( ( status = pmgfli( stlla, &listla ) ) != 0 ) return( status );
/*
***Check if first statement in list is last 
*/
   if ( ( status = pmglin( listla, &nextla, &statla ) ) != 0 )
      return( status );

   if ( nextla == (pm_ptr)NULL ) goto found;
/*
***Continue the search.
*/
   prevla = listla;
   listla = nextla;

   while ( listla != (pm_ptr)NULL )
      { 
/*
***Get contents of list node
*/
      if ( ( status = pmglin( listla, &nextla, &statla ) ) != 0 )
         return( status );
/*
***Check if last statement
*/
      if ( nextla == (pm_ptr)NULL ) goto found;

      prevla = listla;
      listla = nextla;
      }
/*
***Last statement found. Return ID.
*/
found:
   id->ord_val = 1;
   id->p_nextre = NULL;

    if ( pmgsta(statla, &statptr) == 0 &&
         statptr->noclst == STAT && 
        (statptr->suclst == GEO_ST ||
         statptr->suclst == PART_ST))
      {
      id->seq_val = statptr->stsubc.geo_st.geident;
      }
    else
      {
      id->seq_val = 0;
      }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short pmdlst()

/*      Delete last statement.
 *
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
   pm_ptr prevla;     /* PM-pointer to prev list node */
   pm_ptr statla;     /* PM-pointer to statment node */
   short status;


   PMMONO *np;        /* c-pointer to module node */
   pm_ptr stlla;      /* statement list PM-pointer */
   PMLINO *prevlip;   /* c-poineter to list node */
   PMTCNO *tconcp;    /* c-pointer to tconc node */

/*
***Get PM-pointer to module statement list
*/
   np = (PMMONO *)pmgadr((pm_ptr)0 );
   stlla = np->pstl_;

   if ( stlla == (pm_ptr)NULL )
      {  
      return( erpush( "PM1021", "STAT" ) );    /* empty list ref not in PM */
      }

   tconcp = (PMTCNO *)pmgadr( stlla );
/*
***get first list node in list
*/
   if ( ( status = pmgfli( stlla, &listla ) ) != 0 )
      {  
      return( status );                            /* Error */
      }
/*
***check if first statement in list is last 
***get contents of list node
*/
   if ( ( status = pmglin( listla, &nextla, &statla ) ) != 0 )
      return( status );
/*
***check if last statement
*/
   if ( nextla == (pm_ptr)NULL )
      {
/*
***delete statement list from module node
*/
      np->pstl_ = (pm_ptr)NULL;
      return( 0 );
      }

   prevla = listla;
   listla = nextla;

   while ( listla != (pm_ptr)NULL )
      { 
/*
***get contents of list node
*/
      if ( ( status = pmglin( listla, &nextla, &statla ) ) != 0 )
         return( status );
/*
***check if last statement
*/
      if ( nextla == (pm_ptr)NULL )
         {
/*
***delete statement from list
*/
         prevlip = (PMLINO *)pmgadr( prevla );
         prevlip->p_nex_li = nextla;
         tconcp->p_tail = prevla;
         break;
         }

      prevla = listla;
      listla = nextla;
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmchpa(
        V2REFVA *ref,
        short    nr,
        pm_ptr new_expr,
        pm_ptr *retla)

/*      Change actual parameter in part-call.
 *      If statement not is found an NULL pointer will be returned in retla.
 *      If the parameter in the part statement not is found an error severity code
 *      will be returned as the function value.
 *
 *      In:  *ref      =>  Reference to geometric statement
 *            nr       =>  Parameter number, first parameter = 1
 *            new_expr =>  Expression for new actual value
 *
 *      Out: *retla    =>  PM-pointer to statement changed, NULL if statement
 *                         not found
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;                  /* PM-pointer to list node */ 
   pm_ptr newlnla;                 /* PM-pointer to new list node */
   pm_ptr nextla;                  /* PM-pointer to next list node */
   pm_ptr prevla = (pm_ptr)NULL ;  /* PM-pointer to prev list node */
   pm_ptr exprla;                  /* PM-pointer to expression node */ 
   short  i;                       /* loop variable */
 
   PMMONO *np;                     /* c-pointer to module node */
   pm_ptr stlla;                   /* statement list PM-pointer */
   pm_ptr exlla;                   /* expression list PM-pointer */
   PMSTNO *stp;                    /* c-pointer to statement node */
   PMLINO *newlnp;                 /* c-pointer to new list node */
   PMLINO *currlp;                 /* c-pointer to current list node */
   PMLINO *prevlp;                 /* c-pointer to list node */
   PMTCNO *tconcp;                 /* c-pointer to tconc node */

   *retla = (pm_ptr)NULL;

/*
***Get PM-pointer to module statement list
*/
   np = (PMMONO *)pmgadr((pm_ptr)0 );
   stlla = np->pstl_;

/*
***locate geometric statement
*/
   pmlges( ref, &stlla, &listla );

   if ( listla != (pm_ptr)NULL )
      {
/*
***get statement node and set return value for statement pointer
*/
      pmglin( listla, &nextla, retla );
      pmgsta( *retla, &stp );

      if ( stp->suclst == PART_ST )
         {
         exlla = stp->stsubc.partst.modpara;

         if ( exlla == (pm_ptr)NULL )
            { 
            return( erpush( "PM0114", "" ) );          /* error */
            }

         tconcp = (PMTCNO *)pmgadr( exlla ); 

/*
***get first list node in parameter list
*/
         pmgfli( exlla, &listla );

         if ( listla == (pm_ptr)NULL )
            { 
            return( erpush( "PM0114", "" ) );        /* error */
            }
/*
***get contents of list node
*/
         pmglin( listla, &nextla, &exprla );

/*
***Allocate a block for the new list node in PM
*/
         pmallo((DBint)sizeof( *newlnp ), &newlnla );

/*
***Fill in the list node
*/
         newlnp = (PMLINO *)pmgadr( newlnla );
         newlnp->noclli = LIST;                      /* Node class */
         newlnp->p_no_li = new_expr;                 /* put a link to the generic node */ 
         newlnp->p_nex_li = (pm_ptr)NULL;

         if ( nr == 1 )
            {
/*
***first parameter in list, TCONC node will be changed
*/

            if ( nextla == (pm_ptr)NULL )
               {
/*
***only one parameter in list, TCONC.head equal .tail
*/
               tconcp->p_head = newlnla;
               tconcp->p_tail = newlnla;
               }
            else
               {
/*
***more then one parameter in list; link in first
*/
               currlp = (PMLINO *)pmgadr( listla );
               newlnp->p_nex_li = currlp->p_nex_li;
               tconcp->p_head = newlnla;
               }
            }
         else
            {
/*
***get contents from list node for parameter number "nr"
*/
            for ( i = 2; i <= nr; i ++ )
                {
                prevla = listla;
                listla = nextla;
                if ( listla == (pm_ptr)NULL )
                    {
                    return( erpush( "PM0114", "" ) );        /* error */
                    }
                pmglin( listla, &nextla, &exprla );
                }

            currlp = (PMLINO *)pmgadr( listla );
            prevlp = (PMLINO *)pmgadr( prevla );
            prevlp->p_nex_li = newlnla; 
            newlnp->p_nex_li = currlp->p_nex_li;
            if ( nextla == (pm_ptr)NULL )
                {  

/*
***last parameter in list; TCONC node will be changed
*/
                tconcp->p_tail = newlnla;  
                }
            }
         }
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmchnp(
        V2REFVA *ref,
        pmmdid   npid,
        pm_ptr   new_expr,
        pm_ptr  *retla)

/*      Change named parameter.
 *      If statement not is found an NULL pointer will be returned in retla.
 *
 *      In:  *ref      =>  Reference to geometric statement
 *            npid     =>  Named parameter id
 *            new_expr =>  Expression for new actual value
 *
 *      Out: *retla    =>  PM-pointer to statement changed, NULL if statement
 *                         not found
 *
 *      FV:    return  -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;     /* PM-pointer to list node */ 
   pm_ptr nextla;     /* PM-pointer to next list node */

   PMMONO *np;        /* c-pointer to module node */
   pm_ptr stlla;      /* statement list PM-pointer */
   pm_ptr nplla;      /* named parameter list PM-pointer */
   pm_ptr nodli;      /* list node pointer */
   pm_ptr napala;     /* PM-pointer to named parameter node */
   PMNPNO *npp;       /* c-pointer to named parameter node */ 
   PMSTNO *stp;       /* c-pointer to statement node */

   bool found = FALSE;

   *retla = (pm_ptr)NULL;
/*
***Get PM-pointer to module statement list
*/
   np = (PMMONO *)pmgadr((pm_ptr)0 );
   stlla = np->pstl_;
/*
***locate geometric statement
*/
   pmlges( ref, &stlla, &listla );

   if ( listla != (pm_ptr)NULL )
      {
/*
***get statement node and set return value for statement pointer
*/
      pmglin( listla, &nextla, retla );
      pmgsta( *retla, &stp );

      if ( stp->suclst == PART_ST )
         nplla = stp->stsubc.partst.partacna;
      else 
         nplla = stp->stsubc.geo_st.geacna;      /* Geo-stat */

      if ( nplla == (pm_ptr)NULL )
         {  
/*
***empty list, create np-node and tconc-node
*/
         pmcnpa( npid, new_expr, &napala );
         pmtcon( napala, (pm_ptr)NULL, &nplla, &nodli );

         if ( stp->suclst == PART_ST )
            stp->stsubc.partst.partacna = nplla; 
         else 
            stp->stsubc.geo_st.geacna = nplla;      /* Geo-stat */
         }
      else
        {
/*
***search for named parameter in list and set an new value,
***if it not is in the list link a new named parameter to the end.
***get first list node in list
*/
         pmgfli( nplla, &listla );

         while( ( listla != (pm_ptr)NULL) && ( ! found ) )
            {
            pmglin( listla, &nextla, &napala );
            pmgnpa( napala, &npp ); 
            found = ( npp->par_np == npid ); 
            listla = nextla;
            }

         if ( found )
            {
            npp->p_val_np = new_expr; 
            }
         else
            {
/*
***create an new np-nodr and link at end of list
*/
             pmcnpa( npid, new_expr, &napala );
             pmtcon( napala, nplla, &nplla, &nodli );
             }
        }
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmlges(
        V2REFVA *ref,
        pm_ptr  *stllap,
        pm_ptr  *retla)

/*      Locate geometric & part statement.
 *
 *      In:  *ref      =>  Reference to geometric statement
 *           *stllap   =>  PM-pointer to statement list (tconc-node)
 *
 *      Out: *stllap   =>  PM-pointer to statement list (tconc-node)
 *           *retla    =>  PM-pointer to list (statement) node
 *
 *      FV:    return  -   error severity code 
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
   pm_ptr colist;     /*   - " -       condition list */
   pm_ptr colila;     /*   - " -       condition list node */
   pm_ptr conxla;     /*   - " -       next condition list node */
   pm_ptr condla;     /*   - " -       condition */
   bool   found;      /* Flagga, TRUE när vi hittat satsen */
   PMSTNO *stp;       /* c-pointer to statement node */
   PMCONO *condp;     /* c-pointer to condition node */
/*
***Initiering.
*/
   *retla = (pm_ptr)NULL;
   found = FALSE;
/*
***Om stllap = NULL finns inga satser.
*/
   if ( *stllap == (pm_ptr)NULL ) return( 0 );
/*
***Hämta första list-noden i sats-listan. stllap är en pekare
***till sats-listans TCONC-nod. pmgfli() returnerar en pekare
***till första "riktiga" listnoden i listan.
*/
   pmgfli( *stllap, &listla );
/*
***Här börjar loopen för själva sats-listan. Hämta nästa list-nod
***och motsvarande sats. Kolla att det är en sats och vilken typ.
***pmglin() returnerar denna listnods pekare till nästa listnod
***samt pekare till sats. pmgsta() returnerar en C-pekare till
***satsen, stp.
*/
   while ( ( listla != (pm_ptr)NULL ) && ( ! found ) )
     { 
     pmglin( listla, &nextla, &statla );
/*
***Det kan vara en tom sats. Då finns inget statement att hämta
***med pmgsta.
*/
     if ( statla != (pm_ptr)NULL )
       {
/*
***Hämta satsen.
*/
       pmgsta( statla, &stp );
       if ( stp->noclst != STAT ) return( erpush( "PM0084", "" ) );

       switch( stp->suclst )
         {
/*
***If-sats. Här får vi anropa oss själva rekursivt.
***Leta först i Condition-listan (if-grenen) och sedan om det
***behövs även i sats-listan (else-grenen).
*/
         case IF_ST:
         colist = stp->stsubc.if_st.ifcond;
         if ( colist != 0 )
           {
           pmgfli(colist,&colila);
           while ( (colila != (pm_ptr)NULL) && (!found) )
             {
             pmglin(colila,&conxla,&condla);
             pmgcon(condla,&condp );
             *stllap = condp->p_stl_co;
             pmlges(ref,stllap,retla);
             if ( *retla != (pm_ptr)NULL ) found = TRUE;
             colila = conxla;
             }
           }
/*
***else-grenen.
*/
         if ( !found )
           {
           *stllap = stp->stsubc.if_st.ifstat;
           pmlges(ref,stllap,retla);
           if ( *retla != (pm_ptr)NULL ) found = TRUE;
           }
         break;
/*
***For-sats.
*/
         case FOR_ST:
         *stllap = stp->stsubc.for_st.fordo;
         pmlges( ref, stllap, retla );
         if ( *retla != (pm_ptr)NULL ) found = TRUE;
         break;
/*
***Part och geometri-procedur.
*/
         case PART_ST:
         case GEO_ST:
         if ( stp->stsubc.geo_st.geident == abs(ref->seq_val) )
           {
           found = TRUE;
           *retla = listla;
           }
         break;
         }
       }
/*
***Nästa sats.
*/
      listla = nextla;
      }
/*
***Slut.
*/
   return( 0 );
}

/********************************************************/
/*!******************************************************/

        short pmrgps(
        pm_ptr  lstla,
        pm_ptr  newsla)

/*      Replace geometric & part statement.
 *
 *      In:  lstla    =>  PM_pointer to list - node
 *           newsla   =>  PM-pointer to new statement
 *
 *      Out: 
 *
 *      FV:    return  -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMLINO *np;
/*
***Fixa en C-pekare till list-noden.
*/
   np = (PMLINO *)pmgadr(lstla);
   if ( np->noclli != LIST ) return(erpush("PM0064",""));
/*
***Uppdatera sats-pekaren.
*/
   np->p_no_li = newsla;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short pmgpla(
        V2REFVA *id,
        short   *pant,
        pm_ptr   pla[])

/*      Return la for all parameters.
 *
 *      In:  *id       =>  Reference to geometric statement
 *           
 *      Out: *pant     =>  Number of parameters
 *            pla[]    =>  Pointers to expressions
 *
 *      FV:    return  -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr  listla,nextla,exprla,retla,stlla,exlla = (pm_ptr)NULL ;
   PMSTNO *stp;
   PMMONO *np;

/*
***Initiering.
*/
   *pant = 0;
/*
***Lokalisera satsen.
*/
   np = (PMMONO *)pmgadr((pm_ptr)0 );
   stlla = np->pstl_;
   pmlges(id,&stlla,&listla);
/*
***Lokalisera parameterlistan.
*/
   pmglin(listla,&nextla,&retla );
   pmgsta(retla,&stp);
   if ( stp->suclst == PART_ST ) exlla = stp->stsubc.partst.modpara;
   if ( stp->suclst == GEO_ST )  exlla = stp->stsubc.geo_st.geacva;
/*
***Gå igenom listan och notera LA till alla uttryck.
*/
   if ( exlla == (pm_ptr)NULL ) return(0);

   pmgfli(exlla,&listla);

   while ( listla != (pm_ptr)NULL )
     {
     pmglin(listla,&nextla,&exprla);
     pla[ *pant ] = exprla;
    *pant = *pant + 1;
     listla = nextla;
     }

   return(0);
  }  

/********************************************************/
