/**********************************************************************
*
*    wpdef.h
*    =======
*
*    This file is part of the VARKON WindowPac Library.
*    URL: http://www.varkon.com
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

#ifdef V3_OPENGL
#include <GL/glx.h>
#else
#define GLXContext int
#endif

/*
***Max number of windows in wpwtab.
*/
#define WTABSIZ 100

/*
***Window types.
*/
#define TYP_UNDEF   0          /* Odefinierad typ */
#define TYP_IWIN    1          /* Input-Fönster */
#define TYP_EDIT    2          /* Edit */
#define TYP_ICON    3          /* Icon */
#define TYP_BUTTON  4          /* Button */
#define TYP_LWIN    5          /* List-fönster */
#define TYP_GWIN    6          /* Grafiskt fönster */
#define TYP_RWIN    7          /* Renderings-fönster (OpenGL) */

/*
***Colors.
*/
#define WP_NPENS 256             /* Antal normala pennor */
#define WP_SPENS 5               /* Antal systempennor */
#define WP_BGND  WP_NPENS        /* Bakgrund */
#define WP_FGND  WP_NPENS+1      /* Förgrund */
#define WP_TOPS  WP_NPENS+2      /* Toppskugga */
#define WP_BOTS  WP_NPENS+3      /* Bottenskugga */
#define WP_NOTI  WP_NPENS+4      /* Notify */

/*
***Rubberband modes.
*/
#define WP_RUB_NONE    0         /* Ingen figur */
#define WP_RUB_RECT    1         /* Rektangel */
#define WP_RUB_LINE    2         /* Rät linje */
#define WP_RUB_ARROW   3         /* Pil och ring */
/*
**Window-ID.
*/
typedef DBint  wpw_id;          /* Ett fönster-ID är en int */

typedef struct
{
wpw_id     w_id;               /* Eget ID, dvs. entry i "ägarens wintab" */
wpw_id     p_id;               /* Ägarens ID */
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
double     psiz_y;             /* Pixelstorlek i X-led i mm. */
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
***A graphical view.
*/
typedef struct
{
char       vynamn[GPVNLN+1];   /* Vynamn */
bool       valid;              /* FALSE = Ej vettiga data */
bool       vy_3D;              /* TRUE om 3D-vy */
double     vydist;             /* Perspektivavstånd */
double     k1x,k2x,k1y,k2y;    /* 2D vy-transformation */
VYWIN      scrwin;             /* Skärmfönster */
VYWIN      modwin;             /* Modellfönster */
VYTRF      vymat;              /* 3D vy-transformation */
} WPVY;

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
} WPBUTT;

/*
***A graphical window.
*/

#define GWEM_NORM    (ButtonPressMask      | \
                      ExposureMask         | \
                      StructureNotifyMask  | \
                      KeyPressMask         | \
                      FocusChangeMask )

#define GWEM_RUB     (ButtonPressMask      | \
                      ButtonReleaseMask    | \
                      ButtonMotionMask     | \
                      PointerMotionHintMask)

#define WP_GWMAX      25       /* Max antal WPGWIN-fönster */
#define WP_GWSMAX     100      /* Max antal sub-fönster i ett WPGWIN */
#define WP_NIVANT     2000     /* Max antal nivåer/fönster */
#define WP_NTSIZ      250      /* Nivåtabellens storlek i bytes */
#define WP_PMKMAX     10       /* Max antal pekmärken */

typedef struct
{
WPWID         id;                 /* WP-ID */
WPWGEO        geo;                /* X-Geometri */
WPWIN         wintab[WP_GWSMAX];  /* Subfönster för snabbval */
Pixmap        savmap;             /* Pixmap för 'save-under' */
GC            win_gc;             /* Normalt grafiskt GC */
GC            rub_gc;             /* Rubber-band GC */
WPVY          vy;                 /* Aktiv grafisk vy */
WPVY          old_vy;             /* Föregående grafisk vy */
DFPOST       *df_adr;             /* 1:a posten i DF eller NULL */
DBint         df_all;             /* Antal allokerade DF-poster */
DBint         df_ptr;             /* Sista använda posten i DF */
DBint         df_cur;             /* Aktuell DF-post */
unsigned char nivtab[WP_NTSIZ];   /* Nivåtabell */
DBint         pmkant;             /* Antal aktiva pekmärken */
XPoint        pmktab[WP_PMKMAX];  /* Pekmärkestabellen */
bool          reprnt;             /* Bytt förälder TRUE/FALSE */
short         wmandx;             /* Flyttad i X-led före föräldrabyte */
short         wmandy;             /* Flyttad i Y-led före föräldrabyte */
WPBUTT       *mesbpt;             /* Pekare till meddelanderaden eller NULL */
double        linwdt;             /* Aktiv linjebredd */
} WPGWIN;

/*
***An OpenGL Dynamic Rendering window.
*/
#define RWEM_NORM    (ButtonPressMask       | \
                      ButtonReleaseMask     | \
                      ExposureMask          | \
                      StructureNotifyMask   | \
                      KeyPressMask          | \
                      ButtonMotionMask      | \
                      PointerMotionHintMask | \
                      FocusChangeMask )

#define WP_RWSMAX     100      /* Max antal sub-fönster i ett WPRWIN */

typedef struct
{
WPWID         id;                 /* WP-ID */
WPWGEO        geo;                /* X-Geometri */
WPWIN         wintab[WP_GWSMAX];  /* Subfönster för snabbval */
Pixmap        savmap;             /* Pixmap för 'save-under' */
GC            win_gc;             /* Normalt grafiskt GC */
GC            rub_gc;             /* Rubber-band GC */
GLXContext    rc;                 /* OpenGL rendering context */
XVisualInfo  *pvinfo;             /* Visual */
Colormap      colmap;             /* Colormap */
unsigned long bgnd;               /* Pixelvärde för knappbakgrund */
unsigned long fgnd;               /* Dito förgrund */
unsigned long tops;               /* Dito topskugga */
unsigned long bots;               /* Dito bottenskugga */
unsigned long noti;               /* Dito notify */
WPVY          vy;                 /* Aktiv grafisk vy */
bool          rgb_mode;           /* RGB eller Colorindex */
bool          double_buffer;      /* Double buffer eller single */
double        xmin,xmax,ymin,ymax,
              zmin,zmax;          /* Vy-box */
DBint         musmod;             /* Aktuell mosmode */
double        rotx,roty,rotz;     /* Aktuell rotation */
double        movx,movy;          /* Aktuell translation */
double        scale;              /* Aktuell skalfaktor */
double        light;              /* Aktuell ljusstyrka 0 -> 100 */
double        pfactor;            /* Perspektivfaktor 0 -> 100 */
double        ambient[3];         /* Ambient ljusfaktor */
double        diffuse[3];         /* Diffuse ljusfaktor */
double        specular[3];        /* Specualar ljusfaktor */
bool          zclip;              /* Z-klipp på/av */
double        zfactor;            /* Aktuell Z-klippfaktor 0 -> 100 */
} WPRWIN;

/*
***An Input window.
*/
#define WP_IWSMAX    500       /* Max antal sub-fönster i ett WPIWIN */

typedef struct
{
WPWID      id;                 /* ID */
WPWGEO     geo;                /* Geometri */
WPWIN      wintab[WP_IWSMAX];  /* Subfönster */
bool       mapped;             /* Mappat i X, ja/nej */
} WPIWIN;

/*
***A List window.
*/
#define WP_LWSMAX     3        /* Max antal sub-fönster i ett WPIWIN */

typedef struct
{
WPWID      id;                 /* ID */
WPWGEO     geo;                /* Geometri */
WPWIN      wintab[WP_LWSMAX];  /* Subfönster, scroll och save */
FILE      *filpek;             /* Pekare till list-fil */
char       filnam[V3PTHLEN+1]; /* Listfilens namn (inkl. path) */
char       rubrik[V3STRLEN+1]; /* Listans rubrik, dvs. lst_ini(rubrik) */
DBint      maxrln;             /* Max radlängd */
DBint      rstart;             /* Radbörjan */
DBint      trant;              /* Totalt antal rader */
DBint      frant;              /* Antal rader i fönstret */
bool       sscrol;             /* TRUE=Sid-scroll, FALSE=Rad-scroll */
} WPLWIN;

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
} WPEDIT;

/*
***An Icon window.
*/

typedef struct
{
WPWID      id;                 /* ID */
WPWGEO     geo;                /* Geometri */
WPWCOL     color;              /* Färger */
Pixmap     bitmap;             /* Bitmap */
short      acttyp;             /* Aktionstyp, MENU/FUNC etc. */
short      actnum;             /* Aktionens nummer */
} WPICON;
