/*
 * dwt.h
 *
 *  Created on: Jun 28, 2021
 *      Author: esala
 */

#ifndef DWT_H_
#define DWT_H_
	#define dwt_inic()	(DWT->CTRL|= DWT_CTRL_CYCCNTENA_Msk)
	#define dwt_reset()	(DWT->CYCCNT=0)
	#define dwt_read()	(DWT->CYCCNT)
#endif /* DWT_H_ */
