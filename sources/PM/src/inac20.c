/**********************************************************************
*
*    inac20.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    short inwvar();    write MBS-variable
*    short ingpva();    get module parameter values
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

#ifdef DEBUG
#include "../../IG/include/debug.h"
#endif

/********************************************************/
/*!******************************************************/

        short inwvar(
        pm_ptr   acttyla,
        DBint    rtsaddr,
        short    dim,  
        DBint    indarr[],  
        PMLITVA  *valp)

/*      Write MBS-variable.
 *
 *      In:   acttyla   =>  Type pointer for actual parameter.
 *            rtsaddr   =>  Variable address in MBS run-time stack.  
 *            dim       =>  Dimension, number of indexes in array.  
 *                               Zereo indicates simple variable.
 *            indarr[]  =>  Array of indexes for an dimensioned variable, 
 *                               must be positive integers.
 *           *valp      =>  Value to be assigned to the MBS-variable.
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      Error codes:  IN340  =  Ilegal dimension for return variable 
 *                    IN341  =  Array index out of bound for return variable
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {

/*
***local declarations
*/
    STTYTBL typtbl;          /* type record */
    STARRTY arrtype;         /* array type record */
    DBint   arroffs = 0;     /* array offset */
    int     i;               /* loop variable */
    int     up_bound;        /* array index bound */
/*
***simple variable
*/
   if ( dim <= 0 )
      {      
      inpval( rtsaddr, acttyla, FALSE, valp );        /* put value on RTS */
      }
/*
***array variable
*/
   else 
      { 
      strtyp( acttyla, &typtbl );                     /* read type info. */

      for ( i = 0; i < dim; i++ )
         {
/*
***read array info
*/
         if ( typtbl.arr_ty != (pm_ptr)NULL )
             strarr( typtbl.arr_ty, &arrtype);
         else
             return( erpush( "IN3403", "" ) );        /* error */

/*
***read array component type ( next typtbl )
*/
         strtyp( arrtype.base_arr, &typtbl );
         up_bound = arrtype.up_arr - arrtype.low_arr + 1;
/*
***check array bounds
*/
         if ( ( indarr[ i ] < 1 ) || ( indarr[ i ] > up_bound ) )
             {  
             return( erpush( "IN3413", "" ) );        /* error */
             }
/*
***calculate array offset
*/
         arroffs = ( indarr[ i ] - 1 ) * typtbl.size_ty + arroffs;
         }

       if ( typtbl.kind_ty == ST_ARR )
          {  
          return( erpush( "IN3403", "" ) );           /* error not simple type, ilegal dimension */
          }
/*
***put value on RTS
*/
       inpval( rtsaddr + arroffs, arrtype.base_arr, FALSE, valp );

      }
   return( 0 );

  }

/********************************************************/
/*!******************************************************/

        short ingpva(
        pm_ptr    palist,
        char      valarr[], 
        PMPATLOG  typarr[],
        int      *par_nr) 

/*      Get module parameter values.
 *
 *      In:   palist    =>  PM-pointer to module parameter list.
 *            valarr[]  =>  Array for values .  
 *            typarr[]  =>  Array for type id and offset in value array. 
 *
 *      Out:  *par_nr   =>  Number of parameters. 
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *       
 *      1993-09-24 Genomgången och bug-rättad JK                         
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   PMPANO *np;        /* c-pointer to module parameter node */
   char *schp;        /* temporary char pointers */
   STVAR var;         /* interface struct for a parameter */
   DBint rtsstart;    /* start offset in RTS for first parameter */
   DBint valoffs;     /* offset in value array */
   PMLITVA val;
 
   v2int  *intp;
   v2float *floatp;
   V2VECVA *vecp;

   pm_ptr listla;     /* PM-pointer to list node */ 
   pm_ptr nextla;     /* PM-pointer to next list node */
   pm_ptr parala;     /* PM-pointer to module parameter node */
   short status;

/*
***Denna rutin används av evpart() för att från RTS hämta
***evaluerade värden för alla parametrar i ett PART-anrop.
***Metoden är att för varje parameter lagra typ och offset
***i en array av PMPATLOG typarr samt värden i valarr.
***typ, offset och värde hämtas ett och ett. Som offset
***i valarr väljs motsvarande offset från 1:a parametern
***i RTS. Det skulle alltså gå lika bra att bara beräkna
***start- och slut-adress i RTS och kopiera hela data-blocket
***med ett enda V3MOME()-anrop.
***VAR-deklarerade parametrar har bara en adress i RTS. Denna
***kopieras inte in i valarr eftersom den är meningslös  i detta
***sammanhang men 4 bytes reserveras i valarr så att eventuella
***efterföljande parametrar hamnar på rätt plats. Ursprungligen
***hade PO valt att sätta offset för en VAR-deklarerad parameter
***till -1 vilket gjorde det omöjligt för DBinsert_part() att beräkna
***storleken av valarr. Om sista parametern var en VAR-deklarerad
***parameter blev det fel i DBinsert_part().
*/

#ifdef DEBUG
    if ( dbglev(PMPAC) == 20 )
      {
      fprintf(dbgfil(PMPAC),"***Start-ingpva***\n");
      }
#endif


/*
***Initiering.
*/
   *par_nr = 0;
/*
***Finns det några parametrar ?
*/
    if ( palist == (pm_ptr)NULL ) return( 0 );
/*
***Ja, hämta data om 1:a parametern.
*/
    if ( (status=pmgfli(palist,&listla)) != 0 ) return(status);

    if ( listla != (pm_ptr)NULL )
      { 
      if ( (status=pmglin(listla,&nextla,&parala)) != 0 ) return(status);
      if ( (status=pmgpar(parala,&np)) != 0 ) return(status);
      if ( (status=strvar(np->fopa_,&var)) != 0 ) return(status);
/*
***1:a parameterns RTS-adress tas som start för datablocket
***i RTS.
*/
      rtsstart = var.addr_va;
      valoffs = 0;
/*
***Kopiera 1:a parameterns värde från RTS till valarr.
*/
      if ( var.kind_va != ST_RPAVA )
        {
        ingval(var.addr_va,var.type_va,FALSE,&val);
        switch ( val.lit_type )
          {
          case C_INT_VA:
          intp = (v2int *)&valarr[valoffs];
         *intp = val.lit.int_va;
          break;

          case C_FLO_VA:
          floatp = (v2float *)&valarr[valoffs];
         *floatp = val.lit.float_va;
          break;

          case C_STR_VA:
          strcpy(&valarr[valoffs],val.lit.str_va);
          break;

          case C_VEC_VA:
          vecp = (V2VECVA *)&valarr[valoffs];
          vecp->x_val = val.lit.vec_va.x_val;
          vecp->y_val = val.lit.vec_va.y_val;
          vecp->z_val = val.lit.vec_va.z_val;
          break;

          case C_REF_VA:
          schp = (char *)val.lit.ref_va;
          V3MOME(schp,&valarr[valoffs],v2refsz);
          break;
          }
        typarr[ *par_nr ].log_id = val.lit_type;
        typarr[ *par_nr ].log_offs = valoffs;
        }
/*
***VAR-deklarerad parameter. log_offs sätts här till valoffs.
***log_offs var ursprungligen = -1 men detta orsakar fel om
***den sista parametern i listan är VAR-deklarerad. DBinsert_part()
***beräknar totala mängden data i valarr genom att titta på
***sista parameterns log_offs och lägga till dess storlek.
***Det är bättre att låta valarr bli en kopia av motsvarande 
***data i RTS, dvs. även VAR-deklarerade parametrars adress
***(4) bytes tas med i beräkningen även om detta egentligen
***är meningslös information.
*/
      else
        {
        typarr[ *par_nr ].log_id = C_ADR_VA;
        typarr[ *par_nr ].log_offs = valoffs;
        }
      listla = nextla;
      (*par_nr) ++;
      }

#ifdef DEBUG
    if ( dbglev(PMPAC) == 20 )
      {
      fprintf(dbgfil(PMPAC),"Parameter nummer %d\n",*par_nr);
      fprintf(dbgfil(PMPAC),"Typ = %d\n",val.lit_type);
      fprintf(dbgfil(PMPAC),"kind = %d\n",var.kind_va);
      fprintf(dbgfil(PMPAC),"rtsstart = %d\n",var.addr_va);
      fprintf(dbgfil(PMPAC),"valoffs = %d\n",valoffs);
      fprintf(dbgfil(PMPAC),"log_offs = %d\n",typarr[*par_nr-1].log_offs);
      }
#endif
/*
***Resten av parametrarna.
*/
  while ( listla != (pm_ptr)NULL )
    { 
    if ( (status=pmglin(listla,&nextla,&parala)) != 0 ) return(status);
    if ( (status=pmgpar(parala,&np)) != 0 ) return(status);
    if ( (status=strvar(np->fopa_,&var)) != 0 ) return(status);
/*
***Beräkna var i valarr som värdet skall lagras.
*/
    valoffs = var.addr_va - rtsstart;
    if ( valoffs >= sizeof(GMDATA) ) return(erpush("IN2302",""));

    if ( var.kind_va != ST_RPAVA )
        {
        ingval( var.addr_va, var.type_va, FALSE, &val );
        switch ( val.lit_type )
          {
          case C_INT_VA:
          intp = (v2int *)&valarr[valoffs];
         *intp = val.lit.int_va;
          break;

          case C_FLO_VA:
          floatp = (v2float *)&valarr[valoffs];
         *floatp = val.lit.float_va;
          break;

          case C_STR_VA:
          strcpy(&valarr[valoffs],val.lit.str_va);
          break;

          case C_VEC_VA:
          vecp = (V2VECVA *)&valarr[ valoffs ];
          vecp->x_val = val.lit.vec_va.x_val;
          vecp->y_val = val.lit.vec_va.y_val;
          vecp->z_val = val.lit.vec_va.z_val;
          break;

          case C_REF_VA:
          schp = (char *)val.lit.ref_va;
          V3MOME(schp,&valarr[valoffs],v2refsz);
          break;
          }
        typarr[ *par_nr ].log_id = val.lit_type;
        typarr[ *par_nr ].log_offs = valoffs;
        }
    else
        {
        typarr[ *par_nr ].log_id = C_ADR_VA;
        typarr[ *par_nr ].log_offs = valoffs;
        }

    listla = nextla;

    (*par_nr) ++;
    if ( *par_nr > V2MPMX ) return(erpush("IN2312",""));

#ifdef DEBUG
    if ( dbglev(PMPAC) == 20 )
      {
      fprintf(dbgfil(PMPAC),"Parameter nummer %d\n",*par_nr);
      fprintf(dbgfil(PMPAC),"Typ = %d\n",val.lit_type);
      fprintf(dbgfil(PMPAC),"kind = %d\n",var.kind_va);
      fprintf(dbgfil(PMPAC),"rtsstart = %d\n",var.addr_va);
      fprintf(dbgfil(PMPAC),"valoffs = %d\n",valoffs);
      fprintf(dbgfil(PMPAC),"log_offs = %d\n",typarr[*par_nr-1].log_offs);
      }
#endif

    }

#ifdef DEBUG
    if ( dbglev(PMPAC) == 20 )
      {
      fprintf(dbgfil(PMPAC),"***Slut-ingpva***\n");
      }
#endif

    return(0);

  }

/********************************************************/
