/*
 * MC6470Regs.h
 *
 *  Created on: Apr 26, 2019
 *      Author: Vikram 
 */
#ifndef MC6470_REGS_H
#define MC6470_REGS_H

// Magnetometer registers
#define STB 0x0C
#define MORE_INFO_VERSION 0x0D
#define MORE_INFO 0x0E
#define WHO_AM_I 0x0F
#define MAG_X_LSB 0x10
#define MAG_X_MSB 0x11
#define MAG_Y_LSB 0x12
#define MAG_Y_MSB 0x13
#define MAG_Z_LSB 0x14
#define MAG_Z_MSB 0x15

#define STATUS 0x18

#define CNTL1 0x1B
#define CNTL2 0x1C
#define CNTL3 0x1D
#define CNTL4 0x1E

#define OFFSET_X_LSB 0x20
#define OFFSET_X_MSB 0x21
#define OFFSET_Y_LSB 0x22
#define OFFSET_Y_MSB 0x23
#define OFFSET_Z_LSB 0x24
#define OFFSET_Z_MSB 0x25

#define ITHR_L 0x26
#define ITHR_H 0x27

#define TEMPERATURE 0x31

#define SELF_TEST 0x10
#define START_10HZ 0x88


// Accelerometer registers
#define SR 0x03
#define OPSTAT 0x04
#define INTEN 0x06
#define MODE 0x07
#define SRTFR 0x08
#define TAPEN 0x09
#define WAKE 0x01
#define RATE_8HZ 0x02
#define RATE_4HZ 0x03
#define RATE_2HZ 0x04
#define RATE_1HZ 0x05
#define RATE_05HZ 0x06
#define RATE_025HZ 0x07
#define OUTCFG 0x20

#define TTTRX 0x0A
#define TTTRY 0x0B
#define TTTRZ 0x0C

#define XOUT_EX_L 0x0D
#define XOUT_EX_H 0x0E
#define YOUT_EX_L 0x0F
#define YOUT_EX_H 0x10
#define ZOUT_EX_L 0x11
#define ZOUT_EX_H 0x12

#define XOFFL 0x21
#define XOFFH 0x22
#define YOFFL 0x23
#define YOFFH 0x24
#define ZOFFL 0x25
#define ZOFFH 0x26


#endif