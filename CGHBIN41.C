/*                                                                           */
/*  File : CghBin4.C                                                         */
/*  Version 1.98                                                             */
/*  Binary Computer Generated Hologram                                       */
/*                                                                           */
/*  July 23rd - August 7th, 1990                                             */
/*  By Kijun Park, Senior                                                    */
/*  Dept. of Applied Physics                                                 */
/*  Inha University, Inchon,Korea.                                           */
/*                                                                           */

#include  <stdio.h>
#include  <stdlib.h>
#include  "complex.h"

#define   b00000000     0x00                 /* Binary to Hex Definitions    */
#define   b00000011     0x03                 /* For bits -> byte conversion  */
#define   b00001100     0x0C
#define   b00110000     0x30
#define   b11000000     0xC0
#define   MSB           0x07                 /* Most Significant Bit.        */
#define   LSB           0x00                 /* Least Significant Bit.       */

#define   deg2rad(a)    3.1415926535 * a / 180.0    /* Angle conversion      */
#define   rad2deg(a)    180.0 * a / 3.1415926535    /* Degrees <=> Radians   */

#define   PI            3.1415926535         /* Pi constant                  */
#define   TOL           1.0e-28              /* Torelence                    */
#define   TimeFactor    4.105e-3             /* Time requierment for a point */
#define   Threshold     0.0                  /* Hologram Threshold value     */
#define   ESC           0x1b                 /* Define the escape key        */
#define   TRUE          1                    /* Define some handy constants  */
#define   FALSE         0                    /* Define some handy constants  */
#define   xMaxSample    3000                 /* Maximum number of x,y        */
#define   yMaxSample    300                  /* Sampling points.             */
#define   MaxAptrNum    100                  /* Maximum number of Apertures. */


/* Variables */

float          X[MaxAptrNum], Y[MaxAptrNum];  /* Object position             */
unsigned char  Buffer[yMaxSample];            /* Calculated data             */

float  WvLen;                             /*  Wavelength                     */
float  aX, aY;                            /*  Aperture size                  */
float  CarAngle;                          /*  Reference beam angle           */
float  z0;                                /*  Distance : object to hologram  */
float  SampleRateMod;                     /*  Modify sampling rate           */
int    xSamples, ySamples;                /*  Number of x,y sampling points  */
int    page;                              /*  Number of Pages to sample      */
int    bits[4] = { b11000000, b00110000,
                   b00001100, b00000011 };

FILE   *FileOut;

/* Function prototypes */

double    Bandwidth();
double    ProDistance();
double    Distance();
double    Sinc(double x);
fcomplex  Ediff();
fcomplex  Eref();

void  Title(void);
void  InitConst(void);
void  ReadSetting(void);
void  Display(double SamplingRate);
void  GetFilename(char *file1, char *file2);
int   ReadData(char *FileName);
void  SaveInitData(char *FIleName, int xSamps, int ySamps, int PageNum);
void  WriteData(int Byte);
void  PauseTxt(void);


main()
{
   char      InputFile[24], OutputFile[24];
   int       i, j, jj, k, NumAptr;
   int       xSamplesHalf, ySamplesHalf, ySamplesCalc;
   int       Byte0, yBytes, BitsInByte, aByte;

   float     NumLoops, TimeMin;            /* Total calculation time vars.   */
   double    CarBandwidth;
   double    ObjBandwidth, AptrBandwidth;  /* Bandwidth variables            */
   double    kSineCarAngle, z0Sqr;         /* Some calcualted constants      */
   double    WvNum;                        /* Wave number                    */
   double    aXWvNumHalf, aYWvNumHalf;
   double    xSamplingRate, ySamplingRate; /* Sampling rate                  */
   double    x, y, dx, dy, dxSqr, dySqr;   /* x,y position at Hologram plane */
   double    Rx, Ry, R, Eh;                /* Distance and Summed Wave front */
   fcomplex  Er, Ed;                       /* Calculated wave front Diff/Ref */

   Title();
   InitConst();                            /* Initialize Constants           */
   ReadSetting();                          /* Read Some Constants from *.SET */

   GetFilename(InputFile, OutputFile);     /* Get Input/Output Filename      */

   NumAptr = ReadData(InputFile);          /* Read object data from file     */

   WvNum = 2 * PI / WvLen;                 /* Calculate Wave Number (k)      */

   AptrBandwidth = z0 * WvLen / aX;
   ObjBandwidth  = Bandwidth(NumAptr);        /* Object's bandwidth          */
   CarBandwidth  = WvLen / sin(CarAngle);     /* Carrier bandwith            */

   if ( CarBandwidth < ObjBandwidth)          /* Minimum sampling rate       */
       xSamplingRate =  CarBandwidth * SampleRateMod;
   else
       xSamplingRate =  ObjBandwidth * SampleRateMod;

   ySamplingRate = xSamplingRate * 2.0;

   PauseTxt();

   Display(xSamplingRate);                    /* Display current settings    */

   printf(" \n");                             /* Display calculations        */
   printf(" Current calculations are : \n");
   printf(" Wave number (k)               = %f (1/mm) \n", WvNum);
   printf(" Carrier bandwidth  (a Cycle)  = %f (mm)   \n", CarBandwidth);
   printf(" Object bandwidth              = %f (mm)   \n", ObjBandwidth);
   printf(" Apture bandwidth              = %f (mm)   \n", AptrBandwidth);
   printf(" Sampling rate modification    = %f        \n", SampleRateMod);
   printf(" Sampling rate   x             = %f (mm)   \n", xSamplingRate);
   printf(" Sampling rate   y             = %f (mm)   \n", ySamplingRate);
   printf(" Number of apertures of Object = %d        \n", NumAptr);

   PauseTxt();

/*  Find Discrete Fourier Trnasform     */
/*  at the hologram plane.              */
/*  Main Calculations                   */

   yBytes       = ySamples / 8;        /* Calculate some handy Constants.    */

   xSamplesHalf = xSamples / 2;        /* Half of x, y samping points        */
   ySamplesHalf = ySamples / 4;

   BitsInByte   = 4;
   ySamplesCalc = yBytes * BitsInByte;

   kSineCarAngle = WvNum * sin(CarAngle);   /* Pre-calc. for Carrier angle  */
   z0Sqr = z0 * z0;                         /* Pre-calc. distance square    */
   aXWvNumHalf = aX * PI / WvLen;
   aYWvNumHalf = aY * PI / WvLen;

   NumLoops = (float)xSamples * (float)ySamplesCalc * (float)NumAptr;
   TimeMin  = NumLoops * TimeFactor / 60;

   printf(" Proceeding calculations. \n");
   printf(" In the 8087 equipped XT clone :");
   printf(" it will take approximately %f minutes.\n", TimeMin);
   printf(" \n");


   FileOut = fopen(OutputFile, "wb");                  /* Open output file ! */
   SaveInitData(OutputFile, xSamples, ySamples, page); /* Save initial data  */

      /*                               */
      /* Main Loop for DFT Calculation */
      /*                               */
      for(i=0 ; i < xSamples ; i++)
      {
         x =  (i - xSamplesHalf) * xSamplingRate;
         Er = Eref(x, kSineCarAngle);
         for(j=0 ; j < yBytes ; j++)
         {
            Byte0 = j * BitsInByte;
            aByte = b00000000;
            for(jj=0 ; jj < BitsInByte ; jj++)
            {
               y = (Byte0 + jj - ySamplesHalf) * ySamplingRate;
               Ed = Cmplx(0.0, 0.0);
               for(k=1 ; k <= NumAptr ; k++)
               {
                  dx = x - X[k];
                  dy = y - Y[k];
                  dxSqr = dx * dx;
                  dySqr = dy * dy;
                  Rx = ProDistance(z0Sqr, dxSqr);
                  Ry = ProDistance(z0Sqr, dySqr);
                  R  = Distance(z0Sqr, dxSqr, dySqr);
                  Ed = Cadd(Ed, Ediff(WvNum, aXWvNumHalf, aYWvNumHalf
                                           , R, Rx, Ry, dx, dy));
               }
               Eh = Ed.r * Er.r + Ed.i * Er.i;
               if ( Eh > Threshold )
                  aByte = aByte + bits[jj];
               else
                  aByte = aByte + b00000000;
            }
            Buffer[j] = aByte;
         }
         printf("%5d",i);
         WriteData(yBytes);
      }
/*                    */
/* Main Loop Finished */
/*                    */

   fclose(FileOut);                    /* Close Output file                  */
   printf(" \n");
   printf(" End of Calculations..\n");
   exit(0);
}  /* End : Main() */


double ProDistance(double r0Sqr, double DxSqr)
{
   double R;
   R = sqrt(r0Sqr + DxSqr);
   return R;
}


double Distance(double r0Sqr, double DxSqr, double DySqr)
{
   double R;
   R = sqrt(r0Sqr + DxSqr + DySqr);
   return R;
}


fcomplex Ediff(double k, double akHalf, double bkHalf, double R
                       , double Rx, double Ry, double Dx, double Dy)
{
   double xBeta, yBeta, d;
   fcomplex e;

   xBeta = Dx / Rx * akHalf;
   yBeta = Dy / Ry * bkHalf;
   d = Sinc(xBeta) * Sinc(yBeta) / R;
   e = Cremul(Cexp(Cmplx(0.0,-k * R)), d);
   return e;
}


fcomplex Eref(double x, double kSineTheata)
{
   double z;
   fcomplex r;
   z =  x * kSineTheata;
   r = Cexp(Cmplx(0.0, z));
   return r;
}

double Sinc(double x)
{
   double sinc;
   if (fabs(x) < TOL)
      sinc = 1.0;
   else
      sinc = sin(x) / x;
   return sinc;
}


void InitConst(void)                  /* Initialize some constants           */
{
   aX = 0.2;
   aY = 0.2;
   WvLen = 632.8e-6;
   CarAngle = deg2rad(2.50);
   z0 = 130.0;
   SampleRateMod = 0.250;
   page = 1;
   xSamples = 2000;
   ySamples = 2000;
}


void ReadSetting(void)
{

   static char FileName[] = "CGHBIN.SET";
   FILE *f1;

   printf(" Now read initial settings from [%s].\n", FileName);

   if((f1 = fopen(FileName, "r")) == NULL)
   {
      printf(" \n");
      printf(" Cannot open settings file : [%s]  !\n\n", FileName);
      printf(" Using default values !\n\n");
   }
   else
   {
      fscanf(f1,"%f %f", &aX, &aY);
      fscanf(f1,"%e", &WvLen);
      fscanf(f1,"%f", &CarAngle);
      CarAngle = deg2rad(CarAngle);
      fscanf(f1,"%f", &z0);
      fscanf(f1,"%f", &SampleRateMod);
      fscanf(f1, "%d", &page);
      fscanf(f1,"%d %d", &xSamples, &ySamples);
      fclose(f1);
      printf(" \n");
      printf(" Using [%s] values !\n", FileName);
   }
}


void Display(double SamplingRate)
{
   double xHsize,yHsize;

   printf(" \n");
   printf(" Current settings are : \n");
   printf(" Object aperture size  = %f * %f (mm)\n", aX, aY);
   printf(" Reference beam angle  = %f degrees\n", rad2deg(CarAngle));
   printf(" Beam wavelength       = %f (nm) \n", WvLen * 1e+6);
   printf(" Distance to hologram  = %f (mm) \n", z0);
   printf(" Number of pages       = %d \n", page);
   xHsize = SamplingRate * xSamples;
   yHsize = SamplingRate * ySamples;
   printf(" Actual hologram size  = %f * %f (mm)\n", xHsize, yHsize);
   printf(" Sampling points (x*y) = %d * %d (points) \n", xSamples, ySamples);
}



/*  Function get_file_name  */

void GetFilename(char *file1, char *file2)
{
   printf("\n Enter a filename for data input.  -> ");
   gets(file1);
   printf("\n Enter a filename for data output. -> ");
   gets(file2);   
/*   while (strcmp(gets(file2), file1) == NULL)
      printf("\n Please enter different file name for the output.\n");
*/
   printf(" \n");
   printf(" The input  filename for data is [%s].\n", file1);
   printf(" The output filename for data is [%s].\n", file2);
}


/*  Procedure : Reading Object data as apertures from a file                 */
int ReadData(char *FileName)
{
   int i, NumPoints;
   FILE *stream;

   if((stream = fopen(FileName, "r")) == NULL)  {
      printf(" Cannot open file [%s].\n", FileName);
      printf(" Exit with Error \n\n");
      exit(1);
   }
   else {
      stream = fopen(FileName, "r");
      fscanf(stream, "%d", &NumPoints);
      if ( NumPoints < 2 ) {
         printf(" Number of apertures of the object must exceed two ! \n");
         printf(" Program terminated with error.\n");
         exit(1);
      }
      for( i=1 ; i<=NumPoints ; i++) {
         fscanf(stream,"%f %f", &X[i], &Y[i]);
         printf(" Aperture number %3d  ->  %f, %f \n", i, X[i], Y[i]);
      }
      fclose(stream);
      puts("\n");
      puts(" Finished reading data... ");
   }
   return NumPoints;
}



/*   This is a procedure to write sampling data to                           */
/*   disk as byte or  0 ~ 255 or 0x00 ~ 0xFF format                          */
void SaveInitData(char *FileName, int xSamps, int ySamps, int PageNum)
{
   int i, j, yByte;
   int xU, xL, yU, yL;

   xU = xSamps / 0xFF;                 /* x Samples as (xU-xL)Hex points.    */
   xL = xSamps & 0xFF;
   yU = ySamps / 0xFF;                 /* y Samples as (yU-yL)Hex points.    */
   yL = ySamps & 0xFF;
   PageNum = PageNum & 0xFF;
   yByte = ySamps / 8;                 /* Devide y Samples with 8bits.       */

   printf(" Proceeding Calculations also saving data to disk.\n");
   printf(" Now saving data to disk as byte : ");
   printf(" %d*%d or %d*(%d*%d) structure.\n",xSamps,ySamps,xSamps,yByte,8);
   printf(" Page Number %d. \n", PageNum);
   printf(" File name for output is %s.\n", FileName);

   putc(PageNum, FileOut);
   putc(xU, FileOut);                  /* Put first four byte as x,y samples */
   putc(xL, FileOut);
   putc(yU, FileOut);
   putc(yL, FileOut);

   printf(" Initial data saved.\n");
   printf(" Proceed to Calculations.\n");
}


void WriteData(int Byte)
{
   int n;
   for(n = 0 ; n < Byte ; n++)
      putc(Buffer[n], FileOut);
}


double Bandwidth(int NumAptr)
{
   int     i;
   double  Xmax, Ymax, Xmin, Ymin, value, ObjBW;

   Xmax = Xmin =  X[1];
   Ymax = Ymin =  Y[1];
   for(i=2 ; i <= NumAptr ; i++)
   {
      if(Xmax <= X[i])  Xmax = X[i];
      if(Xmin >  X[i])  Xmin = X[i];
      if(Ymax <= Y[i])  Ymax = Y[i];
      if(Ymin >  Y[i])  Ymin = Y[i];
   }
   printf(" Xmax = %f, Xmin = %f, Ymax = %f, Ymin = %f. \n"
                                           , Xmax, Xmin, Ymax, Ymin);

   if((Xmax-Xmin) > (Ymax-Ymin))
      value = Xmax - Xmin;
   else
      value = Ymax - Ymin;

   if (value == 0.0)
      ObjBW = WvLen * z0 / aX;
   else
      ObjBW = WvLen * z0 / value;

   return ObjBW;
}

/*   PauseTxt : Pause until the user enters a keystroke. If the              */
/*              key is an ESC, then exit program, else simply return.        */
void PauseTxt(void)
{
   int c;
   printf(" \n");
   puts(" Press a key to proceed or ESC to quit ... ");
   c = getch();                            /*  Read a character from kbd     */
   if( ESC == c ) {                        /*  Does user wish to leave?      */
      exit( 1 );                           /*  Return to OS                  */
   }
   if( 0 == c ){                           /*  Did use hit a non-ASCII key?  */
     c = getch();                          /*  Read scan code for keyboard   */
   }
}


void Title(void)
{

   puts(" \n ");
   puts("       This is a Computer Generated Hologram Calculation Program ");
   puts(" \n");
   puts(" It will sample four times in a periode along the axis. ");
   puts(" It will represent a hologram as binary intensity levels. ");
   puts(" \n ");
   puts(" CghBin4xx.xxx ");
   puts(" Version 1.94  ");
   puts(" July 23rd - August 7th, 1990. ");
   puts(" \n");
   puts("   ");
   puts(" By Kijun Park, Senior. ");
   puts(" Department of Applied Physics. ");
   puts(" Inha University, Inchon,Korea. ");
   PauseTxt();
}
