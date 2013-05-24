/*!*******************************************************
*
*    extform.c
*    =========
*
*    EXetrf();    Create transformation
*    EXtrmv();    Create TFORM_MOVE
*    EXtrro();    Create TFORM_ROT
*    EXtrmi();    Create TFORM_MIRR
*    EXtrco();    Create TFORM_COMP
*    EXtrus();    Create TFORM_USDEF
*    EXtcop();    Create TCOPY
*    EXtcla();    Transform and copy by DBptr
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
#include "../../DB/include/DBintern.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../include/EX.h"

extern DBptr   lsysla;
extern V2NAPA  defnap;
extern DBTmat  lklsyi;
extern DBTmat *lsyspk;

/*!******************************************************/

       short EXetrf(
       DBId    *id,
       DBTform   *trfpek,
       V2NAPA  *pnp)

/*      Lagrar en transformation i GM.
 *
 *      In: id     => Pekare till identitet.
 *          trfpek => Pekare till GM-stuktur.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          EX2022 => Kan ej lagra 
 *
 *      (C)microform ab 15/12/92 J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;

/*
***Fyll i namnparameterdata.
*/
    trfpek->hed_tf.blank = pnp->blank;
    trfpek->hed_tf.pen   = pnp->pen;
    trfpek->hed_tf.level = pnp->level;
    trfpek->hed_tf.hit   = FALSE;
    trfpek->pcsy_tf      = lsysla;
/*
***Lagra i GM.
*/
    if ( pnp->save )
      if ( DBinsert_tform(trfpek,id,&la) < 0 ) return(erpush("EX2022",""));

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short EXtrmv(
       DBId     *id,
       DBVector *p1,
       DBVector *p2,
       DBfloat   sx,
       DBfloat   sy,
       DBfloat   sz,
       V2NAPA   *pnp)

/*      Skapa translations-transformation. TFORM_MOVE.
 *
 *      In: id       => Pekare till identitet.
 *          p1       => Pekare till basposition.
 *          p2       => Pekare till ny position.
 *          sx,sy,sz => Skalor.
 *          pnp      => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C)microform ab 10/12/92 J. Kjellander
 *
 ******************************************************!*/

  {
    DBTform   trf;

/*
***Transformera indata till basic.
*/
    if ( lsyspk != NULL )
      {
      GEtfpos_to_local(p1,&lklsyi,p1);
      GEtfpos_to_local(p2,&lklsyi,p2);
      }
/*
***Skapa matrisen.
*/
    GEmktf_ts(p1,p2,sx,sy,sz,&trf.mat_tf);
/*
***Lagra i GM.
*/
    return(EXetrf(id,&trf,pnp)); 
  }
  
/********************************************************/
/*!******************************************************/

       short EXtrro(
       DBId     *id,
       DBVector *p1,
       DBVector *p2,
       DBfloat   v,
       DBfloat   sx,
       DBfloat   sy,
       DBfloat   sz,
       V2NAPA   *pnp)

/*      Skapa rotations-transformation. TFORM_ROT.
 *
 *      In: id       => Pekare till identitet.
 *          p1       => Pekare till pos-1 på axeln.
 *          p2       => Pekare till pos-2 på axeln.
 *          v        => Rotationsvinkel.
 *          sx,sy,sz => Skalor.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C)microform ab 10/12/92 J. Kjellander
 *
 ******************************************************!*/

  {
    DBTform   trf;

/*
***Transformera indata till basic.
*/
    if ( lsyspk != NULL )
      {
      GEtfpos_to_local(p1,&lklsyi,p1);
      GEtfpos_to_local(p2,&lklsyi,p2);
      }
/*
***Skapa matrisen.
*/
    GEmktf_rs(p1,p2,v,sx,sy,sz,&trf.mat_tf);
/*
***Lagra i GM.
*/
    return(EXetrf(id,&trf,pnp)); 
  }
  
/********************************************************/
/*!******************************************************/

       short EXtrmi(
       DBId     *id,
       DBVector *p1,
       DBVector *p2,
       DBVector *p3,
       V2NAPA   *pnp)

/*      Skapa speglings-transformation. TFORM_MIRR.
 *
 *      In: id     => Pekare till identitet.
 *          p1     => Pekare till pos-1 i planet.
 *          p2     => Pekare till pos-2 i planet.
 *          p3     => Pekare till pos-3 i planet.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C)microform ab 10/12/92 J. Kjellander
 *
 ******************************************************!*/

  {
    DBTform   trf;

/*
***Transformera indata till basic.
*/
    if ( lsyspk != NULL )
      {
      GEtfpos_to_local(p1,&lklsyi,p1);
      GEtfpos_to_local(p2,&lklsyi,p2);
      GEtfpos_to_local(p3,&lklsyi,p3);
      }
/*
***Skapa matrisen.
*/
    GEmktf_mirr(p1,p2,p3,&trf.mat_tf);
/*
***Lagra i GM.
*/
    return(EXetrf(id,&trf,pnp)); 
  }
  
/********************************************************/
/*!******************************************************/

       short EXtrco(
       DBId     *id,
       DBVector *p1,
       DBVector *p2,
       DBVector *p3,
       DBVector *p4,
       DBVector *p5,
       DBVector *p6,
       V2NAPA   *pnp)

/*      Skapa sammansatt transformation. TFORM_COMP.
 *
 *      In: id     => Pekare till identitet.
 *          p1     => Pekare till pos-1 före transformation.
 *          p2     => Pekare till pos-2 före transformation.
 *          p3     => Pekare till pos-3 före transformation.
 *          p4     => Pekare till pos-1 efter transformation.
 *          p5     => Pekare till pos-2 efter transformation.
 *          p6     => Pekare till pos-3 efter transformation.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C)microform ab 24/2/93 J. Kjellander
 *
 ******************************************************!*/

  {
    short  status;
    DBTform  trf;

/*
***Transformera indata till basic.
*/
    if ( lsyspk != NULL )
      {
      GEtfpos_to_local(p1,&lklsyi,p1);
      GEtfpos_to_local(p2,&lklsyi,p2);
      GEtfpos_to_local(p3,&lklsyi,p3);
      GEtfpos_to_local(p4,&lklsyi,p4);
      GEtfpos_to_local(p5,&lklsyi,p5);
      GEtfpos_to_local(p6,&lklsyi,p6);
      }
/*
***Skapa matrisen.
*/
    if ( (status=GEmktf_6p(p1,p2,p3,p4,p5,p6,&trf.mat_tf)) < 0 )
      return(status);
/*
***Lagra i GM.
*/
    return(EXetrf(id,&trf,pnp)); 
  }
  
/********************************************************/
/*!******************************************************/

       short EXtrus(
       DBId   *id,
       DBTmat *ptr,
       V2NAPA *pnp)

/*      Skapa användardefinierad-transformation.
 *      TFORM_USEDF.
 *
 *      In: id   => Pekare till identitet.
 *          ptr  => Pekare till transformation.
 *          pnp  => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *
 *      (C)microform ab 10/12/92 J. Kjellander
 *
 ******************************************************!*/

  {
    DBTform   trf;

/*
***Skapa matrisen.
*/
    V3MOME(ptr,&trf.mat_tf,sizeof(DBTmat));
/*
***Lagra i GM.
*/
    return(EXetrf(id,&trf,pnp)); 
  }
  
/********************************************************/
/*!******************************************************/

        short EXtcop(
        DBId   *id,
        DBId   *ref,
        DBId   *trid,
        DBshort mode,
        V2NAPA *pnp)

/*      Kopiera och transformera. TCOPY.
 *
 *      In: id     => Pekare till identitet.
 *          ref    => Pekare till id för storhet att kopiera.
 *          trid   => Pekare till id för transformation.
 *          mode   => 0 = Använd ursprungliga attribut
 *                    1 = Använd aktiva attribut
 *          pnp    => Pekare till aktiva attribut.
 *
 *      Ut: Inget.
 *
 *      FV: 0      => OK
 *
 *      Felkoder: EX1402 = Storheten finns ej.
 *                EX1412 = Fel typ av storhet, ej TRFTYP.
 *                EX2032 = Tidigare instanser av annan typ.
 *                EX2082 = Kan ej kopiera, id=%s
 *
 *      (C)microform ab 10/12/92 J. Kjellander
 *
 *       5/12/94 EX2082, J. Kjellander
 *
 ******************************************************!*/

  {
    char  errbuf[81];
    short status;
    DBetype newtyp,reftyp,trtyp;
    DBptr newla,refla,trla;
    DBTform trf;

/*
***Hämta LA för storheten.
*/
    if ( DBget_pointer('I',ref,&refla,&reftyp) < 0 ) return(erpush("EX1402",""));
/*
***Hämta LA för transformationen.
*/
    if ( DBget_pointer('I',trid,&trla,&trtyp) < 0 ) return(erpush("EX1402",""));
    if ( trtyp != TRFTYP ) return(erpush("EX1412",""));
/*
***Hämta transformationen ur GM.
*/
    DBread_tform(&trf,trla);
/*
***Kolla att eventuella tidigare skapade instanser är
***av samma typ.
*/
    if ( DBget_pointer('I',id,&newla,&newtyp) < 0 ) erinit();
    else if ( newtyp != reftyp ) return(erpush("EX2032",""));
/*
***Kopiera.
*/
    status = EXtcla(id,refla,reftyp,&trf.mat_tf,mode,pnp);
    if ( status < 0 )
      {
      igidst(ref,errbuf);
      return(erpush("EX2082",errbuf));
      }
    else return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXtcla(
        DBId    *id,
        DBptr    la,
        DBetype  typ,
        DBTmat  *ptr,
        DBshort  mode,
        V2NAPA  *pnp)

/*      Kopierar en storhet.
 *
 *      In: id     = Nya storhetens ID.
 *          la     = Gamla storhetens LA.
 *          typ    = Storhetens typ.
 *          ptr    = Pekare till transformationsmatris.
 *          mode   = 0 = Kopiera gamla attribut.
 *                   1 = Använd aktiva attribut.
 *          pnp    = Attribut.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX2042 = Felaktigt värde på mode
 *                EX2072 = Kan ej transformera kurva.
 *
 *      (C)microform ab 16/3/88 J. Kjellander
 *
 *      15/4/93    Nya kurvor, J.Kjellander
 *      5/12/94    EX2072, J.Kjellander
 *      19/1-95    Bug BASIC, J.Kjellander
 *      1996-06-25 default:, J.Kjellander
 *      1996-11-06 Grafiska segment, J. Kjellander
 *      2004-07-18 Mesh, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    char     str[V3STRLEN+1];
    DBptr    la_tmp,dummy;
    short    status=0,i;
    DBPatch *patpek;
    DBHeader hed;
    DBPart   part;
    DBPdat   dat;
    DBAny    ogmpst;
    DBAny    ngmpst;
    DBTmat   opmat;
    DBTmat   npmat;
    DBId     cpid;
    V2NAPA  *att;
    V2NAPA   oldatt;
    DBSeg   *oldgra,*oldgeo;
    DBSeg   *newgra,*newgeo;
    DBSeg    aseg1[4],aseg2[4];
    DBfloat  xhtcrd[4*GMXMXL];

/*
***Vilka attribut skall användas ?
*/
    switch ( mode )
      {
      case 0:
      EXgala(la,typ,&oldatt,&dummy);
      att = &oldatt;
      break;

      case 1:
      att = pnp;
      break;

      default:
      return(erpush("EX2042",""));
      }
/*
***Kopiera.
*/
    switch (typ)
      {
      case POITYP:
        DBread_point(&ogmpst.poi_un,la);
        DBread_point(&ngmpst.poi_un,la);
        GEtfPoint_to_local(&ogmpst.poi_un,ptr,&ngmpst.poi_un);
        status = EXepoi(id,&ngmpst.poi_un,att);
        break;

      case LINTYP:
        DBread_line(&ogmpst.lin_un,la);
        DBread_line(&ngmpst.lin_un,la);
        GEtfLine_to_local(&ogmpst.lin_un,ptr,&ngmpst.lin_un);
        status = EXelin(id,&ngmpst.lin_un,att);
        break;

      case ARCTYP:
        DBread_arc(&ogmpst.arc_un,aseg1,la);
        DBread_arc(&ngmpst.arc_un,aseg2,la);
        GEtfArc_to_local(&ogmpst.arc_un,aseg1,ptr,&ngmpst.arc_un,aseg2);
        status = EXearc(id,&ngmpst.arc_un,aseg2,att);
        break;

      case CURTYP:
        DBread_curve(&ogmpst.cur_un,&oldgra,&oldgeo,la);
        DBread_curve(&ngmpst.cur_un,&newgra,&newgeo,la);
        status = GEtfCurve_to_local(&ogmpst.cur_un,oldgeo,oldgra,ptr,
                        &ngmpst.cur_un,newgeo,newgra);
        if ( status < 0 ) status = erpush("EX2072","");
        else              status = EXecur(id,&ngmpst.cur_un,newgra,newgeo,att);
        if ( oldgra != oldgeo ) DBfree_segments(oldgra);
        DBfree_segments(oldgeo);
        if ( newgra != newgeo ) DBfree_segments(newgra);
        DBfree_segments(newgeo);
        break;

      case SURTYP:
        DBread_surface(&ngmpst.sur_un,la);
        DBread_patches(&ngmpst.sur_un,&patpek);
        status = GEtfSurf_to_local(&ngmpst.sur_un,patpek,ptr);
        if ( status == 0 ) status = EXesur(id,&ngmpst.sur_un,patpek,att);
        DBfree_patches(&ngmpst.sur_un,patpek);
        break;

      case CSYTYP:
        DBread_csys(&ogmpst.csy_un,&opmat,la);
        DBread_csys(&ngmpst.csy_un,&npmat,la);
        GEtfCsys_to_local(&ogmpst.csy_un,&opmat,ptr,&ngmpst.csy_un,&npmat);
        status = EXecsy(id,&ngmpst.csy_un,&npmat,att);
        break;

      case BPLTYP:
        DBread_bplane(&ogmpst.bpl_un,la);
        DBread_bplane(&ngmpst.bpl_un,la);
        GEtfBplane_to_local(&ogmpst.bpl_un,ptr,&ngmpst.bpl_un);
        status = EXebpl(id,&ngmpst.bpl_un,att);
        break;

      case MSHTYP:
        DBread_mesh(&ogmpst.msh_un,la,MESH_ALL);
        GEtfMesh_to_local(&ogmpst.msh_un,ptr);
        status = EXemsh(id,&ogmpst.msh_un,att);
        if ( status == 0 ) DBfree_mesh(&ogmpst.msh_un);
        break;

      case TXTTYP:
        DBread_text(&ogmpst.txt_un,str,la);
        DBread_text(&ngmpst.txt_un,str,la);
        GEtfText_to_local(&ogmpst.txt_un,ptr,&ngmpst.txt_un);
        status = EXetxt(id,&ngmpst.txt_un,str,att);
        break;

      case LDMTYP:
        DBread_ldim(&ogmpst.ldm_un,la);
        DBread_ldim(&ngmpst.ldm_un,la);
        GEtfLdim_to_local(&ogmpst.ldm_un,ptr,&ngmpst.ldm_un);
        status = EXeldm(id,&ngmpst.ldm_un,att);
        break;

      case CDMTYP:
        DBread_cdim(&ogmpst.cdm_un,la);
        DBread_cdim(&ngmpst.cdm_un,la);
        GEtfCdim_to_local(&ogmpst.cdm_un,ptr,&ngmpst.cdm_un);
        status = EXecdm(id,&ngmpst.cdm_un,att);
        break;

      case RDMTYP:
        DBread_rdim(&ogmpst.rdm_un,la);
        DBread_rdim(&ngmpst.rdm_un,la);
        GEtfRdim_to_local(&ogmpst.rdm_un,ptr,&ngmpst.rdm_un);
        status = EXerdm(id,&ngmpst.rdm_un,att);
        break;

      case ADMTYP:
        DBread_adim(&ogmpst.adm_un,la);
        DBread_adim(&ngmpst.adm_un,la);
        GEtfAdim_to_local(&ogmpst.adm_un,ptr,&ngmpst.adm_un);
        status = EXeadm(id,&ngmpst.adm_un,att);
        break;

      case XHTTYP:
        DBread_xhatch(&ngmpst.xht_un,xhtcrd,la);
        GEtfHatch_to_local(&ngmpst.xht_un,xhtcrd,ptr);
        status = EXexht(id,&ngmpst.xht_un,xhtcrd,att);
        break;
/*
***Kopiera part.
*/
      case PRTTYP:
        DBread_part(&part,la);
        DBread_part_parameters(&dat,NULL,NULL,part.dtp_pt,part.dts_pt);
        dat.npar_pd = 0;
        dat.matt_pd = BASIC;
        if ( EXoppt(id,NULL,&part,&dat,NULL,NULL) < 0 )
          return(erpush("EX1952",""));

        DBread_part(&part,la);
        for ( i=0; i<part.its_pt; ++i )
          {
          la_tmp = gmrdid(part.itp_pt,i);
          if ( la_tmp >= 0 )
            {
            do
              {
              DBread_header(&hed,la_tmp);
              cpid.seq_val = hed.seknr;
              cpid.ord_val = 0;
              cpid.p_nextre = NULL;
              if ( (status=EXtcla(&cpid,la_tmp,hed.type,ptr,mode,pnp)) < 0 )
                {
                EXclpt();
                return(status);
                }
              la_tmp = hed.n_ptr;
              }
            while ( la_tmp != DBNULL );
            }
          }
        EXclpt();
        break;
/*
***Övriga storheter kan inte transformeras.
*/
      default: status = -2; break;
      }

    return(status);
  }

/********************************************************/
