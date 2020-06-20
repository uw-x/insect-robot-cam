/*
 * HM01B0_CAPTURE.h
 *
 *  Created on: March 27, 2019
 *      Author: Ali
 */

#ifndef HM01B0_COMPRESS_H_
#define HM01B0_COMPRESS_H_

/************************************************************************************************************/
/*Added for compression*/
//#define JPEG_ENCODER  0x01
//#if (defined(JPEG_ENCODER) && (JPEG_ENCODER == 1))
#include <math.h>
#include "jpegDef.h"
#include "jpegencoder.h"
#include "flash_emu.h"
extern quality_rate quality = normal;
extern uint32_t compressed_size;
//#endif

uint32_t main_compression(quality_rate quality);


#endif /* HM01B0_COMPRESS_H_ */