#include "abeoserialport.h"
#include <termios.h>
#include "error.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

AbeoSerialPort::AbeoSerialPort()
{
    _is_opened=false;
}


void AbeoSerialPort::init(char* port){
    _sp=open(port,O_RDWR|O_NOCTTY|O_NDELAY);
    if(_sp==-1){
        _err("Failed to open serial port",ERRCODE_SP_OPEN);
    }
    else fcntl(_sp, F_SETFL, 0);
    //replace 0 with FNDELAY to make the read return immediately

    //some more settings
    struct termios options;
    tcgetattr(_sp,&options);

    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    options.c_cflag |= (CLOCAL|CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_lflag = 0;
    tcsetattr(_sp,TCSANOW, &options);

    flush();
    _is_opened=true;
}

void AbeoSerialPort::release(){ if(_is_opened) close(_sp);}



void AbeoSerialPort::flush(){
     tcflush(_sp,TCIFLUSH);
}

char AbeoSerialPort::read_char(){
    if(!_is_opened)return 0;
    char c;
    read(_sp,&c,1);
    return c;
}

void AbeoSerialPort::write_char(char c){
    if(_is_opened) write(_sp,&c,1);
}
void AbeoSerialPort::write_str(char* str){
    if(_is_opened){
        write(_sp,str,strlen(str));
    }
}
