/**
  ******************************************************************************
  * @file    cube_hal_f4.c
  * @author  CL
  * @version V1.0.0
  * @date    14-April-2016
  * @brief   
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

#include "cube_hal.h"


UART_HandleTypeDef UartHandle;
uint32_t Usart_BaudRate = 115200;

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void Error_Handler()
{
    while(1) {

    }
}


uint32_t user_currentTimeGetTick(void)
{
    return HAL_GetTick();
}


uint32_t user_currentTimeGetElapsedMS(uint32_t Tick1)
{
    volatile uint32_t Delta, Tick2;

    Tick2 = HAL_GetTick();

    /* Capture computation */
    Delta = Tick2 - Tick1;
    return Delta;
}

void USARTConfig(void)
{
   GPIO_InitTypeDef GPIO_InitStruct;  
   
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  USARTx_PRINT_TX_GPIO_CLK_ENABLE();
  USARTx_PRINT_RX_GPIO_CLK_ENABLE();
  
  /* Enable USARTx clock */
  USARTx_PRINT_CLK_ENABLE(); 
  __SYSCFG_CLK_ENABLE();   

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = USARTx_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
 
    GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;    
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;   

    HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = USARTx_RX_PIN;

    HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);
		

    
    /*##-1- Configure the UART peripheral ######################################*/
    /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
    /* UART1 configured as follow:
            - Word Length = 8 Bits
            - Stop Bit = One Stop bit
            - Parity = None
            - BaudRate = 9600 baud
            - Hardware flow control disabled (RTS and CTS signals) */
    UartHandle.Instance        = USART2;
    UartHandle.Init.BaudRate   = Usart_BaudRate;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits   = UART_STOPBITS_1;
    UartHandle.Init.Parity     = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode       = UART_MODE_TX_RX;

    if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
    {
        //          Error_Handler();
      while(1);
    }  
    
    if(HAL_UART_Init(&UartHandle) != HAL_OK)
    {
        //          Error_Handler();
      while(1);
    }
		
		/*##-3- Configure the NVIC for UART ########################################*/
		/* NVIC for USART */
		HAL_NVIC_SetPriority(USART2_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
}


    
    
