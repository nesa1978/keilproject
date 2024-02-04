#include <STC15F2K60S2.H>

void Write_Ds1302(unsigned char temp);
void Write_Ds1302_Byte(unsigned char address, unsigned char dat);
unsigned char Read_Ds1302_Byte(unsigned char address);