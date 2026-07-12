/**
*******************************************************************************
* @file           : main.c
* @brief          : Description of C implementation module
* @author         : 
* @date           : dd/mm/aaaa
*******************************************************************************
* @attention
*
* Copyright (c) <date> grivera. All rights reserved.
*
*/
/******************************************************************************
    Includes
******************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bsp.h"
#include "ads111x_drv.h"
/******************************************************************************
    Defines and constants
******************************************************************************/
static const char *TAG = "[ADC_TEST]";
static ads111x_drv_t adc_sensor;
/******************************************************************************
    Data types
******************************************************************************/

/******************************************************************************
    Local variables
******************************************************************************/

/******************************************************************************
    Local function prototypes
******************************************************************************/
static int ads_i2c_write_adapter(uint8_t slave_addr, uint8_t reg_addr, const uint8_t *data, size_t data_len);
static int ads_i2c_read_adapter(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, size_t data_len);
/******************************************************************************
    Local function definitions
******************************************************************************/
// Wrappers Pueden no estar, puede existir más adelante un modulo que gestione esta parte, etc.
static int ads_i2c_write_adapter(uint8_t slave_addr, uint8_t reg_addr, const uint8_t *data, size_t data_len) 
{
    return (int)bsp_i2c_write(slave_addr, reg_addr, data, (uint16_t)data_len);
}

static int ads_i2c_read_adapter(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, size_t data_len) 
{
    return (int)bsp_i2c_read(slave_addr, reg_addr, data, (uint16_t)data_len);
}
/******************************************************************************
    Public function definitions
******************************************************************************/
void app_main(void)
{
    ESP_LOGI(TAG, "ADS1115 Driver Init...");

    if (!bsp_init()) 
	{
        ESP_LOGE(TAG, "BSP Error. System stopped");
        return;
    }

    adc_sensor.addr = ADS111x_ADDR_GND;
    adc_sensor.hw.i2c_write = ads_i2c_write_adapter;
    adc_sensor.hw.i2c_read  = ads_i2c_read_adapter;
    adc_sensor.hw.gpio_set_interrupt = NULL;
    adc_sensor.ads111x_cb = NULL;

    if (ads111x_drv_init(&adc_sensor) != ADS111x_OK) 
	{
        ESP_LOGE(TAG, "ADS1115 Driver Init Error");
        while (1) 
		{ 
			bsp_delay_ms(1000); 
		}
    }
	
    ESP_LOGI(TAG, "ADS1115 Init OK");

    ads111x_drv_set_input(&adc_sensor, ADS111X_MUX_0_GND);
    ads111x_drv_set_pga(&adc_sensor, ADS111X_GAIN_4V096);

    while (1)
	{
        int16_t raw_adc_value = 0;
        float voltage_mv = 0.0f;

        ads111x_drv_start_conversion(&adc_sensor);
		bsp_delay_ms(20); 

        if (ads111x_drv_get_conversion(&adc_sensor, &raw_adc_value) == ADS111x_OK) 
		{
            // Cálculo a milivoltios: El ADS1115 cuenta con 16 bits de resolución con signo. 
            // Para el fondo de escala de ±4.096V, el valor máximo positivo (32767 cuentas) equivale a 4096 mV.
            // Factor de escala LSB = 4096 mV / 32767 cuentas = 0.125 mV por cuenta.
            voltage_mv = (float)raw_adc_value * 0.125f;

            // Mostrar el valor crudo leído y el voltaje calculado en consola
            printf("ADC Raw: %d | Voltaje Calculado: %.2f mV (%.3f V)\n", raw_adc_value, voltage_mv, voltage_mv / 1000.0f);
        } else 
		{
            ESP_LOGE(TAG, "Error al intentar leer los datos de conversión desde el bus I2C");
        }

        bsp_delay_ms(1000);
    }
}
/******************************************************************************
    End of File
******************************************************************************/