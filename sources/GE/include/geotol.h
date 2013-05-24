/**********************************************************************
*
*    geotol.h
*    ========
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
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
***********************************************************************/

/*
***These tolerances are used by most of the geoxxx()-routines.
***Work is in progress to replace these with the tolerances
***defined in suinit.c
*/

#define  TOL1 0.0001  /* Basic tolerance #defined by the computer
                         and the size of the current object */
#define  TOL2 0.001   /* Criterion for identical points in R*3 */

#define  TOL3 0.00001 /* Criterion for identical derivatives in R*3 */

#define  TOL4 0.0001  /* Criterion for identical points in parameter space */

#define  TOL5 0.1     /* Not yet #defined */

#define  TOL6 50000.0 /* Not yet #defined */

#define  TOL7 0.001   /* Criterion for identical angels (in degrees) */

#define  COMPTOL 0.0000000000001 /* Computational accuracy */

/**********************************************************************/
