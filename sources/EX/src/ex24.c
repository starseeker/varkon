/*!******************************************************************
*            
*    ex24.c     
*    ======      
*
*    EXscon();     Create SUR_CONIC
*    EXsoff();     Create SUR_OFFS  
*    EXsapp();     Create SUR_APPROX 
*    EXscom();     Create SUR_COMP
*    EXstrm();     Create SUR_TRIM 
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

/* varkon_sur_eval         * Temporary ! Surface evaluation routine */

extern DBTmat *lsyspk;     /* Active coord. system                  */
extern DBTmat  lklsyi;     /* Active coord. system, inverted        */
extern DBptr   lsysla;     /*                                       */

/* varkon_sur_facapprox     * Approximate to  FAC_SUR surface       */
/* varkon_sur_bicapprox     * Approximate to  CUB_SUR surface       */
/* varkon_sur_tobspline     * Approximate to NURB_SUR surface       */
/* varkon_sur_boxapprox     * Approximate surface with patch boxes  */
/* varkon_sur_minmaxbox     * Approximate to BOX_SUR surface        */
/* varkon_sur_offset        * Create an offset  surface             */
/* varkon_sur_trim          * Create a  trimmed surface             */
/* varkon_sur_comp          * Create a composite surface SUR_COMP   */
/* varkon_sur_creloft       * Create conic lofting LFT_SUR surface  */

/*!******************************************************/

       short EXscon(
       DBId    *id,
       DBId    *spine,
       DBint    nstrips,
       DBId    *limarr[],
       DBId    *tanarr[],
       DBint    typarr[],
       DBId    *midarr[],
       V2NAPA  *pnp)

/*      Skapa SUR_CONIC.
 *
 *      In:  id      => Pekare till ytans identitet.
 *           spine   => Pekare till ID f|r spine-kurva.
 *           nstrips => Antal strippar.
 *           limarr  => Array av pekare till ID f|r limit-kurvor.
 *           tanarr  => Array av pekare till ID f|r tangent-kurvor.
 *           typarr  => Array med typer f|r mitt-kurvor. 1=P, 2=M.
 *           midarr  => Array av pekare till ID f|r mitt-kurvor.
 *           pnp     => Pekare till attribut.
 *
 *      Ut:  Inget.
 *
 *      FV:       0 => Ok.
 *           EX1402 => Refererad storhet finns ej.
 *           EX1412 => Refererad storhet har fel typ.
 *
 *      (C)microform ab 26/4/94 J. Kjellander
 *
 *      1994-11-27 Deallokeringen fixad G. Liden
 *      1999-12-18 sur800->varkon_sur_creloft G Liden
 *
 ******************************************************!*/

  {
    short   status;
    DBint   i;
    DBptr   la;
    DBetype typ;
    char    errbuf[V3STRLEN+1];
    DBCurve spicur,limcur[MXLSTP+1],tancur[MXLSTP+1],midcur[MXLSTP];
    DBSeg  *spiseg,*limseg[MXLSTP+1],*tanseg[MXLSTP+1],*midseg[MXLSTP];
    DBSurf   sur;
    DBPatch  *ptpat;

/*
***Et meddelande.
*/
   igptma(197,IG_MESS);
/*
***H{mta geometri-data f|r spine.
*/
   if ( DBget_pointer('I',spine,&la,&typ) < 0 ) return(erpush("EX1402",""));
   if ( typ != CURTYP )
     {
     igidst(spine,errbuf);
     return(erpush("EX1412",errbuf));
     }
   DBread_curve(&spicur,NULL,&spiseg,la);
/*
***H{mta limit-kurvor.
*/
   for ( i=0; i<=nstrips; ++i )
     {
     if ( DBget_pointer('I',limarr[i],&la,&typ) < 0 ) return(erpush("EX1402",""));
     if ( typ != CURTYP )
       {
       igidst(limarr[i],errbuf);
       return(erpush("EX1412",errbuf));
       }
     DBread_curve(&limcur[i],NULL,&limseg[i],la);
     }
/*
***H{mta tangent-kurvor.
*/
    for ( i=0; i<=nstrips; ++i )
      {
      if ( DBget_pointer('I',tanarr[i],&la,&typ) < 0 ) return(erpush("EX1402",""));
      if ( typ != CURTYP )
        {
        igidst(tanarr[i],errbuf);
        return(erpush("EX1412",errbuf));
        }
      DBread_curve(&tancur[i],NULL,&tanseg[i],la);
      }
/*
***H{mta mitt-kurvor.
*/
    for ( i=0; i<nstrips; ++i )
      {
      if ( DBget_pointer('I',midarr[i],&la,&typ) < 0 ) return(erpush("EX1402",""));
      if ( typ != CURTYP )
        {
        igidst(midarr[i],errbuf);
        return(erpush("EX1412",errbuf));
        }
      DBread_curve(&midcur[i],NULL,&midseg[i],la);
      }
/*
***Skapa ytan.
*/
    status = varkon_sur_creloft
              (&spicur,spiseg,(DBint)nstrips,limcur,limseg,
                    tancur,tanseg,midcur,midseg,typarr,&sur,&ptpat);

#ifdef DEBUG
    if ( dbglev(EXEPAC) == 1 )
      {
      fprintf(dbgfil(EXEPAC),
      "exe24*EXscon ptpat= %d status %d \n",
                   (int)ptpat,(short)status );
      fflush(dbgfil(EXEPAC)); /* To file from buffer      */
      }
#endif
    if   ( status < 0 && ptpat == NULL ) 
        {
        goto error3;
        }
    else if ( status < 0 ) 
        {
        goto error2;
        }
/*
***Lagra i DB och rita.
*/
    igrsma();
    status = EXesur(id,&sur,ptpat,pnp);
    if ( status < 0 ) goto error2;
/*
***Allt verkar ha g}tt bra !
*/
    status = 0;
/*
***L{mna tillbaks dynamiskt allokerat minne.
*/
/* 1996-11-13 error1: */
    DBfree_segments(spiseg);
    for ( i=0; i<=nstrips; ++i ) DBfree_segments(limseg[i]);
    for ( i=0; i<=nstrips; ++i ) DBfree_segments(tanseg[i]);
    for ( i=0; i<nstrips;  ++i ) DBfree_segments(midseg[i]);
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

       short EXsoff(
       DBId    *id,
       DBId    *rid,
       DBfloat  offset,
       V2NAPA  *pnp)

/*      Skapa SUR_OFFS.
 *
 *      In:  id      => Pekare till ytans identitet.
 *           rid     => Pekare till ID f|r refererad yta.
 *           offset  => Den nya ytans offset.
 *           pnp     => Pekare till attribut.
 *
 *      Ut:  Inget.
 *
 *      FV:       0 => Ok.
 *           EX1402 => Refererad storhet finns ej.
 *           EX1412 => Refererad storhet har fel typ.
 *
 *      (C)microform ab 26/4/94 J. Kjellander
 *      1999-12-18 sur180->varkon_sur_offset G Liden
 *
 ******************************************************!*/

  {
    short   status;
    DBptr   la;
    DBetype typ;
    char    errbuf[V3STRLEN+1];
    DBSurf   oldsur;
    DBPatch  *oldpat,*dummy;

/*
***Et meddelande.
*/
   igptma(199,IG_MESS);
/*
***H{mta geometri-data f|r refererad yta.
*/
    if ( DBget_pointer('I',rid,&la,&typ) < 0 ) return(erpush("EX1402",""));
    if ( typ != SURTYP )
      {
      igidst(rid,errbuf);
      return(erpush("EX1412",errbuf));
      }
    if ( (status=DBread_surface(&oldsur,la)) < 0 ) return(status);
    if ( (status=DBread_patches(&oldsur,&oldpat)) < 0 ) return(status);
/*
***Skapa ytan.
*/
    status = varkon_sur_offset(&oldsur,oldpat,offset,&oldsur,&dummy);
    if ( status < 0 ) goto error;
/*
***Lagra i DB och rita.
*/
    igrsma();
    status = EXesur(id,&oldsur,oldpat,pnp);
/*
***L{mna tillbaks dynamiskt allokerat minne.
*/
    DBfree_patches(&oldsur,oldpat);
error:
/*
***Slut.
*/
    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short EXsapp(
       DBId    *id,
       DBId    *rid,
       char    *yttyp,
       DBint    metod,
       DBfloat  tol_1,
       DBfloat  tol_2,
       V2NAPA  *pnp)

/*      Skapar SUR_APPROX.
 *
 *      In:  id      => Pekare till ytans identitet.
 *           rid     => Id f|r yta att approximera.
 *           typ     => \nskat resultat.
 *           metod   => Approximationsmetod.
 *                      1 = Anv{ndaren ger antal patchar.
 *                      2 = Funktionen best{mmer antal patchar
 *           tol_1/2 => Antal patchar eller andra tolerenser.
 *           pnp     => Pekare till attribut.
 *
 *      Ut:  Inget.
 *
 *      Felkod: EX4142 = Ok{nd yttyp.
 *              EX4152 = Ok{nd metod.
 *
 *      (C)microform ab 1/3/95 J. Kjellander
 *      1995-03-08  np_u, np_v      G. Liden  
 *      1996-01-06  FAC_SUR added   G. Liden  
 *      1996-01-31  Nya metoder FAC_SUR added   G. Liden  
 *      1996-05-15  BOX_SUR, BOX_PAT    added   G. Liden  
 *      1996-11-18  metod= 4 added              G. Liden  
 *      1996-12-14  Initialisering variabler    G. Liden
 *      1997-02-15  NURBS                       G. Liden
 *      1997-03-16  NURBS deleted               G. Liden
 *      1997-05-16  EXbsur deleted              G. Liden
 *      1997-11-06  sur174/NURB_SUR added       G. Liden
 *      1998-03-21  Missing DEBUG  
 *                  goto err1 and err2 sur170   G. Liden
 *      1999-12-18  surxxx->varkon_sur_yyyy     G. Liden
 *
 ******************************************************!*/

  {
   short    status;
   DBptr    la;
   DBetype  typ;
   DBfloat  slim[2][2],ctol,ntol,rtol;
   DBint    stype,acc;
   char     errbuf[V3STRLEN+1];
   DBSurf   oldsur,newsur;
   DBPatch  *oldpat,*newpat;
   DBint    np_u;         /* Number of approx patches per patch in U */
   DBint    np_v;         /* Number of approx patches per patch in V */
   DBfloat  offset;       /* Surface offset value                    */
   DBfloat  thick;        /* Thickness for the patches               */
   DBint    acase;        /* Approximation case: Ska tas bort ....   */
   DBBplane bpl[BPLMAX];  /* B-planes which define BBOX'es           */
   DBint    nbpl;         /* Number of B-planes                      */
   DBint    i_bpl;        /* Loop index B-plane                      */



#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe24*EXsapp tol_1 %20.10g tol_2 %20.10g \n",
      tol_1 , tol_2  );
fprintf(dbgfil(EXEPAC),
"exe24*EXsapp metod %d yttyp %s\n",
      (int)metod, yttyp  );
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif


/*
***Ett meddelande.
*/
   igptma(452,IG_MESS);


/*
***Initialisera lokala variabler
*/
   acc    = I_UNDEF;
   np_u   = I_UNDEF;
   np_v   = I_UNDEF;
   nbpl   = I_UNDEF;
   i_bpl  = I_UNDEF;
   acase  = I_UNDEF;
   ctol   = F_UNDEF;
   ntol   = F_UNDEF;
   rtol   = F_UNDEF;
   offset = F_UNDEF;
   thick  = F_UNDEF;

/*
***H{mta geometri-data f|r refererad yta.
*/
   if ( DBget_pointer('I',rid,&la,&typ) < 0 ) return(erpush("EX1402",""));
   if ( typ != SURTYP )
     {
     igidst(rid,errbuf);
     return(erpush("EX1412",errbuf));
     }
   if ( (status=DBread_surface(&oldsur,la)) < 0 ) return(status);
   if ( (status=DBread_patches(&oldsur,&oldpat)) < 0 ) return(status);
/*
***Kolla yttyp.
*/
   if      ( sticmp(yttyp,"CUB_SUR")  == 0 ) stype =  CUB_SUR;
   else if ( sticmp(yttyp,"FAC_SUR")  == 0 ) stype =  FAC_SUR;
   else if ( sticmp(yttyp,"BOX_SUR")  == 0 ) stype =  BOX_SUR;
   else if ( sticmp(yttyp,"NURB_SUR") == 0 ) stype = NURB_SUR;
   else if ( sticmp(yttyp,"BOX_PAT")  == 0 ) stype =       99;
   else
     {
     status = erpush("EX4142",yttyp);
     goto err2;  
     }

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 &&  stype ==  CUB_SUR  )
{
fprintf(dbgfil(EXEPAC), "exe24*EXsapp Approximate to a CUB_SUR surface\n");
}
if ( dbglev(EXEPAC) == 1 &&  stype ==  FAC_SUR  )
{
fprintf(dbgfil(EXEPAC), "exe24*EXsapp Approximate to a FAC_SUR surface\n");
}
if ( dbglev(EXEPAC) == 1 &&  stype ==  BOX_SUR  )
{
fprintf(dbgfil(EXEPAC), "exe24*EXsapp Create a BOX_SUR surface  \n");
}
if ( dbglev(EXEPAC) == 1 &&  stype == NURB_SUR  )
{
fprintf(dbgfil(EXEPAC), "exe24*EXsapp Create a NURB_SUR surface \n");
}
if ( dbglev(EXEPAC) == 1 &&  stype ==       99  )
{
fprintf(dbgfil(EXEPAC), "EXe24*EXsapp Approximate to BOX_PAT (BBOX'es)\n");
}

if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"EXe24*EXsapp pnp->blank %d pnp->pen %d pnp->level %d\n",
      (int)pnp->blank,(int)pnp->pen,(int)pnp->level );
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif

/*
***Kolla metod.
*/
   if       ( metod == 1 && stype == CUB_SUR )
     {
     acc  = 1;
     np_u = (DBint)(tol_1+0.00000001);
     np_v = (DBint)(tol_2+0.00000001); 
     ctol = 0.123456789;
     ntol = 0.123456789;
     rtol = 0.123456789;
     }
   else if  ( (metod == 1 || metod == 11) && stype == FAC_SUR )
     {
     np_u = (DBint)(tol_1+0.00000001);
     np_v = (DBint)(tol_2+0.00000001); 
     ctol = 0.02; /* Ers{tt med systemets tolerans om/n{r den ska anv{ndas*/
     ntol = 0.20; /* Ers{tt med systemets tolerans om/n{r den ska anv{ndas*/
     offset = 0.0;     
     thick  = 0.0;     
     acase  = metod;  
     acc    = 1;
     rtol   =  -0.123456789;  /* Not defined                           */
     }
   else if  ( (metod == 3 || metod == 13 || metod == 4) && stype == FAC_SUR )
     {
     np_u = (DBint)(tol_1+0.00000001);
     np_v = np_u;
     ctol = 0.02; /* Ers{tt med systemets tolerans om/n{r den ska anv{ndas*/
     ntol = 0.20; /* Ers{tt med systemets tolerans om/n{r den ska anv{ndas*/
     offset = tol_2;   
     thick  = 0.0;     
     acase  = metod;  
     acc    = 1;
     rtol   =  -0.123456789;  /* Not defined                           */
     }
   else if  ( (metod == 2 || metod == 12) && stype == FAC_SUR )
     {
     np_u = 0;
     np_v = 0; 
     ctol = 0.02; /* Ers{tt med systemets tolerans om/n{r den ska anv{ndas*/
     ntol = 0.20; /* Ers{tt med systemets tolerans om/n{r den ska anv{ndas*/
     offset = tol_1;
     thick  = tol_2;
     acase  = metod;  
     acc    = 1;
     rtol   =  -0.123456789;  /* Not defined                           */
     }
   else if  ( metod == 1 && stype == NURB_SUR )
     {
     np_u = (DBint)(tol_1+0.00000001);
     np_v = (DBint)(tol_2+0.00000001); 
     ctol = 0.02; /* Ersatt med systemets tolerans om/nar den ska anvandas*/
     ntol = 0.20; /* Ersatt med systemets tolerans om/nar den ska anvandas*/
     offset = 0.0;     
     thick  = 0.0;     
     acase  = metod;  
     acc    = 1;
     rtol   =  -0.123456789;  /* Not defined                           */
     }
   else if  ( stype ==  BOX_SUR) ; /* No parameters for the moment */
   else if  ( stype ==      99 ) ; /* No parameters for the moment */
   else
     {
     if      ( stype == CUB_SUR ) sprintf(errbuf,"%d - CUB_SUR",(int)metod);
     else if ( stype == FAC_SUR ) sprintf(errbuf,"%d - FAC_SUR",(int)metod);
     else if ( stype == NURB_SUR )sprintf(errbuf,"%d - NURB_SUR",(int)metod);
     else                         sprintf(errbuf,"%d"          ,(int)metod);
     status = erpush("EX4152",errbuf);
     goto err2;
     }
/*
***Skapa nya ytan.
*/
   slim[0][0] = slim[0][1] = slim[1][0] = slim[1][1] = 0.0;

   if        ( stype == CUB_SUR )
     {
     status = varkon_sur_bicapprox(&oldsur,oldpat,slim,metod,np_u,np_v,
                      acc,ctol,ntol,rtol, &newsur,&newpat);
     if ( status < 0 && newpat == NULL ) goto err2;
     if ( status < 0 ) goto err1;
     }
   else if   ( stype == FAC_SUR )
     {
     status = varkon_sur_facapprox
     (&oldsur,oldpat,offset,thick,slim,acase, np_u,np_v,
        acc,ctol,ntol,rtol, &newsur,&newpat); 
     if ( status < 0 && newpat == NULL ) goto err2;
     if ( status < 0 ) goto err1;
     }
   else if   ( stype == BOX_SUR )
     {
     status = varkon_sur_minmaxbox
     (&oldsur,oldpat, &newsur,&newpat); 
     }
   else if   ( stype == NURB_SUR )
     {
    status = varkon_sur_tobspline 
          (&oldsur, oldpat, slim,metod,np_u,np_v,acc,
          ctol,ntol,rtol, &newsur, &newpat);
     if ( status < 0 && newpat == NULL ) goto err2;
     if ( status < 0 ) goto err1;
     }
   else if   ( stype ==      99 )
     {
     status = varkon_sur_boxapprox (&oldsur,oldpat, bpl ,&nbpl  ); 
     if ( status < 0 ) goto err2;
     for ( i_bpl=0; i_bpl < nbpl ; ++i_bpl )
       {
       status= EXebpl(id,&bpl[i_bpl],pnp); 
       if ( status < 0 ) goto err2;
       }
     goto err2;      
     }


/*
* Tillfalligt för test av NURB_SUR evalueringsrutinen Start .......
*/

#ifdef  TILLFALLIG

  EVALS  r00;  /* Tillfalligtorner point U=0 V=0                    */

if  ( stype == NURB_SUR )
{
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe24*EXsapp &newsur %d newpat %d \n",
      (int)(&newsur),(int)newpat);
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif

   status= varkon_sur_eval (&newsur,newpat, 3   ,1.4 ,1.2 ,&r00);
    if(status<0)
    {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  fprintf(dbgfil(EXEPAC),
  "ex24   varkon_sur_eval (sur210) failed for us 1.4 vs 1.2\n" ); 
#endif
    sprintf(errbuf,"exe24");
    return(varkon_erpush("SU2943",errbuf));
    }

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
  "ex24   us 2.4   vs 3.8   X %10.2f Y %10.2f Z %10.2f \n",
     r00.r_x, r00.r_y, r00.r_z ); 
fflush(dbgfil(EXEPAC));
  }
#endif

}

/*
* Tillfalligt för test av NURB_SUR evalueringsrutinen Slut  .......
*/

#endif /*  TILLFALLIG  */




/*
***Lagra i DB och rita.
*/
   status = EXesur(id,&newsur,newpat,pnp);
/*
***L{mna tillbaks dynamiskt allokerat minne.
*/
err1: 
   DBfree_patches(&newsur,newpat);
err2: 
   DBfree_patches(&oldsur,oldpat);

   igrsma();

   return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short EXscom(
       DBId    *id,
       DBId    *ridvek,
       DBint    nref,
       V2NAPA  *pnp)

/*      Skapar SUR_COMP.
 *
 *      In:  id      => Pekare till ytans identitet.
 *           ridvek  => Pekare till array av referenser.
 *           nref    => Antal referenser.
 *           pnp     => Pekare till attribut.
 *
 *      Ut:  Inget.
 *
 *      FV:       0 => Ok.
 *
 *      (C)microform ab 1/3/95 J. Kjellander
 *      1996-05-04 Gunnar Liden
 *      1996-11-13 Check number of input surfaces Gunnar Liden
 *      1997-02-08 Debug                          Gunnar Liden
 *      1997-02-26 short->int for nref            Gunnar Liden
 *      1999-12-18 sur400->varkon_sur_comp        Gunnar Liden
 ******************************************************!*/

  {

   DBId    *rid_surf;              /* Current surface     (# id)   */
   DBptr    la_surf;               /* Current surface     (DB ptr) */
   DBetype  typ_surf;              /* Current surface type         */
   DBint    i_sur;                 /* Loop index surface           */
   DBint    i_sur2;                /* For checking ...             */
   DBptr    la_surf2;              /* For checking ...    (DB ptr) */

                                   /* For varkon_sur_comp:         */
   DBSurf    sur_all[MXSCOMP];      /* All surfaces                 */
   DBPatch   *p_pat_all[MXSCOMP];    /* All patch data        (ptrs) */
   DBint    no_surf;               /* Number of input surfaces     */
   DBint    c_case;                /* Case (not yet used)          */
   DBSurf    sur_out;               /* Output surface               */
   DBPatch   *p_pat_out;             /* Output surface patch data    */

   short   status;                 /* Error code                   */
   char    errbuf[80];             /* For error message            */

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe24*EXscom Fortfarande fel! Number of surfaces nref= %d\n", (int)nref);
fflush(dbgfil(EXEPAC));
}
#endif

    if ( nref < 2 )
     {
     erinit();
     sprintf(errbuf,"%d%%exe24 ", (int)nref);
     return(erpush("SU4013",errbuf));
     }

igplma("Skapar SUR_COMP !",IG_MESS);

   status  = 0;
   no_surf = (DBint)nref;  /* Number of input surfaces             */
   c_case  = -1;           /* Composite surface case (not yet used)*/

/*!                                                                 */
/* Check if any of the surfaces are identical                       */
/*                                                                 !*/

for ( i_sur=1; i_sur <= no_surf; ++i_sur )
  {
  for ( i_sur2=1; i_sur2 <= no_surf; ++i_sur2 )
    {
    if   (  i_sur == i_sur2 ) goto nxtsur;

    rid_surf = &ridvek[i_sur-1];
    if ( DBget_pointer('I',rid_surf,&la_surf,&typ_surf) < 0 ) 
    return(erpush("EX1402",""));
    rid_surf = &ridvek[i_sur2-1];
    if ( DBget_pointer('I',rid_surf,&la_surf2,&typ_surf) < 0 ) 
    return(erpush("EX1402",""));

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe24*EXscom i_sur %d i_sur2 %d la_surf %d la_surf %d\n", 
           (int)i_sur, (int)i_sur2, (int)la_surf, (int)la_surf2 );
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif

    if ( la_surf == la_surf2 )
     {
     erinit();
     sprintf(errbuf,"%d%%%d ",(int)i_sur, (int)i_sur2);
     return(erpush("SU4033",errbuf));
     }

nxtsur:;
    }
  }

/*
***H{mta geometri-data f|r refererade ytor.
*/

/*!                                                                 */
/* Loop all surfaces i_sur= 1,....,no_surf                          */
/*                                                                 !*/

for ( i_sur=1; i_sur <= no_surf; ++i_sur ) /* Start loop surfaces   */
  {

/*!                                                                 */
/*   Pointer (# id) rid_surf for current surface from input array   */
/*                                                                 !*/

     rid_surf = &ridvek[i_sur-1];

/*!                                                                 */
/*   Get the corresponding DB address and type of entity            */
/*                                                                 !*/

     if ( DBget_pointer('I',rid_surf,&la_surf,&typ_surf) < 0 ) 
     return(erpush("EX1402",""));

/*!                                                                 */
/*   Check that the entity is a surface                             */
/*                                                                 !*/

   if ( typ_surf != SURTYP )
     {
     igidst(rid_surf,errbuf);
     return(erpush("EX1412",errbuf));
     }

/*!                                                                 */
/*   Allocate memory                                                */
/*                                                                 !*/

     if ( (status=DBread_surface(&sur_all[i_sur-1], la_surf )) < 0 ) 
     return(status);
     if ( (status=DBread_patches(&sur_all[i_sur-1],&p_pat_all[i_sur-1])) < 0 ) 
     return(status);

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe24*EXscom Surface %d typ_surf %d\n", (int)i_sur, (int)typ_surf);
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif

  }                                      /* End   loop surfaces     */
/*!                                                                 */
/* End  all surfaces i_sur= 1,....,no_surf                          */
/*                                                                 !*/


/*!                                                                 */
/*   Create the composite surface                                   */
/*                                                                 !*/

   status= varkon_sur_comp(sur_all,p_pat_all,no_surf,c_case, &sur_out,&p_pat_out);


/*!                                                                 */
/*   Deallocate memory for the input surfaces                       */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe24*EXscom Deallocate memory for all ( %d) input surfaces\n", (int)no_surf );
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* Loop all surfaces i_sur= 1,....,no_surf                          */
/*                                                                 !*/

for ( i_sur=1; i_sur <= no_surf; ++i_sur ) /* Start loop surfaces   */
  {
  DBfree_patches(&sur_all[i_sur-1],p_pat_all[i_sur-1]);
  }                                      /* End   loop surfaces     */
/*!                                                                 */
/* End  all surfaces i_sur= 1,....,no_surf                          */
/*                                                                 !*/

   if ( status < 0 && p_pat_out == NULL ) goto err2;
   if ( status < 0 ) goto err1;

/*
***Lagra i DB och rita.
*/
    status = EXesur(id,&sur_out,p_pat_out,pnp);
    if ( status < 0 ) goto err1;


err1: 

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe24*EXscom Deallocate memory for output surface %d\n", (int)p_pat_out );
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif

   DBfree_patches(&sur_out,p_pat_out);


err2:;

   igrsma();

return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short EXstrm(
       DBId    *id,
       DBId    *rid,
       DBfloat  u1,
       DBfloat  u2,
       DBfloat  v1,
       DBfloat  v2,
       DBint    fall,
       V2NAPA  *pnp)

/*      Skapar SUR_TRIM.
 *
 *      In:  id      => Pekare till ytans identitet.
 *           rid     => Pekare till refererad yta,
 *           u1,u2   => Trimgr{nser i U-led.
 *           v1,v2   => Trimgr{nser i V-led.
 *           fall    => F|r framtida bruk.
 *           pnp     => Pekare till attribut.
 *
 *      Ut:  Inget.
 *
 *      FV:       0 => Ok.
 *
 *      (C)microform ab 1/3/95 J. Kjellander
 *      1999-12-18 sur181->varkon_sur_trim G Liden
 *
 ******************************************************!*/

  {
   short   status;
   DBptr   la;
   DBetype typ;
   DBfloat slim[2][2];
   char    errbuf[V3STRLEN+1];
   DBSurf   oldsur,newsur;
   DBPatch  *oldpat,*newpat;

/*
***Et meddelande.
*/
   igptma(453,IG_MESS);
/*
***H{mta geometri-data f|r refererad yta.
*/
   if ( DBget_pointer('I',rid,&la,&typ) < 0 ) return(erpush("EX1402",""));
   if ( typ != SURTYP )
     {
     igidst(rid,errbuf);
     return(erpush("EX1412",errbuf));
     }
   if ( (status=DBread_surface(&oldsur,la)) < 0 ) return(status);
   if ( (status=DBread_patches(&oldsur,&oldpat)) < 0 ) return(status);
/*
***Skapa nya ytan.
*/
   slim[0][0] = u1 + 1;
   slim[1][0] = u2 + 1;
   slim[0][1] = v1 + 1;
   slim[1][1] = v2 + 1;

/*
***Ev. debug.
*/
#ifdef DEBUG
    if ( dbglev(EXEPAC) == 1 )
      {
      fprintf(dbgfil(EXEPAC),
      "exe24*EXstrm u1 %g u2 %g v1 %g v2 %g\n",u1,u2,v1,v2);
      fprintf(dbgfil(EXEPAC),
      "exe24*EXstrm fall=%d\n",(int)fall);
      fprintf(dbgfil(EXEPAC),
      "exe24*EXstrm F|re  pnp->blank %d pnp->pen %d pnp->level %d\n",
          (int)pnp->blank,(int)pnp->pen,(int)pnp->level );
      fflush(dbgfil(EXEPAC)); /* To file from buffer      */
      }
#endif

#ifdef  TILLFALLIGT
    surpek->hed_su.blank = pnp->blank;   /* Blank/Unblank */
    surpek->hed_su.pen   = pnp->pen;     /* Penn-nimmer */
    surpek->hed_su.level = pnp->level;   /* Niv}-nummer */
    surpek->fnt_su       = pnp->sfont;   /* Font */
    surpek->lgt_su       = pnp->sdashl;  /* Streckl{ngd */
    surpek->ngu_su       = pnp->nulines; /* Antal "linjer" i U-riktn. */
    surpek->ngv_su       = pnp->nvlines; /* Antal "linjer" i V-riktn. */
#endif


 status = varkon_sur_trim(&oldsur,oldpat,slim,fall,&newsur,&newpat);

   if ( status < 0 && newpat == NULL ) goto err2;
   if ( status < 0 ) goto err1;

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe24*EXstrm Efter pnp->blank %d pnp->pen %d pnp->level %d\n",
      (int)pnp->blank,(int)pnp->pen,(int)pnp->level );
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe24*EXstrm newsur.nu_su %d  newsur.nv_su %d newpat %d\n",
     newsur.nu_su ,newsur.nv_su, (int)newpat );
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif


/*
***Lagra i DB och rita.
*/
   igrsma();
   status = EXesur(id,&newsur,newpat,pnp);
/*
***L{mna tillbaks dynamiskt allokerat minne.
*/
err1: 
   DBfree_patches(&newsur,newpat);
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe24*EXstrm Memory deallocated &newsur %d newpat %d\n",
     (int)&newsur, (int)newpat );
fflush(dbgfil(EXEPAC)); /* To file from buffer      */
}
#endif
err2:  
   DBfree_patches(&oldsur,oldpat);

   return(status);
  }
  
/********************************************************/


