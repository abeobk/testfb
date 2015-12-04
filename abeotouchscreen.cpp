#include "abeotouchscreen.h"
#include "error.h"
#include <linux/input.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

AbeoTouchScreen::AbeoTouchScreen()
{
    _is_opened=false;
}

void AbeoTouchScreen::init(char* tsdev){
    _ts=open(tsdev,O_RDONLY);
    if(_ts==-1){
        _err("Failed to open touch screen device",ERRCODE_TS_OPEN);
    }
    _is_opened=true;
}

void AbeoTouchScreen::release(){
    if(_is_opened)close(_ts);
}

void AbeoTouchScreen::draw_calib_point(AbeoFBDirectDraw *afb, int x, int y){
    afb->set_color(Green);
    afb->line(x-10,y,x+10,y);
    afb->line(x,y-10,x,y+10);
    afb->set_color(Pink);
    afb->line(x-5,y-2,x-5,y-5);
    afb->line(x-5,y+2,x-5,y+5);
    afb->line(x+5,y-2,x+5,y-5);
    afb->line(x+5,y+2,x+5,y+5);

    afb->line(x-5,y-5,x-2,y-5);
    afb->line(x-5,y+5,x-2,y+5);
    afb->line(x+5,y-5,x+2,y-5);
    afb->line(x+5,y+5,x+2,y+5);
}

void AbeoTouchScreen::calib(AbeoFBDirectDraw* afb){
    afb->set_color(Black);
    afb->clear();
    afb->set_color(White);
    afb->text("--AbeoSystem Touchscreen calibration--",270,160);
    afb->text("     Touch the crosshair to calib",270,176);
    //point 1
    draw_calib_point(afb,CORNER_OFFSET,CORNER_OFFSET);
    draw_calib_point(afb,afb->width()-CORNER_OFFSET,CORNER_OFFSET);
    draw_calib_point(afb,afb->width()-CORNER_OFFSET,afb->height()-CORNER_OFFSET);
    draw_calib_point(afb,CORNER_OFFSET,afb->height()-CORNER_OFFSET);
    draw_calib_point(afb,afb->width()/2,afb->height()/2);


}
