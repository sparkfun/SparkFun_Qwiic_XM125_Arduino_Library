// Copyright (c) Acconeer AB, 2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#ifndef REF_APP_BREATHING_H_
#define REF_APP_BREATHING_H_


#include <stdbool.h>
#include <stdint.h>

#include "acc_detector_presence.h"


/**
 * @brief Breathing application context handle
 */
struct ref_app_breathing_handle;

typedef struct ref_app_breathing_handle ref_app_breathing_handle_t;


/**
 * @brief Breathing application config container
 */
typedef struct
{
	/**
	 * Length of time series
	 */
	uint16_t time_series_length_s;
	/**
	 * Lowest anticipated breathing rate (breaths per minute)
	 */
	uint16_t lowest_breathing_rate;
	/**
	 * Highest anticipated breathing rate (breaths per minute)
	 */
	uint16_t highest_breathing_rate;
	/**
	 * Number of distance to analyze in breathing
	 */
	uint16_t num_dists_to_analyze;
	/**
	 * Use presence detector to determine distance to motion
	 */
	bool use_presence_processor;
	/**
	 * Time to determine distance to presence
	 */
	uint16_t distance_determination_duration_s;
	/**
	 * Presence config
	 */
	acc_detector_presence_config_t *presence_config;
} ref_app_breathing_config_t;


/**
 * @brief State of the application
 */
typedef enum
{
	REF_APP_BREATHING_APP_STATE_INIT,
	REF_APP_BREATHING_APP_STATE_NO_PRESENCE,
	REF_APP_BREATHING_APP_STATE_INTRA_PRESENCE,
	REF_APP_BREATHING_APP_STATE_DETERMINE_DISTANCE,
	REF_APP_BREATHING_APP_STATE_ESTIMATE_BREATHING_RATE,
} ref_app_breathing_app_state_t;


/**
 * @brief Breathing application results container
 */
typedef struct
{
	/**
	 * Indication when a new breathing rate result is produced
	 */
	bool result_ready;
	/**
	 * Breathing rate in BPM
	 */
	float breathing_rate;
	/**
	 * State of the application
	 */
	ref_app_breathing_app_state_t app_state;
	/**
	 * Result of the presence detector
	 */
	acc_detector_presence_result_t presence_result;
} ref_app_breathing_result_t;


/**
 * @brief Create a configuration for the ref app breathing
 *
 * @return A configuration for the ref app breathing, NULL if creation was not possible
 */
ref_app_breathing_config_t *ref_app_breathing_config_create(void);


/**
 * @brief Destory a configuration for the ref app breathing
 *
 * @param[in] config The configuration to destroy
 */
void ref_app_breathing_config_destroy(ref_app_breathing_config_t *config);


/**
 * @brief Create a handle for the ref app breathing
 *
 * @param[in] config The configuration to create the handle with
 * @return A handle for the ref app breathing, NULL if creation was not possible
 */
ref_app_breathing_handle_t *ref_app_breathing_create(ref_app_breathing_config_t *config);


/**
 * @brief Destroy a handle for the ref app breathing
 *
 * @param[in] handle The ref app breathing handle to destroy
 */
void ref_app_breathing_destroy(ref_app_breathing_handle_t *handle);


/**
 * @brief Get the buffer size needed for the provided ref app breathing handle
 *
 * @param[in] handle The ref app breathing handle to to get the buffer size for
 * @param[out] buffer_size The buffer size
 * @return true if successful, false otherwise
 */
bool ref_app_breathing_get_buffer_size(ref_app_breathing_handle_t *handle, uint32_t *buffer_size);


/**
 * @brief Prepare the application to do a measurement
 *
 * @param[in] handle The ref app breathing handle to prepare for
 * @param[in] config The configuration to prepare with
 * @param[in] sensor The sensor instance to prepare
 * @param[in] sensor_cal_result The calibration result to prepare with
 * @param[in] buffer Memory used by the application to prepare the sensor for measurements
 *            The buffer will only be used during the duration of this call
 * @param[in] buffer_size The size in bytes of the buffer, should be at least buffer_size
 *            from @ref ref_app_breathing_get_buffer_size
 * @return true if successful, false otherwise
 */
bool ref_app_breathing_prepare(ref_app_breathing_handle_t *handle, ref_app_breathing_config_t *config, acc_sensor_t *sensor,
                               const acc_cal_result_t *sensor_cal_result, void *buffer, uint32_t buffer_size);


/**
 * @brief Process the data
 *
 * @param[in] handle The ref app breathing handle for to get the next result for
 * @param[in] buffer  A reference to the buffer (populated by @ref acc_sensor_read) containing the
 *                    data to be processed.
 * @param[out] result Ref app breathing results
 * @return true if successful, otherwise false
 */
bool ref_app_breathing_process(ref_app_breathing_handle_t *handle, void *buffer, ref_app_breathing_result_t *result);


#endif
