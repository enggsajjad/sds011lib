//! ESP32 C/C++ Arduino library for the Nova Fitness sds011 PM sensor (implementation)

/// @file sds.ino 
/// @author Sajjad Hussain
/// @version 0.1

#include "sds011lib.h"

/// sensor pm10 values to be used
float p10;
/// sensor pm25 values to be used
float p25;
/// the status of the commands generally
bool  status;
/// hardware port to be used with the sensor
HardwareSerial port(2);
/// hardware uart rx pin
#define SDS_RX 13
/// hardware uart tx pin
#define SDS_TX 16
/// sds011 class instance
sds011 sds;
/// temporary array/string of char
char  line[80];
/**************************************************************************/
/*!
    @brief  initialization of peripherals attached to ESP32 board
    @returns void
*/
/**************************************************************************/

void setup() {
  uint8_t fwdate[2], mode, asleep;
  String v;
  uint16_t idd;
    
  sds.begin(&port,SDS_RX,SDS_TX);
  sds.setDebug ( false );
  	
  Serial.begin(115200);
  //Set the ID
  sds.deviceIdCmd(fwdate,0x66,0x50);
  //sds.deviceInfoCmd( fwdate );
  if ( fwdate[0] != 0xFF ){ 
    Serial.print( "fwdate = 20"); Serial.print( fwdate[0]); Serial.print( " month ");Serial.print( fwdate[1]); Serial.print("day "); Serial.println( fwdate[2] ); 
  }
  
  //get the ID and Version
  sds.deviceInfoCmd( &v,&idd );
  Serial.println( "Version = "+v+" ID = "+String(idd));
  
  sds.sleepWorkModeCmd(&asleep, mode, SETMODE );
  // If module is sleeping, then do not expect an answer. Better ask again after a short delay.
  delay(10);
  
  sds.sleepWorkModeCmd(&asleep, mode, ASKMODE );
  if ( asleep != 0xFF ){
    Serial.println("");Serial.print( "Module set to "); Serial.println( asleep == SLEEPMODE?"Sleep":"Work" ); 
  }




  sds.dataReportingModeCmd( &mode, QUERYMODE, SETMODE);
  if ( mode != 0xFF ){
    Serial.print( "dataReportingModeCmd set to "); Serial.println( mode == REPORTMODE?"Reportmode":"Querymode" ); 
  }

  //sds.dataReportingModeCmd( &mode, QUERYMODE, SETMODE);
  sds.workPeriodCmd( &mode, 5,1 );
  if ( mode != 0xFF ){
    Serial.println( "workPeriodCmd set"+String(mode));  
  }
  
  //sds.dataReportingModeCmd( &dataReportingModeCmd, QUERYMODE, SETMODE);
  sds.workPeriodCmd( &mode, 30,0 );
  if ( mode != 0xFF ){
    Serial.println( "workPeriodCmd get"+String(mode));  
  }

 
  delay(10000);
}

/**************************************************************************/
/*!
    @brief  reading the Microphone with timer function
    @returns void
*/
/**************************************************************************/
void loop() {
	status = sds.dataQueryCmd(&p10, &p25 );
	if ( status ) {
     Serial.print("ppm10 : "); Serial.print( p10,1 ); Serial.print("µg/m³ ppm2.5 : "); Serial.println( p25,1); 
	}
  // datasheet advises 3 seconds as minimum poll time.
	delay(3000);
}
