/**********************************************************************
*
*    EXdemo2.c
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
***This program demonstrates DXF export.
*/

/*
***First of all include some standard C header files.
*/
#include <stdio.h>

/*
***Any application calling DB or using the DB datastructures
***must include "DB.h". We also need EX.h
*/
#include "../../DB/include/DB.h"
#include "../include/EX.h"

/*
***The code begins here.
*/
int main(argc,argv)
    int argc;
    char *argv[];

{
   DBstatus  errstat;     /* Local variable for error checking */
   int       status;      /* Return value from EXall_to_dxf() */

/*
***Check that a file name was supplied on the command line.
*/
   if ( argc != 2 )
     {
     printf("Usage: EXdemo2 filenamne\n");
     exit(0);
     }
/*
***Now a little message.
*/
   printf("This is the EXdemo2 program !\n\n");
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
***Write the contents of the DB to a file in DXF format.
*/
   status = EXall_to_dxf("EXdemo2.DXF",0.0,0.0);
   if ( status < 0 ) printf("Error from EXall_to_dxf(), status=%d\n",status);
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
   printf("\nEXdemo2 normally terminated !\n");
}
