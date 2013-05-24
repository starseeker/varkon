/**********************************************************************
*
*    dbdim.c
*    =======
*
*
*    This file includes the following public functions:
*
*    DBinsert_ldim();   Inserts a ldim entity
*    DBread_ldim();     Reads a ldim entity
*    DBupdate_ldim();   Updates a ldim entity
*    DBdelete_ldim();   Deletes a ldim entity
*
*    DBinsert_cdim();   Inserts a cdim entity
*    DBread_cdim();     Reads a cdim entity
*    DBupdate_cdim();   Updates a cdim entity
*    DBdelete_cdim();   Deletes a cdim entity
*
*    DBinsert_rdim();   Inserts a rdim entity
*    DBread_rdim();     Reads a rdim entity
*    DBupdate_rdim();   Updates a rdim entity
*    DBdelete_rdim();   Deletes a rdim entity
*
*    DBinsert_adim();   Inserts a adim entity
*    DBread_adim();     Reads a adim entity
*    DBupdate_adim();   Updates a adim entity
*    DBdelete_adim();   Deletes a adim entity
*
*    This file is part of the VARKON Database Library.
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
***********************************************************************/

#include "../include/DB.h"
#include "../include/DBintern.h"

/*!******************************************************/

        DBstatus DBinsert_ldim(
        DBLdim  *ldmpek,
        DBId    *idpek,
        DBptr   *lapek)

/*      Huvudrutin för lagring av längdmått. Post-specifika
 *      data fylls i och posten lagras därefter med ett
 *      anrop till inpost().
 *
 *      In: ldmpek => Pekare till en ldm-structure.
 *          idpek  => Pekare till identitet-structure.
 *          lapek  => Pekare till DBptr-variabel.
 *
 *      Ut: *la     => Logisk adress till ldm-data i GM.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanför virtuellt område.
 *          -2  => IDTAB full.
 *          -3  => Data får inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      14/10/85   Headerdata, J.Kjellander
 *      23/3/92    GMPOSTV1, J.Kjellander
 *      2007-09-01 GMPOSTV2, J.Kjellander
 *
 ******************************************************!*/

  {
/*
***Type specific data.
*/
    ldmpek->hed_ld.type = LDMTYP;   /* Typ = längdmått */
    ldmpek->hed_ld.vers = GMPOSTV2; /* Version */
/*
***Insert.
*/
    return(inpost((GMUNON *)ldmpek,idpek,lapek,sizeof(DBLdim)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_ldim(
        DBLdim  *ldmptr,
        DBCsys  *csyptr,
        DBptr    la)

/*      Read LDIM data.
 *
 *      In:   la     => The LDIM DB-pointer.
 *
 *      Out: *ldmpek => LDIM data.
 *           *csyptr => Csys data if available and
 *                      csyptr != NULL.
 *
 *      Return: Always 0.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      23/3/92    GMPOSTV1, J. Kjellander
 *      2007-09-17 csyptr, J.Kjellander
 *
 ******************************************************!*/

  {
    DBHeader *hedptr;

    hedptr = (DBHeader *)gmgadr(la);

    switch ( GMVERS(hedptr) )
      {
      case GMPOSTV2:
      V3MOME(hedptr,ldmptr,sizeof(DBLdim));
      if ( csyptr != NULL  &&  ldmptr->pcsy_ld > 0 ) DBread_csys(csyptr,NULL,ldmptr->pcsy_ld);
      break;

      case GMPOSTV1:
      V3MOME(hedptr,ldmptr,sizeof(GMLDM1));
      break;

      default:
      V3MOME(hedptr,ldmptr,sizeof(GMLDM0));
      ldmptr->pcsy_ld = DBNULL;
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_ldim(
        DBLdim  *ldmpek,
        DBptr    la)

/*      Skriver över en existerande längdmått-post.
 *
 *      In: ldmpek => Pekare till en ldm-structure.
 *          la     => Måttets adress i GM.
 *
 *      Ut: Inget.
 *
 *      FV: 0
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    DBHeader *hedptr;

    hedptr = (DBHeader *)gmgadr(la);

    switch ( GMVERS(hedptr) )
      {
      case GMPOSTV2:
      updata( (char *)ldmpek, la, sizeof(DBLdim));
      break;

      case GMPOSTV1:
      updata( (char *)ldmpek, la, sizeof(GMLDM1));
      break;

      default:
      updata( (char *)ldmpek, la, sizeof(GMLDM0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_ldim(DBptr la)

/*      Stryker ett längdmått och deallokerar allokerat minne.
 *
 *      In: la => Måttets GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      16/10/85 Koll av referensräknare J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    DBHeader *hedptr;

    hedptr = (DBHeader *)gmgadr(la);

    switch ( GMVERS(hedptr) )
      {
      case GMPOSTV2:
      rldat1(la,sizeof(DBLdim));
      break;

      case GMPOSTV1:
      rldat1(la,sizeof(GMLDM1));
      break;

      default:
      rldat1(la,sizeof(GMLDM0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBinsert_cdim(
        DBCdim  *cdmptr,
        DBId    *idpek,
        DBptr   *lapek)

/*      Huvudrutin för lagring av diametermått. Post-specifika
 *      data fylls i och posten lagras därefter med ett
 *      anrop till inpost().
 *
 *      In: cdmptr => Pekare till en cdm-structure.
 *          idpek  => Pekare till identitet-structure.
 *          lapek  => Pekare till DBptr-variabel.
 *
 *      Ut: *la     => Logisk adress till cdm-data i GM.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanför virtuellt område.
 *          -2  => IDTAB full.
 *          -3  => Data får inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      14/10/85   Headerdata, J.Kjellander
 *      23/3/92    GMPOSTV1, J.Kjellander
 *      2007-09-01 GMPOSTV2, J.Kjellander
 *
 ******************************************************!*/

  {
/*
***Typ-specifika data.
*/
    cdmptr->hed_cd.type = CDMTYP;   /* Typ = diametermått */
    cdmptr->hed_cd.vers = GMPOSTV2; /* Version */
/*
***Lagra.
*/
    return(inpost((GMUNON *)cdmptr,idpek,lapek,sizeof(DBCdim)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_cdim(
        DBCdim  *cdmptr,
        DBCsys  *csyptr,
        DBptr    la)

/*      Read CDIM data.
 *
 *      In:   la     => The LDIM DB-pointer.
 *
 *      Out: *cdmptr => CDIM data.
 *           *csyptr => Csys data if available and
 *                      csyptr != NULL.
 *
 *      Return: Always 0.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      23/3/92  GMPOSTV1, J. Kjellander
 *      2007-09-17 csyptr, J.Kjellander
 *
 ******************************************************!*/

  {
    DBHeader *hedptr;

    hedptr = (DBHeader *)gmgadr(la);

    switch ( GMVERS(hedptr) )
      {
      case GMPOSTV2:
      V3MOME(hedptr,cdmptr,sizeof(DBCdim));
      if ( csyptr != NULL  &&  cdmptr->pcsy_cd > 0 ) DBread_csys(csyptr,NULL,cdmptr->pcsy_cd);
      break;

      case GMPOSTV1:
      V3MOME(hedptr,cdmptr,sizeof(GMCDM1));
      break;

      default:
      V3MOME(hedptr,cdmptr,sizeof(GMCDM0));
      cdmptr->pcsy_cd = DBNULL;
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_cdim(
        DBCdim  *cdmptr,
        DBptr    la)

/*      Skriver över en existerande diametermått-post.
 *
 *      In: cdmptr => Pekare till en cdm-structure.
 *          la     => Måttets adress i GM.
 *
 *      Ut: Inget.
 *
 *      FV: 0
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    DBHeader *hedptr;

    hedptr = (DBHeader *)gmgadr(la);

    switch ( GMVERS(hedptr) )
      {
      case GMPOSTV2:
      updata( (char *)cdmptr, la, sizeof(DBCdim));
      break;

      case GMPOSTV1:
      updata( (char *)cdmptr, la, sizeof(GMCDM1));
      break;

      default:
      updata( (char *)cdmptr, la, sizeof(GMCDM0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_cdim(DBptr la)

/*      Stryker ett diameter-mått och deallokerar
 *      allokerat minne.
 *
 *      In: la => Måttets GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      16/10/85 Koll av referensräknare J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    DBHeader *hedptr;

    hedptr = (DBHeader *)gmgadr(la);

    switch ( GMVERS(hedptr) )
      {
      case GMPOSTV2:
      rldat1(la,sizeof(DBCdim));
      break;

      case GMPOSTV1:
      rldat1(la,sizeof(GMCDM1));
      break;

      default:
      rldat1(la,sizeof(GMCDM0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBinsert_rdim(
        DBRdim  *rdmptr,
        DBId    *idpek,
        DBptr   *lapek)

/*      Huvudrutin för lagring av radiemått. Post-specifika
 *      data fylls i och posten lagras därefter med ett
 *      anrop till inpost().
 *
 *      In: rdmptr => Pekare till en rdm-structure.
 *          idpek  => Pekare till identitet-structure.
 *          lapek  => Pekare till DBptr-variabel.
 *
 *      Ut: *la     => Logisk adress till rdm-data i GM.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanför virtuellt område.
 *          -2  => IDTAB full.
 *          -3  => Data får inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      14/10/85   Headerdata, J.Kjellander
 *      23/3/92    GMPOSTV1, J.Kjellander
 *      2007-09-01 GMPOSTV2, J.Kjellander
 *
 ******************************************************!*/

  {
/*
***Typ-specifika data.
*/
    rdmptr->hed_rd.type = RDMTYP;   /* Typ = radiemått */
    rdmptr->hed_rd.vers = GMPOSTV2; /* Version */
/*
***Lagra.
*/
    return(inpost((GMUNON *)rdmptr,idpek,lapek,sizeof(DBRdim)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_rdim(
        DBRdim  *rdmptr,
        DBCsys  *csyptr,
        DBptr    la)

/*      Read RDIM data.
 *
 *      In:   la     => The RDIM DB-pointer.
 *
 *      Out: *cdmptr => RDIM data.
 *           *csyptr => Csys data if available and
 *                      csyptr != NULL.
 *
 *      Return: Always 0.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      23/3/92  GMPOSTV1, J. Kjellander
 *      2007-09-17 csyptr, J.Kjellander
 *
 ******************************************************!*/

  {
    DBHeader *hedptr;

    hedptr = (DBHeader *)gmgadr(la);

    switch ( GMVERS(hedptr) )
      {
      case GMPOSTV2:
      V3MOME(hedptr,rdmptr,sizeof(DBRdim));
      if ( csyptr != NULL  &&  rdmptr->pcsy_rd > 0 ) DBread_csys(csyptr,NULL,rdmptr->pcsy_rd);
      break;

      case GMPOSTV1:
      V3MOME(hedptr,rdmptr,sizeof(GMRDM1));
      break;

      default:
      V3MOME(hedptr,rdmptr,sizeof(GMRDM0));
      rdmptr->pcsy_rd = DBNULL;
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_rdim(
        DBRdim  *rdmptr,
        DBptr    la)

/*      Skriver över en existerande radiemått-post.
 *
 *      In: rdmptr => Pekare till en rdm-structure.
 *          la     => Måttets adress i GM.
 *
 *      Ut: Inget.
 *
 *      FV: 0
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    DBHeader *hedptr;

    hedptr = (DBHeader *)gmgadr(la);

    switch ( GMVERS(hedptr) )
      {
      case GMPOSTV2:
      updata( (char *)rdmptr, la, sizeof(DBRdim));
      break;

      case GMPOSTV1:
      updata( (char *)rdmptr, la, sizeof(GMRDM1));
      break;

      default:
      updata( (char *)rdmptr, la, sizeof(GMRDM0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_rdim(DBptr la)

/*      Stryker ett radie-mått och deallokerar
 *      allokerat minne.
 *
 *      In: la => Måttets GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      16/10/85 Koll av referensräknare J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    DBHeader *hedptr;

    hedptr = (DBHeader *)gmgadr(la);

    switch ( GMVERS(hedptr) )
      {
      case GMPOSTV2:
      rldat1(la,sizeof(DBRdim));
      break;

      case GMPOSTV1:
      rldat1(la,sizeof(GMRDM1));
      break;

      default:
      rldat1(la,sizeof(GMRDM0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBinsert_adim(
        DBAdim  *admptr,
        DBId    *idpek,
        DBptr   *lapek)

/*      Huvudrutin för lagring av vinkelmått. Post-specifika
 *      data fylls i och posten lagras därefter med ett
 *      anrop till inpost().
 *
 *      In: admptr => Pekare till en adm-structure.
 *          idpek  => Pekare till identitet-structure.
 *          lapek  => Pekare till DBptr-variabel.
 *
 *      Ut: *la     => Logisk adress till adm-data i GM.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanför virtuellt område.
 *          -2  => IDTAB full.
 *          -3  => Data får inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      14/10/85   Headerdata, J.Kjellander
 *      23/3/92    GMPOSTV1, J.Kjellander
 *      2007-09-01 GMPOSTV2, J.Kjellander
 *
 ******************************************************!*/

  {
/*
***Typ-specifika data.
*/
    admptr->hed_ad.type = ADMTYP;   /* Typ = vinkelmått */
    admptr->hed_ad.vers = GMPOSTV2; /* Version */
/*
***Lagra.
*/
    return(inpost((GMUNON *)admptr,idpek,lapek,sizeof(DBAdim)));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_adim(
        DBAdim  *admptr,
        DBCsys  *csyptr,
        DBptr    la)

/*      Read ADIM data.
 *
 *      In:   la     => The ADIM DB-pointer.
 *
 *      Out: *admptr => ADIM data.
 *           *csyptr => Csys data if available and
 *                      csyptr != NULL.
 *
 *      Return: Always 0.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      23/3/92    GMPOSTV1, J. Kjellander
 *      2007-09-24 csyptr, J.Kjellander
 *
 ******************************************************!*/

  {
    DBHeader *hedptr;

    hedptr = (DBHeader *)gmgadr(la);

    switch ( GMVERS(hedptr) )
      {
      case GMPOSTV2:
      V3MOME(hedptr,admptr,sizeof(DBAdim));
      if ( csyptr != NULL  &&  admptr->pcsy_ad > 0 ) DBread_csys(csyptr,NULL,admptr->pcsy_ad);
      break;

      case GMPOSTV1:
      V3MOME(hedptr,admptr,sizeof(GMADM1));
      break;

      default:
      V3MOME(hedptr,admptr,sizeof(GMADM0));
      admptr->pcsy_ad = DBNULL;
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_adim(
        DBAdim  *admptr,
        DBptr    la)

/*      Skriver över en existerande vinkelmått-post.
 *
 *      In: admptr => Pekare till en adm-structure.
 *          la     => Måttets adress i GM.
 *
 *      Ut: Inget.
 *
 *      FV: 0
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    DBHeader *hedptr;

    hedptr = (DBHeader *)gmgadr(la);

    switch ( GMVERS(hedptr) )
      {
      case GMPOSTV2:
      updata( (char *)admptr, la, sizeof(DBAdim));
      break;

      case GMPOSTV1:
      updata( (char *)admptr, la, sizeof(GMADM1));
      break;

      default:
      updata( (char *)admptr, la, sizeof(GMADM0));
      break;
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_adim(DBptr la)

/*      Stryker ett vinkel-mått och deallokerar
 *      allokerat minne.
 *
 *      In: la => Måttets GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 4/8/85 J. Kjellander
 *
 *      16/10/85 Koll av referensräknare J. Kjellander
 *      23/3/92  GMPOSTV1, J. Kjellander
 *
 ******************************************************!*/

  {
    DBHeader *hedptr;

    hedptr = (DBHeader *)gmgadr(la);

    switch ( GMVERS(hedptr) )
      {
      case GMPOSTV2:
      rldat1(la,sizeof(DBAdim));
      break;

      case GMPOSTV1:
      rldat1(la,sizeof(GMADM1));
      break;

      default:
      rldat1(la,sizeof(GMADM0));
      break;
      }

    return(0);
  }

/********************************************************/
