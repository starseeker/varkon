/**********************************************************************
*
*    eval1.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*    evwval();      Skriver skalära värden
*    evwseg();      Skriver kurv-segment (4X4 mat)
*    evwfvk();      Skriver flyttalsvektor
*    evwvec();      Skriver array med DBVector
*    evwrve();      Skriver element i referensvektor
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

extern PMPARVA *proc_pv;  /* inproc.c *pv      Access structure for MBS routines */

extern PMPARVA *func_pv;   /* Access structure for MBS routines */
extern PMLITVA *func_vp;   /* Pekare till resultat. */

/*!******************************************************/

        short evwval(
        PMLITVA  litval[],
        DBint    nval,
        PMPARVA *pv)

/*      Skriver ett antal skalära värden till MBS-variabler.
 *
 *      In: litval => Array med värden.
 *          nval   => Antal värden.
 *          pv     => Pekare till array med parameteinfo.
 *
 *      Ut: Inget.
 *
 *      FV: Inget, dvs 0.
 *
 *      (C)microform ab 21/3/86 J. Kjellander
 *
 ******************************************************!*/

  {
    DBint i;

    for ( i=0; i<nval; ++i )
      {
      inwvar(pv[i+2].par_ty, pv[i+2].par_va.lit.adr_va, 
             0, NULL, &litval[i]);
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evwseg(
        DBint    nseg,
        DBSeg    seg[],
        DBint    pn,
        PMPARVA *pv)

/*      Skriver segment-data till MBS-4X4 array.
 *
 *      In: nseg => Antal segment.
 *          seg  => Pekare till segment
 *          pn   => Parameterns ordningsnummer
 *          pv   => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar in-rutinens status.
 *
 *      (C)microform ab 18/3/86 J. Kjellander
 *
 *      20/5/86 Initiering av status till 0 R. Svedin
 *
 ******************************************************!*/

  {
     short   status;
     DBint   i,index[3];
     PMLITVA litval;

/*
***Loop för nseg segment.
*/

     status = 0;

     for ( i=0; i<nseg; ++i )
       {
       index[0] = i+1;
/*
***Rad 1.
*/
       index[1] = 1;
       index[2] = 1;

       litval.lit.float_va = seg[i].c0x;
       inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va,
                                          3, index, &litval);
       ++index[2];
       litval.lit.float_va = seg[i].c0y;
       inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va,
                                          3, index, &litval);
       ++index[2];
       litval.lit.float_va = seg[i].c0z;
       inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va,
                                          3, index, &litval);
       ++index[2];
       litval.lit.float_va = seg[i].c0;
       inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va,
                                          3, index, &litval);
/*
***Rad 2.
*/
       index[1] = 2;
       index[2] = 1;

       litval.lit.float_va = seg[i].c1x;
       inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va,
                                           3, index, &litval);
       ++index[2];
       litval.lit.float_va = seg[i].c1y;
       inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va,
                                           3, index, &litval);
       ++index[2];
       litval.lit.float_va = seg[i].c1z;
       inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va,
                                           3, index, &litval);
       ++index[2];
       litval.lit.float_va = seg[i].c1;
       inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va,
                                           3, index, &litval);
/*
***Rad 3.
*/
       index[1] = 3;
       index[2] = 1;

       litval.lit.float_va = seg[i].c2x;
       inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va,
                                           3, index, &litval);
       ++index[2];
       litval.lit.float_va = seg[i].c2y;
       inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va,
                                           3, index, &litval);
       ++index[2];
       litval.lit.float_va = seg[i].c2z;
       inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va,
                                           3, index, &litval);
       ++index[2];
       litval.lit.float_va = seg[i].c2;
       inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va,
                                           3, index, &litval);
/*
***Rad 4.
*/
       index[1] = 4;
       index[2] = 1;

       litval.lit.float_va = seg[i].c3x;
       inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va,
                                           3, index, &litval);
       ++index[2];
       litval.lit.float_va = seg[i].c3y;
       inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va,
                                           3, index, &litval);
       ++index[2];
       litval.lit.float_va = seg[i].c3z;
       inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va,
                                           3, index, &litval);
       ++index[2];
       litval.lit.float_va = seg[i].c3;
       status = inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va,
                                           3, index, &litval);

       }
    return(status);

  }

/********************************************************/
/*!******************************************************/

        short evwfvk(
        DBfloat    fvk[],
        DBint    nval,
        DBint    pn,
        PMPARVA *pv)

/*      Skriver en array av flyttal till en MBS-variabel.
 *
 *      In: fvk   => Array med värden.
 *          nval  => Antal värden.
 *          pn    => Parameterns ordningsnummer.
 *          pv    => Pekare till array med parameteinfo.
 *
 *      Ut: Inget.
 *
 *      FV: Inget, dvs 0.
 *
 *      (C)microform ab 21/3/86 J. Kjellander
 *
 *      20/5/86 Initiering av status till 0 R. Svedin
 *
 ******************************************************!*/

  {
    short   status;
    DBint   i,index[1];
    PMLITVA litval;

    status = 0;

    for ( i=0; i<nval; ++i )
      {
      index[0] = i+1;
      litval.lit.float_va = fvk[i];
      status = inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va, 
               1, index, &litval);
      }

    return(status);

  }

  
  
/********************************************************/
/*!******************************************************/

        short evwivk(
        DBint    ivk[],
        DBint    nval,
        DBint    pn,
        PMPARVA *pv)

/*      Skriver en array av heltal till en MBS-variabel.
 *
 *      In: ivk   => Array med värden.
 *          nval  => Antal värden.
 *          pn    => Parameterns ordningsnummer.
 *          pv    => Pekare till array med parameteinfo.
 *
 *      Ut: Inget.
 *
 *      FV: Inget, dvs 0.
 *
 *      (C)Sören Larsson, Örebro University 2004-09-10
 *
 ******************************************************!*/

  {
    short   status;
    DBint   i,index[1];
    PMLITVA litval;

    status = 0;

    for ( i=0; i<nval; ++i )
      {
      index[0] = i+1;
      litval.lit.int_va = ivk[i];
      status = inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va, 
               1, index, &litval);
      }

    return(status);

  }
  
  
  
  
  
  
  
  
  
  
  
  
  
  
/********************************************************/
/*!******************************************************/

        short evwvec(
        DBVector    vvk[],
        DBint    nval,
        DBint    pn,
        PMPARVA *pv)

/*      Skriver en array av DBVector till en MBS-variabel.
 *
 *      In: vvk   => Array med värden.
 *          nval  => Antal värden.
 *          pn    => Parameterns ordningsnummer.
 *          pv    => Pekare till array med parameteinfo.
 *
 *      Ut: Inget.
 *
 *      FV: Inget, dvs 0.
 *
 *      (C)microform ab 13/9/95 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBint   index[1],i;
    PMLITVA litval;

    status = 0;

    for ( i=0; i<nval; ++i )
      {
      index[0] = i+1;
      litval.lit.vec_va.x_val = vvk[i].x_gm;
      litval.lit.vec_va.y_val = vvk[i].y_gm;
      litval.lit.vec_va.z_val = vvk[i].z_gm;
      status = inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va, 
               1, index, &litval);
      }

    return(status);
  }

/********************************************************/
/*!******************************************************/

        short evwrve(
        PMREFVA  idvek[],
        DBint    i,
        DBint    pn,
        PMPARVA *pv)

/*      Skriver ett element i en array av referenser
 *      till en MBS-variabel.
 *
 *      In: idvek => En referens.
 *          i     => Element index. 1:a element = 0.
 *          pn    => Parameterns ordningsnummer.
 *          pv    => Pekare till array med parameteinfo.
 *
 *      Ut: Inget.
 *
 *      FV: Inget, dvs 0.
 *
 *      (C)microform ab 28/3/86 J. Kjellander
 *
 *      86/10/23 Bug, J. Kjellander
 *
 ******************************************************!*/

  {
    DBint   index[1],j;
    PMLITVA litval;

/*
***Kopiera referensen till en PMLITVA.
*/
    for ( j=0; ; j++ )
      {
      litval.lit.ref_va[j].seq_val = idvek[j].seq_val;
      litval.lit.ref_va[j].ord_val = idvek[j].ord_val;
      litval.lit.ref_va[j].p_nextre = &litval.lit.ref_va[j+1];
      if ( idvek[j].p_nextre == NULL ) break;
      }
    litval.lit.ref_va[j].p_nextre = NULL;
/*
***Skriv till MBS-variablen.
*/
    index[0] = i+1;

    return(inwvar(pv[pn].par_ty, pv[pn].par_va.lit.adr_va, 
           1, index, &litval));
  }

/********************************************************/
