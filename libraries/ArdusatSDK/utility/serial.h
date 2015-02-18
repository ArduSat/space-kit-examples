/**
 * @file   serial.h
 * @Author Ben Peters (ben@ardusat.com)
 * @date   January 7, 2015
 * @brief  Ardusat SDK Unified serial library to wrap software and hardware 
 *         serial libraries under one interface.
 */
#ifndef ARDUSAT_SERIAL_H_
#define ARDUSAT_SERIAL_H_

#include <HardwareSerial.h>
#include <inttypes.h>
#include <Stream.h>

#include "SoftwareSerial.h"

typedef enum {
  SERIAL_MODE_HARDWARE=0,
  SERIAL_MODE_SOFTWARE,
  SERIAL_MODE_HARDWARE_AND_SOFTWARE,
} serialMode;

class ArdusatSerial : public Stream
{
  private:
    SoftwareSerial *_soft_serial; 
    serialMode _mode;

  public:
    ArdusatSerial(serialMode mode);
    ArdusatSerial(serialMode mode, uint8_t softwareReceivePin, uint8_t softwareTransmitPin, 
                  bool softwareInverseLogic = false);
    ~ArdusatSerial();

    void begin(unsigned long speed, bool setXbeeSpeed=false);
    void end();

    virtual int peek();
    virtual int read();
    virtual int available();
    virtual void flush();

    virtual size_t write(uint8_t);
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }

    using Print::write;
    operator bool();
};

void set_xbee_baud_rate(Stream *serial_interface, unsigned long speed);
#endif
