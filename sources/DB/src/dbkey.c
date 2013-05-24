/**********************************************************************
*
*    dbkey.c
*    =======
*
*    This file includes the following public functions:
*
*    DBinsert_named_data();  Inserts data by key
*    DBread_named_data();    Reads data by key
*    DBdelete_named_data();  Deletes data by key
*
*
*    This file is part of the VARKON Database Library.
*    URL:  http://www.varkon.com
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
*    (C)Microform AB 1984-1998, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include "../include/DB.h"
#include "../include/DBintern.h"

/*
***Some internal defines.
*/
#define DEFKTSIZE 10     /* Default size and increment for keytab */
/*
***Internal functions.
*/
static KEYDATA *key_pointer(char *);
static KEYDATA *new_entry();

/*!******************************************************/

        DBstatus DBinsert_named_data(
        char *key,
        DBint type,
        DBint size,
        DBint count,
        char *datptr)

/*      Inserts data by key. count*size bytes are
 *      stored in a contigous block with unique
 *      name = key. The type-parameter is a user
 *      defined integer that is stored in the key-
 *      table but not used by the system other than
 *      that it is returned when the data is read.
 *      In VARKON it is used to keep track of the
 *      basic datatype of the array stored,
 *      type = C_INT_VA or type = C_FLO_VA etc.
 *
 *      In: key    = Key name string
 *          type   = Basic datatype
 *          size   = Basic size
 *          count  = Number of array elements
 *          datptr = Pointer to data block
 *
 *      FV:  0 => Ok.
 *          -1 => Key is already in use.
 *
 *      (C)microform ab 1999-01-21 J. Kjellander
 *
 ******************************************************!*/

  {
   DBptr    la;
   DBint    nbytes;
   KEYDATA *keyptr;

/*
***Check that the key is unique.
*/
   if ( key_pointer(key) != NULL ) return(-1);
/*
***Ok, create a new entry in keytab.
*/
   if ( (keyptr=new_entry()) == NULL ) return(erpush("GM2013",key));
/*
***Write the bulk data to GM.
*/
   nbytes = count*size;
   if ( nbytes <= PAGSIZ ) wrdat1(datptr,&la,nbytes);
   else                    wrdat2(datptr,&la,nbytes);
/*
***Update keytab.
*/
   strcpy(keyptr->key,key);
   keyptr->type  = type;
   keyptr->size  = size;
   keyptr->count = count;
   keyptr->datla = la;
/*
***End.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBread_named_data(
        char  *key,
        DBint *type,
        DBint *size,
        DBint *count,
        char **datptr)

/*      Reads data by key. Allocates a memory block of
 *      the right size and copies the data with the named
 *      key into that memory block. A pointer to the
 *      allocated block of data is returned as well as
 *      the type, size and count specified when the data
 *      was originally stored in the database. It is up
 *      to the calling routines to free the C-memory
 *      allocated.
 *
 *      In:  key    = Key name string
 *
 *      Out: *type   = Basic type
 *           *size   = Basic size
 *           *count  = Number of array elements
 *           *datptr = Pointer to data block
 *
 *      FV:  0 => Ok.
 *          -1 => Key does not exist.
 *
 *      Errcodes: GM2003 = Can't malloc
 *
 *      (C)microform ab 1999-01-21 J. Kjellander
 *
 ******************************************************!*/

  {
   DBint    nbytes;
   KEYDATA *keyptr;

/*
***Check that the key exists and get a pointer to it.
*/
   if ( (keyptr=key_pointer(key)) == NULL ) return(-1);
/*
***Allocate memory for bulk data.
*/
   nbytes = keyptr->count*keyptr->size;

   if ( (*datptr=v3mall(nbytes,"DBread_named_data")) == NULL )
     return(erpush("GM2003",""));
/*
***Read data from GM.
*/
   if ( nbytes <= PAGSIZ ) rddat1(*datptr,keyptr->datla,nbytes);
   else                    rddat2(*datptr,keyptr->datla,nbytes);
/*
***Return size etc.
*/
  *type  = keyptr->type;
  *size  = keyptr->size;
  *count = keyptr->count;
/*
***End.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        DBstatus DBdelete_named_data(char *key)

/*      Deletes data by key. The key itself as well as
 *      the associated data is deleted and the memory
 *      occupied is freed.
 *
 *      In: key = Key name string
 *
 *      FV:  0 => Ok.
 *          -1 => Key does not exist.
 *
 *      (C)microform ab 1999-01-21 J. Kjellander
 *
 ******************************************************!*/

  {
   DBint    nbytes;
   KEYDATA *keyptr;

/*
***Check that the key exists and get a pointer to it.
*/
   if ( (keyptr=key_pointer(key)) == NULL ) return(-1);
/*
***Release from keytab.
*/
   keyptr->key[0] = '\0';
/*
***Delete the bulk data from GM.
*/
   nbytes = keyptr->count*keyptr->size;

   if ( nbytes <= PAGSIZ ) rldat1(keyptr->datla,nbytes);
   else                    rldat2(keyptr->datla,nbytes);
/*
***End.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

 static KEYDATA *key_pointer(char *key)

/*      Returns a pointer to the entry in keytab with
 *      the required key value or NULL.
 *
 *      In: key = Key name string
 *
 *      (C)microform ab 1999-01-21 J. Kjellander
 *
 ******************************************************!*/

  {
   DBint i;

/*
***Search keytab. For now a simple linear search with
***strcmp() will do. Much faster algorithms may be
***used later. The number of objects stored by key
***will however probably never be very large, typically
***less than 50 so there is not much to gain.
*/
   for ( i=0; i<nkeys; ++i )
     {
     if ( strcmp(key,(keytab+i)->key) == 0 ) return(keytab+i);
     }
/*
***If not found or if nkeys = 0, return NULL.
*/
   return(NULL);
  }

/********************************************************/
/*!******************************************************/

 static KEYDATA *new_entry()

/*      Creates a new entry in keytab and returns a
 *      pointer to that entry.
 *
 *      First time this routine is called ketyab
 *      does not exist, so it is created with
 *      default size = DEFKTSIZE. When all entries
 *      are used, the size of keytab is increased
 *      with DEFKTSIZE.
 *
 *      (C)microform ab 1999-01-21 J. Kjellander
 *
 ******************************************************!*/

  {
   DBint i;

/*
***If keytab does not exist, create keytab with default
***size and clear all entries.
*/
   if ( keytab == NULL )
     {
     keytab = (KEYDATA *)v3mall(DEFKTSIZE*sizeof(KEYDATA),"new_entry");
     if ( keytab == NULL ) return(NULL);

     for ( i=0; i<DEFKTSIZE; ++i ) (keytab+i)->key[0] = '\0';
     nkeys = DEFKTSIZE;
     return(keytab);
     }
/*
***If keytab exists, try to reuse deleted entries, ie.
***entries where key="".
*/
   for ( i=0; i<nkeys; ++i )
     {
     if ( (keytab+i)->key[0] == '\0' ) return(keytab+i);
     }
/*
***Keytab exists and there are no free entries. Increase
***the size of keytab and create new free entries.
*/
     keytab = (KEYDATA *)v3rall(keytab,
              (nkeys+DEFKTSIZE)*sizeof(KEYDATA),"new_entry");
     if ( keytab == NULL ) return(NULL);
     else
       {
       nkeys += DEFKTSIZE;
       return(keytab+nkeys-DEFKTSIZE);
       }
  }

/********************************************************/
