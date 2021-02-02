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
char  str[80];

/**************************************************************************/
/*!
    @brief  initialization of peripherals attached to ESP32 board
    @returns void
*/
/**************************************************************************/

void setup() {
  delay(1000);
  uint8_t fw[2], mode, asleep;
  String v;
  uint16_t idd;
    
  sds.begin(&port,SDS_RX,SDS_TX);
  sds.setDebug ( false );
    
  Serial.begin(115200);
  Serial.println("Testing SDS Dust Sensory Libray...");
  //Set the ID
  sds.deviceIdCmd(fw,0x66,0x50);
  if ( fw[0] != MSG_FF ){ 
    sprintf( str,"ID = %02X %02X\n",fw[0], fw[1]  );
    Serial.println( str );
  }
  
  //get the ID and Version
  sds.deviceInfoCmd( &v,&idd );
  Serial.println("Version = "+v+"ID = "+String(idd));
  
  /////////////////////////////////////////////////////////////////////////////////////
  //3rd 0：query the current mode
  //0: query the current mode
  //1: set mode
  //2nd 0：report active mode
  //0：continuous(default)
  //1-30minute：【work 30 seconds and sleep n*60-30 seconds】
  sds.workPeriodCmd( &mode, 3,1 );
  if ( mode != MSG_FF ){
    Serial.println( "Work Period set: "+String(mode));  
  }
  delay(100);
  
  sds.workPeriodCmd( &mode, 30,0 );
  if ( mode != MSG_FF ){
    Serial.println( "Work Period get: "+String(mode));  
  }
  delay(100);

  /////////////////////////////////////////////////////////////////////////////////////
  //3rd param
  //0: query the current mode
  //1: set mode
  //2nd param
  //0: sleep
  //1: work
  
  sds.sleepWorkModeCmd(&asleep, 1, ASKMODE );
  if ( asleep != MSG_FF ){
    Serial.print( "Module  get: "); Serial.println( asleep == SLEEPMODE?"Sleep":"Work" ); 
  }
  delay(100);
  
  sds.sleepWorkModeCmd(&asleep, 1, SETMODE );
  // If module is sleeping, then do not expect an answer. Better ask again after a short delay.
  if ( asleep != MSG_FF ){
    Serial.print( "Module switched to: "); Serial.println( asleep == SLEEPMODE?"Sleep":"Work" );
  }
  delay(100);
  
  sds.sleepWorkModeCmd(&asleep, 1, ASKMODE );
  if ( asleep != MSG_FF ){
    Serial.print( "Module Mode get: "); Serial.println( asleep == SLEEPMODE?"Sleep":"Work" ); 
  }
  delay(100);

  /////////////////////////////////////////////////////////////////////////////////////
  //3rd 0：query the current mode
  //1：set reporting mode
  //2nd 0：report active mode
  //1：Report query mode
  sds.dataReportingModeCmd( &mode, 1, SETMODE);
  if ( mode != MSG_FF ){
    Serial.print( "Reporting switched to: "); Serial.println( mode == REPORTMODE?"Reportmode":"Querymode" ); 
  }
  delay(100);

  sds.dataReportingModeCmd( &mode, 1, ASKMODE);
  if ( mode != MSG_FF ){
    Serial.print( "Reporting Mode get: "); Serial.println( mode == REPORTMODE?"Reportmode":"Querymode" ); 
  }
  delay(100);

}

/**************************************************************************/
/*!
    @brief  reading the Microphone with timer function
    @returns void
*/
/**************************************************************************/
void loop() 
{
  status = sds.dataQueryCmd(&p10, &p25 );
  if ( status ) {
     Serial.print("pm10: "); Serial.print( p10,1 ); Serial.print(", pm2.5: "); Serial.println( p25,1); 
  }
  // datasheet advises 3 seconds as minimum poll time.
  delay(3000);
}
