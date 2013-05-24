/**********************************************************************
*
*    dbgroup.c
*    =========
*
*
*    This file includes the following public functions:
*
*    DBinsert_group();         Inserts a group entity
*    DBread_group();           Reads a group entity
*    DBupdate_group();         Updates a group entity
*    DBdelete_group();         Deletes a group entity
*    DBdelete_group_member();  Deletes a member of a group entity
*    DBadd_group_member();   Adds a member to a group entity
*
*
*    This file is part of the VARKON Database Library.
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
***********************************************************************/

#include "../include/DB.h"
#include "../include/DBintern.h"

/*!******************************************************/

        DBstatus DBinsert_group(
        GMGRP   *grppek,
        DBptr    lavek[],
        DBId    *idpek,
        DBptr   *lapek)

/*      Huvudrutin för lagring av grupp. Post-specifika
 *      data fylls i och posten lagras därefter med ett
 *      anrop till inpost().
 *
 *      In: grppek => Pekare till en grupp-structure.
 *          lavek  => Vektor med la för medlemmar.
 *          idpek  => Pekare till identitet-structure.
 *          lapek  => Pekare till DBptr-variabel.
 *
 *      Ut: *la    => Logisk adress till grupp-post i GM.
 *
 *      FV:  0  => Ok.
 *          -1  => ID utanför virtuellt område.
 *          -2  => IDTAB full.
 *          -3  => Data får inte plats.
 *          -4  => Storhet med detta ID finns redan.
 *
 *      (C)microform ab 5/8/85 J. Kjellander
 *
 *      14/10/85 Headerdata, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr  la_mem;

/*
***Lagra först lavek.
*/
      if ( wrdat1( (char *)lavek, &la_mem,
           grppek->nmbr_gp*sizeof(DBptr)) < 0 ) return(-3);
/*
***Typ-specifika data.
*/
    grppek->hed_gp.type = GRPTYP;   /* Typ = grupp */
    grppek->hed_gp.vers = GMPOSTV0; /* Version */
    grppek->mptr_gp = la_mem;       /* Pekare till medlemmar */
/*
***Lagra själva grupp-posten. 
*/
    return(inpost((GMUNON *)grppek,idpek,lapek,sizeof(GMGRP) ));
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_group(
        GMGRP   *grppek,
        DBptr    lavek[],
        DBptr    la)

/*      Huvudrutin för läsning av grupp.
 *
 *      In: grppek => Pekare till en grupp-structure.
 *          lavek  => Array för medlemmars la.
 *          la     => Gruppens adress i GM.
 *
 *      Ut: *grppek => Grupp-post.
 *          *lavek  => Medlemmars la.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 5/8/85 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Läs själva grupp-posten.
*/
    rddat1( (char *)grppek, la, sizeof(GMGRP));
/*
***Läs lavek.
*/
    rddat1( (char *)lavek, grppek->mptr_gp,
             grppek->nmbr_gp*sizeof(DBptr));

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBupdate_group(
        GMGRP   *grppek,
        DBptr    lavek[],
        DBptr    la)

/*      Skriver över en existerande grupp-post.
 *
 *      In: grppek => Pekare till en grupp-structure.
 *          lavek  => Pekare till medlemmar
 *          la     => Gruppens adress i GM.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 30/8/85 J. Kjellander
 *
 ******************************************************!*/

  {
    updata( (char *)grppek, la, sizeof(GMGRP));
    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_group(DBptr la)

/*      Stryker en grup-post och deallokerar allokerat minne.
 *
 *      In: la => Gruppens GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *
 *      (C)microform ab 5/8/85 J. Kjellander
 *
 *      16/10/85 Koll av referensräknare J. Kjellander
 *      1/10/92  Noll medlemmar, J. Kjellander
 *
 ******************************************************!*/

  {
    GMGRP  grp;
    GMRECH header;
    DBptr  lavek[GMMXGP+1];
    DBint  i;

/*
***Läs gruppen.
*/
    DBread_group(&grp,lavek,la);
/*
***Ta bort grupp-pekare ur alla gruppens medlemmar.
*/
    for ( i=0; i<grp.nmbr_gp; ++i)
      {
      DBread_header(&header,lavek[i]);
      if ( header.g_ptr[0] == la ) header.g_ptr[0] = DBNULL;
      if ( header.g_ptr[1] == la ) header.g_ptr[1] = DBNULL;
      if ( header.g_ptr[2] == la ) header.g_ptr[2] = DBNULL;
      DBupdate_header(&header,lavek[i]);
      }
/*
***Stryk grupp-posten.
*/
    rldat1(la,sizeof(GMGRP));
/*
***Stryk lavek om den finns.
*/
    if ( grp.nmbr_gp > 0 )
      rldat1(grp.mptr_gp,grp.nmbr_gp*sizeof(DBptr));

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_group_member(
        DBptr grpla,
        DBptr memla)

/*      Tar bort en medlem ur en grupp. Gruppens
 *      lavek stryks. En ny genereras och lagras.
 *      Själva grupposten uppdateras. Om resultatet
 *      blir att inga medlemmar finns kvar sätts
 *      medlemspekaren till DBNULL. Detta inträffar
 *      tex. om man tar bort en part som innehåller
 *      en grupp som har andra part-medlemmar som
 *      medlemmar. Gruppens medlemmar kommer då att
 *      tas bort en och en tills inga medlemmar finns
 *      kvar när själva gruppen ska tas bort.
 *
 *      In: grpla => Gruppens GM-adress.
 *          memla => Medlemmens GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *          -3  => Data får inte plats.
 *
 *      (C)microform ab 30/8/85 J. Kjellander
 *
 *      1/10/92  Noll medlemmar, J. Kjellander
 *
 ******************************************************!*/

  {
    GMGRP  grp;
    DBptr  olavek[GMMXGP+1];   /* Gamla lavek */
    DBptr  nlavek[GMMXGP+1];   /* Nya lavek */
    DBint  i,j;
    DBptr  la_mem;

/*
***Läs gruppen.
*/
    DBread_group(&grp,olavek,grpla);
/*
***Generera ny lavek.
*/
    j = 0;
    for ( i=0; i<grp.nmbr_gp; ++i )
       if ( olavek[i] != memla ) nlavek[j++] = olavek[i];
/*
***Stryk gamla lavek.
*/
    rldat1(grp.mptr_gp,grp.nmbr_gp*sizeof(DBptr));
/*
***Uppdatera antal medlemmar.
*/
    grp.nmbr_gp = j;
/*
***Om det finns medlemmar kvar, lagra ny lavek
***och uppdatera medlemms-pekaren.
*/
    if ( grp.nmbr_gp > 0 )
      {
      if ( wrdat1( (char *)nlavek, &la_mem,
         grp.nmbr_gp*sizeof(DBptr)) < 0 ) return(-3);
      grp.mptr_gp = la_mem;
      }
/*
***Inga medlemmar kvar, sätt medlems-pekaren = DBNULL.
*/
    else grp.mptr_gp = DBNULL;
/*
***Skriv tillbaks den nya grupp-posten.
*/
    updata( (char *) &grp,grpla,sizeof(GMGRP));

    return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBadd_group_member(
        DBptr grpla,
        DBptr memla)

/*      Lägger till en medlem till en grupp. Gruppens
 *      lavek stryks. En ny genereras och lagras.
 *      Själva grupposten uppdateras.
 *
 *      In: grpla => Gruppens GM-adress.
 *          memla => Medlemmens GM-adress.
 *
 *      Ut: Inget.
 *
 *      FV:  0  => Ok.
 *          -2  => Gruppen har redan max antal medlemmar.
 *          -3  => Data får inte plats.
 *
 *      (C)microform ab 1/1/86 J. Kjellander
 *
 ******************************************************!*/

  {
    GMGRP  grp;
    DBptr  lavek[GMMXGP+1];
    DBptr  la_mem;

/*
***Läs gruppen.
*/
    DBread_group(&grp,lavek,grpla);
    if ( grp.nmbr_gp == GMMXGP ) return(-2);
/*
***Stryk gamla lavek.
*/
    rldat1(grp.mptr_gp,grp.nmbr_gp*sizeof(DBptr));
/*
***Uppdatera lavek.
*/
    lavek[grp.nmbr_gp] = memla;
    ++grp.nmbr_gp;
/*
***Lagra ny lavek.
*/
    if ( wrdat1( (char *)lavek, &la_mem,
       grp.nmbr_gp*sizeof(DBptr)) < 0 ) return(-3);
/*
***Uppdatera pekare till medlemmar.
*/
    grp.mptr_gp = la_mem;
/*
***Skriv tillbaks den nya grupp-posten.
*/
    updata( (char *) &grp,grpla,sizeof(GMGRP));

    return(0);
  }

/********************************************************/
