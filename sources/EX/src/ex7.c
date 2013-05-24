/*!*******************************************************
*
*    ex7.c
*    =====
*
*    EXsymb();     Create (SYMB) plotfile-part
*
*    EXebpl();     Create B-plane
*    EXbpln();     Create B_PLANE
*
*
*    This file is part of the VARKON Execute Library.
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
#include "../../GP/include/GP.h"
#include "../../WP/include/WP.h"

#include "../include/EX.h"
#include <string.h>

extern char    jobdir[];
extern char    asydir[];
extern DBptr   lsysla;
extern DBTmat *lsyspk;
extern DBTmat  lklsyi;
extern short   gptrty;

/*!******************************************************/

       short EXsymb(
       DBId     *id,
       char     *nam,
       DBVector *pos,
       DBfloat   size,
       DBfloat   ang,
       V2NAPA   *pnp)

/*      Skapa SYMB.
 *
 *      In: id     => Pekare till symbolens identitet.
 *          nam    => Pekare till namnsträng.
 *          pos    => Pekare till position.
 *          size   => Symbolens storlek (skala).
 *          ang    => Vinkel (vridning).
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:       0 = Ok.
 *           EX1582 = Filen %s finns ej.
 *
 *      (C)microform ab 6/12/85 J. Kjellander
 *
 *      27/12/85 Ny konv. av flyttal R. Svedin
 *      29/1/86  Ny inläsning från fil, J. Kjellander
 *      2/1/86   Koll av antal vektorer, J. Kjellander
 *      23/3/86  Bytt namn, J. Kjellander.
 *      30/9/86  asydir, J. Kjellander
 *      19/10/86 Sumbolens origo, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      7/5/87   Plot-origo, J. Kjellander
 *      25/4/88  Bug flcose(), J. Kjellander
 *      28/2/89  Part, J. Kjellander
 *      15/12/89 Pennummer, J. Kjellander
 *      13/2/91  Bytt riktn. på linjer, J. Kjellander
 *      99-02-26 Linjebredd, J.Kjellander
 *
 ******************************************************!*/

  {
    char    sfnam[132];
    DBfloat crdvek[4];
    DBfloat *crdpek;
    DBfloat localx,localy,rotx,roty;
    DBfloat cosv,sinv,width;
    DBfloat origox,origoy;
    char    inbuf[128];
    char    str1[81],str2[81];
    char   *bufpek;
    int     ntkn,ncrd,i;
    double  x,y,nx,ny,mxmin,mymin,mxmax,mymax;
    short   status=0,npnum;
    FILE   *fp;

    DBLine  lin;
    DBId    lid;
    DBPart  prt;
    GMPDAT  dat;
    V2NAPA  attr;

/*
***Transformera pos till basic.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(pos,&lklsyi,pos);
/*
***Skapa en part-post.
*/
    prt.hed_pt.level = pnp->level;
    prt.hed_pt.pen = pnp->pen;
    prt.hed_pt.blank = pnp->blank;
    prt.hed_pt.hit = pnp->hit;
    *(nam+JNLGTH) = '\0';
    strcpy(prt.name_pt,nam);
    prt.its_pt = 2;
    dat.mtyp_pd = 2;
    dat.matt_pd = BASIC;
    dat.npar_pd = 0;
/*
***Skapa en linje-post.
*/
   lin.crd1_l.z_gm = lin.crd2_l.z_gm = 0.0;
   lid.seq_val = 1; lid.ord_val = 1; lid.p_nextre = NULL;
/*
***Skapa attributblock.
*/
   V3MOME(pnp,&attr,sizeof(V2NAPA));
   attr.lfont = 0;
   attr.ldashl = attr.width = 0.0;
/*
***Prova att öppna PLT-filen.
*/
    strcpy(sfnam,jobdir);
    strcat(sfnam,nam);
    strcat(sfnam,SYMEXT);
    if ( (fp=fopen(sfnam,"r")) == NULL )
      {
      strcpy(sfnam,asydir);
      strcat(sfnam,nam);
      strcat(sfnam,SYMEXT);
      if ( (fp=fopen(sfnam,"r")) == NULL )
            return(erpush("EX1582",nam));
      }
/*
***Filen finns, öppna part.
*/
    if ( pnp->save == 1 )
      {
      status = EXoppt(id,NULL,&prt,&dat,NULL,NULL);
      if ( status < 0 ) goto end;
      }
/*
***Lite initiering.
*/
    cosv = COS(ang*DGTORD);
    sinv = SIN(ang*DGTORD);
    origox = origoy = 0.0;
/*
***Läs vektorer från filen och skapa linjer.
*/
    while (fgets(inbuf,127,fp) != NULL)
        {
        if (strncmp (inbuf,"GKSM",4) == 0)
            {
            bufpek = inbuf+4;
/*
***Polyline, GKSM  11.
*/
            if ( strncmp(bufpek,"  11",4) == 0)
                {
                crdpek = crdvek;
                bufpek += 4;
                strncpy(str1,bufpek,6);
                bufpek += 6;
                strncpy(str2,bufpek,6);
                bufpek += 6;
                str1[6] = str2[6] ='\0';
                sscanf(str1,"%d",&ntkn);
                sscanf(str2,"%d",&ncrd);

                for ( i=0; i<ncrd; ++i )
                   {
                   x = gpaton(bufpek);
                   bufpek += 10;
                   y = gpaton(bufpek);
                   bufpek += 10;
/*
***Beräkna modellkoordinater och lagra.
*/
                   localx = size*(x*(mxmax-mxmin)/nx-origox);
                   localy = size*(y*(mymax-mymin)/ny-origoy);

                   rotx = localx*cosv - localy*sinv;
                   roty = localy*cosv + localx*sinv;

                   *crdpek = pos->x_gm + rotx; ++crdpek;
                   *crdpek = pos->y_gm + roty; ++crdpek;

                   if ( i != 0 )
                     {
                     lin.crd2_l.x_gm = *(crdpek-4);
                     lin.crd2_l.y_gm = *(crdpek-3);
                     lin.crd1_l.x_gm = *(crdpek-2);
                     lin.crd1_l.y_gm = *(crdpek-1);
                     *(crdpek-4) = *(crdpek-2);
                     *(crdpek-3) = *(crdpek-1);
                     crdpek -= 2;
                     if ( pnp->save == 1 )
                       if ( (status=EXelin(&lid,&lin,&attr)) < 0 ) 
                         goto end;
                     }
/*
***Läsning av nästa rad i en lång polyline.
*/
                   if ( i != ncrd-1 && (int)(bufpek-inbuf+1) >= (int)strlen(inbuf) )
                     {
                     fgets(inbuf,512,fp);
                     bufpek = inbuf;
                     }
                   }
                }
/*
***Modellfönster, GKSM 175.
*/
            else if ( strncmp(bufpek," 175",4) == 0)
                {
                bufpek += 4;
                sscanf(bufpek,"%6c",str1);
                str1[6] = '\0';
                sscanf(str1,"%d",&ntkn);

                bufpek += 6;
                sscanf(bufpek,"%10c%10c",str1,str2);
                str1[10] = str2[10] = '\0';
                sscanf(str1,"%lf",&mxmin);
                sscanf(str2,"%lf",&mxmax);

                bufpek += 20;
                sscanf(bufpek,"%10c%10c",str1,str2);
                str1[10] = str2[10] = '\0';
                sscanf(str1,"%lf",&mymin);
                sscanf(str2,"%lf",&mymax);

                if ( *(bufpek+21) != '\0' )
                  {
                  bufpek += 20;
                  sscanf(bufpek,"%10c%10c",str1,str2);
                  str1[10] = str2[10] = '\0';
                  sscanf(str1,"%lf",&origox);
                  sscanf(str2,"%lf",&origoy);
                  }
/*
***Beräkna förhållande mellan skärmens xmax o ymax.
*/
                nx = mxmax - mxmin;
                ny = mymax - mymin;

                if  ( nx < ny ) 
                   { nx /= ny; ny = 1.0; }
                else
                   { ny /= nx; nx = 1.0; }
                }
/*
***Linjebredd, GKSM 174.
*/
            else if ( strncmp(bufpek," 174",4) == 0)
                {
                bufpek += 4;
                sscanf(bufpek,"%6c%10c",str1,str2);
                str1[6] = str2[10] = '\0';
                sscanf(str1,"%d",&ntkn);
                sscanf(str2,"%lf",&width);
                attr.width = width;
                }
/*
***Pennummer, GKSM 173.
*/
            else if ( strncmp(bufpek," 173",4) == 0)
                {
                bufpek += 4;
                sscanf(bufpek,"%6c%6c",str1,str2);
                str1[6] = str2[6] = '\0';
                sscanf(str1,"%d",&ntkn);
                sscanf(str2,"%hd",&npnum);
                attr.pen = npnum;
                }
            }
        }
/*
***Slut.
*/
end:
    if ( pnp->save == 1 ) EXclpt();
    fclose(fp);
    return(status);
  }
  
/********************************************************/
/*!******************************************************/

       short EXebpl(
       DBId   *id,
       DBBplane *bplpek,
       V2NAPA *pnp)

/*      Skapar B-plan.
 *
 *      In: id     => Pekare till identitet.
 *          bplpek => Pekare till DB-struktur.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder:      0 = Ok.
 *                EX1842 = Kan ej lagra B-plan i DB.
 *
 *      (C)microform ab 27/8/87 J. Kjellander
 *
 *      20/3/92  lsysla, J. Kjellander
 *      24/7/04  width, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    DBptr   la;

/*
***Fyll i namnparametrar.
*/
    bplpek->hed_bp.blank = pnp->blank;
    bplpek->hed_bp.pen   = pnp->pen;
    bplpek->hed_bp.level = pnp->level;
    bplpek->pcsy_bp      = lsysla;
    bplpek->wdt_bp       = pnp->width;
/*
***Lagra i gm.
*/
    if ( pnp->save )
      {
      bplpek->hed_bp.hit = pnp->hit;
      if ( DBinsert_bplane(bplpek,id,&la) < 0 ) return(erpush("EX1842",""));
      }
    else
      {
      bplpek->hed_bp.hit = 0;
      }
/*
***Rita.
*/
    gpdrbp(bplpek,la,DRAW);

#ifdef V3_X11
    if ( gptrty == X11 ) wpdrbp(bplpek,la,GWIN_ALL);
#endif
#ifdef WIN32
    wpdrbp(bplpek,la,GWIN_ALL);
#endif

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short EXbpln(
       DBId     *id,
       DBVector *p1,
       DBVector *p2,
       DBVector *p3,
       DBVector *p4,
       V2NAPA   *pnp)

/*      Skapar B_PLANE.
 *
 *      In: id     => Pekare till identitet.
 *          p1     => Startpunkt.
 *          p2     => Punkt 2.
 *          p2     => Punkt 3.
 *          p4     => Slutpunkt.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkoder:      0 = Ok.
 *
 *      (C)microform ab 27/8/87 J. Kjellander
 *      1999-12-18 sur718->varkon_bpl_analyse G Liden
 *
 ******************************************************!*/

  {
    DBBplane bpl;

/* #define BPLAN_TEST */
#ifdef BPLAN_TEST

    DBint   btype;       /* Type of B-plane polygon                 */
                         /* Type of polygon                         */
                         /* Eq.  1: Triangle                        */
                         /* Eq.  2: Convex 4-point polygon          */
                         /* Eq.  3: Non-convex polygon              */
                         /* Eq.  4: p4, p1, p2 are colinear         */
                         /* Eq.  5: p1, p2, p3 are colinear         */
                         /* Eq.  6: p2, p3, p4 are colinear         */
                         /* Eq.  7: p3, p4, p1 are colinear         */
                         /* Eq. -1: p1 = p2                         */
                         /* Eq. -2: p3 = p4                         */
                         /* Eq. -3: p1 = p4                         */
                         /* Eq. -4: p1, p2, p3, p4 not coplanar     */
                         /* Eq. -5: Self-intersecting polygon       */
  DBint    pnr;          /* Non-convex point number                 */
  DBint    status;       /* Error code from called function         */
#endif

/*
***Transformera till basic.
*/
    if ( lsyspk != NULL )
      {
      GEtfpos_to_local(p1,&lklsyi,p1);
      GEtfpos_to_local(p2,&lklsyi,p2);
      GEtfpos_to_local(p3,&lklsyi,p3);
      GEtfpos_to_local(p4,&lklsyi,p4);
      }
/*
***Ev. kontroll av planets hörn.
*/
#ifdef BPLAN_TEST
    status = varkon_bpl_analyse (p1, p2, p3, p4, &btype, &pnr );
    if ( btype == -1 )  return(erpush("SU1203",""));
    if ( btype == -2 )  return(erpush("SU1213",""));
    if ( btype == -3 )  return(erpush("SU1223",""));
    if ( btype == -4 )  return(erpush("SU1233",""));
    if ( btype == -5 )  return(erpush("SU1243",""));
#endif
/*
***Lagra positioner i B-plan posten.
*/
    V3MOME((char *)p1,(char *)&bpl.crd1_bp,sizeof(DBVector));
    V3MOME((char *)p2,(char *)&bpl.crd2_bp,sizeof(DBVector));
    V3MOME((char *)p3,(char *)&bpl.crd3_bp,sizeof(DBVector));
    V3MOME((char *)p4,(char *)&bpl.crd4_bp,sizeof(DBVector));
/*
***Lagra i gm och rita.
*/
    return ( EXebpl(id,&bpl,pnp)); 
  }

/********************************************************/
