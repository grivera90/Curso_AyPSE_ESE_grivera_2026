/**
*******************************************************************************
* @file           : bsp.h
* @brief          : Description of header file
* @author         : 
* @date           : dd/mm/aaaa
*******************************************************************************
* @attention
*
* Copyright (c) <date> <name>. All rights reserved.
*
*/
#ifndef __BSP_H__
#define __BSP_H__
/******************************************************************************
        Includes
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
        Constants
******************************************************************************/ 
#define BSP_I2C_ADDR_ADS1115   (0x48)
/******************************************************************************
        Data types
******************************************************************************/
typedef enum
{
	BSP_OK = 0,
	BSP_ERROR = -1
	
} bsp_ret_t;
/******************************************************************************
        Public function prototypes
******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Function to init resources of your board.
 * @return bsp_ret_t: See \p bsp_ret_t in Data types section. 
 */
bsp_ret_t bsp_init(void);

/**
 * @brief Transmits data to a register of a peripheral slave device on the I2C bus.
 * @param dev_addr 7-bit I2C address of the slave device.
 * @param reg_addr Address of the internal register to be written to.
 * @param p_data Pointer to the buffer containing the data to transmit.
 * @param len Number of bytes to write.
 * @return bsp_ret_t: See \p bsp_ret_t in Data types section.
 */
bsp_ret_t bsp_i2c_write(uint8_t dev_addr, uint8_t reg_addr, const uint8_t *p_data, uint16_t len);

/**
 * @brief Reads data from a register of a peripheral slave device using Repeated Start.
 * @param dev_addr 7-bit I2C address of the slave device.
 * @param reg_addr Address of the internal register to be read from.
 * @param p_data Pointer to the buffer where the read data will be stored.
 * @param len Number of bytes to read.
 * @return bsp_ret_t: See \p bsp_ret_t in Data types section.
 */
bsp_ret_t bsp_i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *p_data, uint16_t len);

/**
 * @brief Blocking delay in milliseconds for the current task (FreeRTOS wrapper).
 * @param ms Time in milliseconds.
 */
void bsp_delay_ms(uint32_t ms);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // EOF __BSP_H__

