
 /** @file
 *
 *  Created on on top of nordic image transfer demo: May 05, 2019
 *      Author: Ali Najafi
 */


#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "app_timer.h"
#include "ble_image_transfer_service.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "bsp_btn_ble.h"
#include "nrf_pwr_mgmt.h"

#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

//#include "ArducamMini2MP.h"
#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define PCA10056_USE_FRONT_HEADER       0                                            /**< Use the front header (P24) for the camera module. Requires SB10-15 and SB20-25 to be soldered/cut, as described in the readme. */

#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */

//#define DEVICE_NAME                     "Camera Demo v2"                            /**< Name of device. Will be included in the advertising data. */
#define DEVICE_NAME                     "Beetle Cam"                            /**< Name of device. Will be included in the advertising data. */
#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                  /**< UUID type for the Nordic UART Service (vendor specific). */

#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */

#define APP_ADV_INTERVAL                64                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */

//#define APP_ADV_INTERVAL                1000                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */

#define APP_ADV_DURATION                18000                                       /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(15, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(15, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
//#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(400, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
//#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(650, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */



#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                      /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */


/*********************************************************************************************/
/*Added by Ali*/
/* TWI instance ID. */
//#define TWI_INSTANCE_ID     0
static uint8_t m_new_angle;
static bool                             stream_first_image_done = false;
static bool                             img_info_sent = false;
static uint8_t m_new_device;
static uint8_t m_new_reg_address_msb;
static uint8_t m_new_reg_address_lsb;
static uint8_t m_new_reg_value;



#include "app_pwm.h"
static volatile bool m_xfer_done = false;
#include "HM01B0_CAPTURE.h"
#include "HM01B0_CLK.h"
#include "HM01B0_SPI.h"
#include "HM01B0_PWM_BOOST.h"
#include "HM01B0_LVLD_TIMER.h"
#include "HM01B0_PWM.h"
#include "MC6470Regs.h"
#include "MC6470_FUNC.h"
#include "HM01B0_COMPRESS.h"
//#include "HM01B0_GPIO.h"
uint8_t image_rd_done;
uint32_t hm_pixel_counter;
//uint8_t image_mem[hm_mem_size];
uint8_t       m_rx_buf[total_spi_buffer_size_max+200];       /**< RX buffer. */
//uint16_t m_length_rx;  
uint16_t m_length_rx_done; 
uint8_t LINE_NUM = ROW_INIT;
extern uint16_t total_image_size = total_spi_buffer_size;
uint16_t m_length_rx = spi_buffer_size;
bool spis_xfer_done;
extern uint8_t single_capture_flag = 0;
extern uint8_t m_new_jpeg = 0;
extern uint8_t jpeg_active = 0;

extern uint8_t m_new_acc = 0;
extern bool acc_int_flag = false;
extern uint8_t acc_pic_num = 0;
extern bool acc_int_cmd_flag = false;//flag for for accelerometer is giving interrupt or not
extern bool acc_int_cmd_sweep = false;//flag for if the sweeping is active for accelerometer or not
extern bool acc_rec_flag = false;//flag for checking if we want to record ACC data or not
extern bool cmd_acc_init_flag = false;//flag to show if the camera is already initialized or not
extern uint16_t packet_sent_acc = 0;

uint16_t cam_addr_update;




//COMPRESSION
uint8_t       jpeg_imag_mem[jpeg_imag_mem_size+1];
uint32_t      compressed_size;

////newly added for nrf52832
#if defined(BOARD_PCA10040)
//uint32_t CAM_SPI_CS;
////NRF_GPIO_Type * gpio_p_reg = nrf_gpio_pin_port_decode(&CAM_SPI_CS);
////nrf_gpio_pin_port_decode(&CAM_SPI_CS);
//NRF_GPIO_Type * gpio_p_reg;
//uint32_t CAM_SPI_PIN_MASK;
uint32_t line_count;
uint32_t CAM_LINE_VLD;
//uint32_t rxd_ptr; 
#endif

//uint32_t CAM_SPI_CS =  16;

//comments: - don't initialize gpiote twice!  -care should be taken when having both SPI and I2C!(I have removed SPI stuff here)

/*********************************************************************************************/

BLE_ITS_DEF(m_its, NRF_SDH_BLE_TOTAL_LINK_COUNT);                                   /**< BLE NUS service instance. */
NRF_BLE_GATT_DEF(m_gatt);                                                           /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                             /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);                                                 /**< Advertising module instance. */

extern uint8_t                          m_new_command_received = 0;
static uint8_t                          m_new_resolution, m_new_phy;
//static arducam_mini_2mp_init_t          m_camera_init;

extern bool                             m_stream_mode_active = false;

static ble_its_ble_params_info_t        m_ble_params_info = {20, 50, 1, 1};

static uint16_t   m_conn_handle          = BLE_CONN_HANDLE_INVALID;                 /**< Handle of the current connection. */
static uint16_t   m_ble_its_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3;            /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */
static ble_uuid_t m_adv_uuids[]          =                                          /**< Universally unique service identifier. */
{
    {BLE_UUID_ITS_SERVICE, NUS_SERVICE_UUID_TYPE}
};

enum {APP_CMD_NOCOMMAND = 0, APP_CMD_SINGLE_CAPTURE, APP_CMD_START_STREAM, APP_CMD_STOP_STREAM, 
      APP_CMD_CHANGE_RESOLUTION, APP_CMD_CHANGE_PHY, APP_CMD_SEND_BLE_PARAMS, APP_CMD_CHANGE_ANGLE, 
      APP_CMD_JPEG, APP_CMD_ACC, APP_CMD_WR_REG, APP_CMD_SWEEP_CAM, APP_CMD_CAM_ACC_REC};






/************************************************************************************************************/
/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyse
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the timer module.
 */
static void timers_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of
 *          the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *) DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
    
#if 0
    ble_gap_addr_t ble_address = {.addr_type = BLE_GAP_ADDR_TYPE_RANDOM_STATIC,
                                  .addr_id_peer = 0,
                                  .addr = {0xC3,0x11,0x99,0x33,0x44,0xFF}};
    err_code = sd_ble_gap_addr_set(&ble_address);
    APP_ERROR_CHECK(err_code);
#endif
}


/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for handling the data from the ITS Service.
 *
 * @details This function will process the data received from the ITS BLE Service.
 *
 * @param[in] p_its    ITS Service structure.
 * @param[in] p_data   Data received.
 * @param[in] length   Length of the data.
 */
/**@snippet [Handling the data received over BLE] */
static void its_data_handler(ble_its_t * p_its, uint8_t const * p_data, uint16_t length)
{
    switch(p_data[0])
    {
        // Take picture
        case APP_CMD_SINGLE_CAPTURE:
            m_new_command_received = p_data[0];//Added by Ali
            img_info_sent = false;
            acc_rec_flag = false;
            #if(CAM_SINGLE_CAPTURE_POWER_FIX == 1)
            nrf_gpio_pin_set(CAM_POWER);
            nrf_delay_ms(POR_DELAY);
            #endif
            break;//Added by Ali
        case APP_CMD_SEND_BLE_PARAMS:
            m_new_command_received = p_data[0];
            break;
        
        case APP_CMD_START_STREAM:
            #if(CAM_SINGLE_CAPTURE_POWER_FIX == 1)
            nrf_gpio_pin_set(CAM_POWER);
            nrf_delay_ms(POR_DELAY);
            #endif
            #if(CAM_CLK_GATING == 1)
            nrf_drv_timer_enable(&CAM_TIMER);
            #endif
            single_capture_flag = 0;
            m_stream_mode_active = true;
            img_info_sent = false;
            acc_rec_flag = false;
            cmd_acc_init_flag = false;
            m_new_command_received = p_data[0];
            break;

        case APP_CMD_STOP_STREAM:
            pwm_boost_zero();
            hm_i2c_write_8b(CNTL1,0x08, SLAV_ADDR_MAG);
            hm_i2c_write_8b(MODE, 0x00, SLAV_ADDR_ACC);
            cmd_acc_init_flag = false;
            acc_int_cmd_flag = false;
            acc_int_cmd_sweep = false;
            acc_rec_flag = false;
            nrf_drv_gpiote_in_event_disable(ACC_INT_PIN);
            img_info_sent = false;
            stream_first_image_done = false;
            m_stream_mode_active = false;
            m_new_command_received = p_data[0];
            break;
        
        case APP_CMD_CHANGE_RESOLUTION:
            m_new_command_received = APP_CMD_CHANGE_RESOLUTION;

            lvld_timer_pause(&TIMER_LVLD);
            nrf_gpio_pin_set(CAM_SPI_CS);
            nrf_drv_gpiote_in_event_disable(FRAME_VLD);
            nrf_drv_gpiote_in_event_disable(LINE_VLD);
            img_info_sent = false;

            m_stream_mode_active = false;
            stream_first_image_done = false;
            m_new_resolution = p_data[1];
            break;
        
        case APP_CMD_CHANGE_PHY:
            m_new_command_received = APP_CMD_CHANGE_PHY;
            m_new_phy = p_data[1];
            break;
       
        case APP_CMD_CHANGE_ANGLE:
            m_new_command_received = APP_CMD_CHANGE_ANGLE;
            m_stream_mode_active = false;
            stream_first_image_done = false;
            m_new_angle = p_data[1];
            break;

        case APP_CMD_JPEG:
            m_new_command_received = APP_CMD_JPEG;
            m_stream_mode_active = false;
            stream_first_image_done = false;
            img_info_sent = false;
            m_new_jpeg = p_data[1];
            break;

        case APP_CMD_ACC:
            m_new_command_received = APP_CMD_ACC;
//            m_stream_mode_active = false;
            img_info_sent = false;
//            m_new_acc = p_data[1];
            break;

        case APP_CMD_WR_REG:
            m_new_command_received = APP_CMD_WR_REG;
            m_new_device = p_data[1];
            m_new_reg_address_msb = p_data[2];//Address comes MSB first
            m_new_reg_address_lsb = p_data[3];
            m_new_reg_value = p_data[4];
//            m_stream_mode_active = false;
            img_info_sent = false;
//            m_new_acc = p_data[1];
            break;

        case APP_CMD_SWEEP_CAM:
            m_new_command_received = APP_CMD_WR_REG;
            img_info_sent = false;
            break;

        
        default: 
            NRF_LOG_ERROR("Unknown command!!");
            break;
    }
}
/**@snippet [Handling the data received over BLE] */


/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    uint32_t           err_code;
    ble_its_init_t     its_init;
    nrf_ble_qwr_init_t qwr_init = {0};

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Initialize ITS.
    memset(&its_init, 0, sizeof(its_init));

    its_init.data_handler = its_data_handler;

    err_code = ble_its_init(&m_its, &its_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling an event from the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module
 *          which are passed to the application.
 *
 * @note All this function does is to disconnect. This could have been done by simply setting
 *       the disconnect_on_fail config parameter, but instead we use the event handler
 *       mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    switch(p_evt->evt_type)
    {
        case BLE_CONN_PARAMS_EVT_SUCCEEDED:
            break;
            
        case BLE_CONN_PARAMS_EVT_FAILED:
            //err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
            //APP_ERROR_CHECK(err_code);
            NRF_LOG_ERROR("Conn params failed. Keep the connection anyway..");
            break;
    }
}


/**@brief Function for handling errors from the Connection Parameters module.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}



/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break;
        case BLE_ADV_EVT_IDLE:
            NRF_LOG_INFO("BLE_ADV_EVT_IDLE...");
            err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
            APP_ERROR_CHECK(err_code);
            break;
        default:
            break;
    }
}


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    uint32_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            #if (RELEASE_CODE ==0)
                NRF_LOG_INFO("Connected");
            #endif
            change_pic_resolution(100 , 100);
//            hm_peripheral_connected_init();
            
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            #if (RELEASE_CODE ==0)
                NRF_LOG_INFO("Disconnected");
            #endif
            // LED indication will be changed when advertising starts.
            app_pwm_uninit(&PWM2);
            hm_peripheral_uninit();
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break;
            
        case BLE_GAP_EVT_CONN_PARAM_UPDATE:
        {
            uint16_t max_con_int = p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params.max_conn_interval;
            uint16_t min_con_int = p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params.min_conn_interval;

            m_ble_params_info.con_interval = max_con_int;
            ble_its_ble_params_info_send(&m_its, &m_ble_params_info);
            #if (RELEASE_CODE ==0)
                NRF_LOG_INFO("Con params updated: CI %i, %i", (int)min_con_int, (int)max_con_int);
            #endif
        } break;
            
        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            #if (RELEASE_CODE ==0)
                NRF_LOG_DEBUG("PHY update request.");
            #endif
            
            ble_gap_phys_t const phys =
            {
                BLE_GAP_PHY_AUTO,
                BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_PHY_UPDATE:
            m_ble_params_info.tx_phy = p_ble_evt->evt.gap_evt.params.phy_update.tx_phy;
            m_ble_params_info.rx_phy = p_ble_evt->evt.gap_evt.params.phy_update.rx_phy;    
            ble_its_ble_params_info_send(&m_its, &m_ble_params_info);
            #if (RELEASE_CODE ==0)
                NRF_LOG_INFO("Phy update: %i, %i", (int)m_ble_params_info.tx_phy, (int)m_ble_params_info.rx_phy);
            #endif
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            app_pwm_uninit(&PWM2);
            hm_peripheral_uninit();
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            app_pwm_uninit(&PWM2);
            hm_peripheral_uninit();
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            //NRF_LOG_INFO("BLE event not handled by app: %i", p_ble_evt->header.evt_id);
            break;
    }
}


/**@brief Function for the SoftDevice initialization.
 *
 * @details This function initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}


/**@brief Function for handling events from the GATT library. */
void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    uint32_t data_length;
    if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED))
    {
        data_length = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
        //m_ble_params_info.mtu = m_ble_its_max_data_len;
        
        NRF_LOG_INFO("gatt_event: ATT MTU is set to 0x%X (%d)", data_length, data_length);
    }
    else if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_DATA_LENGTH_UPDATED))
    {
        data_length = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH - 4;
        m_ble_its_max_data_len = data_length;
        m_ble_params_info.mtu = m_ble_its_max_data_len;
        ble_its_ble_params_info_send(&m_its, &m_ble_params_info);
        
        NRF_LOG_INFO("gatt_event: Data len is set to 0x%X (%d)", data_length, data_length);
    }
    //NRF_LOG_DEBUG("ATT MTU exchange completed. central 0x%x peripheral 0x%x",
      //            p_gatt->att_mtu_desired_central,
        //        p_gatt->att_mtu_desired_periph);
}


/**@brief Function for initializing the GATT library. */
void gatt_init(void)
{
    ret_code_t err_code;

    err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
    APP_ERROR_CHECK(err_code);
    
    err_code = nrf_ble_gatt_data_length_set(&m_gatt, BLE_CONN_HANDLE_INVALID, NRF_SDH_BLE_GAP_DATA_LENGTH);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;
    switch (event)
    {
        case BSP_EVENT_SLEEP:
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BSP_EVENT_WHITELIST_OFF:
            if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
            {
                err_code = ble_advertising_restart_without_whitelist(&m_advertising);
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }
            break;

        default:
            break;
    }
}


/**@brief   Function for handling app_uart events.
 *
 * @details This function will receive a single character from the app_uart module and append it to
 *          a string. The string will be be sent over BLE when the last character received was a
 *          'new line' '\n' (hex 0x0A) or if the string has reached the maximum data length.
 */
/**@snippet [Handling the data received over UART] */
void uart_event_handle(app_uart_evt_t * p_event)
{

}
/**@snippet [Handling the data received over UART] */


/**@brief  Function for initializing the UART module.
 */
/**@snippet [UART Initialization] */
static void uart_init(void)
{
    uint32_t                     err_code;
    app_uart_comm_params_t const comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_DISABLED,
        false,
#if defined (UART_PRESENT)
        NRF_UART_BAUDRATE_115200
#else
        NRF_UARTE_BAUDRATE_115200
#endif
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);
}
/**@snippet [UART Initialization] */


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t               err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = false;
    init.advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

    init.srdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.srdata.uuids_complete.p_uuids  = m_adv_uuids;

    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;
    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(void)
{
    bsp_event_t startup_event;

    uint32_t err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    while(NRF_LOG_PROCESS());
//    sd_app_evt_wait();
    sd_app_evt_wait();
//    nrf_pwr_mgmt_run();
//__WFE();
}


/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    uint32_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
}


void conn_evt_len_ext_set(bool status)
{
    ret_code_t err_code;
    ble_opt_t  opt;

    memset(&opt, 0x00, sizeof(opt));
    opt.common_opt.conn_evt_ext.enable = status ? 1 : 0;

    err_code = sd_ble_opt_set(BLE_COMMON_OPT_CONN_EVT_EXT, &opt);
    APP_ERROR_CHECK(err_code);
}

void data_len_set(uint8_t value)
{
    ret_code_t err_code;
    err_code = nrf_ble_gatt_data_length_set(&m_gatt, BLE_CONN_HANDLE_INVALID, value);
    APP_ERROR_CHECK(err_code);

}


void change_pic_resolution(uint16_t spi_size, uint8_t line_num_input )
{
    m_length_rx = spi_size;
    LINE_NUM = line_num_input;
    total_image_size = spi_size*line_num_input;
    lvld_timer_val = (spi_size + 20)*8*2;
    nrf_drv_timer_extended_compare(
    &TIMER_LVLD, NRF_TIMER_CC_CHANNEL4, lvld_timer_val, NRF_TIMER_SHORT_COMPARE4_CLEAR_MASK, true);
}

void gpio_init_low(void){
    uint32_t frame_valid_pin = 19;
    nrf_gpio_cfg(
    frame_valid_pin,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_NOPULL,
//    NRF_GPIO_PIN_H0H1,
    NRF_GPIO_PIN_S0S1,
    NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_clear(frame_valid_pin);   
    nrf_delay_ms(POR_DELAY);
    nrf_gpio_pin_set(frame_valid_pin);
    nrf_gpio_pin_clear(frame_valid_pin);   
    nrf_delay_ms(POR_DELAY);
    nrf_gpio_pin_set(frame_valid_pin);

    frame_valid_pin = 20;
    nrf_gpio_cfg(
    frame_valid_pin,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_NOPULL,
//    NRF_GPIO_PIN_H0H1,
    NRF_GPIO_PIN_S0S1,
    NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_clear(frame_valid_pin);   
    nrf_delay_ms(POR_DELAY);
    nrf_gpio_pin_set(frame_valid_pin);
    nrf_gpio_pin_clear(frame_valid_pin);   
    nrf_delay_ms(POR_DELAY);
    nrf_gpio_pin_set(frame_valid_pin);

    frame_valid_pin = 22;
    nrf_gpio_cfg(
    frame_valid_pin,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_NOPULL,
//    NRF_GPIO_PIN_H0H1,
    NRF_GPIO_PIN_S0S1,
    NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_clear(frame_valid_pin);   
    nrf_delay_ms(POR_DELAY);
    nrf_gpio_pin_set(frame_valid_pin);
    nrf_gpio_pin_clear(frame_valid_pin);   
    nrf_delay_ms(POR_DELAY);
    nrf_gpio_pin_set(frame_valid_pin);

}


/**@brief Application main function.
 */
int main(void)
{

    uint32_t img_data_length = 0;
    uint8_t img_data_buffer[255];
    image_rd_done = 0;
    hm_pixel_counter=0;
//    uint8_t image_mem2[hm_mem_size] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x00, 0xf0, 0x08, 0x02, 0x00, 0x00, 0x00, 0xfe, 0x4f, 0x2a, 0x3c, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x0b, 0x13, 0x00, 0x00, 0x0b, 0x13, 0x01, 0x00, 0x9a, 0x9c, 0x18, 0x00, 0x00, 0x00, 0x07, 0x74, 0x49, 0x4d, 0x45, 0x07, 0xd5, 0x03, 0x1d, 0x13, 0x12, 0x28, 0x38, 0xd5, 0x17, 0xae, 0x00, 0x00, 0x02, 0x9d, 0x49, 0x44, 0x41, 0x54, 0x78, 0xda, 0xed, 0xda, 0xcb, 0x0e, 0x82, 0x30, 0x10, 0x40, 0xd1, 0xa9, 0xe1, 0xff, 0x7f, 0x79, 0x5c, 0x48, 0x48, 0x05, 0x2c, 0x29, 0xc6, 0x60, 0xf5, 0x9c, 0x8d, 0x8a, 0xc4, 0x98, 0x26, 0x97, 0x8e, 0x8f, 0x08, 0x60, 0x58, 0x25, 0x22, 0x32, 0xd3, 0x42, 0xc0, 0x78, 0xf5, 0x96, 0x32, 0x2d, 0xf7, 0x86, 0x78, 0xc7, 0xdf, 0x76, 0xa5, 0x29, 0x2e, 0x7d, 0x96, 0xef, 0xaa, 0x16, 0x32, 0x22, 0xe2, 0x66, 0x21, 0x60, 0x5c, 0xd3, 0x26, 0xeb, 0xdc, 0xdd, 0xa9, 0xeb, 0xa7, 0x96, 0xed, 0xba, 0x3e, 0x79, 0xbb, 0x87, 0x67, 0xe6, 0xea, 0xe0, 0xf6, 0xfc, 0xf6, 0x91, 0xc6, 0x8b, 0x03, 0x3b, 0x01, 0xd7, 0x71, 0xd6, 0xd9, 0xd4, 0x0f, 0xeb, 0xfb, 0xaf, 0xce, 0x6f, 0x4c, 0xed, 0xed, 0x23, 0xbd, 0x2f, 0x08, 0xff, 0xcc, 0x08, 0x0d, 0x3f, 0xb4, 0x03, 0xf7, 0x7f, 0x92, 0xce, 0xae, 0x29, 0xf7, 0x71, 0xfe, 0x6a, 0x6f, 0x37, 0x24, 0xc3, 0x35, 0x01, 0xf7, 0x86, 0x77, 0x38, 0x42, 0x03, 0x46, 0x68, 0xb0, 0x03, 0x3f, 0xef, 0x93, 0xbd, 0xd3, 0x72, 0x34, 0xbf, 0xb8, 0xb6, 0xf1, 0xc2, 0xfb, 0xe6, 0x5f, 0x6e, 0xfc, 0x91, 0xe3, 0xe4, 0xf2, 0xf9, 0x27, 0x82, 0xe5, 0xbb, 0xaa, 0x85, 0x8c, 0x52, 0x8a, 0x11, 0x1a, 0xc6, 0x1f, 0xa1, 0x5d, 0x09, 0x61, 0x44, 0x76, 0x60, 0x10, 0x30, 0x20, 0x60, 0x40, 0xc0, 0x20, 0x60, 0x40, 0xc0, 0x80, 0x80, 0x01, 0x01, 0x83, 0x80, 0x01, 0x01, 0x03, 0x02, 0x06, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x18, 0x04, 0x0c, 0x08, 0x18, 0x10, 0x30, 0x20, 0x60, 0x10, 0x30, 0x20, 0x60, 0x40, 0xc0, 0x20, 0x60, 0x40, 0xc0, 0x80, 0x80, 0x01, 0x01, 0x83, 0x80, 0x01, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x18, 0x04, 0x0c, 0x08, 0x18, 0x10, 0x30, 0x20, 0x60, 0x10, 0x30, 0x20, 0x60, 0x40, 0xc0, 0x20, 0x60, 0x40, 0xc0, 0x80, 0x80, 0x01, 0x01, 0x83, 0x80, 0x01, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x18, 0x04, 0x0c, 0x08, 0x18, 0x10, 0x30, 0x20, 0x60, 0x10, 0x30, 0x20, 0x60, 0x40, 0xc0, 0x80, 0x80, 0x41, 0xc0, 0x80, 0x80, 0x01, 0x01, 0x83, 0x80, 0x01, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x18, 0x10, 0x30, 0x08, 0x18, 0x10, 0x30, 0x20, 0x60, 0x10, 0x30, 0x20, 0x60, 0x40, 0xc0, 0x80, 0x80, 0x41, 0xc0, 0x80, 0x80, 0x01, 0x01, 0x83, 0x80, 0x01, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x18, 0x10, 0x30, 0x08, 0x18, 0x10, 0x30, 0x20, 0x60, 0x10, 0x30, 0x20, 0x60, 0x40, 0xc0, 0x80, 0x80, 0x41, 0xc0, 0x80, 0x80, 0x01, 0x01, 0x03, 0x02, 0x06, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x18, 0x10, 0x30, 0x08, 0x18, 0x10, 0x30, 0x20, 0x60, 0x40, 0xc0, 0x20, 0x60, 0x40, 0xc0, 0x80, 0x80, 0x41, 0xc0, 0x80, 0x80, 0x01, 0x01, 0x03, 0x02, 0x06, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x18, 0x10, 0x30, 0x08, 0x18, 0x10, 0x30, 0x20, 0x60, 0x40, 0xc0, 0x20, 0x60, 0x40, 0xc0, 0x80, 0x80, 0x41, 0xc0, 0x80, 0x80, 0x01, 0x01, 0x03, 0x02, 0x06, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x18, 0x04, 0x0c, 0x08, 0x18, 0x10, 0x30, 0x08, 0x18, 0x10, 0x30, 0x20, 0x60, 0x40, 0xc0, 0x20, 0x60, 0x40, 0xc0, 0x80, 0x80, 0x41, 0xc0, 0x96, 0x00, 0x04, 0x0c, 0x08, 0x18, 0x10, 0x30, 0x08, 0x18, 0x10, 0x30, 0x20, 0x60, 0x40, 0xc0, 0x20, 0x60, 0x40, 0xc0, 0x80, 0x80, 0x41, 0xc0, 0x80, 0x80, 0x01, 0x01, 0x03, 0x02, 0x06, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x18, 0x04, 0x0c, 0x08, 0x18, 0x10, 0x30, 0x08, 0x18, 0x10, 0x30, 0x20, 0x60, 0x40, 0xc0, 0x20, 0x60, 0x40, 0xc0, 0x80, 0x80, 0x01, 0x01, 0x83, 0x80, 0x01, 0x01, 0x03, 0x02, 0x06, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x18, 0x04, 0x0c, 0x08, 0x18, 0x10, 0x30, 0x08, 0x18, 0x10, 0x30, 0x20, 0x60, 0x40, 0xc0, 0x20, 0x60, 0x40, 0xc0, 0x80, 0x80, 0x01, 0x01, 0x83, 0x80, 0x01, 0x01, 0x03, 0x02, 0x06, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x18, 0x04, 0x0c, 0x08, 0x18, 0x10, 0x30, 0x20, 0x60, 0x10, 0x30, 0x20, 0x60, 0x40, 0xc0, 0x20, 0x60, 0x40, 0xc0, 0x80, 0x80, 0x01, 0x01, 0x83, 0x80, 0x01, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x18, 0x04, 0x0c, 0x08, 0x18, 0xf8, 0xa4, 0x69, 0xbe, 0xcd, 0xb4, 0x16, 0x27, 0x58, 0x35, 0xcb, 0x07, 0x70, 0xd2, 0x1d, 0x0f, 0x2f, 0x4d, 0x01, 0x7d, 0x45, 0x66, 0x14, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82} ;
//    uint8_t image_mem[hm_mem_size];

    #if (defined(CAMERA_DEBUG) && (CAMERA_DEBUG == 1)) 
    //Camera initializations

    
    timers_init();
    buttons_leds_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();
    conn_params_init();
    sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);

    //Camera initializations

    nrf_gpio_cfg_output(CAM_SPI_CS);
    gpio_p_reg->OUTSET = CAM_SPI_PIN_MASK;

//    nrf_gpio_cfg_output(CAM_POWER);
    cam_power_up();

    spi_init();//put SPI in RX mode
//    hm_clk_out();
    hm_peripheral_init();


//    mc6470_acc_init();
//
//    accel_sr = hm_i2c_read_8b(SR, SLAV_ADDR_ACC);
//    nrf_drv_gpiote_in_event_enable(ACC_INT_PIN, true);
//    

    //while(!acc_int_flag);
    hm_single_capture_spi_832();

    while(1);

    
    #else

    nrf_gpio_cfg(
        BOOST_PIN,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_PULLDOWN,
        NRF_GPIO_PIN_S0S1,
        NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_clear(BOOST_PIN);

    timers_init();
    buttons_leds_init();
    power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();
    conn_params_init();
    sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);



    //Camera initializations
    nrf_gpio_cfg(
    CAM_SPI_CS,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_PULLUP,
    NRF_GPIO_PIN_S0S1,
    NRF_GPIO_PIN_NOSENSE);


    uint32_t test_pin = 7;
    nrf_gpio_cfg(
    test_pin,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_NOPULL,
    NRF_GPIO_PIN_H0H1,
    NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_clear(test_pin);
//    nrf_gpio_pin_set(test_pin);

    nrf_gpio_cfg(
    BOOST_PNP_SWITCH,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_NOPULL,
    NRF_GPIO_PIN_H0H1,
    NRF_GPIO_PIN_NOSENSE);

    nrf_gpio_cfg(
    BOOST_NPN_SWITCH,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_NOPULL,
    NRF_GPIO_PIN_H0H1,
    NRF_GPIO_PIN_NOSENSE);

    nrf_gpio_cfg(
    BOOST_PIN,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_PULLDOWN,
    NRF_GPIO_PIN_H0H1,
    NRF_GPIO_PIN_NOSENSE);

    //BOOST_PIN not being initialized takes currents!
    nrf_gpio_pin_clear(BOOST_PIN);
    nrf_gpio_pin_clear(BOOST_PNP_SWITCH);
    nrf_gpio_pin_clear(BOOST_NPN_SWITCH);

    




    bsp_board_init(BSP_INIT_LEDS);




    cam_power_up();


    hm_peripheral_init();

    mc6470_acc_init();
    
    mc6470_mag_init();

//    pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[2], PWM_STEPS_NUM , PWM_STEP_DELAY, true);
//
//    pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[3], PWM_STEPS_NUM , PWM_STEP_DELAY, false);
//    
//    pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[4], PWM_STEPS_NUM , PWM_STEP_DELAY, true);
//
//    pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[5], PWM_STEPS_NUM , PWM_STEP_DELAY, false);
//
//    pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[6], PWM_STEPS_NUM , PWM_STEP_DELAY, true);
//
//    pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[7], PWM_STEPS_NUM , PWM_STEP_DELAY, false);
//
//    pwm_boost_zero();
//    pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[5], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, true);
//
//    pwm_boost_zero();


    advertising_start();

    // Enter main loop.
    #if defined(BOARD_PCA10040)
    #if (JPEG_COMPRESS == 0)
    for (;;)
    {
        uint32_t image_size;
        ble_gap_phys_t gap_phys_settings;    
        
        if(m_new_command_received != APP_CMD_NOCOMMAND)
        {
            uint32_t new_command = m_new_command_received;
            m_new_command_received = APP_CMD_NOCOMMAND;
            switch(new_command)
            {
                case APP_CMD_SINGLE_CAPTURE:
                        /*Written by Ali*/
//                    if(ble_bytes_sent_counter >= hm_pixel_counter){
                    if(ble_bytes_sent_counter >= m_length_rx_done){
                        NRF_LOG_INFO("Starting capture...");
                        #if (RELEASE_CODE ==0)
                            printf("Starting capture...\n");
                        #endif
//                        hm_single_capture();

                        if(jpeg_active == 0){
                            hm_peripheral_connected_init();
                            hm_single_capture_spi_832();
//                            hm_peripheral_uninit();

                            //compressed_size = main_compression(argc, argv, normal, fname_jpeg) ;

    //                        while(image_rd_done == 0x00);//wait till we receive hm_image_size number of pixels
                        
    //                        NRF_LOG_INFO("Capture complete: size %i bytes", (uint32_t)(hm_pixel_counter));
    //                        printf("Capture complete: size %i bytes\n", (uint32_t)(hm_pixel_counter));
                            NRF_LOG_INFO("Capture complete: size %i bytes", (uint32_t)(m_length_rx_done));
                        
                            #if (RELEASE_CODE ==0)
                                printf("Capture complete: size %i bytes\n", (uint32_t)(m_length_rx_done));
                            #endif

                            single_capture_flag = 1;

                            #if(CAM_SINGLE_CAPTURE_POWER_FIX == 1)
                            nrf_delay_ms(CAM_TURN_OFF_DELAY);
                            nrf_gpio_pin_clear(CAM_POWER);
                            #endif
                            } 
                        else {
                            NRF_LOG_INFO("Starting capture...");
                        
                            hm_single_capture_spi_832_compressed();
                        
                            NRF_LOG_INFO("Capture complete: size %i bytes", (uint32_t)(compressed_size));
                            #if (RELEASE_CODE ==0)
                            printf("Capture complete: size %i bytes\n", (uint32_t)(compressed_size));
                            #endif
//                            ble_its_img_info_t image_info;
//                            image_info.file_size_bytes = compressed_size;
//                            ble_its_img_info_send(&m_its, &image_info);

                            single_capture_flag = 1;
                        }
                    }
                    break;
            
                case APP_CMD_START_STREAM:
                    NRF_LOG_INFO("Stream mode enabled"); 
                    
                    #if (RELEASE_CODE ==0)
                        printf("Stream mode enabled\n");
                    #endif
                    break;
                
                case APP_CMD_STOP_STREAM:
                    NRF_LOG_INFO("Stream mode disabled");
                    #if (RELEASE_CODE ==0)
                        printf("Stream mode disabled\n");
                    #endif

                    break;
                
                case APP_CMD_CHANGE_RESOLUTION://XXXX to be done later
                        /*  1- stop the camera
                        2- deactivate the timer
                        3- deactivate the SPI
                        4- deactivate the line valid interrupt
                        5- deactivate the frame valid interrupt
                        6- change the resolution and the corresponding camera registers
                        7- change the timer value
                        8- start the streaming like before
                            - activate frame valid interrupt
                            - activate timer
                            - activate the SPI
                            - put the camera on stream mode
                    */
                    NRF_LOG_INFO("Change resolution to mode: %i", (int)m_new_resolution);
                    #if (RELEASE_CODE ==0)
                        printf("Change resolution to mode: %i\n", (int)m_new_resolution);
                    #endif

                    switch(m_new_resolution)
                    {
                        case 0:
                        //100*100
                            change_pic_resolution(100 , 100);
                            break;
                    
                        case 1:
                        //160*120
                            change_pic_resolution(160 , 120);
                            break;

                        case 2:
                            change_pic_resolution(160 , 160);
                            break;

                        case 3:
                            change_pic_resolution(200 , 160);
                            break;

                        case 4:
                            break;
                    
                        case 5:
                            break;
                    
                    } 
                    break;
                
                
                case APP_CMD_CHANGE_PHY:   
                    NRF_LOG_INFO("Attempting to change phy.");                   
                    #if (RELEASE_CODE ==0)
                        printf("Attempting to change phy.\n");
                    #endif
                    gap_phys_settings.tx_phys = (m_new_phy == 0 ? BLE_GAP_PHY_1MBPS : BLE_GAP_PHY_2MBPS);  
                    gap_phys_settings.rx_phys = (m_new_phy == 0 ? BLE_GAP_PHY_1MBPS : BLE_GAP_PHY_2MBPS);  
        
                    sd_ble_gap_phy_update(m_its.conn_handle, &gap_phys_settings);  
                    break;
            
                case APP_CMD_SEND_BLE_PARAMS:
                    ble_its_ble_params_info_send(&m_its, &m_ble_params_info);
                    break;

                case APP_CMD_CHANGE_ANGLE:

                    NRF_LOG_INFO("Change angle to mode: %i", (int)m_new_angle);
                    #if (RELEASE_CODE ==0)
                        printf("Change angle to mode: %i\n", (int)m_new_angle);
                    #endif
//                    app_pwm_uninit(&PWM2);

                    switch(m_new_angle)
                    {
                        case 0:
//                            pwm_boost_step(DUTY_VALUES_PWM_BOOST[1], 5 , 100);
                            /*Make the boost clock pin 0 to turn off the switching transistor*/
                            //pwm_boost_zero();
                            pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[7], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, false);
                            break;
                    
                        case 1:
//                            nrf_gpio_pin_set(BOOST_PNP_SWITCH);
//                            nrf_gpio_pin_clear(BOOST_NPN_SWITCH);
//                            pwm_boost_step_func(DUTY_VALUES_PWM_BOOST[2], PWM_STEPS_NUM , 100);
                            //pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[2], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, true);
                            pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[4], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, false);
                            break;

                        case 2:
//                            nrf_gpio_pin_set(BOOST_NPN_SWITCH);
//                            nrf_gpio_pin_clear(BOOST_PNP_SWITCH);
//                            pwm_boost_step_func(DUTY_VALUES_PWM_BOOST[3], PWM_STEPS_NUM , 100);
                            //pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[3], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, false);
                            pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[2], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, false);
                            break;

                        case 3:
//                            nrf_gpio_pin_set(BOOST_PNP_SWITCH);
//                            nrf_gpio_pin_clear(BOOST_NPN_SWITCH);
//                            pwm_boost_step_func(DUTY_VALUES_PWM_BOOST[4], PWM_STEPS_NUM , 100);
                            //pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[4], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, true);
                            pwm_boost_zero();
                            break;

                        case 4:
//                            nrf_gpio_pin_set(BOOST_NPN_SWITCH);
//                            nrf_gpio_pin_clear(BOOST_PNP_SWITCH);
//                            pwm_boost_step_func(DUTY_VALUES_PWM_BOOST[5], PWM_STEPS_NUM , 100);
                            //pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[5], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, false);
                            pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[2], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, true);
                            break;
                    
                        case 5:
//                            nrf_gpio_pin_set(BOOST_PNP_SWITCH);
//                            nrf_gpio_pin_clear(BOOST_NPN_SWITCH);
//                            pwm_boost_step_func(DUTY_VALUES_PWM_BOOST[6], PWM_STEPS_NUM , 100);
                            //pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[6], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, true);
                            pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[5], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, true);
                            break;

                        case 6:
//                            nrf_gpio_pin_set(BOOST_NPN_SWITCH);
//                            nrf_gpio_pin_clear(BOOST_PNP_SWITCH);
//                            pwm_boost_step_func(DUTY_VALUES_PWM_BOOST[7], PWM_STEPS_NUM , 100);
                            //pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[7], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, false);
                            pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[7], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, true);
                            break;

//                        case 7:
//                            nrf_gpio_pin_clear(BOOST_PNP_SWITCH);
//                            nrf_gpio_pin_clear(BOOST_NPN_SWITCH);
//                            pwm_boost_step_func(DUTY_VALUES_PWM_BOOST[8], 5 , 100);
//                            break;

                    } 
                    break;

                case APP_CMD_JPEG:
                    asm("nop");
                    if (m_new_jpeg == 1){                        
                        jpeg_active = 1;
                    }
                    else{
                        jpeg_active = 0;
                    }
                    break;


                case APP_CMD_ACC:
                    hm_i2c_write_8b(CNTL1,0x88, SLAV_ADDR_MAG);//Activate the Magnetometer in the normal state
//                    hm_i2c_write_8b(CNTL3,0x01, SLAV_ADDR_MAG);//no need to calibrate in the normal state
                    hm_i2c_write_8b(MODE, WAKE, SLAV_ADDR_ACC);
                    cmd_acc_init_flag = true;
                    acc_int_cmd_flag = true;
                    acc_rec_flag = true;
                    acc_int_cmd_sweep = true;//flag to enable the sweeping
                    accel_sr = hm_i2c_read_8b(SR, SLAV_ADDR_ACC);
                    nrf_drv_gpiote_in_event_enable(ACC_INT_PIN, true);

                    break;


                case APP_CMD_WR_REG:
                    if(m_new_device == 1){
                        acc_power_up(); 

                        hm_i2c_write_8b(MODE, 0x00, SLAV_ADDR_ACC);

                        hm_i2c_write_8b(SRTFR, 0x00, SLAV_ADDR_ACC);

                        hm_i2c_write_8b(INTEN, 0x15, SLAV_ADDR_ACC);
                        hm_i2c_write_8b(TAPEN, 0xD5, SLAV_ADDR_ACC);
    

                        hm_i2c_write_8b(TTTRX, 0x01, SLAV_ADDR_ACC);
                        hm_i2c_write_8b(TTTRY, 0x01, SLAV_ADDR_ACC);
                        hm_i2c_write_8b(TTTRZ, 0x0A, SLAV_ADDR_ACC);
    
                        hm_i2c_write_8b(OUTCFG, 0x35, SLAV_ADDR_ACC); // set to 8g range, 14 bits
                        
                        hm_i2c_write_8b(m_new_reg_address_lsb, m_new_reg_value, SLAV_ADDR_ACC);

                        hm_i2c_write_8b(MODE, WAKE, SLAV_ADDR_ACC);

                        accel_sr = hm_i2c_read_8b(SR, SLAV_ADDR_ACC);
                    } else if(m_new_device == 0){
                        uint16_t cam_addr_update = ((uint16_t)m_new_reg_address_msb << 8) | m_new_reg_address_lsb;
                        cam_power_up();
                        hm01b0_init_fixed_rom_qvga_fixed();
                        hm_i2c_write( cam_addr_update, m_new_reg_value);
                    } else { 
//                            hm_i2c_write_8b(CNTL3,0x01, SLAV_ADDR_MAG);
                            hm_i2c_write_8b(m_new_reg_address_lsb, m_new_reg_value, SLAV_ADDR_MAG);
                    }
                    asm("nop");
                    asm("nop");
                    asm("nop");
                    asm("nop");
                    break;

                case APP_CMD_SWEEP_CAM:
                    hm_i2c_write_8b(CNTL1,0x88, SLAV_ADDR_MAG);
//                    hm_i2c_write_8b(CNTL3,0x01, SLAV_ADDR_MAG);
                    hm_i2c_write_8b(MODE, WAKE, SLAV_ADDR_ACC);
                    acc_int_cmd_flag = true;
                    acc_int_cmd_sweep = true;
                    accel_sr = hm_i2c_read_8b(SR, SLAV_ADDR_ACC);
                    nrf_drv_gpiote_in_event_enable(ACC_INT_PIN, true);
                    break;

                case APP_CMD_CAM_ACC_REC://In this mode the last row of the image will be the ACC and MAG data
                    hm_i2c_write_8b(CNTL1,0x88, SLAV_ADDR_MAG);
//                    hm_i2c_write_8b(CNTL3,0x01, SLAV_ADDR_MAG);
                    hm_i2c_write_8b(MODE, WAKE, SLAV_ADDR_ACC);
                    acc_int_cmd_flag = true;
                    acc_int_cmd_sweep = true;
                    acc_rec_flag = true;
                    accel_sr = hm_i2c_read_8b(SR, SLAV_ADDR_ACC);
                    nrf_drv_gpiote_in_event_enable(ACC_INT_PIN, true);
                    break;

            
                default:
                    break;
            }
        }
        


        if(m_stream_mode_active)
        {
            #if (FRAME_VLD_INT == 1)
            if(!stream_first_image_done){
                if(!acc_int_cmd_sweep){
                    stream_first_image_done = true;
                    hm_peripheral_connected_init();
                    if(cmd_acc_init_flag){
                        cmd_acc_init_flag = false;
                        cam_power_up();
                        hm01b0_init_fixed_rom_qvga_fixed();
                        hm_i2c_write(REG_FRAME_LENGTH_LINES_H, 0x02);
                        hm_i2c_write(REG_FRAME_LENGTH_LINES_L, 0x12);   
                    }
                    hm_single_capture_spi_832_stream();
                } else {
                    stream_first_image_done = true;
                    hm_peripheral_connected_init();
//                    cam_power_up();
//                    hm01b0_init_fixed_rom_qvga_fixed();
//                    hm01b0_init_fixed_rom_qvga_fixed_acc();
                    hm_i2c_write(REG_FRAME_LENGTH_LINES_H,CAM_SWEEP_REG_FRAME_LENGTH_LINES_H);
                    hm_i2c_write(REG_FRAME_LENGTH_LINES_L,CAM_SWEEP_REG_FRAME_LENGTH_LINES_L);
                    pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[7], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, false);
                    hm_single_capture_spi_832_stream();
                }
            }

            #else
            if(ble_bytes_sent_counter >= m_length_rx_done) //I don't think this if is needed
//            if(ble_bytes_sent_counter >= compressed_size)
            {

                hm_single_capture_spi_832();

                ble_its_img_info_t image_info;
//                image_info.file_size_bytes = hm_pixel_counter;
                image_info.file_size_bytes = m_length_rx_done;

                ble_its_img_info_send(&m_its, &image_info);
                
            }

            #endif
        }        


//        if(ble_bytes_sent_counter< hm_pixel_counter )
//        if(jpeg_active == 0)
//        {
            if(ble_bytes_sent_counter< m_length_rx_done )
            {
                #if (FRAME_VLD_INT == 1)
                if(!img_info_sent){
                    ble_its_img_info_t image_info;
                    if(acc_rec_flag == true){
                        m_length_rx_done = m_length_rx_done + m_length_rx; 
                    } 
                    image_info.file_size_bytes = m_length_rx_done;
                    ble_its_img_info_send(&m_its, &image_info);
                    img_info_sent = true;
                }

                #endif

                #if (BLE_TEST_PIN_USED == 1)
                nrf_gpio_pin_clear(BLE_START_PIN);
                #endif

                uint32_t ret_code;
                do
                {
                    if(img_data_length == 0)
                    {
                        /*Send the max number of bytes that the m_ble_its_max_data_len lets you*/
                        img_data_length = ((m_length_rx_done - ble_bytes_sent_counter) > m_ble_its_max_data_len ? m_ble_its_max_data_len : (m_length_rx_done - ble_bytes_sent_counter));
                    }

                    ret_code = ble_its_send_file_fragment(&m_its, m_rx_buf+ble_bytes_sent_counter , img_data_length);
                    if(ret_code == NRF_SUCCESS)
                    {
                        ble_bytes_sent_counter = ble_bytes_sent_counter + img_data_length;
                        img_data_length = 0;
                        if(acc_rec_flag == true){
                            packet_sent_acc++;
                            //Read the Acc and Mag data and save it at the end of the image
                            if(packet_sent_acc == 20){
                                packet_sent_acc = 0;
                                mc6470_acc_read();
                            }
                        }
                        

                    }  
                }while(ret_code == NRF_SUCCESS);
             


              #if (FRAME_VLD_INT == 1)
              if((ble_bytes_sent_counter >= m_length_rx_done) && m_stream_mode_active){  
                  img_info_sent = false;
                  if(!acc_int_cmd_flag){
                      APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spis, m_tx_buf, m_length_tx, m_rx_buf, m_length_rx));
                      nrf_drv_gpiote_in_event_enable(FRAME_VLD, true);
                  } 
                  else 
                  {
                      memset(m_rx_buf+total_image_size, 0, m_length_rx);
                      acc_rec_counter = 0;
                      if(!acc_int_cmd_sweep){
                          if(acc_pic_num > 0){
                              APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spis, m_tx_buf, m_length_tx, m_rx_buf, m_length_rx));
                              if(acc_rec_flag == true){
                                  acc_rec_counter = 0;
                                  packet_sent_acc = 0;
                              }
                              nrf_drv_gpiote_in_event_enable(FRAME_VLD, true);
                              acc_pic_num--;
                          } else{
                              hm_i2c_write( REG_MODE_SELECT, 0x00);
                              m_stream_mode_active = false;
                              stream_first_image_done = false;
                              nrf_delay_ms(POR_DELAY);
                              nrf_drv_gpiote_in_event_enable(ACC_INT_PIN, true);
                              accel_sr = hm_i2c_read_8b(SR, SLAV_ADDR_ACC);
                              #if(CAM_CLK_GATING == 1)
                              nrf_drv_timer_disable(&CAM_TIMER);
                              #endif
                              hm_peripheral_uninit();
                          }
                      } else {
                          if(acc_pic_num > 0){
//                              if(acc_pic_num == ACC_PIC_CNT_SWEEP){
//                                  APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spis, m_tx_buf, m_length_tx, m_rx_buf, m_length_rx));
//                                  nrf_drv_gpiote_in_event_enable(FRAME_VLD, true);
//                                  acc_pic_num--;
//                              } else { 
//                                  APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spis, m_tx_buf, m_length_tx, m_rx_buf, m_length_rx));
//                                  app_pwm_uninit(&PWM2);
//                                  pwm_boost_step_func(DUTY_VALUES_PWM_BOOST[7-acc_pic_num], PWM_STEPS_NUM , PWM_STEP_DELAY);
//                                  nrf_drv_gpiote_in_event_enable(FRAME_VLD, true);
//                                  acc_pic_num--;
//                              }
                              APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spis, m_tx_buf, m_length_tx, m_rx_buf, m_length_rx));
                              switch(acc_pic_num){
                                  case 5: 
//                                      pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[7], PWM_STEPS_NUM , PWM_STEP_DELAY, false);
                                      break;

                                  case 4: 
                                      pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[2], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, false);
                                      break;

                                  case 3: 
                                      pwm_boost_zero();
                                      break;

                                  case 2: 
                                      pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[2], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, true);
                                      break;

                                  case 1: 
                                      pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[7], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, true);
                                      break;
                              }
                              acc_pic_num--;
                              nrf_drv_gpiote_in_event_enable(FRAME_VLD, true);
                          } else {
                              hm_i2c_write( REG_MODE_SELECT, 0x00);
                              m_stream_mode_active = false;
                              stream_first_image_done = false;
                              nrf_delay_ms(POR_DELAY);
                              nrf_drv_gpiote_in_event_enable(ACC_INT_PIN, true);
                              accel_sr = hm_i2c_read_8b(SR, SLAV_ADDR_ACC);
                              #if(CAM_CLK_GATING == 1)
                              nrf_drv_timer_disable(&CAM_TIMER);
                              #endif

                              pwm_boost_zero();

                              hm_peripheral_uninit();

                              /*Make the boost clock pin 0 to turn off the switching transistor*/
                              nrf_gpio_cfg(
                              BOOST_PIN,
                              NRF_GPIO_PIN_DIR_OUTPUT,
                              NRF_GPIO_PIN_INPUT_DISCONNECT,
                              NRF_GPIO_PIN_PULLDOWN,
                              NRF_GPIO_PIN_S0S1,
                              NRF_GPIO_PIN_NOSENSE);
                              nrf_gpio_pin_clear(BOOST_PIN);
                          }
                      }
                  }
              } else if(!m_stream_mode_active && (ble_bytes_sent_counter < m_length_rx_done)){
                  hm_i2c_write( REG_MODE_SELECT, 0x00);
              } else if(!m_stream_mode_active && (ble_bytes_sent_counter >= m_length_rx_done)){
                  #if(CAM_CLK_GATING == 1)
                  nrf_drv_timer_disable(&CAM_TIMER);
                  #endif

                  hm_peripheral_uninit();

                  #if(CAM_SINGLE_CAPTURE_POWER_FIX == 1)
                  nrf_delay_ms(CAM_TURN_OFF_DELAY);
                  nrf_gpio_pin_clear(CAM_POWER);
                  #endif
              }
              
              #endif
            }

        if(m_new_command_received == APP_CMD_NOCOMMAND)
        {
            idle_state_handle();
        }
    }
    #else
    for (;;)
    {
        uint32_t image_size;
        ble_gap_phys_t gap_phys_settings;    
        
        if(m_new_command_received != APP_CMD_NOCOMMAND)
        {
            uint32_t new_command = m_new_command_received;
            m_new_command_received = APP_CMD_NOCOMMAND;
            switch(new_command)
            {
                case APP_CMD_SINGLE_CAPTURE:
                        /*Written by Ali*/
//                    if(ble_bytes_sent_counter >= hm_pixel_counter){
                    //if(ble_bytes_sent_counter >= m_length_rx_done){
                    if(ble_bytes_sent_counter >= compressed_size){
                        NRF_LOG_INFO("Starting capture...");
                        printf("Starting capture...\n");
//                        hm_single_capture();
                        hm_single_capture_spi_832_compressed();
                        //compressed_size = main_compression(argc, argv, normal, fname_jpeg) ;

//                        while(image_rd_done == 0x00);//wait till we receive hm_image_size number of pixels
                        
//                        NRF_LOG_INFO("Capture complete: size %i bytes", (uint32_t)(hm_pixel_counter));
//                        printf("Capture complete: size %i bytes\n", (uint32_t)(hm_pixel_counter));
//                        NRF_LOG_INFO("Capture complete: size %i bytes", (uint32_t)(m_length_rx_done));
//                        printf("Capture complete: size %i bytes\n", (uint32_t)(m_length_rx_done));
                        NRF_LOG_INFO("Capture complete: size %i bytes", (uint32_t)(compressed_size));
                        printf("Capture complete: size %i bytes\n", (uint32_t)(compressed_size));
                        ble_its_img_info_t image_info;
//                        image_info.file_size_bytes = hm_pixel_counter;
                        //image_info.file_size_bytes = m_length_rx_done;
                        image_info.file_size_bytes = compressed_size;
                        ble_its_img_info_send(&m_its, &image_info);
                    
//                        // Flush the first byte (or the JPG image will be corrupted)
//                        arducam_mini_2mp_fillBuffer(img_data_buffer, 1);
                    }
                    break;
            
                case APP_CMD_START_STREAM:
                    NRF_LOG_INFO("Stream mode enabled"); 
                    printf("Stream mode enabled\n");
                    break;
                
                case APP_CMD_STOP_STREAM:
                    NRF_LOG_INFO("Stream mode disabled");
                    printf("Stream mode disabled\n");
                    break;
                
                case APP_CMD_CHANGE_RESOLUTION://XXXX to be done later
                    NRF_LOG_INFO("Change resolution to mode: %i", (int)m_new_resolution);
                    printf("Change resolution to mode: %i\n", (int)m_new_resolution);
                    switch(m_new_resolution)
                    {
                        case 0:
//                            arducam_mini_2mp_setResolution(OV2640_160x120);
                            break;
                    
                        case 1:
//                            arducam_mini_2mp_setResolution(OV2640_320x240);
                            break;

                        case 2:
//                            arducam_mini_2mp_setResolution(OV2640_640x480);
                            break;

                        case 3:
//                            arducam_mini_2mp_setResolution(OV2640_800x600);
                            break;

                        case 4:
//                            arducam_mini_2mp_setResolution(OV2640_1024x768);
                            break;
                    
                        case 5:
//                            arducam_mini_2mp_setResolution(OV2640_1600x1200);
                            break;
                    
                    } 
                    break;
                
                case APP_CMD_CHANGE_PHY:   
                    NRF_LOG_INFO("Attempting to change phy.");
                    printf("Attempting to change phy.\n");
                    gap_phys_settings.tx_phys = (m_new_phy == 0 ? BLE_GAP_PHY_1MBPS : BLE_GAP_PHY_2MBPS);  
                    gap_phys_settings.rx_phys = (m_new_phy == 0 ? BLE_GAP_PHY_1MBPS : BLE_GAP_PHY_2MBPS);  
        
                    sd_ble_gap_phy_update(m_its.conn_handle, &gap_phys_settings);  
                    break;
            
                case APP_CMD_SEND_BLE_PARAMS:
                    ble_its_ble_params_info_send(&m_its, &m_ble_params_info);
                    break;
            
                default:
                    break;
            }
        }
        

        if(m_stream_mode_active)
        {
//            if(ble_bytes_sent_counter >= hm_pixel_counter)
//            if(ble_bytes_sent_counter >= m_length_rx_done)
            if(ble_bytes_sent_counter >= compressed_size)
            {
//                hm_single_capture();
                hm_single_capture_spi_832_compressed();
                //compressed_size = main_compression(argc, argv, normal, fname_jpeg) ;

//                while(image_rd_done == 0x00);//wait till we receive hm_image_size number of pixels

                ble_its_img_info_t image_info;
//                image_info.file_size_bytes = hm_pixel_counter;
//                image_info.file_size_bytes = m_length_rx_done;
                image_info.file_size_bytes = compressed_size;
                ble_its_img_info_send(&m_its, &image_info);
                
                // Flush the first byte (or the JPG image will be corrupted)
//                arducam_mini_2mp_fillBuffer(img_data_buffer, 1);
            }
        }        
//        if(img_data_length > 0 || arducam_mini_2mp_bytesAvailable() > 0)
//        {
//            uint32_t ret_code;
//            do
//            {
//                if(img_data_length == 0)
//                {
//                    img_data_length = arducam_mini_2mp_fillBuffer(img_data_buffer, m_ble_its_max_data_len);
//                }
//                ret_code = ble_its_send_file_fragment(&m_its, img_data_buffer, img_data_length);
//                if(ret_code == NRF_SUCCESS)
//                {
//                    img_data_length = 0;
//                }  
//            }while(ret_code == NRF_SUCCESS);    
//        }

//        if(ble_bytes_sent_counter< hm_pixel_counter )
//        if(ble_bytes_sent_counter< m_length_rx_done )
        if(ble_bytes_sent_counter< compressed_size )
        {
            uint32_t ret_code;
            do
            {
                if(img_data_length == 0)
                {
                    /*Send the max number of bytes that the m_ble_its_max_data_len lets you*/
//                    img_data_length = ((hm_pixel_counter - ble_bytes_sent_counter) > m_ble_its_max_data_len ? m_ble_its_max_data_len : (hm_pixel_counter - ble_bytes_sent_counter));
//                    img_data_length = ((m_length_rx_done - ble_bytes_sent_counter) > m_ble_its_max_data_len ? m_ble_its_max_data_len : (m_length_rx_done - ble_bytes_sent_counter));
                    img_data_length = ((compressed_size - ble_bytes_sent_counter) > m_ble_its_max_data_len ? m_ble_its_max_data_len : (compressed_size - ble_bytes_sent_counter));
                }

                /*May have error*/
//                ret_code = ble_its_send_file_fragment(&m_its, img_data_buffer , img_data_length);
//                printf("Change resolution to mode: %i\n", *image_mem);
                //ret_code = ble_its_send_file_fragment(&m_its, m_rx_buf+ble_bytes_sent_counter , img_data_length);
                ret_code = ble_its_send_file_fragment(&m_its, jpeg_imag_mem+ble_bytes_sent_counter , img_data_length);
//                ret_code = ble_its_send_file_fragment(&m_its, image_mem+ble_bytes_sent_counter , img_data_length);
//                ret_code = ble_its_send_file_fragment(&m_its, image_mem2+ble_bytes_sent_counter , img_data_length);
                if(ret_code == NRF_SUCCESS)
                {
                    ble_bytes_sent_counter = ble_bytes_sent_counter + img_data_length;
                    img_data_length = 0;
                }  
            }while(ret_code == NRF_SUCCESS);     
        }

//        m_length_rx_done = 0;
        if(m_new_command_received == APP_CMD_NOCOMMAND)
        {
            idle_state_handle();
        }
    }

    #endif
    #endif


    #if defined(BOARD_PCA10056)
    for (;;)
    {
        uint32_t image_size;
        ble_gap_phys_t gap_phys_settings;    
        
        if(m_new_command_received != APP_CMD_NOCOMMAND)
        {
            uint32_t new_command = m_new_command_received;
            m_new_command_received = APP_CMD_NOCOMMAND;
            switch(new_command)
            {
                case APP_CMD_SINGLE_CAPTURE:
                
//                    if(arducam_mini_2mp_bytesAvailable() == 0)
//                    {
//                        NRF_LOG_INFO("Starting capture...");
//                        arducam_mini_2mp_startSingleCapture();
//                        image_size = arducam_mini_2mp_bytesAvailable();
//                        NRF_LOG_INFO("Capture complete: size %i bytes", (int)(image_size));
//                        ble_its_img_info_t image_info;
//                        image_info.file_size_bytes = image_size - 1;
//                        ble_its_img_info_send(&m_its, &image_info);
//                    
//                        // Flush the first byte (or the JPG image will be corrupted)
//                        arducam_mini_2mp_fillBuffer(img_data_buffer, 1);

                        /*Written by Ali*/
//                    if(ble_bytes_sent_counter >= hm_pixel_counter){
                    if(ble_bytes_sent_counter >= m_length_rx_done){
                        NRF_LOG_INFO("Starting capture...");
                        printf("Starting capture...\n");
//                        hm_single_capture();
                        hm_single_capture_spi_832();

//                        while(image_rd_done == 0x00);//wait till we receive hm_image_size number of pixels
                        
//                        NRF_LOG_INFO("Capture complete: size %i bytes", (uint32_t)(hm_pixel_counter));
//                        printf("Capture complete: size %i bytes\n", (uint32_t)(hm_pixel_counter));
                        NRF_LOG_INFO("Capture complete: size %i bytes", (uint32_t)(m_length_rx));
                        printf("Capture complete: size %i bytes\n", (uint32_t)(m_length_rx));
                        ble_its_img_info_t image_info;
//                        image_info.file_size_bytes = hm_pixel_counter;
                        image_info.file_size_bytes = m_length_rx;
                        ble_its_img_info_send(&m_its, &image_info);
                    
//                        // Flush the first byte (or the JPG image will be corrupted)
//                        arducam_mini_2mp_fillBuffer(img_data_buffer, 1);
                    }
                    break;
            
                case APP_CMD_START_STREAM:
                    NRF_LOG_INFO("Stream mode enabled"); 
                    printf("Stream mode enabled\n");
                    break;
                
                case APP_CMD_STOP_STREAM:
                    NRF_LOG_INFO("Stream mode disabled");
                    printf("Stream mode disabled\n");
                    break;
                
                case APP_CMD_CHANGE_RESOLUTION://XXXX to be done later
                    NRF_LOG_INFO("Change resolution to mode: %i", (int)m_new_resolution);
                    printf("Change resolution to mode: %i\n", (int)m_new_resolution);
                    switch(m_new_resolution)
                    {
                        case 0:
//                            arducam_mini_2mp_setResolution(OV2640_160x120);
                            break;
                    
                        case 1:
//                            arducam_mini_2mp_setResolution(OV2640_320x240);
                            break;

                        case 2:
//                            arducam_mini_2mp_setResolution(OV2640_640x480);
                            break;

                        case 3:
//                            arducam_mini_2mp_setResolution(OV2640_800x600);
                            break;

                        case 4:
//                            arducam_mini_2mp_setResolution(OV2640_1024x768);
                            break;
                    
                        case 5:
//                            arducam_mini_2mp_setResolution(OV2640_1600x1200);
                            break;
                    
                    } 
                    break;
                
                case APP_CMD_CHANGE_PHY:   
                    NRF_LOG_INFO("Attempting to change phy.");
                    printf("Attempting to change phy.\n");
                    gap_phys_settings.tx_phys = (m_new_phy == 0 ? BLE_GAP_PHY_1MBPS : BLE_GAP_PHY_2MBPS);  
                    gap_phys_settings.rx_phys = (m_new_phy == 0 ? BLE_GAP_PHY_1MBPS : BLE_GAP_PHY_2MBPS);  
        
                    sd_ble_gap_phy_update(m_its.conn_handle, &gap_phys_settings);  
                    break;
            
                case APP_CMD_SEND_BLE_PARAMS:
                    ble_its_ble_params_info_send(&m_its, &m_ble_params_info);
                    break;
            
                default:
                    break;
            }
        }
        
//        if(m_stream_mode_active)
//        {
//            if(img_data_length == 0 && arducam_mini_2mp_bytesAvailable() == 0)
//            {
//                arducam_mini_2mp_startSingleCapture();
//
//                image_size = arducam_mini_2mp_bytesAvailable();
//                
//                ble_its_img_info_t image_info;
//                image_info.file_size_bytes = image_size - 1;
//                ble_its_img_info_send(&m_its, &image_info);
//                
//                // Flush the first byte (or the JPG image will be corrupted)
//                arducam_mini_2mp_fillBuffer(img_data_buffer, 1);
//            }
//        }

        if(m_stream_mode_active)
        {
//            if(ble_bytes_sent_counter >= hm_pixel_counter)
            if(ble_bytes_sent_counter >= m_length_rx)
            {
//                hm_single_capture();
                hm_single_capture_spi_832();

//                while(image_rd_done == 0x00);//wait till we receive hm_image_size number of pixels

                ble_its_img_info_t image_info;
//                image_info.file_size_bytes = hm_pixel_counter;
                image_info.file_size_bytes = m_length_rx;
                ble_its_img_info_send(&m_its, &image_info);
                
                // Flush the first byte (or the JPG image will be corrupted)
//                arducam_mini_2mp_fillBuffer(img_data_buffer, 1);
            }
        }        
//        if(img_data_length > 0 || arducam_mini_2mp_bytesAvailable() > 0)
//        {
//            uint32_t ret_code;
//            do
//            {
//                if(img_data_length == 0)
//                {
//                    img_data_length = arducam_mini_2mp_fillBuffer(img_data_buffer, m_ble_its_max_data_len);
//                }
//                ret_code = ble_its_send_file_fragment(&m_its, img_data_buffer, img_data_length);
//                if(ret_code == NRF_SUCCESS)
//                {
//                    img_data_length = 0;
//                }  
//            }while(ret_code == NRF_SUCCESS);    
//        }

//        if(ble_bytes_sent_counter< hm_pixel_counter )
        if(ble_bytes_sent_counter< m_length_rx_done )
        {
            uint32_t ret_code;
            do
            {
                if(img_data_length == 0)
                {
                    /*Send the max number of bytes that the m_ble_its_max_data_len lets you*/
//                    img_data_length = ((hm_pixel_counter - ble_bytes_sent_counter) > m_ble_its_max_data_len ? m_ble_its_max_data_len : (hm_pixel_counter - ble_bytes_sent_counter));
                    img_data_length = ((m_length_rx - ble_bytes_sent_counter) > m_ble_its_max_data_len ? m_ble_its_max_data_len : (m_length_rx - ble_bytes_sent_counter));
                }

                /*May have error*/
//                ret_code = ble_its_send_file_fragment(&m_its, img_data_buffer , img_data_length);
//                printf("Change resolution to mode: %i\n", *image_mem);
                ret_code = ble_its_send_file_fragment(&m_its, m_rx_buf+ble_bytes_sent_counter , img_data_length);
//                ret_code = ble_its_send_file_fragment(&m_its, image_mem+ble_bytes_sent_counter , img_data_length);
//                ret_code = ble_its_send_file_fragment(&m_its, image_mem2+ble_bytes_sent_counter , img_data_length);
                if(ret_code == NRF_SUCCESS)
                {
                    ble_bytes_sent_counter = ble_bytes_sent_counter + img_data_length;
                    img_data_length = 0;
                }  
            }while(ret_code == NRF_SUCCESS);     
        }

//        m_length_rx_done = 0;
        if(m_new_command_received == APP_CMD_NOCOMMAND)
        {
            idle_state_handle();
        }
    }
    #endif

    #endif
}



//int main(void){
//    //Camera initializations
//    hm_peripheral_init();
//    hm_single_capture();
//    while(image_rd_done == 0x00);//wait till we receive hm_image_size number of pixels
//    hm_single_capture();
//    while(image_rd_done == 0x00);//wait till we receive hm_image_size number of pixels
//    hm_single_capture();
//    while(image_rd_done == 0x00);//wait till we receive hm_image_size number of pixels
//    hm_single_capture();
//    while(image_rd_done == 0x00);//wait till we receive hm_image_size number of pixels
//    while (true)
//    {
//        // Do nothing.
////       sw_CAM_SPI_CS = nrf_gpio_pin_read(CAM_SPI_CS);
////       printf("sw_CAM_SPI_CS: %x \n", gpio_read_cam());
//    }
//
//
//}




/**
 * @}
 */
