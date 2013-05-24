/**********************************************************************
*
*    evsur.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*    evsexd();     Evaluerar SUR_EXDEF
*    evscon();     Evaluerar SUR_CONIC
*    evsoff();     Evaluerar SUR_OFFS
*    evssar();     Evaluerar SUR_SPLARR
*    evscar();     Evaluerar SUR_CONARR
*    evsnar();     Evaluerar SUR_NURBSARR
*    evsapp();     Evaluerar SUR_APPROX
*    evscom();     Evaluerar SUR_COMP
*    evstrm();     Evaluerar SUR_TRIM
*    evsrot();     Evaluerar SUR_ROT
*    evsusr();     Evaluerar SUR_USRDEF
*    evscyl();     Evaluerar SUR_CYL
*    evsswp();     Evaluerar SUR_SWEEP
*    evsrul();     Evaluerar SUR_RULED
*    evscur();     Evaluates SUR_CURVES
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

extern PMPARVA *proc_pv; /* inproc.ca *pv     Access structure for MBS routines */
extern short    proc_pc; /* inproc.c parcount Number of actual parameters */

extern V2REFVA *geop_id; /* ingeop.c *identp  Storhetens ID */
extern PMPARVA *geop_pv; /* ingeop.c *pv      Access structure for MBS routines */
extern short    geop_pc; /* ingeop.c parcount Number of actual parameters */
extern V2NAPA  *geop_np; /* ingeop.c *npblock Pekare till namnparameterblock.*/

extern PMPARVA *func_pv; /* Access structure for MBS routines */
extern short    func_pc; /* Number of actual parameters */
extern PMLITVA *func_vp; /* Pekare till resultat. */

/*!******************************************************/

        short evsexd()

/*      Evaluerar geometri-proceduren SUR_EXDEF.
 *
 *
 *      In: extern *geop_id       => Storhetens ID.
 *          extern *geop_pv       => Pekare till array med parametervärden.
 *          extern  geop_pc       => Antal parametrar.
 *          extern *geop_np       => Pekare till namnparameterblock.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 20/11/92 J. Kjellander
 *
 *      1997-03-12 pv[8], J.Kjellander
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
    DBfloat trim[4];
/*
***trim-värden.
*/
   trim[0] = geop_pv[4].par_va.lit.float_va;
   trim[1] = geop_pv[5].par_va.lit.float_va;
   trim[2] = geop_pv[6].par_va.lit.float_va;
   trim[3] = geop_pv[7].par_va.lit.float_va;
/*
***Exekvera SUR_EXDEF.
*/
   return(EXsexd( geop_id, geop_pv[1].par_va.lit.str_va,
                           geop_pv[2].par_va.lit.str_va,
                           geop_pv[3].par_va.lit.str_va,
                           trim,
                      (int)geop_pv[8].par_va.lit.int_va,
                           geop_np));
}

/********************************************************/
/*!******************************************************/

        short evscon()

/*      Evaluerar geometri-proceduren SUR_CONIC.
 *
 *      In: extern *geop_id       => Storhetens ID.
 *          extern *geop_pv       => Pekare till array med parametervärden.
 *          extern  geop_pc       => Antal parametrar.
 *          extern *geop_np       => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkkoder: IN5082 = Typ ej m/M/p/P
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 25/4/94 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   short    status;
   DBint    i,j,nstrips,typarr[MXLSTP];
   V2REFVA *spine;
   V2REFVA *limarr[MXLSTP];
   V2REFVA *tanarr[MXLSTP];
   V2REFVA *midarr[MXLSTP];

/*
***ID fär spinekurva.
*/
   spine = &geop_pv[1].par_va.lit.ref_va[0];
/*
***Hur märga strippar ?
*/
   nstrips = (geop_pc - 3)/4;
/*
***Skapa limarr och tanarr.
*/
   for( j=0,i=2; j<=nstrips; j++,i+=4 )
     { 
     limarr[j] = &geop_pv[i  ].par_va.lit.ref_va[0];
     tanarr[j] = &geop_pv[i+1].par_va.lit.ref_va[0];
     }
/*
***Skapa typarr och midarr.
*/
   for( j=0,i=4; j<nstrips; j++,i+=4 )
     { 
     if      ( sticmp(geop_pv[i].par_va.lit.str_va,"p") == 0 ) typarr[j] = 1;
     else if ( sticmp(geop_pv[i].par_va.lit.str_va,"m") == 0 ) typarr[j] = 2;
     else      return(erpush("IN5082",geop_pv[i].par_va.lit.str_va));
     midarr[j] = &geop_pv[i+1].par_va.lit.ref_va[0];
     }
/*
***Exekvera SUR_CONIC.
*/
    status = EXscon(geop_id,spine,nstrips,
                    limarr,tanarr,typarr,midarr,geop_np);

   return(status);
}

/********************************************************/
/*!******************************************************/

        short evsoff()

/*      Evaluerar geometri-proceduren SUR_OFFS.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 25/4/94 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{

/*
***Exekvera SUR_OFFS.
*/
   return(EXsoff( geop_id, geop_pv[1].par_va.lit.ref_va,
                           geop_pv[2].par_va.lit.float_va,
                           geop_np));
}

/********************************************************/
/*!******************************************************/

        short evssar()

/*      Evaluerar geometri-proceduren SUR_SPLARR.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkkoder: IN5102 = Kan ej allokera minne
 *                 IN5112 = Orimligt antal positioner.
 *                 IN5122 = nu eller nv > deklarerat
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 28/2/95 J. Kjellander
 *
 *      3/4/95  tv,tw, J. Kjellander
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

 {
   char    errbuf[81];
   short   status;
   int     nu_dekl,nv_dekl,dekl_dim,vecsiz;
   DBint   nu,nv,i,j;
   DBint   posadr,tuadr=0,tvadr=0,twadr=0;
   STTYTBL typtbl;
   STARRTY arrtyp;
   DBVector  *ppek,*tupek,*tvpek,*twpek;
   PMLITVA val;

/*
***Hur märga positioner.
*/
   nu = geop_pv[2].par_va.lit.int_va;
   nv = geop_pv[3].par_va.lit.int_va;
/*
***Kolla att antalet positioner är rimligt.
*/
   if ( nu < 2 ) return(erpush("IN5112","U"));
   if ( nv < 2 ) return(erpush("IN5112","V"));
/*
***Kolla att deklarerad dimension pärMBS-arrayer är tillrärklig fär
***nu*nv positioner och tangenter.
*/
   posadr = geop_pv[4].par_va.lit.adr_va;
/*
***Strtyp kan alltid gäras pären variabel och en variabel vet vi
***särert att det är eftersom v3c kollar detta (.av. i formpar.h).
*/
   strtyp(geop_pv[4].par_ty,&typtbl);
/*
***typtbl är en structure som bland annat talar om hurvida variabeln är
***en array eller inte. Om inte, felmeddelande.
*/
   if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5122","no array"));
/*
***Om det är en array kan vi nu ta reda pärvilken slags array.
*/
   strarr(typtbl.arr_ty,&arrtyp);
/*
***arrtyp är en structure som bland annat talar om hur den är dimensionerad.
***Den skall minst vara lika stor som antalet punkter i U-riktningen.
*/
   nu_dekl = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( nu_dekl < nu ) return(erpush("IN5122","nu > decl"));
/*
***Denna array skall i sin tur bestärav arrayer.
*/
   strtyp(arrtyp.base_arr,&typtbl);
   if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5122","1-dim array"));
/*
***Som ocksärskall vara tillrärkligt stora.
***Dvs. minst lika stora som antalet punkter i V-riktningen.
*/
   strarr(typtbl.arr_ty,&arrtyp);
   nv_dekl = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( nv_dekl < nv ) return(erpush("IN5122","nv > decl"));
/*
***Allokera minne fär positioner.
*/
   if ( (ppek=(DBVector *)v3mall(nu*nv*sizeof(DBVector),"evssar")) == NULL )
     {
     sprintf(errbuf,"%d",(int)(nu*nv));
     return(erpush("IN5102",errbuf));
     }
/*
***LikasärU-tangenterna.
*/
   if ( geop_pc > 4 )
     {
     tuadr = geop_pv[5].par_va.lit.adr_va;
     strtyp(geop_pv[5].par_ty,&typtbl);

     if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5122","no array"));
     strarr(typtbl.arr_ty,&arrtyp);
     dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
     if ( dekl_dim != nu_dekl ) return(erpush("IN5122","tu(nu,-) != p(nu,-)"));
     strtyp(arrtyp.base_arr,&typtbl);
     if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5122","1-dim array"));
     strarr(typtbl.arr_ty,&arrtyp);
     dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
     if ( dekl_dim != nv_dekl ) return(erpush("IN5122","tu(-,nv) != p(-,nv)"));

     if ( (tupek=(DBVector *)v3mall(nu*nv*sizeof(DBVector),"evssar")) == NULL )
       {
       sprintf(errbuf,"%d",(int)(nu*nv));
       return(erpush("IN5102",errbuf));
       }
     }
   else tupek = NULL;
/*
***V-tangenterna.
*/
   if ( geop_pc > 5 )
     {
     tvadr = geop_pv[6].par_va.lit.adr_va;
     strtyp(geop_pv[6].par_ty,&typtbl);

     if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5122","no array"));
     strarr(typtbl.arr_ty,&arrtyp);
     dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
     if ( dekl_dim != nu_dekl ) return(erpush("IN5122","tv(nu,-) != p(nu,-)"));
     strtyp(arrtyp.base_arr,&typtbl);
     if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5122","1-dim array"));
     strarr(typtbl.arr_ty,&arrtyp);
     dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
     if ( dekl_dim != nv_dekl ) return(erpush("IN5122","tv(-,nv) != p(-,nv)"));

     if ( (tvpek=(DBVector *)v3mall(nu*nv*sizeof(DBVector),"evssar")) == NULL )
       {
       sprintf(errbuf,"%d",(int)(nu*nv));
       return(erpush("IN5102",errbuf));
       }
     }
   else tvpek = NULL;
/*
***Och twist.
*/
   if ( geop_pc > 6 )
     {
     twadr = geop_pv[7].par_va.lit.adr_va;
     strtyp(geop_pv[7].par_ty,&typtbl);

     if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5122","no array"));
     strarr(typtbl.arr_ty,&arrtyp);
     dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
     if ( dekl_dim != nu_dekl ) return(erpush("IN5122","tw(nu,-) != p(nu,-)"));
     strtyp(arrtyp.base_arr,&typtbl);
     if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5122","1-dim array"));
     strarr(typtbl.arr_ty,&arrtyp);
     dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
     if ( dekl_dim != nv_dekl ) return(erpush("IN5122","tw(-,nv) != p(-,nv)"));

     if ( (twpek=(DBVector *)v3mall(nu*nv*sizeof(DBVector),"evssar")) == NULL )
       {
       sprintf(errbuf,"%d",(int)(nu*nv));
       return(erpush("IN5102",errbuf));
       }
     }
   else twpek = NULL;
/*
***vecsiz = storleken pären VECTOR.
*/
   strtyp(arrtyp.base_arr,&typtbl);
   vecsiz = typtbl.size_ty;
/*
***Kopiera positioner frär RTS till allokerad area.
*/
   for ( i=0; i<nu; ++i )
     {
     for ( j=0; j<nv; ++j )
       {
       ingval(posadr+(i*nv_dekl+j)*vecsiz,arrtyp.base_arr,FALSE,&val);
       (ppek+i*nv+j)->x_gm = val.lit.vec_va.x_val;
       (ppek+i*nv+j)->y_gm = val.lit.vec_va.y_val;
       (ppek+i*nv+j)->z_gm = val.lit.vec_va.z_val;
       if ( geop_pc > 4 )
         {
         ingval(tuadr+(i*nv_dekl+j)*vecsiz,arrtyp.base_arr,FALSE,&val);
         (tupek+i*nv+j)->x_gm = val.lit.vec_va.x_val;
         (tupek+i*nv+j)->y_gm = val.lit.vec_va.y_val;
         (tupek+i*nv+j)->z_gm = val.lit.vec_va.z_val;
         }
       if ( geop_pc > 5 )
         {
         ingval(tvadr+(i*nv_dekl+j)*vecsiz,arrtyp.base_arr,FALSE,&val);
         (tvpek+i*nv+j)->x_gm = val.lit.vec_va.x_val;
         (tvpek+i*nv+j)->y_gm = val.lit.vec_va.y_val;
         (tvpek+i*nv+j)->z_gm = val.lit.vec_va.z_val;
         }
       if ( geop_pc > 6 )
         {
         ingval(twadr+(i*nv_dekl+j)*vecsiz,arrtyp.base_arr,FALSE,&val);
         (twpek+i*nv+j)->x_gm = val.lit.vec_va.x_val;
         (twpek+i*nv+j)->y_gm = val.lit.vec_va.y_val;
         (twpek+i*nv+j)->z_gm = val.lit.vec_va.z_val;
         }
       }
     } 
/*
***Anropa exe-rutinen.
*/
   status = EXssar(geop_id,geop_pv[1].par_va.lit.str_va,nu,nv,
                   ppek,tupek,tvpek,twpek,geop_np);
/*
***ärerlärna allokerat minne.
*/
   v3free(ppek,"evssar");
   if ( tupek != NULL ) v3free(tupek,"evssar");
   if ( tvpek != NULL ) v3free(tvpek,"evssar");
   if ( twpek != NULL ) v3free(twpek,"evssar");

   return(status);
 }

/********************************************************/
/*!******************************************************/

        short evscar()

/*      Evaluerar geometri-proceduren SUR_CONARR.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IN5202 = Antal kurvor < 2.
 *                IN5212 = Antal kurvor > deklarerad storlek.
 *                IN5222 = Kan ej allokera minne.
 *                IN5232 = Typ skall vara m/M/p/P.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 28/2/95 J. Kjellander
 *
 *      3/4/95  metod, J. Kjellander
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   char     errbuf[81];
   short    status;
   int      ncur,dekl_dim,refsiz,strsiz;
   DBint    i,*metpek;
   DBint    limadr,tanadr,metadr,mpadr;
   STTYTBL  typtbl;
   STARRTY  limtyp,tantyp,mettyp,mptyp;
   PMREFVA *lpek,*tpek,*mppek;
   PMLITVA  val;

/*
***Hur märga kurvor.
*/
   ncur = geop_pv[2].par_va.lit.int_va;
/*
***Kolla att antalet är rimligt.
*/
   if ( ncur < 2 ) return(erpush("IN5202",""));
/*
***Kolla att deklarerad dimension pärMBS-arrayer är tillrärklig fär
***ncur kurvor etc.
*/
   limadr = geop_pv[4].par_va.lit.adr_va;
   strtyp(geop_pv[4].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&limtyp);
   dekl_dim = limtyp.up_arr - limtyp.low_arr + 1;
   if ( dekl_dim < ncur ) return(erpush("IN5212",""));

   strtyp(limtyp.base_arr,&typtbl);
   refsiz = typtbl.size_ty;

   tanadr = geop_pv[5].par_va.lit.adr_va;
   strtyp(geop_pv[5].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&tantyp);
   dekl_dim = tantyp.up_arr - tantyp.low_arr + 1;
   if ( dekl_dim < ncur ) return(erpush("IN5212",""));

   metadr = geop_pv[6].par_va.lit.adr_va;
   strtyp(geop_pv[6].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&mettyp);
   dekl_dim = mettyp.up_arr - mettyp.low_arr + 1;
   if ( dekl_dim < ncur-1 ) return(erpush("IN5212",""));

   strtyp(mettyp.base_arr,&typtbl);
   strsiz = typtbl.size_ty;

   mpadr = geop_pv[7].par_va.lit.adr_va;
   strtyp(geop_pv[7].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&mptyp);
   dekl_dim = mptyp.up_arr - mptyp.low_arr + 1;
   if ( dekl_dim < ncur-1 ) return(erpush("IN5212",""));
/*
***Allokera minne.
*/
   if ( (lpek=(PMREFVA *)v3mall(ncur*sizeof(PMREFVA),"evscar")) == NULL )
     {
     sprintf(errbuf,"%d",ncur);
     return(erpush("IN5222",errbuf));
     }

   if ( (tpek=(PMREFVA *)v3mall(ncur*sizeof(PMREFVA),"evscar")) == NULL )
     {
     sprintf(errbuf,"%d",ncur);
     return(erpush("IN5222",errbuf));
     }

   if ( (metpek=(DBint *)v3mall(ncur*sizeof(gmint),"evscar")) == NULL )
     {
     sprintf(errbuf,"%d",ncur);
     return(erpush("IN5222",errbuf));
     }

   if ( (mppek=(PMREFVA *)v3mall(ncur*sizeof(PMREFVA),"evscar")) == NULL )
     {
     sprintf(errbuf,"%d",ncur);
     return(erpush("IN5222",errbuf));
     }
/*
***Kopiera frär RTS till allokerade areor.
*/
   for ( i=0; i<ncur; ++i )
     {
     ingval(limadr+i*refsiz,limtyp.base_arr,FALSE,&val);
     (lpek+i)->seq_val  = val.lit.ref_va[0].seq_val;
     (lpek+i)->ord_val  = val.lit.ref_va[0].ord_val;
     (lpek+i)->p_nextre = val.lit.ref_va[0].p_nextre;

     ingval(tanadr+i*refsiz,tantyp.base_arr,FALSE,&val);
     (tpek+i)->seq_val  = val.lit.ref_va[0].seq_val;
     (tpek+i)->ord_val  = val.lit.ref_va[0].ord_val;
     (tpek+i)->p_nextre = val.lit.ref_va[0].p_nextre;
     }

   for ( i=0; i<ncur-1; ++i )
     {
     ingval(metadr+i*strsiz,mettyp.base_arr,FALSE,&val);
     if ( sticmp(val.lit.str_va,"p") == 0 )
       {
      *(metpek+i) = 1;
       }
     else if ( sticmp(val.lit.str_va,"m") == 0 )
       {
      *(metpek+i) = 2;
       }
     else
       {
       status = erpush("IN5232","");
       goto exit;
       }
     ingval(mpadr+i*refsiz,mptyp.base_arr,FALSE,&val);
     (mppek+i)->seq_val  = val.lit.ref_va[0].seq_val;
     (mppek+i)->ord_val  = val.lit.ref_va[0].ord_val;
     (mppek+i)->p_nextre = val.lit.ref_va[0].p_nextre;
     }
/*
***Anropa exe-rutinen.
*/
   status = EXscar(geop_id,geop_pv[1].par_va.lit.str_va,
                          &geop_pv[3].par_va.lit.ref_va[0],
                          (DBint)ncur,
                           lpek,tpek,metpek,mppek,geop_np);
/*
***Deallokera minne.
*/
exit:
   v3free(lpek,"evscar");
   v3free(tpek,"evscar");
   v3free(metpek,"evscar");
   v3free(mppek,"evscar");

   return(status);
}

/********************************************************/
/*!******************************************************/

        short evsnar()

/*      Evaluerar geometri-proceduren SUR_NURBSARR.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IN5222 = Kan ej allokera minne.
 *                IN5372 = cpts/w felaktigt deklarerad
 *                IN5402 = cpts fel typ
 *                IN5382 = uknots/vknots fär liten
 *                IN5392 = w fär liten
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 1997-12-17 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   short     status;
   int       dekl_dim,dekl_dim_v1,dekl_dim_v2=0,i,j,
             fltsiz=0,vecsiz,np_u,np_v;
   DBint     order_u,order_v,nk_u,nk_v;
   DBint     posadr,ukadr=0,vkadr=0,wadr=0;
   DBfloat  *ukpek,*vkpek,*wpek;
   DBVector *cppek;
   STTYTBL   typtbl;
   STARRTY   arrtyp1,arrtyp2,arrtyp3,arrtyp4;
   PMLITVA   val;

/*
***Ordning i U och V.
*/
   order_u = geop_pv[1].par_va.lit.int_va;
   order_v = geop_pv[2].par_va.lit.int_va;
/*
***Antal knutpunker i U och V.
*/
   nk_u = geop_pv[3].par_va.lit.int_va;
   nk_v = geop_pv[4].par_va.lit.int_va;
/*
***Antal styrpunkter i U och V.
*/
   np_u = nk_u - order_u;
   np_v = nk_v - order_v;
/*
***Kolla att deklarerad dimension pärcpts är tillrärklig.
*/
   posadr = geop_pv[5].par_va.lit.adr_va;
   strtyp(geop_pv[5].par_ty,&typtbl);
   if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5372","no array"));
   strarr(typtbl.arr_ty,&arrtyp1);
   dekl_dim = arrtyp1.up_arr - arrtyp1.low_arr + 1;
   if ( dekl_dim < np_u ) return(erpush("IN5372","u-size too small"));

   strtyp(arrtyp1.base_arr,&typtbl);
   if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5372","1-dim array"));
   strarr(typtbl.arr_ty,&arrtyp1);
   dekl_dim_v1 = arrtyp1.up_arr - arrtyp1.low_arr + 1;
   if ( dekl_dim_v1 < np_v ) return(erpush("IN5372","v-size too small"));

   strtyp(arrtyp1.base_arr,&typtbl);
   if ( typtbl.kind_ty != ST_VEC ) return(erpush("IN5402",""));
   vecsiz = typtbl.size_ty;
/*
***Kolla att deklarerad dimension pärev. knots är ok.
*/
   if ( geop_pc > 5 )
     {
     ukadr = geop_pv[6].par_va.lit.adr_va;
     strtyp(geop_pv[6].par_ty,&typtbl);
     strarr(typtbl.arr_ty,&arrtyp2);
     dekl_dim = arrtyp2.up_arr - arrtyp2.low_arr + 1;
     if ( dekl_dim < nk_u ) return(erpush("IN5382",""));

     strtyp(arrtyp2.base_arr,&typtbl);
     fltsiz = typtbl.size_ty;
     }

   if ( geop_pc > 6 )
     {
     vkadr = geop_pv[7].par_va.lit.adr_va;
     strtyp(geop_pv[7].par_ty,&typtbl);
     strarr(typtbl.arr_ty,&arrtyp3);
     dekl_dim = arrtyp3.up_arr - arrtyp3.low_arr + 1;
     if ( dekl_dim < nk_v ) return(erpush("IN5382",""));
     }
/*
***Kolla typ och dimension pärev. w.
*/
   if ( geop_pc > 7 )
     {
     wadr = geop_pv[8].par_va.lit.adr_va;
     strtyp(geop_pv[8].par_ty,&typtbl);
     if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5372","no array"));
     strarr(typtbl.arr_ty,&arrtyp4);
     dekl_dim = arrtyp4.up_arr - arrtyp4.low_arr + 1;
     if ( dekl_dim < np_u ) return(erpush("IN5372","u-size too small"));

     strtyp(arrtyp4.base_arr,&typtbl);
     if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5372","1-dim array"));
     strarr(typtbl.arr_ty,&arrtyp4);
     dekl_dim_v2 = arrtyp4.up_arr - arrtyp4.low_arr + 1;
     if ( dekl_dim_v2 < np_v ) return(erpush("IN5372","v-size too small"));

     strtyp(arrtyp4.base_arr,&typtbl);
     if ( typtbl.kind_ty != ST_FLOAT ) return(erpush("IN5392",""));
     }
/*
***Allokera minne.
*/
   if ( (cppek=(DBVector *)v3mall(np_u*np_v*sizeof(DBVector),"evsnar")) == NULL )
     return(erpush("IN5412",""));

   if ( geop_pc > 5 )
     {
     if ( (ukpek=(DBfloat *)v3mall(nk_u*sizeof(gmflt),"evsnar")) == NULL )
       return(erpush("IN5412",""));
     }
   else ukpek = NULL;

   if ( geop_pc > 6 )
     {
     if ( (vkpek=(DBfloat *)v3mall(nk_v*sizeof(gmflt),"evsnar")) == NULL )
       return(erpush("IN5412",""));
     }
   else vkpek = NULL;

   if ( geop_pc > 7 )
     {
     if ( (wpek=(DBfloat *)v3mall(np_u*np_v*sizeof(gmflt),"evsnar")) == NULL )
       return(erpush("IN5412",""));
     }
   else wpek = NULL;
/*
***Kopiera frär RTS till allokerade areor.
*/
   for ( i=0; i<np_u; ++i )
     {
     for ( j=0; j<np_v; ++j )
       {
       ingval(posadr+(i*dekl_dim_v1+j)*vecsiz,arrtyp1.base_arr,FALSE,&val);
       (cppek+i*np_v+j)->x_gm = val.lit.vec_va.x_val;
       (cppek+i*np_v+j)->y_gm = val.lit.vec_va.y_val;
       (cppek+i*np_v+j)->z_gm = val.lit.vec_va.z_val;
       }
     } 

   if ( geop_pc > 5 )
     {
     for ( i=0; i<nk_u; ++i )
       {
       ingval(ukadr+i*fltsiz,arrtyp2.base_arr,FALSE,&val);
      *(ukpek+i) = val.lit.float_va;
       }
     }

   if ( geop_pc > 6 )
     {
     for ( i=0; i<nk_v; ++i )
       {
       ingval(vkadr+i*fltsiz,arrtyp3.base_arr,FALSE,&val);
      *(vkpek+i) = val.lit.float_va;
       }
     }

   if ( geop_pc > 7 )
     {
     for ( i=0; i<np_u; ++i )
       {
       for ( j=0; j<np_v; ++j )
         {
         ingval(wadr+(i*dekl_dim_v2+j)*fltsiz,arrtyp4.base_arr,FALSE,&val);
        *(wpek+i*np_v+j) = val.lit.float_va;
         }
       } 
     }
/*
***Anropa exe-rutinen.
*/
   status = EXsnar(geop_id,geop_pv[1].par_va.lit.int_va,
                           geop_pv[2].par_va.lit.int_va,
                           geop_pv[3].par_va.lit.int_va,
                           geop_pv[4].par_va.lit.int_va,
                           cppek,ukpek,vkpek,wpek,geop_np);
/*
***Deallokera minne.
*/
   v3free(ukpek,"evsnar");
   v3free(vkpek,"evsnar");
   v3free(cppek,"evsnar");

   return(status);
}

/********************************************************/
/*!******************************************************/

        short evsapp()

/*      Evaluerar geometri-proceduren SUR_APPROX.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkkoder:
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
   return(EXsapp(geop_id,geop_pv[1].par_va.lit.ref_va,
                         geop_pv[2].par_va.lit.str_va,
                         geop_pv[3].par_va.lit.int_va,
                         geop_pv[4].par_va.lit.float_va,
                         tol_2,
                         geop_np));
}

/********************************************************/
/*!******************************************************/

        short evscom()

/*      Evaluerar geometri-proceduren SUR_COMP.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkkoder:
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 28/2/95 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   int     i;
   int     nref = 0;
   V2REFVA refarr[GMMXCO+1];

/*
***Skapa refarr.
*/
   for ( i=1; i<=geop_pc; i++ )
     {
     refarr[nref].seq_val  = geop_pv[i].par_va.lit.ref_va[0].seq_val;
     refarr[nref].ord_val  = geop_pv[i].par_va.lit.ref_va[0].ord_val;
     refarr[nref].p_nextre = geop_pv[i].par_va.lit.ref_va[0].p_nextre;
     nref++;
     }
/*
***Exekvera.
*/
   return(EXscom(geop_id,refarr,nref,geop_np));
}

/********************************************************/
/*!******************************************************/

        short evstrm()

/*      Evaluerar geometri-proceduren SUR_TRIM.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern  geop_pc   => Antal parametrar.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkkoder:
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 28/2/95 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
   int fall;

/*
***Optionell parameter fall.
*/
   if ( geop_pc == 6 ) fall = geop_pv[6].par_va.lit.int_va;
   else                fall = 1;
/*
***Exekvera.
*/
   return(EXstrm(geop_id,&geop_pv[1].par_va.lit.ref_va[0],
                          geop_pv[2].par_va.lit.float_va,
                          geop_pv[3].par_va.lit.float_va,
                          geop_pv[4].par_va.lit.float_va,
                          geop_pv[5].par_va.lit.float_va,
                         (DBint)fall,geop_np));
}

/********************************************************/
/*!******************************************************/

        short evsrot()

/*      Evaluerar geometri-proceduren SUR_ROT.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkkoder:
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 1996-12-10 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera.
*/
   return(EXsrot(geop_id,&geop_pv[1].par_va.lit.ref_va[0],
             (DBVector *)&geop_pv[2].par_va.lit.vec_va,
             (DBVector *)&geop_pv[3].par_va.lit.vec_va,
                          geop_pv[4].par_va.lit.float_va,
                          geop_pv[5].par_va.lit.float_va,
                          geop_pv[6].par_va.lit.int_va,
                          geop_pv[7].par_va.lit.int_va,
                          geop_np));
}

/********************************************************/
/*!******************************************************/

        short evsusr()

/*      Evaluerar geometri-proceduren SUR_USRDEF.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkkoder:
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 1997-03-27 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera.
*/
   return(EXsusr(geop_id,&geop_pv[1].par_va.lit.ref_va[0],
                          geop_pv[2].par_va.lit.str_va,
                          geop_pv[3].par_va.lit.str_va,
                          geop_pv[4].par_va.lit.str_va,
                          geop_np));
}

/********************************************************/
/*!******************************************************/

        short evsswp()

/*      Evaluerar geometri-proceduren SUR_SWEEP.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkkoder:
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 1997-03-27 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera.
*/
   return(EXsswp(geop_id,&geop_pv[1].par_va.lit.ref_va[0],
                         &geop_pv[2].par_va.lit.ref_va[0],
                          geop_pv[3].par_va.lit.int_va,
             (DBVector *)&geop_pv[4].par_va.lit.vec_va,
                          geop_np));
}

/********************************************************/
/*!******************************************************/

        short evscyl()

/*      Evaluerar geometri-proceduren SUR_CYL.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkkoder:
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 1997-03-27 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera.
*/
   return(EXscyl(geop_id,&geop_pv[1].par_va.lit.ref_va[0],
                          geop_pv[2].par_va.lit.float_va,
                          geop_pv[3].par_va.lit.int_va,
             (DBVector *)&geop_pv[4].par_va.lit.vec_va,
                          geop_np));
}

/********************************************************/
/*!******************************************************/

        short evsrul()

/*      Evaluerar geometri-proceduren SUR_CYL.
 *
 *      In: extern *geop_id   => Storhetens ID.
 *          extern *geop_pv   => Pekare till array med parametervärden.
 *          extern *geop_np   => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkkoder:
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 1997-03-27 J. Kjellander
 *
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 ******************************************************!*/

{
/*
***Exekvera.
*/
   return(EXsrul(geop_id,&geop_pv[1].par_va.lit.ref_va[0],
                         &geop_pv[2].par_va.lit.ref_va[0],
                          geop_pv[3].par_va.lit.int_va,
                          geop_pv[4].par_va.lit.int_va,
                          geop_np));
}

/********************************************************/
/*!******************************************************/

        short evscur()

/*      Evaluates the geometry procedure SUR_CURVES.
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
 *      (C)microform ab 2000-11-01 Gunnar Liden
 *
 *      2000-12-01 Bugfix (ingref()), J.Kjellander
 *      2001-02-11 In-Param utbytta till Globla variabler, R Svedin
 *
 *
 ******************************************************!*/

{
   int      ncur;           /* Number of input curves (V lines) */
   int      nulin;          /* Number of requested U lines      */
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

/*
***Requested number of U curves in the output surface
*/
   nulin = geop_pv[1].par_va.lit.int_va;
/*
***Check that the number of U lines reasonable
*/
/* Not yet impl.    if ( nulin < 2 ) return(erpush("IN5202","")); */

/*
***How many input V curves.
*/
   ncur = geop_pv[2].par_va.lit.int_va;
/*
***Check that number of input curves is reasonable
*/
   if ( ncur < 2 ) return(erpush("IN5202",""));
/*
***Check that the declared dimension of the MBS-array is sufficient for ncur
*/
/* Address to the variable                                */
   curadr = geop_pv[3].par_va.lit.adr_va;
/* Get type of variable, which is defined in a structure  */
   strtyp(geop_pv[3].par_ty,&typtbl);
/* Get type of array                                      */
   strarr(typtbl.arr_ty,&curtyp);
/* Get the declared size of the array                     */
   dekl_dim = curtyp.up_arr - curtyp.low_arr + 1;
/* Return with error if array not has sufficient size     */
   if ( dekl_dim < ncur ) return(erpush("IN5212",""));
/*
***Get size of ???
*/
   strtyp(curtyp.base_arr,&typtbl);
   refsiz = typtbl.size_ty;
/*
***Allocate memory.
*/
   if ( (lpek=(PMREFVA *)v3mall(ncur*sizeof(PMREFVA),"evscur")) == NULL )
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
   status = EXscur(geop_id,               /* Entity ID                    */
            (DBint)nulin,                 /* Requested number of U lines  */
            (DBint)ncur,                  /* Number of input curves       */
            lpek,                         /* Curve references             */
            geop_pv[4].par_va.lit.str_va, /* Twist method                 */
            geop_np);                     /* Name parameter block pointer */
/*
***Free memory
*/

   v3free(lpek,"evscur");


   return(status);
}
/********************************************************/






/*!******************************************************/

        short evstusr()

/*      Evaluates the geometry procedure SUR_TRIM_USRDEF.
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
 *      (c)Orebro University 2006-09-14  S. Larsson
 *
 *
 *
 ******************************************************!*/

{

   return(0); /* Not implemented yet */
}
/********************************************************/





