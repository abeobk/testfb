#ifndef ABEOSERIALPORT_H
#define ABEOSERIALPORT_H
#include <stdint.h>
#include <stdbool.h>

class AbeoSerialPort
{
public:
    AbeoSerialPort();
    void init(char* port);
    void release();
    char read_char();
    void write_char(char c);
    void write_str(char* str);

    void flush();
private:
    int _sp;
    bool _is_opened;
};

#endif // ABEOSERIALPORT_H
