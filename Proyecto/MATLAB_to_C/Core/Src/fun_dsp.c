/*
 * fun_dsp.c
 *
 *  Created on: Feb 21, 2021
 *      Author: franciscomanoukian
 */


#include "data_types.h"
#include "stdlib.h"
#include "stdarg.h"
#include "dsp.h"


/*
 *
 * Generador de stream de datos binarios
 *
 * 	Toma una señal a través de un puntero a int32_t
 * 	y devuelve el stream de cruces por cero a través de otro
 * 	puntero a uint32_t. Asume que ambos ya están declarados
 * 	y contienen el espacio suficiente.
 *
 */

void set_bitstream(volatile int32_t * audiobuf, volatile uint32_t * bitbuf, const uint32_t audio_buffer_length, int32_t maxval_thresh)
{

    uint32_t sample_index = 0, bit_index = 32, bitbuf_index = 0;
    int32_t prev_sample_index = 0;
    int32_t current_max;
    uint8_t zc = 0;

    while(sample_index < audio_buffer_length)
    {

        while((!(zc = zero_cross(audiobuf[sample_index], zc, 100))) && (sample_index < audio_buffer_length))
        {

            bitbuf[bitbuf_index] &= ~(1 << --bit_index);

            if(!bit_index)
            {

                bit_index = 32;
                bitbuf_index++;

            }

            sample_index++;

        }
        //Mientras el cruce por cero de las muestras sea 0, anoto 0 en el stream

        prev_sample_index = sample_index;
        current_max = audiobuf[sample_index];
        sample_index++;
        //Guardo la posición de la primera muestra que dió 1, pero todavía no la anoto

        while((zc = zero_cross(audiobuf[sample_index], zc, 100)) && (sample_index < audio_buffer_length))
        {

            if(audiobuf[sample_index] > current_max) current_max = audiobuf[sample_index];

            sample_index++;

        }
        //Llevo el índice sample_index hasta la posición del nuevo primer 0. Todavía no anoto nada
        if(current_max > maxval_thresh)
        {
        	//Si el máximo de la señal durante el último grupo de unos fue mayor que el umbral, anoto 1s en el rango

            while(prev_sample_index++ < sample_index)
            {

                bitbuf[bitbuf_index] |= (1 << --bit_index);

                if(!bit_index)
                {

                    bit_index = 32;
                    bitbuf_index++;

                }

            }

        }
        else
        {
        	//Si el máximo de la señal durante el último grupo de unos fue menor que el umbral, anoto 0s en el rango

            while(prev_sample_index++ < sample_index)
            {

                bitbuf[bitbuf_index] &= ~(1 << --bit_index);

                if(!bit_index)
                {

                    bit_index = 32;
                    bitbuf_index++;

                }

            }

        }

        //Reinicio el proceso.

    }

}


/*
 *
 * Función de cruces por cero
 *
 */


uint8_t zero_cross(int32_t sample, uint8_t prev_zc, int32_t hysterisis)
{

	uint8_t ret = prev_zc;

	if(!prev_zc)
	{

		if(sample > hysterisis) ret = 1;

	}
	else
	{

		if(sample < -hysterisis) ret = 0;

	}

	return ret;

}


/*
 *
 * Función de autocorrelación bit a bit
 *
 * 	Toma el stream de cruces por cero por un puntero a uint32_t
 * 	y devuelve los resultados del algoritmo a través de otro
 * 	puntero a uint32_t. Asume que ambos están declarados
 * 	y contienen el espacio suficiente.
 *
 * 	Devuelve a través de uint32_t el valor máximo devuelto
 * 	por el algoritmo.
 *
 */


uint32_t bit_autocorrelate(volatile uint32_t * bitbuf, volatile uint32_t * acbuf, uint32_t audio_buffer_length)
{

	uint32_t i, j, shift1, shift2;
	uint32_t aux;
	uint32_t ac_max = 0;
	const uint32_t bitstream_buffer_length = audio_buffer_length >> 5;
	volatile uint32_t * p_0, * p_offset;

	p_0 = p_offset = bitbuf;

	for(i = 0; i < audio_buffer_length >> 1; i++)
	{

		p_0 = bitbuf;
		p_offset = bitbuf + (i / 32);

		shift1 = i % 32;
		shift2 = 32 - shift1;

		acbuf[i] = 0;

		for(j = 0; j < bitstream_buffer_length >> 1; j++)
		{

			if(!shift1)
			{

				aux = (*p_0++ ^ *p_offset++);

			}
			else
			{

				aux = (*p_0++ ^ ((*p_offset << (shift1)) | *(p_offset + 1) >> (shift2)));
				p_offset++;

			}

			acbuf[i] += __builtin_popcountll(aux);

		}

		if(acbuf[i] > ac_max) ac_max = acbuf[i];

	}

	return ac_max;

}


/*
 *
 * Función de estimación de muestras por ciclo
 *
 * 	Busca puntos mínimos en un buffer de autocorrelación
 * 	que toma a través de un puntero. Lleva la cuenta de
 * 	la posición del último encontrado y la cantidad, y al
 * 	llegar al final del buffer calcula el promedio como la
 * 	razón entre ambos. Toma como parámetro una cantidad máxima
 * 	de picos a detectar, con el fin de optimizar el tiempo de
 * 	cómputo cuando el rango de frecuencias esperadas es amplio.
 * 	En caso de que el parámetro "max_peaks" sea cero,
 * 	no se limita la cantidad de picos.
 * 	Según el parámetro "mode", devuelve el estimado de la
 * 	frecuencia (0) o la cantidad de muestras que equivalen
 * 	a un ciclo (!=0)
 *
 * 	Esta funcion NO OFRECE PRECISIÓN ya que trabaja con enteros
 * 	únicamente. La precisión es la necesaria para la aplicación
 * 	en que se utiliza.
 *
 */


uint32_t freq_detect(volatile uint32_t * acbuf, int32_t ac_buffer_length, uint32_t ac_max, uint32_t max_peaks)
{

    uint32_t i = 1;
    uint32_t prev_dv = (acbuf[0] > acbuf[1]), next_dv;
    uint32_t amp_thresh = ac_max >> 1, samp_count_thresh = 5;
    uint32_t n_peaks = 0, last_peak_pos = 0;
    uint32_t est_samples_per_cycle = 0;
    uint32_t compute = 1;

    while((i++ < ac_buffer_length - 1) && compute)
    {

        prev_dv = next_dv;
        next_dv = (acbuf[i + 1] > acbuf[i]);

        if((!prev_dv) && (next_dv) && (acbuf[i] <= amp_thresh)) //Pendiente anterior negativa, actual positiva, amplitud por debajo del umbral
        {

            if(i - last_peak_pos < samp_count_thresh)
            {

                if(acbuf[i] == acbuf[last_peak_pos]) //Ambos picos son de igual valor
                {

                    last_peak_pos = (i + last_peak_pos) / 2; //asumo que hubo un falso negativo entre los dos picos y digo que la ultima posición fue en el medio

                }
                else if(acbuf[i] < acbuf[last_peak_pos]) //El pico actual es de menor valor
                {

                    last_peak_pos = i; //Asumo que el anterior fue un falso positivo y seteo la posición del último

                }
                //En caso de que el pico anterior fuera de menor valor que el actual, no hago nada

            }
            else
            {

                n_peaks++;
                last_peak_pos = i;

            }
            if(n_peaks >= max_peaks) compute = 0;

        }

    }

    if(n_peaks)
    {

        est_samples_per_cycle = last_peak_pos / n_peaks;

    }

    return est_samples_per_cycle;

}


/*
 *
 * Calculadora de promedio de datos de audio
 *
 * 	Esencialmente calcula el promedio entre una cantidad "data_length"
 * 	de datos tomados del buffer referenciado.
 *
 */


uint32_t audio_average(int32_t * data, uint32_t data_length)
{

	uint32_t sum = 0, index = 0;

	while(index < data_length) sum += abs(data[index++]);

	return sum / data_length;

}


/*
 *
 * Mínimo común múltiplo
 *
 */


uint32_t mcm(uint32_t argc, ...)
{

    uint32_t curr_mcm, curr_arg, curr_inc;
    va_list nvals;

    va_start(nvals, argc);

    curr_mcm = va_arg(nvals, uint32_t);
    curr_inc = curr_mcm;

    argc--;

    while(argc--)
    {

        curr_arg = va_arg(nvals, uint32_t);

        //printf("curr_mcm = %d, curr_inc = %d, curr_arg = %d\n", curr_mcm, curr_inc, curr_arg);

        while(curr_mcm % curr_arg)
        {

            curr_mcm += curr_inc;

        }

        curr_inc = curr_mcm;

    }

    return curr_mcm;

}


/*
 *
 * Máximo
 *
 */

int32_t max(uint32_t argc, ...)
{

    uint32_t i = 0;
    int32_t max, curr;
    va_list nvals;

    va_start(nvals, argc);
    max = va_arg(nvals, int32_t);
    argc--;

    while(argc--)
    {

        curr = va_arg(nvals, int32_t);

        if(curr > max) max = curr;

        i++;

    }

    return max;

}
