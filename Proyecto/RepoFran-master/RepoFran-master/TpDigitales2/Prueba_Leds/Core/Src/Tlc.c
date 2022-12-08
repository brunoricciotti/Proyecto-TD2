/*
 * Tlc.c
 *
 *  Created on: 28 oct. 2022
 *      Author: perei
 */

#include "Tlc.h"

/*uint32_t tlc_data_leds[24];


uint32_t LEDS_BUFFER[9]; //288 bits en variables de 32 bits: el de mi dibujo

TLC_data_cero();
	 		  //enviarDatos();
	 		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //Pin de enable

	 		  	  HAL_SPI_Transmit(&hspi2, (uint8_t *)LEDS_BUFFER, 36, HAL_MAX_DELAY);

	 		  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //Pin de enable

	 		  HAL_Delay(500);

	 		  set_led_val (6, 0xFFFF);
	 		 // enviarDatos();
	 		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //Pin de enable

	 		  	  HAL_SPI_Transmit(&hspi2, (uint8_t *)LEDS_BUFFER, 36, HAL_MAX_DELAY);

	 		  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //Pin de enable
	 		  HAL_Delay(500);

	 		  TLC_data_uno();
	 		  //enviarDatos();
	 		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //Pin de enable

	 		  	  HAL_SPI_Transmit(&hspi2, (uint8_t *)LEDS_BUFFER, 36, HAL_MAX_DELAY);

	 		  	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //Pin de enable
	 		  HAL_Delay(500);

	 	  }

	   /* USER CODE END 3
	 }

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
	 	//led_index de dice el numero de led que está, vale de 0 a 23
	 	//de led_val me importan los ultimos 12 SIEMPRE


	 	//en cual de LEDS_BUFFER tengo que escribir (de los 9)
	 	uint8_t buffer_index = (led_index*3) >> 3; // multiplico por 3/8
	 	uint8_t shift_index = (led_index*12)-(32*buffer_index);
	 	led_val &= 0x0FFF;

	 	if ((shift_index != 24) && (shift_index != 28)) //el dato está entero, no se divide en 2 posiciones distintas
	 	{
	 		LEDS_BUFFER[buffer_index] &= (!(0xFFF00000 >> (shift_index/4))); //limpio datos anteriores
	 		LEDS_BUFFER[buffer_index] |= ((uint32_t)led_val << (20-shift_index));
	 	}
	 	if (shift_index == 24)
	 	{
	 		//caso de 2,10 y 18
	 		LEDS_BUFFER[buffer_index] &= (0xFFFFFF00); //limpio datos anteriores
	 		LEDS_BUFFER[buffer_index] |= ((uint32_t)led_val >> 4); //primeros 8 bits de led_val van aca
	 		LEDS_BUFFER[buffer_index+1] &= (0x0FFFFFFF); //limpio datos anteriores
	 		LEDS_BUFFER[buffer_index+1] |= ((uint32_t)led_val << 28); //ultimos 4 bits de led_val van acá
	 	}
	 	if (shift_index == 28)
	 	{
	 		//caso de 5,13 y 21
	 		LEDS_BUFFER[buffer_index] &= (0xFFFFFFF0); //limpio datos anteriores
	 		LEDS_BUFFER[buffer_index] |= ((uint32_t)led_val >> 8); //primeros 4 bits de led_val van aca
	 		LEDS_BUFFER[buffer_index+1] &= (0x00FFFFFF); //limpio datos anteriores
	 		LEDS_BUFFER[buffer_index+1] |= ((uint32_t)led_val << 24); //ultimos 8 bits de led_val van acá
	 	}

	 /* SI FUNCIONA VERIFICAR ESTO
	 	//uno es 255 el otro 15*
	 	//si es 24, anteultimo bit en 0, si es 28, anteultimo bit en 0xF
	 	LEDS_BUFFER[buffer_index] &= ((0xFFFFFFF0) & (((uint32_t)shift_index-24)<<4));
	 	LEDS_BUFFER[buffer_index+1] &= ((0x0FFFFFFF) & ((!((uint32_t)shift_index-24))<< 28)); //limpio datos anteriores
	 	LEDS_BUFFER[buffer_index] |= ((uint32_t)led_val >> (shift_index-20));
	 	aa = (shift_index - 26)*2; //-2 o 2 si lo duplico es -4 o 4
	 	LEDS_BUFFER[buffer_index+1] |= ((uint32_t)led_val << (shift_index - aa));


	 }

	 void enviarDatos(void)
	 {
	 	int i =0;
	 	/*for (i = 0; i< 9; i++)
	 	  {
	 		  datos[i][0] = LEDS_BUFFER[i] >> 24;
	 		  datos[i][1] = ((LEDS_BUFFER[i] >> 16) & 0x00FF);
	 		  datos[i][2] = ((LEDS_BUFFER[i] >> 8) & 0x0000FF);
	 		  datos[i][3] = ((LEDS_BUFFER[i]) & 0x000000FF);
	 		//  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //Pin de enable
	 		 /* HAL_SPI_Transmit(&hspi2, &datos[i], 4, TIMEOUT);
	 		  HAL_SPI_Transmit(&hspi2, &datos[i][1], 1, TIMEOUT);
	 		  HAL_SPI_Transmit(&hspi2, &datos[i][2], 1, TIMEOUT);
	 		  HAL_SPI_Transmit(&hspi2, &datos[i][3], 1, TIMEOUT);

	 		//  HAL_SPI_Transmit(&hspi2, datos[i], 4, TIMEOUT);
	 		//  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); //Pin de enable
	 	  //}

	 	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //Pin de enable

	 	  HAL_SPI_Transmit(&hspi2, (uint8_t *)LEDS_BUFFER, 36, HAL_MAX_DELAY);

	 	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //Pin de enable

	 }

*/
