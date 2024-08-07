/*
 * paramReader.c
 *
 *  Created on: 2 Jul 2024
 *      Author: George Howell
 */

#include "paramReader.h"
#include "code_lib_c.h"

#include "drivers/bm_event_logging_driver/bm_event_logging.h"
#include "common/multicore_shared_memory.h"

paramReader_handle_t paramReaderH;
BM_UART device;
char stateMem[STATE_MEMORY_SIZE] = {0};

error_status_t uartInit(void);
error_status_t uartSetCallback(void);
error_status_t uartReadByte(void);
void uartCallback(void);
error_status_t setParams(void);

error_status_t uartInit(void)
{
	uart_initialize(&device, UART_BAUD_RATE_115200, UART_SERIAL_8N1, UART1);

    return ERR_STATUS_OK;
}

error_status_t uartSetCallback(void)
{
    uart_set_rx_callback(&device, uartCallback);

    return ERR_STATUS_OK;
}

error_status_t uartReadByte(void)
{
    uart_read_byte(&device, (uint8_t*)paramReaderH.states.pParamBuffer);

    return ERR_STATUS_OK;
}

void uartCallback(void)
{
	if (!multicore_data->paramUpdateFlag)
	{
		paramReader_callback(&paramReaderH);
	}
}

extern volatile MULTICORE_DATA *multicore_data;

error_status_t setParams(void)
{
    char msg[128], *pName;
    paramReader_parameters_t *pParamState = paramReaderH.states.pParamsState;

    for (int i = 0; i < paramReaderH.nParams; i++)
    {
    	pName = pParamState[i].name;

    	if (strcmp(pName, "switchPos") == 0)
    	{
    		multicore_data->switchPosition = pParamState[i].value;
    		log_event(EVENT_INFO, "Switch: position parameter update");
    	}
    	else if (strcmp(pName, "RmsAttackTime") == 0)
    	{
    		multicore_data->bassManagementParams.bassExt.rmsAttackTime = pParamState[i].value;
    		log_event(EVENT_INFO, "Bass Extension: rms attack coefficient parameter update");
    	}
    	else
    	{
			sprintf(msg, "ParamReader: %s is not a valid param", pName);
			log_event(EVENT_WARN, msg);
			break;
    	}
    }

    sprintf(msg, "nParams = %d", paramReaderH.nParams);
	log_event(EVENT_INFO, msg);

	multicore_data->paramUpdateFlag = 1U;

    return ERR_STATUS_OK;
}

void paramReaderInit(void)
{
    char msg[128];
    size_t pStateMemReq;

    paramReader_stateParams_t stateParams = {N_PARAMS_MAX, PARAM_LEN_MAX};
    paramReader_params_t params = {uartInit, uartSetCallback, uartReadByte, setParams};

    multicore_data->paramUpdateFlag = 0U;

    log_event(EVENT_INFO, "Param Reader");

    if (paramReader_getStateMemSize(&stateParams, &pStateMemReq) != ERR_STATUS_OK)
	{
		log_event(EVENT_FATAL, "ParamReader Error: getting state memory");
	}
	sprintf(msg, "\t State memory size = %zu bytes", pStateMemReq);
	log_event(EVENT_INFO, msg);

	if (paramReader_init(&paramReaderH, &params, &stateParams, stateMem, STATE_MEMORY_SIZE) != ERR_STATUS_OK)
	{
		log_event(EVENT_FATAL, "ParamReader Error: initialising");
	}

	log_event(EVENT_INFO, "\t initialised successfully");
}
