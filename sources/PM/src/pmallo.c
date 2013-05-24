
/**********************************************************************
*
*    pmac1.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    PM memory allocation and administration routines.
*
*    short pmibuf()    Skapar PM:s minnesareor.
*    short pmallo()    allocate and return a block in PM
*    short pmgeba()    get base address to module in PM and load
*                      module from file if not in PM
*    short pmclea()    clears and reinitialise PM
*    char *pmgadr()    get address to PM location
*    pm_ptr pmgbla()   get PM-base address
*    short pmsbla()    set PM-base address
*    pm_ptr pmgaba()   get base address for active module
*    short pmsaba()    set base address for active module
*    short pmssta()    save module on stack (active module)
*    short pmlsta()    load a module to stack, to be active
*    void  pmmark()    mark current stack pointer position in PM
*    void  pmrele()    release memory allocated from mark
*    void  pmgstp()    Returnerar PM:s stackpekare
*    void  pmsstp()    Sätter om PM:s stackpekare
*    void  pmstat()    Returnerar div data om PM
*
*    Local routines:
*    short clheap()    clears the heap and the module table
*    pm_ptr loadmo()   loads a parted module into PM 
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

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#ifdef UNIX
#include <unistd.h>
#endif

#ifdef WIN32
#include <io.h>
#include <sys\stat.h>
#endif

#define NUPART    80                    /* Max Number of parted modules in PM */

extern char jobdir[];
extern char libdir[];
extern V3MDAT sydata;
extern V3MSIZ sysize;

char *pma;                              /* C-pekare till PM:s stack */
char *pmb;                              /* C-pekare till PM:s heap  */

static pm_ptr pmbasla = 0;              /* current base index in PM's stack */
static short  pa_count = 0;             /* holds the number of parts in PM */
static pm_ptr stackp = (0 + MIN_BLKS);  /* pointer to PM stack, top of memory*/
static pm_ptr pm_markp;                 /* mark level for pmmark() and pmrele() */
static pm_ptr heapp;                    /* Pointer to PM heap */
static pm_ptr actmodba = MIN_BLKS;      /* pointer to node for active module */

static struct motabt
    {
    char modulena[JNLGTH+1];            /* Module name */
    pm_ptr basep;                       /* Base pointer for the module in PM */
    pm_ptr size;                        /* Memory size for an module in PM */
    } motab[ NUPART ];

/*
***Internal function prototypes.
*/
static pm_ptr loadmo(char *moname);

/********************************************************/
/*!******************************************************/

     short pmibuf()

/*   Allokerar primärminne för PM:s stack och heap. 
*
*    Felkod: PM1061 = Kan ej allokera minne
*
*    (C)microform ab 890519 J. Kjellander
*
********************************************************!*/

  {
   char errbuf[V3STRLEN+1];

   if ( (pma=v3mall(sysize.pm+1,"PM-A")) == NULL  ||
        (pmb=v3mall(sysize.pm+1,"PM-B")) == NULL )
     {
     sprintf(errbuf,"%d",sysize.pm);
     return(erpush("PM1061",errbuf));
     }

   heapp = 2*sysize.pm;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short pmallo(
        DBint   blsize,
        pm_ptr *retla)

/*      Get PM block.
 *      Alllocates a memory block in PM's stack and returns a PM-pointer
 *      to it relative the value of PM-base pointer.                    
 *
 *      In:    blsize  =>  Size of block to allocate
 *                         Ändrat blsize till long 861022-JK 
 *
 *      Out:  *retla   =>  Return logical address,
 *                         PM-pointer to the allocated block 
 *
 *      FV:    return - status 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *      2004-02-21 Better errorhandling, J.Kjellander
 *
 ******************************************************!*/

  {
   pm_ptr resla;
   char   errstr[V3STRLEN];

/*
***Check the alignment
*/
   if ( ( blsize % MIN_BLKS ) != 0 )
      {                           
/*
***Not right alignment, fix the alignment
*/
      blsize = ( ( blsize + MIN_BLKS ) / MIN_BLKS ) * MIN_BLKS;
      }
/*
***Is there room for the block in stack ?
*/
   if ( stackp + blsize < sysize.pm )  
      {
      resla = stackp;
      stackp = stackp + blsize;
      }
   else
      {
      sprintf(errstr,"%d", sysize.pm-stackp);
      return(erpush("PM0014", errstr));
      }
/*
***Subtract PM-base pointer from the result
*/
   *retla = resla - pmbasla; 

   return(0);                          /* return status OK */

  }
    
/********************************************************/
/*!******************************************************/

        short   pmgeba(
        char   *moname,
        pm_ptr *retla)

/*      Get base address for module.
 *      Checks if the module is in PM and returnes a base pointer to it if
 *      it is in PM. If it is not in PM it will be loaded to PM from file.
 *      If there is no room in PM the PM_heap will be erased and the
 *      required module is loaded into PM.
 *
 *      In:    moname  =>  Module name
 *
 *      Out:  *retla   =>  PM base pointer to an module in PM
 *
 *      FV:    return - status 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  { 
   short clheap();

   register short i = 0;

/*
***Search for "moname" in module table
*/
   while ( i < pa_count && strcmp(motab[i].modulena, moname) ) i++;

   if ( i < pa_count )
      {
      *retla = motab[i].basep;             /* Module is in PM */
      }
   else
      {
/*
***Module is not in PM
*/
      if ( pa_count < NUPART )
         {
/*
***room for more modules in module table
*/
         if ( (*retla = loadmo( moname )) == (pm_ptr)NULL )
            {
/*
***there was no room in PM clear heap and load
*/
            clheap();
            if (( *retla = loadmo( moname )) == (pm_ptr)NULL )
               {
               return( erpush( "PM3023", moname ) );  /* Error, no room for MODULE in PM */
               }
            else if ( *retla == -1 )
               {
               return( erpush( "PM2062", moname ) );  /* module not found */
               }
            }
         else if ( *retla == -1 )
            {
            return( erpush( "PM2062", moname ) );     /* module not found */
            }
         }
      else
         {
/*
***no room for more modules in module table, clear heap and load
*/
         clheap();
         if (( *retla = loadmo( moname )) == (pm_ptr)NULL )
            {
            return( erpush( "PM3023", moname ) );     /* Error, no room for MODULE in PM */
            }
         else if ( *retla == -1 )
            {
            return( erpush( "PM2062", moname ) );     /* module not found */
            }
         }
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmclea( )

/*      Clear PM.
 *      Clears PM and restart the administration of PM, for the active
 *      module.
 *
 *      FV:    return - status 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short status;

/*
***reset PM_stack pointer to start of area for active module
*/
   if ( actmodba > MIN_BLKS )
      stackp = actmodba; 
/*
***clear the PM heap and module table
*/
   if ( ( status = clheap() ) < -1 )
      return( status );

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        char *pmgadr(
        pm_ptr   pmla)


/*      Get address in PM.
 *      Get address ( c-pointer to char ) to an location in PM.
 *      Where the location is relative to PM-base address.                    
 *
 *      In:    pmla    =>  PM-pointer 
 *
 *      FV:    return - c-pointer to the object in PM 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   register pm_ptr absla; 
   char buf[20];

/*
***Absolut PM-adress = pmla + aktiv basadress.
*/
   absla = pmla + pmbasla;
/*
***Kolla att adressen ligger inom PM:s adressområde.
*/
   if ( absla <= 0  ||  absla > 2*sysize.pm )
     {
     sprintf(buf,"%d",absla);
     erpush("PM0034",buf);
     return( NULL );
     }
/*
***Beräkna motsvarande C-adress.
*/
   if ( absla < sysize.pm ) return(pma+absla);
   else return(pmb+(absla-sysize.pm));
}

/********************************************************/
/*!******************************************************/

        short pmsbla(
        pm_ptr   basla)

/*      Set PM-base address.
 *      Set base index for references in PM.
 *
 *      In:    basla   =>  Base address 
 *
 *      FV:    return  -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {

   if ( basla < 0  ||  basla > 2*sysize.pm )
      {
      return( erpush( "PM0044", "" ) );    /* Ilegal PM-base pointer */
      }

   pmbasla = basla;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        pm_ptr pmgbla()

/*      Get PM-base address.
 *      Returns the value of PM-base address.
 *
 *      FV:    return  -    
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   return( pmbasla );
  }

/********************************************************/
/*!******************************************************/

        short pmsaba(
        pm_ptr   basla)

/*      Set base address for active module.
 *
 *      In:    basla   =>  Base address 
 *
 *      FV:    return  -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   if ( basla <= 0  ||  basla > 2*sysize.pm )
     {
     return( erpush( "PM0044", "" ) );    /* Ilegal PM-base pointer */
     }

   actmodba = basla;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        pm_ptr pmgaba()

/*      Get base address for active module.
 *      Returns the base address for the active module.
 *
 *      FV:    return  -    
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   return( actmodba );
  }

/********************************************************/
/*!******************************************************/

        short pmssta()

/*      Save module from stack.
 *      Saves the active module from PM-stack.
 *
 *      FV:    return  -    
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1986-06-30 Skrivning i 32K block för VAX och IBM, J. Kjellander
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   int n;
   short status;
   pm_ptr modsize;         /* module size in bytes */
   string modstr;          /* module name string */
   int fd;                 /* file descriptor */
   char fname[V3PTHLEN];   /* for file name */
   char *buffer;           /* buffer for file transfare */
   PMMONO *np;

/*
***set pmbase to active module, 861014JK
*/
   pmsbla(pmgaba());
/*
***calculate size of active module
*/
   modsize = stackp - actmodba;
/*
***Get name of active module and make file name
*/
   if ( ( status = pmgmod((pm_ptr)0, &np ) ) != 0 )
      return( status );
   if ( ( status = pmgstr( np->mona_, &modstr ) ) != 0 )
      return( status );

   strcpy( fname, jobdir );    /* Put string for dir to fname */
   strcat( fname, modstr );    /* Put module name to fname */
   strcat( fname, MODEXT );    /* Put string for extention in fname to create
                                                          the total file name */
/*
***open and create the file and test if ok
***creat() bytt mot open() 25/3/90 JK
***0666 = -rw-rw-rw-
***Lagt till TRUNC 19/3/91 JK
***Ändrat till creat på VAX:en igen, 2/2/93
***Open med O_CREAT ger inget nytt fildatum på VAX !
*/

#ifdef UNIX
   if ( (fd = open(fname,O_WRONLY|O_CREAT|O_TRUNC,0666)) < 0 )
      {
      return( erpush( "PM2042", fname ));   /* Error creating file */
      }
#endif

#ifdef WIN32
   if ( (fd = open(fname,O_BINARY|O_WRONLY|O_CREAT|O_TRUNC,
                      S_IWRITE|S_IREAD)) < 0 )
      {  
      return( erpush( "PM2042", fname ));   /* Error creating file */
      }
#endif

#ifdef VMS
   if ( (fd = creat(fname,0666)) < 0 )
      {  
      return( erpush( "PM2042", fname ));   /* Error creating file */
      }
#endif

/*
***convert and make a char pointer to modsize,
***which will be the buffer the file size is
***writed from to the file
*/
   buffer = (char *)&modsize;  
/*
***write module size to the file
*/
   if ( ( n = write(fd, buffer, sizeof(modsize))) != sizeof(modsize)  )
      {
      close( fd );
      return( erpush( "PM2052", fname ) );  /* Error writing file */
      }
/*
***Set PM-base to 0 and read address of module then reset PM-base to
***start of active module area
*/
   if ( ( status = pmsbla((pm_ptr)0 ) ) != 0 )
      return( status );

   buffer = (char *)pmgadr( pmgaba() );

   if ( ( status = pmsbla( pmgaba() ) ) != 0 )
      return( status );
/*
***write module to the file, max 32000 bytes at the time, JK
*/
   while ( modsize > 32000 )
      {
      if ( ( n = write(fd, buffer, 32000) ) != 32000 )
         {
         close( fd );
         return( erpush( "PM2052", fname ) );  /* Error writing file */
         }
      modsize -= 32000;
      buffer  += 32000;
      }

   if ( ( n = write(fd, buffer, modsize) ) != modsize )
      { 
      close( fd );
      return( erpush( "PM2052", fname ) );  /* Error writing file */
      }

   close( fd );
   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short pmlsta(
        char   *moname,   /* in -  */
        pm_ptr *retla)   /* out -  */ 


/*      Load module to stack.
 *      Load a module to stack so the module be active.
 *
 *      In:   *moname  =>  Name of module that will be loaded and made active
 *
 *      Out:  *retla   =>  Base pointer to start of active module in PM
 *
 *      FV:    return  -   error severity code 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *      1986-06-30 Läsning i 32K block för VAX och IBM, J. Kjellander
 *
 ******************************************************!*/

  {
   char    errbuf[V3STRLEN];   
   int     n;
   short   status;
   pm_ptr  fsize;              /* file size */
   int     fd;                 /* file descriptor */
   char    fname[V3PTHLEN];    /* for file name */
   char   *buffer;             /* buffer for file transfare */
   pm_ptr  newstackp;  
   PMMONO *np;                 /* c-pointer to module header structure */
   int     readsize;           /* read size */

   strcpy( fname, jobdir );    /* Put string for dir in fname */
   strcat( fname, moname );    /* Concatenate the module name */
   strcat( fname, MODEXT );    /* Put string for extention in fname to create
                                                          the total file name */

/*
***open the file for read and test if ok
*/
#ifdef WIN32
   if ( (fd = open(fname,O_BINARY | O_RDONLY)) < 0 )
#else
   if ( (fd = open(fname, 0)) < 0 )
#endif
      {  
      return( erpush( "PM2012", fname ));   /*Error File not found */
      }

/*
***convert and make a char pointer to fsize,
***which will be the buffer the file size is
***read to from the file
*/
   buffer = (char *)&fsize;  

/*
***read module (file) size and check if error
*/ 
   if ( ( n = read(fd, buffer, sizeof(fsize))) != sizeof(fsize) )
      {
      close( fd ); 
      return( erpush( "PM2022", fname ) );  /* Error reading file */
      }
/*
***base for active module
*/
   *retla = pmgaba();     
/*
***check if room in PM
*/
   newstackp = *retla + fsize;
   if ( newstackp >= heapp )
      {                                         /* No room */
      clheap();                                 /* clear the heap */
      if ( newstackp >= heapp )
         {
         return( erpush( "PM3023", moname ) );  /*Error No room for module in PM */
         }
      }

   if ( ( status = pmsbla( *retla ) ) != 0 )
      return( status );
   buffer = (char *)pmgadr((pm_ptr)0 );         /* let buffer point to base for  
                                                                  active module */

/*
***load module into PM's stack
*/

/*
***set PM-base = base for active
*/
   status = pmsbla( *retla );    
   stackp = *retla;

/*
***read module header into PM and check if legal to use
*/
   readsize = sizeof( PMMONO );
/*
***read module header from file and check if error
*/
   if ( ( n = read(fd, buffer, (int)readsize) ) != readsize  )
      {
      close( fd );
      return( erpush( "PM2022", fname ) );      /* Error reading file */
      }

   pmgmod((pm_ptr)0, &np );                     /* get c-pointer to module 
                                                              header in PM */
/*
***check serial number in module with systems
*/
   if ( ( np->sysdat.mpcode != 0 ) &&                /* MN 860610 */
      ( np->sysdat.sernr != sydata.sernr ) &&
      ( abs(np->sysdat.mpcode) != sydata.sernr ) &&  /* JK 870413 */
      ( np->sysdat.revnr > 1 ) )
       {  
       close( fd );
       return( erpush( "PM3063", fname ) );          /* ilegal to use don't load the module */
       }
/*
***Check that the module is 1.18A or later.
*/
   if ( np->sysdat.revnr < 18 )
      {
       close(fd);
       sprintf(errbuf,"%s%%%d.%c",moname,np->sysdat.revnr,np->sysdat.level);
       return(erpush("PM3093",errbuf));
       }
/*
***Kolla att det inte är en MACRO-modul, JK 929217
*/
   if ( np->moat_ == MACRO )
      {
      close(fd);
      erpush("PM3073","");
      return(erpush("PM3083",fname));
      }
/*
***read the rest of the module from file, max 32000 at the time, JK
*/
   buffer = buffer + readsize;
   readsize = fsize - readsize;

   while ( readsize > 32000 )
      {
      if ( ( n = read(fd, buffer, 32000) ) != 32000 )
         {  
         close( fd );
         return( erpush( "PM2022", fname ) );    /* Error reading file */
         }
      readsize -= 32000;
      buffer  += 32000;
      }

   if ( ( n = read(fd, buffer, (int)readsize) ) != readsize  )
      {
      close( fd );
      return( erpush( "PM2022", fname ) );       /* Error reading file */
      }
/*
***Update stack pointer
*/
   stackp = newstackp;

   close( fd );
   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        void pmmark()

/*      Mark the current stackp in PM.
 *      Store current stack pointer.
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_markp = stackp;
  }

/********************************************************/
/*!******************************************************/

        void pmrele()

/*      Release PM memory.
 *      Release PM memory allocated since last pmmark().
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   stackp = pm_markp;
  }

/********************************************************/
/*!******************************************************/

        void pmgstp(
        pm_ptr *pstack)

/*      Return the current stackp in PM.
 *      Return current stack pointer.
 *
 *      Out:  *pstack  =>  Stack pointer 
 *
 *      FV:    return  -    
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   *pstack = stackp;
  }

/********************************************************/
/*!******************************************************/

        void pmsstp(
        pm_ptr  stkptr)

/*      Set the current stackp in PM.
 *      Set current stack pointer.
 *
 *      In:   stkptr   =>  Stack pointer 
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   stackp = stkptr;
  }

/********************************************************/
/*!******************************************************/

      void pmstat(
      short  *pnm,
      char   namvek[][JNLGTH+1],
      pm_ptr basvek[],
      pm_ptr sizvek[],
      pm_ptr *pstack,
      pm_ptr *pheap)

/*    Returnerar data om moduler i PM mm.
*
*     In: pnm    = Pekare till antal parter i PM
*         namvek = Pekare till partnamn
*         basvek = Pekare till basadresser
*         sizvek = Pekare till storlekar
*         pstack = Pekare till stackpekare
*         pheap  = Pekare till heappekare
*
*     (C)microform ab 870423 J. Kjellander
*
*******************************************************!*/

  {
   short i;

   *pnm = pa_count;

   for ( i=0; i<pa_count; ++i )
      {
      strcpy(namvek[i],motab[i].modulena);
      basvek[i] = motab[i].basep;
      sizvek[i] = motab[i].size;
      }

   *pstack = stackp;

   *pheap = heapp;

   return;
  }

/********************************************************/
/*!******************************************************/

        short clheap()

/*      Clears the heap and the module table for parted
 *      modules
 *
 *      FV:    return  -    
 *
 *      (C)microform ab 1986-03-21 Per-Ove Agne'
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   register short i;
/* 
***Clear the module table
*/
   for ( i = 0; i < NUPART ; i++ )
      *motab[i].modulena = '\0';    /* put NULL at first byte in Module name */

/*
***Clear the heap pointer
*/
   heapp = 2*sysize.pm;
   pa_count = 0;                    /* clear the number of parts in PM */

   return(0);
  }

/********************************************************/
/*!******************************************************/

      static pm_ptr loadmo(char *moname)

/*    Loads a module from disc into PM.
*
*     In: moname => Name of MBO file to be loaded.
*
*     Return: PM ptr to module in PM or
*              0 if there is no room in PM or
*             -1 if file is not found
*
*     (C)microform ab 870423 J. Kjellander
*
*     LIBDIR, JK 7/10/86
*     Läsning i 32K block för VAX och IBM, 30/9/86 JK 
*     Anrop till pmstpr() 9/12/86
*     VARKON_LIB 15/2/95 JK
*     IGgenv(),  1997-01-15 JK
*     1999-11-18 Rewritten, R. Svedin
*     2004-07-14 1.18A, J.Kjellander, Örebro university
*     2007-11-20 2.0, J.Kjellander
*
********************************************************!*/

  {
   char   errbuf[V3STRLEN];   /* buffer for errormessage */
   pm_ptr fsize;              /* file size */
   int    fd;                 /* file descriptor */
   char   fname[V3PTHLEN];    /* for file name */
   char  *buffer;             /* buffer for file transfer */
   pm_ptr newheapp;
   PMMONO *np;                /* c-pointer to module header structure */
   int    readsize;
   pm_ptr currpmba;
   int n;

/*
***First look for the file in jobdir.
*/
   strcpy(fname,jobdir);
   strcat(fname,moname);
   strcat(fname,MODEXT);

#ifdef WIN32
   if ( (fd=open(fname,O_BINARY | O_RDONLY)) < 0 )
#else
   if ( (fd=open(fname,0)) < 0 )
#endif
     {
/*
***File not found in jobdir, try to open on jobdir/lib.
*/
     strcpy(fname,jobdir);
     strcat(fname,"lib/");
     strcat(fname,moname);
     strcat(fname,MODEXT);

#ifdef WIN32
     if ( (fd=open(fname,O_BINARY | O_RDONLY)) < 0 )
#else
     if ( (fd=open(fname,0)) < 0 )
#endif
       {
/*
***File not found on jobdir/lib, try with libdir.
***If not successful, return -1.
*/
       if ( (fd=IGfopr(libdir,moname,MODEXT)) < 0 )
         {
         return(-1);
         }
       }
     }
/*
***File found. Read file size.
*/
   buffer = (char *)&fsize;

   if ( read(fd,buffer,sizeof(fsize)) < 0 )
     {
     close(fd);
     return((pm_ptr)NULL);
     }
/*
***Check for room in PM
*/
   newheapp = heapp - fsize;

   if ( newheapp > sysize.pm )
     {
/*
***There is room in PM, load module into PM and update module table
*/
     buffer = pmb + (newheapp-sysize.pm);    /* convert and make
                                                a char pointer to
                                                where the new heap
                                                    pointer points */
/*
***Read module header into PM and check if legal to use
*/
     readsize = sizeof(PMMONO);
/*
***Read module header from file and check if error
*/
     if ( (n=read(fd,buffer,(int)readsize) ) != readsize  )
       {
       close(fd);
       return((pm_ptr)NULL);   /* Error("error reading file %s", moname); */
       }

     currpmba = pmbasla;
     pmsbla(newheapp);          /* set new PM-base pointer */
     pmgmod((pm_ptr)0,&np);     /* get c-pointer to module header in PM */
     pmsbla(currpmba);          /* reset PM base to current */
/*
***Check serial number.
*/
     if ( ( np->sysdat.mpcode != 0 ) &&           /* MN 860610 */
        ( np->sysdat.sernr != sydata.sernr ) &&
        ( np->sysdat.mpcode != sydata.sernr ) &&
        ( np->sysdat.revnr > 1 ) )
        {
        close(fd);
        erpush("PM3063",moname);   /* ilegal to use don't load the module */
        return((pm_ptr)NULL);
        }
/*
***Check that the module is 1.18A or later.
*/
     if ( np->sysdat.revnr < 18 )
       {
       close(fd);
       sprintf(errbuf,"%s%%%d.%c",moname,np->sysdat.revnr,np->sysdat.level);
       erpush("PM3093", errbuf);
       return((pm_ptr)NULL);
       }
/*
***Read the rest of the module from file.
*/
     buffer +=  (int)readsize;
     readsize = fsize - readsize;

     while ( readsize > 32000 )
       {
       if ( (n=read(fd,buffer,(int)32000) ) != 32000 )
         {
         close(fd);
         return((pm_ptr)NULL);   /* Error reading file moname */
         }
       readsize -= 32000;
       buffer  += (int)32000;
       }

     if ( (n=read(fd,buffer,(int)readsize) ) != readsize  )
       {
       close(fd);
       return((pm_ptr)NULL);      /* error reading file moname */
       }
/*
***Update heap pointer and module table
*/
     heapp = newheapp;
     strcpy( motab[pa_count].modulena, moname );
     motab[pa_count].basep = heapp;
     motab[pa_count].size = fsize;

     pa_count++;               /* update number of modules in PM */
     pmstpr(heapp);            /* tillagt pga. bug vid partanrop */

     close(fd);
     return(heapp);          /* return pointer to module in PM */
     }
   else
     {
     close(fd);
     return((pm_ptr)NULL);    /* no room in PM */
     }
  }

/********************************************************/
