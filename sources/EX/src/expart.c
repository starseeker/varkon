/*!*******************************************************
*
*    expart.c
*    ========
*
*    EXoppt();    Create/open part
*    EXclpt();    Close part
*    EXmsini();   Reset modstk
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
*    (C)Microform AB 1984-2001, Johan Kjellander, johan@microform.se
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../DB/include/DBintern.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"

#include "../include/EX.h"

extern short   modtyp;
extern DBTmat  lklsys;
extern DBTmat  lklsyi;
extern DBTmat *lsyspk;
extern DBptr   lsysla;
extern V2NAPA  defnap;


/*
***Current module call stack and pointer.
***mspek=modstk => Main module active.
*/
char modstk[MODNIV * (3*sizeof(DBTmat)   +
                      2*sizeof(DBTmat *) +
                      2*sizeof(DBptr)    +
                      sizeof(short))];
char *mspek;

/*
***Current module (global) coordinate system.
*/
DBTmat  modsys;      /* Current module matrix */
DBTmat *msyspk;      /* C-ptr to current module matrix. NULL => BASIC */
DBptr   msysla;      /* DB pointer to current module coordinate system */

/*!******************************************************/

       short EXoppt(
       DBId    *id,
       DBId    *refid,
       DBPart   *prtpek,
       DBPdat  *datpek,
       V2PATLOG typarr[],
       char     valarr[])

/*      Create/open new part in DB.
 *
 *      In: id     => Pekare till partens lokala identitet.
 *          refid  => Pekare till id för koordinatsystem.
 *          prtpek => Pekare till part-structure.
 *          datpek => Pekare till part-data structure.
 *          typarr => Pekare till array med typ-info strukturer.
 *          valarr => Pekare till array med värden för
 *                    ev. parametrar.
 *
 *      Ut: Inget.
 *
 *      Felkoder: 0 => Ok.
 *           EX1762 => Refererat koordinatsystem finns ej i GM.
 *           EX1422 => Kan ej lagra part i GM.
 *           EX1562 => Koordinatsystem saknas
 *           EX1572 => Geometrimodul anropar ritningsmodul
 *           EX1752 => Refererad storhet ej koordinatsystem
 *
 *      (C)microform ab 26/4/85 J. Kjellander
 *
 *      14/10/85 Header-data. J. Kjellander
 *      7/11/85  BASIC, J. Kjellander
 *      21/11/85 GLOBAL-bug, J. Kjellander
 *      1/2/86   Koll av refid, J. Kjellander
 *      23/2/86  Nytt anrop, J. Kjellander
 *      4/3/86   EX1572, J. Kjellander
 *      16/4/86  Bytt geo607 mot 612, J. Kjellander
 *      25/11/86 Koll av typ, J. Kjellander
 *      25/11/86 tmspek, J. Kjellander
 *      7/9/87   Flyttat hant. av modtyp t. PM, J. Kjellander
 *      20/3/92  Attribut, J. Kjellander
 *      6/2/95   msysla, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBCsys   crdsys;
    DBptr   refla;
    DBetype   reftyp;
    char   *tmspek;

/*
***Stacka nu aktiva modul-data på modstk. Använd en
***temporär stackpekare tmspek och uppdatera mspek
***först när allt visat sig gå bra.
*/
    tmspek = mspek;

    V3MOME(&modsys,tmspek,sizeof(DBTmat));    /* modsys */
    tmspek += sizeof(DBTmat);

    V3MOME(&lklsys,tmspek,sizeof(DBTmat));    /* lklsys */
    tmspek += sizeof(DBTmat);

    V3MOME(&lklsyi,tmspek,sizeof(DBTmat));    /* lklsyi */
    tmspek += sizeof(DBTmat);

    V3MOME(&msyspk,tmspek,sizeof(DBTmat *));  /* msyspk */
    tmspek += sizeof(DBTmat *);

    V3MOME(&msysla,tmspek,sizeof(DBptr));     /* msysla */
    tmspek += sizeof(DBptr);

    V3MOME(&lsyspk,tmspek,sizeof(DBTmat *));  /* lsyspk */
    tmspek += sizeof(DBTmat *);

    V3MOME(&lsysla,tmspek,sizeof(DBptr));     /* lsysla */
    tmspek += sizeof(DBptr);
/*
***Test av modtyp.
*
    if ( modtyp == 3 && datpek->mtyp_pd == 2 )
        return(erpush("EX1572",prtpek->name_pt));
*
***Om den anropade modulen har attributet "LOCAL",
***hämta det refererade koordinatsystemet från GM och
***lagra i modsys. Inget lokalt system aktivt när
***modulen startar, alltså är lsyspk = msyspk.
*/
    switch ( datpek->matt_pd )
      {
      case LOCAL:
      if ( refid == NULL )
        return(erpush("EX1562",prtpek->name_pt));
      if ( DBget_pointer('I',refid,&refla,&reftyp) < 0 ) 
        return(erpush("EX1762",prtpek->name_pt));
      if ( reftyp != CSYTYP ) return(erpush("EX1752",prtpek->name_pt));
      DBread_csys(&crdsys,&modsys,refla);
      msyspk = &modsys;
      msysla = refla;
      lsyspk = msyspk;
      lsysla = msysla;
      GEtform_inv(&modsys,&lklsyi);
      datpek->csp_pd = refla;
      break;
/*
***Om den anropade modulen har attributet "GLOBAL"
***skall den ärva anroparens globala system.
*/
      case GLOBAL:
      EXmogl();
      datpek->csp_pd = DBNULL;
      break;
/*
***Om den anropade modulen har attributet "BASIC"
***skall ingen transformation göras.
*/
      case BASIC:
      lsyspk = NULL;
      lsysla = DBNULL;
      datpek->csp_pd = DBNULL;
      break;
      }
/*
***Lagra part-posten i GM.
*/
    if ( DBinsert_part(prtpek,datpek,typarr,valarr,id,&defnap,&la) < 0 )
      return(erpush("EX1422",""));
/*
***Uppdatera mspek.
*/
    mspek = tmspek;

    return(0);

  }
  
/********************************************************/
/*!******************************************************/

       short EXclpt()

/*      Close currently open part and restack previous
 *      part context.
 *
 *      FV:      0 => Ok.
 *
 *      (C)microform ab 26/4/85 J. Kjellander
 *
 *      7/8/87  modtyp, J. Kjellander
 *      6/2/95  msysla, J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Återstacka anropande modulens transformationer från modstk.
*/
    mspek -= sizeof(DBptr);
    V3MOME(mspek,&lsysla,sizeof(DBptr));     /* lsysla */

    mspek -= sizeof(DBTmat *);
    V3MOME(mspek,&lsyspk,sizeof(DBTmat *));  /* lsyspk */

    mspek -= sizeof(DBptr);
    V3MOME(mspek,&msysla,sizeof(DBptr));     /* msysla */

    mspek -= sizeof(DBTmat *);
    V3MOME(mspek,&msyspk,sizeof(DBTmat *));  /* msyspk */

    mspek -= sizeof(DBTmat);
    V3MOME(mspek,&lklsyi,sizeof(DBTmat));    /* lklsyi */

    mspek -= sizeof(DBTmat);
    V3MOME(mspek,&lklsys,sizeof(DBTmat));    /* lklsys */

    mspek -= sizeof(DBTmat);
    V3MOME(mspek,&modsys,sizeof(DBTmat));    /* modsys */
/*
***Stäng parten i GM.
*/
    DBclose_part();

    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       void EXmsini()

/*      Initializes (resets) the modstack pointer
 *      to its default value.
 *
 *      (C) 2001-03-23 J. Kjellander
 *
 ******************************************************!*/

  {
    mspek = modstk;
  }
  
/********************************************************/
