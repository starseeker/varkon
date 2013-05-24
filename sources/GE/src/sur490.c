/********************************************************************/
/*                                                                  */
/*  This file is part of the VARKON Geometry Library.               */
/*  URL:  http://varkon.sourceforge.net                             */
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
/*  (C)Microform AB 1984-1999, Gunnar Liden, gunnar@microform.se    */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"
#include <string.h>

#include <time.h>

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_sur_warning                     File: sur490.c */
/*  =============================================================== */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function handles surface quality warnings and errors        */
/*                                                                  */
/*  !!!!!  To be added by Johan !!!!!!!!!!!!!!!!!!                  */
/*        Get error message from SU.ERM                             */
/*  No function is available according to Johan 1998-02-10 !!!!!!!  */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1998-01-10   Originally written                                 */
/*  1998-02-07   One SURF_ERROR.DAT file, add in file if existing   */
/*               time.h , sur230, ..  added                         */
/*  1998-02-12   Max number of errors in array SWMAX exceeded       */
/*  1998-02-22   Bug: whole array was always written                */
/*  1999-11-28   Free source code modif's and n_w_max for Debug     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_warning    Surface quality warnings/errors  */
/*                                                              */
/*------------------------------------------------------------- */


/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_sur_nopatch     * Retrieve number of patches              */
/* varkon_erinit          * Initialize error message stack          */
/* varkon_errmes          * Warning message to terminal             */
/* varkon_erpush          * Error   message to terminal             */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!-------------- Function calls (internal) ------------------------*/
/*                                                                  */
static short suerror();       /* Get SU error message               */
/*                                                                  */
/*-----------------------------------------------------------------!*/

extern char  jobdir[];          /* Current job directory            */
extern char  jobnam[];          /* Current job name                 */

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2943 = Called function .. failed in varkon_sur_warni  (sur490) */
/* SU2993 = Severe program error (  ) in varkon_sur_warni  (sur490) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_warning (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
/*                                                                  */
  DBSurf    *p_sur,      /* Pointer to the output surface           */
  DBPatch   *p_pat,      /* Pointer to alloc. area for patch data   */
  DBint      n_w,        /* Number of warnings in table SURWARN     */
                         /* Note that maximum in SURWARN is SWMAX   */
                         /* Total errors is n_w. See n_w_max below. */
  SURWARN    p_s_w[] )   /* Warnings/errors in surface       (ptr)  */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   char   f_name[133];   /* File name                               */
   FILE  *f_dat;         /* Output file                       (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint   i_w;          /* Loop index warning                      */
   DBint   n_w_max;      /* Loop index end (max SWMAX)              */

   long    reltim;       /* Relative time January 1, 1970           */
   struct  tm *p_time;   /* Local time                        (ptr) */
   char    c_hour[3];    /* Hour   as string                        */
   char    c_min [3];    /* Minute as string                        */
   char    c_sec [3];    /* Second as string                        */
   char    c_year[5];    /* Year   as string                        */
   char    c_mon [3];    /* Month  as string                        */
   char    c_day [3];    /* Day    as string                        */

   DBint   nu;           /* Number of patches in U direction        */
   DBint   nv;           /* Number of patches in V direction        */
   DBint   surtype;      /* Type CUB_SUR, RAT_SUR, or .....         */
   char    c_styp[80];   /* Surface type                            */

   char   errbuf[80];    /* String for error message fctn erpush    */
   short  status;        /* Error code from called function         */
   char   error_str[80]; /* String for output to file               */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Return if surface quality is OK                               */
/* __________________________________                               */
/*                                                                 !*/

   if  ( n_w == 0 ) return (SUCCED);

/*!                                                                 */
/* 2. Printout of data to Debug File for Debug On                   */
/* _______________________________________________                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur490 Enter*varkon_sur_warning* p_sur %d p_pat %d n_w %d\n", 
          (int)p_sur, (int)p_pat, (int)n_w );
fflush(dbgfil(SURPAC)); 
}
if ( dbglev(SURPAC) == 1 )
{
if   ( n_w <= SWMAX ) n_w_max = n_w;
else n_w_max = SWMAX;
for ( i_w = 0; i_w < n_w_max; ++i_w )
  {
   fprintf(dbgfil(SURPAC),"sur490 No. %d Error %d  iu %d  iv %d\n", 
      (int)(i_w+1),(int)p_s_w[i_w].warning_n1,
      (int)p_s_w[i_w].i_u, (int)p_s_w[i_w].i_v);
   fprintf(dbgfil(SURPAC),"sur490 %s\n", p_s_w[i_w].warning_c1);
  fflush(dbgfil(SURPAC)); 
  }
}
#endif

/*!                                                                 */
/* 3. Write errors to a text file                                   */
/* ______________________________                                   */
/*                                                                 !*/

/*!                                                                 */
/* Retrieve number of patches and surface type.                     */
/* Call of varkon_sur_nopatch (sur230).                             */
/*                                                                 !*/

    status = varkon_sur_nopatch
    (p_sur, &nu, &nv, &surtype);
#ifdef DEBUG
    if(status<0)
    {
    sprintf(errbuf,"sur230%%sur490");
    return(varkon_erpush("SU2943",errbuf));
    }
#endif

   sprintf(c_styp  ,"Undefined_surface_type");

   if      ( surtype ==  CUB_SUR ) sprintf(c_styp," CUB_SUR");
   else if ( surtype ==  RAT_SUR ) sprintf(c_styp," RAT_SUR");
   else if ( surtype ==  MIX_SUR ) sprintf(c_styp," MIX_SUR");
   else if ( surtype ==  CON_SUR ) sprintf(c_styp," CON_SUR");
   else if ( surtype ==  POL_SUR ) sprintf(c_styp," POL_SUR");
   else if ( surtype ==   P3_SUR ) sprintf(c_styp,"  P3_SUR");
   else if ( surtype ==   P5_SUR ) sprintf(c_styp,"  P5_SUR");
   else if ( surtype ==   P7_SUR ) sprintf(c_styp,"  P7_SUR");
   else if ( surtype ==   P9_SUR ) sprintf(c_styp,"  P9_SUR");
   else if ( surtype ==  P21_SUR ) sprintf(c_styp," P21_SUR");
   else if ( surtype == NURB_SUR ) sprintf(c_styp,"NURB_SUR");
   else if ( surtype ==  LFT_SUR ) sprintf(c_styp," LFT_SUR");
   else      sprintf(c_styp,"Surface code= %d",(int)surtype );


/*!                                                                 */
/* Construct the full file name.                                    */
/*                                                                 !*/

        strcpy(f_name,jobdir);
        strcat(f_name,jobnam);
        strcat(f_name,"_");
        strcat(f_name,"Surf_Error");
        strcat(f_name,".DAT");

/*!                                                                 */
/* Open file for output of warnings and errors. Function fopen.     */
/* Write header if file not exists.                                 */
/*                                                                 !*/

/* Append warnings/errors if file exists                            */

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur490 Find out if file %s exists. \n", f_name);
fprintf(dbgfil(SURPAC),
"sur490 Open file in mode r\n");
fflush(dbgfil(SURPAC)); 
}
#endif

if ( (f_dat= fopen(f_name ,"r")) != NULL )
 {                     
/*!                                                                 */
/*    File exists. Open file in append mode.                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur490 File name %s exists. Open in append mode\n", f_name);
fflush(dbgfil(SURPAC)); 
}
#endif
    fclose(f_dat); 

    if ( (f_dat= fopen(f_name ,"a")) == NULL )
     {                     
     sprintf(errbuf, "(open a)%%sur490"); 
     return(varkon_erpush("SU2993",errbuf));
     } /* Open with a */
 }   /* File exists */


else
{
    /* File do not exist */
/*!                                                                 */
/*    File do not exist. Open file in write mode.                   */
/*    Print header for file                                         */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur490 File name %s do not exist. Open in write mode\n", f_name);
fflush(dbgfil(SURPAC)); 
}
#endif

    if ( (f_dat= fopen(f_name ,"w")) == NULL )
     {                     
     sprintf(errbuf,      
     "(open w)%%sur490"); 
     return(varkon_erpush("SU2993",errbuf));
     } /* Open with w failed */

     fprintf(f_dat, "File: %s \n", f_name );  
     fprintf(f_dat, " \n");  
     fprintf(f_dat, " \n");  
     fprintf(f_dat, "Surface Quality Warnings and Errors\n");  
     fprintf(f_dat, "===================================\n");  
     fprintf(f_dat, " \n");  
     fprintf(f_dat, "Job name:      %s \n", jobnam );  
     fprintf(f_dat, "Job directory: %s \n", jobdir );  
     fprintf(f_dat, " \n");  
     fprintf(f_dat, " \n");  

 }  /* File do not exist */

/*!                                                                 */
/* Print date.                                                      */
/*                                                                 !*/

/* Initialize local date variables                                  */
   sprintf(c_hour  ,"UU");
   sprintf(c_min   ,"UU");
   sprintf(c_sec   ,"UU");
   sprintf(c_year  ,"UUUU");
   sprintf(c_mon   ,"UU");
   sprintf(c_day   ,"UU");

/* Get date and time                                                */

   reltim = time((long *)0);
   p_time = localtime(&reltim);

  if  (  p_time->tm_hour <  10 )
      sprintf(c_hour,"0%d",(int)p_time->tm_hour);
  else
      sprintf(c_hour,"%d",(int)p_time->tm_hour);

  if  (  p_time->tm_min  <  10 )
      sprintf(c_min ,"0%d",(int)p_time->tm_min );
  else
      sprintf(c_min ,"%d",(int)p_time->tm_min );

  if  (  p_time->tm_sec  <  10 )
      sprintf(c_sec ,"0%d",(int)p_time->tm_sec );
  else
      sprintf(c_sec ,"%d",(int)p_time->tm_sec );

  if  (  p_time->tm_year > 0 && p_time->tm_year <=  99 )
      sprintf(c_year,"19%d",(int)p_time->tm_year);
  else
      sprintf(c_year,"200%d",(int)p_time->tm_year);

  if  (  p_time->tm_mon  <  10 )
      sprintf(c_mon ,"0%d",(int)p_time->tm_mon );
  else
      sprintf(c_mon ,"%d",(int)p_time->tm_mon );

  if  (  p_time->tm_mday <  10 )
      sprintf(c_day ,"0%d",(int)p_time->tm_mday);
  else
      sprintf(c_day ,"%d",(int)p_time->tm_mday);

   fprintf(f_dat, " \n");  
   fprintf(f_dat, " \n");  
   fprintf(f_dat,"Date: %s/%s/%s  Time: %s:%s:%s \n",
                  c_day, c_mon, c_year, 
                  c_hour, c_min, c_sec);

   fprintf(f_dat, "________________________________ \n");  
   fprintf(f_dat, " \n");  

/*!                                                                 */
/* Surface type and number of patches                               */
/*                                                                 !*/

   fprintf(f_dat,"Surface type: %s  \n", c_styp );
   fprintf(f_dat,"Number of patches nu= %d nv= %d\n",
                  (int)nu, (int)nv );
   fprintf(f_dat, " \n");  

/*!                                                                 */
/* Maximum number of errors/warnings                                */
/*                                                                 !*/

/* 1998-02-22   if   ( n_w <= SWMAX ) n_w_max = SWMAX; */
   if   ( n_w <= SWMAX ) n_w_max = n_w;
   else
     {
     n_w_max = SWMAX;
     fprintf(f_dat, "Maximum Number of Errors/Warnings Exceeded !\n");  
     fprintf(f_dat, "--------------------------------------------\n");  
     fprintf(f_dat,"The total number of errors/warnings is %d\n", 
                              (int)n_w);
     fprintf(f_dat,"Only the first %d is listed in this file \n", 
                              SWMAX );
     fprintf(f_dat, " \n");  
     fprintf(f_dat, " \n");  
     }

/*!                                                                 */
/* Write all errors to the file                                     */
/*                                                                 !*/

for ( i_w = 0; i_w < n_w_max; ++i_w )
  {
   fprintf(f_dat,"Patch iu %d iv %d: \n", 
      (int)p_s_w[i_w].i_u, (int)p_s_w[i_w].i_v);

/*   Get SU error text. Call of internal function suerror           */
     status= suerror(p_s_w[i_w].warning_n1,
              &p_s_w[i_w].warning_c1, error_str   );
     if  ( status < 0 ) 
       {
       sprintf(errbuf, "suerror%%sur490");
       return(varkon_erpush("SU2973",errbuf));
       }

   fprintf(f_dat,"%s\n", error_str);

/*   fprintf(f_dat,"%s\n", p_s_w[i_w].warning_c1); */
  }

/*!                                                                 */
/* Close the output file (C function fclose)                        */
/*                                                                 !*/

 fclose(f_dat); 


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur490 File name %s\n", f_name);
fflush(dbgfil(SURPAC)); 
}
#endif

/*!                                                                 */
/* 4. Warning message for the poor surface quality                  */
/* _______________________________________________                  */
/*                                                                 !*/


   varkon_erinit();

   sprintf(errbuf,"%s%%",f_name);
   varkon_erpush("SU4921",errbuf);

   sprintf(errbuf,"%d %%",(int)n_w);
   varkon_erpush("SU4911",errbuf);

   sprintf(errbuf,"(sur490)%%");
   varkon_erpush("SU4901",errbuf);

     varkon_errmes();

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur490 Exit* Number of warnings %d\n" , (int)n_w);
  fflush(dbgfil(SURPAC)); 
  }
#endif


    return(SUCCED);

  }

/*********************************************************/


/*!********* Internal ** function ** suerror ************************/
/*                                                                  */
/* Purpose                                                          */
/* --------                                                         */
/*                                                                  */
/*    Get error message from SU.ERM                                 */

      static short suerror ( error_n, error_c, error_str )
/*                                                                  */
/******************************************************************!*/


/*!------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */

   DBint  error_n;       /* Error number in SU.ERM                  */
   char   error_c[];     /* String (input to erpush)                */
   char   error_str[];   /* String for output to file               */

/* Out:                                                             */
/*-----------------------------------------------------------------!*/

{ /* Start of function */


/* --------------- Internal variables ------------------------------*/
/*----------------------------------------------------------------- */


/*--------------end-of-declarations---------------------------------*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 2 )
{
fprintf(dbgfil(SURPAC),
"sur490*suerror Enter error_n %d error_c %s\n",
(int)error_n,error_c);
fflush(dbgfil(SURPAC)); /* To file from buffer      */
}
#endif

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

    if         ( error_n == 2221 )
      strcpy(error_str,"INFO Patch is a point");
    else if    ( error_n == 2231 )
      strcpy(error_str,"INFO Patch is a null tile");
    else if    ( error_n == 2241 )
      strcpy(error_str,"INFO Patch is triangular");
    else if    ( error_n == 2251 )
      strcpy(error_str,"WARNING Normal(s) in triangular patch is not OK");
    else if    ( error_n == 2261 )
      strcpy(error_str,"WARNING Normal(s) in null tile is not OK");
    else if    ( error_n == 2271 )
      strcpy(error_str,"WARNING Normal(s) in patch is not OK");
    else if    ( error_n == 2421 )
      strcpy(error_str,"WARNING Surface normal variation abnormal");
    else if    ( error_n == 2433 )
      strcpy(error_str,"Surface normal variation abnormal");
    else 
      sprintf(error_str,"No description of error %d (sur490)",
                       (int)error_n);

    return(SUCCED);

} /* End of function                                                */
/********************************************************************/

