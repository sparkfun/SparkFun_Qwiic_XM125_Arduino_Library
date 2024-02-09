// Copyright (c) Acconeer AB, 2023
// All rights reserved

#ifndef ACC_ALGORITHM_H_
#define ACC_ALGORITHM_H_

#include <complex.h>
#include <stdint.h>

#include "acc_config.h"
#include "acc_definitions_a121.h"
#include "acc_definitions_common.h"


/**
 * @brief Roll array elements and push new element last
 *
 * @param[in, out] data Array to be rolled
 * @param[in] data_length Length of the array
 * @param[in] element The new element
 */
void acc_algorithm_roll_and_push(float *data, uint16_t data_length, float element);


/**
 * @brief Roll row elements and push a new column
 *
 * @param[in, out] data Matrix to be rolled of size rows*cols
 * @param[in] rows Number of rows in the matrix
 * @param[in] cols Number of cols in the matrix
 * @param[in] column The new column
 * @param[in] pos_shift If true will be the same as shift=1 in np.roll, otherwise the same as shift=-1
 */
void acc_algorithm_roll_and_push_matrix_f32(float *data, uint16_t rows, uint16_t cols, const float *column, bool pos_shift);


/**
 * @brief Roll row elements and push a new column
 *
 * @param[in, out] data Matrix to be rolled of size rows*cols
 * @param[in] rows Number of rows in the matrix
 * @param[in] cols Number of cols in the matrix
 * @param[in] column The new column
 * @param[in] pos_shift If true will be the same as shift=1 in np.roll, otherwise the same as shift=-1
 */
void acc_algorithm_roll_and_push_matrix_f32_complex(float complex *data, uint16_t rows, uint16_t cols, const float complex *column,
                                                    bool pos_shift);


/**
 * @brief Roll row elements and push multiple columns
 *
 * @param[in, out] data Matrix to be rolled
 * @param[in] data_rows Number of rows in data matrix
 * @param[in] cols Number of cols in data matrix
 * @param[in] matrix Matrix with new columns
 * @param[in] matrix_rows Number of rows in the matrix
 * @param[in] pos_shift If true will be the same as shift=1 in np.roll, otherwise the same as shift=-1
 */
void acc_algorithm_roll_and_push_mult_matrix_i16_complex(acc_int16_complex_t *data, uint16_t data_rows, uint16_t cols,
                                                         const acc_int16_complex_t *matrix, uint16_t matrix_rows, bool pos_shift);


/**
 * @brief Unwraps a signal by changing elements which have an absolute difference from
 *        their predecessor of more than 2*pi to their period-complementary values.
 *
 * @param[in, out] data Array with signal data
 * @param[in] data_length Length of the array
 */
void acc_algorithm_unwrap(float *data, uint16_t data_length);


/**
 * @brief Find index of largest element in the array
 *
 * @param[in] data Array of data
 * @param[in] data_length Length of the array
 * @return Index of largest element
 */
uint16_t acc_algorithm_argmax(const float *data, uint16_t data_length);


/**
 * @brief Interpolate peak
 *
 * @param[in] y Amplitudes of frequency peaks
 * @param[in] x Frequencies
 * @return Interpolated frequency
 */
float acc_algorithm_interpolate_peaks(const float *y, const float *x);


/**
 * @brief Interpolate equidistant peaks
 *
 * The function fits a second degree polynomial to three consecutive amplitude
 * values where the second element is expected to contain the maximum measured amplitude.
 * The function then finds the position of the maximum amplitude of the polynomial.
 * The position is normalized.
 *
 * @param[in] y Y values to interpolate
 * @param[in] x_start Start value of X-axis
 * @param[in] x_delta Delta between values on X-axis
 * @param[in] peak_idx Idx of peak
 * @return Interpolated X-values
 */
float acc_algorithm_interpolate_peaks_equidistant(const float *y, float x_start, float x_delta, uint16_t peak_idx);


/**
 * @brief Design a 2nd order digital Butterworth lowpass filter
 *
 * @param[in] freq Cutoff freuency
 * @param[in] fs Sampling frequency, > 0 Hz
 * @param[out] b Numerator in polynomial of the IIR filter, length == 3
 * @param[out] a Denominator of polynomial of the IIR filter, length == 2
 */
void acc_algorithm_butter_lowpass(float freq, float fs, float *b, float *a);


/**
 * @brief Design a 2nd order digital Butterworth bandpass filter
 *
 * @param[in] min_freq Low cutoff frequency
 * @param[in] max_freq High cutoff frequency
 * @param[in] fs Sampling frequency, > 0 Hz
 * @param[out] b Numerator in polynomial of the IIR filter, length == 5
 * @param[out] a Denominator of polynomial of the IIR filter, length == 4
 */
void acc_algorithm_butter_bandpass(float min_freq, float max_freq, float fs, float *b, float *a);


/**
 * @brief Filter data with a digital filter
 *
 * @param[in] b Numerator in polynomial of the IIR filter, length == 5
 * @param[in] a Denominator of polynomial of the IIR filter, length == 4
 * @param[in, out] data Data array to filter
 * @param[in] data_length Length of the array
 */
void acc_algorithm_lfilter(const float *b, const float *a, float *data, uint16_t data_length);


/**
 * @brief Filter data along row dimension
 *
 * @param[in] b Numerator in polynomial of the IIR filter, length == 5
 * @param[in] a Denominator of polynomial of the IIR filter, length == 4
 * @param[in, out] data Matrix to filter
 * @param[in] rows Number of rows in the matrix
 * @param[in] cols Number of columns in the matrix
 */
void acc_algorithm_lfilter_matrix(const float *b, const float *a, float *data, uint16_t rows, uint16_t cols);


/**
 * @brief Apply filter coefficients to filtered data matrix and data matrix
 *
 * @param[in] a Denominator of polynomial of the IIR filter
 * @param[in] filt_data Filtered data matrix
 * @param[in] filt_rows Number of rows in filtered data matrix, == len(a)
 * @param[in] filt_cols Number of columns in filtered data matrix, == data_cols
 * @param[in] b Numerator of polynomial of the IIR filter
 * @param[in] data Data matrix
 * @param[in] data_rows Number of rows in data matrix, == len(b)
 * @param[in] data_cols Number of columns in data matrix, == filt_cols
 * @param[out] output Output filtered data array
 * @param[in] output_length Length of output, == data_cols and filt_cols
 */
void acc_algorithm_apply_filter_f32(const float *a, const float *filt_data, uint16_t filt_rows, uint16_t filt_cols, const float *b,
                                    const float *data, uint16_t data_rows, uint16_t data_cols, float *output, uint16_t output_length);


/**
 * @brief Apply filter coefficients to filtered data matrix and data matrix
 *
 * @param[in] a Denominator of polynomial of the IIR filter
 * @param[in] filt_data Filtered data matrix
 * @param[in] filt_rows Number of rows in filtered data matrix, == len(a)
 * @param[in] filt_cols Number of columns in filtered data matrix, == data_cols
 * @param[in] b Numerator of polynomial of the IIR filter
 * @param[in] data Data matrix
 * @param[in] data_rows Number of rows in data matrix, == len(b)
 * @param[in] data_cols Number of columns in data matrix, == filt_cols
 * @param[out] output Output filtered data array
 * @param[in] output_length Length of output, == data_cols and filt_cols
 */
void acc_algorithm_apply_filter_f32_complex(const float *a, const float complex *filt_data, uint16_t filt_rows, uint16_t filt_cols,
                                            const float *b, const float complex *data, uint16_t data_rows, uint16_t data_cols,
                                            float complex *output, uint16_t output_length);


/**
 * @brief Calculate mean sweep of a frame from start_point to end_point
 *
 * @param[in] frame Frame to calculate mean sweep for
 * @param[in] num_points Number of points in a sweep
 * @param[in] sweeps_per_frame Number of sweeps in the frame
 * @param[in] start_point Start point of mean sweep, if 0 will be same start point as for sweeps in frame
 * @param[in] end_point End point of mean sweep, if num_points will be same end point as for sweeps in frame
 * @param[out] sweep Mean sweep returned from calculation, length >= (end_point - start_point)
 */
void acc_algorithm_mean_sweep(const acc_int16_complex_t *frame, uint16_t num_points, uint16_t sweeps_per_frame, uint16_t start_point,
                              uint16_t end_point, float complex *sweep);


/**
 * @brief Calculate mean value of an array
 *
 * @param[in] data Data array
 * @param[in] data_length Length of data array
 * @param[out] out Output mean value
 */
void acc_algorithm_mean_i16_complex(const acc_int16_complex_t *data, uint16_t data_length, float complex *out);


/**
 * @brief Calculate mean array of a matrix
 *
 * @param[in] matrix Matrix of data
 * @param[in] rows Number of rows in matrix
 * @param[in] cols Number of columns in matrix
 * @param[out] out Output mean array, length = cols if axis = 0 or length = rows if axis = 1
 * @param[in] axis Axis over which to calculate mean, must be 0 or 1
 */
void acc_algorithm_mean_matrix_i16_complex(const acc_int16_complex_t *matrix, uint16_t rows, uint16_t cols, float complex *out, uint16_t axis);


/**
 * @brief Inline calculate conjugate of all elements in an array
 *
 * @param[in, out] data Data array, will be replaced with conjugate values
 * @param[in] data_length Length of data array
 */
void acc_algorithm_conj_f32(float complex *data, uint16_t data_length);


/**
 * @brief Normalize all elements in an array individually
 *
 * @param[in, out] data Data array, will be replaced by normalized values
 * @param[in] data_length Length of data array
 */
void acc_algorithm_normalize_f32_complex(float complex *data, uint16_t data_length);


/**
 * @brief 1D Fast Fourier Transform for real input
 *
 * @param[in] data Array of data
 * @param[in] data_length Length of data
 * @param[in] length_shift Integer that specifies the transform length N in accordance with N = (1 << length_shift) and N >= data_length
 * @param[out] output Array for output data, length >= (data_length / 2) + 1
 */
void acc_algorithm_rfft(const float *data, uint16_t data_length, uint16_t length_shift, float complex *output);


/**
 * @brief 1D Fast Fourier Transform for real input matrix
 *
 * @param[in] data Matrix of data
 * @param[in] rows Number of rows in the matrix
 * @param[in] cols Number of columns in the matrix
 * @param[in] length_shift Integer that specifies the transform length N in accordance with N = (1 << length_shift) and N >= rows if axis == 0 or N >= cols if axis == 1
 * @param[out] output Matrix for output data, size = ((rows / 2) + 1, cols) if axis=0 and size = (rows, (cols / 2) + 1) if axis=1
 * @param[in] axis Axis over which to calculate the FFT, must be 0 or 1
 */
void acc_algorithm_rfft_matrix(const float *data, uint16_t rows, uint16_t cols, uint16_t length_shift, float complex *output, uint16_t axis);


/**
 * @brief 1D Fast Fourier Transform for complex input
 *
 * @param[in] data Matrix of data
 * @param[in] data_length Length of data
 * @param[in] length_shift Integer that specifies the transform length N in accordance with N = (1 << length_shift) and N >= rows if axis == 0 or N >= cols if axis == 1
 * @param[out] output Array for output data, must be of length N
 */
void acc_algorithm_fft(const float complex *data, uint16_t data_length, uint16_t length_shift, float complex *output);


/**
 * @brief 1D Fast Fourier Transform for input matrix
 *
 * @param[in] data Matrix of data
 * @param[in] rows Number of rows in the matrix
 * @param[in] cols Number of columns in the matrix
 * @param[in] length_shift Integer that specifies the transform length N in accordance with N = (1 << length_shift) and N >= rows if axis == 0 or N >= cols if axis == 1
 * @param[out] output Matrix for output data, size = (N, cols) if axis=0 and size = (rows, N) if axis=1
 * @param[in] axis Axis over which to calculate the FFT, must be 0 or 1
 */
void acc_algorithm_fft_matrix(const float complex *data, uint16_t rows, uint16_t cols, uint16_t length_shift, float complex *output, uint16_t axis);


/** @brief Calculate delta between frequency bins in rfft
 *
 * @param[in] n Window length, > 0
 * @param[in] d Sample spacing, > 0
 * @return Delta between frequency bins
 */
float acc_algorithm_fftfreq_delta(uint16_t n, float d);


/**
 * @brief Calculate the real Fast Fourier Transform sample frequencies
 *
 * @param[in] n Window length, > 0
 * @param[in] d Sample spacing, > 0
 * @param[out] freqs Sample frequencies, length >= (n / 2) + 1
 */
void acc_algorithm_rfftfreq(uint16_t n, float d, float *freqs);


/**
 * @brief Calculate the Fast Fourier Transform sample frequencies
 *
 * @param[in] n Window length, > 0
 * @param[in] d Sample spacing, > 0
 * @param[out] freqs Sample frequencies, length >= n
 */
void acc_algorithm_fftfreq(uint16_t n, float d, float *freqs);


/**
 * @brief Calculate exponential smoothing coefficient
 *
 * @param[in] fs Sampling frequency
 * @param[in] tc Time constant
 *
 * @return Exponential smoothing coefficient, NAN if fs=0 or tc=0
 */
float acc_algorithm_exp_smoothing_coefficient(float fs, float tc);


/**
 * @brief Divide complex number num / denum
 *
 * @param[in] num Numerator
 * @param[in] denom Denominator
 *
 * @return num / denom
 */
float complex acc_algorithm_cdiv(float complex num, float complex denom);


/**
 * @brief Calculate hamming window for a specified number of points
 *
 * @param[in] n Number of points
 * @param[out] window Returned hamming window, length = n
 */
void acc_algorithm_hamming(uint16_t n, float *window);


/**
 * @brief Calculate non-symmetrical hann window for a specified number of points
 *
 * @param[in] n Number of points
 * @param[out] window Returned hann window, length = n
 */
void acc_algorithm_hann(uint16_t n, float *window);


/**
 * @brief Get the envelope Full Width Half Maximum in meters given a profile
 *
 * @param[in] profile The profile to get envelope FWHM for
 * @return The envelope FWHM in meters or 0.0 if profile is not valid.
 */
float acc_algorithm_get_fwhm(acc_config_profile_t profile);


/**
 * @brief Double buffering frame filter
 *
 * Detects and removes outliers in data that appear when the double buffering mode is enabled,
 * and returns the filtered frame.
 *
 * Outliers are detected along the sweep dimension using the second order difference. For
 * reliable outlier detection, the filter is applied only when there are 32 or more sweeps per frame.
 *
 * The disturbance caused by enabling the double buffering mode can appear in multiple sweeps
 * but, according to observations, is limited to a maximum of two consecutive sweeps. Therefore, the
 * function removes outliers by interpolating between the sample before and the sample two positions
 * ahead.
 *
 * The function does not correct disturbances that may appear in the initial or final sweeps.
 *
 * @param[in, out] frame Data frame to where the filter is applied
 * @param[in] sweeps_per_frame How many sweeps there are in the frame
 * @param[in] num_points The number of points in the frame
 * @param[in] work_buffer A work buffer for the filter, length >= (sweeps_per_frame - 2)
 */
void acc_algorithm_double_buffering_frame_filter(acc_int16_complex_t *frame,
                                                 const uint16_t      sweeps_per_frame,
                                                 const uint16_t      num_points,
                                                 int32_t             *work_buffer);


/**
 * @brief Shift the zero-frequency component to the center along row dimensions
 *
 * @param[in, out] data Matrix to be shifted
 * @param[in] rows Number of rows in the matrix
 * @param[in] cols Number of cols in the matrix
 */
void acc_algorithm_fftshift_matrix(float *data, uint16_t rows, uint16_t cols);


/**
 * @brief Shift the zero-frequency component to the center
 *
 * @param[in, out] data Array of data
 * @param[in] data_length Length of data
 */
void acc_algorithm_fftshift(float *data, uint16_t data_length);


/**
 * @brief Estimate power spectral density (PSD) using Welch’s method along row dimensions
 *
 * See @ref acc_algorithm_welch for more details
 *
 * @param[in] data Matrix of data
 * @param[in] rows Number of rows in the matrix
 * @param[in] cols Number of cols in the matrix
 * @param[in] segment_length Length of each segment
 * @param[in] data_buffer Buffer used for calculations, length = segment_length
 * @param[out] fft_out Array for fft output data, length = segment_length
 * @param[out] psds Matrix for output data, size = (cols, segment_length)
 * @param[in] window Desired window to use, length = segment_length
 * @param[in] length_shift Integer that specifies the transform length N in accordance with N = 1 << length_shift and N >= segment_length
 * @param[in] fs Sampling frequency
 */
void acc_algorithm_welch_matrix(const float complex *data,
                                uint16_t            rows,
                                uint16_t            cols,
                                uint16_t            segment_length,
                                float complex       *data_buffer,
                                float complex       *fft_out,
                                float               *psds,
                                const float         *window,
                                uint16_t            length_shift,
                                float               fs);


/**
 * @brief Estimate power spectral density using Welch’s method
 *
 * Computes an estimate of the PSD by dividing the data into non-overlapping segments,
 * computing a periodogram for each segment and averaging the periodograms.
 *
 * @param[in] data Array of data
 * @param[in] data_length Length of data
 * @param[in] segment_length Length of each segment
 * @param[in] data_buffer Buffer used for calculations, length = segment_length
 * @param[out] fft_out Array for fft output data, length = segment_length
 * @param[out] psd Array for output data, length = segment_length
 * @param[in] window Desired window to use, length = segment_length
 * @param[in] length_shift Integer that specifies the transform length N in accordance with N = 1 << length_shift and N >= segment_length
 * @param[in] fs Sampling frequency
 */
void acc_algorithm_welch(const float complex *data,
                         uint16_t            data_length,
                         uint16_t            segment_length,
                         float complex       *data_buffer,
                         float complex       *fft_out,
                         float               *psd,
                         const float         *window,
                         uint16_t            length_shift,
                         float               fs);


/**
 * @brief Calculate CFAR threshold
 *
 * @param[in] data Array of data
 * @param[in] data_length Length of the data array
 * @param[in] window_length Number of frequency bins next to the CFAR guard from which the threshold level will be calculated
 * @param[in] half_guard_length Number of frequency bins around the point of interest that is omitted when calculating the CFAR threshold
 * @param[in] sensitivity Sensitivity of the CFAR threshold
 * @param[in] idx Index to calculate cfar for
 * @return Threshold value at index
 */
float acc_algorithm_calculate_cfar(const float *data,
                                   uint16_t    data_length,
                                   uint16_t    window_length,
                                   uint16_t    half_guard_length,
                                   float       sensitivity,
                                   uint16_t    idx);


/**
 * @brief Calculate mirrored one sided CFAR threshold
 *
 * Calculate a one sided CFAR. Guard and window are mirrored on the right half of
 * the data. The threshold is extended to also include the CFAR margin. The
 * margin is of size half_guard_length + window_length.
 *
 * On the left half this construction is used
 * | Window | Half guard | t(i) |
 *
 * and on the right half this contruction is used
 * | t(i) | Half guard | Window |
 *
 * The threshold extension looks like this:
 * threshold = [t(m), t(m) ... t(m) t(m+1) t(m+2) ... t(n-m-2) t(n-m-1) t(n-m) ... t(n-m) t(n-m)]
 * where n = data_length and m = window_length + half_guard_length
 *
 * @param[in] data Array of data
 * @param[in] data_length Length of the data array
 * @param[in] middle_idx Middle index
 * @param[in] window_length Number of frequency bins next to the CFAR guard from which the threshold level will be calculated
 * @param[in] half_guard_length Number of frequency bins around the point of interest that is omitted when calculating the CFAR threshold
 * @param[in] sensitivity Sensitivity of the CFAR threshold
 * @param[in] idx Index to calculate cfar for
 * @return Threshold value at index
 */
float acc_algorithm_calculate_mirrored_one_sided_cfar(const float *data,
                                                      uint16_t    data_length,
                                                      uint16_t    middle_idx,
                                                      uint16_t    window_length,
                                                      uint16_t    half_guard_length,
                                                      float       sensitivity,
                                                      uint16_t    idx);


/**
 * @brief Find the index of the distance column containing the largest amplitude, disregarding amplitudes present in the slow zone
 *
 * @param[in] data Matrix of data
 * @param[in] cols Number of cols
 * @param[in] rows Number of rows
 * @param[in] middle_idx Middle index
 * @param[in] half_slow_zone Half size of the number of frequency bins that are regarded as the slow zone
 * @return Distance index
 */
uint16_t acc_algorithm_get_distance_idx(const float *data, uint16_t cols, uint16_t rows, uint16_t middle_idx, uint16_t half_slow_zone);


/**
 * @brief Find the velocity of the peak with the largest amplitude, prioritizing peaks with a velocity over the slow zone limit
 *
 * @param[in] velocities Array of velocities
 * @param[in] energies Array of energies
 * @param[in] peak_idxs Indices of identified peaks
 * @param[in] num_peaks Number of identified peaks
 * @param[in] limit Slow zone limit
 * @return Peak velocity
 */
float acc_algorithm_get_peak_velocity(const float    *velocities,
                                      const float    *energies,
                                      const uint16_t *peak_idxs,
                                      uint16_t       num_peaks,
                                      float          limit);


/**
 * @brief Merges peaks
 *
 * @param[in] max_peak_separation The greatest distance (in meters) between peaks that will result in a merge
 * @param[in] velocities The velocities to merge
 * @param[in] energies The energies to merge
 * @param[in] peak_idxs Indices of identified peaks
 * @param[in] num_peaks Number of peaks in the peak_idxs array, if 0 nothing will happen
 * @param[out] merged_velocities Output array for the merged velocities
 * @param[out] merged_energies Output array for the merged energies
 * @param[in] merged_peaks_length The length of the merged_velocities and merged_energies arrays
 * @param[out] num_merged_peaks The number of peaks that were merged
 * @return true if successful, false otherwise
 */
bool acc_algorithm_merge_peaks(float          max_peak_separation,
                               const float    *velocities,
                               const float    *energies,
                               const uint16_t *peak_idxs,
                               uint16_t       num_peaks,
                               float          *merged_velocities,
                               float          *merged_energies,
                               uint16_t       merged_peaks_length,
                               uint16_t       *num_merged_peaks);


/**
 * @brief Calculate distance for a point at an index
 *
 * @param[in] step_length Step length in points
 * @param[in] start_point Start point
 * @param[in] base_step_length_m Base step length
 * @param[in] idx Distance index
 * @return Distance at index
 */
float acc_algorithm_get_distance_m(uint16_t step_length, uint16_t start_point, float base_step_length_m, uint16_t idx);


/**
 * @brief Select the highest possible profile without interference of direct leakage
 *
 * @param[in] start_point Start point
 * @param[in] base_step_length base_step_length
 * @return A suitable profile
 */
acc_config_profile_t acc_algorithm_select_profile(int32_t start_point, float base_step_length);


/**
 * @brief Select a suitable PRF given a breakpoint and profile
 *
 * @param[in] breakpoint A base step, relative to start_point = 0
 * @param[in] profile The profile at breakpoint
 * @param[in] base_step_length The base step length
 * @return A suitable PRF
 */
acc_config_prf_t acc_algorithm_select_prf(int16_t breakpoint, acc_config_profile_t profile, float base_step_length);


/**
 * @brief Find peaks above threshold
 *
 * A peak is defined as a point with greater value than its two neighbouring
 * points and all three points are above the threshold.
 *
 * @param[in] abs_sweep Absolute values of the mean sweep
 * @param[in] data_length Number of values in the sweep
 * @param[in] threshold_check Bit array with information if peak is above or below threshold
 * @param[out] peak_idxs Indexes of found peaks
 * @param[in] peak_idxs_length Length of the found peaks array. To fit all possible
 *                             peaks the length must be (abs_sweep_length / 2)
 * @param[out] num_peaks Number of found peaks
 * @return true if all peaks could be found, false otherwise
 */
bool acc_algorithm_find_peaks(const float *abs_sweep, const uint16_t data_length, const uint32_t *threshold_check, uint16_t *peak_idxs,
                              uint16_t peak_idxs_length, uint16_t *num_peaks);


/**
 * @brief Count points in matrix above threshold row-wise or col-wise
 *
 * @param[in] matrix Matrix to check data in
 * @param[in] rows Number of rows in matrix
 * @param[in] cols Number of cols in matrix
 * @param[in] threshold Threshold to check against
 * @param[out] count Number of elements above threshold, length = rows if axis = 0 or length = cols if axis = 1
 * @param[in] offset Offset on the selected axis where to start to check threshold
 * @param[in] threshold_check_length The length of part to check threshold for, row wise if axis = 0 and column wise if axis = 1
 * @param[in] axis Axis over which to check, must be 0 or 1
 */
void acc_algorithm_count_points_above_threshold(const float *matrix, uint16_t rows, uint16_t cols, const float threshold, uint16_t *count,
                                                uint16_t offset, uint16_t threshold_check_length, uint16_t axis);


#endif
