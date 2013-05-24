/*!******************************************************************/
/*  File: pl2.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   plprpf();     Generic process plotfile                         */
/*   plpgks();     Process GKS-file                                 */
/*   plpdxf();     Process DXF-file                                 */
/*  *plgets();     PL version of C-gets()                           */
/*                                                                  */
/*  This file is part of the VARKON Plotter Library.                */
/*  URL:  http://www.varkon.com                                     */
/*                                                                  */
/*  This library is free software; you can redistribute it and/or   */
/*  modify it under the terms of the GNU Library General Public     */
/*  License as published by the Free Software Foundation; either    */
/*  version 2 of the License, or (at your option) any later         */
/*  version.                                                        */
/*                                                                  */
/*  This library is distributed in the hope that it will be         */
/*  useful, but WITHOUT ANY WARRANTY; without even the implied      */
/*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR         */
/*  PURPOSE.  See the GNU Library General Public License for more   */
/*  details.                                                        */
/*                                                                  */
/*  You should have received a copy of the GNU Library General      */
/*  Public License along with this library; if not, write to the    */
/*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge,        */
/*  MA 02139, USA.                                                  */
/*                                                                  */
/*  (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/PL.h"
#include <string.h>

extern double x[],y[],skala,wskala;
extern char   a[];

extern short  plchwd(),plfill();

static double atonc();  /* Conv. ascii to float */

#define VISIBLE 1

/*
***Char_to_float conversion table.
*/

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

/*!******************************************************/

        short plprpf()

/*      Processar plotfil.
 *
 *     (C)microform ab 31/1/91 J. Kjellander
 *
 *     1997-01-28 plgets(), J.Kjellander
 *
 ******************************************************!*/

 {
    char inbuf[256];

/*
***Läs en rad. plgets() läser till newline men ersätter
***detta med NULL.
*/
    if ( plgets(inbuf) == NULL ) 
      {
      printf("V3: Can't read plotfile or file empty !\n");
      exit(V3EXOK);
      }
loop:
/*
***Om GKS-fil skall de fyra första tecknen vara "GKSM".
*/
    if      ( strncmp(inbuf,"GKSM",4) == 0 )    return(plpgks(inbuf));
/*
***Om DXF-fil skall första raden vara en nolla och andra
***raden ordet "SECTION".
*/
    else if ( strncmp(inbuf,"SECTION",7) == 0 ) return(plpdxf(inbuf));
/*
***Om varken GKS eller DXF fortsätter vi och läser rad för
***rad tills vi antingen hittar något som verkar vara GKS eller
***DXF eller tills filen är slut.
*/
    else
      {
      if ( plgets(inbuf) == NULL )
        {
        printf("V3: Plotfile not in GKS or DXF format !\n");
        exit(V3EXOK);
        }
      goto loop;
      }
 }

/********************************************************/
/*!******************************************************/

        short plpgks(char inbuf[])

/*      Processar GKS-fil från standard input. Vid anropet
 *      är 1:a raden i filen redan läst och finns i inbuf.
 *
 *      In: inbuf = 81 teckens radbuffert för läsning
 *                  från fil.
 *
 *      Ut: Inget.
 *
 *      FV: Inget. 
 *
 *     (C)microform ab 25/11/85 B. Doverud
 *
 *      16/12/85 Vinkel, J. Kjellander
 *      19/12/85 Ny konv av Flyttal R. Svedin
 *      29/1/86  Ny läsmetod, J. Kjellander
 *      19/10/86 Origo=föntrets origo, J. Kjellander
 *      8/5/87   Plot-origo, J. Kjellander
 *      31/1/91  DXF, J. Kjellander
 *      1997-12-27 Breda linjer, J.Kjellander
 *      1998-11-12 wskala, J.Kjellander
 *      1999-12-15 fill, J.Kjellander
 *
 ******************************************************!*/

 {
    char    str1[81];
    char    str2[81];
    char   *bufpek;
    int     ntkn,ncrd,i;
    int     npnum;
    double  xt,yt,origox,origoy,width;
    double  nx=0,ny=0;
    double  mxmin,mymin,mxmax,mymax;
		
/*
***Läs från filen
*/
    while (plgets(inbuf) != NULL)
        {
        if (strncmp (inbuf,"GKSM",4) == 0)
            {
            bufpek = inbuf+4;
/*
***Polyline, GKSM  11.
*/
            if ( strncmp(bufpek,"  11",4) == 0)
                {
                bufpek += 4;
                sscanf(bufpek,"%6c%6c",str1,str2);
                str1[6] = str2[6] = '\0';
                sscanf(str1,"%d",&ntkn);
                sscanf(str2,"%d",&ncrd);

                bufpek += 12;
                for ( i=0; i<ncrd; ++i )
                   {
                   xt = atonc(bufpek);
                   bufpek += 10;
                   yt = atonc(bufpek);
                   bufpek += 10;
/*
***Beräkna modellkoordinater.
*/
                   x[i] = xt*(mxmax-mxmin)/nx - origox;
                   y[i] = yt*(mymax-mymin)/ny - origoy;
                   a[i] = VISIBLE;
/*
***Läs mera.
*/
                   if ( i != ncrd-1 && (int)(bufpek-inbuf) >= strlen(inbuf) )
                     {
                     plgets(inbuf);
                     bufpek = inbuf;
                     }
                   }
/*
***Rita/måla.
*/
                a[0] = 0;
                pntply((short)(ncrd-1),x,y,a);
                }
/*
***Fill, GKSM  14.
*/
            if ( strncmp(bufpek,"  14",4) == 0)
                {
                bufpek += 4;
                sscanf(bufpek,"%6c%6c",str1,str2);
                str1[6] = str2[6] = '\0';
                sscanf(str1,"%d",&ntkn);
                sscanf(str2,"%d",&ncrd);

                bufpek += 12;
                for ( i=0; i<ncrd; ++i )
                   {
                   xt = atonc(bufpek);
                   bufpek += 10;
                   yt = atonc(bufpek);
                   bufpek += 10;
/*
***Beräkna modellkoordinater.
*/
                   x[i] = xt*(mxmax-mxmin)/nx - origox;
                   y[i] = yt*(mymax-mymin)/ny - origoy;
                   a[i] = VISIBLE;
/*
***Läs mera.
*/
                   if ( i != ncrd-1 && (int)(bufpek-inbuf) >= strlen(inbuf) )
                     {
                     plgets(inbuf);
                     bufpek = inbuf;
                     }
                   }
/*
***Fyll.
*/
                a[0] = 0;
                plfill((short)(ncrd-1),x,y,a);
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
                sscanf(str2,"%d",&npnum);
/*
***Byt penna eller målningsmönster.
*/                   
                plcwdt(npnum);
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
                plchwd(wskala*width);
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
***Beräkna faktor för skillnad på skärmens xmax o ymax.
*/
                nx = mxmax - mxmin;
                ny = mymax - mymin;

                if  (nx < ny) 
                   {
                   nx /= ny;
                   ny = 1.0;
                   }
                else
                   {
                   ny /= nx;
                   nx =1.0;
                   }
                }
            }
        }

    return(0);
}

/********************************************************/
/*!******************************************************/

        short plpdxf(char inbuf[])

/*      Processar DXF-fil från standard input. Vid anropet
 *      är alla rader fram till första förekomsten av ordet
 *      SECTION lästa. Denna rad finns i inbuf.
 *
 *      In: inbuf = 81 teckens radbuffert för läsning
 *                  från fil.
 *
 *      Ut: Inget.
 *
 *      FV: Inget. 
 *
 *     (C)microform ab 31/1/91 J. Kjellander
 *
 ******************************************************!*/

 {

   short  nc;
   int    pen = 1;
   char   str[V3STRLEN+1];
   double xc,yc;
   GMUNON gmpost;


/*
***Läs fram till ENTITIES Section.
*/
fndent:
   if ( plgets(inbuf) == NULL ) goto unxeof;
   if ( strncmp(inbuf,"ENTITIES",8) != 0 ) goto fndent;
/*
***ENTITIES Section.
*/
enloop:
   if ( plgets(inbuf) == NULL ) goto unxeof;
/*
***Punkt.
*/
   if ( strncmp(inbuf,"POINT",5) == 0 )
     {
poloop:
     if ( plgets(inbuf) == NULL ) goto unxeof;
     if ( strncmp(inbuf," 10",3) == 0 )                /*X-Koordinat*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%lf",&gmpost.poi_un.crd_p.x_gm);
       }
     else if ( strncmp(inbuf," 20",3) == 0 )           /*Y-Koordinat*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%lf",&gmpost.poi_un.crd_p.y_gm);
       }
     else if ( strncmp(inbuf," 62",3) == 0 )           /*Penna*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%d",&pen);
       }
     else if ( strncmp(inbuf,"  0",3) == 0 )           /*Plotta*/
       {
       gmpost.poi_un.hed_p.pen = pen;
       pntpoi(&gmpost.poi_un);
       goto enloop;
       }
     goto poloop;
     }
/*
***Linje.
*/
   else if ( strncmp(inbuf,"LINE",4) == 0 )
     {
     gmpost.lin_un.fnt_l = 0;
liloop:
     if ( plgets(inbuf) == NULL ) goto unxeof;
     if ( strncmp(inbuf," 10",3) == 0 )                /*X1-Koordinat*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%lf",&gmpost.lin_un.crd1_l.x_gm);
       }
     else if ( strncmp(inbuf," 20",3) == 0 )           /*Y1-Koordinat*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%lf",&gmpost.lin_un.crd1_l.y_gm);
       }
     else if ( strncmp(inbuf," 11",3) == 0 )           /*X2-Koordinat*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%lf",&gmpost.lin_un.crd2_l.x_gm);
       }
     else if ( strncmp(inbuf," 21",3) == 0 )           /*Y2-Koordinat*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%lf",&gmpost.lin_un.crd2_l.y_gm);
       }
     else if ( strncmp(inbuf,"  6",3) == 0 )           /*Font*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       if ( strncmp(inbuf,"DSH",3) == 0 )
         {
         gmpost.lin_un.fnt_l = 1;
         sscanf(&inbuf[3],"%lf",&gmpost.lin_un.lgt_l);
         }
       else if ( strncmp(inbuf,"CEN",3) == 0 )
         {
         gmpost.lin_un.fnt_l = 2;
         sscanf(&inbuf[3],"%lf",&gmpost.lin_un.lgt_l);
         }
       else if ( plgets(inbuf) == NULL ) goto unxeof;
       }
     else if ( strncmp(inbuf," 62",3) == 0 )           /*Penna*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%d",&pen);
       }
     else if ( strncmp(inbuf,"  0",3) == 0 )           /*Plotta*/
       {
       gmpost.lin_un.hed_l.pen = pen;
       pntlin(&gmpost.lin_un);
       goto enloop;
       }
     goto liloop;
     }
/*
***Cirkel eller cirkelbåge.
*/
   else if ( strncmp(inbuf,"CIRCLE",6) == 0  ||
              strncmp(inbuf,"ARC",3) == 0 )
     {
     gmpost.arc_un.v1_a = 0;
     gmpost.arc_un.v2_a = 360;
     gmpost.arc_un.fnt_a = 0;
arloop:
     if ( plgets(inbuf) == NULL ) goto unxeof;
     if ( strncmp(inbuf," 10",3) == 0 )                /*X-Koordinat*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%lf",&gmpost.arc_un.x_a );
       }
     else if ( strncmp(inbuf," 20",3) == 0 )           /*Y-Koordinat*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%lf",&gmpost.arc_un.y_a );
       }
     else if ( strncmp(inbuf," 40",3) == 0 )           /*Radie*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%lf",&gmpost.arc_un.r_a );
       }
     else if ( strncmp(inbuf," 50",3) == 0 )           /*V1*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%lf",&gmpost.arc_un.v1_a );
       }
     else if ( strncmp(inbuf," 51",3) == 0 )           /*V2*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%lf",&gmpost.arc_un.v2_a );
       }
     else if ( strncmp(inbuf,"  6",3) == 0 )           /*Font*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       if ( strncmp(inbuf,"DSH",3) == 0 )
         {
         gmpost.arc_un.fnt_a = 1;
         sscanf(&inbuf[3],"%lf",&gmpost.arc_un.lgt_a);
         }
       else if ( strncmp(inbuf,"CEN",3) == 0 )
         {
         gmpost.arc_un.fnt_a = 2;
         sscanf(&inbuf[3],"%lf",&gmpost.arc_un.lgt_a);
         }
       else if ( plgets(inbuf) == NULL ) goto unxeof;
       }
     else if ( strncmp(inbuf," 62",3) == 0 )           /*Penna*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%d",&pen);
       }
     else if ( strncmp(inbuf,"  0",3) == 0 )           /*Plotta*/
       {
       gmpost.arc_un.hed_a.pen = pen;
       pntarc(&gmpost.arc_un);
       goto enloop;
       }
     goto arloop;
     }
/*
***Text.
*/
   else if ( strncmp(inbuf,"TEXT",4) == 0 )
     {
txloop:
     if ( plgets(inbuf) == NULL ) goto unxeof;
     if ( strncmp(inbuf," 10",3) == 0 )                /*X-Koordinat*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%lf",&gmpost.txt_un.crd_tx.x_gm);
       }
     else if ( strncmp(inbuf," 20",3) == 0 )           /*Y-Koordinat*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%lf",&gmpost.txt_un.crd_tx.y_gm);
       }
     else if ( strncmp(inbuf," 40",3) == 0 )           /*Höjd*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%lf",&gmpost.txt_un.h_tx);
       }
     else if ( strncmp(inbuf," 41",3) == 0 )           /*Bredd*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%lf",&gmpost.txt_un.b_tx);
       gmpost.txt_un.b_tx *= 57.0;
       }
     else if ( strncmp(inbuf," 50",3) == 0 )           /*Vinkel*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%lf",&gmpost.txt_un.v_tx);
       }
     else if ( strncmp(inbuf," 51",3) == 0 )           /*Lutning*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%lf",&gmpost.txt_un.l_tx);
       gmpost.txt_un.l_tx /= 0.45;
       }
     else if ( strncmp(inbuf,"  1",3) == 0 )           /*Text*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       strcpy(str,inbuf);
       }
     else if ( strncmp(inbuf," 62",3) == 0 )           /*Penna*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%d",&pen);
       }
     else if ( strncmp(inbuf,"  0",3) == 0 )           /*Plotta*/
       {
       gmpost.txt_un.hed_tx.pen = pen;
       pnttxt(&gmpost.txt_un,str);
       goto enloop;
       }
     goto txloop;
     }
/*
***Polyline.
*/
   else if ( strncmp(inbuf,"POLYLINE",8) == 0 )
     {
     nc = 0;
plloop:
     if ( plgets(inbuf) == NULL ) goto unxeof;
     if ( strncmp(inbuf,"VERTEX",6) == 0 )             /*VERTEX*/
       {
vxloop:
       if ( plgets(inbuf) == NULL ) goto unxeof;
       if ( strncmp(inbuf," 10",3) == 0 )              /*X-Koordinat*/
         {
         if ( plgets(inbuf) == NULL ) goto unxeof;
         sscanf(inbuf,"%lf",&xc);
         }
       else if ( strncmp(inbuf," 20",3) == 0 )         /*Y-Koordinat*/
         {
         if ( plgets(inbuf) == NULL ) goto unxeof;
         sscanf(inbuf,"%lf",&yc);
         }
       else if ( strncmp(inbuf,"  0",3) == 0 )         /*Slut vertex*/
         {
         x[nc] = xc;
         y[nc] = yc;
         a[nc] = VISIBLE;
       ++nc;
         goto plloop;
         }
       goto vxloop;
       }
     else if ( strncmp(inbuf,"SEQEND",6) == 0 )        /*SEQEND*/
       {
       a[0] = 0;
       pntply((short)(nc-1),x,y,a);
       goto enloop;
       }
     else if ( strncmp(inbuf," 62",3) == 0 )           /*Penna*/
       {
       if ( plgets(inbuf) == NULL ) goto unxeof;
       sscanf(inbuf,"%d",&pen);
       }
     goto plloop;
     }
/*
***Nästa storhet.
*/
     else if ( strncmp(inbuf,"EOF",3) == 0 ) goto end;
     else goto enloop;
/*
***Slut.
*/
end:
   return(0);
/*
***Unexpected End Of File.
*/
unxeof:  printf("V3: Unexpected End Of Plotfile !\n");
         exit(V3EXOK);

 }

/********************************************************/
/*!******************************************************/

 static double atonc(pos)
        char   *pos;

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
 *      19/12/85 modifierad till varkonformat R. Svedin
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

	for (i=0;i<decno;++i) {
		switch (*pos) {
			
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
/*!******************************************************/

        char *plgets(char *buf)

/*      Konvreterar ascii till flyttal i området 0 - 1.
 *
 *      In:  buf = Pekare till radbuffert.
 *
 *      Ut: *buf = Tecken från stdin/fil.
 *
 *      FV: NULL = Slut på input. 
 *
 *      (C)microform ab 1997-01-28 J.Kjellander
 *
 *       1997-12-16 WIN32, J.Kjellander
 *
 ******************************************************!*/

 {
/*
***Unix-versionen av plotprogrammen läser från standard
***input, dvs. är UNIX-filter.
*/
#ifdef UNIX
   int n;

   if ( fgets(buf,82,stdin) == NULL ) return(NULL);
   else
     {
     n = strlen(buf);
     if ( n > 0  &&  buf[n-1] == '\n' ) buf[n-1] = '\0';
     return(buf);
     }
#endif

/*
***MSW32.DLL läser med egen rutin, övriga plotprogram
***under WIN32 använder gets() precis som i UNIX.
*/
#ifdef WIN32
#ifdef V3_MSW32
extern FILE *pltfpk;

   int n;

   if ( fgets(buf,82,pltfpk) == NULL ) return(NULL);
   else
     {
     n = strlen(buf);
     if ( n > 0  &&  buf[n-1] == '\n' ) buf[n-1] = '\0';
     return(buf);
     }
#else
   return(gets(buf));
#endif
#endif
 }

/********************************************************/
