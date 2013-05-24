/*!******************************************************************
*   
*    exsurexd.c 
*    ==========
*        
*    EXsexd();     Create SUR_EXDEF
*    EXsexdv();    Create SUR_EXDEF for VDA-FS/POL_SUR
*
*    This file is part of the VARKON Execute  Library.
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
*
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../include/EX.h"
#include <string.h>

#ifdef DEBUG
#include "../../IG/include/debug.h"
#endif

/******************************************************************!*/

extern DBTmat *lsyspk;          /* Active coord. system             */
extern DBTmat  lklsyi;          /* Active coord. system, inverted   */
/* varkon_sur_importtype         * Determine surface type           */
/* varkon_nmgbicubic             * CUB_SUR from NMG (F07) file      */
/* varkon_nmglftsurf             * LFT_SUR from NMG (F07) file      */
/* varkon_sur_ratimport          * RAT_SUR from Varkon file         */
/* varkon_nmgconsurf             * CON_SUR from NMG (F07) file      */
/* varkon_vda_nosur              * No. of surfaces in VDA-FS file   */
/* varkon_vda_rsurm              * POL_SUR from VDA-FS file         */
/* varkon_sur_trim               * Trim a surface                   */
/* varkon_sur_bictra             * Transformation of CUB_SUR        */
/* varkon_sur_rbictra            * Transformation of RAT_SUR        */
/* varkon_sur_lofttra            * Transformation of LFT_SUR        */
/* varkon_sur_transf             * Transformation of a surface      */
/* varkon_sur_poltra             * Transformation of POL_SUR        */

/*!******************************************************************/

       short EXsexd(
       DBId    *id,
       char    *str1,
       char    *str2,
       char    *str3,
       DBfloat  trim[4],
       DBint    new,
       V2NAPA  *pnp)


/*      Skapar yta från extern fil.
 *
 *      In: id     => Pekare till identitet.
 *          str1   => Pekare till filnamn.
 *          str2   => Pekare till ursprung.
 *          str3   => Pekare till |nskad typ.
 *          trim   => Anger vilken del av ytan som avses.
 *                    trim[0] = U-start
 *                    trim[1] = U-slut
 *                    trim[2] = V-start
 *                    trim[3] = V-slut
 *                    Alla = 0 => Hela ytan
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod:      0 = Ok.
 *              EX4002 = Kan ej l{sa yta fr}n %s -fil
 *              EX4012 = Kan ej bilda yta av typen %s
 *              EX4022 = Kan ej |ppna %s
 *              EX4052 = Fel vid inl{sning av %s
 *              EX4062 = Kan ej transformera yta fr}n fil %s
 *              EX4082 = U,V riktningar kan ej v{ndas f|r %s
 *              EX4092 = Extrapolering ej till}ten f|r %s
 *              EX4102 = Trimning av yta %s {nnu ej inf|rt  
 *              EX4112 = Trimning misslyckades               
 *
 *      (C)microform ab 20/11/92            J. Kjellander
 *
 *      21/ 3/94 Nya DBPatch,                 J. Kjellander
 *      26/11/94 Trimning av bikubiska ytor G. Liden
 *      19/12/95 Consurf                    G. Liden
 *       3/ 2/96 sur610 för transformation  G. Liden
 *       2/11/96 VDA-FS  (sur550)           G. Liden
 *      20/ 3/97 Additional argument new    G. Liden
 *      19/ 4/97 From EXe17, VDA-FS, sur549 G. Liden
 *      1999-12-18 Long surxxx names        G. Liden
 *
 ******************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr EXsexd                Function corresp. to SUR_EXDEF   */
/*                                                              */
/*------------------------------------------------------------- */

  {
    char    buf[V3STRLEN+1];
    short   status;
    DBSurf   sur;
    DBPatch  *ptpat;
    DBint   surtype;     /* Typ av yta (indata fil)                 */
                         /* =1: NMG bikubisk                        */
                         /* =2: NMG Consurf in section format       */
    DBint   nupatch;     /* Antal "patches" i U riktning            */
    DBint   nvpatch;     /* Antal "patches" i V riktning            */
    DBint   t_flag;      /* Trim flagga= 1 <=> Hela = 2 <=> Del     */
    DBSurf   surwho;     /* Hela ytan som sedan kommer att trimmas  */
    DBPatch *p_patwho;   /* Patch data f|r hela ytan          (ptr) */
    DBfloat  s_lim[2][2];/* Ytan begr{nsas med tv} U,V punkter      */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): Slut  U  (1,1): Slut  V          */
                         /* Hela ytan om alla s_lim {r noll         */
    DBint   tcase;       /* Trimningsfall                           */
                         /* Eq.  1: Reparameterisera                */
                         /* Eq.  2: Ingen reparameterisering        */
                         /* Eq. 11: Som 1 men beh}ll U,V riktning   */
                         /* Eq. 12: Som 2 men beh}ll U,V riktning   */
    char    errbuf[80];  /* Str{ng f|r fel meddelande fktn erpush   */


#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC), "exe21*EXsexd Enter new %d\n", new);
  fflush(dbgfil(EXEPAC));
  }
#endif

/*
***VDA-FS file
*/

    if ( strcmp(str2,"VDA-FS") == 0 )
      {
      status = EXsexdv( id, str1, str2, str3, trim, new, pnp );
      if (status < 0 )
        {
#ifdef DEBUG
        sprintf(errbuf,"EXsexdv%%exe21*EXsexd");
        erpush("SU2943",errbuf);
#endif
        goto  error2;
        }
      goto  error2;
      }

/*
***Testa indata. Tills vidare kan vi bara l{sa NMG-F07-filer.
*/
   if ( (status=varkon_sur_importtype
               (str1,&surtype,&nupatch,&nvpatch)) < 0 )
      {
      goto error2;
      }

    if ( IGftst(str1) == FALSE ) return(erpush("EX4022",str1));
    if ( strcmp(str2,"NMG_F07") != 0 ) return(erpush("EX4002",str2));
/*
***En liten trevlig utskrift medan ytan l{ses in !
*/
   strcpy(buf,IGgtts(164));
   strcat(buf,IGgtts(181));
   strcat(buf,str1);
   IGplma(buf,IG_MESS);
/*
***Anropa inl{sningsrutin. Denna ber{knar antal patchar,
***reserverar minne, skriver in patch-data, fyller i sur-posten
***med antal patchar etc. Om surxxx-rutiner returnerar status < 0
***har inget minne allokerats.
*/

/*
***Bikubisk yta.
*/
/*  Initiera flagga till hela ytan (ingen trimning)                 */
    t_flag = 1;
    if ( strcmp(str3,"CUB_SUR") == 0 )
      {
      if ( trim[0]+trim[1]+trim[2]+trim[3] > 0.000001 )
         {
/*       Bara en del av ytan ska skapas. S{tt flagga till delyta.   */
         t_flag = 2;
/*       Kontrollera att trimningsdata {r OK.                       */
/*       V{ndning av U,V riktningar {r (f.n) inte till}tet.         */
         if ( trim[0] < trim[1] && trim[2] < trim[3]  )     
            {
            /* Data {r OK                                           */
            }
         else
            {
            erinit();
            sprintf(errbuf,"CUB_SUR%%");
            status= erpush("EX4082",errbuf);
            goto error2;
            }
/*       Extrapolering {r (f.n) inte till}tet.                      */
         if ( trim[0]> -0.000000000001 && trim[2]> -0.00000000000001)
            {
            /* Data {r OK (extrapolering kan {nnu inte kollas fullt */
            }
         else
            {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
"exe21 Ingen extrapolering trim(0) %f (1) %f (2) %f (3) %f\n", 
                          trim[0],trim[1],trim[2],trim[3]);
  fflush(dbgfil(EXEPAC));
  }
#endif
            erinit();
            sprintf(errbuf,"CUB_SUR%%");
            status= erpush("EX4092",errbuf);
            goto error2;
            }
/*       Skapa f|rst hela ytan.                                     */
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"exe21 Skapa f|rst hela ytan \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
         if ( (status=varkon_nmgbicubic(str1,&surwho,&p_patwho)) < 0 )
           {
           erinit();
           status = erpush("EX4052",str1);
           goto error2;
           }
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"exe21 Hel yta &surwho %d p_patwho %d\n",
                                (int)&surwho,(int)p_patwho );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
/*       Skapa del av yta genom att anropa c rutin f|r SUR_TRIM     */
         s_lim[0][0]= trim[0]+1.0; /* Start U                       */
         s_lim[1][0]= trim[1]+1.0; /* Slut  U                       */
         s_lim[0][1]= trim[2]+1.0; /* Start V                       */
         s_lim[1][1]= trim[3]+1.0; /* Slut  V                       */

/*       Trimma ytan.                                               */
         IGrsma();      /* Radera meddelande zon                    */
         IGplma("Ytan trimmas",IG_MESS);
         tcase  = 11;
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"exe21 Trimma till |nskad delyta sur181\n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
         status = varkon_sur_trim
         (&surwho,p_patwho,s_lim,tcase, &sur,&ptpat); 
         if ( status < 0 )
            {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"exe21 Trimning sur181 misslyckades\n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
            sprintf(errbuf,"CUB_SUR%%");
            erpush("EX4112",errbuf);
            goto error2;
            }
         }       /* Slut trimning av yta  */
      else 
         {
/*       Hela ytan ska skapas                                       */
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"exe21 Skapa hel yta utan trimning \n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
         if ( (status=varkon_nmgbicubic(str1,&sur,&ptpat)) < 0 )
           {
           status = erpush("EX4052",str1);
           goto error2;
           }
         }      /* Slut hela ytan          */

      if ( lsyspk != NULL )
        { 
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"exe21 Transformera yta sur600\n");
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
        if ( (status=varkon_sur_bictra(&sur,ptpat,&lklsyi)) < 0 )
          {
          status = erpush("EX4062",str1);
          goto error1;
          }
        }
      }
/*
***Rationell yta     
*/
    else if ( strcmp(str3,"RAT_SUR") == 0 )
      {
      if ( trim[0]+trim[1]+trim[2]+trim[3] > 0.000001 )
         {
/*       Denna typ av yta kan {nnu inte trimmas vid inl{sning       */
         erinit();
         sprintf(errbuf,"RAT_SUR%%");
         status= erpush("EX4102",errbuf);
         goto error2;
         }

      if ( (status=varkon_sur_ratimport(str1,&sur,&ptpat)) < 0 )
        {
        status = erpush("EX4052",str1);
        goto error2;
        }

      if ( lsyspk != NULL )
        { 
        if ( (status=varkon_sur_rbictra(&sur,ptpat,&lklsyi)) < 0 )
          {
          status = erpush("EX4062",str1);
          goto error1;
          }
        }
      }
/*
***Conic lofting-yta.
*/
    else if ( strcmp(str3,"LFT_SUR") == 0 )
      {
      if ( trim[0]+trim[1]+trim[2]+trim[3] > 0.000001 )
         {
/*       Denna typ av yta kan {nnu inte trimmas vid inl{sning       */
         erinit();
         sprintf(errbuf,"LFT_SUR%%");
         status= erpush("EX4102",errbuf);
         goto error2;
         }

      if ( (status=varkon_nmglftsurf(str1,&sur,&ptpat)) < 0 )
        {
        status = erpush("EX4052",str1);
        goto error2;
        }

      if ( lsyspk != NULL )
        { 
        if ( (status=varkon_sur_lofttra(&sur,ptpat,&lklsyi)) < 0 )
          {
          status = erpush("EX4062",str1);
          goto error1;
          }
        }
      }


/*
***Consurf            
*/
    else if ( strcmp(str3,"CON_SUR") == 0 )
      {
      if ( trim[0]+trim[1]+trim[2]+trim[3] > 0.000001 )
         {
         t_flag = 2;
         if ( (status=varkon_nmgconsurf(str1,&surwho,&p_patwho)) < 0 )
           {
           status = erpush("EX4052",str1);
           if ( ptpat == NULL ) goto error2;
           else                 goto error1;
           }
         s_lim[0][0]= trim[0]+1.0; /* Start U                       */
         s_lim[1][0]= trim[1]+1.0; /* End   U                       */
         s_lim[0][1]= trim[2]+1.0; /* Start V                       */
         s_lim[1][1]= trim[3]+1.0; /* End   V                       */
/*       Trim the surface                                           */
         IGrsma();      /* Radera meddelande zon                    */
         IGplma("Surface is trimmed",IG_MESS);
         tcase  = 11;
         status = varkon_sur_trim
         (&surwho,p_patwho,s_lim,tcase, &sur,&ptpat); 
         if ( status < 0 )
            {
            sprintf(errbuf,"CON_SUR%%");
            erpush("EX4112",errbuf);
            goto error2;
            }
         }

      else
        {
        if ( (status=varkon_nmgconsurf(str1,&sur,&ptpat)) < 0 )
          {
          status = erpush("EX4052",str1);
          if ( ptpat == NULL ) goto error2;
          else                 goto error1;
          }
         } /* End no trimming of Consurf */

      if ( lsyspk != NULL )
        { 
        if ( (status=varkon_sur_transf(&sur,ptpat,&lklsyi)) < 0 )
          {
          status = erpush("EX4062",str1);
          goto error1;
          }
        }
      }





/*
***Ok{nd typ av yta.
*/
    else
      {
      status = erpush("EX4012",str3);
      goto error2;
      }
/*
***Lagra i DB och rita.
*/
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"exe21 Yta till DB &sur %d ptpat %d\n",
                                (int)&sur,(int)ptpat );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
    IGrsma();
    status = EXesur(id,&sur,ptpat,NULL,NULL,pnp);
/*
***Nu kan vi sl{ppa det dynamiskt allokerade minnet f|r
***patcharna.
*/
error1:
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 && t_flag == 2 )
  {
  fprintf(dbgfil(EXEPAC),"exe21 Deallokera  &surwho %d p_patwho %d\n",
                                (int)&surwho,(int)p_patwho );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"exe21 Deallokera  &sur    %d ptpat    %d\n",
                                (int)&sur,(int)ptpat );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif
    if( t_flag == 2 )DBfree_patches(&surwho,p_patwho);
    DBfree_patches(&sur,ptpat);
/*
***Slut.
*/
error2:
    return(status);
  }

/********************************************************/


/*!******************************************************************/
/*                                                                  */
/*  Function: EXsexdv                                               */
/*  ==================                                              */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function corresponds to SUR_EXDEF for VDA-FS/POL_SUR        */
/*                                                                  */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1997-04-18   Originally written                                 */
/*  1998-01-03   Trimming added                                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr EXsexdv               SUR_EXDEF for VDA-FS/POL_SUR     */
/*                                                              */
/*------------------------------------------------------------- */

/*!****************** Function **************************************/
/*                                                                  */
  short EXsexdv(
  DBId    *sur_id,       /* Reference (# id.) for output surface    */
  char    *str1,         /* Full file name                          */
  char    *str2,         /* Input  type of file (= VDA-FS)          */
  char    *str3,         /* Output surface type (= POL_SUR)         */
  DBfloat  trim[4],      /* Trim data                               */
                         /* trim[0] = U start                       */
                         /* trim[1] = U end                         */
                         /* trim[2] = V start                       */
                         /* trim[3] = V end                         */
                         /* Whole surface for trim[0-3] = 0         */
  DBint    sur_no_in,    /* Requested surface number, input         */
                         /* sur_no = 0: All surfaces in the file    */
  V2NAPA  *pnp)          /* Attribute data for surface sur_id       */


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
    DBint   no_sur_tot;  /* Total number of surfaces in VDA-FS file */
    DBint   i_sur;       /* Loop index surface                      */
    DBint   i_sur_start; /* Start surface number for loop           */
    DBint   i_sur_end;   /* End   surface number for loop           */
    DBSurf   surwho;      /* Whole surface                           */
    DBPatch  *p_patwho;    /* Patch data for the whole surface  (ptr) */
    DBfloat   s_lim[2][2]; /* Surface limited by two U,V points       */
                         /* (0,0): Start U  (0,1): Start V          */
                         /* (1,0): End   U  (1,1): End   V          */
                         /* Whole surface if all s_lim is zero      */
    DBint   tcase;       /* Trimming case                           */
                         /* Eq.  1: Reparameterize                  */
                         /* Eq.  2: No reparameterization           */
                         /* Eq. 11: As 1 but keep U,V direction     */
                         /* Eq. 12: As 2 but keep U,V direction     */
    DBSurf   surtri;      /* Trimmed surface                         */
    DBPatch  *p_pattri;    /* Patch data trimmed surface        (ptr) */
    DBSurf   surtra;      /* Transformated surface                   */
    DBPatch  *p_pattra;    /* Patch data transformated surface  (ptr) */
    DBint   sur_no;      /* Requested VDA-FS surface                */
    char    sur_name[80];/* VDA-FS surface name                     */
    char    errbuf[80];  /* String for error message fctn erpush    */

    short   status;      /* Error code from a called function       */

/*--------------end-of-declarations---------------------------------*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe21*EXsexdv Enter SUR_EXDEF for VDA-FS/POL_SUR \n");
fflush(dbgfil(EXEPAC));
}
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe21*EXsexdv sur_no_in %d\n", sur_no_in);
fflush(dbgfil(EXEPAC));
}
#endif

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

   status = 0;

/* Programming check that str1 is VDA-FS                            */

  if ( strcmp(str2,"VDA-FS") != 0 )
    {
    sprintf(errbuf,"Not VDA-FS%%EXsexdv");
    erinit();
    return(erpush("SU2993",errbuf));
    }
    
/*!                                                                 */
/* A VDA-FS surface can only be converted to a POL_SUR surface      */
/*                                                                 !*/

  if ( strcmp(str3,"POL_SUR") != 0 )
    {
    sprintf(errbuf,"%s%%EXsexdv",str3);
    erinit();
    return(erpush("SU5523",errbuf));
    }
    
/* Initialize local variables                                       */
   i_sur       = I_UNDEF;     
   i_sur_start = I_UNDEF;
   i_sur_end   = I_UNDEF;

/*!                                                                 */
/* 2. Determine loop parameters for read of surface(s)              */
/* __________________________________________________               */
/*                                                                  */
/*                                                                  */
/* Get the number of surfaces in the VDA-FS file.                   */
/* Call of varkon_vda_nosur (sur549).                               */
/*                                                                 !*/

   status = varkon_vda_nosur ( str1, &no_sur_tot );
   if (status < 0 )
     {
#ifdef DEBUG
     sprintf(errbuf,"varkon_vda_nosur (sur549)%%exe21*EXsexdv");
     erpush("SU2943",errbuf);
#endif
     return(status);
     }

/*!                                                                 */
/* Start and end loop values                                        */
/*                                                                 !*/

   if         (  sur_no_in == 0 )
     {
     i_sur_start = 1;
     i_sur_end   = no_sur_tot;
     }
   else if    (  sur_no_in > 0 && sur_no_in <= no_sur_tot )
     {
     i_sur_start = sur_no_in;
     i_sur_end   = sur_no_in;
     }
   else if    (  sur_no_in > 0 && sur_no_in >  no_sur_tot )
     {
     sprintf(errbuf,"sur_no_in %d%%EXsexdv",(int)sur_no_in);
     erinit();
     return(erpush("SU5513",errbuf));
     }
   else
     {
     sprintf(errbuf,"sur_no_in %d%%EXsexdv",(int)sur_no_in);
     erinit();
     return(erpush("SU5533",errbuf));
     }

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe21*EXsexdv no_sur_tot %d\n", (int)no_sur_tot );
fflush(dbgfil(EXEPAC));
}
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),
"exe21*EXsexdv i_sur_start %d  i_sur_end %d\n", 
           (int)i_sur_start, (int)i_sur_end );
fflush(dbgfil(EXEPAC));
}
#endif

/*!                                                                 */
/* 3. Create surface(s)                                             */
/* ____________________                                             */
/*                                                                  */
/*                                                                 !*/

/*!                                                                 */
/* Start loop all surfaces                                          */
/*                                                                 !*/

  for ( i_sur = i_sur_start; i_sur <= i_sur_end; ++i_sur )
    {

/*!                                                                 */
/*  Current surface number                                          */
/*                                                                 !*/

    sur_no = i_sur; 

/*!                                                                 */
/*  Create whole surface without trimming.                          */
/*  Call of varkon_vda_rsurm (sur551).                              */
/*                                                                 !*/

    status=varkon_vda_rsurm(str1,sur_no,&surwho,&p_patwho,sur_name);
    if  (  status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),"exe21*EXsexdv varkon_vda_rsurm status %d sur_no %d\n",
            (short)status,  (int)sur_no);
fflush(dbgfil(EXEPAC));
}
#endif
      status = erpush("EX4052",str1);
      if( p_patwho != NULL )DBfree_patches(&surwho,p_patwho);
      goto error2;
      }

/*!                                                                 */
/*  Trim the surface.                                               */
/*  Call of varkon_sur_surtrim (sur181).                            */
/*                                                                 !*/

    if ( trim[0]+trim[1]+trim[2]+trim[3] > 0.000001 )
      {
       s_lim[0][0]= trim[0]+1.0; /* Start U                       */
       s_lim[1][0]= trim[1]+1.0; /* End   U                       */
       s_lim[0][1]= trim[2]+1.0; /* Start V                       */
       s_lim[1][1]= trim[3]+1.0; /* End   V                       */
/*     Trim the surface                                           */
       IGrsma();      /* Erase message zone                       */
       IGplma("Surface is trimmed",IG_MESS);
       tcase  = 11;
       status = varkon_sur_trim
       (&surwho,p_patwho,s_lim,tcase, &surtri,&p_pattri); 
       if ( status < 0 )
          {
          sprintf(errbuf,"POL_SUR%%");
          erpush("EX4112",errbuf);
          goto error2;
          }
       }
#ifdef  TABORT
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),"exe21*EXsexdv Trimming POL_SUR not impl.\n");
fflush(dbgfil(EXEPAC));
}
#endif
      sprintf(errbuf,"POL_SUR%%EXsexdv");
      erinit();
      return(erpush("SU5543",errbuf));
      }
#endif  /*  TABORT  */

    else
      {
      V3MOME(&surwho,&surtri, sizeof(DBSurf));
      p_pattri = p_patwho;
      }

/*!                                                                 */
/*  Transformate the surface if a local system is active.           */
/*  Call of varkon_sur_poltra (sur520).                             */
/*                                                                 !*/


    if ( lsyspk != NULL )
      { 
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"exe21*EXsexdv Transformate POL_SUR\n");
  fflush(dbgfil(EXEPAC));
  }
#endif
      V3MOME(&surtri,&surtra, sizeof(DBSurf));
      p_pattra = p_pattri;
      status=varkon_sur_poltra (&surtra,p_pattra,&lklsyi);
      if ( status < 0 )
        {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),"exe21 EXesur status %d\n",(short)status );
  fflush(dbgfil(EXEPAC));
}
#endif
        if( p_pattra != NULL )DBfree_patches(&surtra,p_pattra);
        sprintf(errbuf,"varkon_sur_poltra(sur520)%%exe21*EXsexdv");
        status = erpush("SU2943",errbuf);
        goto error2;
        }
      }
    else
      {
      V3MOME(&surtri,&surtra, sizeof(DBSurf));
      p_pattra = p_pattri;
      }

/*!                                                                 */
/*  Current surface to DB and draw surface.                         */
/*  Call of EXesur.                                                 */
/*                                                                 !*/


    status = EXesur(sur_id,&surtra,p_pattra,NULL,NULL,pnp);
    if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),"exe21 EXesur status %d\n",(short)status );
  fflush(dbgfil(EXEPAC));
}
#endif
      if( p_pattra != NULL )DBfree_patches(&surtra,p_pattra);
      sprintf(errbuf,"surexe%%EXsexdv");
      status = erpush("SU2993",errbuf);
      goto error2;
      }


/*!                                                                 */
/*  Deallocate memory for the current surface                       */
/*  Call of DBfree_patches.                                                 */
/*                                                                 !*/

    DBfree_patches(&surtra,p_pattra);

/*!                                                                 */
/* End   loop all surfaces                                          */
/*                                                                 !*/

    }   /* End loop i_sur  */





error2:; /* Label: varkon_vda_rsurm or EXesur failed */


    return(status);

  }
  
/********************************************************************/


