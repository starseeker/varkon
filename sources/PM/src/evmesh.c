/**********************************************************************
*
*    evmesh.c
*    =======
*
*    This file is part of the VARKON PM Library.
*
*    This file includes the following routines:
*
*    evmsar();     Evaluates MESH_ARR
*    evgmsh();     DB-get for mesh header
*    evgvrt();     DB-get for vertex
*    evghdg();     DB-get for halfedge
*    evgface();    DB-get for face
*    evgxflt();    DB-get for extra float data
*    evsrms();     Sort mesh
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
*    (C) Johan Kjellander, Örebro university
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../EX/include/EX.h"

extern V2REFVA *geop_id;
extern PMPARVA *geop_pv;
extern PMPARVA *proc_pv;
extern short    geop_pc;
extern short    proc_pc;
extern V2NAPA  *geop_np;
extern pm_ptr   stintp,stflop,stvecp;

/*!******************************************************/

        short evmsar()

/*      Evaluates geometric procedure MESH_ARR.
 *
 *      In: extern *geop_id   => Mesh ID.
 *          extern *geop_pv   => Ptr to parameter list.
 *          extern *geop_np   => Ptr to attributes.
 *
 *      Returncodes: IN5662 = Illegal parameter value
 *                   IN5672 = Error in parameter declaration
 *                   IN5682 = Can't malloc
 *
 *      (C)2004-07-08 J. Kjellander, Örebro university
 *         2004-10-07 Extra floats, J.Kjellander
 *
 ******************************************************!*/

 {
   short     status;
   int       nv,nh,nf,nx,posadr,ehadr,evadr,ohadr,
             nxhadr,fadr,hadr,xfadr,dekl_dim,vecsiz,
             intsiz,fltsiz,i;
   STTYTBL   typtbl;
   STARRTY   arrtyp;
   DBMesh    mesh;
   PMLITVA   val;

/*
***Initialization.
*/
   status = 0;
/*
***How many vertices, halfedges and faces ?
*/
   nv = geop_pv[1].par_va.lit.int_va;
   nh = geop_pv[4].par_va.lit.int_va;
   nf = geop_pv[9].par_va.lit.int_va;
   nx = geop_pv[11].par_va.lit.int_va;
/*
***Check values.
*/
   if ( nv < 1 ) return(erpush("IN5662","vertex"));
   if ( nh < 0 ) return(erpush("IN5662","edges"));
   if ( nf < 0 ) return(erpush("IN5662","faces"));
   if ( nx < 0 ) return(erpush("IN5662","xfloats"));
/*
***Check declared size of "pos" parameter (vertex position).
*/
   posadr = geop_pv[2].par_va.lit.adr_va;
   strtyp(geop_pv[2].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&arrtyp);
   dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( dekl_dim < nv ) return(erpush("IN5672","vertices"));

   strtyp(arrtyp.base_arr,&typtbl);
   vecsiz = typtbl.size_ty;
/*
***Check declared type and size of "eh" parameter (emanating halfedge).
*/
   ehadr = geop_pv[3].par_va.lit.adr_va;
   strtyp(geop_pv[3].par_ty,&typtbl);
   if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5672","emanating halfedges"));
   strarr(typtbl.arr_ty,&arrtyp);
   dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( dekl_dim < nv ) return(erpush("IN5672","emanating halfedges"));
   if ( arrtyp.base_arr != stintp ) return(erpush("IN5672","emanating halfedges"));
/*
***Check declared type and size of "ev" parameter (end vertex).
*/
   evadr = geop_pv[5].par_va.lit.adr_va;
   strtyp(geop_pv[5].par_ty,&typtbl);
   if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5672","end vertices"));
   strarr(typtbl.arr_ty,&arrtyp);
   dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( dekl_dim < nh ) return(erpush("IN5672","end vertices"));
   if ( arrtyp.base_arr != stintp ) return(erpush("IN5672","end vertices"));
/*
***Check declared type and size of "oh" parameter (opposite halfedge).
*/
   ohadr = geop_pv[6].par_va.lit.adr_va;
   strtyp(geop_pv[6].par_ty,&typtbl);
   if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5672","opposite halfedges"));
   strarr(typtbl.arr_ty,&arrtyp);
   dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( dekl_dim < nh ) return(erpush("IN5672","opposite halfedge"));
   if ( arrtyp.base_arr != stintp ) return(erpush("IN5672","opposite halfedges"));
/*
***Check declared type and size of "nxh" parameter (next halfedge).
*/
   nxhadr = geop_pv[7].par_va.lit.adr_va;
   strtyp(geop_pv[7].par_ty,&typtbl);
   if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5672","next halfedges"));
   strarr(typtbl.arr_ty,&arrtyp);
   dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( dekl_dim < nh ) return(erpush("IN5672","next halfedges"));
   if ( arrtyp.base_arr != stintp ) return(erpush("IN5672","next halfedges"));
/*
***Check declared type and size of "f" parameter (parent face).
*/
   fadr = geop_pv[8].par_va.lit.adr_va;
   strtyp(geop_pv[8].par_ty,&typtbl);
   if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5672","parent faces"));
   strarr(typtbl.arr_ty,&arrtyp);
   dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( dekl_dim < nh ) return(erpush("IN5672","parent face"));
   if ( arrtyp.base_arr != stintp ) return(erpush("IN5672","parent faces"));
/*
***Check declared type and size of "h" parameter (halfedge).
*/
   hadr = geop_pv[10].par_va.lit.adr_va;
   strtyp(geop_pv[10].par_ty,&typtbl);
   if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5672","halfedges"));
   strarr(typtbl.arr_ty,&arrtyp);
   dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( dekl_dim < nf ) return(erpush("IN5672","halfedges"));
   if ( arrtyp.base_arr != stintp ) return(erpush("IN5672","halfedges"));

   strtyp(arrtyp.base_arr,&typtbl);
   intsiz = typtbl.size_ty;
/*
***Check declared type and size of "xf" parameter (extra float).
*/
   if ( nx > 0  &&  geop_pc == 12 )
     {
     xfadr = geop_pv[12].par_va.lit.adr_va;
     strtyp(geop_pv[12].par_ty,&typtbl);
     if ( typtbl.arr_ty == (pm_ptr)NULL ) return(erpush("IN5672","extra floats"));
     strarr(typtbl.arr_ty,&arrtyp);
     dekl_dim = arrtyp.up_arr - arrtyp.low_arr + 1;
     if ( dekl_dim < nx ) return(erpush("IN5672","extra floats"));
     if ( arrtyp.base_arr != stflop ) return(erpush("IN5672","extra floats"));

     strtyp(arrtyp.base_arr,&typtbl);
     fltsiz = typtbl.size_ty;
     }
/*
***Allocate memory for mesh data. Ususally we don't call DB from here
***or create DB entities but mesh data is usually large and doing the
***job here instead of in EX will save time.
*/
   mesh.nv_m = nv;
   mesh.nh_m = nh;
   mesh.nf_m = nf;
   mesh.nx_m = nx;

   if ( DBalloc_mesh(&mesh) < 0 ) return(erpush("IN5682",""));
/*
***Copy pos array from RTS to mesh. Note that MBS indexes start with 1
***but C indexes start with 0 so we need to subtract 1 from all indexes.
*/
   for ( i=0; i<nv; ++i )
     {
     ingval(posadr+i*vecsiz,stvecp,FALSE,&val);
     V3MOME(&val.lit.vec_va,&(mesh.pv_m[i].p),sizeof(DBVector));
     }
/*
***Copy eh array from RTS to mesh.
*/
   for ( i=0; i<nv; ++i )
     {
     ingval(ehadr+i*intsiz,stvecp,FALSE,&val);
     mesh.pv_m[i].i_ehedge = val.lit.int_va - 1;
     }
/*
***Copy ev array from RTS to mesh.
*/
   for ( i=0; i<nh; ++i )
     {
     ingval(evadr+i*intsiz,stvecp,FALSE,&val);
     mesh.ph_m[i].i_evertex = val.lit.int_va - 1;
     }
/*
***Copy oh array from RTS to mesh.
*/
   for ( i=0; i<nh; ++i )
     {
     ingval(ohadr+i*intsiz,stvecp,FALSE,&val);
     mesh.ph_m[i].i_ohedge = val.lit.int_va - 1;
     }
/*
***Copy nxh array from RTS to mesh.
*/
   for ( i=0; i<nh; ++i )
     {
     ingval(nxhadr+i*intsiz,stvecp,FALSE,&val);
     mesh.ph_m[i].i_nhedge = val.lit.int_va - 1;
     }
/*
***Copy f array from RTS to mesh.
*/
   for ( i=0; i<nh; ++i )
     {
     ingval(fadr+i*intsiz,stvecp,FALSE,&val);
     mesh.ph_m[i].i_pface = val.lit.int_va - 1;
     }
/*
***Copy h array from RTS to mesh.
*/
   for ( i=0; i<nf; ++i )
     {
     ingval(hadr+i*intsiz,stvecp,FALSE,&val);
     mesh.pf_m[i].i_hedge = val.lit.int_va - 1;
     }
/*
***Copy h array from RTS to mesh.
*/
   if ( nx > 0  &&  geop_pc == 12 )
     {
     for ( i=0; i<nx; ++i )
       {
       ingval(xfadr+i*fltsiz,stflop,FALSE,&val);
       mesh.px_m[i] = val.lit.float_va;
       }
     }
/*
***Execute.
*/
   status = EXmsar(geop_id,&mesh,geop_np);
/*
***Deallocate memory..
*/
   DBfree_mesh(&mesh);

   return(status);
 }

/********************************************************/
/*!******************************************************/

        short evgmsh()

/*      Evaluates procedure GETMSHH.
 *
 *      In: extern *proc_pv => Ptr to parameter list.
 *
 *      Returncodes: 0 = Ok.
 *
 *      (C)2004-07-15 J. Kjellander, Örebro university
 *
 ******************************************************!*/

 {
   DBstatus status;
   DBint    nv,nh,nf,font;
   DBVector pmin,pmax;
   PMLITVA  litval[6];

/*
***Get mesh header data from DB.
*/
   status = EXgmsh(proc_pv[1].par_va.lit.ref_va,
                  &nv,&nh,&nf,&pmin,&pmax,&font);
   if ( status < 0 ) return(status);
/*
***Return values..
*/
   litval[0].lit.int_va       = nv;
   litval[1].lit.int_va       = nh;
   litval[2].lit.int_va       = nf;
   litval[3].lit.vec_va.x_val = pmin.x_gm;
   litval[3].lit.vec_va.y_val = pmin.y_gm;
   litval[3].lit.vec_va.z_val = pmin.z_gm;
   litval[4].lit.vec_va.x_val = pmax.x_gm;
   litval[4].lit.vec_va.y_val = pmax.y_gm;
   litval[4].lit.vec_va.z_val = pmax.z_gm;
   litval[5].lit.int_va       = font;
   evwval(litval,6,proc_pv);

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evgvrt()

/*      Evaluates procedure GETVERTEX.
 *
 *      In: extern *proc_pv => Ptr to parameter list.
 *
 *      Returncodes: 0 = Ok.
 *
 *      (C)2004-07-15 J. Kjellander, Örebro university
 *
 ******************************************************!*/

 {
   DBstatus status;
   DBVector p;
   DBint    eh;
   PMLITVA  litval;

/*
***Get vertex data from DB.
*/
   status = EXgvrt(proc_pv[1].par_va.lit.ref_va,
                   proc_pv[2].par_va.lit.int_va,
                  &p,&eh);
   if ( status < 0 ) return(status);
/*
***Return values. Note that MBS index = C index +1.
*/
   litval.lit.vec_va.x_val = p.x_gm;
   litval.lit.vec_va.y_val = p.y_gm;
   litval.lit.vec_va.z_val = p.z_gm;
   inwvar(proc_pv[3].par_ty,proc_pv[3].par_va.lit.adr_va,0,NULL,&litval);

   litval.lit.int_va = eh + 1;
   inwvar(proc_pv[4].par_ty,proc_pv[4].par_va.lit.adr_va,0,NULL,&litval);

   return(status);
 }

/********************************************************/
/*!******************************************************/

        short evghdg()

/*      Evaluates procedure GETHEDGE.
 *
 *      In: extern *proc_pv => Ptr to parameter list.
 *
 *      Returncodes: 0 = Ok.
 *
 *      (C)2004-07-15 J. Kjellander, Örebro university
 *
 ******************************************************!*/

 {
   DBstatus status;
   DBint    ev,oh,nh,pf;
   PMLITVA  litval;

/*
***Get hedge data from DB.
*/
   status = EXghdg(proc_pv[1].par_va.lit.ref_va,
                   proc_pv[2].par_va.lit.int_va,
                  &ev,&oh,&nh,&pf);
   if ( status < 0 ) return(status);
/*
***Return values. Note that MBS index = C index +1.
*/
   litval.lit.int_va = ev + 1;
   inwvar(proc_pv[3].par_ty,proc_pv[3].par_va.lit.adr_va,0,NULL,&litval);

   litval.lit.int_va = oh + 1;
   inwvar(proc_pv[4].par_ty,proc_pv[4].par_va.lit.adr_va,0,NULL,&litval);

   litval.lit.int_va = nh + 1;
   inwvar(proc_pv[5].par_ty,proc_pv[5].par_va.lit.adr_va,0,NULL,&litval);

   litval.lit.int_va = pf + 1;
   inwvar(proc_pv[6].par_ty,proc_pv[6].par_va.lit.adr_va,0,NULL,&litval);

  
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evgface()

/*      Evaluates procedure GETFACE.
 *
 *      In: extern *proc_pv => Ptr to parameter list.
 *
 *      Returncodes: 0 = Ok.
 *
 *      (C)2004-07-15 J. Kjellander, Örebro university
 *
 ******************************************************!*/

 {
   DBstatus status;
   DBint    h;
   PMLITVA  litval;

/*
***Get face data from DB.
*/
   status = EXgface(proc_pv[1].par_va.lit.ref_va,
                    proc_pv[2].par_va.lit.int_va,
                   &h);
   if ( status < 0 ) return(status);
 /*
***Return value. Note that MBS index = C index +1.
*/
   litval.lit.int_va = h + 1;
   inwvar(proc_pv[3].par_ty,proc_pv[3].par_va.lit.adr_va,0,NULL,&litval);

 
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evgxflt()

/*      Evaluates procedure GETXFLOAT.
 *
 *      In: extern *proc_pv => Ptr to parameter list.
 *
 *      Returncodes: 0 = Ok.
 *
 *      (C)2004-10-07 J. Kjellander, Örebro university
 *
 ******************************************************!*/

 {
   DBstatus status;
   DBfloat  f;
   PMLITVA  litval;

/*
***Get extra float data from DB.
*/
   status = EXgxflt(proc_pv[1].par_va.lit.ref_va,
                    proc_pv[2].par_va.lit.int_va,
                   &f);
   if ( status < 0 ) return(status);
 /*
***Return value.
*/
   litval.lit.float_va = f;
   inwvar(proc_pv[3].par_ty,proc_pv[3].par_va.lit.adr_va,0,NULL,&litval);
 
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evsrms()

/*      Evaluates procedure SORT_MESH.
 *
 *      In: extern *proc_pv => Ptr to parameter list.
 *
 *      Returncodes: 0 = Ok.
 *
 *      (C)2004-07-30 J. Kjellander, Örebro university
 *
 ******************************************************!*/

 {
   DBstatus status;
   char    *dir,mode,errbuf[V3STRLEN];
   DBint    index[1];
   DBint   *intptr,*facelist,i,font,nv,nh,nf;
   DBfloat *min,*max;
   DBVector pmin,pmax;
   PMLITVA  litval;

/*
***Check direction mode parameter.
*/
   dir = proc_pv[2].par_va.lit.str_va;

   if      ( sticmp(dir,"X") == 0 ) mode = 'X';
   else if ( sticmp(dir,"Y") == 0 ) mode = 'Y';
   else if ( sticmp(dir,"Z") == 0 ) mode = 'Z';
   else return(erpush("IN5692",dir));
/*
***How many faces are there in the mesh ?
*/
   status = EXgmsh(proc_pv[1].par_va.lit.ref_va,
                  &nv,&nh,&nf,&pmin,&pmax,&font);
   if ( status < 0 ) return(status);
/*
***Allocate memory for nf face indexes in facelist.
*/
   if ( (facelist=(DBint *)v3mall(nf*sizeof(DBint),"evsrms")) == NULL )
     {
     sprintf(errbuf,"%d",nf);
     return(erpush("IN5702",errbuf));
     }
/*
***Allocate memory for min and max values.
*/
   if ( (min=(DBfloat *)v3mall(nf*sizeof(DBfloat),"evsrms")) == NULL )
     {
     sprintf(errbuf,"%d",nf);
     return(erpush("IN5702",errbuf));
     }

   if ( (max=(DBfloat *)v3mall(nf*sizeof(DBfloat),"evsrms")) == NULL )
     {
     sprintf(errbuf,"%d",nf);
     return(erpush("IN5702",errbuf));
     }
/*
***Execute,
*/
   status = EXsort_mesh(proc_pv[1].par_va.lit.ref_va,
                        mode,
                        facelist,
                        min,max);
/*
***Write output to RTS.
*/
   intptr = facelist;

     for ( i=0; i<nf; ++i )
       {
       index[0] = i+1;
       litval.lit.int_va = *(intptr+i);
       status = inwvar(proc_pv[3].par_ty,proc_pv[3].par_va.lit.adr_va,1,index,&litval);
       }

    evwfvk(min,nf,4,proc_pv);
    evwfvk(max,nf,5,proc_pv);
/*
***Free memory.
*/
   v3free(facelist,"evsrms");
   v3free(min,"evsrms");
   v3free(max,"evsrms");

   return(0);
 }

/********************************************************/

