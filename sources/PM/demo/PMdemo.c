/**********************************************************************
*
*    PMdemo1.c
*    ========
*
*    This file is part of the VARKON PM module.
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
*    (C)2007-01-18 Johan Kjellander, Örebro uniersity
*
***********************************************************************/

/*
***This program demonstrates how to compile and build a custom
***version of the Varkon interpreter PM.
*/

/*
***First of all include some standard C header files.
*/
#include <stdio.h>

/*
***Any application calling PM or using the PM datastructures
***must include DB.h and PM.h. In this case we include also
***IG.h which includes PM.h
*/
#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../AN/include/AN.h"

/*
***Current system sizes.
*/
V3MSIZ  sysize = {PMSIZE,0,0,RTSSIZE};

/*
***Current jobname and directories.
*/

char    jobnam[JNLGTH+1] = "Test"; 
char    jobdir[V3PTHLEN+1] = "";       /* Current user job directory */
char    amodir[10*V3PTHLEN+10] = "";   /* Alternate user module libs */

/*
***Default system meta data.
*/
V3MDAT  sydata =

           { 1000,       /* Serial number, microform = 1000 */
             1,          /* Version number*/
             19,         /* Revision */
            'A',         /* Development level */
             0,0,0,0,0,  /* Dummy times */
             0,0,0,0,0,
             " ",        /* Sysname */
             " ",        /* Dummy */
             BAS_MOD,    /* opmode */
             0,          /* Module protection code */
             " ",        /* Release */
             " ",        /* Version */
             0      } ;  /* Encrypted serial number */

/*
***Current module type and attribute.
*/
short   modtyp = 3;       /* 3 = GEOMETRY ie. 3D */
short   modatt = BASIC;

/*
***Flag for <CTRL>c interrupt from keyboard.
*/
bool    intrup = FALSE;

/*
***Global scanner data.
*/
struct ANSYREC sy;

/*
***Current pointer to start of active module and top
***of PM stack.
*/
pm_ptr  actmod;

/*
***Replacements for Varkon functions not needed.
*/
short IGwtma(short tsnum) {return(0);}

/*
***The code begins here.
*/
int main(argc,argv)
    int argc;
    char *argv[];

{
 
/*
***First a little message.
*/
   printf("This is the PMdemo1 program !\n\n");
/*
***Now, create and init PM.
*/
   if ( pminit() < 0 )
     {
     printf("Can't init/create ProgramMemory\n");
     exit(0);
     }
     
   if ( pmclea() < 0 )
     {
     printf("Can't clear ProgramMemory\n");
     exit(0);
     }

   if ( incrts() < 0 )
     {
     printf("Can't clear RunTimeStack\n");
     exit(0);
     }
     
   if ( inrdnp() < 0 )
     {
     printf("Can't reset default attribute values\n");
     exit(0);
     }
/*
***The program ends here.
*/
   printf("\nPMdemo1 normally terminated !\n");
}
