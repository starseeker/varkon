/*!*****************************************************
*
*    exget1.c
*    ========
*
*    EXgtid();      Interface routine for GETID   
*    EXgthd();      Interface routine for GETHDR  
*    EXgwdt();      Interface routine for GETWIDTH
*    EXpdat();      Interface routine for PUTDAT 
*    EXgdat();      Interface routine for GETDAT 
*    EXddat();      Interface routine for DELDAT 
*    EXgtpo();      Interface routine for GETPOI 
*    EXgtli();      Interface routine for GETLIN 
*    EXgtar();      Interface routine for GETARC 
*    EXgtcu();      Interface routine for GETCURH
*    EXgtop();      Interface routine for GETTOPP
*    EXgcub();      Interface routine for GETCUBP
*    EXgfac();      Interface routine for GETFACP
*    EXgttx();      Interface routine for GETTXT
*    EXgtxh();      Interface routine for GETXHT
*    EXgtld();      Interface routine for GETLDM
*    EXgtcd();      Interface routine for GETCDM
*    EXgtrd();      Interface routine for GETRDM
*    EXgtad();      Interface routine for GETADM 
*    EXgtgp();      Interface routine for GETGRP 
*    EXgtcs();      Interface routine for GETCSY 
*    EXgttf();      Interface routine for GETTRF
*    EXgtbp();      Interface routine for GETBPL
*    EXgtsy();      Interface routine for GETSYM 
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
*
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/EX.h"

/*!******************************************************/

        short EXgtid(
        DBId    *id,
        char    *code)

/*      Interface-rutin för GETID. Hämtar identitet
 *      ur GM.
 *
 *      In: id     => Pekare till identitet.
 *          code   =>
 *
 *      Ut:
 *
 *      FV:       0 => Ok.
 *           EX1592 => Otillåten funktionskod.
 *           EX1603 => Fel från DBget_pointer().
 *
 *      (C)microform ab 29/3/86 J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    short  status;

/*
***Hämta la, om "F"irst, ett extra anrop till DBget_pointer().
*/
    status = DBget_pointer(code[0],id,&la,&typ);

    if ( code[0] == 'F' ) status = DBget_pointer('N',id,&la,&typ);
/*
***Översätt la till ID.
*/
    if ( status == 0 )
      {
      DBget_id(la,id);
      return(0);
      }
    else if ( status == -4 )
      {
      return(erpush("EX1592",""));
      }
    else if ( status == -5 )
      {
      id[0].seq_val = 0;
      id[0].ord_val = 1;
      id[0].p_nextre = NULL;
      return(0);
      }
    else
      {
      return(erpush("EX1603",""));
      }
  }

/********************************************************/
/*!******************************************************/

        short EXgthd(
        DBId    *id,
        DBHeader  *header)

/*      Interface-rutin för GETHDR. Hämtar header-data
 *      ur GM.
 *
 *      In: id       => Pekare till header identitet.
 *          header   => Pekare till DBHeader.
 *
 *      Ut: *header  => Storhetens header.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *
 *      (C)microform ab 20/2/86 B. Doverud
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
/*
***Läs record-header returnera
*/
    DBread_header(header,la);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgwdt(
        DBId    *id,
        DBfloat *width)

/*      Interface-rutin för GETWIDTH.
 *
 *      In: id      => Pekare till partens ID
 *          width   => Pekare till resultat
 *
 *      Ut: *width  = Aktuell linjebredd
 *
 *      Felkoder: EX1402 => Hittar ej storheten
 *                EX1412 => Otillåten typ för denna operation
 *
 *      (C)microform ab 1998-01-01, J. Kjellander
 *
 *      1998-03-04 Punkt, J.Kjellander
 *      2004-07-15 Mesh+B_plane, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    DBptr    la;
    DBetype  typ;
    DBPoint  poi;
    DBLine   lin;
    DBArc    arc;
    DBCurve  cur;
    DBText   txt;
    DBMesh   mesh;
    DBBplane bpl;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 ) return(erpush("EX1402",""));
/*
***Vilken typ av storhet är det ?
*/
    switch ( typ )
      {
      case POITYP:
      DBread_point(&poi,la);
     *width = poi.wdt_p;
      break;

      case LINTYP:
      DBread_line(&lin,la);
     *width = lin.wdt_l;
      break;

      case ARCTYP:
      DBread_arc(&arc,NULL,la);
     *width = arc.wdt_a;
      break;

      case CURTYP:
      DBread_curve(&cur,NULL,NULL,la);
     *width = cur.wdt_cu;
      break;

      case TXTTYP:
      DBread_text(&txt,NULL,la);
     *width = txt.wdt_tx;
      break;

      case MSHTYP:
      DBread_mesh(&mesh,la,MESH_HEADER);
     *width = mesh.wdt_m;
      break;

      case BPLTYP:
      DBread_bplane(&bpl,la);
     *width = bpl.wdt_bp;
      break;

      default: return(erpush("EX1412","GETWIDTH"));
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXpdat(
        char *key,
        DBint typ,
        DBint size,
        DBint count,
        char *datpek)

/*      Interface-rutin för PUTDAT.
 *
 *      In: key    = Nyckelsträng
 *          typ    = Datatyp, C_INT_VA osv...
 *          size   = Storlek i bytes/element
 *          count  = Antal värden
 *          datpek = Pekare till data
 *
 *      FV:  0     => Ok.
 *          -1     => Nyckeln finns redan.
 *
 *      (C)microform ab 1999-01-20 J.Kjellander
 *
 ******************************************************!*/

  {
/*
***Lagra i GM.
*/
   return(DBinsert_named_data(key,typ,size,count,datpek));
  }

/********************************************************/
/*!******************************************************/

        short EXgdat(
        char  *key,
        DBint *typ,
        DBint *size,
        DBint *count,
        char **datpek)

/*      Interface-rutin för GETDAT.
 *
 *      In: key    = Nyckelsträng
 *          typ    = Pekare till datatyp
 *          size   = Pekare till storlek i bytes/element
 *          count  = Pekare till antal värden
 *          datpek = Pekare till pekare till data
 *
 *      FV:  0     => Ok.
 *          -1     => Nyckeln finns ej.
 *
 *      (C)microform ab 1999-01-20 J.Kjellander
 *
 ******************************************************!*/

  {
/*
***Hämta data från GM.
*/
   return(DBread_named_data(key,typ,size,count,datpek));
  }

/********************************************************/
/*!******************************************************/

        short EXddat(char *key)

/*      Interface-rutin för DELDAT.
 *
 *      In: key    = Nyckelsträng
 *
 *      FV:  0     => Ok.
 *          -1     => Nyckeln finns ej.
 *
 *      (C)microform ab 1999-01-20 J.Kjellander
 *
 ******************************************************!*/

  {

/*
***Ta bort ur GM.
*/
   return(DBdelete_named_data(key));
  }

/********************************************************/
/*!******************************************************/

        short EXgtpo(
        DBId    *id,
        DBPoint   *punkt)

/*      Interface-rutin för GETPOI. Hämtar punkt-data
 *      ur GM.
 *
 *      In: id     => Pekare till punktens identitet.
 *          punkt  => Pekare till DBPoint.
 *
 *      Ut: *punkt => Punkt-post.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ för denna operation
 *
 *      (C)microform ab 2/2/86 J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
    if ( typ != POITYP ) return(erpush("EX1412","GETPOI"));
/*
***Läs punkt-posten och returnera punkt-data.
*/
    DBread_point(punkt,la);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgtli(
        DBId    *id,
        DBLine   *linje)

/*      Interface-rutin för GETLIN. Hämtar linje-data
 *      ur GM.
 *
 *      In: id     => Pekare till linjens identitet.
 *          linje  => Pekare till DBLine.
 *
 *      Ut: *linje => Linje-post.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ för denna operation
 *
 *      (C)microform ab 20/2/86 B. Doverud
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
    if ( typ != LINTYP ) return(erpush("EX1412","GETLIN"));
/*
***Läs linje-posten och returnera linje-data.
*/
    DBread_line(linje,la);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgtar(
        DBId    *id,
        DBArc   *cirkel,
        DBSeg   segment[])

/*      Interface-rutin för GETARC. Hämtar cirkel-data
 *      ur GM.
 *
 *      In: id       => Pekare till cirkelns identitet.
 *          cirkel   => Pekare till DBArc.
 *          segment  => 
 *
 *      Ut: *cirkel  => Cirkel-post.
 *
 *      FV:  0      => Ok.
 *          EX1402  => Hittar ej storheten
 *          EX1412  => Otillåten typ för denna operation
 *
 *      (C)microform ab 20/2/86 B. Doverud
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
    if ( typ != ARCTYP ) return(erpush("EX1412","GETARC"));
/*
***Läs cirkel-posten och returnera cirkel-data.
*/
    DBread_arc(cirkel,segment,la);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgtcu(
        DBId    *id,
        DBCurve   *kurva,
        DBSeg  **graseg,
        DBSeg  **geoseg)

/*      Interface-rutin för GETCURH, GETSEG mm. Hämtar hel kurva
 *      ur GM. Om geoseg eller graseg = NULL hämtas inte mot-
 *      svarande segment.
 *
 *      In: id      => Pekare till kurvans identitet.
 *          kurva   => Pekare till DBCurve.
 *          geoseg  => Pekare till geosegment-pekare eller NULL.
 *          graseg  => Pekare till grasegment-pekare eller NULL.
 *
 *      Ut: *kurva  => Kurv-post.
 *          *geoseg => Pekare till geosegment.
 *          *graseg => Pekare till grasegment.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ för denna operation
 *
 *      (C)microform ab 20/2/86 B. Doverud
 *
 *       7/6/93   Dynamisk allokering av minne, J. Kjellander
 *       10/1/94  Nytt anrop, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
/*
***Läs kurv-posten och returnera kurv-data.
*/
    switch ( typ )
      {
      case CURTYP:
      DBread_curve(kurva,graseg,geoseg,la);
      break;

      default:
      return(erpush("EX1412","GETCURH"));
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgtop(
        DBId    *id,
        DBshort  iu,
        DBshort  iv,
        DBPatch   *toppat)

/*      Interface-rutin för GETTOPP. Hämtar patch-data
 *      ur GM.
 *
 *      In: id     => Pekare till ytans identitet.
 *          iu,iv  => Patchadress.
 *          toppat => Pekare till DBPatch.
 *
 *      Ut: *toppat => DBPatch-post.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ för denna operation
 *          EX4122 => Patch iu,iv finns ej
 *
 *      (C)microform ab 2/3/95 J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    char   errbuf[81];
    DBSurf  yta;
    DBPatchX geopat;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
    if ( typ != SURTYP ) return(erpush("EX1412","GETTOPP"));
/*
***Läs yt-posten.
*/
    DBread_surface(&yta,la);
/*
***Kolla att patchen (iu,iv) finns.
*/
    if ( iu < 1  ||  iu > yta.nu_su   ||
         iv < 1  ||  iv > yta.nv_su )
      {
      sprintf(errbuf,"(%d,%d)",(int)iu,(int)iv);
      return(erpush("EX4122",errbuf));
      }
/*
***Läs topologisk patch med vidhängande geometrisk.
*/
    toppat->spek_c = (char *)&geopat;
    DBread_one_patch(&yta,toppat,iu,iv);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgcub(
        DBId     *id,
        DBshort   iu,
        DBshort   iv,
        DBPatchC *cubpat)

/*      Interface-rutin för GETCUBP. Hämtar patch-data
 *      för bikubisk patch ur GM.
 *
 *      In: id     => Pekare till ytans identitet.
 *          iu,iv  => Patchadress.
 *          cubpat => Pekare till DBPatch_C.
 *
 *      Ut: *cubpat => DBPatch_C-post.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ för denna operation
 *          EX4122 => Patch finns ej
 *          EX4132 => Patch ej bikubisk
 *
 *      (C)microform ab 2/3/95 J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr    la;
    DBetype  typ;
    char     errbuf[81];
    DBSurf   yta;
    DBPatch  toppat;
    DBPatchX geopat;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
    if ( typ != SURTYP ) return(erpush("EX1412","GETCUBP"));
/*
***Läs yt-posten.
*/
    DBread_surface(&yta,la);
/*
***Kolla att patchen (iu,iv) finns.
*/
    if ( iu < 1  ||  iu > yta.nu_su   ||
         iv < 1  ||  iv > yta.nv_su )
      {
      sprintf(errbuf,"(%d,%d)",(int)iu,(int)iv);
      return(erpush("EX4122",errbuf));
      }
/*
***Läs topologisk patch med vidhängande geometrisk.
*/
    toppat.spek_c = (char *)&geopat;
    DBread_one_patch(&yta,&toppat,iu,iv);
/*
***Kolla att sekundär patch = CUB_PAT.
*/
   if ( toppat.styp_pat != CUB_PAT )
     {
     sprintf(errbuf,"(%d,%d)",(int)iu,(int)iv);
     return(erpush("EX4132",errbuf));
     }
/*
***Returnera bikubisk patch. Om vi inte gör en kopiering här
***utan litar på att patchen är bikubisk och använder den pekare
***som skickas som indata kommer systemet att krascha om användaren
***gör fel. Våran geopat är ju DBPatchX.
*/
    V3MOME((char *)&geopat,(char *)cubpat,sizeof(DBPatchC));

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgfac(
        DBId    *id,
        DBshort  iu,
        DBshort  iv,
        DBPatchF  *facpat)

/*      Interface-rutin för GETFACP. Hämtar patch-data
 *      för facettpatch ur GM.
 *
 *      In: id     => Pekare till ytans identitet.
 *          iu,iv  => Patchadress.
 *          facpat => Pekare till DBPatchF.
 *
 *      Ut: *facpat => DBPatchF-post.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ för denna operation
 *          EX4122 => Patch finns ej
 *          EX4182 => Ej facettpatch
 *
 *      (C)microform ab 1997-06-06 J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    char   errbuf[81];
    DBSurf  yta;
    DBPatch  toppat;
    DBPatchX geopat;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
    if ( typ != SURTYP ) return(erpush("EX1412","GETFACP"));
/*
***Läs yt-posten.
*/
    DBread_surface(&yta,la);
/*
***Kolla att patchen (iu,iv) finns.
*/
    if ( iu < 1  ||  iu > yta.nu_su   ||
         iv < 1  ||  iv > yta.nv_su )
      {
      sprintf(errbuf,"(%d,%d)",(int)iu,(int)iv);
      return(erpush("EX4122",errbuf));
      }
/*
***Läs topologisk patch med vidhängande geometrisk.
*/
    toppat.spek_c = (char *)&geopat;
    DBread_one_patch(&yta,&toppat,iu,iv);
/*
***Kolla att sekundär patch = FAC_PAT.
*/
   if ( toppat.styp_pat != FAC_PAT )
     {
     sprintf(errbuf,"(%d,%d)",(int)iu,(int)iv);
     return(erpush("EX4182",errbuf));
     }
/*
***Returnera facettpatch.
*/
    V3MOME((char *)&geopat,(char *)facpat,sizeof(DBPatchF));

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgttx(
        DBId    *id,
        DBText   *text,
        char     str[])

/*      Interface-rutin för GETTXT. Hämtar text-data
 *      ur GM.
 *
 *      In: id     => Pekare till textens identitet.
 *          text   => Pekare till DBText.
 *
 *      Ut: *text  => Text-post.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ för denna operation
 *
 *      (C)microform ab 20/2/86 B. Doverud
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
    if ( typ != TXTTYP ) return(erpush("EX1412","GETTXT"));
/*
***Läs text-posten och returnera text-data.
*/
    DBread_text(text,str,la);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgtxh(
        DBId    *id,
        DBHatch   *snitt,
        DBfloat  lindat[])

/*      Interface-rutin för GETXHT. Hämtar snitt-data
 *      ur GM.
 *
 *      In: id      => Pekare till snittets identitet.
 *          snitt   => Pekare till DBHatch.
 *
 *      Ut: *snitt  => Snitt-post.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ för denna operation
 *
 *      (C)microform ab 20/2/86 B. Doverud
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
    if ( typ != XHTTYP ) return(erpush("EX1412","GETXHT"));
/*
***Läs snitt-posten och returnera snitt-data.
*/
    DBread_xhatch(snitt,lindat,la);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgtld(
        DBId    *id,
        DBLdim   *ldim)

/*      Interface-rutin för GETLDM. Hämtar längdmåttets-data
 *      ur GM.
 *
 *      In: id     => Pekare till längdmåttets identitet.
 *          ldim   => Pekare till DBLdim.
 *
 *      Ut: *ldim  => Längdmått-post.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ för denna operation
 *
 *      (C)microform ab 20/2/86 B. Doverud
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
    if ( typ != LDMTYP ) return(erpush("EX1412","GETLDM"));
/*
***Läs ldim-posten och returnera ldim-data.
*/
    DBread_ldim(ldim,la);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgtcd(
        DBId    *id,
        DBCdim   *cdim)

/*      Interface-rutin för GETCDM. Hämtar diametermått-data
 *      ur GM.
 *
 *      In: id     => Pekare till diametermåttets identitet.
 *          cdim   => Pekare till DBCdim.
 *
 *      Ut: *cdim  => Diametermått-post.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ för denna operation
 *
 *      (C)microform ab 20/2/86 B. Doverud
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
    if ( typ != CDMTYP ) return(erpush("EX1412","GETCDM"));
/*
***Läs cdim-posten och returnera cdim-data.
*/
    DBread_cdim(cdim,la);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgtrd(
        DBId    *id,
        DBRdim   *rdim)

/*      Interface-rutin för GETRDM. Hämtar radiemått-data
 *      ur GM.
 *
 *      In: id     => Pekare till radiemåttets identitet.
 *          rdim   => Pekare till DBRdim.
 *
 *      Ut: *rdim  => Radiemått-post.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ för denna operation
 *
 *      (C)microform ab 20/2/86 B. Doverud
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
    if ( typ != RDMTYP ) return(erpush("EX1412","GETRDM"));
/*
***Läs rdim-posten och returnera rdim-data.
*/
    DBread_rdim(rdim,la);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgtad(
        DBId    *id,
        DBAdim   *adim)

/*      Interface-rutin för GETADM. Hämtar vinkelmått-data
 *      ur GM.
 *
 *      In: id     => Pekare till vinkelmåttets identitet.
 *          adim   => Pekare till DBAdim.
 *
 *      Ut: *adim  => Vinkelmått-post.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ för denna operation
 *
 *      (C)microform ab 20/2/86 B. Doverud
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
    if ( typ != ADMTYP ) return(erpush("EX1412","GETADM"));
/*
***Läs adim-posten och returnera adim-data.
*/
    DBread_adim(adim,la);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgtgp(
        DBId    *id,
        DBGroup   *grupp,
        DBptr    lavek[])

/*      Interface-rutin för GETGRP. Hämtar grupp-data
 *      ur GM.
 *
 *      In: id     => Pekare till gruppns identitet.
 *          grupp  => Pekare till DBGroup.
 *
 *      Ut: *grupp => Grupp-post.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ för denna operation
 *
 *      (C)microform ab 20/2/86 B. Doverud
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
    if ( typ != GRPTYP ) return(erpush("EX1412","GETGRP"));
/*
***Läs grupp-posten och returnera grupp-data.
*/
    DBread_group(grupp,lavek,la);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgtcs(
        DBId    *id,
        DBCsys   *csys)

/*      Interface-rutin för GETCSY. Hämtar koordinatsystem-data
 *      ur GM.
 *
 *      In: id     => Pekare till koordinatsystemets identitet.
 *          csys   => Pekare till DBCsys.
 *
 *      Ut: *csys  => Koordinatsystem-post.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ för denna operation
 *
 *      (C)microform ab 20/2/86 B. Doverud
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
    if ( typ != CSYTYP ) return(erpush("EX1412","GETCSY"));
/*
***Läs csys-posten och returnera csys-data.
*/
    DBread_csys(csys,NULL,la);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgttf(
        DBId    *id,
        DBTform   *ptrf)

/*      Interface-rutin för GETTRF. Hämtar transformation
 *      ur GM.
 *
 *      In: id     => Pekare till ktransformationens identitet.
 *          ptrf   => Pekare till DBTform.
 *
 *      Ut: *ptrf  => Transformations-post.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *
 *      (C)microform ab 15/12/92 J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
    if ( typ != TRFTYP ) return(erpush("EX1412","GETTRF"));
/*
***Läs trf-posten och returnera data.
*/
    DBread_tform(ptrf,la);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgtbp(
        DBId    *id,
        DBBplane   *bplan)

/*      Interface-rutin för GETBPL. Hämtar Bplan-data
 *      ur GM.
 *
 *      In: id     => Pekare till planets identitet.
 *          bplan  => Pekare till DBBplane.
 *
 *      Ut: *bplan => Bplan-post.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ för denna operation
 *
 *      (C)microform ab 27/8/87 J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
       return(erpush("EX1402",""));
    if ( typ != BPLTYP ) return(erpush("EX1412","GETBPL"));
/*
***Läs Bplan-posten och returnera plan-data.
*/
    DBread_bplane(bplan,la);

    return(0);
  }

/********************************************************/
