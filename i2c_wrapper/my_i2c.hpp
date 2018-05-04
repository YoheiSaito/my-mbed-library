#pragma once
#include "mbed.h"

class MyI2C
{
public:
    MyI2C(I2C *bus);
    MyI2C(I2C *bus,  char address);
    void set_address( char address);
    void write( char subaddr,  char data);
    void write( char subaddr,  char* cmd, int bytes);
    char read( char subaddr);
    void read( char subaddr, char *buf, int bytes);
private:
     char addr;
    I2C *i2c;
};

MyI2C::MyI2C(I2C *bus){
    i2c = bus;
}
MyI2C::MyI2C(I2C *bus,  char address){
    i2c = bus;
    addr = address << 1;
}
void MyI2C::set_address( char address){
    addr = address << 1;
}
void MyI2C::write( char subaddr,  char data){
    i2c->write(addr, &subaddr, 1, true);
    i2c->write(data);
    i2c->stop();
}
void MyI2C::write( char subaddr,  char* cmd, int bytes){
    i2c->write(addr, &subaddr, 1, true);
    i2c->write(addr, cmd, bytes);


}
 char MyI2C::read( char subaddr){
    char *ret;
    i2c->write( addr,&subaddr, 1, true);
    i2c->read(addr | 1, ret, 1);
    return *ret;
}
void MyI2C::read( char subaddr,  char *buf, int bytes){
    i2c->write(addr,&subaddr, 1, true);
    i2c->read(addr | 1, buf, bytes);
}