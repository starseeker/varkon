/**********************************************************************
*
*    GEdemo2.c
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
***This program demonstrates how to create a 2D cricle
***offset to another 2D circle. It also demonstrates
***the use of 2D transformations.
*/

/*
***Any application calling GE must include DB.h and GE.h
***in that order. GE.h depends on DB.h
*/
#include "../../DB/include/DB.h"
#include "../include/GE.h"

/*
***Dummy replacements for otherwise unresolved symbols
***from code not yet released. These are not used in 2D.
*/
DBTmat  *lsyspk = NULL;
DBstatus sur776(){return(0);}
DBfloat  sur751(){return(0);}
DBfloat  sur753(){return(0);}
DBstatus sur690(){return(0);}
DBstatus sur214(){return(0);}

/*
***The code begins here.
*/
int main(argc,argv)
    int argc;
    char *argv[];

{
   DBstatus status;
   DBVector p;
   DBTmat   t;
   DBArc    arc1,arc2;

/*
***First a little message.
*/
   printf("This is the GEdemo2 program !\n\n");
/*
***Now create a coordinate system with origin in
***X=Y=100 rotaded 45 degrees CCW around the BASIC
***Z axis.
*/
   p.x_gm = 100.0;
   p.y_gm = 100.0;
   p.z_gm = 0.0;

   status = GEmktf_1p(&p, 0.0, 0.0, 45.0, NULL, &t);
   if ( status < 0 )
     {
     printf("Error from GEmktf_1p(), status=%d\n",(int)status);
     exit();
     }
/*
***Next, create a 2D circle with origin in X=Y=0 relative
***to the above coordinate system. Let start angle = 0
***and end angle = 45 also relative to t. Radius = 10.
*/
   p.x_gm = 0.0;
   p.y_gm = 0.0;
   p.z_gm = 0.0;

   GEtfpos_to_basic(&p,&t,&p);

   status = GE300(&p, 10.0, 0.0, 45.0, &t, &arc1, NULL, (short)2);
   if ( status < 0 )
     {
     printf("Error from GE300(), status=%d\n",(int)status);
     exit();
     }
/*
***Finally, create the offset circle.
*/
   status = GE306(&arc1, NULL, 10.0, NULL, &arc2, NULL);
   if ( status < 0 )
     {
     printf("Error from GE306(), status=%d\n",(int)status);
     exit();
     }
/*
***Display some data for the offset circle.
*/
   printf("Offset origin = %g, %g\n",arc2.x_a,arc2.y_a);
   printf("Offset radius = %g\n",arc2.r_a);
   printf("Offset angles = %g, %g\n",arc2.v1_a,arc2.v2_a);
/*
***The program ends here.
*/
   printf("\nGEdemo2 normally terminated !\n");
}

/**********************************************************************/
