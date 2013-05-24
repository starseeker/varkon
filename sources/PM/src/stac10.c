/**********************************************************************
*
*    stac10.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    ST - Parameter list handler
*
*    stsrou()      Select routine
*    stgpar()      Get formal parameter data 
*    stgret()      Get formal function return type
*
*    Local routines
*
*    static void ps_expr()   Parse regular expression.
*    static void ps_prim()   Parse regular primary.
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
*    (C) Johan Kjellander, Örebro university
*
**********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../IG/include/isch.h"
#include "../include/mbsrout.h"


static void ps_expr();               /* Parse regular expression. */
static void ps_prim();               /* Parse regular primary. */


#define MAXDVA 4                     /* max numb. def.val. in reg.expr.     */

extern pm_ptr stintp,stflop,stvecp,  /* simple type pointers */
              ststrp,strefp,stfilp;
extern pm_ptr st_j_p,st_k_p,st_l_p,  /* array type pointers */
              st_m_p,st_n_p,st_o_p,
              st_p_p;
extern short  modtyp;                /* geometry type */

static bool optflg;                  /* TRUE -> optional parameter */

static char funtyp;                  /* function return type */
static char geotyp;                  /* routine geometric limitation */

static short parcnt;                 /* points to par_arr */

static  struct tag1                  /* parameter data vector */
           {
           char typc;                /* type */
           char clac;                /* class */
           char valc;                /* validity */
           short dfidx;              /* default index value = 0,1,.... */
           }  pararr[V2PARMAX+1];

/*
***OBS OBS OBS V2PARMAX+1 på ovanstående rad borde egentligen bara vara 
***V2PARMAX men det visade sig att parameterlistan till CUR_SPLINE
***där sista parametern är optionell och har defaultvärde samtidigt
***orsakar skrivning av ett element för mycket i pararr. Se V3MOME() i
***ps_prim() nedan. Varför det blir så är inte riktigt utrett men genom
***att reservera plats för ett extra element i pararr (V2PARMAX+1) är
***problemet ur värden. 1996-05024 J.Kjellander
*/

static short defcnt;                 /* number of default values in routine */
static PMLITVA defarr[MAXDVA];

static char *exppnt;                 /* points to reg. expr. */
static  char *exparr[] =             /* parameter regular expressions */
{
#include "../include/formpar.h"      /* include formal parameter file */
};

/********************************************************/
/*!******************************************************/

        void stsrou(
        pmroutn  rout) 

/*      Select routine.
 *
 *      In:    rout    =>  Routine code
 *
 *      (C)microform ab 1986----- Mats Nelson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
/*
***init
*/
   exppnt = exparr[rout];          /* points to reg. expr. */
   parcnt = 0;
   optflg = FALSE;
   defcnt = 0;
/*
***get function type and geom. lim.
*/
   funtyp = *exppnt;         
   exppnt++;
   geotyp = *exppnt;
   exppnt += 2;                    /* skip ':' */
/*
***expand regular expression
*/
   ps_expr();
/*
***Special treatement for Varkon.
*/
   switch(rout)
      {
      case VVEC:
      case VVECP:
         parcnt = modtyp;
         break;
      }

   return;
  }

/********************************************************/
/*!******************************************************/

        void stgpar(
        short     index,
        STPARDAT *par)

/*      Get routine parameter data.
 *
 *      In:    index  =>  1:st par = 1, 2:nd par. = 2 ...
 * 
 *      Out:   par    =>  Parameter data 
 *
 *      (C)microform ab 1986----- Mats Nelson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   if ((index > parcnt) || (index < 1))
      {
      par->valid = ST_ILL;
      return;
      }
/*
***type to access struct.
*/
   switch(pararr[index].typc)
       {
       case 'i' :                       /*  int */
          par->type = stintp;
          break;
       case 'f' :                       /*  float */
          par->type = stflop;
          break;
       case 'v' :                       /*  vector */
          par->type = stvecp;
          break;
       case 's' :                       /*  string */
          par->type = ststrp;
          break;
       case 'r' :                       /*  reference */
          par->type = strefp;
          break;
       case 'd' :                       /*  device/file */
          par->type = stfilp;
          break;
       case 'a' :                       /*  anytype */
          par->type = (pm_ptr)NULL;
          break;
       case 'j' :                       /*  4*4 float matrix */
          par->type = st_j_p;
          break;
       case 'k' :                       /* conf. array of 4*4 float matrix */
          par->type = st_k_p;
          break;
       case 'l' :                       /* conf. array of float */
          par->type = st_l_p;
          break;
       case 'm' :                       /* conf. array of refs. */
          par->type = st_m_p;
          break;
       case 'n' :                       /* conf. array of vector */
          par->type = st_n_p;
          break;
       case 'o' :                       /* conf. array of string */
          par->type = st_o_p;
          break;
       case 'p' :                       /* conf. array of int */
          par->type = st_p_p;
          break;
       }
/*
***class to access struct.
*/
   par->rvclass = pararr[index].clac;

   switch(pararr[index].clac)
      {
      case 'e' :                       /*  class expression */
         par->rvclass = ST_EXPCL;
         break;
      case 'v' :                       /*  class variable */
         par->rvclass = ST_VARCL;
         break;
      case 'd' :                       /*  class default */
         par->rvclass = ST_DEFCL;
         break;
      }
/*
***validity code to access struct
*/
   par->valid = pararr[index].valc;

/*
***default value to access struct
*/
   par->defval.lit_type = C_NO_VA;

   if (pararr[index].dfidx != -1)      /* then we have a default value */
      {
      V3MOME(&defarr[pararr[index].dfidx],&par->defval,sizeof(PMLITVA));
      }

   return;
  }

/********************************************************/
/*!******************************************************/

        void stgret(
        pm_ptr *rtype,
        pmvaty *ltype,
        short  *geol)

/*      Get function return type.
 *
 *      Out:  *rtype  =>   Func. type - NULL if procedure 
 *            *ltype  =>   Func. type - C_NO_VA if proc.type - C_NO_VA if proc. 
 *            *geol   =>   Func/proc geo type 2 - 3 - 5 
 *
 *      (C)microform ab 1986----- Mats Nelson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
/*
***convert and return type code
*/
   switch(funtyp)
      { 
      case 'i' :                       /* return type float */
        *rtype = stintp;
        *ltype = C_INT_VA;
        break;
      case 'f' :                       /* return type float */
        *rtype = stflop;
        *ltype = C_FLO_VA;
        break;
      case 'v' :                       /* return type vector */
        *rtype = stvecp;
        *ltype = C_VEC_VA;
        break;
      case 's' :                       /* return type string */
        *rtype = ststrp;
        *ltype = C_STR_VA;
        break;
      case 'r' :                       /* return type reference */
        *rtype = strefp;
        *ltype = C_REF_VA;
        break;
      default :
        *rtype = (pm_ptr)NULL;         /* return "no type" */
        *ltype = C_NO_VA;
        }
/*
***convert and return type code
*/
   *geol = geotyp - '0';

   return;
  }

/********************************************************/
/*!******************************************************/

        static void ps_expr()

/*      Parse regular expr.
 *
 *      (C)microform ab 1986----- Mats Nelson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   if (*exppnt != '\0')
      ps_prim();
   else
      return;
   while(*exppnt == '.')
      {
      exppnt++;
      ps_prim();
      }
   return;
  }

/********************************************************/
/*!******************************************************/

        static void ps_prim()

/*      Parse regular primary.
 *
 *      (C)microform ab 1986----- Mats Nelson
 *
 *      1999-11-19 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   int   count = 1;                        /* for count */
   int   tmp;                              /* For sscanf() */
   short p;

   if (isdigit(*exppnt))                   /* <count>'(' reg_expr ')' */
      {
      sscanf(exppnt,"%d",&count);          /* get count */
      while(*(exppnt++) != '(')            /* skip until '(' */
         ;
      optflg = TRUE;                       /* optional parameters */
      p = parcnt;
      ps_expr();                           /* parse regular expr. */
      p = parcnt - p;
      while(--count)                       /* copy in pararrÄÅ */
         {
         V3MOME(&pararr[parcnt-p+1],&pararr[parcnt+1],
         p*sizeof(struct tag1));
         parcnt += p;
         }
      exppnt++;                            /* skip ')' */
      }
   else                                    /* <type><class>  */
      {
      parcnt++;                            /* update parameter index  */
/*
***store type code
*/
      pararr[parcnt].typc = *exppnt;
      exppnt++;                            /* skip */
/*
***store class code
*/
      pararr[parcnt].clac = *exppnt;
      exppnt++;                            /* skip */
/*
***store validity code
*/
      if (optflg)
         pararr[parcnt].valc = ST_OPT;     /* optional parameter */
      else
         pararr[parcnt].valc = ST_FOR;     /* forced parameter  */
/*
***check for default value
*/
      if (*exppnt == '<')                  /* default value ? */
         {
         exppnt++;                         /* skip '<' */
         switch(pararr[parcnt].typc)
            {
            case 'i' :                     /* integer default */
                sscanf(exppnt,"%d",&tmp);
                defarr[defcnt].lit.int_va = tmp;
                defarr[defcnt].lit_type = C_INT_VA;
                break;
            case 'f' :                     /* float default */
                sscanf(exppnt,"%lf",&defarr[defcnt].lit.float_va);
                defarr[defcnt].lit_type = C_FLO_VA;
                break;
            case 'v' :                     /* vector default */
                sscanf(exppnt,"%lf,%lf,%lf",&defarr[defcnt].lit.vec_va.x_val,
                                            &defarr[defcnt].lit.vec_va.y_val,
                                            &defarr[defcnt].lit.vec_va.z_val);
                defarr[defcnt].lit_type = C_VEC_VA;
                break;
            case 's' :                     /* string default */
                sscanf(exppnt,"%[^>]",defarr[defcnt].lit.str_va);
                defarr[defcnt].lit_type = C_STR_VA;
                break;
                }
            pararr[parcnt].dfidx = defcnt++; /* store default value ref. */
            while(*(exppnt++) != '>')      /* skip until '>' */
                ;
            }
      else
         {
         pararr[parcnt].dfidx = -1;        /* store no default value ref */
         }
      }

   return;
  }

/********************************************************/
