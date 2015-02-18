#include	"tmp102.h"

Tmp102::Tmp102(TwoWire* wire, byte slv_addr)
:  wire_(wire), slv_addr_(slv_addr)
{
	wire_->begin();
}

double Tmp102::read()
{
	// Check accessibility
  	wire_->beginTransmission(slv_addr_); 
  	wire_->write((uint8_t) 0x00);
  	byte error = wire_->endTransmission();
  	if (error != 0){
  		return TEMP_ABSOLUTE_ZERO;
  	}

  	// Read bytes of temperature data
	wire_->requestFrom(slv_addr_, (uint8_t) 2);
	wire_->endTransmission();
	byte ret[2]; 
	ret[0] = Wire.read(); 
	ret[1] = Wire.read();

	// Convert raw bytes to temperature value
	double tmperature = ((ret[0] << 4) | (ret[1] >> 4)) * 0.0625;  

	return tmperature;
}
