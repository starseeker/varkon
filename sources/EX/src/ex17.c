/*!*******************************************************
*
*    ex17.c
*    ======
*
*    EXesur();     Create surface
*    EXssar();     Create SUR_SPLARR
*    EXscar();     Create SUR_CONARR
*    EXsnar();     Create SUR_NURBSARR
*
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
#include "../../WP/include/WP.h"
#include "../include/EX.h"

#ifdef WIN32
#include  <windows.h>
#endif

#ifdef V3_OPENGL
#include  <GL/gl.h>
#else
#define GLfloat float
#endif
/*
*******************************************************!*/

extern DBTmat *lsyspk;     /* Active coord. system                  */
extern DBTmat  lklsyi;     /* Active coord. system, inverted        */
extern DBptr   lsysla;     /*                                       */

/* varkon_sur_splarrsur     * Create a SUR_SPLARR   surface         */
/* varkon_sur_nurbsarr      * Create a NURBS_SPLARR surface         */
/* varkon_sur_bictra        * Transformation of a surface           */
/* varkon_sur_creloft       * Create conic lofting LFT_SUR surface  */
/* varkon_sur_graphic       * Graphical curve represen. for surface */
/* varkon_sur_granurbs      * Graphical NURBS represen. for surface */

extern short   igptma();   /* String to message zone (buffer)       */
extern short   igplma();   /* String to message zone (buffer)       */
extern short   igrsma();   /* Delete string in message buffer       */
extern short   igidst();   /* Convert # identity to a string        */
extern short   erinit();   /* Initial. error message buffer         */
extern short   erpush();   /* Error message to buffer               */
extern short   gpdrsu();   /* Draw surface                          */

extern short gptrty;       /*                                       */

/*!******************************************************/

       short EXesur(
       DBId    *id,
       DBSurf   *surpek,
       DBPatch   *patpek,
       V2NAPA  *pnp)

/*      Skapar yta.
 *
 *      In: id     => Pekare till identitet.
 *          surpek => Pekare till DBSurf-post.
 *          patpek => Pekare till topologiska patchnätet.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX4032 = Kan ej lagra yta i DB.
 *
 *      (C)microform ab 1/3/93 J. Kjellander
 *
 *      24/1-95    Multifönster, J. Kjellander
 *      1997-03-05 B-spline app. J.Kjellander
 *      1997-05-17 Blank, J.Kjellander
 *      1997-11-04 NURBS, J.Kjellander
 *      1997-12-11 NURBS, G Liden
 *      1999-12-18 sur962->varkon_sur_graphic 
 *                 sur963->varkon_sur_granurbs  G Liden
 *
 ******************************************************!*/

  {
    short    status;
    DBptr    la;
    DBSeg   *sptarr[6];
    GLfloat *p_kvu,*p_kvv,*p_cpts;

/*
***Fyll i attribut etc.
*/
    surpek->hed_su.blank = pnp->blank;   /* Blank/Unblank */
    surpek->hed_su.pen   = pnp->pen;     /* Penn-nummer */
    surpek->hed_su.level = pnp->level;   /* Nivå-nummer */
    surpek->fnt_su       = pnp->sfont;   /* Font */
    surpek->lgt_su       = pnp->sdashl;  /* Strecklängd */
    surpek->ngu_su       = pnp->nulines; /* Antal "linjer" i U-riktn. */
    surpek->ngv_su       = pnp->nvlines; /* Antal "linjer" i V-riktn. */
    surpek->pcsy_su      = lsysla;       /* DB-pekare till aktivt ksys*/
/*
***Lagra i DB.
*/
    igptma(198,IG_MESS);
    if ( pnp->save )
      {
      surpek->hed_su.hit = pnp->hit;
      if ( DBinsert_surface(surpek,patpek,id,&la)< 0 )return(erpush("EX4032",""));
      }
    else surpek->hed_su.hit = 0;
    igrsma();
/*
***Om tänd, skapa grafisk representation.
*/
    sptarr[0]=sptarr[1]=sptarr[2]=sptarr[3]=sptarr[4]=sptarr[5]=NULL;

    if ( !surpek->hed_su.blank )
      {
      switch ( surpek->typ_su )
        {
/*
***Facett-yta och Box-yta har implicit grafisk representation.
*/
        case FAC_SUR:
        case BOX_SUR:
        gpdrsu(surpek,sptarr,la,DRAW);
#ifdef V3_X11
       if ( gptrty == X11 ) wpdrsu(surpek,sptarr,la,GWIN_ALL);
#endif
        break;
/*
***Övriga ytor, först Tråd-representationen.
*/
        default:
        igptma(455,IG_MESS);
        status = varkon_sur_graphic(surpek,patpek,la,1,sptarr);
        if ( status < 0 ) return(status);
        DBadd_srep_curves(surpek,sptarr);
        DBupdate_surface(surpek,la);
/*
***Sen Nurbs-representationen.
*/
        status = varkon_sur_granurbs
                 (surpek,patpek,la,1, &p_kvu,&p_kvv,&p_cpts);
        if ( status < 0 ) return(status);
        DBadd_srep_NURBS(surpek,p_kvu,p_kvv,p_cpts);
        DBupdate_surface(surpek,la);
        DBfree_srep_NURBS(p_kvu,p_kvv,p_cpts);
        igrsma();
/*
***Visa trådrepresentationen.
*/
        igptma(456,IG_MESS);
        gpdrsu(surpek,sptarr,la,DRAW);
        igrsma();
#ifdef V3_X11
       if ( gptrty == X11 ) wpdrsu(surpek,sptarr,la,GWIN_ALL);
#endif
/*
***Lämna tillbaks minne allokerat för trådar.
*/
        DBfree_srep_curves(sptarr);
        break;
        }
      }
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

       short EXssar(
       DBId     *id,
       char     *metod,
       DBint     nu,
       DBint     nv,
       DBVector *p,
       DBVector *u_tan,
       DBVector *v_tan,
       DBVector *twist,
       V2NAPA   *pnp)

/*      Skapar SUR_SPLARR.
 *
 *      In:  id      => Pekare till ytans identitet.
 *           metod   => Adini etc.
 *           nu,nv   => Antal patchar i U- och V-riktn.
 *           p       => Pekare till positioner.
 *           u_tan   => Pekare till U-tangenter eller NULL.
 *           v_tan   => Pekare till V-tangenter eller NULL.
 *           twist   => Pekere till twist-vektorer eller NULL.
 *           pnp     => Pekare till attribut.
 *
 *      Ut:  Inget.
 *
 *      FV:       0 => Ok.
 *
 *      (C)microform ab 1/3/95 J. Kjellander
 *
 *      95-03-30 sur294, G. Liden
 *      95-04-11 p_vtan, p_twist argument EXssar, G Liden
 *      95-06-24 Ordering of points, G Liden
 *      18/9/95  Uppstädning, J. Kjellander
 *      29/9/95  Nytt anrop sur294(), J. Kjellander
 *      19/4/97  Transformera yta     G Liden
 *      1999-12-18 sur294->varkon_sur_splarr 
 *                 sur600->varkon_sur_bictra  G. Liden
 *
 ******************************************************!*/

  {
   DBSurf  sur;
   DBPatch *ptpat;
   short  status;

/*
***Et meddelande.
*/
   igptma(200,IG_MESS);
/*
***Skapa ytan.
*/
   status = varkon_sur_splarr(metod,nu,nv,p,v_tan,u_tan,twist,
                              &sur,&ptpat);

   if      ( status < 0  &&  ptpat == NULL ) goto error3;
   else if ( status < 0 )                    goto error2;
/*
***Transformera yta om lokalt system är aktivt
*/
   if ( lsyspk != NULL )
     { 
     if ( (status=varkon_sur_bictra(&sur,ptpat,&lklsyi)) < 0 ) 
                            goto error2;
     }
/*
***Lagra i DB och rita.
*/
   igrsma();
   status = EXesur(id,&sur,ptpat,pnp);
   if ( status < 0 ) goto error2;
/*
***Allt verkar ha gått bra !
*/
   status = 0;
/*
***Lämna tillbaks dynamiskt allokerat minne.
*/
error2:
   DBfree_patches(&sur,ptpat);

error3:
/*
***Slut.
*/
   return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short EXscar(
       DBId    *id,
       char    *param_case,
       DBId    *spine,
       DBint    ncur,
       DBId    *lim,
       DBId    *tan,
       DBint   *metod,
       DBId    *mid,
       V2NAPA  *pnp)

/*      Skapar SUR_CONARR.
 *
 *      In:  id         => Pekare till ytans identitet.
 *           param_case => Parameter fall 
 *           ncur       => Antal limitkurvor.
 *           spine      => Spinekurvans identitet.
 *           lim,tan    => Referenser till lim- och tan-kurvor.
 *           metod      => "P" eller "M", ncur-1 st.
 *           mid        => Referenser till PV- eller Mid-kurvor.
 *           pnp        => Pekare till attribut.
 *
 *      Ut:  Inget.
 *
 *      FV:       0 => Ok.
 *
 *      (C)microform ab 1/3/95 J. Kjellander
 *
 *      1995-10-04 param_case, G Liden
 *      14/9/95    Deklarerat param_case, J. Kjellander
 *      1999-12-18 sur800->varkon_sur_creloft G. Liden
 *
 ******************************************************!*/

  {
   short   status;
   int     i;
   DBptr   la;
   DBetype   typ;
   char    errbuf[V3STRLEN+1];
   DBCurve spicur,limcur[MXLSTP+1],tancur[MXLSTP+1],midcur[MXLSTP];
   DBSeg  *spiseg,*limseg[MXLSTP+1],*tanseg[MXLSTP+1],*midseg[MXLSTP];
   DBSurf   sur;
   DBPatch  *ptpat;

/*
***Et meddelande.
*/
   igptma(451,IG_MESS);
/*
***Hämta geometri-data för spine.
*/
   if ( DBget_pointer('I',spine,&la,&typ) < 0 ) return(erpush("EX1402",""));
   if ( typ != CURTYP )
     {
     igidst(spine,errbuf);
     return(erpush("EX1412",errbuf));
     }
   DBread_curve(&spicur,NULL,&spiseg,la);
/*
***Hämta limit-kurvor.
*/
   for ( i=0; i<ncur; ++i )
     {
     if ( DBget_pointer('I',(lim+i),&la,&typ) < 0 ) return(erpush("EX1402",""));
     if ( typ != CURTYP )
       {
       igidst((lim+i),errbuf);
       return(erpush("EX1412",errbuf));
       }
     DBread_curve(&limcur[i],NULL,&limseg[i],la);
     }
/*
***Hämta tangent-kurvor.
*/
   for ( i=0; i<ncur; ++i )
     {
     if ( DBget_pointer('I',(tan+i),&la,&typ) < 0 ) return(erpush("EX1402",""));
     if ( typ != CURTYP )
       {
       igidst((tan+i),errbuf);
       return(erpush("EX1412",errbuf));
       }
     DBread_curve(&tancur[i],NULL,&tanseg[i],la);
     }
/*
***Hämta mitt-kurvor.
*/
   for ( i=0; i<ncur-1; ++i )
     {
     if ( DBget_pointer('I',(mid+i),&la,&typ) < 0 ) return(erpush("EX1402",""));
     if ( typ != CURTYP )
       {
       igidst((mid+i),errbuf);
       return(erpush("EX1412",errbuf));
       }
     DBread_curve(&midcur[i],NULL,&midseg[i],la);
     }
/*
***Skapa ytan.
*/
   status = varkon_sur_creloft
                 (&spicur,spiseg,(DBint)(ncur-1),limcur,limseg,
                   tancur,tanseg,midcur,midseg,metod,&sur,&ptpat);

   if      ( status < 0 && ptpat == NULL ) goto error3;
   else if ( status < 0 )                  goto error2;
/*
***Lagra i DB och rita.
*/
   igrsma();
   status = EXesur(id,&sur,ptpat,pnp);
   if ( status < 0 ) goto error2;
/*
***Allt verkar ha gått bra !
*/
   status = 0;
/*
***Lämna tillbaks dynamiskt allokerat minne.
*/
   DBfree_segments(spiseg);
   for ( i=0; i<ncur; ++i ) DBfree_segments(limseg[i]);
   for ( i=0; i<ncur; ++i ) DBfree_segments(tanseg[i]);
   for ( i=0; i<ncur-1; ++i ) DBfree_segments(midseg[i]);
error2:
   DBfree_patches(&sur,ptpat);
/*
***Slut.
*/
error3:
   return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short EXsnar(
       DBId     *id,
       DBint     order_u,
       DBint     order_v,
       DBint     nk_u,
       DBint     nk_v,
       DBVector *cpts,
       DBfloat  *uknots,
       DBfloat  *vknots,
       DBfloat  *w,
       V2NAPA   *pnp)

/*      Skapar SUR_NURBSARR.
 *
 *      In:  id      => Pekare till ytans identitet.
 *           order_u => Ordnig i U-led
 *           order_v => Ordnig i V-led
 *           nk_u    => Antal knutpunkter i U-led
 *           nk_v    => Antal knutpunkter i V-led
 *           cpts    => Pekare till styrpolygon
 *           uknots  => Pekare till knutpunkter i U-led
 *           vknots  => Pekare till knutpunkter i V-led
 *           w       => Pekare till rationell nämnare
 *           pnp     => Pekare till attribut.
 *
 *      Ut:  Inget.
 *
 *      FV:       0 => Ok.
 *
 *      (C)microform ab 1997-11-25 J. Kjellander
 *
 *       1997-11-27   sur890       G. Liden
 *       1999-12-18   sur890->varkon_sur_nurbsarr G. Liden
 *
 ******************************************************!*/

  {
   short  status;
   int    i,np_u,np_v;
   DBSurf  sur;
   DBPatch *ptpat;

/*
***Antal styrpunkter.
*/
   np_u = nk_u - order_u;
   np_v = nk_v - order_v;
/*
***Transformera styrpolygon om lokalt system är aktivt.
*/
   if ( lsyspk != NULL )
     { 
     for ( i=0; i<np_u*np_v; ++i ) GEtfpos_to_local(cpts+i,&lklsyi,cpts+i);
     }
/*
***Skapa ytan.
**/
   status = varkon_sur_nurbsarr(order_u, order_v, nk_u, nk_v, cpts,
                   uknots, vknots, w ,&sur,&ptpat);

   if      ( status < 0  &&  ptpat == NULL ) goto error3;
   else if ( status < 0 )                    goto error2;
/*
***Lagra i DB och rita.
*/
   status = EXesur(id,&sur,ptpat,pnp);
   if ( status < 0 ) goto error2;
/*
***Allt verkar ha gått bra !
*/
   status = 0;
/*
***Lämna tillbaks dynamiskt allokerat minne.
*/
error2:
   DBfree_patches(&sur,ptpat);

error3:
/*
***Slut.
*/
   return(status);
  }
  
/********************************************************/
