//! ESP32 C/C++ Arduino library for the Nova Fitness SDS011 PM sensor (interface)

/// @file
/// @author Matthias Budde / Sajjad Hussain
/// @version 0.1

#ifndef sds011lib_h 
#define sds011lib_h 

#include "Arduino.h"

class sds011
{
    public:
        /// @brief constructor
        sds011(void);
        /// @brief initialization method 
        /// @param uart pointer to HardwareSerial
        /// @param rx RX pin
        /// @param tx TX pin
        void init(HardwareSerial *uart);
        void init(HardwareSerial *uart, int8_t rx, int8_t tx);
        /// @brief read method
        /// @param pm10 pointer to a float to which PM10 return value will be written
        /// @param pm25 pointer to a float to which PM2.5 return value will be written 
        /// @return err error code 
        uint8_t read(float *pm25_return, float *pm10_return);
        /// @brief sleep method, initiates manual hibernation
        /// @return err error code 
        uint8_t sleep();
        /// @brief wakeup method, ends manual hibernation
        /// @return err error code 
        uint8_t wakeup();
    private:
        Stream *_uart;
        uint16_t _serial_no;
        uint8_t _rx;
        uint8_t _tx;
        uint8_t _mode;
        /// @brief send_msg method
        /// @param command code of command that should be sent to SDS011 sensor
        /// @param option_1 first parameter to be set, depending on selected command (default: 0x00)
        /// @param option_2 second parameter to be set, depending on selected command (default: 0x00)
        /// @param id_1 first ID byte of sensor that the command is addressed to (default: 0xFF = all sensors)
        /// @param id_2 seconf ID byte of sensor that the command is addressed to (default: 0xFF = all sensors)
        /// @return err error code 
        uint8_t send_msg(uint8_t command, uint8_t option_1 = 0x00, uint8_t option_2 = 0x00, uint8_t id_1 = 0xFF, uint8_t id_2 = 0xFF);
        /// @brief read_msg method (overloaded)
        /// @param pointer to a float to which a return value will be written
        /// @param pointer to a float to which a return value will be written
        /// @return err error code 
        uint8_t read_msg(float *return_1, float *return_2);
        /// @brief read_msg method (overloaded)
        /// @param byte identifying the type of reply that should be read
        /// @param pointer to a byte to which a return value will be written
        /// @param pointer to a byte to which a return value will be written
        /// @return err error code 
        uint8_t read_msg(uint8_t command, uint16_t *return_1, uint16_t *return_2, uint16_t *id);
        uint8_t set_mode(uint8_t mode);
};



// (0) Basic reading of sensor data

// (1) Manual hibernation (sleep and wake)

// (2) Timed hibernate (cycle to work)

// (3) User ID setting

// (4) Set data reporting mode (active report and query report)

// (5) Version number query


#endif
