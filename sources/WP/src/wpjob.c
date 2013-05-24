/**********************************************************************
*
*    wpjob.c
*    =======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPsave_job();     Write graphical window data to JOB-file
*    WPload_WPGWIN();  Read WPGWIN data from JOB-file and create window
*    WPload_WPRWIN();  Read WPRWIN data from JOB-file and create window
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

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../EX/include/EX.h"
#include "../include/WP.h"
#include <string.h>

extern WPVIEW wpviewtab[];

/*!******************************************************/

        short WPsave_job(FILE *jf)

/*      Write WPGWIN/WPRWIN data to jobfile.
 *
 *      In: jf => C ptr to open jobfile.
 *
 *      Felkod: WP1422 = Can't write to file.
 *
 *      (C)2007-04-10 J. Kjellander
 *
 ******************************************************!*/

 {
   int     i,j,current_x,current_y,corrx,corry;
   WPWIN  *winptr;
   WPGWIN *gwinpt;
   WPRWIN *rwinpt;

/*
***Look upp all graphical windows.
*/
   for ( i=0; i<WTABSIZ; ++i )
     {
/*
***A WPGWIN.
*/
     if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_GWIN )
       {
       gwinpt = (WPGWIN *)winptr->ptr;

       if ( fprintf(jf,"\n#WPGWIN\n")                   < 0 ) goto werror;
/*
***What is the geometry of the window now ? Correct for WM trix and
***save position and size.
*/
       WPgtwp(gwinpt->id.x_id,&current_x,&current_y);
       corrx = current_x - gwinpt->wmandx;
       corry = current_y - gwinpt->wmandy;

       if ( fprintf(jf,"X=%d\n",corrx)                  < 0 ) goto werror;
       if ( fprintf(jf,"Y=%d\n",corry)                  < 0 ) goto werror;

       if ( fprintf(jf,"DX=%d\n",gwinpt->geo.dx)        < 0 ) goto werror;
       if ( fprintf(jf,"DY=%d\n",gwinpt->geo.dy)        < 0 ) goto werror;
/*
***Current view name.
*/
       if ( fprintf(jf,"Viewname=%s\n",gwinpt->vy.name) < 0 ) goto werror;
/*
***The grid.
*/
       if ( gwinpt->grid_on )
         {
         if ( fprintf(jf,"Grid=On\n")                   < 0 ) goto werror;
         if ( fprintf(jf,"X=%g\n",gwinpt->grid_x)       < 0 ) goto werror;
         if ( fprintf(jf,"Y=%g\n",gwinpt->grid_y)       < 0 ) goto werror;
         if ( fprintf(jf,"DX=%g\n",gwinpt->grid_dx)     < 0 ) goto werror;
         if ( fprintf(jf,"DY=%g\n",gwinpt->grid_dy)     < 0 ) goto werror;
         }
       else
         {
         if ( fprintf(jf,"Grid=Off\n")                  < 0 ) goto werror;
         }
/*
***The level table.
*/
       if ( fprintf(jf,"Blanked_levels:\n")             < 0 ) goto werror;
       for ( j=0; j<WP_NIVANT; ++j )
         {
         if ( !WPnivt(gwinpt->nivtab,j) )
           {
           if ( fprintf(jf,"%d\n",j)                    < 0 ) goto werror;
           }
         }
       if ( fprintf(jf,"#End\n")                        < 0 ) goto werror;
       }
/*
***A WPRWIN.
*/
     else if ( (winptr=WPwgwp((wpw_id)i)) != NULL  &&
           winptr->typ == TYP_RWIN )
       {
       rwinpt = (WPRWIN *)winptr->ptr;

       if ( fprintf(jf,"\n#WPRWIN\n")                   < 0 ) goto werror;
/*
***What is the geometry of the window now ? Correct for WM trix and
***save position and size.
*/
       WPgtwp(rwinpt->id.x_id,&current_x,&current_y);
       corrx = current_x - rwinpt->wmandx;
       corry = current_y - rwinpt->wmandy;

       if ( fprintf(jf,"X=%d\n",corrx)                  < 0 ) goto werror;
       if ( fprintf(jf,"Y=%d\n",corry)                  < 0 ) goto werror;

       if ( fprintf(jf,"DX=%d\n",rwinpt->geo.dx)        < 0 ) goto werror;
       if ( fprintf(jf,"DY=%d\n",rwinpt->geo.dy)        < 0 ) goto werror;
/*
***Current view name.
*/
       if ( fprintf(jf,"Viewname=%s\n",rwinpt->vy.name) < 0 ) goto werror;
/*
***The level table.
*/
       if ( fprintf(jf,"Blanked_levels:\n")             < 0 ) goto werror;
       for ( j=0; j<WP_NIVANT; ++j )
         {
         if ( !WPnivt(rwinpt->nivtab,j) )
           {
           if ( fprintf(jf,"%d\n",j)                    < 0 ) goto werror;
           }
         }
       if ( fprintf(jf,"#End\n")                        < 0 ) goto werror;
       }
/*
***Next window.
*/
     }
/*
***The end.
*/
   return(0);
/*
***Error exit.
*/
werror:
   return(erpush("WP1422",""));
 }

/********************************************************/
/*!******************************************************/

        short WPload_WPGWIN(FILE *jf)

/*      Load and creates a WPGWIN from jobfile.
 *
 *      In: f => C ptr to open jobfile.
 *
 *      (C)2007-04-10 J. Kjellander
 *
 ******************************************************!*/

  {
   char    line[V3STRLEN+1];
   int     ix,iy,idx,idy,level,bitmsk,bytofs;
   short   status;
   bool    gwmain;
   DBint   w_id;
   WPGWIN *gwinpt;

/*
***Is this GWIN_MAIN ?
*/
   if ( WPngws() == 0 ) gwmain = TRUE;
   else                 gwmain = FALSE;
/*
***Read the geometry.
*/
   if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
   if ( strncmp(line,"X=",2) != 0 ) return(-1);
   if ( sscanf(&line[2],"%d",&ix) == 0 ) return(-1);

   if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
   if ( strncmp(line,"Y=",2) != 0 ) return(-1);
   if ( sscanf(&line[2],"%d",&iy) == 0 ) return(-1);

   if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
   if ( strncmp(line,"DX=",3) != 0 ) return(-1);
   if ( sscanf(&line[3],"%d",&idx) == 0 ) return(-1);

   if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
   if ( strncmp(line,"DY=",3) != 0 ) return(-1);
   if ( sscanf(&line[3],"%d",&idy) == 0 ) return(-1);
/*
***Fix a good position.
*/
   WPposw(ix,iy,idx+10,idy+25,&ix,&iy);
/*
***Create the window.
*/
   status = WPwcgw((short)ix,(short)iy,(short)idx,(short)idy,"",gwmain,&w_id);
   if ( status < 0 ) return(erpush("WP1242",XDisplayName(NULL)));

   gwinpt  = (WPGWIN *)wpwtab[(wpw_id)w_id].ptr;
/*
***Since DB may not be loaded yet, we can not measure
***model size which might be needed to activate the
***view now. So, just save the view name in the window
***for now.
*/
   if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
   if ( strncmp(line,"Viewname=",9) != 0 ) return(-1);
   if ( sscanf(&line[9],"%s",gwinpt->vy.name) == 0 ) return(-1);
/*
***Grid data. If no grid, leave line to Levels...
*/
   if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);

   if ( strncmp(line,"Grid=On",7) == 0 )
     {
     if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
     if ( strncmp(line,"X=",2) != 0 ) return(-1);
     if ( sscanf(&line[2],"%lf",&gwinpt->grid_x) == 0 ) return(-1);

     if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
     if ( strncmp(line,"Y=",2) != 0 ) return(-1);
     if ( sscanf(&line[2],"%lf",&gwinpt->grid_y) == 0 ) return(-1);

     if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
     if ( strncmp(line,"DX=",3) != 0 ) return(-1);
     if ( sscanf(&line[3],"%lf",&gwinpt->grid_dx) == 0 ) return(-1);

     if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
     if ( strncmp(line,"DY=",3) != 0 ) return(-1);
     if ( sscanf(&line[3],"%lf",&gwinpt->grid_dy) == 0 ) return(-1);

     gwinpt->grid_on = TRUE;
     if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1); /* For Levels.. */
     }
   else
     {
     if ( strncmp(line,"Grid=Off",8) == 0 )
       {
       gwinpt->grid_on = FALSE;
       if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1); /* For Levels.. */
       }
     }
/*
***Levels.
*/
   if ( strncmp(line,"Blanked_levels:",15) != 0 ) return(-1);

   if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);

   while ( sscanf(line,"%d",&level) == 1 )
     {
     bytofs = level>>3;
     bitmsk = 1;
     bitmsk = ~(bitmsk<<(level&7));
     gwinpt->nivtab[bytofs] &= bitmsk;
     if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
     }
/*
***Final "#End".
*/
   if ( strncmp(line,"#End",4) != 0 ) return(-1);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPload_WPRWIN(FILE *jf)

/*      Load and creates a WPRWIN from jobfile.
 *
 *      In: f => C ptr to open jobfile.
 *
 *      (C)2007-04-10 J. Kjellander
 *
 ******************************************************!*/

  {
   char    line[V3STRLEN+1],title[V3STRLEN+1];
   int     ix,iy,idx,idy,level,bitmsk,bytofs;
   short   status;
   DBint   w_id;
   WPRWIN *rwinpt;

/*
***Read the geometry.
*/
   if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
   if ( strncmp(line,"X=",2) != 0 ) return(-1);
   if ( sscanf(&line[2],"%d",&ix) == 0 ) return(-1);

   if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
   if ( strncmp(line,"Y=",2) != 0 ) return(-1);
   if ( sscanf(&line[2],"%d",&iy) == 0 ) return(-1);

   if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
   if ( strncmp(line,"DX=",3) != 0 ) return(-1);
   if ( sscanf(&line[3],"%d",&idx) == 0 ) return(-1);

   if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
   if ( strncmp(line,"DY=",3) != 0 ) return(-1);
   if ( sscanf(&line[3],"%d",&idy) == 0 ) return(-1);
/*
***Fix a good position.
*/
   WPposw(ix,iy,idx+10,idy+25,&ix,&iy);
/*
***Create the WPRWIN window.
*/
   if ( !WPgrst("varkon.rwin.title",title) )
                  strcpy(title,"VARKON Dynamic Rendering");

   status = WPwcrw(ix,iy,idx,idy,title,&w_id);
   if ( status < 0 ) return(erpush("WP1242",XDisplayName(NULL)));

   rwinpt  = (WPRWIN *)wpwtab[(wpw_id)w_id].ptr;
/*
***Since DB may not be loaded yet, we can not measure
***model size which might be needed to activate the
***view now. So, just save the view name in the window
***for now.
*/
   if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
   if ( strncmp(line,"Viewname=",9) != 0 ) return(-1);
   if ( sscanf(&line[9],"%s",rwinpt->vy.name) == 0 ) return(-1);
/*
***Levels.
*/
   if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
   if ( strncmp(line,"Blanked_levels:",15) != 0 ) return(-1);

   if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);

   while ( sscanf(line,"%d",&level) == 1 )
     {
     bytofs = level>>3;
     bitmsk = 1;
     bitmsk = ~(bitmsk<<(level&7));
     rwinpt->nivtab[bytofs] &= bitmsk;
     if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
     }
/*
***Final "#End".
*/
   if ( strncmp(line,"#End",4) != 0 ) return(-1);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
