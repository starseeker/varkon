/**********************************************************************
*
*    wpprint_GL.c
*    ============
*
*    This file is part of the VARKON WindowPac library.
*    URL: http://www.varkon.com
*
*    WPprint_GL();    Print a WPGWIN
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
**********************************************************************/

#ifdef V3USETIFF
/*
***Build the system with TIFF support.
*/
#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../WP/include/WP.h"

/*
***gl_print.h is the function prototype for gl_print();
*/
#include "../GL_to_TIFF/gl_print.h"

extern V3MDAT sydata;
extern char   jobdir[],jobnam[],svnversion[];

/*
***Prototypes for internal functions.
*/
static void draw_callback(void *data);
static void get_modelview(WPRWIN *rwinpt,GLdouble matrix[]);

/********************************************************/

         short WPprint_GL(WPRWIN *rwinpt)

/*      A first test of Armins TIFF creator. Called
 *      from the button handler of WPRWIN as f150.
 *      WPGWIN currently also uses f150 but calls
 *      WPprint_dialog(). This will be fixed later.
 *
 *      Note: Most of the comments are derived from
 *            an e-mail conversation with Armin.
 *
 *      In: rwinpt = C ptr to the WPRWIN to be printed
 *
 *      (C)2008-02-06 J.Kjellander
 *
 ********************************************************/

  {
   short       status;
   int         i,plot_status,npix_x,npix_y;
   double      dpi,width,height,mdz,fd,nearVal,farVal;
   GLdouble    mdvwMatrix[16];
   CameraSet_t cs;
   char        errbuf[V3STRLEN],destination[V3PTHLEN],
               mesbuf[V3PTHLEN],outpath[V3PTHLEN],
               outfile[JNLGTH],tiff_tag[V3STRLEN];

   Window               xwin_id;
   XSetWindowAttributes xwina;
   unsigned long        xwinm;

/*
***Get the TIFF file destination. Default directory
***is set to the current job directory and default
***name to the current job basename.
*/
   strncpy(outpath,jobdir,V3PTHLEN);
start:
   status = WPfile_selector("TIFF file destination",
                            outpath,
                            TRUE,
                            jobnam,
                            "*.TIFF",
                            outfile);

   if ( status == REJECT ) return(REJECT);
/*
***Strip possible trailing ".TIFF".
*/
   if ( IGcmpw("*.TIFF",outfile) )
     {
     i = strlen(outfile) - 5;
     outfile[i] = '\0';
     }
/*
***Add path + file + ".TIFF" to a complete destination.
*/
   strncpy(destination,outpath,V3PTHLEN);
   strncat(destination,outfile,JNLGTH);
   strncat(destination,".TIFF",5);
/*
***Check if file already exists.
*/
   if ( IGftst(destination) && !IGialt(76,67,68,TRUE) ) goto start;
/*
***Set up an environment for Armin Faltl's TIFF creator.
*
***1. Drawing callback. Use draw_callback() and supply a ptr to the WPRWIN as input.
*
***2. Data for callback. Use a ptr to the WPRWIN.
*
***3. Get the mdvwMatrix[] from GL. This works as long as
***   we want to print what we see in the window.
*/
   get_modelview(rwinpt,mdvwMatrix);
/*
***4. The camera is defined by:
*
***The position of the camera in model space (camX, camY, camZ)
***The point in model space that is in the middle of the scene,
***(cenX, cenY, cenZ)
***The direction in model space, that shall be upwards in the
***image (altX, ... like altitude)
***A measure for the width or height of the scene: orthoRadius is the length
***in model space, orthogonal to the view direction, from the scene center
***to the upper (or lower) edge of the display.
***Mode describes several handling flags for the camera. Relevant in this
***context is only (mode & CAMMODE_PRJ_BIT). If this bit is set
***(see #define ORTHOGONAL), then the camera is at infinity instead of
***(camX, camY, camZ), but the vectors of the view- and up  direction
***are still valid, as is the height of scene in model space.
***To be valid, (altX, altY, altZ) must not be colinear to (cenX - camX,
***cenY - camY, cenZ - camZ).
*
***The bounding box of the model in this window is rwinpt->xmin,xmax etc.
***Set the camera at infinite Z, the center in the midle of the bounding
***box, with Y as up_vector. Use the size of the box in the Z-direction
***to calculate a radius. A WPRWIN uses different sizes in X,Y and Z !
*/
   cs.camX = cs.camY = 0.0; cs.camZ = 1.0;
   cs.cenX = (rwinpt->xmin + rwinpt->xmax)*0.5;
   cs.cenY = (rwinpt->ymin + rwinpt->ymax)*0.5;
   cs.cenZ = (rwinpt->zmin + rwinpt->zmax)*0.5;
   cs.altX = cs.altZ = 0.0; cs.altY = 1.0;
   cs.orthoRadius = rwinpt->scale*(rwinpt->zmax - rwinpt->zmin)/2.0;
   cs.mode = CAMMODE_PRJ_BIT;
/*
***5. Device type. Use GLRASTER_TIFF_FILE_GREYSCALE to create TIFF file.
*
***6. dpi is the resolution. 300.0 is a good start according
***   to Armin.
*/
   dpi = 300.0;
/*
***7. width and height
*
***This is mm on paper. The frustum is solely defined in model space
***units. There is a plane, where the mm-value is identical to the width
***in model units, but it's usually not the near or far plane of the frustum.
***This also applies to the "total frustum" of the scene, the frustums
***actually used in the driver are the portions of this pyramide(-stump) that
***correlate to the sub-images (frames) created in one render & read-back pass.
***The connection is built by the combination of a computed pixel-size
***in model-units and the viewport-transformation (pseudocode):
*
*** image_edge_length[pixes] = height[mm] / 25.4 * dpi;
*
*** viewport_height = frame_length[pixels];
*** up to programmer, 1024 in the implemetation
*
*** => pixel_size[model-units] = scene_height_at_near_plane[model-units]
*                               / image_edge_length[pixels];
*** computed differently because of rounding to pixels
*
***The scene_height_at_near_plane is computed from 'orthoRadius' and the relation
***of nearVal and the camera-point to scene-center distance.
***The rounding mentioned above is because pixels must be a natural number,
***so I choose to compute the image max. 1 pixel larger than the exact mm-value
***while preserving the exact scale. The top-left corner of the image is exact,
***and the scene center is projected exactly to the center of the specified
***rectangle on paper, while the raster image may extend a fraction of a pixel
***to the lower-right and the rectangle-center will normally not be a pixel
***center or -border. width has same properties as height.
*
***This is the desired size of the image in millimeters on paper ?
***Independant of size, it is reasonable to give the image the same
***proportions as on the screen. Maybe even the same size ?
*/
   width  = rwinpt->geo.psiz_x *
            (rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin);
   height = rwinpt->geo.psiz_y *
            (rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin);
/*
***8. nerarVal and farVal
*
***These are near and far limit in model units, because this influences
***the depth buffer computation and the clipping of the scene.
***They augment the values in cs and have the same unit.
***In the application, nearVal and farVal are computed from the
***camera - scene center distance, so this could be repeated in the
***driver and the parameters omitted. But since the method of this
***computation may change outside the driver, e.g. because the near
***plane can be used as clipping plane by the user to cut the model
***to see interior parts, I choose to make them explicit parameters.
***If above possible change is realized one will want to store the
***position of the front and back frustum culling planes in saved
***views which would mean, they become part of the CameraSet_t
***structure.
*
***This is the position of the perspective in Z. Same computations
***here as for WPRWIN's.
*/
   mdz = rwinpt->zmax - rwinpt->zmin;
   fd = mdz + 0.2*(100.0-rwinpt->pfactor)*mdz;

   nearVal = fd - 0.5*mdz;
   farVal  = fd + 0.5*mdz;
/*
***9. Filename = destination from above.
*/

/*
***Since the plot resolution is usually higher than the display resolution
***the WPRWIN might not have pixels enough for OpenGL to render so we
***create a simplistic unmapped X window with enough pixels to render the
***scene with the print resolution into it (+ 1 pix to be sure !)
***Armins TIFF creator divides the image into sub images with a fixed
***size and renders each sub image separately. The size of a sub image
***is defined in gl_print.h
*/
   npix_x = GLRASTER_FRAME_WIDTH;
   npix_y = GLRASTER_FRAME_HEIGHT;
/*
***Create an unmapped window with the same colormap and visual as the
***WPRWIN so that it can be used in the same OpenGL rendering context.
*/
   xwina.colormap = rwinpt->colmap;
   xwinm =          CWColormap;

   xwin_id = XCreateWindow(xdisp,
                           rwinpt->id.x_id,
                           0,0,
                           npix_x,npix_y,
                           1,
                           rwinpt->pvinfo->depth,
                           InputOutput,
                           rwinpt->pvinfo->visual,
                           xwinm,
                          &xwina);
/*
***Activate the OpenGL RC in this window.
*/
   glXMakeCurrent(xdisp,xwin_id,rwinpt->rc);
/*
***Turn off Varkon scissor test during plot.
*/
   glDisable(GL_SCISSOR_TEST);
/*
***Tiff tag. A text that is stored in the TIFF file indicating
***the software used to create the file.
*/
   sprintf(tiff_tag,"Varkon %d.%d%c svn#%s using gl_plot() by A.Faltl",
                    sydata.vernr,
                    sydata.revnr,
                    sydata.level,
                    svnversion);
/*
***Call the TIFF creator.
*/
   plot_status = gl_plot(draw_callback,
                         rwinpt,
                         mdvwMatrix,
                        &cs,
                         GLRASTER_TIFF_FILE_COLOR,
                         dpi,
                         width,
                         height,
                         nearVal,
                         farVal,
                         destination,
                         tiff_tag);
/*
***Activate the WPRWIN in the GL Rendering Context again
***so that the RC is valid after we delete the X window.
*/
   glXMakeCurrent(xdisp,rwinpt->id.x_id,rwinpt->rc);
/*
***The temporary X window is now not used anymore and can be deleted.
*/
   XDestroyWindow(xdisp,xwin_id);
/*
***gl_plot() changes the Viewport of the WPRWIN so we must reset it now.
***Also, clear the buffer (margins) and turn on scissor test again.
*/
   glViewport((GLint)rwinpt->vy.scrwin.xmin,(GLint)rwinpt->vy.scrwin.ymin,
              (GLint)(rwinpt->vy.scrwin.xmax - rwinpt->vy.scrwin.xmin),
              (GLint)(rwinpt->vy.scrwin.ymax - rwinpt->vy.scrwin.ymin));
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_SCISSOR_TEST);
/*
***Status processing after gl_plot().
*/
   switch ( plot_status )
     {
     case 0:
     status = 0;
     sprintf(mesbuf,"%s created !",destination);
     WPaddmess_mcwin(mesbuf,WP_MESSAGE);
     break;

     case 1:
     status = erpush("WP1752",destination);
     break;

     case 2:
     case 3:
     sprintf(errbuf,"%d",plot_status);
     status = erpush("WP1763",errbuf);
     break;

     default:
     sprintf(errbuf,"%d",plot_status);
     status = erpush("WP1773",errbuf);
     break;
     }
/*
***The end.
*/
   return(status);
  }

/********************************************************/
/********************************************************/

 static void     get_modelview(
        WPRWIN  *rwinpt,
        GLdouble gl_matrix[])

/*      Sets up a MODELVIEW matrix for printing.
 *
 *      (C)2008-02-02 J.Kjellander
 *
 ********************************************************/

  {
   double   mdx,mdy,mdz,midx,midy,midz,fd;

/*
***The size of the model bounding box.
*/
   mdx = rwinpt->xmax - rwinpt->xmin;
   mdy = rwinpt->ymax - rwinpt->ymin;
   mdz = rwinpt->zmax - rwinpt->zmin;
/*
***Focal distance and thus frustum position in Z depends
***on current perspective.
*/
   fd = mdz + 0.2*(100.0-rwinpt->pfactor)*mdz;
/*
***Model box mid point.
*/
   midx = rwinpt->xmin + 0.5*mdx;
   midy = rwinpt->ymin + 0.5*mdy;
   midz = rwinpt->zmin + 0.5*mdz;
/*
***Create model transformation.
*/
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
/*
***Translate model based on current WPRWIN pan
***and perspective.
*/
   glTranslated(-rwinpt->movx,-rwinpt->movy,-fd);
/*
***Rotate the model with the current WPRWIN 3X3 rotation
***matrix.
*/
   glGetDoublev(GL_MODELVIEW_MATRIX,gl_matrix);
   gl_matrix[ 0] = (GLdouble)rwinpt->vy.matrix.k11;
   gl_matrix[ 1] = (GLdouble)rwinpt->vy.matrix.k21;
   gl_matrix[ 2] = (GLdouble)rwinpt->vy.matrix.k31;
   gl_matrix[ 3] = 0.0;
   gl_matrix[ 4] = (GLdouble)rwinpt->vy.matrix.k12;
   gl_matrix[ 5] = (GLdouble)rwinpt->vy.matrix.k22;
   gl_matrix[ 6] = (GLdouble)rwinpt->vy.matrix.k32;
   gl_matrix[ 7] = 0.0;
   gl_matrix[ 8] = (GLdouble)rwinpt->vy.matrix.k13;
   gl_matrix[ 9] = (GLdouble)rwinpt->vy.matrix.k23;
   gl_matrix[10] = (GLdouble)rwinpt->vy.matrix.k33;
   gl_matrix[11] = 0.0;
   glLoadMatrixd(gl_matrix);

   glRotated(rwinpt->rotx,gl_matrix[0],gl_matrix[4],gl_matrix[8] );
   glRotated(rwinpt->roty,gl_matrix[1],gl_matrix[5],gl_matrix[9] );
/*
***Here is the 1:st transformation. Model
***translated back to the origin.
*/
   glTranslated(-midx,-midy,-midz);
/*
***Get the result.
*/
   glGetDoublev(GL_MODELVIEW_MATRIX,gl_matrix);
/*
***The end.
*/
   return;
  }

/********************************************************/
/********************************************************/

 static void draw_callback(void *data)

/*      The draw callback.
 *
 *      In: data = C ptr to the WPRWIN.
 *
 *      (C)2008-01-27 J.Kjellander
 *
 ********************************************************/

  {

/*
***Execute the main list.
*/
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glCallList((GLuint)1);
   glFlush();
/*
***The end.
*/
   return;
  }

/********************************************************/

/*
***If TIFF is not enabled in the makefiles we supply
***this dummy instead.
*/
#else
   short WPprint_GL(char *rwinpt) {return(erpush("WP1823",""));}
#endif
