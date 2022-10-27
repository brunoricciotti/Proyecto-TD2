/*
 * dsp.h
 *
 *  Created on: Feb 21, 2021
 *      Author: franciscomanoukian
 */

#ifndef DSP_H_
#define DSP_H_

#include "main.h"

uint32_t bit_count_32(uint32_t var);

uint8_t zero_cross(int32_t sample, uint8_t prev_zc, int32_t hysterisis);

void set_bitstream(volatile int32_t * audiobuf, volatile uint32_t * bitbuf, const uint32_t audio_buffer_length, int32_t maxval_thresh);

uint32_t bit_autocorrelate(volatile uint32_t * bitbuf, volatile uint32_t * acbuf, uint32_t audio_buffer_length);

uint16_t freq_detect(volatile uint32_t * acbuf, int32_t ac_buffer_length, uint32_t ac_max, uint32_t max_peaks);

uint32_t audio_average(int32_t * data, uint32_t data_length);

uint32_t mcm(uint32_t argc, ...);

int32_t max(uint32_t argc, ...);

#endif /* DSP_H_ */
