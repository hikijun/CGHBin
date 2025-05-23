/*                                             */
/*  File name : ViewBin4.C                     */
/*  Version 1.24                               */
/*                                             */
/*  July 23rd- August 7th, 1990                */
/*  By Kijun Park, Senior                      */
/*  Dept. of Applied Physics                   */
/*  Inha University, Inchon,Korea.             */
/*                                             */
/*  This program should compiled by            */
/*  Turbo-C 2.0                                */

#ifdef __TINY__
#error ViewBin graph will not run in the tiny model.
#endif

#include <stdio.h>
#include <graphics.h>


#define   b00000000   0x00               /* Define bits data                */
#define   b00000001   0x01               /* For byte -> bits conversion     */
#define   b00000010   0x02
#define   b00000100   0x04
#define   b00001000   0x08
#define   b00010000   0x10
#define   b00100000   0x20
#define   b01000000   0x40
#define   b10000000   0x80
#define   MSB         0x07
#define   LSB         0x00
#define   ByteData    0x08

#define   ESC         0x1b               /* Define the escape key            */
#define   TRUE        1                  /* Define some handy constants      */
#define   FALSE       0                  /* Define some handy constants      */
#define   ySample     300                /* Maximum number of y Data(byte)   */



unsigned char  Buffer[ySample];          /*  Data buffer MAX - 2400 points   */
unsigned int   xSamples, ySamples;       /*  x, y Drawing points             */
unsigned int   Page;                     /*  Current page                    */

FILE *FileInput;

int    GraphDriver;                 /* The Graphics device driver            */
int    GraphMode;                   /* The Graphics mode value               */
double AspectRatio;                 /* Aspect ratio of a pixel on the screen */
int    MaxX, MaxY;                  /* The maximum resolution of the screen  */
int    MaxColors;                   /* The maximum # of colors available     */
int    ErrorCode;                   /* Reports any graphics errors           */


/*   Function prototypes */
void GetFilename(char *FileInput);
void ReadData(char *FileName);
void ReadBytes(int Bytes);
void PreviewMain(int xSamp, int ySamp);
void Initialize(void);
void PauseText(void);
void Pause(void);
void MainWindow(char *header);
void StatusLine(char *msg);
void DrawBorder(void);
int  gprintf(int *xloc, int *yloc, char *fmt, ... );


main()
{
   char  InputFile[14];

   clrscr();
   puts(" \n");
   puts(" This is a Preview display for the calculated hologram plane.\n");

   GetFilename(InputFile);

   ReadData(InputFile);

   PauseText();

   PreviewMain(xSamples, ySamples);

   printf(" \n");
   printf(" Preview draw finished .... \n");
}


/*  Function get_file_name  */
void GetFilename(char *FileInput)
{
   printf("\n\n Enter a file name for data input ->  ");
   gets(FileInput);
   printf("\n Okay, the input file name is [%s].\n", FileInput);
}


/*   This is a procedure to write sampling data to   */
/*   disk as a byte  0 ~ 255 or 0x00 ~ 0xFF format.  */
void ReadData(char *FileName)
{
   int i, j, yBytes;
   int xU, xL, yU, yL;

   printf(" \n\n");
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
   printf(" Then it will draw.\n");
}


/*  Buffered file read procedure       */
void ReadBytes(int Bytes)
{
   int k;
   for(k = 0 ; k < Bytes ; k++)
      Buffer[k] = getc(FileInput);
}


/*  Main Drawing prcedure              */
void PreviewMain(int xSamps,int ySamps)
{
   int     i, j, jj, yByte, aByte;
   int     White, Black, x, y;
   int     bits[8] = { b10000000, b01000000, b00100000,
                       b00010000, b00001000, b00000100,
                       b00000010, b00000001             };

   Initialize();                        /* Initialize graphics devices       */

   MainWindow(" Hologram plane partial view  1 * 1 ratio - Normal ");

   yByte = ySamps / 0x08;

   if (xSamps > MaxY)   xSamps = MaxY - 4;
   if (ySamps > MaxX)   ySamps = MaxX - 4;

   White = MaxColors - 1;
   Black = MaxColors;

   for(i=0 ; i < xSamps ; i++)
   {
      x = i;
      ReadBytes(yByte);                 /* Buffered Reading from a file     */

      for(j=0 ; j < ySamps ; j++)
      {
	 aByte = Buffer[j];
	 for(jj=0 ; jj < 8 ; jj++)                   /* For every bits       */
         {
	    y = j * 0x08 + jj;

	    if ((aByte & bits[jj]) == bits[jj] )     /* Compare bits         */
	       putpixel(y, x, White);
            else
	       putpixel(y, x, Black);
         }
      }
   }
   Pause();
   closegraph();
}


/*	INITIALIZE: Initializes the graphics system and reports 	*/
/*	any errors which occured.					*/
void Initialize(void)
{
  int xasp, yasp;			/* Used to read the aspect ratio*/

  GraphDriver = DETECT; 		/* Request auto-detection	*/
  initgraph( &GraphDriver, &GraphMode, "" );
  ErrorCode = graphresult();		/* Read result of initialization*/
  if( ErrorCode != grOk ){		/* Error occured during init	*/
    printf(" Graphics System Error: %s\n", grapherrormsg( ErrorCode ) );
    exit( 1 );
  }

  MaxColors = getmaxcolor() + 1;       /* Read maximum number of colors*/

  MaxX = getmaxx();
  MaxY = getmaxy();                    /* Read size of screen		*/

  getaspectratio( &xasp, &yasp );	/* read the hardware aspect	*/
  AspectRatio = (double)xasp / (double)yasp; /* Get correction factor	*/

}

/*	PAUSE: Pause until the user enters a keystroke. If the		*/
/*	key is an ESC, then exit program, else simply return.		*/
void Pause(void)
{
  static char msg[] = "Esc aborts or press a key...";
  int c;

  StatusLine( msg );                       /* Put msg at bottom of screen    */

  c = getch();	                           /* Read a character from kbd      */

  if( ESC == c ){                          /* Does user wish to leave?       */
    closegraph();                          /* Change to text mode            */
    exit( 1 );                             /* Return to OS                   */
  }

  if( 0 == c){                             /* Did use hit a non-ASCII key?   */
    c = getch();                           /* Read scan code for keyboard    */
  }

  cleardevice();                           /* Clear the screen               */
}

/*   MAINWINDOW: Establish the main window for the demo and set	  */
/*   a viewport for the demo code.                                */
void MainWindow( char *header )
{
  int height;

  cleardevice();			      /* Clear graphics screen       */
  setcolor( MaxColors - 1 );                  /* Set current color to white  */
  setviewport( 0, 0, MaxX, MaxY, 1 );         /* Open port to full screen    */

  height = textheight( "H" );                 /* Get basic text height       */

  settextjustify( CENTER_TEXT, TOP_TEXT );
  outtextxy( MaxX/2, 2, header );
  setviewport( 0, height+4, MaxX, MaxY-(height+4), 1 );
  DrawBorder();
  setviewport( 1, height+5, MaxX-1, MaxY-(height+5), 1 );

}

/*	STATUSLINE: Display a status line at the bottom of the screen.	*/
/*									*/
void StatusLine( char *msg )
{
  int height;

  setviewport( 0, 0, MaxX, MaxY, 1 );         /* Open port to full screen    */
  setcolor( MaxColors - 1 );                  /* Set current color to white  */

  settextjustify( CENTER_TEXT, TOP_TEXT );
  setlinestyle( SOLID_LINE, 0, NORM_WIDTH );
  setfillstyle( EMPTY_FILL, 0 );

  height = textheight( "H" );                 /* Detemine current height     */
  bar( 0, MaxY-(height+4), MaxX, MaxY );
  rectangle( 0, MaxY-(height+4), MaxX, MaxY );
  outtextxy( MaxX/2, MaxY-(height+2), msg );
  setviewport( 1, height+5, MaxX-1, MaxY-(height+5), 1 );

}

/*	DRAWBORDER: Draw a solid single line around the current 	*/
/*	viewport.							*/
/*									*/
void DrawBorder(void)
{
  struct viewporttype vp;
  setcolor( MaxColors - 1 );		/* Set current color to white	*/
  setlinestyle( SOLID_LINE, 0, NORM_WIDTH );
  getviewsettings( &vp );
  rectangle( 0, 0, vp.right-vp.left, vp.bottom-vp.top );
}

/*	GPRINTF: Used like PRINTF except the output is sent to the	*/
/*	screen in graphics mode at the specified co-ordinate.		*/
int gprintf( int *xloc, int *yloc, char *fmt, ... )
{
  va_list  argptr;			/* Argument list pointer	*/
  char str[140];			/* Buffer to build sting into	*/
  int cnt;				/* Result of SPRINTF for return */

  va_start( argptr, format );		/* Initialize va_ functions	*/

  cnt = vsprintf( str, fmt, argptr );	/* prints string to buffer	*/
  outtextxy( *xloc, *yloc, str );	/* Send string in graphics mode */
  *yloc += textheight( "H" ) + 2;       /* Advance to next line         */

  va_end( argptr );			/* Close va_ functions		*/

  return( cnt );			/* Return the conversion count	*/

}



/*   PauseText : Pause until the user enters a keystroke. If the             */
/*   key is an ESC, then exit program, else simply return.                   */
void PauseText(void)
{
   int c;
   printf(" \n");
   printf(" Press a key to proceed or ESC to quit ... \n");
   c = getch();                            /*  Read a character from kbd     */
   if( ESC == c ) {                        /*  Does user wish to leave?      */
      exit( 1 );                           /*  Return to OS                  */
   }
   if( 0 == c ){                           /*  Did use hit a non-ASCII key?  */
     c = getch();                          /*  Read scan code for keyboard   */
   }
}

