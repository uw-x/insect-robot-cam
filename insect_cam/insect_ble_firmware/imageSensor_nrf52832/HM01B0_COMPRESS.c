/*
 * HM01B0_CAPTURE.c
 *
 *  Created on: March 27, 2019
 *      Author: Ali
 */

#include "HM01B0_COMPRESS.h"
#include "HM01B0_BLE_DEFINES.h"

//uint32_t main_compression(int argc, char** argv, quality_rate quality, char *fname_jpeg) {
uint32_t main_compression(quality_rate quality) {    
 
    char *fname_original = "nrf52832_test_240_120_v49.bin";
    //char *fname_jpeg = "nrf52832_test_240_120_v49.jpg";
    char *fname_jpeg = COMPRESSED_FILE_NAME;
    open_files(fname_original, fname_jpeg);

    //init JPEG meta data
//    memset(jpeg_imag_mem, 0x11, jpeg_imag_mem_size);
    jpeg_data data = {0};
    //set JPEG meta data
    data.width = m_length_rx;
    data.height = LINE_NUM;
    //data.c_info = color_info_yuv411;
    data.c_info = color_info_gray_scale;
    data.quality = quality;

    uint32_t cnt1=encode_image(&data, read_data, write_data);

    asm("nop");

    #if (RELEASE_CODE ==0)
    printf("printed: %d \n", cnt1);
    #endif


    close_files();

    return cnt1;
}



