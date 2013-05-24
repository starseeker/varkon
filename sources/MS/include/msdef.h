/****************************************************
*
*      msdef.h
*      =======
*
*      Detta är WIN32-versionen av wpdef.h
*     
*      (c)microform ab 18/10/95 Johan Kjellander
*
*      1996-04-16 100/500 subfönster i WPGWIN/WPIWIN
*
*****************************************************/

/*
***Max antal fönster i fönstertabellen wpwtab.
*/
#define WTABSIZ 100

/*
***WIN32 fönsterklasser.
*/
#define VARKON_MAIN_CLASS   "VARKON_MAIN"     /* Endast huvudfönstret */
#define VARKON_IWIN_CLASS   "VARKON_IWIN"     /* Alla WPIWIN */
#define VARKON_LWIN_CLASS   "VARKON_LWIN"     /* Alla WPLWIN */
#define VARKON_MGWIN_CLASS  "VARKON_MGWIN"    /* GWIN_MAIN */
#define VARKON_GWIN_CLASS   "VARKON_GWIN"     /* Övriga WPGWIN */
#define VARKON_EWIN_CLASS   "VARKON_EWIN"     /* Alla WPEWIN */
#define VARKON_BUTT_CLASS   "VARKON_IWIN"     /* Alla WPBUTT */
#define VARKON_ICON_CLASS   "VARKON_IWIN"     /* Alla WPICON */
#define VARKON_EDIT_CLASS   "VARKON_IWIN"     /* Alla WPEDIT */
#define VARKON_RWIN_CLASS   "VARKON_RWIN"     /* Alla WPRWIN */
/*
***Ett WIN32 fönstermeddelande.
*/
typedef struct wpmes32
{
HWND   wh;
UINT   msg;
WPARAM wp; 
LPARAM lp;
}WPMES32;

/*
***VARKON-specifika fönstermeddelanden.
*/
#define V3_KILLFOCUS     WM_USER + 1
#define V3_SAVE_AS       WM_USER + 2
#define V3_SAVE          WM_USER + 3
#define V3_PRINT         WM_USER + 4
#define V3_CUT           WM_USER + 5
#define V3_COPY          WM_USER + 6
#define V3_PASTE         WM_USER + 7
#define V3_COMPILE       WM_USER + 8
#define V3_FIND          WM_USER + 9
#define V3_REPLACE       WM_USER + 10

/*
***Koder för msffoc(), hitta edit/knapp med focus.
*/
#define    FIRST_EDIT    1     /* 1:a editfönstret */
#define    FIRST_EDBU    2     /* 1:a subfönstret oavsett typ */
#define    NEXT_EDBU     3     /* Nästa subfönster */
#define    PREV_EDBU     4     /* Föregående subfönster */
#define    FOCUS_EDBU    5     /* Aktiva subfönstret */
#define    LAST_EDBU     6     /* Sista subfönstret */

/*
***Olika typer av WP-fönster.
*/
#define TYP_UNDEF   0          /* Odefinierad typ */
#define TYP_IWIN    1          /* Input-Fönster */
#define TYP_EDIT    2          /* Edit */
#define TYP_ICON    3          /* Icon */
#define TYP_BUTTON  4          /* Button */
#define TYP_LWIN    5          /* List-fönster */
#define TYP_GWIN    6          /* Grafiskt fönster */
#define TYP_EWIN    7          /* Edit-fönster */
#define TYP_RWIN    8          /* Renderings-fönster (OpenGL) */

/*
***Färger.
*/
#define WP_NPENS 256             /* Antal normala pennor */
#define WP_BGND  WP_NPENS        /* Bakgrund */
#define WP_FGND  WP_NPENS+1      /* Förgrund */
#define WP_TOPS  WP_NPENS+2      /* Toppskugga */
#define WP_BOTS  WP_NPENS+3      /* Bottenskugga */
/*
***Olika typer av gummibandsfigurer.
*/
#define WP_RUB_NONE    0         /* Ingen figur */
#define WP_RUB_RECT    1         /* Rektangel */
#define WP_RUB_LINE    2         /* Rät linje */
#define WP_RUB_ARROW   3         /* Pil och ring */

/*
***Fönster-ID.
*/
typedef int  wpw_id;           /* Ett fönster-ID är en int */

typedef struct wpwid
{
wpw_id     w_id;               /* Eget ID, dvs. entry i "ägarens wintab" */
wpw_id     p_id;               /* Ägarens ID */
HWND       ms_id;              /* Window handle */
} WPWID;

/*
***Fönstergeometri.
*/
typedef struct wpwgeo
{
short      x;                  /* Verkligt läge X-koordinat */
short      y;                  /* Verkligt läge Y-koordinat */
short      dx;                 /* Klientareans storlek i X-led */
short      dy;                 /* Klientareans storlek i Y-led */
double     psiz_x;             /* Pixelstorlek i X-led i mm. */
double     psiz_y;             /* Pixelstorlek i X-led i mm. */
} WPWGEO;

/*
***Fönsterfärger
*/
typedef struct wpwcol
{
short      bckgnd;             /* Bakgrundsfärg */
short      forgnd;             /* Förgrundsfärg */
} WPWCOL;

/*
***Ett godtyckligt fönster.
*/
typedef struct wpwin
{
char       typ;                /* Typ av fönster, tex. TYP_IWIN */
char      *ptr;                /* C-pekare till en fönster-structure */
} WPWIN;

/*
***En grafisk punkt (eller koordinat).
*/
typedef struct wpgrpt
{
double     x;                  /* X-koordinat */
double     y;                  /* Y-koordinat */
double     z;                  /* Z-koordinat */
char       a;                  /* Tänd/släck + start/slut */
} WPGRPT;

/*
***Ett modell- eller skärm-fönster i en grafisk vy.
*/
typedef struct vywin
{
double     xmin,xmax;          /* Gränser i X-led */
double     ymin,ymax;          /* Gränser i Y-led */
} VYWIN;

/*
***En transformation från BASIC till 3D-vy:s xy-plan.
*/
typedef struct vytrf
{
double     k11,k12,k13;        /* 1:a raden */
double     k21,k22,k23;        /* 2:a raden */
double     k31,k32,k33;        /* 3:e raden */
} VYTRF;

/*
***Grafisk vy i ett WPGWIN.
*/
typedef struct wpvy
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
***Ett Button-fönster. Måste definieras här eftersom WPGWIN
***använder WBBUTT.
*/
#define FUNC_BUTTON 1          /* Funktionsknapp */
#define NORM_BUTTON 2          /* Normal togglande tryckknapp */
#define DEF_BUTTON  3          /* Defaultknapp */

typedef struct wpbutt
{
WPWID      id;                 /* ID */
WPWGEO     geo;                /* Geometri */
WPWCOL     color;              /* Färger */
int        font;               /* Fontnummer */
char       stron[81];          /* Text  för knapp på */
char       stroff[81];         /* Text  för knapp av */
bool       status;             /* På eller av, default = av = FALSE*/
short      acttyp;             /* Aktionstyp, MENU/FUNC etc. */
short      actnum;             /* Aktionens nummer */
bool       label;              /* Utan ram är det bara en label */
bool       focus;              /* TRUE = Input focus */
bool       def;                /* Default button, triggas av CR */
WNDPROC    ms_cbk;             /* Ursprunglig callbackrutin */
} WPBUTT;

/*
***Ett grafisk fönster.
*/
#define WP_GWMAX      25       /* Max antal WPGWIN-fönster */
#define WP_GWSMAX     100      /* Max antal sub-fönster i ett WPGWIN */
#define WP_NIVANT     2000     /* Max antal nivåer/fönster */
#define WP_NTSIZ      250      /* Nivåtabellens storlek i bytes */
#define WP_PMKMAX     10       /* Max antal pekmärken */

typedef struct wpgwin
{
WPWID         id;                 /* WP-ID */
WPWGEO        geo;                /* X-Geometri */
WPWIN         wintab[WP_GWSMAX];  /* Subfönster för snabbval */
HDC           dc;                 /* Privat DC för rita-rutiner */
HDC           bmdc;               /* DC för 'save-under' */
HBITMAP       bmh;                /* Bitmap för 'save_under' */
HRGN          clprgn;             /* Clipregion for margins */
int           margin_up;          /* Övre marginals storlek */
int           margin_down;        /* Nedre marginal */
int           margin_left;        /* Vänster marginal */
int           margin_right;       /* Höger marginal */
WPVY          vy;                 /* Aktiv grafisk vy */
WPVY          old_vy;             /* Föregående grafisk vy */
DFPOST       *df_adr;             /* 1:a posten i DF eller NULL */
long          df_all;             /* Antal allokerade DF-poster */
long          df_ptr;             /* Sista använda posten i DF */
long          df_cur;             /* Aktuell DF-post */
unsigned char nivtab[WP_NTSIZ];   /* Nivåtabell */
int           pmkant;             /* Antal aktiva pekmärken */
POINT         pmktab[WP_PMKMAX];  /* Pekmärkestabellen */
double        linwdt;             /* Aktiv linjebredd */
} WPGWIN;

/*
***Ett renderingsfönster.
*/
#define WP_RWSMAX     100         /* Max antal sub-fönster i ett WPRWIN */

typedef struct wprwin
{
WPWID         id;                 /* WP-ID */
WPWGEO        geo;                /* X-Geometri */
WPWIN         wintab[WP_GWSMAX];  /* Subfönster för snabbval */
HDC           dc;                 /* Privat DC för rita-rutiner */
HDC           bmdc;               /* DC för 'save-under' */
HBITMAP       bmh;                /* Bitmap för 'save_under' */
HGLRC         rc;                 /* OpenGL rendering context */
WPVY          vy;                 /* Aktiv grafisk vy */
bool          double_buffer;      /* Double buffer eller single */
double        xmin,xmax,ymin,ymax,
              zmin,zmax;          /* Vy-box */
double        rotx,roty,rotz;     /* Aktuell rotation */
double        movx,movy;          /* Aktuell translation */
double        scale;              /* Aktuell skalfaktor */
double        pfactor;            /* Perspektivfaktor */
double        light;              /* Aktuell ljusstyrka */
double        ambient[3];         /* ambient ljusfaktor */
double        diffuse[3];         /* diffuse ljusfaktor */
double        specular[3];        /* specular ljusfaktor */
HWND          tb_id;              /* Toolbarens fönsterhandtag */
bool          leftdown;           /* Vänster musknapp nedtryckt */
bool          zclip;              /* Klipplan på/av */
double        zfactor;            /* Aktuell position 0 -> 100 */
} WPRWIN;

/*
***Ett Input-fönster.
*/
#define WP_IWSMAX    500       /* Max antal sub-fönster i ett WPIWIN */

typedef struct wpiwin
{
WPWID      id;                 /* ID */
WPWGEO     geo;                /* Geometri */
wpw_id     focus_id;           /* ID för subfönster med fokus eller -1 */
WPWIN      wintab[WP_IWSMAX];  /* Subfönster */
} WPIWIN;

/*
***Ett List-fönster.
*/
typedef struct wplwin
{
WPWID      id;                 /* ID */
WPWGEO     geo;                /* Geometri */
FILE      *filpek;             /* Pekare till list-fil */
int        filsiz;             /* Filens storlek i bytes */
char       filnam[V3PTHLEN+1]; /* Listfilens namn (inkl. path) */
int        maxlen;             /* Antal tecken i längsta raden */
int        radant;             /* Antal rader i listfilen */
HWND       edit_id;            /* WIN32-id för editfönstret */
} WPLWIN;

/*
***Ett singel-rads Edit-fönster.
*/
typedef struct wpedit
{
WPWID      id;                 /* ID */
WPWGEO     geo;                /* Geometri */
char       str[V3STRLEN+1];    /* Edit-sträng */
char       dstr[V3STRLEN+1];   /* Default-sträng */
int        tknmax;             /* Max antal tecken */
int        scroll;             /* Scroll-position */
int        curpos;             /* Cursor-position */
bool       fuse;               /* Default-säkring */
bool       focus;              /* TRUE = Input focus */
int        symbol;             /* Senast inmatade symbol */
WNDPROC    ms_cbk;             /* Ursprunglig callbackrutin */
} WPEDIT;

/*
***Ett multi-rads edit-fönster.
*/
typedef struct wpewin
{
WPWID      id;                 /* ID */
WPWGEO     geo;                /* Geometri */
char       dir[V3PTHLEN+1];    /* Filkatalog inkl. "\" */
char       namn[V3PTHLEN+1];   /* Filens namn  */
char       path[V3PTHLEN+1];   /* Båda som en enda sträng dir+namn */
HWND       edit_id;            /* WIN32-id för editfönstret */
bool       actmod;             /* Aktiv modul ja/nej */
} WPEWIN;

/*
***Ett Icon-fönster.
*/
typedef struct wpicon
{
WPWID       id;                 /* ID */
WPWGEO      geo;                /* Geometri */
BITMAPINFO *bmiptr;             /* Pekare till bitmapinfo */
void       *pixptr;             /* Pekare till pixels */
short       acttyp;             /* Aktionstyp, MENU/FUNC etc. */
short       actnum;             /* Aktionens nummer */
} WPICON;
