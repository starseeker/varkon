/*!*****************************************************
*
*    ex1.c
*    =====
*
*    EXstrt();    Interface routine for START
*    EXend();     Interface routine for END
*    EXon();      Interface routine for ON
*    EXion();     Interface routine for INV_ON
*    EXtang();    Interface routine for TANG
*    EXitan();    Interface routine for INV_TANG
*    EXcurv();    Interface routine for CURV
*    EXicur();    Interface routine for INV_CURV
*    EXcen();     Interface routine for CENTRE
*    EXnorm();    Interface routine for NORM
*    EXarcl();    Interface routine for ARCL
*    EXiarc();    Interface routine for INV_ARCL
*    EXsuar();    Interface routine for SURFACE_AREA
*    EXsear();    Interface routine for SECTION_AREA
*    EXsecg();    Interface routine for SECTION_CGRAV
*    EXtxtl();    Interface routine for TEXTL
*    EXsect();    Interface routine for INTERSECT
*    EXnsec();    Interface routine for N_INTERSECT
*    EXidnt();    Interface routine for IDENT
*    EXpos();     Interface routine for POS
*    EXscr();     Interface routine for SCREEN
*    EXarea();    Interface routine for AREA,CGRAV
*
*    EXusec();    Used by EXsect()
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

#ifdef V3_X11
#include "../../WP/include/WP.h"
#endif

#include "../include/EX.h"
#include <string.h>

#ifdef UNIX
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <process.h>
#endif

extern bool    tmpref;
extern short   igtrty,gptrty;
extern DBptr   msysla,lsysla;
extern DBTmat *lsyspk,*msyspk;
extern DBTmat  modsys,lklsys,lklsyi;
extern V2NAPA  defnap;

/*!******************************************************/

        short EXstrt(
        DBId     *idpek,
        DBVector *vecptr)

/*      Interface-rutin för START().
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *
 *      Ut: *vecptr => En DBVector med koordinater.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *
 *      (C)microform ab 1997-01-25 J. Kjellander
 *
 ******************************************************!*/

  {
    DBLine  lin;
    DBArc  arc;
    DBCurve  cur;
    DBSeg *segpek;
    DBSeg  arcseg[4];
    DBptr  la;
    DBetype  typ;
    short  status;

/*
***Hämta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och läs geometridata.
*/
    switch (typ)
      {
      case LINTYP:
      DBread_line(&lin, la);
      status=GEposition((DBAny *)&lin,NULL,0.0,0.0,vecptr);
      break;

      case ARCTYP:
      DBread_arc(&arc,arcseg,la);
      status=GEposition((DBAny *)&arc,(char *)arcseg,0.0,0.0,vecptr);
      break;

      case CURTYP:
      DBread_curve(&cur,NULL,&segpek,la);
      status=GEposition((DBAny *)&cur,(char *)segpek,0.0,0.0,vecptr);
      DBfree_segments(segpek);
      break;

      default:
      return(erpush("EX1412","START()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);
/*
***Transformera till aktivt koordinatsystem.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(vecptr,lsyspk,vecptr);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXend(
        DBId     *idpek,
        DBVector *vecptr)

/*      Interface-rutin för END().
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *
 *      Ut: *vecptr => En DBVector med koordinater.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *
 *      (C)microform ab 1997-01-23 J. Kjellander
 *
 ******************************************************!*/

  {
    DBLine  lin;
    DBArc  arc;
    DBCurve  cur;
    DBSeg *segpek;
    DBSeg  arcseg[4];
    DBptr  la;
    DBetype  typ;
    short  status;

/*
***Hämta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och läs geometridata.
*/
    switch (typ)
      {
      case LINTYP:
      DBread_line(&lin, la);
      status=GEposition((DBAny *)&lin,NULL,1.0,0.0,vecptr);
      break;

      case ARCTYP:
      DBread_arc(&arc,arcseg,la);
      status=GEposition((DBAny *)&arc,(char *)arcseg,1.0,0.0,vecptr);
      break;

      case CURTYP:
      DBread_curve(&cur,NULL,&segpek,la);
      status=GEposition((DBAny *)&cur,(char *)segpek,
                        (DBfloat)cur.ns_cu,0.0,vecptr);
      DBfree_segments(segpek);
      break;

      default:
      return(erpush("EX1412","END()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);
/*
***Transformera till aktivt koordinatsystem.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(vecptr,lsyspk,vecptr);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXon(
        DBId     *idpek,
        DBfloat   u,
        DBfloat   v,
        DBVector *vecptr)

/*      Interface-rutin för funktionen ON. Anropas
 *      med ID som indata. Hämtar geometridata för
 *      den refererade storheten ur DB och anropar
 *      geo-rutinen GEposition() för att beräkna positionen.
 *      Uppdaterar den refererade storhetens referens-
 *      räknare.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          u,v    => Parametervärden.
 *
 *      Ut: *vecptr => En DBVector med koordinater.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *
 *      (C)microform ab 11/1/85 J. Kjellander
 *
 *      29/12/85   Symbol, J. Kjellander
 *      29/10/86   v3dbuf, J. Kjellander
 *      20/12/91   Bågl-par. 3D-cirklar, J. Kjellander
 *      22/2/93    Ytor, J. Kjellander
 *      7/6/93     Dynamisk allokering av segment, J. Kjellander
 *      21/3/94    Nya GMPAT, J. Kjellander
 *      11/10/95   gmrdpat1(), J. Kjellander
 *       9/12/95   sur209, G  Liden
 *      1997-12-17 sur209(), J.Kjellander
 *      1998-09-24 b_plan, J.Kjellander
 *      1999-12-18 sur209->varkon_sur_eval_gm, G  Liden
 *
 ******************************************************!*/

  {
    DBAny  gmpost;
    DBSeg *segpek;
    DBSeg  arcseg[4];
    EVALS  xyz;
    DBptr  la;
    DBetype  typ;
    short  status;

/*
***Hämta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och läs geometridata.
*/
    switch (typ)
      {
      case POITYP:
      DBread_point(&gmpost.poi_un, la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      break;

      case LINTYP:
      DBread_line(&gmpost.lin_un, la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      break;

      case ARCTYP:
      DBread_arc(&gmpost.arc_un,arcseg,la);
      status=GEposition(&gmpost,(char *)arcseg,u,v,vecptr);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      status=GEposition(&gmpost,(char *)segpek,u,v,vecptr);
      DBfree_segments(segpek);
      break;

      case SURTYP:
      DBread_surface(&gmpost.sur_un, la);
      if ( (status=varkon_sur_eval_gm(
             (DBSurf*)&gmpost,(gmint)0, u,v,&xyz)) < 0 ) return(status);
      vecptr->x_gm = xyz.r_x;
      vecptr->y_gm = xyz.r_y;
      vecptr->z_gm = xyz.r_z;
      break;

      case BPLTYP:
      DBread_bplane(&gmpost.bpl_un,la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      break;

      case CSYTYP:
      DBread_csys(&gmpost.csy_un, NULL, la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      break;

      case TXTTYP:
      DBread_text(&gmpost.txt_un, NULL, la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      break;

      case LDMTYP:
      DBread_ldim(&gmpost.ldm_un, la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      break;

      case CDMTYP:
      DBread_cdim(&gmpost.cdm_un, la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      break;

      case RDMTYP:
      DBread_rdim(&gmpost.rdm_un, la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      break;

      case ADMTYP:
      DBread_adim(&gmpost.adm_un, la);
      status=GEposition(&gmpost,NULL,u,v,vecptr);
      break;

      default:
      return(erpush("EX1412","ON()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);
/*
***Transformera till aktivt koordinatsystem.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(vecptr,lsyspk,vecptr);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXion(
        DBId     *idpek,
        DBVector *vecptr,
        DBshort   tnr,
        DBfloat  *tptr)

/*      Interface-rutin för funktionen INV_ON. Anropas
 *      med ID som indata. Hämtar geometridata för
 *      den refererade storheten ur DB och anropar
 *      geo-rutinen GE703() för att beräkna t-värdet.
 *      Uppdaterar den refererade storhetens referens-
 *      räknare.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          vecptr => Pekare till position.
 *          tnr    => Positionens ordningsnummer.
 *
 *      Ut: *tptr => t-värde.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *
 *      (C)microform ab 12/11/91 J. Kjellander
 *
 *      7/6/93 Dynamisk allokering av segment, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBAny  gmpost;
    DBSeg *segpek;
    DBSeg  arcseg[4];

/*
***Transformera till BASIC.
*/
   if ( lsyspk != NULL ) GEtfpos_to_local(vecptr,&lklsyi,vecptr);
/*
***Hämta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och läs geometridata.
***Beräkna u-värde. Observera att GE703() returnerar relativ
***båglängd 0<t<1 för linjer och cirklar (även 3D-cirklar) men
***inte kurvor. För kurvor returneras global parameter 0<t<nseg.
*/
    switch (typ)
      {
      case LINTYP:
      DBread_line(&gmpost.lin_un, la);
      status = GE703(&gmpost,NULL,vecptr,tnr,tptr);
      break;

      case ARCTYP:
      DBread_arc(&gmpost.arc_un,arcseg,la);
      status = GE703(&gmpost,arcseg,vecptr,tnr,tptr);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      status = GE703(&gmpost,segpek,vecptr,tnr,tptr);
      DBfree_segments(segpek);
      break;

      default:
      return(erpush("EX1412","INV_ON()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXtang(
        DBId     *idpek,
        DBfloat   t,
        DBTmat   *crdptr,
        DBVector *vecptr)

/*      Interface-rutin för funktionen TANG. Anropas
 *      med ID som indata. Hämtar geometridata för
 *      den refererade storheten ur DB och anropar
 *      geo-rutinen GEtangent() för att beräkna tangenten.
 *      Uppdaterar den refererade storhetens referens-
 *      räknare.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          t      => Parametervärde.
 *          crdptr => Pekare till lokalt koordinatsystem.
 *
 *      Ut: *vecptr => Normaliserad tangentvector.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *
 *      (C)microform ab 3/1/86 J. Kjellander
 *
 *      29/10/86 v3dbuf, J. Kjellander
 *      20/12/91 Bågl-par. 3D-cirklar, J. Kjellander
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBAny  gmpost;
    DBSeg *segpek;
    DBSeg  arcseg[4];
    DBTmat pmat;

/*
***Hämta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och läs geometridata.
*/
    switch (typ)
      {
      case LINTYP:
      DBread_line(&gmpost.lin_un, la);
      status=GEtangent(&gmpost,NULL,t,NULL,vecptr);
      break;

      case ARCTYP:
      DBread_arc(&gmpost.arc_un,arcseg, la);
      status=GEtangent(&gmpost,arcseg,t,NULL,vecptr);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      status=GEtangent(&gmpost,segpek,t,NULL,vecptr);
      DBfree_segments(segpek);
      break;

      case CSYTYP:
      DBread_csys(&gmpost.csy_un, &pmat, la);
      status=GEtangent(&gmpost,NULL,t,&pmat,vecptr);
      break;

      default:
      return(erpush("EX1412","TANG()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);
/*
***Transformera till aktivt koordinatsystem.
*/
    if ( lsyspk != NULL ) GEtfvec_to_local(vecptr,lsyspk,vecptr);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXitan(
        DBId     *idpek,
        DBVector *vecptr,
        DBshort   tnr,
        DBfloat  *tptr)

/*      Interface-rutin för funktionen INV_TANG. Anropas
 *      med ID som indata. Hämtar geometridata för
 *      den refererade storheten ur DB och anropar
 *      geo-rutinen GE702() för att beräkna t-värdet.
 *      Uppdaterar den refererade storhetens referens-
 *      räknare.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          vecptr => Pekare till tangentvector.
 *          tnr    => Tangentens ordningsnummer.
 *
 *      Ut: *tptr => t-värde.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *
 *      (C)microform ab 12/11/91 J. Kjellander
 *
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype typ;
    short   status;
    DBAny   gmpost;
    DBSeg  *segpek;
    DBSeg   arcseg[4];

/*
***Transformera tangenten till BASIC.
*/
   if ( lsyspk != NULL ) GEtfvec_to_local(vecptr,&lklsyi,vecptr);
/*
***Hämta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och läs geometridata. Beräkna u-värde.
*/
    switch (typ)
      {
      case ARCTYP:
      DBread_arc(&gmpost.arc_un,arcseg,la);
      status = GE702(&gmpost,arcseg,vecptr,tnr,tptr);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      status = GE702(&gmpost,segpek,vecptr,tnr,tptr);
      DBfree_segments(segpek);
      break;

      default:
      return(erpush("EX1412","INV_TANG()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXcurv(
        DBId    *idpek,
        DBfloat  t,
        DBfloat *fltptr)

/*      Interface-rutin för funktionen CURV. Anropas
 *      med ID som indata. Hämtar geometridata för
 *      den refererade storheten ur DB och anropar
 *      geo-rutinen GEcurvature() för att beräkna krökninen.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          t      => Parametervärde.
 *          fltptr => Pekare till utdata.
 *
 *      Ut: *fltptr => Krökningscentrum.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *
 *      (C)microform ab 12/12/91 J. Kjellander
 *
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBAny  gmpost;
    DBSeg *segpek;

/*
***Hämta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ, läs geometridata och beräkna krökning.
*/
    switch (typ)
      {
      case LINTYP:
      DBread_line(&gmpost.lin_un,la);
      status=GEcurvature(&gmpost,NULL,t,fltptr);
      break;

      case ARCTYP:
      DBread_arc(&gmpost.arc_un, NULL, la);
      status=GEcurvature(&gmpost,NULL,t,fltptr);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      status=GEcurvature(&gmpost,segpek,t,fltptr);
      DBfree_segments(segpek);
      break;

      default:
      return(erpush("EX1412","CURV()"));
      }
/*
***Slut.
*/
    if ( status < 0 ) return(status);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXicur(
        DBId    *idpek,
        DBfloat  kappa,
        DBshort  tnr,
        DBfloat *tptr)

/*      Interface-rutin för funktionen INV_CURV. Anropas
 *      med ID som indata. Hämtar geometridata för
 *      den refererade storheten ur DB och anropar
 *      geo-rutinen GE704() för att beräkna t-värdet.
 *      Uppdaterar den refererade storhetens referens-
 *      räknare.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          kappa  => Önskad krökning.
 *          tnr    => Krökningens ordningsnummer.
 *
 *      Ut: *tptr => t-värde.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *
 *      (C)microform ab 19/11/91 J. Kjellander
 *
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBfloat tmp;
    DBCurve  cur;
    DBSeg *segpek;

/*
***Hämta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ, läs geometridata och beräkna u-värde.
*/
    switch (typ)
      {
      case CURTYP:
      DBread_curve(&cur,NULL,&segpek, la);
      tmp = kappa;
      status = GE704((DBAny *)&cur,segpek,&tmp,tnr,tptr);
      DBfree_segments(segpek);
      break;

      default:
      return(erpush("EX1412","INV_CURV()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXcen(
        DBId     *idpek,
        DBfloat   t,
        DBTmat   *crdptr,
        DBVector *vecptr)

/*      Interface-rutin för funktionen CENTRE. Anropas
 *      med ID som indata. Hämtar geometridata för
 *      den refererade storheten ur DB och anropar
 *      geo-rutinen GEcentre() för att beräkna kröknings-
 *      centrum. Uppdaterar den refererade storhetens
 *      referensräknare.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          t      => Parametervärde.
 *          crdptr => Pekare till lokalt koordinatsystem.
 *          vecptr => Pekare till utdata.
 *
 *      Ut: *vecptr => Krökningscentrum.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *
 *      (C)microform ab 3/1/86 J. Kjellander
 *
 *      29/10/86 v3dbuf, J. Kjellander
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBAny  gmpost;
    DBSeg *segpek;
    DBSeg  arcseg[4];

/*
***Hämta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och läs geometridata.
*/
    switch (typ)
      {
      case ARCTYP:
      DBread_arc(&gmpost.arc_un,arcseg,la);
      if ( gmpost.arc_un.ns_a > 1 ) t = t*gmpost.arc_un.ns_a;
      status=GEcentre(&gmpost,arcseg,t,vecptr);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      status=GEcentre(&gmpost,segpek,t,vecptr);
      DBfree_segments(segpek);
      break;

      default:
      return(erpush("EX1412","CENTRE()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);
/*
***Transformera till aktivt koordinatsystem.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(vecptr,lsyspk,vecptr);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXnorm(
        DBId     *idpek,
        DBfloat   u,
        DBfloat   v,
        DBVector *vecptr)

/*      Interface-rutin för funktionen NORM.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          u,v    => Parametervärden.
 *          vecptr => Pekare till resultat.
 *
 *      Ut: *vecptr => Normaliserad normalvector.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *
 *      (C)microform ab 3/12/92 J. Kjellander
 *
 *      22/2/93    Ytor, J. Kjellander
 *      7/6/93     Dynamisk allokering av segment, J. Kjellander
 *      21/3/94    Nya GMPAT, J. Kjellander
 *       9/12/95   sur209, G  Liden      
 *      1997-12-17 sur209(), J.Kjellander
 *      1999-12-18 sur209->varkon_sur_eval_gm, G  Liden
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    EVALS  xyz;
    DBAny  gmpost;
    DBSeg  arcseg[4];
    DBSeg *segpek;
    DBTmat pmat;

/*
***Hämta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och läs geometridata.
*/
    switch (typ)
      {
      case ARCTYP:
      DBread_arc(&gmpost.arc_un,arcseg,la);
      status=GEnormal(&gmpost,(char *)arcseg,u,v,vecptr);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      status=GEnormal(&gmpost,(char *)segpek,u,v,vecptr);
      DBfree_segments(segpek);
      break;

      case CSYTYP:
      DBread_csys(&gmpost.csy_un, &pmat, la);
      status=GEnormal(&gmpost,(char *)&pmat,u,v,vecptr);
      break;

      case BPLTYP:
      DBread_bplane(&gmpost.bpl_un, la);
      status=GEnormal(&gmpost,NULL,u,v,vecptr);
      break;

      case SURTYP:
      DBread_surface(&gmpost.sur_un, la);
      if ( (status=varkon_sur_eval_gm((DBSurf*)&gmpost,(gmint)3, 
                 u,v,&xyz)) < 0 ) return(status);
      vecptr->x_gm = xyz.n_x;
      vecptr->y_gm = xyz.n_y;
      vecptr->z_gm = xyz.n_z;
      break;

      default:
      return(erpush("EX1412","NORM()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(status);
/*
***Transformera till aktivt koordinatsystem.
*/
    if ( lsyspk != NULL ) GEtfvec_to_local(vecptr,lsyspk,vecptr);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXarcl(
        DBId    *idpek,
        DBfloat *length)

/*      Interface-rutin för funktionen ARCL. Anropas
 *      med ID som indata. Hämtar geometridata för
 *      den refererade storheten ur GM och anropar
 *      geo-rutinen GEarclength() för att beräkna båglängden.
 *      Uppdaterar den refererade storhetens referens-
 *      räknare.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *
 *      Ut: *length => Båglängd.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *
 *      (C)microform ab 3/1/86 J. Kjellander
 *
 *      29/10/86 v3dbuf, J. Kjellander
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *      23/11/94 EX2062, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBAny  gmpost;
    DBSeg  arcseg[4];
    DBSeg *segpek;

/*
***Hämta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och läs geometridata.
*/
    switch (typ)
      {
      case LINTYP:
      DBread_line(&gmpost.lin_un, la);
      status=GEarclength(&gmpost,NULL,length);
      break;

      case ARCTYP:
      DBread_arc(&gmpost.arc_un,arcseg,la);
      status=GEarclength(&gmpost,arcseg,length);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      if ( (*length=gmpost.cur_un.al_cu) == 0.0 )
        status=GEarclength(&gmpost,segpek,length);
      else status = 0;
      DBfree_segments(segpek);
      break;

      default:
      return(erpush("EX1412","ARCL()"));
      }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(erpush("EX2062","ARCL"));

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXiarc(
        DBId    *idpek,
        DBfloat  l,
        DBfloat *tptr)

/*      Interface-rutin för funktionen INV_ARCL. Anropas
 *      med ID som indata. Hämtar geometridata för
 *      den refererade storheten ur DB och anropar
 *      geo-rutinen GE717() för att beräkna t-värdet.
 *      Uppdaterar den refererade storhetens referens-
 *      räknare.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *          l      => Längd i mm.
 *
 *      Ut: *tptr => t-värde.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *
 *      (C)microform ab 12/12/91 J. Kjellander
 *
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *      23/11/94 EX2062, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    gmflt  relu,length;
    DBAny  gmpost;
    DBSeg  arcseg[4];
    DBSeg *segpek;

/*
***Hämta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och läs geometridata.
*/
    switch (typ)
      {
      case LINTYP:
      DBread_line(&gmpost.lin_un,la);
      status = GEarclength(&gmpost,NULL,&length);
      break;

      case ARCTYP:
      DBread_arc(&gmpost.arc_un,arcseg,la);
      status = GEarclength(&gmpost,arcseg,&length);
      break;

      case CURTYP:
      DBread_curve(&gmpost.cur_un,NULL,&segpek,la);
      if ( (length=gmpost.cur_un.al_cu) == 0.0 )
        status = GEarclength(&gmpost,segpek,&length);
      else
        status = 0;
      break;

      default:
      return(erpush("EX1412","INV_ARCL()"));
      }
/*
***Gick det bra ?
*/
   if ( status < 0 ) return(status);
/*
***Indata är en längd i millimeter.
***Beräkna relativt båglängd.
*/
   relu = l/length;
  *tptr = relu;
/*
***För en 3D-cirkel skall relativ båglängd returneras. Detta är
***redan klart. För en kurva skall global parameter returneras.
***Observera att DBSeg-minne deallokeras först här.
*/
   if ( typ == CURTYP )
     {
     status = GE717(&gmpost,segpek,lsyspk,relu,tptr);
    *tptr -= 1.0;
     DBfree_segments(segpek);
     }
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(erpush("EX2062","INV_ARCL"));

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXsuar(
        DBId    *idpek,
        DBfloat  tol,
        DBfloat *p_area)

/*      Interface-rutin för funktionen SURFACE_AREA().
 *
 *      In: idpek  => Pekare till ytans identitet.
 *          tol    => Tolerens.
 *          p_area => Pekare till utdata.
 *
 *      Ut: *p_area => Beräknad area.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *
 *      (C)microform ab 1997-12-17 J. Kjellander
 *       1997-12-18 sur300 tillagd G. Liden
 *       1999-12-18 sur300->varkon_sur_sarea  G. Liden
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBSurf sur;
    DBPatch *patpek;
    DBTmat csys;    /* Coordinate system for m.o.i.            */
    gmint  acc;     /* Calculation accuracy:                   */
                    /* Eq. 1: Whole patch                      */
                    /* Eq. 2: Patch divided  4 times           */
                    /* Eq. 3: Patch divided  9 times           */
                    /* Eq. 4:  .....                           */
    gmint  c_case;  /* Calculation case:                       */
                    /* Eq. 1: ..                               */
                    /* Eq. 2: ..                               */
                    /* Eq. 3: ..                               */
    gmflt  sarea;   /* Surface area                            */
    DBVector  cog;     /* Center of gravity                       */
    DBVector  moi;     /* Moments of inertia (jx,jy,jz)           */
                    /* w.r.t to coordinate system p_csys       */
    DBVector  axis1;   /* Principal axis 1                        */
    DBVector  axis2;   /* Principal axis 2                        */
    DBVector  axis3;   /* Principal axis 3                        */
/*                                                                  */
/*                                                                  */

/*
***Hämta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och läs geometridata.
*/
    switch (typ)
      {
      case SURTYP:
      DBread_surface(&sur,la);
      DBread_patches(&sur,&patpek);
      break;

      default:
      return(erpush("EX1412","SURFACE_AREA()"));
      }
/*
***Calculate surface area
*/
      c_case = 2;

      acc =  4; 

      status = varkon_sur_sarea
      ( &sur,patpek, &csys, acc, tol, c_case, 
        &sarea,  &cog,  &moi,  &axis1,  &axis2,  &axis3 );

   *p_area = sarea;

/*
***Free memory for patches
*/
    DBfree_patches(&sur,patpek);
/*
***Error handling     
*/
    if ( status < 0 ) return(erpush("EX2062","SURFACE_AREA"));

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXsear(
        DBId    *idpek,
        DBfloat  tol,
        DBfloat *p_area)

/*      Interface-routine for SECTION_AREA().
 *
 *      In: idpek  => Pekare till kurvans identitet.
 *          tol    => Tolerens.
 *          p_area => Pekare till utdata.
 *
 *      Ut: *p_area => Beräknad area.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *
 *      (C)microform ab 1997-12-17 J. Kjellander
 *      1997-12-20 sur302 tillagd  G. Liden
 *      1998-12-18 sur302->varkon_cur_secta  
 *                 sur715->varkon_cur_fromarc   
 *                 sur741->varkon_idpoint    G. Liden
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBCurve  cur;
    DBSeg *segpek;
    DBTmat csys;       /* Coordinate system for m.o.i.            */
    gmflt  delta;      /* Calculation start accuracy value, equal */
                       /* to delta arclength for calculation      */
    gmint  c_case;     /* Calculation case:                       */
                       /* Eq.  1: Use only delta and not a_crit   */
                       /* Eq.  2: Use delta as start and a_crit   */
                       /* Eq. 11: As 1, but error unclosed curve  */
                       /* Eq. 12: As 2, but error unclosed curve  */
    gmflt  maxdev;     /* Maximum deviation from plane            */
    gmflt  sarea;      /* Surface area                            */
    DBVector  cog;     /* Center of gravity                       */
    DBVector  moi;     /* Moments of inertia (jx,jy,jz)           */
                       /* w.r.t to coordinate system p_csys       */
    DBVector  axis1;   /* Principal axis 1                        */
    DBVector  axis2;   /* Principal axis 2                        */
    DBVector  axis3;   /* Principal axis 3                        */
    bool   alloc1;
    DBArc  arc;        /* Arc section curve                       */
    DBSeg  arcseg1[4]; /* Arc segments for conversion to curve    */
    char   errbuf[80]; /* String for error message fctn erpush    */

/*
***Initiering.
*/
    alloc1  = FALSE;
    segpek  = NULL;
/*
***Hämta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och läs geometridata.
*/
    switch ( typ )
      {
      case CURTYP:
      DBread_curve(&cur,NULL,&segpek,la);
      alloc1 = TRUE;
      break;

      case ARCTYP:
      DBread_arc(&arc,arcseg1, la);
      varkon_cur_fromarc(&arc,&cur,arcseg1);
      segpek = arcseg1;
      break;

      default:
      return(erpush("EX1412","SECTION_AREA()"));
      }
/*
***Beräkna ytan.
*/
   delta  = F_UNDEF;
   c_case = 2;
   status = varkon_cur_secta(
                   &cur,segpek,&csys,delta,tol,c_case,&maxdev,
                   &sarea,&cog,&moi,&axis1,&axis2,&axis3);

   *p_area = sarea;
/*
***Lämna tillbaks allokerat minne.
*/
    if ( alloc1 ) DBfree_segments(segpek);
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(erpush("EX2062","SECTION_AREA"));
/*
***Varning om kurvan inte är plan
*/
   if  (  maxdev > 10.0*varkon_idpoint() )
     {
     sprintf(errbuf,"%8.4f%%", maxdev );
     erinit();
     erpush("SU7411",errbuf);
     errmes();
     erinit();
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXsecg(
        DBId     *idpek,
        DBfloat   tol,
        DBVector *p_cgrav)

/*      Interface-routine for SECTION_CGRAV().
 *
 *      In: idpek  => Pekare till kurvans identitet.
 *          tol    => Tolerens.
 *          p_area => Pekare till utdata.
 *
 *      Ut: *p_area => Beräknad area.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *
 *      (C)microform ab 1997-12-17 J. Kjellander
 *      1997-12-20 sur302 tillagd  G. Liden
 *      1998-12-18 sur302->varkon_cur_secta  
 *                 sur715->varkon_cur_fromarc   
 *                 sur741->varkon_idpoint    G. Liden
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;
    DBCurve  cur;
    DBSeg *segpek;
    DBTmat csys;       /* Coordinate system for m.o.i.            */
    gmflt  delta;      /* Calculation start accuracy value, equal */
                       /* to delta arclength for calculation      */
    gmint  c_case;     /* Calculation case:                       */
                       /* Eq.  1: Use only delta and not a_crit   */
                       /* Eq.  2: Use delta as start and a_crit   */
                       /* Eq. 11: As 1, but error unclosed curve  */
                       /* Eq. 12: As 2, but error unclosed curve  */
    gmflt  maxdev;     /* Maximum deviation from plane            */
    gmflt  sarea;      /* Surface area                            */
    DBVector  moi;     /* Moments of inertia (jx,jy,jz)           */
                       /* w.r.t to coordinate system p_csys       */
    DBVector  axis1;   /* Principal axis 1                        */
    DBVector  axis2;   /* Principal axis 2                        */
    DBVector  axis3;   /* Principal axis 3                        */
    bool   alloc1;
    DBArc  arc;        /* Arc section curve                       */
    DBSeg  arcseg1[4]; /* Arc segments for conversion to curve    */
    char   errbuf[80]; /* String for error message fctn erpush    */

/*
***Initiering.
*/
    alloc1  = FALSE;
    segpek  = NULL;
/*
***Hämta den refererade storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idpek, &la, &typ) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ och läs geometridata.
*/
    switch ( typ )
      {
      case CURTYP:
      DBread_curve(&cur,NULL,&segpek,la);
      alloc1 = TRUE;
      break;

      case ARCTYP:
      DBread_arc(&arc,arcseg1, la);
      varkon_cur_fromarc(&arc,&cur,arcseg1);
      segpek = arcseg1;
      break;

      default:
      return(erpush("EX1412","SECTION_AREA()"));
      }
/*
***Beräkna ytan.
*/
   delta  = F_UNDEF;
   c_case = 2;
   status = varkon_cur_secta(
                   &cur,segpek,&csys,delta,tol,c_case,&maxdev,
                   &sarea,p_cgrav,&moi,&axis1,&axis2,&axis3);
/*
***Lämna tillbaks allokerat minne.
*/
    if ( alloc1 ) DBfree_segments(segpek);
/*
***Lite felhantering.
*/
    if ( status < 0 ) return(erpush("EX2062","SECTION_CGRAV"));
/*
***Varning om kurvan inte är plan
*/
   if  (  maxdev > 10.0*varkon_idpoint() )
     {
     sprintf(errbuf,"%8.4f%%", maxdev );
     erinit();
     erpush("SU7411",errbuf);
     errmes();
     erinit();
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXtxtl(
        char    *str,
        DBfloat *l)

/*      Interface-rutin för funktionen TEXTL. Beräknar
 *      en textsträng:s längd i mm.
 *
 *      In: str     => Textsträng.
 *
 *      Ut: *l => Längd.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 19/3/89 J. Kjellander
 *
 ******************************************************!*/

  {
   int n;

   n = strlen(str);

   *l = 0.01*defnap.tsize*defnap.twidth*(double)n + 
         0.0067*defnap.tsize*defnap.twidth*(double)(n-1);

   return(0);

  }

/********************************************************/
/*!******************************************************/

        short EXsect(
        DBId     *idp1,
        DBId     *idp2,
        DBint     inr,
        DBint     alt,
        DBVector *vecptr)

/*      Interface-rutin för funktionen INTERSECT. Anropas
 *      med ID som indata. Hämtar geometridata för
 *      de refererade storheterna ur DB och anropar
 *      geo-rutinen GEintersect_pos() för att beräkna skärningen.
 *
 *      In: idp1   => Pekare till 1:a storhetens identitet.
 *          idp2   => Pekare till 2:a storhetens identitet.
 *          inr    => Skärningsnummer.
 *          alt    => Typ av resultat.
 *
 *      Ut: *vecptr => En DBVector med koordinater.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *          EX1732 => Storheterna är en och samma
 *          EX1972 => 2 plan
 *          EX1982 => Skärning saknas
 *          EX1992 => inr = 0
 *          EX2002 => inr < 0
 *
 *      (C)microform ab 11/1/85 J. Kjellander
 *
 *      3/1/86   Komposit-bug, J. Kjellander
 *      22/10/86 EX1732, J. Kjellander
 *      29/10/86 v3dbuf, J. Kjellander
 *      28/11/91 Skärning med plan, J. Kjellander
 *      20/12/91 inr = 0, J. Kjellander
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *      6/9/95   Yta-linje, J. Kjellander
 *      1997-01-27 inr > 1000, J.Kjellander
 *      1997-03-25 alt, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la1,la2;
    DBetype typ1,typ2;
    short   status;
    char    idstr1[V3STRLEN+1],idstr2[V3STRLEN+1];
    char   *pdat1=NULL,*pdat2=NULL;
    DBAny   gmstr1,gmstr2;
    DBSeg  *segpk1,*segpk2;
    DBSeg   arcsg1[4],arcsg2[4];
    DBPatch  *patpek;

/*
***Om alt != 0 använder vi EXusec().
*/
    if ( alt != 0 ) return(EXusec(idp1,idp2,(short)inr,lsyspk,vecptr));
/*
***Kolla att skärningens ordningsnummer inte == 0.
*/
    if ( inr == 0 ) return(erpush("EX1992",""));
/*
***Hämta den 1:a storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idp1, &la1, &typ1) < 0 )
      return ( erpush("EX1402",""));
/*
***Hämta den 2:a storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idp2, &la2, &typ2) < 0 )
      return ( erpush("EX1402",""));
/*
***Kolla att det är två olika storheter.
*/
    if ( la1 == la2 )
     {
     igidst(idp1,idstr1);
     return(erpush("EX1732",idstr1));
     }
/*
***Kolla att inte koordinatsystem eller B-plan förekommer
***2 gånger.
*/
    if ( (typ1 == CSYTYP  ||  typ1 == BPLTYP  ||  typ1 == SURTYP )  &&
         (typ2 == CSYTYP  ||  typ2 == BPLTYP  ||  typ2 == SURTYP ) )
      {
      igidst(idp1,idstr1);
      igidst(idp2,idstr2);
      strcat(idstr1,"%");
      strcat(idstr1,idstr2);
      return(erpush("EX1982",idstr1));
      }
/*
***Läs geometridata för storhet 1.
*/
    switch (typ1)
      {
      case LINTYP:
      DBread_line(&gmstr1.lin_un, la1);
      break;

      case ARCTYP:
      DBread_arc(&gmstr1.arc_un,arcsg1, la1);
      pdat1 = (char *)arcsg1;
      break;

      case CURTYP:
      DBread_curve(&gmstr1.cur_un,NULL,&segpk1,la1);
      pdat1 = (char *)segpk1;
      break;

      case CSYTYP:
      DBread_csys(&gmstr1.csy_un, NULL, la1);
      break;

      case BPLTYP:
      DBread_bplane(&gmstr1.bpl_un, la1);
      break;

      case SURTYP:
      DBread_surface(&gmstr1.sur_un, la1);
      DBread_patches(&gmstr1.sur_un,&patpek);
      pdat1 = (char *)patpek;
      break;

      default:
      igidst(idp1,idstr1);
      return(erpush("EX1412",idstr1));
      }
/*
***Läs geometridata för storhet 2.
*/
    switch (typ2)
      {
      case LINTYP:
      DBread_line(&gmstr2.lin_un, la2);
      break;

      case ARCTYP:
      DBread_arc(&gmstr2.arc_un,arcsg2,la2);
      pdat2 = (char *)arcsg2;
      break;

      case CURTYP:
      DBread_curve(&gmstr2.cur_un,NULL,&segpk2,la2);
      pdat2 = (char *)segpk2;
      break;

      case CSYTYP:
      DBread_csys(&gmstr2.csy_un, NULL, la2);
      break;

      case BPLTYP:
      DBread_bplane(&gmstr2.bpl_un, la2);
      break;

      case SURTYP:
      DBread_surface(&gmstr2.sur_un, la2);
      DBread_patches(&gmstr2.sur_un,&patpek);
      pdat2 = (char *)patpek;
      break;

      default:
      igidst(idp2,idstr2);
      if ( typ1 == CURTYP ) DBfree_segments(segpk1);
      if ( typ1 == SURTYP ) DBfree_patches(&gmstr1.sur_un,patpek);
      return(erpush("EX1412",idstr2));
      }
/*
***Kolla att inte negativt inr används där det ej är tillåtet.
***Tillåtet i alla lägen där en linje eller 2D-cirkel figurerar
***annars inte.
*/
    if ( inr < 0 )
      {
      if ( typ1 == LINTYP  ||  typ2 == LINTYP ) ;
      else if ( typ1 == ARCTYP  &&  gmstr1.arc_un.ns_a == 0 ) ;
      else if ( typ2 == ARCTYP  &&  gmstr2.arc_un.ns_a == 0 ) ;
      else
        {
        igidst(idp1,idstr1);
        igidst(idp2,idstr2);
        strcat(idstr1,"%");
        strcat(idstr1,idstr2);
        erpush("EX1982",idstr1);
        status = erpush("EX2002","");
        goto end;
        }
      }
/*
***Beräkna skärningen.
*/
    if ( (status=GEintersect_pos(&gmstr1,pdat1,&gmstr2,pdat2,
                                               lsyspk,inr,vecptr)) < 0 )
      {
      igidst(idp1,idstr1);
      igidst(idp2,idstr2);
      strcat(idstr1,"%");
      strcat(idstr1,idstr2);
      status = erpush("EX1982",idstr1);
      goto end;
      }
/*
***Transformera till aktivt koordinatsystem.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(vecptr,lsyspk,vecptr);
/*
***Om någon av storheterna är en kurva eller yta skall minne deallokeras
***innan vi avslutar.
*/
end:
    if ( typ1 == CURTYP ) DBfree_segments(segpk1);
    if ( typ2 == CURTYP ) DBfree_segments(segpk2);
    if ( typ1 == SURTYP ) DBfree_patches(&gmstr1.sur_un,patpek);
    if ( typ2 == SURTYP ) DBfree_patches(&gmstr2.sur_un,patpek);

    return(status);
  }

/********************************************************/
/*!******************************************************/

        short EXnsec(
        DBId    *idp1,
        DBId    *idp2,
        DBshort  inr,
        DBTmat  *crdptr,
        DBshort *numint)

/*      Interface-rutin för funktionen N_INTERSECT.
 *
 *      In: idp1   => Pekare till 1:a storhetens identitet.
 *          idp2   => Pekare till 2:a storhetens identitet.
 *          inr    => -1 anger förlängning.
 *          crdptr => Pekare till lokalt koordinatsystem.
 *
 *      Ut: *numint => Antal skärningar.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *          EX1732 => Storheterna är en och samma
 *          EX1972 => 2 plan
 *
 *      (C)microform ab 12/2/92 J. Kjellander
 *
 *      7/6/93   Dynamisk allokering av segment, J. Kjellander
 *      6/9/95   Yta - linje, J. Kjellander
 *      1997-01-27 return(status), J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la1,la2;
    DBetype  typ1,typ2;
    short  status;
    char   idstr1[V3STRLEN+1],idstr2[V3STRLEN+1];
    char  *pdat1=NULL,*pdat2=NULL;
    DBAny  gmstr1,gmstr2;
    DBSeg  arcsg1[4],arcsg2[4];
    DBSeg *segpk1,*segpk2;
    DBPatch *patpek;

/*
***Hämta den 1:a storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idp1, &la1, &typ1) < 0 )
      return ( erpush("EX1402",""));
/*
***Hämta den 2:a storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idp2, &la2, &typ2) < 0 )
      return ( erpush("EX1402",""));
/*
***Kolla att det är två olika storheter.
*/
    if ( la1 == la2 )
     {
     igidst(idp1,idstr1);
     return(erpush("EX1732",idstr1));
     }
/*
***Kolla att inte koordinatsystem eller B-plan förekommer
***2 gånger.
*/
    if ( (typ1 == CSYTYP  ||  typ1 == BPLTYP  ||  typ1 == SURTYP )  &&
         (typ2 == CSYTYP  ||  typ2 == BPLTYP  ||  typ2 == SURTYP ) )
      {
      igidst(idp1,idstr1);
      igidst(idp2,idstr2);
      strcat(idstr1,"%");
      strcat(idstr1,idstr2);
      return(erpush("EX1982",idstr1));
      }
/*
***Testa typ, läs geometridata.
*/
    switch (typ1)
      {
      case LINTYP:
      DBread_line(&gmstr1.lin_un,la1);
      break;

      case ARCTYP:
      DBread_arc(&gmstr1.arc_un,arcsg1,la1);
      pdat1 = (char *)arcsg1;
      break;

      case CURTYP:
      DBread_curve(&gmstr1.cur_un,NULL,&segpk1,la1);
      pdat1 = (char *)segpk1;
      break;

      case CSYTYP:
      DBread_csys(&gmstr1.csy_un,NULL,la1);
      break;

      case BPLTYP:
      DBread_bplane(&gmstr1.bpl_un,la1);
      break;

      case SURTYP:
      DBread_surface(&gmstr1.sur_un, la1);
      DBread_patches(&gmstr1.sur_un,&patpek);
      pdat1 = (char *)patpek;
      break;

      default:
      igidst(idp1,idstr1);
      return(erpush("EX1412",idstr1));
      }

    switch (typ2)
      {
      case LINTYP:
      DBread_line(&gmstr2.lin_un,la2);
      break;

      case ARCTYP:
      DBread_arc(&gmstr2.arc_un,arcsg2,la2);
      pdat2 = (char *)arcsg2;
      break;

      case CURTYP:
      DBread_curve(&gmstr2.cur_un,NULL,&segpk2,la2);
      pdat2 = (char *)segpk2;
      break;

      case CSYTYP:
      DBread_csys(&gmstr2.csy_un,NULL,la2);
      break;

      case BPLTYP:
      DBread_bplane(&gmstr2.bpl_un,la2);
      break;

      case SURTYP:
      DBread_surface(&gmstr2.sur_un, la2);
      DBread_patches(&gmstr2.sur_un,&patpek);
      pdat2 = (char *)patpek;
      break;

      default:
      igidst(idp2,idstr2);
      if ( typ1 == CURTYP ) DBfree_segments(segpk1);
      if ( typ1 == SURTYP ) DBfree_patches(&gmstr1.sur_un,patpek);
      return(erpush("EX1412",idstr2));
      }
/*
***Kolla att inte negativt inr används där det ej är tillåtet.
***Tillåtet i alla lägen där en linje eller 2D-cirkel figurerar
***annars inte.
*/
    if ( inr < 0 )
      {
      if ( typ1 == LINTYP  ||  typ2 == LINTYP ) ;
      else if ( typ1 == ARCTYP  &&  gmstr1.arc_un.ns_a == 0 ) ;
      else if ( typ2 == ARCTYP  &&  gmstr2.arc_un.ns_a == 0 ) ;
      else
        {
        igidst(idp1,idstr1);
        igidst(idp2,idstr2);
        strcat(idstr1,"%");
        strcat(idstr1,idstr2);
        erpush("EX1982",idstr1);
        status = erpush("EX2002","");
        goto end;
        }
      }
/*
***Beräkna antal skärningar.
*/
    status = GEintersect_npos(&gmstr1,pdat1,&gmstr2,pdat2,crdptr,inr,numint);
/*
***Om någon av storheterna är en kurva eller yta skall minne deallokeras.
*/
end:
    if ( typ1 == CURTYP ) DBfree_segments(segpk1);
    if ( typ2 == CURTYP ) DBfree_segments(segpk2);
    if ( typ1 == SURTYP ) DBfree_patches(&gmstr1.sur_un,patpek);
    if ( typ2 == SURTYP ) DBfree_patches(&gmstr2.sur_un,patpek);

    return(status);
  }

/********************************************************/
/*!******************************************************/

       short EXidnt(
       DBetype *typmsk,
       DBId     ident[],
       bool    *end,
       bool    *right)

/*      Funktionen IDENT.
 *
 *      In: typmsk => Pekare till typmask.
 *          ident  => Array av DBId.
 *          end    => Pekare till ände
 *          right  => Pekare till sida
 *
 *      Ut: *ident => Identitet för utpekad storhet.
 *          *end   => Ände
 *          *right => Sida
 *
 *      FV:      0 = Ok.
 *
 *      (C)microform ab 22/11/85 J. Kjellander
 *
 *      20/11/92 end och right, J. Kjellander
 *
 ******************************************************!*/

  {

    if ( getidt(ident,typmsk,end,right,(short)0) < 0 )
      {
      ident->seq_val = 0;
      ident->ord_val = 1;
      ident->p_nextre = NULL;
      }

    gphgal(0);

    return(0);

  }
  
/********************************************************/
/*!******************************************************/

       short EXpos(
       double *px,
       double *py,
       char   *pc)

/*      Funktionen POS.
 *
 *      In: px => Pekare till X-koordinat.
 *          py => Pekare till Y-koordinat.
 *          pc => Pekare till pektecknet.
 *
 *      Ut: *px => Modellkoordinat-X
 *          *py => Modellkoordinat-Y
 *          *pc => Pektecknet.
 *
 *      FV:      0 = Ok.
 *
 *      (C)microform ab 22/11/85 J. Kjellander
 *
 *       18/5/92 Pektecken, J. Kjellander
 *
 ******************************************************!*/

  {
    DBVector pos;

    gpgtmc(pc,px,py,TRUE);
    if ( lsyspk != NULL )
      {
      pos.x_gm = *px;
      pos.y_gm = *py;
      pos.z_gm = 0.0;
      GEtfpos_to_local(&pos,lsyspk,&pos);
      *px = pos.x_gm;
      *py = pos.y_gm;
      }

    return(0);

  }
  
/********************************************************/
/*!******************************************************/

       short EXscr(
       DBshort *pix,
       DBshort *piy,
       DBint   *win_id)

/*      Funktionen SCREEN.
 *
 *      In: pix    => Pekare till utdata.
 *          piy    => Pekare till utdata.
 *          win_id => Pekare till utdata.
 *
 *      Ut: *pix    => Skärmkoordinat-X
 *          *piy    => Skärmkoordinat-Y
 *          *win_id => Fönsterid.
 *
 *      FV:      0 = Ok.
 *          REJECT = Avbruten operation.
 *
 *      (C)microform ab 22/11/85 J. Kjellander
 *
 *      22/1-95  Multifönster, J. Kjellander
 *
 ******************************************************!*/

  {
    char c;

#ifdef V3_X11
    if ( gptrty == X11 )
      {
      wpgtsc(FALSE,&c,pix,piy,win_id);
      }
    else
      {
     *win_id = 0;
      gpgtsc(&c,pix,piy,FALSE);
      }
#else
   *win_id = 0;
    gpgtsc(&c,pix,piy,FALSE);
#endif

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short EXarea(
       DBId     *ridvek,
       DBshort   nref,
       DBfloat   dist,
       DBfloat  *area,
       DBVector *tp)

/*      Beräknar area och tyngdpunkt. MBS-funktionerna
 *      AREA() och CGRAV().
 *
 *      In: ridvek => Pekare till vektor med referenser.
 *          nref   => Antal referenser.
 *          dist   => Avstånd mellan snittlinjer.
 *
 *      Ut: *area  => Beräknad area.
 *          *tp    => Beräknad tyngdpunkt.
 *
 *      FV:      0 = Ok.
 *          EX1402 = Den refererade storhten finns ej i DB
 *          EX1412 = Otillåten geometri-typ för denna operation
 *          EX1962 = Fel från malloc().
 *
 *      (C)microform ab 26/7/90 J. Kjellander
 *
 *      7/6/93   Dynamiska segment, J. Kjellander
 *
 ******************************************************!*/

  {
    int      i;
    DBptr    la;
    DBetype  typ;
    short    nlin,narc,ncur,status;
    DBLine  *linvek=NULL,*lpvek[GMMXXH];
    DBArc   *arcvek=NULL,*apvek[GMMXXH];
    DBCurve *curvek=NULL,*cpvek[GMMXXH];
    DBSeg   *spvek[GMMXXH];

/*
***Hämta geometri-data för samtliga refererade storheter och
***lagra i mallokerade minnesareor.
*/
    nlin = narc = ncur = 0;

    for ( i=0; i<nref ; ++i )
      {
      if ( DBget_pointer('I',&ridvek[i],&la,&typ) < 0 ) 
        return(erpush("EX1402",""));

      switch ( typ )
         {
         case (LINTYP):
         if ( nlin == 0 )
           {
           if ( (linvek=(DBLine *)v3mall((unsigned)(GMMXXH*sizeof(DBLine)),
                           "EXarea")) == NULL ) return(erpush("EX1962",""));
           }
         DBread_line(&linvek[nlin],la);
         lpvek[nlin] = &linvek[nlin];
         ++nlin;
         break;

         case (ARCTYP):
         if ( narc == 0 )
           {
           if ( (arcvek=(DBArc *)v3mall((unsigned)(GMMXXH*sizeof(DBArc)),
                          "EXarea")) == NULL ) return(erpush("EX1962",""));
           }
         DBread_arc(&arcvek[narc],NULL,la);
         apvek[narc] = &arcvek[narc];
         ++narc;
         break;

         case (CURTYP):
         if ( ncur == 0 )
           {
           if ((curvek=(DBCurve *)v3mall((unsigned)(GMMXXH*sizeof(DBCurve)),
                          "EXarea")) == NULL ) return(erpush("EX1962",""));
           }
         DBread_curve(&curvek[ncur],NULL,&spvek[ncur],la);
         cpvek[ncur] = &curvek[ncur];
         ++ncur;
         break;

         default:
         return(erpush("EX1412",""));
         }
      }
/*
***Beräkna arean.
*/
    if ( GEarea2D(lpvek,nlin,apvek,narc,cpvek,spvek,ncur,dist,area,tp) < 0 )
      status = erpush("EX1552","");
    else 
      status = 0;
/*
***Deallokera minne.
*/
    if ( nlin > 0 ) v3free(linvek,"EXarea");
    if ( narc > 0 ) v3free(arcvek,"EXarea");
    if ( ncur > 0 )
      {
      v3free(curvek,"EXarea");
      for ( i=0; i<ncur; ++i ) DBfree_segments(spvek[i]);
      }

    return(status);
  }
  
/********************************************************/
/*!******************************************************/

        short EXusec(
        DBId     *idp1,
        DBId     *idp2,
        DBshort   inr,
        DBTmat   *crdptr,
        DBVector *vecptr)

/*      Används av EXsect() om man begärt UV-värde som utdata.
 *
 *      In: idp1   => Pekare till 1:a storhetens identitet.
 *          idp2   => Pekare till 2:a storhetens identitet.
 *          inr    => Skärningsnummer.
 *          crdptr => Pekare till lokalt koordinatsystem.
 *
 *      Ut: *vecptr => En DBVector med koordinater.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *          EX1732 => Storheterna är en och samma
 *          EX1972 => 2 plan
 *          EX1982 => Skärning saknas
 *          EX1992 => inr = 0
 *          EX2002 => inr < 0
 *
 *      1996-08-19 Modified EXe1:EXsect(), Gunnar Liden
 *
 *      1997-02-27 Error message typ     
 *                 Line/surface intersect Gunnar Liden
 *      1997-05-16 Elimination of compiler warnings
 *      1998-04-06 inr no longer > 1000
 *      1998-04-08 Flyttad till EXe1.c, J.Kjellander
 *      1999-12-18 sur164->varkon_sur_curint G Liden
 *
 ******************************************************!*/

  {
    DBptr    la1,la2;
    DBetype  typ1,typ2;
    short    status;
    char     idstr1[V3STRLEN+1],idstr2[V3STRLEN+1];
    char    *pdat1,*pdat2;
    DBAny    gmstr1,gmstr2;
    DBSeg   *segpk1,*segpk2;
    DBSeg    arcsg1[4],arcsg2[4];
    DBPatch *patpek;
    char     errbuf[80];  /* String for error message  */

/*
***Parameters for varkon_sur_curint
*/
   gmint  ocase,acc,sol,nsol;

   DBVector  start,r3_pt,uv_pt,
          all_uv[25],all_xyz[25];
   gmflt  all_u[25];    /* All curve parameter values */
   gmflt  uvalue; 
/*
***Initialization of internal variables 
*/
  pdat1 = NULL;
  pdat2 = NULL;
/*
***Initializations for surface/curve intersect
***No start point       
*/
   start.x_gm =  0.0;
   start.y_gm =  0.0;
   start.z_gm = -1.0;
/*
***Sortering (ocase) relativt kurvans startpunkt.
***Beräkningsfall (acc) dvs. alla patchar. Ingen startpunkt given.
***Önskad lösning = 1 
***Antal lösningar innan anropet = 0.
*/
   ocase = 1 + 1000;
   acc   = 1;
   sol   = inr;
   nsol  = 0;
/*
***Kolla att sk{rningens ordningsnummer inte == 0.
*/
    if ( inr == 0 ) return(erpush("EX1992",""));
/*
***Hämta den 1:a storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idp1, &la1, &typ1) < 0 )
      return ( erpush("EX1402",""));
/*
***Hämta den 2:a storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idp2, &la2, &typ2) < 0 )
      return ( erpush("EX1402",""));
/*
***Kolla att det är två olika storheter.
*/
    if ( la1 == la2 )
     {
     igidst(idp1,idstr1);
     return(erpush("EX1732",idstr1));
     }
/*
***Kolla att inte koordinatsystem eller B-plan förekommer
***2 gånger.
*/
    if ( (typ1 == CSYTYP  ||  typ1 == BPLTYP  ||  typ1 == SURTYP )  &&
         (typ2 == CSYTYP  ||  typ2 == BPLTYP  ||  typ2 == SURTYP ) )
      {
      igidst(idp1,idstr1);
      igidst(idp2,idstr2);
      strcat(idstr1,"%");
      strcat(idstr1,idstr2);
      return(erpush("EX1982",idstr1));
      }
/*
***Läs geometridata för storhet 1.
*/
    switch (typ1)
      {
      case LINTYP:
      DBread_line(&gmstr1.lin_un, la1);
      break;

      case ARCTYP:
      DBread_arc(&gmstr1.arc_un,arcsg1, la1);
      pdat1 = (char *)arcsg1;
      break;

      case CURTYP:
      DBread_curve(&gmstr1.cur_un,NULL,&segpk1,la1);
      pdat1 = (char *)segpk1;
      break;

      case CSYTYP:
      DBread_csys(&gmstr1.csy_un, NULL, la1);
      break;

      case BPLTYP:
      DBread_bplane(&gmstr1.bpl_un, la1);
      break;

      case SURTYP:
      DBread_surface(&gmstr1.sur_un, la1);
      DBread_patches(&gmstr1.sur_un,&patpek);
      pdat1 = (char *)patpek;
      break;

      default:
      igidst(idp1,idstr1);
      return(erpush("EX1412",idstr1));
      }
/*
***Läs geometridata för storhet 2.
*/
    switch (typ2)
      {
      case LINTYP:
      DBread_line(&gmstr2.lin_un, la2);
      break;

      case ARCTYP:
      DBread_arc(&gmstr2.arc_un,arcsg2,la2);
      pdat2 = (char *)arcsg2;
      break;

      case CURTYP:
      DBread_curve(&gmstr2.cur_un,NULL,&segpk2,la2);
      pdat2 = (char *)segpk2;
      break;

      case CSYTYP:
      DBread_csys(&gmstr2.csy_un, NULL, la2);
      break;

      case BPLTYP:
      DBread_bplane(&gmstr2.bpl_un, la2);
      break;

      case SURTYP:
      DBread_surface(&gmstr2.sur_un, la2);
      DBread_patches(&gmstr2.sur_un,&patpek);
      pdat2 = (char *)patpek;
      break;

      default:
      igidst(idp2,idstr2);
      if ( typ1 == CURTYP ) DBfree_segments(segpk1);
      if ( typ1 == SURTYP ) DBfree_patches(&gmstr1.sur_un,patpek);
      return(erpush("EX1412",idstr2));
      }
/*
***Kolla att inte negativt inr används där det ej är tillåtet.
***Tillåtet i alla lägen där en linje eller 2D-cirkel figurerar
***annars inte.
*/
    if ( inr < 0 )
      {
      if ( typ1 == LINTYP  ||  typ2 == LINTYP ) ;
      else if ( typ1 == ARCTYP  &&  gmstr1.arc_un.ns_a == 0 ) ;
      else if ( typ2 == ARCTYP  &&  gmstr2.arc_un.ns_a == 0 ) ;
      else
        {
        igidst(idp1,idstr1);
        igidst(idp2,idstr2);
        strcat(idstr1,"%");
        strcat(idstr1,idstr2);
        erpush("EX1982",idstr1);
        status = erpush("EX2002","");
        goto end;
        }
      }
/*
***Beräkna skärningen.
***Bara för yta/kurva.
*/
   if     ( typ1 == CURTYP  &&  typ2 == SURTYP )
     {
/*
***Beräkna skärningar. Skärningarna returneras i all_uv som här
***deklarerats till 25 element men som borde ha samma deklaration 
***som motsvarande variabel i surpac nämligen SMAX fn. = 10.
*/
     status = varkon_sur_curint((DBSurf*)&gmstr2,(DBPatch *)pdat2,
             (DBCurve*)&gmstr1,(DBSeg *)pdat1,&start,ocase,acc,sol,
             &nsol,&r3_pt,&uv_pt,&uvalue,all_u,all_uv,all_xyz);
     }
   else if ( typ1 == SURTYP  &&  typ2 == CURTYP )
     {
     status = varkon_sur_curint((DBSurf*)&gmstr1,(DBPatch *)pdat1,
              (DBCurve*)&gmstr2,(DBSeg *)pdat2,&start,ocase,acc,sol,
               &nsol,&r3_pt,&uv_pt,&uvalue,all_u,all_uv,all_xyz);
     }
/*
***Linje - yta. Oavsett i vilken ordning de kommer lägger
***geo723() alltid linjens parametervärden i uout1 så att
***GEposition() kan beräkna position.
*/
   else if ( typ1 == LINTYP  &&  typ2 == SURTYP )
     {
     uv_pt.z_gm = F_UNDEF;
     status = GEintersect_pv(&gmstr2,pdat2,
                     &gmstr1,NULL,NULL,sol,&uv_pt.x_gm,&uv_pt.y_gm);
       if ( status < 0 ) return(erpush("GE7112",""));
     }
   else if ( typ2 == LINTYP  &&  typ1 == SURTYP )
     {
     uv_pt.z_gm = F_UNDEF;
     status = GEintersect_pv(&gmstr1,pdat1,
                     &gmstr2,NULL,NULL,sol,&uv_pt.x_gm,&uv_pt.y_gm);
       if ( status < 0 ) return(erpush("GE7112",""));
     }
   else
     {
     erinit();
     sprintf(errbuf,"types %d %d not OK%%EXe1_gl",typ1,typ2);
     status= erpush("SU2993",errbuf);
     return(status);
     }
/*
***Felhantering.
*/
  if ( status < 0 )
    {
    igidst(idp1,idstr1);
    igidst(idp2,idstr2);
    strcat(idstr1,"%");
    strcat(idstr1,idstr2);
    status = erpush("EX1982",idstr1);
    goto end;
    }
/*
***Utdata
*/
   vecptr->x_gm  = uv_pt.x_gm - 1.0;
   vecptr->y_gm  = uv_pt.y_gm - 1.0;
   vecptr->z_gm  = uvalue     - 1.0;       
/*
***Om n}gon av storheterna {r en kurva eller yta skall minne deallokeras
***innan vi avslutar.
*/
end:
    if ( typ1 == CURTYP ) DBfree_segments(segpk1);
    if ( typ2 == CURTYP ) DBfree_segments(segpk2);
    if ( typ1 == SURTYP ) DBfree_patches(&gmstr1.sur_un,patpek);
    if ( typ2 == SURTYP ) DBfree_patches(&gmstr2.sur_un,patpek);

    return(status);
  }

/********************************************************/
