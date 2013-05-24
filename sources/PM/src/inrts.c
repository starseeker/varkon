/**********************************************************************
*
*    inrts.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    short inpfrh()    put frame header on RTS
*    short ingfrh()    get frame header from RTS
*    short inpdfr()    put data frame on RTS
*    short inpval()    put value on RTS
*    short ingval()    get value from RTS
*    short ingref()    get REF-value from RTS
*    short inirts()    initiate RTS
*    short incrts()    clear RTS
*    short insrtb()    Set RTS base pointer
*    short insrsp()    Set RTS stack pointer
*    char *ingrtb()    Get RTS base pointer 
*    char *inggrb()    Get global RTS base pointer
*    char *inglrb()    Get local RTS base pointer JK
*    char *ingrsp()    Get RTS stack pointer JK
*    short inslrb()    Set base pointer for local area
*    char *incrsa()    Calculate the c-address from RTS-offset
*
*    Internal routines:
*    short inallo()    allocate a block in RTS
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

extern V3MSIZ sysize;

/*
***INFHEAD  -  RTS frame header
*/
typedef struct infhead
    {
    char *oldsbas;                   /* old RTS base pointer */
    char  pmmnam[JNLGTH+1];          /* old module name in PM (caller module) */
                                     /* Bug, *pmnamÄV2SYNLEN+1Å, 920602 JK */
    }  INFHEAD;

static char *rtstart = NULL;         /* Beginning of RTS, NULL => not allocated */
static char *rtsp;                   /* Run-time stack pointer */
static char *rtsbas;                 /* current RTS base pointer */
static char *rtslbas;                /* RTS base pointer to local area */
static char *rtstl;                  /* RTS top limit */
static char *rtsbl;                  /* RTS bottom limit */

/********************************************************/
/*!******************************************************/

        short inpfrh(
        char *mnamp,
        DBint  datasz)

/*      Put frame header.
 *      Makes a frame header and put it on RTS, then update "rtsbas" to point
 *      at top of stack. The frame header contains the old values for "rtsbas"
 *      and "pmbas". Memory for local variables are also allocated.
 *
 *      In:   *mnamp   =>  Pointer to old module name
 *             datasz  =>  Size of local data area          
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   INFHEAD *fhp;
   char *chp;
   DBint rtsofs;
   short status;

   if ( ( status = inallo( (DBint)sizeof( *fhp ), &chp, &rtsofs ) ) != 0 )
      return( status );

   fhp = (INFHEAD *)chp;

   fhp->oldsbas = rtsbas;
   strcpy(fhp->pmmnam, mnamp);      /* copy module name into header */
/*
***update "rtsbas" to point to new stack top
*/
   if ( ( status = insrtb( rtsp ) ) < -1 )
      return( status );
/*
***allocate a frame for variables
*/
   if ( ( status = inpdfr( datasz ) ) < -1 )
      return( status );

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short ingfrh(
        char *mnamp)

/*      Get frame header.
 *      Reads the current frame header and restore "rtsbas" to the value in the 
 *      frame header.
 *
 *      In:   *mnamp   =>  Pointer to old module name
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   INFHEAD *fhp;
   short    status;
   DBint     size;

/*
***Om aktiv modul är aktiv finns inga fler frames
***att stryka.
*/
   if ( rtsbas <= rtslbas ) return(erpush("IN2022",""));
/*
***Stryk data-delen från rts.
*/
   rtsp = rtsbas;
/*
***Stryk själva frame-headern från rts. Hur stor är den ?.
*/
   size = sizeof(*fhp);
   if ( ( size % MIN_BLKS ) != 0 )
    size = ( ( size + MIN_BLKS ) / MIN_BLKS ) * MIN_BLKS;
   rtsp -= size;
/*
***Namnet på anropande modul.
*/
   fhp = (INFHEAD *)rtsp;
   strcpy(mnamp, fhp->pmmnam);

/*
***Ny RTS base pointer.
*/
   if ( (status=insrtb(fhp->oldsbas) ) < -1 ) return(status);

   return( 0 );

  }

/********************************************************/
/*!******************************************************/

        short inpdfr(
        DBint datasize) 

/*      Put data frame on RTS.
 *
 *      In:   datasize =>  size of data frame
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  { 
   char *chp;
   DBint rtsofs;
   short status;

   if ( ( status = inallo( datasize, &chp, &rtsofs ) ) != 0 )
      return( status );

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short inpval(
        DBint     rtsofs,
        pm_ptr    tyla,
        bool      addrflg,
        PMLITVA  *litvap)

/*      Put value on RTS.
 *
 *      In:    rtsofs   =>  Address offset in RTS 
 *             tyla     =>  Pointer to type def          
 *             addrflg  =>  Flag to indicate that the value is an address           
 *             *litvap  =>  Pointer to literal structure          
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   v2int *intp;
   v2float *floatp;
   V2VECVA *vecp;
   V2REFVA *rtsrefp, *litrefp;
   v2addr *addrp;
   char *schp;                 /* c-pointer to RTS for source under block transfare */
   char *chp;                  /* temp. c-pointer to RTS */
   STTYTBL type;
   char errbuf[20];

/*
***calculate the c-address for the location in RTS
*/
   chp = incrsa( rtsofs );

/*
***check if value is adress and store in RTS
*/
   if ( addrflg )
      {
      addrp = (v2addr *)chp;
      *addrp = litvap->lit.adr_va;
      }

/*
***check type
*/
   else 
      {
/*
***read type info
*/
      strtyp( tyla, &type );

      switch( type.kind_ty )
         {
         case ST_INT:
            intp = (v2int *)chp;
            *intp = litvap->lit.int_va;

            break;

         case ST_FLOAT:
            floatp = (v2float *)chp;
            *floatp = litvap->lit.float_va;

            break;

         case ST_STR:
            *( litvap->lit.str_va + ( type.size_ty - 1 ) ) = '\0';
            strcpy( chp, litvap->lit.str_va );

           break;

         case ST_VEC:
            vecp = (V2VECVA *)chp;

            vecp->x_val = litvap->lit.vec_va.x_val;
            vecp->y_val = litvap->lit.vec_va.y_val;
            vecp->z_val = litvap->lit.vec_va.z_val;

            break;

         case ST_REF:
/*
***copy data from pmlitva into RTS
*/
            rtsrefp = (V2REFVA *)chp;
            litrefp = litvap->lit.ref_va;
          
            rtsrefp->seq_val = litrefp->seq_val;
            rtsrefp->ord_val = litrefp->ord_val;

            while ( litrefp->p_nextre != NULL )
               {
/*
***increment ref-pointers
*/
               rtsrefp->p_nextre = ( rtsrefp + 1 );
               rtsrefp ++;
               litrefp ++;

               rtsrefp->seq_val = litrefp->seq_val;
               rtsrefp->ord_val = litrefp->ord_val;
               }

            rtsrefp->p_nextre = NULL;

            break;

         case ST_FILE:

            break;

         case ST_ARR:
/*
***copy the data from address in "litvap"
***calculate c-address for the source and check status
*/
            schp = incrsa( litvap->lit.adr_va );
  
/*
***copy
*/
            V3MOME( schp, chp, type.size_ty );

            break;

         default:
            sprintf(errbuf,"Put-%d",(int)type.kind_ty);
            return( erpush( "IN0064",errbuf) );          /* ilegal type id */
            break;

         }
      }

   return( 0 );

  }

/********************************************************/
/*!******************************************************/

        short ingval(
        DBint     rtsofs,
        pm_ptr    tyla,
        bool      addrflg,
        PMLITVA  *litvap)

/*      Get value from RTS.
 *
 *      In:    rtsofs   =>  Address offset in RTS 
 *             tyla     =>  Pointer to type def          
 *             addrflg  =>  Flag to indicate that the value is an address           
 *
 *      Out:  *litvap   =>  Pointer to literal structure          
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   v2int *intp;
   v2float *floatp;
   V2VECVA *vecp;
   V2REFVA *rtsrefp, *litrefp;
   v2addr  *addrp;
   char *chp;                   /* temp. c-pointer to RTS */
   STTYTBL type;
   char errbuf[20];

/*
***calculate the c-address for the location in RTS
*/
   chp = incrsa( rtsofs );

   if ( addrflg )
      {
      addrp = (v2addr *)chp;
      litvap->lit_type = C_ADR_VA;
      litvap->lit.adr_va = *addrp;
      }
   else
      {
/*
***read type info
*/
      strtyp( tyla, &type );

      switch( type.kind_ty )
         {
         case ST_INT:
            intp = (v2int *)chp;
            litvap->lit_type = C_INT_VA;
            litvap->lit.int_va = *intp;

            break;

         case ST_FLOAT:
            floatp = (v2float *)chp;
            litvap->lit_type = C_FLO_VA;
            litvap->lit.float_va = *floatp; 

            break;

         case ST_STR:
            litvap->lit_type = C_STR_VA;
            strcpy( litvap->lit.str_va, chp );

            break;

         case ST_VEC:
            vecp = (V2VECVA *)chp;

            litvap->lit_type = C_VEC_VA;
            litvap->lit.vec_va.x_val = vecp->x_val; 
            litvap->lit.vec_va.y_val = vecp->y_val; 
            litvap->lit.vec_va.z_val = vecp->z_val; 

            break;

         case ST_REF:
/*
***copy data from RTS into pmlitva
*/
            rtsrefp = (V2REFVA *)chp;
            litrefp = litvap->lit.ref_va;
          
            litrefp->seq_val = rtsrefp->seq_val;
            litrefp->ord_val = rtsrefp->ord_val;

            while ( rtsrefp->p_nextre != NULL )
               {
/*
***increment ref-pointers
*/
               litrefp->p_nextre = ( litrefp + 1 );
               litrefp ++;
               rtsrefp ++;

               litrefp->seq_val = rtsrefp->seq_val;
               litrefp->ord_val = rtsrefp->ord_val;
               }

            litrefp->p_nextre = NULL;

            litvap->lit_type = C_REF_VA;

            break;

         case ST_FILE:
            litvap->lit.fil_va = (V2FILE *)chp;
            litvap->lit_type = C_FIL_VA;

            break;

         case ST_ARR:
/*
***cannot get the value from an array, return address
*/
            litvap->lit_type = C_ADR_VA;
            litvap->lit.adr_va = rtsofs;

            break;

         default:
            sprintf(errbuf,"Get-%d",(int)type.kind_ty);
            return( erpush( "IN0063",errbuf) );           /* ilegal type id */
            break;

         }

      }

   return( 0 );

  }

/********************************************************/
/*!******************************************************/

        void      ingref(
        DBint     rtsofs,
        PMLITVA  *litvap)

/*      Get REF value from RTS.
 *
 *      In:    rtsofs   =>  Address offset in RTS 
 *
 *      Out:  *litvap   =>  Pointer to literal structure          
 *
 *
 *      (C)Johan Kjellander 2000-12-01
 *
 ******************************************************!*/

  {
   V2REFVA *rtsrefp,*litrefp;
   char    *chp;

/*
***calculate the c-address for the location in RTS
*/
   chp = incrsa(rtsofs);
/*
***Create pointers of right type.
*/
   rtsrefp = (V2REFVA *)chp;
   litrefp = litvap->lit.ref_va;
/*
***Copy 1:st node in REF-list.
*/
   litrefp->seq_val  = rtsrefp->seq_val;
   litrefp->ord_val  = rtsrefp->ord_val;
   litrefp->p_nextre = rtsrefp->p_nextre;

   litvap->lit_type = C_REF_VA;

   return;
  }

/********************************************************/
/*!******************************************************/

        short inirts( )

/*      Init RTS.
 *
 *      In:    
 *
 *      Out:  
 *
 *      FV:  
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char errbuf[80];

/*
***Om minne för RTS ännu inte allokerats gör vi det nu.
*/
   if ( rtstart == NULL )
     {
     if ( (rtstart=(char *)v3mall(sysize.rts,"RTS")) == NULL )
       {
       sprintf(errbuf,"%d",sysize.rts);
       return(erpush("IN5072",errbuf));
       }
     }
/*
***Initiera stack ock baspekare.
*/
   rtsp = rtsbas = rtslbas = rtstart;
/*
***Initiera top och bottom limit.
*/
   rtstl = rtstart + sysize.rts - 1;
   rtsbl = rtstart;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short incrts( )

/*      Clear RTS.
 *
 *      In:    
 *
 *      Out:  
 *
 *      FV:  
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
/*
***Initiera stack ock baspekare.
*/
   rtsp = rtsbas = rtslbas;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short inallo(
        DBint size,
        char **chp,
        DBint *rtsofs)

/*      Allocate a block in RTS.
 *
 *      In:     size    =>  Block size to be allocated ,long JK 
 *             
 *      Out:  **chp     =>  C-pointer to start of allocated block 
 *              rtsofs  =>  Offset address in RTS relative "rtsbas" 
 *
 *      FV:   return - error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char *newrtsp;
   char  errbuf[80];

/*
***Kolla alignment. Om size modulus MIN_BLKS > 0 är inte size
***jämt delbart med (en jämn multipel av) MIN_BLKS. Då får vi
***justera size lite.
*/
   if ( ( size % MIN_BLKS ) != 0 )
     {
     size = ( ( size + MIN_BLKS ) / MIN_BLKS ) * MIN_BLKS;
     }

   newrtsp = rtsp + size;
/*
***Finns det plats ?
*/
   if ( newrtsp > rtstl )
     {
     sprintf(errbuf,"%d",newrtsp-rtsbl);
     erpush("IN3142",errbuf);
     sprintf(errbuf,"%d",rtstl-rtsbl+1);
     return(erpush("IN3033",errbuf));
     }
/*
***Jadå, allokera.
*/
   *chp = rtsp;
   *rtsofs = rtsp - rtsbas;
/*
***Ny stackpekare.
*/
   rtsp = newrtsp;


   return( 0 );

  }

/********************************************************/
/*!******************************************************/

        char *incrsa(
        DBint rtsofs)


/*      Calculate the c-address from offset.
 *
 *      In:    *rtsofs    =>  Offset address in RTS relative "rtsbas 
 *             
 *      Out:  
 *
 *      FV:     return - c-address in RTS, NULL is error 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char *adr;

/*
char errbufÄ80Å;
*/
   adr = rtsbas + rtsofs;

/*
***check if legal RTS address
*/
   if ( ( adr < rtsbl ) || ( adr > rtstl ) )
      { 

/*
    sprintf(errbuf,"RTS-offset=%d",rtsofs);
    erpush("IN0053",errbuf );
*/
      erpush("IN0054","");
      }

   return( adr );
  }

/********************************************************/
/*!******************************************************/

        short insrtb(
        char    *rbasp)

/*      Set RTS base pointer.
 *
 *      In:    *rbasp  =>  New Run-Time stack base pointer 
 *             
 *      Out:  
 *
 *      FV:     return - error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char errbuf[80];

   if ( rbasp < rtsbl || rbasp > rtstl )
      { /* Error */
      sprintf(errbuf,"%d",(int)(rbasp-rtslbas));
      return(erpush("IN3073",errbuf) );         /* Ilegal RTS base pointer */
      } 

   rtsbas = rbasp;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short insrsp(
        char    *rsp)

/*      Set RTS stack pointer.
 *
 *      In:    *rsp   =>  New Run-Time stack pointer 
 *             
 *      Out:  
 *
 *      FV:     return - error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  { 
   rtsp = rsp; 
   return(0);
  }

/********************************************************/
/*!******************************************************/

        char *ingrtb()

/*      Get RTS base pointer.
 *
 *      FV:  return - RTS base pointer 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  { 
   return(rtsbas); 
  }

/********************************************************/
/*!******************************************************/

        char *inggrb()

/*      Get global RTS base pointer.
 *
 *      FV:  return - global RTS base pointer 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  { 
   return(rtstart);
  }

/********************************************************/
/*!******************************************************/

        char *inglrb()

/*      Get local RTS base pointer.
 *
 *      FV:  return - local RTS base pointer 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
    return(rtslbas);
   }

/********************************************************/
/*!******************************************************/

        char *ingrsp()

/*      Get RTS stack pointer.
 *
 *      FV:  return - RTS stack pointer 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
    return(rtsp); 
  }

/********************************************************/
/*!******************************************************/

        short inslrb()

/*      Set RTS base for local area.
 *
 *      FV:  return - error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-14 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   rtslbas = rtsp;

   insrtb( rtsp );     /* set current base pointer */

   return( 0 );
  }

/********************************************************/
