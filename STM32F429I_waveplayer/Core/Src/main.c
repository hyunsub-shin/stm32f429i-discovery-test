/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "touch.h"
#include "mems.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern DMA_HandleTypeDef hdma_dac2;
extern __IO uint16_t TIM6ARRValue;

extern float Gyro[3];
extern unsigned char nexMessage[8];

uint16_t Tmr_cnt1, Tmr_cnt2 = 0;
FATFS *pfs;
FRESULT fres;
DWORD free_clust;
uint32_t total, freee;

char buffer[512];
char c;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	char commandline[25];
	uint16_t tmp=0;
	uint16_t value=0;
	uint16_t buffer[200];
	uint16_t sector_size;
	uint16_t sector_cnt;
	for(int i=0;i<200;i++)
	{
		value = (uint16_t)rint((sinf(((2*3.141592)/200)*i)+1)*2048);
		buffer[i]=(value <4096 ? value : 4095);
	}
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  TIM6ARRValue = 4;
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DAC_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_FATFS_Init();
  MX_ADC3_Init();
  MX_I2C3_Init();
  MX_SPI5_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  MX_TIM6_Init(TIM6ARRValue);

  HAL_UART_Receive_IT(&huart2, nexMessage, 8);
  printf("\r\n\r\n\r\nLED3 OFF\r\n");
  HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_RESET);
  printf("LED4 OFF\r\n");
  HAL_GPIO_WritePin(LED4_GPIO_Port,LED4_Pin,GPIO_PIN_RESET);

  /* Touch */
  TSP_RESET();
  TSP_FnctCmd(IOE_ADC_FCT, ENABLE);
  TSP_TP_Config();

  /* Gyroscope configuration */
  Demo_GyroConfig();
  /* Gyroscope calibration */
  Gyro_SimpleCalibration(Gyro);

  /* TIM2 : PWM Output */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);

  /* DAC Channel2 */
  printf("\r\nDAC Timer Start\r\n");

  /* TIM6 : DAC Trigger */
  HAL_TIM_Base_Start(&htim6);
//  printf("\r\nDAC DMA Start\r\n");
  HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_2, (uint32_t*)buffer, 200, DAC_ALIGN_12B_R);
  HAL_DAC_Start(&hdac, DAC_CHANNEL_2);

  /* TIM3 : Used Fatfs */
  HAL_TIM_Base_Start_IT(&htim3);

  /* SD Card Initial */
  if(MMC_CDState()==1)
  {
	  tmp = disk_initialize(0);
	  printf("disk_initialize return value : %d\r\n", tmp);

	  if(f_mount(&USERFatFS, USERPath, 0) != FR_OK)
	  {
		  printf("mount error\r\n");
	  }
	  disk_ioctl(0, GET_SECTOR_SIZE, &sector_size);
	  disk_ioctl(0, GET_SECTOR_COUNT, &sector_cnt);
	  printf("SECTOR COUNT : %d\r\n", sector_cnt);
	  printf("SECTOR SIZE : %d\r\n", sector_size);

	  if(f_getfree(USERPath, &free_clust, &pfs) != FR_OK)
	  {
		  printf("ger_free error\r\n");
	  }
	  else
	  {
		  printf("mount success\r\n");
	  }
	  total = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
	  freee = (uint32_t)(free_clust * pfs->csize * 0.5);
	  printf("Total size : %d\r\n", total);
	  printf("Free size  : %d\r\n", freee);
	  if(freee < 1)
	  {
		  printf("Not Free clust\r\n");
	  }
	  if(f_mount(NULL, USERPath, 1) != FR_OK)
	  {
		  printf("mount error\r\n");
	  }
	  else
	  {
		  printf("Unmount success\r\n");
	  }
  }
  else
  {
	  printf("SD Card Not Insert!!\r\n");
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  DebugUart_PutStr("\r\nTEST> ");
	  GetCommand(commandline, 25);
	  if(MyStrNCmp(commandline, "ledon", 5) == 0)
	  {
		  HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
	  }
	  else if(MyStrNCmp(commandline, "ledoff", 6) == 0)
	  {
		  HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);
	  }
	  else if(MyStrNCmp(commandline, "tone", 4) == 0)
	  {
		  hdma_dac2.Init.Mode = DMA_CIRCULAR;
		  HAL_TIM_Base_Start(&htim6);
		  HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_2, (uint32_t*)buffer, 200, DAC_ALIGN_12B_R);
		  hdma_dac2.Init.Mode = DMA_NORMAL;
	  }
	  else if(MyStrNCmp(commandline, "stop", 4) == 0)
	  {
		  HAL_TIM_Base_Stop(&htim6);
		  HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_2);
	  }
	  else if(MyStrNCmp(commandline, "freq", 4) == 0)
	  {
		  sscanf(commandline, "%*s %d", &tmp);
		  MX_TIM6_Init(tmp);
	  }
	  else if(MyStrNCmp(commandline, "play", 4) == 0)
	  {
		  uint8_t fileName[13];

		  sscanf(commandline, "%*s %s", &fileName);
		  printf("\r\nFILE NAME -> %s\r\n",fileName);
		  WavePlayer_Start(fileName);
	  }
	  else if(MyStrNCmp(commandline, "led", 3) == 0)
	  {
		  sscanf(commandline, "%*s %d", &tmp);
		  if(tmp==1)
			  HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
		  else
			  HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);
	  }
	  else if(MyStrNCmp(commandline, "readfile",8) == 0)
	  {
		  uint16_t sd_buffer[512];
		  uint8_t fileName[13];
		  uint16_t bytesToRead = 512;
		  uint16_t bytesRead = 0;
		  FRESULT res;

		  for(int i=0;i<512;i++)
		  {
			  sd_buffer[i] = 0;
		  }
		  sscanf(commandline, "%*s %s", &fileName);
		  printf("\n\rFILE NAME -> %s\n\r",fileName);
		  if(f_mount(&USERFatFS, USERPath, 0) != FR_OK)
		  {
			  printf("mount error\r\n");
		  }
		  if(f_open(&USERFile, fileName, FA_READ) == FR_OK)
		  {
			  for(;;)
			  {
				  res = f_read(&USERFile, sd_buffer, bytesToRead, &bytesRead);
				  if(res)
				  {
				  	  printf("\r\nread error\r\n");
				  	  break;
				  }
				  else if(!bytesRead)
				  {
					  printf("\r\nFile Read Complete\r\n\r\n");
					  break;
				  }
				  else
				  {
					  printf((void*)sd_buffer);
					  buff_clr(sd_buffer,512);
				  }
			  }

			  if(f_close(&USERFile) == FR_OK)
			  {
			  	  printf("\r\nfile close\r\n");
			  }
			  else
			  {
				  printf("\r\nfile close error\r\n");
			  }
			  if(f_mount(NULL, USERPath, 1) != FR_OK)
			  {
			  	  printf("\r\nmount error\r\n");
			  }
			  else
			  {
				  printf("\r\nUnmount success\r\n");
			  }
		  }
		  else
		  {
			  printf("file open error\r\n");
		  }
	  }
	  else if(MyStrNCmp(commandline, "scan", 4) == 0)
	  {
		  uint16_t data1, data2, data3;

		  sscanf(commandline, "%*s %x %x %x", &data1, &data2, &data3);
		  printf("data1 = %x\ndata2 = %x\ndata3 = %x\n",data1, data2, data3);
	  }
	  else
	  {
		  printf("command error\n\r");
	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint8_t Touch_Device_ID[2];
	uint8_t Touch_ID_Ver[1];
	uint8_t Mems_ID[1];

	if(GPIO_Pin == USER_INT_Pin)
	{
		printf("*** USER Button INT input ***\r\n");

		HAL_I2C_Mem_Read(&hi2c3, TP_ADDR, IOE_REG_CHP_ID, 1, Touch_Device_ID, 2, 10);
		printf("  ==> Touch Device ID = %02x%02x\r\n", Touch_Device_ID[0], Touch_Device_ID[1]);

		HAL_I2C_Mem_Read(&hi2c3, TP_ADDR, IOE_REG_ID_VER, 1, Touch_ID_Ver, 1, 10);
		printf("  ==> Touch ID Ver = %02x\r\n", Touch_ID_Ver[0]);

		L3GD20_Read(Mems_ID,L3GD20_WHO_AM_I_ADDR,1);
		printf("  ==> MEMS ID Ver = %02X\r\n", Mems_ID[0]);
	}
	else if(GPIO_Pin == TP_INT_Pin)
	{

	}
}
/* USER CODE END 4 */

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
	if(htim->Instance == TIM3)
	{
		if(Tmr_cnt1 > 0)
		{
			Tmr_cnt1--;
		}
		if(Tmr_cnt2 > 0)
		{
			Tmr_cnt2--;
		}
	}
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
