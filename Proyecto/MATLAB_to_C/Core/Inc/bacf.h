/*
 * bacf.h
 *
 *  Created on: Oct 22, 2022
 *      Author: franciscomanoukian
 */

#ifndef INC_BACF_H_
#define INC_BACF_H_

#include "main.h"

uint8_t count_bits(uint32_t word);

void BACF_Compute_1(uint32_t * bs_in_ptr, uint32_t * out_ptr, uint32_t in_samples_len);
void BACF_Compute_2(uint32_t * bs_in_ptr, uint32_t * out_ptr, uint32_t in_samples_len);

#endif /* INC_BACF_H_ */
