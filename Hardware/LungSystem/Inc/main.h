/**
  ******************************************************************************
  * @file    Wifi/WiFi_HTTP_Server/Inc/main.h 
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright © 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif  

/* Includes ------------------------------------------------------------------*/

  
  #include "stm32l4xx_hal.h"
   #include "stm32l4xx_hal_conf.h" 

#define USARTx_PRINT_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_PRINT_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()  
#define USARTx_PRINT_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()  


#include "cube_hal.h"  
  
#ifdef __cplusplus
}
#endif
/* Includes ------------------------------------------------------------------*/
#include "wifi.h"
#include "stm32l475e_iot01.h"
#include "stdio.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#define VDDA_APPLI                     ((uint32_t) 3300)    /* Value of analog voltage supply Vdda (unit: mV) */

/* Internal temperature sensor: constants data used for indicative values in  */
/* this example. Refer to device datasheet for min/typ/max values.            */
/* For more accurate values, device should be calibrated on offset and slope  */
/* for application temperature range.                                         */
#define INTERNAL_TEMPSENSOR_V30        ((int32_t)760)           /* Internal temperature sensor, parameter V25 (unit: mV). Refer to device datasheet for min/typ/max values. */
#define INTERNAL_TEMPSENSOR_AVGSLOPE   ((int32_t)2500)          /* Internal temperature sensor, parameter Avg_Slope (unit: uV/DegCelsius). Refer to device datasheet for min/typ/max values. */
#define TEMP30_CAL_ADDR   ((uint16_t*) ((uint32_t)0x1FFF75A8))  /* Internal temperature sensor, parameter TS_CAL1: TS ADC raw data acquired at a temperature of 30 DegC (+-5 DegC) */
#define TEMP110_CAL_ADDR  ((uint16_t*) ((uint32_t)0x1FFF75CA))  /* Internal temperature sensor, parameter TS_CAL2: TS ADC raw data acquired at a temperature of  110 DegC (+-5 DegC) */
#define VDDA_TEMP_CAL                  ((uint32_t)3000)        /* Vdda value with which temperature sensor has been calibrated in production (+-10 mV). */


/* Private macro -------------------------------------------------------------*/
/**
  * @brief  Computation of temperature (unit: degree Celsius) from the internal
  *         temperature sensor measurement by ADC.
  *         Computation is using temperature sensor calibration values done
  *         in production.
  *         Computation formula:
  *         Temperature = (TS_ADC_DATA - TS_CAL1) * (110degC - 30degC)
  *                       / (TS_CAL2 - TS_CAL1) + 30degC
  *         with TS_ADC_DATA = temperature sensor raw data measured by ADC
  *              Avg_Slope = (TS_CAL2 - TS_CAL1) / (110 - 30)
  *              TS_CAL1 = TS_ADC_DATA @30degC (calibrated in factory)
  *              TS_CAL2 = TS_ADC_DATA @110degC (calibrated in factory)
  *         Calculation validity conditioned to settings: 
  *          - ADC resolution 12 bits (need to scale conversion value 
  *            if using a different resolution).
  *          - Power supply of analog voltage set to literal VDDA_APPLI
  *            (need to scale value if using a different value of analog
  *            voltage supply).
  * @param TS_ADC_DATA: Temperature sensor digital value measured by ADC
  * @retval None
  */
#define COMPUTATION_TEMPERATURE_TEMP30_TEMP110(TS_ADC_DATA)                    \
  (((( ((int32_t)((TS_ADC_DATA * VDDA_APPLI) / VDDA_TEMP_CAL)                  \
        - (int32_t) *TEMP30_CAL_ADDR)                                          \
     ) * (int32_t)(110 - 30)                                                   \
    ) / (int32_t)(*TEMP110_CAL_ADDR - *TEMP30_CAL_ADDR)                        \
   ) + 30                                                                      \
  )


/* ADC handler declaration */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


typedef enum {
  wifi_state_reset = 0,
  wifi_state_ready,
  wifi_state_idle,
  wifi_state_connected,
  wifi_state_connecting,
  wifi_state_disconnected,
  wifi_state_activity,
  wifi_state_inter,
  wifi_state_print_data,
  wifi_state_error,
  wifi_undefine_state       = 0xFF,
} wifi_state_t;

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
