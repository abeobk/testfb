#include "abeofbdirectdraw.h"
#include <linux/fb.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include "error.h"
#include <sys/mman.h>
#include <string.h>
#include <linux/kd.h>
#include <memory.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <unistd.h>
#include "rasterfont.h"

AbeoFBDirectDraw::AbeoFBDirectDraw()
{
    _font = FONT6x12;
    _mode = Direct;
    _fb=_scr=_scr_buff=0;
}


// Initialize frame buffer
void AbeoFBDirectDraw::init(char* fbdev, char* ttydev){
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    //open frame buffer device
    _fbfd = open(fbdev, O_RDWR);
    if(_fbfd==-1){
        _err("Failed to enter graphic mode",ERRCODE_FB_OPEN);
    }
    // Get fixed screen information
    if (ioctl(_fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        _err("Error reading fixed information", ERRCODE_FB_FIX_INFO);
    }

    // Get variable screen information
    if (ioctl(_fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        _err("Error reading variable information",ERRCODE_FB_VAR_INFO);
    }
    //print info
    _bpp = vinfo.bits_per_pixel;
    _Bpp = _bpp/8;
    _w = vinfo.xres;
    _h = vinfo.yres;
    _scr_size8=_w*_h*_Bpp;
    _scr_size16=_scr_size8/2;
    _scr_size32=_scr_size16/2;
    printf("FBINFO: %dx%d@%dbpp\n",_w,_h,_bpp);
    printf("SCREENSIZE: %d bytes\n",_scr_size8);
    _fb= (ushort*)mmap(0,_scr_size8,PROT_READ|PROT_WRITE, MAP_SHARED, _fbfd,0);
    if((int)_scr==-1){
        _err("Cant map framebuffer memory", ERRCODE_FB_MEM_MAP);
    }

    //enter graphic mode
    _tty = open(ttydev, O_RDWR);
    if(ioctl(_tty,KDSETMODE,KD_GRAPHICS)==-1){
        _err("Cant change fb to graphics mode", ERRCODE_FB_GRAPHICS);
    }

    _scr_buff=(ushort*)malloc(_scr_size8);

    switch(_mode){
    case Direct: _scr=_fb;break;
    case DoulbeBuffering: _scr=_scr_buff;break;
    }

    printf("Successfully initialized framebuffer\n");

}

int AbeoFBDirectDraw::width(){return _w;}
int AbeoFBDirectDraw::height(){return _h;}
//change fb back to text mode
void AbeoFBDirectDraw::release(){
    if(_fb)
    munmap(_fb,_scr_size8);
    if(_scr_buff)
    free(_scr_buff);
    if(ioctl(_tty,KDSETMODE,KD_TEXT)==-1){
        _err("Cant change fb to text mode", ERRCODE_FB_TEXT);
    }

    close(_fbfd);
    close(_tty);
    printf("FB is now in text mode\n");
}

//set current color
void AbeoFBDirectDraw::set_color(ushort color){
    _crr_color = color;
}

// Fill screen with current color
void AbeoFBDirectDraw::clear(){
    unsigned int color_int_packed = (_crr_color<<16)|_crr_color;
    unsigned int* scr_ptr=(unsigned int*)_scr;
    int scr_size32=_scr_size32;
    while(scr_size32--)*scr_ptr++=color_int_packed;
}
// Draw a pixel
void AbeoFBDirectDraw::pixel(int x,int y){
    if(x<0||x>=_w || y<0||y>=_h)return;
    _scr[y*_w+x]=_crr_color;
}
void AbeoFBDirectDraw::pixel(int offset){
    if(offset<0||offset>_scr_size16)return;
    _scr[offset]=_crr_color;
}

// Draw vertical line
void AbeoFBDirectDraw::vline(int x, int y0, int y1){
    //swap if needed
    if(y0>y1){
        int tmp=y0;
        y0=y1;
        y1=tmp;
    }
    int offset=y0*_w+x;
    while(y0++<y1){
        pixel(offset);
        offset+=_w;
    }
}

// Draw horizontal line
void AbeoFBDirectDraw::hline(int x0,int x1, int y){
    if(x0>x1){
        int tmp=x0;
        x0=x1;
        x1=tmp;
    }
    int offset=y*_w+x0;
    while(x0++<x1){
        pixel(offset++);
    }
}

// Draw rectangle
void AbeoFBDirectDraw::rect(int x, int y, int w, int h){
    if(x==0||y==0)return;
    vline(x,y,y+h);
    vline(x+w-1,y,y+h);
    hline(x,x+w,y);
    hline(x,x+w,y+h-1);
}

// Draw circle
void AbeoFBDirectDraw::circle(int xc, int yc, int radius){
    int f = 1 - radius;
    int ddF_x = 1;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    int offset=yc*_w+xc;
    int roffset=radius*_w;
    //DRAW NESW pnts
    pixel(offset+roffset);
    pixel(offset-roffset);
    pixel(offset+radius);
    pixel(offset-radius);
    while(x < y)
    {
        if(f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        int yoffset=y*_w;
        int xoffset=x*_w;
        pixel(offset+x+yoffset);
        pixel(offset-x+yoffset);
        pixel(offset+x-yoffset);
        pixel(offset-x-yoffset);
        pixel(offset+y+xoffset);
        pixel(offset-y+xoffset);
        pixel(offset+y-xoffset);
        pixel(offset-y-xoffset);
    }
}
// Draw line
void AbeoFBDirectDraw::line(int x0,int y0, int x1, int y1){
    int dx=abs(x1-x0);
    int dy=abs(y1-y0);
    int sx=(x0<x1)?1:-1;
    int sy=(y0<y1)?1:-1;
    int err=dx-dy;
    int offset=y0*_w+x0;
    while(true){
       pixel(offset);
       if(x0==x1 && y0==y1)return;
       int e2=err<<1;
       if(e2>-dy){
           err-=dy;
           x0+=sx;
           offset+=sx;
       }
       if(x0==x1 && y0==y1){
           pixel(offset);
           return;
       }
       if(e2<dx){
           err+=dx;
           y0+=sy;
           offset+=sy*_w;
       }
    }
}
// Draw ellipse
void AbeoFBDirectDraw::ellipse(int x,int y, int w, int h){
   int a = w-=1, b = h-=1, b1 = b&1; /* values of diameter */
   long dx = 4*(1-a)*b*b, dy = 4*(b1+1)*a*a; /* error increment */
   long err = dx+dy+b1*a*a, e2; /* error of 1.step */
   int x0=x, x1=x+w;
   int y0=y;
   y0 += (b+1)/2;
   int y1 = y0-b1;   /* starting pixel */
   a *= 8*a; b1 = 8*b*b;
   int offset0=y0*_w;
   int offset1=y1*_w;
   do {
       pixel(offset0+x1);
       pixel(offset0+x0);
       pixel(offset1+x0);
       pixel(offset1+x1);
       e2 = 2*err;
       if (e2 <= dy) { offset0+=_w; y0++; y1--; offset1-=_w; err += dy += a; }  /* y step */
       if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; } /* x step */
   } while (x0 <= x1);

   while (y0-y1 <= b) {  /* too early stop of flat ellipses a=1 */
       pixel(offset0 + x0-1);
       pixel(offset0+x1+1);
       pixel(offset1+x0-1);
       pixel(offset1+x1+1);
       y0++;
       y1--;
       offset0+=_w;
       offset1-=_w;
   }
}
// Draw bezier curve
void AbeoFBDirectDraw::bezier(int x0, int y0, int x1, int y1, int x2, int y2){
//     ushort color=_crr_color;
//     set_color(White);
//     line(x0,y0,x1,y1);
//     line(x1,y1,x2,y2);
//     set_color(color);
     int sx = x2-x1, sy = y2-y1;
     long xx = x0-x1, yy = y0-y1, xy;         /* relative values for checks */
     double dx, dy, err, cur = xx*sy-yy*sx;                    /* curvature */
     //assert(xx*sx <= 0 && yy*sy <= 0);  /* sign of gradient must not change */
     if (sx*(long)sx+sy*(long)sy > xx*xx+yy*yy) { /* begin with longer part */
       x2 = x0; x0 = sx+x1; y2 = y0; y0 = sy+y1; cur = -cur;  /* swap P0 P2 */
     }
     if (cur != 0) {                                    /* no straight line */
       xx += sx; xx *= sx = x0 < x2 ? 1 : -1;           /* x step direction */
       yy += sy; yy *= sy = y0 < y2 ? 1 : -1;           /* y step direction */
       xy = 2*xx*yy; xx *= xx; yy *= yy;          /* differences 2nd degree */
       if (cur*sx*sy < 0) {                           /* negated curvature? */
         xx = -xx; yy = -yy; xy = -xy; cur = -cur;
       }
       dx = 4.0*sy*cur*(x1-x0)+xx-xy;             /* differences 1st degree */
       dy = 4.0*sx*cur*(y0-y1)+yy-xy;
       xx += xx; yy += yy; err = dx+dy+xy;                /* error 1st step */
       do {
         pixel(x0,y0);                                     /* plot curve */
         if (x0 == x2 && y0 == y2) return;  /* last pixel -> curve finished */
         y1 = 2*err < dx;                  /* save value for test of y step */
         if (2*err > dy) { x0 += sx; dx -= xy; err += dy += yy; } /* x step */
         if (    y1    ) { y0 += sy; dy -= xy; err += dx += xx; } /* y step */
       } while (dy < dx );           /* gradient negates -> algorithm fails */
     }
     line(x0,y0, x2,y2);                  /* plot remaining part to end */
}

// Draw line with aa
void AbeoFBDirectDraw::lineaa(int x0, int y0, int x1, int y1){
//    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
//   int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
//   int err = dx-dy, e2, x2;                       /* error value e_xy */
//   int ed = dx+dy == 0 ? 1 : sqrt((float)dx*dx+(float)dy*dy);

//   for ( ; ; ){                                         /* pixel loop */
//      setPixelAA(x0,y0, 255*abs(err-dx+dy)/ed);
//      e2 = err; x2 = x0;
//      if (2*e2 >= -dx) {                                    /* x step */
//         if (x0 == x1) break;
//         if (e2+dy < ed) setPixelAA(x0,y0+sy, 255*(e2+dy)/ed);
//         err -= dy; x0 += sx;
//      }
//      if (2*e2 <= dy) {                                     /* y step */
//         if (y0 == y1) break;
//         if (dx-e2 < ed) setPixelAA(x2+sx,y0, 255*(dx-e2)/ed);
//         err += dx; y0 += sy;
//    }
//  }
}
// Draw bezier curve aa
void AbeoFBDirectDraw::bezieraa(int x0, int y0, int x1, int y1, int x2, int y2){}
// Draw line with thickness
void AbeoFBDirectDraw::lineth(int x0, int y0, int x1, int y1, float thickness){}
// Draw text
void AbeoFBDirectDraw::set_font(int font){
    _font = (Font)font;
}
void AbeoFBDirectDraw::putch(char ch, int x, int y){
    unsigned char* chptr;
    int offset=x+y*_w;
    switch (_font)
    {
    case FONT5x7:
        if(y<=-7||y>=_h)return;
        chptr = Font5x7 + (ch - ' ') * 6;
        for (int i = 0; i < 6; i++,x++,offset++){
            if (x >= _w)return;
            if (x < 0)continue;
            unsigned char c = chptr[i];

#define     _pixel5x7(k)   if(c&(1<<k))pixel(offset+k*_w);

            _pixel5x7(0);
            _pixel5x7(1);
            _pixel5x7(2);
            _pixel5x7(3);
            _pixel5x7(4);
            _pixel5x7(5);
            _pixel5x7(6);
            _pixel5x7(7);
        }
        break;

    case FONT6x12:
        if(x<=-6||x>=_w)return;
        chptr = Font6x12 + (ch - ' ') * 12;
        for (int i = 0; i < 12; i++, offset+=_w){
            if (y >= _h)return;
            if (y < 0)continue;
            unsigned char c = chptr[i];
#define     _pixel6x12(k)   if(c&(1<<(7-k)))pixel(offset+k);
            _pixel6x12(0);
            _pixel6x12(1);
            _pixel6x12(2);
            _pixel6x12(3);
            _pixel6x12(4);
            _pixel6x12(5);
            _pixel6x12(6);
        }
        break;
    }
}

void AbeoFBDirectDraw::text(char* txt, int x, int y){
    int chid = 0;
    int chw = 6;
    int chh = 8;
    switch (_font)
    {
        case FONT5x7: chw = 6; chh = 8; break;
        case FONT6x12:chw = 7; chh = 12; break;
    }
    char ch;
    int orgx=x;
    while (ch=txt[chid++]){
        if (ch == '\n'){
            x=orgx;
            y += chh;
            if (y >= _h)return;
            continue;
        }

        if (x>=_w)continue;
        putch(ch, x, y);
        x += chw;
    }
}

void AbeoFBDirectDraw::set_mode(int mode){
    _mode=(Mode)mode;
    switch(_mode){
    case Direct: _scr=_fb;break;
    case DoulbeBuffering: _scr=_scr_buff;break;
    }
}

void AbeoFBDirectDraw::update(){
    switch(_mode){
    case Direct: break;
    case DoulbeBuffering:
        //  memcpy(_fb,_scr_buff,_scr_size8);
        int sz = _scr_size32;
        uint* _fb_ptr=(uint*)_fb;
        uint* _buff_ptr=(uint*)_scr_buff;
        while(sz--){
            *_fb_ptr++=*_buff_ptr++;
        }
        break;
    }

}

// Draw
