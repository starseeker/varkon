/**********************************************************************
*
*    inac13.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes the following routines:
*
*    short inevnl();     evaluate  named parameter list, SET m.fl.
*    short insetb();     interpret SET_BASIC
*    short inpunp();     push default named parameters and put new value
*    short inponp();     pop default      - " -        to default area
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

/********************************************************/
/********************************************************/

        short inpunp(
        V2NAPA *newdefnp)

/*      Push default named parameter block and put new
 *      values in "defnap" .
 *
 *      In: *newdefnp => New default values for named parameters
 *
 *      Return - error severity code
 *
 *      (C)2007-11-26 J.Kjellander
 *
 ********************************************************/

  {
   char errbuf[V3STRLEN];

/*
***Increase stack pointer.
*/
   npstp ++;

   if ( npstp > PARTNIMX - 1 )
/*
***NP stack overflow.
*/
     {
     sprintf(errbuf,"%d",PARTNIMX);
     erpush("IN2032",errbuf);
     return(erpush("IN2042",""));
     }
/*
***Copy "defnap" to NP stack.
*/
   V3MOME((char *)&defnap,(char *)&npstack[npstp],sizeof(V2NAPA));
/*
***Copy new values to "defnap".
*/
   V3MOME((char *)newdefnp,(char *)&defnap,sizeof(V2NAPA));
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/********************************************************/

        short inponp()

/*      Pop default named parameter block and put in "defnap".
 *
 *      Return - error severity code
 *
 *      (C)2007-11-26 J.Kjellander
 *
 ******************************************************!*/

  {

/*
***Check for NP stack underflow.
*/
   if ( npstp < 0 )
     {
     return( erpush( "IN2052", "" ) );
     }
/*
***Copy from NP stack to "defnap".
*/
   V3MOME((char *)&npstack[npstp],(char *)&defnap,sizeof(V2NAPA));
/*
***Decrease stack pointer.
*/
   npstp--;
/*
***The end.
*/
   return( 0 );
  }

/********************************************************/
