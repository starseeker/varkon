/*
 * This is a driver to use the OpenGL redering system of a machine
 * to create a raster image in TIFF-format ready for printing.
 *
 * Copyright 2007, 2008 by Dipl.Ing. Armin Faltl
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 *
 * Please report all bugs and problems to the following address:
 *
 *     armin@faltl.com
 */

#ifndef GL_PRINT_H
#define GL_PRINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <GL/gl.h>
#include "camera.h"

#define GLRASTER_TIFF_FILE_GREYSCALE	1
#define GLRASTER_TIFF_FILE_COLOR	2
/*
#define GLRASTER_PCL_PIPE				3
#define GLRASTER_TIFF_PIPE_GREYSCALE	4
#define GLRASTER_TIFF_PIPE_COLOR		5
#define GLRASTER_PNG_FILE	6
*/

int gl_plot(void (*draw)(void* hdl),		/* drawing callback */
			void *drawHdl,					/* data handle for callback */
			GLdouble mdvwMatrix[16],		/* model-view matrix for initialization */
			CameraSet_t *cs,
			int deviceType,
			double dpi,    /* resolution, dpi */
			double width,  /* at the front, millimeters */
			double height, /*           "               */
			double nearVal,    /* millimeters, front depth of frustum */
			double farVal,     /* millimeters, back depth of frustum */
			char *fileName,
			char *softwareName);

/*
 * The following definitions are here, to set the size of the internal
 * rasterization sub frames. Change them, if you want to tweak buffer
 * sizes vs. required rendering passes for a given output size.
 * In order for the changes to take effect, gl_print.c must be recompiled.
 * The height has linear influence on the biggest buffer, while width
 * should be set to a value reasonable for your common image widths.
 * The influence of width on buffer size is via block-effect: a large
 * value gives coarse blocks, that may fit rather bad to your image,
 * while a small value always leads to more rendering passes.
 * Use multiples of 8 for the values.
 * To get correct results, you should use the values for the size of
 * an unmapped (background-) rendering window for your graphics context.
 */
#define GLRASTER_FRAME_WIDTH	256
#define GLRASTER_FRAME_HEIGHT	256

#ifdef __cplusplus
}
#endif

#endif
