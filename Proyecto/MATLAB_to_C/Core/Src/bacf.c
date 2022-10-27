/*
 * bacf.c
 *
 *  Created on: Oct 22, 2022
 *      Author: franciscomanoukian
 */


#include "bacf.h"

uint8_t count_bits(uint32_t word)
{
	uint8_t count = 0;
	while(word)
	{
		count += word & 0x01;
		word >>= 1;
	}
	return count;
}

void BACF_Compute_1(uint32_t * bs_in_ptr, uint32_t * out_ptr, uint32_t in_samples_len)
{
	//bs: "bitstream" (buffer de unos y ceros)

	uint32_t in_words = in_samples_len / 32; 	//cantidad de palabras de entrada
	uint32_t out_len = in_samples_len / 2;		//largo del buffer de salida
	uint32_t max_bs_shift = in_words / 2;

	uint32_t out_index = 0;
	uint32_t bs_index = 0;
	uint32_t global_shift = 0;
	uint32_t local_shift = 0;
	uint32_t aux = 0;
	uint32_t word1, word2;

	uint32_t *static_ptr = bs_in_ptr;
	uint32_t *shift_ptr = bs_in_ptr;

	while(out_index < out_len) //iteración para cada muestra de salida
	{
		out_ptr[out_index] = 0;
		static_ptr = bs_in_ptr;
		shift_ptr = bs_in_ptr + (global_shift / 32);

		bs_index = 0;
		while(bs_index < max_bs_shift)
		{
			word1 = *static_ptr;

			word2 = (*shift_ptr) << local_shift;
			word2 |= (*(shift_ptr + 1)) >> (32 - local_shift);

			aux = word1 ^ word2;
			out_ptr[out_index] += count_bits(aux);

			bs_index++;
			static_ptr++;
			shift_ptr++;
		}

		out_index++;
		global_shift++;
		local_shift = global_shift % 32;
	}
}

void BACF_Compute_2(uint32_t * bs_in_ptr, uint32_t * out_ptr, uint32_t in_samples_len)
{
	//bs: "bitstream" (buffer de unos y ceros)

	uint32_t in_words = in_samples_len / 32; 	//cantidad de palabras de entrada
	uint32_t out_len = in_samples_len / 2;		//largo del buffer de salida
	uint32_t max_bs_shift = in_words / 2;

	uint32_t out_index = 0;
	uint32_t bs_index = 0;
	uint32_t global_shift = 0;
	uint32_t local_shift = 0;
	uint32_t aux = 0;
	uint32_t word1, word2, word3, word4;

	uint32_t *static_ptr = bs_in_ptr;
	uint32_t *shift_ptr = bs_in_ptr;

	while(out_index < out_len) out_ptr[out_index++] = 0;

	while(bs_index < max_bs_shift) //itero para cada buffer de bits
	{
		out_index = 0; // reseteo el contador de salida
		word1 = *static_ptr; 		// primera palabra siempre es igual
		word2 = *shift_ptr;	 		// segunda palabra empieza igual a la primera,
									// pero va rotando cada 32 (lo hago en el loop)
		word3 = *(shift_ptr + 1);	// esta palabra siempre está una adelante de la segunda

		while(out_index < out_len) 	// itero para cada muestra de salida
		{
			word4 = word2 << local_shift | word3 >> (32 - local_shift);
									// auxiliar de desplazamiento inter-buffer
			aux = word1 ^ word4;	// auxiliar de xor
			out_ptr[out_index] += count_bits(aux); //calculo de salida

			global_shift++;			// aumento el shift global (quiza no hace falta)
			local_shift = global_shift % 32; // este solo sirve para el desplazamiento
									// inter-buffer (desplazamiento relativo)
			if(!local_shift) 		// despl. relativo == 0 ? roto el buffer desplazable
			{
				shift_ptr++;
				word2 = word3;
				word3 = *(shift_ptr + 1);
			}

			out_index++;
		}

		bs_index++;					// calcule todas las salidas con el primer buffer de bits?
									// incremento el indice y roto todos los buffers
		static_ptr++;
		shift_ptr = static_ptr;
	}
}
