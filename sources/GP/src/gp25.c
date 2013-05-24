/**********************************************************************
*
*    gp25.c
*    ======
*
*    This file is part of the VARKON Graphics Library.
*    URL: http://www.varkon.com
*
*    gpgnpd();   Generate Plane data
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
#include "../include/GP.h"
#include <math.h>

/*
***Skapar GPBPL
*/
static short mk_gpbpl(GMBPL *bpl, GPBPL *plan);

/*
***Genererar ett plans 2D-box
*/
static void gen2db(GPBPL *pln);

/*
***Gör Z-sortering
*/
static void sortz(int np, GPBPL **ppek);


extern int     pblant,pblofs,np;
extern bool    gpgenz;
extern tbool   nivtb1[];
extern double  x[],y[],z[];
extern GPBPL  *pblpek[];
extern GPBPL **ppek;
extern VY      actvy;

/*!******************************************************/

        short gpgnpd()

/*      Går igenom GM och skapar GPBPL-plan. Ett för
 *      varje B_PLANE och många för varje FAC_SUR och MESH.
 * 
 *      Felkoder: GP0072 = Plandata kräver för många block
 *                GP0082 = Fel från malloc() för plandata
 *                GP0092 = Fel från malloc() för planpekare
 *
 *      (C)microform ab 23/1/89 J. Kjellander
 *
 *      13/3/92  Penna, J. Kjellander
 *      10/1/96  Ytor, J. Kjellander
 *      10/7/04  Mesh, J.Kjellander, Örebro university
 *
 ******************************************************!*/

 {
   short      status;
   int        i,j,edge_1,edge_2,edge_3,
              vertex_1,vertex_2,vertex_3;
   DBetype    type;
   DBptr      la;
   DBBplane   bpl;
   DBSurf     sur;
   DBMesh     mesh;
   GMPAT     *patpek,*toppat;
   GMPATF    *facpat;
   GPBPL      plan;

/*
***Div. initiering.
*/
   np = 0;
   pblant = 0;
   pblofs = 100;
   gpgenz = TRUE;
/*
***Allokera minne för plan-pekare. Max antal plan = MAXPLN.
*/
   if ( (ppek=(GPBPL **)v3mall(MAXPLN*sizeof(GPBPL *),"gpgnpd")) == NULL )
     return(erpush("GP0092",""));
/*
***Leta upp alla plan och ytor i GM.
*/
   DBget_pointer('F',NULL,&la,&type);

   while ( DBget_pointer('N',NULL,&la,&type ) == 0 )
     {
     switch ( type )
       {
/*
***Om det är ett B_PLANE som ligger på en tänd nivå skall det ritas.
***Detta kollas nu och lagras som blank TRUE/FALSE i GPBPL-posten.
***När all trådgeometri klippts och planen skall ritas testas
***plan.blank. Om TRUE ritas planet inte.
***Spara även pennummer så att planet kan ritas med rätt
***penna om det är synligt.
*/
       case BPLTYP:
       DBread_bplane(&bpl,la);
       if ( !bpl.hed_bp.blank )
         {
         plan.blank = nivtb1[bpl.hed_bp.level];
         plan.pen   = bpl.hed_bp.pen;
         if ( (status=mk_gpbpl(&bpl,&plan)) < 0 ) return(status);
         }
       break;
/*
***Om det är en yta med facetter behandlar vi dessa som
***B-plan.
*/
       case SURTYP:
       DBread_surface(&sur,la);
       if ( sur.typ_su == FAC_SUR  &&  !sur.hed_su.blank )
         {
         DBread_patches(&sur,&patpek);
         toppat = patpek;
         for ( i=0; i<sur.nu_su; ++i )
           {
           for ( j=0; j<sur.nv_su; ++j )
             {
             if ( toppat->styp_pat == FAC_PAT )
               {
               facpat = (GMPATF *)toppat->spek_c;
               plan.blank = nivtb1[sur.hed_su.level];
               plan.pen   = sur.hed_su.pen;
/*
***Antingen är facetten fyrsidig....
*/
               if ( !facpat->triangles )
                 {
                 V3MOME(&facpat->p1,&bpl.crd1_bp,sizeof(DBVector));
                 V3MOME(&facpat->p2,&bpl.crd2_bp,sizeof(DBVector));
                 V3MOME(&facpat->p3,&bpl.crd3_bp,sizeof(DBVector));
                 V3MOME(&facpat->p4,&bpl.crd4_bp,sizeof(DBVector));
                 if ( (status=mk_gpbpl(&bpl,&plan)) < 0 ) return(status);
                 }
/*
***eller också består den av två trianglar.
*/
               else
                 {
                 V3MOME(&facpat->p1,&bpl.crd1_bp,sizeof(DBVector));
                 V3MOME(&facpat->p2,&bpl.crd2_bp,sizeof(DBVector));
                 V3MOME(&facpat->p2,&bpl.crd3_bp,sizeof(DBVector));
                 V3MOME(&facpat->p3,&bpl.crd4_bp,sizeof(DBVector));
                 if ( (status=mk_gpbpl(&bpl,&plan)) < 0 ) return(status);
                 V3MOME(&facpat->p1,&bpl.crd1_bp,sizeof(DBVector));
                 V3MOME(&facpat->p3,&bpl.crd2_bp,sizeof(DBVector));
                 V3MOME(&facpat->p3,&bpl.crd3_bp,sizeof(DBVector));
                 V3MOME(&facpat->p4,&bpl.crd4_bp,sizeof(DBVector));
                 if ( (status=mk_gpbpl(&bpl,&plan)) < 0 ) return(status);
                 }
               }
             ++toppat;
             }
           }
         DBfree_patches(&sur,patpek);
         }
       break;
/*
***Mesh.
*/
       case MSHTYP:
       DBread_mesh(&mesh,la,MESH_ALL);
       if ( !mesh.hed_m.blank  &&  (mesh.font_m == 0 || mesh.font_m == 4) )
         {
         plan.blank = nivtb1[mesh.hed_m.level];
         plan.pen   = mesh.hed_m.pen;
/*
***Copy all (triangular) faces to DBBplane.
***TODO !!!! What if a face is not triangular ?
*/
         for ( i=0; i<mesh.nf_m; ++i )
           {
           edge_1 = mesh.pf_m[i].i_hedge;
           edge_2 = mesh.ph_m[edge_1].i_nhedge;
           edge_3 = mesh.ph_m[edge_2].i_nhedge;

           vertex_1 = mesh.ph_m[edge_1].i_evertex;
           vertex_2 = mesh.ph_m[edge_2].i_evertex;
           vertex_3 = mesh.ph_m[edge_3].i_evertex;

           V3MOME(&(mesh.pv_m[vertex_1].p),&bpl.crd1_bp,sizeof(DBVector));
           V3MOME(&(mesh.pv_m[vertex_2].p),&bpl.crd2_bp,sizeof(DBVector));
           V3MOME(&(mesh.pv_m[vertex_2].p),&bpl.crd3_bp,sizeof(DBVector));
           V3MOME(&(mesh.pv_m[vertex_3].p),&bpl.crd4_bp,sizeof(DBVector));
           if ( (status=mk_gpbpl(&bpl,&plan)) < 0 ) return(status);
           }
         }
       break;
       }
     }
/*
***Alla plan i GM är genomgångna, gör Z-sortering.
*/
   if ( np > 0 ) sortz(np,ppek);
/*
***Dumpa plan-data till fil.
*/
/*   pddump(); */
/*
****Slut.
*/
   if ( actvy.vydist == 0.0 ) gpgenz = FALSE;

   return(0);
 }

/********************************************************/
/*!******************************************************/

static  short  mk_gpbpl(
        GMBPL *bpl,
        GPBPL *plan)

/*      Processar ett GMBPL och skapar motsvarande
 *      GPBPL.
 * 
 *      (C)microform ab 10/1/96 J. Kjellander
 *
 ******************************************************!*/

 {
   int      k;
   double   d;
   DBVector p21,p41,prod;

/*
***Transformera till aktiv vy genom att anropa gpplbp.
*/
   k = -1;
   gpplbp(bpl,&k);
/*
***Beräkna normalvektor vecn(vprod(p2-p1,p4-p1)).
*/
   p21.x_gm = x[1] - x[0];
   p21.y_gm = y[1] - y[0];
   p21.z_gm = z[1] - z[0];

   p41.x_gm = x[3] - x[0];
   p41.y_gm = y[3] - y[0];
   p41.z_gm = z[3] - z[0];

   prod.x_gm = p21.y_gm*p41.z_gm - p21.z_gm*p41.y_gm;
   prod.y_gm = p21.z_gm*p41.x_gm - p21.x_gm*p41.z_gm;
   prod.z_gm = p21.x_gm*p41.y_gm - p21.y_gm*p41.x_gm;

   d = prod.x_gm*prod.x_gm + prod.y_gm*prod.y_gm + prod.z_gm*prod.z_gm;

   if ( d > DTOL )
     {
     d = 1.0/SQRT(d);
     plan->nv.x_gm = prod.x_gm * d;
     plan->nv.y_gm = prod.y_gm * d;
     plan->nv.z_gm = prod.z_gm * d;
     }
   else
     {
     plan->nv.x_gm = 0.0;
     plan->nv.y_gm = 0.0;
     plan->nv.z_gm = 0.0;
     }
/*
***Om planet är vänt mot betraktaren, beräkna även övriga konstanter.
***Lagra resultatet i en GPBPL-post.
*/
   if ( plan->nv.z_gm > DTOL )
     {
/*
*** 4 hörnpositioner.
*/
     plan->p1.x_gm = x[0]; plan->p1.y_gm = y[0]; plan->p1.z_gm = z[0];
     plan->p2.x_gm = x[1]; plan->p2.y_gm = y[1]; plan->p2.z_gm = z[1];
     plan->p3.x_gm = x[2]; plan->p3.y_gm = y[2]; plan->p3.z_gm = z[2];
     plan->p4.x_gm = x[3]; plan->p4.y_gm = y[3]; plan->p4.z_gm = z[3];
/*
***Min och Max i X- och Y-led samt Max i Z-led.
*/
     gen2db(plan);
/*
*** dx och dy för de fyra sidorna.
*/
     plan->dx1 = p21.x_gm;
     plan->dy1 = p21.y_gm;
     plan->dx2 = plan->p3.x_gm - plan->p2.x_gm;
     plan->dy2 = plan->p3.y_gm - plan->p2.y_gm;
     plan->dx3 = plan->p4.x_gm - plan->p3.x_gm;
     plan->dy3 = plan->p4.y_gm - plan->p3.y_gm;
     plan->dx4 = -p41.x_gm;
     plan->dy4 = -p41.y_gm;
/*
***Beräkna längden i kvadrat för de fyra sidorna.
***Om någon har längden 0 sätt k till -1.
***Annars, sätt k till 0 och beräkna  1/(verklig längd). 
*/
     plan->l1 = plan->dx1*plan->dx1 + plan->dy1*plan->dy1;
     if ( plan->l1 > DTOL ) { plan->l1 = 1.0/SQRT(plan->l1); plan->k1 = 0.0; }
     else { plan->l1 = 0.0; plan->k1 = -1.0; }
  
     plan->l2 = plan->dx2*plan->dx2 + plan->dy2*plan->dy2;
     if ( plan->l2 > DTOL ) { plan->l2 = 1.0/SQRT(plan->l2); plan->k2 = 0.0; }
     else { plan->l2 = 0.0; plan->k2 = -1.0; }

     plan->l3 = plan->dx3*plan->dx3 + plan->dy3*plan->dy3;
     if ( plan->l3 > DTOL ) { plan->l3 = 1.0/SQRT(plan->l3); plan->k3 = 0.0; }
     else { plan->l3 = 0.0; plan->k3 = -1.0; }

     plan->l4 = plan->dx4*plan->dx4 + plan->dy4*plan->dy4;
     if ( plan->l4 > DTOL ) { plan->l4 = 1.0/SQRT(plan->l4); plan->k4 = 0.0; }
     else { plan->l4 = 0.0; plan->k4 = -1.0; }
/*
***Plan-data för detta plan är nu klara. Allokera minne för plan-
***data och lagra.
*/
     if (  pblofs == 100 )
       {
       if ( pblant == MAXPBL ) 
         return(erpush("GP0072",""));
       if ( (pblpek[pblant]=(GPBPL *)v3mall(PBLSIZ,"gpgnpd")) == NULL )
         return(erpush("GP0082",""));
       else
         { ++pblant; pblofs = 0; }
       }
     *(ppek+np) = pblpek[pblant-1] + pblofs;
     V3MOME((char *)plan,(char *)*(ppek+np),sizeof(GPBPL));
     ++pblofs; ++np;
     }

  return(0);
 }

/********************************************************/
/*!******************************************************/

/*        static void pddump() */

/*      Skriver ut Plan-data till fil.
 * 
 *      (C)microform ab 29/1/89 J. Kjellander
 *
 ******************************************************!*/

/*
 {
   int i;
   int   ploffs,longnp,longbl;
   GPBPL plan;
   FILE *dat,*tmp;

***Öppna TMP- och DAT-fil.
   igplma("Skriver ut Plan-data till fil !",IG_MESS);
   tmp = fopen("pdata.TMP","w+");
   dat = fopen("pdata.DAT","w+");
***Skriv ut antal plan till DAT-filen.
   longnp = np;
   fwrite(&longnp,sizeof(int),1,dat);
***Skriv ut data till TMP-filen.
   ploffs = 0;

   for ( i=0; i<np; ++i )
     ä
     V3MOME(*(ppek+i),&plan,sizeof(GPBPL));

     if ( plan.blank ) longbl = 1;
     else longbl = 0;
     fwrite(&longbl,sizeof(int),1,tmp);

     fwrite(&plan.p1,sizeof(DBVector),1,tmp);
     fwrite(&plan.p2,sizeof(DBVector),1,tmp);
     fwrite(&plan.p3,sizeof(DBVector),1,tmp);
     fwrite(&plan.p4,sizeof(DBVector),1,tmp);
 
     fwrite(&plan.nv,sizeof(DBVector),1,tmp);
  
     fwrite(&plan.xmin,sizeof(double),1,tmp);
     fwrite(&plan.xmax,sizeof(double),1,tmp);
     fwrite(&plan.ymin,sizeof(double),1,tmp);
     fwrite(&plan.ymax,sizeof(double),1,tmp);

     fwrite(&plan.dx1,sizeof(double),1,tmp);
     fwrite(&plan.dy1,sizeof(double),1,tmp);
     fwrite(&plan.l1,sizeof(double),1,tmp);
     fwrite(&plan.k1,sizeof(double),1,tmp);
     fwrite(&plan.dx2,sizeof(double),1,tmp);
     fwrite(&plan.dy2,sizeof(double),1,tmp);
     fwrite(&plan.l2,sizeof(double),1,tmp);
     fwrite(&plan.k2,sizeof(double),1,tmp);
     fwrite(&plan.dx3,sizeof(double),1,tmp);
     fwrite(&plan.dy3,sizeof(double),1,tmp);
     fwrite(&plan.l3,sizeof(double),1,tmp);
     fwrite(&plan.k3,sizeof(double),1,tmp);
     fwrite(&plan.dx4,sizeof(double),1,tmp);
     fwrite(&plan.dy4,sizeof(double),1,tmp);
     fwrite(&plan.l4,sizeof(double),1,tmp);
     fwrite(&plan.k4,sizeof(double),1,tmp);
***Och data till DAT-filen.
     fwrite(&ploffs,sizeof(int),1,dat);
     fwrite(&plan.zmax,sizeof(double),1,dat);
     ploffs += 284;
     å
***Slut.
   fclose(tmp);
   fclose(dat);
   igrsma();

  return;
 }
*/
/********************************************************/
/*!******************************************************/

        static void gen2db(
        GPBPL *pln)

/*      Genererar ett plans 2D-box.
 *
 *      In: p = Pekare till plan.
 * 
 *      (C)microform ab 26/1/89 J. Kjellander
 *
 *      14/3/92 Tolerenser, J. Kjellander
 *
 ******************************************************!*/

 {
#define UTTOL 0.0015  /* Så mycket för stor som 2D-boxen görs */

/*
***Min och Max i X-led.
*/
   if ( pln->p1.x_gm < pln->p2.x_gm )
     {
     if ( pln->p1.x_gm < pln->p3.x_gm )
       {
       if ( pln->p1.x_gm < pln->p4.x_gm )
         {
/*
***p1 < p2,p3,p4.
*/
         pln->xmin = pln->p1.x_gm;
         if ( pln->p4.x_gm > pln->p3.x_gm )
           { if ( pln->p4.x_gm > pln->p2.x_gm ) pln->xmax = pln->p4.x_gm;
           else pln->xmax = pln->p2.x_gm; }
         else
           { if ( pln->p3.x_gm > pln->p2.x_gm ) pln->xmax = pln->p3.x_gm;
           else pln->xmax = pln->p2.x_gm; }
         }
       else
/*
***p4 < p1 < p2,p3.
*/
         { pln->xmin = pln->p4.x_gm;
         if ( pln->p2.x_gm > pln->p3.x_gm ) pln->xmax = pln->p2.x_gm;
         else pln->xmax = pln->p3.x_gm; }
       }
     else
       {
       if ( pln->p3.x_gm < pln->p4.x_gm )
/*
***p3 < p1 < p2 och p3 < p4.
*/
         { pln->xmin = pln->p3.x_gm;
         if ( pln->p2.x_gm > pln->p4.x_gm ) pln->xmax = pln->p2.x_gm;
         else pln->xmax = pln->p4.x_gm; }
       else
/*
***p4 < p3 < p2 < p1.
*/
         { pln->xmin = pln->p4.x_gm; pln->xmax = pln->p2.x_gm; }
       }
     }
   else
     {
/*
***p2 < p1,p3,p4.
*/
     if ( pln->p2.x_gm < pln->p3.x_gm )
       {
       if ( pln->p2.x_gm < pln->p4.x_gm )
         {
/*
***p2 < p1,p3,p4.
*/
         pln->xmin = pln->p2.x_gm;
         if ( pln->p4.x_gm > pln->p3.x_gm )
           { if ( pln->p4.x_gm > pln->p1.x_gm ) pln->xmax = pln->p4.x_gm;
           else pln->xmax = pln->p1.x_gm; }
         else
           { if ( pln->p3.x_gm > pln->p1.x_gm ) pln->xmax = pln->p3.x_gm;
           else pln->xmax = pln->p1.x_gm; }
         }
       else
/*
***p4 < p2 < p1,p3.
*/
         { pln->xmin = pln->p4.x_gm;
         if ( pln->p1.x_gm > pln->p3.x_gm ) pln->xmax = pln->p1.x_gm;
         else pln->xmax = pln->p3.x_gm; }
       }
     else
       {
       if ( pln->p3.x_gm < pln->p4.x_gm )
/*
***p3 < p2 < p1 och p3 < p4.
*/
         { pln->xmin = pln->p3.x_gm;
         if ( pln->p1.x_gm > pln->p4.x_gm ) pln->xmax = pln->p1.x_gm;
         else pln->xmax = pln->p4.x_gm; }
       else
/*
***p4 < p3 < p2 < p1.
*/
         { pln->xmin = pln->p4.x_gm; pln->xmax = pln->p1.x_gm; }
       }
     }
/*
***Min och Max i Y-led.
*/
   if ( pln->p1.y_gm < pln->p2.y_gm )
     {
     if ( pln->p1.y_gm < pln->p3.y_gm )
       {
       if ( pln->p1.y_gm < pln->p4.y_gm )
         {
/*
***p1 < p2,p3,p4.
*/
         pln->ymin = pln->p1.y_gm;
         if ( pln->p4.y_gm > pln->p3.y_gm )
           { if ( pln->p4.y_gm > pln->p2.y_gm ) pln->ymax = pln->p4.y_gm;
           else pln->ymax = pln->p2.y_gm; }
         else
           { if ( pln->p3.y_gm > pln->p2.y_gm ) pln->ymax = pln->p3.y_gm;
           else pln->ymax = pln->p2.y_gm; }
         }
       else
/*
***p4 < p1 < p2,p3.
*/
         { pln->ymin = pln->p4.y_gm;
         if ( pln->p2.y_gm > pln->p3.y_gm ) pln->ymax = pln->p2.y_gm;
         else pln->ymax = pln->p3.y_gm; }
       }
     else
       {
       if ( pln->p3.y_gm < pln->p4.y_gm )
/*
***p3 < p1 < p2 och p3 < p4.
*/
         { pln->ymin = pln->p3.y_gm;
         if ( pln->p2.y_gm > pln->p4.y_gm ) pln->ymax = pln->p2.y_gm;
         else pln->ymax = pln->p4.y_gm; }
       else
/*
***p4 < p3 < p2 < p1.
*/
         { pln->ymin = pln->p4.y_gm; pln->ymax = pln->p2.y_gm; }
       }
     }
   else
     {
/*
***p2 < p1,p3,p4.
*/
     if ( pln->p2.y_gm < pln->p3.y_gm )
       {
       if ( pln->p2.y_gm < pln->p4.y_gm )
         {
/*
***p2 < p1,p3,p4.
*/
         pln->ymin = pln->p2.y_gm;
         if ( pln->p4.y_gm > pln->p3.y_gm )
           { if ( pln->p4.y_gm > pln->p1.y_gm ) pln->ymax = pln->p4.y_gm;
           else pln->ymax = pln->p1.y_gm; }
         else
           { if ( pln->p3.y_gm > pln->p1.y_gm ) pln->ymax = pln->p3.y_gm;
           else pln->ymax = pln->p1.y_gm; }
         }
       else
/*
***p4 < p2 < p1,p3.
*/
         { pln->ymin = pln->p4.y_gm;
         if ( pln->p1.y_gm > pln->p3.y_gm ) pln->ymax = pln->p1.y_gm;
         else pln->ymax = pln->p3.y_gm; }
       }
     else
       {
       if ( pln->p3.y_gm < pln->p4.y_gm )
/*
***p3 < p2 < p1 och p3 < p4.
*/
         { pln->ymin = pln->p3.y_gm;
         if ( pln->p1.y_gm > pln->p4.y_gm ) pln->ymax = pln->p1.y_gm;
         else pln->ymax = pln->p4.y_gm; }
       else
/*
***p4 < p3 < p2 < p1.
*/
         { pln->ymin = pln->p4.y_gm; pln->ymax = pln->p1.y_gm; }
       }
     }
/*
***Bestäm planets Z-max.
*/
   if ( pln->p1.z_gm > pln->p2.z_gm )
     {
     if ( pln->p1.z_gm > pln->p3.z_gm )
       {
       if ( pln->p1.z_gm > pln->p4.z_gm ) pln->zmax = pln->p1.z_gm;
       else pln->zmax = pln->p4.z_gm;
       }
     else
       {
       if ( pln->p3.z_gm > pln->p4.z_gm ) pln->zmax = pln->p3.z_gm;
       else pln->zmax = pln->p4.z_gm;
       }
     }
   else
     {
     if ( pln->p2.z_gm > pln->p3.z_gm )
       {
       if ( pln->p2.z_gm > pln->p4.z_gm ) pln->zmax = pln->p2.z_gm;
       else pln->zmax = pln->p4.z_gm;
       }
     else
       {
       if ( pln->p3.z_gm > pln->p4.z_gm ) pln->zmax = pln->p3.z_gm;
       else pln->zmax = pln->p4.z_gm;
       }
     }
/*
***Gör planets 2D-box lite större. Detta för att inte linjer
***som råkar ligga exakt på planets 2D-boxrand skall klassas
***som synliga. Om 2D-boxranden är identisk med planets rand,
***tex. en rektangel med räta hörn innebär detta att testen
***mot 2D-boxen resulterar att linjen klassas som inuti 2D-boxen
***och får testas mot planet självt. Linjen kommer då att
***klassas som osynlig. Använd tolerensen 0.0015, samma som i
***clip().
*/
    pln->xmin -= UTTOL;  pln->xmax += UTTOL;  
    pln->ymin -= UTTOL;  pln->ymax += UTTOL;  

    return;

 }

/********************************************************/
/*!******************************************************/

        static void sortz(
        int      np,
        GPBPL  **ppek)

/*      Sorterar ppek i fallande Z-ordning.
 *
 *      In:   np    = Antal plan.
 *            ppek  = C-pekare till array av GPBPL-pekare.
 * 
 *      (C)microform ab 26/1/89 J. Kjellander
 *
 ******************************************************!*/

 {
   bool shift;
   double zmax;
   int    start,end,izmax,i;
   GPBPL *tpek;

   start = 0;
   end = np-1;

loop:
   shift = FALSE;
   zmax = (*(ppek+start))->zmax; izmax = start;

   for ( i=start; i<end; ++i )
     {
     if ( (*(ppek+i+1))->zmax > (*(ppek+i))->zmax )
       {
       shift = TRUE;
       tpek = *(ppek+i); *(ppek+i) = *(ppek+i+1); *(ppek+i+1) = tpek;
       if ( (*(ppek+i))->zmax > zmax )
         { zmax = (*(ppek+i))->zmax; izmax = i; }
       }
     }

   if ( shift )
     {
     tpek = *(ppek+izmax); *(ppek+izmax) = *(ppek+start);
    *(ppek+start) = tpek; ++start; --end; goto loop;
     }
 }

/********************************************************/
