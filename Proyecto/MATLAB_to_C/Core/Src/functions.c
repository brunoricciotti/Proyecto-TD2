/*
 * functions.c
 *
 *  Created on: Oct 19, 2022
 *      Author: bruno
 */
#include "main.h"
#include "functions.h"


void zero_cross_hyst(uint32_t* ADC_vector,uint32_t* buff_salida){

	uint16_t i,j,x;
	uint8_t actual;
	uint32_t *buff_ptr = buff_salida;

	for(i=0;i<ADC_BUF_SALIDA_LEN;i++){
		*buff_ptr = 0;
		//empieza for de 0 a 31
		for(j=0; j < 32; j++){

			x = 32*(i) + j;

			if((ADC_vector[x] > BIAS_POS)&&(actual == 0)) 	    actual = 1;
			else if((ADC_vector[x] < BIAS_NEG)&&(actual == 1))	actual = 0;

			*(buff_ptr) |= (actual << (31 - j % 32));
		}

		buff_ptr++;
	}
}

uint16_t closest_period(uint16_t period){

	uint16_t i,j,period_aprox;

	for(i=0; i < CANT_OCTAVAS; i++){

		if(period < ciclos[CANT_CICLOS-1]/(i+1)){

			for(j=0; j < CANT_CICLOS-1; j++){

				if((period < ciclos[j+1]/(i+1)) && (freq > ciclos[j]/(i+1))){

					freq_aprox = ciclos[j]*(i+1);

				}
			}

		}

	}
	return freq_aprox;
}
