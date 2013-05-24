/**********************************************************************
*
*    EXdemo1.c
*    ========
*
*    This file is part of the VARKON Library.
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
***This program demonstrates the use of the DXF import
***function EXimport_dxf(). EXdemo1.DXF is a DXF file
***including some points and lines. The program reads
***EXdemo1.DXF twice and stores all points and lines in the DB.
***A logfile EXdemo1.log is created.
*/

/*
***First of all include some standard C header files.
*/
#include <stdio.h>

/*
***Any application calling DB or using the DB datastructures
***must include "DB.h".
*/
#include "../../DB/include/DB.h"

/*
***Include EX.h
*/
#include "../include/EX.h"

/*
***The code begins here.
*/
int main(argc,argv)
    int argc;
    char *argv[];

{
   DBstatus  errstat;     /* Local variable for error checking */

/*
***Now a little message.
*/
   printf("This is the EXdemo1 program !\n\n");
/*
***Now init DB and create a new DB file.
*/
   errstat = DBinit("EXdemo1.RIT",0,DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL);
   if ( errstat < 0 )
     {
     printf("Error from DBinit(), errstat=%d\n",(int)errstat);
     exit(0);
     }
/*
***Create a part by loading a DXF file.
*/
   EXimport_dxf("EXdemo1.DXF","EXdemo1",0.0,0.0,0.0,1.0,"EXdemo1.log");
/*
***Create a another part by loading the same DXF file but with
***a different base position (X=200).
*/
   EXimport_dxf("EXdemo1.DXF","EXdemo1",200.0,0.0,0.0,1.0,"EXdemo1.log");
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
   printf("\nEXdemo1 terminated !\n");
}
