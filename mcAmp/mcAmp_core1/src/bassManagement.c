/*
 * bassManagement.c
 *
 *  Created on: 6 Jul 2024
 *      Author: George Howell
 */

#include "bassManagement.h"
#include "code_lib_c.h"

#include "drivers/bm_event_logging_driver/bm_event_logging.h"

#include "tymphany_ta6fd00_sealed_box_bassExtParams.h"

#define STATE_MEMORY_SIZE           (192U)

#define N_CHANNELS					(2U)

// handle
bassManagement_f32_handle_t bassManagementH;

// state memory
char bassManagementStateMem[STATE_MEMORY_SIZE] = {0};

void bassManagementInit(void)
{
	bassManagement_f32_stateParams_t stateParams = {N_CHANNELS, N_STAGES_XO_LOW, N_STAGES_XO_HIGH};
	bassManagement_f32_params_t params = {{sosXoLow[0]},
										  {sosXoHigh[0]},
										  {RMS_ATTACK_COEFF, RMS_GAIN, RMS_GRAD, RMS_Y_INT, SMOOTH_ATTACK_COEFF,
										   SMOOTH_RELEASE_COEFF, EXT_POLE_LOW_REAL, EXT_POLE_LOW_IMAG, EXT_POLE_HIGH_REAL,
										   EXT_POLE_HIGH_IMAG, K_LOW, K_LOW_INV, FS, sosExtHigh[0], sosInd[0]}};

	size_t pStateMemReq;

	if (bassManagement_f32_getStateMemSize(&stateParams, &pStateMemReq) != ERR_STATUS_OK)
	{
		log_event(EVENT_FATAL, "Bass Management: error getting state memory size");
	}
	char msg[128];
	sprintf(msg, "Bass Management: state memory size required is %d bytes", (uint32_t)pStateMemReq);
	log_event(EVENT_INFO, msg);

	if (bassManagement_f32_init(&bassManagementH, &params, &stateParams, bassManagementStateMem, STATE_MEMORY_SIZE) != ERR_STATUS_OK)
	{
		log_event(EVENT_FATAL, "Bass Management: error initialising");
	}
	log_event(EVENT_INFO, "Bass Management: initialised");
}
