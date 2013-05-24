/********************************************************************
 *  igpart.c
 *  ========
 *
 *  This file includes:
 *
 *  IGpart();        Create and run PART statement (generic mode)
 *  IGrun_named();   Create a part (explicit mode, no statement)
 *  IGmacro();       Run a macro
 *  IGcall_part();   Call a part
 *  IGcall_macro();  Call a macro
 *  IGuppt();        Uppdate Part
 *  IGcptw();        Edit part call parameters
 *  IGedit_MBS();    Edit MBS program in lib directory
 *  IGcompile_all(); Compile all MBS programs in lib directory
 *  IGgnps();        Build part call
 *
 *  This file is part of the VARKON IG Library.
 *  URL:  http://varkon.sourceforge.net
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later
 *  version.
 *
 *  This library is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU Library General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU Library
 *  Public License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 675 Mass Ave, Cambridge,
 *  MA 02139, USA.
 *
 ********************************************************************/

#include "../../DB/include/DB.h"
#include "../../DB/include/DBintern.h"
#include "../include/IG.h"
#include "../../AN/include/AN.h"
#include "../../EX/include/EX.h"
#include "../../WP/include/WP.h"

extern pm_ptr  actmod;
extern short   sysmode,modtyp;
extern int     posmode,actfunc;
extern char    jobdir[],jobnam[],actpnm[];
extern bool    tmpref,iggflg;
extern V2NAPA  defnap;
extern struct  ANSYREC sy;

/*
***The current directory for part selection.
*/
static int part_dir = 1;

/*
***Prototypes for internal functions.
*/
static bool  optional_parameter(char *prompt);
static bool  menu_parameter(char *prompt, short *mnum);
static bool  posalt_parameter(char *prompt, int *posalt);
static bool  typemask_parameter(char *prompt, DBetype *typmsk);
static void  file_parameter(char *prompt, PMLITVA *defval);
static void  t_string_prompt(char *prompt);
static short get_partname(char *name);

/********************************************************/

       short IGpart()

/*      Interactive create (and run) PART statement.
 *
 *      Return:    0 = OK
 *            REJECT = Operation rejected
 *            GOMAIN = Back to main menu
 *
 *      (C)2007-04-27 J.Kjellander
 *
 ******************************************************!*/

  {
    char    filnam[JNLGTH+1];
    short   status;

/*
***Get the name of the module to call.
*/
    if ( (status=get_partname(filnam)) < 0 ) return(status);
/*
***Create the part statement.
*/
    if ( (status=IGcall_part(filnam,PART)) == REJECT || status == GOMAIN ) goto end;
    else if ( status < 0 ) errmes();
/*
***The end.
*/
end:
    WPerhg();
    return(status);
  }

/********************************************************/
/********************************************************/

       short IGrun_named()

/*      Interactive function for executing a module.
 *      No statement is added to the active module
 *      but any result created, is saved in DB.
 *
 *      Return:    0 = OK
 *            REJECT = Operation rejected
 *            GOMAIN = Back to main menu
 *
 *      (C)2007-04-27 J.Kjellander
 *
 ******************************************************!*/

  {
    char    filnam[JNLGTH+1];
    short   status;

/*
***Get the name of the module to call.
*/
    if ( (status=get_partname(filnam)) < 0 ) return(status);
/*
***Execute the module in RUN mode.
*/
    if ( (status=IGcall_part(filnam,RUN)) == REJECT || status == GOMAIN ) goto end;
    else if ( status < 0 ) errmes();
/*
***The end.
*/
end:
    WPerhg();
    return(status);
  }

/********************************************************/
/********************************************************/

       short IGmacro()

/*      Interactive function for calling a macro.
 *
 *      Return: 0      = OK
 *              REJECT = Operation rejected
 *              GOMAIN = Back to main menu
 *
 *      (C)2007-04-27 J. Kjellander
 *
 ******************************************************!*/

  {
    char    filnam[JNLGTH+1];
    short   status;
    static  char dstr[JNLGTH+1] = "";

/*
***Get name of macro module.
*/
    if ( (status=IGssip(IGgtts(167),IGgtts(267),filnam,dstr,JNLGTH)) < 0 ) goto end;
    strcpy(dstr,filnam);
/*
***Create a PART statement an execute in MACRO mode.
*/
    if ((status=IGcall_macro(filnam)) == REJECT || status == GOMAIN) goto end;
    else if ( status < 0 ) errmes();
/*
***The end.
*/
end:
    WPclear_mcwin();
    WPerhg();
    return(status);
  }

/********************************************************/
/********************************************************/

       short IGcall_part(
       char *part_name,
       short atyp)

/*      Prompts the user for the module parameters, 
 *      creates a PART statement and executes the module.
 *      In RUN mode the statement is deleted after the
 *      module is executed.
 *
 *      In: filnam => Name of MBO-file.
 *          atyp   => Execution mode, PART or RUN
 *
 *      Return: 0 = OK.
 *              REJECT = Operation rejected.
 *              GOMAIN = Back to main menu.
 *
 *      Error:  IG5272   => Can't load module %s
 *              IG3982   => Syntax error parameter value
 *              IG5023   => Can't create PART statement
 *              IG5222   => Error executing PART statement
 *              IG5043   => Error linking statement to module
 *              IG5352   => Module is a MACRO
 *
 *      (C)2007-04-27 J. Kjellander
 *
 ******************************************************!*/

  {
    pm_ptr   csys_ptr;            /* PM ptr to csys for LOCAL module */
    pm_ptr   param_list;          /* The parameter list */
    pm_ptr   retla;               /* The PART statement */
    pm_ptr   oldmod;              /* Base adress of caller */
    pm_ptr   newmod;              /* Base adress of called module */
    pm_ptr   expr;                /* For IGcpos() and IGcref() */
    pm_ptr   dummy;               /* For data not used */
    short    status;              /* Return status */
    DBetype csymask;             /* For IGcref() */
    bool     end,right;           /* For IGcref() */
    pm_ptr   panola;              /* PM-pointer to param. node */
    char     prompt[V3STRLEN];    /* Parameter prompt string */
    PMLITVA  defval;              /* Parameter default value */
    PMLITVA  actval;              /* Parameter actual value */
    PMMONO  *mnpnt;               /* C pointer to module node */
    PMPANO  *np;                  /* C pointer to parameter node */
    short    oldafu;              /* Currently active function */
    bool     oldtrf;              /* Currently active tmpref */
    int      oldhit;              /* Current HIT */
    int      old_posmode;         /* Global posmode */
    short    prtid;               /* Part ID sequence number */
    PMREFVA  prtref;              /* Part ID */
    pm_ptr   ref,arglst;          /* For GLOABL_REF */
    stidcl   kind;                /* For GLOABL_REF */
    int      i;                   /* Loop variable */
    STVAR    var;                 /* Interface struct for a parameter */
    char     errbuf[V3STRLEN];    /* Error message data */
    ANFSET   set;                 /* For the analyser */
    ANATTR   attr;                /* For the analyzer */
    char     title[81];           /* WPmsip() window title */
    char     mesbuf[V3STRLEN];    /* Message buffer for WPaddmess.. */

/*
***Each npars parameter has a name, type, default value and prompt.
***VECTOR parameters have pos_modes and REF parameters have type_masks.
***Each parameter can be hidden (no prompt string) or visible.
***Each parameter after processing gets a PM pointer (expnode_ptrs).
*/
    int      npars;
    char     parameter_names[V2MPMX][V3STRLEN];
    int      parameter_types[V2MPMX];
    char     parameter_defvals[V2MPMX][V3STRLEN];
    char     parameter_prompts[V2MPMX][V3STRLEN];
    int      pos_modes[V2MPMX];
    DBetype type_masks[V2MPMX];
    bool     hidden[V2MPMX];
    pm_ptr   expnode_ptrs[V2MPMX];
/*
***Each nvisible parameter has a name, type, default value and prompt.
***WPmsip() also wants a max length for each parameter.
***pos_modes and type_masks are updated before input to reflect
***only visible parameters.
*/
    int      nvisible;
    char     input_names[V2MPMX][V3STRLEN];
    int      input_types[V2MPMX];
    char    *input_defvals[V2MPMX];
    char    *input_prompts[V2MPMX];
    short    input_maxlengths[V2MPMX];
/*
***WPmsip() returns input strings and want's ptrs. to them as input.
*/
    char     input_strings[V2MPMX][V3STRLEN];
    char    *input_ptrs[V2MPMX];

/*
***Texts from the ini-file.
*/
    if ( !WPgrst("varkon.part.title",title) ) strcpy(title,"Parameters for");

    strcat(title," ");
    strcat(title,part_name);
/*
***Initializations.
*/
    oldtrf = tmpref;
    oldhit = defnap.hit;
/*
***If this is a PART call in RUN mode, set tmpref = TRUE
***and HIT = 0.
*/
    if ( atyp == RUN )
      {
      tmpref = TRUE;
      defnap.hit = 0;
      }
/*
***Save the number of the currently active function
***and repalce it with -2 (part call). Set the active
***part name to filnam so the help system can list the
***right help file.
*/
    oldafu = actfunc;
    actfunc = -2;
    strcpy(actpnm,part_name);
/*
***Remember current position on the PM stack.
*/
    pmmark();
/*
***Load module to PM.
*/
    oldmod = pmgbla();
    if ( pmgeba(part_name,&newmod) != 0 ) 
      {
      status = erpush("IG5272",part_name);
      goto exit;
      }
/*
***Read module header.
*/
    pmsbla(newmod);                 /* Set base adress to new module */
    pmgmod((pm_ptr)0,&mnpnt);       /* Get c-pointer to module node */
    pmsbla(oldmod);                 /* Set base adress back to old module */
/*
***A MACRO module may not be used as a part.
*/
    if ( (mnpnt->moat_ == MACRO) )
      {
      status = erpush("IG5352",part_name);
      goto exit;
      }
/*
***A module with the LOCAL attribute needs a ref to a csys.
*/
    csys_ptr = (pm_ptr)NULL;

    if ( mnpnt->moat_ == LOCAL )
        {
        csymask = CSYTYP;
        if ( (status=IGcref(271,&csymask,&expr,&end,&right)) < 0 ) goto reject;
        pmtcon(expr,(pm_ptr)NULL,&csys_ptr,&dummy);
        }
/*
***Loop through the parameter list and fix prompt
***strings and default values.
*/
    pmsbla(newmod);
    pmrpap((pm_ptr)0);
    pmgpad(&panola);
    npars = 0;

    while ( panola != (pm_ptr)NULL )
      {
/*
***Get parameter name, prompt and default value literal.
*/
      pmrpar(panola,parameter_names[npars],prompt,&defval);
/*
***First check is to see if this is a hidden parameter
***with default value from file ie. "@fpath" or "@fpath(n)"
***and then nothing more.
*/
      file_parameter(prompt,&defval);
/*
***If there still is a prompt, check for the following and in that order:
***Position alternative:   "@an"
***Typemask;               "@tn"
***File parameter again:  "@fpath" or "@fpath(n)"
***t-string prompt:        "tn"
*/
      if ( strlen(prompt) > 0 )
        {
        if ( defval.lit_type == C_VEC_VA  &&  !posalt_parameter(prompt,&pos_modes[npars]) )
          pos_modes[npars] = -1;
        if ( defval.lit_type == C_REF_VA  &&  !typemask_parameter(prompt,&type_masks[npars]) )
          type_masks[npars] = ALLTYP;
        file_parameter(prompt,&defval);
        t_string_prompt(prompt);
        }
      strcpy(parameter_prompts[npars],prompt);
/*
***Does the parameter still have a prompt or is it a hidden parameter
***that should not be displayed to the user ?
*/
      if ( strlen(prompt) == 0 ) hidden[npars] = TRUE;
      else                       hidden[npars] = FALSE;
/*
***Parameter type.
*/
      parameter_types[npars] = defval.lit_type;
/*
***Default value.
*/
      pmgpar(panola,&np);
      strvar(np->fopa_,&var);
      pprexs(var.def_va,modtyp,parameter_defvals[npars],V3STRLEN);
/*
***Next parameter.
*/
      pmgpad(&panola);
    ++npars;
      }

    pmsbla(oldmod);
/*
***Copy parameter data to input data.
***Don't display hidden parameters.
*/
    for ( i=0,nvisible=0; i<npars; ++i )
      {
      if ( !hidden[i] )
        {
        input_defvals[nvisible]    = parameter_defvals[i];
        input_prompts[nvisible]    = parameter_prompts[i];
        input_ptrs[nvisible]       = input_strings[i];
        input_types[nvisible]      = parameter_types[i];
        input_maxlengths[nvisible] = V3STRLEN;
        pos_modes[nvisible]        = pos_modes[i];
        type_masks[nvisible]       = type_masks[i];
        strcpy(input_names[nvisible],parameter_names[i]);
      ++nvisible;
        }
      else
        {
        strcpy(input_strings[i],parameter_defvals[i]);
        }
      }
/*
***Add type and parameter name to prompt strings.
*/
    for ( i=0; i<nvisible; ++i )
      {
      switch ( input_types[i] )
        {
        case C_INT_VA: strcat(input_prompts[i],"  (INT "); break;
        case C_FLO_VA: strcat(input_prompts[i],"  (FLOAT "); break;
        case C_STR_VA: strcat(input_prompts[i],"  (STRING "); break;
        case C_VEC_VA: strcat(input_prompts[i],"  (VECTOR "); break;
        case C_REF_VA: strcat(input_prompts[i],"  (REF "); break;
        }
      strcat(input_prompts[i],input_names[i]);
      strcat(input_prompts[i],")");
      }
/*
***Prompt for actual (visible) parameter values.
***With only one parameter which is VECTOR or REF use IGcpos() and IGcref().
***This method requires less interaction from the user.
***Otherwise, use WPmsip() which gives a better overview.
*/
retry:
    if ( nvisible == 1  &&  input_types[0] == C_VEC_VA )
      {
      WPaddmess_mcwin(input_prompts[0],WP_MESSAGE);
      if ( pos_modes[0] >= 0 )
        {
        old_posmode = posmode; posmode = pos_modes[0];
        status = IGcpos(0,&expr);
        posmode = old_posmode;
        }
      else { status = IGcpos(0,&expr); }
      if ( status < 0 ) goto reject;
      pprexs(expr,modtyp,input_ptrs[0],V3STRLEN);
      }
    else if ( nvisible == 1  &&  input_types[0] == C_REF_VA )
      {
      WPaddmess_mcwin(input_prompts[0],WP_MESSAGE);
      status = IGcref(0,&type_masks[0],&expr,&end,&right);
      if ( status < 0 ) goto reject;
      pprexs(expr,modtyp,input_ptrs[0],V3STRLEN);
      }
    else if ( nvisible > 0 )
      {
      status = WPmsip(title,input_prompts,input_defvals,input_ptrs,
                      input_maxlengths,input_types,nvisible);
      if ( status < 0 ) goto reject;
      }
/*
***Analyze parameter values.
*/
    for ( i=0; i<npars; ++i )
      {
       anlogi();
       if ( (status=asinit(input_strings[i],ANRDSTR)) < 0 ) goto exit;
       ancset(&set,NULL,0,0,0,0,0,0,0,0,0);
       anascan(&sy);
       anarex(&expnode_ptrs[i],&attr,&set);
       asexit();

       if ( anyerr() )
         {
         sprintf(errbuf,"%d%%%s",i+1,input_strings[i]);
         erpush("IG3982",errbuf);
         errmes();
         goto retry;
         }
      }
/*
***Check that the input actual parameter types match the formal
***types. This is not done by WPmsip() etc.  FLOAT can use INT
***but all other types need to match exactly.
*/
    for ( i=0; i<npars; ++i )
      {
      inevev(expnode_ptrs[i],&actval,&dummy);
      switch ( parameter_types[i] )
        {
        case C_FLO_VA:
        if ( actval.lit_type != C_INT_VA  && actval.lit_type != C_FLO_VA )
          {
          sprintf(errbuf,"%d%%%s",i+1,input_strings[i]);
          erpush("IG3982",errbuf);
          errmes();
          goto retry;
          }
        break;

        default:
        if ( actval.lit_type != parameter_types[i] )
          {
          sprintf(errbuf,"%d%%%s",i+1,input_strings[i]);
          erpush("IG3982",errbuf);
          errmes();
          goto retry;
          }
        }
      }
/*
***If requested, add GLOBAL_REF() to REF-parameters.
*/
    if ( iggflg )
      {
      for ( i=0; i<npars; ++i )
        {
        if ( parameter_types[i] == C_REF_VA )
          {
          pmtcon(expnode_ptrs[i],(pm_ptr)NULL,&arglst,&dummy);
          stlook("GLOBAL_REF",&kind,&ref);
          pmcfue(ref,arglst,&expnode_ptrs[i]);
          }
        }
      }
/*
***Link parameters into a list.
*/
    param_list = (pm_ptr)NULL;

    for ( i=0; i<npars; ++i )
      {
      pmtcon(expnode_ptrs[i],param_list,&param_list,&dummy);
      }
/*
***Allocate a new ID and create the PART statement.
*/
    prtid = IGgnid();

    if ( pmcpas(prtid,part_name,(pm_ptr)NULL,param_list,csys_ptr,
                (pm_ptr)NULL,&retla) != 0 )
      {
      status = erpush("IG5023","PART");
      goto exit;
      }
/*
***Anything but a PART statement with the system in
***generic mode should be released.
*/
    if ( atyp == RUN ||  sysmode == EXPLICIT ) pmrele();
/*
***Now finally, execute the PART statement.
*/
#ifdef UNIX
    WPwait(GWIN_ALL,TRUE);
#endif
    status = inssta(retla);
#ifdef UNIX
    WPwait(GWIN_ALL,FALSE);
#endif
/*
***If execution failed there might be garbage in DB that
***needs cleaning. To be sure we call EXdel() on the part.
***If the execution error was detected before any data was
***stored in the DB this will yeild an extra error message
***stating that the referenced entity does not exist. Not
***very clear to the user but too much job to fix right now.
*/
    if ( status < 0 )
      {
      if ( defnap.save == 1 )
        {
        prtref.seq_val = prtid; prtref.ord_val = 1;
        prtref.p_nextre = NULL; EXdel(&prtref);
        }
      if ( atyp == PART ) pmrele();
      status = erpush("IG5222","PART");
      goto exit;
      }
/*
***The called module executed an EXIT("message") with error message.
*/
    else if ( status == 3 )
      {
      if ( defnap.save == 1 )
        {
        prtref.seq_val = prtid; prtref.ord_val = 1;
        prtref.p_nextre = NULL; EXdel(&prtref);
        }
      status = 0;
      goto exit;
      }
/*
***The called module executed an EXIT() without error mesage.
*/
    else if ( status == 4 ) status = 0;
/*
***Everything went fine. Link the PART statement to the
***end of the active module.
*/
    if ( atyp == PART )
      {
      if ( sysmode & GENERIC  &&  pmlmst(actmod, retla) < 0 )
        {
        status = erpush("IG5043","");
        goto exit;
        }
      else
        {
        sprintf(mesbuf,"%s%s",IGgtts(58),part_name);
        WPaddmess_mcwin(mesbuf,WP_MESSAGE);
        }
/*
***Update WPRWIN's.
*/
      WPrepaint_RWIN(RWIN_ALL,FALSE);
      }
/*
***Finish. Reset global state variables.
*/
    status = 0;
exit:
    tmpref     = oldtrf;
    defnap.hit = oldhit;
    actfunc    = oldafu;
    WPerhg();
/*
***The end.
*/
    return(status);
/*
***Error exits.
*/
reject:
    pmrele();
    goto exit;
  }

/********************************************************/
/********************************************************/

       short IGcall_macro(char *filnam)

/*      Interactive input of parameters for, and execution
 *      of, a MACRO module.
 *
 *      In: filnam => Name of MBO-file.
 *
 *      Return: 0 = OK.
 *              REJECT = Operation rejected.
 *              GOMAIN = Back to main menu.
 *
 *      Error:  IG5272   => Can't load module %s
 *              IG5283   => Can't read module parameters
 *              IG5023   => Can't create PART statement
 *              IG5222   => Error executing PART statement
 *              IG5362   => Not a MACRO module
 *
 *      (C)2007-04-27 Johan Kjellander
 *
 *      2007-09-22 WPRWIN repaint added, J.Kjellander
 *
 ******************************************************!*/

  {
    pm_ptr   parlst;              /* soft parameter list */
    pm_ptr   exnpt;               /* pekare till expr. node */
    pm_ptr   retla;
    pm_ptr   oldmod;              /* base adress of caller */
    pm_ptr   newmod;              /* base adress of called module */
    pm_ptr   dummy;
    short    status,mnum,oldpmd;
    int      posalt;
    DBetype typmsk;
    bool     end,right;
    pm_ptr   panola;              /* PM-pointer to param. node */
    char     name[80];            /* parameter name string buffer */
    char     prompt[80];          /* parameter prompt string buffer */
    PMLITVA  defval;              /* literal value structure */
    PMMONO  *mnpnt;               /* pointer to module node */
    char     dstr[V3STRLEN+1];    /* Defaultsträng för param.värde */
    char     istr[V3STRLEN+1];    /* Inputsträng för param.värde */
    short    tmpafu;              /* Aktiv funktion */
    bool     oldtrf;              /* Aktiv tmpref */
    int      oldhit,oldsav;       /* Temporary HIT and SAVE */
    short    prtid;               /* Partens sekvensnummer */
    bool     optflg,optpar;       /* Optionella parametrar */
    bool     menpar=FALSE;        /* Meny-parameter */
    bool     pospar=FALSE;        /* Pos-parameter */
    pm_ptr   ref,arglst;          /* För GLOABL_REF */
    stidcl   kind;                /* För GLOABL_REF */

/*
***Save current state etc. 
*/
    optflg = FALSE;

    oldtrf = tmpref;
    tmpref = TRUE;

    oldhit = defnap.hit;
    oldsav = defnap.save;
    defnap.hit  = 0;
    defnap.save = 0;
/*
***Save the number of the currently active function
***and repalce it with -2 (part call). Set the active
***part name to filnam so the help system can list the
***right help file.
*/
    tmpafu  = actfunc;
    actfunc = -2;
    strcpy(actpnm,filnam);
/*
***Remember current position on the PM stack.
*/
    pmmark();
/*
***Load module to PM.
*/
    oldmod = pmgbla();
    if ( pmgeba(filnam,&newmod) != 0 ) 
      {
      status = erpush("IG5272",filnam);
      goto exit;
      }
/*
***Read module header.
*/
    pmsbla(newmod);                 /* Set base adress to new module */
    pmgmod((pm_ptr)0,&mnpnt);       /* Get c-pointer to module node */
    pmsbla(oldmod);                 /* Set base adress back to old module */
/*
***The module must have the MACRO attribute !
*/
    if ( (mnpnt->moat_ != MACRO) )
      {
      status = erpush("IG5362",filnam);
      goto exit;
      }
/*
***Create the parameter list.
*/
    parlst = (pm_ptr)NULL;
    pmsbla(newmod);                          /* set base adress */
    if ( pmrpap((pm_ptr)0) != 0 ) goto error1;

    for(;;)
        {
        if ( pmgpad(&panola) != 0 ) goto error1;
        if ( panola == (pm_ptr)NULL )      /* no more params. ? */
            {
            pmsbla(oldmod);
            break;
            }
        if ( pmrpar(panola,name,prompt,&defval) != 0 ) goto error1;

        pmsbla(oldmod);            /* reset base adress */
/*
***Does this parameter get it's defult value from a file ?
*/
        file_parameter(prompt,&defval);
/*
***Parameter with prompt string.
*/
        if ( strlen(prompt) > 0 )
          {
          optpar = optional_parameter(prompt);
          if ( defval.lit_type == C_STR_VA )
            menpar = menu_parameter(prompt,&mnum);
          if ( defval.lit_type == C_VEC_VA )
            pospar = posalt_parameter(prompt,&posalt);
          if ( defval.lit_type == C_REF_VA &&
            !typemask_parameter(prompt,&typmsk) ) typmsk = ALLTYP;

          file_parameter(prompt,&defval);

          if ( optflg && optpar )
            {
            pmclie(&defval,&exnpt);
            }
          else
            {
            optflg = FALSE;
/*
***Substitute optional t-string with text.
*/
            t_string_prompt(prompt);
/*
***What kind of parameter ?
*/
            switch(defval.lit_type)
              {
/*
***INT parameter.
*/
              case C_INT_VA:
              IGplma(prompt,IG_INP);
              if ( optpar ) status = IGcint(0,"",istr,&exnpt);
              else
                {
                sprintf(dstr,"%d",defval.lit.int_va);
                status = IGcint(0,dstr,istr,&exnpt);
                }
              break;
/*
***FLOAT parameter.
*/
              case C_FLO_VA: 
              WPaddmess_mcwin(prompt,WP_MESSAGE);
              if ( optpar ) status = IGcflt(0,"",istr,&exnpt);
              else
                {
                sprintf(dstr,"%g",defval.lit.float_va);
                status = IGcflt(0,dstr,istr,&exnpt);
                }
              break;
/*
***STRING parameter.
*/
              case C_STR_VA:
              if ( optpar )
                {
                if ( menpar )
                  {
                  IGplma(prompt,IG_MESS);
                  status = IGcstm(mnum,&exnpt);
                  }
                else
                  {
                  IGplma(prompt,IG_INP);
                  status = IGcstr(0,"",istr,&exnpt);
                  }
                }
              else
                {
                if ( menpar )
                  {
                  IGplma(prompt,IG_MESS);
                  status = IGcstm(mnum,&exnpt);
                  }
                else
                  {
                  IGplma(prompt,IG_INP);
                  status = IGcstr(0,defval.lit.str_va,istr,&exnpt);
                  }
                }
              break;
/*
***VECTOR parameter.
*/
              case C_VEC_VA:
              IGplma(prompt,IG_MESS);
              if ( pospar )
                {
                oldpmd = posmode; posmode = posalt;
                status = IGcpos(0,&exnpt);
                posmode = oldpmd;
                }
              else { status = IGcpos(0,&exnpt); }
              break;
/*
***REF parameter.
*/
              case C_REF_VA:
              IGplma(prompt,IG_MESS);
              status = IGcref(0,&typmsk,&exnpt,&end,&right);
              if ( iggflg )
                {
                pmtcon(exnpt,(pm_ptr)NULL,&arglst,&dummy);
                stlook("GLOBAL_REF",&kind,&ref);
                pmcfue(ref,arglst,&exnpt);
                }
              break;
/*
***Error, unknown parameter type.
*/
              default:
              WPerhg();    
              status = erpush("IG5302",name);
              goto exit;
              }
/*
***If the user rejected and there are more parameters and these
***are optional, just go on with the next parameter.
*/
            if ( optpar && (status == REJECT) )
              {
              optflg = TRUE;
              pmclie(&defval,&exnpt);
              }
            else if ( status < 0 ) goto reject;

            IGrsma();
            }
          }
/*
***Parameter without prompt string.
*/
        else pmclie(&defval,&exnpt);
/*
***Link parameter to parameter list.
*/
        pmtcon(exnpt,parlst,&parlst,&dummy);
        pmsbla(newmod);
        }
/*
***Allocate a new ID and create the PART statement.
*/
    prtid = IGgnid();

    if ( pmcpas(prtid,filnam,(pm_ptr)NULL,parlst,(pm_ptr)NULL,
                (pm_ptr)NULL,&retla) != 0 )
      {
      status = erpush("IG5023","PART");
      goto exit;
      }
/*
***Since the part is a MACRO it may create new entities in the
***currently active module. In that case it's time to release
***the statement from PM now before execution so that the
***space can be reused.
*/
    pmrele();
/*
***Execute.
*/
#ifdef UNIX
    WPwait(GWIN_ALL,TRUE);
#endif

    status = inssta(retla);

#ifdef UNIX
    WPwait(GWIN_ALL,FALSE);
#endif

    if ( status < 0 )
      {
      status = erpush("IG5222","MACRO");
      goto exit;
      }
/*
***The called module executed an EXIT("message") with error message.
*/
    else if ( status == 3 )
      {
      status = 0;
      goto exit;
      }
/*
***The called module executed an EXIT() without error mesage.
*/
    else if ( status == 4 ) status = 0;
/*
***Update WPRWIN's.
*/
    WPrepaint_RWIN(RWIN_ALL,FALSE);
/*
***Finish.
*/
    status = 0;
exit:
/*
***Reset active values for HIT and SAVE etc..
*/
    tmpref  = oldtrf;
    actfunc = tmpafu;
    defnap.hit  = oldhit;
    defnap.save = oldsav;

    WPerhg();
/*
***The end.
*/
    return(status);
/*
***Error exits.
*/
reject:
    IGrsma();
    pmrele();
    goto exit;

error1:
    pmsbla(oldmod);
    status = erpush("IG5283","");
    goto exit;
  }

/********************************************************/
/********************************************************/

        short IGuppt()

/*      Interative Update Part function.
 *
 *      Return: 0      = OK
 *              REJECT = Operation cancelled
 *              GOMAIN = Back to main menu
 *
 *      Felkod:  IG5222  => Fel vid interpr. av PART-sats
 *               IG5332  => Kan ej hitta PART-sats i PM
 *
 *      (C)microform ab 27/10/88 R. Svedin
 *
 *      1/12/88  Ritpaketet, J. Kjellander
 *      3/5/89   Bug REJECT, J. Kjellander
 *      30/3/92  Ny re-interpretering, J. Kjellander
 *      20/5/94  Bug DBread_part_attributes()-1.10, J. Kjellander
 *
 ******************************************************!*/

  {
    char     mesbuf[V3STRLEN];
    DBetype  typ;
    bool     end,right,dstflg;
    short    status;
    pm_ptr   retla,stlla,dummy;
    PMREFVA  idvek[MXINIV];
    DBPart   part;
    DBPdat   pdat;
    PMMONO  *np;
    V2NAPA   oldnap;

/*
***Init.
*/
    dstflg = FALSE;
/*
***Get part ID.
*/
    IGptma(269,IG_MESS);
    typ = PRTTYP;
    status=IGgsid(idvek,&typ,&end,&right,(short)0);
    IGrsma();
    if ( status < 0 ) goto exit;
/*
***If mode is explicit, create part statement.
*/
    if ( sysmode == EXPLICIT )
      {
      if ( (status=IGgnps(idvek)) < 0 ) goto error;
      else dstflg = TRUE;
      }
/*
***Get PM ptr to part statement.
*/
    np = (PMMONO *)pmgadr((pm_ptr)0 );
    stlla = np->pstl_;

    if ( pmlges(idvek, &stlla, &retla) < 0 || retla == (pm_ptr)NULL)
      {
      erpush("IG5332","");
      goto error;
      }

    pmglin(retla, &dummy, &retla);
/*
***Re-execute.
*/
    V3MOME(&defnap,&oldnap,sizeof(V2NAPA));
    EXgtpt(idvek,&part);
    DBread_part_parameters(&pdat,NULL,NULL,part.dtp_pt,part.dts_pt);
/*
***Om det handlar om en mycket gammal part, 1.10 etc. kanske den
***inte har några attribut lagrade !!!
*/
    if ( part.dts_pt == sizeof(DBPdat)  &&  pdat.attp_pd > DBNULL )
      DBread_part_attributes((char *)&defnap,(int)pdat.asiz_pd,pdat.attp_pd);

    EXdraw(idvek,FALSE);
    gmmtm((DBseqnum)idvek->seq_val);
    status = inssta(retla); 
    V3MOME(&oldnap,&defnap,sizeof(V2NAPA));

    if ( status < 0 ) 
      {
      erpush("IG5222","PART");
      errmes();
      EXdraw(idvek,FALSE);
      gmumtm();
      EXdraw(idvek,TRUE);
      }
    else
      {
      sprintf(mesbuf,"Part %s %s",part.name_pt,IGgtts(61));
      WPaddmess_mcwin(mesbuf,WP_MESSAGE);
      }
/*
***The end.
*/
exit:
    if ( dstflg ) pmdlst();
    WPerhg();
    return(status);
/*
***Error exit.
*/
error:
    errmes();
    goto exit;
  }

/********************************************************/
/********************************************************/

        short IGcptw()

/*      The interactive Edit Part function.
 *
 *      Return: 0      = OK
 *              REJECT = Operation cancelled
 *              GOMAIN = Back to main menu
 *
 *      Felkod:  IG5272  => Kan ej ladda modulen %s
 *               IG5222  => Fel vid interpr. av PART-sats
 *               IG5322  => Kan ej ändra PART-sats
 *               IG3982  => Fel vid analysering av parameter
 *               IG4072  => Parten har inga parametrar
 *
 *      (C)microform ab 2/9/92 J. Kjellander
 *
 *      20/5/94 Bug DBread_part_attributes()-1.10, J. Kjellander
 *      13/2/95 ALLTYP, J. Kjellander
 *      1996-04-30 Noll parametrar, J. Kjellander
 *      1996-12-12 Flyttad till igepac, J.Kjellander
 *      1998-09-17 t/@ i promtsträngar, J.Kjellander
 *      1998-09-25 actfun, J.Kjellander
 *      2004-02-21 pmmark()+pmrele(), J.Kjellander
 *      2007-01-05 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
    int     typarr[V2MPMX];                 /* Parametertyper */
    char    oldval[V2MPMX][V3STRLEN];       /* Gamla värden */
    char   *osparr[V2MPMX];
    char    newval[V2MPMX][V3STRLEN];       /* Nya värden */
    char   *nsparr[V2MPMX];
    char    pmtarr[V2MPMX][V3STRLEN+1];     /* Promptar */
    char   *pmparr[V2MPMX];
    short   maxtkn[V2MPMX];                 /* Stränglängder */
    short   pant;                           /* Antal parametrar */
    short   i,status;
    char    parnam[V3STRLEN+1];
    char    rubrik[V3STRLEN],errbuf[V3STRLEN],mesbuf[V3STRLEN];
    pm_ptr  pla[V2MPMX];                    /* Parametrars PM-adress */
    pm_ptr  oldmod,newmod,panola,exnpt,retla;
    DBetype typ;
    bool    end,right,edit,ref,dstflg;
    short   oldafn;
    ANFSET  set;
    ANATTR  attr;
    DBPart  part;
    PMREFVA idvek[MXINIV];
    DBPdat  pdat;
    PMLITVA defval;
    V2NAPA  oldnap;

/*
***Initiering.
*/
    edit = dstflg = FALSE;
    pmmark();
/*
***Ta reda på parten:s ID.
*/
    WPaddmess_mcwin(IGgtts(269),WP_PROMPT);
    typ = PRTTYP;
    status=IGgsid(idvek,&typ,&end,&right,(short)0);
    WPclear_mcwin();

    if ( status < 0 )
      goto exit;
    else
      {
      idvek[0].ord_val = 1;
      idvek[0].p_nextre = NULL;
      }
/*
***Läs part-posten.
*/
    if ( EXgtpt(idvek,&part) < 0 ) goto error1;
/*
***Om ritpaket, skapa partanrop.
*/
    if ( sysmode == EXPLICIT )
      {
      if ( (status=IGgnps(idvek)) < 0 ) goto error1;
      else dstflg = TRUE;
      }
/*
***Notera nuvarande parametrars antal och PM-adresser.
*/
    pmgpla(idvek,&pant,pla);
/*
***Om parametrar saknas slutar vi här.
*/
   if ( pant == 0 )
     {
     erpush("IG4072",part.name_pt);
     goto error1;
     }
/*
***Dekompilera de aktuella parametrarnas värde till strängar
***så att lämpliga värden kan presenteras i inmatningsfälten.
*/
    for ( i=0; i<pant; ++i )
      {
      maxtkn[i] = V3STRLEN;
      pprexs(pla[i],modtyp,oldval[i],maxtkn[i]);
      }
/*
***Är parten refererad ?
*/
    if ( sysmode & GENERIC  &&  pmamir(idvek) ) ref = TRUE;
    else  ref = FALSE;
/*
***Ladda in modulen.
*/
    oldmod = pmgbla();
    if ( pmgeba(part.name_pt,&newmod) != 0 )
      {
      erpush("IG5272",part.name_pt);
      goto error1;
      }
/*
***Gör den anropade modulen aktiv och plocka fram promtar
***och typer.
*/
    pmsbla(newmod);
    pmrpap((pm_ptr)0);

    for ( i=0; i<pant; ++i )
      {
      pmgpad(&panola);
/*
***Initiera pekare till strängar för senare indata till WPmsip().
*/
      osparr[i] = oldval[i];
      nsparr[i] = newval[i];
      pmparr[i] = pmtarr[i];
/*
***Läs parameterns namn, prompt och defaultvärde.
*/
      pmrpar(panola,parnam,pmtarr[i],&defval);
/*
***Mappa ev. t-sträng till klartext och strippa
***ev. inledande "@ " eller om det är 2 @ strippa
***det 1:a.
*/
      if ( strlen(pmtarr[i]) > 0 )
        {
        t_string_prompt(pmtarr[i]);
        optional_parameter(pmtarr[i]);
        }
/*
***Parameter utan promt är gömd.
*/
     else strcpy(pmtarr[i],IGgtts(1627));
/*
***Vilken typ har den ?
*/
      typarr[i] = defval.lit_type;
/*
***Lägg till typ och namn efter promten.
*/
      switch(defval.lit_type)
        {
        case C_INT_VA: strcat(pmtarr[i],"  (INT "); break;
        case C_FLO_VA: strcat(pmtarr[i],"  (FLOAT "); break;
        case C_STR_VA: strcat(pmtarr[i],"  (STRING "); break;
        case C_VEC_VA: strcat(pmtarr[i],"  (VECTOR "); break;
        case C_REF_VA: strcat(pmtarr[i],"  (REF "); break;
        }
      strcat(pmtarr[i],parnam);
      strcat(pmtarr[i],")");
      }

    pmsbla(oldmod);
/*
***Låt hjälpsystemet få veta vad vi gör.
*/
    oldafn  = actfunc;
    actfunc = -2;
    strcpy(actpnm,part.name_pt);
/*
***Anropa WPmsip. t1599 = "Parametrar för parten : "
*/
    strcpy(rubrik,IGgtts(1599));
    strcat(rubrik,part.name_pt);
#ifdef UNIX
    status = WPmsip(rubrik,pmparr,osparr,nsparr,maxtkn,typarr,pant);
#endif
#ifdef WIN32
    status = (short)msmsip(rubrik,pmparr,osparr,nsparr,maxtkn,typarr,pant);
#endif

    actfunc = oldafn;

    if ( status < 0 ) goto exit;
/*
***Gå igenom de nya strängarna och byt ut alla parametrar i
***anropet som har ändrats.
*/
    for ( i=0; i<pant; ++i )
      {
      if ( strcmp(nsparr[i],osparr[i]) != 0 )
        {
/*
***Parametern "i" har ändrats ! Analysera den nya MBS-strängen.
*/
        anlogi();
        if ( (status=asinit(nsparr[i],ANRDSTR)) < 0 ) goto exit;
        ancset(&set,NULL,0,0,0,0,0,0,0,0,0);
        anascan(&sy);
        anarex(&exnpt,&attr,&set);
        if ( (status=asexit()) < 0 ) goto exit;
/*
***Hur gick det.
*/ 
        if ( anyerr() )
          {
          sprintf(errbuf,"%d%%%s",i+1,nsparr[i]);
          erpush("IG3982",errbuf);
          goto error1;
          }
/*
***Kolla om det nya parametervärdet innebär framåt-referenser.
*/
        if ( sysmode & GENERIC  &&  pmarex(idvek,exnpt) == TRUE )
          {
          erpush("IG3882",nsparr[i]);
          goto error1;
          }
/*
***Byt parametern i part-anropet.
*/
        if ( pmchpa(idvek,(short)(i+1),exnpt,&retla) < 0 || retla == (pm_ptr)NULL )
          {
          erpush("IG5322","");
          goto error1;
          }
        edit = TRUE;
        }
      }
/*
***Om refererad, fråga om hela modulen skall köras. Annars
***kör bara om part-anropet.
*/
    if ( edit )
      {
      if ( ref && IGialt(373,67,68,FALSE) ) IGrun_active();
/*
***Reinterpretera inkrementellt.
*/
      else
        {
        V3MOME(&defnap,&oldnap,sizeof(V2NAPA));
        DBread_part_parameters(&pdat,NULL,NULL,part.dtp_pt,part.dts_pt);
        if ( part.dts_pt == sizeof(DBPdat)  &&  pdat.attp_pd > DBNULL )
          DBread_part_attributes((char *)&defnap,(int)pdat.asiz_pd,pdat.attp_pd);
        EXdraw(idvek,FALSE);
        gmmtm((DBseqnum)idvek->seq_val);
#ifdef UNIX
        WPwait(GWIN_MAIN,TRUE);
#endif
        status = inssta(retla); 
#ifdef UNIX
        WPwait(GWIN_MAIN,FALSE);
#endif
        V3MOME(&oldnap,&defnap,sizeof(V2NAPA));

        if ( status < 0  ||  status == 3 ) 
          {
          if ( status < 0 )
            {
            erpush("IG5222","PART");
            errmes();
            }
          EXdraw(idvek,FALSE);
          gmumtm();
          EXdraw(idvek,TRUE);
          if ( sysmode & GENERIC ) for ( i=0; i<pant; ++i )
            pmchpa(idvek,(short)(i+1),pla[i],&retla);
          }
        else
          {
          sprintf(mesbuf,"Part %s %s",part.name_pt,IGgtts(61));
          WPaddmess_mcwin(mesbuf,WP_MESSAGE);
          }
/*
***Update WPRWIN's.
*/
        WPrepaint_RWIN(RWIN_ALL,FALSE);
        }
      }
/*
***Avslutning.
*/
exit:
    if ( dstflg ) pmdlst();
    if ( sysmode == EXPLICIT ) pmrele();
    WPerhg();
    return(status);
/*
***Felutgångar.
*/
error1:
    errmes();
    goto exit;
  }

/********************************************************/
/********************************************************/

       short IGgnps(PMREFVA *id)

/*     Creates (reengineers) a PART statement from an
 *     existing part in the DB.
 *
 *      In: id  => Part ID.
 *
 *      Error: IG5023 = Can't create PART statement
 *             IG5342 = A parameter is call by reference
 *
 *      (C)microform ab 29/11/88 J. Kjellander
 *
 ******************************************************!*/

  {
    short    prtid;               /* Partens sekvensnummer */
    pm_ptr   dummy;               /* Som det låter */
    pm_ptr   oblpar;              /* Obligatoriska parametrar */
    pm_ptr   parlst;              /* Parameterlista */
    pm_ptr   exnpt;               /* Pekare till expr. node */
    pm_ptr   retla;               /* Pekare till den skapade satsen */
    short    status;              /* Anropade rutiners status */
    short    i;                   /* Loop-variabel */
    PMLITVA  litval;              /* Litterärt värde */
    DBPart    part;                /* Part-posten */
    DBPdat   data;                /* Part-data */
    PMPATLOG typarr[V2MPMX];      /* Parametertyper */

/*
***Get part dtata from DB.
*/
    if ( (status=EXgtpt(id,&part)) < 0 ) return(status);
    DBread_part_parameters(&data,typarr,NULL,part.dtp_pt,part.dts_pt);
/*
***If part attribute is LOCAL, create reference to csys.
*/
    if ( data.matt_pd  == LOCAL )
        {
        litval.lit_type = C_REF_VA;
        DBget_id(data.csp_pd,&litval.lit.ref_va[0]);
        pmclie(&litval,&exnpt);
        pmtcon(exnpt,(pm_ptr)NULL,&oblpar,&dummy);
        }
     else oblpar = (pm_ptr)NULL;
/*
***Create the parameter list.
*/
    parlst = (pm_ptr)NULL;

    for( i=0; i<data.npar_pd; ++i)
      {
      switch ( typarr[i].log_id )
        {
        case C_INT_VA:
        litval.lit_type = C_INT_VA;
        if ( (status=EXgint(id,(short)(i+1),&litval)) < 0 ) return(status);
        break;

        case C_FLO_VA: 
        litval.lit_type = C_FLO_VA;
        if ( (status=EXgflt(id,(short)(i+1),&litval)) < 0 ) return(status);
        break;

        case C_STR_VA:
        litval.lit_type = C_STR_VA;
        if ( (status=EXgstr(id,(short)(i+1),&litval)) < 0 ) return(status);
        break;

        case C_VEC_VA:
        litval.lit_type = C_VEC_VA;
        if ( (status=EXgvec(id,(short)(i+1),&litval)) < 0 ) return(status);
        break;

        case C_REF_VA:
        litval.lit_type = C_REF_VA;
        if ( (status=EXgref(id,(short)(i+1),&litval)) < 0 ) return(status);
        break;

        default:
        return(erpush("IG5342",""));
        }

      pmclie(&litval,&exnpt);
      pmtcon(exnpt,parlst,&parlst,&dummy);
      }
/*
***Create the statement.
*/
    prtid = id[0].seq_val;

    if ( pmcpas(prtid,part.name_pt,(pm_ptr)NULL,parlst,
                oblpar,(pm_ptr)NULL,&retla) != 0 )
      return(erpush("IG5023","PART"));
    else
      pmlmst(actmod,retla);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/********************************************************/

static bool optional_parameter(char *prompt)

/*      Check if a parameter is optional, ie. if
 *      it's prompt string starts with @+space or @@.
 *
 *      In: Parameter prompt string.
 *
 *      Out: Prompt string stripped from leading @.
 *
 *      Return: TRUE  = Parameter is optional.
 *              FALSE = Parameter is not optional.
 *
 *      (C)microform ab 19/2/88 J. Kjellander
 *
 *      11/11/88 Meny-parametrar, J. Kjellander
 *      2007-04-23 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
  char tmp[V3STRLEN+1];

    if ( *prompt == '@'  &&  *(prompt+1) == ' ' )
      {
      strcpy(tmp,prompt);
      strcpy(prompt,tmp+2);
      return(TRUE);
      }
    else if ( *prompt == '@'  &&  *(prompt+1) == '@' )
      {
      strcpy(tmp,prompt);
      strcpy(prompt,tmp+1);
      return(TRUE);
      }
    else return(FALSE);
  }
  
/********************************************************/
/********************************************************/

static bool   menu_parameter(
       char  *prompt,
       short *mnum)

/*      Check if parameter should get it's value from a menu,
 *      ie. the prompt string starts with @+m+int.
 *
 *      In: prompt = Parameter prompt string.
 *
 *      Out: Prompt string stripped from leading @mn
 *           *mnum = Menu number
 *
 *      Return: TRUE  = Menu parameter.
 *              FALSE = Not menu parameter.
 *
 *      (C)microform ab 11/11/88 J. Kjellander
 *
 *      2007-04-23 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short n,i,pl;
    char tmp[V3STRLEN+1];

/*
***Do we have @m....
*/
    if ( *prompt == '@'  &&  *(prompt+1) == 'm' )
      {
      pl = strlen(prompt);
      for ( i=2; i<pl; ++i) if ( prompt[i] == ' '  ) break;
      if ( i < pl )
        {
        n = sscanf(prompt+2,"%hd",mnum);
        if ( n == 1 )
          {
          strcpy(tmp,prompt+i+1);
          strcpy(prompt,tmp);
          return(TRUE);
          }
        }
      }
/*
***No hit.
*/
    return(FALSE);
  }
  
/********************************************************/
/********************************************************/

static bool   posalt_parameter(
       char  *prompt,
       int   *posalt)

/*      Check if VECTOR parameter wants input with a specific
 *      method, ie. if it's prompt string starts with @+a+int.
 *
 *      In: prompt = Parameter prompt string.
 *
 *      Out: Prompt string stripped from leading @an
 *          *posalt = Position method alternative
 *
 *      Return: TRUE  = This is a posalt parameter.
 *              FALSE = Not a posalt parameter.
 *
 *      (C)microform ab 15/11/88 J. Kjellander
 *
 *      2007-04-23 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    int  n,i,pl;
    char tmp[V3STRLEN+1];

    if ( *prompt == '@'  &&  *(prompt+1) == 'a' )
      {
      pl = strlen(prompt);
      for ( i=2; i<pl; ++i) if ( prompt[i] == ' '  ) break;
      if ( i < pl )
        {
        n = sscanf(prompt+2,"%d",posalt);
        if ( n == 1 )
          {
          strcpy(tmp,prompt+i+1);
          strcpy(prompt,tmp);
          return(TRUE);
          }
        }
      }
    return(FALSE);
  }
  
/********************************************************/
/********************************************************/

static bool     typemask_parameter(
       char    *prompt,
       DBetype *typemask)

/*      Check if REF-parameter is restricted by a typemask,
 *      ie. if it's prompt string starts with @+t+int.
 *
 *      In: prompt = Parameter prompt string.
 *
 *      Ut: Prompt string stripped on leading @tn.
 *          *typemask = The typemask.
 *
 *      Return: TRUE  = This is a typemask parameter.
 *              FALSE = Not a typemask parameter.
 *
 *      (C)microform ab 25/11/88 J. Kjellander
 *
 *      2007-04-23 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    int  n,i,pl,tmpint;
    char tmp[V3STRLEN+1];

    if ( *prompt == '@'  &&  *(prompt+1) == 't' )
      {
      pl = strlen(prompt);
      for ( i=2; i<pl; ++i) if ( prompt[i] == ' '  ) break;
      if ( i < pl )
        {
        n = sscanf(prompt+2,"%d",&tmpint);
       *typemask = tmpint;
        if ( n == 1 )
          {
          strcpy(tmp,prompt+i+1);
          strcpy(prompt,tmp);
          return(TRUE);
          }
        }
      }
    return(FALSE);
  }
  
/********************************************************/
/********************************************************/

static void     file_parameter(
       char    *prompt,
       PMLITVA *defval)

/*      Check if parameter uses its own default value or
 *      a default value from a file, ie. if its prompt
 *      string starts with  @f+path or @fpath(line_number)
 *      If path = t119 = "act_job", jobdir+jobnam is used.
 *
 *      In: prompt = Parameter prompt string.
 *
 *      Out: Prompt string stripped on leading @fpath.
 *          *defval = Default value from file.
 *
 *      Return: 0.
 *
 *      (C)microform ab 11/8/90 J. Kjellander
 *
 *      1/10/91  Strippat Ön, J. Kjellander.
 *      10/11/91 Gömd parameter, J. Kjellander
 *      12/2/92  Bug "IGgtts(119)", J. Kjellander
 *      2007-04-23 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    int    n,i,pl,rn,ival;
    double fval;
    char   tmp[V3STRLEN+1];
    char   defstr[V3STRLEN+1];
    char   path[V3PTHLEN+1];
    FILE  *f;

/*
***Do we have '@ + f' ?
*/
    if ( *prompt == '@'  &&  *(prompt+1) == 'f' )
      {
/*
***Get the path.
*/
      pl = strlen(prompt);
      for ( n=2; n<pl; ++n)
        if ( prompt[n] == ' '  ||  prompt[n] == '(' ) break;

      strncpy(path,&prompt[2],n-2);
      path[n-2] = '\0';
/*
***Replace optional "act_job" with jobdir + jobnam.
*/
      if ( strncmp(IGgtts(119),path,7) == 0 )
        {
        strcpy(tmp,jobdir);
        strcat(tmp,jobnam);
        strcat(tmp,&path[7]);
        strcpy(path,tmp);
        }
/*
***Get optional line number.
*/
      if ( prompt[n] == '(' )
        {
        for ( i=n; i<pl; ++i) if ( prompt[i] == ')' ) break;
        strncpy(tmp,&prompt[n+1],i-n-1);
        tmp[i-n-1] = '\0';
        sscanf(tmp,"%d",&rn);
        }
      else
        {
        rn = 1;
        i = n - 1;
        }
/*
***Read value from file.
*/
      if ( (f=fopen(path,"r")) != 0 )
        {
        for ( n=0; n<rn; ++n )
          fgets(defstr,V3STRLEN,f);
        n = strlen(defstr);
        if ( defstr[n-1] == '\n' ) defstr[n-1] = '\0';
        fclose(f);
/*
***Save the value in the PMLITVA.
*/
        switch ( defval->lit_type )
          {
          case C_STR_VA:
          strcpy(defval->lit.str_va,defstr);
          break;

          case C_INT_VA:
          if ( sscanf(defstr,"%d",&ival) == 1 )
            defval->lit.int_va = ival;
          break;

          case C_FLO_VA:
          if ( sscanf(defstr,"%lf",&fval) == 1 )
            defval->lit.float_va = fval;
          break;
          }
        }
/*
***Strip prompt fom @f....
*/
      if ( (int)strlen(prompt) > i+1 )
        {
        strcpy(tmp,prompt+i+2);
        strcpy(prompt,tmp);
        }
      else prompt[0] = '\0';
      }
/*
***The end.
*/
    return;
  }
  
/********************************************************/
/********************************************************/

static void t_string_prompt(char *prompt)

/*      Check if prompt string should be replaced
 *      with the value of a t-string, ie. if it starts
 *      with t+int.
 *
 *      In: Prompt string.
 *
 *      Out: Prompt string replaced.
 *
 *      (C)microform ab 1996-05-30 J. Kjellander
 *
 *      2007-04-23 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
   int  tnum;

   if ( *prompt == 't'  &&  (sscanf(prompt+1,"%d",&tnum) == 1) )
     strcpy(prompt,IGgtts(tnum));
/*
***The end.
*/
   return;
  }

/********************************************************/
/********************************************************/

       short  IGedit_MBS()

/*     Edit MBS program in the lib directory.
*
*      (C)2007-11-20 J. Kjellander.
*
*******************************************************!*/

 {
/*
***Call WP.
*/
   return(WPomod());
 }

/******************************************************!*/
/********************************************************/

       short  IGcompile_all()

/*     Compile all MBS programs in the lib directory.
*
*      (C)2007-11-20 J. Kjellander.
*
*******************************************************!*/

 {
/*
***Call WP.
*/
   return(WPcoal());
 }

/******************************************************!*/
/********************************************************/

static short get_partname(char *name)

/*      Returns the name of a module that can be called
 *      using the PART statement.
 *
 *      Out: name = C ptr to module name
 *
 *      (C)2007-05-28 J. Kjellander
 *
 *      2007-11-20 2.0, J.Kjellander
 *
 ******************************************************!*/

  {
   short status;
   char *nparr1[5000],*nparr2[5000],strarr1[100000],
         strarr2[100000],libdir[V3PTHLEN];
   int   i,j,n_names1,n_names2;

/*
***Get the two file lists.
*/
   if ( (status=IGdir(jobdir,MODEXT,5000,100000,nparr1,strarr1,&n_names1)) < 0 ) return(status);
   strcpy(libdir,jobdir);
   strcat(libdir,"lib/");
   if ( (status=IGdir(libdir,MODEXT,5000,100000,nparr2,strarr2,&n_names2)) < 0 ) return(status);
/*
***Remove the active job from the first list.
*/
   for ( i=0; i<n_names1; ++i )
     {
     if ( strcmp(nparr1[i],jobnam) == 0 )
       {
     --n_names1;
       for ( ; i<n_names1; ++i ) nparr1[i] = nparr1[i+1];
       break;
       }
     }
/*
***Remove entries from the second list which are equal to the first list.
*/
   for ( i=0; i<n_names1; ++i )
     {
     for ( j=0; j<n_names2; ++j )
       {
       if ( strcmp(nparr1[i],nparr2[j]) == 0 )
         {
       --n_names2;
         for ( ; j<n_names2; ++j ) nparr2[j] = nparr2[j+1];
         break;
         }
       }
     }
/*
***Let the user make a selection from the directory that
***is currently active.
*/
start:

   switch ( part_dir )
     {
     case 1: status = WPselect_partname(1,nparr1,n_names1,name); break;
     case 2: status = WPselect_partname(2,nparr2,n_names2,name); break;
     default: return(REJECT);
     }
/*
***Did the user select another directory ?
*/
   if ( status == 1  ||  status == 2 )
     {
     part_dir = status;
     goto start;
     }
/*
***Maybe WPselect_partname() failed because of too many files.
***In that case, use simple input. WPselect_partname() should
***be changed so it supports multi page mode.
*/
   else if ( status == 3 )
     {
     if ( (status=IGssip(IGgtts(244),IGgtts(267),name,"",JNLGTH)) < 0 ) errmes();
     }
/*
***No, then this is the end.
*/
   return(status);
  }

/********************************************************/
