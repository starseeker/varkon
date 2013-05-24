/*!*******************************************************
*
*    exgroup.c
*    =========
*
*    EXgrp();     Create GROUP
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

/*!******************************************************/

       short EXgrp(
       DBId  *id,
       DBId  *ridvek,
       DBGroup *grppek)

/*      Skapa grupp.
 *
 *      In: id     => Pekare till gruppens identitet.
 *          ridvek => Pekare till vektor med referenser.
 *          grppek => Pekare till group-structure.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          EX1402 => Refererad storhet finns ej.
 *          EX1482 => Kan ej lagra grupp i GM.
 *          EX1492 => Storheten redan med i max antal grupper.
 *
 *      (C)microform ab 5/8/85 J. Kjellander
 *
 *      14/10/85 Header-data. J. Kjellander
 *
 ******************************************************!*/

  {
    DBint   i;
    DBptr   lavek[GMMXGP];
    DBptr   la;
    DBetype typ;

/*
***Hämta la för samtliga refererade storheter.
*/
    for ( i=0; i<grppek->nmbr_gp; ++i )
      {
      if ( DBget_pointer('I',&ridvek[i],&lavek[i],&typ) < 0 ) 
        return(erpush("EX1402",""));
      }
/*
***Lagra grupp-posten i GM.
*/
    if ( DBinsert_group(grppek,lavek,id,&la) < 0 )
      return(erpush("EX1482",""));
/*
***Lagra grupp-pekare i de refererade storheterna.
*/
    for ( i=0; i<grppek->nmbr_gp; ++i )
      {
      if ( gmagpp(lavek[i],la) < 0 ) 
        erpush("EX1492","");
      }
    
    return(0);
  }
  
/********************************************************/
