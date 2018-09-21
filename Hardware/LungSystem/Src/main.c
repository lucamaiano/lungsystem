/**
  ******************************************************************************
  * @file    Wifi/WiFi_HTTP_Server/src/main.c
  * @author  MCD Application Team
  * @brief   This file provides main program functions
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h> 
#include <stdio.h>  
#include <math.h>   
#include "stm32l4xx_hal_rtc.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "x_nucleo_ika02a1.h"
#include "sensor_calibration_TGS5141.h"


#define USE_UART_PRINTS

RTC_HandleTypeDef RtcHandle;

volatile uint32_t Sensors_Enabled = 0;
volatile uint32_t DataTxPeriod = 1000;
volatile uint8_t AutoInit = 0;

float gas_value_ppm = 0;
float gas_value_ppm_comp = 0;
float temperature = 0;

char uartOutBuffer[256];
char uartInBuffer[16];
int8_t i=0;
char dati[20];
char * hostname = "";
char * post_hostname = "http://localhost:5000/api/measures";
char * gcfg_key1 = "ip_ipaddr";
char * gcfg_key2 = "nv_model";
//res = requests.post('http://localhost:5000/api/measures', json=dictToSend)
void SystemClock_Config(void);

__IO ITStatus UartReady = RESET;
volatile uint8_t Message=0;

	#define FLASH_SENS_ADDR           ((uint32_t)0x0803F000)
	#define FLASH_GAIN_ADDR           ((uint32_t)0x0803F800)
	#define FLASH_SENS_PAGE 					318
	#define FLASH_GAIN_PAGE						319
	#define FLASH_TYPEPROGRAM_WORD FLASH_TYPEPROGRAM_DOUBLEWORD



int32_t* pSens = (int32_t*)FLASH_SENS_ADDR;
int32_t* pGain = (int32_t*)FLASH_GAIN_ADDR;


/* Private function prototypes -----------------------------------------------*/
void RTC_TimeRegulate(uint8_t hh, uint8_t mm, uint8_t ss);
void FlashStoreDialogSens(uint32_t sens);
void FlashStoreDialogGain(uint32_t gain);
uint8_t TestDialog(void);
void Test_Pin_Init(void);
void Test_Pin_HiZ(void);
void Test_Pin_On(void);
void Test_Pin_Off(void);
uint16_t ADC_ref = 0;
uint16_t ADC_val = 0;


#define AVERAGING 4
/* Private defines -----------------------------------------------------------*/
/* Update SSID and PASSWORD with own Access point settings */
#define SSID     "YOUR_SSID" 
#define PASSWORD "YOUR_PASSWORD"
/*#define SSID     "depa_enea" 
#define PASSWORD "zTcqxftS"*/
/*#define SSID     "enea-two" 
#define PASSWORD "primitivodimanduria"*/
#define WIFI_WRITE_TIMEOUT 10000
#define WIFI_READ_TIMEOUT  10000

#define TERMINAL_USE

#define PORT           80
/* Private typedef------------------------------------------------------------*/
typedef enum
{
  WS_IDLE = 0,
  WS_CONNECTED,
  WS_DISCONNECTED,
  WS_ERROR,
} WebServerState_t;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined (TERMINAL_USE)  
extern UART_HandleTypeDef hDiscoUart;
#endif /* TERMINAL_USE */
static   uint8_t http[1024];
static   uint8_t http1[1024];
static   uint8_t resp[1024];
uint16_t respLen;
uint8_t  IP_Addr[4]; 
uint8_t  MAC_Addr[6]; 
int32_t Socket = -1;
static   WebServerState_t  State = WS_ERROR;
static ADC_HandleTypeDef    AdcHandle;
char     ModuleName[32];

/* Private function prototypes -----------------------------------------------*/
#if defined (TERMINAL_USE)
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
#endif /* TERMINAL_USE */

static void SystemClock_Config(void);

static WIFI_Status_t SendWebPage(uint8_t ledIsOn, char * datiinvio);
static void WebServerProcess(void);
static uint32_t TEMP_SENSOR_Init(void);
static uint32_t TEMP_SENSOR_GetValue(void);
      float temperature_value = 0;
    float humidity_value = 0;
    float pressure_value = 0;
//DigitalOut led(LED2);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  
    

        gas_value_ppm = 0;
        ADC_val = 0;
        ADC_ref = 0;
        temperature = 0;

    BSP_TSENSOR_Init();
    BSP_HSENSOR_Init();
    BSP_PSENSOR_Init();
    

    
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

      HAL_Init();
 
    /* Configure the system clock */
    SystemClock_Config();
    SystemCoreClockUpdate();
	
    /* Initialize UART */
    USARTConfig(); 
		HAL_UART_Receive_IT(&UartHandle, (uint8_t*)uartInBuffer, 1);	
	
		//Test_Pin_Init();
		BSP_LED_Init(LED1);
         //       BSP_LED_CO_Init(LED1);
		
	
		if (*pSens==0 || *pSens==-1){
			FlashStoreDialogSens(0);
		}
		if (*pGain==0 || *pGain==-1){
			FlashStoreDialogGain(0);
		}

	
    sprintf(uartOutBuffer, "\rSTMicroelectronics gas sensor demo V1.0\r\n Sensor:      TGS5141 (Carbon monoxide)\r\n Sensitivity: %.3f nA/ppm\r\n Gain:        %d\r\n\r\n",((float)*pSens)/1000,*pGain);
    HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000); 
  /*Initialize Temperature sensor */
  TEMP_SENSOR_Init();
  HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_SINGLE_ENDED);
  HAL_ADC_Start(&AdcHandle) ;

  /* WIFI Web Server demonstration */
#if defined (TERMINAL_USE)
  /* Initialize all configured peripherals */
  hDiscoUart.Instance = DISCOVERY_COM1; 
  hDiscoUart.Init.BaudRate = 115200;
  hDiscoUart.Init.WordLength = UART_WORDLENGTH_8B;
  hDiscoUart.Init.StopBits = UART_STOPBITS_1;
  hDiscoUart.Init.Parity = UART_PARITY_NONE;
  hDiscoUart.Init.Mode = UART_MODE_TX_RX;
  hDiscoUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hDiscoUart.Init.OverSampling = UART_OVERSAMPLING_16;
  hDiscoUart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hDiscoUart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  BSP_COM_Init(COM1, &hDiscoUart);
  printf("****** WIFI Web Server demonstration****** \n\n"); 
   
#endif /* TERMINAL_USE */
 
  /*Initialize and use WIFI module */
  if(WIFI_Init() ==  WIFI_STATUS_OK)
  {
#if defined (TERMINAL_USE)
  printf("ES-WIFI Initialized.\n");
#endif /* TERMINAL_USE */     
    
      if(WIFI_GetMAC_Address(MAC_Addr) == WIFI_STATUS_OK)
      {       
#if defined (TERMINAL_USE)    
        printf("> es-wifi module MAC Address : %X:%X:%X:%X:%X:%X\n",     
               MAC_Addr[0],
               MAC_Addr[1],
               MAC_Addr[2],
               MAC_Addr[3],
               MAC_Addr[4],
               MAC_Addr[5]);   
#endif /* TERMINAL_USE */
      }
      else
      {
#if defined (TERMINAL_USE)  
        printf("> ERROR : CANNOT get MAC address\n");
#endif /* TERMINAL_USE */  
      }
   if( WIFI_Connect(SSID, PASSWORD, WIFI_ECN_WPA2_PSK) == WIFI_STATUS_OK)
    {
#if defined (TERMINAL_USE)  
      printf("> es-wifi module connected \n");
#endif /* TERMINAL_USE */ 
      
      if(WIFI_GetIP_Address(IP_Addr) == WIFI_STATUS_OK)
      {
#if defined (TERMINAL_USE)
      printf("> es-wifi module got IP Address : %d.%d.%d.%d\n",     
               IP_Addr[0],
               IP_Addr[1],
               IP_Addr[2],
               IP_Addr[3]); 
            hostname =  strcat(strcat(strcat(strcat(strcat(strcat((char*)IP_Addr[0],"."),(char*)IP_Addr[1]),"."),(char*)IP_Addr[2]),"."),(char*)IP_Addr[3]); 

      printf(hostname);
      printf(">Start HTTP Server... \n");
      printf(">Wait for connection...  \n");
#endif /* TERMINAL_USE */
        State = WS_IDLE;
        
      }
      else
      {    
#if defined (TERMINAL_USE)  
    printf("> ERROR : es-wifi module CANNOT get IP address\n");
#endif /* TERMINAL_USE */
      }
    }
    else
    {
      
#if defined (TERMINAL_USE) 
 printf("> ERROR : es-wifi module NOT connected\n");
#endif /* TERMINAL_USE */ 
    }
  }
  else
  {   
#if defined (TERMINAL_USE)  
   printf("> ERROR : WIFI Module cannot be initialized.\n"); 
#endif /* TERMINAL_USE */
  }
    
	
		if (*pSens==0 || *pSens==-1){
			FlashStoreDialogSens(0);
		}
		if (*pGain==0 || *pGain==-1){
			FlashStoreDialogGain(0);
		}

	
    sprintf(uartOutBuffer, "\rSTMicroelectronics gas sensor demo V1.0\r\n Sensor:      TGS5141 (Carbon monoxide)\r\n Sensitivity: %.3f nA/ppm\r\n Gain:        %d\r\n\r\n",((float)*pSens)/1000,*pGain);
    HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000); 
  uint8_t LedState = 0, temp;
		Message=0;
                
    while(1) {
		if(Message=='s' || Message=='S'){
			FlashStoreDialogSens(*pSens);
		}
		if(Message=='g' || Message=='G'){
			FlashStoreDialogGain(*pGain);
		}
		if(Message=='t' || Message=='T'){
			Message=0;
			TestDialog();
		}
		if(Message=='o' || Message=='O'){
			Message=0;
			BSP_LED_On(LED1);
			Test_Pin_Init();
		}
		if(Message=='p' || Message=='P'){
			Message=0;
			BSP_LED_Off(LED1);
			Test_Pin_HiZ();
		}
		
    gas_value_ppm = Get_Gas_concentration((float)*pGain,((float)*pSens)/1000);
    temperature = Get_Temperature();
    gas_value_ppm_comp = Get_comp_gas_concentration(gas_value_ppm,(int8_t)round(temperature));
			
    sprintf(uartOutBuffer, " CO content %.1f ppm (temp %.1f C)                         \r",gas_value_ppm_comp,temperature);
    HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000); 
    HAL_Delay(500);		
      WebServerProcess ();
    
#if defined (TERMINAL_USE)  
            BSP_LED_On(LED1);

 
        BSP_LED_Off(LED1);
             BSP_LED_On(LED1);
       
        /*        for (uint16_t iii = 0;iii<AVERAGING;iii++){
            gas_value_ppm += Get_Gas_concentration(custom_values.cust_floats[1]*1000, custom_values.cust_floats[0]);
            ADC_val += Get_ADC_value(ANALOG_GAS_VALUE_PIN);
            ADC_ref += Get_ADC_value(ANALOG_REF_VOLT_VALUE_PIN);
            temperature += Get_Temperature();
        }
        gas_value_ppm = gas_value_ppm / AVERAGING;
        ADC_val = ADC_val / AVERAGING;
        ADC_ref = ADC_ref / AVERAGING;
        temperature = temperature / AVERAGING;
        
        
        gas_value_ppm = Get_Gas_concentration(ANALOG_GAIN, SENSOR_SENSITIVITY);
        ADC_val = Get_ADC_value(ANALOG_GAS_VALUE_PIN);
        ADC_ref = Get_ADC_value(ANALOG_REF_VOLT_VALUE_PIN);
        temperature = Get_Temperature();
    
        gas_value_ppm_comp = Get_comp_gas_concentration(gas_value_ppm,(int8_t)round(temperature));*/
             		if(Message=='s' || Message=='S'){
			FlashStoreDialogSens(*pSens);
		}
		if(Message=='g' || Message=='G'){
			FlashStoreDialogGain(*pGain);
		}
		if(Message=='t' || Message=='T'){
			Message=0;
			TestDialog();
		}
		if(Message=='o' || Message=='O'){
			Message=0;
			BSP_LED_On(LED1);
			Test_Pin_Init();
		}
		if(Message=='p' || Message=='P'){
			Message=0;
			BSP_LED_Off(LED1);
			Test_Pin_HiZ();
		}
		
 
 //    temp = TEMP_SENSOR_GetValue();
   printf(" %.1f                          \n",temp); 
    HAL_Delay(5000);  
#endif /* TERMINAL_USE */      
  }
}

/**
  * @brief  Send HTML page
  * @param  None
  * @retval None
  */
static void WebServerProcess(void)
{
  uint8_t LedState = 0, temp;
  
  switch(State)
  {
  case WS_IDLE:
    Socket = 0;
    WIFI_StartServer(Socket, WIFI_TCP_PROTOCOL, "", PORT);
    
    if(Socket != -1)
    {
#if defined (TERMINAL_USE)
      printf("> HTTP Server Started \n");  
#endif /* TERMINAL_USE */
      State = WS_CONNECTED;
    }
    else
    {
#if defined (TERMINAL_USE)
      printf("> ERROR : Connection cannot be established.\n"); 
#endif /* TERMINAL_USE */      
      State = WS_ERROR;
    }    
    break;
    
  case WS_CONNECTED:
    
    WIFI_ReceiveData(Socket, resp, 1200, &respLen, WIFI_READ_TIMEOUT);
    
    if( respLen > 0)
    {
      if(strstr((char *)resp, "GET")) /* GET: put web page */
      {
        temp = TEMP_SENSOR_GetValue();
         temperature_value = BSP_TSENSOR_ReadTemp();
     //   printf("\nTEMPERATURE = %.2f degC\n", temperature_value);
          
        humidity_value = BSP_HSENSOR_ReadHumidity();
     //   printf("HUMIDITY    = %.2f %%\n", humidity_value);

        pressure_value = BSP_PSENSOR_ReadPressure();
     //   printf("PRESSURE is = %.2f mBar\n", pressure_value);
    gas_value_ppm = Get_Gas_concentration((float)*pGain,((float)*pSens)/1000);
    temperature = Get_Temperature();
    gas_value_ppm_comp = Get_comp_gas_concentration(gas_value_ppm,(int8_t)round(temperature));
			
    sprintf(uartOutBuffer, " CO content %.1f ppm (temp %.1f C)                         \r",gas_value_ppm_comp,temperature);
    HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);         
        sprintf(dati,"%.2f;%.2f;%.2f;%.2f",temperature_value,humidity_value,pressure_value,temperature/100);   
        if(SendWebPage(LedState, dati) != WIFI_STATUS_OK)
        {
#if defined (TERMINAL_USE)
          printf("> ERROR : Cannot send web page\n");
#endif /* TERMINAL_USE */   
          State = WS_ERROR;
        }
      }
      else if(strstr((char *)resp, "POST"))/* POST: received info */
      {
    if(WIFI_StopServer(Socket) == WIFI_STATUS_OK)
    {
#if defined (TERMINAL_USE)
              printf("> Server disconnected\n");
#endif /* TERMINAL_USE */   
              State = WS_IDLE;
    }


      }
    }

    break;
  case WS_ERROR:   
  default:
    break;
  }
}

/**
  * @brief  Send HTML page
  * @param  None
  * @retval None
  */
static WIFI_Status_t SendWebPage(uint8_t ledIsOn, char * datiinvio)
{
  uint8_t  temp[50];
  uint16_t SentDataLength;
  WIFI_Status_t ret;
  
  /* construct web page content */
  
sprintf((char *)dati, "%s", datiinvio);
 
  ret = WIFI_SendData(0, (uint8_t *)dati, strlen((char *)dati), &SentDataLength, WIFI_WRITE_TIMEOUT); 
  
  if((ret == WIFI_STATUS_OK) && (SentDataLength != strlen((char *)dati)))
  {
    ret = WIFI_STATUS_ERROR;
  }
    
  return ret;
}


/**
  * @brief  Configure the ADC.
  * @param  None
  * @retval None
  */
static uint32_t TEMP_SENSOR_Init(void)
{
  ADC_ChannelConfTypeDef sConfig;
  
   /* ADC1 Periph clock enable */
  __HAL_RCC_ADC_CLK_ENABLE();
  
  /* Configure the ADC peripheral */
  AdcHandle.Instance          = ADC1;
  
  AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV2;
  AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;
  AdcHandle.Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
  AdcHandle.Init.ContinuousConvMode    = ENABLE;                        /* Continuous mode enabled to have continuous conversion */
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
  AdcHandle.Init.NbrOfDiscConversion   = 0;
  AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;        /* Conversion start not trigged by an external event */
  AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  AdcHandle.Init.NbrOfConversion       = 1;
  AdcHandle.Init.DMAContinuousRequests = DISABLE;
  AdcHandle.Init.EOCSelection          = DISABLE;
  
  if (HAL_ADC_Init(&AdcHandle)== HAL_OK)
  {
    /* Configure ADC Temperature Sensor Channel */
    sConfig.Channel      = ADC_CHANNEL_TEMPSENSOR;
    sConfig.Rank         = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_247CYCLES_5;
    sConfig.SingleDiff   = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset       = 0;  /* Parameter discarded because offset correction is disabled */
    
    if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) == HAL_OK)
    {
      return 0;
    }
  }
  return 1;
}


/**
  * @brief  Get JunctionTemp level in 12 bits.
  * @retval JunctionTemp level(0..0xFFF) / 0xFFFFFFFF : Error
  */
static uint32_t TEMP_SENSOR_GetValue(void)
{   
  static uint32_t   wTemperature_DegreeCelsius = 0;
  
  if(HAL_ADC_PollForConversion(&AdcHandle, 1000)== HAL_OK)
  {
    wTemperature_DegreeCelsius = COMPUTATION_TEMPERATURE_TEMP30_TEMP110(HAL_ADC_GetValue(&AdcHandle) );
    
    return (wTemperature_DegreeCelsius);
  } 
  
  return  0xFFFFFFFF;
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 40
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            PLL_Q                          = 4
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* MSI is enabled after System reset, activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}

#if defined (TERMINAL_USE)
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&hDiscoUart, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}
#endif /* TERMINAL_USE */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(char* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif


void FlashStoreDialogSens(uint32_t sens){
	uint32_t tmpSens=0;
	uint8_t done=0;
	uint8_t first_flash=0;
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PageError = 0;
	
	if(sens==0){
		tmpSens=2300;
		first_flash=1;
	}

	Message=0;
	sprintf(uartOutBuffer, "Update sensitivity calibration stored in flash? (%d) (Y/N)\r\n",sens);
	HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000); 
	while(Message==0){};
	
	if(Message=='y' ||Message=='Y'){
		first_flash=0;
		while (done==0){
			Message=0;
			sprintf(uartOutBuffer, "Please write sensitivity (6 digits)\r\n");
			HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
			tmpSens=0;
			for (i=5;i>=0;i--){
				while(Message==0){};
				tmpSens+=pow(10,i)*(Message-48);
				sprintf(uartOutBuffer, "%d",Message-48);
				HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
				Message=0;
			}
				sprintf(uartOutBuffer, "\r\n");
				HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
		
			sprintf(uartOutBuffer, "Is it right value (%d)? (Y/N)\r\n",tmpSens);
			HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
			while(Message==0){};
			if(Message=='y' ||Message=='Y'){
				done=1;
			}
		}
		
		if(HAL_FLASH_Unlock()!=HAL_OK){
			sprintf(uartOutBuffer, "Error to unlock flash\r\n\r\n");
		}
		

		EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page = FLASH_SENS_PAGE;
    EraseInitStruct.NbPages = 1;

		
		
    

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK){ 
			sprintf(uartOutBuffer, "Error erasing flash\r\n\r\n");
		}else if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_SENS_ADDR, tmpSens)!=HAL_OK){
			sprintf(uartOutBuffer, "Writing into flash was unsuccesfull\r\n\r\n");
		}else if(HAL_FLASH_Lock() !=HAL_OK){
			sprintf(uartOutBuffer, "Error to lock flash\r\n\r\n");
		}else{
			sprintf(uartOutBuffer, "Successfully stored\r\n\r\n");
			
		}
		HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
	}
	
	if(first_flash){
		HAL_FLASH_Unlock();

		EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page = FLASH_SENS_PAGE;
    EraseInitStruct.NbPages = 1;


    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
		
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_SENS_ADDR, tmpSens);
		HAL_FLASH_Lock();
		sprintf(uartOutBuffer, "Default value (%d) used\r\n\r\n",tmpSens);
		HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
	}
}

void FlashStoreDialogGain(uint32_t gain){
	uint32_t tmpGain=0;
	uint8_t done=0;
	uint8_t first_flash=0;
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PageError = 0;
	
	if(gain==0){
		tmpGain=470000;
		first_flash=1;
	}

	Message=0;
	sprintf(uartOutBuffer, "Update gain stored in flash? (%d) (Y/N)\r\n",gain);
	HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000); 
	while(Message==0){};
	
	if(Message=='y' ||Message=='Y'){
		first_flash=0;
		while (done==0){
			Message=0;
			sprintf(uartOutBuffer, "Please write gain (6 digits)\r\n");
			HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
			tmpGain=0;
			for (i=5;i>=0;i--){
				while(Message==0){};
				tmpGain+=pow(10,i)*(Message-48);
				sprintf(uartOutBuffer, "%d",Message-48);
				HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
				Message=0;
			}
				sprintf(uartOutBuffer, "\r\n");
				HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
		
			sprintf(uartOutBuffer, "Is it right value? (%d) (Y/N)\r\n",tmpGain);
			HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
			while(Message==0){};
			if(Message=='y' ||Message=='Y'){
				done=1;
			}
		}
		
		if(HAL_FLASH_Unlock()!=HAL_OK){
			sprintf(uartOutBuffer, "Error to unlock flash\r\n\r\n");
		}
		
		
#
		EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page = FLASH_SENS_PAGE;
    EraseInitStruct.NbPages = 1;
	


    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK){ 
			sprintf(uartOutBuffer, "Error erasing flash\r\n\r\n");
		}else if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_GAIN_ADDR, tmpGain)!=HAL_OK){
			sprintf(uartOutBuffer, "Writing into flash was unsuccesfull\r\n\r\n");
		}else if(HAL_FLASH_Lock() !=HAL_OK){
			sprintf(uartOutBuffer, "Error to lock flash\r\n\r\n");
		}else{
			sprintf(uartOutBuffer, "Successfully stored\r\n\r\n");
		}
		HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
	}
	
	if(first_flash){
		HAL_FLASH_Unlock();




		EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page = FLASH_SENS_PAGE;
    EraseInitStruct.NbPages = 1;


    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
		
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_GAIN_ADDR, tmpGain);
		HAL_FLASH_Lock();
		sprintf(uartOutBuffer, "Default value (%d) used\r\n\r\n",tmpGain);
		HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
	}
}

uint8_t TestDialog(){
	uint8_t result = 0;
	int16_t tmp_ref= 0;
	int16_t tmp_gas= 0;
	float tmp_temp = 0;
	int32_t time=0;
	
	enum STATES {INIT=0,STATIC,WAIT_FOR_JUMPER,WAIT_FOR_RELEASE,RELEASED,FAILED,ABORTED,DONE};
	uint8_t state=INIT;
	
	while (state!=DONE){
		
		if(state==INIT){   //init ask to run test procedure 
			Message=0;
			sprintf(uartOutBuffer, "\r\n\r\nRun test procedure? (Y/N)\r\n");
			HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000); 
			while(Message==0){};
			if(Message=='y' ||Message=='Y'){
				state=STATIC;
			}else{
				state=ABORTED;
			}
			Message=0;
		
		}else if (state==STATIC){   //perform static test
			tmp_gas=Get_ADC_value(ANALOG_GAS_VALUE_PIN);
			if(tmp_gas>650 || tmp_gas<450){
				result = 1;
			}
			tmp_ref=Get_ADC_value(ANALOG_REF_VOLT_VALUE_PIN);
			if(tmp_ref>600 || tmp_ref<500){
				result = 1;
			}
			tmp_temp = Get_Temperature();
			if(tmp_temp>50 || tmp_temp<15){
				result = 1;
			}
			if(result==0){
				sprintf(uartOutBuffer, "Static measurement: OK \r\n");
				state=WAIT_FOR_JUMPER;
			}else{
				sprintf(uartOutBuffer, "Static measurement: FAIL \r\nGAS:%d REF:%d TEMP:%.1f\r\n",tmp_gas,tmp_ref,tmp_temp);
				state=FAILED;
			}
			HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000); 
		
		}else if (state==WAIT_FOR_JUMPER){
			Test_Pin_Init();
			
			Message=0;
			time=HAL_GetTick();
			while (Message==0 && Get_ADC_value(ANALOG_GAS_VALUE_PIN)>80 && (HAL_GetTick()-time)<30000){};
			if (Message!=0){
				state=ABORTED;
				result = 2;
				Message=0;
			}else if((HAL_GetTick()-time)>=30000){
				sprintf(uartOutBuffer, "    JP2 pin 1 was not connected in 30 seconds\r\n");
				HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
				state=ABORTED;
				result = 2;
				Message=0;
			}else{
				state=WAIT_FOR_RELEASE;
				HAL_Delay(1000);
			}
			
		}else if (state==WAIT_FOR_RELEASE){
			Test_Pin_HiZ();
			sprintf(uartOutBuffer, "Sensor response: WAIT \r");
			HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
			time=HAL_GetTick();
			
			while (Get_ADC_value(ANALOG_GAS_VALUE_PIN)<100 && (HAL_GetTick()-time)<10000){};
			if(HAL_GetTick()-time>=10000){
				sprintf(uartOutBuffer, "Sensor response: FAIL (JP2 was not disconnected in 10s)\r\n");
				HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
				result = 1;
				state = FAILED;
			}else{
				state=RELEASED;
			}
			
		}else if(state == RELEASED){
			HAL_Delay(2000);
			tmp_gas=Get_ADC_value(ANALOG_GAS_VALUE_PIN);
			tmp_ref=Get_ADC_value(ANALOG_REF_VOLT_VALUE_PIN);
			
			if(tmp_gas<0.8*tmp_ref && tmp_gas>0.3*tmp_ref){
				sprintf(uartOutBuffer, "Sensor response:    OK     \r\n");
				HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
				sprintf(uartOutBuffer, "Test procedue:      PASSED\r\n\r\n");
				state=DONE;	
			}else{
				sprintf(uartOutBuffer, "Sensor response:    FAIL \r\n    GAS:%d REF:%d\r\n",tmp_gas,tmp_ref);
				result = 1;
				state=FAILED;
			}
			HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
		
		}else if(state == FAILED){
			sprintf(uartOutBuffer, "Test procedue:  FAILED\r\n\r\n");
			HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
			state=DONE;
		
		}else if(state == ABORTED){
			sprintf(uartOutBuffer, "Test has been:  ABORTED\r\n\r\n");
			HAL_UART_Transmit(&UartHandle, (uint8_t*)uartOutBuffer, strlen(uartOutBuffer), 5000);
			state=DONE;
		}
		
	}
	return result;
}

	

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and 
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete */
  UartReady = SET;
	Message=uartInBuffer[0];
	HAL_UART_Receive_IT(UartHandle, (uint8_t*)uartInBuffer, 1);
}


/**
  * @brief  Configures LED GPIO.
  * @param  Led: Specifies the Led to be configured. 
  *   This parameter can be one of following parameters:
  *            @arg  LED2
  * @retval None
  */
void Test_Pin_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /* Enable the GPIO_LED Clock */
  LEDx_GPIO_CLK_ENABLE(LED1);

  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = NUCLEO_ANALOG_PIN[1];
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  
  HAL_GPIO_Init(NUCLEO_ANALOG_GPIO[1], &GPIO_InitStruct);
  HAL_GPIO_WritePin(NUCLEO_ANALOG_GPIO[1], NUCLEO_ANALOG_PIN[1], GPIO_PIN_SET); 
}

void Test_Pin_HiZ(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /* Enable the GPIO_LED Clock */
  LEDx_GPIO_CLK_ENABLE(LED1);

  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = NUCLEO_ANALOG_PIN[1];
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  
  HAL_GPIO_Init(NUCLEO_ANALOG_GPIO[1], &GPIO_InitStruct);
  HAL_GPIO_WritePin(NUCLEO_ANALOG_GPIO[1], NUCLEO_ANALOG_PIN[1], GPIO_PIN_RESET); 
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on. 
  *   This parameter can be one of following parameters:
  *            @arg  LED2
  * @retval None
  */
void Test_Pin_On(void)
{
  HAL_GPIO_WritePin(NUCLEO_ANALOG_GPIO[1], NUCLEO_ANALOG_PIN[1], GPIO_PIN_SET); 
}

/**
  * @brief  Turns selected LED Off. 
  * @param  Led: Specifies the Led to be set off. 
  *   This parameter can be one of following parameters:
  *            @arg  LED2
  * @retval None
  */
void Test_Pin_Off(void)
{
  HAL_GPIO_WritePin(NUCLEO_ANALOG_GPIO[1], NUCLEO_ANALOG_PIN[1], GPIO_PIN_RESET); 
}

/**
* @brief  wifi_http_post
*         Issue an HTTP GET of the given path to the specified host
* @param  hostname: Target host. DNS resolvable name or IP address.
* @param  path: Document path
* @param  port_number: port number to connect to
* @retval WiFi_Status_t : status of Http Post Request
*/

/*WiFi_Status_t wifi_http_post(uint8_t * hostname, uint8_t * path, uint32_t port_number)
{
  if(hostname == NULL || path == NULL)
    return WiFi_MODULE_ERROR;
  
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  
  while(IO_status_flag.sock_read_ongoing || IO_status_flag.WIND64_count!= 0)//wait till any pending data is read
    {
      __NOP(); //nothing to do
    }
  
  // AT+S.HTTPPOST = posttestserver.com,/post.php,name=demo&email=mymail&subject=subj&body=message<cr>
  Queue_Http_Event(hostname, path, port_number,1);
  
  //Make the user wait anyway
  status = USART_Receive_AT_Resp( );
  return status;
}
*/
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

