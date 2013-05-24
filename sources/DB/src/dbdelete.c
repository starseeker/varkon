/**********************************************************************
*
*    dbdelete.c
*    ==========
*
*
*    This file includes the following public functions:
*
*    DBdelete_entity();   Generic delete entity
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

        DBstatus DBdelete_entity(DBptr la, DBetype typ)

/*      Deletes an entity.
 *
 *      In: la   => Storhetens LA.
 *          typ  => Typ av storhet.
 *
 *      Ut: Inget.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 24/3/92 J. Kjellander
 *
 *      26/11/92 Ytor, J. Kjellander
 *      16/5/94  TRFTYP, J. Kjellander
 *      1997-12-04 BPLTYP, J.Kjellander
 *      1998-12-22 Tagit bort SYMTYP, J.Kjellander
 *      1998-02-03 actprt, J.Kjellander
 *      2004-07-10 Mesh, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
    GMRECH  hed;
    GMPRT   part;
    DBptr   la_tmp;
    DBint   i;

/*
***Don't delete the currently active part.
*/
   if ( la == actprt ) return(erpush("GM0202",""));
/*
***Unlink.
*/
    gmunlk(la);
/*
***Delete.
*/
    switch (typ)
      {
      case TRFTYP: DBdelete_tform(la); break;

      case POITYP: DBdelete_point(la); break;

      case LINTYP: DBdelete_line(la); break;

      case ARCTYP: DBdelete_arc(la); break;

      case CURTYP: DBdelete_curve(la); break;

      case SURTYP: DBdelete_surface(la); break;

      case CSYTYP: DBdelete_csys(la); break;

      case TXTTYP: DBdelete_text(la); break;

      case LDMTYP: DBdelete_ldim(la); break;

      case CDMTYP: DBdelete_cdim(la); break;

      case RDMTYP: DBdelete_rdim(la); break;

      case ADMTYP: DBdelete_adim(la); break;

      case XHTTYP: DBdelete_xhatch(la); break;

      case GRPTYP: DBdelete_group(la); break;

      case BPLTYP: DBdelete_bplane(la); break;

      case MSHTYP: DBdelete_mesh(la); break;
/*
***Part, läs posten och ta bort alla storheter i partens
***ID-tabell, inklusive andra parts och alla instanser.
***Ta slutligen bort part-posten själv med dess ID-tabell.
*/
      case PRTTYP:
      DBread_part(&part,la);

      for ( i=0; i < part.its_pt; ++i )
        {
        la_tmp = gmrdid(part.itp_pt,(DBseqnum)i);

        if ( la_tmp >= 0 )
          {
          do
            {
            DBread_header(&hed,la_tmp);
            DBdelete_entity(la_tmp,hed.type);
            la_tmp = hed.n_ptr;
            }
          while (la_tmp != DBNULL);
          }
        }
       DBdelete_part(la);
       break;
       }
    return(0);
  }

/********************************************************/
