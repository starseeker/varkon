/*!******************************************************************
*                         
*    ex22.c                
*    ======                 
*                            
*    EXcsil();      Create CUR_SIL
*    EXcint();      Create CUR_INT 
*    EXciso();      Create CUR_ISO  
*    EXcfan();      Create CUR_FANGA 
*    EXcged();      Create CUR_GEODESIC 
*    EXcapp();      Create CUR_APPROX
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
*    (C)Microform AB 1984-1998, Johan Kjellander, johan@microform.se
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../include/EX.h"

#ifdef DEBUG
#include "../../IG/include/debug.h"
#endif

extern DBTmat *lsyspk;     /* Active coord. system                  */
extern DBTmat  lklsyi;     /* Active coord. system, inverted        */

/* varkon_sur_silhouette    * Calculate silhouette curve            */
/* varkon_sur_interplane    * Calculate surface/plane   intersect   */
/* varkon_sur_intersurf     * Calculate surface/surface intersect   */
/* varkon_sur_planplan      * Calculate plane/plane     intersect   */
/* varkon_sur_geodesic      * Calculate geodesic curve on surface   */
/* varkon_sur_interfacet    * Calculate FAC_SUR/FAC_SUR intersect   */
/* varkon_sur_fanga         * Calc. FANGA surface analysis curves   */
/* varkon_sur_isoparam      * Create isoparametric curve            */
/* varkon_cur_approx        * Calculate approximate curve           */
/* varkon_ini_gmlin         * Initialize line (DBLine) data         */
/* varkon_sur_scur_gra      * Graphical representation of curve     */
/* varkon_sur_scur_uv       * Graphical representation, scaled crv  */

/*!******************************************************/

       short EXcsil(
       V2REFVA  *id,
       V2REFVA  *rid,
       DBVector *pos,
       DBshort   fall,
       DBshort   gren,
       V2NAPA   *pnp)

/*      Skapa CUR_SIL.
 *
 *      In: id     => Pekare till kurvans identitet.
 *          rid    => Pekare till refererad ytas id.
 *          pos    => Pekare till position.
 *          fall   => Ber{kningsfall, 1 = \gonpunkt.
 *                                    2 = Riktning.
 *          gren   => Kurvgren.
 *          pnp    => Pekare till attribut.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          EX1402 => Refererad storhet finns ej.
 *          EX1412 => Refererad storhet har fel typ.
 *          EX4042 => fall har felaktigt v{rde.
 *          EX4072 => gren <= 0
 *
 *      (C)microform ab 18/2/93 J. Kjellander
 *
 *      21/3/94  Nya DBPatch, J. Kjellander
 *      16/4/94  case 3: Tillagt, G. Liden
 *      06/5/94  case 3: Tillagt igen, G. Liden
 *      03/6/94  [ndrade argument f|r sur100 G. Liden
 *     22/10/96  la added as sur100 argument G. Liden
 *     1999-12-18 sur100->varkon_sur_silhouette G Liden
 *
 ******************************************************!*/

  {
    short   status;
    DBptr   la;
    DBetype typ;
    DBfloat   eyedat[9];
    char    errbuf[V3STRLEN+1];
    DBPatch  *patpek;
    DBSurf   sur;
    DBCurve   cur;
    DBint   no_branch;     /* Totalt antal kurvgrenar */
    DBSeg  *graseg,*geoseg;
    DBint   gren_in;           /* F|r skalad UV kurva */
/*
***Ev. debug.
*/
#ifdef DEBUG
    if ( dbglev(EXEPAC) == 1 )
      {
      fprintf(dbgfil(EXEPAC),
      "exe22*EXcsil Enter***\n");
      fprintf(dbgfil(EXEPAC),
      "exe22*EXcsil pos=%g,%g,%g\n",pos->x_gm,pos->y_gm,pos->z_gm);
      fprintf(dbgfil(EXEPAC),
      "exe22*EXcsil fall=%d,gren=%d\n",(int)fall,(int)gren);
      fflush(dbgfil(EXEPAC)); /* To file from buffer      */
      }
#endif
/*
***Kolla att gren har rimligt v{rde.
*/
    if ( gren <= 0 ) return(erpush("EX4072",""));
    gren_in = gren;                        /*F|r skalad UV kurva */
    if ( gren > 1000 ) gren = gren - 1000; /*F|r skalad UV kurva */
       
/*
***H{mta geometri-data f|r refererad yta. Minne f|r patchar
***allokeras av DBread_patches().
*/
    if ( DBget_pointer('I',rid,&la,&typ) < 0 ) return(erpush("EX1402",""));

    switch ( typ )
      {
      case SURTYP:
      if ( (status=DBread_surface(&sur,la)) < 0 ) return(status);
      if ( (status=DBread_patches(&sur,&patpek)) < 0 ) return(status);
      break;

      default:
      igidst(rid,errbuf);
      return(erpush("EX1412",errbuf));
      }
/*
***Bilda kurva. \gonpunkt/riktning transformeras till LOCAL
***och lagras i eyedat[9].
*/
    switch ( fall )
      {
      case 1:
      if ( lsyspk != NULL ) GEtfpos_to_local(pos,&lklsyi,pos);
      eyedat[3] = pos->x_gm;
      eyedat[4] = pos->y_gm;
      eyedat[5] = pos->z_gm;
      break;

      case 2:
      if ( lsyspk != NULL ) GEtfvec_to_local(pos,&lklsyi,pos);
      eyedat[0] = pos->x_gm;
      eyedat[1] = pos->y_gm;
      eyedat[2] = pos->z_gm;
      break;

      case 3:
      if ( lsyspk != NULL ) GEtfvec_to_local(pos,&lklsyi,pos);
      eyedat[0] = pos->x_gm;
      eyedat[1] = pos->y_gm;
      eyedat[2] = pos->z_gm;
      eyedat[3] = 0.0;      
      eyedat[4] = 0.0;      
      eyedat[5] = 0.0;      
      eyedat[6] = 0.0;      
      break;

      default:
      status = erpush("EX4042","");
      goto err3;
      }
/*
***Calculate surface curve. Memory for UV-segment is allocated
***by varkon_sur_silhouette. If return geoseg value = NULL    
***has no memory been allocated
*/
    status = varkon_sur_silhouette
                (&sur,patpek,la,eyedat,(int)fall,1,(int)gren,
                    &no_branch, &cur,&geoseg);
    if ( status < 0 || geoseg == NULL ) goto err3;

/*
***Bilda grafisk representation. Minne f|r grafiska segment allokeras
***av sur960(). Om sur960() returnerar negativ status har inget
***minne allokerats.
*/
    if ( gren_in > 1000 )                   /*For scaled UV curve */
      { 
      status = varkon_sur_scur_uv(&cur,geoseg,&graseg);
      } 
    else                                    /*For R*3       curve */
      { 
      status = varkon_sur_scur_gra(&cur,geoseg,&graseg);
      }     
    if ( status < 0 ) goto err2;
/*
***Lagra cur-posten i DB och rita.
*/
    status = EXecur(id,&cur,graseg,geoseg,pnp);
    if ( status < 0 ) goto err1;
/*
***L{mna tillbaks dynamiskt allokerat minne f|r patch-data och
***segment-data.
*/
err1:
    DBfree_segments(graseg);
err2:
    DBfree_segments(geoseg);
err3:
    DBfree_patches(&sur,patpek);
/*
***Ev. debug.
*/
#ifdef DEBUG
    if ( dbglev(EXEPAC) == 1 )
      {
      fprintf(dbgfil(EXEPAC),"exe22*EXcil Exit**********\n");
      fflush(dbgfil(EXEPAC)); /* To file from buffer      */
      }
#endif
/*
***Slut.
*/
    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short EXcint(
       DBId    *id,
       DBId    *yta,
       DBId    *plan,
       DBshort  gren,
       V2NAPA  *pnp)

/*      Skapa CUR_INT.
 *
 *      In: id     => Pekare till kurvans identitet.
 *          yta    => Pekare till refererad yta:s eller b-plan:s id.
 *          plan   => Pekare till refererat plan:s id.
 *          gren   => Kurvgren.
 *          pnp    => Pekare till attribut.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          EX1402 => Refererad storhet finns ej.
 *          EX1412 => Refererad storhet har fel typ.
 *          EX4072 => gren <= 0
 *
 *      (C)microform ab 18/2/93 J. Kjellander
 *
 *      21/3/94  Nya DBPatch, J. Kjellander
 *      01/5/94  [ndrade argument f|r sur102  
 *               B_PLANE till koord. system    Gunnar Liden
 *      1994-12-06 Debug (Typ av storhet n|dv{ndigt f|r geo110) G Liden
 *      1996-01-01 sur104 tillagd      G Liden
 *      1996-01-06 sur105 och sur107 tillagda      G Liden
 *      1996-01-18 Debug, DBptr indata sur104      G Liden
 *      1996-05-25        DBptr indata sur102      G Liden
 *      1997-03-23 Initialize p_csy, lin_no        G Liden
 *      1998-03-31 subtyp 2 -> 1 Temporarely       G Liden
 *      1998-04-06 Debug la_pln                    G Liden
 *      1999-12-18 surxxx->varkon_sur_yyyyy        G Liden
 *
 ******************************************************!*/

  {
    short   i,status;
    DBptr   la_pln;
    DBptr   la_sur;
    DBetype typ;
    char    errbuf[V3STRLEN+1];
    DBPatch  *patpek;
    DBSurf   sur;
    DBCurve   cur;
    DBCsys  csy;
    DBSeg   *graseg,*geoseg;
    DBint   gren_in;       /* GL f|r skalad UV kurva */
    DBint   no_branch;     /* Totalt antal kurvgrenar */
    DBVector vecx;         /* X axel f|r sk{rningskoordinatsystem     */
    DBVector vecy;         /* Y axel f|r sk{rningskoordinatsystem     */
    DBTmat  isys;          /* Sk{rningskoordinatsystem                */
    DBTmat *p_csy;         /* Pekare till sk{rningskoordinatsystem    */
    DBint   int_case;      /* =1: Yta/plan  =2: Yta/yta =3: Plan/plan */
                           /* =4: Fasett ytor                         */
    DBSurf  sur2;          /* Yta 2 f|r sk{rning yta/yta              */
    DBPatch *patpek2;      /* Patches f|r yta 2                       */
    DBBplane bpl1;         /* F|r fallet att yta 1 {r ett B-plan      */
    DBBplane bpl2;         /* F|r fallet att yta 2 {r ett B-plan      */
    DBLine  lin;           /* Line for plane/plane intersect          */
    DBint   lin_no;        /* Requested intersection line             */
    DBint   nlin;          /* Number of lines 0 or 1                  */
    DBint   a_size;        /* Array size as dimensioned here = 3000   */
    DBVector s_pts[3000];  /* Start points for lines                  */
    DBVector e_pts[3000];  /* End   points for lines                  */
    DBint   nlines;        /* Number of output lines                  */
    DBint   i_lin;         /* Loop index line                         */

/*
***F|rst{ll till yta/plan sk{rning    
*/

    int_case =    1;
    a_size   = 3000;
    gren_in  = I_UNDEF;
    p_csy    = NULL;
    lin_no   = I_UNDEF;

    la_pln   = DBNULL;
    la_sur   = DBNULL;

/*
***H{mta geometri-data f|r refererad yta. Minne f|r patchar
***allokeras av DBread_patches().
*/
    if ( DBget_pointer('I',yta,&la_sur,&typ) < 0 ) return(erpush("EX1402",""));

    switch ( typ )
      {
      case SURTYP:
      if ( (status=DBread_surface(&sur,la_sur)) < 0 ) return(status);
      if ( (status=DBread_patches(&sur,&patpek)) < 0 ) return(status);
      break;

      case BPLTYP:
      DBread_bplane(&bpl1,la_sur);
      int_case = 3;
      break;

      default:
      igidst(yta,errbuf);
      return(erpush("EX1412",errbuf));
      }
/*
***H{mta geometri-data f|r refererat sk{rningsplan.
*/
    if ( DBget_pointer('I',plan,&la_pln,&typ) < 0 ) return(erpush("EX1402",""));

    switch ( typ )
      {
      case CSYTYP:
      DBread_csys(&csy,NULL,la_pln);
      p_csy = &csy.mat_pl;
      if ( int_case == 3 )
         {
         igidst(plan,errbuf);  /* Inf|r s{rskilt fel f|r detta !!! */
         status = erpush("EX1412",errbuf);
         goto c_lin;
         }
      break;

      case BPLTYP:
      DBread_bplane(&bpl2,la_pln);
      vecx.x_gm = bpl2.crd4_bp.x_gm - bpl2.crd1_bp.x_gm;
      vecx.y_gm = bpl2.crd4_bp.y_gm - bpl2.crd1_bp.y_gm;
      vecx.z_gm = bpl2.crd4_bp.z_gm - bpl2.crd1_bp.z_gm;
      vecy.x_gm = bpl2.crd2_bp.x_gm - bpl2.crd1_bp.x_gm;
      vecy.y_gm = bpl2.crd2_bp.y_gm - bpl2.crd1_bp.y_gm;
      vecy.z_gm = bpl2.crd2_bp.z_gm - bpl2.crd1_bp.z_gm;
      status = GEmktf_3p (&bpl2.crd1_bp,&vecx,&vecy,&isys);
      if ( status < 0 ) goto err3;
      p_csy = &isys;
      break;

      case SURTYP:
      if ( int_case == 3 )
         {
         igidst(plan,errbuf);  /* Inf|r s{rskilt fel f|r detta !!! */
         status = erpush("EX1412",errbuf);
         goto c_lin;
         }
      if ( (status=DBread_surface(&sur2, la_pln )) < 0 ) return(status);
      if ( (status=DBread_patches(&sur2,&patpek2)) < 0 ) return(status);
      int_case = 2;
      break;

      default:
      igidst(plan,errbuf);
      status = erpush("EX1412",errbuf);
      if      ( int_case == 1 || int_case == 2 ) goto err3;
      else if ( int_case == 3 ) goto c_lin;
      }

    if  (int_case == 2 && sur.typ_su == FAC_SUR && sur2.typ_su == FAC_SUR )
      {
      int_case = 4;
      }

/*
***Kolla att gren har rimligt v{rde.
*/
    if  (  int_case ==  1  || int_case == 2 )
      {
      if ( gren <= 0 ) return(erpush("EX4072",""));
      gren_in = gren;                        /*GL f|r skalad UV kurva */
      if ( gren > 1000 ) gren = gren - 1000; /*GL f|r skalad UV kurva */
      }

    if  (  int_case ==  3  )
      {
      if ( gren == 1 || gren == -1 || gren == -2 )  
         {
         ;
         }
      else
         {
         return(erpush("EX4072",""));
         }
       }

    if  (  int_case ==  4  )
      {
      ; /* gren utnyttjas ej  */
      }

/*
***Bilda kurva.
*/

    if      ( int_case == 1 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe22*EXcint Yta/plan sk{rning int_case %d\n", (int)int_case );
fprintf(dbgfil(EXEPAC),
"exe22*EXcint &sur  %d patpek  %d la_sur %d\n", (int)&sur , 
        (int)patpek, (int)la_sur );
fprintf(dbgfil(EXEPAC),
"exe22*EXcint p_csy %d la_pln %d\n", (int)p_csy, (int)la_pln );
fprintf(dbgfil(EXEPAC),
"exe22*EXcint gren %d\n", (int)gren );
fflush(dbgfil(EXEPAC)); 
}
#endif
      status = varkon_sur_interplane
               (&sur,patpek,la_sur,p_csy,(DBint)1,(DBint)gren,
                      &no_branch,&cur,&geoseg);
      if ( status < 0 || geoseg == NULL ) goto err3;
      }
    else if ( int_case == 2 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe22*EXcint Yta/yta sk{rning int_case %d\n", (int)int_case );
fprintf(dbgfil(EXEPAC),
"exe22*EXcint &sur  %d patpek  %d la_sur %d\n", (int)&sur , (int)patpek, (int)la_sur );
fprintf(dbgfil(EXEPAC),
"exe22*EXcint &sur2 %d patpek2 %d la_pln %d\n", (int)&sur2, (int)patpek2, (int)la_pln );
fprintf(dbgfil(EXEPAC),
"exe22*EXcint gren %d\n", (int)gren );
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif

      status = varkon_sur_intersurf
               (&sur,patpek,la_sur,&sur2,patpek2,la_pln,
               (DBint)1,(DBint)gren, &no_branch,&cur,&geoseg);
      if ( status < 0 || geoseg == NULL ) goto err3;
      }
    else if ( int_case == 3 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe22*EXcint Plan/plan sk{rning int_case %d\n", (int)int_case );
fprintf(dbgfil(EXEPAC),
"exe22*EXcint &bpl1 %d &bpl2 %d\n", (int)&bpl1,(int)&bpl2 );
fprintf(dbgfil(EXEPAC),
"exe22*EXcint bpl1.crd1_bp %f %f %f (p11)\n", 
  bpl1.crd1_bp.x_gm, bpl1.crd1_bp.y_gm, bpl1.crd1_bp.z_gm  );
fprintf(dbgfil(EXEPAC),
"exe22*EXcint bpl2.crd1_bp %f %f %f (p12)\n", 
  bpl1.crd2_bp.x_gm, bpl1.crd2_bp.y_gm, bpl1.crd2_bp.z_gm  );
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif

      if       ( gren ==   1 ) lin_no = 12;
      else if  ( gren ==  -1 ) lin_no =  1;
      else if  ( gren ==  -2 ) lin_no =  2;
      else if  ( gren == -99 ) lin_no = 99;

      status = varkon_sur_planplan(&bpl1,&bpl2,lin_no,&lin ,&nlin);
      if ( status < 0 ) goto c_lin;
      EXelin(id,&lin,pnp);
      goto c_lin; 
      }

    else if ( int_case == 4 )
      {
      status = varkon_sur_interfacet(&sur,patpek,&sur2,patpek2,a_size,
                      s_pts, e_pts, &nlines  );
      if ( status < 0 ) goto c_lin;
      varkon_ini_gmlin(&lin); /* Initialize DBLine data */
      for ( i_lin = 0; i_lin < nlines; ++ i_lin)
        {
        lin.crd1_l = s_pts[i_lin];
        lin.crd2_l = e_pts[i_lin];
        EXelin(id,&lin,pnp);
        }
      goto c_lin; 
      }


/*
***Fyll i subtyp och ytans DB-adress i UV-segmenten.
*/

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe22*EXcint DB address to UV-segments int_case %d\n", (int)int_case );
fprintf(dbgfil(EXEPAC),
"exe22*EXcint spek_gm= la_sur= %d spek2_gm= la_pln= %d\n", 
              (int)la_sur, (int)la_pln );
fflush(dbgfil(EXEPAC)); 
}
#endif

    for ( i=0; i<cur.ns_cu; ++ i)
      {
      (geoseg+i)->subtyp   = 2;
      (geoseg+i)->subtyp   = 1;  /* Must be surface, not plane       */
      (geoseg+i)->spek_gm  = la_sur;
      (geoseg+i)->spek2_gm = la_pln;
      (geoseg+i)->spek2_gm  = 0; /* Bug reported to Johan 1998-03-31 */
      }

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
"exe22 Before sur960 cur.hed_cu.type %d\n", cur.hed_cu.type );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

/*
***Bilda grafisk representation.
*/
    if ( gren_in > 1000 )                   /* For scaled UV curve */
      {
      status = varkon_sur_scur_uv(&cur,geoseg,&graseg);
      }
    else                                    /* For R*3       curve */
      {
      status = varkon_sur_scur_gra(&cur,geoseg,&graseg);
      } 
    if ( status < 0 ) goto err2;
/*
***Lagra cur-posten i DB och rita.
*/
    status = EXecur(id,&cur,graseg,geoseg,pnp);
    if ( status < 0 ) goto err1;
/*
***L{mna tillbaks dynamiskt allokerat minne f|r patch-data och
***segment-data.
*/
err1:
    DBfree_segments(graseg);
err2:
    DBfree_segments(geoseg);
err3:
    DBfree_patches(&sur,patpek);
    if      ( int_case == 2 )
      {
      DBfree_patches(&sur2,patpek2);
      }
c_lin:

/*
***Slut.
*/
    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short EXciso(
       DBId    *id,
       DBId    *rid,
       char    *fall,
       DBfloat  value,
       DBfloat  start,
       DBfloat  slut,
       V2NAPA  *pnp)

/*      Skapa CUR_ISO.
 *
 *      In: id     => Pekare till kurvans identitet.
 *          rid    => Pekare till refererad ytas id.
 *          fall   => "U" eller "V".
 *          value  => U- alt. V-v{rde.
 *          start  => Start-v{rde (i andra riktningen).
 *          slut   => Slut-v{rde (i andra riktningen).
 *          pnp    => Pekare till attribut.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          EX1402 => Refererad storhet finns ej.
 *          EX1412 => Refererad storhet har fel typ.
 *
 *      (C)microform ab 18/2/93 J. Kjellander
 *
 *      21/3/94  Nya DBPatch, J. Kjellander
 *      1994-10-29 la indata till sur150  Gunnar Liden
 *      1995-03-02 deallokering, Gunnar Liden
 *      1999-12-18 sur150->varkon_sur_isoparam G Liden
 *
 ******************************************************!*/

  {
    short   i,status;
    DBptr   la;
    DBetype typ;
    char    errbuf[V3STRLEN+1];
    DBPatch  *patpek;
    DBSurf   sur;
    DBCurve   cur;
    DBSeg  *graseg,*geoseg;

/*
***Ev DEBUG.
*/
#ifdef DEBUG
    if ( dbglev(EXEPAC) == 1 )
      {
      fprintf(dbgfil(EXEPAC),"***Start-EXciso***\n");
      fflush(dbgfil(EXEPAC));
      }
#endif

/*
***H{mta geometri-data f|r refererad yta. Minne f|r patchar
***allokeras av DBread_patches().
*/
    if ( DBget_pointer('I',rid,&la,&typ) < 0 ) return(erpush("EX1402",""));

    switch ( typ )
      {
      case SURTYP:
      if ( (status=DBread_surface(&sur,la)) < 0 ) return(status);
      if ( (status=DBread_patches(&sur,&patpek)) < 0 ) return(status);
      break;

      default:
      igidst(rid,errbuf);
      return(erpush("EX1412",errbuf));
      }

#ifdef DEBUG
    if ( dbglev(EXEPAC) == 1 )
      {
      fprintf(dbgfil(EXEPAC),"patpek=%d\n",(int)patpek);
      fprintf(dbgfil(EXEPAC),"i,status=%d,%d\n",(int)i,(int)status);
      fprintf(dbgfil(EXEPAC),"geoseg,graseg=%d,%d\n",(int)geoseg,(int)graseg);
      fflush(dbgfil(EXEPAC));
      }
#endif
/*
***Bilda geometrisk representation.
*/
    value += 1.0;
    start += 1.0;
    slut  += 1.0;
    status = varkon_sur_isoparam
          (&sur,patpek,fall,value,start,slut,la,&cur,&geoseg);
    if      ( status < 0  &&  geoseg == NULL ) goto err3;
    else if ( status < 0                     ) goto err2;

#ifdef DEBUG
    if ( dbglev(EXEPAC) == 1 )
      {
      fprintf(dbgfil(EXEPAC),"i,status=%d,%d\n",(int)i,(int)status);
      fprintf(dbgfil(EXEPAC),"geoseg,graseg=%d,%d\n",(int)geoseg,(int)graseg);
      fflush(dbgfil(EXEPAC));
      }
#endif
/*
***Fyll i subtyp och ytans DB-adress i UV-segmenten.
*/
    for ( i=0; i<cur.ns_cu; ++ i)
      {
      (geoseg+i)->subtyp   = 1;
      (geoseg+i)->spek_gm  = la;
      (geoseg+i)->spek2_gm =  0;  
#ifdef DEBUG
    if ( dbglev(EXEPAC) == 1 )
      {
      fprintf(dbgfil(EXEPAC),"geoseg nr:%d, typ=%d, ofs=%g\n",
                              (int)i+1,(int)(geoseg+i)->typ,(geoseg+i)->ofs);
      fflush(dbgfil(EXEPAC));
      }
#endif
      }
/*
***Bilda grafisk representation.
*/
    status = varkon_sur_scur_gra(&cur,geoseg,&graseg);
    if      ( status < 0  &&  graseg == NULL ) goto err2;
    else if ( status < 0                     ) goto err1;

#ifdef DEBUG
    if ( dbglev(EXEPAC) == 1 )
      {
      fprintf(dbgfil(EXEPAC),"i,status=%d,%d\n",(int)i,(int)status);
      fprintf(dbgfil(EXEPAC),"geoseg,graseg=%d,%d\n",(int)geoseg,(int)graseg);
      fflush(dbgfil(EXEPAC));

      for ( i=0; i<cur.nsgr_cu; ++i )
        {
        fprintf(dbgfil(EXEPAC),"graseg nr:%d, typ=%d, ofs=%g\n",
                                (int)i+1,(int)(graseg+i)->typ,(graseg+i)->ofs);
        fflush(dbgfil(EXEPAC));
        }
      }
#endif
/*
***Lagra cur-posten i DB och rita.
*/
    status = EXecur(id,&cur,graseg,geoseg,pnp);

#ifdef DEBUG
    if ( dbglev(EXEPAC) == 1 )
      {
      fprintf(dbgfil(EXEPAC),"i,status=%d,%d\n",(int)i,(int)status);
      fprintf(dbgfil(EXEPAC),"geoseg,graseg=%d,%d\n",(int)geoseg,(int)graseg);
      fflush(dbgfil(EXEPAC));
      }
#endif

    if ( status < 0 ) goto err1;
/*
***L{mna tillbaks dynamiskt allokerat minne f|r patch-data.
*/
err1:
    DBfree_segments(graseg);
err2:
    DBfree_segments(geoseg);
err3:
    DBfree_patches(&sur,patpek);
/*
***Slut.
*/
#ifdef DEBUG
    if ( dbglev(EXEPAC) == 1 )
      {
      fprintf(dbgfil(EXEPAC),"***Slut-EXciso***\n\n");
      fflush(dbgfil(EXEPAC));
      }
#endif

    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short EXcfan(
       DBId     *id,
       DBId     *rid,
       DBVector *rikt_1,
       DBVector *rikt_2,
       DBint     sstart,
       DBint     sslut,
       DBfloat   ustart,
       DBfloat   uslut,
       V2NAPA   *pnp)

/*      Skapa CUR_FANGA.
 *
 *      In: id     => Pekare till kurvans identitet.
 *          rid    => Pekare till refererad ytas id.
 *          rikt_1 => Riktning 1.
 *          rikt_2 => Riktning_2.
 *          sstart => 1:a strip.
 *          sslut  => Sista strip.
 *          ustart => Startv{rde.
 *          uslut  => Slutv{rde.
 *          pnp    => Pekare till attribut.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          EX1402 => Refererad storhet finns ej.
 *          EX1412 => Refererad storhet har fel typ.
 *
 *      (C)microform ab 5/3/95 J. Kjellander
 *      1995-06-17 f_case missing, output not one UV curve  G Liden
 *      1996-01-01 Outnyttjad lokal variabel suddad         G Liden
 *      1996-11-07 No. of graphic seegments nsgr_cu had no value Gunnar
 *      1999-12-18 sur120->varkon_sur_fanga G Liden
 *
 ******************************************************!*/

  {
    short   status;
    DBptr   la;
    DBetype typ;
    char    errbuf[V3STRLEN+1];
    DBPatch  *patpek;
    DBSurf   sur;
    DBCurve   cur;
    DBint    f_case;       /* FANGA case                              */
    DBSeg   *segarr[6];    /* FANGA curve segments        (ptr array) */
    DBint    i_seg;        /* Loop index segment                      */
    DBint    nseg;         /* Number of segments in curve             */

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
"exe22*EXcfan sstart %d sslut %d ustart %f uslut %f \n", 
              (short)sstart ,  (short)sslut, ustart,  uslut );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
/*
***H{mta geometri-data f|r refererad yta. Minne f|r patchar
***allokeras av DBread_patches().
*/
    if ( DBget_pointer('I',rid,&la,&typ) < 0 ) return(erpush("EX1402",""));

    switch ( typ )
      {
      case SURTYP:
      if ( (status=DBread_surface(&sur,la)) < 0 ) return(status);
      if ( (status=DBread_patches(&sur,&patpek)) < 0 ) return(status);
      break;

      default:
      igidst(rid,errbuf);
      return(erpush("EX1412",errbuf));
      }
/*
***Bilda geometrisk representation. OBS!!!!!! rikt_1 och rikt_2 {r
***pekare inte som i EXcugl(), [NDRA!!!!!!! 1995-06-17 [ndrat G Liden
*/

/*
***Et meddelande.
*/
  igptma(454,IG_MESS);


/* 1995-06-17 Johan! Vore bra om Du skickar -1 n{r inget matats in */
/*                   sstart och sslut borde kunna anges utan u v{rden ocks} */
/*                   Suddningen av flera instanser fungerar inte. Bara f|rsta */
/*                   verkar suddas, men |vriga f|rsvinner om sk{rmen ritas om */

    if ( sstart == 0 ) 
        {
        sstart = -1;       
        sslut  = -1;       
        ustart = -1.0;
        uslut  = -1.0;
        }

    if ( ustart >= 0.0 ) ustart += 1.0;
    if ( uslut  >= 0.0 ) uslut  += 1.0;

    f_case = 0;  /* Not yet used */

    status = varkon_sur_fanga(&sur,patpek,la,rikt_1,rikt_2,ustart,uslut,
                     sstart,sslut,f_case,&cur, segarr);
    if ( status < 0 ) goto err3;
    cur.nsgr_cu = cur.ns_cu;  /* 1996-11-07 */

/*!                                                                 */
/* Transformate if localsystem is active and store created          */
/* curves in Geometry Memory (DB).                                  */
/* Calls of GEtfseg_to_local and EXecur.                                      */
/*                                                                 !*/

  if ( segarr[0] != NULL )
    {
    if ( lsyspk != NULL )
      {
      nseg = cur.ns_cu;
      for ( i_seg=0; i_seg<nseg; ++i_seg ) 
      GEtfseg_to_local(segarr[0]+i_seg,&lklsyi,segarr[0]+i_seg);
      }
    status = EXecur(id,&cur,segarr[0],segarr[0],pnp);
    if ( status < 0 )
      {
      sprintf(errbuf,"EXecur 1%%EXcugl*fanga");
      return(erpush("SU2943",errbuf));
      }
    }

  if ( segarr[1] != NULL )
    {
    if ( lsyspk != NULL )
      {
      nseg = cur.ns_cu;
      for ( i_seg=0; i_seg<nseg; ++i_seg ) 
      GEtfseg_to_local(segarr[1]+i_seg,&lklsyi,segarr[1]+i_seg);
      }
    status = EXecur(id,&cur,segarr[1],segarr[1],pnp);
    if ( status < 0 )
      {
      sprintf(errbuf,"EXecur 2%%EXcugl*fanga");
      return(erpush("SU2943",errbuf));
      }
    }

  if ( segarr[2] != NULL )
    {
    if ( lsyspk != NULL )
      {
      nseg = cur.ns_cu;
      for ( i_seg=0; i_seg<nseg; ++i_seg ) 
      GEtfseg_to_local(segarr[2]+i_seg,&lklsyi,segarr[2]+i_seg);
      }
    status = EXecur(id,&cur,segarr[2],segarr[2],pnp);
    if ( status < 0 )
      {
      sprintf(errbuf,"EXecur 3%%EXcugl*fanga");
      return(erpush("SU2943",errbuf));
      }
    }

  if ( segarr[3] != NULL )
    {
    if ( lsyspk != NULL )
      {
      nseg = cur.ns_cu;
      for ( i_seg=0; i_seg<nseg; ++i_seg ) 
      GEtfseg_to_local(segarr[3]+i_seg,&lklsyi,segarr[3]+i_seg);
      }
    status = EXecur(id,&cur,segarr[3],segarr[3],pnp);
    if ( status < 0 )
      {
      sprintf(errbuf,"EXecur 4%%EXcugl*fanga");
      return(erpush("SU2943",errbuf));
      }
    }

  if ( segarr[4] != NULL )
    {
    if ( lsyspk != NULL )
      {
      nseg = cur.ns_cu;
      for ( i_seg=0; i_seg<nseg; ++i_seg ) 
      GEtfseg_to_local(segarr[4]+i_seg,&lklsyi,segarr[4]+i_seg);
      }
    status = EXecur(id,&cur,segarr[4],segarr[4],pnp);
    if ( status < 0 )
      {
      sprintf(errbuf,"EXecur 5%%EXcugl*fanga");
      return(erpush("SU2943",errbuf));
      }
    }

  if ( segarr[5] != NULL )
    {
    if ( lsyspk != NULL )
      {
      nseg = cur.ns_cu;
      for ( i_seg=0; i_seg<nseg; ++i_seg ) 
      GEtfseg_to_local(segarr[5]+i_seg,&lklsyi,segarr[5]+i_seg);
      }
    status = EXecur(id,&cur,segarr[5],segarr[5],pnp);
    if ( status < 0 )
      {
      sprintf(errbuf,"EXecur 6%%EXcugl*fanga");
      return(erpush("SU2943",errbuf));
      }
    }

/*!                                                                 */
/* Deallocation of memory for all FANGA curves.                     */
/* Call of function DBfree_srep_curves.                                         */
/*                                                                 !*/

    DBfree_srep_curves(segarr);

    igrsma();


err3:
    DBfree_patches(&sur,patpek);
/*
***Slut.
*/
    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short EXcged(
       DBId     *id,
       DBId     *rid,
       char     *metod,
       DBVector *p1,
       DBVector *p2,
       V2NAPA   *pnp)

/*      Skapa CUR_GEODESIC.
 *
 *      In: id     => Pekare till kurvans identitet.
 *          rid    => Pekare till refererad ytas id.
 *          metod  => "1POS" eller "2POS".
 *          p1,p2  => Start- och slut-position i UV-planet.
 *          pnp    => Pekare till attribut.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          EX1402 => Refererad storhet finns ej.
 *          EX1412 => Refererad storhet har fel typ.
 *
 *      (C)microform ab 5/3/95 J. Kjellander
 *      1996-01-01 gtype tillagt Gunnar Liden
 *      1999-12-18 sur106->varkon_sur_geodesic G Liden
 *
 ******************************************************!*/

  {
    short   i,status;
    DBptr   la;
    DBetype typ;
    DBint   acc;
    char    errbuf[V3STRLEN+1];
    DBPatch  *patpek;
    DBSurf   sur;
    DBCurve   cur;
    DBSeg  *graseg,*geoseg;
    DBint    gtype;              /* Computation case                        */
                                 /* Eq. 1: Start U,V point and tangent      */

/*
***H{mta geometri-data f|r refererad yta. Minne f|r patchar
***allokeras av DBread_patches().
*/
    if ( DBget_pointer('I',rid,&la,&typ) < 0 ) return(erpush("EX1402",""));

    switch ( typ )
      {
      case SURTYP:
      if ( (status=DBread_surface(&sur,la)) < 0 ) return(status);
      if ( (status=DBread_patches(&sur,&patpek)) < 0 ) return(status);
      break;

      default:
      igidst(rid,errbuf);
      return(erpush("EX1412",errbuf));
      }
/*
***Bilda geometrisk representation.
*/
    p1->x_gm += 1.0;
    p1->y_gm += 1.0;

    acc = 4;

   if       ( sticmp(metod, "1POS" ) == 0 ) gtype  = 1;
   else if  ( sticmp(metod, "2POS" ) == 0 ) gtype  = 2;
   else
     {
     gtype  = -1;                        
     }

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "exe22*EXcged metod %s \n", metod );
  fprintf(dbgfil(EXEPAC),
  "exe22*EXcged &sur %d patpek %d la %d \n", (int)&sur, (int)patpek, (int)la );
  fprintf(dbgfil(EXEPAC),
  "exe22*EXcged p1 %f %f %f\n", p1->x_gm, p1->y_gm, p1->z_gm );
  fprintf(dbgfil(EXEPAC),
  "exe22*EXcged p2 %f %f %f\n", p2->x_gm, p2->y_gm, p1->z_gm );
  fprintf(dbgfil(EXEPAC),
  "exe22*EXcged acc %d gtype %d \n", (int)acc, (int)gtype );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

    status = varkon_sur_geodesic
          (&sur,patpek,la,p1,p2,gtype,acc,&cur,&geoseg);
    if ( status < 0 && geoseg  == NULL ) goto err3;
    if ( status < 0 && geoseg  != NULL ) goto err2;

/*
***Fyll i subtyp och ytans DB-adress i UV-segmenten.
*/
    for ( i=0; i<cur.ns_cu; ++ i)
      {
      (geoseg+i)->subtyp   = 1;
      (geoseg+i)->spek_gm  = la;
      (geoseg+i)->spek2_gm =  0;  
      }
/*
***Bilda grafisk representation.
*/
    status = varkon_sur_scur_gra(&cur,geoseg,&graseg);
    if ( status < 0 ) goto err2;
/*
***Lagra cur-posten i DB och rita.
*/
    status = EXecur(id,&cur,graseg,geoseg,pnp);
    if ( status < 0 ) goto err1;
/*
***L{mna tillbaks dynamiskt allokerat minne f|r patch-data.
*/
err1:
    DBfree_segments(graseg);
err2:
    DBfree_segments(geoseg);
err3:
    DBfree_patches(&sur,patpek);
/*
***Slut.
*/
    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short EXcapp(
       DBId    *id,
       DBId    *rid,
       char    *segtyp,
       DBint    metod,
       DBfloat  tol1,
       DBfloat  tol2,
       V2NAPA  *pnp)

/*      Skapa CUR_APPROX.
 *
 *      In: id     => Pekare till kurvans identitet.
 *          rid    => Pekare till refererad kurvas id.
 *          typ    => Tills vidare bara "CUB_SEG".
 *          metod  => 1.
 *          tol1   => Tolerens 1.
 *          tol2   => Tolerens 2.
 *          pnp    => Pekare till attribut.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          EX1402 => Refererad storhet finns ej.
 *          EX1412 => Refererad storhet har fel typ.
 *          EX4162 => Ok{nd segtyp.
 *          EX4172 => Ok{nd metod.
 *
 *      (C)microform ab 5/3/95 J. Kjellander
 *      1995-12-31 Anrop av sur722 tillagt Gunnar Liden
 *      1996-09-30 No transformation &
 *                 deallocation non uv curve Gunnar Liden
 *      1996-10-12 metod=2 tillagt           Gunnar Liden
 *      1998-02-28 CIRCLE a_case= 3 added    Gunnar Liden
 *      1999-12-18 sur722->varkon_cur_approx Gunnar Liden
 *
 ******************************************************!*/

  {
    short   status;
    DBptr   la;
    DBetype typ;
    char    errbuf[V3STRLEN+1];
    DBCurve   oldcur;
    DBSeg  *oldgra,*oldgeo;
  DBCurve  newcur;       /* Output curve                            */
  DBSeg   *newgeo;       /* Output geom. & graphic segments (ptr)   */
  DBint    c_type;       /* Type of calculation                     */
                         /* Eq. 1: No of output segments is input   */
                         /* Eq. 2: Add only segments                */
                         /* Eq. 3: Try to reduce no of segments     */
  DBint    n_req;        /* Requested number for c_type             */
  DBint    t_incur;      /* Type of input curve (what to use)       */
                         /* Eq. 1: Points (segment end points)      */
                         /* Eq. 2: Lines                            */
                         /* Eq. 3: Points and tangents              */
                         /* Eq. 4: Points, tangents and curvature   */

  DBint    a_case;       /* Approximation case                      */
                         /* Eq. 1: Approximate to LINE   segments   */
                         /* Eq. 2: Approximate to CUBIC  segments   */
                         /* Eq. 3: Approximate to CIRCLE segments   */
                         /* Eq. 4: Approximate to CONIC  segments   */
                         /* Eq. 5: Approximate to RATCUB segments   */

  DBfloat  d_che;        /* Arclength check distance                */

                         /* Input tolerances ( < 0 ==> default )    */
  DBfloat  in_ctol;      /* Coordinate       tolerance              */
  DBfloat  in_ntol;      /* Tangent          tolerance (degrees)    */
  DBfloat  in_rtol;      /* Curvature radius tolerance              */

/*
***H{mta geometri-data f|r refererad kurva.
*/
    if ( DBget_pointer('I',rid,&la,&typ) < 0 ) return(erpush("EX1402",""));

    switch ( typ )
      {
      case CURTYP:
      DBread_curve(&oldcur,&oldgra,&oldgeo,la);
      break;

      default:
      igidst(rid,errbuf);
      return(erpush("EX1412",errbuf));
      }
/*
***Kolla segmenttyp.
*/
   if       ( sticmp(segtyp,"CUB_SEG") == 0 ) a_case = 2;
   else if  ( sticmp(segtyp,"CUBIC"  ) == 0 ) a_case = 2;
   else if  ( sticmp(segtyp,"CIRCLE" ) == 0 ) a_case = 3;
   else if  ( sticmp(segtyp,"CIR_SEG" )== 0 ) a_case = 3;
   else if  ( sticmp(segtyp,"RAT_SEG") == 0 ) a_case = 5;
   else if  ( sticmp(segtyp,"RATCUB" ) == 0 ) a_case = 5;
   else
     {
     status = erpush("EX4162",segtyp);
     goto err3 ;
     }
/*
***Kolla metod.
*/
   if       ( metod == 1 )
     {
     c_type = 1;
     n_req   = (DBint)floor(tol1+0.001);
     t_incur = 3;
     in_ctol = F_UNDEF;       
     in_ntol = F_UNDEF;       
     in_rtol = -0.987654321; 
     d_che   = F_UNDEF;       
     }
   else if  ( metod == 2 )
     {
     c_type  =      2;
     n_req   =      7;  /* Number of check points for d_che < 0 */
     t_incur =      3;
     in_ctol =   tol1;          
     if  ( 3 == a_case ) 
       {
       in_ntol = F_UNDEF;
       in_rtol = tol2; 
       }
     else                
       {
       in_ntol =   tol2;          
       in_rtol = -0.987654321; 
       }
     d_che   =  -1.23456789;  
     }
   else if  ( metod == 3 )
     {
     c_type  =      3;
     n_req   = -12345;
     t_incur =      3;
     in_ctol =   tol1;          
     in_ntol =   tol2;          
     d_che   =   30.0;  /* Borde kunna vara negativ, dvs optionell */
     in_rtol = -0.987654321; 
     }
   else
     {
     sprintf(errbuf,"%d",(int)metod);
     status = erpush("EX4152",errbuf);
     goto err3 ;
     }

  status = varkon_cur_approx
  (&oldcur,oldgeo, c_type, n_req, t_incur, 
   a_case, in_ctol, in_ntol, in_rtol , d_che, 
   &newcur, &newgeo );

#ifdef  DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "exe22*EXcapp status %d newgeo %d \n",(int)status , (int)newgeo );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

  if ( status < 0 && newgeo  == NULL ) goto err3;
  if ( status < 0 && newgeo  != NULL ) goto err2;

/*
***Grafiska representation {r lika med geometriska segmenten. 
***Antalet grafiska segment ges som data h{r (detta kunde eventuellt
***ske i varkon_cur_approx. vilket {r snyggast ? Om pekaren till geometriska
***segmenten inte {r lika med pekaren till grafiska segmenten (som
***f|r ytkurvor) fungerar annars inte TCOPY (exe18) ...
*/

  newcur.nsgr_cu = newcur.ns_cu;


/*
***Lagra cur-posten i DB och rita.
*/
    status = EXecur(id,&newcur,newgeo,newgeo,pnp);
    if ( status < 0 ) goto err1;

#ifdef  DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "exe22*EXcapp Back from EXecur  \n" );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif


/*!                                                                 */
/* Deallocation of memory for the approximative curve               */
/* Calls of function DBfree_srep_curves.                                        */
/*                                                                 !*/

err1:
err2:
    DBfree_segments(newgeo);
err3:

#ifdef  DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "exe22*EXcapp Memory deallocated for newgeo (approx. curve)\n" );
  fprintf(dbgfil(EXEPAC),
  "exe22*EXcapp Deallocate for oldgra %d and oldgeo %d\n",
                               (int)oldgra,  (int)oldgeo  );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

/*
***L{mna tillbaks dynamiskt allokerat minne.
*/

    if  (  oldgra == oldgeo )  /* Non-surface curve */
      {
      DBfree_segments(oldgeo);
      }
    else
      {
      DBfree_segments(oldgra);
      DBfree_segments(oldgeo);
      }
/*
***Slut.
*/
    return(status);
  }
  
/********************************************************/
