/**********************************************************************
*
*    wplight.c
*    =========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
*
*    This file includes:
*
*    WPltvi();    Servs LIGHT_VIEW()
*    WPlton();    Servs LIGHT_ON/OFF()
*    WPconl();    Sets light parameters
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
#include "../include/WP.h"
#include <GL/glu.h>
#include <math.h>

/*
***Ljuskällor, max 8 stycken.
*/
typedef struct wplight
{
GLfloat pos[4];            /* Läge/riktning */
GLfloat dir[3];            /* Spotriktning */
GLfloat ang;               /* Spotvinkel */
GLfloat focus;             /* Spotfocus (0-128) */
GLfloat intensity;         /* Ljusstyrka (0-1) */
bool    state;             /* På/av */
} WPLIGHT;

static WPLIGHT lt_tab[8] = { {{0,0,1,0},{0,0,-1},180,0,1,TRUE},
                             {{0,0,1,0},{0,0,-1},180,0,1,FALSE},
                             {{0,0,1,0},{0,0,-1},180,0,1,FALSE},
                             {{0,0,1,0},{0,0,-1},180,0,1,FALSE},
                             {{0,0,1,0},{0,0,-1},180,0,1,FALSE},
                             {{0,0,1,0},{0,0,-1},180,0,1,FALSE},
                             {{0,0,1,0},{0,0,-1},180,0,1,FALSE},
                             {{0,0,1,0},{0,0,-1},180,0,1,FALSE} };

/*!******************************************************/

        short  WPltvi(
        DBint     ltnum,
        DBVector *pos1,
        DBVector *pos2,
        DBfloat   ang,
        DBfloat   focus)

/*      Sätter ljuskällas geometry.
 *
 *      In: ltnum = Ljuskälla
 *          pos1  = Position/riktning
 *          pos2  = Spotposition
 *          ang   = Spotvinkel
 *          focus = Spotfokus
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 1997-02-19 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Lite felkontroll.
*/
   if ( ltnum < 0  ||  ltnum > 7 ) return(0);
/*
***Om ang = 180 är det en ingen spotlight utan
***en vanlig lampa som lyser i alla riktningar.
*/
   if ( ang == 180.0 )
     {
     if ( pos1->x_gm == 0.0  &&
          pos1->y_gm == 0.0  &&
          pos1->z_gm == 0.0 ) return(0);

     lt_tab[ltnum].pos[0] = (GLfloat)pos1->x_gm;
     lt_tab[ltnum].pos[1] = (GLfloat)pos1->y_gm;
     lt_tab[ltnum].pos[2] = (GLfloat)pos1->z_gm;
     lt_tab[ltnum].pos[3] = (GLfloat)0.0;
     lt_tab[ltnum].ang    = 180.0;
     }
/*
***Om spotang <> 180 är det en spot. pos1 och pos2 får
***då inte vara lika för då kan vi inte beräkna nån riktning.
*/
   else
     {
     if ( pos2->x_gm == pos1->x_gm  &&
          pos2->y_gm == pos1->x_gm  &&
          pos2->z_gm == pos1->x_gm ) return(0);

     if ( ang > 90.0  ||  ang <= 0.0 ) return(0);
     if ( focus < 0  ||  focus > 100 ) return(0);

     lt_tab[ltnum].pos[0] = (GLfloat)pos1->x_gm;
     lt_tab[ltnum].pos[1] = (GLfloat)pos1->y_gm;
     lt_tab[ltnum].pos[2] = (GLfloat)pos1->z_gm;
     lt_tab[ltnum].pos[3] = (GLfloat)1.0;
     lt_tab[ltnum].dir[0] = (GLfloat)(pos2->x_gm - pos1->x_gm);
     lt_tab[ltnum].dir[1] = (GLfloat)(pos2->y_gm - pos1->y_gm);
     lt_tab[ltnum].dir[2] = (GLfloat)(pos2->z_gm - pos1->z_gm);
     lt_tab[ltnum].ang    = (GLfloat)ang;
     lt_tab[ltnum].focus  = (GLfloat)focus*1.28;
     }

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short WPlton(
        int   ltnum,
        DBfloat intensity,
        bool  state)

/*      Slår på och av olika ljuskällor.
 *
 *      In: ltnum = Ljuskälla
 *          state = På/Av.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 1997-02-19 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Lite felkontroll.
*/
   if ( ltnum < 0  ||  ltnum > 7 ) return(0);
   if ( intensity < 0.0  ||  intensity > 100.0 ) return(0);
/*
***Slå på/av.
*/
   if ( state )
     {
     lt_tab[ltnum].state = TRUE;
     lt_tab[ltnum].intensity = (GLfloat)(intensity/100.0);
     }
   else lt_tab[ltnum].state = FALSE;

   return(0);
  }

/********************************************************/
/*!******************************************************/

       short WPconl(
       WPGWIN *gwinpt)

/*      Konfigurerar aktiva ljuskällor.
 *
 *      In: gwinpt = Pekare till grafisk fönster
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 1997-02-19 J. Kjellander
 *
 ******************************************************!*/

  {
   int     i,ival;
   char    buf[V3STRLEN];
   GLfloat pos[4],dir[3],ambient[4],diffuse[4],specular[4];

   GLfloat scene_ambient[4]  = {0.2,0.2,0.2,1.0};
   GLfloat lt_ambient[4]  = {1.0,1.0,1.0,1.0};
   GLfloat lt_diffuse[4]  = {1.0,1.0,1.0,1.0};
   GLfloat lt_specular[4] = {1.0,1.0,1.0,1.0};
/*
***Slå på ljus.
*/
   glEnable(GL_LIGHTING);
/*
***Justera scenens eget bakgrundsljus. Default är
***enligt manualen (0.2,0.2,0.2).
*/
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT,scene_ambient);
/*
***Default bakgrundsljus för alla ljuskällor hämtar vi från
***resursfilen.
*/
#ifdef UNIX
   if ( WPgrst("varkon.shade.ambient.red",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.AMBIENT.RED",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_ambient[0] = ival/100.0; 

#ifdef UNIX
   if ( WPgrst("varkon.shade.ambient.green",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.AMBIENT.GREEN",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_ambient[1] = ival/100.0; 

#ifdef UNIX
   if ( WPgrst("varkon.shade.ambient.blue",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.AMBIENT.BLUE",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_ambient[2] = ival/100.0; 
/*
***Default diffust ljus likaså.
*/
#ifdef UNIX
   if ( WPgrst("varkon.shade.diffuse.red",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.DIFFUSE.RED",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_diffuse[0] = ival/100.0; 

#ifdef UNIX
   if ( WPgrst("varkon.shade.diffuse.green",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.DIFFUSE.GREEN",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_diffuse[1] = ival/100.0; 

#ifdef UNIX
   if ( WPgrst("varkon.shade.diffuse.blue",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.DIFFUSE.BLUE",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_diffuse[2] = ival/100.0; 
/*
***Och default reflekterat ljus.
*/
#ifdef UNIX
   if ( WPgrst("varkon.shade.specular.red",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.SPECULAR.RED",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_specular[0] = ival/100.0; 

#ifdef UNIX
   if ( WPgrst("varkon.shade.specular.green",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.SPECULAR.GREEN",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_specular[1] = ival/100.0; 

#ifdef UNIX
   if ( WPgrst("varkon.shade.specular.blue",buf)  &&
#endif
#ifdef WIN32
   if ( msgrst("SHADE.SPECULAR.BLUE",buf)  &&
#endif
        sscanf(buf,"%d",&ival) == 1  &&
        ival >=0  &&  ival <= 100 ) lt_specular[2] = ival/100.0; 
/*
***Gå igenom lt_tab och sätt upp alla aktiva ljuskällor.
*/
   for ( i=0; i<8; ++i )
     {
     if ( lt_tab[i].state )
       {
       glEnable(GL_LIGHT0+i);
/*
***Transformera positionen.
*
       pos[0] = gwinpt->vy.vymat.k11 * lt_tab[i].pos[0] +
                gwinpt->vy.vymat.k12 * lt_tab[i].pos[1] +
                gwinpt->vy.vymat.k13 * lt_tab[i].pos[2];
       pos[1] = gwinpt->vy.vymat.k21 * lt_tab[i].pos[0] +
                gwinpt->vy.vymat.k22 * lt_tab[i].pos[1] +
                gwinpt->vy.vymat.k23 * lt_tab[i].pos[2];
       pos[2] = gwinpt->vy.vymat.k31 * lt_tab[i].pos[0] +
                gwinpt->vy.vymat.k32 * lt_tab[i].pos[1] +
                gwinpt->vy.vymat.k33 * lt_tab[i].pos[2];
/*
***En vanlig ljuskälla har spotvinkel 180.0 och
***ingen avklingning, pos[3] = 0.0.
*/
       if ( lt_tab[i].ang == 180.0 )
         {
         pos[3] = 0.0;
         glLightfv(GL_LIGHT0+i,GL_POSITION,pos);
         glLightf(GL_LIGHT0+i,GL_SPOT_CUTOFF,(GLfloat)180.0);
         }
/*
***Om det är en spot skall även riktning transformeras
***och avklingning slås på.
*
       if ( lt_tab[i].ang < 180.0 )
         {
         pos[3] = 1.0;
         dir[0] = gwinpt->vy.vymat.k11 * lt_tab[i].dir[0] +
                  gwinpt->vy.vymat.k12 * lt_tab[i].dir[1] +
                  gwinpt->vy.vymat.k13 * lt_tab[i].dir[2];
         dir[1] = gwinpt->vy.vymat.k21 * lt_tab[i].dir[0] +
                  gwinpt->vy.vymat.k22 * lt_tab[i].dir[1] +
                  gwinpt->vy.vymat.k23 * lt_tab[i].dir[2];
         dir[2] = gwinpt->vy.vymat.k31 * lt_tab[i].dir[0] +
                  gwinpt->vy.vymat.k32 * lt_tab[i].dir[1] +
                  gwinpt->vy.vymat.k33 * lt_tab[i].dir[2];
         glLightfv(GL_LIGHT0+i,GL_POSITION,pos);
         glLightfv(GL_LIGHT0+i,GL_SPOT_DIRECTION,dir);
         glLightf(GL_LIGHT0+i,GL_SPOT_CUTOFF,(GLfloat)lt_tab[i].ang);
         glLightf(GL_LIGHT0+i,GL_SPOT_EXPONENT,(GLfloat)lt_tab[i].focus);
         }
/*
***Bakgrundskomponentens styrka.
*/
       ambient[0] = lt_tab[i].intensity * lt_ambient[0];
       ambient[1] = lt_tab[i].intensity * lt_ambient[1];
       ambient[2] = lt_tab[i].intensity * lt_ambient[2];
       ambient[3] = 1.0;
       glLightfv(GL_LIGHT0+i,GL_AMBIENT,ambient);
/*
***Diffusa komponentens styrka.
*/
       diffuse[0] = lt_tab[i].intensity * lt_diffuse[0];
       diffuse[1] = lt_tab[i].intensity * lt_diffuse[1];
       diffuse[2] = lt_tab[i].intensity * lt_diffuse[2];
       diffuse[3] = 1.0;
       glLightfv(GL_LIGHT0+i,GL_DIFFUSE,diffuse);
/*
***Spekulära komponentens styrka.
*/
       specular[0] = lt_tab[i].intensity * lt_specular[0];
       specular[1] = lt_tab[i].intensity * lt_specular[1];
       specular[2] = lt_tab[i].intensity * lt_specular[2];
       specular[3] = 1.0;
       glLightfv(GL_LIGHT0+i,GL_SPECULAR,specular);
       }
     }
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
