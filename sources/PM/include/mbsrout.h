/*!******************************************************************
*  File: mbsrout.h 
*  ===============   
* 
*  Definitions for the MBS interpreter. This file compiles
*  for Varkon if VARKON is defined. If not, see end of file.
*
*  This file is part of the Program Module (PM) Library. 
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
*  You should have received a copy of the GNU Library General 
*  Public License along with this library; if not, write to the 
*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge, 
*  MA 02139, USA. 
*   
******************************************************************/

/*
***Routine-ID's for MBS functions and procedures in Varkon.
*/
#ifdef VARKON

#define VPOI      1    /*evpofr      POI_FREE    */
#define VPOIP     2    /*evpopr      POI_PROJ    */
#define VLINF     3    /*evlifr      LIN_FREE    */
#define VLINP     4    /*evlipr      LIN_PROJ    */
#define VLINA     5    /*evlian      LIN_ANG     */
#define VLINO     6    /*evliof      LIN_OFS     */
#define VLINT1    7    /*evlit1      LIN_TAN1    */
#define VLINT2    8    /*evlit2      LIN_TAN2    */
#define VARCF     9    /*evarfr      ARC_FREE    */
#define VARC2P   10    /*evar2p      ARC_2POS    */
#define VARC3P   11    /*evar3p      ARC_3POS    */
#define VARCO    12    /*evarof      ARC_OFS     */
#define VARCFI   13    /*evarfl      ARC_FIL     */
#define VCURF    14    /*evcufr      CUR_FREE   Utgått ! */
#define VCURP    15    /*evcupr      CUR_PROJ   Utgått ! */
#define VCURO    16    /*evcuro      CUR_OFFS    */
#define VCOMP    17    /*evcomp      COMP        */
#define VXHT     18    /*evxht       XHATCH      */
#define VPART    19    /*evpart      PART        */
#define VIONSUR  20    /*evions      INV_ON_SUR  */
#define VTEXT    21    /*evtext      TEXT        */
#define VPINC    22    /*evpinc      POS_IN_CONE */
#define VCSYS3P  23    /*evcs3p      CSYS_3P     */
#define VGROUP   24    /*evgrp       GROUP       */
#define VLDIM    25    /*evldim      LDIM        */
#define VCDIM    26    /*evcdim      CDIM        */
#define VRDIM    27    /*evrdim      RDIM        */
#define VADIM    28    /*evadim      ADIM        */
#define VACOS    29    /*evacos      ARCSIN      */
#define VASIN    30    /*evasin      ARCSIN      */
#define VATAN    31    /*evatan      ARCTAN      */
#define VCOS     32    /*evcos       COS         */
#define VSIN     33    /*evsin       SIN         */
#define VTAN     34    /*evtan       TAN         */
#define VLN      35    /*evnlog      LN          */
#define VLOG     36    /*evlogn      LOG         */
#define VSQRT    37    /*evsqrt      SQRT        */
#define VABS     38    /*evabs       ABS         */
#define VFRAC    39    /*evfrac      FRAC        */
#define VROUND   40    /*evroud      ROUND       */
#define VTRUNC   41    /*evtrnc      TRUNC       */

/*
***Nya rutiner i samband med Kartro 30/5/86
*/

#define VGTREF   42    /*evgref      GETREF      */
#define VLDGM    43    /*evldgm      LOAD_GM     */
#define VSVGM    44    /*evsvgm      SAVE_GM     */

#define VBLEV    45    /*evbllv      BLANK_LEV   Removed */
#define VUBLEV   46    /*evublv      UNBLANK_LEV Removed */
#define VGTLEV   47    /*evgtlv      GET_LEV     Removed */
#define VACLEV   48    /*evalev      ACT_LEV     Removed */

#define VNCURI   49    /*evncui      N_CUR_INT   */
#define VEVAL    50    /*eveval      EVAL        */

/*
***Ny rutin för version 1.4  9/4/87 
*/

#define VFINDS   51    /*evfins      FINDS       */
#define VARCL    52    /*evarcl      ARCL        */
#define VTANG    53    /*evtang      TANG        */
#define VCENTRE  54    /*evcen       CENTRE      */
#define VINTERS  55    /*evsect      INTERSECT   */
#define VON      56    /*evon        ON          */
#define VVECP    57    /*evvecp      VECP        */
#define VIDENT   58    /*evidnt      IDENT       */
#define VPOS     59    /*evpos       POS         */
#define VSCREEN  60    /*evscr       SCREEN      */
#define VBPLAN   61    /*evbpln      B_PLANE     */
#define VMODLO   62    /*evmolo      MODE_LOCAL  */
#define VMODGL   63    /*evmogl      MODE_GLOBAL */
#define VVEC     64    /*evvec       VEC         */
#define VSET     65    /*evset       SET         */
#define VOS      66    /*evos        OS          */

/*
***Nya rutiner för version 1.4  9/4/87
*/

#define VVECL    67    /*evvecl      VECL        */
#define VVECN    68    /*evvecn      VECN        */
#define VANGLE   69    /*evangl      ANGLE       */
#define VASCII   70    /*evasci      ASCII       */
#define VSTR     71    /*evstr       STR         */
#define VCHR     72    /*evchr       CHR         */
#define VVAL     73    /*evval       VAL         */
#define VLENGTH  74    /*evleng      LENGTH      */
#define VSUBSTR  75    /*evsubs      SUBSTR      */
#define VOPEN    76    /*evopen      OPEN        */
#define VSEEK    77    /*evseek      SEEK        */
#define VOUTINT  78    /*evouti      OUTINT      */
#define VOUTFLT  79    /*evoutf      OUTFLT      */
#define VOUTSTR  80    /*evouts      OUTSTR      */
#define VOUTLIN  81    /*evoutl      OUTLIN      */
#define VOUTBIN  82    /*evoutb      OUTBIN      */
#define VININT   83    /*evinii      ININT       */
#define VINFLT   84    /*evinif      INFLT       */
#define VINSTR   85    /*evinis      INSTR       */
#define VINLIN   86    /*evinl       INLIN       */
#define VINBIN   87    /*evinb       INBIN       */
#define VFPOS    88    /*evfpos      FPOS        */
#define VCLOSE   89    /*evclos      CLOSE       */
#define VIOSTAT  90    /*eviost      IOSTAT      */

/*
***Nya rutiner i och med version 1.2, 13/3/86.
*/

#define VGTID    91    /*evgtid      GETID      Removed */
#define VGTHDR   92    /*evghdr      GETHDR      */
#define VGTPOI   93    /*evgpoi      GETPOI      */
#define VGTLIN   94    /*evglin      GETLIN      */
#define VGTARC   95    /*evgarc      GETARC      */
#define VGTCUR   96    /*evgcur      GETCUR     Removed */
#define VGTTRF   97    /*evgtrf      GETTRF      */
#define VGTTXT   98    /*evgtxt      GETTXT      */
#define VGTXHT   99    /*evgxht      GETXHT      */
#define VGTLDM  100    /*evgldm      GETLDM      */
#define VGTCDM  101    /*evgcdm      GETCDM      */
#define VGTRDM  102    /*evgrdm      GETRDM      */
#define VGTADM  103    /*evgadm      GETADM      */
#define VGTGRP  104    /*evggrp      GETGRP      */
#define VGTCSY  105    /*evgcsy      GETCSY      */
#define VGTSYM  106    /*      GETSYM     Utgï¿½ */
#define VGTPRT  107    /*evgprt      GETPRT      */
#define VGTTYP  108    /*evgtyp      GETTYP      */
#define VGTINT  109    /*evgint      GETINT      */
#define VGTFLT  110    /*evgflt      GETFLT      */
#define VGTSTR  111    /*evgstr      GETSTR      */
#define VGTVEC  112    /*evgvec      GETVEC      */
#define VSYMB   113    /*evsymb      SYMB        */
#define VTRIM   114    /*evtrim      TRIM        */
#define VDEL    115    /*evdel       DEL         */
#define VCLGM   116    /*evclgm      CLEAR_GM    */
#define VBLNK   117    /*evblk       BLANK      Utgått */
#define VUBLNK  118    /*evublk      UNBLANK    Utgått */

/*
*** Nya rutiner för Kartro 30/6/86
*/

#define VCREVI  119    /*evcrvi      CRE_VIEW    */
#define VACTVI  120    /*evacvi      ACT_VIEW    */
#define VSCLVI  121    /*evscvi      SCL_VIEW    */
#define VCENVI  122    /*evcevi      CEN_VIEW    */
#define VERAVI  123    /*evervi      ERASE_VIEW  */
#define VREPVI  124    /*evrpvi      REP_VIEW    */
#define VCACVI  125    /*evcavi      CACC_VIEW   */

/*
***Ny rutin för version 1.3 3/11/86     
*/

#define VPLTVI  126    /*evplvi      PLOT_VIEW   Removed */

/*
***Nya rutiner för version 1.3 1/10/86    
*/

#define VTIM    127    /*evtime      TIME        */
#define VRSTR   128    /*evrstr      RSTR        */
#define VRVAL   129    /*evrval      RVAL        */
#define VREFC   130    /*evrefc      REFC        */
#define VLSIN   131    /*evlsin      LST_INI     */
#define VLSEX   132    /*evlsex      LST_EXI     */
#define VLSLN   133    /*evlsln      LST_LIN     */

/*
***Nya rutiner för version 1.3 30/10/86    
*/

#define VACBLK  134   /*evablk      ACT_BLANK   */
#define VACVIN  135   /*evavin      ACT_VNAM    */
#define VACPEN  136   /*evapen      ACT_PEN     */
#define VACSCL  137   /*evascl      ACT_SCL     */
#define VACDSCL 138   /*evadsc      ACT_DSCL    */
#define VACCAC  139   /*evacac      ACT_CACC    */
#define VACGRX  140   /*evagrx      ACT_GRIDX   */
#define VACGRY  141   /*evagry      ACT_GRIDY   */
#define VACGRDX 142   /*evagdx      ACT_GRIDDX  */
#define VACGRDY 143   /*evagdy      ACT_GRIDDY  */
#define VACGRID 144   /*evagrd      ACT_GRID    */
#define VACJOBN 145   /*evajbn      ACT_JOBNAM  */
#define VACMTYP 146   /*evamtp      ACT_MTYPE   */
#define VACMATR 147   /*evamat      ACT_MATTR   */
#define VACJOBD 148   /*evajbd      ACT_JOBDIR  */
#define VACLFNT 149   /*evalft      ACT_LFONT   */
#define VACAFNT 150   /*evaaft      ACT_AFONT   */
#define VACXFNT 151   /*evaxft      ACT_XFONT   */
#define VACLDSL 152   /*evaldl      ACT_LDASHL  */
#define VACADSL 153   /*evaadl      ACT_ADASHL  */
#define VACXDSL 154   /*evaxdl      ACT_XDASHL  */
#define VACTSIZ 155   /*evatsi      ACT_TSIZE   */
#define VACTWID 156   /*evatwi      ACT_TWIDTH  */
#define VACTSLT 157   /*evatsl      ACT_TSLANT  */
#define VACDTSZ 158   /*evadts      ACT_DTSIZ   */
#define VACDASZ 159   /*evadas      ACT_DASIZ   */
#define VACDNDG 160   /*evadnd      ACT_DNDIG   */
#define VACDAUT 161   /*evadau      ACT_DAUTO   */

/* 
***Ny rutin för version 1.4 21/12/86       
*/

#define VGLOREF 162   /*evglor      GLOBAL_REF  */

/*
***Nya rutiner för version 1.4 April 87    
*/

#define VPSPMT  163   /*evpspt      PSH_PMT     */
#define VPOPMT  164   /*evpopt      POP_PMT     */
#define VCRTSTR 165   /*evcrts      CRE_TSTR    */
#define VGTTSTR 166   /*evgtts      GET_TSTR    */
#define VCRMEN  167   /*evcrmy      CRE_MEN     */
#define VPSMEN  168   /*evpsmy      PSH_MEN     */
#define VPOMEN  169   /*evpomy      POP_MEN     */
#define VGTALT  170   /*evgtal      GET_ALT     */
#define VGTMEN  171   /*evgtmy      GET_MEN     */
#define VVPROD  172   /*evcrpr      VPROD       */
#define VLINPER 173   /*evlipe      LIN_PERP    */

/* 
***Nya rutiner fï¿½ databaskommunikation Maj 87 
*/

#define VDBOP   174   /*evdbop      DB_OPEN     */
#define VDBBG   175   /*evdbbg      DB_BEGIN    */
#define VDBSL   176   /*evdbsl      DB_SELECT   */
#define VDBUP   177   /*evdbup      DB_UPDATE   */
#define VDBIN   178   /*evdbin      DB_INSERT   */
#define VDBDL   179   /*evdbdl      DB_DELETE   */
#define VDBEN   180   /*evdben      DB_END      */
#define VDBRL   181   /*evdbrl      DB_ROLLBACK */
#define VDBCL   182   /*evdbcl      DB_CLOSE    */
#define VDBFI   183   /*evdbfi      DB_FETCHI   */
#define VDBFF   184   /*evdbff      DB_FETCHF   */
#define VDBFS   185   /*evdbfs      DB_FETCHS   */
#define VDBNX   186   /*evdbnx      DB_NEXT     */

/* 
***Nya rutiner fï¿½ V 1.5 
*/

#define VGTBPL  187   /*evgbpl      GETBPL      */
#define VCSYS1P 188   /*evcs1p      CSYS_1P     */

/* 
***Nya rutiner fï¿½ V 1.6 
*/

#define VPLTWI  189   /*evplwi      PLOT_WIN    */

/* 
***Nya rutiner fï¿½ V1.7 Oktober 1988 
*/

#define VUPHDR  190   /*evuhdr      UPDHDR      */
#define VLDJOB  191   /*evldjb      LOAD_JOB    */
#define VSVJOB  192   /*evsvjb      SAVE_JOB    */
#define VINPMT  193   /*evinpt      INPMT       */

/*
***Nya rutiner fï¿½ V1.8 Mars 1989 
*/

#define VHIDVI  194   /*evhdvi      HIDE_VIEW   */
#define VPRPVI  195   /*evprvi      PERP_VIEW   */
#define VTEXTL  196   /*evtxtl      TEXTL       */
#define VEXIT   197   /*evexit      EXIT        */

/*
***Nya rutiner fï¿½ V1.10 Juli 1990.
*/

#define VEXEST  198   /*evexst      EXESTAT     */
#define VAREA   199   /*evarea      AREA        */
#define VCGRAV  200   /*evcgra      CGRAV       */
#define VSETB   201   /*evsetb      SET_BASIC   */

/*
***Nya rutiner fï¿½ V1.11 1991.
*/

#define VCURC   202   /*evcurc      CUR_CONIC   */
#define VITANG  203   /*evitan      INV_TANG    */
#define VCURS   204   /*evcurs      CUR_SPLINE  */
#define VION    205   /*evion       INV_ON      */
#define VCURT   206   /*evcurt      CUR_TRIM    */
#define VCURU   207   /*evcuru      CUR_USRDEF  */
#define VCURV   208   /*evcurv      CURV        */
#define VICURV  209   /*evicur      INV_CURV    */
#define VIARCL  210   /*eviarc      INV_ARCL    */
#define VACCFNT 211   /*evacft      ACT_CFONT   */
#define VACCDSL 212   /*evacdl      ACT_CDASHL  */
#define VADDMBS 213   /*evaddm      ADD_MBS     */
#define VNINTER 214   /*evnsec      N_INTERSECT */
#define VGPMBS  215   /*evgpm       GETP_MBS    */
#define VUPMBS  216   /*evupm       UPDP_MBS    */

/*
***Nya rutiner fï¿½ ytmodulen 10/7/92 
*/

#define VSUREX  217   /*evsexd      SUR_EXDEF   */
#define VCURSIL 218   /*evcsil      CUR_SIL     */
#define VCURINT 219   /*evcint      CUR_INT     */
#define VCURISO 220   /*evciso      CUR_ISO     */
#define VNORM   221   /*evnorm      NORM        */

/*
***Nya rutiner för transformationer 10/12/92
*/

#define VTFMOV  222   /*evtrmv      TFORM_MOVE   */
#define VTFROTP 223   /*evtrrp      TFORM_ROTP   */
#define VTFROTL 224   /*evtrrl      TFORM_ROTL   */
#define VTFMIRR 225   /*evtrmi      TFORM_MIRR   */
#define VTFCOMP 226   /*evtrco      TFORM_COMP   */
#define VTFU    227   /*evtrus      TFORM_USRDEF */
#define VTCOPY  228   /*evtcop      TCOPY        */

/*
***Ny rutin fï¿½ GL 17/2/93
*/

#define VCURGL  229   /*evcugl      CUR_GL       */

/*
***Ny rutin fï¿½ SAAB 8/8/93
*/

#define VMSIZE  230   /*evmsiz      MSIZE_VIEW   */

/*
***Nya rutiner för fönsterhantering 6/12/93
*/

#define VCRWIN  231   /*evcrwi      CRE_WIN    */
#define VCREDI  232   /*evcred      CRE_EDIT   */
#define VCRICO  233   /*evcric      CRE_ICON   */
#define VCRBUT  234   /*evcrbu      CRE_BUTTON */
#define VSHWIN  235   /*evshwi      SHOW_WIN   */
#define VWTWIN  236   /*evwtwi      WAIT_WIN   */
#define VGTEDI  237   /*evgted      GET_EDIT   */
#define VUPEDI  238   /*evuped      UPD_EDIT   */
#define VGTBUT  239   /*evgtbu      GET_BUTTON */
#define VDLWIN  240   /*evdlwi      DEL_WIN    */

/*
***Nya GM-get rutiner fï¿½ kurvor och ytor. 10/1/94
*/

#define VGTCUH  241   /*evgcuh      GETCURH    */
#define VGTSEG  242   /*evgseg      GETSEG     */
#define VGTSUH  243   /*evgsuh      GETSURH    */

/*
***Nya Macro-rutiner 2/2/94.
*/

#define VUPPMBS 244   /*evuppm      UPDPP_MBS  */

/*
***Nya Ytmodul-rutiner 25/4/94.
*/

#define VSURCON 245   /*evscon      SUR_CONIC  */
#define VSUROF  246   /*evsoff      SUR_OFFS   */

/*
***Multifï¿½ster mm. 20/1-95.
*/

#define VGTVIEW 247   /*evgtvi      GET_VIEW   */
#define VDELMBS 248   /*evdelm      DEL_MBS    */
#define VACTCSY 249   /*evacsy      ACT_CSY    */

/*
***Nya rutiner fï¿½ SAAB 27/2-95.
*/

#define VMODBA  250   /*evmoba      MODE_BASIC        */
#define VSURSA  251   /*evssar      SUR_SPLARR        */
#define VSURCA  252   /*evscar      SUR_CONARR        */
#define VSURAP  253   /*evsapp      SUR_APPROX        */
#define VSURCO  254   /*evscom      SUR_COMP          */
#define VSURT   255   /*evstrm      SUR_TRIM          */
#define VGTTOP  256   /*evgtop      GETTOPP           */
#define VGTCUP  257   /*evgcub      GETCUBP           */
#define VGTRAP  258   /*evgrat      GETRATP           */
#define VCURSA  259   /*evcusa      CUR_SPLARR        */
#define VCURCA  260   /*evcuca      CUR_CONARR        */
#define VCURFA  261   /*evcfan      CUR_FANGA         */
#define VCURGD  262   /*evcged      CUR_GEODESIC      */
#define VCURAP  263   /*evcapp      CUR_APPROX        */
#define VRUNMBS 264   /*evrunm      RUN_MBS           */

/*
***Nya rutiner fï¿½ version 1.14.
*/

#define VAPID      265   /*evapid      ACT_PID           */
#define VAVVERS    266   /*evavvr      ACT_VARKON_VERS   */
#define VAVSER     267   /*evavsr      ACT_VARKON_SERIAL */
#define VAOSTYP    268   /*evaost      ACT_OSTYPE        */
#define VAHOST     269   /*evahst      ACT_HOST          */
#define VUFNAM     270   /*evufnm      UNIQUE_FILENAME   */
#define VRAND      271   /*evrand      RANDOM            */
#define VALLILS    272   /*evails      ALL_INT_LIN_SUR   */
#define VALLIOS    273   /*evaios      ALL_INV_ON_SUR    */
#define VAPPFIL    274   /*evappf      APPEND_FILE       */
#define VDELFIL    275   /*evdelf      DELETE_FILE       */
#define VMOVFIL    276   /*evmovf      MOVE_FILE         */
#define VCOPFIL    277   /*evcopf      COPY_FILE         */
#define VTSTFIL    278   /*evtstf      TEST_FILE         */
#define VGETENV    279   /*evgenv      GET_ENVIRONMENT   */
#define VGRID      280   /*evgrid      GRID_VIEW         */
#define VGRIDX     281   /*evgrix      GRIDX_VIEW        */
#define VGRIDY     282   /*evgriy      GRIDY_VIEW        */
#define VGRIDDX    283   /*evgrdx      GRIDDX_VIEW       */
#define VGRIDDY    284   /*evgrdy      GRIDDY_VIEW       */
#define VSRTGM     285   /*evsrgm      SET_ROOT_GM       */
#define VGNXGM     286   /*evgngm      GET_NEXT_GM       */

/*
***Nya rutiner fï¿½ version 1.15
*/

#define VCRFBUT    287  /*evcrfb      CRE_FBUTTON       */
#define VCRFICO    288  /*evcrfi      CRE_FICON         */
#define VGETWIN    289  /*evgtwi      GET_WIN           */
#define VTXTLW     290  /*evgttl      TEXTL_WIN         */
#define VTXTHW     291  /*evgtth      TEXTH_WIN         */
#define VLDMDF     292  /*evlmdf      LOAD_MDF          */
#define VBOX1P     293  /* ?          BOX_1P            */
#define VBOXEN     294  /* ?          BOX_ENCLOSE       */
#define VSURROT    295  /*evsrot      SUR_ROT           */
#define VCRDBUT    296  /*evcrdb      CRE_DBUTTON       */
#define VEND       297  /*evend       ENDP              */
#define VSTART     298  /*evstrt      STARTP            */
#define VLDDLL     299  /*evlddl      LOAD_DLL          */
#define VCLDLL     300  /*evcldl      CALL_DLL_FUNCTION */
#define VULDLL     301  /*evuldl      UNLOAD_DLL        */
#define VCRMAT     302  /*evcrmt      CRE_MATERIAL      */
#define VLTVIEW    303  /*evltvi      LIGHT_VIEW        */
#define VGTCOL     304  /*evgtco      GET_COLOR         */
#define VCRCOL     305  /*evcrco      CRE_COLOR         */
#define VLTON      306  /*evlton      LIGHT_ON          */
#define VLTOFF     307  /*evltof      LIGHT_OFF         */
#define VSURUSD    308  /*evsusr      SUR_USRDEF        */
#define VSURCYL    309  /*evscyl      SUR_CYL           */
#define VCLPM      310  /*evclpm      CLEAR_PM          */
#define VSURSWP    311  /*evsswp      SUR_SWEEP         */
#define VSURRUL    312  /*evsrul      SUR_RULED         */
#define VPOSMBS    313  /*evposm      POS_MBS           */

/*
***Nya rutiner fï¿½ version 1.16
*/

#define VGTFAP     314   /*evgfac      GETFACP           */
#define VSURNA     315   /*evsnar      SUR_NURBSARR      */
#define VSUAREA    316   /*evsuar      SURFACE_AREA      */
#define VSEAREA    317   /*evsear      SECTION_AREA      */
#define VGETWDT    318   /*evgwdt      GETWIDTH          */
#define VACTWDT    319   /*evawdt      ACT_WIDTH         */
#define VSCR_2     320   /*evscr2      SCREEN_2          */
#define VPOS_2     321   /*evpos2      POS_2             */
#define VIDNT_2    322   /*evidn2      IDENT_2           */
#define VHIGHL     323   /*evhigh      HIGHLIGHT         */
#define VGTPTR     324   /*evgptr      GET_POINTER       */
#define VINFNM     325   /*evinfn      INFNAME           */
#define VLOADPM    326   /*evldpm      LOAD_PM           */
#define VALLILM    327   /*evailm      ALL_INT_LIN_MSUR  */
#define VCALEXT    328   /*evcaxt      CALL_EXTERN       */

/*
***Nya rutiner fï¿½ version 1.17
*/

#define VIVAL      329  /*evival      IVAL              */
#define VFVAL      330  /*evfval      FVAL              */
#define VODBCCO    331  /*evodco      ODBC_CONNECT      */
#define VODBCED    332  /*evoded      ODBC_EXECDIRECT   */
#define VODBCFE    333  /*evodfe      ODBC_FETCH        */
#define VODBCGS    334  /*evodgs      ODBC_GETSTRING    */
#define VODBCDC    335  /*evoddc      ODBC_DISCONNECT   */
#define VODBCER    336  /*evoder      ODBC_ERROR        */
#define VPUTDAT    337  /*evpdat      PUTDAT            */
#define VGETDAT    338  /*evgdat      GETDAT            */
#define VDELDAT    339  /*evddat      DELDAT            */
#define VCRAYNPES  340  /*evcrnp      CRAY_NUM_PES      */
#define VCRAYMYPE  341  /*evcrmp      CRAY_MY_PE        */
#define VSECGRAV   342  /*evsecg      SECTION_CGRAV     */

/*
***New routines by Orebro University version 1.17D
*/

#define VSURCURV   343  /*evscur      SUR_CURVES        */
#define VCURNA     344  /*evcuna      CUR_NURBSARR      */

/*
***New routines for TCP/IP, ï¿½ebro university.
*/

#define VTCPCO     345  /*evtcco      TCP_CONNECT       */
#define VTCPLI     346  /*evtcli      TCP_LISTEN        */
#define VTCPCL     347  /*evtccl      TCP_CLOSE         */
#define VTCPOI     348  /*evtcoi      TCP_OUTINT        */
#define VTCPII     349  /*evtcii      TCP_ININT         */
#define VTCPOF     350  /*evtcof      TCP_OUTFLOAT      */
#define VTCPIF     351  /*evtcif      TCP_INFLOAT       */
#define VTCPOC     352  /*evtcoc      TCP_OUTCHAR       */
#define VTCPIC     353  /*evtcic      TCP_INCHAR        */
#define VTCPGL     354  /*evtcgl      TCP_GETLOCAL      */
#define VTCPGR     355  /*evtcgt      TCP_GETREMOTE     */
#define VTCPOV     356  /*evtcov      TCP_OUTVEC        */
#define VTCPIV     357  /*evtciv      TCP_INVEC         */

/*
***New routines for Robot Application Protocol (RAP)
***by Orebro University
*/


#define VRPCCC     358  /*evrpcc      RPC_CLIENTCREATE  */
#define VRPCCD     359  /*evrpcd      RPC_CLIENTDESTROY */
#define VRAPOF     360  /*evrpof      RAP_OUTFLOAT      */
#define VRAPIF     361  /*evrpif      RAP_INFLOAT       */
#define VRAPOI     362  /*evrpoi      RAP_OUTINT        */
#define VRAPII     363  /*evrpii      RAP_ININT         */
#define VRAPORT    364  /*evrport     RAP_OUTROBTARGET  */
#define VRAPIRT    365  /*evrpirt     RAP_INROBTARGET   */
#define VRAPOS     366  /*evrpos      RAP_OUTSPEED      */
#define VRAPIS     367  /*evrpis      RAP_INSPEED       */
#define VRAPORTA   368  /*evrpora     RAP_OUTROBTARR    */
#define VRAPIRTA   369  /*evrpira     RAP_INROBTARR     */
#define VORURSTD   370  /*evorrstd    ORU_RSTIMEDIFF    */
#define VRAPOFA    371  /*evrpofa     RAP_OUTFLOATARR   */
#define VRAPIFA    372  /*evrpifa     RAP_INFLOATARR    */

/*
***Mesh, Örebro university 2004-07-08
*/
#define VMSHARR    373  /* evmsar     MESH_ARR          */
#define VGTMSHH    374  /* evgmsh     GETMSHH           */
#define VGTVRTX    375  /* evgvrt     GETVERTEX         */
#define VGTHEDG    376  /* evghdg     GETHEDGE          */
#define VGTFACE    377  /* evgface    GETFACE           */
#define VSRTMSH    378  /* evsrms     SORT_MESH         */
#define VGTXFLT    379  /* evgxflt    GETXFLOAT         */

#define VTMULT     380  /* evtmult    TFORM_MULT        */
#define VTINV      381  /* evtinv     TFORM_INV         */

#define VCCMPARR   382  /* evccmparr  CUR_COMPARR       */
#define VSURTUSRD  383  /* evstusr    SUR_TRIM_USRDEF   */

/*
***New routines for 1.19
*/
#define VACPFNT    384  /* evapft     ACT_PFONT         */
#define VACPSIZ    385  /* evapsi     ACT_PSIZE         */
#define VBLLEVEL   386  /* evbllv     BLANK_LEVEL       */
#define VUBLEVEL   387  /* evublv     UNBLANK_LEVEL     */
#define VGTLEVEL   388  /* evgtlv     GET_LEVEL         */
#define VACLEVEL   389  /* evalev     ACT_LEVEL         */
#define VNMLEVEL   390  /* evnmlv     NAME_LEVEL        */

/*
***If you are building your own application,
***include your own file here.
*/
#else
#include "my_mbsrout.h"
#endif
