/*
 * HM01B0Regs.h
 *
 *  Created on: May 22, 2018
 *      Author: Mehrdad Hessar
 */

#ifndef HM01B0REGS_H_
#define HM01B0REGS_H_

#define EXAMPLE_I2C_MASTER (Driver_I2C0)
#define EXAMPLE_I2C_SLAVE (Driver_I2C1)

/**#############################Registers#############################**/
//Sensor ID
#define REG_MODEL_ID_H      0x0000
#define REG_MODEL_ID_L      0x0001
#define REG_SILICON_REV     0x0002
#define REG_FRAME_COUNT     0x0005
#define REG_PIXEL_ORDER     0x0006

//Sensor mode control
#define REG_MODE_SELECT         0x0100
#define REG_IMAGE_ORIENTATION   0x0101
#define REG_SW_RESET            0x0103
#define REG_GRP_PARAM_HOLD      0x0104

//Sensor exposure gain control

//Frame timing control
#define REG_FRAME_LENGTH_LINES_H        0x0340
#define REG_FRAME_LENGTH_LINES_L        0x0341
#define REG_FRAME_LENGTH_PCK_H          0x0342
#define REG_FRAME_LENGTH_PCK_L          0x0343

//Bining mode control


//Test pattern control
#define REG_TEST_PATTERN_MODE       0x0601

//Black level control
#define REG_BLC_CFG         0x1000
#define REG_BLC_TGT         0x1003
#define REG_BLI_EN          0x1006
#define REG_BLC2_TGT        0x1007

//Sensor resevred

//VSYNC, HSYNC and pixel shift register
#define REG_VSYNC_HSYNC_PIXEL_SHIFT_EN      0x1012


//Binning mode control
#define REG_BIN_RDOUT_X     0x0383
#define REG_BIN_RDOUT_Y     0x0387
#define REG_BIN_MODE     0x0390

//Statistic control and read only

//Automatic exposure gain control


//Motion detection control

//Sensor timing control
#define REG_QVGA_WIN_EN                 0x3010
#define REG_SIX_BIT_MODE_EN             0x3011
#define REG_PMU_PROGRAMMABLE_FRAMECNT   0x3020
#define REG_ADVANCE_VSYNC               0x3022
#define REG_ADVANCE_HSYNC               0x3023
#define REG_EARLY_GAIN                  0x3035


//IO and clock control
#define REG_BIT_CONTROL                 0x3059
#define REG_OSC_CLK_DIV                 0x3060
#define REG_ANA_REGISTER_11             0x3061
#define REG_IO_DRIVE_STR                0x3062
#define REG_IO_DRIVE_STR2               0x3063
#define REG_ANA_REGISTER_14             0x3064
#define REG_OUTPUT_PIN_STATUS_CONTROL   0x3065
#define REG_ANA_REGISTER_17             0x3067
#define REG_PCLK_POLARITY               0x3068


//I2C slave registers


/**#############################Modes#############################**/
#define MODE_STAND_BY       0x00
#define MODE_STREAMING      0x01
#define MODE_STREAMING2     0x02
#define MODE_STREAMING3     0x03

#define ORIENTATION_HORINONTAL  0x00
#define ORIENTATION_VERTICAL  0x01


#define TEST_PATTERN_OFF            0x00
#define TEST_PATTERN_COLOR_BAR      0x01
#define TEST_PATTERN_WALKING_1      0x11


#endif /* IMAGESENSOR_HM01B0REGS_H_ */
