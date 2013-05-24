/*
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
 *    armin@faltl.com
 */
#ifndef CAMERA_H
#define CAMERA_H

#define CAMMODE_ROT_BIT     0x0001
#define QUAKE               0
#define DESCENT             1

#define CAMMODE_MAN_BIT     0x0002
#define CAMMODE_MANIPULATE  0
#define CAMMODE_FLYTROUGH   2

#define CAMMODE_PRJ_BIT     0x0004
#define PERSPECTIVE         0
#define ORTHOGONAL          4

/*
 * this structure is a means to store the
 * camera settings of different views
 */
typedef struct {
	GLfloat camX, camY, camZ,
			cenX, cenY, cenZ,
			altX, altY, altZ;
	double orthoRadius;
	int mode; 
} CameraSet_t;

#endif
