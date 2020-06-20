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

/**
 * Sample Image Reader/Writer.
 * Spec:
 *       Data Attach Type: Address Base
 *       Image Format: 1-dimensional Sequential Array
 */
//#define DEBUG_ON_PC 1
//#ifdef DEBUG_ON_PC

#ifndef FLASH_EMU_H
#define FLASH_EMU_H

#include <stdio.h>
#include <stdlib.h>


/*Added by Ali*/
#include "jpegencoder.h"
uint8_t       jpeg_imag_mem[jpeg_imag_mem_size+1];   

FILE *fp_original;
FILE *fp_converted;
//#endif

int open_files(char *file_original, char *file_converted) {
//#ifdef DEBUG_ON_PC
//    fp_original = fopen( file_original, "rb" );
//    if( fp_original == NULL ){
//      return -1;
//    }
    fp_converted = fopen( file_converted, "wb" );
    if( fp_converted == NULL ){
      return -1;
    }
//#endif
    return 1;
}
int close_files() {
#ifdef DEBUG_ON_PC
    fclose(fp_original);
    fclose(fp_converted);
#endif
    return 1;
}

int write_data(unsigned char *data, unsigned int len) {
    int ret = 0;
//#ifdef DEBUG_ON_PC
    ret = fwrite(data, sizeof(unsigned char), len, fp_converted);
//#endif
    return ret;
}

int write_data2(unsigned char *data, unsigned int len, unsigned int pos) {
    int ret = 0;

//for (int i = 0; i<len; i++){
//    jpeg_imag_mem[pos+i] = data[i];
//    }
      memcpy(jpeg_imag_mem+pos, data, len);
    return ret;
}

int read_data(unsigned int pos, unsigned char *data, unsigned int len) {
    int ret = 0;
#ifdef DEBUG_ON_PC
    fseek(fp_original, pos, SEEK_SET);
    ret = fread(data, sizeof(unsigned char), len, fp_original);
#else
    for(int i = 0; i < len; i++) {
            data[i] = i;
    }
#endif   

    return ret;
}

//int read_data_cam(unsigned int pos, unsigned char *data, unsigned int len) {
//    int ret = 0;
//    for(int i = 0; i < len; i++) {
//            data[i] = i;
//    }
//
//
//    return ret;
//}

#endif /*FLASH_EMU_H*/