#include <stdlib.h>
#include <stdio.h>


#define   ByteData    0x08
#define   Dec         10
#define   Hex         16
#define   ESC         0x1b               /* Define the escape key            */
#define   BEL         0x07
#define   FF          0x0C
#define   MODE        71                 /* ASCII "G"                        */
#define   TRUE        1                  /* Define some handy constants      */
#define   FALSE       0                  /* Define some handy constants      */
#define   ySample     300                /* Maximum number of y Data(byte)   */



unsigned char  amp[ySample];             /*  Data buffer MAX - 2400 points   */
unsigned int   xSamples, ySamples;       /*  x, y Drawing points             */
unsigned int   yBytes;
unsigned int   Page;                     /*  Current page                    */

FILE *FileInput;
FILE *FileOutput;

void GetFilename(char *File1, char *File2);
void ReadData(char *FileName);
void PauseText(void);

main()
{
   char  InputFile[24], OutputFile[24];
   char  yStr[4], xStr[4];
   int   ch, i, j, k;

   GetFilename(InputFile, OutputFile);
   ReadData(InputFile);


   printf(" \n");
   printf(" It will make 300 DPI print file  ! \n");

   FileOutput =  fopen(OutputFile, "wb");

   itoa(yBytes, yStr, Dec);
   itoa(xSamples, xStr, Dec);

   putc(ESC, FileOutput);
   putc(BEL, FileOutput);
   putc(MODE, FileOutput);
   fprintf(FileOutput,"%s,%s,%1d,%1d;", yStr, xStr,1,1);



   for(i=0 ; i < xSamples ; i++) {
      printf(".");
      for(j=0 ; j < yBytes ; j++)  {
         putc(getc(FileInput), FileOutput);
      }
   }
   fclose(FileInput);
   putc(ESC, FileOutput);
   putc(BEL, FileOutput);
   putc(FF, FileOutput);
   fclose(FileOutput);
   printf(" \n");
   printf(" Convert finished .... \n");
}


/*  Function get_file_name  */
void GetFilename(char *File1, char *File2)
{
   printf("\n Enter a file name for data input ->  ");
   gets(File1);
   printf("\n Enter a file name for data output -> ");
   gets(File2);
   printf("\n\n");
   printf(" The input filename is  [%s].\n", File1);
   printf(" The output filename is [%s].\n", File2);
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
