/**********************************************************************
*
*    DBdemo3.c
*    ========
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

/*
***This program demonstrates a simple DB traversal. It
***scans through a DB file and prints it's contents to
***standard output as ASCII text. The name of the DB file
***should be supplied on the commandline. To run DBdemo3
***you must first create a DB file. You can use the file
***created by DBdemo1 if you want to or any other DB file
***You can also use DB files created with Varkon.
*/

/*
***First of all include some standard C header files.
*/
#include <stdio.h>

/*
***Any application calling DB or using the DB datastructures
***must include "DB.h". (DB.h also includes common.h)
***We assume they reside in "../include". This may
***be changed later.
*/
#include "../include/DB.h"

/*
***The code begins here.
*/
int main(argc,argv)
    int argc;
    char *argv[];

{
   DBstatus  errstat;     /* Local variable for error checking */
   DBId      id;          /* The root ID */
   DBptr     eptr;        /* A DB entity pointer */
   DBHeader  header;      /* A DB entity header */
   DBPoint   point;       /* A DB point entity */
   DBLine    line;        /* A DB line entity */
   DBPart    part;        /* A DB part entity */

/*
***Check that a file name was supplied on the command line.
*/
   if ( argc != 2 )
     {
     printf("Usage: DBdemo3 filenamne\n");
     exit(0);
     }
/*
***Now a little message.
*/
   printf("This is the DBdemo3 program !\n\n");
/*
***Load the DB file.
*/
   errstat = DBload(argv[1],0,DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL);
   if ( errstat < 0 )
     {
     printf("Error from DBload(), errstat=%d\n",(int)errstat);
     exit(0);
     }
/*
***To start the traversal at the top of the hierarchy set
***the root id to #0.
*/
   id.seq_val = 0;

   errstat = DBset_root_id(&id);
   if ( errstat < 0 )
     {
     printf("Error from DBset_root_id(), errstat=%d\n",(int)errstat);
     exit(0);
     }
/*
***Get a pointer to the next entity.
*/
loop:
   errstat = DBget_next_ptr(0,ALLTYP,NULL,NULL,&eptr);
   if ( errstat < 0 )
     {
     printf("Error from DBget_next_ptr(), errstat=%d\n",(int)errstat);
     exit(0);
     }
/*
***If the pointer returned != DBNULL it's a valid pointer otherwise
***we are at the end of the DB.
*/
   if ( eptr != DBNULL )
     {
/*
***An entity is found. Get its header. This is needed in order to
***determine what type of entity it is.
*/
     errstat = DBread_header(&header,eptr);
     if ( errstat < 0 )
       {
       printf("Error from DBread_header(), errstat=%d\n",(int)errstat);
       exit(0);
       }
/*
***Check entity type, read entity and print some data. This program
***only recognizes points, lines and parts but it should be rather straight
***forward to add any other entities here.
*/
     switch ( header.type )
       {
       case POITYP:
       printf("Point  ID=#%d.%d, DBptr=%d\n",(int)header.seknr,
                                             (int)header.ordnr,(int)eptr);
       errstat = DBread_point(&point,eptr);
       if ( errstat < 0 )
         {
         printf("Error from DBread_point(), errstat=%d\n",(int)errstat);
         exit(0);
         }
       printf("X=%g, Y=%g\n\n",point.crd_p.x_gm,point.crd_p.y_gm);
       break;

       case LINTYP:
       printf("Line   ID=#%d.%d, DBptr=%d\n",(int)header.seknr,
                                             (int)header.ordnr,(int)eptr);
       errstat = DBread_line(&line,eptr);
       if ( errstat < 0 )
         {
         printf("Error from DBread_line(), errstat=%d\n",(int)errstat);
         exit(0);
         }
       printf("X1=%g, Y1=%g\n",line.crd1_l.x_gm,line.crd1_l.y_gm);
       printf("X2=%g, Y2=%g\n\n",line.crd2_l.x_gm,line.crd2_l.y_gm);
       break;

       case PRTTYP:
       printf("Part   ID=#%d.%d, DBptr=%d\n",(int)header.seknr,
                                             (int)header.ordnr,(int)eptr);
       errstat = DBread_part(&part,eptr);
       if ( errstat < 0 )
         {
         printf("Error from DBread_part(), errstat=%d\n",(int)errstat);
         exit(0);
         }
       printf("Name=%s\n\n",part.name_pt);
       break;
       }
/*
***Continue with the next entity.
*/
     goto loop;
     }
/*
***Finally, close the DB.
*/
   errstat = DBexit();
   if ( errstat < 0 )
     {
     printf("Error from DBexit(), errstat=%d\n",(int)errstat);
     exit(0);
     }
/*
***The program ends here.
*/
   printf("\nDBdemo3 normally terminated !\n");
}
