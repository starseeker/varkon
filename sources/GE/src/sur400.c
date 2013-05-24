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

#define MXSCOMP   25  /* Max number of surfaces SUR_COMP Temp. V3.h */

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_sur_comp                      File: sur400.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  Creates a composite surface.                                    */
/*                                                                  */
/*  The function corresponds to MBS function SUR_COMP.              */
/*                                                                  */
/*  Memory will be allocated for the output composite surface.      */
/*                                                                  */
/*  Author: Gunnar Liden                                            */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1996-05-12   Originally written                                 */
/*  1996-11-13   compilation warnings ...                           */
/*  1997-03-02   MXSCOMP definition erased                          */
/*  1999-12-14   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_comp       Composite surface (SUR_COMP)     */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_ctol           * Coordinate tolerance                     */
/* varkon_ntol           * Surface normal tolerance                 */
/* varkon_comptol        * Retrieve computer tolerance              */
/* varkon_idpoint        * Identical points criterion               */
/* varkon_idangle        * Identical angles criterion               */
/* varkon_sur_combtwo    * Combine two surfaces                     */
/* varkon_erpush         * Error message to terminal                */
/* varkon_erinit         * Initialize error message stack           */
/* DBfree_patches        * Deallocate memory                        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU4003 = Maximum number of SUR_COMP surfaces is 2       (sur400) */
/* SU4013 = Minimum number of SUR_COMP surfaces is 2       (sur400) */
/* SU2943 = Called function .. failed in varkon_sur_comp   (sur400) */
/* SU2993 = Severe program error (  ) in varkon_sur_comp   (sur400) */
/*                                                                  */
/*-----------------------------------------------------------------!*/


/*!****************** Function **************************************/
/*                                                                  */
       DBstatus   varkon_sur_comp (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBSurf   sur_all[],    /* All input surfaces                      */
  DBPatch *p_pat_all[],  /* All topological patch data        (ptr) */
  DBint    no_surf,      /* Number of input surfaces                */
  DBint    c_case,       /* Composite surface case (not yet used)   */
  DBSurf  *p_surout,     /* Output surface                    (ptr) */
/*                                                                  */
/* Out:                                                             */
/*                                                                  */
  DBPatch **pp_patout )  /* Alloc. area for topol. patch data (ptr) */
                         /* (memory is allocated in this fctn)      */
/*                                                                  */
/*                                                                  */
/*-----------------------------------------------------------------!*/


{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
   SURCOMP  scomp;       /* Surface composite calculation data      */
   DBSurf  *p_sur_1;     /* Current surface 1                 (ptr) */
   DBPatch *p_pat_1;     /* Current surface 1 patch data      (ptr) */
   DBSurf  *p_sur_2;     /* Current surface 2                 (ptr) */
   DBPatch *p_pat_2;     /* Current surface 2 patch data      (ptr) */
   DBSurf   sur_two;     /* Two surfaces combined                   */
   DBSurf   sur_iter;    /* Equal to sur_two, since sur_two not can */
                         /* be input and output to sur410. The out- */
                         /* put argument is initialized in sur410   */
   DBPatch *p_pat_two;   /* Two surfaces combined patch data  (ptr) */
/*                                                                  */
/*-----------------------------------------------------------------!*/

   DBint    i_sur;       /* Loop index corresponding to surface     */
   DBfloat  ntol;        /* Surface normal tolerance                */
   DBfloat  ctol;        /* Coordinate tolerance                    */
   DBfloat  comptol;     /* Computer   tolerance                    */
   DBfloat  idpoint;     /* Identical point criterion               */
   DBfloat  idangle;     /* Identical angle criterion               */
   DBint    status;      /* Error code from called function         */
   char     errbuf[80];  /* String for error message fctn erpush    */

#ifdef  DEBUG
   DBint    i_sur2;      /* Loop index corresponding to surface     */
#endif
/*--------------end-of-declarations---------------------------------*/


/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initializations                       */
/* __________________________________________                       */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur400 Enter*varkon_sur_comp no_surf %d Max. number %d (c_case %d)\n",
          (int)no_surf, MXSCOMP, (int)c_case );
fflush(dbgfil(SURPAC)); /* From buffer to file */
}
#endif
 
/*!                                                                 */
/* Initialize pp_patout to NULL. Variable pp_patout is also a flag  */
/* to the calling function. Memory must be deallocated if           */
/* pp_patout not is equal to NULL (also for errors).                */
/*                                                                 !*/

*pp_patout = NULL;                     /* Initialize output pointer */

/*!                                                                 */
/* The maximum number of surfaces is MXSCOMP (defined in V3.h)      */
/* The minimum number of surfaces is one (1).                       */
/*                                                                 !*/

   if       ( no_surf <=    1    )
     {
     varkon_erinit();
     sprintf(errbuf,"%d%%sur400",(int)no_surf);
     return(varkon_erpush("SU4013",errbuf));
     }
   else if  ( no_surf >  MXSCOMP )
     {
     varkon_erinit();
     sprintf(errbuf,"%d%%sur400", MXSCOMP );
     return(varkon_erpush("SU4003",errbuf));
     }

/*!                                                                 */
/* Check if any of the surfaces are identical for Debug On          */
/* The check shall (must) be done in the calling routine            */
/*                                                                 !*/

#ifdef  DEBUG
for ( i_sur=1; i_sur <= no_surf; ++i_sur )
  {
  p_sur_1 =   &sur_all[i_sur-1]; 
  p_pat_1 =  p_pat_all[i_sur-1];
  for ( i_sur2=1; i_sur2 <= no_surf; ++i_sur2 )
    {
    p_sur_2 =   &sur_all[i_sur2-1]; 
    p_pat_2 =  p_pat_all[i_sur2-1];
    if   (  i_sur == i_sur2 ) goto nxtsur;
    if ( p_sur_1 == p_sur_2 || p_pat_1 == p_pat_2 )
     {
     varkon_erinit();
     sprintf(errbuf,"%d%%%d ",(int)i_sur, (int)i_sur2);
     return(varkon_erpush("SU4033",errbuf));
     }

nxtsur:;
    }
  }
#endif

/*!                                                                 */
/* Retrieve tolerances for the calculation. Call of                 */
/* varkon_ctol (sur751), _ntol (sur752), _comptol (sur753),         */
/* varkon_idpoint (sur741) and varkon_idangle (sur742)              */
/*                                                                 !*/

   ntol      = varkon_ntol();
   ctol      = varkon_ctol();
   comptol   = varkon_comptol();
   idpoint   = varkon_idpoint();
   idangle   = varkon_idangle();

/*!                                                                 */
/* Composite calculation data to SURCOMP.                           */
/*                                                                 !*/

   scomp.ctype   = I_UNDEF;       /* Type of calculation            */
   scomp.method  = I_UNDEF;       /* Method to be used              */
   scomp.nstart  = I_UNDEF;       /* Number of restarts             */
   scomp.maxiter = I_UNDEF;       /* Maximum number of restarts     */

   scomp.ctol    = ctol;          /* Coordinate     tolerance       */
   scomp.ntol    = ntol;          /* Surface normal tolerance       */
   scomp.idpoint = idpoint;       /* Identical point criterion      */
   scomp.idangle = idangle;       /* Identical angle criterion      */
   scomp.comptol = comptol;       /* Computer accuracy tolerance    */

   scomp.d_che   = F_UNDEF;       /* Maximum check R*3 distance     */
   scomp.che_add = 2;             /* No. of added points for check  */

   scomp.nu1      =  I_UNDEF;      /* Number of U patches surface 1  */
   scomp.nv1      =  I_UNDEF;      /* Number of U patches surface 1  */
   scomp.nu2      =  I_UNDEF;      /* Number of V patches surface 2  */
   scomp.nv2      =  I_UNDEF;      /* Number of V patches surface 2  */
   scomp.surtype1 =  I_UNDEF;      /* Surface type        surface 1  */
   scomp.surtype2 =  I_UNDEF;      /* Surface type        surface 2  */

   scomp.equal_case= I_UNDEF;      /* Equal     boundary case        */
   scomp.comp_case=  I_UNDEF;      /* Composite boundary case        */

   scomp.s_p_sur1 =  F_UNDEF;      /* Start parameter surface 1      */
   scomp.e_p_sur1 =  F_UNDEF;      /* End   parameter surface 1      */
   scomp.s_p_sur2 =  F_UNDEF;      /* Start parameter surface 2      */
   scomp.e_p_sur2 =  F_UNDEF;      /* End   parameter surface 2      */


/*!New-Page--------------------------------------------------------!*/
/*!                                                                !*/
/*!                                                                !*/

/*!                                                                 */
/* 3. Create the composite surface                                  */
/* ____________________________                                     */
/*                                                                  */
/* Loop all surfaces i_sur= 1,....,no_surf                          */
/*                                                                 !*/

for ( i_sur=1; i_sur < no_surf; ++i_sur )  /* Start loop surfaces   */
  {

/*!                                                                 */
/*   Data (pointers) to current surface 1 and 2                     */
/*                                                                  */
/*                                                                 !*/

     if  ( i_sur == 1 )
       {
       p_sur_1 =   &sur_all[i_sur-1]; 
       p_pat_1 =  p_pat_all[i_sur-1];
       }
     else
       {
       V3MOME((char *)(&sur_two),(char *)(&sur_iter),sizeof(DBSurf));
       p_sur_1 =   &sur_iter; 
       p_pat_1 =  p_pat_two;
       }

     p_sur_2 =   &sur_all[i_sur  ]; 
     p_pat_2 =  p_pat_all[i_sur  ];

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),"sur400 p_sur_1 %d p_pat_1 %d i_sur %d\n",
                        (int)p_sur_1, (int)p_pat_1, (int)i_sur );
fprintf(dbgfil(SURPAC),"sur400 p_sur_2 %d p_pat_2 %d \n",
                        (int)p_sur_2, (int)p_pat_2 );
}
#endif

      status = varkon_sur_comptwo
    (p_sur_1,p_pat_1,p_sur_2,p_pat_2,&scomp,&sur_two,&p_pat_two);
     if           ( status < 0 && p_pat_two == NULL ) 
       {
       sprintf(errbuf,"varkon_sur_comptwo(sur410)%%sur400" );
       return(varkon_erpush("SU2943",errbuf));
       }
     else if      ( status < 0  ) 
       {
       V3MOME((char *)(&sur_two),(char *)(p_surout),sizeof(DBSurf));
       *pp_patout = p_pat_two;        /* Allocated area for patches */
       sprintf(errbuf,"varkon_sur_comptwo(sur410)%%sur400" );
       return(varkon_erpush("SU2943",errbuf));
       }

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 && i_sur > 1 )
{
fprintf(dbgfil(SURPAC),
"sur400 Deallocate memory for p_sur_1 %d p_pat_1 %d (Loop= i_sur %d)\n",
                        (int)p_sur_1, (int)p_pat_1, (int)i_sur );
}
#endif

   if ( i_sur >  1 ) DBfree_patches(p_sur_1,p_pat_1);

  }                                      /* End   loop surfaces     */
/*!                                                                 */
/* End  all surfaces i_sur= 1,....,no_surf                          */
/*                                                                 !*/

/*!                                                                 */
/* 4. Exit                                                          */
/* ________                                                         */
/*                                                                  */
/* Surface header data and pointer to allocated area                */
/*                                                                 !*/

   V3MOME((char *)(&sur_two),(char *)(p_surout),sizeof(DBSurf));
   *pp_patout = p_pat_two;            /* Allocated area for patches */
   

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur400 Exit ****** varkon_sur_comp    ********\n");
fflush(dbgfil(SURPAC)); 
}
#endif


    return(SUCCED);

  }

/*********************************************************/
