/**********************************************************************
*
*    GEdemo3.c
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
***This program demonstrates how to compute the intersection
***between a 2D line and a 2D circle.
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
DBstatus sur161(){return(0);}
DBstatus sur164(){return(0);}
DBstatus sur210(){return(0);}
DBstatus sur214(){return(0);}
DBstatus sur360(){return(0);}
DBstatus sur520(){return(0);}
DBstatus sur600(){return(0);}
DBstatus sur602(){return(0);}
DBstatus sur604(){return(0);}
DBstatus sur606(){return(0);}
DBstatus sur609(){return(0);}
DBstatus sur610(){return(0);}
DBstatus sur690(){return(0);}
DBstatus sur709(){return(0);}
DBstatus sur776(){return(0);}
DBstatus sur785(){return(0);}
DBfloat  sur751(){return(0);}
DBfloat  sur753(){return(0);}

/*
***The code begins here.
*/
int main(argc,argv)
    int argc;
    char *argv[];

{
   DBstatus status;
   DBLine   lin;
   DBArc    arc;
   DBfloat  radius,ang1,ang2;
   DBVector origin,intpos;

/*
***First a little message.
*/
   printf("This is the GEdemo3 program !\n\n");
/*
***Create the line.
*/
   lin.hed_l.type  = LINTYP;
   lin.crd1_l.x_gm = 0.0;
   lin.crd1_l.y_gm = 0.0;
   lin.crd1_l.z_gm = 0.0;

   lin.crd2_l.x_gm = 100.0;
   lin.crd2_l.y_gm = 0.0;
   lin.crd2_l.z_gm = 0.0;
/*
***Create a circle.
*/
   arc.hed_a.type = ARCTYP;
   origin.x_gm    =  50.0;
   origin.y_gm    =   0.0;
   origin.z_gm    =   0.0;
   radius         =  30.0;
   ang1           = -90.0;
   ang2           =  90.0;

   status = GE300(&origin,radius,ang1,ang2,NULL,&arc,NULL,(short)2);
   if ( status < 0 )
     {
     printf("Error from GE300(), status=%d\n",(int)status);
     exit();
     }
/*
***Compute the intersect.
*/
   status = GEintersect_pos((DBAny *)&lin,NULL,
                            (DBAny *)&arc,NULL,NULL,(short)1,&intpos);
   if ( status < 0 )
     {
     printf("Error from GEintersect_pos(), status=%d\n",(int)status);
     exit();
     }
/*
***Print the result.
*/
   printf("\nThe intersect coordinates are:\n");
   printf("X=%g, Y=%g, Z=%g\n",intpos.x_gm,intpos.y_gm,intpos.z_gm);
/*
***The program ends here.
*/
   printf("\nGEdemo3 normally terminated !\n");
}
