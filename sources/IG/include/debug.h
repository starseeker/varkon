/*!******************************************************************/
/*  File: debug.h                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.varkon.com                                     */
/*                                                                  */
/*  This library is free software; you can redistribute it and/or   */
/*  modify it under the terms of the GNU Library General Public     */
/*  License as published by the Free Software Foundation; either    */
/*  version 2 of the License, or (at your option) any later         */
/*  version.                                                        */
/*                                                                  */
/*  This library is distributed in the hope that it will be         */
/*  useful, but WITHOUT ANY WARRANTY; without even the implied      */
/*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR         */
/*  PURPOSE.  See the GNU Library General Public License for more   */
/*  details.                                                        */
/*                                                                  */
/*  You should have received a copy of the GNU Library General      */
/*  Public License along with this library; if not, write to the    */
/*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge,        */
/*  MA 02139, USA.                                                  */
/*                                                                  */
/*  (C)Microform AB 1984-2000, Johan Kjellander, johan@microform.se */
/*                                                                  */
/********************************************************************/


/*
***Constants used by debug-functions 
*/
#define IGEPAC  0          /* Code for IG     */
#define GRAPAC  1          /* Code for GR     */
#define GMPAC   2          /* Code for DB     */
#define EXEPAC  3          /* Code for EX     */
#define ANAPAC  4          /* Code for AN     */
#define PMPAC   5          /* Code for PM     */
#define GEOPAC  6          /* Code for GE     */
#define SURPAC  7          /* Code for surpac */
#define WINPAC  8          /* Code for WP     */
#define MSPAC   9          /* Code for MS     */

/*
***Debug function prototypes
*/
short dbgini();
short dbgexi();
short dbgon(char *str);
int   dbglev(int srcpac);
FILE *dbgfil(int srcpac);

/********************************************************************/
