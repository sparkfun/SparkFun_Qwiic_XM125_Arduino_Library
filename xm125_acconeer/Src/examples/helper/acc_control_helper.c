// Copyright (c) Acconeer AB, 2022
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "acc_control_helper.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration.h"
#include "acc_rss_a121.h"


#define SENSOR_CALIBRATION_TIMEOUT_MS 500
#define SENSOR_MEASURE_TIMEOUT_MS     1000


static bool acc_control_helper_calibrate(acc_control_helper_t *radar)
{
	bool cal_status;
	bool cal_complete = false;

	do
	{
		cal_status = acc_sensor_calibrate(radar->sensor, &cal_complete, &radar->cal_result, radar->buffer, radar->buffer_size);

		if (cal_status && !cal_complete)
		{
			cal_status = acc_hal_integration_wait_for_sensor_interrupt(radar->sensor_id, SENSOR_CALIBRATION_TIMEOUT_MS);
		}
	} while (cal_status && !cal_complete);

	return cal_status;
}


bool acc_control_helper_create(acc_control_helper_t *radar, acc_sensor_id_t sensor_id)
{
	memset(radar, 0, sizeof(acc_control_helper_t));
	radar->sensor_id = sensor_id;
	radar->config    = acc_config_create();
	return radar->config != NULL ? true : false;
}


void acc_control_helper_destroy(acc_control_helper_t *radar)
{
	if (radar->sensor != NULL)
	{
		acc_sensor_destroy(radar->sensor);
		radar->sensor = NULL;
	}

	if (radar->buffer != NULL)
	{
		acc_integration_mem_free(radar->buffer);
		radar->buffer = NULL;
	}

	if (radar->config != NULL)
	{
		acc_config_destroy(radar->config);
		radar->config = NULL;
	}

	if (radar->processing != NULL)
	{
		acc_processing_destroy(radar->processing);
		radar->processing = NULL;
	}

	acc_hal_integration_sensor_disable(radar->sensor_id);
	acc_hal_integration_sensor_supply_off(radar->sensor_id);
}


bool acc_control_helper_activate(acc_control_helper_t *radar)
{
	radar->processing = acc_processing_create(radar->config, &radar->proc_meta);
	if (radar->processing == NULL)
	{
		fprintf(stderr, "Failed to create processing\n");
		goto fail;
	}

	if (!acc_rss_get_buffer_size(radar->config, &radar->buffer_size))
	{
		fprintf(stderr, "acc_rss_get_buffer_size() failed\n");
		goto fail;
	}

	radar->buffer = acc_integration_mem_alloc(radar->buffer_size);
	if (radar->buffer == NULL)
	{
		fprintf(stderr, "acc_rss_get_buffer_size() failed\n");
		goto fail;
	}

	acc_hal_integration_sensor_supply_on(radar->sensor_id);
	acc_hal_integration_sensor_enable(radar->sensor_id);

	radar->sensor = acc_sensor_create(radar->sensor_id);
	if (radar->sensor == NULL)
	{
		fprintf(stderr, "Failed to create sensor\n");
		goto fail;
	}

	if (!acc_control_helper_calibrate(radar))
	{
		fprintf(stderr, "calibration failed\n");
		goto fail;
	}

	acc_hal_integration_sensor_disable(radar->sensor_id);
	acc_hal_integration_sensor_enable(radar->sensor_id);

	if (!acc_sensor_prepare(radar->sensor, radar->config, &radar->cal_result, radar->buffer, radar->buffer_size))
	{
		fprintf(stderr, "acc_sensor_prepare failed\n");
		acc_sensor_status(radar->sensor);
		goto fail;
	}

	return true;

fail:
	acc_hal_integration_sensor_disable(radar->sensor_id);
	acc_hal_integration_sensor_supply_off(radar->sensor_id);

	if (radar->sensor != NULL)
	{
		acc_sensor_destroy(radar->sensor);
		radar->sensor = NULL;
	}

	if (radar->buffer != NULL)
	{
		acc_integration_mem_free(radar->buffer);
		radar->buffer = NULL;
	}

	if (radar->processing != NULL)
	{
		acc_processing_destroy(radar->processing);
		radar->processing = NULL;
	}

	return false;
}


bool acc_control_helper_get_next(acc_control_helper_t *radar)
{
	if (!acc_sensor_measure(radar->sensor))
	{
		fprintf(stderr, "acc_sensor_measure failed\n");
		goto fail;
	}

	if (!acc_hal_integration_wait_for_sensor_interrupt(radar->sensor_id, SENSOR_MEASURE_TIMEOUT_MS))
	{
		fprintf(stderr, "Sensor interrupt timeout\n");
		goto fail;
	}

	if (!acc_sensor_read(radar->sensor, radar->buffer, radar->buffer_size))
	{
		fprintf(stderr, "acc_sensor_read failed\n");
		goto fail;
	}

	acc_processing_execute(radar->processing, radar->buffer, &radar->proc_result);

	return true;

fail:
	acc_sensor_status(radar->sensor);
	return false;
}
