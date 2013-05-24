/**********************************************************************
*
*    DBdemo2.c
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
***This program demonstrates the use of "named data".
***A named data is a block fo data, any size, that you
***store and retrieve by a name. The name is any unique
***string of characters (max 132 chars).
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
***The following defines the struct type for a very simple layer.
*/
typedef struct
{
int color;
bool visible;
} LAYER;

/*
***The code begins here.
*/
   int   main(argc,argv)
   int   argc;
   char *argv[];

{
   DBstatus  errstat;     /* Local variable for error checking */

/*
***The following variables will be used when calling the DB.
*/
   LAYER layertable[2];
   int   type,count,size;
   char *datptr;
/*
***Now a little message.
*/
   printf("This is the DBdemo2 program !\n\n");
/*
***Now init DB and create a new DB file.
*/
   errstat = DBinit("DBdemo2.RIT",0,DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL);
   if ( errstat < 0 )
     {
     printf("Error from DBinit(), errstat=%d\n",(int)errstat);
     exit(0);
     }
/*
***Now we can insert our named data. Lets use an array
***of layer-structures in this example and store it
***with name ="LAYERTABLE".
*/
   layertable[0].color =   1;
   layertable[0].visible = TRUE;
   layertable[1].color =   2;
   layertable[1].visible = FALSE;

   type   = 0;                      /* Type of data, structs = 0 */
   size   = sizeof(layertable);     /* Datasize in bytes */
   count  = 1;                      /* Nymber of items */
   datptr = (char *)layertable;     /* A char-pointer to layertable */

   errstat = DBinsert_named_data("LAYERTABLE",
                                 type,
                                 size,
                                 count,
                                 datptr);
   if ( errstat < 0 )
     {
     printf("Error from DBinsert_named_data(), errstat=%d\n",(int)errstat);
     exit(0);
     }
/*
***Now, close the DB.
*/
   errstat = DBexit();
   if ( errstat < 0 )
     {
     printf("Error from DBexit(), errstat=%d\n",(int)errstat);
     exit(0);
     }
/*
***Load the DB again.
*/
   errstat = DBload("DBdemo2.RIT",0,DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL);
   if ( errstat < 0 )
     {
     printf("Error from DBload(), errstat=%d\n",(int)errstat);
     exit(0);
     }
/*
***Read the data from the DB. DB allocates C-memory as needed.
*/
   errstat = DBread_named_data("LAYERTABLE",
                                &type,
                                &size,
                                &count,
                                &datptr);
   if ( errstat < 0 )
     {
     printf("Error from DBread_named_data(), errstat=%d\n",(int)errstat);
     exit(0);
     }
/*
***Print the layer colors.
*/
   printf("Color of layer 1 = %d\n",((LAYER *)datptr)->color);
   printf("Color of layer 2 = %d\n",((LAYER *)datptr+1)->color);
/*
***Free the memory allocated by DB.
*/
   free(datptr);
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
   printf("\nDBdemo2 normally terminated !\n");
}
