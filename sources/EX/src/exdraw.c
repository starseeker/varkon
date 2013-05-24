/*!*****************************************************
*
*     exdraw.c
*     ========
*
*     EXtrim();     Interface routine for TRIM
*     EXblk();      Interface routine for BLANK
*     EXblla();     Blank entity by DBptr
*     EXublk();     Interface routine for UNBLANK
*     EXdraw();     Draw/erase entity
*     EXdren();     Draw/erase entity by DBptr
*     EXdral();     Draw everything
*
*    This file is part of the VARKON Execute Library.
*    URL:  http://varkon.sourceforge.net
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
#include "../../DB/include/DBintern.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../../WP/include/WP.h"

#include "../include/EX.h"

extern bool    intrup;
extern DBTmat lklsyi,*lsyspk;

/*!******************************************************/

        short EXtrim(
        DBId    *idp1,
        DBId    *idp2,
        DBshort  end,
        DBshort  inr)

/*      Interface routine for MBS procedure TRIM().
 *
 *      In: idp1   => Ptr to ID of first entity (line/arc).
 *          idp2   => Ptr to ID of second entity (line/arc/curve).
 *          end    => 0 = Start, 1 = End.
 *          inr    => Intersect number.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Entity does not exist
 *          EX1412 => Illegal entity type
 *          EX1942 => No intersect found
 *          EX1932 => Result to short
 *
 *      (C)microform ab 3/1/86 J. Kjellander
 *
 *      13/4/86    EXdren(), J. Kjellander
 *      7/12/89    Extend, längdkoll, J. Kjellander
 *      5/1/92     Status från GEintersect_pv(), J. Kjellander
 *      9/6/93     Dynamiska segment, J. Kjellander
 *      2008-05-13 3D arc, J.Kjellander
 *      2008-12-14 Bugfix arclengths, J.Kjellander
 *
 ******************************************************!*/

  {
    DBfloat  u1,u2,v,l,dummy;
    DBptr    la1,la2;
    DBetype  typ1,typ2;
    short    status;
    DBSeg    arcseg1[4],arcseg2[4];
    DBSeg   *segpt1,*segpt2;
    DBAny    entity1,entity2;
    DBVector pos,p12;
    DBArc    newarc;
    DBSeg    newseg[4];

/*
***Get DBptr and type of first entity.
*/
    if ( DBget_pointer( 'I', idp1, &la1, &typ1) < 0 )
      return ( erpush("EX1402",""));
/*
***Get entity data from DB.
*/
    switch (typ1)
      {
      case LINTYP:
      segpt1 = NULL;
      DBread_line((DBLine *)&entity1, la1);
      break;

      case ARCTYP:
      segpt1 = arcseg1;
      DBread_arc((DBArc *)&entity1,segpt1,la1);
      break;

      default:
      return(erpush("EX1412",""));
      }
/*
***Get DBptr and type of second entity.
*/
    if ( DBget_pointer( 'I', idp2, &la2, &typ2) < 0 )
      return ( erpush("EX1402",""));
/*
***Get entity data from DB.
*/
    switch (typ2)
      {
      case LINTYP:
      segpt2 = NULL;
      DBread_line((DBLine *)&entity2,la2);
      break;

      case ARCTYP:
      segpt2 = arcseg2;
      DBread_arc((DBArc *)&entity2,segpt2,la2);
      break;

      case CURTYP:
      DBread_curve((DBCurve *)&entity2,NULL,&segpt2,la2);
      break;

      default:
      return(erpush("EX1412",""));
      }
/*
***Calculate the parameter value of the intersect position.
*/
    status=GEintersect_pv(&entity1,(char *)segpt1,&entity2,(char *)segpt2,
                                               lsyspk,inr,&u1,&u2);

    if ( typ2 == CURTYP ) DBfree_segments(segpt2);
    if ( status < 0 ) return(erpush("EX1942",""));
/*
***Uppdate entity1.
*/
    switch (typ1)
      {
/*
***Line.
*/
      case LINTYP:
      GEposition(&entity1,(char *)segpt1,u1-1.0,(DBfloat)0.0,&pos);

      if ( end == 0 )
        {
        entity1.lin_un.crd1_l.x_gm = pos.x_gm;
        entity1.lin_un.crd1_l.y_gm = pos.y_gm;
        }
      else
        {
        entity1.lin_un.crd2_l.x_gm = pos.x_gm;
        entity1.lin_un.crd2_l.y_gm = pos.y_gm;
        }
/*
***Check length.
*/
      p12.x_gm = entity1.lin_un.crd1_l.x_gm - entity1.lin_un.crd2_l.x_gm;
      p12.y_gm = entity1.lin_un.crd1_l.y_gm - entity1.lin_un.crd2_l.y_gm;
      l = GEvector_length2D(&p12);

      if ( l < 0.001 ) return(erpush("EX1932",""));
/*
***Update DB and display.
*/
      else
        {
        EXdren(la1,typ1,FALSE,GWIN_ALL);
        DBupdate_line((DBLine *)&entity1,la1);
        EXdren(la1,typ1,TRUE,GWIN_ALL);
        }
      break;
/*
***Arc.
*/
      case ARCTYP:
      v = entity1.arc_un.v1_a + (u1-1.0)*(entity1.arc_un.v2_a-
                                         entity1.arc_un.v1_a);

      if ( end == 0 ) entity1.arc_un.v1_a = v;
      else            entity1.arc_un.v2_a = v;
/*
***Check length.
*/
      if ( GE312(&entity1.arc_un.v1_a,&entity1.arc_un.v2_a) < 0 )
        return(erpush("EX1932",""));
/*
***Create the trimmed arc.
*/
      else
        {
        pos.x_gm = entity1.arc_un.x_a;
        pos.y_gm = entity1.arc_un.y_a;
        pos.z_gm = 0.0;

        if ( lsyspk != NULL )
          {
          GEtfang_to_basic(entity1.arc_un.v1_a,&lklsyi,&entity1.arc_un.v1_a);
          GEtfang_to_basic(entity1.arc_un.v2_a,&lklsyi,&entity1.arc_un.v2_a);
          }

        status = GE300(&pos,
                       entity1.arc_un.r_a,
                       entity1.arc_un.v1_a,
                       entity1.arc_un.v2_a,
                       lsyspk,
                      &newarc,
                       newseg,
                       3);
       if ( status < 0 )  return(status);
/*
***Update number of segs after trimming.
*/
       entity1.arc_un.ns_a = newarc.ns_a;
/*
***Update lengths after trimming. Bugfix 2008-12-14, JK
*/
       status = GEarclength((DBAny *)&entity1,newseg,&dummy);
       if ( status < 0 )  return(status);
/*
***Update DB and display.
*/
        EXdren(la1,typ1,FALSE,GWIN_ALL);
        DBupdate_arc((DBArc *)&entity1,newseg,la1);
        EXdren(la1,typ1,TRUE,GWIN_ALL);
        }
      break;
      }
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXblk(DBId *idvek)

/*      Interface-rutin för proceduren BLANK.
 *
 *      In: idvek   => Pekare till storhets identitet.
 *
 *      Ut: Inget.
 *
 *      FV: EX1402 = Storheten finns ej.
 *
 *      (C)microform ab 10/4/86 J. Kjellander
 *
 *      23/12/86 gmincr(), J. Kjellander
 *      20/2/92  part, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;

/*
***Ta reda på DB-adress.
*/
    if ( DBget_pointer('I',idvek,&la,&typ) < 0 )
                        return(erpush("EX1402",""));
/*
***Blanka.
*/
    EXblla(la,typ);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXblla(
        DBptr   la,
        DBetype typ)

/*      Blanka storheten vid la.
 *
 *      In: la   => Storhetens LA.
 *          typ  => Typ av storhet.
 *
 *      Ut: Inget.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 20/2/92 J. Kjellander
 *
 ******************************************************!*/

  {
    DBHeader  hed;
    DBPart   part;
    DBptr   la_tmp;
    short   i;

    switch (typ)
      {
/*
***Part, samma metod som i EXdlla().
*/
      case PRTTYP:
      DBread_part(&part,la);

      for ( i=0; i < part.its_pt; ++i )
        {
        la_tmp = gmrdid(part.itp_pt,i);
        if ( la_tmp >= 0 )
          {
          do
            {
            DBread_header(&hed,la_tmp);
            EXblla(la_tmp,hed.type);
            la_tmp = hed.n_ptr;
            }
          while (la_tmp != DBNULL);
          }
        }
       break;
/*
***Enskild storhet.
*/
       default:
       DBread_header(&hed,la);
       if ( !hed.blank )
         {
         EXdren(la,typ,FALSE,GWIN_ALL);
         hed.blank = TRUE;
         DBupdate_header(&hed,la);
         }
       break;
       }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXublk(DBId *idvek)

/*      Interface-rutin för proceduren UNBLANK.
 *
 *      In: idvek   => Pekare till storhets identitet.
 *
 *      Ut: Inget.
 *
 *      FV: EX1402 = Storheten finns ej.
 *
 *      (C)microform ab 10/4/86 J. Kjellander
 *
 *      23/12/86 gmincr(), J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr    la;
    DBetype  typ;
    DBHeader hed;

/*
***Ta reda på DB-adress.
*/
    if ( DBget_pointer('I',idvek,&la,&typ) < 0 )
                        return(erpush("EX1402",""));
/*
***Hämta header och uppdatera.
*/
    DBread_header(&hed,la);

    if ( hed.blank )
      {
      hed.blank = FALSE;
      DBupdate_header(&hed,la);
      EXdren(la,typ,TRUE,GWIN_ALL);
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXdraw(
        DBId *idvek,
        bool  draw)

/*      Ritar/suddar en storhet.
 *
 *      In: idvek   => Pekare till storhets identitet.
 *
 *      Ut: Inget.
 *
 *      FV: EX1402 = Storheten finns ej.
 *
 *      (C)microform ab 15/3/92 J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;

/*
***Ta reda på DB-adress.
*/
    if ( DBget_pointer('I',idvek,&la,&typ) < 0 )
                        return(erpush("EX1402",""));
/*
***Rita/sudda.
*/
    EXdren(la,typ,draw,GWIN_ALL);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short   EXdren(
        DBptr   la,
        DBetype typ,
        bool    draw,
        DBint   win_id)

/*      Ritar/suddar en storhet. Innan suddning kollas om
 *      storheten finns i DF. Om inte görs inget. Före
 *      ritning sker ingen koll.
 *
 *      In: la     => Storhetens DB-adress.
 *          typ    => Typ av storhet.
 *          draw   => Rita, TRUE/FALSE
 *          win_id => WP-ID för grafiskt fönster eller 
 *                    WP_ALL.
 *
 *      Return:  0 => Ok.
 *
 *      (C)microform ab 10/4/86 J. Kjellander
 *
 *      15/10/86 SAVE, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      16/3/92  Part, J.Kjellander
 *      26/11/92 Ytor, J. Kjellander
 *      15/4/93  Nya kurvor, J. Kjellander
 *      7/6/93   Dynamisk allokering segment, J. Kjellander
 *      27/12/94 Multifönster, J. Kjellander
 *      1996-12-17 WIN32, J.Kjellander
 *      2004-07-09 Mesh, J.Kjellander
 *      2006-12-09 Removed gpxxxx() calls, J.Kjellander
 *      2007-01-08 piso, pborder,  Sören L
 *
 ******************************************************!*/

  {
    DBptr     la_tmp;
    short     i;
    char      str[V3STRLEN+1];
    DBfloat   crdvek[4*GMXMXL];
    DBAny     gmpost;
    DBSeg     arcseg[4];
    DBSeg    *graseg;
    DBCsys    csy;
    DBHeader  hed;
    DBPart    part;
    DBSegarr *pborder;
    DBSegarr *piso; 

/*
***Read entity data from DB and draw or delete.
*/
    switch(typ)
      {
/*
***Point.
*/
      case POITYP:
      DBread_point((DBPoint*)&gmpost.poi_un,la);
      if ( draw ) WPdrpo(&gmpost.poi_un,la,win_id);
      else        WPdlpo(&gmpost.poi_un,la,win_id);
      break;
/*
***Line.
*/
      case LINTYP:
      DBread_line((DBLine *)&gmpost.lin_un,la);
      if ( draw ) WPdrli(&gmpost.lin_un,la,win_id);
      else        WPdlli(&gmpost.lin_un,la,win_id);
      break;
/*
***Arc.
*/
      case ARCTYP:
      DBread_arc((DBArc *)&gmpost.arc_un,arcseg,la);
      if ( draw ) WPdrar(&gmpost.arc_un,arcseg,la,win_id);
      else        WPdlar(&gmpost.arc_un,arcseg,la,win_id);
      break;
/*
***Curve.
*/
      case CURTYP:
      DBread_curve((DBCurve *)&gmpost.cur_un,&graseg,NULL,la);
      if ( draw ) WPdrcu(&gmpost.cur_un,graseg,la,win_id);
      else        WPdlcu(&gmpost.cur_un,graseg,la,win_id);
      DBfree_segments(graseg);
      break;
/*
***Surface. If the surface is blanked, it has no graphical
***representation that can be read from DB or free'd.
*/
      case SURTYP:
      DBread_surface((DBSurf *)&gmpost.sur_un,la);
      if ( !gmpost.hed_un.blank )
        {
        DBread_sur_grwire((DBSurf *)&gmpost.sur_un,&pborder,&piso);
        if ( draw ) WPdrsu(&gmpost.sur_un,pborder,piso,la,win_id);
        else        WPdlsu(&gmpost.sur_un,pborder,piso,la,win_id);
        DBfree_sur_grwire((DBSurf *)&gmpost,pborder,piso);
        }
      break;
/*
***Text.
*/
      case TXTTYP:
      DBread_text((DBText*)&gmpost.txt_un,str,la);
      if ( draw ) WPdrtx(&gmpost.txt_un,str,la,win_id);
      else        WPdltx(&gmpost.txt_un,str,la,win_id);
      break;
/*
***Crosshatch.
*/
      case XHTTYP:
      DBread_xhatch((DBHatch *)&gmpost.xht_un,crdvek,&csy,la);
      if ( draw ) WPdrxh(&gmpost.xht_un,crdvek,&csy,la,win_id);
      else        WPdlxh(&gmpost.xht_un,crdvek,&csy,la,win_id);
      break;
/*
***Längdmått.
*/
      case LDMTYP:
      DBread_ldim((DBLdim *)&gmpost,&csy,la);
      if ( draw ) WPdrdm(&gmpost,&csy,la,win_id);
      else        WPdldm(&gmpost,&csy,la,win_id);
      break;
/*
***Diametermått.
*/
      case CDMTYP:
      DBread_cdim((DBCdim *)&gmpost,&csy,la);
      if ( draw ) WPdrdm(&gmpost,&csy,la,win_id);
      else        WPdldm(&gmpost,&csy,la,win_id);
      break;
/*
***Radiemått.
*/
      case RDMTYP:
      DBread_rdim((DBRdim *)&gmpost,&csy,la);
      if ( draw ) WPdrdm(&gmpost,&csy,la,win_id);
      else        WPdldm(&gmpost,&csy,la,win_id);
      break;
/*
***Vinkelmått.
*/
      case ADMTYP:
      DBread_adim((DBAdim *)&gmpost,&csy,la);
      if ( draw ) WPdrdm(&gmpost,&csy,la,win_id);
      else        WPdldm(&gmpost,&csy,la,win_id);
      break;
/*
***Koordinatsystem.
*/
      case CSYTYP:
      DBread_csys((DBCsys *)&gmpost.csy_un,NULL,la);
      if ( draw ) WPdrcs(&gmpost.csy_un,la,win_id);
      else        WPdlcs(&gmpost.csy_un,la,win_id);
      break;
/*
***B-plan.
*/
      case BPLTYP:
      DBread_bplane((DBBplane*)&gmpost.bpl_un,la);
      if ( draw ) WPdrbp(&gmpost.bpl_un,la,win_id);
      else        WPdlbp(&gmpost.bpl_un,la,win_id);
      break;
/*
***Mesh.
*/
      case MSHTYP:
      DBread_mesh((DBMesh*)&gmpost,la,MESH_ALL);
      if ( draw ) WPdrms(&gmpost.msh_un,la,win_id);
      else        WPdlms(&gmpost.msh_un,la,win_id);
      break;
/*
***Part, samma metod som i EXdlla().
*/
      case PRTTYP:
      DBread_part(&part,la);

      for ( i=0; i < part.its_pt; ++i )
        {
        la_tmp = gmrdid(part.itp_pt,i);
        if ( la_tmp >= 0 )
          {
          do
            {
            DBread_header(&hed,la_tmp);
            EXdren(la_tmp,hed.type,draw,win_id);
            la_tmp = hed.n_ptr;
            }
          while (la_tmp != DBNULL);
          }
        }
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXdral(DBint win_id)

/*      Ritar allt som finns i DB. (Gamla gpdrgm()).
 *
 *      In: win_id = WP-id för fönster att rita i 
 *                   eller GWIN_ALL.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 28/9/86 J. Kjellander
 *
 *       7/1/93   TRFTYP, J. Kjellander
 *       27/12/94 multifönster, J. Kjellander
 *
 ******************************************************!*/

 {
    DBId    dummy;
    DBptr   la;
    DBetype typ;

/*
***Hämta LA och typ för huvud-parten.
*/
    DBget_pointer('F',&dummy,&la,&typ);
/*
***Loopa runt och hämta LA och typ för resten av DB.
*/
    while ( DBget_pointer('N',&dummy,&la,&typ) == 0 )
      {
      switch(typ)
        {
        case PRTTYP:
        case GRPTYP:
        case TRFTYP:
        break;

        default:
        EXdren(la,typ,TRUE,win_id);
        break;
        }
      }
    return(0);
}

/********************************************************/
