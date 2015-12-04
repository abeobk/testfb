#ifndef ABEOTOUCHSCREEN_H
#define ABEOTOUCHSCREEN_H

#include "abeofbdirectdraw.h"

#define CORNER_OFFSET 40

class AbeoTouchScreen
{
public:
    AbeoTouchScreen();
    void init(char* tsdev);
    void release();
    void calib(AbeoFBDirectDraw* afb);

private:
    void draw_calib_point(AbeoFBDirectDraw* afb, int x, int y);
private:
    int _ts;
    bool _is_opened;
};

#endif // ABEOTOUCHSCREEN_H
