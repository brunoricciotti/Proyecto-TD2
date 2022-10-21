/*
 * functions.c
 *
 *  Created on: Oct 19, 2022
 *      Author: bruno
 */
#include "main.h"
#include "functions.h"


uint32_t* zero_cross_hyst(uint32_t* ADC_vector){

	uint16_t i,j;
	uint8_t actual;
	uint32_t *buff_ptr = buff_salida;

	if(ADC_vector[0] > BIAS_POS) actual = 1;
	else actual = 0;

	*(buff_salida) |= (actual << 0);

	for(i=1;i<ADC_BUF_LEN_SALIDA;i++){
		*buff_ptr = 0;
		//empieza for de 0 a 31
		for(j=0; j < 32; j++){

			if((ADC_vector[i] > BIAS_POS)&&(actual == 0)) 	    actual = 1;
			else if((ADC_vector[i] < BIAS_NEG)&&(actual == 1))	actual = 0;

			*(buff_ptr) |= (actual <<i%32);
		}

		buff_ptr++;
	}

	return buff_ptr;
}
