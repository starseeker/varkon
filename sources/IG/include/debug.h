/*!******************************************************************/
/*  File: debug.h                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.tech.oru.se/cad/varkon                         */
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

/* Constants used by time measurement system*/

#define V3_TIMER_RESET 0
#define V3_TIMER_ON    1
#define V3_TIMER_OFF   2
#define V3_TIMER_WRITE 3

/*
***Function prototypes
*/
void  dbgini();
void  dbgexi();
int   dbgon(char *str);
int   dbglev(int srcpac);
FILE *dbgfil(int srcpac);
void  v3time(int op, int num, char *s);

/********************************************************************/
