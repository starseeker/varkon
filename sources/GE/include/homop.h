/**********************************************************************
*
*    homop.h
*    =======
*
*    This file is part of the VARKON Geometry Library.
*    URL: http://www.varkon.com
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
*    (C)Microform AB 1984-1999, Gunnar Liden, gunnar@microform.se
*
***********************************************************************/


/*!*********************** HOMOP **********************************!*/
/*!                                                                !*/
/*! Homogenous coordinates for a point (4 dimensional point)       !*/
/*! -------------------------------------------------------------- !*/
/*!                                                                !*/
/*! Homogenous point P= (  X  ,  Y  ,  Z  ,  w  )                  !*/
/*! Cartesian  point p= ( X/w , Y/w , Z/w )                        !*/
/*!                                                                !*/
/*!                                                                !*/

typedef struct
{
gmflt X;                          /* Homogenous coordinate X        */
gmflt Y;                          /* Homogenous coordinate Y        */
gmflt Z;                          /* Homogenous coordinate Z        */
gmflt w;                          /* Homogenous coordinate w        */
} HOMOP;

/*!*********************** HOMOP **********************************!*/
