/**********************************************************************
*
*    anlog.c
*    =======
*
*    This file is part of the VARKON Analyzer Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*    anlogi();       Initialize diagnostic log file handler.
*    analog();       Accept & store diagnostic messages
*    anlogx();       No more logging close files.
*    anlist();       Open source an list files.
*    anlmsg();       Return next diagnostic message.
*    anyerr();       Report if any errors so far.
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

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/AN.h"
#include <string.h>

#ifdef UNIX
#include <unistd.h>
#endif

#ifdef ANALYZER

extern V3MDAT sydata;

#define  display_w 80
#define  almax     100

static short alpgl;                /* Page length                */
static short alpgw;                /* Page width                 */
static bool  allist;	           /* Print list file when true  */
extern char *mktemp();             /* Create temporary file name */

static bool eof_src = FALSE;
FILE *alstf, *fopen();	           /* List file output	*/
static FILE *alsrcf;	           /* Source file input	*/
static short alpgno;	           /* Current page number  */
static short allin;	           /* Current line number  */

static FILE *allogf;               /* Temporary log file */ 
static bool  alopen = FALSE;       /* Is log file open ? */
static char  alfile[256];          /* Name of log file, 30->256 931116 JK  */

static FILE *almsgf;               /* File of diag messages  */
static bool  msopen = FALSE;       /* Is messagefile open ? */
/*
***contents: allmbf - structure for message handling.
*/
typedef struct
	{
	short ptr;
	char errtext[255];
 	} allmbf;
/*
*** Buffer for binary i/o
*/
static union
        {                
        struct
          {
          short line;
          short col;
          char  errcode[7];
          char  insert[254];
          } 
        msg;
        }
        albuf;
/*
***Internal sort structure for messages. 
*/
static struct
        {
        short line;                /* line number */
        short col;                 /* column number */
        short cnt;                 /* record number in file */
        }
        almsg[almax];

static short  almcnt = 0;          /* Number of messages so far */
static short  alecnt = 0;          /* Number of errors (errors <= messages)
                                      so far */

static char *anindex(char s[], char t);
static void  alimsg(char mess[], char ins[], char sp[255]);
static short algetl(char line[], char ecode[]);
static short alcomp(short i1, short i2);
static bool  alsrtp(short l1, short c1, short l2, short c2);
static void  alejct();
static void  trim(char s[]);
static void  alout(char lead[], char s[], bool newline, bool termflg);
static void  create_lead(char *lead, short ptr);
static void  crout(char lead[], char errtext[], short s, short e, bool newline);
static void  save_msg(short tno, short ino, char *errtext, allmbf m_bufs[]);
static short rd_pr(short *srclin, short *msglin);
static void  pr_form_msg(allmbf *m);
static void  prem_msg(short msgno);
static void  prem_source(short *src_lin_num);
static void  pr_epilogue();
static void  pr_source_line(char line[], short lineno, bool termflg);
static void  read_source_line(char line[]);
static void  act();

/*!******************************************************/

        static char *anindex(
        char s[],
        char t)

/*      Message collection phase of AL
 *      Return index of t in s, -1 if none 
 *
 *      In:  
 *
 *      Out: 
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 * 
 *       1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
  int i;

   for ( i=0; s[i] != '\0'; i++ )
     {
     if( s[i] == t )
     return( s+i );
     }
   return(0);
  }

/********************************************************/
/*!******************************************************/

       static  void alimsg(
       char  mess[],
       char  ins[],
  	 char sp[255])

/*      Insert insert text into message.
 *      Return  generated message.        
 *
 *      In:  
 *
 *      Out: 
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 * 
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   int  i;
   string ind;
   string p[5];
   int  found;
/*
*** Divide insert string into subparts
*** strings are separated with cntrl-d.
*/
   for ( i = 0; i < 4; i++ ) p[i] = "";
 
   i = 0;
   p[0] = ins;

   found = 0;         
   while ( ( found == 0 ) && ( i < 4 ) )
     {
     i++;
     if ( ( ind = ( string )anindex( p [i-1],'\004' ) ) == 0 )
       {
       found = 1;
       }
     else
       {
       p[i] = ind+1;
       *ind = '\0';
       }
     }
    sprintf(sp,mess,p[0],p[1],p[2],p[3],p[4]);
  }

/********************************************************/
/*!******************************************************/

        static short algetl(
        char line[],
        char ecode[])

/*      Find line in message file with
 *      right error code
 *
 *      In:  
 *
 *      Out: 
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 * 
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/
  
  { 
   int i;
   int found;
/*
***Clear previous message number.
*/
   strcpy( line,"       " );
   found = 0;
   while( found == 0 )
     {
     if( fgets( line,100,almsgf ) == NULL ) return(-1);
     i = 0;

     while ( i <= 2 )
       {
       if( line[i+4] == ecode[i+2] )
         found = 1;
       else
         { 
         found = 0; break;
         }

       i++;
       }
     }
   return(0);
  }

/********************************************************/
/*!******************************************************/

        static short alcomp(
        short i1,
        short i2)

/*      Compare values for sort.        
 *
 *      In:  
 *
 *      Out: 
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 * 
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   if( i1 == i2 )     return(3);
   else if( i1 == 0 ) return(2);
   else if( i2 == 0 ) return(1);
   else if( i1 < i2 ) return(1);

   else return(2);
  }

/********************************************************/
/*!******************************************************/

        static bool  alsrtp(
        short l1,
        short c1,
        short l2,
        short c2)

/*      Sort predicate for line and column numbers.
 *      0,0 is greater than all numbers
 *      x,0 is greater than all columns       
 *
 *      In: l1  => line number
 *          c1  => kolumn position
 *          l2  => line number
 *          c2  => kolumn position
 *
 *      Out: 
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 * 
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   switch ( alcomp(l1,l2) ) 
     {
     case 1 : return(TRUE);

     case 2 : return(FALSE);

     case 3 : 
       switch( alcomp(c1,c2) )
         {
         case 1 : return(TRUE);
         case 2 : return(FALSE);
         case 3 : return(TRUE);
         }
     }
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short anlogi( 
        char  *logfnm,
        int   lipag,
        int   licol,
        bool  list)

/*      Initialize diagnostic log file handler.
 *
 *      In:   logfnam    => Pointer to string containing name of file.                      
 *            lipag,     => Lines per page.
 *            licol,	 => Columns per line.
 *            list)      => TRUE print list.
 *
 *      Out:  Nothing
 *
 *      Ret:  Severity code (negated).
 *              0 = file opened            
 *             -2 = Can not open file.
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 * 
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  { 
   int  i;
   char *alp;

   allist = list;                       /* Supress or print source listing */

   almcnt = 0;
   alecnt = 0;

   alpgl  = ( short )lipag;
   if( lipag < 8 ) alpgl = 24;          /* Saftey measure */

   alpgw = (short)licol;

   if( licol < 24 ) alpgw = 50;         /* Only source may be shortened */

   i = 0;
/*
*** Copy name of file to local variable
*/
   while( ( alfile[i] = logfnm[i] ) != '\0' ) 
     i++;
      
   alfile[i-3] = '\0';
/*
***XXX->XXXXXX for all platforms, 1999-01-24, J.Kjellander.
*/
   strcat( alfile,"XXXXXX" );
   alp = mktemp(alfile);
       
#ifdef WIN32
   if ( (allogf=fopen(alfile,"w+b")) == NULL )

#else
   if ( (allogf=fopen(alfile,"w+")) == NULL )

#endif
     {
     printf("mbsc: Can't open logfile %s.\n",alfile);
     return(-2);
     }
   else alopen = TRUE;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short analog(
        short line,
        short col,
        char  *errcode,
        char  *insert)

/*      Accept & store diagnostic messages
 *
 *      In:   line       => line number in source
 *            col        => column number in source
 *            errcode    => Pointer to string cont.  
 *                           error code (6 chars).
 *            insert     => pointer to string 
 *                           inserts separated by Öd.
 *
 *      Out:  Nothing
 *
 *      Ret:  fault val  =>  0     ok
 *                          -2     error
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 * 
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/
  
  {
   int i;
     
   albuf.msg.line = line;
   albuf.msg.col  = col;

   for ( i = 0; i <= 5; ++i )
   albuf.msg.errcode[i] = errcode[i];
   albuf.msg.errcode[6] = '\0';

   i = 0;
   while ( (albuf.msg.insert[i] = insert[i]) != '\0') i++;
/*
*** Uppdate alecnt if error (not warning).
*/
   if( errcode[5] >= '2' ) alecnt++;

   if( almcnt >= almax ) return(-1);      /* Too many errors */

   if( almcnt >= (almax-1) )
     {
     line = 0;  col = 0;
     albuf.msg.line = 0;
     albuf.msg.col  = 0;
     strcpy(albuf.msg.errcode,"AN9982");
     }
/*
***Write buffer to file
*/
   if ( fwrite(&albuf,sizeof(albuf),1,allogf) == 0 ) 
     {
     printf("mbsc: Can't write to logfile !\n");
     return(-2);
     }
/*
***Copy source position to internal sort array
*/
   almsg[almcnt].line = line;
   almsg[almcnt].col  = col;
   almsg[almcnt].cnt  = almcnt;
   almcnt++;
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short anlogx()
          
/*      No more logging close files.
 *
 *      In:   
 *
 *      Out:  
 *
 *      Ret:  fault val  => 0     ok
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 * 
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short templ, tempc, tempcn;
   short i,j, flag;
   short n;                        /* Number of elements in array */

   if( almax < almcnt )
     n = almax-1;
   else 
     n = almcnt-1;
/*
*** Sort messsage array in source position order.
*** Bubble sort is used.
*/
    fclose(allogf);
   alopen = FALSE;

   if ( n == 0 ) return(0);        /* only one element */

   for ( i = 0; i <= n; i++ )
     {
     templ = almsg[almcnt-1].line;
     tempc = almsg[almcnt-1].col;
     tempcn = almsg[almcnt-1].cnt;
     flag = 1; 

     for (j = n; j > i; j--)
       {
       if ( alsrtp(almsg[j-1].line,almsg[j-1].col,templ,tempc) ) 
         {
/* 
*** EXPAND to a real swap 
*/
         almsg[j].line = templ;
         almsg[j].col  = tempc;
         almsg[j].cnt  = tempcn;
                                                               
         templ  = almsg[j-1].line;
         tempc  = almsg[j-1].col;
         tempcn = almsg[j-1].cnt;
         }
       else
         {
         flag = 0;
         almsg[j].line = almsg[j-1].line;
         almsg[j].col  = almsg[j-1].col;
         almsg[j].cnt  = almsg[j-1].cnt;
         }
       }
       almsg[i].line = templ;
       almsg[i].col  = tempc;
       almsg[i].cnt  = tempcn;
       if (flag==1) break;
     }                             /* end i loop */
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short anlmsg(
        short msgnum,
        short *line_num,
        short *col_num,
        char  *errtext)

/*      Return next diagnostic message.
 *
 *      In:   msgnum   => message number in 
 *                           source order.
 *                           1 is first message.
 *
 *      Out:  line     => line number in source
 *            col      => column number in source
 *            errtext  => Pointer to string cont.  
 *                        error message.
 *
 *      Ret:  fault val  =>  0     ok
 *                          -2     error
 *                           2     no messages left
 *
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 *  
 *      1995-02-16 VARKON_ERM, J. Kjellander
 *      1997-01-15 v3genv(), J.Kjellander
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {  
   long lrec;                      /* byte number in file     */
   char line[128];                 /* Message file buffer     */
   char errfil[V3PTHLEN];          /* Name & path to ERM-file */

   if( msgnum > almcnt ) return(2);
   if( msgnum > almax ) return(2);
   if( almsg==0 ) return(2);       /* No messages were generated */
        
   lrec = (long)(sizeof(albuf))*(long)almsg[msgnum-1].cnt;  
/*
*** Windows95 har lite special med sina textfiler. "r+b" för
*** binary mode.
*/
   if ( alopen == FALSE )
     {

#ifdef WIN32
     if ( (allogf=fopen(alfile,"r+b")) == NULL )

#else
     if ( (allogf=fopen(alfile,"r+")) == NULL )

#endif
       {
       printf("Can not reopen log file %s.\n",alfile);
       return(-2);
       }
     else alopen = TRUE;
     }
/*
***Numera gör man seek relativt start=SEEK_SET inte 0.
*/
   if ( fseek(allogf,lrec,SEEK_SET) != 0 )
     {
     printf("Can not perform lseek on log file");
     return(-1);
     }

   if ( fread(&albuf,sizeof(albuf),1,allogf) == 0 )
     {
     printf("Can not read log file");
     return(-1);
     }

   *line_num = almsg[msgnum-1].line;
   *col_num  = almsg[msgnum-1].col;
       
   if ( msopen == FALSE )
     {
     sprintf(errfil,"%sAN.ERM",v3genv(VARKON_ERM));

     if ( (almsgf=fopen(errfil,"r")) == NULL )
       {
       printf("Can't open error message file : %s\n",errfil);
       printf("Error message = %s\n",albuf.msg.errcode);
       return(-2); 
       }
     else msopen = TRUE;
     }
   else fseek(almsgf,(long)0,0);

   if ( algetl(line,albuf.msg.errcode) == -1 )
     {
     printf("Message not found %s \n",albuf.msg.errcode);
     return(-2);
     }

   alimsg(line,albuf.msg.insert,errtext);        

   if(errtext[8] >= '2') errtext[8] = 'E';
   else if (errtext[8] > '0') errtext[8] = 'W';

   return(0);              
  }

/********************************************************/
/*!******************************************************/

        bool anyerr() 

/*      Report if any errors so far.
 *
 *
 *      In:   
 *
 *      Out:   
 *
 *      Ret:  TRUE      --> 1 or more errors has occured
 *
 *            FALSE     --> no errors has occured
 *
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   return(alecnt);
  }

/********************************************************/
/*!******************************************************/

        static void alejct()

/*      List phase part.
 *
 *      In:   
 *
 *      Out:   
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

 {
/*
***Eject page in output listing 
*/
  if( alpgno > 0 ) fprintf(alstf,"\f");
  else fprintf(alstf,"\n");

  fprintf(alstf,"\n \n");
  ++alpgno;

  allin = 8;
 
  fprintf(alstf," MBS Version %d.%d  -  %d                  Page %4d\n\n",
          sydata.vernr,sydata.revnr,sydata.sernr,alpgno);
 }

/********************************************************/
/*!******************************************************/

        static void trim(char s[])

/*      
 *      In:   
 *
 *      Out:   
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

 {
  int i;
 
  i = strlen(s);

   while( (--i >= 0 ) && ( s[i] == ' ') )
     s[i] = 0;
 }

/********************************************************/
/*!******************************************************/

        static void alout(
        char lead[],
        char s[],
        bool newline,
        bool termflg)

/*      Print one line to list file and when termflg == TRUE to terminal.
 *
 *      In:   
 *
 *      Out:   
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char c;                         /* Temporary char in very long lines  */
   int   lead_len;                 /* Lenght of lead text                */

   trim(s);

   if( allist)
   if( ++allin > alpgl ) alejct();
 
   lead_len = strlen(lead);

   if( allist)
     {
      if( (short)(lead_len + strlen(s) ) <= alpgw ) 
        fprintf(alstf,"%s %s",lead,s);
/*
*** Line to long to fit on one line in output - split 
*/
      else
        { 
        c = s[alpgw - lead_len - 1];
        s[alpgw-lead_len-1] = '\0';
        fprintf(alstf,"%s %s\n",lead,s);
        s[alpgw-lead_len-1] = c;
        if( ++allin > alpgl ) alejct();

        fprintf(alstf,"%s",&s[alpgw-lead_len-1]);
        }

      if( newline == TRUE ) fprintf(alstf,"\n");
     }

   if (termflg == TRUE)
     {
     printf("%s %s",lead,s);
     if (newline == TRUE) printf("\n");
     }
  } 

/********************************************************/
/*!******************************************************/

        static void create_lead(
        char *lead,
        short ptr)

/*      Create lead text information for message on line.
 *
 *      In:   
 *
 *      Out:   
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   lead[0] = ' ';
   lead[1] = '*'; lead[2] = ptr + '0';
   lead[3] = '*'; lead[4] = '\0';
  }

/********************************************************/
/*!******************************************************/

        static void crout(
        char lead[],
        char errtext[],
        short s,
        short e,
        bool newline)

/*      Print line for error message.
 *
 *      In:   
 *
 *      Out:   
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char line[128];
   short l;
   short i;

   l = -1;

   for(i=s; i<= e; i++)
     line[++l] = errtext[i];

   line[++l] = '\0';

   alout(lead,line,newline,TRUE);
  }

/********************************************************/
/*!******************************************************/

        static void save_msg(
        short tno,
        short ino,
        char *errtext,
        allmbf m_bufs[])

/*      Save messages for current line in a temporarybuffer.
 *
 *      In:   
 *
 *      Out:   
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   m_bufs[ino].ptr = tno;
   strcpy(m_bufs[ino].errtext,errtext);
  } 

/********************************************************/
/*!******************************************************/

        static short rd_pr(
        short *srclin,
        short *msglin)

/*      Read and print source lines up to next diagnostic message.
 *
 *      In:   
 *
 *      Out:   
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char line[128];

   read_source_line(line);
   ++(*srclin);

   while( ( (*srclin < *msglin ) || (*msglin == 0) ) && (eof_src == FALSE))
     {
      pr_source_line(line,*srclin,FALSE);
      read_source_line(line);
      ++(*srclin);
     };

   if (eof_src == TRUE)
     return(1);
   else
     {
     if (*srclin == *msglin) 
       { 
       pr_source_line(line,*srclin,TRUE);
       }
     else pr_source_line(line,*srclin,FALSE);
     return(0);
     }
  }

/********************************************************/
/*!******************************************************/

        static void pr_form_msg(
        allmbf *m)

/*      Print formatted version of messsage.
 *
 *      In:   
 *
 *      Out:   
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   int   fold_pos;         	     /* Max fold position    */
   int   f_pos;         	     /* Actual fold position */
   int   start_pos,len;
   char lead[20];
   int   pw;		           /* Page width used      */

   if( alpgw > 78 ) pw = 78;
   else pw = alpgw;

   fold_pos = pw - 8;

   f_pos = fold_pos;
   start_pos = 0;

   len = strlen(m->errtext);
 
   if( m->ptr != 0 ) 
     create_lead(lead,m->ptr);
   else strcpy(lead,"    ");

   while(len > 0)
     {
     if( len <= fold_pos )
       {
       crout(lead,m->errtext,(short)start_pos,(short)(len+start_pos),FALSE);
       len = 0;
       }
     else
       {
       f_pos = fold_pos + start_pos;

       while( (m->errtext[f_pos] != ' ' ) && (f_pos >= (start_pos + 30)) )
	   f_pos--;

         crout(lead,m->errtext,(short)start_pos,(short)f_pos,TRUE);
         len = len - (f_pos-start_pos);
         start_pos = f_pos + 1;

         strcpy(lead,"                ");
         fold_pos = pw - 17;
       }
     }
  }

/********************************************************/
/*!******************************************************/

        static void prem_msg(
        short msgno)

/*      Print REMaining messages.
 *
 *      In:   
 *
 *      Out:   
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   allmbf m;                       /* Current message + prefix number */
   short msg_line,msg_col,rm;

   rm = anlmsg(msgno,&msg_line,&msg_col,m.errtext);

   while(rm == 0)
     {
     m.ptr = 0;
     alout(" "," ",TRUE,TRUE);
     pr_form_msg(&m);
     
     rm = anlmsg(++msgno,&msg_line,&msg_col,m.errtext);
     }
  }

/********************************************************/
/*!******************************************************/

        static void prem_source(
        short *src_lin_num)

/*      Print REMaining source.
 *
 *      In:   
 *
 *      Out:   
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char line[128];

   read_source_line(line);

   while (eof_src == FALSE)
     {
     pr_source_line(line,++(*src_lin_num),FALSE);
     read_source_line(line);
     }
  } 

/********************************************************/
/*!******************************************************/

        static void pr_epilogue()

/*      Print epilogue page.
 *
 *      In:   
 *
 *      Out:   
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char lead[10];
   short no_err;
   char count[10];

   strcpy(lead,"      ");

   if(allist) alejct();

   alout(" "," ",TRUE,TRUE);

   alout(lead,"Compilation ended",TRUE,TRUE);

   alout(lead,lead,TRUE,FALSE);
   alout(lead,lead,TRUE,TRUE);

   no_err = anyerr();
   if( no_err > 0 )
     {
     alout(" "," ",TRUE,TRUE);
     alout(lead,"Number of compiler detected errors =",FALSE,TRUE);
     sprintf(count," %d",no_err);
    
     alout(count," ",TRUE,TRUE);
     }
   else alout(lead,"No compiler detected errors",TRUE,TRUE);
  }

/********************************************************/
/*!******************************************************/

        static void pr_source_line(
        char  line[],
        short lineno,
        bool  termflg)

/*      
 *
 *      In:   
 *
 *      Out:   
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char srcl[10];

   sprintf(srcl,"  %4d. ",lineno);
   alout(srcl,line,FALSE,termflg);
  } 

/********************************************************/
/*!******************************************************/

        static void read_source_line(
        char line[])

/*      
 *
 *      In:   
 *
 *      Out:   
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  { 
   char tmp[512];

   if( NULL == fgets(tmp,512,alsrcf) )
     eof_src = TRUE;
   else
    {
    eof_src = FALSE;
    tmp[126] = 12;                 /* lf */
    tmp[127] = '\0';
    strcpy(line,tmp);
    }
  }

/********************************************************/
/*!******************************************************/

        static void act()

/*      Print all source and messages associated to the source.
 *
 *      In:   
 *
 *      Out:   
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short  msgno = 1;         /* pointer into message array    */
   short  i;                 /* loop var		      */
   short  src_lin_num;	     /* Current source line number    */
   short  msg_lin_num;	     /* Current message line number   */
   short  msg_col_num;	     /* Current message column        */
   short  rm; 		     /* Return code from anlmsg	      */
   short  num_msg;	     /* Number ptr for message        */
   short  indx;		     /* Num msgs on current line      */
   char   ptr_arr[128];	     /* line to contain msg pointers  */
   allmbf m_bufs[10];	     /* Saved msgs on current line    */
   char   errtext[255];	     /* Created message text 	      */
   char   line[128];	     /* Source line		      */
   short  prev_col;	     /* prev col for ptr handling     */
   short  mn;		     /* Check number for resume point */
/*
*** Function: act reads source lines and print them to source 
*** list file. For each line it checks if there are any messages 
*** associated to it. When such a message is found all messages  
*** are printed line formatted. Under the source line a ptr line 
*** pointing to the error columns is printe
*/
   msgno = 1;
 
   rm = anlmsg(msgno,&msg_lin_num,&msg_col_num,errtext);

   prev_col = 0;

   if(allist) alejct();

   read_source_line(line);
/* 
*** rm != 0 nedan i if-satsen tillagt 19/1/89 JK för att slippa
*** att programmets 1:a rad ibland skrivs ut på skärmen vid komp. 
*/
   if ( (rm != 0)  ||  ((eof_src == FALSE) && (msg_lin_num != 1)) )
     pr_source_line(line,1,FALSE);
   else pr_source_line(line,1,TRUE);

   src_lin_num = 1;

   while((rm == 0) && (eof_src == FALSE) && (msg_lin_num != 0))
     {
     for( i=0; i < 127; i++ ) ptr_arr[i] = ' '; ptr_arr[127] = 0;

     if( (src_lin_num < msg_lin_num ) || (msg_lin_num == 0) )
       {
       if(rd_pr(&src_lin_num,&msg_lin_num) != 0)
         {
/* 
*** eof reached 
*/
         if (rm == 0) prem_msg(msgno);
	   pr_epilogue();
	   return;
         }
       }

     num_msg = 0;
     indx = 0;
     prev_col = 0;

     while((msg_lin_num == src_lin_num) && (rm == 0))
       {
       if(msg_col_num >= 127) msg_col_num = 126;

       sscanf(errtext,"%hd",&mn);

       if( mn == 999 )
         {
         prev_col = msg_col_num - 1;
         if(ptr_arr[msg_col_num-1] == ' ')
	   ptr_arr[msg_col_num-1] = '*';
         }
       else 
         {
         if( msg_col_num != prev_col ) ++num_msg;

         if( indx < 9 ) 
           {
           save_msg(num_msg,++indx,errtext,m_bufs);
	     ptr_arr[msg_col_num] = num_msg + '0';
	     prev_col = msg_col_num;
           }
	   }
         rm = anlmsg(++msgno,&msg_lin_num,&msg_col_num,errtext); 
       } 

     alout("======>",ptr_arr,TRUE,TRUE);

     for( i = 1; i <= indx; i++ )
       pr_form_msg(&m_bufs[i]);

     alout(" "," ",TRUE,FALSE);
     }

   if(eof_src == FALSE) prem_source(&src_lin_num);

   if(rm == 0) prem_msg(msgno);
   pr_epilogue();
  }

/********************************************************/
/*!******************************************************/

        void anlist(
        char filename[])

/*      Open source an list files.
 *
 *      In:  filename[]  =>  Name of source file. 
 *
 *      Out:   
 *
 *      (C)microform ab 1986-01-16 J. Wilander
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char lstfname[40];
   short i;
/* 
*** Open source an list files 		
*/
   if( (alsrcf = fopen(filename,"r") ) == NULL)
     {
     printf("Can't open%s\n",filename);
     exit(1);
     };

   strcpy(lstfname,filename);

   i = 0;

   while( (i < 40) && (lstfname[i] != '\0') && (lstfname[i] != '.') ) ++i;
/*
*** i points to period unless no period found
*** Skall det inte vara == 'Ö0' nedan ??? JK 
*/
   if( lstfname[i] == 0 ) 
     strcat(lstfname,".LST");
   else 
     {
     lstfname[++i] = '\0';
     strcat(lstfname,"LST");
     }

   if(allist)
     {
     if ((alstf = fopen(lstfname,"w")) == NULL)
	 {
       printf("Can't open list file %s\n",lstfname);
	 exit(1);
	 }
     }

   alpgno = 0;  allin = 0;

   act();
  
   if(allist) fclose(alstf);              /* close -> fclose 13/8/87 JK  */

   if ( msopen == TRUE ) 
     {
     fclose(almsgf);                      /* close -> flcose, 13/8/87 JK */
     msopen = FALSE;
     }

   fclose(alsrcf);                        /* close -> flcose, 13/8/87 JK */

   if ( alopen == TRUE )
     {
     fclose(allogf);
     alopen = FALSE;
     }

    if ( unlink(alfile) != 0 )
      printf("Can not delete temporary file %s \n",alfile);
  }

/********************************************************/
/********************************************************
 *
 *     ANALOG interface routines for interactive V3
 *
 ********************************************************/

#else

short  almcnt = 0;        /* Number of messages so far */
short  alecnt = 0;        /* Number of errors (errors <= messages) so far */

/*!******************************************************/
        short anlogi() 

/*      Initialize error log handler
 *
 *      In:  
 *
 *      Out:  Nothing 
 *
 *      (C)microform ab 1985-06-25 Mats Nelson
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   almcnt = 0;
   alecnt = 0;
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short analog(
        short line,
        short col,
        char  *errcode,
        char  *insert)

/*      Accept & store diagnostic messages
 *
 *      In:  line       => line number in source
 *           col        => column number in source
 *           errcode    => Pointer to string cont.  
 *                         error code (6 chars).
 *           insert     => pointer to string 
 *                         inserts separated by Öd.
 *
 *      Out: Nothing 
 *
 *      (C)microform ab 1985-06-25 Mats Nelson
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   almcnt++;
   if(errcode[5] >= '2')
     alecnt++;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        bool anyerr() 

/*      Report if any errors so far.
 *
 *      In:  
 *
 *      Out: 
 *
 *       Ret:  TRUE --> 1 or more errors has occured
 *
 *             FALSE --> no errors has occured
 *
 *      (C)microform ab 1985-06-25 Mats Nelson
 *  
 *      1999-04-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   return(alecnt);
  }

/*******************************************************/

#endif

/*********************************************************************/
