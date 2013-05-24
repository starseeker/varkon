/**********************************************************************
*
*    pmac4.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    Parameter scanning routine
*
*    pmrpap()   -> reset parameter adress pointer
*    pmgpad()   -> get next parameter adress 
*    pmrpar()   -> read parameter data 
*    pmupar()   -> update parameter data
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

static pm_ptr lnptr;    /* pointer to present parameter list node */

/********************************************************/
/*!******************************************************/

        short pmrpap(
        pm_ptr  modptr)

/*      Reset parameter adress pointer.
 *
 *      In:    modptr  =>  PM-pekare till modul
 *
 *      Out:   
 *
 *      FV:    return  -   status 
 *
 *      (C)microform ab 1986----- Mats Nelson
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

 {
   short status;          /* return status value */
   pm_ptr nlnptr;         /* PM-pointer to next list node */
   PMMONO *mnpnt;         /* C-pointer to module node */

/*
***C-pekare till modulen.
*/
   if((status=pmgmod(modptr,&mnpnt)) != 0) return(status);
/*
***Kolla att det var en modul.
*/
   if(mnpnt->monocl != MODULE)
      {
      printf("detta {r ingen modulnod\n");
      return(erpush("PM9092",""));
      }
/*
***Hämta PM-adress till parameterlistans TCONC-node.
*/
   nlnptr = mnpnt->ppali_;
/*
***Inga parametrar ?
*/
   if(nlnptr == (pm_ptr)NULL)
      {
      lnptr = (pm_ptr)NULL;
      return(0);
      }
/*
***PM-adress till första parametern.
*/
   if((status=pmgfli(nlnptr,&lnptr)) != 0) return(status);

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short pmgpad(
        pm_ptr  *panola)

/*      Get next parameter adress.
 *
 *      In:  
 *
 *      Out: *panola  =>  PM-pointer to parameter node
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986----- Mats Nelson
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   pm_ptr nlnptr;              /* PM-pointer to next list node */
   short status;               /* return status value */

/*
***End of list ?
*/
   if(lnptr == (pm_ptr)NULL)   
      {
      *panola=(pm_ptr)NULL;    /* yes ! */
      return(0);
      }
/*
***get par. node PM-adress
*/
   if((status=pmglin(lnptr,&nlnptr,panola)) < 0)   
      return(status);
/*
***update list node PM-pointer
*/
   lnptr=nlnptr;                              
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short pmrpar(
        pm_ptr   panola,
        char    *name,
        char    *prompt,
        PMLITVA *litval)

/*      Read parameter data.
 *
 *      In:   panola  =>  PM-pointer to module parameter node
 *
 *      Out: *name    =>  Parameter name string buffer
 *           *prompt  =>  Parameter prompt string buffer
 *           *litval  =>  Parameter default value buffer
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986----- Mats Nelson
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   char *ptstr;      /* pointer to prompt-string */
   char *nmstr;      /* pointer to name-string */
   PMPANO *np;       /* c-pointer to module parameter node */
   STVAR var;        /* interface struct for a parameter */
   PMEXNO *ep;       /* C-pointer to expr. node */
   PMLITEX *litp;    /* C-pointer to literal expr. node */
   PMVECVA *vecp;    /* C-pointer to vector value */
   short status;     /* return status */
   stidcl idclass;   /* identifier class */
   pmvaty type;      /* param. type - int, float .. */
   string str_val;   /* c-pointer to string in PM */       /* POA 851028 */

   if((status = pmgpar(panola,&np)) != 0)     /* get C-pointer to param. node */
      return(status);

   if(np->noclpa != PARAM)                    /* check node class */
      return(erpush("PM1111",""));            /* Not a module parameter node */

   if((status = pmgstr( np->ppro_,&ptstr)) != 0) /* get C-poi to prompt */
      return(status);

   strcpy(prompt,ptstr);                      /* copy name to callers buffer */

   if((status = stratt(np->fopa_,&idclass,&nmstr)) != 0) /* get par name */
      return(status);

   strcpy(name,nmstr);                        /* copy name to callers buffer */

   if((status = strvar(np->fopa_,&var)) != 0) /* get var. info */
      return(status);

   if((status = pmgexp(var.def_va,&ep)) != 0) /* get C-poi. to expr. node */
      return(status);

   litp= &(ep->ex_subcl.lit_ex);

   type=litp->lit_type;

   litval->lit_type=type;                     /* write type to interface PMLITVA */

   switch(type)                               /* write value to interface PMLITVA */
      {
      case C_INT_VA:
         litval->lit.int_va=litp->litex.int_li;      /* copy C_INT_VA value */
         break;
      case C_FLO_VA:
         litval->lit.float_va=litp->litex.float_li;  /* copy C_FLO_VA value */
         break;
      case C_STR_VA:
         pmgstr(litp->litex.str_li,&str_val);        /*copy C_STR_VA value*/
         strcpy(litval->lit.str_va, str_val);        /* POA 851028 */
         break;
      case C_VEC_VA:
         if((status = pmgvec(litp->litex.vec_li,&vecp)) != 0)
            return(status);
         litval->lit.vec_va.x_val=vecp->x_val;       /* copy C_VEC_VA value */
         litval->lit.vec_va.y_val=vecp->y_val;
         litval->lit.vec_va.z_val=vecp->z_val;
         break;
      case C_REF_VA:
         litval->lit.ref_va[0].seq_val = 0;          /* Returnera #0.1 JK */
         litval->lit.ref_va[0].ord_val = 1;
         litval->lit.ref_va[0].p_nextre = NULL;
         break;
      }    
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short pmupar(
        pm_ptr   panola,
        PMLITVA *litval)

/*      Update parameter data.
 *
 *      In:   panola  =>  PM-pointer to module parameter node
 *           *litval  =>  Parameter default value
 *
 *      Out: 
 *
 *      FV:    return -   error severity code 
 *
 *      (C)microform ab 1986----- Mats Nelson
 *
 *      1999-11-18 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
/*   char *ptstr;       /- pointer to prompt-string -/  */
   PMPANO *np;        /* c-pointer to module parameter node */
   STVAR var;         /* interface struct for a parameter */
   PMEXNO *ep;        /* C-pointer to expr. node */
   PMLITEX *litp;     /* C-pointer to literal expr. node */
   PMVECVA *vecp;     /* C-pointer to vector value */
   short status;      /* return status */
   pmvaty type;       /* param. type - int, float .. */

   if((status = pmgpar(panola,&np)) != 0)    /* get C-pointer to param. node */
      return(status);

   if(np->noclpa != PARAM)                   /* check node class */
      return(erpush("PM1111","pmrpar"));     /* Not a module parameter node */

/*
***update prompt string
*/

/*   if((status = pmgstr( np->ppro_,&ptstr)) != 0) /- get C-poi to prompt -/
      return(status);

   if(strcmp(ptstr,prompt) != 0)             /- replace prompt ? -/
      ä
      pmcstr(prompt,&ref);                   /- create PM-string -/
      np->ppro_=ref;                         /- update prompt referens -/
      å   
*/

/*
***update par. value
*/
   if((status = strvar(np->fopa_,&var)) != 0) /* get var. info */
      return(status);

   if((status = pmgexp(var.def_va,&ep)) != 0) /* get C-poi. to expr. node */
      return(status);

   litp= &(ep->ex_subcl.lit_ex);

   type=litp->lit_type;

/*
***check type
*/
   if(litval->lit_type != type)               
      return(erpush("PM1111","pmupar"));      /* attemt to change type */

/*
***select from type
*/
   switch(type)                     
      {
      case C_INT_VA:
         litp->litex.int_li=litval->lit.int_va;          /* update C_INT_VA value */
         break;
      case C_FLO_VA:
         litp->litex.float_li=litval->lit.float_va;      /* update C_FLO_VA value */
         break;
      case C_STR_VA:
         pmcstr(litval->lit.str_va,&litp->litex.str_li); /* new C_STR_VA value */
         break;                                          /* old string is now garbage */
      case C_VEC_VA:
         if((status = pmgvec(litp->litex.vec_li,&vecp)) != 0)
            return(status);
         vecp->x_val=litval->lit.vec_va.x_val;           /* update C_VEC_VA value */
         vecp->y_val=litval->lit.vec_va.y_val;
         vecp->z_val=litval->lit.vec_va.z_val;
         break;
      case C_REF_VA:
         printf("pmupar - no support for referens params.");
         break;
      }    
   return(0);
  }
/********************************************************/

