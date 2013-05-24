/**********************************************************************
*
*    evgprocs.c
*    ==========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*    evblk();      Evaluerar BLANK
*    evublk();     Evaluerar UNBLANK
*    evtrim();     Evaluerar TRIM
*    evails();     Evaluerar ALL_INT_LIN_SUR
*    evailm();     Evaluerar ALL_INT_LIN_MSUR
*    evaios();     Evaluerar ALL_INV_ON_SUR
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
#include "../../DB/include/DBintern.h"
#include "../../IG/include/IG.h"
#include "../../EX/include/EX.h"

extern short    proc_pc; /* inproc.c parcount Number of actual parameters */
extern PMPARVA *proc_pv;  /* inproc.c *pv      Access structure for MBS routines */

/*!******************************************************/

        short evblk()

/*      Evaluerar proceduren BLANK.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    return( EXblk( &proc_pv[1].par_va.lit.ref_va[0] ) );

  }

/********************************************************/
/*!******************************************************/

        short evublk()

/*      Evaluerar proceduren UBLANK.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 18/3/86 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    return( EXublk( &proc_pv[1].par_va.lit.ref_va[0] ) );

  }

/********************************************************/
/*!******************************************************/

        short evtrim()

/*      Evaluerar proceduren TRIM.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
  
    return( EXtrim( &proc_pv[1].par_va.lit.ref_va[0],
                    &proc_pv[3].par_va.lit.ref_va[0],
              (short)proc_pv[2].par_va.lit.int_va,
              (short)proc_pv[4].par_va.lit.int_va ) );

  }

/********************************************************/
/*!******************************************************/

        short evails()

/*      Evaluerar proceduren ALL_INT_LIN_SUR.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *          extern proc_pc => Antal parametrar.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 13/9/95 J. Kjellander
 *
 *      2001-02-21 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   short   status;
   DBint   nint;
   DBVector   uv[25],xyz[25];
   PMLITVA litval;

/*
***Exekvera.
*/
   status = EXails(        &proc_pv[1].par_va.lit.ref_va[0],
               (DBVector *)&proc_pv[2].par_va.lit.vec_va,
               (DBVector *)&proc_pv[3].par_va.lit.vec_va,
                            proc_pv[4].par_va.lit.int_va,
               (DBVector *)&proc_pv[5].par_va.lit.vec_va,
                           &nint,uv,xyz);

   if ( status < 0 ) return(status);
/*
***Skriv in resultat i motsvarande MBS-variabler.
*/
   litval.lit.int_va = nint;
   inwvar(proc_pv[6].par_ty,proc_pv[6].par_va.lit.adr_va,0,NULL,&litval);

   if ( proc_pc > 6 )
     {
     if ( (status=evwvec(uv,(short)nint,(short)7,proc_pv)) < 0 ) return(status);
     if ( proc_pc > 7 )
       {
       if ( (status=evwvec(xyz,(short)nint,(short)8,proc_pv)) < 0 ) return(status);
       }
     }

   return(0);
}

/********************************************************/
/*!******************************************************/

        short evailm()

/*      Evaluerar proceduren ALL_INT_LIN_MSUR.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *          extern proc_pc => Antal parametrar.
 *
 *      Felkoder: IN5442 = För få ytor
 *                IN5452 = Feldimensionerad in-array
 *                IN5462 = Can't malloc
 *                IN5472 = Feldimensionerad ut-array
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 1998-04-24 J. Kjellander
 *
 *      2001-02-21 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   short    status;
   char     errbuf[V3STRLEN];
   DBint    nint,nref;
   DBint    i,dekl_dim,refsiz,*snumpt;
   DBint    refadr;
   DBVector uv[25],xyz[25];
   PMLITVA *rlitpt,litval;
   STTYTBL  typtbl;
   STARRTY  reftyp;
/*
***Antal ytor.
*/
   nref = proc_pv[2].par_va.lit.int_va;
/*
***Kolla att antalet ytor är rimligt.
*/
   if ( nref < 1 ) return(erpush("IN5442",""));
/*
***Kolla att deklarerad dimension på refarr är tillräcklig.
***Att det är rätt typ kollas av mbsc.
*/
   refadr = proc_pv[1].par_va.lit.adr_va;
   strtyp(proc_pv[1].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&reftyp);
   dekl_dim = reftyp.up_arr - reftyp.low_arr + 1;
   if ( dekl_dim < nref ) return(erpush("IN5452",""));

   strtyp(reftyp.base_arr,&typtbl);
   refsiz = typtbl.size_ty;
/*
***Allokera minne för refererade ytor.
*/
   if ( (rlitpt=(PMLITVA *)v3mall(nref*sizeof(PMLITVA),"evailm")) == NULL )
     {
     sprintf(errbuf,"%d",(int)nref);
     return(erpush("IN5462",errbuf));
     }
/*
***Kopiera från RTS till allokerad area.
*/
   for ( i=0; i<nref; ++i )
     ingval(refadr+i*refsiz,reftyp.base_arr,FALSE,rlitpt+i);
/*
***Exekvera. EXailm() allokerar minne för den array
***av PMREFVA som innehåller Yt-ID för träffarna.
*/
   status = EXailm( rlitpt,
                            proc_pv[2].par_va.lit.int_va,
               (DBVector *)&proc_pv[3].par_va.lit.vec_va,
               (DBVector *)&proc_pv[4].par_va.lit.vec_va,
                            proc_pv[5].par_va.lit.int_va,
                           &nint,uv,xyz,&snumpt);

   if ( status < 0 )
     {
     v3free(rlitpt,"evailm");
     return(status);
     }
/*
***Skriv in resultat i motsvarande MBS-variabler. Först
***antal lösningar.
*/
   litval.lit.int_va = nint;
   inwvar(proc_pv[6].par_ty,proc_pv[6].par_va.lit.adr_va,0,NULL,&litval);
/*
***Sen UV-värden.
*/
   if ( proc_pc > 6 )
     {
     refadr = proc_pv[7].par_va.lit.adr_va;
     strtyp(proc_pv[7].par_ty,&typtbl);
     strarr(typtbl.arr_ty,&reftyp);
     dekl_dim = reftyp.up_arr - reftyp.low_arr + 1;
     if ( dekl_dim < nint ) return(erpush("IN5452",""));
     if ( (status=evwvec(uv,(short)nint,(short)7,proc_pv)) < 0 ) return(status);
/*
***Sen R3-värden.
*/
     if ( proc_pc > 7 )
       {
       refadr = proc_pv[8].par_va.lit.adr_va;
       strtyp(proc_pv[8].par_ty,&typtbl);
       strarr(typtbl.arr_ty,&reftyp);
       dekl_dim = reftyp.up_arr - reftyp.low_arr + 1;
       if ( dekl_dim < nint ) return(erpush("IN5452",""));
       if ( (status=evwvec(xyz,(short)nint,(short)8,proc_pv)) < 0 ) return(status);
/*
***Slutligen Yt-ID:n.
*/
       if ( proc_pc > 8 )
         {
         refadr = proc_pv[9].par_va.lit.adr_va;
         strtyp(proc_pv[9].par_ty,&typtbl);
         strarr(typtbl.arr_ty,&reftyp);
         dekl_dim = reftyp.up_arr - reftyp.low_arr + 1;
         if ( dekl_dim < nint ) return(erpush("IN5452",""));
         else for ( i=0; i<nint; ++i )
                evwrve(rlitpt[*(snumpt+i)].lit.ref_va,(short)i,(short)9,proc_pv);
         }
       }
     }
/*
***Slut.
*/
   v3free(rlitpt,"evailm");
   if ( nint > 0 ) v3free(snumpt,"evailm");

   return(0);
}

/********************************************************/
/*!******************************************************/

        short evaios()

/*      Evaluerar proceduren ALL_INV_ON_SUR.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *          extern proc_pc => Antal parametrar.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 13/9/95 J. Kjellander
 *
 *      2001-02-21 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   short   status;
   DBint   nint;
   DBVector   uv[25],xyz[25];
   PMLITVA litval;

/*
***Exekvera.
*/
   status = EXaios(         &proc_pv[1].par_va.lit.ref_va[0],
                (DBVector *)&proc_pv[2].par_va.lit.vec_va,
                             proc_pv[3].par_va.lit.int_va,
                            &nint,uv,xyz);

   if ( status < 0 ) return(status);
/*
***Skriv in resultat i motsvarande MBS-variabler.
*/
   litval.lit.int_va = nint;
   inwvar(proc_pv[4].par_ty,proc_pv[4].par_va.lit.adr_va,0,NULL,&litval);

   if ( proc_pc > 4 )
     {
     if ( (status=evwvec(uv,(short)nint,(short)5,proc_pv)) < 0 ) return(status);
     if ( proc_pc > 5 )
       {
       if ( (status=evwvec(xyz,(short)nint,(short)6,proc_pv)) < 0 ) return(status);
       }
     }

   return(0);
}

/********************************************************/

