/* JPEG Encoder Library
 * Copyright (c) 2006-2016 SIProp.org Noritsuna Imamura(noritsuna@siprop.org)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef JPEGENCODER_H
#define JPEGENCODER_H

#include "HM01B0_BLE_DEFINES.h"
//*************************************************************************************
/*Added by Ali*/
#include "HM01B0_SPI.h"
#include "flash_emu.h"
//uint8_t       m_rx_buf[total_spi_buffer_size+1];       /**< RX buffer. */
extern uint8_t       jpeg_imag_mem[jpeg_imag_mem_size+1]; 
//uint8_t       m_rx_buf[total_spi_buffer_size+1];       /**< RX buffer. */
//*************************************************************************************     

//#include "jpegDef.h"
#include <stdint.h>

#define BLOCK_SIZE 8
#define MCU_SIZE_GRAY 8
#define MCU_SIZE_COLOR 16

#define JPEG_HEADER_SIZE_GRAY_SCALE 324
#define JPEG_HEADER_SIZE_FULL_COLOR 607
#define JPEG_BUF_SIZE JPEG_HEADER_SIZE_GRAY_SCALE

typedef unsigned int        uint_t;

typedef enum quality_rate {
        normal,
        lowest,
        lower,
        low,
        higher,
        highest
} quality_rate;

typedef enum color_type {
        YUV411,
        gray_scale  //8bits
} color_type;

typedef struct color_info{
    color_type color;
    int mcu_size;
    int byte_size;
    int pixel_size_by_byte;
} color_info;

#if(JPEG_16B == 1)
typedef struct jpeg_data {
    int width;
    int height;
    int mcu_width_max;
    int mcu_height_max;
    color_info c_info;
    quality_rate quality;
    
    uint8_t ret_data[JPEG_BUF_SIZE];
    int data_len; 
    
    struct{
        uint_t rest;
        uint8_t  byte;
    }stream;
    
    int pre_DC_Y;
    int pre_DC_Cb;
    int pre_DC_Cr;
    
    struct {
        uint8_t block_y[BLOCK_SIZE*BLOCK_SIZE];
        uint8_t block_Cr[BLOCK_SIZE*BLOCK_SIZE];
        uint8_t block_Cb[BLOCK_SIZE*BLOCK_SIZE];
        int block_int[BLOCK_SIZE*BLOCK_SIZE];
        uint8_t mcu[MCU_SIZE_COLOR*MCU_SIZE_COLOR];
        int temp[BLOCK_SIZE*BLOCK_SIZE];
    }work;
} jpeg_data ;
#else
typedef struct jpeg_data {
    int width;
    int height;
    int mcu_width_max;
    int mcu_height_max;
    color_info c_info;
    quality_rate quality;
    
    uint8_t ret_data[JPEG_BUF_SIZE];
    int data_len; 
    
    struct{
        uint_t rest;
        uint8_t  byte;
    }stream;
    
    int8_t pre_DC_Y;
    int8_t pre_DC_Cb;
    int8_t pre_DC_Cr;
    
    struct {
        uint8_t block_y[BLOCK_SIZE*BLOCK_SIZE];
        uint8_t block_Cr[BLOCK_SIZE*BLOCK_SIZE];
        uint8_t block_Cb[BLOCK_SIZE*BLOCK_SIZE];
        int8_t block_int[BLOCK_SIZE*BLOCK_SIZE];
        uint8_t mcu[MCU_SIZE_COLOR*MCU_SIZE_COLOR];
        int8_t temp[BLOCK_SIZE*BLOCK_SIZE];
    }work;
} jpeg_data ;
#endif

static const color_info color_info_gray_scale = {gray_scale, MCU_SIZE_GRAY, 8, 1};
static const color_info color_info_yuv411 = {YUV411, MCU_SIZE_COLOR, 16, 2};

/* Prototype */
/**
 * Generate JPEG file's header.
 * Required "Width" and "Height", "Color type", "Quality".
 * Header data in "ret_data[]".
 *
 * @param data JPEG Structure
 * @return Header Length. Under 0 is error.
 */
int generate_header(jpeg_data *data);

/**
 * Generate JPEG file's footer.
 * Footer data in "ret_data[]".
 *
 * @param data JPEG Structure
 * @return Footer Length. Under 0 is error.
 */
int generateFooter(jpeg_data *data);

/**
 * Encoding image data by MCU unit.
 * Supported Image format is "8bit Grayscale" and "YUV422(CbYCrY format)".
 * Encoded data in "ret_data[]".
 *
 * @param data JPEG Structure
 * @param mcu MCU or Grayscale-Block
 * @return Encoded data Length. Under 0 is error.
 */
int encode_MCU(jpeg_data *data, const uint8_t *mcu);

/**
 * Reading Image data & Writing Encoded data by each MCU.
 * Supported Image format is "8bit Grayscale" and "YUV422(CbYCrY format)" 1-dimensional array.
 *
 * @param data JPEG Structure
 * @param read_data_func Reading Image data function
 * @param write_data_func Writing Ecoding data Function
 * @return JPEG file size. Under 0 is error.
 */
int encode_image(jpeg_data *data, int (*read_data_func)(uint_t pos, uint8_t *data, uint_t len), int (*write_data_func)(uint8_t *data, uint_t len));

#endif
