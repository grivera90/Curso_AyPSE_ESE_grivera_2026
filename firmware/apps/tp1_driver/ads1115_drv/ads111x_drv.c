/**
*******************************************************************************
* @file           : ads111x_drv.c
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

#include "ads111x_drv.h"
/******************************************************************************
    Defines and constants
******************************************************************************/
#define REG_CONVERSION 				(0)
#define REG_CONFIG     				(1)
#define REG_THRESH_L   				(2)
#define REG_THRESH_H   				(3)

/**
 * @brief Bits y Máscaras del Registro de Configuración (REG_CONFIG)
 */
#define ADS111x_OS_OFFSET          15
#define ADS111x_OS_MASK            (0x01U << ADS111x_OS_OFFSET)

#define ADS111x_MUX_OFFSET         12
#define ADS111x_MUX_MASK           (0x07U << ADS111x_MUX_OFFSET)

#define ADS111x_PGA_OFFSET         9
#define ADS111x_PGA_MASK           (0x07U << ADS111x_PGA_OFFSET)

#define ADS111x_MODE_OFFSET        8
#define ADS111x_MODE_MASK          (0x01U << ADS111x_MODE_OFFSET)

#define ADS111x_DR_OFFSET          5
#define ADS111x_DR_MASK            (0x07U << ADS111x_DR_OFFSET)

#define ADS111x_COMP_MODE_OFFSET   4
#define ADS111x_COMP_MODE_MASK     (0x01U << ADS111x_COMP_MODE_OFFSET)

#define ADS111x_COMP_POL_OFFSET    3
#define ADS111x_COMP_POL_MASK      (0x01U << ADS111x_COMP_POL_OFFSET)

#define ADS111x_COMP_LAT_OFFSET    2
#define ADS111x_COMP_LAT_MASK      (0x01U << ADS111x_COMP_LAT_OFFSET)

#define ADS111x_COMP_QUE_OFFSET    0
#define ADS111x_COMP_QUE_MASK      (0x03U << ADS111x_COMP_QUE_OFFSET)

// --- Input Multiplexer Configuration (MUX) ---
#define ADS111x_MUX_DIFF_0_1       (0x00U << ADS111x_MUX_OFFSET) // Diferencial AIN0 - AIN1 (Default)
#define ADS111x_MUX_DIFF_0_3       (0x01U << ADS111x_MUX_OFFSET) // Diferencial AIN0 - AIN3
#define ADS111x_MUX_DIFF_1_3       (0x02U << ADS111x_MUX_OFFSET) // Diferencial AIN1 - AIN3
#define ADS111x_MUX_DIFF_2_3       (0x03U << ADS111x_MUX_OFFSET) // Diferencial AIN2 - AIN3
#define ADS111x_MUX_SINGLE_0       (0x04U << ADS111x_MUX_OFFSET) // Single-ended AIN0
#define ADS111x_MUX_SINGLE_1       (0x05U << ADS111x_MUX_OFFSET) // Single-ended AIN1
#define ADS111x_MUX_SINGLE_2       (0x06U << ADS111x_MUX_OFFSET) // Single-ended AIN2
#define ADS111x_MUX_SINGLE_3       (0x07U << ADS111x_MUX_OFFSET) // Single-ended AIN3

// --- Programmable Gain Amplifier (PGA / FSR) ---
#define ADS111x_PGA_6_144V         (0x00U << ADS111x_PGA_OFFSET) // FSR = ±6.144V
#define ADS111x_PGA_4_096V         (0x01U << ADS111x_PGA_OFFSET) // FSR = ±4.096V
#define ADS111x_PGA_2_048V         (0x02U << ADS111x_PGA_OFFSET) // FSR = ±2.048V (Default)
#define ADS111x_PGA_1_024V         (0x03U << ADS111x_PGA_OFFSET) // FSR = ±1.024V
#define ADS111x_PGA_0_512V         (0x04U << ADS111x_PGA_OFFSET) // FSR = ±0.512V
#define ADS111x_PGA_0_256V         (0x05U << ADS111x_PGA_OFFSET) // FSR = ±0.256V

// --- Operating Mode (MODE) ---
#define ADS111x_MODE_CONTINUOUS    (0x00U << ADS111x_MODE_OFFSET) // Modo Continuo
#define ADS111x_MODE_SINGLE_SHOT   (0x01U << ADS111x_MODE_OFFSET) // Modo Single-Shot / Power-Down (Default)

// --- Data Rate (DR) ---
#define ADS111x_DR_8_SPS           (0x00U << ADS111x_DR_OFFSET)  // 8 muestras por segundo
#define ADS111x_DR_16_SPS          (0x01U << ADS111x_DR_OFFSET)  // 16 muestras por segundo
#define ADS111x_DR_32_SPS          (0x02U << ADS111x_DR_OFFSET)  // 32 muestras por segundo
#define ADS111x_DR_64_SPS          (0x03U << ADS111x_DR_OFFSET)  // 64 muestras por segundo
#define ADS111x_DR_128_SPS         (0x04U << ADS111x_DR_OFFSET)  // 128 muestras por segundo (Default)
#define ADS111x_DR_250_SPS         (0x05U << ADS111x_DR_OFFSET)  // 250 muestras por segundo
#define ADS111x_DR_475_SPS         (0x06U << ADS111x_DR_OFFSET)  // 475 muestras por segundo
#define ADS111x_DR_860_SPS         (0x07U << ADS111x_DR_OFFSET)  // 860 muestras por segundo

// --- Comparator Queue (COMP_QUE) - Desactiva o configura alertas ---
#define ADS111x_COMP_QUE_ASSERT_1  (0x00U << ADS111x_COMP_QUE_OFFSET) // Alerta tras 1 conversión
#define ADS111x_COMP_QUE_ASSERT_2  (0x01U << ADS111x_COMP_QUE_OFFSET) // Alerta tras 2 conversiones
#define ADS111x_COMP_QUE_ASSERT_4  (0x02U << ADS111x_COMP_QUE_OFFSET) // Alerta tras 4 conversiones
#define ADS111x_COMP_QUE_DISABLE   (0x03U << ADS111x_COMP_QUE_OFFSET) // Desactivar comparador (Default)

#define ADS111x_THRESH_H_READY_MODE    0x8000U // Bit 15 en 1
#define ADS111x_THRESH_L_READY_MODE    0x0000U // Bit 15 en 0

/******************************************************************************
    Data types
******************************************************************************/

/******************************************************************************
    Local variables
******************************************************************************/
const float ads111x_gain_values[] = 
{
    [ADS111X_GAIN_6V144]   = 6.144,
    [ADS111X_GAIN_4V096]   = 4.096,
    [ADS111X_GAIN_2V048]   = 2.048,
    [ADS111X_GAIN_1V024]   = 1.024,
    [ADS111X_GAIN_0V512]   = 0.512,
    [ADS111X_GAIN_0V256]   = 0.256,
    [ADS111X_GAIN_0V256_2] = 0.256,
    [ADS111X_GAIN_0V256_3] = 0.256
};
/******************************************************************************
    Local function prototypes
******************************************************************************/
static ads111x_ret_t write_register(ads111x_drv_t *drv, uint8_t reg_addr, uint16_t value);
static ads111x_ret_t read_register(ads111x_drv_t *drv, uint8_t reg_addr, uint16_t *value);
/******************************************************************************
    Local function definitions
******************************************************************************/

static ads111x_ret_t write_register(ads111x_drv_t *drv, uint8_t reg_addr, uint16_t value)
{
    if (drv == NULL || drv->hw.i2c_write == NULL) 
	{
        return ADS111x_ERROR;
    }

    // Little-Endian (esp32) to Big-Endian (ADS1115).
    uint8_t buf[2];
    buf[0] = (uint8_t)((value >> 8) & 0xFF); // MSB primero
    buf[1] = (uint8_t)(value & 0xFF);        // LSB después

    int ret = drv->hw.i2c_write(drv->addr, reg_addr, buf, 2);
    if (ret != 0)
    {
        return ADS111x_ERROR;
    }
    
    return ADS111x_OK;
}

static ads111x_ret_t read_register(ads111x_drv_t *drv, uint8_t reg_addr, uint16_t *value)
{
    if (drv == NULL || value == NULL || drv->hw.i2c_read == NULL) 
	{
        return ADS111x_ERROR;
    }

    uint8_t buf[2] = {0};
    int ret = drv->hw.i2c_read(drv->addr, reg_addr, buf, 2);
    if (ret != 0)
    {
        return ADS111x_ERROR;
    }
	
    *value = (uint16_t)((buf[0] << 8) | buf[1]);	// Big-Endian to Little-endian (esp32).
    
    return ADS111x_OK;
}
/******************************************************************************
    Public function definitions
******************************************************************************/
ads111x_ret_t ads111x_drv_init(ads111x_drv_t *drv)
{
	if (NULL == drv->hw.i2c_write || NULL == drv->hw.i2c_read)
	{
		return ADS111x_BAD_ARGS;
	}

	if (drv->addr != ADS111x_ADDR_VCC && drv->addr != ADS111x_ADDR_GND &&
		drv->addr != ADS111x_ADDR_SCL && drv->addr != ADS111x_ADDR_SDA) 
	{
		return ADS111x_ERROR;
	}

	// TODO: chequear si es necesario alguna config inicial...
		
	return ADS111x_OK;	
}

ads111x_ret_t ads111x_drv_start_conversion(ads111x_drv_t *drv)
{
    if (drv == NULL)
    {
        return ADS111x_ERROR;
    }

    uint16_t config_reg = 0;
    ads111x_ret_t ret = read_register(drv, REG_CONFIG, &config_reg);
    if (ret != ADS111x_OK)
    {
        return ADS111x_ERROR;
    }

    config_reg |= ADS111x_OS_MASK;
    ret = write_register(drv, REG_CONFIG, config_reg);
    if (ret != ADS111x_OK)
    {
        return ADS111x_ERROR;
    }

    return ADS111x_OK;
}

ads111x_ret_t ads111x_drv_get_conversion(ads111x_drv_t *drv, int16_t *value)
{
    if (drv == NULL || value == NULL)
    {
        return ADS111x_ERROR;
    }

	uint16_t raw_data = 0;
	ads111x_ret_t ret = read_register(drv, REG_CONVERSION, &raw_data);
    if (ret != ADS111x_OK)
    {
        return ADS111x_ERROR;
    }

    *value = (int16_t) raw_data;

    return ADS111x_OK;
}

ads111x_ret_t ads111x_drv_set_input(ads111x_drv_t *drv, ads111x_mux_t mux)
{
    if (drv == NULL)
	{
		return ADS111x_BAD_ARGS;
	}

    uint16_t config_reg = 0;
    if (read_register(drv, REG_CONFIG, &config_reg) != ADS111x_OK)
	{
		return ADS111x_ERROR;
	}

    // Limpiamos los bits viejos del MUX y aplicamos la nueva configuración
    config_reg &= ~ADS111x_MUX_MASK;
    config_reg |= ((uint16_t)mux << ADS111x_MUX_OFFSET) & ADS111x_MUX_MASK;

    return write_register(drv, REG_CONFIG, config_reg);
}

ads111x_ret_t ads111x_drv_get_input(ads111x_drv_t *drv, ads111x_mux_t *mux)
{
    if (drv == NULL || mux == NULL)
	{
		return ADS111x_BAD_ARGS;
	}

    uint16_t config_reg = 0;
    if (read_register(drv, REG_CONFIG, &config_reg) != ADS111x_OK)
	{
		return ADS111x_ERROR;
	}

    // Aislamos el campo MUX y desplazamos para obtener el valor crudo correlativo (0-7)
    *mux = (ads111x_mux_t)((config_reg & ADS111x_MUX_MASK) >> ADS111x_MUX_OFFSET);
    return ADS111x_OK;
}

ads111x_ret_t ads111x_drv_set_pga(ads111x_drv_t *drv, ads111x_gain_t pga_value)
{
    if (drv == NULL)
	{
		return ADS111x_BAD_ARGS;
	}

    uint16_t config_reg = 0;
    if (read_register(drv, REG_CONFIG, &config_reg) != ADS111x_OK)
	{
		return ADS111x_ERROR;
	}

    // Limpiamos los bits del PGA y aplicamos el nuevo rango
    config_reg &= ~ADS111x_PGA_MASK;
    config_reg |= ((uint16_t)pga_value << ADS111x_PGA_OFFSET) & ADS111x_PGA_MASK;

    return write_register(drv, REG_CONFIG, config_reg);
}

ads111x_ret_t ads111x_drv_get_pga(ads111x_drv_t *drv, ads111x_gain_t *pga_value)
{
    if (drv == NULL || pga_value == NULL)
	{
		return ADS111x_BAD_ARGS;
	}

    uint16_t config_reg = 0;
    if (read_register(drv, REG_CONFIG, &config_reg) != ADS111x_OK)
	{
		return ADS111x_ERROR;
	}

    // Extraemos el valor del PGA
    *pga_value = (ads111x_gain_t)((config_reg & ADS111x_PGA_MASK) >> ADS111x_PGA_OFFSET);
    return ADS111x_OK;
}

ads111x_ret_t ads111x_drv_set_mode(ads111x_drv_t *drv, ads111x_mode_t mode)
{
    if (drv == NULL)
	{
		return ADS111x_BAD_ARGS;
	}

    uint16_t config_reg = 0;
    if (read_register(drv, REG_CONFIG, &config_reg) != ADS111x_OK)
	{
		return ADS111x_ERROR;
	}

    // Limpiamos el bit de modo (Continuo vs Single-Shot) y aplicamos el nuevo
    config_reg &= ~ADS111x_MODE_MASK;
    config_reg |= ((uint16_t)mode << ADS111x_MODE_OFFSET) & ADS111x_MODE_MASK;

    return write_register(drv, REG_CONFIG, config_reg);
}

ads111x_ret_t ads111x_drv_get_mode(ads111x_drv_t *drv, ads111x_mode_t *mode)
{
    if (drv == NULL || mode == NULL)
	{
		return ADS111x_BAD_ARGS;
	}

    uint16_t config_reg = 0;
    if (read_register(drv, REG_CONFIG, &config_reg) != ADS111x_OK)
	{
		return ADS111x_ERROR;
	}

    // Extraemos el bit de modo
    *mode = (ads111x_mode_t)((config_reg & ADS111x_MODE_MASK) >> ADS111x_MODE_OFFSET);
    return ADS111x_OK;
}

ads111x_ret_t ads111x_drv_set_data_rate(ads111x_drv_t *drv, ads111x_data_rate_t data_rate)
{
    if (drv == NULL)
	{
		return ADS111x_BAD_ARGS;
	}

    uint16_t config_reg = 0;
    if (read_register(drv, REG_CONFIG, &config_reg) != ADS111x_OK)
	{
		return ADS111x_ERROR;
	}

    // Limpiamos los bits del Data Rate (SPS) y aplicamos el valor solicitado
    config_reg &= ~ADS111x_DR_MASK;
    config_reg |= ((uint16_t)data_rate << ADS111x_DR_OFFSET) & ADS111x_DR_MASK;

    return write_register(drv, REG_CONFIG, config_reg);
}

ads111x_ret_t ads111x_drv_get_data_rate(ads111x_drv_t *drv, ads111x_data_rate_t *data_rate)
{
    if (drv == NULL || data_rate == NULL)
	{
		return ADS111x_BAD_ARGS;
	}

    uint16_t config_reg = 0;
    if (read_register(drv, REG_CONFIG, &config_reg) != ADS111x_OK)
	{
		return ADS111x_ERROR;
	}

    // Extraemos el valor de la tasa de muestreo
    *data_rate = (ads111x_data_rate_t)((config_reg & ADS111x_DR_MASK) >> ADS111x_DR_OFFSET);
    return ADS111x_OK;
}

ads111x_ret_t ads111x_drv_enable_int(ads111x_drv_t *drv)
{
    if (drv == NULL)
    {
        return ADS111x_ERROR;
    }

    // Forzar los registros de umbral al modo especial "Conversion Ready".
    if (write_register(drv, REG_THRESH_H, ADS111x_THRESH_H_READY_MODE) != ADS111x_OK)
    {
        return ADS111x_ERROR;
    }
    if (write_register(drv, REG_THRESH_L, ADS111x_THRESH_L_READY_MODE) != ADS111x_OK)
    {
        return ADS111x_ERROR;
    }

    // Modificar REG_CONFIG para activar la cola del comparador
    uint16_t config_reg = 0;
    if (read_register(drv, REG_CONFIG, &config_reg) != ADS111x_OK)
    {
        return ADS111x_ERROR;
    }

    // Limpiamos los bits actuales de COMP_QUE usando su máscara
    config_reg &= ~ADS111x_COMP_QUE_MASK;
    // Seteamos para que genere la alerta luego de 1 conversión terminada
    config_reg |= ADS111x_COMP_QUE_ASSERT_1;

    // Escribimos la configuración actualizada
    if (write_register(drv, REG_CONFIG, config_reg) != ADS111x_OK)
    {
        return ADS111x_ERROR;
    }

	// Callback para que el BSP/APP configure/gestione la interrupcion.
	if (drv->hw.gpio_set_interrupt != NULL)
	{
		drv->hw.gpio_set_interrupt(true);	
	}
	
    return ADS111x_OK;
}

ads111x_ret_t ads111x_drv_disable_int(ads111x_drv_t *drv)
{
    if (drv == NULL)
    {
        return ADS111x_ERROR;
    }

    uint16_t config_reg = 0;

    // Leemos la configuración actual
    if (read_register(drv, REG_CONFIG, &config_reg) != ADS111x_OK)
    {
        return ADS111x_ERROR;
    }

    // Limpiamos los bits actuales de COMP_QUE y aplicamos el valor de deshabilitado (0x03)
    config_reg &= ~ADS111x_COMP_QUE_MASK;
    config_reg |= ADS111x_COMP_QUE_DISABLE;

    // Escribimos la configuración de vuelta para apagar la interrupción
    if (write_register(drv, REG_CONFIG, config_reg) != ADS111x_OK)
    {
        return ADS111x_ERROR;
    }

	// Disable interrupt desde BSP/APP.
	if (drv->hw.gpio_set_interrupt != NULL)
	{
		drv->hw.gpio_set_interrupt(false);	
	}
	
    return ADS111x_OK;
}

void ads111x_drv_set_cb(ads111x_drv_t *drv, ads111x_cb_t cb)
{
	drv->ads111x_cb = cb;
}

ads111x_ret_t ads111x_drv_interrupt_handler(ads111x_drv_t *drv, void *ctx)
{
    if (drv == NULL)
    {
        return ADS111x_ERROR;
    }

    int16_t raw_conversion = 0;
    ads111x_ret_t ret = ads111x_drv_get_conversion(drv, &raw_conversion);
    if (ret != ADS111x_OK)
    {
        return ADS111x_ERROR; 
    }

    if (drv->ads111x_cb != NULL)
    {
        drv->ads111x_cb(ctx, raw_conversion);
    }

    return ADS111x_OK;
}

