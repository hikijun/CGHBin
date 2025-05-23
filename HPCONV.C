/*
       FileName :     HPConv.C
       Version  :     1.0
       Date     :     January 19th, 1991.
       Author   :     Kijun Park
                      Dept. of Applied Physics, Inha University
                      Inchon, 402-751, Korea.

                -     This program will produce a plot file for HP Printers.
*/
#include <stdio.h>

#define   ESC         0x1b               /* Define the escape key            */
#define   BEL         0x07
#define   FF          0x0C
#define   ByteData    0x08
#define   TRUE        1                  /* Define some handy constants      */
#define   FALSE       0                  /* Define some handy constants      */

unsigned int   xSamples, ySamples;       /*  x, y Drawing points             */
unsigned int   yBytes;
unsigned int   DPI;
unsigned int   Page;                     /*  Current page                    */

FILE *FileInput;
FILE *FileOutput;

void GetFilename(char *File1, char *File2);
void ReadData(char *FileName);
void InitPrn(int x, int y, int dpi);

main()
{
   char  InputFile[24], OutputFile[24];
   int   i, j;

   GetFilename(InputFile, OutputFile);
   ReadData(InputFile);

   printf("\n Please enter resolution [300/150/100/75] ");
   scanf("%d", &DPI);
   printf(" It will make %d DPI print file  ! \n\n", DPI);

   FileOutput = fopen(OutputFile, "wb");   /* Open desired file              */

   InitPrn(100, 100, DPI);

   for(i=0 ; i < xSamples ; i++)
   {
      printf(".");
      fprintf(FileOutput,"\33*b%dW", yBytes);

      for(j=0 ; j < yBytes ; j++)
         fputc(getc(FileInput), FileOutput);
   }
   fprintf(FileOutput, "\33*rB");
   fclose(FileInput);
   fprintf(FileOutput, "\f\33E");
   printf(" \n");
   printf(" Plot finished .... \n");
}


/*  Function GetFileName  */
void GetFilename(char *File1, char *File2)
{
   printf("\n Enter a file name for data input ->  ");
   gets(File1);
   printf("\ Enter a file name for data output ->  ");
   gets(File2);
   printf("\n\n");
   printf(" The input filename is  [%s].\n", File1);
}


/*   This is a procedure to write sampling data to   */
/*   disk as a byte  0 ~ 255 or 0x00 ~ 0xFF format.  */
void ReadData(char *FileName)
{
   int xU, xL, yU, yL;

   printf(" \n");
   printf(" Now reading data from disk as byte :\n");

   FileInput = fopen(FileName, "rb");   /* Open desired file                 */

   Page = getc(FileInput);              /* Read the number of current page   */
   xU = getc(FileInput);                /* It reads number of x,y samples    */
   xL = getc(FileInput);                /* from first four bytes in the file */
   yU = getc(FileInput);
   yL = getc(FileInput);

   xSamples = xU * 256 + xL;            /* Convert a couple of bytes into    */
   ySamples = yU * 256 + yL;            /* integer number.                   */

   yBytes   = ySamples / ByteData;      /* Devide y Samples with 8bits       */

   printf(" Current page number %d.\n",Page);
   printf(" %d * %d or  %d * (%d * %d) structure.\n"
            , xSamples, ySamples, xSamples, yBytes, 0x08);
   printf(" File name for input is [%s].\n\n", FileName);
   printf(" Now reading %d*%d data (bytes) from file.\n", xSamples, yBytes);
   printf(" Then Convert..\n");
}


void InitPrn (int x, int y, int dpi)
{
   fprintf(FileOutput, "\33E");
   fprintf(FileOutput, "\33*p%dX\33*p%dY", x, y);/* Set pen position       */
   fprintf(FileOutput, "\33*t%dR", dpi);         /* Set Resolution in DPI  */
   fprintf(FileOutput, "\33*r1A");               /* Plot from current position */
}
