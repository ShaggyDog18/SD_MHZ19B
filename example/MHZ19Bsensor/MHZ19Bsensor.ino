#include <SD_MHZ19B.h>
#include <SoftwareSerial.h>
 
//Create an instance of Software Serial before declaring the Module Class

// Arduino UNO, Nano, Pro Mini, etc 
//SoftwareSerial MHZ19B_Serial(4, 5); // RX, TX

// ESP8266
SoftwareSerial MHZ19B_Serial(D1, D2); // RX, TX

// Initiate the MH-Z19B Module
SD_MHZ19B MHZ19B( MHZ19B_Serial );  // initiate the module, also sets Auto calibration off and sets the range of 2000

// --------------

void setup() {
  Serial.begin(9600);
  delay(1000);    
  Serial.println();

  Serial.println("-- Initializing MH-Z19B Module...");

  if( MHZ19B_Serial ) 
    MHZ19B_Serial.begin(9600);  // Software Serial
  else { 
    Serial.println("Error setting up Software Serial for MH-Z19B Module");
    Serial.println("Cannot move forward. Please, push `Reset`");
    while(true);
  }
    
  delay(100);

  MHZ19B.setAutoCalibration( false );  // default settings - off autocalibration
  //MHZ19B.setDetectionRange(2000);  // default range of 2000 ppm
  MHZ19B.setDetectionRange();  // SAME AS ABOVE LINE: default range of 2000 ppm

  Serial.println("-- Reading MH-Z19B --");
  delay(200);
  Serial.print( "Auto Calibration (ABC) Status:" ); Serial.println( MHZ19B.getABCstatus()? "ON" : "OFF" );
  Serial.print( "Detection Range: " ); Serial.println( 2000 );
}

//--------------------

void loop() {
  uint16_t co2PPM;
  
  co2PPM = MHZ19B.getPPM();
  
  if( MHZ19B.getStatus() == 0 ){
    Serial.println("MH-Z19B Status is OK");
  }
  
  if( co2PPM > 0 ) { 
    Serial.print("co2(ppm): "); Serial.print( co2PPM );
    Serial.print(" : Temp(*C): "); Serial.println( MHZ19B.getTemperature() );
  } else {
    Serial.println( "Error Reading MH-Z19B Module" );
  }

  delay(5000);

  /* //Test ABC Status ON/OFF

  MHZ19B.setAutoCalibration( false ); 
  delay(100);
  Serial.print( "ABC Status (should be off):" ); Serial.println( MHZ19B.getABCstatus()? "ON" : "OFF" );
  delay(1000);
  Serial.print("co2: "); Serial.println( MHZ19B.getPPM() );
  delay(1000);
    
  MHZ19B.setAutoCalibration( true ); 
  delay(100);
  Serial.print( "ABC Status (should be on):" ); Serial.println( MHZ19B.getABCstatus()? "ON" : "OFF" );
  delay(1000);
  Serial.print("co2: "); Serial.println( MHZ19B.getPPM() );
  delay(1000);

  MHZ19B.setAutoCalibration( false ); 
  delay(100);
  Serial.print( "ABC Status (should be off again):" ); Serial.println( MHZ19B.getABCstatus()? "ON" : "OFF" );
  delay(1000);
  Serial.print("co2: "); Serial.println( MHZ19B.getPPM() );
  delay(1000);

  */
 
  /*
  //--- Test set / get(undocumented feature) detection range - under development getRange - does not work
  Serial.print("SET detection range: 2000"); 
  MHZ19B.setDetectionRange(2000); 
  delay(200);
  co2PPM = MHZ19B.getPPM();
  delay(200);
  Serial.print("get detection range: "); Serial.println( MHZ19B.getDetectionRange(), HEX );
  //---
  Serial.print("SET detection range: 3000"); 
  MHZ19B.setDetectionRange(3000); 
  delay(200);
  co2PPM = MHZ19B.getPPM();
  delay(200);
  Serial.print("get detection range: "); Serial.println( MHZ19B.getDetectionRange(), HEX );
  delay(200);
  //----
  Serial.print("SET detection range: 4000"); 
  MHZ19B.setDetectionRange(4000); 
  delay(200);
  co2PPM = MHZ19B.getPPM();
  delay(200);
  Serial.print("get detection range: "); Serial.println( MHZ19B.getDetectionRange(), HEX );
  delay(200);
  //----
  Serial.print("SET detection range: 5000"); 
  MHZ19B.setDetectionRange(5000); 
  delay(200);
  co2PPM = MHZ19B.getPPM();
  delay(200);
  Serial.print("get detection range: "); Serial.println( MHZ19B.getDetectionRange(), HEX );
  delay(200);
  //----
  Serial.print("SET detection range: 10000"); 
  MHZ19B.setDetectionRange(10000); 
  delay(200);
  co2PPM = MHZ19B.getPPM();
  delay(200);
  Serial.print("get detection range: "); Serial.println( MHZ19B.getDetectionRange(), HEX );
  delay(200);
  */

  

}
