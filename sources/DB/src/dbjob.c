/**********************************************************************
*
*    dbjob.c
*    =======
*
*    This file includes the following public functions:
*
*    DBimport_jobfile();  Stores a JOB-file in DB as jobdata
*    DBexport_jobfile();  Writes DB jobdata to a JOB-file
*
*    This file is part of the VARKON Database Library.
*    URL:  http://varkon.sourceforge.net
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
***********************************************************************/

#include "../include/DB.h"
#include "../../IG/include/IG.h"

/********************************************************/

        DBstatus DBimport_jobfile(char *filepath)

/*      Reads a JOB-file and saves its contents
 *      in DB as jobdata.
 *
 *      In: filepath = C-ptr to file name with path.
 *
 *      Return:  0      = Ok
 *               GM2053 = Can't open JOB-file %s
 *               GM2043 = Can't close JOB-file %s
 *
 *      (C)2007-10-13 J. Kjellander
 *
 ********************************************************/

  {
   FILE *jf;

/*
***Open the JOB-file.
*/
    if ( (jf=fopen(filepath,"r")) == 0 ) return(erpush("DB2053",filepath));
/*
***Close the JOB-file.
*/
    if ( fclose(jf) == EOF ) return(erpush("DB2043",filepath));
/*
***End.
*/
   return(0);
  }

/********************************************************/
/********************************************************/

        DBstatus DBexport_jobfile(char *filepath)

/*      Writes DB jobdata to a JOB-file.
 *
 *      In: filepath = C-ptr to file name with path.
 *
 *      Return:  0      = Ok
 *               GM2033 = Can't create JOB-file %s
 *               GM2043 = Can't close JOB-file %s
 *
 *      (C)2007-10-13 J. Kjellander
 *
 ********************************************************/

  {
   FILE *jf;

/*
***Create and open the JOB-file.
*/
    if ( (jf=fopen(filepath,"w+")) == 0 ) return(erpush("DB2033",filepath));
/*
***Close the JOB-file.
*/
    if ( fclose(jf) == EOF ) return(erpush("DB2043",filepath));
/*
***End.
*/
   return(0);
  }

/********************************************************/
