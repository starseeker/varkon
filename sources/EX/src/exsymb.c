/*!*******************************************************
*
*    exsymb.c
*    ========
*
*    EXsymb();     Create (SYMB) plotfile-part
*
*    This file is part of the VARKON Execute Library.
*    URL:  http://www.varkon.com
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
*
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../../WP/include/WP.h"

#include "../include/EX.h"
#include <string.h>

/* Tabell för gpitoa */

static short tal[] = {
9000, 8000, 7000, 6000, 5000, 4000, 3000, 2000, 1000,
900, 800, 700, 600, 500, 400, 300, 200, 100,
90, 80, 70, 60, 50, 40, 30, 20, 10 };

static short *acc_tab[] = { &tal[0], &tal[9], &tal[18]};

static char ch[] = {'9','8','7','6','5','4','3','2','1','0'};

/* Tabell för gpaton */

static double tab_1[] = {
      0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001, 0.0000001,
      0.00000001, 0.000000001, 0.0000000001};
static double tab_2[] = { 
      0.2, 0.02, 0.002, 0.0002, 0.00002, 0.000002, 0.0000002, 
      0.00000002, 0.000000002, 0.0000000002};
static double tab_3[] = { 
      0.3, 0.03, 0.003, 0.0003, 0.00003, 0.000003, 0.0000003, 
      0.00000003, 0.000000003, 0.0000000003};
static double tab_4[] = { 
      0.4, 0.04, 0.004, 0.0004, 0.00004, 0.000004, 0.0000004, 
      0.00000004, 0.000000004, 0.0000000004};
static double tab_5[] = { 
      0.5, 0.05, 0.005, 0.0005, 0.00005, 0.000005, 0.0000005,
      0.00000005, 0.000000005, 0.0000000005};
static double tab_6[] = { 
      0.6, 0.06, 0.006, 0.0006, 0.00006, 0.000006, 0.0000006,
      0.00000006, 0.000000006, 0.0000000006};
static double tab_7[] = { 
      0.7, 0.07, 0.007, 0.0007, 0.00007, 0.000007, 0.0000007, 
      0.00000007, 0.000000007, 0.0000000007};
static double tab_8[] = { 
      0.8, 0.08, 0.008, 0.0008, 0.00008, 0.000008, 0.0000008, 
      0.00000008, 0.000000008, 0.0000000008};
static double tab_9[] = { 
      0.9, 0.09, 0.009, 0.0009, 0.00009, 0.000009, 0.0000009, 
      0.00000009, 0.000000009, 0.0000000009};

extern char    jobdir[];
extern char    asydir[];
extern DBptr   lsysla;
extern DBTmat *lsyspk;
extern DBTmat  lklsyi;

static double gpaton(char *pos);

/*!******************************************************/

       short     EXsymb(
       DBId     *id,
       char     *nam,
       DBVector *pos,
       DBfloat   size,
       DBfloat   ang,
       V2NAPA   *pnp)

/*      Skapa SYMB.
 *
 *      In: id     => Pekare till symbolens identitet.
 *          nam    => Pekare till namnsträng.
 *          pos    => Pekare till position.
 *          size   => Symbolens storlek (skala).
 *          ang    => Vinkel (vridning).
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:       0 = Ok.
 *           EX1582 = Filen %s finns ej.
 *
 *      (C)microform ab 6/12/85 J. Kjellander
 *
 *      27/12/85 Ny konv. av flyttal R. Svedin
 *      29/1/86  Ny inläsning från fil, J. Kjellander
 *      2/1/86   Koll av antal vektorer, J. Kjellander
 *      23/3/86  Bytt namn, J. Kjellander.
 *      30/9/86  asydir, J. Kjellander
 *      19/10/86 Sumbolens origo, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      7/5/87   Plot-origo, J. Kjellander
 *      25/4/88  Bug flcose(), J. Kjellander
 *      28/2/89  Part, J. Kjellander
 *      15/12/89 Pennummer, J. Kjellander
 *      13/2/91  Bytt riktn. på linjer, J. Kjellander
 *      99-02-26 Linjebredd, J.Kjellander
 *
 ******************************************************!*/

  {
    char    sfnam[132];
    DBfloat crdvek[4];
    DBfloat *crdpek;
    DBfloat localx,localy,rotx,roty;
    DBfloat cosv,sinv,width;
    DBfloat origox,origoy;
    char    inbuf[128];
    char    str1[81],str2[81];
    char   *bufpek;
    int     ntkn,ncrd,i;
    double  x,y,nx,ny,mxmin,mymin,mxmax,mymax;
    short   status=0,npnum;
    FILE   *fp;

    DBLine  lin;
    DBId    lid;
    DBPart  prt;
    DBPdat  dat;
    V2NAPA  attr;

/*
***Transformera pos till basic.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(pos,&lklsyi,pos);
/*
***Skapa en part-post.
*/
    prt.hed_pt.level = pnp->level;
    prt.hed_pt.pen = pnp->pen;
    prt.hed_pt.blank = pnp->blank;
    prt.hed_pt.hit = pnp->hit;
    *(nam+JNLGTH) = '\0';
    strcpy(prt.name_pt,nam);
    prt.its_pt = 2;
    dat.mtyp_pd = 2;
    dat.matt_pd = BASIC;
    dat.npar_pd = 0;
/*
***Skapa en linje-post.
*/
   lin.crd1_l.z_gm = lin.crd2_l.z_gm = 0.0;
   lid.seq_val = 1; lid.ord_val = 1; lid.p_nextre = NULL;
/*
***Skapa attributblock.
*/
   V3MOME(pnp,&attr,sizeof(V2NAPA));
   attr.lfont = 0;
   attr.ldashl = attr.width = 0.0;
/*
***Prova att öppna PLT-filen.
*/
    strcpy(sfnam,jobdir);
    strcat(sfnam,nam);
    strcat(sfnam,SYMEXT);
    if ( (fp=fopen(sfnam,"r")) == NULL )
      {
      strcpy(sfnam,asydir);
      strcat(sfnam,nam);
      strcat(sfnam,SYMEXT);
      if ( (fp=fopen(sfnam,"r")) == NULL )
            return(erpush("EX1582",nam));
      }
/*
***Filen finns, öppna part.
*/
    if ( pnp->save == 1 )
      {
      status = EXoppt(id,NULL,&prt,&dat,NULL,NULL);
      if ( status < 0 ) goto end;
      }
/*
***Lite initiering.
*/
    cosv = COS(ang*DGTORD);
    sinv = SIN(ang*DGTORD);
    origox = origoy = 0.0;
/*
***Läs vektorer från filen och skapa linjer.
*/
    while (fgets(inbuf,127,fp) != NULL)
        {
        if (strncmp (inbuf,"GKSM",4) == 0)
            {
            bufpek = inbuf+4;
/*
***Polyline, GKSM  11.
*/
            if ( strncmp(bufpek,"  11",4) == 0)
                {
                crdpek = crdvek;
                bufpek += 4;
                strncpy(str1,bufpek,6);
                bufpek += 6;
                strncpy(str2,bufpek,6);
                bufpek += 6;
                str1[6] = str2[6] ='\0';
                sscanf(str1,"%d",&ntkn);
                sscanf(str2,"%d",&ncrd);

                for ( i=0; i<ncrd; ++i )
                   {
                   x = gpaton(bufpek);
                   bufpek += 10;
                   y = gpaton(bufpek);
                   bufpek += 10;
/*
***Beräkna modellkoordinater och lagra.
*/
                   localx = size*(x*(mxmax-mxmin)/nx-origox);
                   localy = size*(y*(mymax-mymin)/ny-origoy);

                   rotx = localx*cosv - localy*sinv;
                   roty = localy*cosv + localx*sinv;

                   *crdpek = pos->x_gm + rotx; ++crdpek;
                   *crdpek = pos->y_gm + roty; ++crdpek;

                   if ( i != 0 )
                     {
                     lin.crd2_l.x_gm = *(crdpek-4);
                     lin.crd2_l.y_gm = *(crdpek-3);
                     lin.crd1_l.x_gm = *(crdpek-2);
                     lin.crd1_l.y_gm = *(crdpek-1);
                     *(crdpek-4) = *(crdpek-2);
                     *(crdpek-3) = *(crdpek-1);
                     crdpek -= 2;
                     if ( pnp->save == 1 )
                       if ( (status=EXelin(&lid,&lin,&attr)) < 0 ) 
                         goto end;
                     }
/*
***Läsning av nästa rad i en lång polyline.
*/
                   if ( i != ncrd-1 && (int)(bufpek-inbuf+1) >= (int)strlen(inbuf) )
                     {
                     fgets(inbuf,512,fp);
                     bufpek = inbuf;
                     }
                   }
                }
/*
***Modellfönster, GKSM 175.
*/
            else if ( strncmp(bufpek," 175",4) == 0)
                {
                bufpek += 4;
                sscanf(bufpek,"%6c",str1);
                str1[6] = '\0';
                sscanf(str1,"%d",&ntkn);

                bufpek += 6;
                sscanf(bufpek,"%10c%10c",str1,str2);
                str1[10] = str2[10] = '\0';
                sscanf(str1,"%lf",&mxmin);
                sscanf(str2,"%lf",&mxmax);

                bufpek += 20;
                sscanf(bufpek,"%10c%10c",str1,str2);
                str1[10] = str2[10] = '\0';
                sscanf(str1,"%lf",&mymin);
                sscanf(str2,"%lf",&mymax);

                if ( *(bufpek+21) != '\0' )
                  {
                  bufpek += 20;
                  sscanf(bufpek,"%10c%10c",str1,str2);
                  str1[10] = str2[10] = '\0';
                  sscanf(str1,"%lf",&origox);
                  sscanf(str2,"%lf",&origoy);
                  }
/*
***Beräkna förhållande mellan skärmens xmax o ymax.
*/
                nx = mxmax - mxmin;
                ny = mymax - mymin;

                if  ( nx < ny ) 
                   { nx /= ny; ny = 1.0; }
                else
                   { ny /= nx; nx = 1.0; }
                }
/*
***Linjebredd, GKSM 174.
*/
            else if ( strncmp(bufpek," 174",4) == 0)
                {
                bufpek += 4;
                sscanf(bufpek,"%6c%10c",str1,str2);
                str1[6] = str2[10] = '\0';
                sscanf(str1,"%d",&ntkn);
                sscanf(str2,"%lf",&width);
                attr.width = width;
                }
/*
***Pennummer, GKSM 173.
*/
            else if ( strncmp(bufpek," 173",4) == 0)
                {
                bufpek += 4;
                sscanf(bufpek,"%6c%6c",str1,str2);
                str1[6] = str2[6] = '\0';
                sscanf(str1,"%d",&ntkn);
                sscanf(str2,"%hd",&npnum);
                attr.pen = npnum;
                }
            }
        }
/*
***Slut.
*/
end:
    if ( pnp->save == 1 ) EXclpt();
    fclose(fp);
    return(status);
  }
  
/********************************************************/
/*!******************************************************/

 static double gpaton(
        char   *pos)

/*      Konvreterar ascii till flyttal i området 0 - 1.
 *
 *      In: pos -> Pekare till en asciisträng.
 *
 *      Ut: Strängen konv. till flyttal.
 *
 *      FV: Inget. 
 *
 *      LDAB  18/12/85  Håkan Svensson 
 *
 *      (C)microform ab 
 *
 *      19/12/85 Modifierad till varkonformat R. Svedin
 *      29/3/86  Flyttad från exe7.c  R. Svedin
 *
 ******************************************************!*/
   {
     register short  i,decno;
     register char  *start = pos;
     register double sum;

     while (*pos++ != '.');

     if ( *(pos-2) == '1') return(1.0);

     decno = 10 - (pos-start);
     sum = 0.0;

     for (i=0;i<decno;++i) 
       {
       switch (*pos) 
         {
         case '0': break;

         case '1': sum += tab_1[i]; break;

         case '2': sum += tab_2[i]; break;

         case '3': sum += tab_3[i]; break;

         case '4': sum += tab_4[i]; break;
	
         case '5': sum += tab_5[i]; break;

         case '6': sum += tab_6[i]; break;

         case '7': sum += tab_7[i]; break;

         case '8': sum += tab_8[i]; break;

         case '9': sum += tab_9[i]; break;

         default:;

         }
 
       ++pos;

      }

    return(sum);

  }

/********************************************************/
