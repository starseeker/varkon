/********************************************************************/
/*                                                                  */
/*  This file is part of the VARKON Geometry Library.               */
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
/*  (C)Microform AB 1984-1999, Gunnar Liden, gunnar@microform.se    */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_sur_readpts                   File: sur296.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function reads point and optionally derivative and          */
/*  curvature data from a file.                                     */
/*                                                                  */
/*  The function allocates memory for the data. Memory that         */
/*  must be deallocated by the calling function!                    */
/*  Memory shall not be deallocated if this function fails. It      */
/*  will be done here.                                              */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1994-06-27   Originally written                                 */
/*  1996-02-22   malloc -> v3mall free -> v3free                    */
/*  1997-02-09   Elimination of compilation warnings                */
/*  1999-12-18   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_readpts    Read points from a file          */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ini_surpoi       * Initialize SURPOI                      */
/**v3mall                  * Allocate memory                        */
/* v3free                  * Free allocated memory                  */
/* varkon_erinit           * Initialize error buffer                */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2643 = There is no file  infile                                */
/* SU2653 = File infile is not OK                                   */
/* SU2693 = Point number iu,iv for SUR_POINTS is outside nu,nv      */
/* SU2703 = Point data code .. for SUR_POINTS is not valid          */
/* SU2943 = Called function .. failed in varkon_sur_readpts         */
/* SU2993 = Severe program error (  ) in varkon_sur_readpts         */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!****************** Function **************************************/

    DBstatus     varkon_sur_readpts (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  char    *infile,       /* File with points and derivatives        */

/* Out:                                                             */
/*                                                                  */
  SURPOI **pp_ptsout,    /* Allocated area for point data     (ptr) */
  gmint   *p_nu,         /* Number of points in U direction   (ptr) */
  gmint   *p_nv )        /* Number of points in V direction   (ptr) */
                         /* No of records in SURPOI table is nu*nv  */
/*                                                                  */
/*      Data to pp_ptsout in allocated memory area                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  int     nu;            /* Number of points in U direction         */
  int     nv;            /* Number of points in V direction         */
  gmint   maxnum;        /* Maximum number of points (=nu*nv)       */
  gmint   i_p;           /* Loop index point (record) in input file */
  gmint   i_r;           /* Loop index row   (field) for point      */
  SURPOI *p_frst;        /* First   record in table SURPOI    (ptr) */
  SURPOI *p_p;           /* Current record in table SURPOI    (ptr) */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  gmint   iu,iv;         /* Loop index point address                */
  gmint   npts;          /* Number of points in input file          */
  FILE   *f;             /* Input file                        (ptr) */
  char    row_code[10];  /* Code for row in file                    */
  char    rad[133];      /* One line in the input file              */

  int    iu_f,iv_f;      /* Patch address from file                 */
  double k1,k2,k3;       /* Dummy reals                             */

  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur296 Enter***varkon_sur_readpts   infile: %s\n" , infile );
}
#endif
 
/*!                                                                 */
/* Open the input  file (C function fopen)                          */
/*                                                                 !*/

if ( (f= fopen( infile,"r")) == NULL )
  { 
  sprintf(errbuf," %s %%varkon_sur_readpts" ,infile); 
  varkon_erinit();
  return(varkon_erpush("SU2643",errbuf));
  }

/*!                                                                 */
/* 3. Allocate memory area for output points                        */
/* _________________________________________                        */
/*                                                                  */
/*                                                                 !*/

fgets(row_code,9,f);                     /* Read code for row       */
if (ferror(f) != 0 || feof(f) != 0 )     /* Check file              */
  {
  sprintf(errbuf," %s %%varkon_sur_readpts" ,infile); 
  varkon_erinit();
  return(varkon_erpush("SU2653",errbuf));
  }  /* End ferror or feof  */

if( strcmp(row_code,"total   ") == 0 ||  /* Check that it is        */
    strcmp(row_code,"TOTAL   ") == 0   ) /* total                   */
    ;                                    /*                         */
else
 {
 sprintf(errbuf, "(not total)%%varkon_sur_readpts");
 return(varkon_erpush("SU2993",errbuf));
 }

fgets(rad,132,f);                        /* Read the rest of line 1 */
sscanf(rad,"%d%d",&nu,&nv);              /* Total number of patches */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur296 row_code %s nu %d nv %d\n",
     row_code , nu, nv ); 
  }
#endif


/*!                                                                 */
/* Dynamic allocation of area (pp_ptsout) for the point data.       */
/* Call of v3mall.                                                  */
/*                                                                 !*/

   maxnum = nu*nv;


if((p_frst=(SURPOI *)v3mall((unsigned)
               (maxnum*sizeof(SURPOI)),"sur926"))==NULL) 
 {
#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur296 Allocation failed  nu %d nv %d maxnum %d\n",
    nu, nv, (int)maxnum );
  }
#endif
  sprintf(errbuf, "(alloc)%%varkon_sur_readpts  ");
  return(varkon_erpush("SU2993",errbuf));
  }

 *pp_ptsout = p_frst;                    /* Output pointer          */


#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur296 Table SURPOI memory allocation for (max) nu*nv %d pts\n",
    (int)maxnum);
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur296 Ptr to first point p_frst  = %d in table SURPOI\n",
    (int)p_frst );
  }
#endif

/*!                                                                 */
/* 4. Initialize data in table SURPOI                               */
/* __________________________________                               */
/*                                                                  */
/* Let all point addresses ( iu , iv ) = ( -1 , -1 )                */
/* Calls of varkon_ini_surpoi (sur171).                             */
/*                                                                 !*/

for ( iu =1; iu <= nu;    ++iu  )        /* Start loop U pts        */
  {                                      /*                         */
  for ( iv =1; iv <= nv;    ++iv  )      /* Start loop V pts        */
      {                                  /*                         */
      p_p=p_frst+(iu - 1)*nv+iv - 1;     /* Ptr to  current point   */
      varkon_ini_surpoi (p_p);           /* Initialize              */
      }                                  /* End   loop V pts        */
  }                                      /* End   loop U pts        */

/*!                                                                 */
/*!                                                                 */
/* 5. Retrieve point data and write in table SURPOI                 */
/* ________________________________________________                 */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Loop all (possible) points in file  i_p= 1,2,....,maxnum         */
/*                                                                 !*/

fgets(row_code,9,f);                     /* Read code for row 2     */
if( strcmp(row_code,"pointadr") == 0 ||  /* Check that it is        */
    strcmp(row_code,"POINTADR") == 0   ) /* pointadr                */
    npts = 1;                            /* Initialize no. of pts   */
else
 {
 v3free(p_frst,"sur296");
 sprintf(errbuf, "(row 2)%%varkon_sur_readpts ");
 return(varkon_erpush("SU2993",errbuf));
 }


for ( i_p=1; i_p<=maxnum; ++i_p )        /* Start loop pts in file  */
  {

/*!                                                                 */
/*    Retrieve point number iu,iv (iu_f,iv_f) from input file       */
/*                                                                 !*/

                                         /* row_code= pointadr is   */
                                         /* already retrieved       */
fgets(rad,132,f);                        /* Read the rest of line   */
sscanf(rad,"%d%d",&iu_f,&iv_f);          /* Patch adress            */

 if ( iu_f > nu || iu_f < 1 )
 {                                        /*                        */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur296 Failure iu_f= %d > nu= %d\n",
     iu_f , nu ); 
  }
#endif
 v3free(p_frst,"sur296");                /* Deallocate memory       */
 sprintf(errbuf, " %d,%d %% %d,%d",iu_f,iv_f,nu,nv);
 return(varkon_erpush("SU2693",errbuf));
 }
 if ( iv_f > nv || iv_f < 1 )
 {                                        /*                        */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur296 Failure iv_f= %d > nv= %d\n",
     iv_f , nv ); 
  }
#endif
 v3free(p_frst,"sur296");                /* Deallocate memory       */
 sprintf(errbuf, " %d,%d %% %d,%d",iu_f,iv_f,nu,nv);
 return(varkon_erpush("SU2693",errbuf));
 }



/*!                                                                 */
/*    Pointer to current pt record p_p= p_first+(iu-1)*nv+iv-1      */
/*                                                                 !*/

      p_p=p_frst+(iu_f-1)*nv+iv_f-1;     /* Ptr to  current point   */

#ifdef  DEBUG
 if((iu_f-1)*nv+iv_f - 1 > maxnum - 1 )
 {                                        /*                        */
 v3free(p_frst,"sur296");                /* Deallocate memory       */
 sprintf(errbuf, "(p_p)%% varko_sur_readpts");
 return(varkon_erpush("SU2993",errbuf));
 }
#endif



#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && npts <  1000  )
  {
  fprintf(dbgfil(SURPAC), 
  "sur296 row_code %s npts %d iu_f %d iv_f %d\n",
     row_code , (int)npts, iu_f,iv_f ); 
  }
if ( dbglev(SURPAC) == 2 && npts <  10  )
  {
  fprintf(dbgfil(SURPAC), 
  "sur296 (iu_f-1)*nv+iv_f-1 %d iu_f %d iv_f %d\n",
     (iu_f-1)*nv+iv_f-1, iu_f,iv_f ); 
  }
#endif



/*!                                                                 */
/*    Point address iu,iv to table SURPOI                           */
/*                                                                 !*/

      p_p->iu   = iu_f;                  /* Point number            */
      p_p->iv   = iv_f;                  /*                         */

/*!                                                                 */
/*    Loop all (possible) rows (fields) for point i_r=1,2,...,20    */
/*                                                                 !*/

      for ( i_r=1; i_r<=20; ++i_r )      /* Start loop pts in file  */
          {
/*!                                                                 */
/*        Read  row_code                                            */
/*                                                                 !*/

          fgets(row_code,9,f);            /* Read code for row      */

       if (ferror(f) != 0 || feof(f) != 0 )
         {
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur296 i_p %d i_r %d ferror %d feof %d\n",
     (int)i_p , (int)i_r ,ferror(f) ,feof(f) ); 
  }
#endif
         sprintf(errbuf," %s %%varkon_sur_readpts" ,infile); 
         varkon_erinit();
         return(varkon_erpush("SU2653",errbuf));
         }  /* End ferror or feof  */

if     (strcmp(row_code,"pointadr" )==0 ||/* Next point             */
        strcmp(row_code,"POINTADR" )==0 ) /*                        */
        {                                 /*                        */
        npts = npts + 1;                  /* An additional point    */
        goto nxtpt;                       /*                        */
        }                                 /*                        */
else if(strcmp(row_code,"eof     " )==0 ||/* End Of File            */
        strcmp(row_code,"EOF     " )==0 ) /*                        */
        {                                 /*                        */
        goto nomore;                      /*                        */
        }                                 /*                        */
else if(strcmp(row_code,"xyz     " )==0 ||/* Point                  */
        strcmp(row_code,"XYZ     " )==0 ) /*                        */
        {                                 /*                        */
        fgets(rad,132,f);                 /* Read rest of line      */
        sscanf(rad,"%lf%lf%lf",           /* Data to dummy          */
             &k1,&k2,&k3);                /* variables              */
        p_p->spt.r_x= (gmflt)k1;          /* Output data            */
        p_p->spt.r_y= (gmflt)k2;          /*                        */
        p_p->spt.r_z= (gmflt)k3;          /*                        */
        }                                 /*                        */
else if(strcmp(row_code,"drdu    " )==0 ||/* Tangent in U direction */
        strcmp(row_code,"DRDU    " )==0 ) /*                        */
        {                                 /*                        */
        fgets(rad,132,f);                 /* Read rest of line      */
        sscanf(rad,"%lf%lf%lf",           /* Data to dummy          */
             &k1,&k2,&k3);                /* variables              */
        p_p->spt.u_x= (gmflt)k1;          /* Output data            */
        p_p->spt.u_y= (gmflt)k2;          /*                        */
        p_p->spt.u_z= (gmflt)k3;          /*                        */
        }                                 /*                        */
else if(strcmp(row_code,"drdv    " )==0 ||/* Tangent in V direction */
        strcmp(row_code,"DRDV    " )==0 ) /*                        */
        {                                 /*                        */
        fgets(rad,132,f);                 /* Read rest of line      */
        sscanf(rad,"%lf%lf%lf",           /* Data to dummy          */
             &k1,&k2,&k3);                /* variables              */
        p_p->spt.v_x= (gmflt)k1;          /* Output data            */
        p_p->spt.v_y= (gmflt)k2;          /*                        */
        p_p->spt.v_z= (gmflt)k3;          /*                        */
        }                                 /*                        */
else if(strcmp(row_code,"d2rdu2  " )==0 ||/* 2'nd der. U direction  */
        strcmp(row_code,"D2RDU2  " )==0 ) /*                        */
        {                                 /*                        */
        fgets(rad,132,f);                 /* Read rest of line      */
        sscanf(rad,"%lf%lf%lf",           /* Data to dummy          */
             &k1,&k2,&k3);                /* variables              */
        p_p->spt.u2_x= (gmflt)k1;         /* Output data            */
        p_p->spt.u2_y= (gmflt)k2;         /*                        */
        p_p->spt.u2_z= (gmflt)k3;         /*                        */
        }                                 /*                        */
else if(strcmp(row_code,"d2rdv2  " )==0 ||/* 2'nd der. V direction  */
        strcmp(row_code,"D2RDV2  " )==0 ) /*                        */
        {                                 /*                        */
        fgets(rad,132,f);                 /* Read rest of line      */
        sscanf(rad,"%lf%lf%lf",           /* Data to dummy          */
             &k1,&k2,&k3);                /* variables              */
        p_p->spt.v2_x= (gmflt)k1;         /* Output data            */
        p_p->spt.v2_y= (gmflt)k2;         /*                        */
        p_p->spt.v2_z= (gmflt)k3;         /*                        */
        }                                 /*                        */
else if(strcmp(row_code,"d2rdudv " )==0 ||/* Twist vector           */
        strcmp(row_code,"D2RDUDV " )==0 ) /*                        */
        {                                 /*                        */
        fgets(rad,132,f);                 /* Read rest of line      */
        sscanf(rad,"%lf%lf%lf",           /* Data to dummy          */
             &k1,&k2,&k3);                /* variables              */
        p_p->spt.uv_x= (gmflt)k1;         /* Output data            */
        p_p->spt.uv_y= (gmflt)k2;         /*                        */
        p_p->spt.uv_z= (gmflt)k3;         /*                        */
        }                                 /*                        */
else if(strcmp(row_code,"normal  " )==0 ||/* Surface normal         */
        strcmp(row_code,"NORMAL  " )==0 ) /*                        */
        {                                 /*                        */
        fgets(rad,132,f);                 /* Read rest of line      */
        sscanf(rad,"%lf%lf%lf",           /* Data to dummy          */
             &k1,&k2,&k3);                /* variables              */
        p_p->spt.n_x= (gmflt)k1;          /* Output data            */
        p_p->spt.n_y= (gmflt)k2;          /*                        */
        p_p->spt.n_z= (gmflt)k3;          /*                        */
        }                                 /*                        */
else if(strcmp(row_code,"dndu    " )==0 ||/* Normal derivative U    */
        strcmp(row_code,"DNDU    " )==0 ) /*                        */
        {                                 /*                        */
        fgets(rad,132,f);                 /* Read rest of line      */
        sscanf(rad,"%lf%lf%lf",           /* Data to dummy          */
             &k1,&k2,&k3);                /* variables              */
        p_p->spt.nu_x= (gmflt)k1;         /* Output data            */
        p_p->spt.nu_y= (gmflt)k2;         /*                        */
        p_p->spt.nu_z= (gmflt)k3;         /*                        */
        }                                 /*                        */
else if(strcmp(row_code,"dndv    " )==0 ||/* Normal derivative V    */
        strcmp(row_code,"DNDV    " )==0 ) /*                        */
        {                                 /*                        */
        fgets(rad,132,f);                 /* Read rest of line      */
        sscanf(rad,"%lf%lf%lf",           /* Data to dummy          */
             &k1,&k2,&k3);                /* variables              */
        p_p->spt.nv_x= (gmflt)k1;         /* Output data            */
        p_p->spt.nv_y= (gmflt)k2;         /*                        */
        p_p->spt.nv_z= (gmflt)k3;         /*                        */
        }                                 /*                        */
else if(strcmp(row_code,"kappamin" )==0 ||/* Min. curvature         */
        strcmp(row_code,"KAPPAMIN" )==0 ) /*                        */
        {                                 /*                        */
        fgets(rad,132,f);                 /* Read rest of line      */
/* 970209 sscanf(rad,"%lf%lf%lf",  */     /* Data to dummy          */
        sscanf(rad,"%lf",                 /* Data to dummy          */
             &k1);                        /* variable               */
        p_p->spt.kmin= (gmflt)k1;         /* Output data            */
        }                                 /*                        */
else if(strcmp(row_code,"kappamax" )==0 ||/* Max. curvature         */
        strcmp(row_code,"KAPPAMAX" )==0 ) /*                        */
        {                                 /*                        */
        fgets(rad,132,f);                 /* Read rest of line      */
        sscanf(rad,"%lf",                 /* Data to dummy          */
             &k1);                        /* variable               */
        p_p->spt.kmax= (gmflt)k1;         /* Output data            */
        }                                 /*                        */
else if(strcmp(row_code,"r3min   " )==0 ||/* Min. prin. direct. R*3 */
        strcmp(row_code,"R3MIN   " )==0 ) /*                        */
        {                                 /*                        */
        fgets(rad,132,f);                 /* Read rest of line      */
        sscanf(rad,"%lf%lf%lf",           /* Data to dummy          */
             &k1,&k2,&k3);                /* variables              */
        p_p->spt.kmin_x= (gmflt)k1;       /* Output data            */
        p_p->spt.kmin_y= (gmflt)k2;       /*                        */
        p_p->spt.kmin_z= (gmflt)k3;       /*                        */
        }                                 /*                        */
else if(strcmp(row_code,"r3max   " )==0 ||/* Max. prin. direct. R*3 */
        strcmp(row_code,"R3MAX   " )==0 ) /*                        */
        {                                 /*                        */
        fgets(rad,132,f);                 /* Read rest of line      */
        sscanf(rad,"%lf%lf%lf",           /* Data to dummy          */
             &k1,&k2,&k3);                /* variables              */
        p_p->spt.kmax_x= (gmflt)k1;       /* Output data            */
        p_p->spt.kmax_y= (gmflt)k2;       /*                        */
        p_p->spt.kmax_z= (gmflt)k3;       /*                        */
        }                                 /*                        */

else                                      /* Not a valid row_code   */
 {                                        /*                        */
#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur296 Failure row_code= %s\n",
     row_code); 
  }
#endif
 v3free(p_frst,"sur296");                /* Deallocate memory       */
 sprintf(errbuf, " %s %%varkon_sur_readpts ",row_code);
 return(varkon_erpush("SU2703",errbuf));
 }

      }                                  /* End   loop rows for pt  */
/*!                                                                 */
/*    End  all (possible) rows (fields) for point i_r=1,2,...,20    */
/*                                                                 !*/


#ifdef DEBUG
 v3free(p_frst,"sur296");                /* Deallocate memory       */
 sprintf(errbuf, "(rows>20)%%varkon_sur_readpts ");
 return(varkon_erpush("SU2993",errbuf));
#endif



nxtpt:; /*! Label nxtpt: Next point (record) in file               !*/

  }                                      /* End   loop pts in file  */
/*!                                                                 */
/* End  all points in file  i_p= 1,2,3,....,maxnum                  */
/*                                                                 !*/

nomore:;/*! Label nomore: End Of File                              !*/

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), "sur296 Close input file %s\n",infile);
  }
#endif

/*!                                                                 */
/* Close the input file (C function fclose)                         */
/*                                                                 !*/

 fclose(f);                              /* Close the input file    */


/*                                                                  */
/* 6. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                  */

  *p_nu = nu;                            /* To output variable      */
  *p_nv = nv;

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur296 Total number of points %d (max is nu*nv= %d)\n", 
       (int)npts ,nu*nv  );
  }
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur296 Exit***varkon_sur_readpts ** Output nu %d nv %d\n"
    , (int)*p_nu , (int)*p_nv  );
  }
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/
