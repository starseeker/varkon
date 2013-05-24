/*!*****************************************************
*
*    exlevel.c
*    =========
*
*    EXinit_levels();   Init the levelname table
*    EXclear_levels();  Resets all names to ""
*    EXblank_level();   Interface-routine for BLANK_LEVEL
*    EXunblank_level(); Interface-routine for UNBLANK_LEVEL
*    EXget_level();     Interface-routine for GET_LEVEL
*    EXname_level();    Interface-routine for NAME_LEVEL
*
*    This file is part of the VARKON Execute Library.
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
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../WP/include/WP.h"
#include "../include/EX.h"

/*
***The Blank/Unblank property of a level is window dependant.
***Each graphical window WPGWIN/WPRWIN has its own level table.
***See wpdef.h and wplevels.c in WP.
*
***Level names are system wide. This means that all windows share
***the same levelname table. The levelname table is implemented
***as a table of dynamically allocated C-pointers. (2000 levels
***and 132 chars/name would need 1/4 Mb static memory !) Memory
***for the name of a level is allocated when the level gets a
***name by EXname_level();
*/
static struct
{
   char *name;
   bool  allocated;
} level_names[WP_NIVANT];

/*!******************************************************/

        void EXinit_levels()

/*      Initializes the levelname table during
 *      system startup. At this state no levels
 *      have memory allocated for a name. Called
 *      by main().
 *
 *      (C)2007-03-31 J. Kjellander
 *
 ******************************************************!*/

  {
   int i;

/*
***At this state no memory is allocated.
*/
   for ( i=0; i<WP_NIVANT; ++i )
     {
     level_names[i].allocated = FALSE;
     }
/*
***The end.
*/
   return;
  }

/********************************************************/
/*!******************************************************/

        void EXclear_levels()

/*      Resets all level names to "". Used before a
 *      job is loaded to ensure that no levels have
 *      names that belong to a previous job.
 *
 *      (C)2007-03-31 J. Kjellander
 *
 ******************************************************!*/

  {
   int i;

/*
***If memory is allocated, set the name to "".
*/
   for ( i=0; i<WP_NIVANT; ++i )
     {
     if ( level_names[i].allocated ) level_names[i].name[0] = '\0';;
     }
/*
***The end.
*/
   return;
  }

/********************************************************/
/*!******************************************************/

        short EXblank_level(
        DBint levnum,
        DBint win_id)

/*      Executes BLANK_LEVEL(). Blanks a level.
 *
 *      In: levnum = Level number.
 *          win_id = Window ID.
 *
 *      Error: EX1632 = Illegal level number
 *
 *      (C)microform ab 1/6/86 J. Kjellander
 *
 *      1/9/86   R. Svedin. Ändrad feltest.
 *      29/9/86  R. Svedin. Ny nivåhant.
 *      17/1-95  Multifönster, J. Kjellander
 *      2007-04-01 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
   char errbuf[V3STRLEN];

/*
***Check for illegal level number.
*/
   if ( levnum < 0 || levnum > WP_NIVANT-1)
     {
     sprintf(errbuf,"%d",levnum);
     return(erpush("EX1632",errbuf));
     }
/*
***Blank the level in the specified window.
*/
   WPmtsl(win_id,levnum,levnum,WP_BLANKL);
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXunblank_level(
        DBint levnum,
        DBint win_id)

/*      Executes UNBLANK_LEVEL(). Unblanks a level.
 *
 *      In: levnum = Level number.
 *          win_id = Window ID.
 *
 *      Error: EX1632 = Illegal level number
 *
 *      (C)microform ab 1/6/86 J. Kjellander
 *
 *      1/8/86   Ändrad feltest, R. Svedin.
 *      29/9/86  Ny nivåhantering, R. Svedin.
 *      17/1-95  Multifönster, J. Kjellander
 *      2007-04-01 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
   char errbuf[V3STRLEN];

/*
***Check for illegal level number.
*/
   if ( levnum < 0 || levnum > WP_NIVANT-1)
     {
     sprintf(errbuf,"%d",levnum);
     return(erpush("EX1632",errbuf));
     }
/*
***Unblank the level in the specified window.
*/
   WPmtsl(win_id,levnum,levnum,WP_UBLANKL);
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXget_level(
        DBint levnum,
        DBint win_id,
        bool *blank,
        char *name)

/*      Executes GET_LEVEL(). Returns level information.
 *
 *      In:  levnum = Level number.
 *           win_id = Window ID.
 *
 *      Out: blank  = 0=>Unblanked, 1=>Blanked.
 *           name   = C ptr to level name.
 *
 *      Error: EX1632 = Illegal level number
 *
 *      (C)2007-04-01 J. Kjellander
 *
 ******************************************************!*/

  {
   char    errbuf[V3STRLEN];
   WPWIN  *winptr;
   WPGWIN *gwinpt;
   WPRWIN *rwinpt;

/*
***Check for illegal level number.
*/
   if ( levnum < 0 || levnum > WP_NIVANT-1)
     {
     sprintf(errbuf,"%d",levnum);
     return(erpush("EX1632",errbuf));
     }
/*
***Is this level blanked or unblanked in the specified window ?
*/
   winptr = WPwgwp(win_id);

   if ( winptr->ptr != NULL  &&  winptr->typ == TYP_GWIN )
     {
     gwinpt = (WPGWIN *)winptr->ptr;
    *blank = !WPnivt(gwinpt->nivtab,levnum);
     }
   else if ( winptr->ptr != NULL  &&  winptr->typ == TYP_RWIN )
     {
     rwinpt = (WPRWIN *)winptr->ptr;
    *blank = !WPnivt(rwinpt->nivtab,levnum);
     }
   else
     {
     sprintf(errbuf,"%d",win_id);
     return(erpush("EX2162",errbuf));
     }
/*
***Level name.
*/
   if ( level_names[levnum].allocated )  strcpy(name,level_names[levnum].name);
   else                                 *name = '\0';                       
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/;

        short EXname_level(
        DBint levnum,
        char *name)

/*      Executes NAME_LEVEL(). Names a level.
 *
 *      In: levnum = Level number.
 *          name   = C ptr to name.
 *
 *      Error: EX1632 = Illegal level number
 *
 *      (C)2007-03-31 J. Kjellander
 *
 ******************************************************!*/

  {
   char *ptr,errbuf[V3STRLEN];

/*
***Check for illegal level number.
*/
   if ( levnum < 0 || levnum > WP_NIVANT-1)
     {
     sprintf(errbuf,"%d",levnum);
     return(erpush("EX1632",errbuf));
     }
/*
***Do we need to allocate memory for this name ?
*/
   if ( !level_names[levnum].allocated )
     {
     ptr = (char *)v3mall(V3STRLEN+1,"EXname_level");
     if ( ptr == NULL ) return(-3);
     level_names[levnum].name = ptr;
     level_names[levnum].allocated = TRUE;
     }
/*
***Save name.
*/
   strcpy(level_names[levnum].name,name);
/*
***The end.
*/
   return(0);
  }

/********************************************************/
