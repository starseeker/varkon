/**********************************************************************
*
*    wpdef.h
*    =======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.tech.oru.se/cad/varkon
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
***********************************************************************/

#include <GL/glx.h>

/*
***Max number of windows in wpwtab.
*/
#define WTABSIZ 100

/*
***Window types.
*/
#define TYP_UNDEF   0            /* Undefined */
#define TYP_IWIN    1            /* Input window */
#define TYP_EDIT    2            /* Edit */
#define TYP_ICON    3            /* Icon */
#define TYP_BUTTON  4            /* Button */
#define TYP_LWIN    5            /* List window */
#define TYP_GWIN    6            /* X11 Graphical window */
#define TYP_RWIN    7            /* OpenGL Graphical window */
#define TYP_MCWIN   8            /* Message and Command window */

/*
***Colors.
*/
#define WP_NPENS 256             /* Number of normal pens */
#define WP_SPENS 9               /* Number of system pens */
#define WP_BGND1 WP_NPENS        /* Background for WPIWIN's, varkon.background */
#define WP_BGND2 WP_NPENS+1      /* Background for WPBUTT's, varkon.buttonColor */
#define WP_BGND3 WP_NPENS+2      /* Background for "selected" WPBUTT's, varkon.selectedButtonColor */
#define WP_FGND  WP_NPENS+3      /* Foreground, varkon.foreground */
#define WP_TOPS  WP_NPENS+4      /* Top shadow, varkon.topShadowColor */
#define WP_BOTS  WP_NPENS+5      /* Bottom shadow, varkon.bottomShadowColor */
#define WP_NOTI  WP_NPENS+6      /* Button highlight, varkon.highlightedButtonColor */
#define WP_TTIP  WP_NPENS+7      /* Tooltip background, varkon.tooltipColor */
#define WP_ENTHG WP_NPENS+8      /* Entity highlight, varkon.highlightedEntityColor */

/*
***Rubberband modes.
*/
#define WP_RUB_NONE    0         /* Nothing */
#define WP_RUB_RECT    1         /* Rectangle */
#define WP_RUB_LINE    2         /* Line */
#define WP_RUB_ARROW   3         /* Arrow and ring */

/*
***Codes to use with WPwwtw() for event loop service level.
*/
#define SLEVEL_ALL    0          /* All events returned served */
#define SLEVEL_MBS    1          /* Input from MBS, ie. WAIT_WIN(...) */
#define SLEVEL_V3_INP 2          /* Input from Varkon, ie. WPmsip() */
#define SLEVEL_NONE   3          /* All events served locally */

/*
***Defs for keyboard input. A key press can be interpreted
***as a symbol [1-14] or as a character. Mapping is done
***by the keypress function of the WPEDIT.
*/ 
#define SMBNONE     -1          /* No symbol and no character */
#define SMBCHAR      0          /* No symbol but a character */
#define SMBRETURN    1          /* Symbol "Return" */
#define SMBBACKSP    2          /* Symbol "Backspace" */
#define SMBDELETE    3          /* Symbol "Delete" */
#define SMBLEFT      4          /* Symbol "Left" */
#define SMBRIGHT     5          /* Symbol "Right" */
#define SMBUP        6          /* Symbol "Up" */
#define SMBDOWN      7          /* Symbol "Down" */
#define SMBESCAPE    8          /* Symbol "Escape" */
#define SMBMAIN      9          /* Symbol "Main menu" */
#define SMBHELP     10          /* Symbol "Help" */
#define SMBPOSM     11          /* Symbol "Pos menu" */
#define SMBALT      12          /* Symbol "Menu alternative" */
#define SMBENDL     13          /* Symbol "End of line" */
#define SMBBEGL     14          /* Symbol "Beginning of line" */

/*
***Graphical windows.
*/
#define GWIN_MAIN     0          /* Window ID for main WPGWIN */
#define GWIN_ALL     -1          /* All WPGWIN's */
#define RWIN_ALL     -1          /* All WPRWIN's */

/*
***Level handling.
*/
#define WP_BLANKL     1          /* Blank levels */
#define WP_UBLANKL    2          /* Unblank levels */
#define WP_LISTL      3          /* List levels */

/*
***Status codes for points in polylines.
*/
#define INVISIBLE 0              /* Invisible */
#define VISIBLE   1              /* Visible */
#define ENDSIDE   2              /* End */

/*
***Codes for coordinate system status.
*/
#define V3_CS_NORMAL  0          /* Not active */
#define V3_CS_ACTIVE  1          /* Active */

/*
**Window-ID.
*/
typedef DBint  wpw_id;          /* A window ID is an int */

typedef struct
{
wpw_id     w_id;               /* Local ID, index in parent wintab */
wpw_id     p_id;               /* Parent ID */
Window     x_id;               /* X-window ID */
} WPWID;

/*
***Window geometry.
*/
typedef struct
{
short      x;                  /* Läge X-koordinat */
short      y;                  /* Läge Y-koordinat */
short      dx;                 /* Storlek i X-led */
short      dy;                 /* Storlek i Y-led */
short      bw;                 /* Ramens tjocklek */
double     psiz_x;             /* Pixelstorlek i X-led i mm. */
double     psiz_y;             /* Pixelstorlek i Y-led i mm. */
} WPWGEO;

/*
***Window colors.
*/
typedef struct
{
short      bckgnd;             /* Bakgrundsfärg */
short      forgnd;             /* Förgrundsfärg */
} WPWCOL;

/*
***A window.
*/
typedef struct
{
char       typ;                /* Typ av fönster, tex. TYP_IWIN */
char      *ptr;                /* C-pekare till en fönster-structure */
} WPWIN;

/*
***A graphical coordinate.
*/
typedef struct
{
double     x;                  /* X-koordinat */
double     y;                  /* Y-koordinat */
double     z;                  /* Z-koordinat */
char       a;                  /* Tänd/släck + start/slut */
} WPGRPT;

/*
***The model/screen window of a graphical view.
*/
typedef struct
{
double     xmin,xmax;          /* Gränser i X-led */
double     ymin,ymax;          /* Gränser i Y-led */
} VYWIN;

/*
***A view transformation.
*/
typedef struct
{
double     k11,k12,k13;        /* 1:a raden */
double     k21,k22,k23;        /* 2:a raden */
double     k31,k32,k33;        /* 3:e raden */
} VYTRF;

/*
***A graphical view. Used by WPGWIN, WPRWIN and wpviewtab[].
*/
#define    WPVNLN        20    /* Max view name length */
#define    WPMAXVIEWS    40    /* Max number of views in wpviewtab[] */
#define    VIEW_NOT_USED  0    /* View not used */
#define    VIEW_3D_ONLY   1    /* Only name and matrix defined */
#define    VIEW_3D_AND_2D 2    /* View fully defined */

typedef struct
{
char       name[WPVNLN+1];     /* View name + NULL */
VYTRF      matrix;             /* View transformation matrix */
double     pdist;              /* Perspective distance or 0 */
double     k1x,k2x,k1y,k2y;    /* Viewport transformation */
VYWIN      scrwin;             /* Screen window */
VYWIN      modwin;             /* Model window */
int        status;             /* Not/Partly or fully defined */
} WPVIEW;

/*
***A DisplayFile record. 
*/
typedef union
{
struct
  {                            /* A Header */ 
  short type;                  /* Type */
  int   nvec;                  /* Number of vectors */
  char  hili;                  /* Highlight TRUE/FALSE */
  } hdr;
DBptr la;                      /* DB-pointer or -1 = Deleted */
struct
  {                            /* A Vector */
  short x;                     /* X-position */
  short y;                     /* Y-position */
  char  a;                     /* Status */
  } vec;
} DFPOST;

/*
***A Button window.
*/
typedef struct
{
WPWID      id;                 /* ID */
WPWGEO     geo;                /* Geometri */
WPWCOL     color;              /* Färger */
short      font;               /* Fontnummer */
char       stron[81];          /* Text  för knapp på */
char       stroff[81];         /* Text  för knapp av */
bool       status;             /* Logiskt tillstånd, default = FALSE */
short      acttyp;             /* Aktionstyp, MENU/FUNC etc. */
short      actnum;             /* Aktionens nummer */
bool       hlight;             /* Highlight, TRUE/FALSE */
char       tt_str[81];         /* Tooltip string */
} WPBUTT;

/*
***An Edit window.
*/
#define    FIRST_EDIT    1     /* 1:a edit-fönstret */
#define    NEXT_EDIT     2     /* Nästa edit-fönster */
#define    PREV_EDIT     3     /* Föregående edit-fönster */
#define    FOCUS_EDIT    4     /* Aktiva edit-fönstret */
#define    LAST_EDIT     5     /* Sista edit-fönstret */

typedef struct
{
WPWID      id;                 /* ID */
WPWGEO     geo;                /* Geometri */
char       str[V3STRLEN+1];    /* Edit-sträng */
char       dstr[V3STRLEN+1];   /* Default-sträng */
DBint      tknmax;             /* Max antal tecken */
DBint      scroll;             /* Scroll-position */
DBint      curpos;             /* Cursor-position */
bool       fuse;               /* Default-säkring */
bool       focus;              /* TRUE = Input focus */
int        symbol;             /* Senast inmatade symbol */
char       tt_str[81];         /* Tooltip string */
} WPEDIT;

/*
***A Message and Command window.
*/
#define WP_MESSAGE   0         /* An informational message */
#define WP_ERROR     1         /* An error message */
#define WP_PROMPT    2         /* A prompt */
#define WP_CLEARED   3         /* An empty line */

typedef struct
{
wpw_id     parent_id;          /* ID of parent WPGWIN or WPRWIN */
WPWGEO     geo;                /* Overall geometry (5 pix resize not included) */
Window     messcom_xid;        /* X-ID of Message and Command window*/
Window     resize_xid;         /* X-ID of resize window */
WPEDIT    *cmdptr;             /* C ptr to command edit */

} WPMCWIN;
/*
***Graphical windows.
*/
#define WP_GWMAX     25        /* Max number of WPGWIN/WPRWIN windows */
#define WP_GWSMAX    100       /* Max number of child windows WPGWIN/WPRWIN */
#define WP_RWSMAX    WP_GWSMAX /* Same for WPRWIN's */
#define WP_NIVANT    2000      /* Max number of levels for each WPGWIN/WPRWIN */
#define WP_NTSIZ     250       /* Size of level table in bytes = 2000/8 */
#define WP_PMKMAX    10        /* Max number of pointer marks */

/*
***Graphical window using X11 only, WPGWIN.
*/
#define GWEM_NORM    (ButtonPressMask      | \
                      ButtonReleaseMask    | \
                      ExposureMask         | \
                      StructureNotifyMask  | \
                      KeyPressMask         | \
                      FocusChangeMask )

#define GWEM_RUB     (ButtonPressMask      | \
                      ButtonReleaseMask    | \
                      ButtonMotionMask     | \
                      PointerMotionHintMask)

typedef struct
{
WPWID         id;                 /* WP-ID */
WPWGEO        geo;                /* X-Geometry */
WPWIN         wintab[WP_GWSMAX];  /* Child windows */
Pixmap        savmap;             /* Pixmap for 'save-under' */
GC            win_gc;             /* Normal GC */
GC            rub_gc;             /* Rubber-band GC */
WPVIEW        vy;                 /* Active view */
WPVIEW        old_vy;             /* Previous view */
DFPOST       *df_adr;             /* 1:st record in DF or NULL */
DBint         df_all;             /* Number of DF records allocated */
DBint         df_ptr;             /* Last record in DF */
DBint         df_cur;             /* Current record in DF */
unsigned char nivtab[WP_NTSIZ];   /* Level table */
DBint         pmkant;             /* Number of active pointer marks */
XPoint        pmktab[WP_PMKMAX];  /* Pointer mark table */
bool          reprnt;             /* Reparent TRUE/FALSE for WM's */
int           wmandx;             /* Moved in X before reparented */
int           wmandy;             /* Moved in Y before reparented */
double        linwdt;             /* Current linewidth */
bool          grid_on;            /* Grid on/off */
double        grid_x;             /* Grid base position X */
double        grid_y;             /* Grid base position Y */
double        grid_dx;            /* Grid spacing in X-direction */
double        grid_dy;            /* Grid spacing in Y-direction */
WPMCWIN      *mcw_ptr;            /* Ptr to Message_and_Command window */
} WPGWIN;

/*
***Graphical window using OpenGL, WPRWIN.
*/
#define RWEM_NORM    (ButtonPressMask       | \
                      ButtonReleaseMask     | \
                      ExposureMask          | \
                      StructureNotifyMask   | \
                      KeyPressMask          | \
                      ButtonMotionMask      | \
                      PointerMotionHintMask | \
                      FocusChangeMask )

typedef struct
{
WPWID         id;                 /* ID */
WPWGEO        geo;                /* Geometry */
WPWIN         wintab[WP_GWSMAX];  /* Child window table */
Pixmap        savmap;             /* Pixmap for 'save-under' */
GC            win_gc;             /* Normal X11 GC */
GC            rub_gc;             /* Rubber-band GC */
GLXContext    rc;                 /* OpenGL Rendering Context */
XVisualInfo  *pvinfo;             /* Visual */
Colormap      colmap;             /* Colormap */
unsigned long bgnd1;              /* Pixelvalue for background 1 */
unsigned long bgnd2;              /* Pixelvalue for background 2 */
unsigned long bgnd3;              /* Pixelvalue for background 3 */
unsigned long fgnd;               /* Dito förgrund */
unsigned long tops;               /* Dito topskugga */
unsigned long bots;               /* Dito bottenskugga */
unsigned long noti;               /* Dito notify */
WPVIEW        vy;                 /* Aktiv grafisk vy */
unsigned char nivtab[WP_NTSIZ];   /* Level table */
bool          rgb_mode;           /* RGB eller Colorindex */
bool          double_buffer;      /* Double buffer eller single */
double        xmin,xmax,ymin,ymax,
              zmin,zmax;          /* Current view box */
DBint         musmod;             /* Aktuell musmode */
double        rotx,roty,rotz;     /* Aktuell rotation */
double        movx,movy;          /* Aktuell translation */
double        scale;              /* Aktuell skalfaktor */
double        light;              /* Aktuell ljusstyrka 0 -> 100 */
double        pfactor;            /* Perspektivfaktor 0 -> 100 */
double        ambient[3];         /* Ambient light */
double        diffuse[3];         /* Diffuse light */
double        specular[3];        /* Specualar light */
bool          zclip;              /* Z-clip on/off */
double        zfactor;            /* Current Z clip factor 0 -> 100 */
bool          fill;               /* Fill mode TRUE/FALSE */
bool          reprnt;             /* Reparent TRUE/FALSE for WM's */
int           wmandx;             /* Moved in X before reparented */
int           wmandy;             /* Moved in Y before reparented */
WPMCWIN      *mcw_ptr;            /* Ptr to Message_and_Command window */
} WPRWIN;

/*
***An Input window.
*/
#define WP_IWSMAX    500          /* Max antal sub-fönster i ett WPIWIN */

typedef struct
{
WPWID      id;                    /* ID */
WPWGEO     geo;                   /* Geometri */
WPWIN      wintab[WP_IWSMAX];     /* Subfönster */
bool       mapped;                /* Mappat i X, ja/nej */
} WPIWIN;

/*
***A List window.
*/
#define WP_LWSMAX     3           /* Max antal sub-fönster i ett WPIWIN */

typedef struct
{
WPWID      id;                    /* ID */
WPWGEO     geo;                   /* Geometri */
WPWIN      wintab[WP_LWSMAX];     /* Subfönster, scroll och save */
FILE      *filpek;                /* Pekare till list-fil */
char       filnam[V3PTHLEN+1];    /* Listfilens namn (inkl. path) */
char       rubrik[V3STRLEN+1];    /* Listans rubrik, dvs. lst_ini(rubrik) */
DBint      maxrln;                /* Max radlängd */
DBint      rstart;                /* Radbörjan */
DBint      trant;                 /* Totalt antal rader */
DBint      frant;                 /* Antal rader i fönstret */
bool       sscrol;                /* TRUE=Sid-scroll, FALSE=Rad-scroll */
} WPLWIN;

/*
***An Icon window.
*/

typedef struct
{
WPWID      id;                 /* ID */
WPWGEO     geo;                /* Geometry */
Pixmap     pixmap;             /* Pixmap */
Pixmap     mask;               /* Mask */
GC         gc;                 /* The icon graphics context */
short      acttyp;             /* Action type, MENU/CFUNC etc. */
short      actnum;             /* Action number */
char       tt_str[81];         /* Tooltip string */
} WPICON;
