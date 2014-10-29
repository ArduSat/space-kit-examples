#ifndef		TMP102_h
#define		TMP102_h
#include	<arduino.h>
#include	<Wire.h>

#define TEMP_ABSOLUTE_ZERO -273.15

class Tmp102
{
public:
    Tmp102(TwoWire* wire, byte slv_addr);
    double 	read();
    
private:
	TwoWire*   wire_;
    byte	slv_addr_;
};

#endif
