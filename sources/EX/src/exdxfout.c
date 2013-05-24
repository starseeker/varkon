/*!******************************************************
*
*    exdxfout.c
*    =========
*
*    This file includes the following public functions:
*
*    EXall_to_dxf();    Exports entire DB in DXF format
*    EXpoint_to_dxf();  Writes point entity
*    EXline_to_dxf();   Writes line entity
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
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
*********************************************************/

/*
***This file includes DXF export routines. There
***is one routine for each type of entity and
***there is aslo EXall_to_dxf() which writes
***all entities in the DB to a complete DXF file.
*/

/*
***Include some headerfiles.
*/
#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/EX.h"

static int write_level(),write_pen();

/*!******************************************************/

        int     EXall_to_dxf(
        char   *dxffile,
        DBfloat px,
        DBfloat py)

/*      Exports entire DB to file in DXF format.
 *
 *      In:
 *          dxffile = Name and path of DXF file
 *          px,py   = Base position
 *
 *      Return:  0 = Ok.
 *              -1 = Can't open file.
 *              -2 = DB error.
 *
 *      (C)microform ab 1999-02-08 J. Kjellander
 *
 ******************************************************!*/

  {
   FILE    *df;
   DBId     id;
   DBptr    eptr;
   DBHeader header;
   DBAny    ent;

/*
***Open the file.
*/
   if ( (df=fopen(dxffile,"w")) == NULL ) return(-1);
/*
***Start entities section.
*/
   fprintf(df,"  0\nSECTION\n  2\nENTITIES\n");
/*
***Start traversal at top of DB.
*/
   id.seq_val = 0;
   if ( DBset_root_id(&id) < 0 ) return(-2);
/*
***Get a pointer to the next entity.
*/
loop:
   DBget_next_ptr(0,ALLTYP,NULL,NULL,&eptr);
/*
***If the pointer returned != DBNULL it's a valid pointer otherwise
***we are at the end of the DB. Get the entity header and check
***what type it is.
*/
   if ( eptr != DBNULL )
     {
     DBread_header(&header,eptr);
/*
***Read entity data and switch to the corresponding output routine.
*/
     switch ( header.type )
       {
       case POITYP:
       DBread_point(&ent.poi_un,eptr);
       EXpoint_to_dxf(&ent.poi_un,df,px,py);
       break;

       case LINTYP:
       DBread_line(&ent.lin_un,eptr);
       EXline_to_dxf(&ent.lin_un,df,px,py);
       break;
       }
/*
***Next entity.
*/
     goto loop;
     }
/*
***End entities section.
*/
   fprintf(df,"  0\nENDSEC\n  0\nEOF\n");
/*
***Close the file.
*/
   fclose(df);
/*
***End.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        int      EXpoint_to_dxf(point,df,tx,ty)
        DBPoint *point;
        FILE    *df;
        double   tx,ty;

/*      Writes POINT entity to DXF file.
 *
 *      In:
 *         point = Ptr to point data
 *         df    = Ptr to open file
 *         tx,ty = Base position
 *
 *      (C)microform ab 1999-02-08 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Write point data.
*/
   fprintf(df,"  0\nPOINT\n");
   write_level(&point->hed_p,df);
   write_pen(&point->hed_p,df);
   fprintf(df," 10\n%g\n",point->crd_p.x_gm - tx);
   fprintf(df," 20\n%g\n",point->crd_p.y_gm - ty);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        int      EXline_to_dxf(line,df,tx,ty)
        DBLine  *line;
        FILE    *df;
        double   tx,ty;

/*      Writes LINE entity to DXF file.
 *
 *      In:
 *         line  = Ptr to line data
 *         df    = Ptr to open file
 *         tx,ty = Base position
 *
 *      (C)microform ab 1999-02-08 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Write line data.
*/
   fprintf(df,"  0\nLINE\n");
   write_level(&line->hed_l,df);
   write_pen(&line->hed_l,df);
   fprintf(df," 10\n%g\n",line->crd1_l.x_gm - tx);
   fprintf(df," 20\n%g\n",line->crd1_l.y_gm - ty);
   fprintf(df," 11\n%g\n",line->crd2_l.x_gm - tx);
   fprintf(df," 21\n%g\n",line->crd2_l.y_gm - ty);

   return(0);
  }

/********************************************************/
/*!******************************************************/

 static int       write_level(header,df)
        DBHeader *header;
        FILE     *df;

/*      Writes level data.
 *
 *      In:
 *         header = Ptr to entity header
 *         df     = Ptr to open file
 *
 *      (C)microform ab 1999-02-08 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Write level data.
*/
   fprintf(df,"  8\n%d\n",(int)header->level);

   return(0);
  }

/********************************************************/
/*!******************************************************/

 static int       write_pen(header,df)
        DBHeader *header;
        FILE     *df;

/*      Writes pen data.
 *
 *      In:
 *         header = Ptr to entity header
 *         df     = Ptr to open file
 *
 *      (C)microform ab 1999-02-08 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Write pen data.
*/
   fprintf(df," 62\n%d\n",(int)header->pen);

   return(0);
  }

/********************************************************/
