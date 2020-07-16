/**
# SD_MHZ19B Library for Winsen MH-Z19B NDIR CO2 Module

by **ShaggyDog18@gmail.com**, JULY 2020

github: https://github.com/ShaggyDog18/SD_MHZ19B

Arduino library for **MH-Z19B NDIR CO2 Module** by [Zhengzhou Winsen Electronics Technology Co., Ltd](www.winsen-sensor.com)

License: [GNU GPLv3](https://choosealicense.com/licenses/gpl-3.0/)

MH-Z19B NDIR infrared gas module is a common type, small size sensor, using non-dispersive infrared (NDIR) principle to detect the existence of CO 2 in the air, with good selectivity, non-oxygen dependent and long life. Built-in temperature compensation; and it has UART output and PWM output. 

This library features access to all module features though an abstracted class and methods to manage the module implemented in user-level functionality.

The library does not work with PWM module signal. Communicates with the module by Hardware or Software UART.

CO2 Measurement Range 0~2000ppm or 0~5000ppm. Accuracy: ±(50ppm+3%).

## Library Methods

- `SD_MHZ19B( Stream& serial )` - Class Constructor
- `~SD_MHZ19B()` - Class Distructor
- `uint16_t getPPM(void)` - read data from the module; returns value of CO2 concentration in ppm; Data are verified and valid (validate by calculating checkSum of the data received). Retunt `FALSE` i.e. ZERO value if communication or CRC error.
- `int8_t getTemp(void)` - returns module temperature in degrees Celcium. Should be called after getPPM() function; otherwise will return a previous value. Rather inaccurate ±2*C; is used for internal compensation.
- `void setAutoCalibration( bool _autoCalib )` - toggles Auto Calibration (Automatic Baseline Correction - ABC) ON/OFF. Set `true` for Enabled/ON; `false` for Disabled/OFF (default). Refer to Auto Calibration Notes below.
- `bool getABCstatus(void)` - gets Auto Calibration ABC status (undocumented feature). Returns `true` for Enabled/ON, `false` - Disabled/OFF.
- `void calibrateZeroPoint(void)` - calibrate Zero point. During the zero point calibration, the sensor must work in stable gas environment (400ppm) for over 20 minutes.
- `bool calibrateSpanPoint( uint16_t _spanPoint )`- do `ZeroPoint` calibration before `Span` calibration. Make sure the sensor worked under a certain level co2 for over 20 minutes. Suggest using 2000ppm as span, at least 1000ppm. Default value is 2000ppm. Returns true if the requested Span value is OK.
- `bool setDetectionRange( uint16_t _detectionRange )` - sets detection range, default is 0~2000ppm. The range could be 2,000-5,000ppm. Returns `true` if the requested range value is OK.
- `uint8_t getStatus(void)` - Reads module status (undocumented feature), Returns 0 if OK; The value is available after successful getPPM() reading only; otherwise, will return previous value; to be called after getPPM(). 

For more details on the library use refer to the example that utilizes major library methods.

## Auto Calibration Notes

if Auto Calibration (Automatic Baseline Correction - ABC) is ON, the sensor itself performs a zero point judgment and automatic calibration procedure intelligently after a continuous operation period. The automatic calibration cycle is every 24 hours after powered on. The zero point for automatic calibration is **400ppm**. 

This function is usually suitable for indoor air quality monitor such as offices and homes, not suitable for greenhouse, farm and refrigeratory where this function should be off. Please do zero calibration timely, such as manual or command calibration.

## Compatibility

The library developed for Arduino UNO, NANO, Pro Mini, ESP8266, etc.

**If you like and use this library please consider making a small donation using [PayPal](https://paypal.me/shaggyDog18/3USD)**
 */

#include "Arduino.h"
#include <SD_MHZ19B.h>

//#define DEBUG  // for debug only

extern "C" {
#include <stdlib.h>
#include <stdio.h>
}

#define RANGE_TOP_LIMIT 5000  // in ppm, pre-defined for the MH-Z19B, refer to datasheet

/**
 * @brief Constructor for SD_MHZ19B class
 * @param  a Stream ({Software/Hardware}Serial) object.
 * @note The serial stream should be already initialized
 * @return  void
 */
SD_MHZ19B::SD_MHZ19B( Stream& serial ): _serial(serial) {
  _serial.setTimeout(100);
  setAutoCalibration( false );  // default settings - off autocalibration
  setDetectionRange();  // default range of 2000 ppm
}


// Class Destructor
SD_MHZ19B::~SD_MHZ19B() {
  //_serial = nullptr;
}


uint16_t SD_MHZ19B::getPPM(void) {
  // request data: send a request command
  uint8_t cmd[4] = {0x86, 0x00, 0x00, 0x79};  // last byte is a checkSum
  _sendCmd( cmd );

  if( _readData() ) return _unionFrame.MHZ19B.co2concPPM;
  return false;  // return 0
}


// UNDOCUMENTED 125  // Get ABC status: true - enabled, false - disabled  
bool SD_MHZ19B::getABCstatus(void) {   // last byte is a pre-calculated checkSum
  uint8_t cmd[4] = {0x7D, 0x00, 0x00, 0x82};
  //cmd[3] = _calcCmdCheckSum( cmd );
  //Serial.print( "getABCstatus check sum" ); Serial.println( cmd[3], HEX);
  _sendCmd( cmd );

  if( _readData() ) return (bool) _unionFrame.MHZ19B.ABCstatus;

  #ifdef DEBUG   
    Serial.println( "Error getting ABC Status" );
  #endif
  return false;
}


// UNDOCUMENTED 155  // Get detection range
uint16_t SD_MHZ19B::getDetectionRange(void) {   // last byte is a pre-calculated checkSum
/*
  uint8_t cmd[4] = {0x9B, 0x00, 0x00, 0x64};
  //cmd[3] = _calcCmdCheckSum( cmd );
  //Serial.print( "getDetectionRange checkSum" ); Serial.println( cmd[3], HEX);
  _sendCmd( cmd );

  if( _readData() ) return _unionFrame.buffer[3];
*/  
  return false;
}


bool SD_MHZ19B::_readData(void) {
  delay(20);
  // expect data header to fly in
  while( (_serial.peek() != 0xFF) && _serial.available() ) {
    _serial.read();  // read untill we get the data header 0xff
  }

  #ifdef DEBUG
    Serial.print("available to read: "); Serial.println( _serial.available() );
    //Serial.print(("Sizeof frame struct:"); Serial.println( sizeof(MHZ19frameStruct_t) ); // debug only
  #endif

  if( _serial.available() < SIZEOF_FRAME ) {  //overall/at least 9 bytes should be available
    #ifdef DEBUG
	   Serial.println( "Err:Insufficiend buffer length" );
    #endif
    return false;
  }

  // read the entire buffer
  _serial.readBytes( _unionFrame.buffer, SIZEOF_FRAME );

  #ifdef DEBUG
    Serial.print("MH-Z19 Header:"); Serial.print( _unionFrame.MHZ19B.frameHeader[0], HEX );
    Serial.print( ":" ); Serial.println( _unionFrame.MHZ19B.frameHeader[1], HEX ); 
  #endif

  // re-sort the buffer: swap high and low bytes since they are not in the "machine" order 
  uint8_t tmp;
  tmp = _unionFrame.buffer[2];
  _unionFrame.buffer[2] = _unionFrame.buffer[3];
  _unionFrame.buffer[3] = tmp;

  // Debug - print received buffer
  #ifdef DEBUG  
    Serial.println("Read from module:");
    for( uint8_t i=0; i < SIZEOF_FRAME; i++ ) {
      Serial.print(":"); Serial.print( _unionFrame.buffer[i], HEX );
    }
    Serial.println();
  #endif

  uint8_t calcCheckSum = 0; // refer to datasheet for check sum calculation
  for( uint8_t i=1; i < SIZEOF_FRAME-1; i++ ) calcCheckSum += _unionFrame.buffer[i];
  calcCheckSum = (~calcCheckSum) + 1; 

  if( calcCheckSum != _unionFrame.MHZ19B.checkSum ) {
    #ifdef DEBUG
      Serial.println(  "Check Sum Error" );
    #endif
    return false;
  }
  return true;
}


// returns temperature in degrees Celcium. Should be called after getPPM() function; otherwise, will return a previous value.
int8_t SD_MHZ19B::getTemp(void) {
  return ((int8_t)_unionFrame.MHZ19B.temperature - 40);   // corrected temperature
}


// undocumented features
// returns status of the module. Should be called after getPPM() function; otherwise, will return a previous value.
uint8_t SD_MHZ19B::getStatus(void) {
  return _unionFrame.MHZ19B.status;
}


/*
// returns `Warming` status of the module (not documented, does not work). 
bool SD_MHZ19::isWarming(void) {
  return( getStatus() <= 1 );
}
*/


void SD_MHZ19B::setAutoCalibration( bool _autoCalib ) {  // true for ON; false for OFF
  uint8_t cmd[4] = {0x79, 0x00, 0x00, 0x86};  // checkSum is knows for the off command, no need to calculate

  if( _autoCalib ) {
    cmd[1] = 0xA0; 
    //cmd[3] = _calcCmdCheckSum( cmd );
	cmd[3] = 0xE6;  // checksum is known, no need to calculate
  }
  _sendCmd( cmd );
}


void SD_MHZ19B::calibrateZeroPoint(void) {   // last byte is a pre-calculated checkSum
  uint8_t cmd[4] = {0x87, 0x00, 0x00, 0x78};
  _sendCmd( cmd );
}


bool SD_MHZ19B::calibrateSpanPoint( uint16_t _spanPoint ) {

  if( _spanPoint < 1000 || _spanPoint > RANGE_TOP_LIMIT ) return false;  // Suggest using 2000ppm as span, at least 1000ppm

  uint8_t cmd[4];
  cmd[0] = 0x88;
  cmd[1] = _spanPoint / 256;
  cmd[2] = _spanPoint % 256;
  cmd[3] = _calcCmdCheckSum( cmd ); 
  _sendCmd( cmd );
  return true;
}


bool SD_MHZ19B::setDetectionRange( uint16_t _detectionRange ) {

  if( _detectionRange < 2000 || _detectionRange > RANGE_TOP_LIMIT ) return false;  // Suggest using 2000ppm as default

  uint8_t cmd[4];
  cmd[0] = 0x99;
  cmd[1] = _detectionRange / 256;
  cmd[2] = _detectionRange % 256;
  cmd[3] = _calcCmdCheckSum( cmd ); 
  _sendCmd( cmd );
  return true;
}

//----------------------------
// internal protected methods
//----------------------------
uint8_t SD_MHZ19B::_calcCmdCheckSum( const uint8_t _cmd[] ) {
  uint8_t checkSum = 0x01;  // byte #1 is always 0x01 in the command

  for( uint8_t i=0; i<3; i++ ) checkSum += _cmd[i];
  checkSum = (~checkSum)+1; 
  #ifdef DEBUG
    Serial.print( "CMD checkSum:" ); Serial.println( checkSum, HEX );
  #endif
  return checkSum;
}


void SD_MHZ19B::_sendCmd( const uint8_t _cmd[] ) {  // 4 bytes command
  _serial.flush(); // clear buffer

  // send command header
  _serial.write((uint8_t)0xFF); 
  _serial.write((uint8_t)0x01);

  // send command
  for( uint8_t i=0; i<3; i++ ) _serial.write( _cmd[i] );

  // send reserved 3 x zeros
  for( uint8_t i=0; i<3; i++ ) _serial.write( (uint8_t)0x00 );
  
  //send command tail/check value
  _serial.write( _cmd[3] );
  _serial.flush(); // clear buffer
}
