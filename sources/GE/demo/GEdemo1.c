/**********************************************************************
*
*    GEdemo1.c
*    ========
*
*    This file is part of the VARKON Geometry Library.
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
***********************************************************************/

/*
***This program demonstrates how to compute the coordinates
***for a line parallell to another line.
*/

/*
***Any application calling GE must include DB.h and GE.h
***in that order. GE.h depends on DB.h
*/
#include "../../DB/include/DB.h"
#include "../include/GE.h"

/*
***The code begins here.
*/
int main(argc,argv)
    int argc;
    char *argv[];

{
   DBstatus status;
   DBLine   lin1,lin2;

/*
***First a little message.
*/
   printf("This is the GEdemo1 program !\n\n");
/*
***Now create the first line.
*/
   lin1.crd1_l.x_gm = 0.0;
   lin1.crd1_l.y_gm = 0.0;
   lin1.crd1_l.z_gm = 0.0;

   lin1.crd2_l.x_gm = 100.0;
   lin1.crd2_l.y_gm = 0.0;
   lin1.crd2_l.z_gm = 0.0;

   printf("The coordinates for the original line are:\n");
   printf("X1=%g, Y1=%g, X2=%g, Y2=%g\n",
          lin1.crd1_l.x_gm,lin1.crd1_l.y_gm,
          lin1.crd2_l.x_gm,lin1.crd2_l.y_gm);
/*
***Create a parallell line by calling GE202().
***Make the parallell distance = 10.0. A positive
***value means that the new line will end up on
***the right side of the old line, in this case
***below the original line.
*/
   status = GE202(&lin1,10.0,NULL,&lin2);
   if ( status < 0 )
     {
     printf("Error from GE202(), status=%d\n",(int)status);
     exit();
     }
/*
***Print the result.
*/
   printf("\nThe coordinates for the new parallell line are:\n");
   printf("X1=%g, Y1=%g, X2=%g, Y2=%g\n",
          lin2.crd1_l.x_gm,lin2.crd1_l.y_gm,
          lin2.crd2_l.x_gm,lin2.crd2_l.y_gm);
/*
***The program ends here.
*/
   printf("\nGEdemo1 normally terminated !\n");
}
