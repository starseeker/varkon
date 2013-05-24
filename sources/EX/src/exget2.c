/*!*****************************************************
*
*    exget2.c
*    ========
*
*    EXgtpt();     Interface routine for GETPRT
*    EXgtyp();     Interface routine for GETTYP
*    EXgint();     Interface routine for GETINT
*    EXgflt();     Interface routine for GETFLT
*    EXgstr();     Interface routine for GETSTR
*    EXgvec();     Interface routine for GETVEC
*    EXgref();     Interface routine for GETREF
*    EXuphd();     Interface routine for UPDHDR
*
*    EXdel();      Interface-routine for DEL
*    EXdlla();     Delete entity by DBptr
*
*    This file is part of the VARKON Execute  Library.
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
#include "../../IG/include/IG.h"
#include "../../WP/include/WP.h"
#include "../include/EX.h"
#include <string.h>
#include <time.h>

/*!******************************************************/

        short EXgtpt(
        DBId    *id,
        DBPart   *part)

/*      Interface-rutin för GETPRT. Hämtar part-data
 *      ur GM.
 *
 *      In: id     => Pekare till partens identitet.
 *          part   => Pekare till DBPart.
 *
 *      Ut: *part  => Part-post.
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
    if ( typ != PRTTYP ) return(erpush("EX1412",""));
/*
***Läs part-posten och returnera part-data.
*/
    DBread_part(part,la);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgtyp(
        DBId    *id,
        DBshort  pnum,
        PMLITVA *typ)

/*      Interface-rutin för GETTYP. Hämtar typ-data
 *      ur GM.
 *
 *      In: id     => Pekare till partens ID
 *          pnum   => Parameterns orningsnummer.
 *          typ    => Pekare till short.
 *
 *      Ut: *typ   => Typ av parameter.
 *
 *      FV:  0     => Ok.
 *          EX1612 => Parametern finns ej
 *
 *      (C)microform ab 1/4/86 J. Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    DBPart    part;
    DBPdat   data;
    PMPATLOG typarr[V2MPMX];

/*
***Läs part-posten.
*/
    if ( (status=EXgtpt(id,&part)) < 0 ) return(status);
/*
***Läs data-posten.
*/
    DBread_part_parameters(&data,typarr,NULL,part.dtp_pt,part.dts_pt);
/*
***Kolla om parametern pnum finns.
*/
    if ( pnum > data.npar_pd || pnum < 1 ) return(erpush("EX1612",""));
/*
***Returnera typ.
*/
    typ->lit.int_va = typarr[pnum-1].log_id;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgint(
        DBId    *id,
        DBshort  pnum,
        PMLITVA *intval)

/*      Interface-rutin för GETINT. Hämtar heltalsparam.
 *      ur GM.
 *
 *      In: id      => Pekare till partens ID
 *          pnum    => Parameterns ordningsnummer.
 *          intval  => Pekare till heltal.
 *
 *      Ut: *intval  => Heltalet.
 *
 *      FV:  0      => Ok.
 *          EX1612 => Parametern finns ej
 *          EX1622 => Parametern har inte typen %s
 *
 *      (C)microform ab 1/4/86 J. Kjellander
 *
 *      23/10/86 v3dbuf, J. Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    DBPart   part;
    DBPdat   data;
    PMPATLOG typarr[V2MPMX];
    DBint   *valpek;
    char     valarr[V3STRLEN*V2MPMX];

/*
***Läs part-posten.
*/
    if ( (status=EXgtpt(id,&part)) < 0 ) return(status);
/*
***Läs data-posten.
*/
    DBread_part_parameters(&data,typarr,valarr,part.dtp_pt,part.dts_pt);
/*
***Kolla om parametern pnum finns och har rätt typ.
*/
    if ( pnum > data.npar_pd || pnum < 1 ) return(erpush("EX1612",""));
    if ( typarr[pnum-1].log_id != C_INT_VA ) return(erpush("EX1622","<int>"));
/*
***Returnera värde.
*/
    valpek = (DBint *) &valarr[typarr[pnum-1].log_offs];
    intval->lit.int_va = *valpek;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgflt(
        DBId    *id,
        DBshort  pnum,
        PMLITVA *fltval)

/*      Interface-rutin för GETFLT. Hämtar flyttals-param.
 *      ur GM.
 *
 *      In: id      => Pekare till partens ID
 *          pnum    => Parameterns ordningsnummer
 *          fltval  => Pekare till flyttal
 *
 *      Ut: *fltval = Flyttal
 *
 *      FV:      0 => Ok.
 *          EX1612 => Parametern finns ej
 *          EX1622 => Parametern har inte typen %s
 *
 *      (C)microform ab 2/4/86 J. Kjellander
 *
 *      23/10/86 v3dbuf, J. Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    DBPart   part;
    DBPdat   data;
    PMPATLOG typarr[V2MPMX];
    v2float *valpek;
    char     valarr[V3STRLEN*V2MPMX];

/*
***Läs part-posten.
*/
    if ( (status=EXgtpt(id,&part)) < 0 ) return(status);
/*
***Läs data-posten.
*/
    DBread_part_parameters(&data,typarr,valarr,part.dtp_pt,part.dts_pt);
/*
***Kolla om parametern pnum finns och har rätt typ.
*/
    if ( pnum > data.npar_pd || pnum < 1 ) return(erpush("EX1612",""));
    if ( typarr[pnum-1].log_id != C_FLO_VA )
                  return(erpush("EX1622","<float>"));
/*
***Returnera värde.
*/
    valpek = (v2float *) &valarr[typarr[pnum-1].log_offs];
    fltval->lit.float_va = *valpek;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgstr(
        DBId    *id,
        DBshort  pnum,
        PMLITVA *strval)

/*      Interface-rutin för GETSTR. Hämtar sträng-param
 *      ur GM.
 *
 *      In: id     => Pekare till partens ID
 *          pnum   => Parameterns ordningsnummer
 *          strval => Pekare till sträng
 *
 *      Ut: *strval  => Sträng
 *
 *      FV:      0 => Ok.
 *          EX1612 => Parametern finns ej
 *          EX1622 => Parametern har inte typen %s
 *
 *      (C)microform ab 2/4/86 J. Kjellander
 *
 *      23/10/86 v3dbuf, J. Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    DBPart   part;
    DBPdat   data;
    PMPATLOG typarr[V2MPMX];
    char     valarr[V3STRLEN*V2MPMX];

/*
***Läs part-posten.
*/
    if ( (status=EXgtpt(id,&part)) < 0 ) return(status);
/*
***Läs data-posten.
*/
    DBread_part_parameters(&data,typarr,valarr,part.dtp_pt,part.dts_pt);
/*
***Kolla om parametern pnum finns och har rätt typ.
*/
    if ( pnum > data.npar_pd || pnum < 1 ) return(erpush("EX1612",""));
    if ( typarr[pnum-1].log_id != C_STR_VA )
                          return(erpush("EX1622","<string>"));
/*
***Returnera värde.
*/
    strcpy(strval->lit.str_va,&valarr[typarr[pnum-1].log_offs]);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgvec(
        DBId    *id,
        DBshort  pnum,
        PMLITVA *vecval)

/*      Interface-rutin för GETVEC. Hämtar vector-param.
 *      ur GM.
 *
 *      In: id     => Pekare till partens ID
 *          pnum   => Parameterns ordningsnummer
 *          vecval => Pekare till v2vecva
 *
 *      Ut: *vecval => Vektor
 *
 *      FV:      0 => Ok.
 *          EX1612 => Parametern finns ej
 *          EX1622 => Parametern har inte typen %s
 *
 *      (C)microform ab 2/4/86 J. Kjellander
 *
 *      23/10/86 v3dbuf, J. Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    DBPart   part;
    DBPdat   data;
    PMPATLOG typarr[V2MPMX];
    V2VECVA *vecp;
    char     valarr[V3STRLEN*V2MPMX];

/*
***Läs part-posten.
*/
    if ( (status=EXgtpt(id,&part)) < 0 ) return(status);
/*
***Läs data-posten.
*/
    DBread_part_parameters(&data,typarr,valarr,part.dtp_pt,part.dts_pt);
/*
***Kolla om parametern pnum finns och har rätt typ.
*/
    if ( pnum > data.npar_pd || pnum < 1 ) return(erpush("EX1612",""));
    if ( typarr[pnum-1].log_id != C_VEC_VA ) 
                         return(erpush("EX1622","<vector>"));
/*
***Returnera värde.
*/
    vecp = (V2VECVA *) &valarr[typarr[pnum-1].log_offs];
    vecval->lit.vec_va.x_val = vecp->x_val;
    vecval->lit.vec_va.y_val = vecp->y_val;
    vecval->lit.vec_va.z_val = vecp->z_val;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXgref(
        DBId    *id,
        DBshort  pnum,
        PMLITVA *refval)

/*      Interface-rutin för GETVEC. Hämtar ref-param.
 *      ur GM.
 *
 *      In: id     => Pekare till partens ID
 *          pnum   => Parameterns ordningsnummer
 *          vecval => Pekare till värde
 *
 *      Ut: *refval => Referens
 *
 *      FV:      0 => Ok.
 *          EX1612 => Parametern finns ej
 *          EX1622 => Parametern har inte typen %s
 *
 *      (C)microform ab 1/6/86 J. Kjellander
 *
 *      23/10/86 v3dbuf, J. Kjellander
 *
 ******************************************************!*/

  {
    short    status,i;
    DBPart   part;
    DBPdat   data;
    char     valarr[V3STRLEN*V2MPMX];
    PMPATLOG typarr[V2MPMX];
    V2REFVA *refp;

/*
***Läs part-posten.
*/
    if ( (status=EXgtpt(id,&part)) < 0 ) return(status);
/*
***Läs data-posten.
*/
    DBread_part_parameters(&data,typarr,valarr,part.dtp_pt,part.dts_pt);
/*
***Kolla om parametern pnum finns och har rätt typ.
*/
    if ( pnum > data.npar_pd || pnum < 1 ) return(erpush("EX1612",""));
    if ( typarr[pnum-1].log_id != C_REF_VA ) 
                         return(erpush("EX1622","<ref>"));
/*
***Returnera värde.
*/
    refp = (V2REFVA *) &valarr[typarr[pnum-1].log_offs];

    for ( i=0;; ++i )
      {
      refval->lit.ref_va[i].seq_val = (refp+i)->seq_val;
      refval->lit.ref_va[i].ord_val = (refp+i)->ord_val;
      if ( (refp+i)->p_nextre == NULL )
        {
        refval->lit.ref_va[i].p_nextre = NULL;
        break;
        }
      else
        {
        refval->lit.ref_va[i].p_nextre = &refval->lit.ref_va[i+1];
        }
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXuphd(
        DBId    *id,
        DBshort blank,
        DBshort level,
        DBshort pen)

/*      Interface-rutin för UPDHDR. Skriver header-data
 *      till angiven storhet.
 *
 *      In: id       => Storhetens identitet.
 *          blank    => Tänd/Släckt.
 *          level    => Nivå.
 *          pen      => Pennummer.
 *
 *      Ut: Inget.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Storheten finns ej.
 *
 *      (C)microform ab 17/10/88 J. Kjellander
 *
 *      15/3/92  refcnt, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la;
    DBetype  typ;
    DBHeader header;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',id,&la,&typ) < 0 )
               return(erpush("EX1402",""));
/*
***Läs gamla headern.
*/
    DBread_header(&header,la);
/*
***Lite indatakontroll.
*/
    if ( blank != 0 ) header.blank  = 1; else header.blank = 0;
    if ( pen    < 0 ) header.pen    = 0; else header.pen = pen;

    if ( level  < 0  ||  level > WP_NIVANT-1 )
      header.level  = 0;
    else
      header.level = level;
/*
***Uppdatera.
*/
    return(DBupdate_header(&header,la));
  }

/********************************************************/
/*!******************************************************/

        short EXdel(DBId *idpek)

/*      Interface-rutin för DEL. Tar reda på storhetens
 *      LA och typ samt anropar EXdlla() för att ta bort
 *      den.
 *
 *      In: idpek  => Pekare till storhetens identitet.
 *
 *      Ut: Inget.
 *
 *      FV:  0     => Ok.
 *          EX1402 => Hittar ej storheten
 *
 *      (C)microform ab 22/1/86 J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype   typ;

/*
***Översätt ID -> la.
*/
    if ( DBget_pointer('I',idpek,&la,&typ) < 0 )
       return(erpush("EX1402",""));
/*
***Ta bort storheten.
*/
    return(EXdlla(la,typ));
  }

/********************************************************/
/*!******************************************************/

        short EXdlla(
        DBptr   la,
        DBetype typ)

/*      Tar bort storheten vid la.
 *
 *      In: la   => Storhetens LA.
 *          typ  => Typ av storhet.
 *
 *      Ut: Inget.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 1/2/86 J. Kjellander
 *
 *      10/4/86 EXdren(), J. Kjellander
 *      24/3/92 DBdelete_entity(), J. Kjellander
 *      1999-02-04 felhantering, J.Kjellander
 *
 ******************************************************!*/

  {
   short status;

/*
***Sudda storheten ur GP.
*/
#ifdef VARKON 
    EXdren(la,typ,FALSE,GWIN_ALL);
#endif
/*
***Sudda storheten ur GM.
*/
    status = DBdelete_entity(la,typ);
    if ( status < 0 )
      {
#ifdef VARKON
      EXdren(la,typ,TRUE,GWIN_ALL);
#endif
      return(status);
      }

    return(0);
  }

/********************************************************/
