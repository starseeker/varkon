/*!*****************************************************
*
*    exjob.c
*    =======
*
*    EXsave_job();   Interface-routine for SAVE_JOB
*    EXload_job();   Interface-routine for LOAD_JOB
*
*    EXclgm();       Interface-routine for CLEAR_GM
*    EXldgm();       Interface-routine for LOAD_GM
*    EXsvgm();       Interface-routine for SAVE_GM
*
*    This file is part of the VARKON Execute Library.
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
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../DB/include/DBintern.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../../WP/include/WP.h"
#include "../include/EX.h"
#include "../include/oldatts.h"
#include <string.h>

extern V3MDAT  sydata;
extern int     sysmode;
extern int     posmode;
extern bool    relpos;
extern char    jobnam[],jobdir[],tmpres[],actcnm[],svnversion[];
extern DBTmat *lsyspk;
extern DBptr   lsysla;
extern DBTmat  lklsys;
extern V2NAPA  defnap;
extern WPVIEW  wpviewtab[];
extern V3MSIZ  sysize;

static short write_VYTRF(FILE *jf, VYTRF *matrix);
static short read_VYTRF(FILE *jf,VYTRF *matrix);
static short write_DBTmat(FILE *jf, DBTmat *matrix);
static short read_DBTmat(FILE *jf, DBTmat *matrix);

static short get_VARKON_jobfile(FILE  *jf);
static short get_Expressions(FILE *jf);
static short get_Coordinate_system(FILE *jf);
static short get_Attributes(FILE *jf);
static short get_Viewtable(FILE *jf);
static short get_Leveltable(FILE *jf);
static short get_Pentable(FILE *jf);

/*!******************************************************/

        short EXsave_job(char *filename)

/*      Save job data in jobname.JOB
 *
 *      In: filnam = C-ptr to file name with path
 *
 *      Error: EX1863 => Can't create jobfile
 *             EX1873 => Can't write to jobfile
 *             EX1883 => Can't close jobfile
 *
 *      (C)2007-03-31 1.19, J.Kjellander
 *
 *      2007-09-05 SVN_version added, J.Kjellander
 *      2007-09-09 relpos added, J.Kjellander
 *      2008-01-19 Pentable added, J.Kjellander
 *
 ******************************************************!*/

  {
    int     i,y,m,d,h,min,s,r,g,b,ar,ag,ab,dr,dg,db,
            sr,sg,sb,er,eg,eb;
    char    idstr[V3STRLEN],name[V3STRLEN+1];
    double  curacc;
    bool    blank,defined;
    DBId    id[MXINIV];
    FILE   *jf;

/*
***Create and open the file.
*/
    if ( (jf=fopen(filename,"w+")) == 0 ) return(erpush("EX1863",filename));
/*
***Start with a VARKON_jobfile: record.
*/
    EXtime(&y,&m,&d,&h,&min,&s);

    if ( fprintf(jf,"#VARKON_jobfile\n")                     < 0 ) goto werror;
    if ( fprintf(jf,"Serial=%d\n",(int)sydata.sernr)         < 0 ) goto werror;
    if ( fprintf(jf,"Version=%d\n",(int)sydata.vernr)        < 0 ) goto werror;
    if ( fprintf(jf,"Revision=%d\n",(int)sydata.revnr)       < 0 ) goto werror;
    if ( fprintf(jf,"Level=%c\n",sydata.level)               < 0 ) goto werror;
    if ( fprintf(jf,"SVN_version=%s\n",svnversion)           < 0 ) goto werror;
    if ( fprintf(jf,"Jobname=%s\n",jobnam)                   < 0 ) goto werror;
    if ( fprintf(jf,"Creation_date=%d-%d-%d\n",y,m,d)        < 0 ) goto werror;
    if ( fprintf(jf,"Creation_time=%d:%d\n",h,min)           < 0 ) goto werror;
    if ( fprintf(jf,"OS_sysname=%s\n",sydata.sysname)        < 0 ) goto werror;
    if ( fprintf(jf,"OS_release=%s\n",sydata.release)        < 0 ) goto werror;
    if ( fprintf(jf,"#End\n")                                < 0 ) goto werror;
/*
***Data related to generation of expressions.
*/
    if ( fprintf(jf,"\n#Expressions\n")                      < 0 ) goto werror;
    if ( fprintf(jf,"Position_method=%d\n",(int)posmode)     < 0 ) goto werror;
    if ( relpos && fprintf(jf,"Relative=On\n")               < 0 ) goto werror;
    if ( fprintf(jf,"#End\n")                                < 0 ) goto werror;
/*
***Data related to the currently active coordinate system.
*/
    if ( fprintf(jf,"\n#Coordinate_system\n")                < 0 ) goto werror;
    if ( fprintf(jf,"Name=%s\n",actcnm)                      < 0 ) goto werror;

    if ( lsyspk != NULL )
      {
      DBget_id(lsysla,id);
      IGidst(id,idstr);
      if ( fprintf(jf,"ID=%s\n",idstr)                       < 0 ) goto werror;
      if ( fprintf(jf,"DBptr=%d\n",lsysla)                   < 0 ) goto werror;
      if ( write_DBTmat(jf,&lklsys)                          < 0 ) goto werror;
      }

    if ( fprintf(jf,"#End\n")                                < 0 ) goto werror;
/*
***Current graphical attributes.
*/
    if ( fprintf(jf,"\n#Attributes\n")                       < 0 ) goto werror;
    if ( fprintf(jf,"PEN=%d\n",defnap.pen)                   < 0 ) goto werror;
    if ( fprintf(jf,"LEVEL=%d\n",defnap.level)               < 0 ) goto werror;
    if ( fprintf(jf,"PFONT=%d\n",defnap.pfont)               < 0 ) goto werror;
    if ( fprintf(jf,"PSIZE=%g\n",defnap.psize)               < 0 ) goto werror;
    if ( fprintf(jf,"LFONT=%d\n",defnap.lfont)               < 0 ) goto werror;
    if ( fprintf(jf,"LDASHL=%g\n",defnap.ldashl)             < 0 ) goto werror;
    if ( fprintf(jf,"AFONT=%d\n",defnap.afont)               < 0 ) goto werror;
    if ( fprintf(jf,"ADASHL=%g\n",defnap.adashl)             < 0 ) goto werror;
    if ( fprintf(jf,"CFONT=%d\n",defnap.cfont)               < 0 ) goto werror;
    if ( fprintf(jf,"CDASHL=%g\n",defnap.cdashl)             < 0 ) goto werror;
    if ( fprintf(jf,"SFONT=%d\n",defnap.sfont)               < 0 ) goto werror;
    if ( fprintf(jf,"SDASHL=%g\n",defnap.sdashl)             < 0 ) goto werror;
    if ( fprintf(jf,"NULINES=%d\n",defnap.nulines)           < 0 ) goto werror;
    if ( fprintf(jf,"NVLINES=%d\n",defnap.nvlines)           < 0 ) goto werror;
    if ( fprintf(jf,"MFONT=%d\n",defnap.mfont)               < 0 ) goto werror;
    if ( fprintf(jf,"TFONT=%d\n",defnap.tfont)               < 0 ) goto werror;
    if ( fprintf(jf,"TSIZE=%g\n",defnap.tsize)               < 0 ) goto werror;
    if ( fprintf(jf,"TWIDTH=%g\n",defnap.twidth)             < 0 ) goto werror;
    if ( fprintf(jf,"TSLANT=%g\n",defnap.tslant)             < 0 ) goto werror;
    if ( fprintf(jf,"TPMODE=%d\n",defnap.tpmode)             < 0 ) goto werror;
    if ( fprintf(jf,"XFONT=%d\n",defnap.xfont)               < 0 ) goto werror;
    if ( fprintf(jf,"XDASHL=%g\n",defnap.xdashl)             < 0 ) goto werror;
    if ( fprintf(jf,"DAUTO=%d\n",defnap.dauto)               < 0 ) goto werror;
    if ( fprintf(jf,"DTSIZE=%g\n",defnap.dtsize)             < 0 ) goto werror;
    if ( fprintf(jf,"DASIZE=%g\n",defnap.dasize)             < 0 ) goto werror;
    if ( fprintf(jf,"DNDIG=%d\n",defnap.dndig)               < 0 ) goto werror;
    if ( fprintf(jf,"WIDTH=%g\n",defnap.width)               < 0 ) goto werror;
    if ( fprintf(jf,"BLANK=%d\n",defnap.blank)               < 0 ) goto werror;
    if ( fprintf(jf,"HIT=%d\n",defnap.hit)                   < 0 ) goto werror;
    if ( fprintf(jf,"SAVE=%d\n",defnap.save)                 < 0 ) goto werror;
    if ( fprintf(jf,"#End\n")                                < 0 ) goto werror;
/*
***The view table.
*/
    if ( fprintf(jf,"\n#Viewtable\n")                        < 0 ) goto werror;

    WPget_cacc(&curacc);
    if ( fprintf(jf,"Curve_accuracy=%g\n",curacc)            < 0 ) goto werror;

    for ( i=0; i<WPMAXVIEWS; ++i )
      {
      if ( wpviewtab[i].status > 0 )
        {
        if ( fprintf(jf,"Name=%s\n",wpviewtab[i].name)       < 0 ) goto werror;
        if ( write_VYTRF(jf,&wpviewtab[i].matrix)            < 0 ) goto werror;
        }
      }

    if ( fprintf(jf,"#End\n")                                < 0 ) goto werror;
/*
***The level name table. Use window 0 to get the names.
*/
    if ( fprintf(jf,"\n#Leveltable\n")                       < 0 ) goto werror;

    for ( i=0; i<WP_NIVANT; ++i )
      {
      EXget_level(i,0,&blank,name);
      if ( name[0] != '\0' )
        {
        if ( fprintf(jf,"%d=%s\n",i,name)                    < 0 ) goto werror;
        }
      }

    if ( fprintf(jf,"#End\n"));
/*
***Colors and materials.
*/
    if ( fprintf(jf,"\n#Pentable\n")                         < 0 ) goto werror;

    for ( i=0; i<WP_NPENS; ++i )
      {
      WPgmat(i,&defined,&ar,&ag,&ab,&dr,&dg,&db,
                        &sr,&sg,&sb,&er,&eg,&eb,&s);
      if ( defined )
        {
        if ( fprintf(jf,"%d:Material\n",i)                   < 0 ) goto werror;
        if ( fprintf(jf,"Ambient=%d %d %d 255\n",ar,ag,ab)   < 0 ) goto werror;
        if ( fprintf(jf,"Diffuse=%d %d %d 255\n",dr,dg,db)   < 0 ) goto werror;
        if ( fprintf(jf,"Specular=%d %d %d 255\n",sr,sg,sb)  < 0 ) goto werror;
        if ( fprintf(jf,"Emitted=%d %d %d 255\n",er,eg,eb)   < 0 ) goto werror;
        if ( fprintf(jf,"Shininess=%d\n",s)                  < 0 ) goto werror;
        }
      else
        {
        WPgpen(i,&defined,&r,&g,&b);
        if ( defined )
          {
          if ( fprintf(jf,"%d:Color\n",i)                    < 0 ) goto werror;
          if ( fprintf(jf,"RGB=%d %d %d\n",r,g,b)            < 0 ) goto werror;
          }
        }
      }

    if ( fprintf(jf,"#End\n"));
/*
***Graphical windows.
*/
    if ( WPsave_job(jf) < 0 ) goto werror;
/*
***Close the file.
*/
    if ( fclose(jf) == EOF ) return(erpush("EX1883",filename));
/*
***The end.
*/
    return(0);
/*
***Error exit.
*/
werror:
    return(erpush("EX1873",filename));
  }

/********************************************************/
/*!******************************************************/

        short EXload_job(char *filename)

/*      Reads job data from jobfile.
 *
 *      In: filename = C ptr to filenamen width path.
 *
 *      Error: EX1862 => Can't open the file
 *             EX1893 => Error while reading "filenamne".
 *
 *      (C)2007-04-10 1.19 J.Kjellander
 *
 *      2008-01-19 Pentable added, J.Kjellander
 *      2008-10-21 Batch mode, J.Kjellander
 *
 ******************************************************!*/

  {
   FILE  *jf;
   char   line[V3STRLEN+1];
   bool   valid;

/*
***Lets assume the file is invalid. If a valid header can be
***read valid is set to TRUE.
*/
   valid = FALSE;
/*
***Open the file.
*/
   if ( (jf=fopen(filename, "r")) == 0 ) return(erpush("EX1862",filename));
/*
***Read lines until EOF.
***fgets() stops reading AFTER newline or EOF and then adds a \0.
***fgets() returns NULL on EOF or error.
*/
   while ( fgets(line,V3STRLEN,jf) != NULL )
     {
/*
***Find a line that starts with "#".
*/
     if ( line[0] == '#' )
       {
/*
***#VARKON_jobfile
*/
       if ( strncmp(line,"#VARKON_jobfile",15) == 0 )
         {
         if ( get_VARKON_jobfile(jf) < 0 ) return(erpush("EX1893",filename));
         valid = TRUE;
         }
/*
***#Expressions
*/
       else if ( strncmp(line,"#Expressions",12) == 0 )
         {
         if ( get_Expressions(jf) < 0 ) return(erpush("EX1893",filename));
         }
/*
***#Coordinate_system
*/
       else if ( strncmp(line,"#Coordinate_system",18) == 0 )
         {
         if ( get_Coordinate_system(jf) < 0 ) return(erpush("EX1893",filename));
         }
/*
***#Attributes
*/
       else if ( strncmp(line,"#Attributes",11) == 0 )
         {
         if ( get_Attributes(jf) < 0 ) return(erpush("EX1893",filename));
         }
/*
***#Viewtable
*/
       else if ( strncmp(line,"#Viewtable",10) == 0 )
         {
         if ( get_Viewtable(jf) < 0 ) return(erpush("EX1893",filename));
         }
/*
***#Leveltable
*/
       else if ( strncmp(line,"#Leveltable",11) == 0 )
         {
         if ( get_Leveltable(jf) < 0 ) return(erpush("EX1893",filename));
         }
/*
***#Pentable
*/
       else if ( strncmp(line,"#Pentable",9) == 0 )
         {
         if ( get_Pentable(jf) < 0 ) return(erpush("EX1893",filename));
         }
/*
***#WPGWIN
*/
       else if ( strncmp(line,"#WPGWIN",7) == 0 )
         {
         if ( WPload_WPGWIN(jf) < 0 ) return(erpush("EX1893",filename));
         }
/*
***#WPRWIN
*/
       else if ( strncmp(line,"#WPRWIN",7) == 0 )
         {
         if ( WPload_WPRWIN(jf) < 0 ) return(erpush("EX1893",filename));
         }
       }
     }
/*
***Close the file.
*/
    if ( fclose(jf) == EOF ) return(erpush("EX1893",filename));
/*
***The end.
*/
    if ( valid ) return(0);
    else         return(erpush("EX1862",filename));
  }

/********************************************************/
/*!******************************************************/

        short EXclgm()

/*      Interface-rutin för CLEAR_GM(). Nollställer och
 *      initierar GM.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 18/2/86 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Sudda skärm och displayfil.
*/
    WPergw(GWIN_ALL);
/*
***Töm och initiera GM.
*/
    DBreset();

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXldgm(char *filnam)

/*      Interface-rutin för LOAD_GM(fil). Laddar resultatfil
 *      till GM.
 *
 *      In: filnam = Pekare till filnamn.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1642 = Kan ej ladda filen till GM.
 *
 *      (C)microform ab 1/6/86 J. Kjellander
 *
 *      22/4/88  gmexit(), J. Kjellander
 *
 ******************************************************!*/

  {
    short status;

/*
***Sudda skärm och displayfil.
*/
    WPergw(GWIN_ALL);
/*
***Lagra GM.
*/
    if ( (status=DBexit()) < 0 ) return(status);
/*
***Ladda den nya resultatfilen.
*/
    if ( DBload(filnam,sysize.gm,
                DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL) < 0 )
                              return(erpush("EX1642",filnam));
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXsvgm(char *filnam)

/*      Interface-rutin för SAVE_GM(fil). Lagrar GM i
 *      resultatfilen filnam.
 *
 *      In: filnam = Pekare till filnamn.
 *
 *      Ut: Inget.
 *
 *      Felkoder: EX1652 = Kan ej lagra resultatfil
 *
 *      (C)microform ab 1/6/86 J. Kjellander
 *
 *      19/9/95  Ritmodulen, J. Kjellander
 *
 ******************************************************!*/

  {
    short status;
    char  resfil[V3PTHLEN+1];

/*
***Lagra GM.
*/
    if ( (status=DBexit()) < 0 ) return(status);
/*
***Om vi kör bas-moulen heter pagefilen någonting.RES.
*/
    if ( sysmode == GENERIC )
      {
      strcpy(resfil,jobdir);
      strcat(resfil,jobnam);
      strcat(resfil,RESEXT);
      }
/*
***Om vi kör ritmodulen heter den något temporärt.
*/
    else strcpy(resfil,tmpres);
/*
***Kopiera den lagrade resultatfilen till en fil med det nya namnet.
*/ 
    if ( IGfcpy(resfil,filnam) < 0 ) return(erpush("EX1652",filnam));
/*
***Ladda den gamla resultatfilen igen.
*/
    return(DBload(resfil,sysize.gm,
                  DB_LIBVERSION,DB_LIBREVISION,DB_LIBLEVEL));
  }

/********************************************************/
/*!******************************************************/

 static short  write_DBTmat(
        FILE  *jf,
        DBTmat *matrix)

/*      Write a 4X4 transformation matrix to the jobfile.
 *
 *      In: jf     = C Ptr to open file
 *          matrix = C Ptr to matrix
 *
 *      (C)2007-03-19 J. Kjellander
 *
 ******************************************************!*/

  {
   if ( fprintf(jf,"Matrix:\n")                     < 0 ) return(-1);
   if ( fprintf(jf,"%g,%g,",matrix->g11,matrix->g12)  < 0 ) return(-1);
   if ( fprintf(jf,"%g,%g\n",matrix->g13,matrix->g14) < 0 ) return(-1);
   if ( fprintf(jf,"%g,%g,",matrix->g21,matrix->g22)  < 0 ) return(-1);
   if ( fprintf(jf,"%g,%g\n",matrix->g23,matrix->g24) < 0 ) return(-1);
   if ( fprintf(jf,"%g,%g,",matrix->g31,matrix->g32)  < 0 ) return(-1);
   if ( fprintf(jf,"%g,%g\n",matrix->g33,matrix->g34) < 0 ) return(-1);
   if ( fprintf(jf,"%g,%g,",matrix->g41,matrix->g42)  < 0 ) return(-1);
   if ( fprintf(jf,"%g,%g\n",matrix->g43,matrix->g44) < 0 ) return(-1);

   return(0);
  }

/******************************************************!*/
/*!******************************************************/

 static short  read_DBTmat(
        FILE  *jf,
        DBTmat *matrix)

/*      Reads a 4X4 transformation matrix from the jobfile.
 *
 *      In: jf     = C Ptr to open file
 *          matrix = C Ptr to matrix
 *
 *      (C)2007-03-19 J. Kjellander
 *
 ******************************************************!*/

  {
   char c,line[V3STRLEN+1];

/*
***Eat the Matrix: line.
*/
   if ( fgets(line,V3STRLEN,jf) == NULL )                     return(-1);
   if ( strncmp(line,"Matrix:",7) != 0 )                      return(-1);
/*
***Get the matrix.
*/
   if ( fgets(line,V3STRLEN,jf) == NULL )                     return(-1);
   if ( sscanf(line,"%lf%c%lf%c%lf%c%lf,",&matrix->g11,&c,
                                          &matrix->g12,&c,
                                          &matrix->g13,&c,
                                          &matrix->g14) < 7 ) return(-1);

   if ( fgets(line,V3STRLEN,jf) == NULL )                     return(-1);
   if ( sscanf(line,"%lf%c%lf%c%lf%c%lf,",&matrix->g21,&c,
                                          &matrix->g22,&c,
                                          &matrix->g23,&c,
                                          &matrix->g24) < 7 ) return(-1);

   if ( fgets(line,V3STRLEN,jf) == NULL )                     return(-1);
   if ( sscanf(line,"%lf%c%lf%c%lf%c%lf,",&matrix->g31,&c,
                                          &matrix->g32,&c,
                                          &matrix->g33,&c,
                                          &matrix->g34) < 7 ) return(-1);

   if ( fgets(line,V3STRLEN,jf) == NULL )                     return(-1);
   if ( sscanf(line,"%lf%c%lf%c%lf%c%lf,",&matrix->g41,&c,
                                          &matrix->g42,&c,
                                          &matrix->g43,&c,
                                          &matrix->g44) < 7 ) return(-1);

   return(0);
  }

/******************************************************!*/
/*!******************************************************/

 static short  write_VYTRF(
        FILE  *jf,
        VYTRF *matrix)

/*      Write a 3X3 view transformation matrix to the jobfile.
 *
 *      In: jf     = C Ptr to open file
 *          matrix = C Ptr to matrix
 *
 *      (C)2007-03-19 J. Kjellander
 *
 ******************************************************!*/

  {
   if ( fprintf(jf,"Matrix:\n")                                      < 0 )
     return(-1);
   if ( fprintf(jf,"%g,%g,%g\n",matrix->k11,matrix->k12,matrix->k13) < 0 )
     return(-1);
   if ( fprintf(jf,"%g,%g,%g\n",matrix->k21,matrix->k22,matrix->k23) < 0 )
     return(-1);
   if ( fprintf(jf,"%g,%g,%g\n",matrix->k31,matrix->k32,matrix->k33) < 0 )
     return(-1);

   return(0);
  }

/******************************************************!*/
/*!******************************************************/

 static short  read_VYTRF(
        FILE  *jf,
        VYTRF *matrix)

/*      Read a 3X3 view transformation matrix from the jobfile.
 *
 *      In: jf     = C Ptr to open file
 *          matrix = C Ptr to matrix
 *
 *      (C)2007-03-19 J. Kjellander
 *
 ******************************************************!*/

  {
   char c,line[V3STRLEN+1];

/*
***Eat the Matrix: line.
*/
   if ( fgets(line,V3STRLEN,jf) == NULL )                     return(-1);
   if ( strncmp(line,"Matrix:",7) != 0 )                      return(-1);
/*
***Get the matrix.
*/
   if ( fgets(line,V3STRLEN,jf) == NULL )                return(-1);
   if ( sscanf(line,"%lf%c%lf%c%lf,",&matrix->k11,&c,
                                     &matrix->k12,&c,
                                     &matrix->k13) < 5 ) return(-1);

   if ( fgets(line,V3STRLEN,jf) == NULL )                return(-1);
   if ( sscanf(line,"%lf%c%lf%c%lf,",&matrix->k21,&c,
                                     &matrix->k22,&c,
                                     &matrix->k23) < 5 ) return(-1);

   if ( fgets(line,V3STRLEN,jf) == NULL )                return(-1);
   if ( sscanf(line,"%lf%c%lf%c%lf,",&matrix->k31,&c,
                                     &matrix->k32,&c,
                                     &matrix->k33) < 5 ) return(-1);

   return(0);
  }

/******************************************************!*/
/*!******************************************************/

 static short get_VARKON_jobfile(FILE *jf)

/*      Reads a #VARKON_jobfile record from a jobfile.
 *
 *      In: jf = C Ptr to open file
 *
 *      (C)2007-03-19 J. Kjellander
 *
 ******************************************************!*/

  {
   char line[V3STRLEN+1];


   while ( fgets(line,V3STRLEN,jf) != NULL )
     {
/*
***Read until next "#End".
*/
     if ( strncmp(line,"#End",4) == 0 )
       {
       return(0);
       }
/*
***Serial=n
*/
     else if ( strncmp(line,"Serial=",7) == 0 )
       {
       ;
       }
     }
/*
***If we reach here something went wrong.
*/
   return(-1);
  }

/******************************************************!*/
/*!******************************************************/

 static short get_Expressions(FILE *jf)

/*      Reads a #Expression record from a jobfile.
 *
 *      In: jf = C Ptr to open file
 *
 *      (C)2007-03-19 J. Kjellander
 *
 *      2007-09-09 relpos, J.Kjellander
 *
 ******************************************************!*/

  {
   char line[V3STRLEN+1];

/*
***Init posmode and relpos.
*/
   posmode = 2;
   relpos = FALSE;
/*
***Read file.
*/
   while ( fgets(line,V3STRLEN,jf) != NULL )
     {
/*
***Read until next "#End".
*/
     if ( strncmp(line,"#End",4) == 0 )
       {
       return(0);
       }
/*
***Position_method=n
*/
     else if ( strncmp(line,"Position_method=",16) == 0 )
       {
       if ( sscanf(&line[16],"%d",&posmode) == 0 ) return(-1);
       }
/*
***Relative=On
*/
     else if ( strncmp(line,"Relative=On",11) == 0 )
       {
       relpos = TRUE;
       }
     }
/*
***If we reach here something went wrong.
*/
   return(-1);
  }

/******************************************************!*/
/*!******************************************************/

 static short get_Coordinate_system(FILE *jf)

/*      Reads a #Coordinate_system record from a jobfile.
 *
 *      In: jf = C Ptr to open file
 *
 *      (C)2007-03-19 J. Kjellander
 *
 ******************************************************!*/

  {
   char    line[V3STRLEN+1];

/*
***Start with the assumption that a local system is not active.
*/
   lsyspk = NULL;
/*
***Start reading.
*/
   while ( fgets(line,V3STRLEN,jf) != NULL )
     {
/*
***Read until next "#End".
*/
     if ( strncmp(line,"#End",4) == 0 )
       {
       return(0);
       }
/*
***Name=string
*/
     else if ( strncmp(line,"Name=",5) == 0 )
       {
       strcpy(actcnm,"GLOBAL");
       if ( sscanf(&line[5],"%s",actcnm) == 0 ) return(-1);
       }
/*
***ID=string. This means that a local csys is active.
***For the time being we don't care about the ID but
***we must have a DBptr + a matrix here.
*/
     else if ( strncmp(line,"ID=",3) == 0 )
       {
       if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
       if ( strncmp(line,"DBptr=",6) != 0 ) return(-1);
       if ( sscanf(&line[6],"%d",&lsysla) == 0 ) return(-1);
       if ( read_DBTmat(jf,&lklsys) < 0 ) return(-1);
       lsyspk = &lklsys;
       }
     }
/*
***If we reach here something went wrong.
*/
   return(-1);
  }

/******************************************************!*/
/*!******************************************************/

 static short get_Attributes(FILE *jf)

/*      Reads a #Attributes record from a jobfile.
 *
 *      In: jf = C Ptr to open file
 *
 *      (C)2007-03-22 J. Kjellander
 *
 ******************************************************!*/

  {
   char line[V3STRLEN+1];

   while ( fgets(line,V3STRLEN,jf) != NULL )
     {
/*
***Read until next "#End".
*/
     if ( strncmp(line,"#End",4) == 0 )
       {
       return(0);
       }
/*
***ATTRIBUTE=n
*/
     else if ( strncmp(line,"PEN=",4) == 0 )
       { if ( sscanf(&line[4],"%d",&defnap.pen) == 0 )     return(-1); }
     else if ( strncmp(line,"LEVEL=",6) == 0 )
       { if ( sscanf(&line[6],"%d",&defnap.level) == 0 )   return(-1); }
     else if ( strncmp(line,"PFONT=",6) == 0 )
       { if ( sscanf(&line[6],"%d",&defnap.pfont) == 0 )   return(-1); }
     else if ( strncmp(line,"PSIZE=",6) == 0 )
       { if ( sscanf(&line[6],"%lf",&defnap.psize) == 0 )  return(-1); }
     else if ( strncmp(line,"LFONT=",6) == 0 )
       { if ( sscanf(&line[6],"%d",&defnap.lfont) == 0 )   return(-1); }
     else if ( strncmp(line,"LDASHL=",7) == 0 )
       { if ( sscanf(&line[7],"%lf",&defnap.ldashl) == 0 ) return(-1); }
     else if ( strncmp(line,"AFONT=",6) == 0 )
       { if ( sscanf(&line[6],"%d",&defnap.afont) == 0 )   return(-1); }
     else if ( strncmp(line,"ADASHL=",7) == 0 )
       { if ( sscanf(&line[7],"%lf",&defnap.adashl) == 0 ) return(-1); }
     else if ( strncmp(line,"CFONT=",6) == 0 )
       { if ( sscanf(&line[6],"%d",&defnap.cfont) == 0 )   return(-1); }
     else if ( strncmp(line,"CDASHL=",7) == 0 )
       { if ( sscanf(&line[7],"%lf",&defnap.cdashl) == 0 ) return(-1); }
     else if ( strncmp(line,"SFONT=",6) == 0 )
       { if ( sscanf(&line[6],"%d",&defnap.sfont) == 0 )   return(-1); }
     else if ( strncmp(line,"SDASHL=",7) == 0 )
       { if ( sscanf(&line[7],"%lf",&defnap.sdashl) == 0 ) return(-1); }
     else if ( strncmp(line,"NULINES=",8) == 0 )
       { if ( sscanf(&line[8],"%d",&defnap.nulines) == 0 ) return(-1); }
     else if ( strncmp(line,"NVLINES=",8) == 0 )
       { if ( sscanf(&line[8],"%d",&defnap.nvlines) == 0 ) return(-1); }
     else if ( strncmp(line,"MFONT=",6) == 0 )
       { if ( sscanf(&line[6],"%d",&defnap.mfont) == 0 )   return(-1); }
     else if ( strncmp(line,"TFONT=",6) == 0 )
       { if ( sscanf(&line[6],"%d",&defnap.tfont) == 0 )   return(-1); }
     else if ( strncmp(line,"TSIZE=",6) == 0 )
       { if ( sscanf(&line[6],"%lf",&defnap.tsize) == 0 )  return(-1); }
     else if ( strncmp(line,"TWIDTH=",7) == 0 )
       { if ( sscanf(&line[7],"%lf",&defnap.twidth) == 0 ) return(-1); }
     else if ( strncmp(line,"TSLANT=",7) == 0 )
       { if ( sscanf(&line[7],"%lf",&defnap.tslant) == 0 ) return(-1); }
     else if ( strncmp(line,"TPMODE=",7) == 0 )
       { if ( sscanf(&line[7],"%d",&defnap.tpmode) == 0 )  return(-1); }
     else if ( strncmp(line,"XFONT=",6) == 0 )
       { if ( sscanf(&line[6],"%d",&defnap.xfont) == 0 )   return(-1); }
     else if ( strncmp(line,"XDASHL=",7) == 0 )
       { if ( sscanf(&line[7],"%lf",&defnap.xdashl) == 0 ) return(-1); }
     else if ( strncmp(line,"DAUTO=",6) == 0 )
       { if ( sscanf(&line[6],"%d",&defnap.dauto) == 0 )   return(-1); }
     else if ( strncmp(line,"DTSIZE=",7) == 0 )
       { if ( sscanf(&line[7],"%lf",&defnap.dtsize) == 0 ) return(-1); }
     else if ( strncmp(line,"DASIZE=",7) == 0 )
       { if ( sscanf(&line[7],"%lf",&defnap.dasize) == 0 ) return(-1); }
     else if ( strncmp(line,"DNDIG=",6) == 0 )
       { if ( sscanf(&line[6],"%d",&defnap.dndig) == 0 )   return(-1); }
     else if ( strncmp(line,"WIDTH=",6) == 0 )
       { if ( sscanf(&line[6],"%lf",&defnap.width) == 0 )  return(-1); }
     else if ( strncmp(line,"BLANK=",6) == 0 )
       { if ( sscanf(&line[6],"%d",&defnap.blank) == 0 )   return(-1); }
     else if ( strncmp(line,"HIT=",4) == 0 )
       { if ( sscanf(&line[4],"%d",&defnap.hit) == 0 )     return(-1); }
     else if ( strncmp(line,"SAVE=",5) == 0 )
       { if ( sscanf(&line[5],"%d",&defnap.save) == 0 )    return(-1); }
     }
/*
***If we reach here something went wrong.
*/
   return(-1);
  }

/******************************************************!*/
/*!******************************************************/

 static short get_Viewtable(FILE *jf)

/*      Reads a #Viewtable record from a jobfile.
 *
 *      In: jf = C Ptr to open file
 *
 *      (C)2007-03-19 J. Kjellander
 *
 ******************************************************!*/

  {
   char   line[V3STRLEN+1];
   double curacc;
   WPVIEW view;


   while ( fgets(line,V3STRLEN,jf) != NULL )
     {
/*
***Read until next "#End".
*/
     if ( strncmp(line,"#End",4) == 0 )
       {
       return(0);
       }
/*
***Curve_accuracy=n
*/
     if ( strncmp(line,"Curve_accuracy=",15) == 0 )
       {
       if ( sscanf(&line[15],"%lf",&curacc) == 0 ) return(-1);
       WPset_cacc(curacc);
       }
/*
***Name=string
*/
     else if ( strncmp(line,"Name=",5) == 0 )
       {
       if ( sscanf(&line[5],"%s",view.name) == 0 ) return(-1);
       if ( read_VYTRF(jf,&view.matrix) < 0 )      return(-1);
       WPcreate_view(&view,VIEW_3D_ONLY);
       }
     }
/*
***If we reach here something went wrong.
*/
   return(-1);
  }

/******************************************************!*/
/*!******************************************************/

 static short get_Leveltable(FILE *jf)

/*      Reads a #Leveltable record from a jobfile.
 *
 *      In: jf = C Ptr to open file
 *
 *      (C)2007-03-31 J. Kjellander
 *
 ******************************************************!*/

  {
   char line[V3STRLEN+1];
   int  level,n_chars,nl;


   while ( fgets(line,V3STRLEN,jf) != NULL )
     {
/*
***Read until next "#End".
*/
     if ( strncmp(line,"#End",4) == 0 )
       {
       return(0);
       }
/*
***Levelnumber=name. Strip newline in name string.
*/
     else
       {
       if ( sscanf(line,"%d%*[=]%n",&level,&n_chars) < 1 ) return(-1);
       nl = strlen(line) - 1;
       if ( line[nl] == '\n' ) line[nl] = '\0';
       EXname_level(level,&line[n_chars]);
       }
     }
/*
***If we reach here something went wrong.
*/
   return(-1);
  }

/******************************************************!*/
/*!******************************************************/

 static short get_Pentable(FILE *jf)

/*      Reads a #Pentable record from a jobfile.
 *
 *      In: jf = C Ptr to open file
 *
 *      (C)2008-01-19 J. Kjellander
 *
 ******************************************************!*/

  {
   char line[V3STRLEN+1];
   int  pen,n,r,g,b,ar,ag,ab,aa,dr,dg,db,da,sr,sg,sb,sa,
        er,eg,eb,ea,s;


   while ( fgets(line,V3STRLEN,jf) != NULL )
     {
/*
***Read until next "#End".
*/
     if ( strncmp(line,"#End",4) == 0 )
       {
       return(0);
       }
/*
***Pennumber:Color or Pennumber:Material
*/
     else
       {
       if ( sscanf(line,"%d%n",&pen,&n) < 1 ) return(-1);
       if ( strncmp(&line[n],":Color",6) == 0 )
         {
         if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
         if ( sscanf(&line[4],"%d%d%d",&r,&g,&b) < 3 ) return(-1);
         WPccol(pen,r,g,b);
         }
       else if ( strncmp(&line[n],":Material",9) == 0 )
         {
         if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
         if ( sscanf(&line[8],"%d%d%d%d",&ar,&ag,&ab,&aa) < 4 ) return(-1);
         if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
         if ( sscanf(&line[8],"%d%d%d%d",&dr,&dg,&db,&da) < 4 ) return(-1);
         if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
         if ( sscanf(&line[9],"%d%d%d%d",&sr,&sg,&sb,&sa) < 4 ) return(-1);
         if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
         if ( sscanf(&line[8],"%d%d%d%d",&er,&eg,&eb,&ea) < 4 ) return(-1);
         if ( fgets(line,V3STRLEN,jf) == NULL ) return(-1);
         if ( sscanf(&line[10],"%d",&s) < 1 ) return(-1);
         WPcmat(pen,ar,ag,ab,dr,dg,db,sr,sg,sb,er,eg,eb,s);
         }
       }
     }
/*
***If we reach here something went wrong.
*/
   return(-1);
  }

/******************************************************!*/
