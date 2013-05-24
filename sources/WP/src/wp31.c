/**********************************************************************
*
*    wp31.c
*    ======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
*
*    This file includes:
*
*    wpinsh();     Init for shade
*    wpexsh();     Exit for shade
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

#include "../../DB/include/DB.h"

#ifdef V3_OPENGL
#include "../../IG/include/IG.h"
#include "../include/WP.h"

extern bool     rgb_gl;

static WPGWIN    *gwinpt; /* Fönsterpekare */
static GLXContext rc;     /* Motsvarande Rendering Context */


/*!******************************************************/

        short wpinsh(
        int    wid,
        double zmin,
        double zmax,
        bool   smooth)

/*      Initrutin för OpenGL-shading med X-Windows.
 *
 *      In: wid        = Fönsterid.
 *          zmin, zmax = Scenens storlek i Z-led.
 *          smooth     = FLAT/SMOOTH
 *
 *      Ut: Inget.
 *
 *      Felkoder: WP1623 = GLX-saknas.
 *                WP1633 = Ingen visual
 *                WP1643 = Ingen context
 *                WP1663 = Visual mismatch
 *
 *      (C)microform ab 1997-02-11 J. Kjellander
 *
 *      1997-06-11 Visualresurser, J.Kjellander
 *
 ******************************************************!*/

  {
   char              errbuf[V3STRLEN],tmpbuf[V3STRLEN];
   int               i,dummy;
   VisualID          vidwin,vidglx;
   XVisualInfo      *visinfo;
   XWindowAttributes wa;
   int               visatt[50];


/*
***Kolla att OpenGL supportas av X-servern.
*/
   if ( !glXQueryExtension(xdisp,&dummy,&dummy) )
     return(erpush("WP1623",""));
/*
***Fixa lämplig visual.
*/
   i = 0;

   if ( wpgrst("varkon.shade.rgba",tmpbuf)  &&
        strcmp(tmpbuf,"True") == 0 )
     {
     visatt[i++] = GLX_RGBA;
     visatt[i++] = GLX_RED_SIZE;
     visatt[i++] = 2;
     visatt[i++] = GLX_GREEN_SIZE;
     visatt[i++] = 2;
     visatt[i++] = GLX_BLUE_SIZE;
     visatt[i++] = 2;
     rgb_gl = TRUE;
     }
   else rgb_gl = FALSE;

   visatt[i++] = GLX_DEPTH_SIZE;
   visatt[i++] = 16;

   if ( wpgrst("varkon.shade.doublebuffer",tmpbuf)  &&
        strcmp(tmpbuf,"True") == 0 ) visatt[i++] = GLX_DOUBLEBUFFER;

   visatt[i]   = None;

   visinfo = glXChooseVisual(xdisp,xscr,visatt);

   if ( !visinfo )
     {
     if ( rgb_gl ) return(erpush("WP1633","RGB Mode"));
     else          return(erpush("WP1633","Color Index Mode"));
     }
/*
***Pekare till aktuellt grafiskt fönster.
*/
   if ( (gwinpt=(WPGWIN *)wpwtab[wid].ptr) == NULL ) return(-2);
/*
***Kolla att vald visual matchar det grafiska fönstrets visual.
*/
   XGetWindowAttributes(xdisp,gwinpt->id.x_id,&wa);

   vidwin = XVisualIDFromVisual(wa.visual);
   vidglx = XVisualIDFromVisual(visinfo->visual);

   if ( vidwin != vidglx )
     {
     sprintf(errbuf,"%d%%%d",(int)vidwin,(int)vidglx);
     return(erpush("WP1663",errbuf));
     }
/*
***Skapa rendering context.
*/
   if ( (rc=glXCreateContext(xdisp,visinfo,NULL,TRUE)) == NULL )
     return(erpush("WP1643",""));
/*
***Aktivera Rendering Contextet.
*/
   glXMakeCurrent(xdisp,gwinpt->id.x_id,rc);
/*
***Aktivera djuptest.
*/
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);
   glClearDepth((GLclampd)1.0);
/* 
***Transformation. I varkon kan zmin = zmax men OpenGL gillar
***det inte.
*/
   if ( zmax - zmin < 1e-3 )
     {
     zmax = zmax + 0.01;
     zmin = zmin - 0.01;
     }

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(gwinpt->vy.modwin.xmin,gwinpt->vy.modwin.xmax,
              gwinpt->vy.modwin.ymin,gwinpt->vy.modwin.ymax,
              -zmax,-zmin);
/*
***Viewport. Den del av det grafiska fönstret som får användas.
***Eventuella marginaler för knappar skall ej skrivas över.
*/
   glScissor((GLint)gwinpt->vy.scrwin.xmin,(GLint)gwinpt->vy.scrwin.ymin,
             (GLint)(gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin),
             (GLint)(gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin));
   glEnable(GL_SCISSOR_TEST);

   glViewport((GLint)gwinpt->vy.scrwin.xmin,(GLint)gwinpt->vy.scrwin.ymin,
              (GLint)(gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin),
              (GLint)(gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin));
/*
***Låt positiv normal definieras av moturs riktning precis
***som i Varkon.
*/
   glFrontFace(GL_CCW);
/*
***Configurera aktiva ljuskällor.
*/
   wpconl(gwinpt);
/*
***Normalisera ytnormaler så att färgstyrka
***beräknas på rätt sätt.
*/
   glEnable(GL_NORMALIZE);
/*
***Smooth shading kan ha betydelse om man använder
***spitlights.
*/
   if ( smooth ) glShadeModel(GL_SMOOTH);
   else          glShadeModel(GL_FLAT);
/*
***Sudda färgbuffert och djupbuffert.
*/ 
   glClearColor((GLclampf)1.0,(GLclampf)1.0,(GLclampf)1.0,(GLclampf)1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short wpexsh()

/*      Exitrutin för OpenGL-shading.
 *
 *      FV: 0.
 *
 *      (C)microform ab 1997-02-12 J. Kjellander
 *
 *      1997-04-09 Save-under, J.Kjellander
 *      1997-06-11 Dubbelbuffring, J.Kjellander
 *
 ******************************************************!*/

  {
   char tmpbuf[V3STRLEN];

/*
***Flusha OpenGL.
*/
   if ( wpgrst("varkon.shade.doublebuffer",tmpbuf)  &&
        strcmp(tmpbuf,"True") == 0 )
     {
     glXSwapBuffers(xdisp,gwinpt->id.x_id);
     }
   else
     {
     glFlush();
     }
/*
***Stäng OpenGL.
*/
   glXMakeCurrent(xdisp,None,NULL);
   glXDestroyContext(xdisp,rc);
/*
***Kopiera fönstrets innehåll till "save-under"-
***bitmappen.
*/
   XCopyArea(xdisp,gwinpt->id.x_id,gwinpt->savmap,gwinpt->win_gc,
             0,0,gwinpt->geo.dx,gwinpt->geo.dy,0,0);

   return(0);
  }

/********************************************************/
#else
short  wpinsh(int wid, double zmin, double zmax, bool smooth)  {return(0);}
short  wpexsh()                                                {return(0);}
#endif

