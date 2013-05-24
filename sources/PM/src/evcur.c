/**********************************************************************
*
*    evcur.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    vcufr();      Evaluerar CUR_FREE
*    vcupr();      Evaluerar CUR_PROJ
*    evcurs();     Evaluerar CUR_SPLINE
*    evcusa();     Evaluerar CUR_SPLARR
*    evcuna();     Evaluerar CUR_NURBSARR
*    evcurc();     Evaluerar CUR_CONIC
*    evcuca();     Evaluerar CUR_CONARR
*    evcomp();     Evaluerar CUR_COMP
*    evcuro();     Evaluerar CUR_OFFS
*    evcurt();     Evaluerar CUR_TRIM
*    evcuru();     Evaluerar CUR_USRDEF
*
*    evcsil();     Evaluerar CUR_SIL
*    evcint();     Evaluerar CUR_INT
*    evciso();     Evaluerar CUR_ISO
*    evcfan();     Evaluerar CUR_FANGA
*    evcged();     Evaluerar CUR_GEODESIC
*    evcapp();     Evaluerar CUR_APPROX
*    evcugl();     Evaluerar CUR_GL

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
#include "../../EX/include/EX.h"
#include "../../GE/include/GE.h" /* comptol() i evcuna */


extern short  modtyp;

extern V2REFVA *geop_id; /* ingeop.c *identp  Storhetens ID */
extern PMPARVA *geop_pv; /* ingeop.c *pv Access structure for MBS routines */
extern short    geop_pc; /* ingeop.c parcount Number of actual parameters */
extern V2NAPA  *geop_np; /* ingeop.c *npblock Pekare till namnparameterblock.*/

/*!******************************************************/

        short evcufr()

/*      Evaluerar geometri-proceduren CUR_FREE.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 26/12/86 J. Kjellander
 *
 *      2001-02-02 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   short   npoi = 0;          /* Number of points */ 
   short   i;                 /* loop variable */
   V2VECVA poiarr[V2PARMAX];  /* point array */
   V2VECVA tanarr[V2PARMAX];  /* tangent array */

/*
***Skapa poiarr och tanarr.
*/
   for( i = 1; i <= geop_pc; i+=2 )
     { 
     poiarr[ npoi ].x_val = geop_pv[ i ].par_va.lit.vec_va.x_val;
     poiarr[ npoi ].y_val = geop_pv[ i ].par_va.lit.vec_va.y_val;
     poiarr[ npoi ].z_val = geop_pv[ i ].par_va.lit.vec_va.z_val;
     tanarr[ npoi ].x_val = geop_pv[ i+1 ].par_va.lit.vec_va.x_val;
     tanarr[ npoi ].y_val = geop_pv[ i+1 ].par_va.lit.vec_va.y_val;
     tanarr[ npoi ].z_val = geop_pv[ i+1 ].par_va.lit.vec_va.z_val;
     npoi++;
     }
/*
***Exekvera CUR_FREE.
*/
   return(EXcufr( geop_id, (DBVector *)poiarr, (DBVector *)tanarr, npoi, geop_np));
}

/********************************************************/
/*!******************************************************/

        short evcupr()

/*      Evaluerar geometri-proceduren CUR_PROJ.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 26/12/86 J. Kjellander
 *
 *      2001-02-02 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   short   npoi = 0;          /* Number of points */ 
   short   i;                 /* loop variable */
   V2VECVA poiarr[V2PARMAX];  /* point array */
   V2VECVA tanarr[V2PARMAX];  /* tangent array */

/*
***Skapa poiarr och tanarr.
*/
   for( i = 1; i <= geop_pc; i+=2 )
     { 
     poiarr[ npoi ].x_val = geop_pv[ i ].par_va.lit.vec_va.x_val;
     poiarr[ npoi ].y_val = geop_pv[ i ].par_va.lit.vec_va.y_val;
     poiarr[ npoi ].z_val = geop_pv[ i ].par_va.lit.vec_va.z_val;
     tanarr[ npoi ].x_val = geop_pv[ i+1 ].par_va.lit.vec_va.x_val;
     tanarr[ npoi ].y_val = geop_pv[ i+1 ].par_va.lit.vec_va.y_val;
     tanarr[ npoi ].z_val = geop_pv[ i+1 ].par_va.lit.vec_va.z_val;
     npoi++;
     }
/*
***Exekvera CUR_PROJ.
*/
   return(EXcupr( geop_id, (DBVector *)poiarr, (DBVector *)tanarr, npoi, geop_np));
}

/********************************************************/
/*!******************************************************/

        short evcurs()

/*      Evaluerar geometri-proceduren CUR_SPLINE.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IN5002 = Felaktig typ av kurva
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 15/11/91 J. Kjellander
 *
 *      2001-02-05 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   DBint   npoi;                  /* Number of points */ 
   int     i;                     /* Loop variable */
   char   *ptyp;                  /* Ferguson/Chord.... */
   DBint   type;                  /* Type code */
   V2VECVA poiarr[V2PARMAX];      /* Point array */
   V2VECVA tanarr[V2PARMAX];      /* Tangent array */

/*
***What type ?
*/
   ptyp = geop_pv[1].par_va.lit.str_va;

   if      ( sticmp(ptyp,"ferguson")  == 0 ) type = 1;
   else if ( sticmp(ptyp,"chord")     == 0 ) type = 2;
   else if ( sticmp(ptyp,"stiffness") == 0 ) type = 3;
   else if ( sticmp(ptyp,"free")      == 0 ) type = 4;
   else if ( sticmp(ptyp,"proj")      == 0 ) type = 5;
   else return(erpush("IN5002",ptyp));
/*
***Skapa poiarr och tanarr.
*/
   npoi = 0;

   for( i = 2; i <= geop_pc; i+=2 )
     { 
     poiarr[ npoi ].x_val = geop_pv[ i ].par_va.lit.vec_va.x_val;
     poiarr[ npoi ].y_val = geop_pv[ i ].par_va.lit.vec_va.y_val;
     poiarr[ npoi ].z_val = geop_pv[ i ].par_va.lit.vec_va.z_val;
     tanarr[ npoi ].x_val = geop_pv[ i+1 ].par_va.lit.vec_va.x_val;
     tanarr[ npoi ].y_val = geop_pv[ i+1 ].par_va.lit.vec_va.y_val;
     tanarr[ npoi ].z_val = geop_pv[ i+1 ].par_va.lit.vec_va.z_val;
     npoi++;
     }
/*
***Exekvera.
*/
   return(EXcurs(geop_id,type,(DBVector *)poiarr,
                              (DBVector *)tanarr,npoi,geop_np));
}

/********************************************************/
/*!******************************************************/

        short evcusa()

/*      Evaluerar geometri-proceduren CUR_SPLARR.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IN5002 = Felaktig typ av kurva
 *                IN5132 = Antal positioner < 2
 *                IN5142 = Deklarerad storlek < npos
 *                IN5152 = Kan ej allokera minne
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 28/2/95 J. Kjellander
 *
 *      2001-02-05 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   char     *ptyp;
   char      errbuf[81];
   short     status;
   int       dekl_dim,vecsiz;
   DBint     type,npos,i;
   DBint     posadr,tanadr = 0;
   STTYTBL   typtbl;
   STARRTY   arrtyp;
   DBVector *ppek,*tpek;
   PMLITVA   val;

/*
***What type ?
*/
   ptyp = geop_pv[1].par_va.lit.str_va;

   if      ( sticmp(ptyp,"ferguson")  == 0 ) type = 1;
   else if ( sticmp(ptyp,"chord")     == 0 ) type = 2;
   else if ( sticmp(ptyp,"stiffness") == 0 ) type = 3;
   else if ( sticmp(ptyp,"free")      == 0 ) type = 4;
   else if ( sticmp(ptyp,"proj")      == 0 ) type = 5;
   else return(erpush("IN5002",ptyp));
/*
***Hur många positioner.
*/
   npos = geop_pv[2].par_va.lit.int_va;
/*
***Kolla att antalet positioner är rimligt.
*/
   if ( npos < 2 ) return(erpush("IN5132",""));
/*
***Kolla att deklarerad dimension på MBS-arrayer är tillräcklig för
***npos positioner och tangenter.
*/
   posadr = geop_pv[3].par_va.lit.adr_va;
   strtyp(geop_pv[3].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&arrtyp);
   dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( dekl_dim < npos ) return(erpush("IN5142",""));

   if ( geop_pc == 4 )
     {
     tanadr = geop_pv[4].par_va.lit.adr_va;
     strtyp(geop_pv[4].par_ty,&typtbl);
     strarr(typtbl.arr_ty,&arrtyp);
     dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
     if ( dekl_dim < npos ) return(erpush("IN5142",""));
     }
/*
***Allokera minne för positioner och tangenter.
*/
   if ( (ppek=(DBVector *)v3mall(npos*sizeof(DBVector),"evcusa")) == NULL )
     {
     sprintf(errbuf,"%d",npos);
     return(erpush("IN5152",errbuf));
     }

   if ( (tpek=(DBVector *)v3mall(npos*sizeof(DBVector),"evcusa")) == NULL )
     {
     sprintf(errbuf,"%d",npos);
     return(erpush("IN5152",errbuf));
     }
/*
***vecsiz = storleken på en VECTOR.
*/
   strtyp(arrtyp.base_arr,&typtbl);
   vecsiz = typtbl.size_ty;
/*
***Kopiera positioner från RTS till allokerad area.
*/
   for ( i=0; i<npos; ++i )
     {
     ingval(posadr+i*vecsiz,arrtyp.base_arr,FALSE,&val);
     (ppek+i)->x_gm = val.lit.vec_va.x_val;
     (ppek+i)->y_gm = val.lit.vec_va.y_val;
     (ppek+i)->z_gm = val.lit.vec_va.z_val;
     if ( geop_pc == 4 )
       {
       ingval(tanadr+i*vecsiz,arrtyp.base_arr,FALSE,&val);
       (tpek+i)->x_gm = val.lit.vec_va.x_val;
       (tpek+i)->y_gm = val.lit.vec_va.y_val;
       (tpek+i)->z_gm = val.lit.vec_va.z_val;
       }
     else
       {
       (tpek+i)->x_gm = (tpek+i)->y_gm = (tpek+i)->z_gm = 0.0;
       }
     }
/*
***Anropa exe-rutinen.
*/
   status = EXcurs(geop_id,type,ppek,tpek,npos,geop_np);
/*
***Deallokera minne.
*/
   v3free(ppek,"evcusa");
   v3free(tpek,"evcusa");

   return(status);
}


/********************************************************/
/*!******************************************************/

        short evcuna()

/*      Evaluerar geometri-proceduren CUR_NURBSARR.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Error codes: IN5572  Knotvector must be non-decreasing
 *                   IN5582  Minimum 2 control points required
 *                   IN5592  Required number of controlpoints > declared size
 *                   IN5602  Required number of knot values > declared size
 *                   IN5612  Can't malloc for %s controlpoints/weights/knots
 *                   IN5622  Required number of weights > declared size
 *
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)020405 University of Örebro, J. Kjellander
 *
 *      2003-02-28 Weights and knots optional, Sören Larsson
 *
 ******************************************************!*/

{
   char      errbuf[81];
   short     status;
   int       dekl_dim,vecsiz,fltsiz,degree;
   DBint     ncpts,nknots,i;
   DBint     cptadr,wadr,kadr = 0;
   STTYTBL   typtbl;
   STARRTY   cpttyp,wtyp,ktyp;
   DBVector *cptpek;
   DBfloat  *wpek,*kpek;
   PMLITVA   val;
   int       knotindex;
   DBfloat   comptol;

   comptol   = varkon_comptol();


/*
***What degree (degree=order-1)
*/
   degree = geop_pv[1].par_va.lit.int_va-1;

/*
***How many controlpoints.
*/
   ncpts = geop_pv[2].par_va.lit.int_va;

/*
***Check that there are at least 2 cpts.
*/
   if ( ncpts < 2 ) return(erpush("IN5582",""));

/*
***Check declared dimension of cpts.
*/
   cptadr = geop_pv[3].par_va.lit.adr_va;
   strtyp(geop_pv[3].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&cpttyp);
   dekl_dim = cpttyp.up_arr - cpttyp.low_arr + 1;
   if ( dekl_dim < ncpts ) return(erpush("IN5592",""));
   strtyp(cpttyp.base_arr,&typtbl);
   vecsiz = typtbl.size_ty;


/*
***Check declared dimension of w.
*/

   if ( geop_pc > 3 ) /* Weights is given */
      {
      wadr = geop_pv[4].par_va.lit.adr_va;
      strtyp(geop_pv[4].par_ty,&typtbl);
      strarr(typtbl.arr_ty,&wtyp);
      dekl_dim = wtyp.up_arr - wtyp.low_arr + 1;
      if ( dekl_dim < ncpts ) return(erpush("IN5622",""));
      strtyp(wtyp.base_arr,&typtbl);
      fltsiz = typtbl.size_ty;
      }


/*
***Check size of knotvector.
*/
   nknots=ncpts+degree+1;
   
   if ( geop_pc > 4 ) /* Knots is given */
      {
      kadr = geop_pv[5].par_va.lit.adr_va;
      strtyp(geop_pv[5].par_ty,&typtbl);
      strarr(typtbl.arr_ty,&ktyp);
      dekl_dim = ktyp.up_arr - ktyp.low_arr + 1;
      if ( dekl_dim < nknots ) return(erpush("IN5602",""));
      }


/*
***Allocate memory for cpts, w and k 
*/
   if ( (cptpek=(DBVector *)v3mall(ncpts*sizeof(DBVector),"evcuna")) == NULL )
     {
     sprintf(errbuf,"%d",ncpts);
     return(erpush("IN5612",errbuf));
     }

   if ( (wpek=(DBfloat *)v3mall(ncpts*sizeof(DBfloat),"evcuna")) == NULL )
     {
     sprintf(errbuf,"%d",ncpts);
     return(erpush("IN5612",errbuf));
     }

  if ( (kpek=(DBfloat *)v3mall(nknots*sizeof(DBfloat),"evcuna")) == NULL )
     {
     sprintf(errbuf,"%d",nknots);
     return(erpush("IN5612",errbuf));
     }


/*
***Copy from RTS to allocated area.
*/
   
   /* cpts and weights */
   if ( geop_pc > 3 )  /* Weights is given */
      {
      for ( i=0; i<ncpts; ++i )
        {
        ingval(cptadr+i*vecsiz,cpttyp.base_arr,FALSE,&val);
        (cptpek+i)->x_gm = val.lit.vec_va.x_val;
        (cptpek+i)->y_gm = val.lit.vec_va.y_val;
        (cptpek+i)->z_gm = val.lit.vec_va.z_val;

        ingval(wadr+i*fltsiz,wtyp.base_arr,FALSE,&val);
         *(wpek+i) = val.lit.float_va;
        }
      } 
 
   else /* Weights is not given */
      {
      for ( i=0; i<ncpts; ++i )
        {
        ingval(cptadr+i*vecsiz,cpttyp.base_arr,FALSE,&val);
        (cptpek+i)->x_gm = val.lit.vec_va.x_val;
        (cptpek+i)->y_gm = val.lit.vec_va.y_val;
        (cptpek+i)->z_gm = val.lit.vec_va.z_val;

        *(wpek+i) = 1.0;
        }
      } 
    


   /* knots */
   if ( geop_pc > 4 )  /* Knots is given, check non-decreasing */
      {
      ingval(kadr+0*fltsiz,ktyp.base_arr,FALSE,&val);
      *(kpek+0) = val.lit.float_va;
      
      for ( i=1; i < nknots; ++i )
        {
        ingval(kadr+i*fltsiz,ktyp.base_arr,FALSE,&val);
        *(kpek+i) = val.lit.float_va;
        if((*(kpek+i-1) - *(kpek+i) )> comptol)return(erpush("IN5572",""));
        }
      
      }  
    else   /* Knots is not given, create them */
      {
      knotindex=0;
      for ( i=0; i < degree+1; ++i )
         {
         *(kpek+knotindex) = 0.0;
         ++knotindex;
         }
      for ( i=1; i < 1 + nknots - 2*(degree+1);  ++i )
         {
         *(kpek+knotindex) = i;
         ++knotindex;
         }
      for ( i=0; i < (degree+1);  ++i )
         {
         *(kpek+knotindex) = nknots - 2 * (degree + 1) +1;
         ++knotindex;
         }
      }


/*
***Call executer.
*/
   status = EXcunu(geop_id,cptpek,wpek,ncpts,kpek,nknots,degree,geop_np);


/*
***Deallocate memory.
*/
   v3free(cptpek,"evcuna");
   v3free(wpek,"evcuna");
   v3free(kpek,"evcuna");

   return(status);
}



/********************************************************/
/*!******************************************************/

        short evcurc()

/*      Evaluerar geometri-proceduren CUR_CONIC.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IN5002 = Felaktig typ av kurva
 *                IN5012 = P-värde < 0
 *                IN5022 = P-värde felaktig typ
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 12/11/91 J. Kjellander
 *
 *      2001-02-05 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   short   npoi = 0;              /* Number of points */ 
   short   i,j;                   /* loop variable */
   char   *typ;                   /* FREE/PROJ */
   bool    proj;                  /* Projicera ja/nej */
   V2VECVA poiarr[V2PARMAX];      /* point array */
   V2VECVA tanarr[V2PARMAX];      /* tangent array */
   V2VECVA iparr [V2PARMAX];      /* intermediate point array */
   DBfloat   pvarr [V2PARMAX];      /* P-value array */
   char    errbuf[20];            /* Felbuffert */

/*
***FREE eller PROJ.
*/
   typ = geop_pv[1].par_va.lit.str_va;
   if      ( sticmp(typ,"free") == 0 ) proj = FALSE;
   else if ( sticmp(typ,"proj") == 0 ) proj = TRUE;
   else return(erpush("IN5002",typ));
/*
***Skapa poiarr och tanarr.
*/
   for( j=0,i=2; i<geop_pc; j++,i+=3 )
     { 
     poiarr[j].x_val = geop_pv[i  ].par_va.lit.vec_va.x_val;
     poiarr[j].y_val = geop_pv[i  ].par_va.lit.vec_va.y_val;
     poiarr[j].z_val = geop_pv[i  ].par_va.lit.vec_va.z_val;
     tanarr[j].x_val = geop_pv[i+1].par_va.lit.vec_va.x_val;
     tanarr[j].y_val = geop_pv[i+1].par_va.lit.vec_va.y_val;
     tanarr[j].z_val = geop_pv[i+1].par_va.lit.vec_va.z_val;
     npoi++;
     }
/*
***Skapa pvarr och iparr.
*/
   for( j=0,i=4; i<geop_pc; j++,i+=3 )
     { 
     switch ( geop_pv[i].par_va.lit_type )
       {
       case C_INT_VA:
       iparr[j].x_val = 0.0;
       iparr[j].y_val = 0.0;
       iparr[j].z_val = 0.0;
       pvarr[j] = (DBfloat)geop_pv[i].par_va.lit.int_va;
       if ( pvarr[j] < 0.0 )
         {
         sprintf(errbuf,"%d",(int)(j+1));
         return(erpush("IN5012",errbuf));
         }
       break;

       case C_FLO_VA:
       iparr[j].x_val = 0.0;
       iparr[j].y_val = 0.0;
       iparr[j].z_val = 0.0;
       pvarr[j] = (DBfloat)geop_pv[i].par_va.lit.float_va;
       if ( pvarr[j] < 0.0 )
         {
         sprintf(errbuf,"%d",(int)(j+1));
         return(erpush("IN5012",errbuf));
         }
       break;

       case C_VEC_VA:
       iparr[j].x_val = geop_pv[i].par_va.lit.vec_va.x_val;
       iparr[j].y_val = geop_pv[i].par_va.lit.vec_va.y_val;
       iparr[j].z_val = geop_pv[i].par_va.lit.vec_va.z_val;
       pvarr[j] = -1.0;
       break;

       default:
       return(erpush("IN5022",""));
       }
     }
/*
***Exekvera CUR_CONIC.
*/
   return(EXcurc(geop_id,proj,(DBVector *)poiarr,
	                          (DBVector *)tanarr,
                              (DBVector *)iparr,pvarr,npoi,geop_np));
}

/********************************************************/
/*!******************************************************/

        short evcuca()

/*      Evaluerar geometri-proceduren CUR_CONARR.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IN5002 = Felaktig typ av kurva
 *                IN5162 = Antal positioner < 2
 *                IN5172 = Deklarerad storlek < npos
 *                IN5182 = Kan ej allokera minne
 *                IN5192 = Typ skall vara m/M/p/P
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 2/3/95 J. Kjellander
 *
 *      2001-02-05 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   bool    proj;
   char   *typ;
   char    errbuf[81];
   short   status;
   int     dekl_dim,vecsiz,strsiz,fltsiz;
   DBint   npos,i;
   DBint   posadr,tanadr,metadr,pvadr,mpadr;
   DBfloat  *pvpek;
   STTYTBL typtbl;
   STARRTY postyp,tantyp,mettyp,pvtyp,mptyp;
   DBVector  *ppek,*tpek,*mppek;
   PMLITVA val;

/*
***FREE eller PROJ.
*/
   typ = geop_pv[1].par_va.lit.str_va;
   if      ( sticmp(typ,"free") == 0 ) proj = FALSE;
   else if ( sticmp(typ,"proj") == 0 ) proj = TRUE;
   else return(erpush("IN5002",typ));
/*
***Hur många positioner.
*/
   npos = geop_pv[2].par_va.lit.int_va;
/*
***Kolla att antalet positioner är rimligt.
*/
   if ( npos < 2 ) return(erpush("IN5162",""));
/*
***Kolla att deklarerad dimension på MBS-arrayer är tillräcklig för
***npos positioner, tangenter osv.
*/
   posadr = geop_pv[3].par_va.lit.adr_va;
   strtyp(geop_pv[3].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&postyp);
   dekl_dim = postyp.up_arr - postyp.low_arr + 1;
   if ( dekl_dim < npos ) return(erpush("IN5172",""));

   strtyp(postyp.base_arr,&typtbl);
   vecsiz = typtbl.size_ty;

   tanadr = geop_pv[4].par_va.lit.adr_va;
   strtyp(geop_pv[4].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&tantyp);
   dekl_dim = tantyp.up_arr - tantyp.low_arr + 1;
   if ( dekl_dim < npos ) return(erpush("IN5172",""));

   metadr = geop_pv[5].par_va.lit.adr_va;
   strtyp(geop_pv[5].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&mettyp);
   dekl_dim = mettyp.up_arr - mettyp.low_arr + 1;
   if ( dekl_dim < npos-1 ) return(erpush("IN5172",""));

   strtyp(mettyp.base_arr,&typtbl);
   strsiz = typtbl.size_ty;

   pvadr = geop_pv[6].par_va.lit.adr_va;
   strtyp(geop_pv[6].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&pvtyp);
   dekl_dim = pvtyp.up_arr - pvtyp.low_arr + 1;
   if ( dekl_dim < npos-1 ) return(erpush("IN5172",""));

   strtyp(pvtyp.base_arr,&typtbl);
   fltsiz = typtbl.size_ty;

   mpadr = geop_pv[7].par_va.lit.adr_va;
   strtyp(geop_pv[7].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&mptyp);
   dekl_dim = mptyp.up_arr - mptyp.low_arr + 1;
   if ( dekl_dim < npos-1 ) return(erpush("IN5172",""));
/*
***Allokera minne.
*/
   if ( (ppek=(DBVector *)v3mall(npos*sizeof(DBVector),"evcuca")) == NULL )
     {
     sprintf(errbuf,"%d",npos);
     return(erpush("IN5182",errbuf));
     }

   if ( (tpek=(DBVector *)v3mall(npos*sizeof(DBVector),"evcuca")) == NULL )
     {
     sprintf(errbuf,"%d",npos);
     return(erpush("IN5182",errbuf));
     }

   if ( (pvpek=(DBfloat *)v3mall(npos*sizeof(gmflt),"evcuca")) == NULL )
     {
     sprintf(errbuf,"%d",npos);
     return(erpush("IN5182",errbuf));
     }

   if ( (mppek=(DBVector *)v3mall(npos*sizeof(DBVector),"evcuca")) == NULL )
     {
     sprintf(errbuf,"%d",npos);
     return(erpush("IN5182",errbuf));
     }
/*
***Kopiera från RTS till allokerade areor.
*/
   for ( i=0; i<npos; ++i )
     {
     ingval(posadr+i*vecsiz,postyp.base_arr,FALSE,&val);
     (ppek+i)->x_gm = val.lit.vec_va.x_val;
     (ppek+i)->y_gm = val.lit.vec_va.y_val;
     (ppek+i)->z_gm = val.lit.vec_va.z_val;

     ingval(tanadr+i*vecsiz,tantyp.base_arr,FALSE,&val);
     (tpek+i)->x_gm = val.lit.vec_va.x_val;
     (tpek+i)->y_gm = val.lit.vec_va.y_val;
     (tpek+i)->z_gm = val.lit.vec_va.z_val;
     }

   for ( i=0; i<npos-1; ++i )
     {
     ingval(metadr+i*strsiz,mettyp.base_arr,FALSE,&val);
     if ( sticmp(val.lit.str_va,"p") == 0 )
       {
       ingval(pvadr+i*fltsiz,pvtyp.base_arr,FALSE,&val);
      *(pvpek+i) = val.lit.float_va;
       (mppek+i)->x_gm =
       (mppek+i)->y_gm =
       (mppek+i)->z_gm = 0.0;
       }
     else if ( sticmp(val.lit.str_va,"m") == 0 )
       {
       ingval(mpadr+i*vecsiz,mptyp.base_arr,FALSE,&val);
      *(pvpek+i) = -1.0;
       (mppek+i)->x_gm = val.lit.vec_va.x_val;
       (mppek+i)->y_gm = val.lit.vec_va.y_val;
       (mppek+i)->z_gm = val.lit.vec_va.z_val;
       }
     else
       {
       status = erpush("IN5192","");
       goto exit;
       }
     }
/*
***Anropa exe-rutinen.
*/
   status = EXcurc(geop_id,proj,ppek,tpek,mppek,pvpek,(short)npos,geop_np);
/*
***Deallokera minne.
*/
exit:
   v3free(ppek,"evcuca");
   v3free(tpek,"evcuca");
   v3free(pvpek,"evcuca");
   v3free(mppek,"evcuca");

   return(status);
}

/********************************************************/
/*!******************************************************/

        short evcuro()

/*      Evaluerar geometri-proceduren CUR_OFFS.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 15/11/91 J. Kjellander
 *
 *      2001-02-05 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera CUR_OFFS.
*/
   return(EXcuro(geop_id,geop_pv[1].par_va.lit.ref_va,
                         geop_pv[2].par_va.lit.float_va,
                         geop_np));
}

/********************************************************/
/*!******************************************************/

        short evcomp()

/*      Evaluerar geometri-proceduren CUR_COMP.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 26/12/86 J. Kjellander
 *
 *      2001-02-05 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   short i;                        /* loop variable */
   short  nref = 0;                /* Number of referenses */ 
   V2REFVA  refarr[ GMMXCO + 1 ];  /* referens array */

/*
***Skapa refarr.
*/
   for ( i = 1 ; i <= geop_pc ; i++ )
     {
     refarr[ nref ].seq_val = geop_pv[i].par_va.lit.ref_va[ 0 ].seq_val;
     refarr[ nref ].ord_val = geop_pv[i].par_va.lit.ref_va[ 0 ].ord_val;
     refarr[ nref ].p_nextre = geop_pv[i].par_va.lit.ref_va[ 0 ].p_nextre;
     nref++;
     }
/*
***Exekvera COMP.
*/
   return(EXcomp( geop_id, refarr, nref,NULL, geop_np));

}

/********************************************************/
/*!******************************************************/

        short evcurt()

/*      Evaluerar geometri-proceduren CUR_TRIM.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 15/11/91 J. Kjellander
 *
 *      2001-02-05 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera CUR_TRIM.
*/
    return(EXcurt(geop_id,geop_pv[1].par_va.lit.ref_va,
                          geop_pv[2].par_va.lit.float_va,
                          geop_pv[3].par_va.lit.float_va,
                          geop_np));
}

/********************************************************/
/*!******************************************************/

        short evcuru()

/*      Evaluerar geometri-proceduren CUR_USDEF.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IN5032 = nseg < 0
 *                IN5042 = Kan ej mallokera minne för nseg DBSeg
 *                IN5052 = nseg > Dimensionen på 4X4-variabeln.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 15/11/91 J. Kjellander
 *
 *      2/10/92    Slutputs, J. Kjellander
 *      5/3/93     CUB_SEG, J. Kjellander
 *      14/6/93    Dynamiska segment, J. Kjellander
 *      1/6/94     VAX-bug sl=0, J. Kjellander
 *      1997-12-18 Yt-ID, J.Kjellander
 *      2001-02-05 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   short    nseg,status;
   int      up_bound,i;
   DBint    valadr;
   int      segsiz,radsiz,fltsiz;
   PMREFVA *surid;
   PMLITVA  fval;
   STTYTBL  typtbl;
   STARRTY  arrtyp;
   DBSeg   *segptr,*segdat;


/*
***v3c kollar att indata är:
***  Par 1: INT          Antal segment, ett värde.
***  Par 2: FLOAT(?,4,4) Segmentdata, en adress.
***Antal segment ?.
*/
   nseg = (short)geop_pv[1].par_va.lit.int_va;
   if ( nseg < 1 ) return(erpush("IN5032",""));
/*
***Allokera minne för nseg segment.
*/
   segptr = segdat = DBcreate_segments((int)nseg);
   if ( segptr == NULL ) return(erpush("IN5042",""));
/*
***Kolla att dimensionen på MBS-segdata-arrayen är tillräcklig för
***nseg segment, dvs att den är deklarerad minst (nseg,4,4), detta
***kan ju inte kollas av v3c.
*/
   strtyp(geop_pv[2].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&arrtyp);
   up_bound = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( up_bound < nseg )
     {
     status = erpush("IN5052","");
     goto end;
     }
/*
***Beräkna div. RTS-offset.
***segsiz = storleken på en FLOAT (4,4)     Normalt 128 bytes.
***radsiz = storleken på en FLOAT (4)       Normalt 32  bytes.
***fltsiz = storleken på en FLOAT           Normalt 8   bytes.
*/
   strtyp(arrtyp.base_arr,&typtbl);
   segsiz = typtbl.size_ty;

   strarr(typtbl.arr_ty,&arrtyp);
   strtyp(arrtyp.base_arr,&typtbl);
   radsiz = typtbl.size_ty;

   strarr(typtbl.arr_ty,&arrtyp);
   strtyp(arrtyp.base_arr,&typtbl);
   fltsiz = typtbl.size_ty;
/*
***Kopiera 4X4-matriserna till DBSeg, och sätt offset = 0.0.
*/
   valadr = geop_pv[2].par_va.lit.adr_va;

   for ( i=0; i<nseg; ++i )
     {
     ingval(valadr,arrtyp.base_arr,FALSE,&fval);
     segptr->c0x = fval.lit.float_va;
     ingval(valadr+fltsiz,arrtyp.base_arr,FALSE,&fval);
     segptr->c0y = fval.lit.float_va;
     ingval(valadr+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
     segptr->c0z = fval.lit.float_va;
     ingval(valadr+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
     segptr->c0  = fval.lit.float_va;

     ingval(valadr+radsiz,arrtyp.base_arr,FALSE,&fval);
     segptr->c1x = fval.lit.float_va;
     ingval(valadr+radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
     segptr->c1y = fval.lit.float_va;
     ingval(valadr+radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
     segptr->c1z = fval.lit.float_va;
     ingval(valadr+radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
     segptr->c1  = fval.lit.float_va;

     ingval(valadr+2*radsiz,arrtyp.base_arr,FALSE,&fval);
     segptr->c2x = fval.lit.float_va;
     ingval(valadr+2*radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
     segptr->c2y = fval.lit.float_va;
     ingval(valadr+2*radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
     segptr->c2z = fval.lit.float_va;
     ingval(valadr+2*radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
     segptr->c2  = fval.lit.float_va;

     ingval(valadr+3*radsiz,arrtyp.base_arr,FALSE,&fval);
     segptr->c3x = fval.lit.float_va;
     ingval(valadr+3*radsiz+fltsiz,arrtyp.base_arr,FALSE,&fval);
     segptr->c3y = fval.lit.float_va;
     ingval(valadr+3*radsiz+2*fltsiz,arrtyp.base_arr,FALSE,&fval);
     segptr->c3z = fval.lit.float_va;
     ingval(valadr+3*radsiz+3*fltsiz,arrtyp.base_arr,FALSE,&fval);
     segptr->c3  = fval.lit.float_va;

     segptr->typ = CUB_SEG;
     segptr->ofs = 0.0;
     segptr->sl  = 0.0;

     valadr += segsiz;
     segptr++;
     }
/*
***Har yt-id skickats med ?
*/
   if ( geop_pc == 3 ) surid = geop_pv[3].par_va.lit.ref_va;
   else           surid = NULL;
/*
***Exekvera CUR_USDEF.
*/
   status = EXcuru(geop_id,nseg,segdat,surid,geop_np);
/*
***Slut.
*/
end:
   DBfree_segments(segdat);
   return(status);
}

/********************************************************/
/*!******************************************************/

        short evcsil()

/*      Evaluerar geometri-proceduren CUR_SIL.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 20/11/92 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera CUR_SIL.
*/
   return(EXcsil( geop_id,   &geop_pv[1].par_va.lit.ref_va[0],
                 (DBVector *)&geop_pv[2].par_va.lit.vec_va,
                       (short)geop_pv[3].par_va.lit.int_va,
                       (short)geop_pv[4].par_va.lit.int_va, geop_np));
}

/********************************************************/
/*!******************************************************/

        short evcint()

/*      Evaluerar geometri-proceduren CUR_INT.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 20/11/92 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera CUR_INT.
*/
   return(EXcint( geop_id,&geop_pv[1].par_va.lit.ref_va[0],
                          &geop_pv[2].par_va.lit.ref_va[0],
                   (short)geop_pv[3].par_va.lit.int_va, geop_np));
}

/********************************************************/
/*!******************************************************/

        short evciso()

/*      Evaluerar geometri-proceduren CUR_ISO.
 *
 *      In: extern geop_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 20/11/92 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera CUR_ISO.
*/
   return(EXciso( geop_id,&geop_pv[1].par_va.lit.ref_va[0],
                           geop_pv[2].par_va.lit.str_va,
                           geop_pv[3].par_va.lit.float_va,
                           geop_pv[4].par_va.lit.float_va,
                           geop_pv[5].par_va.lit.float_va, geop_np));
}

/********************************************************/
/*!******************************************************/

        short evcfan()

/*      Evaluerar geometri-proceduren CUR_FANGA.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 28/2/95 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   DBint sstart,sslut;
   DBfloat ustart,uslut;

/*
***Optionella parametrar.
*/
   if ( geop_pc == 7 )
     {
     sstart = geop_pv[4].par_va.lit.int_va;
     sslut  = geop_pv[5].par_va.lit.int_va;
     ustart = geop_pv[6].par_va.lit.float_va;
     uslut  = geop_pv[7].par_va.lit.float_va;
     }
   else
     {
     sstart = 0;
     sslut  = 0;
     ustart = 0.0;
     uslut  = 0.0;
     }
/*
***Exekvera.
*/
   return(EXcfan( geop_id,&geop_pv[1].par_va.lit.ref_va[0],
              (DBVector *)&geop_pv[2].par_va.lit.vec_va,
              (DBVector *)&geop_pv[3].par_va.lit.vec_va,
               sstart,sslut,ustart,uslut,geop_np));
 }

/********************************************************/
/*!******************************************************/

        short evcged()

/*      Evaluerar geometri-proceduren CUR_GEODESIC.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 28/2/95 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera.
*/
   return(EXcged( geop_id,&geop_pv[1].par_va.lit.ref_va[0],
                           geop_pv[2].par_va.lit.str_va,
              (DBVector *)&geop_pv[3].par_va.lit.vec_va,
              (DBVector *)&geop_pv[4].par_va.lit.vec_va,
                           geop_np));
}

/********************************************************/
/*!******************************************************/

        short evcapp()

/*      Evaluerar geometri-proceduren CUR_APPROX.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 28/2/95 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   DBfloat tol_2;

/*
***Optionell parameter.
*/
   if ( geop_pc == 5 ) tol_2 = geop_pv[5].par_va.lit.float_va;
   else                tol_2 = geop_pv[4].par_va.lit.float_va;
/*
***Exekvera.
*/
   return(EXcapp( geop_id,&geop_pv[1].par_va.lit.ref_va[0],
                           geop_pv[2].par_va.lit.str_va,
                           geop_pv[3].par_va.lit.int_va,
                           geop_pv[4].par_va.lit.float_va,
                     tol_2,geop_np));
}

/********************************************************/
/*!******************************************************/

        short evcugl()

/*      Evaluerar geometri-proceduren CUR_GL.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 20/11/92 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera CUR_GL.
*/
   return(EXcugl( geop_id,&geop_pv[1].par_va.lit.ref_va[0],
                           geop_pv[2].par_va.lit.str_va, geop_np));
}

/********************************************************/
/********************************************************/






/*!******************************************************/

        short evccmparr()

/*      Evaluates the geometry procedure CUR_COMPARR.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Out: Nothing.
 *
 *      Errors:   IN5202 = Number of curves < 2.
 *                IN5212 = Number of curves > declared size.
 *                IN5222 = Unable to allocate memory.
 *
 *      FV: Returns status from called functions.
 *
 *      (c)Örebro University 2006-09-14  S. Larsson
 *
 *
 *
 ******************************************************!*/

{
   int      ncur;           /* Number of input entities         */
   DBint    curadr;         /* Adress to input curves           */
   STTYTBL  typtbl;         /* Type of variable                 */
   STARRTY  curtyp;         /* Type of array for input curves   */
   int      dekl_dim;       /* Declared dimension for the array */
   int      refsiz;         /* Size of ????                     */
   PMLITVA  val;            /* Value from ??                    */
   PMREFVA *lpek;           /* Pointer to array with references */
   int      i;              /* Loop index                       */
   short    status;         /* Status from execution function   */
   char     errbuf[81];     /* String variable for errors       */
   V2REFVA  *surid;
   
/*
***Id of surface for uv curves
*/
   if (geop_pc == 3) 
     {
     surid = &geop_pv[3].par_va.lit.ref_va[0];
     }
   else 
     {
     surid = NULL;
     }   

/*
***How many input entities for cur_comparr?.
*/
   ncur = geop_pv[1].par_va.lit.int_va;

/*
***Check that number of input entities are more than zero
*/
   if ( ncur < 1 ) return(erpush("IN5742",""));
/*
***Check that the declared dimension of the MBS-array is sufficient for ncur
*/
/* Address to the variable                                */
   curadr = geop_pv[2].par_va.lit.adr_va;
/* Get type of variable, which is defined in a structure  */
   strtyp(geop_pv[2].par_ty,&typtbl);
/* Get type of array                                      */
   strarr(typtbl.arr_ty,&curtyp);
/* Get the declared size of the array                     */
   dekl_dim = curtyp.up_arr - curtyp.low_arr + 1;
/* Return with error if array not has sufficient size     */
   if ( dekl_dim < ncur ) return(erpush("IN5752",""));

/*
***Get size
*/
   strtyp(curtyp.base_arr,&typtbl);
   refsiz = typtbl.size_ty;
/*
***Allocate memory.
*/
   if ( (lpek=(PMREFVA *)v3mall(ncur*sizeof(PMREFVA),"evccmparr")) == NULL )
     {
     sprintf(errbuf,"%d",ncur);
     return(erpush("IN5222",errbuf));
     }
/*
***Copy from RTS to the allocated area.
*/
   for ( i=0; i<ncur; ++i )
     {
     ingref(curadr+i*refsiz,&val);
     (lpek+i)->seq_val  = val.lit.ref_va[0].seq_val;
     (lpek+i)->ord_val  = val.lit.ref_va[0].ord_val;
     (lpek+i)->p_nextre = val.lit.ref_va[0].p_nextre;
     }

/*
***Call the execution function.
*/
status= EXcomp( geop_id, lpek, ncur, surid, geop_np);

/*
***Free memory
*/
    v3free(lpek,"evccmparr");
 
   return(status);
}
/********************************************************/




