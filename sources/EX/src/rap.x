/******************************************************************************
   Mycket av detta är utlurat indirekt från ABB's dokumentation
   En del datatyper i unionen RAPVAR_DATA_TYPE har utelämnats
   eftersom de ändå inte behövs i vårt fall. Då blir storleken
   på vår union inte identisk med den på servern (roboten), men det gör
   inget eftersom RPC ändå bara skickar storleken på den använda datatypen
   Några datatyper är inte inskrivna, medans andra bara är
   bortkommenterade. Lite debug kan behövas om man avkommenterar dem.

   Denna kod ska köras med rpcgen för att få:

   oru_rap_clnt.c
   oru_rap_.h
   oru_rap_xdr.c

   Som i sin tur används för att bygga progarmmet. 
******************************************************************************/


/*  RAP_BYTE_TYPE  */
typedef u_char RAP_BYTE_TYPE;

/*  RAP_USHORT_TYPE  */
typedef u_short RAP_USHORT_TYPE;

/*  RAP_SHORT_TYPE  */
typedef short RAP_SHORT_TYPE;

/*  RAP_BOOL_TYPE  */
typedef int RAP_BOOL_TYPE;

/*  RAP_FLOAT_TYPE  */
typedef float RAP_FLOAT_TYPE;

/*  RAP_LONG_TYPE  */
typedef long RAP_LONG_TYPE;

/*  RAP_STATUS_TYPE  */
typedef long RAP_STATUS_TYPE;

/*  RAP_STRING_TYPE  */
typedef string RAP_STRING_TYPE<80>;

/*  FILE_DESCRIPTOR  */
typedef long FILE_DESCRIPTOR;

/* RAPVAR_LONG_TYPE */
typedef long RAPVAR_LONG_TYPE;

/*  RAPVAR_NAME_TYPE  */
typedef RAP_USHORT_TYPE RAPVAR_DOMAIN_TYPE;
struct RAPVAR_NAME_TYPE {
                    RAP_STRING_TYPE name;
                    RAP_STRING_TYPE type;
                    RAPVAR_DOMAIN_TYPE domain;
                    RAP_SHORT_TYPE number1;
                    RAP_SHORT_TYPE number2;
};
typedef struct RAPVAR_NAME_TYPE RAPVAR_NAME_TYPE;






/*  RAP_HEAD_TYPE  */
struct RAP_HEAD_TYPE {
long userdef;
long data1;
};
typedef struct RAP_HEAD_TYPE RAP_HEAD_TYPE;


/* ******************* DATA *****************************  */

/*  RAPVAR_NUM_TYPE  */
typedef RAP_FLOAT_TYPE RAPVAR_NUM_TYPE;

/*  RAPVAR_STRING_TYPE  */
typedef RAP_STRING_TYPE RAPVAR_STRING_TYPE;

/*  RAPVAR_BOOL_TYPE  */
typedef RAP_FLOAT_TYPE RAPVAR_BOOL_TYPE;

/* eget tillägg, fanns ej angivet i dokumentation (ej hittat i alla fall)*/
typedef int RAPVAR_NUMBER_TYPE;

typedef RAP_SHORT_TYPE RAPVAR_SHORT_TYPE;





/*  RAPVAR_ARRAY_DIM_TYPE  */
struct RAPVAR_ARRAY_DIM_TYPE {
                    RAPVAR_SHORT_TYPE dimension;
                    RAPVAR_SHORT_TYPE elements[6];
};
typedef struct RAPVAR_ARRAY_DIM_TYPE RAPVAR_ARRAY_DIM_TYPE;










/*  RAPVAR_CONFDATA_TYPE  */
struct RAPVAR_CONFDATA_TYPE {
RAPVAR_NUM_TYPE cf1;
RAPVAR_NUM_TYPE cf4;
RAPVAR_NUM_TYPE cf6;
RAPVAR_NUM_TYPE cfx;
};
typedef struct RAPVAR_CONFDATA_TYPE RAPVAR_CONFDATA_TYPE;

/*  RAPVAR_EXTJOINT_TYPE  */
struct RAPVAR_EXTJOINT_TYPE {
RAPVAR_NUM_TYPE eax_a;
RAPVAR_NUM_TYPE eax_b;
RAPVAR_NUM_TYPE eax_c;    
RAPVAR_NUM_TYPE eax_d;
RAPVAR_NUM_TYPE eax_e;
RAPVAR_NUM_TYPE eax_f;
};
typedef struct RAPVAR_EXTJOINT_TYPE RAPVAR_EXTJOINT_TYPE;

/*  RAPVAR_LOADDATA_TYPE
struct RAPVAR_LOADDATA_TYPE {
RAPVAR_NUM_TYPE mass;
RAPVAR_POS_TYPE cog;
RAPVAR_ORIENT_TYPE aom;
RAPVAR_NUM_TYPE ix;
RAPVAR_NUM_TYPE iy;
RAPVAR_NUM_TYPE iz;
};
typedef struct RAPVAR_LOADDATA_TYPE RAPVAR_LOADDATA_TYPE;
*/

/*  RAPVAR_MOTSETDATA_TYPE
struct RAPVAR_ACCDATA_TYPE {
RAPVAR_NUM_TYPE acc;
RAPVAR_NUM_TYPE ramp;
};
typedef struct RAPVAR_ACCDATA_TYPE RAPVAR_ACCDATA_TYPE;
struct RAPVAR_VELDATA_TYPE {
RAPVAR_NUM_TYPE oride;
RAPVAR_NUM_TYPE max_vel;
};
typedef struct RAPVAR_VELDATA_TYPE RAPVAR_VELDATA_TYPE;
struct RAPVAR_SINGDATA_TYPE {
RAPVAR_BOOL_TYPE wrist;
RAPVAR_BOOL_TYPE arm;
RAPVAR_BOOL_TYPE base;
};
typedef struct RAPVAR_SINGDATA_TYPE RAPVAR_SINGDATA_TYPE;
struct RAPVAR_CONFSUPDATA_TYPE {
RAPVAR_BOOL_TYPE jsup;
RAPVAR_BOOL_TYPE lsup;
RAPVAR_NUM_TYPE ax1;
RAPVAR_NUM_TYPE ax4;
RAPVAR_NUM_TYPE ax6;
};
typedef struct RAPVAR_CONFSUPDATA_TYPE RAPVAR_CONFSUPDATA_TYPE;
struct RAPVAR_GRIPDATA_TYPE {
RAPVAR_LOADDATA_TYPE load;
RAPVAR_NUM_TYPE acc;
};
typedef struct RAPVAR_GRIPDATA_TYPE RAPVAR_GRIPDATA_TYPE;
struct RAPVAR_MOTSETDATA_TYPE {
RAPVAR_VELDATA_TYPE vel;
RAPVAR_ACCDATA_TYPE acc;
RAPVAR_SINGDATA_TYPE sing;strdata
RAPVAR_CONFSUPDATA_TYPE conf;
RAPVAR_GRIPDATA_TYPE grip;
};
typedef struct RAPVAR_MOTSETDATA_TYPE RAPVAR_MOTSETDATA_TYPE;
 */


 
/*  RAPVAR_ORIENT_TYPE  */
struct RAPVAR_ORIENT_TYPE {
RAPVAR_NUM_TYPE q1;
RAPVAR_NUM_TYPE q2;
RAPVAR_NUM_TYPE q3;
RAPVAR_NUM_TYPE q4;
};
typedef struct RAPVAR_ORIENT_TYPE RAPVAR_ORIENT_TYPE;

/*  RAPVAR_POS_TYPE  */
struct RAPVAR_POS_TYPE {
RAPVAR_NUM_TYPE x;
RAPVAR_NUM_TYPE y;
RAPVAR_NUM_TYPE z;
};
typedef struct RAPVAR_POS_TYPE RAPVAR_POS_TYPE;

/*  RAPVAR_POSE_TYPE  */
struct RAPVAR_POSE_TYPE {
RAPVAR_POS_TYPE trans;
RAPVAR_ORIENT_TYPE rot;
};
typedef struct RAPVAR_POSE_TYPE RAPVAR_POSE_TYPE;

/*  RAPVAR_PROGDISP_TYPE
struct RAPVAR_PROGDISP_TYPE {
RAPVAR_POSE_TYPE pdisp;
RAPVAR_EXTJOINT_TYPE eoffs;
};
typedef struct RAPVAR_PROGDISP_TYPE RAPVAR_PROGDISP_TYPE;
*/

/*  RAPVAR_ROBTARGET_TYPE  */
struct RAPVAR_ROBTARGET_TYPE {
RAPVAR_POS_TYPE trans;
RAPVAR_ORIENT_TYPE rot;
RAPVAR_CONFDATA_TYPE robconf;
RAPVAR_EXTJOINT_TYPE extax;
};
typedef struct RAPVAR_ROBTARGET_TYPE RAPVAR_ROBTARGET_TYPE;

/*  RAPVAR_SPEEDDATA_TYPE  */
struct RAPVAR_SPEEDDATA_TYPE {
RAPVAR_NUM_TYPE v_tcp;
RAPVAR_NUM_TYPE v_ori;
RAPVAR_NUM_TYPE v_leax;
RAPVAR_NUM_TYPE v_reax;
};
typedef struct RAPVAR_SPEEDDATA_TYPE RAPVAR_SPEEDDATA_TYPE;

/*  RAPVAR_TOOLDATA_TYPE
struct RAPVAR_TOOLDATA_TYPE {
RAPVAR_BOOL_TYPE robhold;
RAPVAR_POSE_TYPE tframe;
RAPVAR_LOADDATA_TYPE tload;
};
typedef struct RAPVAR_TOOLDATA_TYPE RAPVAR_TOOLDATA_TYPE;
*/

/*  RAPVAR_WOBJDATA_TYPE  */
struct RAPVAR_WOBJDATA_TYPE {
RAPVAR_BOOL_TYPE robhold;
RAPVAR_BOOL_TYPE ufprog;
RAPVAR_STRING_TYPE ufmec;
RAPVAR_POSE_TYPE uframe;
RAPVAR_POSE_TYPE oframe;
};
typedef struct RAPVAR_WOBJDATA_TYPE RAPVAR_WOBJDATA_TYPE;

/*  RAPVAR_ZONEDATA_TYPE  */
struct RAPVAR_ZONEDATA_TYPE {
RAPVAR_BOOL_TYPE finep;
RAPVAR_NUM_TYPE pzone_tcp;
RAPVAR_NUM_TYPE pzone_ori;
RAPVAR_NUM_TYPE pzone_eax;
RAPVAR_NUM_TYPE zone_ori;
RAPVAR_NUM_TYPE zone_leax;
RAPVAR_NUM_TYPE zone_reax;
};
typedef struct RAPVAR_ZONEDATA_TYPE RAPVAR_ZONEDATA_TYPE;

/*  RAPVAR_EIODATA_TYPE
struct RAPVAR_EIODATA_TYPE {
RAP_STRING_TYPE signal;
RAP_FLOAT_TYPE level1;
RAP_FLOAT_TYPE level2;
};
typedef struct RAPVAR_EIODATA_TYPE RAPVAR_EIODATA_TYPE;
*/

/*  RAPVAR_CTLVERSDATA_TYPE
struct RAPVAR_CTLVERSDATA_TYPE {
RAP_STRING_TYPE sysversion;
/* Product ÍD
RAP_STRING_TYPE bootversion;
/* Robot type
RAP_STRING_TYPE rapversion;
RAP_STRING_TYPE tpuversion;
};
typedef struct RAPVAR_CTLVERSDATA_TYPE RAPVAR_CTLVERSDATA_TYPE;
*/

/*  RAPVAR_CTLIDDATA_TYPE
struct RAPVAR_CTLIDDATA_TYPE {
RAP_STRING_TYPE id;
};
typedef struct RAPVAR_CTLIDDATA_TYPE RAPVAR_CTLIDDATA_TYPE;
*/

/*  RAPVAR_SYSSTDATA_TYPE
struct RAPVAR_SYSSTDATA_TYPE {
RAP_LONG_TYPE ctlstate;
RAP_LONG_TYPE oprstate;
RAP_LONG_TYPE pgmctlnum;
RAP_LONG_TYPE pgmctlstate;
RAP_LONG_TYPE pgmstate;
RAP_LONG_TYPE pgmfree;
};
typedef struct RAPVAR_SYSSTDATA_TYPE RAPVAR_SYSSTDATA_TYPE;
*/

/*  RAPVAR_PGMDATA_TYPE
struct RAPVAR_PGMDATA_TYPE {
RAP_LONG_TYPE pgmctlstate;
RAP_LONG_TYPE pgmstate;
RAP_LONG_TYPE pgmfree;
};
typedef struct RAPVAR_PGMDATA_TYPE RAPVAR_PGMDATA_TYPE;
*/

/*  RAPVAR_CURR_ROBTARGET_TYPE  */
struct RAPVAR_CURR_ROBTARGET_TYPE {
RAPVAR_ROBTARGET_TYPE robtarget;
RAPVAR_STRING_TYPE wobj;
RAPVAR_STRING_TYPE toolobj;
};
typedef struct RAPVAR_CURR_ROBTARGET_TYPE RAPVAR_CURR_ROBTARGET_TYPE;

/*  RAPVAR_CLOCKDATA_TYPE
struct RAPVAR_CLOCKDATA_TYPE {
RAP_RTC_TM rtc_tm;
RAP_STRING_TYPE rap_time;
};
typedef struct RAPVAR_CLOCKDATA_TYPE RAPVAR_CLOCKDATA_TYPE;
*/

/*  RAP_PGMINFODATA_TYPE
struct RAPVAR_TASKINFO_TYPE {
RAPVAR_LONG_TYPE progno;
RAP_STRING_TYPE task_name;
RAPVAR_LONG_TYPE pgmstate;
RAPVAR_LONG_TYPE taskstate;
RAP_STRING_TYPE main_mod;
RAP_STRING_TYPE main_rout;
RAPVAR_LONG_TYPE type;
RAPVAR_LONG_TYPE tot_mem;
RAPVAR_LONG_TYPE proc_free;
};
typedef struct RAPVAR_TASKINFO_TYPE RAPVAR_TASKINFO_TYPE;
*/

/*
typedef struct {
u_int RAPVAR_PGMINFODATA_TYPE_len;
RAPVAR_TASKINFO_TYPE
*RAPVAR_PGMINFODATA_TYPE_val;
} RAPVAR_PGMINFODATA_TYPE;
*/

/*  RAPVAR_SUBSCWRITE_TYPE
struct RAPVAR_SUBSCWRITE_TYPE {
RAPVAR_SPONDEF_TYPE def;
};
typedef struct RAPVAR_SUBSCWRITE_TYPE RAPVAR_SUBSCWRITE_TYPE;
*/

/*  RAPVAR_DATA_TYPE  */
union RAPVAR_DATA_TYPE switch(RAPVAR_NUMBER_TYPE var_type){
case 1:
RAPVAR_NUM_TYPE num;
case 2:
RAP_LONG_TYPE raplong;
case 3:
RAPVAR_STRING_TYPE charstring;
case 4:
RAPVAR_BOOL_TYPE boolean;

case 7:
RAPVAR_CONFDATA_TYPE confdata;
case 8:
RAPVAR_EXTJOINT_TYPE extjoint;
/*
case 9:
RAPVAR_LOADDATA_TYPE loaddata;
case 10:
RAPVAR_MOTSETDATA_TYPE motsetdata;
*/
case 11:
RAPVAR_ORIENT_TYPE orient;
case 12:
RAPVAR_POS_TYPE pos;
case 13:
RAPVAR_POSE_TYPE pose;
/*
case 14:
RAPVAR_PROGDISP_TYPE progdisp;
*/
case 15:
RAPVAR_ROBTARGET_TYPE robtarget;
case 16:
RAPVAR_SPEEDDATA_TYPE speeddata;
/*
case 17:
RAPVAR_TOOLDATA_TYPE tooldata;
*/
case 18:
RAPVAR_WOBJDATA_TYPE wobjdata;
case 19:
RAPVAR_ZONEDATA_TYPE zonedata;
/*
case 21:
RAPVAR_EIODATA_TYPE eiodata;
case 22:
RAPVAR_CTLVERSDATA_TYPE ctlversdata;
case 23:
RAPVAR_CTLIDDATA_TYPE ctliddata;
case 24:
RAPVAR_SYSSTDATA_TYPE sysstdata;
case 25:
RAPVAR_PGMDATA_TYPE pgmstatedata;
*/

case 26:
RAPVAR_CURR_ROBTARGET_TYPE robposdata;
/*
case 27:
RAPVAR_SUBLOG_TYPE sublogdata;
case 28:
RAPVAR_SUBEIO_TYPE subeiodata;
case 29:
RAP_INTARRAY_TYPE intarray;
case 30:
RAP_CLOCKDATA_TYPE clockdata;
/*
case 31:
RAPVAR_PGMINFODATA_TYPE pgminfo;
case 32:
RAPVAR_SUBSCWRITE_TYPE scwritesub;
*/
};
/*typedef union RAPVAR_DATA_TYPE RAPVAR_DATA_TYPE;*/



struct RAPVAR_XDATA_TYPE {
  RAP_FLOAT_TYPE datacfg<>;
  RAP_FLOAT_TYPE numdata<>;
  RAP_STRING_TYPE strdata<>;
  };
typedef struct RAPVAR_XDATA_TYPE RAPVAR_XDATA_TYPE;
  
typedef RAPVAR_XDATA_TYPE RAPVAR_ARRAY_DATA_TYPE<500>; 




/******************** RAPVAR_READ_NAME (1001)**********************************/
/* request parameters */
struct RAPVAR_READ_NAME_REQ_TYPE{
   RAP_HEAD_TYPE head;
   RAPVAR_NAME_TYPE var;
};
typedef struct RAPVAR_READ_NAME_REQ_TYPE RAPVAR_READ_NAME_REQ_TYPE;

/* response parameters */
union RAPVAR_READ_NAME_RESPX_TYPE switch (RAP_STATUS_TYPE status){
                       case 0:
                            RAPVAR_DATA_TYPE data;
                       default:
                            void;
                        } ;
typedef union RAPVAR_READ_NAME_RESPX_TYPE RAPVAR_READ_NAME_RESPX_TYPE;
struct RAPVAR_READ_NAME_RESP_TYPE {
                   RAP_HEAD_TYPE head;
                   RAPVAR_READ_NAME_RESPX_TYPE resp;
};
typedef struct RAPVAR_READ_NAME_RESP_TYPE RAPVAR_READ_NAME_RESP_TYPE;


/******************** RAPVAR_WRITE_NAME (1002)*********************************/
/* request parameters */
struct RAPVAR_WRITE_NAME_REQ_TYPE{
   RAP_HEAD_TYPE head;
   RAPVAR_NAME_TYPE var;
   RAPVAR_DATA_TYPE data;
};
typedef struct RAPVAR_WRITE_NAME_REQ_TYPE RAPVAR_WRITE_NAME_REQ_TYPE;

/* response parameters */
struct RAPVAR_WRITE_NAME_RESP_TYPE{
   RAP_HEAD_TYPE head;
   RAP_STATUS_TYPE status;
};
typedef struct RAPVAR_WRITE_NAME_RESP_TYPE RAPVAR_WRITE_NAME_RESP_TYPE;




/******************** RAPVAR_READ_NAME_ARRAY (1004)**********************************/
/* request parameters */
struct RAPVAR_READ_NAME_ARRAY_REQ_TYPE{
   RAP_HEAD_TYPE head;
   RAPVAR_NAME_TYPE var;
   RAPVAR_ARRAY_DIM_TYPE dim;
};
typedef struct RAPVAR_READ_NAME_ARRAY_REQ_TYPE RAPVAR_READ_NAME_ARRAY_REQ_TYPE;

/* response parameters */
union RAPVAR_READ_NAME_ARRAY_RESPX_TYPE switch (RAP_STATUS_TYPE status){
                       case 0:
                            RAPVAR_ARRAY_DATA_TYPE data;
                       default:
                            void;
                        } ;
typedef union RAPVAR_READ_NAME_ARRAY_RESPX_TYPE RAPVAR_READ_NAME_ARRAY_RESPX_TYPE;

struct RAPVAR_READ_NAME_ARRAY_RESP_TYPE {
                   RAP_HEAD_TYPE head;
                   RAPVAR_READ_NAME_ARRAY_RESPX_TYPE resp;
                   };
typedef struct RAPVAR_READ_NAME_ARRAY_RESP_TYPE RAPVAR_READ_NAME_ARRAY_RESP_TYPE;



/******************** RAPVAR_WRITE_NAME_ARRAY (1005)*********************************/
/* request parameters */
struct RAPVAR_WRITE_NAME_ARRAY_REQ_TYPE{
   RAP_HEAD_TYPE head;
   RAPVAR_NAME_TYPE var;
   RAPVAR_ARRAY_DIM_TYPE dim;
   RAPVAR_ARRAY_DATA_TYPE data;
};
typedef struct RAPVAR_WRITE_NAME_ARRAY_REQ_TYPE RAPVAR_WRITE_NAME_ARRAY_REQ_TYPE;

/* response parameters */
struct RAPVAR_WRITE_NAME_ARRAY_RESP_TYPE{
   RAP_HEAD_TYPE head;
   RAP_STATUS_TYPE status;
};
typedef struct RAPVAR_WRITE_NAME_ARRAY_RESP_TYPE RAPVAR_WRITE_NAME_ARRAY_RESP_TYPE;



/************ DEFINITION OF SERVER PROGRAM AND PROCEDURES  *******************/
program OTHER_SERVICES{
   version OTHER_SERVICES_VERS{
      RAPVAR_READ_NAME_RESP_TYPE rapvar_read_name(RAPVAR_READ_NAME_REQ_TYPE)=1001;
      RAPVAR_WRITE_NAME_RESP_TYPE rapvar_write_name(RAPVAR_WRITE_NAME_REQ_TYPE)=1002;
      RAPVAR_READ_NAME_ARRAY_RESP_TYPE rapvar_read_name_array(RAPVAR_READ_NAME_ARRAY_REQ_TYPE)=1004;
      RAPVAR_WRITE_NAME_ARRAY_RESP_TYPE rapvar_write_name_array(RAPVAR_WRITE_NAME_ARRAY_REQ_TYPE)=1005;
   }=1;
}=300456;



