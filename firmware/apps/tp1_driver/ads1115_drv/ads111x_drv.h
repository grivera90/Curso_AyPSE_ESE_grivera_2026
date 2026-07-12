/**
*******************************************************************************
* @file           : ads111x_drv.h
* @brief          : Description of header file
* @author         : 
* @date           : dd/mm/aaaa
*******************************************************************************
* @attention
*
* Copyright (c) <date> <name>. All rights reserved.
*
*/
#ifndef __ADS111x_DRV_H__
#define __ADS111x_DRV_H__
/******************************************************************************
        Includes
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
/******************************************************************************
        Constants
******************************************************************************/

/******************************************************************************
        Data types
******************************************************************************/
/**
*	@brief Possibles Return Values.
*/
typedef enum
{
	ADS111x_OK = 0,
	ADS111x_BAD_ARGS,
	ADS111x_ERROR
	
} ads111x_ret_t;

/**
*	@brief Address Settings
*/
typedef enum
{
	ADS111x_ADDR_GND = 0x48,
	ADS111x_ADDR_VCC,
	ADS111x_ADDR_SDA,
	ADS111x_ADDR_SCL
	
} ads111x_addr_t;

/**
*	@brief Data Rate Settings (128 by default)
*/
typedef enum
{
	ADS111x_DATA_RATE_8 = 0,
	ADS111x_DATA_RATE_16,
	ADS111x_DATA_RATE_32,
	ADS111x_DATA_RATE_64,
	ADS111x_DATA_RATE_128,
	ADS111x_DATA_RATE_250,
	ADS111x_DATA_RATE_475,
	ADS111x_DATA_RATE_860
	
} ads111x_data_rate_t;

/**
* @brief Programmable Gain amplifier (PGA) options.
*/
typedef enum
{
    ADS111X_GAIN_6V144 = 0, //!< +-6.144V
    ADS111X_GAIN_4V096,     //!< +-4.096V
    ADS111X_GAIN_2V048,     //!< +-2.048V (default)
    ADS111X_GAIN_1V024,     //!< +-1.024V
    ADS111X_GAIN_0V512,     //!< +-0.512V
    ADS111X_GAIN_0V256,     //!< +-0.256V
    ADS111X_GAIN_0V256_2,   //!< +-0.256V (same as ADS111X_GAIN_0V256)
    ADS111X_GAIN_0V256_3    //!< +-0.256V (same as ADS111X_GAIN_0V256)
	
} ads111x_gain_t;

/**
* @brief Channels MUX (ONLY ADS1115!)
*/
typedef enum
{
	ADS111X_MUX_0_1 = 0, //!< positive = AIN0, negative = AIN1 (default)
	ADS111X_MUX_0_3,     //!< positive = AIN0, negative = AIN3
	ADS111X_MUX_1_3,     //!< positive = AIN1, negative = AIN3
	ADS111X_MUX_2_3,     //!< positive = AIN2, negative = AIN3
	ADS111X_MUX_0_GND,   //!< positive = AIN0, negative = GND
	ADS111X_MUX_1_GND,   //!< positive = AIN1, negative = GND
	ADS111X_MUX_2_GND,   //!< positive = AIN2, negative = GND
	ADS111X_MUX_3_GND    //!< positive = AIN3, negative = GND
		
} ads111x_mux_t;

/**
*	@brief Operational Mode. 
*/
typedef enum
{
	ADS111X_MODE_CONTINUOUS = 0,	//!< Continuous conversion mode
	ADS111X_MODE_SINGLE_SHOT    	//!< Power-down single-shot mode (default)	
	
} ads111x_mode_t;

/**
*	@brief Comparator Mode (ADS1114/5 ONLY). 
*/
typedef enum
{
    ADS111X_COMP_MODE_NORMAL = 0, 	//!< Traditional comparator with hysteresis (default)
    ADS111X_COMP_MODE_WINDOW      	//!< Window comparator
	
} ads111x_comp_mode_t;

/**
*	@brief Comparator Polarity (ADS1114/5 ONLY). 
*/
typedef enum
{
    ADS111X_COMP_POLARITY_LOW = 0, //!< Active low (default)
    ADS111X_COMP_POLARITY_HIGH     //!< Active high
	
} ads111x_comp_polarity_t;

/**
*	@brief Comparator latch (ADS1114/5 only)
*/
typedef enum
{
    ADS111X_COMP_LATCH_DISABLED = 0, //!< Non-latching comparator (default)
    ADS111X_COMP_LATCH_ENABLED       //!< Latching comparator
	
} ads111x_comp_latch_t;

/**
*	@brief Comparator queue
*/
typedef enum
{
    ADS111X_COMP_QUEUE_1 = 0,   //!< Assert ALERT/RDY pin after one conversion
    ADS111X_COMP_QUEUE_2,       //!< Assert ALERT/RDY pin after two conversions
    ADS111X_COMP_QUEUE_4,       //!< Assert ALERT/RDY pin after four conversions
    ADS111X_COMP_QUEUE_DISABLED //!< Disable comparator (default)
	
} ads111x_comp_queue_t;

/**
 * @brief Hardware Proxy.
 */
typedef struct
{
    int (*i2c_write)(uint8_t slave_addr, uint8_t reg_addr, const uint8_t *data, size_t data_len);
    int (*i2c_read)(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, size_t data_len);
    void (*gpio_set_interrupt)(bool enable);
    
} ads111x_hw_proxy_t;

/**
*	@brief Callback Function Type.
*/
typedef void (*ads111x_cb_t)(void *ctx, int16_t conversion_result);

/**
*	@brief Device Handler
*/
typedef struct
{
	ads111x_hw_proxy_t hw;
	ads111x_addr_t addr;
	ads111x_cb_t ads111x_cb;
	
} ads111x_drv_t;
/******************************************************************************
        Public function prototypes
******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the ADS111x driver instance.
 * @param drv Pointer to the driver instance structure.
 * @return ads111x_ret_t: See \p ads111x_ret_t in Data types section.
 */
ads111x_ret_t ads111x_drv_init(ads111x_drv_t *drv);

/**
 * @brief Starts a single-shot analog-to-digital conversion.
 * @param drv Pointer to the driver instance structure.
 * @return ads111x_ret_t: See \p ads111x_ret_t in Data types section.
 */
ads111x_ret_t ads111x_drv_start_conversion(ads111x_drv_t *drv);

/**
 * @brief Reads the raw 16-bit digital conversion result.
 * @param drv Pointer to the driver instance structure.
 * @param value Pointer to the variable where the conversion value will be stored.
 * @return ads111x_ret_t: See \p ads111x_ret_t in Data types section.
 */
ads111x_ret_t ads111x_drv_get_conversion(ads111x_drv_t *drv, int16_t *value);

/**
 * @brief Configures the input multiplexer (MUX) selection.
 * @param drv Pointer to the driver instance structure.
 * @param mux The multiplexer configuration to set.
 * @return ads111x_ret_t: See \p ads111x_ret_t in Data types section.
 */
ads111x_ret_t ads111x_drv_set_input(ads111x_drv_t *drv, ads111x_mux_t mux);

/**
 * @brief Retrieves the current input multiplexer (MUX) configuration.
 * @param drv Pointer to the driver instance structure.
 * @param mux Pointer to the variable where the current MUX configuration will be stored.
 * @return ads111x_ret_t: See \p ads111x_ret_t in Data types section.
 */
ads111x_ret_t ads111x_drv_get_input(ads111x_drv_t *drv, ads111x_mux_t *mux);

/**
 * @brief Sets the Programmable Gain Amplifier (PGA) value.
 * @param drv Pointer to the driver instance structure.
 * @param pga_value The desired gain configuration value to set.
 * @return ads111x_ret_t: See \p ads111x_ret_t in Data types section.
 */
ads111x_ret_t ads111x_drv_set_pga(ads111x_drv_t *drv, ads111x_gain_t pga_value);

/**
 * @brief Retrieves the current Programmable Gain Amplifier (PGA) configuration.
 * @param drv Pointer to the driver instance structure.
 * @param pga_value Pointer to the variable where the current PGA configuration will be stored.
 * @return ads111x_ret_t: See \p ads111x_ret_t in Data types section.
 */
ads111x_ret_t ads111x_drv_get_pga(ads111x_drv_t *drv, ads111x_gain_t *pga_value);

/**
 * @brief Sets the device operational mode.
 * @param drv Pointer to the driver instance structure.
 * @param mode The desired operational mode to set.
 * @return ads111x_ret_t: See \p ads111x_ret_t in Data types section.
 */
ads111x_ret_t ads111x_drv_set_mode(ads111x_drv_t *drv, ads111x_mode_t mode);

/**
 * @brief Retrieves the current operational mode configuration.
 * @param drv Pointer to the driver instance structure.
 * @param mode Pointer to the variable where the current operational mode will be stored.
 * @return ads111x_ret_t: See \p ads111x_ret_t in Data types section.
 */
ads111x_ret_t ads111x_drv_get_mode(ads111x_drv_t *drv, ads111x_mode_t *mode);

/**
 * @brief Configures the data rate (Samples Per Second).
 * @param drv Pointer to the driver instance structure.
 * @param data_rate The desired data rate configuration to set.
 * @return ads111x_ret_t: See \p ads111x_ret_t in Data types section.
 */
ads111x_ret_t ads111x_drv_set_data_rate(ads111x_drv_t *drv, ads111x_data_rate_t data_rate);

/**
 * @brief Retrieves the current data rate configuration.
 * @param drv Pointer to the driver instance structure.
 * @param data_rate Pointer to the variable where the current data rate will be stored.
 * @return ads111x_ret_t: See \p ads111x_ret_t in Data types section.
 */
ads111x_ret_t ads111x_drv_get_data_rate(ads111x_drv_t *drv, ads111x_data_rate_t *data_rate);

/**
 * @brief Enables the ALERT/RDY pin hardware interrupt functionality.
 * @param drv Pointer to the driver instance structure.
 * @return ads111x_ret_t: See \p ads111x_ret_t in Data types section.
 */
ads111x_ret_t ads111x_drv_enable_int(ads111x_drv_t *drv);

/**
 * @brief Disables the ALERT/RDY pin hardware interrupt functionality.
 * @param drv Pointer to the driver instance structure.
 * @return ads111x_ret_t: See \p ads111x_ret_t in Data types section.
 */
ads111x_ret_t ads111x_drv_disable_int(ads111x_drv_t *drv);

/**
 * @brief Registers a user callback function to handle conversion events.
 * @param drv Pointer to the driver instance structure.
 * @param cb User callback function pointer matching the ads111x_cb_t signature.
 */
void ads111x_drv_set_cb(ads111x_drv_t *drv, ads111x_cb_t cb);

/**
 * @brief Interrupt service handler triggered from the external GPIO interrupt.
 * @param drv Pointer to the driver instance structure.
 * @param ctx Optional user application context pointer.
 * @return ads111x_ret_t: See \p ads111x_ret_t in Data types section.
 */
ads111x_ret_t ads111x_drv_interrupt_handler(ads111x_drv_t *drv, void *ctx);

/* Add more later */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // EOF __ADS1115_DRV_H__

