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
#include <math.h>

#include "jpegDef.h"
#include "jpegencoder.h"
#include "flash_emu.h"

int main(int argc, char** argv) {
    
    char *fname_original = "Image_sample_01_converted.y";
    char *fname_jpeg = "Image_sample_01_converted_int.jpg";
    open_files(fname_original, fname_jpeg);

    //init JPEG meta data
    jpeg_data data = {0};
    //set JPEG meta data
    data.width = 640;
    data.height = 480;
    data.c_info = color_info_yuv411;
    //data.c_info = color_info_gray_scale;
    data.quality = normal;

    encode_image(&data, read_data, write_data);

    close_files();

    return 1;
}

