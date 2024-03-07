// Copyright (c) Acconeer AB, 2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <complex.h>
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "acc_alg_basic_utils.h"
#include "acc_algorithm.h"
#include "acc_definitions_a121.h"
#include "acc_definitions_common.h"

#define DOUBLE_BUFFERING_MEAN_ABS_DEV_OUTLIER_TH 5


//-----------------------------
// Private declarations
//-----------------------------

static void mean_i16_complex(const acc_int16_complex_t *data, uint16_t data_length, float complex *out, uint16_t stride);


static void rfft(const float *data, uint16_t data_length, uint16_t length_shift, float complex *output, uint16_t stride);


static void fftshift(float *data, uint16_t data_length, uint16_t stride);


static void welch(const float complex *data,
                  uint16_t            data_length,
                  uint16_t            segment_length,
                  float complex       *data_buffer,
                  float complex       *fft_out,
                  float               *psd,
                  const float         *window,
                  uint16_t            length_shift,
                  float               fs,
                  uint16_t            stride);


static void filter_inplace_apply(uint16_t    sample_idx,
                                 const float *b,
                                 const float *a,
                                 float       state[5],
                                 float       *data);


static float complex get_data_padded_f32_to_f32_complex(const float *data, uint16_t data_length, uint16_t index, uint16_t stride);


static float complex get_data_padded_f32_complex(const float complex *data, uint16_t data_length, uint16_t index, uint16_t stride);


static void small_rfft(const float *data, uint16_t data_length, uint16_t length_shift, float complex *output, uint16_t stride);


static void small_fft(const float complex *data, uint16_t data_length, uint16_t length_shift, float complex *output, uint16_t stride);


static void small_fft_transformation(uint16_t length_shift, float complex *output, uint16_t stride);


static void small_rfft_real_symmetry_conversion(float complex *output, uint16_t length_shift, uint16_t stride);


/**
 * @brief Interpolate function for Double buffering
 *
 * This function will calculate the interpolated value from the sweep
 * before and the sweep two positions ahead.
 *
 * @param[in, out] frame Data frame to where the filter is applied
 * @param[in] sweeps_per_frame How many sweeps there are in the frame
 * @param[in] num_points The number of points in the frame
 * @param[in] sweep The sweep to generate with interpolatation
 * @param[in] point The point to generate with interpolatation
 */
static void double_buffering_interpolate(acc_int16_complex_t *frame,
                                         const uint16_t      sweeps_per_frame,
                                         const uint16_t      num_points,
                                         const uint16_t      sweep,
                                         const uint16_t      point);


/**
 * @brief Median function for Double buffering
 *
 * This function will calculate the median value of four complex values.
 *
 * @param[in, out] frame Data frame to where the filter is applied
 * @param[in] num_points The number of points in the frame
 * @param[in] sweep The sweep to generate with median filter
 * @param[in] point The point to generate with median filter
 * @param[in] median_start_sweep The start sweep for the median calculation
 */
static void double_buffering_median_filter(acc_int16_complex_t *frame,
                                           const uint16_t      num_points,
                                           const uint16_t      sweep,
                                           const uint16_t      point,
                                           const uint16_t      median_start_sweep);


/**
 * @brief Merge peak cluster
 *
 * @param[in] start_idx Start index
 * @param[in] num_peaks Number of peaks to merge
 * @param[in] velocities Velocities array
 * @param[in] energies Energies array
 * @param[in] peak_idxs Peak indexes array
 * @param[out] merged_velocities Merged velocities array
 * @param[out] merged_energies Merged energies array
 * @param[in] cluster_count Cluster count
 */
static void merge_peak_cluster(uint16_t       start_idx,
                               uint16_t       num_peaks,
                               const float    *velocities,
                               const float    *energies,
                               const uint16_t *peak_idxs,
                               float          *merged_velocities,
                               float          *merged_energies,
                               uint16_t       cluster_count);


/**
 * @brief Get max measurable distance for some PRF
 *
 * @param[in] prf Pulse repetition frequency (PRF)
 * @return Max measurable distance in meters
 */
static float max_measurable_dist(acc_config_prf_t prf);


/**
 * Get profile by value
 *
 * @param[in] value Integer value that corresponds to an ACC_CONFIG_PROFILE enum
 * @return An ACC_CONFIG_PROFILE enum
 */
static acc_config_profile_t get_profile(uint16_t value);


//-----------------------------
// Public definitions
//-----------------------------


void acc_algorithm_roll_and_push(float *data, uint16_t data_length, float element)
{
	for (uint16_t i = 1U; i < data_length; i++)
	{
		data[i - 1U] = data[i];
	}

	data[data_length - 1U] = element;
}


void acc_algorithm_roll_and_push_matrix_f32(float *data, uint16_t rows, uint16_t cols, const float *column, bool pos_shift)
{
	if (pos_shift)
	{
		for (uint16_t r = rows - 1U; r > 0U; r--)
		{
			for (uint16_t c = 0U; c < cols; c++)
			{
				data[(r * cols) + c] = data[((r - 1U) * cols) + c];
			}
		}

		for (uint16_t c = 0U; c < cols; c++)
		{
			data[c] = column[c];
		}
	}
	else
	{
		for (uint16_t r = 1U; r < rows; r++)
		{
			for (uint16_t c = 0U; c < cols; c++)
			{
				data[((r - 1U) * cols) + c] = data[(r * cols) + c];
			}
		}

		for (uint16_t c = 0U; c < cols; c++)
		{
			data[((rows - 1U) * cols) + c] = column[c];
		}
	}
}


void acc_algorithm_roll_and_push_matrix_f32_complex(float complex *data, uint16_t rows, uint16_t cols, const float complex *column,
                                                    bool pos_shift)
{
	if (pos_shift)
	{
		for (uint16_t r = rows - 1U; r > 0U; r--)
		{
			for (uint16_t c = 0U; c < cols; c++)
			{
				data[(r * cols) + c] = data[((r - 1U) * cols) + c];
			}
		}

		for (uint16_t c = 0U; c < cols; c++)
		{
			data[c] = column[c];
		}
	}
	else
	{
		for (uint16_t r = 1U; r < rows; r++)
		{
			for (uint16_t c = 0U; c < cols; c++)
			{
				data[((r - 1U) * cols) + c] = data[(r * cols) + c];
			}
		}

		for (uint16_t c = 0U; c < cols; c++)
		{
			data[((rows - 1U) * cols) + c] = column[c];
		}
	}
}


void acc_algorithm_roll_and_push_mult_matrix_i16_complex(acc_int16_complex_t *data, uint16_t data_rows, uint16_t cols,
                                                         const acc_int16_complex_t *matrix, uint16_t matrix_rows, bool pos_shift)
{
	for (uint16_t m_rows = 0U; m_rows < matrix_rows; m_rows++)
	{
		if (pos_shift)
		{
			for (uint16_t r = data_rows - 1U; r > 0U; r--)
			{
				for (uint16_t c = 0U; c < cols; c++)
				{
					data[(r * cols) + c].real = data[((r - 1U) * cols) + c].real;
					data[(r * cols) + c].imag = data[((r - 1U) * cols) + c].imag;
				}
			}

			for (uint16_t c = 0U; c < cols; c++)
			{
				data[c].real = matrix[(m_rows * cols) + c].real;
				data[c].imag = matrix[(m_rows * cols) + c].imag;
			}
		}
		else
		{
			for (uint16_t r = 1U; r < data_rows; r++)
			{
				for (uint16_t c = 0U; c < cols; c++)
				{
					data[((r - 1U) * cols) + c].real = data[(r * cols) + c].real;
					data[((r - 1U) * cols) + c].imag = data[(r * cols) + c].imag;
				}
			}

			for (uint16_t c = 0U; c < cols; c++)
			{
				data[((data_rows - 1U) * cols) + c].real = matrix[(m_rows * cols) + c].real;
				data[((data_rows - 1U) * cols) + c].imag = matrix[(m_rows * cols) + c].imag;
			}
		}
	}
}


void acc_algorithm_unwrap(float *data, uint16_t data_length)
{
	for (uint16_t i = 1U; i < data_length; i++)
	{
		float diff = data[i] - data[i - 1U];

		while ((diff > (float)M_PI) || (diff < -(float)M_PI))
		{
			if (diff > (float)M_PI)
			{
				data[i] -= 2.0f * (float)M_PI;
			}
			else
			{
				data[i] += 2.0f * (float)M_PI;
			}

			diff = data[i] - data[i - 1U];
		}
	}
}


uint16_t acc_algorithm_argmax(const float *data, uint16_t data_length)
{
	uint16_t idx = 0U;
	float    max = data[idx];

	for (uint16_t i = 1U; i < data_length; i++)
	{
		if (data[i] > max)
		{
			idx = i;
			max = data[i];
		}
	}

	return idx;
}


float acc_algorithm_interpolate_peaks(const float *y, const float *x)
{
	float a = ((x[0] * (y[2] - y[1])) + (x[1] * (y[0] - y[2])) + (x[2] * (y[1] - y[0]))) / (
		(x[0] - x[1]) * (x[0] - x[2]) * (x[1] - x[2]));
	float b = ((y[1] - y[0]) / (x[1] - x[0])) - (a * (x[0] + x[1]));

	return -b / (2.0f * a);
}


float acc_algorithm_interpolate_peaks_equidistant(const float *y, float x_start, float x_delta, uint16_t peak_idx)
{
	float peak_offset = (y[peak_idx - 1U] - y[peak_idx + 1U]) / ((2.0f * y[peak_idx - 1U]) - (4.0f * y[peak_idx]) + (2.0f * y[peak_idx + 1U]));

	return x_start + (((float)peak_idx + peak_offset) * x_delta);
}


void acc_algorithm_butter_lowpass(float freq, float fs, float *b, float *a)
{
	float factor = (2.0f * freq) / fs;

	// Values are centered around 0 to ensure an exactly real pole
	float complex p[2];

	p[0] = -cexpf(((-1.0f * (float)M_PI) / 4.0f) * I);
	p[1] = -cexpf(((1.0f * (float)M_PI) / 4.0f) * I);

	// Pre-wrap frequencies for digital filter design
	factor = 4.0f * tanf(((float)M_PI * factor) / 2.0f);

	// Scale all points radially from origin to shift cutoff frequency
	p[0] = (float complex)factor * p[0];
	p[1] = (float complex)factor * p[1];

	// Cancel out gain change from frequency scaling
	float k = factor * factor;

	// Compensate for gain change
	float complex real_four = (float complex)4.0f;
	float complex z_prod    = 1.0f;
	float complex p_prod    = (real_four - p[0]) * (real_four - p[1]);

	k = k * crealf(acc_algorithm_cdiv(z_prod, p_prod));

	// Bilinear transform the poles and zeros
	p[0] = acc_algorithm_cdiv(real_four + p[0], real_four - p[0]);
	p[1] = acc_algorithm_cdiv(real_four + p[1], real_four - p[1]);

	// Any zeroes that were at infinity get moved to the Nyquist Frequency
	float z_z[2] = {-1.0f, -1.0f};

	// Calculate polynomial transfer functions
	a[0] = -(p[0] + p[1]);
	a[1] = p[0] * p[1];
	b[0] = k;
	b[1] = -k * (z_z[0] + z_z[1]);
	b[2] = k * (z_z[0] * z_z[1]);
}


void acc_algorithm_butter_bandpass(float min_freq, float max_freq, float fs, float *b, float *a)
{
	float min_f = (2.0f * min_freq) / fs;
	float max_f = (2.0f * max_freq) / fs;

	// Values are centered around 0 to ensure an exactly real pole
	float complex p[2];

	p[0] = -cexpf(((-1.0f * (float)M_PI) / 4.0f) * I);
	p[1] = -cexpf(((1.0f * (float)M_PI) / 4.0f) * I);
	float k = 1.0f;

	// Pre-wrap frequencies for digital filter design
	min_f = 4.0f * tanf(((float)M_PI * min_f) / 2.0f);
	max_f = 4.0f * tanf(((float)M_PI * max_f) / 2.0f);

	// Transform lowpass filter prototype to a bandspass filter
	float         bw = max_f - min_f;
	float complex w0 = (float complex)sqrtf(min_f * max_f);

	// Scale poles and zeros to desired bandwidth
	float complex scale = (float complex)(bw / 2.0f);

	p[0] = scale * p[0];
	p[1] = scale * p[1];

	// Duplicate poles and zeros and shift from baseband to +w0 and -w0
	float complex p_bp[4];

	w0     *= w0;
	p_bp[0] = p[0] + csqrtf((p[0] * p[0]) - w0);
	p_bp[1] = p[1] + csqrtf((p[1] * p[1]) - w0);
	p_bp[2] = p[0] - csqrtf((p[0] * p[0]) - w0);
	p_bp[3] = p[1] - csqrtf((p[1] * p[1]) - w0);

	// Cancel out gain change from frequency scaling
	float k_bp = k * bw * bw;

	// Bilinear transform the poles and zeros
	float complex real_four = (float complex)4.0f;
	float complex p_z[4];

	p_z[0] = acc_algorithm_cdiv(real_four + p_bp[0], real_four - p_bp[0]);
	p_z[1] = acc_algorithm_cdiv(real_four + p_bp[1], real_four - p_bp[1]);
	p_z[2] = acc_algorithm_cdiv(real_four + p_bp[2], real_four - p_bp[2]);
	p_z[3] = acc_algorithm_cdiv(real_four + p_bp[3], real_four - p_bp[3]);

	// Any zeroes that were at infinity get moved to the Nyquist Frequency
	float z_z[4] = {1.0f, 1.0f, -1.0f, -1.0f};

	// Compensate for gain change
	float complex z_prod = 16.0f;
	float complex p_prod = (real_four - p_bp[0]) * (real_four - p_bp[1]) * (real_four - p_bp[2]) *
	                       (real_four - p_bp[3]);
	float k_z = k_bp * crealf(acc_algorithm_cdiv(z_prod, p_prod));

	// Calculate polynomial transfer functions
	a[0] = -(p_z[0] + p_z[1] + p_z[2] + p_z[3]);
	a[1] = (p_z[0] * p_z[1]) + (p_z[0] * p_z[2]) + (p_z[0] * p_z[3]) + (p_z[1] * p_z[2]) + (p_z[1] * p_z[3]) + (p_z[2] * p_z[3]);
	a[2] = -((p_z[0] * p_z[1] * p_z[2]) + (p_z[0] * p_z[1] * p_z[3]) + (p_z[0] * p_z[2] * p_z[3]) + (p_z[1] * p_z[2] * p_z[3]));
	a[3] = p_z[0] * p_z[1] * p_z[2] * p_z[3];
	b[0] = k_z;
	b[1] = -k_z * (z_z[0] + z_z[1] + z_z[2] + z_z[3]);
	b[2] = k_z * ((z_z[0] * z_z[1]) + (z_z[0] * z_z[2]) + (z_z[0] * z_z[3]) + (z_z[1] * z_z[2]) + (z_z[1] * z_z[3]) + (z_z[2] * z_z[3]));
	b[3] = -k_z * ((z_z[0] * z_z[1] * z_z[2]) + (z_z[0] * z_z[1] * z_z[3]) + (z_z[0] * z_z[2] * z_z[3]) + (z_z[1] * z_z[2] * z_z[3]));
	b[4] = k_z * (z_z[0] * z_z[1] * z_z[2] * z_z[3]);
}


void acc_algorithm_lfilter(const float *b, const float *a, float *data, uint16_t data_length)
{
	float filter_states[5] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

	for (uint16_t i = 0U; i < data_length; i++)
	{
		filter_inplace_apply(i, b, a, filter_states, data);
	}
}


void acc_algorithm_lfilter_matrix(const float *b, const float *a, float *data, uint16_t rows, uint16_t cols)
{
	for (uint16_t i = 0U; i < rows; i++)
	{
		acc_algorithm_lfilter(b, a, &data[i * cols], cols);
	}
}


void acc_algorithm_apply_filter_f32(const float *a, const float *filt_data, uint16_t filt_rows, uint16_t filt_cols, const float *b,
                                    const float *data, uint16_t data_rows, uint16_t data_cols, float *output, uint16_t output_length)
{
	for (uint16_t i = 0U; i < output_length; i++)
	{
		output[i] = 0.0f;

		for (uint16_t r = 0U; r < filt_rows; r++)
		{
			output[i] -= a[r] * filt_data[i + (r * filt_cols)];
		}

		for (uint16_t r = 0U; r < data_rows; r++)
		{
			output[i] += b[r] * data[i + (r * data_cols)];
		}
	}
}


void acc_algorithm_apply_filter_f32_complex(const float *a, const float complex *filt_data, uint16_t filt_rows, uint16_t filt_cols,
                                            const float *b, const float complex *data, uint16_t data_rows, uint16_t data_cols,
                                            float complex *output, uint16_t output_length)
{
	for (uint16_t i = 0U; i < output_length; i++)
	{
		float real = 0.0f;
		float imag = 0.0f;

		for (uint16_t r = 0U; r < filt_rows; r++)
		{
			real -= a[r] * crealf(filt_data[i + (r * filt_cols)]);
			imag -= a[r] * cimagf(filt_data[i + (r * filt_cols)]);
		}

		for (uint16_t r = 0U; r < data_rows; r++)
		{
			real += b[r] * crealf(data[i + (r * data_cols)]);
			imag += b[r] * cimagf(data[i + (r * data_cols)]);
		}

		output[i] = real + (imag * I);
	}
}


void acc_algorithm_mean_sweep(const acc_int16_complex_t *frame, uint16_t num_points, uint16_t sweeps_per_frame, uint16_t start_point,
                              uint16_t end_point, float complex *sweep)
{
	for (uint16_t n = start_point; n < end_point; n++)
	{
		float real = 0.0f;
		float imag = 0.0f;
		for (uint16_t s = 0U; s < sweeps_per_frame; s++)
		{
			acc_int16_complex_t sample = frame[n + (s * num_points)];
			real += (float)sample.real;
			imag += (float)sample.imag;
		}

		real /= (float)sweeps_per_frame;
		imag /= (float)sweeps_per_frame;

		sweep[n - start_point] = real + (imag * I);
	}
}


void acc_algorithm_mean_i16_complex(const acc_int16_complex_t *data, uint16_t data_length, float complex *out)
{
	mean_i16_complex(data, data_length, out, 1U);
}


void acc_algorithm_mean_matrix_i16_complex(const acc_int16_complex_t *matrix, uint16_t rows, uint16_t cols, float complex *out, uint16_t axis)
{
	if (axis == 1U)
	{
		for (uint16_t i = 0U; i < rows; i++)
		{
			mean_i16_complex(&matrix[i * cols], cols, &out[i], 1U);
		}
	}
	else if (axis == 0U)
	{
		for (uint16_t i = 0U; i < cols; i++)
		{
			mean_i16_complex(&matrix[i], rows, &out[i], cols);
		}
	}
	else
	{
		// Do nothing
	}
}


void acc_algorithm_conj_f32(float complex *data, uint16_t data_length)
{
	for (uint16_t i = 0U; i < data_length; i++)
	{
		data[i] = conjf(data[i]);
	}
}


void acc_algorithm_normalize_f32_complex(float complex *data, uint16_t data_length)
{
	for (uint16_t i = 0U; i < data_length; i++)
	{
		data[i] = data[i] / cabsf(data[i]);
	}
}


void acc_algorithm_rfft(const float *data, uint16_t data_length, uint16_t length_shift, float complex *output)
{
	rfft(data, data_length, length_shift, output, 1U);
}


void acc_algorithm_rfft_matrix(const float *data, uint16_t rows, uint16_t cols, uint16_t length_shift, float complex *output, uint16_t axis)
{
	uint16_t full_cols = ((uint16_t)1U) << length_shift;

	if (axis == 1U)
	{
		uint16_t output_cols = (full_cols / 2U) + 1U;
		for (uint16_t i = 0U; i < rows; i++)
		{
			rfft(&data[i * cols], cols, length_shift, &output[i * output_cols], 1U);
		}
	}
	else if (axis == 0U)
	{
		for (uint16_t i = 0U; i < cols; i++)
		{
			rfft(&data[i], rows, length_shift, &output[i], cols);
		}
	}
	else
	{
		// Do nothing
	}
}


void acc_algorithm_fft(const float complex *data, uint16_t data_length, uint16_t length_shift, float complex *output)
{
	small_fft(data, data_length, length_shift, output, 1U);
}


void acc_algorithm_fft_matrix(const float complex *data, uint16_t rows, uint16_t cols, uint16_t length_shift, float complex *output, uint16_t axis)
{
	uint16_t full_cols = ((uint16_t)1U) << length_shift;

	if (axis == 1U)
	{
		uint16_t output_cols = full_cols;
		for (uint16_t i = 0U; i < rows; i++)
		{
			small_fft(&data[i * cols], cols, length_shift, &output[i * output_cols], 1U);
		}
	}
	else if (axis == 0U)
	{
		for (uint16_t i = 0U; i < cols; i++)
		{
			small_fft(&data[i], rows, length_shift, &output[i], cols);
		}
	}
	else
	{
		// Do nothing
	}
}


float acc_algorithm_fftfreq_delta(uint16_t n, float d)
{
	float df = NAN;

	if ((n > 0U) && (d > 0.0f))
	{
		df = 1.0f / ((float)n * d);
	}

	return df;
}


void acc_algorithm_rfftfreq(uint16_t n, float d, float *freqs)
{
	uint16_t n_freqs = (n / 2U) + 1U;
	float    df      = acc_algorithm_fftfreq_delta(n, d);

	for (uint16_t i = 0U; i < n_freqs; i++)
	{
		freqs[i] = (float)i * df;
	}
}


void acc_algorithm_fftfreq(uint16_t n, float d, float *freqs)
{
	float    df  = acc_algorithm_fftfreq_delta(n, d);
	uint16_t mid = n / 2U;

	for (uint16_t i = 0U; i < mid; i++)
	{
		freqs[i] = (float)i * df;
	}

	int32_t n_int32 = (int32_t)n;

	for (int32_t i = (int32_t)mid; i < n_int32; i++)
	{
		int32_t diff = i - n_int32;
		freqs[i] = (float)diff * df;
	}
}


float acc_algorithm_exp_smoothing_coefficient(float fs, float tc)
{
	float res = NAN;

	if ((fs != 0.0f) && (tc != 0.0f))
	{
		float dt = 1.0f / fs;

		res = expf(-dt / tc);
	}

	return res;
}


float complex acc_algorithm_cdiv(float complex num, float complex denom)
{
	double a = (double)crealf(denom);
	double b = (double)cimagf(denom);
	double c = (double)crealf(num);
	double d = (double)cimagf(num);

	float real = (float)(((c * a) + (b * d)) / ((a * a) + (b * b)));
	float imag = (float)(((a * d) - (c * b)) / ((a * a) + (b * b)));

	return real + (imag * I);
}


void acc_algorithm_hamming(uint16_t n, float *window)
{
	const float a      = 0.54f;
	const float b      = 0.46f;
	const float factor = (2.0f * (float)M_PI) / ((float)n - 1.0f);

	for (uint16_t i = 0U; i < n; i++)
	{
		window[i] = a - (b * cosf((float)i * factor));
	}
}


void acc_algorithm_hann(uint16_t n, float *window)
{
	const float a = 0.5f;

	const float factor = (2.0f * (float)M_PI) / (float)n;

	for (uint16_t i = 0U; i < n; i++)
	{
		window[i] = a - (a * cosf((float)i * factor));
	}
}


float acc_algorithm_get_fwhm(acc_config_profile_t profile)
{
	float fwhm;

	switch (profile)
	{
		case ACC_CONFIG_PROFILE_1:
			fwhm = 0.04f;
			break;
		case ACC_CONFIG_PROFILE_2:
			fwhm = 0.07f;
			break;
		case ACC_CONFIG_PROFILE_3:
			fwhm = 0.14f;
			break;
		case ACC_CONFIG_PROFILE_4:
			fwhm = 0.19f;
			break;
		case ACC_CONFIG_PROFILE_5:
			fwhm = 0.32f;
			break;
		default:
			fwhm = 0.0f;
			break;
	}

	return fwhm;
}


void acc_algorithm_double_buffering_frame_filter(acc_int16_complex_t *frame,
                                                 const uint16_t      sweeps_per_frame,
                                                 const uint16_t      num_points,
                                                 int32_t             *work_buffer)
{
	int32_t first_diff_r[2U];
	int32_t first_diff_i[2U];

	if (sweeps_per_frame >= 32U)
	{
		for (uint16_t point = 0U; point < num_points; point++)
		{
			int32_t abs_mad_sum = 0;

			for (uint16_t sweep = 0U; sweep < (sweeps_per_frame - 2U); sweep++)
			{
				/* Calculate 1st discrete difference */
				for (uint16_t idx = 0U; idx < 2U; idx++)
				{
					uint16_t sweep_idx      = (sweep + idx) * num_points;
					uint16_t next_sweep_idx = (sweep + idx + 1U) * num_points;
					int32_t  diff1_r        = frame[(next_sweep_idx + point)].real;
					int32_t  diff1_i        = frame[(next_sweep_idx + point)].imag;
					int32_t  diff2_r        = frame[(sweep_idx + point)].real;
					int32_t  diff2_i        = frame[(sweep_idx + point)].imag;
					first_diff_r[idx] = diff1_r - diff2_r;
					first_diff_i[idx] = diff1_i - diff2_i;
				}

				/* Calculate 2nd discrete difference */
				int32_t second_diff_r = first_diff_r[1] - first_diff_r[0U];
				int32_t second_diff_i = first_diff_i[1] - first_diff_i[0U];

				/* Estimating magnitude using: abs(real) + abs(imag) */
				int32_t abs_r = (second_diff_r < 0) ? -second_diff_r : second_diff_r;
				int32_t abs_i = (second_diff_i < 0) ? -second_diff_i : second_diff_i;

				work_buffer[sweep] = abs_r + abs_i;

				/* Sum mean absolute deviation */
				abs_mad_sum += work_buffer[sweep];
			}

			/* Mean absolute deviation */
			int32_t nof_of_abs = (int32_t)sweeps_per_frame;
			nof_of_abs = nof_of_abs - 2;
			int32_t diff_mad  = abs_mad_sum / nof_of_abs;
			int32_t threshold = DOUBLE_BUFFERING_MEAN_ABS_DEV_OUTLIER_TH * diff_mad;

			for (uint16_t sweep = 1U; sweep < (sweeps_per_frame - 1U); sweep++)
			{
				if (work_buffer[sweep - 1U] <= threshold)
				{
					continue;
				}

				if (sweep == 1U)
				{
					/* First Sweep */
					double_buffering_median_filter(frame, num_points, 1U, point, 0U);
				}
				else if (sweep == (sweeps_per_frame - 2U))
				{
					/* Last Sweep */
					double_buffering_median_filter(frame, num_points, sweeps_per_frame - 2U, point, sweeps_per_frame - 4U - 1U);
				}
				else
				{
					double_buffering_interpolate(frame, sweeps_per_frame, num_points, sweep, point);
				}
			}
		}
	}
}


void acc_algorithm_fftshift_matrix(float *data, uint16_t rows, uint16_t cols)
{
	for (uint16_t i = 0U; i < cols; i++)
	{
		fftshift(&(data[i]), rows, cols);
	}
}


void acc_algorithm_fftshift(float *data, uint16_t data_length)
{
	fftshift(data, data_length, 1U);
}


void acc_algorithm_welch_matrix(const float complex *data,
                                uint16_t            rows,
                                uint16_t            cols,
                                uint16_t            segment_length,
                                float complex       *data_buffer,
                                float complex       *fft_out,
                                float               *psds,
                                const float         *window,
                                uint16_t            length_shift,
                                float               fs)
{
	for (uint16_t i = 0U; i < cols; i++)
	{
		welch(&(data[i]), rows, segment_length, data_buffer, fft_out, &(psds[i]), window, length_shift, fs,
		      cols);
	}
}


void acc_algorithm_welch(const float complex *data,
                         uint16_t            data_length,
                         uint16_t            segment_length,
                         float complex       *data_buffer,
                         float complex       *fft_out,
                         float               *psd,
                         const float         *window,
                         uint16_t            length_shift,
                         float               fs)
{
	welch(data, data_length, segment_length, data_buffer, fft_out, psd, window, length_shift, fs, 1U);
}


float acc_algorithm_calculate_cfar(const float *data,
                                   uint16_t    data_length,
                                   uint16_t    window_length,
                                   uint16_t    half_guard_length,
                                   float       sensitivity,
                                   uint16_t    idx)
{
	const uint16_t start_idx = window_length + half_guard_length;
	const uint16_t end_idx   = data_length - start_idx;
	float          threshold = 0.0f;

	if ((idx < start_idx) || (idx >= end_idx))
	{
		threshold = FLT_MAX;
	}
	else
	{
		float    samples_sum             = 0.0f;
		uint16_t sample_count            = 0U;
		uint16_t take_window_close_start = idx - half_guard_length - window_length;
		uint16_t take_window_far_start   = idx + half_guard_length + 1U;

		for (uint16_t k = 0; k < window_length; k++)
		{
			samples_sum  += data[take_window_close_start + k];
			samples_sum  += data[take_window_far_start + k];
			sample_count += 2U;
		}

		threshold  = (sample_count > 0U) ? (samples_sum / (float)sample_count) : 0.0f;
		threshold += sensitivity;
	}

	return threshold;
}


float acc_algorithm_calculate_mirrored_one_sided_cfar(const float *data,
                                                      uint16_t    data_length,
                                                      uint16_t    middle_idx,
                                                      uint16_t    window_length,
                                                      uint16_t    half_guard_length,
                                                      float       sensitivity,
                                                      uint16_t    idx)
{
	uint16_t margin                        = window_length + half_guard_length;
	uint16_t half_sweep_len_without_margin = (uint16_t)rint(((double)data_length / 2.0) - (double)margin);

	float min = INFINITY;

	for (uint16_t i = 0U; i < data_length; i++)
	{
		min = fminf(data[i], min);
	}

	float sum = 0.0f;

	if (idx <= margin)
	{
		for (uint16_t j = 0U; j < window_length; j++)
		{
			sum += data[j];
		}
	}

	if ((idx > margin) && (idx < middle_idx))
	{
		for (uint16_t j = 0U; j < window_length; j++)
		{
			sum += data[j + (idx - margin)];
		}
	}

	if ((idx >= middle_idx) && (idx < (data_length - margin - 1U)))
	{
		for (uint16_t j = 0U; j < window_length; j++)
		{
			sum += data[data_length - half_sweep_len_without_margin - j + idx - middle_idx];
		}
	}

	if (idx >= (data_length - margin - 1U))
	{
		for (uint16_t j = 0U; j < window_length; j++)
		{
			sum += data[data_length - j - 1U];
		}
	}

	return ((sum / (float)window_length) + min) / sensitivity;
}


uint16_t acc_algorithm_get_distance_idx(const float *data, uint16_t cols, uint16_t rows, uint16_t middle_idx, uint16_t half_slow_zone)
{
	float max = -INFINITY;

	uint16_t idx = 0U;

	for (uint16_t i = 0U; i < rows; i++)
	{
		if ((i < (middle_idx + half_slow_zone)) && (i >= (middle_idx - half_slow_zone)))
		{
			continue;
		}

		for (uint16_t j = 0U; j < cols; j++)
		{
			if (data[(i * cols) + j] > max)
			{
				max = data[(i * cols) + j];
				idx = j;
			}
		}
	}

	return idx;
}


float acc_algorithm_get_peak_velocity(const float    *velocities,
                                      const float    *energies,
                                      const uint16_t *peak_idxs,
                                      uint16_t       num_peaks,
                                      float          limit)
{
	float slow_vs              = 0.0f;
	float valid_vs             = 0.0f;
	bool  has_valid            = false;
	float biggest_energy_slow  = -INFINITY;
	float biggest_energy_valid = -INFINITY;

	for (uint16_t i = 0U; i < num_peaks; i++)
	{
		uint16_t idx = (peak_idxs != NULL) ? peak_idxs[i] : i;

		if (energies[idx] > biggest_energy_slow)
		{
			if (fabsf(velocities[idx]) < limit)
			{
				slow_vs             = velocities[idx];
				biggest_energy_slow = energies[idx];
			}
		}

		if (energies[i] > biggest_energy_valid)
		{
			if (fabsf(velocities[idx]) >= limit)
			{
				valid_vs             = velocities[idx];
				biggest_energy_valid = energies[idx];
				has_valid            = true;
			}
		}
	}

	return has_valid ? valid_vs : slow_vs;
}


bool acc_algorithm_merge_peaks(float          max_peak_separation,
                               const float    *velocities,
                               const float    *energies,
                               const uint16_t *peak_idxs,
                               uint16_t       num_peaks,
                               float          *merged_velocities,
                               float          *merged_energies,
                               uint16_t       merged_peaks_length,
                               uint16_t       *num_merged_peaks)
{
	bool     status            = true;
	uint16_t cluster_count     = 0U;
	uint16_t cluster_start_idx = 0U;

	if (num_peaks > 1U)
	{
		for (uint16_t i = 0U; i < (num_peaks - 1U); i++)
		{
			uint16_t current_idx = peak_idxs[i];
			uint16_t next_idx    = peak_idxs[i + 1U];

			uint16_t num_peaks_in_cluster = i - cluster_start_idx + 1U;

			bool current_peak_is_in_cluster = (velocities[next_idx] - velocities[current_idx]) < max_peak_separation;

			if (current_peak_is_in_cluster)
			{
				continue;
			}

			status = cluster_count < merged_peaks_length;

			if (status)
			{
				merge_peak_cluster(cluster_start_idx, num_peaks_in_cluster, velocities, energies, peak_idxs,
				                   merged_velocities, merged_energies, cluster_count);
			}

			if (!status)
			{
				break;
			}

			cluster_count++;
			cluster_start_idx = i + 1U;
		}
	}

	bool last_cluster_not_merged = cluster_start_idx < num_peaks;

	if (status && last_cluster_not_merged)
	{
		status = cluster_count < merged_peaks_length;

		if (status)
		{
			merge_peak_cluster(cluster_start_idx, num_peaks - cluster_start_idx, velocities,
			                   energies, peak_idxs,
			                   merged_velocities, merged_energies, cluster_count);
		}

		if (status)
		{
			cluster_count++;
		}
	}

	if (status)
	{
		*num_merged_peaks = cluster_count;
	}

	return status;
}


float acc_algorithm_get_distance_m(uint16_t step_length, uint16_t start_point, float base_step_length_m, uint16_t idx)
{
	uint16_t steps = (idx * step_length) + start_point;

	return (float)steps * base_step_length_m;
}


acc_config_profile_t acc_algorithm_select_profile(int32_t start_point, float base_step_length)
{
	acc_config_profile_t profile = ACC_CONFIG_PROFILE_1;

	// Array with minimum start point for each profile without interference from direct leakage
	// Profile 1 has special case -1 to default to this if no other work
	float MIN_DIST_M[5] = {-1.0f, 0.07f * 2.0f, 0.14f * 2.0f, 0.19f * 2.0f, 0.32f * 2.0f};

	for (uint16_t i = (uint16_t)ACC_CONFIG_PROFILE_1; i <= (uint16_t)ACC_CONFIG_PROFILE_5; i++)
	{
		if ((MIN_DIST_M[i - 1U] == -1.0f) || (MIN_DIST_M[i - 1U] <= ((float)start_point * base_step_length)))
		{
			profile = get_profile(i);
		}
	}

	return profile;
}


acc_config_prf_t acc_algorithm_select_prf(int16_t breakpoint, acc_config_profile_t profile, float base_step_length)
{
	float            breakpoint_p = (float)breakpoint;
	float            breakpoint_m = breakpoint_p * base_step_length;
	acc_config_prf_t prf;

	if ((breakpoint_m < max_measurable_dist(ACC_CONFIG_PRF_19_5_MHZ))
	    && (profile == ACC_CONFIG_PROFILE_1))
	{
		prf = ACC_CONFIG_PRF_19_5_MHZ;
	}
	else if (breakpoint_m < max_measurable_dist(ACC_CONFIG_PRF_15_6_MHZ))
	{
		prf = ACC_CONFIG_PRF_15_6_MHZ;
	}
	else if (breakpoint_m < max_measurable_dist(ACC_CONFIG_PRF_13_0_MHZ))
	{
		prf = ACC_CONFIG_PRF_13_0_MHZ;
	}
	else if (breakpoint_m < max_measurable_dist(ACC_CONFIG_PRF_8_7_MHZ))
	{
		prf = ACC_CONFIG_PRF_8_7_MHZ;
	}
	else if (breakpoint_m < max_measurable_dist(ACC_CONFIG_PRF_6_5_MHZ))
	{
		prf = ACC_CONFIG_PRF_6_5_MHZ;
	}
	else
	{
		prf = ACC_CONFIG_PRF_5_2_MHZ;
	}

	return prf;
}


bool acc_algorithm_find_peaks(const float *abs_sweep, const uint16_t data_length, const uint32_t *threshold_check, uint16_t *peak_idxs,
                              uint16_t peak_idxs_length, uint16_t *num_peaks)
{
	bool     success     = true;
	uint16_t found_peaks = 0U;
	uint16_t i           = 1U;

	while (i < data_length)
	{
		/*
		 * Find a peak candidate.
		 */

		if (!acc_alg_basic_utils_is_bit_set_bitarray_uint32(threshold_check, ((size_t)i - 1U)))
		{
			i++;
			continue;
		}

		if (!acc_alg_basic_utils_is_bit_set_bitarray_uint32(threshold_check, i))
		{
			i += 2U;
			continue;
		}

		if (abs_sweep[i - 1U] >= abs_sweep[i])
		{
			i++;
			continue;
		}

		/*
		 * Peak candidate found at abs_sweep[d].
		 *
		 * We have two consecutive peaks above threshold: abs_sweep[d-1] and
		 * abs_sweep[d], where abs_sweep[d-1] < abs_sweep[d].
		 *
		 * Now search for an upper bound where abs_sweep[upper] < abs_sweep[d],
		 * but still above threshold.
		 */

		uint16_t d_upper    = i + 1U;
		bool     upper_done = false;

		while (!upper_done)
		{
			if (d_upper >= (data_length - 1U))
			{
				upper_done = true;
			}
			else if (!acc_alg_basic_utils_is_bit_set_bitarray_uint32(threshold_check, d_upper))
			{
				upper_done = true;
			}
			else if (abs_sweep[d_upper] > abs_sweep[i])
			{
				// Growing slope; reset the peak candidate to the new larger value.
				i = d_upper;
				d_upper++;
			}
			else if (abs_sweep[d_upper] < abs_sweep[i])
			{
				/*
				 * Ensure that the value after a peak candidate (i) isn't below threshold; e.g:
				 *
				 * abs_sweep = [1, 2, 3, 4, 2, 2]
				 * threshold = [2, 2, 2, 2, 2, 2]
				 *
				 * Candidate: abs_sweep[i] = 4
				 */
				if (acc_alg_basic_utils_is_bit_set_bitarray_uint32(threshold_check, ((size_t)i + 1U)))
				{
					if (found_peaks < peak_idxs_length)
					{
						peak_idxs[found_peaks] = i;
						found_peaks++;
					}
					else
					{
						success = false;
					}
				}

				upper_done = true;
			}
			else
			{
				d_upper++;
			}
		}

		i = d_upper;
	}

	*num_peaks = found_peaks;

	return success;
}


void acc_algorithm_count_points_above_threshold(const float *matrix, uint16_t rows, uint16_t cols, const float threshold, uint16_t *count,
                                                uint16_t offset, uint16_t threshold_check_length, uint16_t axis)
{
	if (axis == 0U)
	{
		for (uint16_t r = offset; r < (threshold_check_length + offset); r++)
		{
			count[r] = 0U;
			for (uint16_t c = 0U; c < cols; c++)
			{
				if (matrix[c + (r * cols)] > threshold)
				{
					count[r]++;
				}
			}
		}
	}
	else if (axis == 1U)
	{
		for (uint16_t c = offset; c < (threshold_check_length + offset); c++)
		{
			count[c] = 0U;
			for (uint16_t r = 0U; r < rows; r++)
			{
				if (matrix[c + (r * cols)] > threshold)
				{
					count[c]++;
				}
			}
		}
	}
	else
	{
		// Do nothing
	}
}


//-----------------------------
// Private definitions
//-----------------------------


static void mean_i16_complex(const acc_int16_complex_t *data, uint16_t data_length, float complex *out, uint16_t stride)
{
	float real_mean = 0.0f;
	float imag_mean = 0.0f;

	for (uint16_t i = 0U; i < data_length; i++)
	{
		real_mean += (float)data[i * stride].real;
		imag_mean += (float)data[i * stride].imag;
	}

	real_mean /= (float)data_length;
	imag_mean /= (float)data_length;

	*out = real_mean + (imag_mean * I);
}


static void rfft(const float *data, uint16_t data_length, uint16_t length_shift, float complex *output, uint16_t stride)
{
	small_rfft(data, data_length, length_shift - 1U, output, stride);

	small_rfft_real_symmetry_conversion(output, length_shift - 1U, stride);

	output[(((uint16_t)1U) << (length_shift - 1U)) * stride] = cimagf(output[0]);
	output[0]                                                = crealf(output[0]);
}


static void fftshift(float *data, uint16_t data_length, uint16_t stride)
{
	uint16_t half_data_length = (data_length + 1U) / 2U;

	for (uint16_t i = 0U; i < half_data_length; i++)
	{
		float x = data[0];

		for (uint16_t j = 0U; j < (data_length - 1U); j++)
		{
			data[j * stride] = data[(j * stride) + stride];
		}

		data[(data_length * stride) - stride] = x;
	}
}


static void welch(const float complex *data,
                  uint16_t            data_length,
                  uint16_t            segment_length,
                  float complex       *data_buffer,
                  float complex       *fft_out,
                  float               *psd,
                  const float         *window,
                  uint16_t            length_shift,
                  float               fs,
                  uint16_t            stride)
{
	uint16_t num_segments = data_length / segment_length;
	float    scale        = 0.0f;

	for (uint16_t i = 0U; i < num_segments; i++)
	{
		scale = 0.0f;
		float complex mean = 0.0f;

		for (uint16_t j = 0U; j < segment_length; j++)
		{
			mean += data[(i * segment_length * stride) + (j * stride)];
		}

		float complex adj_mean_real = crealf(mean) / (float)segment_length;
		float complex adj_mean_imag = cimagf(mean) / (float)segment_length;
		mean = adj_mean_real + (adj_mean_imag * I);

		for (uint16_t j = 0U; j < segment_length; j++)
		{
			data_buffer[j] = data[(i * segment_length * stride) + (j * stride)] - mean;

			float complex adj_buf_real = crealf(data_buffer[j]) * window[j];
			float complex adj_buf_imag = cimagf(data_buffer[j]) * window[j];
			data_buffer[j] = adj_buf_real + (adj_buf_imag * I);

			scale += window[j] * window[j];
		}

		acc_algorithm_fft(data_buffer, segment_length, length_shift, fft_out);

		for (uint16_t j = 0U; j < segment_length; j++)
		{
			psd[j * stride] += cabsf(fft_out[j]) *
			                   cabsf(fft_out[j]);
		}
	}

	if (scale != 0.0f)
	{
		scale = 1.0f / (scale * fs * (float)num_segments);
	}

	for (uint16_t i = 0U; i < segment_length; i++)
	{
		psd[i * stride] *= scale;
	}
}


static void filter_inplace_apply(uint16_t    sample_idx,
                                 const float *b,
                                 const float *a,
                                 float       state[5],
                                 float       *data)
{
	float x = data[sample_idx];
	float y;

	y = state[0] + (b[0] * x);

	state[0] = state[1] + (b[1] * x) - (a[0] * y);
	state[1] = state[2] + (b[2] * x) - (a[1] * y);
	state[2] = state[3] + (b[3] * x) - (a[2] * y);
	state[3] = (b[4] * x) - (a[3] * y);

	data[sample_idx] = y;
}


static float complex get_data_padded_f32_to_f32_complex(const float *data, uint16_t data_length, uint16_t index, uint16_t stride)
{
	float    real = 0.0f;
	float    imag = 0.0f;
	uint16_t i    = index * 2U;

	if (i < data_length)
	{
		real = data[i * stride];
	}

	i++;

	if (i < data_length)
	{
		imag = data[i * stride];
	}

	return real + (imag * I);
}


static float complex get_data_padded_f32_complex(const float complex *data, uint16_t data_length, uint16_t index, uint16_t stride)
{
	float complex res = 0.0f + 0.0f * I;

	if (index < data_length)
	{
		res = data[index * stride];
	}

	return res;
}


static void small_rfft(const float *data, uint16_t data_length, uint16_t length_shift, float complex *output, uint16_t stride)
{
	uint16_t full_data_length = ((uint16_t)1U) << length_shift;

	if (length_shift == 0U)
	{
		// Trivial 1-element FFT
		output[0] = get_data_padded_f32_to_f32_complex(data, data_length, 0U, stride);
	}
	else if (length_shift == 1U)
	{
		// 2-element FFT
		output[0] = get_data_padded_f32_to_f32_complex(data, data_length, 0U, stride) + get_data_padded_f32_to_f32_complex(data,
		                                                                                                                   data_length, 1U,
		                                                                                                                   stride);
		output[1U * stride] = get_data_padded_f32_to_f32_complex(data, data_length, 0U, stride) - get_data_padded_f32_to_f32_complex(data,
		                                                                                                                             data_length, 1U,
		                                                                                                                             stride);
	}
	else
	{
		// Perform element reordering
		uint16_t reverse_i = 0U;
		for (uint16_t i = 0U; i < full_data_length; i++)
		{
			if (i < reverse_i)
			{
				float complex tmp = get_data_padded_f32_to_f32_complex(data, data_length, i, stride);
				output[i*stride]         = get_data_padded_f32_to_f32_complex(data, data_length, reverse_i, stride);
				output[reverse_i*stride] = tmp;
			}
			else if (i == reverse_i)
			{
				output[i*stride] = get_data_padded_f32_to_f32_complex(data, data_length, i, stride);
			}
			else
			{
				// Do nothing
			}

			uint16_t bit = full_data_length >> 1U;
			while ((bit & reverse_i) != 0U)
			{
				reverse_i &= ~bit;
				bit      >>= 1U;
			}
			reverse_i |= bit;
		}

		small_fft_transformation(length_shift, output, stride);
	}
}


static void small_fft(const float complex *data, uint16_t data_length, uint16_t length_shift, float complex *output, uint16_t stride)
{
	uint16_t full_data_length = ((uint16_t)1U) << length_shift;

	if (length_shift == 0U)
	{
		// Trivial 1-element FFT
		output[0] = get_data_padded_f32_complex(data, data_length, 0U, stride);
	}
	else if (length_shift == 1U)
	{
		// 2-element FFT
		output[0] = get_data_padded_f32_complex(data, data_length, 0U, stride) + get_data_padded_f32_complex(data, data_length, 1U,
		                                                                                                     stride);
		output[1U * stride] = get_data_padded_f32_complex(data, data_length, 0U, stride) - get_data_padded_f32_complex(data, data_length, 1U,
		                                                                                                               stride);
	}
	else
	{
		// Perform element reordering
		uint16_t reverse_i = 0U;
		for (uint16_t i = 0U; i < full_data_length; i++)
		{
			if (i < reverse_i)
			{
				float complex tmp = get_data_padded_f32_complex(data, data_length, i, stride);
				output[i*stride]         = get_data_padded_f32_complex(data, data_length, reverse_i, stride);
				output[reverse_i*stride] = tmp;
			}
			else if (i == reverse_i)
			{
				output[i*stride] = get_data_padded_f32_complex(data, data_length, i, stride);
			}
			else
			{
				// Do nothing
			}

			uint16_t bit = full_data_length >> 1U;
			while ((bit & reverse_i) != 0U)
			{
				reverse_i &= ~bit;
				bit      >>= 1U;
			}
			reverse_i |= bit;
		}

		small_fft_transformation(length_shift, output, stride);
	}
}


static void small_fft_transformation(uint16_t length_shift, float complex *output, uint16_t stride)
{
	uint16_t full_data_length = ((uint16_t)1U) << length_shift;

	// Perform 4-element base transformations
	for (uint16_t i = 0U; i < full_data_length; i += 4U)
	{
		float complex s0 = output[i * stride] + output[(i + 1U) * stride];
		float complex d0 = output[i * stride] - output[(i + 1U) * stride];
		float complex s1 = output[(i + 2U) * stride] + output[(i + 3U) * stride];
		float complex d1 = output[(i + 2U) * stride] - output[(i + 3U) * stride];

		d1 = cimagf(d1) - (I * crealf(d1));          // d1 = -I*d1;

		output[(i + 0U) * stride] = s0 + s1;
		output[(i + 2U) * stride] = s0 - s1;
		output[(i + 1U) * stride] = d0 + d1;
		output[(i + 3U) * stride] = d0 - d1;
	}

	// Main part of the FFT computation
	uint16_t      block_length = 4U;
	float complex phase_incr   = -I;

	while (block_length < full_data_length)
	{
		// Update the phase_incr unit vector to have half phase angle
		phase_incr = (phase_incr + 1.0f) / cabsf(phase_incr + 1.0f);

		float complex phase = 1.0f;
		for (uint16_t m = 0U; m < block_length; m++)
		{
			for (uint16_t i = m; i < full_data_length; i += block_length << 1U)
			{
				float complex delta = output[(i + block_length) * stride] * phase;

				output[(i + block_length) * stride] = output[i*stride] - delta;

				output[i*stride] += delta;
			}

			// This phase increment is the leading error source for large transforms
			phase = phase * phase_incr;
		}

		block_length <<= 1U;
	}
}


static void small_rfft_real_symmetry_conversion(float complex *output, uint16_t length_shift, uint16_t stride)
{
	uint16_t full_data_length = ((uint16_t)1U) << length_shift;

	output[0] = (1.0f + (1.0f * I)) * conjf(output[0]);

	if (length_shift > 0U)
	{
		float complex phase_incr = I;
		float complex z1_factor  = 0.5f * phase_incr;

		for (uint16_t i = 1U; i < length_shift; i++)
		{
			phase_incr = (phase_incr + 1.0f) / cabsf(phase_incr + 1.0f);
		}

		uint16_t mid = full_data_length / 2U;
		for (uint16_t i = 1U; i < mid; i++)
		{
			float complex t;
			float complex z0 = output[i * stride];
			float complex z1 = output[(full_data_length - i) * stride];

			t  = z0 + conjf(z1);
			z1 = conjf(z0) - z1;
			z0 = t;

			z0       *= 0.5f;
			z1_factor = z1_factor * phase_incr;
			z1        = z1 * z1_factor;

			t  = z0 + conjf(z1);
			z1 = conjf(z0) - z1;
			z0 = t;

			output[i * stride]                      = z0;
			output[(full_data_length - i) * stride] = z1;
		}

		output[mid * stride] = conjf(output[mid * stride]);
	}
}


static void double_buffering_median_filter(acc_int16_complex_t *frame,
                                           const uint16_t      num_points,
                                           const uint16_t      sweep,
                                           const uint16_t      point,
                                           const uint16_t      median_start_sweep)
{
	/* Get the complex median value over an array of length 4 */
	int32_t point_r[4U];
	int32_t point_i[4U];
	int32_t point_abs[4U];

	/* Calculate abs value */
	for (uint16_t idx = 0U; idx < 4U; idx++)
	{
		point_r[idx]   = frame[((median_start_sweep + idx)*num_points) + point].real;
		point_i[idx]   = frame[((median_start_sweep + idx)*num_points) + point].imag;
		point_abs[idx] = (point_r[idx]*point_r[idx]) + (point_i[idx]*point_i[idx]);
	}

	uint16_t high_index = 0U;
	uint16_t low_index  = 0U;
	int32_t  high_val   = INT32_MIN;
	int32_t  low_val    = INT32_MAX;

	/* Find highest/lowest abs index */
	for (uint16_t idx = 0; idx < 4U; idx++)
	{
		if (point_abs[idx] > high_val)
		{
			high_val   = point_abs[idx];
			high_index = idx;
		}

		if (point_abs[idx] < low_val)
		{
			low_val   = point_abs[idx];
			low_index = idx;
		}
	}

	/* Clear highest and lowest */
	point_r[high_index] = 0;
	point_i[high_index] = 0;
	point_r[low_index]  = 0;
	point_i[low_index]  = 0;

	int32_t median_real = 0;
	int32_t median_imag = 0;

	/* Sum complex points */
	for (uint16_t idx = 0U; idx < 4U; idx++)
	{
		median_real += point_r[idx];
		median_imag += point_i[idx];
	}

	/* Update frame with median filtered value */
	median_real = median_real / 2;
	median_imag = median_imag / 2;

	frame[(sweep*num_points) + point].real = (int16_t)median_real;
	frame[(sweep*num_points) + point].imag = (int16_t)median_imag;
}


static void double_buffering_interpolate(acc_int16_complex_t *frame,
                                         const uint16_t      sweeps_per_frame,
                                         const uint16_t      num_points,
                                         const uint16_t      sweep,
                                         const uint16_t      point)
{
	/* 2/3 of the sweep value before */
	int32_t interpolate_real_i32 = frame[((sweep - 1U)*num_points) + point].real;
	int32_t interpolate_imag_i32 = frame[((sweep - 1U)*num_points) + point].imag;

	interpolate_real_i32 = interpolate_real_i32 * 2;
	interpolate_imag_i32 = interpolate_imag_i32 * 2;

	/* 1/3 of the sweep value two positions ahead */
	uint16_t sweep_idx = sweep + 2U;

	if (sweep_idx > (sweeps_per_frame - 1U))
	{
		sweep_idx = sweeps_per_frame - 1U;
	}

	interpolate_real_i32 += frame[((sweep_idx)*num_points) + point].real;
	interpolate_imag_i32 += frame[((sweep_idx)*num_points) + point].imag;

	interpolate_real_i32 = interpolate_real_i32 / 3;
	interpolate_imag_i32 = interpolate_imag_i32 / 3;

	/* Update frame with interpolated value */
	frame[(sweep*num_points) + point].real = (int16_t)interpolate_real_i32;
	frame[(sweep*num_points) + point].imag = (int16_t)interpolate_imag_i32;
}


static void merge_peak_cluster(uint16_t       start_idx,
                               uint16_t       num_peaks,
                               const float    *velocities,
                               const float    *energies,
                               const uint16_t *peak_idxs,
                               float          *merged_velocities,
                               float          *merged_energies,
                               uint16_t       cluster_count)
{
	float min = INFINITY;
	float max = -INFINITY;

	for (uint16_t i = 0U; i < num_peaks; i++)
	{
		merged_velocities[cluster_count] +=
			velocities[peak_idxs[start_idx + i]];
		merged_energies[cluster_count] +=
			energies[peak_idxs[start_idx + i]];

		min = fminf(
			velocities[peak_idxs[start_idx + i]], min);

		max = fmaxf(velocities[peak_idxs[start_idx + i]], max);
	}

	merged_velocities[cluster_count] /= (float)num_peaks;
	merged_energies[cluster_count]   /= (float)num_peaks;
}


static float max_measurable_dist(acc_config_prf_t prf)
{
	float mmd;

	switch (prf)
	{
		case ACC_CONFIG_PRF_19_5_MHZ:
			mmd = 3.1f;
			break;
		case ACC_CONFIG_PRF_15_6_MHZ:
			mmd = 5.1f;
			break;
		case ACC_CONFIG_PRF_13_0_MHZ:
			mmd = 7.0f;
			break;
		case ACC_CONFIG_PRF_8_7_MHZ:
			mmd = 12.7f;
			break;
		case ACC_CONFIG_PRF_6_5_MHZ:
			mmd = 18.5f;
			break;
		case ACC_CONFIG_PRF_5_2_MHZ:
			mmd = 24.2f;
			break;
		default:
			mmd = 0.0f;
			break;
	}

	return mmd;
}


static acc_config_profile_t get_profile(uint16_t value)
{
	acc_config_profile_t profile = ACC_CONFIG_PROFILE_3;

	switch (value)
	{
		case 1U:
			profile = ACC_CONFIG_PROFILE_1;
			break;

		case 2U:
			profile = ACC_CONFIG_PROFILE_2;
			break;

		case 4U:
			profile = ACC_CONFIG_PROFILE_4;
			break;

		case 5U:
			profile = ACC_CONFIG_PROFILE_5;
			break;

		default:
			break;
	}

	return profile;
}
