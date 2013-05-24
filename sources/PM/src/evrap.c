/**********************************************************************
*
*    evrap.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    evrpcc();      Evaluerar RPC_CLIENTCREATE
*    evrpcd();      Evaluerar RPC_CLIENTDESTROY
*    evrpof();      Evaluerar RAP_OUTFLOAT
*    evrpif();      Evaluerar RAP_INFLOAT
*    evrpoi();      Evaluerar RAP_OUTINT
*    evrpii();      Evaluerar RAP_ININT  
*    evrport();     Evaluerar RAP_OUTROBTARGET
*    evrpirt();     Evaluerar RAP_INROBTARGET
*    evrpos();      Evaluerar RAP_OUTSPEED
*    evrpis();      Evaluerar RAP_INSPEED
*    evrpora();     Evaluerar RAP_OUTROBTARR
*    evrpira();     Evaluerar RAP_INROBTARR
*    evrpofa();     Evaluerar RAP_OUTFLOATARR
*    evrpifa();     Evaluerar RAP_INFLOATARR
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
*    (C) 2004-09-10 Sören Larsson, Örebro University                             
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../EX/include/EX.h"
#include <string.h>

extern PMPARVA *func_pv;  /* Access structure for MBS routines */
extern short    func_pc;  /* Number of actual parameters */
extern PMLITVA *func_vp;  /* Pekare till resultat. */

#define CONF_DIM   4
#define ROT_DIM    4
#define EXTAX_DIM  6
#define SPEED_DIM  4

/*!******************************************************/

        short evrpcc()
 
/*      Interface-routine for RPC_CLIENTCREATE
 *
 *      In:  Global  func_pv  => Parameter value array
 *      Out: Global *func_vp  =  Pointer to result value.
 *      FV:  Return the status of called routines
 *
 *      (C) 2004-04-26 Sören Larsson, Örebro University
 *
 ******************************************************!*/

   {
   short   status;
   PMLITVA litval;
 
   status = EXrpcclntcr(func_pv[1].par_va.lit.str_va,
                    func_pv[2].par_va.lit.int_va,
                    func_pv[3].par_va.lit.int_va,
                   &litval.lit.int_va, 
                   &func_vp->lit.int_va);
   
   if ( status < 0 ) return(status);
   /*
   ***Write to MBS-variable 'client'.
   */
   return(inwvar(func_pv[4].par_ty,func_pv[4].par_va.lit.adr_va,0,NULL,&litval));
   }



/*!******************************************************/

        short evrpcd()

/*      Interface-routine for RPC_CLIENTDESTROY
 *
 *      In:  Global  func_pv  => Parameter value array
 *      Out: Global *func_vp  =  Pointer to result value.
 *      FV:  Return the status of called routines
 *
 *      (C) 2004-04-26 Sören Larsson, Örebro University
 *
 ******************************************************!*/

   {
   return(EXrpcclntde(func_pv[1].par_va.lit.int_va,
                    &func_vp->lit.int_va));
   }


/*!******************************************************/

        short evrpof()

/*      Interface-routine for RAP_OUTFLOAT
 *
 *      In:  Global  func_pv  => Parameter value array
 *      Out: Global *func_vp  =  Pointer to result value.
 *      FV:  Return the status of called routines
 *
 *      (C) 2004-04-26 Sören Larsson, Örebro University
 *
 ******************************************************!*/

   {
   return(EXrapoutfloat(func_pv[1].par_va.lit.int_va,
                    func_pv[2].par_va.lit.str_va,
                    func_pv[3].par_va.lit.float_va,
                   &func_vp->lit.int_va));
   }



/*!******************************************************/

        short evrpif()

/*      Interface-routine for RAP_INFLOAT
 *
 *      In:  Global  func_pv  => Parameter value array
 *      Out: Global *func_vp  =  Pointer to result value.
 *      FV:  Return the status of called routines
 *
 *      (C) 2004-04-26 Sören Larsson, Örebro University
 *
 ******************************************************!*/

   {
   short   status;
   PMLITVA litval;

   status = EXrapinfloat(func_pv[1].par_va.lit.int_va,
                    func_pv[2].par_va.lit.str_va,
                   &litval.lit.float_va,
                   &func_vp->lit.int_va);

   if ( status < 0 ) return(status);
   /*
   ***Write to MBS-variable 'data'.
   */
   return(inwvar(func_pv[3].par_ty,func_pv[3].par_va.lit.adr_va,0,NULL,&litval));
   }



/*!******************************************************/

        short evrpoi()

/*      Interface-routine for RAP_OUTINT
 *
 *      In:  Global  func_pv  => Parameter value array
 *      Out: Global *func_vp  =  Pointer to result value.
 *      FV:  Return the status of called routines
 *
 *      (C) 2004-04-26 Sören Larsson, Örebro University
 *
 ******************************************************!*/
  
   {
   return(EXrapoutint(func_pv[1].par_va.lit.int_va,
                    func_pv[2].par_va.lit.str_va,
                    func_pv[3].par_va.lit.int_va,
                   &func_vp->lit.int_va));
   }
 
   


/*!******************************************************/

        short evrpii()

/*      Interface-routine for RAP_ININT
 *
 *      In:  Global  func_pv  => Parameter value array
 *      Out: Global *func_vp  =  Pointer to result value.
 *      FV:  Return the status of called routines
 *
 *      (C) 2004-04-26 Sören Larsson, Örebro University
 *
 ******************************************************!*/

   {
   short   status;
   PMLITVA litval;

   status = EXrapinint(func_pv[1].par_va.lit.int_va,
                    func_pv[2].par_va.lit.str_va,
                   &litval.lit.int_va,
                   &func_vp->lit.int_va);

   if ( status < 0 ) return(status);
   /*
   ***Write to MBS-variable 'data'.
   */
   return(inwvar(func_pv[3].par_ty,func_pv[3].par_va.lit.adr_va,0,NULL,&litval));
   }



/*!******************************************************/

        short evrport()

/*      Interface-routine for RAP_OUTROBTARGET
 *
 *      In:  Global  func_pv  => Parameter value array
 *      Out: Global *func_vp  =  Pointer to result value.
 *      FV:  Return the status of called routines
 *
 *      (C) 2004-04-26 Sören Larsson, Örebro University
 *
 ******************************************************!*/

   {
   short   status;
   int       dekl_dim,i;
   DBfloat   rot[ROT_DIM], extax[EXTAX_DIM];
   DBint     conf[CONF_DIM];
   DBint     rotadr,confadr,extaxadr = 0;
   STTYTBL   typtbl;
   STARRTY   rottyp,conftyp,extaxtyp;
   PMLITVA   litval;
         
/*
***Check declared dimension of rot.
*/
   rotadr = func_pv[4].par_va.lit.adr_va;
   strtyp(func_pv[4].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&rottyp);
   dekl_dim = rottyp.up_arr - rottyp.low_arr + 1;
   if ( dekl_dim < ROT_DIM ) return(erpush("IN5642",""));
   /*strtyp(rottyp.base_arr,&typtbl);*/
 
   
/*
***Check declared dimension of conf.
*/
   confadr = func_pv[5].par_va.lit.adr_va;
   strtyp(func_pv[5].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&conftyp);
   dekl_dim = conftyp.up_arr - conftyp.low_arr + 1;
   if ( dekl_dim < CONF_DIM ) return(erpush("IN5642",""));
   /*strtyp(conftyp.base_arr,&typtbl);*/
   
/*
***Check declared dimension of extax.
*/
   extaxadr = func_pv[6].par_va.lit.adr_va;
   strtyp(func_pv[6].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&extaxtyp);
   dekl_dim = extaxtyp.up_arr - extaxtyp.low_arr + 1;
   if ( dekl_dim < EXTAX_DIM ) return(erpush("IN5642",""));
   strtyp(extaxtyp.base_arr,&typtbl);
   /*strtyp(conftyp.base_arr,&typtbl);*/

/*
***Copy from RTS to allocated area.
*/
   for ( i=0; i< ROT_DIM; ++i )
      {
      ingval(rotadr+i*sizeof(DBfloat),rottyp.base_arr,FALSE,&litval);
      rot[i] = litval.lit.float_va;
      }

   for ( i=0; i< CONF_DIM; ++i )
      {
      ingval(confadr+i*sizeof(DBint),conftyp.base_arr,FALSE,&litval);
      conf[i] = litval.lit.int_va;
      }

   for ( i=0; i< EXTAX_DIM; ++i )      
      {
      ingval(extaxadr+i*sizeof(DBfloat),extaxtyp.base_arr,FALSE,&litval);
      extax[i] = litval.lit.float_va;
      }   

/*
***Call executer.
*/
   status = EXrapoutrobtarget(func_pv[1].par_va.lit.int_va,
                              func_pv[2].par_va.lit.str_va,
                 (DBVector *)&func_pv[3].par_va.lit.vec_va,
                              rot,
                              conf,
                              extax,                              
                              &func_vp->lit.int_va);
   return(status);
   }


   
/*!******************************************************/

        short evrpirt()

/*      Interface-routine for RAP_INROBTARGET
 *
 *      In:  Global  func_pv  => Parameter value array
 *      Out: Global *func_vp  =  Pointer to result value.
 *      FV:  Return the status of called routines
 *
 *      (C) 2004-04-27 Sören Larsson, Örebro University
 *
 ******************************************************!*/

   {
   short     status;
   int       dekl_dim;
   DBfloat   rot[ROT_DIM], extax[EXTAX_DIM];
   DBint     conf[CONF_DIM];
   DBint     rotadr,confadr,extaxadr = 0;
   STTYTBL   typtbl;
   STARRTY   rottyp,conftyp,extaxtyp;
   PMLITVA   trans_litval;
   
/*
***Check declared dimension of rot.
*/
   rotadr = func_pv[4].par_va.lit.adr_va;
   strtyp(func_pv[4].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&rottyp);
   dekl_dim = rottyp.up_arr - rottyp.low_arr + 1;
   if ( dekl_dim < ROT_DIM ) return(erpush("IN5642",""));

/*
***Check declared dimension of conf.
*/
   confadr = func_pv[5].par_va.lit.adr_va;
   strtyp(func_pv[5].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&conftyp);
   dekl_dim = conftyp.up_arr - conftyp.low_arr + 1;
   if ( dekl_dim < CONF_DIM ) return(erpush("IN5642",""));

/*
***Check declared dimension of extax.
*/
   extaxadr = func_pv[6].par_va.lit.adr_va;
   strtyp(func_pv[6].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&extaxtyp);
   dekl_dim = extaxtyp.up_arr - extaxtyp.low_arr + 1;
   if ( dekl_dim < EXTAX_DIM ) return(erpush("IN5642",""));


/*
***Call executer.
*/
   status = EXrapinrobtarget(func_pv[1].par_va.lit.int_va,
                              func_pv[2].par_va.lit.str_va,
                 (DBVector *)&trans_litval.lit.vec_va,
                              rot,
                              conf,
                              extax,
                              &func_vp->lit.int_va);
/*
***Copy from allocated area to RTS.
*/
   status = inwvar(func_pv[3].par_ty, func_pv[3].par_va.lit.adr_va,0,NULL,&trans_litval);
   status = evwfvk(rot  , ROT_DIM   ,4 , func_pv);
   status = evwivk(conf , CONF_DIM  ,5 , func_pv);
   status = evwfvk(extax, EXTAX_DIM ,6 , func_pv);

   return(status);
   }


 
/*!******************************************************/

        short evrpos()

/*      Interface-routine for RAP_OUTSPEED
 *
 *      In:  Global  func_pv  => Parameter value array
 *      Out: Global *func_vp  =  Pointer to result value.
 *      FV:  Return the status of called routines
 *
 *      (C) 2004-04-27 Sören Larsson, Örebro University
 *
 ******************************************************!*/

   {
   short   status;
   int       dekl_dim,i;
   DBfloat   speed[SPEED_DIM];
   DBint     speedadr = 0;
   STTYTBL   typtbl;
   STARRTY   speedtyp;
   PMLITVA   litval;
   
/*
***Check declared dimension of speed.
*/
   speedadr = func_pv[3].par_va.lit.adr_va;
   strtyp(func_pv[3].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&speedtyp);
   dekl_dim = speedtyp.up_arr - speedtyp.low_arr + 1;
   if ( dekl_dim < SPEED_DIM ) return(erpush("IN5642",""));

/*
***Copy from RTS to speed array.
*/
   for ( i=0; i< SPEED_DIM; ++i )
      {
      ingval(speedadr+i*sizeof(DBfloat),speedtyp.base_arr,FALSE,&litval);
      speed[i] = litval.lit.float_va;
      }
/*
***Call executer.
*/
   status = EXrapoutspeed(func_pv[1].par_va.lit.int_va,
                              func_pv[2].par_va.lit.str_va,
                              speed,
                              &func_vp->lit.int_va);
   return(status);
   }



/*!******************************************************/

        short evrpis()

/*      Interface-routine for RAP_INSPEED
 *
 *      In:  Global  func_pv  => Parameter value array
 *      Out: Global *func_vp  =  Pointer to result value.
 *      FV:  Return the status of called routines
 *
 *      (C) 2004-04-27 Sören Larsson, Örebro University
 *
 ******************************************************!*/
   {
   short     status;
   int       dekl_dim;
   DBfloat   speed[SPEED_DIM];
   DBint     speedadr = 0;
   STTYTBL   typtbl;
   STARRTY   speedtyp;
/*
***Check declared dimension of speed.
*/
   speedadr = func_pv[3].par_va.lit.adr_va;
   strtyp(func_pv[3].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&speedtyp);
   dekl_dim = speedtyp.up_arr - speedtyp.low_arr + 1;
   if ( dekl_dim < SPEED_DIM ) return(erpush("IN5642",""));
/*
***Call executer.
*/
   status = EXrapinspeed(func_pv[1].par_va.lit.int_va,
                              func_pv[2].par_va.lit.str_va,
                              speed,
                              &func_vp->lit.int_va);
/*
***Copy from speed array to RTS.
*/
   status = evwfvk(speed, SPEED_DIM ,3 , func_pv);

   return(status);
   }
 


/*!******************************************************/

        short evrpora()

/*      Interface-routine for RAP_OUTROBTARR
 *
 *      In:  Global  func_pv  => Parameter value array
 *      Out: Global *func_vp  =  Pointer to result value.
 *      FV:  Return the status of called routines
 *
 *      (C) 2004-04-27 Sören Larsson, Örebro University
 *
 ******************************************************!*/

   {
   short   status;
   int       dekl_dim,i;
   DBfloat   *rotpek, *extaxpek;
   DBint     *confpek; 
   DBVector  *transpek;
   DBint     transadr,rotadr,confadr,extaxadr = 0;
   STTYTBL   typtbl;
   STARRTY   transtyp,rottyp,conftyp,extaxtyp;
   PMLITVA   litval;
   DBint     first,last,asize;
/*
***size of array
*/
   first=func_pv[3].par_va.lit.int_va;
   last =func_pv[4].par_va.lit.int_va;
   asize=last-first+1;
   if ( asize > 500 ) return(erpush("IN5652","")); 
/*
***Check declared dimension of trans.
*/
   transadr = func_pv[5].par_va.lit.adr_va;
   strtyp(func_pv[5].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&transtyp);
   dekl_dim = transtyp.up_arr - transtyp.low_arr + 1;
   if ( dekl_dim < asize ) return(erpush("IN5642",""));
/*
***Check declared dimension of rot.
*/
   rotadr = func_pv[6].par_va.lit.adr_va;
   strtyp(func_pv[6].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&rottyp);
   dekl_dim = rottyp.up_arr - rottyp.low_arr + 1;
   if ( dekl_dim < asize*ROT_DIM ) return(erpush("IN5642",""));
/*
***Check declared dimension of conf.
*/
   confadr = func_pv[7].par_va.lit.adr_va;
   strtyp(func_pv[7].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&conftyp);
   dekl_dim = conftyp.up_arr - conftyp.low_arr + 1;
   if ( dekl_dim < asize*CONF_DIM ) return(erpush("IN5642",""));
/*
***Check declared dimension of extax.
*/
   extaxadr = func_pv[8].par_va.lit.adr_va;
   strtyp(func_pv[8].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&extaxtyp);
   dekl_dim = extaxtyp.up_arr - extaxtyp.low_arr + 1;
   if ( dekl_dim < asize*EXTAX_DIM ) return(erpush("IN5642",""));
/*
***Allocate memory.
*/
   if ( (transpek=(DBVector *)v3mall(asize*sizeof(DBVector),"evrporta")) == NULL )
      return(erpush("IN5632",""));

   if ( (rotpek=(DBfloat *)v3mall(asize*ROT_DIM*sizeof(DBfloat),"evrporta")) == NULL )
      return(erpush("IN5632",""));

   if ( (confpek=(DBint *)v3mall(asize*CONF_DIM*sizeof(DBint),"evrporta")) == NULL )
      return(erpush("IN5632",""));

   if ( (extaxpek=(DBfloat *)v3mall(asize*EXTAX_DIM*sizeof(DBfloat),"evrporta")) == NULL )
      return(erpush("IN5632",""));

/*
***Copy from RTS to allocated area.
*/
   for ( i=0; i< asize; ++i )
      {
      ingval(transadr+i*sizeof(DBVector),transtyp.base_arr,FALSE,&litval);
      (transpek+i)->x_gm = litval.lit.vec_va.x_val;
      (transpek+i)->y_gm = litval.lit.vec_va.y_val;
      (transpek+i)->z_gm = litval.lit.vec_va.z_val;
      }
   for ( i=0; i< ROT_DIM*asize; ++i )
      {
      ingval(rotadr+i*sizeof(DBfloat),rottyp.base_arr,FALSE,&litval);
      *(rotpek+i) = litval.lit.float_va;
      }
   for ( i=0; i< CONF_DIM*asize; ++i )
      {
      ingval(confadr+i*sizeof(DBint),conftyp.base_arr,FALSE,&litval);
      *(confpek+i) = litval.lit.int_va;
      }
   for ( i=0; i< EXTAX_DIM*asize; ++i )
      {
      ingval(extaxadr+i*sizeof(DBfloat),extaxtyp.base_arr,FALSE,&litval);
      *(extaxpek+i) = litval.lit.float_va;
      }
/*
***Call executer.
*/
   status = EXrapoutrobtarr(func_pv[1].par_va.lit.int_va,
                            func_pv[2].par_va.lit.str_va,
                            first,
                            last,
                            transpek,
                            rotpek,
                            confpek,
                            extaxpek,
                            &func_vp->lit.int_va);
/*
***Deallocate memory.
*/
   v3free(transpek,"evrporta");
   v3free(rotpek,"evrporta");
   v3free(confpek,"evrporta");
   v3free(extaxpek,"evrporta");

   return(status);
   }



/*!******************************************************/

        short evrpira()

/*      Interface-routine for RAP_INROBTARR
 *
 *      In:  Global  func_pv  => Parameter value array
 *      Out: Global *func_vp  =  Pointer to result value.
 *      FV:  Return the status of called routines
 *
 *      (C) 2004-04-27 Sören Larsson, Örebro University
 *
 ******************************************************!*/

   {
   short   status;
   int       dekl_dim;
   DBfloat   *rotpek, *extaxpek;
   DBint     *confpek;
   DBVector  *transpek;
   DBint     transadr,rotadr,confadr,extaxadr = 0;
   STTYTBL   typtbl;
   STARRTY   transtyp,rottyp,conftyp,extaxtyp;
   DBint     first,last,asize;


   
/*
***size of array
*/
   first=func_pv[3].par_va.lit.int_va;
   last=func_pv[4].par_va.lit.int_va;
   asize=last-first+1;
   if ( asize > 500 ) return(erpush("IN5652",""));   
/*
***Check declared dimension of trans.
*/
   transadr = func_pv[5].par_va.lit.adr_va;
   strtyp(func_pv[5].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&transtyp);
   dekl_dim = transtyp.up_arr - transtyp.low_arr + 1;
   if ( dekl_dim < asize ) return(erpush("IN5642",""));
/*
***Check declared dimension of rot.
*/
   rotadr = func_pv[6].par_va.lit.adr_va;
   strtyp(func_pv[6].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&rottyp);
   dekl_dim = rottyp.up_arr - rottyp.low_arr + 1;
   if ( dekl_dim < asize*ROT_DIM ) return(erpush("IN5642",""));
/*
***Check declared dimension of conf.
*/
   confadr = func_pv[7].par_va.lit.adr_va;
   strtyp(func_pv[7].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&conftyp);
   dekl_dim = conftyp.up_arr - conftyp.low_arr + 1;
   if ( dekl_dim < asize*CONF_DIM ) return(erpush("IN5642",""));
/*
***Check declared dimension of extax.
*/
   extaxadr = func_pv[8].par_va.lit.adr_va;
   strtyp(func_pv[8].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&extaxtyp);
   dekl_dim = extaxtyp.up_arr - extaxtyp.low_arr + 1;
   if ( dekl_dim < asize*EXTAX_DIM ) return(erpush("IN5642",""));
/*
***Allocate memory.
*/
   if ( (transpek=(DBVector *)v3mall(asize*sizeof(DBVector),"evrporta")) == NULL )
      return(erpush("IN5632",""));

   if ( (rotpek=(DBfloat *)v3mall(asize*ROT_DIM*sizeof(DBfloat),"evrporta")) == NULL )
      return(erpush("IN5632",""));

   if ( (confpek=(DBint *)v3mall(asize*CONF_DIM*sizeof(DBint),"evrporta")) == NULL )
      return(erpush("IN5632",""));

   if ( (extaxpek=(DBfloat *)v3mall(asize*EXTAX_DIM*sizeof(DBfloat),"evrporta")) == NULL )
      return(erpush("IN5632",""));
/*
***Call executer.
*/
   status = EXrapinrobtarr(func_pv[1].par_va.lit.int_va,
                           func_pv[2].par_va.lit.str_va,
                           first,
                           last,
                           transpek,
                           rotpek,
                           confpek,
                           extaxpek,
                           &func_vp->lit.int_va);
/*
***Copy from allocated area to RTS.
*/
   status = evwvec(transpek, asize           ,5 , func_pv);   
   status = evwfvk(rotpek  , asize*ROT_DIM   ,6 , func_pv);
   status = evwivk(confpek , asize*CONF_DIM  ,7 , func_pv);
   status = evwfvk(extaxpek, asize*EXTAX_DIM ,8 , func_pv);
/*
***Deallocate memory.
*/
   v3free(transpek,"evrpirta");
   v3free(rotpek,  "evrpirta");
   v3free(confpek, "evrpirta");
   v3free(extaxpek,"evrpirta");

   return(status);
   }

   

                                          
/*!******************************************************/

        short evrpofa()

/*      Interface-routine for RAP_OUTFLOATARR
 *
 *      In:  Global  func_pv  => Parameter value array
 *      Out: Global *func_vp  =  Pointer to result value.
 *      FV:  Return the status of called routines
 *
 *      (C) 2004-05-13 Sören Larsson, Örebro University
 *
 ******************************************************!*/

   {
   short     status;
   int       dekl_dim,i;
   DBfloat   *datapek;
   DBint     dataadr = 0;
   STTYTBL   typtbl;
   STARRTY   datatyp;
   PMLITVA   litval;
   DBint     first,last,asize;
/*
***size of array
*/
   first=func_pv[3].par_va.lit.int_va;
   last =func_pv[4].par_va.lit.int_va;
   asize=last-first+1;
   if ( asize > 500 ) return(erpush("IN5652",""));

/*
***Check declared dimension of data.
*/
   dataadr = func_pv[5].par_va.lit.adr_va;
   strtyp(func_pv[5].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&datatyp);
   dekl_dim = datatyp.up_arr - datatyp.low_arr + 1;
   if ( dekl_dim < asize) return(erpush("IN5642",""));

/*
***Allocate memory.
*/

   if ( (datapek=(DBfloat *)v3mall(asize*sizeof(DBfloat),"evrpofa")) == NULL )
      return(erpush("IN5632",""));


/*
***Copy from RTS to allocated area.
*/

   for ( i=0; i< asize; ++i )
      {
      ingval(dataadr+i*sizeof(DBfloat),datatyp.base_arr,FALSE,&litval);
      *(datapek+i) = litval.lit.float_va;
      }

/*
***Call executer.
*/
   status = EXrapoutfloatarr(func_pv[1].par_va.lit.int_va,
                            func_pv[2].par_va.lit.str_va,
                            first,
                            last,
                            datapek,
                            &func_vp->lit.int_va);
/*
***Deallocate memory.
*/
   v3free(datapek,"evrpofa");
   return(status);
   }



/*!******************************************************/

        short evrpifa()

/*      Interface-routine for RAP_INFLOATARR
 *
 *      In:  Global  func_pv  => Parameter value array
 *      Out: Global *func_vp  =  Pointer to result value.
 *      FV:  Return the status of called routines
 *
 *      (C) 2004-05-13 Sören Larsson, Örebro University
 *
 ******************************************************!*/

   {
   short   status;
   int       dekl_dim;
   DBfloat   *datapek;
   DBint     dataadr = 0;
   STTYTBL   typtbl;
   STARRTY   datatyp;
   DBint     first,last,asize;

/*
***size of array
*/
   first=func_pv[3].par_va.lit.int_va;
   last=func_pv[4].par_va.lit.int_va;
   asize=last-first+1;
   if ( asize > 500 ) return(erpush("IN5652",""));

/*
***Check declared dimension of data.
*/
   dataadr = func_pv[5].par_va.lit.adr_va;
   strtyp(func_pv[5].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&datatyp);
   dekl_dim = datatyp.up_arr - datatyp.low_arr + 1;
   if ( dekl_dim < asize ) return(erpush("IN5642",""));

/*
***Allocate memory.
*/
   if ((datapek=(DBfloat *)v3mall(asize*sizeof(DBfloat),"evrpofa")) == NULL )
      return(erpush("IN5632",""));

/*
***Call executer.
*/
   status = EXrapinfloatarr(func_pv[1].par_va.lit.int_va,
                            func_pv[2].par_va.lit.str_va,
                            first,
                            last,
                            datapek,
                            &func_vp->lit.int_va);
/*
***Copy from allocated area to RTS.
*/
   status = evwfvk(datapek  , asize   ,5 , func_pv);

   
/*
***Deallocate memory.
*/
   v3free(datapek,"evrpifa");

   return(status);
   }











                                                                                                                                    









































































































































































































































































































                                                                                                                                    









































































































































































































































































































             
