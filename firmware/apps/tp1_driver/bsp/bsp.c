/**
*******************************************************************************
* @file           : bsp_xxx.c
* @brief          : Description of C implementation module
* @author         : 
* @date           : dd/mm/aaaa
*******************************************************************************
* @attention
*
* Copyright (c) <date> <name>. All rights reserved.
*
*/
/******************************************************************************
    Includes
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "bsp.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/******************************************************************************
    Defines and constants
******************************************************************************/
static const char *TAG = "[BSP]";

#define BSP_I2C_SCL_PIN     	(GPIO_NUM_12)
#define BSP_I2C_SDA_PIN     	(GPIO_NUM_13)
#define BSP_I2C_PORT            (0)
#define BSP_I2C_FREQ_HZ         (100000)       // 100 kHz (Standard Mode para ADS1115)

/******************************************************************************
    Data types
******************************************************************************/

/******************************************************************************
    Local variables
******************************************************************************/
static i2c_master_bus_handle_t i2c_bus_handle = NULL;
static i2c_master_dev_handle_t ads1115_dev_handle = NULL;
/******************************************************************************
    Local function prototypes
******************************************************************************/

/******************************************************************************
    Local function definitions
******************************************************************************/

/******************************************************************************
    Public function definitions
******************************************************************************/
bsp_ret_t bsp_init(void)
{
    if (i2c_bus_handle != NULL) 
	{
        return BSP_OK;
    }

    i2c_master_bus_config_t bus_config = 
	{
        .i2c_port = BSP_I2C_PORT,
        .sda_io_num = BSP_I2C_SDA_PIN,
        .scl_io_num = BSP_I2C_SCL_PIN,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    esp_err_t err = i2c_new_master_bus(&bus_config, &i2c_bus_handle);
    if (err != ESP_OK) 
	{
        ESP_LOGE(TAG, "Error al inicializar el Bus Master I2C: %s", esp_err_to_name(err));
        return BSP_ERROR;
    }

	i2c_device_config_t ads_dev_config = 
	{
	    .device_address = BSP_I2C_ADDR_ADS1115,
	    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
	    .scl_speed_hz = BSP_I2C_FREQ_HZ,
	};
	
    err = i2c_master_bus_add_device(i2c_bus_handle, &ads_dev_config, &ads1115_dev_handle);
    if (err != ESP_OK) 
	{
        ESP_LOGE(TAG, "Error al añadir el dispositivo al bus");
        return BSP_ERROR;
    }

    ESP_LOGI(TAG, "Bus I2C init ok");
    return BSP_OK;
}

bsp_ret_t bsp_i2c_write(uint8_t dev_addr, uint8_t reg_addr, const uint8_t *p_data, uint16_t len)
{
    if (i2c_bus_handle == NULL)
	{
		return BSP_ERROR;
	}

    /* TODO: implementar una funcion para obtener el dev_handle 
	cuando haya más perifericos i2c */

    // Armamos el buffer: [Dirección del Registro interno] + [Bloque de datos]
    uint8_t write_buffer[len + 1];
    write_buffer[0] = reg_addr;
    
	for (uint16_t i = 0; i < len; i++) 
	{
        write_buffer[i + 1] = p_data[i];
    }

    esp_err_t err = i2c_master_transmit(ads1115_dev_handle, write_buffer, sizeof(write_buffer), 50);

    return (err == ESP_OK) ? BSP_OK : BSP_ERROR;
}

bsp_ret_t bsp_i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *p_data, uint16_t len)
{
	if (i2c_bus_handle == NULL)
	{
		return BSP_ERROR;
	}

	/* TODO: implementar una funcion para obtener el dev_handle 
	cuando haya más perifericos i2c */

    uint8_t reg_buf[1] = { reg_addr };
	esp_err_t err = i2c_master_transmit_receive(ads1115_dev_handle, reg_buf, sizeof(reg_buf), p_data, len, 50);

    return (err == ESP_OK) ? BSP_OK : BSP_ERROR;
}

void bsp_delay_ms(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

