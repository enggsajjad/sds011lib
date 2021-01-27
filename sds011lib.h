//! ESP32 C/C++ Arduino library for the Nova Fitness SDS011 PM sensor (interface)

/// @file sds011lib.h
/// @author Sajjad Hussain
/// @version 0.1

#ifndef PM_SDS011_h 
#define PM_SDS011_h 

#include "Arduino.h"

/// Reporting mode as auto
#define REPORTMODE 0
/// Reporting mode as queries
#define QUERYMODE  1
/// get or receive
#define ASKMODE 0
/// set or write
#define SETMODE 1
/// mode set to sleep
#define SLEEPMODE 0
/// mode set to normal
#define WORKMODE  1
/// set the maximum trials for response
#define MAX_WAIT 30

/// sds011 sensor class interface to interace with the hardware
class sds011 {
	public:
		sds011(void);
		bool begin(HardwareSerial* uart,	uint8_t rx, uint8_t tx, uint8_t id_1 = 0xFF, uint8_t id_2 = 0xFF );
		bool dataReportingModeCmd( 			uint8_t *response, 	uint8_t mode = REPORTMODE,	uint8_t set = ASKMODE );
		bool dataQueryCmd( float *ppm10, 		float *ppm25 );
		bool deviceIdCmd( 		uint8_t response[2], 	uint8_t new_Id1, uint8_t new_Id2 );
		bool sleepWorkModeCmd( 		uint8_t *response, 	uint8_t awake = WORKMODE,uint8_t set = ASKMODE);
		bool workPeriodCmd(		uint8_t *response, 	uint8_t minutes = 0,uint8_t set = ASKMODE);
		bool deviceInfoCmd( String *ver, uint16_t *id );
    void setDebug( bool on );
	private:
    /// uart rx pin
		uint8_t _rx;
		/// uart tx pin
   uint8_t _tx;
   /// serial number lower byte
		uint8_t _id_1;
   /// serial number lower byte
   uint8_t _id_2;
   /// the debugging flag
		bool _debug;
    /// the hardware uart port
    Stream *_uart;  
    bool sendCommand( uint8_t command, uint8_t option_1, uint8_t  option_2, uint8_t id_1, uint8_t id_2 );
    bool getResponse(uint8_t cmd, uint8_t reply[10] );
    bool sdsCommunicate( uint8_t command, uint8_t option_1, uint8_t  option_2, uint8_t id_1, uint8_t id_2, uint8_t reply[10]  );
			
};

#endif
