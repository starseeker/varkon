/**********************************************************************
*
*    annaml.c
*    ========
*
*    This file is part of the VARKON Analyzer Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*    annaml();         Analyse named_params.
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
#include "../../PM/include/mbsrout.h"
#include "../include/AN.h"

extern struct ANSYREC sy;          /* scanner interface variable */

extern char *typstr[];             /* type strings */
extern pm_ptr stintp;              /* PM-pointer to integer type descr. */
extern short modtyp;               /* geometry type, for annaml() */

#define TABLEN 30                  /* parameter table entry points */

/*!******************************************************/

        void annaml(
        pm_ptr  *rptr,
        ANFSET  *follow, 
        pmroutn  proc)  

/*      Analyse named_params.
 *      named_params ::= named_param_def ä ',' named_param_def å
 *      named_param_def ::= Identifier '=' arith_expression
 *
 *      In:  *follow  =>  Follow set.
 *            proc    =>  Procedure code.
 *
 *      Out: *rptr    =>  Pointer to named parameter list.
 *
 *      (C)microform ab 1986-02-19 M. Nelson
 * 
 *      1999-04-20 Rewritten, R. Svedin
 *      2004-07-18 Mesh, J.Kjellander, Örebro university
 *      2007-01-17 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
   short i;                        /* loop */
   short slin,scol;                /* named parameter location */
   char eristr[32];                /* error insert strings */
   char tstr[32];
   char name[ANSYNLEN+1];          /* parameter name copy */
   pm_ptr valptr;                  /* PM-pointer to named parameter value */
   pm_ptr parptr;                  /* PM-pointer to named parameter node */
   pm_ptr dumptr;                  /* dummy */
   ANATTR vattr;                   /* value attributes */
   ANFSET locfol;                  /* local follow set */
   bool conflg = TRUE;             /* loop continue flag */
   pmmdid nampar = 0;              /* the named parameter */
   bool okflg = TRUE;              /* the named parameter fits the procedue */
   static struct namcod            /* parameter name/code table */
     {
     char *name;
     pmmdid code;
     sttycl typcl;
     } namtab[] = { {""       ,0        ,ST_UNDEF},
                       {"PEN"    ,PMPEN    ,ST_INT},
                       {"LEVEL"  ,PMLEVEL  ,ST_INT},
                       {"LFONT"  ,PMLFONT  ,ST_INT},
                       {"AFONT"  ,PMAFONT  ,ST_INT},
                       {"LDASHL" ,PMLDASHL ,ST_FLOAT},
                       {"ADASHL" ,PMADASHL ,ST_FLOAT},
                       {"TSIZE"  ,PMTSIZE  ,ST_FLOAT},
                       {"TWIDTH" ,PMTWIDTH ,ST_FLOAT},
                       {"TSLANT" ,PMTSLANT ,ST_FLOAT},
                       {"DTSIZ"  ,PMDTSIZE ,ST_FLOAT},
                       {"DASIZ"  ,PMDASIZE ,ST_FLOAT},
                       {"DNDIG"  ,PMDNDIG  ,ST_INT},
                       {"DAUTO"  ,PMDAUTO  ,ST_INT},
                       {"XFONT"  ,PMXFONT  ,ST_INT},
                       {"XDASHL" ,PMXDASHL ,ST_FLOAT},
                       {"BLANK"  ,PMBLANK  ,ST_INT},
                       {"HIT"    ,PMHIT    ,ST_INT},
                       {"SAVE"   ,PMSAVE   ,ST_INT},
                       {"CFONT"  ,PMCFONT  ,ST_INT},
                       {"CDASHL" ,PMCDASHL ,ST_FLOAT},
                       {"TFONT"  ,PMTFONT  ,ST_INT},
                       {"SFONT"  ,PMSFONT  ,ST_INT},
                       {"SDASHL" ,PMSDASHL ,ST_FLOAT},
                       {"NULINES",PMNULIN  ,ST_INT},
                       {"NVLINES",PMNVLIN  ,ST_INT},
                       {"WIDTH",  PMWIDTH  ,ST_FLOAT},
                       {"TPMODE", PMTPMODE ,ST_INT},
                       {"MFONT",  PMMFONT  ,ST_INT},
                       {"PFONT",  PMPFONT  ,ST_FLOAT},
                       {"PSIZE",  PMPSIZE  ,ST_INT}
                    };
/*
***Create local follow set
*/
   ancset(&locfol,follow,1,ANSYCOM,0,0,0,0,0,0,0);
/*
***Process the named parameters
*/
   *rptr = (pm_ptr)NULL;                  /* init. result */
   do
     {
/*
***Store parameter name and source location
*/
     slin=sy.sypos.srclin;
     scol=sy.sypos.srccol;

     if( sy.sytype == ANSYID )
       {
       strcpy(name,sy.syval.name);
       anascan(&sy);
       }
     else
       strcpy(name,"");
/*
***Check for '='
*/
     if(sy.sytype == ANSYEQ)
       anascan(&sy);
     else
       anperr("AN9172","=",NULL,sy.sypos.srclin,sy.sypos.srccol);
/*
***Analyse arith_expr
*/
     anarex(&valptr,&vattr,&locfol);
/*
****Look for the parameter in the information table
*/
     for( i=1; ( i <= TABLEN ) && sticmp(name, namtab[i].name); ++i )
       ;
/*
***Check if valid name
*/
     if( i > TABLEN )
       {
       anperr("AN9182",name,NULL,slin,scol);
       }

     else
/*
***Check value datatype
*/
       {
       nampar = namtab[i].code;
       if(namtab[i].typcl == angtcl(vattr.type))
         ;
       else if( (namtab[i].typcl == ST_FLOAT) && (angtcl(vattr.type) == ST_INT))
         ;
       else if(vattr.type == (pm_ptr)NULL)
         ;
       else 
         {
         angtps(vattr.type,tstr);
         sprintf(eristr,"%s\004%s",
         typstr[(int)namtab[i].typcl],tstr);
         anperr("AN9192",eristr,NULL,slin,scol);
         }
/*
***Check parameter/geometry
*/
       if ( modtyp == 3 )
         {
         switch ( nampar )
           {
           case PMPEN:
           case PMWIDTH:
           case PMLEVEL:
           case PMBLANK:
           case PMHIT:
           case PMSAVE:
           case PMPFONT:
           case PMLFONT:
           case PMAFONT:
           case PMCFONT:
           case PMSFONT:
           case PMNULIN:
           case PMNVLIN:
           case PMPSIZE:
           case PMLDASHL:
           case PMADASHL:
           case PMCDASHL:
           case PMSDASHL:
           case PMTFONT:
           case PMTSIZE:
           case PMTWIDTH:
           case PMTSLANT:
           case PMTPMODE:
           case PMMFONT:
           break;

           default:
           anperr("AN9402",name,NULL,slin,scol);
           break;
           }
         }
/*
***Check parameter/procedure
*/
       if ( (nampar != PMLEVEL) && (nampar != PMPEN) && 
            (nampar != PMBLANK) && (nampar != PMHIT) &&
            (nampar != PMSAVE))
         {
         switch(proc)
           {
#ifdef VARKON
/*
***Points.
*/
           case VPOI:
           case VPOIP:
             if ( (nampar != PMWIDTH) && (nampar != PMPFONT) &&
                  (nampar != PMPSIZE) ) okflg = FALSE;
           break;
/*
***Lines.
*/
           case VLINF:
           case VLINA:
           case VLINO:
           case VLINT1:
           case VLINT2:
             if ( (nampar != PMLFONT) && (nampar != PMLDASHL) &&
                  (nampar != PMWIDTH) ) okflg = FALSE;
           break;
/*
***Arc's
*/
           case VARCF:
           case VARC2P:
           case VARC3P:
           case VARCO:
           case VARCFI:
             if ( (nampar != PMAFONT) && (nampar != PMADASHL) &&
                  (nampar != PMWIDTH) ) okflg = FALSE;
           break;
/*
***Curves.
*/
           case VCURF:
           case VCURP:
           case VCURC:
           case VCOMP:
           case VCURS:
           case VCURO:
           case VCURT:
           case VCURU:
             if ( (nampar != PMCFONT) && (nampar != PMCDASHL) &&
                  (nampar != PMWIDTH)) okflg = FALSE;
           break;
/*
***Surfaces.
*/
           case VSUREX:  case VSURCON: case VSUROF:   case VSURSA:
           case VSURCA:  case VSURAP:  case VSURCO:   case VSURT:
           case VSURROT: case VSURUSD: case VSURCYL:  case VSURSWP:
           case VSURRUL: case VSURNA:  case VSURCURV: case VSURTUSRD:
             if ( (nampar != PMSFONT) && (nampar != PMSDASHL) &&
                  (nampar != PMNULIN) && (nampar != PMNVLIN)  &&
                  (nampar != PMWIDTH) ) okflg = FALSE;
           break;
/*
***Hatch.
*/
           case VXHT:
             if ( (nampar != PMXFONT) && (nampar != PMXDASHL) )
               okflg = FALSE;
           break;
/*
***Coordinate systems.
*/
           case VCSYS1P:
           case VCSYS3P:
           case VGROUP:
           case VSYMB:
             okflg = FALSE;
           break;
/*
***B_planes.
*/
           case VBPLAN:
             if ( nampar != PMWIDTH ) okflg = FALSE;
           break;
/*
***Text.
*/
           case VTEXT:
             if ( (nampar != PMTSIZE)  && (nampar != PMTWIDTH) &&
                  (nampar != PMTSLANT) && (nampar != PMTFONT)  &&
                  (nampar != PMWIDTH)  && (nampar != PMTPMODE) )
               okflg = FALSE;
           break;
/*
***Dimensions.
*/
           case VLDIM:
           case VCDIM:
           case VRDIM:
           case VADIM:
             if ( (nampar != PMDTSIZE) && (nampar != PMDASIZE) &&
                  (nampar != PMDNDIG) && (nampar != PMDAUTO) )
               okflg = FALSE;
           break;
/*
***Mesh.
*/
           case VMSHARR:
             if ( (nampar != PMMFONT) )
               okflg = FALSE;
           break;
#endif
           }
/*
***Error message.
*/
         if ( okflg == FALSE ) anperr("AN9412",name,NULL,slin,scol);
         }
       }
/*
***Update PM tree
*/
     if ( !anyerr() )
       {
       pmcnpa(namtab[i].code,valptr,&parptr);      /* create par. node */
       pmtcon(parptr,*rptr,rptr,&dumptr);          /* add node to par. list */
       }
/*
***Continue if ','
*/
     if ( sy.sytype == ANSYCOM ) anascan(&sy);
     else                        conflg = FALSE;
     }  
   while( conflg );
/*
***Ready, no ',' , return
*/
   return;
  }

/*********************************************************************/
