#include <QCoreApplication>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdbool.h>
#include <linux/input.h>
#include <linux/kd.h>
#include <time.h>

#include <signal.h>
#include "tslib.h"
#include "abeofbdirectdraw.h"
#include <errno.h>
#include <termios.h>
#include "abeoserialport.h"
#include "error.h"

#include "abeotouchscreen.h"
#include <vector>
#include <string.h>
using namespace std;

#define RANDX (rand()%SCR_W)
#define RANDY (rand()%SCR_H)
#define RANDCOLOR RGB16(rand()%255,rand()%255,rand()%255)



void parse_cmd(char* cmd){
//    int idx=0;
//    char c;

//    vector<string> tokens;
//    string* t = new string("");
//    while(1){
//        c=cmd[idx++];
//        if(c==0)break;
//        if(c=='(' || c==',' || c==')'){
//            tokens.push_back(*t);
//            t = new string("");
//        }
//        else *t+=c;
//    }


//    for(int i=0;i<tokens.size();i++)
//        printf("TOKEN[%d] = %s\n",i,tokens[i]);

}


void show_stripe(AbeoFBDirectDraw * afb, int gap, int time){
    afb->set_color(Black);
    afb->clear();
    afb->set_color(White);
    for(int i=0;i<afb->width();i+=gap){
        afb->line(i,0,i,afb->height());
    }
    afb->update();
    usleep(time);
}

int main(int argc, char *argv[])
{

    AbeoFBDirectDraw afb;
    AbeoSerialPort asp;
    AbeoTouchScreen ats;

    afb.init("/dev/fb0","/dev/tty1");
    ats.init("/dev/input/event1");


    afb.set_mode(1);


   // ats.calib(&afb);

    //while(1){}
    //DRAW STUFF
   // clrscr(fbp,White);
    int line_cnt=100;
    printf("Drawing %d lines randomly...\n",line_cnt);
    clock_t t;
    t=clock();


    for(int k=0;k<10;k++){
        for(int i=5;i<100;i+=5){
            show_stripe(&afb,i,50000);
        }
    }
//    afb.set_color(Black);
//    afb.clear();

//    //init serial
//    asp.init("/dev/ttyO1");

//    string a("a");

//    afb.set_color(White);
//    //getting command
//    char cmd[255];
//    char c;
//    int idx=0;
//    do{
//        c = asp.read_char();
//        asp.write_char(c);
//        if(c=='\n' || c=='\r'){
//            cmd[idx]=0;
//            break;
//        }else cmd[idx++]=c;
//    }while(1);

//    afb.text(cmd,100,100);

    //unmap memory and close framebuffer
    t = clock() - t;
    float elapse_sec=t/(float)CLOCKS_PER_SEC;
    printf ("Elapsed time=%f second, performance = %f lines per second\n",elapse_sec,line_cnt/elapse_sec);

    //getchar();
    //afb.release();
    printf("DONE!\n");
    return 0;
}

