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
#ifndef JPEGDEF_H
#define JPEGDEF_H

/**
 * Use FPU or Fixed-point in DCT.
 * "__FPU_USED" is defined in Cortex-M header file.
 */
/* #define __FPU_USED 1 */

//typedef          char       int8_t;
typedef          int        int32_t;
typedef unsigned int        uint32_t;

typedef          short      int16_t;
typedef          int        int_t;
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint_t;
typedef unsigned char       uint8_t;
typedef unsigned int        uint_t;

#endif
