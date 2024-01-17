// Copyright (c) Acconeer AB, 2023
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_definitions_a121.h"
#include "acc_detector_distance.h"
#include "acc_hal_definitions_a121.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration.h"
#include "acc_integration_log.h"
#include "acc_rss_a121.h"
#include "acc_sensor.h"
#include "acc_version.h"


/** \example example_detector_distance_with_iq_data_print.c
 * @brief This is an example on how the Detector Distance API can be used
 * @n
 * This example executes as follows:
 *   - Retrieve HAL integration
 *   - Initialize distance detector resources:
 *     + Create distance detector configuration
 *     + Update configuration settings
 *     + Create Distance detector handle
 *     + Create buffer for detector calibration data
 *     + Create buffer for sensor data
 *   - Create and calibrate the sensor
 *   - Calibrate the detector
 *   - Measure distances with the detector (loop):
 *     + Prepare sensor with the detector
 *     + Measure and wait until a read can be done
 *     + Print IQ data amplitude
 *     + Process measurement and print the result
 *     + Handle "sensor_calibration_needed" indication
 *   - Cleanup:
 *     + Destroy detector configuration
 *     + Destroy detector handle
 *     + Destroy sensor data buffer
 *     + Destroy detector calibration data buffer
 */


typedef enum
{
	DISTANCE_PRESET_CONFIG_NONE = 0,
	DISTANCE_PRESET_CONFIG_BALANCED,
	DISTANCE_PRESET_CONFIG_HIGH_ACCURACY,
} distance_preset_config_t;


#define SENSOR_ID (1U)
// 2 seconds should be enough even for long ranges and high signal quality
#define SENSOR_TIMEOUT_MS (2000U)


typedef struct
{
	acc_sensor_t                   *sensor;
	acc_detector_distance_config_t *config;
	acc_detector_distance_handle_t *handle;
	void                           *buffer;
	uint32_t                       buffer_size;
	void                           *calibration_buffer;
	uint32_t                       calibration_buffer_size;
} distance_detector_resources_t;


static void cleanup(distance_detector_resources_t *resources);


static void set_config(acc_detector_distance_config_t *detector_config, distance_preset_config_t preset);


static bool initialize_detector_resources(distance_detector_resources_t *resources);


static bool do_sensor_calibration(acc_sensor_t     *sensor,
                                  acc_cal_result_t *sensor_cal_result,
                                  void             *buffer,
                                  uint32_t         buffer_size);


static bool do_full_detector_calibration(distance_detector_resources_t *resources,
                                         const acc_cal_result_t        *sensor_cal_result);


static bool do_detector_recalibration(distance_detector_resources_t *resources,
                                      const acc_cal_result_t        *sensor_cal_result);


static bool do_detector_get_next(distance_detector_resources_t  *resources,
                                 const acc_cal_result_t         *sensor_cal_result,
                                 acc_detector_distance_result_t *result);


static void print_distance_result(const acc_detector_distance_result_t *result);


static void print_iq_data(const acc_config_t        *sensor_config,
                          acc_processing_result_t   *processing_result,
                          acc_processing_metadata_t *processing_metadata);


int acconeer_main(int argc, char *argv[]);


int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	distance_detector_resources_t resources = { 0 };

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}

	resources.config = acc_detector_distance_config_create();
	if (resources.config == NULL)
	{
		printf("acc_detector_distance_config_create() failed\n");
		cleanup(&resources);
		return EXIT_FAILURE;
	}

	set_config(resources.config, DISTANCE_PRESET_CONFIG_BALANCED);

	if (!initialize_detector_resources(&resources))
	{
		printf("Initializing detector resources failed\n");
		cleanup(&resources);
		return EXIT_FAILURE;
	}

	// Print the configuration
	acc_detector_distance_config_log(resources.handle, resources.config);

	/* Turn the sensor on */
	acc_hal_integration_sensor_supply_on(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	resources.sensor = acc_sensor_create(SENSOR_ID);
	if (resources.sensor == NULL)
	{
		printf("acc_sensor_create() failed\n");
		cleanup(&resources);
		return EXIT_FAILURE;
	}

	acc_cal_result_t sensor_cal_result;

	if (!do_sensor_calibration(resources.sensor, &sensor_cal_result, resources.buffer, resources.buffer_size))
	{
		printf("Sensor calibration failed\n");
		cleanup(&resources);
		return EXIT_FAILURE;
	}

	if (!do_full_detector_calibration(&resources, &sensor_cal_result))
	{
		printf("Detector calibration failed\n");
		cleanup(&resources);
		return EXIT_FAILURE;
	}

	while (true)
	{
		acc_detector_distance_result_t result = { 0 };

		if (!do_detector_get_next(&resources, &sensor_cal_result, &result))
		{
			printf("Could not get next result\n");
			cleanup(&resources);
			return EXIT_FAILURE;
		}

		print_distance_result(&result);

		/* If "sensor calibration needed" is indicated, the sensor and detector needs to be recalibrated.
		 *
		 * Note that the detector result (printed above) is invalid if 'sensor_calibration_needed' is true.
		 */
		if (result.sensor_calibration_needed)
		{
			printf("Sensor- and detector recalibration needed ... \n");

			if (!do_sensor_calibration(resources.sensor, &sensor_cal_result, resources.buffer, resources.buffer_size))
			{
				printf("Sensor calibration failed\n");
				cleanup(&resources);
				return EXIT_FAILURE;
			}

			/* Once the sensor is recalibrated, the detector should be recalibrated and measuring can continue. */
			if (!do_detector_recalibration(&resources, &sensor_cal_result))
			{
				printf("Detector recalibration failed\n");
				cleanup(&resources);
				return EXIT_FAILURE;
			}

			printf("Sensor- and detector recalibration done!\n");
		}
	}

	cleanup(&resources);

	printf("Done!\n");

	return EXIT_SUCCESS;
}


static void cleanup(distance_detector_resources_t *resources)
{
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_supply_off(SENSOR_ID);

	acc_detector_distance_config_destroy(resources->config);
	acc_detector_distance_destroy(resources->handle);

	acc_integration_mem_free(resources->buffer);
	acc_integration_mem_free(resources->calibration_buffer);

	if (resources->sensor != NULL)
	{
		acc_sensor_destroy(resources->sensor);
	}
}


static void set_config(acc_detector_distance_config_t *detector_config, distance_preset_config_t preset)
{
	// Add configuration of the detector here
	switch (preset)
	{
		case DISTANCE_PRESET_CONFIG_NONE:
			// Add configuration of the detector here
			break;

		case DISTANCE_PRESET_CONFIG_BALANCED:
			acc_detector_distance_config_start_set(detector_config, 0.25f);
			acc_detector_distance_config_end_set(detector_config, 3.0f);
			acc_detector_distance_config_max_step_length_set(detector_config, 0U);
			acc_detector_distance_config_max_profile_set(detector_config, ACC_CONFIG_PROFILE_5);
			acc_detector_distance_config_reflector_shape_set(detector_config, ACC_DETECTOR_DISTANCE_REFLECTOR_SHAPE_GENERIC);
			acc_detector_distance_config_peak_sorting_set(detector_config, ACC_DETECTOR_DISTANCE_PEAK_SORTING_STRONGEST);
			acc_detector_distance_config_threshold_method_set(detector_config, ACC_DETECTOR_DISTANCE_THRESHOLD_METHOD_CFAR);
			acc_detector_distance_config_num_frames_recorded_threshold_set(detector_config, 100U);
			acc_detector_distance_config_threshold_sensitivity_set(detector_config, 0.5f);
			acc_detector_distance_config_signal_quality_set(detector_config, 15.0f);
			acc_detector_distance_config_close_range_leakage_cancellation_set(detector_config, true);
			break;

		case DISTANCE_PRESET_CONFIG_HIGH_ACCURACY:
			acc_detector_distance_config_start_set(detector_config, 0.25f);
			acc_detector_distance_config_end_set(detector_config, 3.0f);
			acc_detector_distance_config_max_step_length_set(detector_config, 2U);
			acc_detector_distance_config_max_profile_set(detector_config, ACC_CONFIG_PROFILE_3);
			acc_detector_distance_config_reflector_shape_set(detector_config, ACC_DETECTOR_DISTANCE_REFLECTOR_SHAPE_GENERIC);
			acc_detector_distance_config_peak_sorting_set(detector_config, ACC_DETECTOR_DISTANCE_PEAK_SORTING_STRONGEST);
			acc_detector_distance_config_threshold_method_set(detector_config, ACC_DETECTOR_DISTANCE_THRESHOLD_METHOD_CFAR);
			acc_detector_distance_config_num_frames_recorded_threshold_set(detector_config, 100U);
			acc_detector_distance_config_threshold_sensitivity_set(detector_config, 0.5f);
			acc_detector_distance_config_signal_quality_set(detector_config, 20.0f);
			acc_detector_distance_config_close_range_leakage_cancellation_set(detector_config, true);
			break;
	}
}


static bool initialize_detector_resources(distance_detector_resources_t *resources)
{
	resources->handle = acc_detector_distance_create(resources->config);
	if (resources->handle == NULL)
	{
		printf("acc_detector_distance_create() failed\n");
		return false;
	}

	if (!acc_detector_distance_get_buffer_sizes(resources->handle, &(resources->buffer_size), &(resources->calibration_buffer_size)))
	{
		printf("acc_detector_distance_get_buffer_sizes() failed\n");
		return false;
	}

	resources->buffer = acc_integration_mem_alloc(resources->buffer_size);
	if (resources->buffer == NULL)
	{
		printf("sensor buffer allocation failed\n");
		return false;
	}

	resources->calibration_buffer = acc_integration_mem_alloc(resources->calibration_buffer_size);
	if (resources->calibration_buffer == NULL)
	{
		printf("calibration buffer allocation failed\n");
		return false;
	}

	return true;
}


static bool do_sensor_calibration(acc_sensor_t     *sensor,
                                  acc_cal_result_t *sensor_cal_result,
                                  void             *buffer,
                                  uint32_t         buffer_size)
{
	bool           status              = false;
	bool           cal_complete        = false;
	const uint16_t calibration_retries = 1U;

	// Random disturbances may cause the calibration to fail. At failure, retry at least once.
	for (uint16_t i = 0; !status && (i <= calibration_retries); i++)
	{
		// Reset sensor before calibration by disabling/enabling it
		acc_hal_integration_sensor_disable(SENSOR_ID);
		acc_hal_integration_sensor_enable(SENSOR_ID);

		do
		{
			status = acc_sensor_calibrate(sensor, &cal_complete, sensor_cal_result, buffer, buffer_size);

			if (status && !cal_complete)
			{
				status = acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS);
			}
		} while (status && !cal_complete);
	}

	if (status)
	{
		/* Reset sensor after calibration by disabling/enabling it */
		acc_hal_integration_sensor_disable(SENSOR_ID);
		acc_hal_integration_sensor_enable(SENSOR_ID);
	}

	return status;
}


static bool do_full_detector_calibration(distance_detector_resources_t *resources,
                                         const acc_cal_result_t        *sensor_cal_result)
{
	bool done = false;
	bool status;

	do
	{
		status = acc_detector_distance_calibrate(resources->sensor,
		                                         resources->handle,
		                                         sensor_cal_result,
		                                         resources->buffer,
		                                         resources->buffer_size,
		                                         resources->calibration_buffer,
		                                         resources->calibration_buffer_size,
		                                         &done);

		if (status && !done)
		{
			status = acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS);
		}
	} while (status && !done);

	return status;
}


static bool do_detector_recalibration(distance_detector_resources_t *resources,
                                      const acc_cal_result_t        *sensor_cal_result)
{
	bool done = false;
	bool status;

	do
	{
		status = acc_detector_distance_recalibrate(resources->sensor,
		                                           resources->handle,
		                                           sensor_cal_result,
		                                           resources->buffer,
		                                           resources->buffer_size,
		                                           resources->calibration_buffer,
		                                           resources->calibration_buffer_size,
		                                           &done);

		if (status && !done)
		{
			status = acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS);
		}
	} while (status && !done);

	return status;
}


static bool do_detector_get_next(distance_detector_resources_t  *resources,
                                 const acc_cal_result_t         *sensor_cal_result,
                                 acc_detector_distance_result_t *result)
{
	bool result_available = false;

	do
	{
		if (!acc_detector_distance_prepare(resources->handle, resources->config, resources->sensor, sensor_cal_result, resources->buffer,
		                                   resources->buffer_size))
		{
			printf("acc_detector_distance_prepare() failed\n");
			return false;
		}

		if (!acc_sensor_measure(resources->sensor))
		{
			printf("acc_sensor_measure() failed\n");
			return false;
		}

		if (!acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS))
		{
			printf("Sensor interrupt timeout\n");
			return false;
		}

		if (!acc_sensor_read(resources->sensor, resources->buffer, resources->buffer_size))
		{
			printf("acc_sensor_read() failed\n");
			return false;
		}

		if (!acc_detector_distance_process(resources->handle, resources->buffer, resources->calibration_buffer, &result_available,
		                                   result))
		{
			printf("acc_detector_distance_process() failed\n");
			return false;
		}

		/**
		 * The function for printing the IQ data must be called every time
		 * the acc_detector_distance_process has been called.
		 */
		print_iq_data(result->sensor_config, result->processing_result, result->processing_metadata);
	} while (!result_available);

	return true;
}


static void print_iq_data(const acc_config_t        *sensor_config,
                          acc_processing_result_t   *processing_result,
                          acc_processing_metadata_t *processing_metadata)
{
	/* Get the iq data frame from the processing result */
	acc_int16_complex_t *frame = processing_result->frame;

	/* Get information about the frame from the sensor config */
	uint16_t sweeps_per_frame = acc_config_sweeps_per_frame_get(sensor_config);
	uint8_t  num_subsweeps    = acc_config_num_subsweeps_get(sensor_config);

	printf("BEGIN:Distance(m),Amplitude\n");

	/* Loop over all subsweeps */
	for (uint8_t subsweep_idx = 0; subsweep_idx < num_subsweeps; subsweep_idx++)
	{
		/* Get information about the start point and step length from the sensor config */
		int32_t  start_point = acc_config_subsweep_start_point_get(sensor_config, subsweep_idx);
		uint16_t step_length = acc_config_subsweep_step_length_get(sensor_config, subsweep_idx);

		/* Get subsweep offset and length from the processing metadata */
		uint16_t subsweep_offset = processing_metadata->subsweep_data_offset[subsweep_idx];
		uint16_t subsweep_length = processing_metadata->subsweep_data_length[subsweep_idx];

		/* Loop over all points in subsweep */
		for (uint16_t point_idx = 0; point_idx < subsweep_length; point_idx++)
		{
			/* Perform a coherent mean calculation for the point over sweeps per frame */
			uint32_t iq_point_real_acc = 0;
			uint32_t iq_point_imag_acc = 0;

			/* Loop over all points in sweeps_per_frame */
			for (uint16_t sweep_idx = 0; sweep_idx < sweeps_per_frame; sweep_idx++)
			{
				uint16_t point_offset = sweep_idx * subsweep_length + subsweep_offset + point_idx;
				iq_point_real_acc += frame[point_offset].real;
				iq_point_imag_acc += frame[point_offset].imag;
			}

			iq_point_real_acc = iq_point_real_acc / sweeps_per_frame;
			iq_point_imag_acc = iq_point_imag_acc / sweeps_per_frame;

			/* Calculate the absolute value of the IQ point */
			uint32_t iq_point_abs = (uint32_t)sqrt(iq_point_real_acc * iq_point_real_acc + iq_point_imag_acc * iq_point_imag_acc);

			/* Print the point distance, in meters, and the point absolute value */
			float distance = acc_processing_points_to_meter(start_point + point_idx*step_length);
			printf("%" PRIfloat ",%" PRIu32 "\n", ACC_LOG_FLOAT_TO_INTEGER(distance), iq_point_abs);
		}
	}

	printf("END:Distance(m),Amplitude\n");
}


static void print_distance_result(const acc_detector_distance_result_t *result)
{
	printf("%d detected distances", result->num_distances);
	if (result->num_distances > 0)
	{
		printf(": ");

		for (uint8_t i = 0; i < result->num_distances; i++)
		{
			printf("%" PRIfloat " m ", ACC_LOG_FLOAT_TO_INTEGER(result->distances[i]));
		}
	}

	printf("\n");
}
