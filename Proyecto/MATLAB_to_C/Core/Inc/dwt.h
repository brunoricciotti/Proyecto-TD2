/*
 * dwt.h
 *
 *  Created on: 12 may. 2021
 *      Author: esala
 */

#ifndef INTERFAZ_INC_DWT_H_
#define INTERFAZ_INC_DWT_H_
#define dwt_inic()	(DWT->CTRL|= DWT_CTRL_CYCCNTENA_Msk)
#define dwt_reset()	(DWT->CYCCNT=0)
#define dwt_read()	(DWT->CYCCNT)


#endif /* INTERFAZ_INC_DWT_H_ */
