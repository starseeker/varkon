/**********************************************************************
*
*    oldatts.h
*    =========
*
*    This file is part of the VARKON Execute Library.
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
***This file defines old versions of the named parameter
***block for compatibility with old versions of jobfiles.
*/

/*
***Attributes up to Version 1.2B.
*/

typedef struct
 {
 DBint   pen;
 DBint   level;
 DBint   lfont;
 DBint   afont;
 DBfloat ldashl;
 DBfloat adashl;
 DBfloat tsize;
 DBfloat twidth;
 DBfloat tslant;
 DBfloat dtsize;
 DBfloat dasize;
 DBint   dndig;
 DBint   dauto;
 DBint   xfont;
 DBfloat xdashl;
 DBint   blank;
 } ONAP13;

/*
***Attributes up to Version 1.10L.
*/

typedef struct
 {
 DBint   pen;
 DBint   level;
 DBint   lfont;
 DBint   afont;
 DBfloat ldashl;
 DBfloat adashl;
 DBfloat tsize;
 DBfloat twidth;
 DBfloat tslant;
 DBfloat dtsize;
 DBfloat dasize;
 DBint   dndig;
 DBint   dauto;
 DBint   xfont;
 DBfloat xdashl;
 DBint   blank;
 DBint   hit;
 DBint   save;
 } ONAP110;

/*
***Attributes up to 1.11F.
*/

typedef struct
 {
 DBint   pen;
 DBint   level;
 DBint   lfont;
 DBint   afont;
 DBfloat ldashl;
 DBfloat adashl;
 DBfloat tsize;
 DBfloat twidth;
 DBfloat tslant;
 DBfloat dtsize;
 DBfloat dasize;
 DBint   dndig;
 DBint   dauto;
 DBint   xfont;
 DBfloat xdashl;
 DBint   blank;
 DBint   hit;
 DBint   save;
 DBint   cfont;
 DBfloat cdashl;
 } ONAP111;

/*
***Attributes up to 1.15B
*/

typedef struct
 {
 DBint   pen;
 DBint   level;
 DBint   lfont;
 DBint   afont;
 DBfloat ldashl;
 DBfloat adashl;
 DBfloat tsize;
 DBfloat twidth;
 DBfloat tslant;
 DBfloat dtsize;
 DBfloat dasize;
 DBint   dndig;
 DBint   dauto;
 DBint   xfont;
 DBfloat xdashl;
 DBint   blank;
 DBint   hit;
 DBint   save;
 DBint   cfont;
 DBfloat cdashl;
 DBint   tfont;
 DBint   sfont;
 DBfloat sdashl;
 DBint   nulines;
 DBint   nvlines;
 } ONAP115;

/*
***Attributes up to 1.16B.
*/

typedef struct
 {
 DBint   pen;
 DBint   level;
 DBint   lfont;
 DBint   afont;
 DBfloat ldashl;
 DBfloat adashl;
 DBfloat tsize;
 DBfloat twidth;
 DBfloat tslant;
 DBfloat dtsize;
 DBfloat dasize;
 DBint   dndig;
 DBint   dauto;
 DBint   xfont;
 DBfloat xdashl;
 DBint   blank;
 DBint   hit;
 DBint   save;
 DBint   cfont;
 DBfloat cdashl;
 DBint   tfont;
 DBint   sfont;
 DBfloat sdashl;
 DBint   nulines;
 DBint   nvlines;
 DBfloat width;
 } ONAP116B;

/*
***Attributes up to 1.17D.
*/

typedef struct
 {
 v2int   pen;
 v2int   level;
 v2int   lfont;
 v2int   afont;
 v2float ldashl;
 v2float adashl;
 v2float tsize;
 v2float twidth;
 v2float tslant;
 v2float dtsize;
 v2float dasize;
 v2int   dndig;
 v2int   dauto;
 v2int   xfont;
 v2float xdashl;
 v2int   blank;
 v2int   hit;
 v2int   save;
 v2int   cfont;
 v2float cdashl;
 v2int   tfont;
 v2int   sfont;
 v2float sdashl;
 v2int   nulines;
 v2int   nvlines;
 v2float width;
 v2int   tpmode;
 } ONAP117D;
