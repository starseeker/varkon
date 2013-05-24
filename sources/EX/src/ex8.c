/*!*****************************************************
*
*     ex8.c
*     =====
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
#include "../../DB/include/DBintern.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../../GP/include/GP.h"
#include "../../WP/include/WP.h"

#include "../include/EX.h"

extern short   gptrty;
extern bool    intrup;
extern DBTmat *lsyspk;

/*!******************************************************/

        short EXtrim(
        DBId    *idp1,
        DBId    *idp2,
        DBshort  end,
        DBshort  inr)

/*      Interface-rutin för proceduren TRIM.
 *
 *      In: idp1   => Pekare till 1:a storhetens identitet.
 *          idp2   => Pekare till 2:a storhetens identitet.
 *          sida   => 0 = Start, 1 = slut.
 *          inr    => Skärningsnummer.
 *
 *      Ut: Inget.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *          EX1412 => Otillåten typ 
 *          EX1942 => Ingen skärning
 *          EX1932 => Oändligt kort
 *
 *      (C)microform ab 3/1/86 J. Kjellander
 *
 *      13/4/86  EXdren(), J. Kjellander
 *      7/12/89  Extend, längdkoll, J. Kjellander
 *      5/1/92   Status från GEintersect_pv(), J. Kjellander
 *      9/6/93   Dynamiska segment, J. Kjellander
 *
 ******************************************************!*/

  {
    DBfloat  u1,u2,v,l;
    DBptr  la1,la2;
    DBetype  typ1,typ2;
    short  status;
    DBSeg  arcseg[4];
    DBSeg *segpek;
    DBAny  gmstr1,gmstr2;
    DBVector  pos,p12;

/*
***Hämta den 1:a storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idp1, &la1, &typ1) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ, läs geometridata.
*/
    switch (typ1)
      {
      case LINTYP:
      DBread_line((DBLine *)&gmstr1, la1);
      break;

      case ARCTYP:
      DBread_arc((DBArc *)&gmstr1,NULL,la1);
      if ( gmstr1.arc_un.ns_a > 0 ) return(erpush("EX1412",""));
      break;

      default:
      return(erpush("EX1412",""));
      }
/*
***Hämta den 2:a storhetens la och typ.
*/
    if ( DBget_pointer( 'I', idp2, &la2, &typ2) < 0 )
      return ( erpush("EX1402",""));
/*
***Testa typ, läs geometridata.
*/
    switch (typ2)
      {
      case LINTYP:
      segpek = NULL;
      DBread_line((DBLine *)&gmstr2,la2);
      break;

      case ARCTYP:
      segpek = arcseg;
      DBread_arc((DBArc *)&gmstr2,segpek,la2);
      break;

      case CURTYP:
      DBread_curve((DBCurve *)&gmstr2,NULL,&segpek,la2);
      break;

      default:
      return(erpush("EX1412",""));
      }
/*
***Beräkna skärningen.
*/
    status=GEintersect_pv(&gmstr1,NULL,&gmstr2,(char *)segpek,
                                               lsyspk,inr,&u1,&u2);
    if ( typ2 == CURTYP ) DBfree_segments(segpek);
    if ( status < 0 ) return(erpush("EX1942",""));
/*
***Uppdatera storhetens ände.
*/
    switch (typ1)
      {
/*
***Linje.
*/
      case LINTYP:
      GEposition(&gmstr1,NULL,u1-1.0,(DBfloat)0.0,&pos);

      if ( end == 0 )
        {
        gmstr1.lin_un.crd1_l.x_gm = pos.x_gm;
        gmstr1.lin_un.crd1_l.y_gm = pos.y_gm;
        }
      else
        {
        gmstr1.lin_un.crd2_l.x_gm = pos.x_gm;
        gmstr1.lin_un.crd2_l.y_gm = pos.y_gm;
        }
/*
***Testa att den inte blev oändligt kort. 0.001 = TOL2.
*/
      p12.x_gm = gmstr1.lin_un.crd1_l.x_gm - gmstr1.lin_un.crd2_l.x_gm;
      p12.y_gm = gmstr1.lin_un.crd1_l.y_gm - gmstr1.lin_un.crd2_l.y_gm;
      l = GEvector_length2D(&p12);

      if ( l < 0.001 ) return(erpush("EX1932",""));
      else
        {
        EXdren(la1,typ1,FALSE,GWIN_ALL);
        DBupdate_line((DBLine *)&gmstr1, la1);
        EXdren(la1,typ1,TRUE,GWIN_ALL);
        }
      break;
/*
***Cirkel.
*/
      case ARCTYP:
      v = gmstr1.arc_un.v1_a + (u1-1.0)*(gmstr1.arc_un.v2_a-
                                         gmstr1.arc_un.v1_a);

      if ( end == 0 ) gmstr1.arc_un.v1_a = v;
      else gmstr1.arc_un.v2_a = v;
/*
***Kolla att inte v1 = v2.
*/
      if ( GE312(&gmstr1.arc_un.v1_a,&gmstr1.arc_un.v2_a) < 0 )
        return(erpush("EX1932",""));
      else
        {
        EXdren(la1,typ1,FALSE,GWIN_ALL);
        DBupdate_arc((DBArc *)&gmstr1, NULL, la1);
        EXdren(la1,typ1,TRUE,GWIN_ALL);
        }
      break;
      }
/*
***Slut.
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

        short EXdren(
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
 *      Ut: Inget.
 *
 *      FV:  0     => Ok.
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
 *      2004-07-09, Mesh, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la_tmp;
    short   i;
    char    str[V3STRLEN+1];
    DBfloat crdvek[4*GMXMXL];
    DBAny   gmpost;
    DBSeg   arcseg[4];
    DBSeg  *graseg;
    DBSeg  *sptarr[6];
    DBTmat  pmat;
    DBHeader hed;
    DBPart  part;

/*
***Hoppa till respektive typs rutin.
*/
    switch(typ)
      {
/*
***Punkt.
*/
      case POITYP:
      DBread_point((DBPoint*)&gmpost.poi_un,la);
      if ( win_id == GWIN_MAIN  ||  win_id == GWIN_ALL )
        {
        if ( draw ) gpdrpo(&gmpost.poi_un,la,DRAW);
        else        gpdlpo(&gmpost.poi_un,la);
        }
#ifdef V3_X11
      if ( win_id != GWIN_MAIN  &&  gptrty == X11 )
        {
        if ( draw ) wpdrpo(&gmpost.poi_un,la,win_id);
        else        wpdlpo(&gmpost.poi_un,la,win_id);
        }
#endif
#ifdef WIN32
      if ( win_id != GWIN_MAIN )
        {
        if ( draw ) wpdrpo(&gmpost.poi_un,la,win_id);
        else        wpdlpo(&gmpost.poi_un,la,win_id);
        }
#endif
      break;
/*
***Linje.
*/
      case LINTYP:
      DBread_line((DBLine *)&gmpost,la);
      if ( win_id == GWIN_MAIN  ||  win_id == GWIN_ALL )
        {
        if ( draw ) gpdrli(&gmpost.lin_un,la,DRAW);
        else        gpdlli(&gmpost.lin_un,la);
        }
#ifdef V3_X11
      if ( win_id != GWIN_MAIN  &&  gptrty == X11 )
        {
        if ( draw ) wpdrli(&gmpost.lin_un,la,win_id);
        else        wpdlli(&gmpost.lin_un,la,win_id);
        }
#endif
#ifdef WIN32
      if ( win_id != GWIN_MAIN )
        {
        if ( draw ) wpdrli(&gmpost.lin_un,la,win_id);
        else        wpdlli(&gmpost.lin_un,la,win_id);
        }
#endif
      break;
/*
***Arc.
*/
      case ARCTYP:
      DBread_arc((DBArc *)&gmpost,arcseg,la);
      if ( win_id == GWIN_MAIN  ||  win_id == GWIN_ALL )
        {
        if ( draw ) gpdrar(&gmpost.arc_un,arcseg,la,DRAW);
        else        gpdlar(&gmpost.arc_un,arcseg,la);
        }
#ifdef V3_X11
      if ( win_id != GWIN_MAIN  &&  gptrty == X11 )
        {
        if ( draw ) wpdrar(&gmpost.arc_un,arcseg,la,win_id);
        else        wpdlar(&gmpost.arc_un,arcseg,la,win_id);
        }
#endif
#ifdef WIN32
      if ( win_id != GWIN_MAIN )
        {
        if ( draw ) wpdrar(&gmpost.arc_un,arcseg,la,win_id);
        else        wpdlar(&gmpost.arc_un,arcseg,la,win_id);
        }
#endif
      break;
/*
***Kurva.
*/
      case CURTYP:
      DBread_curve((DBCurve *)&gmpost,&graseg,NULL,la);
      if ( win_id == GWIN_MAIN  ||  win_id == GWIN_ALL )
        {
        if ( draw ) gpdrcu(&gmpost.cur_un,graseg,la,DRAW);
        else        gpdlcu(&gmpost.cur_un,graseg,la);
        }
#ifdef V3_X11
      if ( win_id != GWIN_MAIN  &&  gptrty == X11 )
        {
        if ( draw ) wpdrcu(&gmpost.cur_un,graseg,la,win_id);
        else        wpdlcu(&gmpost.cur_un,graseg,la,win_id);
        }
#endif
#ifdef WIN32
      if ( win_id != GWIN_MAIN )
        {
        if ( draw ) wpdrcu(&gmpost.cur_un,graseg,la,win_id);
        else        wpdlcu(&gmpost.cur_un,graseg,la,win_id);
        }
#endif
      DBfree_segments(graseg);
      break;
/*
***Yta.
*/
      case SURTYP:
      DBread_surface((DBSurf *)&gmpost,la);
      DBread_srep_curves((DBSurf *)&gmpost,sptarr);
      if ( win_id == GWIN_MAIN  ||  win_id == GWIN_ALL )
        {
        if ( draw ) gpdrsu(&gmpost.sur_un,sptarr,la,DRAW);
        else        gpdlsu(&gmpost.sur_un,sptarr,la);
        }
#ifdef V3_X11
      if ( win_id != GWIN_MAIN  &&  gptrty == X11 )
        {
        if ( draw ) wpdrsu(&gmpost.sur_un,sptarr,la,win_id);
        else        wpdlsu(&gmpost.sur_un,sptarr,la,win_id);
        }
#endif
#ifdef WIN32
      if ( win_id != GWIN_MAIN )
        {
        if ( draw ) wpdrsu(&gmpost.sur_un,sptarr,la,win_id);
        else        wpdlsu(&gmpost.sur_un,sptarr,la,win_id);
        }
#endif
      DBfree_srep_curves(sptarr);
      break;
/*
***Text.
*/

      case TXTTYP:
      DBread_text((DBText*)&gmpost,str,la);
      if ( win_id == GWIN_MAIN  ||  win_id == GWIN_ALL )
        {
        if ( draw ) gpdrtx(&gmpost.txt_un,str,la,DRAW);
        else        gpdltx(&gmpost.txt_un,str,la);
        }
#ifdef V3_X11
      if ( win_id != GWIN_MAIN  &&  gptrty == X11 )
        {
        if ( draw ) wpdrtx(&gmpost.txt_un,str,la,win_id);
        else        wpdltx(&gmpost.txt_un,str,la,win_id);
        }
#endif
#ifdef WIN32
      if ( win_id != GWIN_MAIN )
        {
        if ( draw ) wpdrtx(&gmpost.txt_un,str,la,win_id);
        else        wpdltx(&gmpost.txt_un,str,la,win_id);
        }
#endif
      break;
/*
***Snitt.
*/
      case XHTTYP:
      DBread_xhatch((GMXHT *)&gmpost,crdvek,la);
      if ( win_id == GWIN_MAIN  ||  win_id == GWIN_ALL )
        {
        if ( draw ) gpdrxh(&gmpost.xht_un,crdvek,la,DRAW);
        else        gpdlxh(&gmpost.xht_un,crdvek,la);
        }
#ifdef V3_X11
      if ( win_id != GWIN_MAIN  &&  gptrty == X11 )
        {
        if ( draw ) wpdrxh(&gmpost.xht_un,crdvek,la,win_id);
        else        wpdlxh(&gmpost.xht_un,crdvek,la,win_id);
        }
#endif
#ifdef WIN32
      if ( win_id != GWIN_MAIN )
        {
        if ( draw ) wpdrxh(&gmpost.xht_un,crdvek,la,win_id);
        else        wpdlxh(&gmpost.xht_un,crdvek,la,win_id);
        }
#endif
      break;
/*
***Längdmått.
*/
      case LDMTYP:
      DBread_ldim((GMLDM *)&gmpost,la);
      if ( win_id == GWIN_MAIN  ||  win_id == GWIN_ALL )
        {
        if ( draw ) gpdrld(&gmpost.ldm_un,la,DRAW);
        else        gpdlld(&gmpost.ldm_un,la);
        }
#ifdef V3_X11
      if ( win_id != GWIN_MAIN  &&  gptrty == X11 )
        {
        if ( draw ) wpdrdm(&gmpost,la,win_id);
        else        wpdldm(&gmpost,la,win_id);
        }
#endif
#ifdef WIN32
      if ( win_id != GWIN_MAIN )
        {
        if ( draw ) wpdrdm(&gmpost,la,win_id);
        else        wpdldm(&gmpost,la,win_id);
        }
#endif
      break;
/*
***Diametermått.
*/
      case CDMTYP:
      DBread_cdim((GMCDM *)&gmpost,la);
      if ( win_id == GWIN_MAIN  ||  win_id == GWIN_ALL )
        {
        if ( draw ) gpdrcd(&gmpost.cdm_un,la,DRAW);
        else        gpdlcd(&gmpost.cdm_un,la);
        }
#ifdef V3_X11
      if ( win_id != GWIN_MAIN  &&  gptrty == X11 )
        {
        if ( draw ) wpdrdm(&gmpost,la,win_id);
        else        wpdldm(&gmpost,la,win_id);
        }
#endif
#ifdef WIN32
      if ( win_id != GWIN_MAIN )
        {
        if ( draw ) wpdrdm(&gmpost,la,win_id);
        else        wpdldm(&gmpost,la,win_id);
        }
#endif
      break;
/*
***Radiemått.
*/
      case RDMTYP:
      DBread_rdim((GMRDM *)&gmpost,la);
      if ( win_id == GWIN_MAIN  ||  win_id == GWIN_ALL )
        {
        if ( draw ) gpdrrd(&gmpost.rdm_un,la,DRAW);
        else        gpdlrd(&gmpost.rdm_un,la);
        }
#ifdef V3_X11
      if ( win_id != GWIN_MAIN  &&  gptrty == X11 )
        {
        if ( draw ) wpdrdm(&gmpost,la,win_id);
        else        wpdldm(&gmpost,la,win_id);
        }
#endif
#ifdef WIN32
      if ( win_id != GWIN_MAIN )
        {
        if ( draw ) wpdrdm(&gmpost,la,win_id);
        else        wpdldm(&gmpost,la,win_id);
        }
#endif
      break;
/*
***Vinkelmått.
*/
      case ADMTYP:
      DBread_adim((GMADM *)&gmpost,la);
      if ( win_id == GWIN_MAIN  ||  win_id == GWIN_ALL )
        {
        if ( draw ) gpdrad(&gmpost.adm_un,la,DRAW);
        else        gpdlad(&gmpost.adm_un,la);
        }
#ifdef V3_X11
      if ( win_id != GWIN_MAIN  &&  gptrty == X11 )
        {
        if ( draw ) wpdrdm(&gmpost,la,win_id);
        else        wpdldm(&gmpost,la,win_id);
        }
#endif
#ifdef WIN32
      if ( win_id != GWIN_MAIN )
        {
        if ( draw ) wpdrdm(&gmpost,la,win_id);
        else        wpdldm(&gmpost,la,win_id);
        }
#endif
      break;
/*
***Koordinatsystem.
*/
      case CSYTYP:
      DBread_csys((GMCSY *)&gmpost,&pmat,la);
      if ( win_id == GWIN_MAIN  ||  win_id == GWIN_ALL )
        {
        if ( draw ) gpdrcs(&gmpost.csy_un,&pmat,la,DRAW);
        else        gpdlcs(&gmpost.csy_un,&pmat,la);
        }
#ifdef V3_X11
      if ( win_id != GWIN_MAIN  &&  gptrty == X11 )
        {
        if ( draw ) wpdrcs(&gmpost.csy_un,&pmat,la,win_id);
        else        wpdlcs(&gmpost.csy_un,&pmat,la,win_id);
        }
#endif
#ifdef WIN32
      if ( win_id != GWIN_MAIN )
        {
        if ( draw ) wpdrcs(&gmpost.csy_un,&pmat,la,win_id);
        else        wpdlcs(&gmpost.csy_un,&pmat,la,win_id);
        }
#endif
      break;
/*
***B-plan.
*/
      case BPLTYP:
      DBread_bplane((DBBplane*)&gmpost,la);
      if ( win_id == GWIN_MAIN  ||  win_id == GWIN_ALL )
        {
        if ( draw ) gpdrbp(&gmpost.bpl_un,la,DRAW);
        else        gpdlbp(&gmpost.bpl_un,la);
        }
#ifdef V3_X11
      if ( win_id != GWIN_MAIN  &&  gptrty == X11 )
        {
        if ( draw ) wpdrbp(&gmpost.bpl_un,la,win_id);
        else        wpdlbp(&gmpost.bpl_un,la,win_id);
        }
#endif
#ifdef WIN32
      if ( win_id != GWIN_MAIN )
        {
        if ( draw ) wpdrbp(&gmpost.bpl_un,la,win_id);
        else        wpdlbp(&gmpost.bpl_un,la,win_id);
        }
#endif
      break;
/*
***Mesh.
*/
      case MSHTYP:
      DBread_mesh((DBMesh*)&gmpost,la,MESH_ALL);
      if ( win_id == GWIN_MAIN  ||  win_id == GWIN_ALL )
        {
        if ( draw ) gpdrms(&gmpost.msh_un,la,DRAW);
        else        gpdlms(&gmpost.msh_un,la);
        }
#ifdef V3_X11
      if ( win_id != GWIN_MAIN  &&  gptrty == X11 )
        {
        if ( draw ) wpdrms(&gmpost.msh_un,la,win_id);
        else        wpdlms(&gmpost.msh_un,la,win_id);
        }
#endif
#ifdef WIN32
      if ( win_id != GWIN_MAIN )
        {
        if ( draw ) wpdrms(&gmpost.msh_un,la,win_id);
        else        wpdlms(&gmpost.msh_un,la,win_id);
        }
#endif
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
    DBetype   typ;

/*
***Hämta LA och typ för huvud-parten.
*/
    DBget_pointer('F',&dummy,&la,&typ);
/*
***Loopa runt och hämta LA och typ för resten av DB.
*/
    while ( DBget_pointer('N',&dummy,&la,&typ) == 0 )
      {
      if ( intrup ) return(igwtma(168));

#ifdef V3_X11
      if ( gptrty == X11  &&  wpintr() ) return(igwtma(168));
#endif

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
