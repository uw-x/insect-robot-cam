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
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "jpegencoder.h"

#define QUANT_SIZE BLOCK_SIZE*BLOCK_SIZE




/**
 ISO/IEC 10918 ITU-T Recommendation T.81 Appendix:K
 */
/**
 * zigzag table.
 */
static const int zigzag_tbl[] = {
    0,1,8,16,9,2,3,10,
    17,24,32,25,18,11,4,5,
    12,19,26,33,40,48,41,34,
    27,20,13,6,7,14,21,28,
    35,42,49,56,57,50,43,36,
    29,22,15,23,30,37,44,51,
    58,59,52,45,38,31,39,46,
    53,60,61,54,47,55,62,63
};

/**
 * Standard quantization table For luminance (sorted by zigzag_tbl).
 * Quality rate is "0".
 */
static const uint8_t std_luminance_quant_tbl_0[] = {
    0x20, 0x16, 0x18, 0x1C, 0x18, 0x14, 0x20, 0x1C, 
    0x1A, 0x1C, 0x24, 0x22, 0x20, 0x26, 0x30, 0x50, 
    0x34, 0x30, 0x2C, 0x2C, 0x30, 0x62, 0x46, 0x4A, 
    0x3A, 0x50, 0x74, 0x66, 0x7A, 0x78, 0x72, 0x66, 
    0x70, 0x6E, 0x80, 0x90, 0xB8, 0x9C, 0x80, 0x88, 
    0xAE, 0x8A, 0x6E, 0x70, 0xA0, 0xDA, 0xA2, 0xAE, 
    0xBE, 0xC4, 0xCE, 0xD0, 0xCE, 0x7C, 0x9A, 0xE2, 
    0xF2, 0xE0, 0xC8, 0xF0, 0xB8, 0xCA, 0xCE, 0xC6
};
/**
 * Standard quantization table For luminance (sorted by zigzag_tbl).
 * Quality rate is "10".
 */
static const uint8_t std_luminance_quant_tbl_10[] = {
    0x10, 0x0B, 0x0C, 0x0E, 0x0C, 0x0A, 0x10, 0x0E,
    0x0D, 0x0E, 0x12, 0x11, 0x10, 0x13, 0x18, 0x28,
    0x1A, 0x18, 0x16, 0x16, 0x18, 0x31, 0x23, 0x25,
    0x1D, 0x28, 0x3A, 0x33, 0x3D, 0x3C, 0x39, 0x33,
    0x38, 0x37, 0x40, 0x48, 0x5C, 0x4E, 0x40, 0x44,
    0x57, 0x45, 0x37, 0x38, 0x50, 0x6D, 0x51, 0x57,
    0x5F, 0x62, 0x67, 0x68, 0x67, 0x3E, 0x4D, 0x71,
    0x79, 0x70, 0x64, 0x78, 0x5C, 0x65, 0x67, 0x63
};
/**
 * Standard quantization table For luminance (sorted by zigzag_tbl).
 * Quality rate is "20".
 */
static const uint8_t std_luminance_quant_tbl_20[] = {
    0x0C, 0x08, 0x09, 0x0B, 0x09, 0x08, 0x0C, 0x0B, 
    0x0A, 0x0B, 0x0E, 0x0D, 0x0C, 0x0F, 0x13, 0x20, 
    0x14, 0x13, 0x11, 0x11, 0x13, 0x27, 0x1C, 0x1D, 
    0x17, 0x20, 0x2E, 0x28, 0x30, 0x30, 0x2D, 0x28, 
    0x2C, 0x2C, 0x33, 0x39, 0x49, 0x3E, 0x33, 0x36, 
    0x45, 0x37, 0x2C, 0x2C, 0x40, 0x57, 0x40, 0x45, 
    0x4C, 0x4E, 0x52, 0x53, 0x52, 0x31, 0x3D, 0x5A, 
    0x60, 0x59, 0x50, 0x60, 0x49, 0x50, 0x52, 0x4F
};
/**
 * Standard quantization table For luminance (sorted by zigzag_tbl).
 * Quality rate is "50".
 */
static const uint8_t std_luminance_quant_tbl_50[] = {
    0x08, 0x05, 0x06, 0x07, 0x06, 0x05, 0x08, 0x07, 
    0x06, 0x07, 0x09, 0x08, 0x08, 0x09, 0x0C, 0x14, 
    0x0D, 0x0C, 0x0B, 0x0B, 0x0C, 0x18, 0x11, 0x12, 
    0x0E, 0x14, 0x1D, 0x19, 0x1E, 0x1E, 0x1C, 0x19, 
    0x1C, 0x1B, 0x20, 0x24, 0x2E, 0x27, 0x20, 0x22, 
    0x2B, 0x22, 0x1B, 0x1C, 0x28, 0x36, 0x28, 0x2B, 
    0x2F, 0x31, 0x33, 0x34, 0x33, 0x1F, 0x26, 0x38, 
    0x3C, 0x38, 0x32, 0x3C, 0x2E, 0x32, 0x33, 0x31
};
/**
 * Standard quantization table For luminance (sorted by zigzag_tbl).
 * Quality rate is "90".
 */
static const uint8_t std_luminance_quant_tbl_90[] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x04, 
    0x02, 0x02, 0x02, 0x02, 0x02, 0x04, 0x03, 0x03, 
    0x02, 0x04, 0x05, 0x05, 0x06, 0x06, 0x05, 0x05, 
    0x05, 0x05, 0x06, 0x07, 0x09, 0x07, 0x06, 0x06, 
    0x08, 0x06, 0x05, 0x05, 0x08, 0x0A, 0x08, 0x08, 
    0x09, 0x09, 0x0A, 0x0A, 0x0A, 0x06, 0x07, 0x0B, 
    0x0C, 0x0B, 0x0A, 0x0C, 0x09, 0x0A, 0x0A, 0x09
};
/**
 * Standard quantization table For luminance (sorted by zigzag_tbl).
 * Quality rate is "95".
 */
static const uint8_t std_luminance_quant_tbl_95[] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x01, 0x01, 
    0x01, 0x02, 0x02, 0x02, 0x03, 0x03, 0x02, 0x02, 
    0x02, 0x02, 0x03, 0x03, 0x04, 0x03, 0x03, 0x03, 
    0x04, 0x03, 0x02, 0x02, 0x04, 0x05, 0x04, 0x04, 
    0x04, 0x04, 0x05, 0x05, 0x05, 0x03, 0x03, 0x05, 
    0x06, 0x05, 0x05, 0x06, 0x04, 0x05, 0x05, 0x04
};
/**
 * Standard quantization table For luminance (sorted by zigzag_tbl).
 * Quality rate is "100".
 */
static const uint8_t std_luminance_quant_tbl_100[] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};

/**
 * Standard quantization table For chrominance (sorted by zigzag_tbl).
 * Quality rate is "0".
 */
static const uint8_t std_chrominance_quant_tbl_0[] = {
    0x22, 0x24, 0x24, 0x30, 0x2A, 0x30, 0x5E, 0x34, 
    0x34, 0x5E, 0xC6, 0x84, 0x70, 0x84, 0xC6, 0xC6, 
    0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 
    0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 
    0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 
    0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 
    0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 
    0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6
};
/**
 * Standard quantization table For chrominance (sorted by zigzag_tbl).
 * Quality rate is "10".
 */
static const uint8_t std_chrominance_quant_tbl_10[] = {
    0x11, 0x12, 0x12, 0x18, 0x15, 0x18, 0x2F, 0x1A, 
    0x1A, 0x2F, 0x63, 0x42, 0x38, 0x42, 0x63, 0x63, 
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63
};
/**
 * Standard quantization table For chrominance (sorted by zigzag_tbl).
 * Quality rate is "20".
 */
static const uint8_t std_chrominance_quant_tbl_20[] = {
    0x0D, 0x0E, 0x0E, 0x13, 0x10, 0x13, 0x25, 0x14, 
    0x14, 0x25, 0x4F, 0x34, 0x2C, 0x34, 0x4F, 0x4F, 
    0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 
    0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 
    0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 
    0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 
    0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 
    0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F
};
/**
 * Standard quantization table For chrominance (sorted by zigzag_tbl).
 * Quality rate is "50".
 */
static const uint8_t std_chrominance_quant_tbl_50[] = {
    0x08, 0x09, 0x09, 0x0C, 0x0A, 0x0C, 0x17, 0x0D, 
    0x0D, 0x17, 0x31, 0x21, 0x1C, 0x21, 0x31, 0x31, 
    0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 
    0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 
    0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 
    0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 
    0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 
    0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31
};
/**
 * Standard quantization table For chrominance (sorted by zigzag_tbl).
 * Quality rate is "90".
 */
static const uint8_t std_chrominance_quant_tbl_90[] = {
    0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x04, 0x02, 
    0x02, 0x04, 0x09, 0x06, 0x05, 0x06, 0x09, 0x09, 
    0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
    0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
    0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
    0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
    0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
    0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09
};
/**
 * Standard quantization table For chrominance (sorted by zigzag_tbl).
 * Quality rate is "95".
 */
static const uint8_t std_chrominance_quant_tbl_95[] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x01, 
    0x01, 0x02, 0x04, 0x03, 0x02, 0x03, 0x04, 0x04, 
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04
};
/**
 * Standard quantization table For chrominance (sorted by zigzag_tbl).
 * Quality rate is "100".
 */
static const uint8_t std_chrominance_quant_tbl_100[] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};



/**
 * Standard Huffman code table For DC luminance.
 */
static const uint8_t std_dc_luminance_huff_val_tbl[] = {
    0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B
};
/**
 * Standard Huffman code's length table For DC luminance.
 */
static const uint8_t std_dc_luminance_huff_len_tbl[] = {
    2,3,3,3,3,3,4,5,6,7,8,9
};
/**
 * Calculated Huffman code table For DC luminance.
 */
static const int16_t std_dc_luminance_huff_code_tbl[] = {
    0x0000, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x000e, 0x001e,
    0x003e, 0x007e, 0x00fe, 0x01fe
};

/**
 * Standard Huffman code table For AC luminance.
 */
static const uint8_t std_ac_luminance_huff_val_tbl[] = {
    0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 
    0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7D,
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
    0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08,
    0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16,
    0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
    0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
    0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
    0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6,
    0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5,
    0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4,
    0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2,
    0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA,
    0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
    0xF9, 0xFA
};
/**
 * Standard Huffman code's length table For AC luminance.
 */
static const uint8_t std_ac_luminance_huff_len_tbl[] = {
    4, 2, 2, 3, 4, 5, 7, 8,
   10,16,16, 4, 5, 7, 9,11,
   16,16,16,16,16, 5, 8,10,
   12,16,16,16,16,16,16, 6,
    9,12,16,16,16,16,16,16,
   16, 6,10,16,16,16,16,16,
   16,16,16, 7,11,16,16,16,
   16,16,16,16,16, 7,12,16,
   16,16,16,16,16,16,16, 8,
   12,16,16,16,16,16,16,16,
   16, 9,15,16,16,16,16,16,
   16,16,16, 9,16,16,16,16,
   16,16,16,16,16, 9,16,16,
   16,16,16,16,16,16,16,10,
   16,16,16,16,16,16,16,16,
   16,10,16,16,16,16,16,16,
   16,16,16,11,16,16,16,16,
   16,16,16,16,16,16,16,16,
   16,16,16,16,16,16,16,11,
   16,16,16,16,16,16,16,16,
   16,16
};
/**
 * Calculated Huffman code table For AC luminance.
 */
static const int16_t std_ac_luminance_huff_code_tbl[] = {
    0x000a, 0x0000, 0x0001, 0x0004, 0x000b, 0x001a, 0x0078, 0x00f8,
    0x03f6, 0xff82, 0xff83, 0x000c, 0x001b, 0x0079, 0x01f6, 0x07f6,
    0xff84, 0xff85, 0xff86, 0xff87, 0xff88, 0x001c, 0x00f9, 0x03f7,
    0x0ff4, 0xff89, 0xff8a, 0xff8b, 0xff8c, 0xff8d, 0xff8e, 0x003a,
    0x01f7, 0x0ff5, 0xff8f, 0xff90, 0xff91, 0xff92, 0xff93, 0xff94,
    0xff95, 0x003b, 0x03f8, 0xff96, 0xff97, 0xff98, 0xff99, 0xff9a,
    0xff9b, 0xff9c, 0xff9d, 0x007a, 0x07f7, 0xff9e, 0xff9f, 0xffa0,
    0xffa1, 0xffa2, 0xffa3, 0xffa4, 0xffa5, 0x007b, 0x0ff6, 0xffa6,
    0xffa7, 0xffa8, 0xffa9, 0xffaa, 0xffab, 0xffac, 0xffad, 0x00fa,
    0x0ff7, 0xffae, 0xffaf, 0xffb0, 0xffb1, 0xffb2, 0xffb3, 0xffb4,
    0xffb5, 0x01f8, 0x7fc0, 0xffb6, 0xffb7, 0xffb8, 0xffb9, 0xffba,
    0xffbb, 0xffbc, 0xffbd, 0x01f9, 0xffbe, 0xffbf, 0xffc0, 0xffc1,
    0xffc2, 0xffc3, 0xffc4, 0xffc5, 0xffc6, 0x01fa, 0xffc7, 0xffc8,
    0xffc9, 0xffca, 0xffcb, 0xffcc, 0xffcd, 0xffce, 0xffcf, 0x03f9,
    0xffd0, 0xffd1, 0xffd2, 0xffd3, 0xffd4, 0xffd5, 0xffd6, 0xffd7,
    0xffd8, 0x03fa, 0xffd9, 0xffda, 0xffdb, 0xffdc, 0xffdd, 0xffde,
    0xffdf, 0xffe0, 0xffe1, 0x07f8, 0xffe2, 0xffe3, 0xffe4, 0xffe5,
    0xffe6, 0xffe7, 0xffe8, 0xffe9, 0xffea, 0xffeb, 0xffec, 0xffed,
    0xffee, 0xffef, 0xfff0, 0xfff1, 0xfff2, 0xfff3, 0xfff4, 0x07f9,
    0xfff5, 0xfff6, 0xfff7, 0xfff8, 0xfff9, 0xfffa, 0xfffb, 0xfffc,
    0xfffd, 0xfffe
};

/**
 * Standard Huffman code table For DC chrominance.
 */
static const uint8_t std_dc_chrominance_huff_val_tbl[] = {
    0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
    0x08, 0x09, 0x0A, 0x0B
};
/**
 * Standard Huffman code's length table For DC chrominance.
 */
static const uint8_t std_dc_chrominance_huff_len_tbl[] = {
    2,2,2,3,4,5,6,7,8,9,10,11
};
/**
 * Calculated Huffman code table For DC chrominance.
 */
static const int16_t std_dc_chrominance_huff_code_tbl[] = {
    0x0000, 0x0001, 0x0002, 0x0006, 0x000e, 0x001e, 0x003e, 0x007e,
    0x00fe, 0x01fe, 0x03fe, 0x07fe
};

/**
 * Standard Huffman code table For AC chrominance.
 */
static const uint8_t std_ac_chrominance_huff_val_tbl[] = {
    0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 
    0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 
    0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 
    0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0, 
    0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 
    0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26, 
    0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 
    0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 
    0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 
    0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 
    0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 
    0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 
    0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 
    0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 
    0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 
    0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 
    0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 
    0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 
    0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 
    0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 
    0xF9, 0xFA
};
/**
 * Standard Huffman code's length table For AC chrominance.
 */
static const uint8_t std_ac_chrominance_huff_len_tbl[] = {
    2, 2, 3, 4, 5, 5, 6, 7,
     9,10,12, 4, 6, 8, 9,11,
    12,16,16,16,16, 5, 8,10,
    12,15,16,16,16,16,16, 5,
     8,10,12,16,16,16,16,16,
    16, 6, 9,16,16,16,16,16,
    16,16,16, 6,10,16,16,16,
    16,16,16,16,16, 7,11,16,
    16,16,16,16,16,16,16, 7,
    11,16,16,16,16,16,16,16,
    16, 8,16,16,16,16,16,16,
    16,16,16, 9,16,16,16,16,
    16,16,16,16,16, 9,16,16,
    16,16,16,16,16,16,16, 9,
    16,16,16,16,16,16,16,16,
    16, 9,16,16,16,16,16,16,
    16,16,16,11,16,16,16,16,
    16,16,16,16,16,14,16,16,
    16,16,16,16,16,16,16,10,
    15,16,16,16,16,16,16,16,
    16,16
};
/**
 * Calculated Huffman code table For AC chrominance.
 */
static const int16_t std_ac_chrominance_huff_code_tbl[] = {
    0x0000, 0x0001, 0x0004, 0x000a, 0x0018, 0x0019, 0x0038, 0x0078,
    0x01f4, 0x03f6, 0x0ff4, 0x000b, 0x0039, 0x00f6, 0x01f5, 0x07f6,
    0x0ff5, 0xff88, 0xff89, 0xff8a, 0xff8b, 0x001a, 0x00f7, 0x03f7,
    0x0ff6, 0x7fc2, 0xff8c, 0xff8d, 0xff8e, 0xff8f, 0xff90, 0x001b,
    0x00f8, 0x03f8, 0x0ff7, 0xff91, 0xff92, 0xff93, 0xff94, 0xff95,
    0xff96, 0x003a, 0x01f6, 0xff97, 0xff98, 0xff99, 0xff9a, 0xff9b,
    0xff9c, 0xff9d, 0xff9e, 0x003b, 0x03f9, 0xff9f, 0xffa0, 0xffa1,
    0xffa2, 0xffa3, 0xffa4, 0xffa5, 0xffa6, 0x0079, 0x07f7, 0xffa7,
    0xffa8, 0xffa9, 0xffaa, 0xffab, 0xffac, 0xffad, 0xffae, 0x007a,
    0x07f8, 0xffaf, 0xffb0, 0xffb1, 0xffb2, 0xffb3, 0xffb4, 0xffb5,
    0xffb6, 0x00f9, 0xffb7, 0xffb8, 0xffb9, 0xffba, 0xffbb, 0xffbc,
    0xffbd, 0xffbe, 0xffbf, 0x01f7, 0xffc0, 0xffc1, 0xffc2, 0xffc3,
    0xffc4, 0xffc5, 0xffc6, 0xffc7, 0xffc8, 0x01f8, 0xffc9, 0xffca,
    0xffcb, 0xffcc, 0xffcd, 0xffce, 0xffcf, 0xffd0, 0xffd1, 0x01f9,
    0xffd2, 0xffd3, 0xffd4, 0xffd5, 0xffd6, 0xffd7, 0xffd8, 0xffd9,
    0xffda, 0x01fa, 0xffdb, 0xffdc, 0xffdd, 0xffde, 0xffdf, 0xffe0,
    0xffe1, 0xffe2, 0xffe3, 0x07f9, 0xffe4, 0xffe5, 0xffe6, 0xffe7,
    0xffe8, 0xffe9, 0xffea, 0xffeb, 0xffec, 0x3fe0, 0xffed, 0xffee,
    0xffef, 0xfff0, 0xfff1, 0xfff2, 0xfff3, 0xfff4, 0xfff5, 0x03fa,
    0x7fc3, 0xfff6, 0xfff7, 0xfff8, 0xfff9, 0xfffa, 0xfffb, 0xfffc,
    0xfffd, 0xfffe
};

/**
 * Calculated Cos table For Standard DCT.
 */
#if (NRF_FPU_USED == 1)
static const double sqrt2 = 0.7071067811865475;
static const double cos_tbl[BLOCK_SIZE][BLOCK_SIZE] = {
    {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
    {0.9807852804032304, 0.8314696123025452, 0.5555702330196023, 0.19509032201612833, -0.1950903220161282, -0.555570233019602, -0.8314696123025453, -0.9807852804032304},
    {0.9238795325112867, 0.38268343236508984, -0.3826834323650897, -0.9238795325112867, -0.9238795325112868, -0.38268343236509034, 0.38268343236509, 0.9238795325112865},
    {0.8314696123025452, -0.1950903220161282, -0.9807852804032304, -0.5555702330196022, 0.5555702330196018, 0.9807852804032304, 0.19509032201612878, -0.8314696123025451},
    {0.7071067811865476, -0.7071067811865475, -0.7071067811865477, 0.7071067811865474, 0.7071067811865477, -0.7071067811865467, -0.7071067811865471, 0.7071067811865466},
    {0.5555702330196023, -0.9807852804032304,  0.1950903220161283, 0.8314696123025456, -0.8314696123025451, -0.19509032201612803, 0.9807852804032307, -0.5555702330196015},
    {0.38268343236508984, -0.9238795325112868, 0.9238795325112865, -0.3826834323650899, -0.38268343236509056,0.9238795325112867, -0.9238795325112864, 0.38268343236508956},
    {0.19509032201612833, -0.5555702330196022, 0.8314696123025456, -0.9807852804032307, 0.9807852804032304, -0.831469612302545, 0.5555702330196015, -0.19509032201612858}
};
#else
//#define INTIZATION_RATE 1024
//static const int sqrt2 = (int)(0.7071067811865475*INTIZATION_RATE);
//static const int cos_tbl[BLOCK_SIZE][BLOCK_SIZE] = {
//    {(int)(1.0*INTIZATION_RATE), (int)(1.0*INTIZATION_RATE), (int)(1.0*INTIZATION_RATE), (int)(1.0*INTIZATION_RATE), (int)(1.0*INTIZATION_RATE), (int)(1.0*INTIZATION_RATE), (int)(1.0*INTIZATION_RATE), (int)(1.0*INTIZATION_RATE)},
//    {(int)(0.9807852804032304*INTIZATION_RATE), (int)(0.8314696123025452*INTIZATION_RATE), (int)(0.5555702330196023*INTIZATION_RATE), (int)(0.19509032201612833*INTIZATION_RATE), (int)(-0.1950903220161282*INTIZATION_RATE), (int)(-0.555570233019602*INTIZATION_RATE), (int)(-0.8314696123025453*INTIZATION_RATE), (int)(-0.9807852804032304*INTIZATION_RATE)},
//    {(int)(0.9238795325112867*INTIZATION_RATE), (int)(0.38268343236508984*INTIZATION_RATE), (int)(-0.3826834323650897*INTIZATION_RATE), (int)(-0.9238795325112867*INTIZATION_RATE), (int)(-0.9238795325112868*INTIZATION_RATE), (int)(-0.38268343236509034*INTIZATION_RATE), (int)(0.38268343236509*INTIZATION_RATE), (int)(0.9238795325112865*INTIZATION_RATE)},
//    {(int)(0.8314696123025452*INTIZATION_RATE), (int)(-0.1950903220161282*INTIZATION_RATE), (int)(-0.9807852804032304*INTIZATION_RATE), (int)(-0.5555702330196022*INTIZATION_RATE), (int)(0.5555702330196018*INTIZATION_RATE), (int)(0.9807852804032304*INTIZATION_RATE), (int)(0.19509032201612878*INTIZATION_RATE), (int)(-0.8314696123025451*INTIZATION_RATE)},
//    {(int)(0.7071067811865476*INTIZATION_RATE), (int)(-0.7071067811865475*INTIZATION_RATE), (int)(-0.7071067811865477*INTIZATION_RATE), (int)(0.7071067811865474*INTIZATION_RATE), (int)(0.7071067811865477*INTIZATION_RATE), (int)(-0.7071067811865467*INTIZATION_RATE), (int)(-0.7071067811865471*INTIZATION_RATE), (int)(0.7071067811865466*INTIZATION_RATE)},
//    {(int)(0.5555702330196023*INTIZATION_RATE), (int)(-0.9807852804032304*INTIZATION_RATE),  (int)(0.1950903220161283*INTIZATION_RATE), (int)(0.8314696123025456*INTIZATION_RATE), (int)(-0.8314696123025451*INTIZATION_RATE), (int)(-0.19509032201612803*INTIZATION_RATE), (int)(0.9807852804032307*INTIZATION_RATE), (int)(-0.5555702330196015*INTIZATION_RATE)},
//    {(int)(0.38268343236508984*INTIZATION_RATE), (int)(-0.9238795325112868*INTIZATION_RATE), (int)(0.9238795325112865*INTIZATION_RATE), (int)(-0.3826834323650899*INTIZATION_RATE), (int)(-0.38268343236509056*INTIZATION_RATE), (int)(0.9238795325112867*INTIZATION_RATE), (int)(-0.9238795325112864*INTIZATION_RATE), (int)(0.38268343236508956*INTIZATION_RATE)},
//    {(int)(0.19509032201612833*INTIZATION_RATE), (int)(-0.5555702330196022*INTIZATION_RATE), (int)(0.8314696123025456*INTIZATION_RATE), (int)(-0.9807852804032307*INTIZATION_RATE), (int)(0.9807852804032304*INTIZATION_RATE), (int)(-0.831469612302545*INTIZATION_RATE), (int)(0.5555702330196015*INTIZATION_RATE), (int)(-0.19509032201612858*INTIZATION_RATE)}
//};
//#endif
#define INTIZATION_RATE 64
static const int8_t sqrt2 = (int8_t)(0.7071067811865475*INTIZATION_RATE);
static const int8_t cos_tbl[BLOCK_SIZE][BLOCK_SIZE] = {
    {(int8_t)(1.0*INTIZATION_RATE), (int8_t)(1.0*INTIZATION_RATE), (int8_t)(1.0*INTIZATION_RATE), (int8_t)(1.0*INTIZATION_RATE), (int8_t)(1.0*INTIZATION_RATE), (int8_t)(1.0*INTIZATION_RATE), (int8_t)(1.0*INTIZATION_RATE), (int8_t)(1.0*INTIZATION_RATE)},
    {(int8_t)(0.9807852804032304*INTIZATION_RATE), (int8_t)(0.8314696123025452*INTIZATION_RATE), (int8_t)(0.5555702330196023*INTIZATION_RATE), (int8_t)(0.19509032201612833*INTIZATION_RATE), (int8_t)(-0.1950903220161282*INTIZATION_RATE), (int8_t)(-0.555570233019602*INTIZATION_RATE), (int8_t)(-0.8314696123025453*INTIZATION_RATE), (int8_t)(-0.9807852804032304*INTIZATION_RATE)},
    {(int8_t)(0.9238795325112867*INTIZATION_RATE), (int8_t)(0.38268343236508984*INTIZATION_RATE), (int8_t)(-0.3826834323650897*INTIZATION_RATE), (int8_t)(-0.9238795325112867*INTIZATION_RATE), (int8_t)(-0.9238795325112868*INTIZATION_RATE), (int8_t)(-0.38268343236509034*INTIZATION_RATE), (int8_t)(0.38268343236509*INTIZATION_RATE), (int8_t)(0.9238795325112865*INTIZATION_RATE)},
    {(int8_t)(0.8314696123025452*INTIZATION_RATE), (int8_t)(-0.1950903220161282*INTIZATION_RATE), (int8_t)(-0.9807852804032304*INTIZATION_RATE), (int8_t)(-0.5555702330196022*INTIZATION_RATE), (int8_t)(0.5555702330196018*INTIZATION_RATE), (int8_t)(0.9807852804032304*INTIZATION_RATE), (int8_t)(0.19509032201612878*INTIZATION_RATE), (int8_t)(-0.8314696123025451*INTIZATION_RATE)},
    {(int8_t)(0.7071067811865476*INTIZATION_RATE), (int8_t)(-0.7071067811865475*INTIZATION_RATE), (int8_t)(-0.7071067811865477*INTIZATION_RATE), (int8_t)(0.7071067811865474*INTIZATION_RATE), (int8_t)(0.7071067811865477*INTIZATION_RATE), (int8_t)(-0.7071067811865467*INTIZATION_RATE), (int8_t)(-0.7071067811865471*INTIZATION_RATE), (int8_t)(0.7071067811865466*INTIZATION_RATE)},
    {(int8_t)(0.5555702330196023*INTIZATION_RATE), (int8_t)(-0.9807852804032304*INTIZATION_RATE),  (int8_t)(0.1950903220161283*INTIZATION_RATE), (int8_t)(0.8314696123025456*INTIZATION_RATE), (int8_t)(-0.8314696123025451*INTIZATION_RATE), (int8_t)(-0.19509032201612803*INTIZATION_RATE), (int8_t)(0.9807852804032307*INTIZATION_RATE), (int8_t)(-0.5555702330196015*INTIZATION_RATE)},
    {(int8_t)(0.38268343236508984*INTIZATION_RATE), (int8_t)(-0.9238795325112868*INTIZATION_RATE), (int8_t)(0.9238795325112865*INTIZATION_RATE), (int8_t)(-0.3826834323650899*INTIZATION_RATE), (int8_t)(-0.38268343236509056*INTIZATION_RATE), (int8_t)(0.9238795325112867*INTIZATION_RATE), (int8_t)(-0.9238795325112864*INTIZATION_RATE), (int8_t)(0.38268343236508956*INTIZATION_RATE)},
    {(int8_t)(0.19509032201612833*INTIZATION_RATE), (int8_t)(-0.5555702330196022*INTIZATION_RATE), (int8_t)(0.8314696123025456*INTIZATION_RATE), (int8_t)(-0.9807852804032307*INTIZATION_RATE), (int8_t)(0.9807852804032304*INTIZATION_RATE), (int8_t)(-0.831469612302545*INTIZATION_RATE), (int8_t)(0.5555702330196015*INTIZATION_RATE), (int8_t)(-0.19509032201612858*INTIZATION_RATE)}
};
#endif




//#define INTIZATION_RATE 1024
//static const int sqrt2 = (int)(0.7071067811865475*INTIZATION_RATE);
//static const int cos_tbl[BLOCK_SIZE][BLOCK_SIZE] = {
//    {(int)(1.0*INTIZATION_RATE), (int)(1.0*INTIZATION_RATE), (int)(1.0*INTIZATION_RATE), (int)(1.0*INTIZATION_RATE), (int)(1.0*INTIZATION_RATE), (int)(1.0*INTIZATION_RATE), (int)(1.0*INTIZATION_RATE), (int)(1.0*INTIZATION_RATE)},
//    {(int)(0.9807852804032304*INTIZATION_RATE), (int)(0.8314696123025452*INTIZATION_RATE), (int)(0.5555702330196023*INTIZATION_RATE), (int)(0.19509032201612833*INTIZATION_RATE), (int)(-0.1950903220161282*INTIZATION_RATE), (int)(-0.555570233019602*INTIZATION_RATE), (int)(-0.8314696123025453*INTIZATION_RATE), (int)(-0.9807852804032304*INTIZATION_RATE)},
//    {(int)(0.9238795325112867*INTIZATION_RATE), (int)(0.38268343236508984*INTIZATION_RATE), (int)(-0.3826834323650897*INTIZATION_RATE), (int)(-0.9238795325112867*INTIZATION_RATE), (int)(-0.9238795325112868*INTIZATION_RATE), (int)(-0.38268343236509034*INTIZATION_RATE), (int)(0.38268343236509*INTIZATION_RATE), (int)(0.9238795325112865*INTIZATION_RATE)},
//    {(int)(0.8314696123025452*INTIZATION_RATE), (int)(-0.1950903220161282*INTIZATION_RATE), (int)(-0.9807852804032304*INTIZATION_RATE), (int)(-0.5555702330196022*INTIZATION_RATE), (int)(0.5555702330196018*INTIZATION_RATE), (int)(0.9807852804032304*INTIZATION_RATE), (int)(0.19509032201612878*INTIZATION_RATE), (int)(-0.8314696123025451*INTIZATION_RATE)},
//    {(int)(0.7071067811865476*INTIZATION_RATE), (int)(-0.7071067811865475*INTIZATION_RATE), (int)(-0.7071067811865477*INTIZATION_RATE), (int)(0.7071067811865474*INTIZATION_RATE), (int)(0.7071067811865477*INTIZATION_RATE), (int)(-0.7071067811865467*INTIZATION_RATE), (int)(-0.7071067811865471*INTIZATION_RATE), (int)(0.7071067811865466*INTIZATION_RATE)},
//    {(int)(0.5555702330196023*INTIZATION_RATE), (int)(-0.9807852804032304*INTIZATION_RATE),  (int)(0.1950903220161283*INTIZATION_RATE), (int)(0.8314696123025456*INTIZATION_RATE), (int)(-0.8314696123025451*INTIZATION_RATE), (int)(-0.19509032201612803*INTIZATION_RATE), (int)(0.9807852804032307*INTIZATION_RATE), (int)(-0.5555702330196015*INTIZATION_RATE)},
//    {(int)(0.38268343236508984*INTIZATION_RATE), (int)(-0.9238795325112868*INTIZATION_RATE), (int)(0.9238795325112865*INTIZATION_RATE), (int)(-0.3826834323650899*INTIZATION_RATE), (int)(-0.38268343236509056*INTIZATION_RATE), (int)(0.9238795325112867*INTIZATION_RATE), (int)(-0.9238795325112864*INTIZATION_RATE), (int)(0.38268343236508956*INTIZATION_RATE)},
//    {(int)(0.19509032201612833*INTIZATION_RATE), (int)(-0.5555702330196022*INTIZATION_RATE), (int)(0.8314696123025456*INTIZATION_RATE), (int)(-0.9807852804032307*INTIZATION_RATE), (int)(0.9807852804032304*INTIZATION_RATE), (int)(-0.831469612302545*INTIZATION_RATE), (int)(0.5555702330196015*INTIZATION_RATE), (int)(-0.19509032201612858*INTIZATION_RATE)}
//};

typedef enum block_mode {
    Y,
    Cr,
    Cb
} block_mode;

typedef struct quant_tbl {
    const uint8_t *luminance_quant_tbl;
    const uint8_t *chrominance_quant_tbl;
} quant_tbl;
static const quant_tbl quant_tbl_0 = {std_luminance_quant_tbl_0, std_chrominance_quant_tbl_0};
static const quant_tbl quant_tbl_10 = {std_luminance_quant_tbl_10, std_chrominance_quant_tbl_10};
static const quant_tbl quant_tbl_50 = {std_luminance_quant_tbl_50, std_chrominance_quant_tbl_50};
static const quant_tbl quant_tbl_90 = {std_luminance_quant_tbl_90, std_chrominance_quant_tbl_90};
static const quant_tbl quant_tbl_95 = {std_luminance_quant_tbl_95, std_chrominance_quant_tbl_95};
static const quant_tbl quant_tbl_100 = {std_luminance_quant_tbl_100, std_chrominance_quant_tbl_100};
static const quant_tbl* get_quality_tbl(quality_rate quality) {
    switch(quality) {
        case lowest:
            return &quant_tbl_0;
        case lower:
            return &quant_tbl_10;
        case low:
            return &quant_tbl_50;
        case higher:
            return &quant_tbl_95;
        case highest:
            return &quant_tbl_100;
        case normal:
        default:
            return &quant_tbl_90;
    }    
}


/* Prototype */
static int split_MCU(jpeg_data *data, const uint8_t *mcu);
static int encode_block_mode(jpeg_data *data, const uint8_t *block, const block_mode mode);
static int dct(int *block, int *temp);
static int quantization(int *block, const block_mode mode, const quality_rate quality);
static int coding_huff(jpeg_data *data, const int *block, const int pre_DC,
                    const uint8_t *dc_huff_len_tbl, const int16_t *dc_huff_code_tbl, 
                    const uint8_t *ac_huff_len_tbl, const int16_t *ac_huff_code_tbl);

static void put_bits(jpeg_data *data, const uint_t nbits, const uint16_t val);
static void put_byte(jpeg_data *data, const uint8_t val);
static void flush_bits(jpeg_data *data);
static int init_data(jpeg_data *data);


/**
 * Generate JPEG file's header.
 * Required "Width" and "Height", "Color type", "Quality".
 * Header data in "ret_data[]".
 *
 * @param data JPEG Structure
 * @return Header Length. Under 0 is error.
 */
int generate_header(jpeg_data *data) {
    uint8_t *p;
    const uint8_t *qt;
    int cnt = 0;
    const quant_tbl *q_tbl;
    color_type color;
    
    init_data(data);
    
    p = data->ret_data;
    color = data->c_info.color;
     
    
    // JFIF header
    {
        // SOI
        p[cnt++]=0xFF;p[cnt++]=0xD8;

        // APP0(JFIF)
        p[cnt++]=0xFF;p[cnt++]=0xE0;//APP0
        p[cnt++]=0x00;p[cnt++]=0x10;//La(Length) = 16 bytes
        p[cnt++]=0x4A;p[cnt++]=0x46;p[cnt++]=0x49;p[cnt++]=0x46;p[cnt++]=0x00;//"JFIF\0"
        p[cnt++]=0x01;p[cnt++]=0x01;//Version = 1.01
        p[cnt++]=0x01;//dots/inch(dpi)
        p[cnt++]=0x00;p[cnt++]=0x48;p[cnt++]=0x00;p[cnt++]=0x48;//72*72dpi
        p[cnt++]=0x00;p[cnt++]=0x00;//No thumbnail
    }

    // DQT
    {
        p[cnt++]=0xFF;p[cnt++]=0xDB;//DQT
        switch(color) {
            case gray_scale:
                p[cnt++]=0x00;p[cnt++]=0x43;//Lq(Length) = 2+65 bytes
                //ID:0, Quantization table For luminance
                p[cnt++]=0x00;
                q_tbl = get_quality_tbl(data->quality);
                qt = q_tbl->luminance_quant_tbl;
                memcpy(&p[cnt], qt, QUANT_SIZE);
                cnt += QUANT_SIZE;
                break;
            default:
                p[cnt++]=0x00;p[cnt++]=0x84;//Lq(Length) = 2+65+65 bytes
                //ID:0, Quantization table For luminance
                p[cnt++]=0x00;
                q_tbl = get_quality_tbl(data->quality);
                qt = q_tbl->luminance_quant_tbl;
                memcpy(&p[cnt], qt, QUANT_SIZE);
                cnt += QUANT_SIZE;
                //ID:1, Quantization table For chrominance
                p[cnt++]=0x01;
                qt = q_tbl->chrominance_quant_tbl;
                memcpy(&p[cnt], qt, QUANT_SIZE);
                cnt += QUANT_SIZE;
                break;
        }
    }

    // DHT
    {
        p[cnt++]=0xFF;p[cnt++]=0xC4;//DHT
        switch(color) {
            case gray_scale:
                p[cnt++]=0x00;p[cnt++]=0xD2;//Lh(Length) = 2+29+179 bytes
                //ID:01, Huffman code table For DC luminance.
                p[cnt++]=0x00;
                memcpy(&p[cnt], std_dc_luminance_huff_val_tbl, sizeof(std_dc_luminance_huff_val_tbl));
                cnt += sizeof(std_dc_luminance_huff_val_tbl);
                //ID:10, Huffman code table For AC luminance.
                p[cnt++]=0x10;
                memcpy(&p[cnt], std_ac_luminance_huff_val_tbl, sizeof(std_ac_luminance_huff_val_tbl));
                cnt += sizeof(std_ac_luminance_huff_val_tbl);
                break;
            default:
                p[cnt++]=0x01;p[cnt++]=0xA2;//Lh(Length) = 2+29+179+29+179 bytes
                //ID:01, Huffman code table For DC luminance.
                p[cnt++]=0x00;
                memcpy(&p[cnt], std_dc_luminance_huff_val_tbl, sizeof(std_dc_luminance_huff_val_tbl));
                cnt += sizeof(std_dc_luminance_huff_val_tbl);
                //ID:10, Huffman code table For AC luminance.
                p[cnt++]=0x10;
                memcpy(&p[cnt], std_ac_luminance_huff_val_tbl, sizeof(std_ac_luminance_huff_val_tbl));
                cnt += sizeof(std_ac_luminance_huff_val_tbl);
                //ID:01, Huffman code table For DC chrominance.
                p[cnt++]=0x01;
                memcpy(&p[cnt], std_dc_chrominance_huff_val_tbl, sizeof(std_dc_chrominance_huff_val_tbl));
                cnt += sizeof(std_dc_chrominance_huff_val_tbl);
                //ID:11, Huffman code table For AC chrominance.
                p[cnt++]=0x11;
                memcpy(&p[cnt], std_ac_chrominance_huff_val_tbl, sizeof(std_ac_chrominance_huff_val_tbl));
                cnt += sizeof(std_ac_chrominance_huff_val_tbl);
                break;
        }
    }

    //SOF0
    {
        p[cnt++]=0xFF;p[cnt++]=0xC0;//SOF0
        switch(color) {
            case gray_scale:
                p[cnt++]=0x00;p[cnt++]=0x0b;//Lf(Length) = 11 bytes
                p[cnt++]=0x08;//Sampling accuracy = 8 bits
                p[cnt++]=(uint8_t)(data->height >> 8);p[cnt++]=(uint8_t)(data->height & 0xff);//Height
                p[cnt++]=(uint8_t)(data->width >> 8);p[cnt++]=(uint8_t)(data->width & 0xff);//Width
                p[cnt++]=0x01;//Number of image components = 1(Grayscale)
                p[cnt++]=0x00;p[cnt++]=0x22;p[cnt++]=0x00;//Parameter of Specified luminance component(Sampling coefficient=2/2, Quantization table=0)
                break;
            default:
                p[cnt++]=0x00;p[cnt++]=0x11;//Lf(Length) = 17 bytes
                p[cnt++]=0x08;//Sampling accuracy = 8 bits
                p[cnt++]=(uint8_t)(data->height >> 8);p[cnt++]=(uint8_t)(data->height & 0xff);//Height
                p[cnt++]=(uint8_t)(data->width >> 8);p[cnt++]=(uint8_t)(data->width & 0xff);//Width
                p[cnt++]=0x03;//Number of image components = 3(Full Color)
                p[cnt++]=0x00;p[cnt++]=0x22;p[cnt++]=0x00;//Parameter of Specified luminance component(Sampling coefficient=2/2, Quantization table=0)
                p[cnt++]=0x01;p[cnt++]=0x11;p[cnt++]=0x01;//Parameter of Specified chrominance component(Sampling coefficient=1/1, Quantization table=1)
                p[cnt++]=0x02;p[cnt++]=0x11;p[cnt++]=0x01;//Parameter of Specified chrominance component(Sampling coefficient=1/1, Quantization table=1)
                break;
        }
    }

    // SOS
    {
        p[cnt++]=0xFF;p[cnt++]=0xDA;//SOS
        switch(color) {
            case gray_scale:
                p[cnt++]=0x00;p[cnt++]=0x08;//Ls(Length) = 8bytes
                p[cnt++]=0x01;//Number of image components = 1(Grayscale)
                p[cnt++]=0x00;p[cnt++]=0x00;//Parameter of Specified luminance component
                break;
            default:
                p[cnt++]=0x00;p[cnt++]=0x0c;//Ls(Length) = 12bytes
                p[cnt++]=0x03;//Number of image components = 3(Full Color)
                p[cnt++]=0x00;p[cnt++]=0x00;//Parameter of Specified luminance component
                p[cnt++]=0x01;p[cnt++]=0x11;//Parameter of Specified chrominance component
                p[cnt++]=0x02;p[cnt++]=0x11;//Parameter of Specified chrominance component
                break;
        }
        p[cnt++]=0x00;p[cnt++]=0x3F;p[cnt++]=0x00;//Reserved
    }
    data->data_len = cnt;
    
    return cnt;
}

/**
 * Generate JPEG file's footer.
 * Footer data in "ret_data[]".
 *
 * @param data JPEG Structure
 * @return Footer Length. Under 0 is error.
 */
int generateFooter(jpeg_data *data) {
    uint8_t *p;
    int cnt = 0;
    
    init_data(data);
    
    p = data->ret_data;
    
    // Flush left data.
    data->data_len = cnt;
    flush_bits(data);
    cnt = data->data_len;
    
    // EOI
    p[cnt++]=0xFF;
    p[cnt++]=0xD9;
    
    data->data_len = cnt;
    
    return cnt;
}

/**
 * Encoding image data by MCU unit.
 * Supported Image format is "8bit Grayscale" and "YUV422(CbYCrY format)".
 * Encoded data in "ret_data[]".
 *
 * @param data JPEG Structure
 * @param mcu MCU or Grayscale-Block
 * @return Encoded data Length. Under 0 is error.
 */
int encode_MCU(jpeg_data *data, const uint8_t *mcu) {
    int ret = 0;
    init_data(data);

//    switch(data->c_info.color) {
//        case gray_scale:
            ret = encode_block_mode(data, mcu, Y);
//            break;
//        case YUV411:
//        default:
//            ret = split_MCU(data, mcu);
//            break;
//    }
    
    return ret;
}

/**
 * Reading Image data & Writing Encoded data by each MCU.
 * Supported Image format is "8bit Grayscale" and "YUV422(CbYCrY format)" 1-dimensional array.
 *
 * @param data JPEG Structure
 * @param read_data_func Reading Image data function
 * @param write_data_func Writing Ecoding data Function
 * @return JPEG file size. Under 0 is error.
 */
int encode_image(jpeg_data *data, int (*read_data_func)(uint_t pos, uint8_t *data, uint_t len), int (*write_data_func)(uint8_t *data, uint_t len)) {

	int mcu_size = data->c_info.mcu_size;
	int width =  data->width;
	int height = data->height;
	int pixel_size_by_byte = data->c_info.pixel_size_by_byte;
        //CAUTION!!!!!!!!!! MUST initializing jpeg_data structure!
        if(data->mcu_width_max == 0) {
            data->mcu_width_max = (int)ceil(width / (float)mcu_size);
            data->mcu_height_max = (int)ceil(height / (float)mcu_size);
        }
	int mcu_width_max = data->mcu_width_max;
	int mcu_height_max = data->mcu_height_max;
	uint8_t *mcu = data->work.mcu;
	uint8_t prePixel_y = 0;
	uint8_t prePixel_Crb = 0;
	int cnt = 0;

	generate_header(data);
        #if (RELEASE_CODE ==0)
            write_data_func(data->ret_data, data->data_len);
        #endif
        write_data2(data->ret_data, data->data_len, cnt);
	cnt += data->data_len;

	//MUC by CbYCr format & Grayscale.
	for(int cur_mcu_height = 0; cur_mcu_height < mcu_height_max; cur_mcu_height++) {
	  for(int cur_mcu_width = 0; cur_mcu_width < mcu_width_max; cur_mcu_width++) {
		  for(int cur_height = 0; cur_height < mcu_size; cur_height++) {
			  if((cur_mcu_height*mcu_size + cur_height) >= height) {
				  for(int i = 0; i < mcu_size*pixel_size_by_byte; i++) {
//					  if(i%2 == 0) {
//						  mcu[cur_height*mcu_size*pixel_size_by_byte + i] = prePixel_Crb;
//					  } else {
						  mcu[cur_height*mcu_size*pixel_size_by_byte + i] = prePixel_y;
//					  }
				  }
			  } else {
				  for(int cur_width = 0; cur_width < mcu_size*pixel_size_by_byte; cur_width++) {
					  if(cur_mcu_width*mcu_size*pixel_size_by_byte + cur_width >= width*pixel_size_by_byte) {
//						  if(cur_width%2 == 0) {
//							  mcu[cur_height*mcu_size*pixel_size_by_byte + cur_width] = prePixel_Crb;
//						  } else {
							  mcu[cur_height*mcu_size*pixel_size_by_byte + cur_width] = prePixel_y;
//						  }
					  } else {
						  //read_data_func((cur_mcu_height*mcu_size + cur_height)*width*pixel_size_by_byte + (cur_mcu_width*mcu_size*pixel_size_by_byte + cur_width), &mcu[cur_height*mcu_size*pixel_size_by_byte + cur_width],1);
						  /*Added by Ali*/
                                                  mcu[cur_height*mcu_size*pixel_size_by_byte + cur_width]=m_rx_buf[(cur_mcu_height*mcu_size + cur_height)*width*pixel_size_by_byte + (cur_mcu_width*mcu_size*pixel_size_by_byte + cur_width)];
//                                                  uint8_t printx1 = m_rx_buf[(cur_mcu_height*mcu_size + cur_height)*width*pixel_size_by_byte + (cur_mcu_width*mcu_size*pixel_size_by_byte + cur_width)];
//                                                  if(printx1 != 0x00){
//                                                      //printf("printed: %x \n", printx1);
//                                                      asm("nop");
//                                                  }
//                                                  if(cur_width%2 == 0) {
//							  prePixel_Crb =  mcu[cur_height*mcu_size*pixel_size_by_byte + cur_width];
//						  } else {
							  prePixel_y =  mcu[cur_height*mcu_size*pixel_size_by_byte + cur_width];
//						  }
					  }
				  }
			  }
		  }
		  encode_MCU(data, mcu);
                  #if (RELEASE_CODE ==0)
                      write_data_func(data->ret_data, data->data_len);
                  #endif
                  write_data2(data->ret_data, data->data_len, cnt);
		  cnt += data->data_len;

	  }
	}

	generateFooter(data);
        #if (RELEASE_CODE ==0)
            write_data_func(data->ret_data, data->data_len);
        #endif
        write_data2(data->ret_data, data->data_len, cnt);
	cnt += data->data_len;

	return cnt;
}


/* private func */

/**
 * Split MCU to {Y|Cr|Cb}Blocks.
 *
 * @param data JPEG Structure
 * @param mcu MCU
 * @return Encoded data Length. Under 0 is error.
 */
static int split_MCU(jpeg_data *data, const uint8_t *mcu) {
    int ret = 0;
    int step = data->c_info.pixel_size_by_byte;
    uint8_t *block_y = data->work.block_y;
    uint8_t *block_Cr = data->work.block_Cr;
    uint8_t *block_Cb = data->work.block_Cb;
    
    // Y
    for(int y = 0; y < 2; y++) {
        for(int x = 0; x < 2; x++) {
            for(int cur_block_height = 0; cur_block_height < BLOCK_SIZE; cur_block_height++) {
                for(int cur_block_width = 0; cur_block_width < BLOCK_SIZE; cur_block_width++) {
                    block_y[cur_block_height*BLOCK_SIZE + cur_block_width] = mcu[((cur_block_height+(y*BLOCK_SIZE))*MCU_SIZE_COLOR*step) + (cur_block_width*step+(x*MCU_SIZE_COLOR))+1];
                }
            }
            ret = encode_block_mode(data, block_y, Y);
        }
    }
    
    // Cb, Cr
    for(int cur_block_height = 0; cur_block_height < BLOCK_SIZE; cur_block_height++) {
        for(int cur_block_width = 0; cur_block_width < BLOCK_SIZE; cur_block_width++) {
            block_Cb[cur_block_height*BLOCK_SIZE + cur_block_width] = mcu[(cur_block_height*MCU_SIZE_COLOR*2*step) + (cur_block_width*2*step)];
            block_Cr[cur_block_height*BLOCK_SIZE + cur_block_width] = mcu[(cur_block_height*MCU_SIZE_COLOR*2*step) + (cur_block_width*2*step) +2];
        }
    }
    ret = encode_block_mode(data, block_Cb, Cb);
    ret = encode_block_mode(data, block_Cr, Cr);
    return ret;
}

/**
 * Encoding image data by {Y|Cr|Cb}Block unit.
 * Encoded data in "ret_data[]".
 *
 * @param data JPEG Structure
 * @param block {Y|Cr|Cb}Block
 * @param mode YUV(Y,Cr,Cb) Type
 * @return Encoded data Length. Under 0 is error.
 */
#if (JPEG_16B == 1) 
static int encode_block_mode(jpeg_data *data, const uint8_t *block, const block_mode mode) {
    int *block_int;
    int ret = 0;
    block_int = data->work.block_int;

    for(int i = 0; i < BLOCK_SIZE*BLOCK_SIZE; i++) {
        block_int[i] = block[i]&0xff;
        block_int[i] -= 128;
    }
    
    dct(block_int, data->work.temp);
    quantization(block_int, mode, data->quality);
    
//    switch(mode) {
//        case Cr:
//            coding_huff(data, block_int, data->pre_DC_Cr, std_dc_chrominance_huff_len_tbl, std_dc_chrominance_huff_code_tbl, std_ac_chrominance_huff_len_tbl, std_ac_chrominance_huff_code_tbl);
//            data->pre_DC_Cr = block_int[0];
//            break;
//        case Cb:
//            coding_huff(data, block_int, data->pre_DC_Cb, std_dc_chrominance_huff_len_tbl, std_dc_chrominance_huff_code_tbl, std_ac_chrominance_huff_len_tbl, std_ac_chrominance_huff_code_tbl);
//            data->pre_DC_Cb = block_int[0];
//            break;
//        case Y:
//        default:
            coding_huff(data, block_int, data->pre_DC_Y, std_dc_luminance_huff_len_tbl, std_dc_luminance_huff_code_tbl, std_ac_luminance_huff_len_tbl, std_ac_luminance_huff_code_tbl);
            data->pre_DC_Y = block_int[0];
//            break;
    //}
    ret = data->data_len;
    return ret;
}
#else
static int encode_block_mode(jpeg_data *data, const uint8_t *block, const block_mode mode) {
    int8_t *block_int;
    int ret = 0;
    block_int = data->work.block_int;

    for(int i = 0; i < BLOCK_SIZE*BLOCK_SIZE; i++) {
        block_int[i] = block[i]&0xff;
        block_int[i] -= 64;
    }
    
    dct(block_int, data->work.temp);
    quantization(block_int, mode, data->quality);
    
//    switch(mode) {
//        case Cr:
//            coding_huff(data, block_int, data->pre_DC_Cr, std_dc_chrominance_huff_len_tbl, std_dc_chrominance_huff_code_tbl, std_ac_chrominance_huff_len_tbl, std_ac_chrominance_huff_code_tbl);
//            data->pre_DC_Cr = block_int[0];
//            break;
//        case Cb:
//            coding_huff(data, block_int, data->pre_DC_Cb, std_dc_chrominance_huff_len_tbl, std_dc_chrominance_huff_code_tbl, std_ac_chrominance_huff_len_tbl, std_ac_chrominance_huff_code_tbl);
//            data->pre_DC_Cb = block_int[0];
//            break;
//        case Y:
//        default:
            coding_huff(data, block_int, data->pre_DC_Y, std_dc_luminance_huff_len_tbl, std_dc_luminance_huff_code_tbl, std_ac_luminance_huff_len_tbl, std_ac_luminance_huff_code_tbl);
            data->pre_DC_Y = block_int[0];
//            break;
    //}
    ret = data->data_len;
    return ret;
}
#endif




/**
 * DCT function.
 * 
 * @param block {Y|Cr|Cb}Block
 * @param work Temporary Array
 * @return success(1) or error(Under 0)
 */
#if (JPEG_16B == 1) 
static int dct(int *block, int *work){
	int ret = 1;
        int v = 0;
        int cv = 0;
        int u = 0;
        int cu = 0;
        int sum = 0;
        int tmp = 0;
        int y = 0; 
        int x = 0;
        int i = 0;
//	int8_t ret = 1;
//        int8_t v = 0;
//        int8_t cv = 0;
//        int8_t u = 0;
//        int8_t cu = 0;
//        int8_t sum = 0;
//        int8_t tmp = 0;
//        int8_t y = 0; 
//        int8_t x = 0;
//        int8_t i = 0;
	//DCT
    for(v = 0; v < BLOCK_SIZE; v++){
#if (NRF_FPU_USED == 1)
        double cv = (v == 0) ? sqrt2 : 1.0;
#else
        cv = (v == 0) ? sqrt2 : INTIZATION_RATE;
#endif
        for(u = 0; u < BLOCK_SIZE; u++){
#if (NRF_FPU_USED == 1)
            double cu = (u == 0) ? sqrt2 : 1.0;
            double sum = 0.0;
#else
            cu = (u == 0) ? sqrt2 : INTIZATION_RATE;
            sum = 0;
            tmp = 0;
#endif

            for( y = 0; y < BLOCK_SIZE;y++) {
                for( x = 0; x < BLOCK_SIZE; x++) {
#if (NRF_FPU_USED == 1)
                    sum += block[y * BLOCK_SIZE + x] * cos_tbl[u][x] * cos_tbl[v][y];
#else
                    tmp = block[y * BLOCK_SIZE + x] * cos_tbl[u][x] / INTIZATION_RATE;
                    tmp = tmp * cos_tbl[v][y] / INTIZATION_RATE;
                    sum += tmp;
#endif
                }
            }
#if (NRF_FPU_USED == 1)
            work[v * BLOCK_SIZE + u] = (int)(sum * cu * cv / 4);
#else
            tmp = sum * cu / 4 / INTIZATION_RATE;
            work[v * BLOCK_SIZE + u] = tmp * cv / INTIZATION_RATE;
#endif
        }
    }
    for( i = 0; i < BLOCK_SIZE*BLOCK_SIZE; i++) {
        block[i] = work[zigzag_tbl[i]];
    }
    return ret;
}
#else
static int8_t dct(int8_t *block, int8_t *work){
	int8_t ret = 1;

	//DCT
    for(int8_t v = 0; v < BLOCK_SIZE; v++){
#if (NRF_FPU_USED == 1)
        double cv = (v == 0) ? sqrt2 : 1.0;
#else
        int8_t cv = (v == 0) ? sqrt2 : INTIZATION_RATE;
#endif
        for(int8_t u = 0; u < BLOCK_SIZE; u++){
#if (NRF_FPU_USED == 1)
            double cu = (u == 0) ? sqrt2 : 1.0;
            double sum = 0.0;
#else
            int8_t cu = (u == 0) ? sqrt2 : INTIZATION_RATE;
            int8_t sum = 0;
            int8_t tmp = 0;
#endif

            for(int8_t y = 0; y < BLOCK_SIZE;y++) {
                for(int8_t x = 0; x < BLOCK_SIZE; x++) {
#if (NRF_FPU_USED == 1)
                    sum += block[y * BLOCK_SIZE + x] * cos_tbl[u][x] * cos_tbl[v][y];
#else
                    tmp = block[y * BLOCK_SIZE + x] * cos_tbl[u][x] / INTIZATION_RATE;
                    tmp = tmp * cos_tbl[v][y] / INTIZATION_RATE;
                    sum += tmp;
#endif
                }
            }
#if (NRF_FPU_USED == 1)
            work[v * BLOCK_SIZE + u] = (int)(sum * cu * cv / 4);
#else
            tmp = sum * cu / 4 / INTIZATION_RATE;
            work[v * BLOCK_SIZE + u] = tmp * cv / INTIZATION_RATE;
#endif
        }
    }
    for(int8_t i = 0; i < BLOCK_SIZE*BLOCK_SIZE; i++) {
        block[i] = work[zigzag_tbl[i]];
    }
    return ret;
}
#endif







//static int dct(int *block, int *work){
//	int ret = 1;
//
//	//DCT
//    for(int v = 0; v < BLOCK_SIZE; v++){
//
//        int cv = (v == 0) ? sqrt2 : INTIZATION_RATE;
//        for(int u = 0; u < BLOCK_SIZE; u++){
//
//            int cu = (u == 0) ? sqrt2 : INTIZATION_RATE;
//            int sum = 0;
//            int tmp = 0;
//
//            for(int y = 0; y < BLOCK_SIZE;y++) {
//                for(int x = 0; x < BLOCK_SIZE; x++) {
//                    tmp = block[y * BLOCK_SIZE + x] * cos_tbl[u][x] / INTIZATION_RATE;
//                    tmp = tmp * cos_tbl[v][y] / INTIZATION_RATE;
//                    sum += tmp;
//                }
//            }
//            tmp = sum * cu / 4 / INTIZATION_RATE;
//            work[v * BLOCK_SIZE + u] = tmp * cv / INTIZATION_RATE;
//        }
//    }
//    for(int i = 0; i < BLOCK_SIZE*BLOCK_SIZE; i++) {
//        block[i] = work[zigzag_tbl[i]];
//    }
//    return ret;
//}
/**
 * Quantization function.
 *
 * @param block {Y|Cr|Cb}Block
 * @param mode YUV(Y,Cr,Cb) Type
 * @param quality Quality of quantization.
 * @return success(1) or error(Under 0)
 */
#if (JPEG_16B == 1)
static int quantization(int *block, const block_mode mode, const quality_rate quality){
	int ret = 1;
    const uint8_t *qt;
    const quant_tbl *qt_tbl;
    qt_tbl = get_quality_tbl(quality);

//    switch(mode) {
//        case Cr:
//        case Cb:
//            qt = qt_tbl->chrominance_quant_tbl;
//            break;
//        case Y:
//        default:
            qt = qt_tbl->luminance_quant_tbl;
//            break;
//    }

    for(int i = 0; i < QUANT_SIZE; i++) {
        block[i] /= qt[i];
    }
    return ret;
}
#else
static int8_t quantization(int8_t *block, const block_mode mode, const quality_rate quality){
	int ret = 1;
    const uint8_t *qt;
    const quant_tbl *qt_tbl;
    qt_tbl = get_quality_tbl(quality);

//    switch(mode) {
//        case Cr:
//        case Cb:
//            qt = qt_tbl->chrominance_quant_tbl;
//            break;
//        case Y:
//        default:
            qt = qt_tbl->luminance_quant_tbl;
//            break;
//    }

    for(int i = 0; i < QUANT_SIZE; i++) {
        block[i] /= qt[i];
    }
    return ret;
}
#endif

/**
 * Huffman Code function.
 *
 * @param data JPEG Structure
 * @param block {Y|Cr|Cb}Block
 * @param preDC DC component of Previous Block
 * @param dc_huff_len_tbl Huffman code's length table For DC
 * @param dc_huff_code_tbl Huffman code table For DC
 * @param ac_huff_len_tbl Huffman code's length table For AC
 * @param ac_huff_code_tbl Huffman code table For AC
 * @return Coding data Length. Under 0 is error.
 */

#if (JPEG_16B == 1) 
static int coding_huff(jpeg_data *data, const int *block, const int pre_DC,
                    const uint8_t *dc_huff_len_tbl, const int16_t *dc_huff_code_tbl, 
                    const uint8_t *ac_huff_len_tbl, const int16_t *ac_huff_code_tbl) {
    //DC component
    int diff = block[0] - pre_DC;//Difference value of DC component
    int absDiff = (int)abs(diff);//Absolute value of Difference value
    int index = 0;

    //Number of bits of Difference value of DC component
    while(absDiff > 0){
        absDiff >>= 1;
        index++;
    }
    put_bits(data, dc_huff_len_tbl[index], dc_huff_code_tbl[index]);//Put Huffman code

    if(index != 0){
        if(diff < 0)
            --diff;
        put_bits(data, index, diff);//Put Difference value of DC component
    }

    //AC component
    int run = 0;//Run length
    for(int n = 1; n < 64; n++){
        int absCoefficient = (int)abs(block[n]);//Absolute value of Coefficient
        if(absCoefficient != 0){
            while(run > 15){//ZRL
                put_bits(data, ac_huff_len_tbl[151], ac_huff_code_tbl[151]);//Put ZRL
                run -= 16;
            }

            //Number of bits of Coefficient
            int s = 0;
            while(absCoefficient > 0){
                absCoefficient >>= 1;
                s++;
            }
            int aIdx = run * 10 + s + ((run == 15) ? 1 : 0);
            put_bits(data, ac_huff_len_tbl[aIdx], ac_huff_code_tbl[aIdx]);//Put Huffman Code

            int v = block[n];//Coefficient
            if(v < 0) {
                v--;
            }
            put_bits(data, s, v);//Put Coefficient

            run = 0;//Clear Run length
        } else {
            if(n == 63)//End of Block
                put_bits(data, ac_huff_len_tbl[0], ac_huff_code_tbl[0]);//Put EOB
            else
                run++;
        }
    }
    return data->data_len;
}
#else
static int coding_huff(jpeg_data *data, const int8_t *block, const int8_t pre_DC,
                    const uint8_t *dc_huff_len_tbl, const int16_t *dc_huff_code_tbl, 
                    const uint8_t *ac_huff_len_tbl, const int16_t *ac_huff_code_tbl) {
    //DC component
    int8_t diff = block[0] - pre_DC;//Difference value of DC component
    int8_t absDiff = (int8_t)abs(diff);//Absolute value of Difference value
    int8_t index = 0;

    //Number of bits of Difference value of DC component
    while(absDiff > 0){
        absDiff >>= 1;
        index++;
    }
    put_bits(data, dc_huff_len_tbl[index], dc_huff_code_tbl[index]);//Put Huffman code

    if(index != 0){
        if(diff < 0)
            --diff;
        put_bits(data, index, diff);//Put Difference value of DC component
    }

    //AC component
    int run = 0;//Run length
    for(int n = 1; n < 64; n++){
        int8_t absCoefficient = (int8_t)abs(block[n]);//Absolute value of Coefficient
        if(absCoefficient != 0){
            while(run > 15){//ZRL
                put_bits(data, ac_huff_len_tbl[151], ac_huff_code_tbl[151]);//Put ZRL
                run -= 16;
            }

            //Number of bits of Coefficient
            int s = 0;
            while(absCoefficient > 0){
                absCoefficient >>= 1;
                s++;
            }
            int8_t aIdx = run * 10 + s + ((run == 15) ? 1 : 0);
            put_bits(data, ac_huff_len_tbl[aIdx], ac_huff_code_tbl[aIdx]);//Put Huffman Code

            int8_t v = block[n];//Coefficient
            if(v < 0) {
                v--;
            }
            put_bits(data, s, v);//Put Coefficient

            run = 0;//Clear Run length
        } else {
            if(n == 63)//End of Block
                put_bits(data, ac_huff_len_tbl[0], ac_huff_code_tbl[0]);//Put EOB
            else
                run++;
        }
    }
    return data->data_len;
}
#endif



/**
 * Put Filled uffman code's bits to Encoding data.
 *
 * @param data JPEG Structure
 * @param val Filled Encoding data
 */
static void put_byte(jpeg_data *data, const uint8_t val){
    uint_t pos = data->data_len;
    data->ret_data[pos++] = val;
    data->data_len = pos;
}
/**
 * Fill Huffman code's bits to byte.
 *
 * @param data JPEG Structure
 * @param nbits number of Huffman code's bits
 * @param val Filled Encoding data
 */
static void put_bits(jpeg_data *data, const uint_t nbits, const uint16_t val) {
    uint_t codebits = nbits;
    uint16_t bits = val << (16 - codebits);
    uint8_t byte = data->stream.byte;
    uint_t rest = data->stream.rest;
    if(rest == 0) {
        rest = 8;
    }
    while(codebits != 0) {
        uint_t n = codebits;
        if(n > rest){
            n = rest;
        }
        codebits -= n;
        rest -= n;
        byte = (byte << n) | (uint8_t)((bits >> (16 - n)) & 0xff);
        bits <<= n;
        if(rest == 0) {
            put_byte(data, byte);
            if(byte == 0xff) {
                put_byte(data, 0x00U);
            }
            rest = 8;
        }
    }
    data->stream.byte = byte;
    data->stream.rest = rest;
}
/**
 * Flush Huffman code's bits to byte.
 *
 * @param data JPEG Structure
 */
static void flush_bits(jpeg_data *data) {
    uint_t rest = data->stream.rest;
    if((rest & 7) != 0) {
        uint8_t byte = data->stream.byte;
        byte = byte << rest;
        put_byte(data, byte);
        data->stream.byte = 0x00;
        data->stream.rest = 0;
    }
}

/**
 * Initialize "ret_data" and "data_len" in jpeg_data;
 *
 * @param data JPEG Structure
 * @return Error Code(negative value)
 */
static int init_data(jpeg_data *data) {
    //Exception handling in here, if you need some exception handlings.
    data->data_len = 0;
//    memset(data->ret_data, '\n', JPEG_BUF_SIZE);
    return 1;
}
