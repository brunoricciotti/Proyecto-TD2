/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "dwt.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TIMEOUT 		5000
#define TIEMPO_DATOS	500 //ms

#define SALTOS_LEDS		16
#define CANT_LEDS		16
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_tx;

/* USER CODE BEGIN PV */
uint32_t ticks = 0;

enum states {ESPERANDO_BUFFER,SETEOACERO,SET_LEDS,ENVIO_DATOS,ESPERANDO_TRANSFERENCIA};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */
void TLC_data_cero(void);
void TLC_data_uno(void);
void set_led_val(uint8_t, uint16_t);
void enviarDatos(void);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t data[36];
uint32_t LEDS_BUFFER[9];

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	uint8_t i = 4,j=0;
	static uint8_t NextState = ESPERANDO_BUFFER;
	uint
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI2_Init();
  dwt_inic();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


	  dwt_reset();
	  for(j =0;j<20;j++){

		  switch(NextState){
		  case(SETEOACERO):
			  if(flag_graficar == 1)//el buffer se lleno
			  {
				  TLC_data_cero();
				  NextState = SET_LEDS;
			  }
		  break;

		  case(SET_LEDS):
		  	  if(flag_seteoacero == 1)
		  	  {
		  		for(i = 0; i < CANT_LEDS; i++)
		  		{
		  			set_led_val(i, buf[i*SALTOS_LEDS]);//ver si graficar los 16 leds de una o de a partes para no usar mucho % del tic
		  		}
		  		NextState = ENVIO_DATOS;
		  	  }
		  break;

		  case(ENVIO_DATOS):

			  enviarDatos();
			  NextState = ESPERANDO_TRANSFERENCIA;

		  break;

		  case(ESPERANDO_TRANSFERENCIA):
			  if(__HAL_DMA_GET_FLAG(hspi2->hdmatx,DMA_FLAG_TC5))
				{
					// Si entré acá es porque el dma llegó al final del buffer
					//(reinicia solo pero tengo que liberar el flag)
				  	__HAL_DMA_CLEAR_FLAG(hspi2->hdmatx,DMA_FLAG_TC5|DMA_FLAG_HT5|DMA_FLAG_TE5);
					hspi2.State = HAL_DMA_STATE_READY;
					hspi2.hdmatx->State = HAL_DMA_STATE_READY;
					hspi2.hdmatx->Lock = HAL_UNLOCKED;
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);

					NextState = ESPERANDO_TRANSFERENCIA;
				}
		  case(ESPERANDO_BUFFER):
			  if(flag_datos_enviados == 1)
			  {
				  NextState = SETEOACERO;
			  }
		  default:
			  break;
		  }

		  //TC1 transferencia completa canal 1
		  //HT1 transferencia por la mitad canal 1
		  if(__HAL_DMA_GET_FLAG(hspi2->hdmatx,DMA_FLAG_TC5))
			{
				 // Si entré acá es porque el dma llegó al final del buffer
				 //(reinicia solo pero tengo que liberar el flag)
				__HAL_DMA_CLEAR_FLAG(hspi2->hdmatx,DMA_FLAG_TC5|DMA_FLAG_HT5|DMA_FLAG_TE5);
				hspi2.State = HAL_DMA_STATE_READY;
				hspi2.hdmatx->State = HAL_DMA_STATE_READY;
				hspi2.hdmatx->Lock = HAL_UNLOCKED;
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
			}

		  TLC_data_cero();
		  set_led_val(i,0xFFF);
		  enviarDatos();
		  //HAL_Delay(500);
		  i++;
		  if (i == 24)
		  {
			  i = 4;
		  }

	  }
	  if(dwt_read()>ticks)
	  {
	  	ticks = dwt_read();
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_LSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void TLC_data_cero(void)
 {
	uint8_t i = 0;
	for (i = 0; i < 9; i++)
	{
		LEDS_BUFFER[i] = 0;
	}
 }

 void TLC_data_uno(void)
 {
	uint8_t i = 0;
	for (i = 0; i < 9; i++)
	{
		LEDS_BUFFER[i] = 0xFFFFFFFF;
	}
 }


 void set_led_val(uint8_t led_index, uint16_t led_val)
 {
	uint8_t i;
	uint16_t buf[256];



	//led_index de dice el numero de led que está, vale de 0 a 23
	//de led_val me importan los ultimos 12 SIEMPRE


	//en cual de LEDS_BUFFER tengo que escribir (de los 9)
	uint8_t buffer_index = (led_index*3) >> 3; // multiplico por 3/8
	uint8_t shift_index = (led_index*12)-(32*buffer_index);
	led_val &= 0x0FFF;

	if ((shift_index != 24) && (shift_index != 28)) //el dato está entero, no se divide en 2 posiciones distintas
	{
		LEDS_BUFFER[buffer_index] &= ~(((0x00000FFF) << (shift_index))); //limpio datos anteriores
		LEDS_BUFFER[buffer_index] |= ((uint32_t)led_val << (shift_index));
	}
	if (shift_index == 24)
	{
		//caso de 2,10 y 18
		LEDS_BUFFER[buffer_index] &= (0x00FFFFFF); //limpio datos anteriores
		LEDS_BUFFER[buffer_index] |= ((uint32_t)led_val << shift_index); //primeros 8 bits de led_val van aca
		LEDS_BUFFER[buffer_index+1] &= (0xFFFFFFF0); //limpio datos anteriores
		LEDS_BUFFER[buffer_index+1] |= ((uint32_t)led_val >> 8); //ultimos 4 bits de led_val van acá
	}
	if (shift_index == 28)
	{
		//caso de 5,13 y 21
		LEDS_BUFFER[buffer_index] &= (0x0FFFFFFF); //limpio datos anteriores
		LEDS_BUFFER[buffer_index] |= ((uint32_t)led_val << shift_index); //primeros 4 bits de led_val van aca
		LEDS_BUFFER[buffer_index+1] &= (0xFFFFFF00); //limpio datos anteriores
		LEDS_BUFFER[buffer_index+1] |= ((uint32_t)led_val >> 4); //ultimos 8 bits de led_val van acá
	}

 /* SI FUNCIONA VERIFICAR ESTO
	//uno es 255 el otro 15*
	//si es 24, anteultimo bit en 0, si es 28, anteultimo bit en 0xF
	LEDS_BUFFER[buffer_index] &= ((0xFFFFFFF0) & (((uint32_t)shift_index-24)<<4));
	LEDS_BUFFER[buffer_index+1] &= ((0x0FFFFFFF) & ((!((uint32_t)shift_index-24))<< 28)); //limpio datos anteriores
	LEDS_BUFFER[buffer_index] |= ((uint32_t)led_val >> (shift_index-20));
	aa = (shift_index - 26)*2; //-2 o 2 si lo duplico es -4 o 4
	LEDS_BUFFER[buffer_index+1] |= ((uint32_t)led_val << (shift_index - aa));

*/
 }

 void enviarDatos(void)
 {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

	HAL_SPI_Transmit_DMA(&hspi2, (uint8_t *)LEDS_BUFFER, 36);


 }
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
