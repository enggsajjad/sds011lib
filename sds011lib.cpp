//! ESP32 C/C++ Arduino library for the Nova Fitness SDS011 PM sensor (implementation)

/// @file sds011lib.cpp 
/// @author Matthias Budde / Sajjad Hussain
/// @version 0.1

unsigned char MSG_HEAD        = 0xAA; //170
unsigned char MSG_TAIL        = 0xAB; //172
unsigned char MSG_RESERVED    = 0x00;
unsigned char CMD             = 0xB4; //180
unsigned char REPLY_DATA      = 0xC0; //192
unsigned char REPLY_CFG       = 0xC5; //197
unsigned char CMD_MODE        = 0x02;
unsigned char CMD_QUERY       = 0x04;
unsigned char CMD_ID          = 0x05;
unsigned char CMD_SLEEP       = 0x06;
unsigned char CMD_FW_VERSION  = 0x07;
unsigned char CMD_CYCLE       = 0x08;
unsigned char MODE_QUERY      = 0x01;

#include "sds011lib.h"

/**
 * @mainpage 
 * @section Description
 *
 * ESP32 C/C++ Arduino library for the Nova Fitness SDS011 PM sensor 
 *
 * @author Matthias Budde
 * @version 0.1
 *
 * @section License
 *
 * Copyright 2020 Matthias Budde
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @see https://opensource.org/licenses/MIT
 *
 * @section Specs
 *
 * Relevant specs are included in the documentation of the individual methods. The full and detailed specification can be found in the Nova Fitness Co. Ltd. SDS011 data sheet, V1.3 and the Nova Fitness Co. Ltd. Laser Dust Sensor Control Protocol, V1.4  
 * 
 * @see https://web.archive.org/web/20200525083221/https://www-sd-nf.oss-cn-beijing.aliyuncs.com/%E5%AE%98%E7%BD%91%E4%B8%8B%E8%BD%BD/SDS011%20laser%20PM2.5%20sensor%20specification-V1.4.pdf
 * @see https://www.arduinoforum.de/attachment.php?aid=3023
 */


sds011::sds011(void) {
}


/// Specification from the Nova Fitness Co. Ltd. SDS011 data sheet, V1.3 
///
/// | UART ||
/// | :-------------------- | :-------------|
/// | Bit rate              | 9600          |
/// | Data bit              | 8             |
/// | Parity bit            | NO            |
/// | Stop bit              | 1             |
/// | Data Packet frequency | 1Hz           |
void sds011::init(HardwareSerial* uart, int8_t rx, int8_t tx) {
    uart->begin(9600,SERIAL_8N1, rx, tx);
 _uart=uart;
  _rx = rx;
  _tx = tx;
};

/// Sends command to the sensor. 
/// 
/// Specification from the Nova Fitness Co. Ltd. Laser Dust Sensor Control Protocol, V1.4  
/// 
/// | Byte  |  Name           ||  Reporting Mode     |  Query Data         |  Set ID             |  Manual Sleep / Work| Get Firmware Version| Set Working Period  |
/// | :---: | :-------------- || :------------------ | :------------------ | :------------------ | :------------------ | :------------------ | :------------------ |
/// |  0    |  Message header || AA                  | AA                  | AA                  | AA                  | AA                  | AA                  |
/// |  1    |  Command ID     || B4                  | B4                  | B4                  | B4                  | B4                  | B4                  |
/// |  2    |  DATA 1         || 2                   | 4                   | 5                   | 6                   | 7                   | 8                   |
/// |  3    |  DATA 2         || 0: query / 1: set   | 0                   | 0                   | 0: query / 1: set   | 0                   | 0: query / 1: set   |
/// |  4    |  DATA 3         || 0: auto / 1: query  | 0                   | 0                   | 0: sleep / 1: work  | 0                   | 0: continuous / 1-30: minutes sleep |
/// |  5    |  DATA 4         || 0                   | 0                   | 0                   | 0                   | 0                   | 0                   |
/// |  6    |  DATA 5         || 0                   | 0                   | 0                   | 0                   | 0                   | 0                   |
/// |  7    |  DATA 6         || 0                   | 0                   | 0                   | 0                   | 0                   | 0                   |
/// |  8    |  DATA 7         || 0                   | 0                   | 0                   | 0                   | 0                   | 0                   |
/// |  9    |  DATA 8         || 0                   | 0                   | 0                   | 0                   | 0                   | 0                   |
/// | 10    |  DATA 9         || 0                   | 0                   | 0                   | 0                   | 0                   | 0                   |
/// | 11    |  DATA 10        || 0                   | 0                   | 0                   | 0                   | 0                   | 0                   |
/// | 12    |  DATA 11        || 0                   | 0                   | 0                   | 0                   | 0                   | 0                   |
/// | 13    |  DATA 12        || 0                   | 0                   | New ID Byte 1       | 0                   | 0                   | 0                   |
/// | 14    |  DATA 13        || 0                   | 0                   | New ID Byte 2       | 0                   | 0                   | 0                   |
/// | 15    |  DATA 14        || ID Byte 1 (FF: all) | ID Byte 1 (FF: all) | ID Byte 1 (FF: all) | ID Byte 1 (FF: all) | ID Byte 1 (FF: all) | ID Byte 1 (FF: all) |
/// | 16    |  DATA 15        || ID Byte 2 (FF: all) | ID Byte 2 (FF: all) | ID Byte 2 (FF: all) | ID Byte 2 (FF: all) | ID Byte 2 (FF: all) | ID Byte 2 (FF: all) |
/// | 17    |  Check-sum      || checksum            | checksum            | checksum            | checksum            | checksum            | checksum            |
/// | 18    |  Message tail   || AB                  | AB                  | AB                  | AB                  | AB                  | AB                  |
///
/// | Calculation  ||
/// | :----------  | :-------------------------------------------------------------|
/// | Check-sum:   | Check-sum = DATA1 + DATA2 + ... + DATA15                      |
///
/// @see https://www.arduinoforum.de/attachment.php?aid=3023
///
uint8_t sds011::send_msg(uint8_t command, uint8_t option_1, uint8_t option_2, uint8_t id_1, uint8_t id_2) {
    uint8_t checksum = command + option_1 + option_2 + id_1 + id_2;

    _uart->write(MSG_HEAD);
    _uart->write(CMD);
    _uart->write(command);
    if ((command == CMD_MODE) || (command == CMD_SLEEP) || (command == CMD_CYCLE)) {
        _uart->write(option_1); 
        _uart->write(option_2);
    }
    for (uint8_t i = 0; i < 10; i++) {
        _uart->write(MSG_RESERVED);
    }
    if (command == CMD_ID) {
        _uart->write(option_1); 
        _uart->write(option_2);
    } else if ((command == CMD_QUERY) || (command == CMD_FW_VERSION)) {
        _uart->write(MSG_RESERVED);
        _uart->write(MSG_RESERVED);
    }
    _uart->write(id_1);
    _uart->write(id_2);
    _uart->write(checksum);
    _uart->write(MSG_TAIL);

    _uart->flush();

    return 0;
};

/// Prompts sensor to report measurement data. If sensor is in report query mode, an according query command is sent first, otherwise, the next measurent reported in the 
/// 
/// Specification from the Nova Fitness Co. Ltd. Laser Dust Sensor Control Protocol, V1.4  
/// 
/// | Byte  |  Name           || Set Mode Reply    | Query Data Reply| Set ID reply    | Sleep / Work Reply | FW Version Reply| Working Period Reply|                 
/// | :---: | :-------------- || :---------------- | :-------------- | :-------------- | :----------------- | :-------------- | :------------------ |                  
/// | 0     |  Message header || AA                | AA              | AA              | AA                 | AA              | AA                  |                 
/// | 1     |  Command ID     || C5                | C0              | C5              | C5                 | C5              | C5                  |                 
/// | 2     |  DATA 1         || 2                 | PM2.5 Low byte  | 5               | 6                  | 7               | 8                   |                 
/// | 3     |  DATA 2         || 0: query / 1: set | PM2.5 High byte | 0               | 0: query / 1: set  | Byte 1: year    | 0: query / 1: set   |                 
/// | 4     |  DATA 3         || 0: auto / 1: query| PM10 Low byte   | 0               | 0: sleep / 1: work | Byte 2: month   | 0: continuous / 1-30: minutes sleep | 
/// | 5     |  DATA 4         || 0                 | PM10 High byte  | 0               | 0                  | Byte 3: day     | 0                   |
/// | 6     |  DATA 5         || ID byte 1         | ID byte 1       | New ID byte 1   | ID byte 1          | ID byte 1       | ID byte 1           |
/// | 7     |  DATA 6         || ID byte 2         | ID byte 2       | New ID byte 2   | ID byte 2          | ID byte 2       | ID byte 2           |
/// | 8     |  Check-sum      || Check-sum         | Check-sum       | Check-sum       | Check-sum          | Check-sum       | Check-sum           |
/// | 9     |  Message tail   || AB                | AB              | AB              | AB                 | AB              | AB                  |
///                                                                
/// | Calculation  ||
/// | :----------  | :-------------------------------------------------------------|
/// | Check-sum:   | Check-sum = DATA1 + DATA2 + ... + DATA6                       |
/// | PM2.5 value: | PM2.5 (μg /m3) = ((PM2.5 High byte *256) + PM2.5 low byte)/10 |
/// | PM10 value:  | PM10 (μg /m3) = ((PM10 high byte*256) + PM10 low byte)/10     |
/// 
/// @see https://web.archive.org/web/20200525083221/https://www-sd-nf.oss-cn-beijing.aliyuncs.com/%E5%AE%98%E7%BD%91%E4%B8%8B%E8%BD%BD/SDS011%20laser%20PM2.5%20sensor%20specification-V1.4.pdf
/// @see https://www.arduinoforum.de/attachment.php?aid=3023
/// 
uint8_t sds011::read_msg(float *return_1, float *return_2) {
    uint8_t current_byte = 0x00;
    uint8_t pm10_low_byte = 0x00;
    uint8_t pm10_high_byte = 0x00;
    uint8_t pm25_low_byte = 0x00;
    uint8_t pm25_high_byte = 0x00;
    uint8_t id_byte_1 = 0x00;
    uint8_t id_byte_2 = 0x00;
    uint8_t err = 1;
    uint8_t bytes_read = 0;
    int checksum = 0;
    int ok = 0;

    while((_uart->available() > 0) && (_uart->available() >= (10-bytes_read))) 
    {
        current_byte = _uart->read();
        /*Serial.print("bytes count,read= ");
        Serial.print(bytes_read);
        Serial.print(", ");
        Serial.println(current_byte);*/
        switch(bytes_read) {
            case 0: if(current_byte == MSG_HEAD) { bytes_read++;} break;
            case 1: if(current_byte == REPLY_DATA) {bytes_read++;} else {bytes_read = 0;}  break;
            case 2: pm25_low_byte = current_byte; bytes_read++; checksum = current_byte;  break;
            case 3: pm25_high_byte = current_byte; bytes_read++; checksum += current_byte;  break;
            case 4: pm10_low_byte = current_byte; bytes_read++; checksum += current_byte;  break;
            case 5: pm10_high_byte = current_byte; bytes_read++; checksum += current_byte;  break;
            case 6: id_byte_1 = current_byte; bytes_read++; checksum += current_byte;  break;
            case 7: id_byte_2 = current_byte; bytes_read++; checksum += current_byte;  break;
            case 8: if (current_byte ==  ((uint8_t)(checksum%256))) {ok = 1;bytes_read++;} else {bytes_read = 0;}  break;
            case 9: if(current_byte == MSG_TAIL) {bytes_read++;} else {bytes_read = 0; }  break;
        }
        
        if((bytes_read == 10) && (ok == 1)) 
        {
            *return_1 = float(uint8_t(pm25_high_byte)*256 + uint8_t(pm25_low_byte))/10.0; 
            *return_2 = float(uint8_t(pm10_high_byte)*256 + uint8_t(pm10_low_byte))/10.0; 
            bytes_read = 0;
            err = 0;
            ok = 0;
        }
        yield();
    }    
    return err;

};

uint8_t sds011::read_msg(uint8_t command, uint16_t *return_1, uint16_t *return_2, uint16_t *id) {
    uint8_t current_byte = 0x00;
    uint8_t command_byte = 0x00;
    uint8_t return_byte_1 = 0x00;
    uint8_t return_byte_2 = 0x00;
    uint8_t id_byte_1 = 0x00;
    uint8_t id_byte_2 = 0x00;
    uint8_t pm10_low_byte,pm10_high_byte,pm25_low_byte,pm25_high_byte;
    
    ///@todo change id to uint16_t instead of two uint8_t
    //uint16_t id = 0x0000;
    uint8_t err = 1;
    uint8_t bytes_read = 0;

    while(_uart->available() > 10-bytes_read) {
        current_byte = _uart->read();
        switch(bytes_read) {
            case 0: if(current_byte == MSG_HEAD) {bytes_read++;} break;
            case 1: if(current_byte == REPLY_CFG) {bytes_read++;} else {bytes_read = 0;} break;
            case 2: command_byte = current_byte; bytes_read++; break;
            case 3: bytes_read++; break;
            case 4: bytes_read++; break;
            case 5: bytes_read++; break;
            case 6: id_byte_1 = current_byte; bytes_read++; break;
            case 7: id_byte_2 = current_byte; bytes_read++; break;
            case 8: if (uint8_t(current_byte) ==  (uint8_t(return_byte_1)+uint8_t(return_byte_2)+uint8_t(id_byte_1)+uint8_t(id_byte_2))) {err = 0;} else {bytes_read = 0;} break;
            case 9: if(current_byte == MSG_TAIL) {bytes_read++;} else {bytes_read = 0; err = 1;} break;
        };

        if((bytes_read == 10) && (err == 0)) {
        }

        pm10_low_byte = 0x00;
        pm10_high_byte = 0x00;
        pm25_low_byte = 0x00;
        pm25_high_byte = 0x00;
        id_byte_1 = 0x00;
        id_byte_2 = 0x00;
        err = 1;
        bytes_read = 0;

    };

    return err;

};


/// Prompts sensor to report measurement data. If sensor is in report query mode, an according query command is sent first, otherwise, the most recent measurement reported in the working period is returned. 
/// @todo FULLY IMPLEMENT read()
uint8_t sds011::read(float *pm10_return, float *pm25_return) {
  uint8_t err = 1;
    if(_mode == MODE_QUERY) {
        ///@todo send query command 
        //send_msg(CMD_QUERY);
    };
    err = read_msg(pm25_return, pm10_return);
    return(err);
};

/// @todo TEST sleep()
uint8_t sds011::sleep() {
    send_msg(CMD_SLEEP, 1, 0);
    uint8_t val = 0;
    //uint8_t err = read_msg(*val);
    uint8_t err;
    return err;
};


/// @todo TEST wakeup()
uint8_t sds011::wakeup() {
    uint8_t err = send_msg(CMD_SLEEP, 1, 1);
    return err;
};

// (2) Timed hibernate (cycle to work)

// (3) User ID setting

// (4) Set data reporting mode (active report and query report)
/// @todo TEST set_mode()
uint8_t sds011::set_mode(uint8_t mode) {
    uint8_t err = send_msg(CMD_MODE, 1, mode);
    return err;
};

// (5) Version number query
