/*!*******************************************************
*
*   exgeofun2.c
*   ===========
*
*    EXions();     Interface routine for INV_ON_SUR
*    EXeval();     Interface routine for EVAL
*    EXncui();     Interface routine for N_CUR_INT
*    EXails();     Interface routine for ALL_INT_LIN_SUR
*    EXailm();     Interface routine for ALL_INT_LIN_MSUR
*    EXaios();     Interface routine for ALL_INV_ON_SUR
*
*    This file is part of the VARKON Execute  Library.
*    URL:  http://www.varkon.com
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
*
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../include/EX.h"

#ifdef DEBUG
#include "../../IG/include/debug.h"
#endif

extern DBTmat *lsyspk,lklsyi;

static bool   hitable(BBOX *box, DBVector *p, DBVector *v);

/*!******************************************************/

       short EXions(
       DBId     *rid,
       DBVector *r3pos,
       DBint     kod,
       DBint     sol_no,
       DBVector *uv_start,
       DBVector *ut_pos)

/*      Exekverar INV_ON_SUR.
 *
 *      In: rid      => Pekare till identitet f|r refererad yta.
 *          r3pos    => Yttre punkt.
 *          kod      => Ber{kningsmetod.
 *          sol_no   => \nskad l|sning nummer
 *          uv_start => Optionell startpunkt. Z<0 => Hela ytan.
 *          ut_pos   => Pekare till resultat.
 *
 *      Ut: *ut_pos = UV-v{rde.
 *
 *      Felkod:      0 => Ok.
 *              EX1402 => Refererad storhet finns ej.
 *              EX1412 => Refererad storhet har fel typ.
 *
 *      (C)microform ab 27/4/49 J. Kjellander
 *
 *      1994-05-01 Anrop av sur160 tillagt, G. Liden
 *      1994-05-20 Debug av uv_start (fel f|r Johan att
 *                 }tg{rda, uv_start x & y alltid 0  , G. Liden
 *      1994-06-22 acc=3        Gunnar Liden
 *      1994-06-24 acc=2 Problem PBMAX f|r litet   Gunnar Liden
 *      1994-10-08 Parameter no_points till sur160 tillagt, G. Liden
 *      1998-02-25 erinit() added  G. Liden
 *      1999-12-18 sur160->varkon_sur_closestpt  G. Liden
 *
 ******************************************************!*/

  {
    short   status;
    DBptr   la;
    DBetype   typ;
    char    errbuf[V3STRLEN+1];
    DBSurf   sur;
    DBPatch  *ptpat;
    DBVector   c_pt;
    DBint   no_points; /* Totalt antal l|sningar                */

#ifdef DEBUG
    if ( dbglev(EXEPAC) == 1 )
      {
      fprintf(dbgfil(EXEPAC),
              "exe19*EXions (INV_ON_SUR) Enter r3pos %f %f %f \n",
              r3pos->x_gm,r3pos->y_gm,r3pos->z_gm);
      fprintf(dbgfil(EXEPAC),
              "exe19*EXions (INV_ON_SUR) Enter uv_start %f %f %f \n",
              uv_start->x_gm,uv_start->y_gm,uv_start->z_gm);
      fflush(dbgfil(EXEPAC));
      }
#endif

/*
***H{mta geometri-data f|r refererad yta.
*/
    if ( DBget_pointer('I',rid,&la,&typ) < 0 ) return(erpush("EX1402",""));
    if ( typ != SURTYP )
      {
      IGidst(rid,errbuf);
      return(erpush("EX1412",errbuf));
      }
    if ( (status=DBread_surface(&sur,la)) < 0 ) return(status);
    if ( (status=DBread_patches(&sur,&ptpat)) < 0 ) return(status);
/*
***Ber{kna.
***Computation accuracy
***Eq. 1: One   "layer"  of patches
***Eq. 2: Two   "layers" of patches
***Eq. 3: Three "layers" of patches
*** ... should be input data to INV_ON_SUR !!!!!!!
*** .... maybe as the tolerances .... (SET statement) ???
*/
    uv_start->x_gm += 1.0;
    uv_start->y_gm += 1.0;

    status= varkon_sur_closestpt(&sur,ptpat,r3pos,uv_start,kod,2,sol_no,
                      &no_points, &c_pt,ut_pos);
    if ( status >= 0 ) erinit();

/*
***Konvertera till MBS  globala parameter.
*/
    ut_pos->x_gm -= 1.0;
    ut_pos->y_gm -= 1.0;
/*
***Deallokera minne.
*/
    DBfree_patches(&sur,ptpat);
/*
***Slut.
*/
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short EXeval(
       DBId     *rid,
       char     *fall,
       DBVector *in_val,
       DBVector *ut_val)

/*      Exekverar EVAL.
 *
 *      In: rid      => Pekare till identitet för refererad kurva/yta.
 *          fall     => Önskad evaluering.
 *          in_val   => Pekare till parametervärde(n).
 *          ut_val   => Pekare till resultat.
 *
 *      Ut: *ut_val = V{rde.
 *
 *      Felkod:      0 => Ok.
 *              EX1402 => Refererad storhet finns ej.
 *              EX1412 => Refererad storhet har fel typ.
 *
 *      (C)microform ab 27/4/49 J. Kjellander
 *       1994-05-01  Anrop av sur215     Gunnar Liden
 *       1994-05-22  Anrop av sur216     Gunnar Liden
 *       1995-12-09  Ingen inl{sning av yta (sur215 anropar sur209) Gunnar Liden
 *       1998-02-25 erinit() added  G. Liden
 *       1999-12-18  sur215->varkon_sur_mbseval  
 *                   sur216->varkon_cur_mbseval  G Liden
 *
 ******************************************************!*/

  {
    short   status;
    DBptr   la;
    DBetype   typ;
    char    errbuf[V3STRLEN+1];
    DBSurf   sur;
    DBPatch  *ptpat;
    DBCurve   cur;
    DBSeg  *p_seg;

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe19*EXeval (EVAL) Enter fall %s in_val %f %f \n",
         fall,in_val->x_gm, in_val->y_gm);
 fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif

/*
***H{mta geometri-data f|r refererad storhet. Tills vidare
***bara ytor !!!!!!
*/
    if ( DBget_pointer('I',rid,&la,&typ) < 0 ) return(erpush("EX1402",""));

    if ( typ == CURTYP )
      {
      DBread_curve(&cur,NULL,&p_seg,la);            /* H{mta segment data */
      in_val->x_gm = in_val->x_gm + 1.0;      /* geopac's global p. */
      status=varkon_cur_mbseval(&cur,p_seg,fall, /* Evaluate curve  */
       in_val->x_gm,ut_val);                  /*                    */
      DBfree_segments(p_seg);                          /* Deallokera minne   */
      if ( status<0 )return(-1234);           /* Return for error   */
      goto fini;                              /* Exit               */
      }

    if ( typ != SURTYP )
      {
      IGidst(rid,errbuf);
      return(erpush("EX1412",errbuf));
      }
    if ( (status=DBread_surface(&sur,la)) < 0 ) return(status);
    if ( sur.typ_su == LFT_SUR ) /* I sur215 anropas sur236 som kr{ver ptpat */
      {
      if ( (status=DBread_patches(&sur,&ptpat)) < 0 ) return(status);
      }
      else ptpat= NULL;

/*
***Ber{kna.
*/

    /* Konvertera till surpac's och geopac's globala parameter  */
      in_val->x_gm =   in_val->x_gm + 1.0;
      in_val->y_gm =   in_val->y_gm + 1.0;

    status=varkon_sur_mbseval
             (&sur,ptpat,fall,in_val->x_gm,in_val->y_gm,ut_val);

    if ( status >= 0 ) erinit();

/*
***Deallokera minne.
*/
    if ( sur.typ_su == LFT_SUR )
      {
      DBfree_patches(&sur,ptpat);
      }
/*
***Slut.
*/

fini:;

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe19*EXeval ut_val %f %f %f\n",
         ut_val->x_gm, ut_val->y_gm, ut_val->z_gm);
 fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif

    return(status);

  }

/********************************************************/
/*!******************************************************/

       short EXncui(
       DBId  *rid1,
       DBId  *rid2,
       DBint *pnsect)

/*      Exekverar N_CUR_INT.
 *
 *      In: rid1   => Pekare till ID f|r yta 1.
 *          rid2   => Pekare till ID f|r yta 2.
 *          pnsect => Pekare till resultat.
 *
 *      Ut: *pnsect = Antal sk{rningskurvor.
 *
 *      Felkod:      0 => Ok.
 *              EX1402 => Refererad storhet finns ej.
 *              EX1412 => Refererad storhet har fel typ.
 *
 *      (C)microform ab 27/4/49 J. Kjellander
 *       1994-05-01  Anrop av sur102    Gunnar Liden
 *       1996-01-05  Anrop av sur104 och sur105   Gunnar Liden
 *       1996-05-25  DBptr indata sur102          Gunnar Liden
 *       1998-02-25 erinit() added  G. Liden
 *       1999-12-18  sur102->varkon_sur_interplane 
 *                   sur104->varkon_sur_intersurf
 *                   sur105->varkon_sur_planplan   G Liden
 *
 ******************************************************!*/

  {
    short   status;
    DBptr   la_sur;
    DBptr   la_pln;
    DBetype   typ;
    char    errbuf[V3STRLEN+1];
    DBSurf   sur;
    DBPatch  *ptpat;
    DBCsys   csy;
    DBSeg   *geoseg;       /* Allokerad minnesarea f|r segment (NULL) */
    DBCurve  cur;          /* Kurva                                   */
    DBint   no_branch;     /* Totalt antal kurvgrenar */
    DBVector origin;       /* Origo f|r sk{rningskoordinatsystem      */
    DBVector vecx;         /* X axel f|r sk{rningskoordinatsystem     */
    DBVector vecy;         /* Y axel f|r sk{rningskoordinatsystem     */
    DBTmat  isys;          /* Sk{rningskoordinatsystem                */
    DBint   gren;          /* \nskad gren. (gren < 0 antal grenar)    */
    DBint   int_case;      /* =1: Yta/plan  =2: Yta/yta =3: Plan/plan */
    DBSurf  sur2;          /* Yta 2 f|r sk{rning yta/yta              */
    DBPatch *patpek2;      /* Patches f|r yta 2                       */
    DBBplane bpl1;         /* F|r fallet att yta 1 {r ett B-plan      */
    DBBplane bpl2;         /* F|r fallet att yta 2 {r ett B-plan      */
    DBLine   lin;          /* Line for plane/plane intersect          */
    DBint   lin_no;        /* Requested intersection line             */
    DBint   nlin;          /* Number of intersect lines 0 or 1        */



#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe19*EXncui (N_INT_CUR) Enter \n");
 fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif

/*
***F|rst{ll till yta/plan sk{rning    
*/

    int_case = 1;
    status   = 0;

#ifdef  TABORT
/*
***H{mta geometri-data f|r refererad yta.
*/
    if ( DBget_pointer('I',rid1,&la,&typ) < 0 ) return(erpush("EX1402",""));
    if ( typ != SURTYP )
      {
      igidst(rid1,errbuf);
      return(erpush("EX1412",errbuf));
      }
    if ( (status=DBread_surface(&sur,la)) < 0 ) return(status);
    if ( (status=DBread_patches(&sur,&ptpat)) < 0 ) return(status);
#endif


/*
***H{mta geometri-data f|r refererad yta. Minne f|r patchar
***allokeras av DBread_patches().
*/
    if ( DBget_pointer('I',rid1,&la_sur,&typ) < 0 ) return(erpush("EX1402",""));

    switch ( typ )
      {
      case SURTYP:
      if ( (status=DBread_surface(&sur,la_sur)) < 0 ) return(status);
      if ( (status=DBread_patches(&sur,&ptpat)) < 0 ) return(status);
      break;

      case BPLTYP:
      DBread_bplane(&bpl1,la_sur);
      int_case = 3;
      break;

      default:
      IGidst(rid1,errbuf);
      return(erpush("EX1412",errbuf));
      }


/*
***H{mta geometri-data f|r refererat plan.
*/
    if ( DBget_pointer('I',rid2,&la_pln,&typ) < 0 ) return(erpush("EX1402",""));

    switch ( typ )
      {
      case CSYTYP:
      DBread_csys(&csy,NULL,la_pln);
      isys.g11 = csy.mat_pl.g11;
      isys.g12 = csy.mat_pl.g12;
      isys.g13 = csy.mat_pl.g13;
      isys.g14 = csy.mat_pl.g14;
      isys.g21 = csy.mat_pl.g21;
      isys.g22 = csy.mat_pl.g22;
      isys.g23 = csy.mat_pl.g23;
      isys.g24 = csy.mat_pl.g24;
      isys.g31 = csy.mat_pl.g31;
      isys.g32 = csy.mat_pl.g32;
      isys.g33 = csy.mat_pl.g33;
      isys.g34 = csy.mat_pl.g34;
      isys.g41 = csy.mat_pl.g41;
      isys.g42 = csy.mat_pl.g42;
      isys.g43 = csy.mat_pl.g43;
      isys.g44 = csy.mat_pl.g44;
      if ( int_case == 3 )
         {
         IGidst(rid2,errbuf);  /* Inf|r s{rskilt fel f|r detta !!! */
         status = erpush("EX1412",errbuf);
         goto c_lin;
         }
      break;
 
      case BPLTYP:
      DBread_bplane(&bpl2,la_pln);
      origin.x_gm= bpl2.crd1_bp.x_gm;
      origin.y_gm= bpl2.crd1_bp.y_gm;
      origin.z_gm= bpl2.crd1_bp.z_gm; 
      vecx.x_gm  = bpl2.crd4_bp.x_gm - bpl2.crd1_bp.x_gm;
      vecx.y_gm  = bpl2.crd4_bp.y_gm - bpl2.crd1_bp.y_gm;
      vecx.z_gm  = bpl2.crd4_bp.z_gm - bpl2.crd1_bp.z_gm;
      vecy.x_gm  = bpl2.crd2_bp.x_gm - bpl2.crd1_bp.x_gm;
      vecy.y_gm  = bpl2.crd2_bp.y_gm - bpl2.crd1_bp.y_gm;
      vecy.z_gm  = bpl2.crd2_bp.z_gm - bpl2.crd1_bp.z_gm;
      status = GEmktf_3p (&origin,&vecx,&vecy,&isys);
      if ( status < 0 )
        {
        if      ( int_case == 1 || int_case == 2 ) goto err3;
        else if ( int_case == 3 ) goto c_lin;
        }
      break;
 
      case SURTYP:
      if ( int_case == 3 )
         {
         IGidst(rid2,errbuf);  /* Inf|r s{rskilt fel f|r detta !!! */
         status = erpush("EX1412",errbuf);
         goto c_lin;
         }
      if ( (status=DBread_surface(&sur2, la_pln )) < 0 ) 
        {
        if      ( int_case == 1 ) goto err3;
        else if ( int_case == 3 ) goto c_lin;
        }
      if ( (status=DBread_patches(&sur2,&patpek2)) < 0 )
        {
        if      ( int_case == 1 ) goto err3;
        else if ( int_case == 3 ) goto c_lin;
        }
      int_case = 2;
      break;

      default:
      IGidst(rid2,errbuf);
      erpush("EX1412",errbuf);
      if      ( int_case == 1 ) goto err3;
      else if ( int_case == 3 ) goto c_lin;
      }

/*
***Ber{kna.
*/

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe19*EXncui isys plane %f %f %f \n",
  isys.g31,isys.g32,isys.g33   );
 fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif

    gren = -1;

    if      ( int_case == 1 )
      {
      status = varkon_sur_interplane
          (&sur,ptpat,la_sur,&isys,(DBint)1,(DBint)gren,
                    &no_branch,&cur,&geoseg);
      if ( status < 0  ) goto err3;
      *pnsect = no_branch;             
      }
    else if ( int_case == 2 )
      {
      status = I_UNDEF;
#ifdef   FCTN_UNDER_DEVOLOPMENT
              varkon_sur_intersurf
                (&sur,ptpat,&sur2,patpek2,(DBint)1,(DBint)gren,
                      &no_branch,&cur,&geoseg);
#endif
      if ( status < 0 ) goto err3;
      *pnsect = no_branch;             
      }
    else if ( int_case == 3 )
      {
      lin_no = -12; 
/* TODO Make -12, -1 or -2 available here ?          */
      status  = varkon_sur_planplan(&bpl1, &bpl2, lin_no, &lin, &nlin );
      *pnsect = nlin;
      goto c_lin; 
      }

/*
***Deallokera minne.
*/

err3:;
    DBfree_patches(&sur,ptpat);

c_lin:;

/*
***Slut.
*/

    if ( status >= 0 ) erinit();

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe19*EXncui isys *pnsect= %d\n", (int)*pnsect );
 fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif

    return(status);
  }

/********************************************************/
/*!******************************************************/

       short EXails(
       DBId     *rid,
       DBVector *punkt,
       DBVector *riktn,
       DBint     sort,
       DBVector *start,
       DBint    *nint,
       DBVector  uv[],
       DBVector  xyz[])

/*      Exekverar ALL_INT_LIN_SUR.
 *
 *      In: rid      => Pekare till identitet f|r refererad yta.
 *          punkt    => Yttre position.
 *          riktn    => Riktning.
 *          sort     => Sorteringsmetod.
 *          start    => Startv{rde i UV-planet.
 *
 *      Ut: *nint   = Antal sk{rningar.
 *           uv[]   = Sk{rningars UV-koordinater.
 *           xyz[]  = Sk{rningars R3-koordinater.
 *
 *      Felkod:      0 => Ok.
 *              EX1402 => Refererad storhet finns ej.
 *              EX1412 => Refererad storhet har fel typ.
 *
 *      (C)microform ab 13/9/95 J. Kjellander
 *
 *      29/9/95 startpunkt, J. Kjellander
 *      4/10/95 startpunkt surpac parameter G. Liden
 *      25/2/98 test of sur162, erinit G. Liden
 *      1998-05-06 hitable(), J.Kjellander
 *      1999-12-18 sur161->varkon_sur_intlinept G Liden
 *
 ******************************************************!*/

  {
    short   i,status;
    DBptr   la;
    DBetype   typ;
    char    errbuf[V3STRLEN+1];
    DBint   ocase,acc,sol;
    DBSurf   sur;
    DBPatch  *ptpat;
    DBVector   r3pt,uvpt;

/*
***Position och riktning skall vara i BASIC.
*/
   if ( lsyspk != NULL )
     {
     GEtfpos_to_local(punkt,&lklsyi,punkt);
     GEtfvec_to_local(riktn,&lklsyi,riktn);
     }
/*
***Hämta geometri-data för refererad yta.
*/
   if ( DBget_pointer('I',rid,&la,&typ) < 0 ) return(erpush("EX1402",""));
   if ( typ != SURTYP )
     {
     IGidst(rid,errbuf);
     return(erpush("EX1412",errbuf));
     }
   if ( (status=DBread_surface(&sur,la)) < 0 ) return(status);
/*
***Gör snabbtest.
*/
   if ( sort > 1  &&  !hitable(&sur.box_su,punkt,riktn) )
     {
    *nint = 0;
     return(0);
     }
/*
***Träff kan inte uteslutas.
*/
   if ( (status=DBread_patches(&sur,&ptpat)) < 0 ) return(status);
/*
***Beräkna.
*/
   ocase = 1001;
   acc   = 1;
   sol   = 1;
   start->x_gm = start->x_gm + 1.0;
   start->y_gm = start->y_gm + 1.0;

   status = varkon_sur_intlinept
               (&sur,ptpat,punkt,riktn,start,ocase,sort,
                         acc,sol,nint,&r3pt,&uvpt,uv,xyz);
   if ( status < 0 ) goto end;
   else              erinit();
/*
***Parametrar i MBS går från 0 till 1.
*/
   for ( i=0; i<*nint; ++i )
     {
     uv[i].x_gm -= 1.0;
     uv[i].y_gm -= 1.0;
     uv[i].z_gm -= 1.0;
     }
/*
***R3-positioner skall returneras i aktivt koordinatsystem.
*/
   if ( lsyspk != NULL )
     for ( i=0; i<*nint; ++i ) GEtfpos_to_local(&xyz[i],lsyspk,&xyz[i]);
/*
***Deallokera minne.
*/
end:
    DBfree_patches(&sur,ptpat);
/*
***Slut.
*/
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short EXailm(
       PMLITVA   ref[],
       DBint     nref,
       DBVector *punkt,
       DBVector *riktn,
       DBint     sort,
       DBint    *nint,
       DBVector  uv[],
       DBVector  xyz[],
       DBint   **snum)

/*      Exekverar ALL_INT_LIN_MSUR.
 *
 *      In: ref      = Array med yt-ID:s.
 *          nref     = Antal ytor
 *          punkt    = Yttre position.
 *          riktn    = Riktning.
 *          sort     = Sorteringsmetod.
 *          start    = Startvärde i UV-planet.
 *
 *      Ut: *nint   = Antal skärningar.
 *           uv[]   = Skärningars UV-koordinater.
 *           xyz[]  = Skärningars R3-koordinater.
 *          *snum   = Motsvarande ytid:ns ordningnummer i ref.
 *
 *      Felkod:      0 = Ok.
 *              EX1402 = Refererad storhet finns ej.
 *              EX1412 = Refererad storhet har fel typ.
 *              EX4192 = Fel från malloc.
 *              EX4202 = Kan ej beräkna skärning.
 *
 *      (C)microform ab 1998-05-06 J. Kjellander
 *
 *      1998-05-08 SGI, J.Kjellander
 *      1999-12-18 sur163->varkon_sur_int2linpt 
 *                 sur980->varkon_sur_order      G Liden
 *
 ******************************************************!*/

  {
    short    status;
    DBptr    la;
    DBetype  typ;
    DBint    i,j,ns,ns_tot,ns_sort,acc;
    char     errbuf[V3STRLEN+1];
    DBSurf    sur;
    DBPatch   *ptpat;
    EVALS    sol[SMAX],*sol_tot,*sol_sort=NULL,*sp;
    DBVector start_not_defined;

/*
***Initiering.
*/
   ns_tot = ns_sort = 0;
/*
***Position och riktning skall vara i BASIC.
*/
   if ( lsyspk != NULL )
     {
     GEtfpos_to_local(punkt,&lklsyi,punkt);
     GEtfvec_to_local(riktn,&lklsyi,riktn);
     }
/*
***Allokera minne för maximalt antal lösningar.
*/
   if ( (sol_tot=(EVALS *)v3mall(nref*SMAX*sizeof(EVALS),"EXailm")) == NULL )
     {
     sprintf(errbuf,"%d",(int)nref);
     return(erpush("EX4192",errbuf));
     }
/*
***Nu loopar vi igenom alla ytorna och beräknar skärning
***med dessa var och en. För varje yta där skärning erhålls
***sparar vi på oss skärningarna i sol_tot.
*/
   for ( i=0; i<nref; ++i )
     {
     if ( DBget_pointer('I',ref[i].lit.ref_va,&la,&typ) < 0 )
       return(erpush("EX1402",""));
     if ( typ != SURTYP )
       {
       IGidst(ref[i].lit.ref_va,errbuf);
       return(erpush("EX1412",errbuf));
       }
/*
***Hämta bara själva yt-posten till att börja med och
***gör snabb träfftest av ytboxen. Om träff kan uteslutas
***tar vi nästa yta direkt.
*/
     if ( (status=DBread_surface(&sur,la)) < 0 ) return(status);
     if ( sort > 1  &&  !hitable(&sur.box_su,punkt,riktn) ) goto next;
/*
***Träff kan inte uteslutas.
*/
     if ( (status=DBread_patches(&sur,&ptpat)) < 0 ) return(status);
/*
***Beräkning skärning linje/yta
***Indata är yta, linje, sort och utdata är antal lösningar
***och en array av lösningar (EVALS). 
*/
     acc                    =  1;
     start_not_defined.x_gm = -1.0;
     start_not_defined.y_gm = -1.0;
     start_not_defined.z_gm = -1.0;

     status = varkon_sur_int2linpt
              (&sur,ptpat,punkt,riktn,&start_not_defined,sort,
                            acc,&ns,sol);
     if ( status < 0 )
       {
       v3free(sol_tot,"EXailm");
       DBfree_patches(&sur,ptpat);
       return(erpush("EX4202",""));
       }
     else erinit();
/*
***Vilken yta i ordningen hör just dessa lösningar ihop med ?
***Skriv in löpnumret i lösningarnas EVALS-structure. (ordnum ?)
***Kopiera sen lösningarna till den stora arrayen och räkna upp ns_tot.
*/
     if ( ns > 0 )
       {
       for ( j=0; j<ns; ++j ) (sol+j)->ordnum = i;
       V3MOME(sol,sol_tot+ns_tot,ns*sizeof(EVALS));
       ns_tot += ns;
       }
/*
***Nästa yta.
*/
     DBfree_patches(&sur,ptpat);
next:;                    /* ; behövs för SGI-kompilatorn */
     }
/*
***Slut loopen för alla ytor. Sortera resultatet till sol_sort.
*/
   if ( ns_tot > 0 )
     {
     if ( (sol_sort=(EVALS *)v3mall(ns_tot*sizeof(EVALS),"EXailm")) == NULL )
       {
       v3free(sol_tot,"EXailm");
       sprintf(errbuf,"%d",(int)nref);
       return(erpush("EX4192",errbuf));
       }

     status = varkon_sur_order
              (punkt,sol_tot,ns_tot,(DBint)1,sol_sort,&ns_sort);
     if ( status < 0 )
       {
       v3free(sol_tot,"EXailm");
       v3free(sol_sort,"EXailm");
       return(erpush("EX4202",""));
       }
     else erinit();
     }
/*
***Kopiera resultatet till utdataparametrar uv, xyz och snum.
***För snum måste minne först allokeras. Detta minne deallokeras
***i anropande rutin.
*/
   if ( ns_sort > 0 )
     {
     if ( (*snum=(DBint *)v3mall(ns_sort*sizeof(DBint),"EXailm")) == NULL )
       {
       v3free(sol_tot,"EXailm");
       v3free(sol_sort,"EXailm");
       sprintf(errbuf,"%d",(int)nref);
       return(erpush("EX4192",errbuf));
       }
     }

   for ( i=0; i<ns_sort; ++i )
     {
     sp = sol_sort+i;
     (uv+i)->x_gm  = sp->u - 1.0;
     (uv+i)->y_gm  = sp->v - 1.0;
     (uv+i)->z_gm  = 0.0;
     (xyz+i)->x_gm = sp->r_x;
     (xyz+i)->y_gm = sp->r_y;
     (xyz+i)->z_gm = sp->r_z;
    *(*snum+i)     = sp->ordnum;
     }

   *nint = ns_sort;
/*
***R3-positioner skall returneras i aktivt koordinatsystem.
*/
   if ( lsyspk != NULL )
     for ( i=0; i<*nint; ++i ) GEtfpos_to_local(xyz+i,lsyspk,xyz+i);
/*
***Deallokera minne.
*/
    if ( ns_tot > 0 ) v3free(sol_sort,"EXailm");
    v3free(sol_tot,"EXailm");
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

       short EXaios(
       DBId     *rid,
       DBVector *punkt,
       DBint     sort,
       DBint    *nsol,
       DBVector  uv[],
       DBVector  xyz[])

/*      Exekverar ALL_INV_ON_SUR.
 *
 *      In: rid      => Pekare till identitet f|r refererad yta.
 *          punkt    => Yttre position.
 *          sort     => Sorteringsmetod.
 *
 *      Ut: *nsol   = Antal l|sningar.
 *           uv[]   = L|sningars UV-koordinater.
 *           xyz[]  = L|sningars R3-koordinater.
 *
 *      Felkod:      0 => Ok.
 *              EX1402 => Refererad storhet finns ej.
 *              EX1412 => Refererad storhet har fel typ.
 *
 *      (C)microform ab 13/9/95 J. Kjellander
 *      1998-02-25 erinit() added  G. Liden
 *      1999-12-18 sur160->varkon_sur_closestpt  G. Liden
 *
 ******************************************************!*/

  {
    short   i,status;
    DBptr   la;
    DBetype   typ;
    char    errbuf[V3STRLEN+1];
    DBSurf   sur;
    DBPatch  *ptpat;
    DBVector   start;

#ifdef DEBUG
    if ( dbglev(EXEPAC) == 19 )
      {
      fprintf(dbgfil(EXEPAC),"***Start-EXaios***\n");
      fflush(dbgfil(EXEPAC));
      }
#endif

/*
***H{mta geometri-data f|r refererad yta.
*/
    if ( DBget_pointer('I',rid,&la,&typ) < 0 ) return(erpush("EX1402",""));
    if ( typ != SURTYP )
      {
      IGidst(rid,errbuf);
      return(erpush("EX1412",errbuf));
      }
    if ( (status=DBread_surface(&sur,la)) < 0 ) return(status);
    if ( (status=DBread_patches(&sur,&ptpat)) < 0 ) return(status);
/*
***Ber{kna.
*/
   start.x_gm =  0.0;
   start.y_gm =  0.0;
   start.z_gm = -1.0;

   status = varkon_sur_closestpt
            (&sur,ptpat,punkt,&start,sort,2,(DBint)1,nsol,xyz,uv);
   if ( status < 0 ) goto end;
/*
***Parametrar i MBS g}r fr}n 0 till 1.
*/
   for ( i=0; i<*nsol; ++i )
     {
     uv[i].x_gm -= 1.0;
     uv[i].y_gm -= 1.0;
     uv[i].z_gm -= 1.0;
     }
/*
***Deallokera minne.
*/
end:
    DBfree_patches(&sur,ptpat);
/*
***Slut.
*/
 
    if ( status >= 0 ) erinit();

#ifdef DEBUG
    if ( dbglev(EXEPAC) == 19 )
      {
      fprintf(dbgfil(EXEPAC),"***Slut-EXaios***\n\n");
      fflush(dbgfil(EXEPAC));
      }
#endif

    return(status);
  }

/********************************************************/
/*!******************************************************/

 static bool hitable(
        BBOX     *box,
        DBVector *p,
        DBVector *v)

/*      Snabbtest av skärning ytbox/linje. Används
 *      av EXailm(). OBS ! Förutsätter att man bara
 *      är intresserad av träffar i skjutriktningen.
 *
 *      In: box = Pekare till ytbox.
 *          p   = Pekare till linjens startposition.
 *          v   = Pekare till skjutriktning.
 *
 *      FV:  TRUE  = Träff kan inte uteslutas.
 *           FALSE = Träff kan säkert uteslutas.
 *
 *      (C)microform ab 1998-05-06 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Först kollar vi att boxen är OK.
*/
    if ( box->flag != (short)1 ) return(TRUE);
/*
***Om startpunkten ligger till höger om boxen
***och linjen är riktad åt höger kan det inte bli träff.
***Samma fast tvärt om om den ligger till vänster.
*/
    if ( p->x_gm > box->xmax  &&   v->x_gm > 0 ) return(FALSE);
    if ( p->x_gm < box->xmin  &&   v->x_gm < 0 ) return(FALSE);
/*
***Över/under.
*/
    if ( p->y_gm > box->ymax  &&   v->y_gm > 0 ) return(FALSE);
    if ( p->y_gm < box->ymin  &&   v->y_gm < 0 ) return(FALSE);
/*
***Hitom/bakom.
*/
    if ( p->z_gm > box->zmax  &&   v->z_gm > 0 ) return(FALSE);
    if ( p->z_gm < box->zmin  &&   v->z_gm < 0 ) return(FALSE);

    return(TRUE);
  }

/********************************************************/
