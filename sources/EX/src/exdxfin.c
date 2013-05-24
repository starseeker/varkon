/*!******************************************************
*
*    exdxfin.c
*    =========
*
*    This file includes the following public functions:
*
*    EXimport_dxf();    Imports DXF-file
*
*    This file is part of the VARKON Execute Library.
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
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
*********************************************************/

/*
***This is a first version of EXimport_dxf. It only supports
***POINT and LINE entities and only reads their coordinates
***from the file. All other data like layer or pen number is
***skipped. All POINTS and LINES in the DXF file are stored
***as members to a single DBPart in the DB. POINTS and LINES
***are translated with px and py but not rotated or scaled.
***This would have been easy with the GEO module but since
***it's not yet available I think we can just do something
***simple here and fix that later.
*/

#include <string.h>
#include "../../DB/include/DB.h"

static int    pass;   /* 1=pass1 and 2=pass2 */
static double tx,ty;  /* Temporar fix */
static FILE  *df,*lf; /* Filepointers to the current DXF-file and logfile */
static DBId   ent_id; /* DB entity ID */

static int    pr_dxf(),pr_hedsec(),pr_tabsec(),pr_blksec(),
              pr_entsec(),pr_xsec();
static int    pr_point(),pr_line();
static char  *get_lin();


/*!******************************************************/

        int     EXimport_dxf(
        char   *dxffile,
        char   *name,
        DBfloat px,
        DBfloat py,
        DBfloat angle,
        DBfloat scale,
        char   *logfile)

/*      Imports DXF-file. The file is read and entities
 *      found are stored in DB as a part. An optional
 *      logfile is created.
 *
 *      In pass 1 the DXF file is only parsed and
 *      checked for validity.
 *      In pass 2 the file is read again and entities
 *      are inserted into the DB.
 *
 *      In:
 *          dxffile = Name and path of DXF file
 *          name    = Name to give the part in the DB
 *          px,py   = Base position
 *          angle   = CCW rotation
 *          scale   = Scalefactor
 *          logfile = Name and path to logfile or ""
 *
 *      Return:  0 = Ok.
 *              -1 = Error, check logfile for details.
 *
 *      (C)microform ab 1999-02-08 J. Kjellander
 *
 ******************************************************!*/

  {
   DBstatus errstat;
   DBptr    ptr;
   DBId     part_id;
   DBPart   part;

/*
***Pass 1.
***Process the DXF file for errors and count the
***number of entities.
*/
   pass = 1;
   if ( pr_dxf(dxffile,logfile) < 0 ) return(-1);
/*
***If no entities were found, exit without creating
***a part in the DB.
*/
   if ( ent_id.seq_val == 1 ) return(0);
/*
***Pass 2.
***Get a free entiy id from the DB to use for the part.
*/
   DBget_free_id(&part_id.seq_val);
/*
***Create/open a new part in the DB.
*/
   part.hed_pt.blank = FALSE;
   part.hed_pt.hit   = TRUE;
   part.hed_pt.pen   = 1;
   part.hed_pt.level = 1;
   part.its_pt       = ent_id.seq_val;    /* Number of ents. + 1 */
   strcpy(part.name_pt,name);
 
   errstat = DBinsert_part(&part,NULL,NULL,NULL,&part_id,NULL,&ptr);
   if ( errstat < 0 )
     {
     if ( lf != NULL )
       {
       fprintf(lf,"Error fron DBinsert_part, errstat=%d\n",(int)errstat);
       fclose(lf);
       }
     return(-1);
     }
/*
***Set tx and ty so taht entities are correctly placed. This
***will be replaced with GEO-calls later.
*/
   tx = px;
   ty = py;
/*
***Pass 2. Save entities in DB, don't write to logfile this time.
*/
   pass = 2;
   if ( pr_dxf(dxffile,"") < 0 ) return(-1);
/*
***Close part.
*/
   DBclose_part();
/*
***End.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

static  int   pr_dxf(dxffile,logfile)
        char *dxffile;
        char *logfile;

/*      Processes the complete DXF file.
 *
 *      (C)microform ab 1999-02-08 J. Kjellander
 *
 ******************************************************!*/

  {
   char     linbuf[256];
   int      gc;

/*
***Is a logfile requested ?
*/
   if ( *logfile == '\0' ) lf = NULL;
   else                    lf = fopen(logfile,"w");
/*
***Init logfile.
*/
   if ( lf != NULL )
     {
     fprintf(lf,"DXF-logfile generated by EXimport_dxf\n");
     fprintf(lf,"DXF file = %s\n",dxffile);
     }
/*
***Open the DXF-file for direct access.
*/
   if ( (df=fopen(dxffile,"r+")) == NULL )
     {
     if ( lf != NULL )
       {
       fprintf(lf,"Can't open %s\n",dxffile);
       fclose(lf);
       }
     return(-1);
     }
/*
***Set id of first entity to 1. Each entity increments
***ent_id.seq_val with 1.
*/
   ent_id.seq_val = 1;
/*
***Read next line from DXF-file.
*/
loop:
   if ( get_lin(linbuf) == NULL ) goto error1;
/*
***Skip lines until a zero group code is found.
*/
   if ( sscanf(linbuf,"%d",&gc) != 1 ) goto loop;
/*
***This may be the beginning of a new section.
*/
   if ( gc == 0 )
     {
     if ( get_lin(linbuf) == NULL ) goto error1;
     if ( strcmp(linbuf,"SECTION") == 0 )
       {
/*
***A section is found.
*/
secloop:
       if ( get_lin(linbuf) == NULL ) goto error1;
       if ( sscanf(linbuf,"%d",&gc) != 1 ) goto secloop;
       if ( gc == 2 )
         {
         if ( get_lin(linbuf) == NULL ) goto error1;
/*
***HEADER section.
*/
         if ( strcmp(linbuf,"HEADER") == 0 )
           {
           if ( pr_hedsec() < 0 ) goto error2;
           }
/*
***TABLES section.
*/
         else if ( strcmp(linbuf,"TABLES") == 0 )
           {
           if ( pr_tabsec() < 0 ) goto error2;
           }
/*
***BLOCKS section.
*/
         else if ( strcmp(linbuf,"BLOCKS") == 0 )
           {
           if ( pr_blksec() < 0 ) goto error2;
           }
/*
***ENTITIES section.
*/
         else if ( strcmp(linbuf,"ENTITIES") == 0 )
           {
           if ( pr_entsec() < 0 ) goto error2;
           }
/*
***Unknown SECTION.
*/
         else
           {
           if ( lf != NULL )
             fprintf(lf,"Processing unknown section: %s\n",linbuf);
           if ( pr_xsec() < 0 ) goto error2;
           }
         }
/*
***Next section.
*/
       else goto loop;
/*
***EOF.
*/
       }
     else if ( strcmp(linbuf,"EOF") == 0 )
       {
       if ( lf != NULL )fprintf(lf,"DXF file successfully processed\n");
       goto end;
       }
     goto loop;
     }
/*
***Close file(s) and exit normally.
*/
end:
   fclose(df);
   if ( lf != NULL ) fclose(lf);
   return(0);
/*
***Unexpected end of file.
*/
error1:
   if ( lf != NULL ) fprintf(lf,"Unexpected end_of_file...\n");
/*
***Other error.
*/
error2:
   fclose(df);
   if ( lf != NULL ) fclose(lf);
   return(-1);
  }

/********************************************************/
/*!******************************************************/

static char *get_lin(char *buf)

/*      Reads a line from df and strips trailing \n and \r.
 *
 *      (C)microform ab 1999-02-08 J. Kjellander
 *
 ******************************************************!*/

  {
   int   i;
   char *cp;

/*
***Read a line.
*/
   if ( fgets(buf,256,df) == NULL ) return(NULL);
/*
***Strip CR and newline characters.
*/
   cp = buf;

   for ( i=0; i<256; ++i, ++cp )
     {
     if ( *cp == '\n'  ||  *cp == '\r' || *cp == '\0' )
       {
      *cp = '\0';
       return(buf);
       }
     }
   return(NULL);
  }

/********************************************************/
/*!******************************************************/

static int pr_hedsec()

/*      Processes HEADER-section.
 *
 *      (C)microform ab 1999-02-08 J. Kjellander
 *
 ******************************************************!*/

  {
   char linbuf[256];
   int  gc;

/*
***Logfile message.
*/
   if ( lf != NULL )fprintf(lf,"Processing HEADER section\n");
/*
***Read next line from DXF-file.
*/
loop:
   if ( get_lin(linbuf) == NULL ) goto error1;
/*
***Skip lines until a zero group code is found.
*/
   if ( sscanf(linbuf,"%d",&gc) != 1 ) goto loop;
/*
***This may be the end of the section.
*/
   if ( gc == 0 )
     {
     if ( get_lin(linbuf) == NULL ) goto error1;
     if ( strcmp(linbuf,"ENDSEC") == 0 )
       {
       return(0);
       }
     else goto loop;
     }
   else goto loop;
/*
***Unexpected end of file.
*/
error1:
   if ( lf != NULL ) fprintf(lf,"Unexpected end_of_file in HEADER-section\n");
   return(-1);
  }

/********************************************************/
/*!******************************************************/

static int pr_tabsec()

/*      Processes TABLES-section.
 *
 *      (C)microform ab 1999-02-08 J. Kjellander
 *
 ******************************************************!*/

  {
   char linbuf[256];
   int  gc;

/*
***Logfile message.
*/
   if ( lf != NULL )fprintf(lf,"Processing TABLES section\n");
/*
***Read next line from DXF-file.
*/
loop:
   if ( get_lin(linbuf) == NULL ) goto error1;
/*
***Skip lines until a zero group code is found.
*/
   if ( sscanf(linbuf,"%d",&gc) != 1 ) goto loop;
/*
***This may be the end of the section.
*/
   if ( gc == 0 )
     {
     if ( get_lin(linbuf) == NULL ) goto error1;
     if ( strcmp(linbuf,"ENDSEC") == 0 )
       {
       return(0);
       }
     else goto loop;
     }
   else goto loop;
/*
***Unexpected end of file.
*/
error1:
   if ( lf != NULL ) fprintf(lf,"Unexpected end_of_file in TABLES-section\n");
   return(-1);
  }

/********************************************************/
/*!******************************************************/

static int pr_blksec()

/*      Processes BLOCKS-section.
 *
 *      (C)microform ab 1999-02-08 J. Kjellander
 *
 ******************************************************!*/

  {
   char linbuf[256];
   int  gc;

/*
***Logfile message.
*/
   if ( lf != NULL )fprintf(lf,"Processing BLOCKS section\n");
/*
***Read next line from DXF-file.
*/
loop:
   if ( get_lin(linbuf) == NULL ) goto error1;
/*
***Skip lines until a zero group code is found.
*/
   if ( sscanf(linbuf,"%d",&gc) != 1 ) goto loop;
/*
***This may be the end of the section.
*/
   if ( gc == 0 )
     {
     if ( get_lin(linbuf) == NULL ) goto error1;
     if ( strcmp(linbuf,"ENDSEC") == 0 )
       {
       return(0);
       }
     else goto loop;
     }
   else goto loop;
/*
***Unexpected end of file.
*/
error1:
   if ( lf != NULL ) fprintf(lf,"Unexpected end_of_file in BLOCKS-section\n");
   return(-1);
  }

/********************************************************/
/*!******************************************************/

static int pr_entsec()

/*      Processes ENTITIES-section.
 *
 *      (C)microform ab 1999-02-08 J. Kjellander
 *
 ******************************************************!*/

  {
   char linbuf[256];
   int  gc;

/*
***Logfile message.
*/
   if ( lf != NULL )fprintf(lf,"Processing ENTITIES section\n");
/*
***Read next line from DXF-file.
*/
loop:
   if ( get_lin(linbuf) == NULL ) goto error1;
/*
***Skip lines until a zero group code is found.
*/
   if ( sscanf(linbuf,"%d",&gc) != 1 ) goto loop;
/*
***This may be the end of the section.
*/
   if ( gc == 0 )
     {
entloop:
     if ( get_lin(linbuf) == NULL ) goto error1;
     if ( strcmp(linbuf,"ENDSEC") == 0 )
       {
       return(0);
       }
/*
***Or it may be a POINT entity.
*/
     else if ( strcmp(linbuf,"POINT") == 0 )
       {
       if ( pr_point() < 0 ) return(-1);
       goto entloop;
       }
/*
***Or it may be a LINE entity.
*/
     else if ( strcmp(linbuf,"LINE") == 0 )
       {
       if ( pr_line() < 0 ) return(-1);
       goto entloop;
       }
/*
***Or it may be an unknown entity.
*/
     goto loop;
     }
   goto loop;
/*
***Unexpected end of file.
*/
error1:
   if ( lf != NULL ) fprintf(lf,"Unexpected end_of_file in ENTITIES-section\n");
   return(-1);
  }

/********************************************************/
/********************************************************/

static int pr_xsec()

/*      Processes unknown section.
 *
 *      (C)microform ab 1999-02-08 J. Kjellander
 *
 ******************************************************!*/

  {
   char linbuf[256];
   int  gc;

/*
***Read next line from DXF-file.
*/
loop:
   if ( get_lin(linbuf) == NULL ) goto error1;
/*
***Skip lines until a zero group code is found.
*/
   if ( sscanf(linbuf,"%d",&gc) != 1 ) goto loop;
/*
***This may be the end of the section.
*/
   if ( gc == 0 )
     {
     if ( get_lin(linbuf) == NULL ) goto error1;
     if ( strcmp(linbuf,"ENDSEC") == 0 )
       {
       return(0);
       }
     else goto loop;
     }
   else goto loop;
/*
***Unexpected end of file.
*/
error1:
   if ( lf != NULL ) fprintf(lf,"Unexpected end_of_file in unknown section\n");
   return(-1);
  }

/********************************************************/
/********************************************************/

static int pr_point()

/*      Processes POINT entity.
 *
 *      (C)microform ab 1999-02-08 J. Kjellander
 *
 ******************************************************!*/

  {
   char     linbuf[256];
   int      gc;
   double   x,y;
   DBstatus errstat;
   DBptr    ptr;
   DBPoint  point;

/*
***Logfile message.
*/
   if ( lf != NULL ) fprintf(lf,"Processing POINT entity\n");
/*
***Read next line from DXF-file.
*/
loop:
   if ( get_lin(linbuf) == NULL ) goto error1;
/*
***Skip lines until group code is found.
*/
   if ( sscanf(linbuf,"%d",&gc) != 1 ) goto loop;
/*
***X-coordinate.
*/
   if ( gc == 10 )
     {
     if ( get_lin(linbuf) == NULL ) goto error1;
     if ( sscanf(linbuf,"%lf",&x) != 1 ) goto error2;
     }
/*
***Y-coordinate.
*/
   else if ( gc == 20 )
     {
     if ( get_lin(linbuf) == NULL ) goto error1;
     if ( sscanf(linbuf,"%lf",&y) != 1 ) goto error2;
     }
/*
***Save in DB.
*/
   else if ( gc == 0 )
     {
     if ( lf != NULL ) fprintf(lf,"X=%g, Y=%g\n",x,y);
     if ( pass == 2 )
       {
       point.hed_p.blank = FALSE;
       point.hed_p.hit   = TRUE;
       point.hed_p.pen   = 1;
       point.hed_p.level = 1;
       point.crd_p.x_gm  = x + tx;
       point.crd_p.y_gm  = y + ty;
       point.crd_p.z_gm  = 0.0;
       point.pcsy_p      = DBNULL;
       point.wdt_p       = 0.0;
       errstat = DBinsert_point(&point,&ent_id,&ptr);
       if ( errstat < 0 ) goto error3;
       }
   ++ent_id.seq_val;
     return(0);
     }
/*
***Skip all other group codes.
*/
   goto loop;
/*
***Unexpected end of file.
*/
error1:
   if ( lf != NULL ) fprintf(lf,"Unexpected end_of_file while reading POINT\n");
   return(-1);
/*
***Parse error.
*/
error2:
   if ( lf != NULL ) fprintf(lf,"Parse error while reading POINT\n");
   return(-1);
/*
***DB error.
*/
error3:
    if ( lf != NULL )
       fprintf(lf,"Error from DBinsert_point(), errstat=%d\n",(int)errstat);
    return(-1);
  }

/********************************************************/
/********************************************************/

static int pr_line()

/*      Processes LINE entity.
 *
 *      (C)microform ab 1999-02-08 J. Kjellander
 *
 ******************************************************!*/

  {
   char     linbuf[256];
   int      gc;
   double   x1,x2,y1,y2;
   DBstatus errstat;
   DBptr    ptr;
   DBLine   line;

/*
***Logfile message.
*/
   if ( lf != NULL ) fprintf(lf,"Processing LINE entity\n");
/*
***Read next line from DXF-file.
*/
loop:
   if ( get_lin(linbuf) == NULL ) goto error1;
/*
***Skip lines until group code is found.
*/
   if ( sscanf(linbuf,"%d",&gc) != 1 ) goto loop;
/*
***X1-coordinate.
*/
   if ( gc == 10 )
     {
     if ( get_lin(linbuf) == NULL ) goto error1;
     if ( sscanf(linbuf,"%lf",&x1) != 1 ) goto error2;
     }
/*
***X2-coordinate.
*/
   if ( gc == 11 )
     {
     if ( get_lin(linbuf) == NULL ) goto error1;
     if ( sscanf(linbuf,"%lf",&x2) != 1 ) goto error2;
     }
/*
***Y1-coordinate.
*/
   else if ( gc == 20 )
     {
     if ( get_lin(linbuf) == NULL ) goto error1;
     if ( sscanf(linbuf,"%lf",&y1) != 1 ) goto error2;
     }
/*
***Y2-coordinate.
*/
   else if ( gc == 21 )
     {
     if ( get_lin(linbuf) == NULL ) goto error1;
     if ( sscanf(linbuf,"%lf",&y2) != 1 ) goto error2;
     }
/*
***End of entity.
*/
   else if ( gc == 0 )
     {
     if ( lf != NULL ) fprintf(lf,"X1=%g, Y1=%g\n",x1,y1);
     if ( lf != NULL ) fprintf(lf,"X2=%g, Y2=%g\n",x2,y2);
     if ( pass == 2 )
       {
       line.hed_l.blank  = FALSE;
       line.hed_l.hit    = TRUE;
       line.hed_l.pen    = 1;
       line.hed_l.level  = 1;
       line.fnt_l        = 0;
       line.lgt_l        = 0.0;
       line.crd1_l.x_gm  = x1 + tx;
       line.crd1_l.y_gm  = y1 + ty;
       line.crd1_l.z_gm  = 0.0;
       line.crd2_l.x_gm  = x2 + tx;
       line.crd2_l.y_gm  = y2 + ty;
       line.crd2_l.z_gm  = 0.0;
       line.pcsy_l       = DBNULL;
       line.wdt_l        = 0.0;
       errstat = DBinsert_line(&line,&ent_id,&ptr);
       if ( errstat < 0 ) goto error3;
       }
   ++ent_id.seq_val;
     return(0);
     }
/*
***Skip all other group codes.
*/
   goto loop;
/*
***Unexpected end of file.
*/
error1:
   if ( lf != NULL ) fprintf(lf,"Unexpected end_of_file while reading LINE\n");
   return(-1);
/*
***Parse error.
*/
error2:
   if ( lf != NULL ) fprintf(lf,"Parse error while reading LINE\n");
   return(-1);
/*
***DB error.
*/
error3:
    if ( lf != NULL )
       fprintf(lf,"Error from DBinsert_line(), errstat=%d\n",(int)errstat);
    return(-1);
  }

/********************************************************/
