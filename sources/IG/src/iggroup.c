/*!******************************************************************/
/*  iggroup.c                                                       */
/*  =========                                                       */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGgrp();     Genererate group.. statement                       */
/*  IGchgr();    Change group by name                               */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://varkon.sourceforge.net                             */
/*                                                                  */
/*  This library is free software; you can redistribute it and/or   */
/*  modify it under the terms of the GNU Library General Public     */
/*  License as published by the Free Software Foundation; either    */
/*  version 2 of the License, or (at your option) any later         */
/*  version.                                                        */
/*                                                                  */
/*  This library is distributed in the hope that it will be         */
/*  useful, but WITHOUT ANY WARRANTY; without even the implied      */
/*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR         */
/*  PURPOSE.  See the GNU Library General Public License for more   */
/*  details.                                                        */
/*                                                                  */
/*  You should have received a copy of the GNU Library General      */
/*  Public License along with this library; if not, write to the    */
/*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge,        */
/*  MA 02139, USA.                                                  */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/IG.h"
#include "../../WP/include/WP.h"

/*!******************************************************/

       short IGgrp()

/*      Huvudrutin för group(#id,name,ref1,ref2,,,,)
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5022 = Kan ej skapa GROUP sats
 *
 *      (C)microform ab 5/8/85 J. Kjellander
 *
 *      6/9/85   Anrop till IGcges(), R. Svedin
 *      31/10/85 Ände och sida, J. Kjellander
 *      16/11/85 t-sträng, J. Kjellander
 *      16/11/85 Sträng sist, J. Kjellander
 *      4/12/85  Tagit bort loop, J. Kjellander
 *      6/3/86   Defaultsträng, J. Kjellander
 *      20/3/86  Anrop till pmtcon, B. Doverud
 *      24/3/86  Felutgångar B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *
 ******************************************************!*/

  {
    short   i,status,nref;
    pm_ptr  exnpt[GMMXGP];
    pm_ptr  retla,dummy,exnpt1;
    DBetype   typ;
    bool    end,right;
    char    istr[V3STRLEN+1];

/*
***Gruppens medlemmar.
*/
    nref = 0;

    while ( nref < GMMXGP )
      {
      typ = ALLTYP;
      if ( (status=IGcref(268,&typ,&exnpt[nref],&end,&right)) == REJECT) break;
      if (status == GOMAIN ) goto exit;
      ++nref;
      }
/*
***Gruppens namn.
*/
    if ( (status=IGcstr(318,"",istr,&exnpt1)) < 0 ) goto exit;
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
/*
***Länka ihop parameterlistan.
*/
    for ( i=0; i<nref; ++i )
      pmtcon(exnpt[i],retla,&retla,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( IGcges("GROUP",retla) < 0 ) goto error;

exit:
    WPerhg();
    return(status);
/*
***Felutgångar.
*/ 
error:
    erpush("IG5023","");
    errmes();
    goto exit;
  }

/********************************************************/
/*!******************************************************/

        short IGchgr()

/*      Varkonfunktion för att ändra grupp via namn.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IG3512 = Storheten refereras av andra storheter.
 *                IG3702 = Otillåtet pennummer.
 *                IG3542 = Gruppen finns ej.
 *                IG2242 = Syntaxfel i id-sträng.
 *                IG3762 = Angiven identitet finns ej.
 *                IG3552 = Gruppen har ingen medlem med angiv.identitet.
 *                IG5162 = Storheten redan utpekad.
 *                IG3772 = Storheten är med i max. antal grupper.
 *                IG3782 = Gruppen har max antal medlemmar.
 *                IG3792 = Data får ej plats.
 *
 *      (C)microform ab 11/11-85 R. Svedin
 *
 *      30/12/85 Lägg till och Ta bort medlem.  R. Svedin
 *      8/12/86  Ändrade menynummer, R. Svedin
 *
 ******************************************************!*/

  {
    DBptr   lavek[GMMXGP];
    DBptr   la;
    DBptr   mbrla;
    DBetype typ;
    char    str[JNLGTH+1];
    char    grpnam[JNLGTH+1];
    short   alt,i,ngrp,status;
    int     value;
    bool    end,right;
    DBId    id[MXINIV];
    DBId    idvek[MXINIV];
    DBGroup   grupp;
    DBHeader  rhed;
    
/*
***Läs in gruppnamn.
*/
    status = IGssip(IGgtts(318),IGgtts(267),grpnam,"",JNLGTH);
    IGrsma();
    if ( status < 0 ) return(status);

start:
    ngrp = 0;

/*
***Skriv meny och utför enl. alternativ.
*/
    IGexfu(134,&alt);
    if ( alt < 0 ) return(alt);

    switch(alt)
      {
/*
***Nivå.
*/
      case 1:
      IGptma(227,IG_INP);
      status = IGsiip(IGgtts(319),&value);
      IGrsma();
      if ( status < 0 ) break;
      if ( value < 0 || value > WP_NIVANT-1 ) goto error1;
/* 
***Hämta LA och typ för huvud_parten. 
*/
      DBget_pointer('F',id,&la,&typ);        

      while ( DBget_pointer('N',id,&la,&typ) == 0 )
        {
        if ( typ == GRPTYP )
          {
          DBread_group(&grupp,lavek,la);
          if ( strcmp(grpnam,grupp.name_gp) == 0 )
/*
***Uppdatera grupp-posten.
*/
            {
            grupp.hed_gp.level = value;
            DBupdate_group(&grupp,lavek,la);
            ++ngrp;
            }
          }
        }
      if ( ngrp == 0) goto error3;
      break;
/*
***Penna.
*/
      case 2:
      IGptma(16,IG_INP);
      status = IGsiip(IGgtts(319),&value);
      IGrsma();
      if ( status < 0 ) break;
      if ( value < 0 || value > 32768 ) goto error2;
/* 
***Hämta LA och typ för huvud_parten. 
*/
      DBget_pointer('F',id,&la,&typ);        

      while ( DBget_pointer('N',id,&la,&typ) == 0 )
        {
        if ( typ == GRPTYP )
          {
          DBread_group(&grupp,lavek,la);
          if ( strcmp(grpnam,grupp.name_gp) == 0 )
/*
***Uppdatera grupp-posten.
*/
            {
            grupp.hed_gp.pen = value;
            DBupdate_group(&grupp,lavek,la);
            ++ngrp;
            }
          }
        }
      if ( ngrp == 0) goto error3;
      break;
/*
***Byt namn.
*/
      case 3:
      status = IGssip(IGgtts(135),IGgtts(267),str,"",JNLGTH);
      if ( status < 0 ) break;
/*
***Hämta LA och typ för huvud_parten. 
*/
      DBget_pointer('F',id,&la,&typ);        

      while ( DBget_pointer('N',id,&la,&typ) == 0 )
        {
        if ( typ == GRPTYP )
          {
          DBread_group(&grupp,lavek,la);
          if ( strcmp(grpnam,grupp.name_gp) == 0 )
/*
***Uppdatera grupp-posten.
*/
            {
            strcpy(grupp.name_gp,str);
            DBupdate_group(&grupp,lavek,la);
            ++ngrp;
            }
          }
        }

      if ( ngrp == 0) goto error3;
      break;
/*
***Ta bort medlem.
*/
      case 4:
      IGptma(268,IG_INP);
      typ = ALLTYP;
      status = IGgsid(idvek,&typ,&end,&right,(short)0);
      IGrsma();
      if ( status < 0 ) break;
      DBget_pointer('I',idvek,&mbrla,&typ);
/* 
***Hämta först LA och typ för huvud_parten 
***sök sedan vidare efter angiven grupp.
*/
      DBget_pointer('F',id,&la,&typ);        

      while ( DBget_pointer('N',id,&la,&typ) == 0 )
        {
        if ( typ == GRPTYP )
          {
          DBread_group(&grupp,lavek,la);
          if ( strcmp(grpnam,grupp.name_gp) == 0 )
           {
/*
***Sök efter angiven medlem i gruppen.
*/
           for ( i=0; i<grupp.nmbr_gp; ++i)
             {
             if ( lavek[i] == mbrla ) break;
             }

           if ( i+1 > grupp.nmbr_gp ) goto error6;
/*
***Ta bort medlemmen ur grupp-posten.
*/
           if ( DBdelete_group_member(la,mbrla) < 0 ) goto error9;
/*
***Ta bort grupp-pekaren i fd. medlemmen. 
*/
           DBread_header(&rhed,mbrla);

           if ( rhed.g_ptr[0] == la ) rhed.g_ptr[0] = DBNULL;
           else if ( rhed.g_ptr[1] == la ) rhed.g_ptr[1] = DBNULL;
           else if ( rhed.g_ptr[2] == la ) rhed.g_ptr[2] = DBNULL;

           DBupdate_header(&rhed,mbrla);
           ++ ngrp;
           }
          }
        }

      if ( ngrp == 0) goto error3;
      break;
/*
***Lägg till medlem.
*/
      case 5:
      IGptma(268,IG_INP);
      typ = ALLTYP;
      status = IGgsid(idvek,&typ,&end,&right,(short)0);
      IGrsma();
      if ( status < 0 ) break;
      DBget_pointer('I',idvek,&mbrla,&typ);
/* 
***Hämta först LA och typ för huvud_parten 
***sök sedan vidare efter angiven grupp.
*/
      DBget_pointer('F',id,&la,&typ);        

      while ( DBget_pointer('N',id,&la,&typ) == 0 )
        {
        if ( typ == GRPTYP )
          {
          DBread_group(&grupp,lavek,la);
          if ( strcmp(grpnam,grupp.name_gp) == 0 )
           {
/*
***Kolla om angiven medlem redan är med i gruppen.
*/
           for ( i=0; i<grupp.nmbr_gp; ++i)
             {
             if ( lavek[i] == mbrla ) goto error7;
             }
/*
***Kolla så att den inte är med i max antal grupper.
*/
           DBread_header(&rhed,mbrla);

           if ( rhed.g_ptr[0] == DBNULL ) rhed.g_ptr[0] = la;
           else if ( rhed.g_ptr[1] == DBNULL ) rhed.g_ptr[1] = la;
           else if ( rhed.g_ptr[2] == DBNULL ) rhed.g_ptr[2] = la;

           else goto error8;

           DBupdate_header(&rhed,mbrla);
/*
***Lägg till ny medlem till grupp-posten.
*/
           status = DBadd_group_member(la,mbrla);
           if ( status == -3 ) goto error9;
           if ( status == -2 ) goto eror10;

           ++ ngrp;
           }
          }
        }

      if ( ngrp == 0) goto error3;
      break;
      }

    WPerhg();
    if ( status == REJECT ) goto start;
    if ( status == GOMAIN ) return(GOMAIN);
/*
***Om igen.
*/
    goto start;
/*
***Felhantering.
*/
error1:
   erpush("IG3212","");
   errmes();
   goto start;

error2:
   erpush("IG3702","");
   errmes();
   goto start;

error3:
   erpush("IG3542","");
   errmes();
   goto start;

error6:
   erpush("IG3552","");
   errmes();
   goto start;

error7:
   erpush("IG5162","");
   errmes();
   goto start;

error8:
   erpush("IG3772","");
   errmes();
   goto start;

error9:
   erpush("IG3782","");
   errmes();
   goto start;

eror10:
   erpush("IG3792","");
   errmes();
   goto start;

  }

/********************************************************/
