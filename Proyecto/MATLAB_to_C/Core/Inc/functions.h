/*
 * functions.h
 *
 *  Created on: Oct 19, 2022
 *      Author: bruno
 */

#ifndef INC_FUNCTIONS_H_
#define INC_FUNCTIONS_H_

#define BIAS_POS	2078
#define BIAS_NEG	2018

uint32_t buff_salida[ADC_BUF_LEN_SALIDA];

uint32_t* zero_cross_hyst(uint32_t* ADC_vector);

#endif /* INC_FUNCTIONS_H_ */
