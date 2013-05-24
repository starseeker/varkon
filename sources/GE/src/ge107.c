/**********************************************************************
*
*    ge107.c
*    =======
*
*    This file is part of the VARKON Geometry Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    GE107() Compute geometric data on curve segment
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
#include "../include/GE.h"

static short calcr(DBfloat u); /* Calculate coordinates */
static short calcro();         /* Calculate coordinates in offset */
static short caldu(DBfloat u); /* Calculate 1:st derivative */
static short calduo();         /* Calculate 1:st derivative in offset */
static short cald2(DBfloat u); /* Calculate 2:nd derivative */
static short cald2o();         /* Calculate 2:nd derivative in offset */
static short calnbo();         /* Calculate N, B and kappa in offset */

/*
***Static-variables used by routines in this file.
*/
static bool  ratseg,kubseg,offseg;
static gmflt c0x,c1x,c2x,c3x,c0y,c1y,c2y,c3y,px,py,pz,
             c0z,c1z,c2z,c3z,c0,c1,c2,c3,offset,dsdu,
             xw,yw,zw,w,x,y,z,dxdu,dydu,dzdu,tx,ty,tz,
             dxwdu,dywdu,dzwdu,dwdu,w2,w4,dsdu3,kappa,
             d2xdu2,d2ydu2,d2zdu2,d2wdu2,bx,by,bz,
             d2xwdu2,d2ywdu2,d2zwdu2,nx,ny,nz,npx,npy,npz,
             xoff,yoff,zoff,dxoff,dyoff,dzoff,d2xoff,d2yoff,d2zoff,
             noffx,noffy,noffz,boffx,boffy,boffz,kapofs,dkappadu;

/*
***Pekare till aktivt koordinatsystem. Används för att bestämma
***kurvplan för 3D-cirklar.
*/
extern DBTmat *lsyspk;


/*!******************************************************/

        DBstatus GE107(
        DBAny   *gmpost,
        DBSeg   *sp,
        DBfloat  u,
        short    rcode,
        DBfloat  out[])

/*      Calculates coordinates, derivatives and other geometric
 *      data for DBArc- and DBCurve-segments.
 *
 *      In: gmpost   =  Ptr to curve/arc-structure
 *          sp       =  Ptr to a segment
 *          u        =  Parameter value, 0 -> 1
 *          rcode    =  Requested result
 *                      0 = Only coordinates       out[0-2]
 *                      1 = Also 1:st derivative   out[3-5]
 *                      2 = Also 2:nd derivative   out[6-8]
 *                      3 = Also Normal            out[9-11]
 *                           and Bi-normal         out[12-14]
 *                           and kappa             out[15]
 *                      4 = Also dkappadu          out[16]
 *
 *      Out: out[0-16] = See above.
 *
 *      (C)microform ab 15/12/91 J. Kjellander
 *
 *      5/5/92   Offset=0 i calcro och calduo, J. Kjellander
 *      6/9/93   UV-segment, J. Kjellander
 *     19/6/94   Initiering outÄ0-15Å Gunnar Liden
 *     1/12/94   Bytt geo108 mot GE110, J. Kjellander
 *     6/12/94   Bytt geo108 mot GE110, G. Liden       
 *    24/01/95   Ännu ej färdig !!!!!!!! G. Liden       
 *    15/03/95   Bytt geo108 mot GE110  G. Liden       
 *    1999-05-15 Rewritten, J.Kjellander
 *    1999-05-28 rcode 4, J.Kjellander
 *    1999-12-18 sur776->varkon_ini_evalc G. Liden
 *
 ******************************************************!*/

  {
    short status;
    EVALC curdata;

/*
***Om det är ett UV-segment använder vi GE110 som anropar
***varkon_sur_uvsegeval, annars gör vi allt här.
*/
    if ( sp->typ == UV_CUB_SEG )
      {
/*
***Kontrollera att ytans GM-adress har ett vettigt värde för Debug On
*/
      varkon_ini_evalc(&curdata);    

      curdata.t_local = u;

      if      ( rcode == 0 ) 
        curdata.evltyp = EVC_R;
      else if ( rcode == 1 ) 
        curdata.evltyp = EVC_R+EVC_DR;
      else if ( rcode == 2 ) 
        curdata.evltyp = EVC_R+EVC_DR+EVC_D2R;
      else if ( rcode == 3 ) 
        curdata.evltyp = EVC_R+EVC_DR+EVC_D2R+EVC_PN+EVC_KAP+EVC_BN;

      if ( (status=GE110(gmpost,sp,&curdata)) < 0 ) return(status);

      out[0]  = curdata.r.x_gm;
      out[1]  = curdata.r.y_gm;
      out[2]  = curdata.r.z_gm;
      out[3]  = curdata.drdt.x_gm;
      out[4]  = curdata.drdt.y_gm;
      out[5]  = curdata.drdt.z_gm;
      out[6]  = curdata.d2rdt2.x_gm;
      out[7]  = curdata.d2rdt2.y_gm;
      out[8]  = curdata.d2rdt2.z_gm;
      out[9]  = curdata.p_norm.x_gm;
      out[10] = curdata.p_norm.y_gm;
      out[11] = curdata.p_norm.z_gm;
      out[12] = curdata.b_norm.x_gm;
      out[13] = curdata.b_norm.y_gm;
      out[14] = curdata.b_norm.z_gm;
      out[15] = kappa;
      return(SUCCED);
      }
/*
***Det är inte ett UV-segment, alltså är det ett CUB-SEG.
***Först klassificering.
***ratseg = TRUE om rationellt segment.
***cubseg = TRUE om kubiskt segment.
***offseg = TRUE om offset-segment.
***offset = segmentets offset.
*/
    if ( sp->c0 == 1.0  &&  sp->c1 == 0.0  &&
         sp->c2 == 0.0  &&  sp->c3 == 0.0 ) ratseg = FALSE;
    else ratseg = TRUE;

    if ( sp->c3x == 0.0  &&  sp->c3y == 0.0  &&
         sp->c3z == 0.0  &&  sp->c3 == 0.0 ) kubseg = FALSE;
    else kubseg = TRUE;
/*
***Om det är ett offset-segment lagras kurvplanets normal i
***(px,py,pz). För vanliga offset-kurvor väljs kurvans eget plan
***som kurvplan. För 3D offset-cirklar väljs aktivt XY-plan och för
***övriga väljs BASIC:s XY-plan.
***If rcode = 4, we need to return dkappadu which will only be
***calculated if offseg is forced to TRUE.
*/
    if ( sp->ofs != 0.0  ||  rcode == 4 )
      {
      offseg = TRUE;
      offset = sp->ofs;
      if ( gmpost->hed_un.type == CURTYP )
        {
        px = gmpost->cur_un.csy_cu.g31;
        py = gmpost->cur_un.csy_cu.g32;
        pz = gmpost->cur_un.csy_cu.g33;
        }
      else if ( gmpost->hed_un.type == ARCTYP  &&
                gmpost->arc_un.ns_a > 0  &&  lsyspk != NULL )
        {
        px = lsyspk->g31;
        py = lsyspk->g32;
        pz = lsyspk->g33;
        }
      else
        {
        px = 0.0; py = 0.0; pz = 1.0;
        }
      }
    else offseg = FALSE;
/*
***Kopiera segment-koefficienterna till static-variabler.
***Detta gör det snabbare att accessa dem eftersom adresserna
***inte behöver beräknas vid runtime och data slipper utväxlas
***på stacken mellan subrutinanrop.
*/
    c0x = sp->c0x; c1x = sp->c1x; c2x = sp->c2x; c3x = sp->c3x;
    c0y = sp->c0y; c1y = sp->c1y; c2y = sp->c2y; c3y = sp->c3y;
    c0z = sp->c0z; c1z = sp->c1z; c2z = sp->c2z; c3z = sp->c3z;
    c0  = sp->c0;  c1  = sp->c1;  c2  = sp->c2;  c3  = sp->c3;
/*
***Oavsett rcode beräknas alltid koordinaterna. Det enda fall
***då detta är onödigt är om kurvan är icke rationell och man
***inte behöver några koordinater. Den lilla prestandahöjning
***som det skulle ge att slippa detta struntar vi.
*/
   calcr(u);
   if ( offseg )
     { 
     caldu(u);
     calcro();
     out[0] = xoff; out[1] = yoff; out[2] = zoff;
     }
   else { out[0] = x; out[1] = y; out[2] = z; }
/*
***1:a derivatan.
*/
   if ( rcode >= 1 )
     {
     if ( offseg )
       {
       cald2(u);
       calduo();
       out[3] = dxoff; out[4] = dyoff; out[5] = dzoff;
       }
     else
       {
       caldu(u);
       out[3] = dxdu; out[4] = dydu; out[5] = dzdu;
       }
     }
   else return(0);
/*
***2:a derivatan.
*/
   if ( rcode >= 2 )
     {
     if ( offseg )
       {
       cald2o();
       out[6] = d2xoff; out[7] = d2yoff; out[8] = d2zoff;
       }
     else
       {
       cald2(u);
       out[6] = d2xdu2; out[7] = d2ydu2; out[8] = d2zdu2;
       }
     }
   else return(0);
/*
***Principal-normal, Bi-normal och Krökning.
*/
   if ( rcode >= 3 )
     {
     if ( offseg )
       {
       calnbo();
       out[9]  = noffx; out[10] = noffy; out[11] = noffz;
       out[12] = boffx; out[13] = boffy; out[14] = boffz;
       out[15] = kapofs;
       }
     else
       {
       offset = 0.0;
       calcro();         /* Beräkna dsdu och T */
       calduo();         /* Veräkna kappa, N och B */
       out[9]  = nx; out[10] = ny; out[11] = nz;
       out[12] = bx; out[13] = by; out[14] = bz;
       out[15] = kappa;
       }
     }
/*
***Curvature derivative.
*/
   if ( rcode == 4 ) out[16] = dkappadu;

   return(0);
  }


/********************************************************/
/*!******************************************************/

 static short calcr(DBfloat u)

/*      Beräknar koordinater (x,y,z) och (xw,yw,zw,w).
 *
 *      In: 
 *         u  =  Parametervärde.
 *
 *      Ut:
 *         Se ovan.
 *
 *      FV: 0
 *
 *      (C)microform ab 15/12/91 J. Kjellander
 *
 ******************************************************!*/

{
   if ( ratseg )
     {
     if ( kubseg )
       { /* Rationellt kubiskt */
       xw = c0x + u*(c1x + u*(c2x + u*c3x));
       yw = c0y + u*(c1y + u*(c2y + u*c3y));
       zw = c0z + u*(c1z + u*(c2z + u*c3z));
       w  = c0  + u*(c1  + u*(c2  + u*c3));
       }
     else
       { /* Rationellt kvadratiskt */
       xw = c0x + u*(c1x + u*c2x);
       yw = c0y + u*(c1y + u*c2y);
       zw = c0z + u*(c1z + u*c2z);
       w =  c0  + u*(c1  + u*c2);
       }
     x = xw/w; y = yw/w; z = zw/w;
     }
   else 
     {
     if ( kubseg )
       { /* Icke rationellt kubiskt */
       x = c0x + u*(c1x + u*(c2x + u*c3x));
       y = c0y + u*(c1y + u*(c2y + u*c3y));
       z = c0z + u*(c1z + u*(c2z + u*c3z));
       }
     else
       { /* Icke rationellt kvadratiskt */
       x = c0x + u*(c1x + u*c2x);
       y = c0y + u*(c1y + u*c2y);
       z = c0z + u*(c1z + u*c2z);
       }
     w = 1.0; /* Skall inte behövas !!! */
     }
   return(0);
}

/********************************************************/
/*!******************************************************/

 static short calcro()

/*      Beräknar dsdu och normerad tangent (tx,ty,tz).
 *      Beräknar även koordinater i offset om så önskas.
 *
 *      Förutsätter att koordinater och 1:a derivata redan beräknats.
 *
 *      Denna rutin kan anropas med offset = 0 bara för att
 *      beräkna dsdu och T. I så fall finns möjligen inget
 *      kurvplan P och därför kan inte koordinater beräknas.
 *      Dessa behövs ju inte heller och vi spar tid genom
 *      att skippa dom.
 *
 *      Ut:
 *         Se ovan.
 *
 *      FV: 0
 *
 *      (C)microform ab 15/12/91 J. Kjellander
 *
 *      5/5/92   Skippa ber. av koord. om offset = 0, JK
 *
 ******************************************************!*/

{

/*
***dr/du = (dxdu,dydu,dzdu). Tangenten T = dr/ds =
***enhetsvektorn med samma riktning som dr/du. 
***Beräkna längden av dr/du = dsdu.
*/
    dsdu = SQRT(dxdu*dxdu + dydu*dydu + dzdu*dzdu);
    if ( dsdu < 1e-14 ) dsdu = 1e-10;
/*
***Beräkna normerad tangent, T.
*/
    tx = dxdu/dsdu; ty = dydu/dsdu; tz = dzdu/dsdu;
/*
***Beräkna offset-vektorn som tangenten X kurvplanets normal.
***T är normerad och P likaså. Resultatet av T X P = en
***vektor med samma riktning som kurv-normalen, ev. motsatt.
***När kurvan svänger till höger i kurvplanet är T X P = N men
***när kurvan svänger till vänster är T X P = -N. Detta är V3:s
***definition av vad som menas med offset. Positivt offset är
***alltså alltid kurvans högra sida i kurvplanet.
***Längd = kurvans offset.
*/
    if ( offseg )
      {
      xoff = x + offset*(ty*pz - py*tz);
      yoff = y + offset*(tz*px - pz*tx);
      zoff = z + offset*(tx*py - px*ty);
      }

    return(0);
}

/*********************************************************/
/*!******************************************************/

 static short caldu(DBfloat u)

/*      Beräknar 1:a derivatan (dxdu,dydu,dzdu)
 *      och (dxwdu,dywdu,dzwdu,dwdu).
 *
 *      In: 
 *         u  =  Parametervärde.
 *
 *      Ut:
 *         Se ovan.
 *
 *      FV: 0
 *
 *      (C)microform ab 15/12/91 J. Kjellander
 *
 ******************************************************!*/

{
   if ( ratseg )
     {
     if ( kubseg )
       { /* Rationellt kubiskt */
       dxwdu = c1x + u*(2.0*c2x + 3.0*u*c3x);
       dywdu = c1y + u*(2.0*c2y + 3.0*u*c3y);
       dzwdu = c1z + u*(2.0*c2z + 3.0*u*c3z);
       dwdu  = c1  + u*(2.0*c2  + 3.0*u*c3);
       }
     else
       { /* Rationellt kvadratiskt */
       dxwdu = c1x + u*2.0*c2x;
       dywdu = c1y + u*2.0*c2y;
       dzwdu = c1z + u*2.0*c2z;
       dwdu  = c1  + u*2.0*c2;
       }
     w2 = w*w;
     dxdu = (w*dxwdu - dwdu*xw)/w2;
     dydu = (w*dywdu - dwdu*yw)/w2;
     dzdu = (w*dzwdu - dwdu*zw)/w2;
     }
   else
     {
     if ( kubseg )
       { /* Icke rationellt kubiskt */
       dxdu = c1x + u*(2.0*c2x + 3.0*u*c3x);
       dydu = c1y + u*(2.0*c2y + 3.0*u*c3y);
       dzdu = c1z + u*(2.0*c2z + 3.0*u*c3z);
       }
     else
       { /* Icke rationellt kvadratiskt */
       dxdu = c1x + u*2.0*c2x;
       dydu = c1y + u*2.0*c2y;
       dzdu = c1z + u*2.0*c2z;
       }
     }
   return(0);
}

/********************************************************/
/*!******************************************************/

 static short calduo()

/*      Beräknar krökning, Binormal och Normal.
 *
 *      Beräknar 1:a derivatan (dxdu,dydu,dzdu) i offset
 *      om så önskas.
 *
 *      FV: 0
 *
 *      (C)microform ab 15/12/91 J. Kjellander
 *
 *      5/5/92  Skippa ber. av derivata om offset=0, JK
 *      1999-10-17 COMPTOL, J.Kjellander
 *
 ******************************************************!*/

{
    gmflt tmpx,tmpy,tmpz,tmp;

/*
***Först beräknar vi kappa för offset = 0. Först drdu X d2rdu2.
***Sedan beloppet av denna vektor dividerat med dsdu ** 3. Se
***Faux-Pratt sid. 101.
***Om kurvan här är en rät linje är kappa 0. Inget problem, men
***om kurvan är degenererad till en punkt, dvs. ingen båglängd
***är drdu och d2rdu2 båda 0 och dsdu likaså. Då gäller det att
***inte försöka beräkna kappa utan att bara sätta kappa till 0.
*/
    tmpx  = dydu*d2zdu2-dzdu*d2ydu2;
    tmpy  = dzdu*d2xdu2-dxdu*d2zdu2;
    tmpz  = dxdu*d2ydu2-dydu*d2xdu2;
    tmp   = tmpx*tmpx + tmpy*tmpy + tmpz*tmpz;
    dsdu3 = dsdu*dsdu*dsdu;

    if ( tmp > COMPTOL ) kappa = SQRT(tmp)/dsdu3;
    else                 kappa = 0.0;
/*
***Binormalen. Denna beräknas som drdu X d2rdu2 dividerat med 
***dsdu ** 3 och kappa. Se Faux-Pratt sid. 100.
***Om segmentet är en rät linje är d2rdu2 == 0.0.
***och binormalen följdaktligen = (0,0,0).
*/
    bx = tmpx/dsdu3;
    by = tmpy/dsdu3;
    bz = tmpz/dsdu3;

   if ( kappa > COMPTOL )
     {
     bx /= kappa;
     by /= kappa;
     bz /= kappa;
     }
/*
***Principal-normal, N = B X T. Denna blir också = (0,0,0)
***för en rät linje.
*/
   nx = by*tz - ty*bz;
   ny = bz*tx - tz*bx;
   nz = bx*ty - tx*by;
/*
***Offset-tangenten har samma riktning som den ursprungliga
***tangenten T. Dock eftersom offset räknas positivt på
***kurvans högra sida och negativt på kurvans vänstra
***måste vi justera tecknet på tangenten så att den nya
***tangenten verkligen blir längre i en ytterkurva och kortare
***i en innerkurva. Jfr. beräkning av koordinater i offset.
***Offset-tangentens riktning beräknas som  N X P.
*/
   if ( offseg )
     {
     npx = ny*pz - py*nz;
     npy = nz*px - pz*nx;
     npz = nx*py - px*ny;
/*
***Tangenten i offset = dr/du + offset*kappa * dsdu*(npx,npy,npz). 
***Helt enkelt en faktor kappa*offset större. Tecknet på np tar
***ut tecknet på offset så att resultatet blir rätt.
*/
     dxoff = dxdu + offset*kappa*dsdu*npx;
     dyoff = dydu + offset*kappa*dsdu*npy;
     dzoff = dzdu + offset*kappa*dsdu*npz;
     }

   return(0);
}

/********************************************************/
/*!******************************************************/

 static short cald2(DBfloat u)

/*      Beräknar 2:a derivatan (d2xdu2,d2ydu2,d2zdu2)
 *      och (d2xwdu2,d2ywdu2,d2zwdu2,d2wdu2).
 *
 *      In: 
 *         u  =  Parametervärde.
 *
 *      Ut:
 *         Se ovan.
 *
 *      FV: 0
 *
 *      (C)microform ab 15/12/91 J. Kjellander
 *
 ******************************************************!*/

{
   if ( ratseg )
     {
     if ( kubseg )
       { /* Rationellt kubiskt */
       d2xwdu2 = 2.0*c2x + 6.0*u*c3x;
       d2ywdu2 = 2.0*c2y + 6.0*u*c3y;
       d2zwdu2 = 2.0*c2z + 6.0*u*c3z;
       d2wdu2  = 2.0*c2  + 6.0*u*c3;
       }
     else
       { /* Rationellt kvadratiskt */
       d2xwdu2 = 2.0*c2x;
       d2ywdu2 = 2.0*c2y;
       d2zwdu2 = 2.0*c2z;
       d2wdu2  = 2.0*c2;
       }
     w4 = w2*w2;
     d2xdu2 = (w*d2xwdu2 - dwdu*dxwdu)/w2 -
              ((d2wdu2*xw + dwdu*dxwdu)*w2 - 2.0*w*xw*dwdu*dwdu)/w4;
     d2ydu2 = (w*d2ywdu2 - dwdu*dywdu)/w2 -
              ((d2wdu2*yw + dwdu*dywdu)*w2 - 2.0*w*yw*dwdu*dwdu)/w4;
     d2zdu2 = (w*d2zwdu2 - dwdu*dzwdu)/w2 -
              ((d2wdu2*zw + dwdu*dzwdu)*w2 - 2.0*w*zw*dwdu*dwdu)/w4;
     }
   else
     {
     if ( kubseg )
       { /* Icke rationellt kubiskt */
       d2xdu2 = 2.0*c2x + 6.0*u*c3x;
       d2ydu2 = 2.0*c2y + 6.0*u*c3y;
       d2zdu2 = 2.0*c2z + 6.0*u*c3z;
       }
     else
       { /* Icke rationellt kvadratiskt */
       d2xdu2 = 2.0*c2x;
       d2ydu2 = 2.0*c2y;
       d2zdu2 = 2.0*c2z;
       }
     }
   return(0);
}

/********************************************************/
/*!******************************************************/

 static short cald2o()

/*      Beräknar 2:a derivatan (d2xdu2,d2ydu2,d2zdu2) i offset.
 *
 *      FV: 0
 *
 *      (C)microform ab 15/12/91 J. Kjellander
 *
 ******************************************************!*/

{
   gmflt d3xwdu3,d3ywdu3,d3zwdu3,d3wdu3,d3xdu3,d3ydu3,d3zdu3,
         d2sdu2,vx,vy,vz,ux,uy,uz,dsdu6,ntpx,ntpy,
         ntpz;

/*
***Först beräknar vi 3:e-derivatan.
*/
   if ( kubseg )
     {
     d3xwdu3 = 6.0*c3x; d3ywdu3 = 6.0*c3y;
     d3zwdu3 = 6.0*c3z; d3wdu3 =  6.0*c3;
     }
   else d3xwdu3 = d3ywdu3 = d3zwdu3 = d3wdu3 = 0.0;

   if ( ratseg )
     {
     d3xdu3 = (d3xwdu3-3.0*d2xdu2*dwdu-3.0*dxdu*d2wdu2-x*d3wdu3)/w;
     d3ydu3 = (d3ywdu3-3.0*d2ydu2*dwdu-3.0*dydu*d2wdu2-y*d3wdu3)/w;
     d3zdu3 = (d3zwdu3-3.0*d2zdu2*dwdu-3.0*dzdu*d2wdu2-z*d3wdu3)/w;
     }
   else
     {
     d3xdu3 = d3xwdu3;
     d3ydu3 = d3ywdu3;
     d3zdu3 = d3zwdu3;
     }
/*
***Andra-derivatan av s map. u.
*/
   d2sdu2 = d2xdu2*tx + d2ydu2*ty + d2zdu2*tz;
/*
***dr/du X d2r/du2.
*/
   vx = dydu*d2zdu2 - dzdu*d2ydu2;
   vy = dzdu*d2xdu2 - dxdu*d2zdu2;
   vz = dxdu*d2ydu2 - dydu*d2xdu2;
/*
***dr/du X d3r/du3.
*/
   ux = dydu*d3zdu3 - dzdu*d3ydu3;
   uy = dzdu*d3xdu3 - dxdu*d3zdu3;
   uz = dxdu*d3ydu3 - dydu*d3xdu3;
/*
***Scalär produkt ( dr/du ! dr/du )**3
*/
   dsdu6 = dsdu3*dsdu3;
/*
***dkappa/du.
*/
   dkappadu = vx*ux + vy*uy + vz*uz;

   if ( dsdu6 > TOL1 && kappa > TOL1 )
     {
     dkappadu = dkappadu/dsdu6/kappa;
     }
   else
     {
     dkappadu = px*ux + py*uy + pz*uz;
     dkappadu = dkappadu/dsdu3;
     }

   dkappadu = dkappadu - 3.0*kappa*(d2xdu2*dxdu+d2ydu2*dydu+d2zdu2*dzdu)/
                                      (dsdu*dsdu);
/*
***Normalen i offset korrigerad för tecknet på offset.
*/
   ntpx = ty*pz - py*tz;
   ntpy = tz*px - pz*tx;
   ntpz = tx*py - px*ty;
/*
***Beräkna 2:a derivatan.
*/
   d2xoff = d2xdu2 + offset*(dkappadu*dsdu*npx +
                     kappa*(d2sdu2*npx - dsdu*dsdu*kappa*ntpx));
   d2yoff = d2ydu2 + offset*(dkappadu*dsdu*npy +
                     kappa*(d2sdu2*npy - dsdu*dsdu*kappa*ntpy));
   d2zoff = d2zdu2 + offset*(dkappadu*dsdu*npz +
                     kappa*(d2sdu2*npz - dsdu*dsdu*kappa*ntpz));

   return(0);
}

/********************************************************/
/*!******************************************************/

 static short calnbo()

/*      Beräknar Normal, binormal och krökning  i offset.
 *      Denna rutin är densamma som caldo() men alla variabler
 *      utbytta mot offset.
 *
 *      FV: 0
 *
 *      (C)microform ab 15/12/91 J. Kjellander
 *
 *      2000-03-19 COMPTOL, J.Kjellander
 *
 ******************************************************!*/

{
    gmflt tmpx,tmpy,tmpz,tmp,dsoffs,dsoff3;

/*
***Först beräknar vi kappa i offset.
*/
    tmpx = dyoff*d2zoff-dzoff*d2yoff;
    tmpy = dzoff*d2xoff-dxoff*d2zoff;
    tmpz = dxoff*d2yoff-dyoff*d2xoff;

    tmp = tmpx*tmpx + tmpy*tmpy + tmpz*tmpz;

    if ( tmp > COMPTOL )
      {
      kapofs  = SQRT(tmp);
      dsoffs  = SQRT(dxoff*dxoff + dyoff*dyoff + dzoff*dzoff);
      dsoff3  = dsoffs*dsoffs*dsoffs;
      kapofs /= dsoff3;
      }
    else kapofs = 0.0;
/*
***Binormalen har samma riktning som för offset = 0. Båda
***kurvorna ligger ju säkert i samma plan och längden = 1.
*/
    boffx = bx;
    boffy = by;
    boffz = bz;
/*
***Principal-normal, N = B X T. Denna är också densamma
***som i offset = 0.
*/
   noffx = nx;
   noffy = ny;
   noffz = nz;

   return(0);
}

/********************************************************/
