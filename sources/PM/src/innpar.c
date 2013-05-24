/**********************************************************************
*
*    innpar.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    short inrdnp();    reset default named parameter values
*    short inevnp();    evaluate  named parameter
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
#include "../../WP/include/WP.h"

extern V2NAPA  defnap;     /* Gloabal area for named parameter values */

/*
***standard types
*/
extern pm_ptr  stintp;
extern pm_ptr  stflop;
extern pm_ptr  ststrp;
extern pm_ptr  stvecp;
extern pm_ptr  strefp;

/********************************************************/
/*!******************************************************/

        short inrdnp()

/*      Reset default named parameter values.
 *
 *      (C)microform ab 1986-03-14 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *      2004-07-11 Mesh, J.Kjellander, Örebro university
 *      2007-01-17 NULINES=NVLINES=1, J.Kjellander
 *
 ******************************************************!*/

  {
/*
***Gloabal area for named parameter values
*/
   defnap.pen     = 1;         /* pen */
   defnap.level   = 0;         /* level */
   defnap.blank   = 0;         /* blank */
   defnap.lfont   = 0;         /* lfont */
   defnap.afont   = 0;         /* afont */
   defnap.ldashl  = 3.0;       /* ldashl */
   defnap.adashl  = 3.0;       /* adashl */
   defnap.tsize   = 5.0;       /* tsize */
   defnap.twidth  = 60.0;      /* twidth */
   defnap.tslant  = 15.0;      /* tslant */
   defnap.dtsize  = 5.0;       /* dtsize */
   defnap.dasize  = 5.0;       /* dasize */
   defnap.dndig   = 0;         /* dndig */
   defnap.dauto   = 1;         /* dauto */
   defnap.xfont   = 0;         /* xfont */
   defnap.xdashl  = 3.0;       /* xdashl */
   defnap.hit     = 1;         /* hit */
   defnap.save    = 1;         /* save */
   defnap.cfont   = 0;         /* cfont */
   defnap.cdashl  = 3.0;       /* cdashl */
   defnap.tfont   = 0;         /* tfont */
   defnap.sfont   = 0;         /* sfont */
   defnap.sdashl  = 3.0;       /* sdashl */
   defnap.nulines = 1;         /* nvlines */
   defnap.nvlines = 1;         /* nulines */
   defnap.width   = 0.0;       /* width */
   defnap.tpmode  = 0;         /* tpmode */
   defnap.mfont   = 0;         /* mfont */
   defnap.pfont   = 0;         /* PFONT */
   defnap.psize   = 2;         /* PSIZE */

   return(0);

  }

/********************************************************/
/*!******************************************************/

        short inevnp(
        pm_ptr napala,
        V2NAPA *npblockp)

/*      Evaluate named parameter.
 *
 *      In:     napala     =>  PM-pointer to named parameter 
 *             *npblockp   =>  C-pointer to a name parameter block 
 *
 *      Return - error severity code 
 *
 *      (C)microform ab 1986-03-14 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *      2004-07-11 Mesh, J.Kjellander
 *      2007-03-24 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
   PMNPNO *np;         /* C-pointer to NP node */
   PMLITVA val;        /* Named param value */
   pm_ptr  tyla;       /* Type pointer for value */
   double  fltval;     /* Float value */
   short   status;     /* Return status */

/*
***Get c-pointer to NP node
*/
   if ( ( status = pmgnpa( napala, &np ) ) < -1 )
       return( status );

   if ( np->noclnp != NAMEPA )
       return( erpush( "IN0154", "" ) );           /* Not an NP node */

/*
***Evaluate NP value
*/
   if ( ( status = inevev( np->p_val_np, &val, &tyla ) ) < -1 )
        return( status );

   switch( np->par_np )
     {
/*
***PEN.
*/
     case PMPEN:                                   /* integer */
        if ( ineqty( tyla, stintp ) )
           {
           if ( val.lit.int_va < 0 || val.lit.int_va > WP_NPENS-1 )
              return(erpush("IN2102","PEN"));
           else npblockp->pen = val.lit.int_va;
           }
        else   
           return( erpush( "IN3193", "PEN" ) );    /* Ilegal type for named parameter */
        break;
/*
***WIDTH.
*/
     case PMWIDTH:                                 /* float */
        if ( ineqty( tyla, stflop ) )
           npblockp->width = val.lit.float_va;
        else if ( ineqty( tyla, stintp ) )
           npblockp->width = val.lit.int_va;
        else
           return( erpush( "IN3193", "WIDTH" ) );
        break;
/*
***LEVEL.
*/
     case PMLEVEL:                                 /* integer */
        if ( ineqty( tyla, stintp ) )
           {
           if ( val.lit.int_va < 0 || val.lit.int_va > WP_NIVANT-1 )
              return(erpush("IN2102","LEVEL"));
           else npblockp->level = val.lit.int_va;
           }
         else
           return( erpush( "IN3193", "LEVEL" ) );
         break;
/*
***BLANK.
*/
     case PMBLANK:                                 /* integer */
        if ( ineqty( tyla, stintp ) )
           npblockp->blank = val.lit.int_va;
        else
           return( erpush( "IN3193", "BLANK" ) );
        break;
/*
***HIT.
*/
     case PMHIT:                                   /* integer */
        if ( ineqty( tyla, stintp ) )
           npblockp->hit = val.lit.int_va;
        else
           return( erpush( "IN3193", "HIT" ) );
        break;
/*
***SAVE.
*/
     case PMSAVE:                                  /* integer */
        if ( ineqty( tyla, stintp ) )
           npblockp->save = val.lit.int_va;
        else
           return( erpush( "IN3193", "SAVE" ) );
        break;

     case PMPFONT:                                 /* integer */
        if ( ineqty( tyla, stintp ) )
           npblockp->pfont = val.lit.int_va;
        else
           return( erpush( "IN3193", "PFONT" ) );
        break;

     case PMLFONT:                                 /* integer */
        if ( ineqty( tyla, stintp ) )
           npblockp->lfont = val.lit.int_va;
        else
           return( erpush( "IN3193", "LFONT" ) );
        break;

     case PMAFONT:                                 /* integer */
        if ( ineqty( tyla, stintp ) )
           npblockp->afont = val.lit.int_va;
        else
           return( erpush( "IN3193", "AFONT" ) );
        break;

     case PMCFONT:                                 /* integer */
        if ( ineqty( tyla, stintp ) )
           npblockp->cfont = val.lit.int_va;
        else
           return( erpush( "IN3193", "CFONT" ) );
        break;

     case PMTFONT:                                 /* integer */
        if ( ineqty( tyla, stintp ) )
           npblockp->tfont = val.lit.int_va;
        else
           return( erpush( "IN3193", "TFONT" ) );
        break;

     case PMSFONT:                                 /* integer */
        if ( ineqty( tyla, stintp ) )
           npblockp->sfont = val.lit.int_va;
        else
           return( erpush( "IN3193", "SFONT" ) );
        break;

     case PMNULIN:                                 /* integer */
        if ( ineqty( tyla, stintp ) )
           npblockp->nulines = val.lit.int_va;
        else
           return( erpush( "IN3193", "NULINES" ) );
        break;

     case PMNVLIN:                                 /* integer */
        if ( ineqty( tyla, stintp ) )
           npblockp->nvlines = val.lit.int_va;
        else
           return( erpush( "IN3193", "NVLINES" ) );
        break;
/*
***Point size. Should not be 0 or less.
*/
     case PMPSIZE:
     if      ( ineqty(tyla,stflop) ) fltval = val.lit.float_va;
     else if ( ineqty(tyla,stintp) ) fltval = val.lit.int_va;
     else                            return(erpush("IN3193","PSIZE"));
     if      ( fltval <= 0.0 )       return(erpush("IN2102","PSIZE"));
     else                            npblockp->psize = fltval;
     break;
/*
***Line dash length.
*/
     case PMLDASHL:
        if ( ineqty( tyla, stflop ) )
           npblockp->ldashl = val.lit.float_va;
        else if ( ineqty( tyla, stintp ) )
           npblockp->ldashl = val.lit.int_va;
        else
           return( erpush( "IN3193", "LDASHL" ) );
        break;
/*
***Arc dash length.
*/
     case PMADASHL:                                /* float */
        if ( ineqty( tyla, stflop ) )
           npblockp->adashl = val.lit.float_va;
        else if ( ineqty( tyla, stintp ) )
           npblockp->adashl = val.lit.int_va;
        else
           return( erpush( "IN3193", "ADASHL" ) );
        break;
/*
***Curve dash length.
*/
     case PMCDASHL:                                /* float */
        if ( ineqty( tyla, stflop ) )
           npblockp->cdashl = val.lit.float_va;
        else if ( ineqty( tyla, stintp ) )
           npblockp->cdashl = val.lit.int_va;
        else
           return( erpush( "IN3193", "CDASHL" ) );
        break;

     case PMSDASHL:                                /* float */
        if ( ineqty( tyla, stflop ) )
           npblockp->sdashl = val.lit.float_va;
        else if ( ineqty( tyla, stintp ) )
           npblockp->sdashl = val.lit.int_va;
        else
           return( erpush( "IN3193", "SDASHL" ) );
        break;

     case PMTSIZE:                                 /* float */
        if ( ineqty( tyla, stflop ) )
           npblockp->tsize = val.lit.float_va;
        else if ( ineqty( tyla, stintp ) )
           npblockp->tsize = val.lit.int_va;
        else
           return( erpush( "IN3193", "TSIZE" ) );
       break;

     case PMTWIDTH:                                /* float, faktor i % av höjd */
        if ( ineqty( tyla, stflop ) )
           npblockp->twidth = val.lit.float_va;
        else if ( ineqty( tyla, stintp ) )
           npblockp->twidth = val.lit.int_va;
         else
           return( erpush( "IN3193", "TWIDTH" ) );
       break;

     case PMTSLANT:                                /* float, lutning i % av 45 grader */
        if ( ineqty( tyla, stflop ) )
           npblockp->tslant = val.lit.float_va;
        else if ( ineqty( tyla, stintp ) )
           npblockp->tslant = val.lit.int_va;
        else
           return( erpush( "IN3193", "TSLANT" ) );
       break;

     case PMTPMODE:                                /* integer, Textplanmode */
        if ( ineqty( tyla, stintp ) )
           npblockp->tpmode = val.lit.int_va;
        else
           return( erpush( "IN3193", "TPMODE" ) );
        break;

     case PMDTSIZE:                                /* float, dim text size */
        if ( ineqty( tyla, stflop ) )
           npblockp->dtsize = val.lit.float_va;
        else if ( ineqty( tyla, stintp ) )
           npblockp->dtsize = val.lit.int_va;
        else
           return( erpush( "IN3193","DTSIZE" ) );
        break;

     case PMDASIZE:                                /* float, pilstorlek för måttsättning */
        if ( ineqty( tyla, stflop ) )
           npblockp->dasize = val.lit.float_va;
        else if ( ineqty( tyla, stintp ) )
           npblockp->dasize = val.lit.int_va;
        else
           return( erpush( "IN3193","DASIZE" ) );
        break;

     case PMDNDIG:                                 /* integer, decimaler vid måttsättning */
        if ( ineqty( tyla, stintp ) )
           npblockp->dndig = val.lit.int_va;
        else
           return( erpush( "IN3193", "DNDIG" ) );
        break;

     case PMDAUTO:                                 /* integer, automatisk måttsättnig */
        if ( ineqty( tyla, stintp ) )
           npblockp->dauto = val.lit.int_va;
        else
           return( erpush( "IN3193", "DAUTO" ) );
        break;

     case PMXFONT:                                 /* integer, snittyp */
        if ( ineqty( tyla, stintp ) )
           npblockp->xfont = val.lit.int_va;
        else
           return( erpush( "IN3193", "XFONT" ) );
        break;

     case PMXDASHL:                                /* float, snittlinjers strecklängd */
        if ( ineqty( tyla, stflop ) )
           npblockp->xdashl = val.lit.float_va;
        else if ( ineqty( tyla, stintp ) )
           npblockp->xdashl = val.lit.int_va;
        else
           return( erpush( "IN3193", "XDASHL" ) );
        break;

     case PMMFONT:                                 /* integer */
        if ( ineqty( tyla, stintp ) )
           npblockp->mfont = val.lit.int_va;
        else
           return( erpush( "IN3193", "MFONT" ) );
        break;
/*
***Error
*/
     default:
     return( erpush( "IN0164", "" ) );             /* Unknown named parameter */
     break;
   }
   return( 0 );

  }

/********************************************************/
