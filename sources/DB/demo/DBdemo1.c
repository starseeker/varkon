/**********************************************************************
*
*    DBdemo1.c
*    ========
*
*    This file is part of the VARKON Database Library.
*    URL: http://www.tech.oru.se/cad/varkon
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

/*
***This program demonstrates the basic functionality of the
***VARKON Database. It creates a DB file and inserts
***two points in the DB. Then it closes the DB, opens
***the file again and retrieves the points from the DB
***using two different accessing methods.
***The DB file created is compatible with a standard
***Varkon DB file. I loaded the DB file created with this
***program into Varkon for verification and everything
***seems normal.
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
   int   main(argc,argv)
   int   argc;
   char *argv[];

{
   DBstatus  errstat;     /* Local variable for error checking */
   DBPoint   point;       /* A point entity */
   DBId      id;          /* The point ID */
   DBptr     db_pointer1; /* The DB pointer to the first point */
   DBptr     db_pointer2; /* The DB pointer to the second point */
   DBetype   type;        /* The entity type */

/*
***First a little message.
*/
   printf("This is the DBdemo1 program !\n\n");
/*
***Now init DB and create a new DB file. You can use any
***name you want. ".RIT" is default for drawings in the
***VARKON drawing editor. Note that if a file already
***exists it will be overwritten without notice. If
***you want to know if a file already exists and load it
***with DBload() you will have to do this check yourself.
*/
   errstat = DBinit("DBdemo1.RIT",0,DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL);
   if ( errstat < 0 )
     {
     printf("Error from DBinit(), errstat=%d\n",(int)errstat);
     exit(0);
     }
/*
***Now we can insert our entities. Lets try a point, DBpoint in DB.h.
***First we initialize some neccesary data in the header.
*/
   point.hed_p.blank = FALSE;     /* This point is visible */
   point.hed_p.hit   = TRUE;      /* and selectable */
   point.hed_p.pen   = 1;         /* Pen/color */
   point.hed_p.level = 1;         /* Level/layer */
/*
***The rest of the header is set up by DB. No need to care about
***that. Now set up the point specific data.
*/
   point.crd_p.x_gm  = 100;       /* X-coordinate */
   point.crd_p.y_gm  = 200;       /* Y-coordinate */
   point.crd_p.z_gm  = 0.0;       /* Z-coordinate */
   point.wdt_p       = 0.0;       /* Smallest possible linewidth */
   point.pcsy_p      = DBNULL;    /* No local coordinate system defined */
/*
***Finally we have to give the point a unique ID. This is the most
***complicated part and we will most probably have to discuss
***how to do that and make some changes. For the time being
***we can hardcode something. Lets use ID =1.
*/
   id.seq_val = 1;
/*
***Insert the point into the DB. In return you get the low level
***DB pointer (logical adress) of the point. This pointer is
***unique and persistent (never changes). You can keep this
***pointer if you want to or retrieve it later by calling
***DBget_pointer() who translates a DBid to a DBptr.
*/
   errstat = DBinsert_point(&point,&id,&db_pointer1);
   if ( errstat < 0 )
     {
     printf("Error from DBinsert_point(), errstat=%d\n",(int)errstat);
     exit(0);
     }
/*
***Now a second point.
*/
   point.crd_p.x_gm = 50;
   id.seq_val       = 2;

   errstat = DBinsert_point(&point,&id,&db_pointer2);
   if ( errstat < 0 )
     {
     printf("Error from DBinsert_point(), errstat=%d\n",(int)errstat);
     exit(0);
     }
/*
***Finally, close the DB. DBexit() writes all modified data
***in the cache to the pagefile and closes the file. The only
***calls to DBlib you can do after DBexit() is DBinit() or DBload().
*/
   errstat = DBexit();
   if ( errstat < 0 )
     {
     printf("Error from DBexit(), errstat=%d\n",(int)errstat);
     exit(0);
     }
/*
***Now load the DB again and ask for the points to see
***if they still are there. 
*/
   errstat = DBload("DBdemo1.RIT",0,DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL);
   if ( errstat < 0 )
     {
     printf("Error from DBload(), errstat=%d\n",(int)errstat);
     exit(0);
     }
/*
***Read the first point from the DB using its DBptr
***and print some point data for verification.
*/
   errstat = DBread_point(&point,db_pointer1);
   if ( errstat < 0 )
     {
     printf("Error from DBread_point(), errstat=%d\n",(int)errstat);
     exit(0);
     }

   printf("Point ID = %d\n",point.hed_p.seknr);
   printf("X = %g, Y = %g\n",point.crd_p.x_gm,point.crd_p.y_gm);
/*
***Read the second point from the DB using its DBid
***and print some point data for verification.
*/
   id.seq_val  = 2;        /* Point number 2 */
   id.ord_val  = 1;        /* First instance */
   id.p_nextre = NULL;     /* No more levels in ID hierarchy */

   db_pointer2 = DBNULL;   /* Just to verify the old value isn't used */

   errstat = DBget_pointer('I',&id,&db_pointer2,&type);
   if ( errstat < 0 )
     {
     printf("Error from DBget_pointer(), errstat=%d\n",(int)errstat);
     exit(0);
     }

   errstat = DBread_point(&point,db_pointer2);
   if ( errstat < 0 )
     {
     printf("Error from DBread_point(), errstat=%d\n",(int)errstat);
     exit(0);
     }

   printf("Point ID = %d\n",point.hed_p.seknr);
   printf("X = %g, Y = %g\n",point.crd_p.x_gm,point.crd_p.y_gm);
/*
***Finally, close the DB for the last time and exit.
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
   printf("\nDBdemo1 normally terminated !\n");
}
