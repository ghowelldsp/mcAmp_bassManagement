/*
 * switch.c
 *
 *  Created on: 6 Jul 2024
 *      Author: George Howell
 */

#include "switch.h"
#include "code_lib_c.h"

#include "drivers/bm_event_logging_driver/bm_event_logging.h"

#define SWTICH_STATE_MEMORY_SIZE      		(10U)

#define SWITCH_FS 							(48000U)
#define SWITCH_N_CHANNELS 					(2U)
#define SWITCH_INIT_POSITION 				(0U)
#define SWITCH_FADE_TIME 					(1.0F)

switch_f32_handle_t switchH;

char stateMem[SWTICH_STATE_MEMORY_SIZE] = {0};

void switchInit(void)
{
	switch_f32_stateParams_t stateParams = {2U};
	switch_f32_params_t params = {SWITCH_INIT_POSITION, SWITCH_FADE_TIME, SWITCH_N_CHANNELS, SWITCH_FS};

	size_t pStateMemReq;

	if (switch_f32_getStateMemSize(&stateParams, &pStateMemReq) != 0)
	{
		log_event(EVENT_FATAL, "Switch: error getting state memory size");
	}
	char msg[128];
	sprintf(msg, "Switch: state memory size required is %d bytes", (uint32_t)pStateMemReq);
	log_event(EVENT_INFO, msg);

	if (switch_f32_init(&switchH, &params, &stateParams, stateMem, SWTICH_STATE_MEMORY_SIZE) != ERR_STATUS_OK)
	{
		log_event(EVENT_FATAL, "Switch: error initialising");
	}
	log_event(EVENT_INFO, "Switch: initialised");
}

