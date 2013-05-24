/**********************************************************************
*
*    wplight.c
*    =========
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes:
*
*    WPcreate_light();   Creates an OpenGL light source
*    WPactivate_light(); Turns light sources off and on
*    WPenable_lights();  Sets light parameters
*    WPget_light();      Returns light data
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
***Light sources, max 8.
*/
typedef struct
{
GLfloat ambient[4];        /* OpenGL ambient RGBA */
GLfloat diffuse[4];        /* OpenGL diffuse RGBA */
GLfloat specular[4];       /* OpenGL specular RGBA */
GLfloat pos[4];            /* Light direction or spot position */
GLfloat dir[3];            /* Spot direction */
GLfloat ang;               /* Spot angle */
GLfloat focus;             /* Spotfocus (0-128) */
GLfloat intensity;         /* Light intensity (0-1) */
bool    follow_model;      /* Transform with model */
bool    on;                /* On/off */
bool    defined;           /* Defined/Not defined */
} WPLIGHT;

static WPLIGHT lt_tab[8] = {
   {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,1,0},{0,0,-1},180,0,1,FALSE,FALSE,FALSE},
   {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,1,0},{0,0,-1},180,0,1,FALSE,FALSE,FALSE},
   {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,1,0},{0,0,-1},180,0,1,FALSE,FALSE,FALSE},
   {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,1,0},{0,0,-1},180,0,1,FALSE,FALSE,FALSE},
   {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,1,0},{0,0,-1},180,0,1,FALSE,FALSE,FALSE},
   {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,1,0},{0,0,-1},180,0,1,FALSE,FALSE,FALSE},
   {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,1,0},{0,0,-1},180,0,1,FALSE,FALSE,FALSE},
   {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,1,0},{0,0,-1},180,0,1,FALSE,FALSE,FALSE} };

/********************************************************/

        short     WPcreate_light(
        DBint     ltnum,
        DBVector *pos1,
        DBVector *pos2,
        DBfloat   ang,
        DBfloat   focus)

/*      Create a light source.
 *
 *      In: ltnum = Light source number
 *          pos1  = Light direction or Spot position
 *          pos2  = Spot direction
 *          ang   = Spot angle
 *          focus = Spot focus
 *
 *      (C)2007-11-30 J.Kjellander
 *
 ******************************************************!*/

  {

/*
***Light number range check.
*/
   if ( ltnum < 0  ||  ltnum > 7 ) return(0);
/*
***Default light color = White. With default intensity of 50%
***this will produce a reasonable scene.
*/
   lt_tab[ltnum].ambient[0] =
   lt_tab[ltnum].ambient[1] =
   lt_tab[ltnum].ambient[2] = 0.4;

   lt_tab[ltnum].diffuse[0] =
   lt_tab[ltnum].diffuse[1] =
   lt_tab[ltnum].diffuse[2] = 1.0;

   lt_tab[ltnum].specular[0] =
   lt_tab[ltnum].specular[1] =
   lt_tab[ltnum].specular[2] = 1.0;
/*
***If ang = 180 it's no spotlight.
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
***If ang <> 180 it's a spot. pos1 and pos2 may then not be
***equal. W component of pos = 1 activates attenuation.
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
/*
***This light source is now defined but not on.
*/
   lt_tab[ltnum].defined = TRUE;
   lt_tab[ltnum].on      = FALSE;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/********************************************************/

        short   WPactivate_light(
        int     ltnum,
        DBfloat intensity,
        int     tmode,
        bool    on)

/*      Turns on and off a defined lightsource.
 *      LIGHT_ON() and LIGHT_OFF() in MBS.
 *
 *      In: ltnum     = Lightsource number
 *          intensity = 0-100%
 *          tmode     = 1 = Follow model, 0 otherwise
 *          on        = TRUE->On, FALSE->Off
 *
 *      (C)2007-11-30 J.Kjellander
 *
 ********************************************************/

  {
   GLfloat gl_matrix[16],ambient[4],diffuse[4],specular[4];
/*
***Range check. TODO add erpush()
*/
   if ( ltnum < 0  ||  ltnum > 7 ) return(0);
   if ( intensity < 0.0  ||  intensity > 100.0 ) return(0);
/*
***Is this light source defined ?
*/
   if ( !lt_tab[ltnum].defined ) return(0);
/*
***Turn on this light source.
*/
   if ( on )
     {
     lt_tab[ltnum].on = TRUE;
     lt_tab[ltnum].intensity = (GLfloat)(intensity/100.0);

     ambient[0] = lt_tab[ltnum].ambient[0]*lt_tab[ltnum].intensity;
     ambient[1] = lt_tab[ltnum].ambient[1]*lt_tab[ltnum].intensity;
     ambient[2] = lt_tab[ltnum].ambient[2]*lt_tab[ltnum].intensity;
     ambient[3] = lt_tab[ltnum].ambient[3];
     glLightfv(GL_LIGHT0+ltnum,GL_AMBIENT,ambient);

     diffuse[0] = lt_tab[ltnum].diffuse[0]*lt_tab[ltnum].intensity;
     diffuse[1] = lt_tab[ltnum].diffuse[1]*lt_tab[ltnum].intensity;
     diffuse[2] = lt_tab[ltnum].diffuse[2]*lt_tab[ltnum].intensity;
     diffuse[3] = lt_tab[ltnum].diffuse[3];
     glLightfv(GL_LIGHT0+ltnum,GL_DIFFUSE,diffuse);

     specular[0] = lt_tab[ltnum].specular[0]*lt_tab[ltnum].intensity;
     specular[1] = lt_tab[ltnum].specular[1]*lt_tab[ltnum].intensity;
     specular[2] = lt_tab[ltnum].specular[2]*lt_tab[ltnum].intensity;
     specular[3] = lt_tab[ltnum].specular[3];
     glLightfv(GL_LIGHT0+ltnum,GL_SPECULAR,specular);
/*
***Distant light or spot ? In any case, if follow_model is off,
***the position is interpreted as BASIC coordinates.
***Then set OpenGL's MODELVIEW rotation to zero before setting the light position.
***If follow_model is on, don't tuch the MODELVIEW matrix.
*/
     if ( tmode == 0 )
       {
       glGetFloatv(GL_MODELVIEW_MATRIX,gl_matrix);
       gl_matrix[ 0] = (GLfloat)1.0;
       gl_matrix[ 1] = (GLfloat)0.0;
       gl_matrix[ 2] = (GLfloat)0.0;
       gl_matrix[ 3] = (GLfloat)0.0;
       gl_matrix[ 4] = (GLfloat)0.0;
       gl_matrix[ 5] = (GLfloat)1.0;
       gl_matrix[ 6] = (GLfloat)0.0;
       gl_matrix[ 7] = (GLfloat)0.0;
       gl_matrix[ 8] = (GLfloat)0.0;
       gl_matrix[ 9] = (GLfloat)0.0;
       gl_matrix[10] = (GLfloat)1.0;
       gl_matrix[11] = (GLfloat)0.0;
       glMatrixMode(GL_MODELVIEW);
       glLoadMatrixf(gl_matrix);
       }
/*
***Now set the light position and enable it.
*/
     if ( lt_tab[ltnum].ang == 180.0 )
       {
       glLightfv(GL_LIGHT0+ltnum,GL_POSITION,lt_tab[ltnum].pos);
       }
     else
       {
       glLightfv(GL_LIGHT0+ltnum,GL_POSITION,lt_tab[ltnum].pos);
       glLightfv(GL_LIGHT0+ltnum,GL_SPOT_DIRECTION,lt_tab[ltnum].dir);
       glLightf(GL_LIGHT0+ltnum,GL_SPOT_CUTOFF,lt_tab[ltnum].ang);
       glLightf(GL_LIGHT0+ltnum,GL_SPOT_EXPONENT,lt_tab[ltnum].focus);
       glLightf(GL_LIGHT0+ltnum,GL_CONSTANT_ATTENUATION,1.0);
       glLightf(GL_LIGHT0+ltnum,GL_LINEAR_ATTENUATION,0.0);
       glLightf(GL_LIGHT0+ltnum,GL_QUADRATIC_ATTENUATION,0.0);
       }

     glEnable(GL_LIGHT0+ltnum);
/*
***Transformation mode. No transformation (static) or follow the model.
*/
     if ( tmode == 0 ) lt_tab[ltnum].follow_model = FALSE;
     else              lt_tab[ltnum].follow_model = TRUE;
     }
/*
***Turn light off.
*/
   else
     {
     glDisable(GL_LIGHT0+ltnum);
     lt_tab[ltnum].on = FALSE;
     }
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/********************************************************/

       short WPenable_lights(bool enable)

/*      Enables or disables all active light sources.
 *
 *      In: enable = TRUE->Enable, FALSE->Disable
 *
 *      (C)2008-01-21 J. Kjellander
 *
 ******************************************************!*/

  {
   int     i;

/*
***Loop through lights 0 to 7.
*/
   for ( i=0; i<8; ++i )
     {
     if ( lt_tab[i].defined && lt_tab[i].on )
       {
       if ( enable ) glEnable(GL_LIGHT0+i);
       else          glDisable(GL_LIGHT0+i);
       }
     }
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/********************************************************/

       short    WPget_light(
       int      ltnum,
       bool    *defined,
       bool    *on,
       DBfloat *intensity,
       bool    *follow_model)

/*      Returns light source data.
 *
 *      In: ltnum        = Lightsource number
 *          defined      = TRUE if defined
 *          on           = TRUE if on
 *          intensity    = MBS intensity (0-100)
 *          follow_model = TRUE if transformed with model
 *
 *      (C)2008-01-23 J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Get data from lt_tab[].
*/
  *defined      = lt_tab[ltnum].defined;
  *on           = lt_tab[ltnum].on;
  *intensity    = 100.0*lt_tab[ltnum].intensity;
  *follow_model = lt_tab[ltnum].follow_model;
/*
***The end.
*/
   return(0);
  }

/********************************************************/
