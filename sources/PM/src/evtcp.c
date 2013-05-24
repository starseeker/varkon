/**********************************************************************
*
*    evtcp.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    evtcco();      Evaluerar TCP_CONNECT
*    evtcli();      Evaluerar TCP_LISTEN
*    evtccl();      Evaluerar TCP_CLOSE
*    evtcoi();      Evaluerar TCP_OUTINT
*    evtcii();      Evaluerar TCP_ININT
*    evtcof();      Evaluerar TCP_OUTFLOAT
*    evtcif();      Evaluerar TCP_INFLOAT
*    evtcov();      Evaluerar TCP_OUTVEC
*    evtciv();      Evaluerar TCP_INVEC
*    evtcoc();      Evaluerar TCP_OUTCHAR
*    evtcic();      Evaluerar TCP_INCHAR
*    evtcgl();      Evaluerar TCP_GETLOCAL
*    evtcgr();      Evaluerar TCP_GETREMOTE
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
*    (C) 2003-01-27 Sören Larsson, Örebro University                             
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../EX/include/EX.h"
#include <string.h>

extern PMPARVA *func_pv;  /* Access structure for MBS routines */
extern short    func_pc;  /* Number of actual parameters */
extern PMLITVA *func_vp;  /* Pekare till resultat. */



/*!******************************************************/

        short evtcco()
 
/*      Interface-routine for TCP_CONNECT.
 *
 *      In:  Global  func_pv  => Parameter value array
 *           Global *func_vp  => Pointer to result value.
 *
 *      Out: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Return the status of called routines
 *
 *      (C) 2003-01-27 Sören Larsson, Örebro University
 *
 *
 ******************************************************!*/

   {
   short   status;
   PMLITVA litval;
   /*
   ***Execute EXtcpco(), retrieve socket id
   */
   status = EXtcpco(func_pv[1].par_va.lit.str_va,
                   func_pv[2].par_va.lit.int_va,
                   func_pv[3].par_va.lit.int_va,
                  &litval.lit.int_va, 
                  &func_vp->lit.int_va);
   
   if ( status < 0 ) return(status);
   /*
   ***Write to MBS-variable 'socket'.
   */
   return(inwvar(func_pv[4].par_ty,func_pv[4].par_va.lit.adr_va,0,NULL,&litval));
   }


/*!******************************************************/

        short evtcli()
 
/*      Interface-routine for TCP_LISTEN.
 *
 *      In:  Global  func_pv  => Parameter value array
 *           Global *func_vp  => Pointer to result value.
 *
 *      Out: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Return the status of called routines
 *
 *      (C) 2003-01-27 Sören Larsson, Örebro University
 *
 *
 ******************************************************!*/

   {
   short   status;
   PMLITVA litval;
   /*
   ***Execute EXtcpli(), retrieve socket id
   */
   status = EXtcpli(func_pv[1].par_va.lit.int_va,
                    func_pv[2].par_va.lit.float_va,
                   &litval.lit.int_va, 
                   &func_vp->lit.int_va);
   if ( status < 0 ) return(status);
   /*
   ***Write to MBS-variable 'socket'.
   */
   return(inwvar(func_pv[3].par_ty,func_pv[3].par_va.lit.adr_va,0,NULL,&litval));
   }



/*!******************************************************/

        short evtccl()
 
/*      Interface-routine for TCP_CLOSE.
 *
 *      In:  Global  func_pv  => Parameter value array
 *           Global *func_vp  => Pointer to result value.
 *
 *      Out: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Return the status of called routines
 *
 *      (C) 2003-01-27 Sören Larsson, Örebro University
 *
 *
 ******************************************************!*/
   {
   return(EXtcpcl(func_pv[1].par_va.lit.int_va,&func_vp->lit.int_va));
   }



/*!******************************************************/

        short evtcoi()
 
/*      Interface-routine for TCP_OUTINT.
 *
 *      In:  Global  func_pv  => Parameter value array
 *           Global *func_vp  => Pointer to result value.
 *
 *      Out: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Return the status of called routines
 *
 *      (C) 2003-01-27 Sören Larsson, Örebro University
 *
 *
 ******************************************************!*/
   {
    return (EXtcpoutint(func_pv[1].par_va.lit.int_va,    /* socket    */
                func_pv[2].par_va.lit.int_va,            /* bytecount */
                func_pv[3].par_va.lit.int_va,            /* byteorder */
                func_pv[4].par_va.lit.float_va,          /* timeout   */
                func_pv[5].par_va.lit.int_va,            /* data      */
               &func_vp->lit.int_va));   
   }



/*!******************************************************/

        short evtcii()
 
/*      Interface-routine for TCP_ININT.
 *
 *      In:  Global  func_pv  => Parameter value array
 *           Global *func_vp  => Pointer to result value.
 *
 *      Out: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Return the status of called routines
 *
 *      (C) 2003-01-27 Sören Larsson, Örebro University
 *
 *
 ******************************************************!*/
   {
   short   status;
   PMLITVA litval;
    
   status = (EXtcpinint(func_pv[1].par_va.lit.int_va,    /* socket    */
                func_pv[2].par_va.lit.int_va,            /* bytecount */
                func_pv[3].par_va.lit.int_va,            /* byteorder */
                func_pv[4].par_va.lit.float_va,          /* timeout   */
               &litval.lit.int_va,                       /* data      */
               &func_vp->lit.int_va));   
   if ( status < 0 ) return(status);
   /*
   ***Write to MBS-variable 'data'.
   */
   return(inwvar(func_pv[5].par_ty,func_pv[5].par_va.lit.adr_va,0,NULL,&litval));
   }



/*!******************************************************/

        short evtcof()
 
/*      Interface-routine for TCP_OUTFLOAT
 *
 *      In:  Global  func_pv  => Parameter value array
 *           Global *func_vp  => Pointer to result value.
 *
 *      Out: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Return the status of called routines
 *
 *      (C) 2003-01-30 Sören Larsson, Örebro University
 *
 *
 ******************************************************!*/

   {
    return (EXtcpoutfloat(func_pv[1].par_va.lit.int_va,   /* socket    */
                 func_pv[2].par_va.lit.int_va,            /* bytecount */
                 func_pv[3].par_va.lit.int_va,            /* byteorder */
                 func_pv[4].par_va.lit.float_va,          /* timeout   */
                 func_pv[5].par_va.lit.float_va,          /* data      */
                &func_vp->lit.int_va));   
   }


/*!******************************************************/

        short evtcif()
 
/*      Interface-routine for TCP_INFLOAT.
 *
 *      In:  Global  func_pv  => Parameter value array
 *           Global *func_vp  => Pointer to result value.
 *
 *      Out: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Return the status of called routines
 *
 *      (C) 2003-01-30 Sören Larsson, Örebro University
 *
 *
 ******************************************************!*/

   {
   short   status;
   PMLITVA litval;
    
   status = (EXtcpinfloat(func_pv[1].par_va.lit.int_va,  /* socket    */
                 func_pv[2].par_va.lit.int_va,            /* bytecount */
                 func_pv[3].par_va.lit.int_va,            /* byteorder */
                 func_pv[4].par_va.lit.float_va,          /* timeout   */
                &litval.lit.float_va,                     /* data      */
                &func_vp->lit.int_va));   
   
   if ( status < 0 ) return(status);
   
   /*
   ***Write to MBS-variable 'data'.
   */
   return(inwvar(func_pv[5].par_ty,func_pv[5].par_va.lit.adr_va,0,NULL,&litval));
   }


/*!******************************************************/

        short evtcov()
 
/*      Interface-routine for TCP_OUTVEC
 *
 *      In:  Global  func_pv  => Parameter value array
 *           Global *func_vp  => Pointer to result value.
 *
 *      Out: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Return the status of called routines
 *
 *      (C) 2003-01-30 Sören Larsson, Örebro University
 *
 *
 ******************************************************!*/
    {
    return (EXtcpoutvec(func_pv[1].par_va.lit.int_va,   /* socket    */
                 func_pv[2].par_va.lit.int_va,          /* bytecount */
                 func_pv[3].par_va.lit.int_va,          /* byteorder */
                 func_pv[4].par_va.lit.float_va,        /* timeout   */
                 func_pv[5].par_va.lit.vec_va.x_val,    /* data      */
                 func_pv[5].par_va.lit.vec_va.y_val,    /* data      */
                 func_pv[5].par_va.lit.vec_va.z_val,    /* data      */
                &func_vp->lit.int_va));   
   }



/*!******************************************************/

        short evtciv()
 
/*      Interface-routine for TCP_INVEC
 *
 *      In:  Global  func_pv  => Parameter value array
 *           Global *func_vp  => Pointer to result value.
 *
 *      Out: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Return the status of called routines
 *
 *      (C) 2003-01-30 Sören Larsson, Örebro University
 *
 *
 ******************************************************!*/
   {
   short   status;
   PMLITVA litval;
    
   status = (EXtcpinvec(func_pv[1].par_va.lit.int_va,  /* socket    */
                 func_pv[2].par_va.lit.int_va,         /* bytecount */
                 func_pv[3].par_va.lit.int_va,         /* byteorder */
                 func_pv[4].par_va.lit.float_va,       /* timeout   */
                &litval.lit.vec_va.x_val,              /* data      */
                &litval.lit.vec_va.y_val,              /* data      */
                &litval.lit.vec_va.z_val,              /* data      */
                &func_vp->lit.int_va));   
   
   if ( status < 0 ) return(status);
   
   /*
   ***Write to MBS-variable 'data'.
   */
   return(inwvar(func_pv[5].par_ty,func_pv[5].par_va.lit.adr_va,0,NULL,&litval));
   }


/*!******************************************************/

        short evtcoc()
 
/*      Interface-routine for TCP_OUTCHAR
 *
 *      In:  Global  func_pv  => Parameter value array
 *           Global *func_vp  => Pointer to result value.
 *
 *      Out: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Return the status of called routines
 *
 *      (C) 2003-01-30 Sören Larsson, Örebro University
 *
 *
 ******************************************************!*/
   {
    return (EXtcpoutchar(func_pv[1].par_va.lit.int_va,   /* socket    */
                 func_pv[2].par_va.lit.float_va,         /* timeout   */
                 func_pv[3].par_va.lit.str_va,           /* data      */
                &func_vp->lit.int_va));   
   }



/*!******************************************************/

        short evtcic()
 
/*      Interface-routine for TCP_INCHAR.
 *
 *      In:  Global  func_pv  => Parameter value array
 *           Global *func_vp  => Pointer to result value.
 *
 *      Out: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Return the status of called routines
 *
 *      (C) 2003-01-30 Sören Larsson, Örebro University
 *
 *
 ******************************************************!*/
   {
   short   status;
   PMLITVA litval;
    
   status = (EXtcpinchar(func_pv[1].par_va.lit.int_va,  /* socket    */
                 func_pv[2].par_va.lit.float_va,          /* timeout   */
                 litval.lit.str_va,                     /* data      */
                &func_vp->lit.int_va));   
   
   if ( status < 0 ) return(status);
   
   /*
   ***Write to MBS-variable 'data'.
   */
   return(inwvar(func_pv[3].par_ty,func_pv[3].par_va.lit.adr_va,0,NULL,&litval));
   }



/*!******************************************************/

        short evtcgl()
 
/*      Interface-routine for TCP_GETLOCAL.
 *
 *      In:  Global  func_pv  => Parameter value array
 *           Global *func_vp  => Pointer to result value.
 *
 *      Out: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Return the status of called routines
 *
 *      (C) 2003-01-30 Sören Larsson, Örebro University
 *
 *
 ******************************************************!*/
   {
   short   status;
   PMLITVA litval2,litval3;
   status = (EXtcpgetlocal(func_pv[1].par_va.lit.int_va,    /* socket */
                          litval2.lit.str_va,              /* name   */
                          &litval3.lit.int_va,              /* port   */
                          &func_vp->lit.int_va));   
   if ( status < 0 ) return(status);
   /*
   ***Write to MBS-variable 'name' and 'port'.
   */
   status = (inwvar(func_pv[2].par_ty,func_pv[2].par_va.lit.adr_va,0,NULL,&litval2));
   if ( status < 0 ) return(status);
   return(inwvar(func_pv[3].par_ty,func_pv[3].par_va.lit.adr_va,0,NULL,&litval3));
   }



/*!******************************************************/

        short evtcgr()
 
/*      Interface-routine for TCP_GETREMOTE.
 *
 *      In:  Global  func_pv  => Parameter value array
 *           Global *func_vp  => Pointer to result value.
 *
 *      Out: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Return the status of called routines
 *
 *      (C) 2003-01-30 Sören Larsson, Örebro University
 *
 *
 ******************************************************!*/
   {
   short   status;
   PMLITVA litval2,litval3;
  
   status = (EXtcpgetremote(func_pv[1].par_va.lit.int_va,    /* socket */
                           litval2.lit.str_va,               /* name   */
                           &litval3.lit.int_va,              /* port   */
                           &func_vp->lit.int_va));   
   if ( status < 0 ) return(status);
   /*
   ***Write to MBS-variable 'name' and 'port'.
   */
   status = (inwvar(func_pv[2].par_ty,func_pv[2].par_va.lit.adr_va,0,NULL,&litval2));
   if ( status < 0 ) return(status);
   return(inwvar(func_pv[3].par_ty,func_pv[3].par_va.lit.adr_va,0,NULL,&litval3));
   }
