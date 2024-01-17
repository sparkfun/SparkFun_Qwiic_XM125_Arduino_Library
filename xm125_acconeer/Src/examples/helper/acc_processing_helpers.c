// Copyright (c) Acconeer AB, 2022
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.


#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "acc_control_helper.h"
#include "acc_processing_helpers.h"

#include "acc_integration_log.h"


#define MAX_DATA_ENTRY_LEN_IQ    87
#define MAX_DATA_ENTRY_LEN_FLOAT 44


acc_vector_iq_t *acc_vector_iq_alloc(uint32_t data_length)
{
	acc_vector_iq_t *vector_iq = malloc(sizeof(acc_vector_iq_t));

	if (vector_iq == NULL)
	{
		return NULL;
	}

	vector_iq->data = malloc(data_length * sizeof(float complex));

	if (vector_iq->data == NULL)
	{
		free(vector_iq);
		return NULL;
	}

	vector_iq->data_length = data_length;

	return vector_iq;
}


acc_vector_float_t *acc_vector_float_alloc(uint32_t data_length)
{
	acc_vector_float_t *vector_float = malloc(sizeof(acc_vector_float_t));

	if (vector_float == NULL)
	{
		return NULL;
	}

	vector_float->data = malloc(data_length * sizeof(float));

	if (vector_float->data == NULL)
	{
		free(vector_float);
		return NULL;
	}

	vector_float->data_length = data_length;

	return vector_float;
}


void acc_vector_iq_free(acc_vector_iq_t *vector)
{
	if (vector == NULL)
	{
		return;
	}

	free(vector->data);
	free(vector);
}


void acc_vector_float_free(acc_vector_float_t *vector)
{
	if (vector == NULL)
	{
		return;
	}

	free(vector->data);
	free(vector);
}


float acc_processing_helper_tc_to_sf(float time_constant_s, float update_rate_hz)
{
	if (time_constant_s <= 0.0f)
	{
		return 0.0f;
	}
	else
	{
		return expf(-1.0f / (time_constant_s * update_rate_hz));
	}
}


float acc_processing_helper_dynamic_sf(float static_sf, uint32_t update_count)
{
	return fminf(static_sf, 1.0f - 1.0f / (1.0f + update_count));
}


void acc_vector_iq_update_exponential_average(const acc_vector_iq_t *current, acc_vector_iq_t *averaged_data, float sf)
{
	assert(current->data_length == averaged_data->data_length);

	for (uint32_t i = 0; i < current->data_length; i++)
	{
		averaged_data->data[i] = sf * averaged_data->data[i] + (1.0f - sf) * current->data[i];
	}
}


void acc_vector_float_update_exponential_average(const acc_vector_float_t *current, acc_vector_float_t *averaged_data, float sf)
{
	assert(current->data_length == averaged_data->data_length);

	for (uint32_t i = 0; i < current->data_length; i++)
	{
		averaged_data->data[i] = sf * averaged_data->data[i] + (1.0f - sf) * current->data[i];
	}
}


void acc_get_iq_sweep_vector(const acc_control_helper_t *control_helper_state, acc_vector_iq_t *vector_out)
{
	uint32_t data_length = control_helper_state->proc_meta.sweep_data_length;

	assert(data_length == vector_out->data_length);

	for (uint32_t i = 0; i < data_length; i++)
	{
		acc_int16_complex_t iq = control_helper_state->proc_result.frame[i];
		vector_out->data[i] = iq.real + iq.imag * I;
	}
}


void acc_get_iq_point_vector(const acc_control_helper_t *control_helper_state, uint32_t point, acc_vector_iq_t *vector_out)
{
	assert(point < control_helper_state->proc_meta.sweep_data_length);

	assert(vector_out->data_length == (control_helper_state->proc_meta.frame_data_length / control_helper_state->proc_meta.sweep_data_length));

	for (uint32_t i = 0; i < vector_out->data_length; i++)
	{
		acc_int16_complex_t iq = control_helper_state->proc_result.frame[point + i * control_helper_state->proc_meta.sweep_data_length];
		vector_out->data[i] = iq.real + iq.imag * I;
	}
}


uint32_t acc_processing_helper_get_filter_length(uint32_t peak_width_points, uint32_t step_length)
{
	return (peak_width_points / step_length) | 1;
}


void acc_vector_float_create_depth_filter_vector(acc_vector_float_t *vector_out)
{
	uint32_t data_length = vector_out->data_length;

	assert(data_length % 2 == 1);

	uint32_t mid_offset = (data_length + 1) / 2;
	float    sum        = 0.0f;

	for (uint32_t i = 0; i < data_length; i++)
	{
		if (i < mid_offset)
		{
			vector_out->data[i] = (float)i + 1;
		}
		else
		{
			vector_out->data[i] = (float)(data_length - i);
		}

		sum += vector_out->data[i];
	}

	// Normalize
	for (uint32_t i = 0; i < data_length; i++)
	{
		vector_out->data[i] /= sum;
	}
}


void acc_vector_iq_apply_filter(const acc_vector_iq_t *vector_a, acc_vector_float_t *filter_vector, acc_vector_iq_t *vector_out)
{
	assert(vector_a->data_length == vector_out->data_length);
	assert(filter_vector->data_length % 2 == 1); // Filter length must be odd

	uint32_t offset = filter_vector->data_length  / 2;

	for (uint32_t i = 0; i < vector_out->data_length; i++)
	{
		vector_out->data[i] = 0.0f;

		for (uint32_t j = 0; j < filter_vector->data_length; j++)
		{
			int32_t a_index = i + j - offset;
			if (a_index >= 0 && (uint32_t)a_index < vector_a->data_length)
			{
				vector_out->data[i] += vector_a->data[a_index] * filter_vector->data[j];
			}
		}
	}
}


void acc_vector_iq_copy(const acc_vector_iq_t *vector_a, acc_vector_iq_t *vector_out)
{
	assert(vector_a->data_length == vector_out->data_length);

	for (uint32_t i = 0; i < vector_a->data_length; i++)
	{
		vector_out->data[i] = vector_a->data[i];
	}
}


void acc_vector_iq_add(const acc_vector_iq_t *vector_a, const acc_vector_iq_t *vector_b, acc_vector_iq_t *vector_out)
{
	assert(vector_a->data_length == vector_b->data_length);
	assert(vector_a->data_length == vector_out->data_length);

	for (uint32_t i = 0; i < vector_a->data_length; i++)
	{
		vector_out->data[i] = vector_a->data[i] + vector_b->data[i];
	}
}


void acc_vector_iq_subtract(const acc_vector_iq_t *vector_a, const acc_vector_iq_t *vector_b, acc_vector_iq_t *vector_out)
{
	assert(vector_a->data_length == vector_b->data_length);
	assert(vector_a->data_length == vector_out->data_length);

	for (uint32_t i = 0; i < vector_a->data_length; i++)
	{
		vector_out->data[i] = vector_a->data[i] - vector_b->data[i];
	}
}


void acc_vector_iq_mult(const acc_vector_iq_t *vector_a, const acc_vector_iq_t *vector_b, acc_vector_iq_t *vector_out)
{
	assert(vector_a->data_length == vector_b->data_length);
	assert(vector_a->data_length == vector_out->data_length);

	for (uint32_t i = 0; i < vector_a->data_length; i++)
	{
		vector_out->data[i] = vector_a->data[i] * vector_b->data[i];
	}
}


void acc_vector_iq_mult_conj(const acc_vector_iq_t *vector_a, const acc_vector_iq_t *vector_b, acc_vector_iq_t *vector_out)
{
	assert(vector_a->data_length == vector_b->data_length);
	assert(vector_a->data_length == vector_out->data_length);

	for (uint32_t i = 0; i < vector_a->data_length; i++)
	{
		vector_out->data[i] = vector_a->data[i] * conjf(vector_b->data[i]);
	}
}


void acc_vector_iq_rotate_phase_inline(acc_vector_iq_t *vector_a, float radians)
{
	float complex rotated_unit_vector = cexpf(radians*I);

	for (uint32_t i = 0; i < vector_a->data_length; i++)
	{
		vector_a->data[i] *= rotated_unit_vector;
	}
}


void acc_vector_iq_conj_inline(acc_vector_iq_t *vector_a)
{
	for (uint32_t i = 0; i < vector_a->data_length; i++)
	{
		vector_a->data[i] = conjf(vector_a->data[i]);
	}
}


void acc_vector_iq_amplitude(const acc_vector_iq_t *vector_a, acc_vector_float_t *vector_out)
{
	assert(vector_a->data_length == vector_out->data_length);

	for (uint32_t i = 0; i < vector_a->data_length; i++)
	{
		vector_out->data[i] = cabsf(vector_a->data[i]);
	}
}


float acc_vector_iq_coherent_mean_amplitude(const acc_vector_iq_t *vector_a)
{
	assert(vector_a->data_length > 0);

	float complex coherent_mean = 0;

	for (uint32_t i = 0; i < vector_a->data_length; i++)
	{
		coherent_mean += vector_a->data[i];
	}

	return cabsf(coherent_mean / vector_a->data_length);
}


float acc_vector_iq_noncoherent_mean_amplitude(const acc_vector_iq_t *vector_a)
{
	assert(vector_a->data_length > 0);

	float noncoherent_mean_amplitude = 0;

	for (uint32_t i = 0; i < vector_a->data_length; i++)
	{
		noncoherent_mean_amplitude += cabsf(vector_a->data[i]);
	}

	return noncoherent_mean_amplitude / vector_a->data_length;
}


void acc_vector_iq_phase(const acc_vector_iq_t *vector_a, acc_vector_float_t *vector_out)
{
	assert(vector_a->data_length == vector_out->data_length);

	for (uint32_t i = 0; i < vector_a->data_length; i++)
	{
		vector_out->data[i] = cargf(vector_a->data[i]);
	}
}


uint32_t acc_vector_float_argmax(acc_vector_float_t *vector_a)
{
	float max       = -INFINITY;
	int   max_index = -1;

	for (uint32_t i = 0; i < vector_a->data_length; i++)
	{
		if (vector_a->data[i] > max)
		{
			max       = vector_a->data[i];
			max_index = i;
		}
	}

	return max_index;
}


uint32_t acc_vector_float_argmax_skip_edges(acc_vector_float_t *vector_a, uint32_t elemets_to_skip)
{
	float max       = -INFINITY;
	int   max_index = -1;

	assert(elemets_to_skip < vector_a->data_length - elemets_to_skip);

	for (uint32_t i = elemets_to_skip; i < (vector_a->data_length - elemets_to_skip); i++)
	{
		if (vector_a->data[i] > max)
		{
			max       = vector_a->data[i];
			max_index = i;
		}
	}

	return max_index;
}


float acc_processing_helper_interpolate_peak_position(float y1, float y2, float y3)
{
	return (y1 - y3) / (2.0f * y1 - 4.0f * y2 + 2.0f * y3);
}


void acc_vector_float_print(const char *label, acc_vector_float_t *vector_a)
{
	printf("%s:\n", label);

	char buffer[MAX_DATA_ENTRY_LEN_FLOAT];

	for (uint16_t i = 0; i < vector_a->data_length; i++)
	{
		if ((i > 0) && ((i % 8) == 0))
		{
			printf("\n");
		}

		snprintf(buffer, sizeof(buffer), "%" PRIfloat " ", ACC_LOG_FLOAT_TO_INTEGER(vector_a->data[i]));

		printf("%14s ", buffer);
	}

	printf("\n");
}


void acc_vector_iq_print(const char *label, acc_vector_iq_t *vector_a)
{
	printf("%s: ", label);

	char buffer[MAX_DATA_ENTRY_LEN_IQ];

	for (uint16_t i = 0; i < vector_a->data_length; i++)
	{
		if ((i > 0) && ((i % 8) == 0))
		{
			printf("\n");
		}

		snprintf(buffer, sizeof(buffer), "%" PRIfloat "+%" PRIfloat "i",
		         ACC_LOG_FLOAT_TO_INTEGER(crealf(vector_a->data[i])),
		         ACC_LOG_FLOAT_TO_INTEGER(cimagf(vector_a->data[i])));

		printf("%14s ", buffer);
	}

	printf("\n");
}
