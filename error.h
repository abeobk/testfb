#ifndef ERROR
#define ERROR

#define _err(msg,code)  printf(msg);printf(", ERROR_CODE=0x%4X\n",code);
#define ERRCODE_FB_OPEN     0x0001  //cant open fb
#define ERRCODE_FB_FIX_INFO 0x0002  //cant read fb fix info
#define ERRCODE_FB_VAR_INFO 0x0003  //cant read fb var info
#define ERRCODE_FB_MEM_MAP  0x0004  //cant map fb memory
#define ERRCODE_FB_GRAPHICS 0x0005  //failed to change fb to graphic mode
#define ERRCODE_FB_TEXT     0x0006  //failed to change fb to text mode

#define ERRCODE_SP_OPEN     0x0011  //failed to open serial port

#define ERRCODE_TS_OPEN     0x0021  //failed to open touch screen device
#endif // ERROR

