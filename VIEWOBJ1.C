/*                                             */
/*  File name : ViewOBJ1.C                     */
/*  Version 0.1                                */
/*                                             */
/*  August 18th, 1990.                         */
/*  November 9th, 1990.                        */
/*  By Kijun Park, Senior                      */
/*  Dept. of Applied Physics                   */
/*  Inha University, Inchon,Korea.             */
/*                                             */
/*  This program should compiled by            */
/*  Turbo-C 2.0                                */

#ifdef __TINY__
#error ViewOBJ graph will not run in the tiny model.
#endif

#include <stdio.h>
#include <graphics.h>


#define   RESFAC      100
#define   PRNRATIO    1.5
#define   ESC         0x1b               /* Define the escape key            */
#define   TRUE        1                  /* Define some handy constants      */
#define   FALSE       0                  /* Define some handy constants      */
#define   NumAptr     300                /* Maximum number of  Data(byte)    */



float  X[NumAptr],Y[NumAptr];       /* Aperture data                         */
float  aX,aY;

FILE   *FileInput;

int    GraphDriver;                 /* The Graphics device driver            */
int    GraphMode;                   /* The Graphics mode value               */
double AspectRatio;                 /* Aspect ratio of a pixel on the screen */
int    MaxX, MaxY;                  /* The maximum resolution of the screen  */
int    MaxColors;                   /* The maximum # of colors available     */
int    ErrorCode;                   /* Reports any graphics errors           */


/*   Function prototypes */
void GetFilename(char *FileInput);
int  ReadData(char *FileName);
void DrawMain(int Number);
void PutAptr(int x, int y, int xSize, int ySize, int Color);
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
   int   NumAperture;

      clrscr();
      puts(" \n");
      puts(" This is a Preview display for the calculated hologram plane.\n");

      GetFilename(InputFile);

      NumAperture = ReadData(InputFile);

      printf(" There are %3d apertures..\n",NumAperture);
      printf(" Please enter aperture size x [mm] => ");
      scanf("%f", &aX);
      printf(" Please enter aperture size y [mm] => ");
      scanf("%f", &aY);
      PauseText();

      DrawMain(NumAperture);
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

      for( i=1 ; i<=NumPoints ; i++)
         fscanf(stream,"%f %f", &X[i], &Y[i]);
      fclose(stream);
      puts("\n");
      puts(" Finished reading data... ");
   }
   return NumPoints;
}



/*  Main Drawing prcedure              */
void DrawMain(int Number)
{
   int     i;
   int     White, Black, x, y;
   int     xMagFactor, yMagFactor;

   Initialize();                        /* Initialize graphics devices       */

   MainWindow(" Object plane view : a Pixel = 0.01mm ");


   line(0.1*MaxX, MaxY/2, 0.9*MaxX, MaxY/2);
   line(MaxX/2, 0.1*MaxY, MaxX/2, 0.9*MaxY);
   White = MaxColors - 1;
   Black = MaxColors;
   xMagFactor = (int)(aX * RESFAC * PRNRATIO);
   yMagFactor = (int)(aY * RESFAC);

   for(i=1 ; i <= Number ; i++)
   {
       x =  MaxX/2 + (X[i] * RESFAC) * AspectRatio;
       y =  MaxY/2 - (Y[i] * RESFAC);

       PutAptr(x, y, xMagFactor, yMagFactor, White);
       putpixel(x, y, Black);
   }
   Pause();
   closegraph();
}

void PutAptr(int x, int y, int xSize, int ySize, int Color)
{
   int i,j;

   for(i = -xSize/2 ; i <= xSize/2 ; i++)

      for(j = -ySize/2 ; j <= ySize/2 ; j++)

         putpixel(x+i, y+j, Color);

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