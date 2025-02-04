#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdio.h>
#include <dos.h>
#include <fcntl.h>
#include <conio.h>
#include "error.h"
#include "gamemem.h"

#define clear_screen(x,y)  memset( x, y, 64000 )
#define WaitForSync   while(inp(0X3DA) & 0x08);while(!(inp(0x3DA) & 0X08));


#define MAXINDEXES 65535 /* used by open_vbackground() and put_vbackground() */
#define MAXINDEXES_2 131070   /* MAXINDEXES * 2 */
#define MAXINDEXES_3 196605   /* MAXINDEXES * 3 */
#define MAXINDEXES_4 262140

long index=0;

typedef long int counter_type;

typedef unsigned char * page_type;

/* virtual background */
typedef struct vbackground_tag {
        char **bitmap;           /* 2d cause index !> 65535 for 1d */
        short ulx, uly;
        short width, height;
} vbackground_type;

typedef struct image_tag {
	unsigned char *bitmap;
        short width;
        short height;
} image_type;

typedef struct {
	char    manufacturer;
	char    version;
	char    encoding;
	char    bits_per_pixel;
        short   xmin,ymin;
        short   xmax,ymax;
        short   hres;
        short   vres;
	char    pallete[48];
	char    reserved;
	char    colour_planes;
        short   bytes_per_line;
        short   pallete_type;
	char    filler[58];
		} PCXHEAD;
PCXHEAD   header;

/* The variable header is a struct that will contain all the usefull picture
information shown in the above fields.  */

unsigned int width,depth;    /* will hold the header info for width and depth*/
unsigned int bytes;

typedef unsigned char palette_type;
palette_type palette[768];

unsigned char *videoscreen=(unsigned char *)0xA0000;

void cpalette(char color, char r, char g, char b );
void init(void);
void deinit(void);           /* return to normal     */
void setVGApalette(char *p); /* set the vga pallete  */
void open_image(char *filename, image_type **image);
void open_vbackground(char *filename, vbackground_type **vbackground);
void close_image(image_type **image);
void close_vbackground(vbackground_type **image);
void put_image (image_type *image, short ulx, short uly, char transparent, page_type where );
void put_vbackground (vbackground_type *vbackground, char *where );
void fade_to_black( palette_type *, int );
void fade_to_palette( palette_type *, int );
long sqrt_FAST(long v);
void load_palette(char *filename);
void scrollx_single( int , char *, int );
void DrawScreen( char *from );
void MakePage( unsigned char **page );
void DestroyPage( unsigned char **page );

void Mosaic( image_type *image, int block_size, int ulx, int uly, char transparent, page_type where );
void DrawFilledRectangle( int ulx, int uly, int size, int color, page_type where );

image_type *VBackgroundToImage( vbackground_type *vbackground );
void Zoom( image_type *image, double ratio, int ulx, int uly, page_type where );

#endif
