/*
 * This is a driver routine, that creates a printable raster image in
 * TIFF-format from a scene via OpenGL frame buffer rendering.
 * The output is a compressed halftone grey image with 8 bits per pixel,
 * because my printer happens to like it and it's relatively small.
 *
 * Memory requirements are determined by the resolution, total width
 * and height of a single rendering frame, as one row of frames
 * has to be stored for conversion into TIFF-stripes or scanlines.
 *
 * Preferably the GL-back buffer or an auxiliary buffer are used,
 * to avoid artefacts on the visible screen.
 *
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

#include "gl_print.h"
/* #include <GL/gl.h> */
#include <tiffio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*#define AJF_DEBUG*/

/*#define MAX_BUFFER	8388608	/* 2^23, 2^24 = 16777216 */


int gl_plot(void (*draw)(void *hdl),	/* drawing callback */
		    void *drawHdl,
			GLdouble mdvwMatrix[16],
			CameraSet_t *cs,
			int deviceType,	/* type of output device */
			double dpi, 	/* resolution, dpi */
			double width,	/* at the front, millimeters */
			double height,	/*           "               */
			double nearVal,	/* millimeters, front depth of frustum */
			double farVal,	/* millimeters, back depth of frustum */
			char *fileName,
			char *softwareName)
	/*
	 * Consider: the values defining the frustum must be in millimeters
	 * and not in drawing units, as the raster output must fit on paper
	 * with the specified resolution.
	 *
	 * If you want your near clipping plane at a distance, that doesn't
	 * fit for your papersize you can achieve the desired result with
	 * a "wrong" dpi-value preserving the constant k = nearVal * dpi.
	 *
	 * RETURN:
	 * 0 ... everything OK
	 * 1 ... couldn't open TIFF-file
	 * 2,3 . couldn't allocate buffers
	 *
	 * ToDo:
	 * -> portrait/landscape (re)orientation and corresponding 90° rotation
	 * -> different color output modes
	 * -> camera decentered to enable multi-page output
	 */
{
	GLint maxView[2], maxViewWdt, maxViewHgt;
	GLsizei viewWdt, viewHgt;
	int i, j, k, hpix, vpix, pixMemSize,
		horizontalFrames, verticalFrames, frameOffset;
	double pixSize, x, y, z, distance;
	unsigned int rowCnt;
	GLenum format, type;
	GLdouble left, right, top, bottom;
	char *pixBuffer, *pixBuffPtr, *scanBuffer, *scanBuffPtr;
	TIFF *imgHdl;

#ifdef AJF_DEBUG
	printf("gl_print() called.\n");
#endif

	/* set the view dimensions to reasonable, memory-limited defaults */
	maxViewWdt = GLRASTER_FRAME_WIDTH;
	maxViewHgt = GLRASTER_FRAME_HEIGHT;
	/*
	maxViewWdt = 2048;
	maxViewWdt = 2043; // use an odd number for debugging
	maxViewWdt = 1465; // use an odd number for debugging, 3 h-frames
	*/

	/* compute the image-, frame- and stripe dimensions */
	hpix = ceil(dpi * width  / 25.4);
	vpix = ceil(dpi * height / 25.4);
#ifdef AJF_DEBUG
	printf("Image size [mm2] = %f, %f; resolution [dpi] = %f\n "
		   "horizontal pixels: %d\n"
		   "  vertical pixels: %d\n",
		   width, height, dpi,
		   hpix, vpix);
#endif
	if (maxViewWdt > hpix) maxViewWdt = hpix;
	if (maxViewHgt > vpix) maxViewHgt = vpix;

	glGetIntegerv(GL_MAX_VIEWPORT_DIMS, maxView);
#ifdef AJF_DEBUG
	printf("maxView = (%d, %d)\n", maxView[0], maxView[1]);
#endif
	if (maxViewWdt > maxView[0]) maxViewWdt = maxView[0];
	if (maxViewHgt > maxView[1]) maxViewHgt = maxView[1];

	maxViewWdt = (maxViewWdt / 8) * 8;	/* reduce the maximum dimensions of the */
	maxViewHgt = (maxViewHgt / 8) * 8;	/* viewport to a multiple of 8          */
#ifdef AJF_DEBUG
	printf("maxViewWdt = %d, maxViewHgt %d\n", maxViewWdt, maxViewHgt);
#endif

	horizontalFrames = hpix / maxViewWdt;
	if (hpix % maxViewWdt) horizontalFrames++;
	verticalFrames   = vpix / maxViewHgt;
	if (vpix % maxViewHgt) verticalFrames++;
	switch (deviceType) {
	case GLRASTER_TIFF_FILE_GREYSCALE:
		pixMemSize = 1; /* a pixel happens to use one byte */
		break;
	case GLRASTER_TIFF_FILE_COLOR:
		pixMemSize = 3;
		break;
	}
	frameOffset = pixMemSize * maxViewWdt * maxViewHgt;
#ifdef AJF_DEBUG
	printf("horizontalFrames = %d, verticalFrames = %d, frameOffset = %d\n",
			horizontalFrames, verticalFrames, frameOffset);
#endif

	/* allocate the client-frame buffer */
	pixBuffer = malloc(frameOffset * horizontalFrames + 4096);
				/* extra page for debugging? */
	if (pixBuffer == NULL) return 2;

	scanBuffer = malloc(pixMemSize * hpix + 4096);
	if (scanBuffer == NULL) {
		free(pixBuffer);
		return 3;
	}

	/* open the TIFF-image file and set its characteristics */
	imgHdl = TIFFOpen(fileName, "w");
	if (imgHdl == 0) return 1;
	TIFFSetField(imgHdl, TIFFTAG_SOFTWARE, softwareName);
	TIFFSetField(imgHdl, TIFFTAG_BITSPERSAMPLE, 8);
	switch (deviceType) {
	case GLRASTER_TIFF_FILE_GREYSCALE:
		TIFFSetField(imgHdl, TIFFTAG_SAMPLESPERPIXEL, 1);
		TIFFSetField(imgHdl, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
		break;
	case GLRASTER_TIFF_FILE_COLOR:
		TIFFSetField(imgHdl, TIFFTAG_SAMPLESPERPIXEL, 3);
		TIFFSetField(imgHdl, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
		break;
	}
	TIFFSetField(imgHdl, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(imgHdl, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(imgHdl, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
	TIFFSetField(imgHdl, TIFFTAG_XRESOLUTION, (float)dpi);
	TIFFSetField(imgHdl, TIFFTAG_YRESOLUTION, (float)dpi);
	TIFFSetField(imgHdl, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
			/* ev. COMPRESSION_NONE = what the name says
			 * or  COMPRESSION_CCITTRLE = CCITT modified Huffman RLE
			 * or  COMPRESSION_PACKBITS = Macintosh RLE
			 * or  COMPRESSION_IT8MP = ANSI IT8 Monochrome picture
			 */ 

	TIFFSetField(imgHdl, TIFFTAG_IMAGEWIDTH, hpix);
	TIFFSetField(imgHdl, TIFFTAG_IMAGELENGTH, vpix);

	/*
	 * setup the GL drawing context
	 */
	glClearColor(1.0, 1.0, 1.0, 1.0);	/* set the background to white */
	glDrawBuffer(GL_BACK);		/* select the buffer to draw to,
								 * ev. use GL_AUX0 or GL_BACK_LEFT */

	glReadBuffer(GL_BACK);		/* select the buffer to read from,
								 * ev. use GL_AUX0 */
	switch (deviceType) {
	case GLRASTER_TIFF_FILE_GREYSCALE:
		format = GL_LUMINANCE;	/* create a greyscale image */
		type   = GL_UNSIGNED_BYTE;
		glPixelTransferf(GL_RED_SCALE, 0.299);		/* values recommended by NTSC */
		glPixelTransferf(GL_GREEN_SCALE, 0.587);	/*         -"-                */
		glPixelTransferf(GL_BLUE_SCALE, 0.114);		/*         -"-                */
		break;
	case GLRASTER_TIFF_FILE_COLOR:
		format = GL_RGB;		/* create a color image */
		type   = GL_UNSIGNED_BYTE;
		glPixelTransferf(GL_RED_SCALE, 1.0);		/* identity factor */
		glPixelTransferf(GL_GREEN_SCALE, 1.0);		/*      -"-        */
		glPixelTransferf(GL_BLUE_SCALE, 1.0);		/*      -"-        */
		break;
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);		/* avoid 4-byte alignment (Client to FB) */
	glPixelStorei(GL_PACK_ALIGNMENT, 1);		/*         -"-            (FB to Client) */

	glEnable(GL_DEPTH_TEST);

	/*
	 * Size of a pixel and top-left corner in device coordinates.
	 *
	 * The parameter 'orthoRadius' is the visible radius of the scene
	 * at the depth of the 'scene center point'. It is meant to always fit
	 * into the vertical extent of the display medium.
	 */
	x = cs->camX - cs->cenX;
	y = cs->camY - cs->cenY;
	z = cs->camZ - cs->cenZ;
	distance = sqrt(x*x + y*y + z*z);
	if (cs->mode & CAMMODE_PRJ_BIT) {
		/* orthogonal projection */
		pixSize = 2.0 * cs->orthoRadius / (dpi * height / 25.4);
		top = cs->orthoRadius;
	} else {
		/* perspective projection */
		pixSize = 2.0 * cs->orthoRadius * nearVal
				/ ((dpi * height / 25.4) * distance);
		top = cs->orthoRadius * nearVal / distance;
	}

	/* Compute the stripes of the image by re-defining the frustum
	 * and rendering the scene repeatedly, output to the raster-file.
	 * As the rasterimage starts at top-left, so do our render buffers */
	rowCnt = 0;
	for (i = 0; i < verticalFrames; i++)
		/* process a row of frames to (a) TIFF-stripe(s)*/
	{
		if (i < verticalFrames - 1 || vpix % maxViewHgt == 0)
			viewHgt = maxViewHgt;
		else
			viewHgt = vpix % maxViewHgt;
		bottom = top - pixSize * viewHgt;
		if (cs->mode & CAMMODE_PRJ_BIT) {
			left  = -cs->orthoRadius * width / height;
		} else {
			left  = -cs->orthoRadius * width * nearVal / (height * distance);
		}
		pixBuffPtr = pixBuffer;
		for (j = 0; j < horizontalFrames; j++)
			/* process a single frame */
		{
			if (j < horizontalFrames - 1 || hpix % maxViewWdt == 0)
				viewWdt = maxViewWdt;
			else
				viewWdt = hpix % maxViewWdt;
			right = left + pixSize * viewWdt;

			/* render the scene */
#ifdef AJF_DEBUG
			printf("i = %d, j = %d\n   viewWdt = %d, viewHgt = %d\n"
				   "    left = %f, right = %f, bottom = %f, top = %f\n",
				   i, j, viewWdt, viewHgt,
				   left, right, bottom, top);
			fflush(stdout);
#endif
			glViewport(0, 0, viewWdt, viewHgt);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			if (cs->mode & CAMMODE_PRJ_BIT) {
				/* orthogonal projection */
				glOrtho(left,
						right,
						bottom,
						top,
						nearVal,	/* near (OpenGL reverses Z-distance...) */
						farVal);	/* far clipping plane */
			} else {
				/* perspective projection -
				 * the first four coords and the near plane build
				 * the near corners of the view pyramid (frustum) */
				glFrustum(left,
						  right,
						  bottom,
						  top,
						  nearVal,
						  farVal);
			}

	        glMatrixMode(GL_MODELVIEW);
			glLoadMatrixd(mdvwMatrix);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			draw(drawHdl);

#ifdef AJF_DEBUG
			/* for debugging, preset the application buffer to a dark grey */
			memset(pixBuffPtr, 0x44, frameOffset);
#endif

			/* read back from the frame buffer */
			glReadPixels(0, 0, viewWdt, viewHgt, format, type, pixBuffPtr);

                    
			
#ifdef AJF_DEBUG
			/*JK assuming it's color*/
			{
				char *ptr;
				int  cnt = 0;
				ptr = pixBuffPtr + pixMemSize * viewWdt * viewHgt - 1;
				while (ptr >= pixBuffPtr && *ptr == 0x44) {
					cnt++;
					ptr--;
				}
				printf("count of 0x44-bytes is %d\n", cnt);
			}
#endif

			pixBuffPtr += frameOffset;
			left = right;
		}

		/* combine the rows of adjecent frames to image-scanlines and
		 * write them to the TIFF-file */
		for (k = viewHgt - 1; k >= 0; k--) /* GL-frames start at bottom */
		{
#ifdef AJF_DEBUG
			/* for debugging, preset the application buffer to a medium grey */
			memset(scanBuffer, 0xbb, hpix);
#endif

			scanBuffPtr = scanBuffer;
			for (j = 0; j < horizontalFrames; j++) {
				if (j < horizontalFrames - 1 || hpix % maxViewWdt == 0)
					viewWdt = maxViewWdt;
				else
					viewWdt = hpix % maxViewWdt;
				memcpy(scanBuffPtr,
					   pixBuffer + j * frameOffset + k * pixMemSize * viewWdt,
					   pixMemSize * viewWdt);
#ifdef AJF_DEBUG
				/* for debugging reasons, outline the frame borders in grey */
				*scanBuffPtr = 0x88;
				if (k == 0) memset(scanBuffPtr, 0x88, pixMemSize * viewWdt);
				/* end debug-borderlines */
#endif
				scanBuffPtr += pixMemSize * viewWdt;
			}
			TIFFWriteScanline(imgHdl, scanBuffer, rowCnt++, 0);
		}

		top = bottom;
	}
	TIFFClose(imgHdl);

	free(scanBuffer);
	free(pixBuffer);
	return 0;
}/* gl_print() */



/* see tiff.h for the definitions of available macro constants
 *


 	TIFFSetField(imgHdl, TIFFTAG_STRIPBYTECOUNTS, XXXX);
 	TIFFSetField(imgHdl, TIFFTAG_ROWSPERSTRIP, XXXX);
 	TIFFSetField(imgHdl, TIFFTAG_STRIPBYTECOUNTS, XXXX);
 	TIFFSetField(imgHdl, TIFFTAG_STRIPBYTECOUNTS, XXXX);

	TIFFSetField(imgHdl, TIFFTAG_DOCUMENTNAME, char *);
	TIFFSetField(imgHdl, TIFFTAG_IMAGEDESCRIPTION, char *);
	TIFFSetField(imgHdl,);
	RowsPerStrip
		SamplesPerPixel
		ResolutionUnit
		Software
		Orientation
		Compression
		DocumentName
		PageName
		PageNumber
		PlanarConfiguration
		RowsPerStrip
		SampleFormat
		StripByteCounts
*/

		/* convert the tile information created by the frames to
		 * stripes suitable for a striped TIFF format (to avoid trouble
		 * with most applications, that can't read tiled images) */
		/*
		for (j = 0; j < stripesPerFrame; j++) {
			
			tsize_t TIFFWriteEncodedStrip(TIFF *tif, tstrip_t strip,
										  tdata_t buf, tsize_t size)
			TIFFWriteEncodedStrip();
		}
		*/

/*
	printf("TEST-RETURN from gl_print\n");
	return 10;
*/
