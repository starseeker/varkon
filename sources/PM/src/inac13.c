/**********************************************************************
*
*    inac13.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*    short inevnl();     evaluate  named parameter list, SET m.fl.
*    short insetb();     interpret SET_BASIC
*    short inpunp();     push default named parameters and put new value
*    short inponp();     pop default      - " -        to default area
*    short incdnp();     uppdaterar attribut (namgivna parametrar)
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


V2NAPA  defnap;                      /* Gloabal area for named parameter values */

static V2NAPA  npstack[ PARTNIMX ];  /* stack for module named parameter environment */
static int npstp = 0;                /* stack pointer for NP-stack */

/*!******************************************************/
/*!******************************************************/

        short inevnl(
        pm_ptr nplist,
        V2NAPA *npblockp) 

/*      Evaluate named parameter list.
 *
 *      In:   nplist    =>    PM-pointer to named parameter list
 *            *npblockp =>    C-pointer to a name parameter block
 *
 *      Out: 
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;          /* PM-pointer to list node */
   pm_ptr nextla;          /* PM-pointer to next list node */
   pm_ptr napala;          /* PM-pointer to named param node */
   short status;

/* 
***copy "defnap" ( default name param block ) 
*/
   V3MOME( (char *)&defnap, (char *)npblockp, sizeof( V2NAPA ) );

   if ( nplist == (pm_ptr)NULL )
     {
     return( 0 );
     }
/* 
***Get first list node in lista 
*/
   if ( ( status = pmgfli( nplist, &listla ) ) < -1 )
     return( status );

   while ( listla != (pm_ptr)NULL )
      {
/* 
***Get contents of list node
*/
      if ( ( status = pmglin( listla, &nextla, &napala ) ) < -1 )
         return( status );

/*
***Evaluate named parameter
*/
      if ( ( status = inevnp( napala, npblockp ) ) < -1 )
         return( status );

      listla = nextla;
      }

   return( 0 );
  }

/*!******************************************************/
/*!******************************************************/

        short insetb(
        pm_ptr nplist)

/*      Interpret SET_BASIC.
 *
 *      In:   nplist  =>  PM-pointer to named parameter list
 *
 *      Out: 
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr listla;            /* PM-pointer to list node */
   pm_ptr nextla;            /* PM-pointer to next list node */
   pm_ptr napala;            /* PM-pointer to named param node */
   short  status;
   int    i;
   V2NAPA tmpblk;

   if ( nplist == (pm_ptr)NULL ) return(0);

   for ( i=0; i<npstp; ++i )
     {
/* 
***Copy stacked block to temp. block 
*/
     V3MOME((char *)&npstack[i+1],(char *)&tmpblk,sizeof(V2NAPA));
/*
***Evaluate name-parameter list and store in tmpblk 
*/
     if ( (status=pmgfli(nplist,&listla)) < -1 ) return(status);

     while ( listla != (pm_ptr)NULL )
        {
/*
***Get contents of list node
*/
        if ( (status=pmglin(listla,&nextla,&napala)) < -1 ) return(status);
/*
***Evaluate named parameter
*/
        if ( (status=inevnp(napala,&tmpblk)) < -1 ) return(status);
        listla = nextla;
        }
/*
***Copy changed block back to stack
*/
     V3MOME((char *)&tmpblk,(char *)&npstack[i+1],sizeof(V2NAPA));
     }
/*
***Copy defnap to temp. block
*/
V3MOME((char *)&defnap,(char *)&tmpblk,sizeof(V2NAPA));
/*
***Evaluate name-parameter list and store in tmpblk
*/
   if ( (status=pmgfli(nplist,&listla)) < -1 ) return(status);

   while ( listla != (pm_ptr)NULL )
      {
/*
***Get contents of list node
*/
      if ( (status=pmglin(listla,&nextla,&napala)) < -1 ) return(status);
/*
***Evaluate named parameter*/
      if ( (status=inevnp(napala,&tmpblk)) < -1 ) return(status);
      listla = nextla;
      }
/*
***Copy changed block back to defnap
*/
   V3MOME((char *)&tmpblk,(char *)&defnap,sizeof(V2NAPA));

   return( 0 );
  }

/*!******************************************************/
/*!******************************************************/

        short inpunp(
        V2NAPA *newdefnp)

/*      Push default named parameter block and put new valus in "defnap" .
 *
 *      In:  *newdefnp  =>  New default values for named parameters
 *
 *      Out: 
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   npstp ++;
   if ( npstp > PARTNIMX - 1 )
/*
***NP stack overflow
*/
     {
     return( erpush( "IN2042", "" ) );
     }
/*
***copy "defnap" to NP stack
*/
   V3MOME( (char *)&defnap, (char *)&npstack[ npstp ], sizeof( V2NAPA ) );
/*
***copy new values to "defnap"
*/
   V3MOME( (char *)newdefnp, (char *)&defnap, sizeof( V2NAPA ) );

   return( 0 );
  }

/*!******************************************************/
/*!******************************************************/

        short inponp()

/*      Pop default named parameter block and put in "defnap" .
 *
 *      In:  
 *
 *      Out: 
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   if ( npstp < 0 )
/*
***NP stack underflow
*/
     {
     return( erpush( "IN2052", "" ) );
     }
/*
***copy from NP stack to "defnap"
*/
   V3MOME( (char *)&npstack[ npstp ], (char *)&defnap, sizeof( V2NAPA ) );

   npstp --;

   return( 0 );
  }

/*!******************************************************/
/*!*******************************************************/

        short incdnp(
        V2NAPA *nppek,
        short   kod)

/*      Updaterar attribut. Om anropet kommer från aktiv modul
 *      uppdatera direkt i defnap, annars uppdatera defnap på
 *      stacken enligt angiven kod.
 *
 *      IN :  nppek = Pekare till en V2NAPA med attribut.
 *            kod   = Typ av jobbdata.
 *
 *      UT :  Inget.
 *
 *      (C) microform ab 27/10/88 R. Svedin
 *
 *      1999-03-26 New atts, J.Kjellander
 *
 ********************************************************!*/

  {
/*
***Aktiv modul vill uppdatera.
*/
   if ( npstp == 0 )
     {
/*
****Hela jobbfilen.
*/
     if( kod == 0 ) V3MOME(nppek,&defnap,sizeof(V2NAPA));
/*
****Endast nivå.
*/
     else if( kod == 2 )
        {
        defnap.level = nppek->level;
        }
/*
****Övriga attribut.
*/
     else
        {
        defnap.pen     = nppek->pen;
        defnap.blank   = nppek->blank;
        defnap.lfont   = nppek->lfont;
        defnap.afont   = nppek->afont;
        defnap.ldashl  = nppek->ldashl;
        defnap.adashl  = nppek->adashl;
        defnap.tsize   = nppek->tsize;
        defnap.twidth  = nppek->twidth;
        defnap.tslant  = nppek->tslant;
        defnap.dtsize  = nppek->dtsize;
        defnap.dasize  = nppek->dasize;
        defnap.dndig   = nppek->dndig;
        defnap.dauto   = nppek->dauto;
        defnap.xfont   = nppek->xfont;
        defnap.xdashl  = nppek->xdashl;
        defnap.hit     = nppek->hit;
        defnap.save    = nppek->save;
        defnap.cfont   = nppek->cfont;
        defnap.cdashl  = nppek->cdashl;
        defnap.tfont   = nppek->tfont;
        defnap.sfont   = nppek->sfont;
        defnap.sdashl  = nppek->sdashl;
        defnap.nulines = nppek->nulines;
        defnap.nvlines = nppek->nvlines;
        defnap.width   = nppek->width;
        defnap.tpmode  = nppek->tpmode;
        }
     }
/*
***En part vill uppdatera.
*/
   else
      {
/*
****Hela jobbfilen.
*/
      if( kod == 0 ) V3MOME(nppek,&npstack[npstp],sizeof(V2NAPA));
/*
****Endast nivå.
*/
      else if( kod == 2 )
         {
         npstack[ npstp ].level = nppek->level;
         }
/*
****Övriga attribut.
*/
      else
         {
         npstack[ npstp ].pen     = nppek->pen;
         npstack[ npstp ].blank   = nppek->blank;
         npstack[ npstp ].lfont   = nppek->lfont;
         npstack[ npstp ].afont   = nppek->afont;
         npstack[ npstp ].ldashl  = nppek->ldashl;
         npstack[ npstp ].adashl  = nppek->adashl;
         npstack[ npstp ].tsize   = nppek->tsize;
         npstack[ npstp ].twidth  = nppek->twidth;
         npstack[ npstp ].tslant  = nppek->tslant;
         npstack[ npstp ].dtsize  = nppek->dtsize;
         npstack[ npstp ].dasize  = nppek->dasize;
         npstack[ npstp ].dndig   = nppek->dndig;
         npstack[ npstp ].dauto   = nppek->dauto;
         npstack[ npstp ].xfont   = nppek->xfont;
         npstack[ npstp ].xdashl  = nppek->xdashl;
         npstack[ npstp ].hit     = nppek->hit;
         npstack[ npstp ].save    = nppek->save;
         npstack[ npstp ].cfont   = nppek->cfont;
         npstack[ npstp ].cdashl  = nppek->cdashl;
         npstack[ npstp ].tfont   = nppek->tfont;
         npstack[ npstp ].sfont   = nppek->sfont;
         npstack[ npstp ].sdashl  = nppek->sdashl;
         npstack[ npstp ].nulines = nppek->nulines;
         npstack[ npstp ].nvlines = nppek->nvlines;
         npstack[ npstp ].width   = nppek->width;
         npstack[ npstp ].tpmode  = nppek->tpmode;
         }
      }
   return(0);
  }
/*********************************************************/
