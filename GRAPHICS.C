#include "wad.h"
#include "graphics.h"

void MakePage( unsigned char **page ) {
	if( ((*page)=(unsigned char *)GameMalloc( sizeof(unsigned char)*64000 )) == NULL )
		Error( "Not enough memory.\n" );
}

void DestroyPage( unsigned char **page ) {
	free( (*page) );
}

void DrawScreen( char *from ) {
        WaitForSync;
	memcpy( videoscreen, from, 64000 );
}

void scrollx_single( int pixels, char *what, int size ) {
	int i;
	char *buffer;

        if( (buffer = (char *)GameMalloc( size )) == NULL)
		Error( "Not enough memory.\n" );
	for(i=0;i<200;++i) {
		buffer[ (i<<8)+(i<<6) ] = what[ (i<<8)+(i<<6) ];
	  //	memcpy( &(buffer[(i<<8)+(i<<6) + 1]), &(what[ (i<<8)+(i<<6) + 1]), 319 );
	}
	memcpy( what, buffer, size );
}

void close_image(image_type **image) {
//	free( (*image)->bitmap );
//	free( (*image) );
}

void close_vbackground(vbackground_type **vbackground) {
        int i,
		 number_of_i=((*vbackground)->height*(*vbackground)->width)/MAXINDEXES;

//	for(i=0; i<number_of_i; ++i )
//		free( (*vbackground)->bitmap[i] );
//	free( (*vbackground)->bitmap[number_of_i] );
//	free( (*vbackground)->bitmap );
//	free( (*vbackground) );
}

void init( void ) {
   union REGS r;
   r.x.eax = 0x13;
   int386( 0x10, &r, &r );
   /*
	asm {
		mov ax, 13h
		int 10h
	};
    */
}
void load_palette(char *filename)
{
	int i;
        file_type *fp;
	PCXHEAD dummy;

        if((fp=OpenFile(filename)) != NULL){
			/* read in the header */
                  if(wad_fread((char *) &dummy,sizeof(PCXHEAD),1,fp) == 1){
		  /*check to make sure it's a picture */
			 if(dummy.manufacturer==0x0a && dummy.version ==5){
                                 if(!wad_fseek(fp,-769L,SEEK_END)){
                                         if(wad_fgetc(fp) == 0x0c && wad_fread(palette,1,768,fp) == 768) {
                                                 wad_fseek(fp,128L,SEEK_SET);
					 }
					 else
						 Error("error reading pallete");
				 }
				 else
					 Error("error seeking to pallete");
			}
			else
				 Error("not a 256 color pcx file.\n");
		}
		else
			Error("error reading %s.\n",filename);
	}
	else
		Error("Error opening %s.\n",filename);

CloseFile(fp);

//convert eight bits to six bits
for (i=0;i<768;i++) palette[i]=palette[i] >> 2;
}

void cpalette( char color, char r, char g, char b )
{
     outp( 0x3c8, color );
     outp( 0x3c9, r     );
     outp( 0x3c9, g     );
     outp( 0x3c9, b     );
}

void put_vbackground (vbackground_type *vbackground, char *where )
{
        int x, y;
	counter_type byte=((vbackground->width*vbackground->uly)+vbackground->ulx);
        int add=(vbackground->width-320);

	for ( y = 0; y < 200; ++y) {
		for ( x = 0; x < 320; ++x) {
			/* determine which array has the byte */
			if( byte < MAXINDEXES )
			  where[( (x) + (((y) << 8) + ((y) << 6)))] = vbackground->bitmap[0][byte];
			else if( byte < MAXINDEXES_2 )
			  where[( (x) + (((y) << 8) + ((y) << 6)))] = vbackground->bitmap[1][byte-MAXINDEXES];
			else if( byte < MAXINDEXES_3 )
			  where[( (x) + (((y) << 8) + ((y) << 6)))] = vbackground->bitmap[2][byte-MAXINDEXES_2];
			else if( byte < MAXINDEXES_4 )
			  where[( (x) + (((y) << 8) + ((y) << 6)))] = vbackground->bitmap[3][byte-MAXINDEXES_3];
			++byte;
		}
		byte += add;
	}
}

void put_image( image_type *image, short ulx, short uly, char transparent, page_type where )
{
	counter_type x, y, byte=0;
        int ystop=uly+image->height,
		 xstop=ulx+image->width;

	for ( y = uly; y < ystop; ++y)
		for ( x = ulx; x < xstop; ++x)
		{
		  if(x>-1 && y>-1 && x<320 && y<200)
			if ( image->bitmap[byte] != transparent )
				where[( x + (y << 8) + (y << 6))] = image->bitmap[byte];
			++byte;
		}
}


void open_vbackground(char *filename, vbackground_type **vbackground )
{
        int x,c;
	counter_type i=0;
	long int   inc;
        file_type *fp;
	c=0;


	if ( ((*vbackground) = (vbackground_type *)malloc( sizeof(vbackground_type) )) ==NULL )
		Error( "Not enough memory.\n" );

        if((fp=OpenFile(filename)) != NULL){
			/* read in the header */
                  if( wad_fread( &header, sizeof(PCXHEAD), 1, fp) == 1 ){
		  /*check to make sure it's a picture */
			 if(header.manufacturer==0x0a && header.version ==5){
                                 if(!wad_fseek(fp,-769L,SEEK_END)){
                                         if(wad_fgetc(fp) == 0x0c && wad_fread(palette,1,768,fp) == 768) {
						 int number_of_i, leftover_j; /* for allocated bitmap */

                                                 wad_fseek(fp,128L,SEEK_SET);
						 /*allocate a big buffer */
						 (*vbackground)->width  = ((header.xmax - header.xmin)+1);
						 (*vbackground)->height = ((header.ymax - header.ymin)+1);

						 number_of_i = (((*vbackground)->width)*((*vbackground)->height)) / MAXINDEXES;
						 leftover_j  = (((*vbackground)->width)*((*vbackground)->height)) % MAXINDEXES;

						 /***** allocate memory for bitmap *****/
						 if( (((*vbackground)->bitmap) = (char **)malloc( sizeof(char *)*(number_of_i+1) )) == NULL )
							 Error( "Ran out of memory.\n");
						 for(i=0; i<number_of_i; ++i ) {
							 if( ((*vbackground)->bitmap[i] = (char *)malloc( sizeof(char)*MAXINDEXES )) == NULL )
								 Error( "Ran out of memory.\n");
						 }
						 if( ((*vbackground)->bitmap[number_of_i] =  (char *)malloc( sizeof(char)*leftover_j )) == NULL )
							 Error( "Ran out of memory.\n");

						 bytes=header.bytes_per_line;
					} else
						 Error("error reading pallete");
				}else
					Error("error seeking to pallete");
			}else
				Error("not a 256 color pcx file.\n");
	  }else
		  Error("error reading %s.\n",filename);
 } else
	 Error ("Error opening %s.\n",filename);

/***********************************************************************/
	for (i = 0; i < (((*vbackground)->height)*((*vbackground)->width)); ++i){
                c=wad_fgetc(fp) & 0xff;  /* if it's a run of bytes field */

		if((c & 0xc0) == 0xc0) { /* and off the high bits */
			x=c & 0x3f;
			/* run the byte */
                        c=wad_fgetc(fp);
			while(x--) {
				if( i < MAXINDEXES )
					(*vbackground)->bitmap[0][i] = c;
				else if( i < MAXINDEXES_2 )
					(*vbackground)->bitmap[1][i-MAXINDEXES] = c;
				else if( i < MAXINDEXES_3 )
					(*vbackground)->bitmap[2][i-MAXINDEXES_2] = c;
				else if( i < MAXINDEXES_4 )
					(*vbackground)->bitmap[3][i-MAXINDEXES_3] = c;
				++i;
			}
			i--;
		}
		/*else just store it */
		else {
			if( i < MAXINDEXES )
				(*vbackground)->bitmap[0][i] = c;
			else if( i < MAXINDEXES_2 )
				(*vbackground)->bitmap[1][i-MAXINDEXES] = c;
			else if( i < MAXINDEXES_3 )
				(*vbackground)->bitmap[2][i-MAXINDEXES_2] = c;
			else if( i < MAXINDEXES_4 )
				(*vbackground)->bitmap[3][i-MAXINDEXES_3] = c;
		}
	}
	(*vbackground)->ulx = (*vbackground)->uly = 0;
        CloseFile(fp);
}


void open_image(char *filename, image_type **image )
{
	int size,x,c,count;
	long int n;
	counter_type i=0;
	long int   inc;
        file_type *fp;
	c=0;

        if( ((*image) = (image_type *)GameMalloc( sizeof(image_type) )) == NULL )
           Error( "Not enough memory for this.\n");

        if((fp=OpenFile(filename)) != NULL){
			/* read in the header */
                  if(wad_fread((char *) &header,sizeof(PCXHEAD),1,fp) == 1){
		  /*check to make sure it's a picture */
			 if(header.manufacturer==0x0a && header.version ==5){
                                 if(!wad_fseek(fp,-769L,SEEK_END)){
                                         if(wad_fgetc(fp) == 0x0c && wad_fread(palette,1,768,fp) == 768) {
                                                 wad_fseek(fp,128L,SEEK_SET);
						 /*allocate a big buffer */
						 (*image)->width  = (header.xmax - header.xmin) +1;
						 (*image)->height = (header.ymax - header.ymin)+1;
						 if ( ((*image)->bitmap = (unsigned char*)GameMalloc( 1+sizeof(char)*(((*image)->width)*((*image)->height)) )) == NULL )
							 Error( "Ran out of memory.\n" );
						 bytes=header.bytes_per_line;
					} else
						 Error("error reading pallete");
				}else
					Error("error seeking to pallete");
			}else
				Error("not a 256 color pcx file.\n");
	  }else
		  Error("error reading %s.\n",filename);
 } else
	 Error ("Error opening %s.\n",filename);

/***********************************************************************/

	for (i = 0; i < (((*image)->height)*((*image)->width)); ++i){
                c=wad_fgetc(fp) & 0xff;  /* if it's a run of bytes field */

		if((c & 0xc0) == 0xc0) { /* and off the high bits */
			x=c & 0x3f;
			/* run the byte */
                        c=wad_fgetc(fp);
			while(x--)
			{
				(*image)->bitmap[i++] = c;
			}
			i--;
			}
			/*else just store it */
			else
			{
				(*image)->bitmap[i] = c;
			}
		}
         CloseFile(fp);
}

void  deinit(void)   /*turn off graphics card */
{
   union REGS r;
   r.x.eax = 0x03;
   int386( 0x10, &r, &r );
   /*
	asm{
		mov ax,03h
		int 10h
	};
   */
}

void setVGApalette(char *p)   /*set the VGA palette to RGB buffer p */
{

	int cv;
	for(cv = 0; cv <= 255; cv++)
			cpalette(cv,p[cv*3],p[cv*3+1],p[cv*3+2]);
/*
	union REGS r;
	struct SREGS sr;
	int i;
      
	r.x.eax=0x1012;
	r.x.ebx=0;
	r.x.ecx=256;
	r.x.edx=FP_OFF(p);
	sr.es=FP_SEG(p);
        int386x(0x10,&r,&r,&sr);  
*/         
}

long sqrt_FAST(long v)
{
	 int		i;
	 unsigned	long result,tmp;
	 unsigned	long low,high;

if (v <= 1L) return((unsigned)v);

low = v;
high = 0L;
result = 0;

for (i = 0; i < 16; i++)
	 {
	 result += result;
	 high = (high << 2) | ((low >>30) & 0x3);
	 low <<= 2;

	 tmp = result + result + 1;
	 if (high >= tmp)
	{
	result++;
	high -= tmp;
	}
	 }

if (v - (result * result) >= (result - 1))
	 result++;

return(result);
}

void fade_to_black( palette_type *pal, int time )
{
	int index, notfinished = 1;
        palette_type temp_pal[ 768 ];

        memcpy( temp_pal, pal, sizeof( char )*768 );

	while ( notfinished )
	{
		notfinished = 0;
		for ( index = 0; index < 768; ++index )
		{
                        if ( temp_pal[ index ] > 0 ){
                              notfinished = 1;
                              --temp_pal[ index ];
                        }
                        else if ( temp_pal[ index ] < 0 ){
                              notfinished = 1;
                              ++temp_pal[ index ];
                        }
		}
                setVGApalette( temp_pal );
                delay(time);
	}
}

void fade_to_palette(palette_type *pal, int time )
{
	int index, notfinished = 1;
        palette_type temp_pal[768];

        memset( temp_pal, 0, 768 );

	while ( notfinished )
	{
		notfinished = 0;
                for ( index = 0; index < 768; ++index )
		{
                        if ( temp_pal[ index ] < pal[ index ] ) {
				notfinished = 1;
                                temp_pal[ index ] += 1;
			}
                        else if ( temp_pal[ index ] > pal[ index ] ) {
				notfinished = 1;
                                temp_pal[ index ] -= 1;
                        }
                }
                setVGApalette( temp_pal );
		delay(time);
	}
}

void Mosaic( image_type *image, int block_size, int ulx, int uly, char transparent, page_type where ){
        int x, y, byte=0;
        int ystop=(uly+image->height),  /* actual x width and y height on screen */
            xstop=(ulx+image->width);

        for ( y = uly; y < ystop; y+=block_size){
                for ( x = ulx; x < xstop; x+=block_size){
                        if( image->bitmap[byte] != transparent )
                                DrawFilledRectangle( x, y, block_size, image->bitmap[byte], where );
                        byte+=block_size;
		}
                byte=((y-uly)*(image->width))+((image->width)*block_size);
                //byte=y*xstop+(image->width)*(block_size-1);
       }
}

void DrawFilledRectangle( int ulx, int uly, int size, int color, page_type where ){
        int x, y, xstop=ulx+size, ystop=uly+size;

        for ( y = uly; y < ystop; ++y){
                for ( x = ulx; x < xstop; ++x){
                        if(x>-1 && y>-1 && x<320 && y<200)
                                where[( x + (y << 8) + (y << 6))] = color;
		}
       }
}
              
image_type *VBackgroundToImage( vbackground_type *vbackground ){
        image_type *temp;
        int i;

        temp->bitmap = (char *)malloc( 64000 );
        temp->width = 320;
        temp->height = 200;

        for(i=0; i<64000; ++i ){
                temp->bitmap[i] = vbackground->bitmap[0][i];
        }

        return( temp );
}

void Zoom( image_type *image, double ratio, int ulx, int uly, page_type where ){
        int x, y, byte=0;
        int ystop, xstop;

        double real_byte=0.0;   /* use double to keep track when add is decimal */
        double add;             /* number to increment bytes each iteration */
                                /* keeps the perspective realistic */
        double incr_y;          /* keep perspective in y plane */
        int cur_y=0;/*uly;*/          /* counter for current y pos */
        double real_cur_y=0;/*uly;*/  /* use double because incr_y may be decimal */

        add=(1.0/ratio);    /* adjust values to work with ratio */
        ystop = (ratio*image->height);
        xstop = (ratio*image->width);
        incr_y=(image->height)/((double)ystop);
        ystop+=uly;
        xstop+=ulx;

        for ( y = uly; y < ystop; ++y){
                for ( x = ulx; x < xstop; ++x){
                        if(x>-1 && y>-1 && x<320 && y<200)
                                where[( x + (y << 8) + (y << 6))] = image->bitmap[byte];
                        byte=real_byte+=add;
		}
                /* new y may not already have byte at next y position, */
                byte=real_byte=cur_y*image->width;  /* this fixes it */
                cur_y=real_cur_y+=incr_y;
       }
}



