#ifndef ABEOFBDIRECTDRAW_H
#define ABEOFBDIRECTDRAW_H

#include "type.h"


#define RGB16(r,g,b) (((r>>3)<<11) | ((g>>2)<<5) | (b>>3))
/* some RGB color definitions                                                 */
#define Black           0x0000      /*   0,   0,   0 */
#define Navy            0x000F      /*   0,   0, 128 */
#define DarkGreen       0x03E0      /*   0, 128,   0 */
#define DarkCyan        0x03EF      /*   0, 128, 128 */
#define Maroon          0x7800      /* 128,   0,   0 */
#define Purple          0x780F      /* 128,   0, 128 */
#define Olive           0x7BE0      /* 128, 128,   0 */
#define LightGrey       0xC618      /* 192, 192, 192 */
#define DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Blue            0x001F      /*   0,   0, 255 */
#define Green           0x07E0      /*   0, 255,   0 */
#define Cyan            0x07FF      /*   0, 255, 255 */
#define Red             0xF800      /* 255,   0,   0 */
#define Magenta         0xF81F      /* 255,   0, 255 */
#define Yellow          0xFFE0      /* 255, 255,   0 */
#define White           0xFFFF      /* 255, 255, 255 */
#define Orange          0xFD20      /* 255, 165,   0 */
#define GreenYellow     0xAFE5      /* 173, 255,  47 */
#define Pink            0xF81F


class AbeoFBDirectDraw
{

public:
    enum Font{
        FONT5x7=0,
        FONT6x12,
    };
    enum Mode{
        Direct=0,
        DoulbeBuffering,
    };

public:
    AbeoFBDirectDraw();

    // Init frame buffer, enter graphic mode
    void init(char* fbdev, char* ttydev);
    // Exit graphic mode
    void release();

    //set current color
    void set_color(ushort color);
    // Fill screen with current color
    void clear();
    // Draw a pixel
    void inline pixel(int x,int y);
    void inline pixel(int offset);
    // Draw vertical line
    void vline(int x, int y0, int y1);
    // Draw horizontal line
    void hline(int x0,int x1, int y);
    // Draw rectangle
    void rect(int x, int y, int w, int h);
    // Draw circle
    void circle(int xc, int yc, int radius);
    // Draw line
    void line(int x0,int y0, int x1, int y1);
    // Draw ellipse
    void ellipse(int x,int y, int w, int h);
    // Draw bezier curve
    void bezier(int x0, int y0, int x1, int y1, int x2, int y2);
    // Draw line with aa
    void lineaa(int x0, int y0, int x1, int y1);
    // Draw bezier curve aa
    void bezieraa(int x0, int y0, int x1, int y1, int x2, int y2);
    // Draw line with thickness
    void lineth(int x0, int y0, int x1, int y1, float thickness);

    // Draw text
    void set_font(int font);
    void putch(char c, int x, int y);
    void text(char* txt, int x, int y);
    void set_mode(int mode);
    void update();
    // Draw

    //properties
public:
    int width();
    int height();

private:

    // Frame buffer related
    int _w, _h;         //width and height of the frame buffer
    int _bpp;           //bit per pixel
    int _Bpp;            //byte per pixel
    int _scr_size8;       //screen size int 8bit
    int _scr_size16;     //screen size in 16bit
    int _scr_size32;        //screen size in 32bit
    ushort *_scr,*_fb, *_scr_buff;
    int _tty;
    int _fbfd;

    //Drawing context
    float _thickness;   //current thickness
    ushort _crr_color;  //curent color
    Font _font;
    Mode _mode;
};

#endif // ABEOFBDIRECTDRAW_H
