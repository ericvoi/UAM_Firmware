/*
 * comm_txrx_menu.c
 *
 *  Created on: Feb 2, 2025
 *      Author: ericv
 */

/* Private includes ----------------------------------------------------------*/

#include "comm_menu_registration.h"
#include "comm_menu_system.h"
#include "comm_function_loops.h"

#include "cfg_parameters.h"

#include "mess_main.h"
#include "mess_packet.h"

#include "cmsis_os.h"

#include "check_inputs.h"
#include "usb_comm.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* Private typedef -----------------------------------------------------------*/



/* Private define ------------------------------------------------------------*/



/* Private macro -------------------------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/

void transmitBitsOut(void* argument);
void transmitBitsFb(void* argument);
void transmitStringOut(void* argument);
void transmitStringFb(void* argument);
void transmitIntOut(void* argument);
void transmitIntFb(void* argument);
void transmitFloatOut(void* argument);
void transmitFloatFb(void* argument);
void togglePrint(void* argument);

void transmitBits(FunctionContext_t* context, bool is_feedback);
void transmitString(FunctionContext_t* context, bool is_feedback);
void transmitInt(FunctionContext_t* context, bool is_feedback);
void transmitFloat(FunctionContext_t* context, bool is_feedback);

/* Private variables ---------------------------------------------------------*/

static MenuID_t txrxMenuChildren[] = {
  MENU_ID_TXRX_BITSOUT,   MENU_ID_TXRX_BITSFB,    MENU_ID_TXRX_STROUT, 
  MENU_ID_TXRX_STRFB,     MENU_ID_TXRX_INTOUT,    MENU_ID_TXRX_INTFB,
  MENU_ID_TXRX_FLOATOUT,  MENU_ID_TXRX_FLOATFB ,  MENU_ID_TXRX_ENPNT
};
static const MenuNode_t txrxMenu = {
  .id = MENU_ID_TXRX,
  .description = "Transmit and Receive Data Menu",
  .handler = NULL,
  .parent_id = MENU_ID_MAIN,
  .children_ids = txrxMenuChildren,
  .num_children = sizeof(txrxMenuChildren) / sizeof(txrxMenuChildren[0]),
  .access_level = 0,
  .parameters = NULL
};

static ParamContext_t txrxBitsTransducerParam = {
  .state = PARAM_STATE_0,
  .param_id = MENU_ID_TXRX_BITSOUT
};
static const MenuNode_t txrxBitsTransducer = {
  .id = MENU_ID_TXRX_BITSOUT,
  .description = "Bits Through Transducer",
  .handler = transmitBitsOut,
  .parent_id = MENU_ID_TXRX,
  .children_ids = NULL,
  .num_children = 0,
  .access_level = 0,
  .parameters = &txrxBitsTransducerParam
};

static ParamContext_t txrxBitsFeedbackParam = {
  .state = PARAM_STATE_0,
  .param_id = MENU_ID_TXRX_BITSFB
};
static const MenuNode_t txrxBitsFeedback = {
  .id = MENU_ID_TXRX_BITSFB,
  .description = "Bits Through Feedback",
  .handler = transmitBitsFb,
  .parent_id = MENU_ID_TXRX,
  .children_ids = NULL,
  .num_children = 0,
  .access_level = 0,
  .parameters = &txrxBitsFeedbackParam
};

static ParamContext_t txrxStrTransducerParam = {
  .state = PARAM_STATE_0,
  .param_id = MENU_ID_TXRX_STROUT
};
static const MenuNode_t txrxStrTransducer = {
  .id = MENU_ID_TXRX_STROUT,
  .description = "String Through Transducer",
  .handler = transmitStringOut,
  .parent_id = MENU_ID_TXRX,
  .children_ids = NULL,
  .num_children = 0,
  .access_level = 0,
  .parameters = &txrxStrTransducerParam
};

static ParamContext_t txrxStrFeedbackParam = {
  .state = PARAM_STATE_0,
  .param_id = MENU_ID_TXRX_STRFB
};
static const MenuNode_t txrxStrFeedback = {
  .id = MENU_ID_TXRX_STRFB,
  .description = "String Through Feedback",
  .handler = transmitStringFb,
  .parent_id = MENU_ID_TXRX,
  .children_ids = NULL,
  .num_children = 0,
  .access_level = 0,
  .parameters = &txrxStrFeedbackParam
};

static ParamContext_t txrxIntTransducerParam = {
  .state = PARAM_STATE_0,
  .param_id = MENU_ID_TXRX_INTOUT
};
static const MenuNode_t txrxIntTransducer = {
  .id = MENU_ID_TXRX_INTOUT,
  .description = "Integer Through Transducer",
  .handler = transmitIntOut,
  .parent_id = MENU_ID_TXRX,
  .children_ids = NULL,
  .num_children = 0,
  .access_level = 0,
  .parameters = &txrxIntTransducerParam
};

static ParamContext_t txrxIntFeedbackParam = {
  .state = PARAM_STATE_0,
  .param_id = MENU_ID_TXRX_INTFB
};
static const MenuNode_t txrxIntFeedback = {
  .id = MENU_ID_TXRX_INTFB,
  .description = "Integer Through Feedback",
  .handler = transmitIntFb,
  .parent_id = MENU_ID_TXRX,
  .children_ids = NULL,
  .num_children = 0,
  .access_level = 0,
  .parameters = &txrxIntFeedbackParam
};

static ParamContext_t txrxFloatTransducerParam = {
  .state = PARAM_STATE_0,
  .param_id = MENU_ID_TXRX_FLOATOUT
};
static const MenuNode_t txrxFloatTransducer = {
  .id = MENU_ID_TXRX_FLOATOUT,
  .description = "Float Through Transducer",
  .handler = transmitFloatOut,
  .parent_id = MENU_ID_TXRX,
  .children_ids = NULL,
  .num_children = 0,
  .access_level = 0,
  .parameters = &txrxFloatTransducerParam
};

static ParamContext_t txrxFloatFeedbackParam = {
  .state = PARAM_STATE_0,
  .param_id = MENU_ID_TXRX_FLOATFB
};
static const MenuNode_t txrxFloatFeedback = {
  .id = MENU_ID_TXRX_FLOATFB,
  .description = "Float Through Feedback",
  .handler = transmitFloatFb,
  .parent_id = MENU_ID_TXRX,
  .children_ids = NULL,
  .num_children = 0,
  .access_level = 0,
  .parameters = &txrxFloatFeedbackParam
};

static ParamContext_t txrxTogglePrintParam = {
  .state = PARAM_STATE_0,
  .param_id = MENU_ID_TXRX_ENPNT
};
static const MenuNode_t txrxTogglePrint = {
  .id = MENU_ID_TXRX_ENPNT,
  .description = "Enable/Disable Printing Of Received Messages",
  .handler = togglePrint,
  .parent_id = MENU_ID_TXRX,
  .children_ids = NULL,
  .num_children = 0,
  .access_level = 0,
  .parameters = &txrxTogglePrintParam
};


/* Exported function definitions ---------------------------------------------*/

bool COMM_RegisterTxRxMenu()
{
  bool ret = registerMenu(&txrxMenu) && registerMenu(&txrxBitsTransducer) &&
             registerMenu(&txrxStrTransducer) && registerMenu(&txrxIntTransducer) &&
             registerMenu(&txrxFloatTransducer) && registerMenu(&txrxTogglePrint) &&
             registerMenu(&txrxStrFeedback) && registerMenu(&txrxBitsFeedback) &&
             registerMenu(&txrxIntFeedback) && registerMenu(&txrxFloatFeedback);
  return ret;
}

/* Private function definitions ----------------------------------------------*/

void transmitBitsOut(void* argument)
{
  FunctionContext_t* context = (FunctionContext_t*) argument;
  context->state->state = PARAM_STATE_COMPLETE;
}
 
void transmitBitsFb(void* argument) {
  FunctionContext_t* context = (FunctionContext_t*) argument;
  context->state->state = PARAM_STATE_COMPLETE;
}

void transmitStringOut(void* argument)
{
  FunctionContext_t* context = (FunctionContext_t*) argument;
  
  transmitString(context, false);
}

void transmitStringFb(void* argument)
{
  FunctionContext_t* context = (FunctionContext_t*) argument;
  
  transmitString(context, true);
}

void transmitIntOut(void* argument)
{
  FunctionContext_t* context = (FunctionContext_t*) argument;
  
  transmitInt(context, false);
}

void transmitIntFb(void* argument)
{
  FunctionContext_t* context = (FunctionContext_t*) argument;
  
  transmitInt(context, true);
}

void transmitFloatOut(void* argument)
{
  FunctionContext_t* context = (FunctionContext_t*) argument;
  
  transmitFloat(context, false);
}

void transmitFloatFb(void* argument)
{
  FunctionContext_t* context = (FunctionContext_t*) argument;
  
  transmitFloat(context, true);
}

void togglePrint(void* argument)
{
  FunctionContext_t* context = (FunctionContext_t*) argument;

  COMMLoops_LoopToggle(context, PARAM_PRINT_ENABLED);
}


void transmitBits(FunctionContext_t* context, bool is_feedback)
{
  // TODO: implement
}

void transmitString(FunctionContext_t* context, bool is_feedback)
{
  ParamState_t old_state = context->state->state;

  do {
    switch (context->state->state) {
      case PARAM_STATE_0:
        sprintf((char*) context->output_buffer, "\r\n\r\nPlease enter a string to "
            "send to the %s with a maximum length of 128 characters:\r\n", 
            is_feedback ? "feedback network" : "transducer");
        COMM_TransmitData(context->output_buffer, CALC_LEN, context->comm_interface);
        context->state->state = PARAM_STATE_1;
        break;
      case PARAM_STATE_1:
        if (context->input_len > 128) {
          sprintf((char*) context->output_buffer, "\r\nInput string must be"
              "less than 128 characters!\r\n");
          COMM_TransmitData(context->output_buffer, CALC_LEN, context->comm_interface);
          context->state->state = PARAM_STATE_0;
        }
        else {
          Message_t msg;
          msg.type = is_feedback ? MSG_TRANSMIT_FEEDBACK : MSG_TRANSMIT_TRANSDUCER;
          msg.timestamp = osKernelGetTickCount();
          msg.data_type = STRING;
          msg.length_bits = 8 * Packet_MinimumSize(context->input_len);
          for (uint16_t i = 0; i < msg.length_bits / 8; i++) {
            if (context->input_len > i) {
              msg.data[i] = context->input[i];
            }
            else {
              msg.data[i] = context->input[i];
            }
          }
          if (MESS_AddMessageToTxQ(&msg) == pdPASS) {
            sprintf((char*) context->output_buffer, "\r\nSuccessfully added to feedback queue!\r\n\r\n");
            COMM_TransmitData(context->output_buffer, CALC_LEN, context->comm_interface);
          }
          else {
            sprintf((char*) context->output_buffer, "\r\nError adding message to feedback queue\r\n\r\n");
            COMM_TransmitData(context->output_buffer, CALC_LEN, context->comm_interface);
          }
          context->state->state = PARAM_STATE_COMPLETE;
        }
        break;
      default:
        context->state->state = PARAM_STATE_COMPLETE;
        break;
    }
  } while (old_state > context->state->state);
}

void transmitInt(FunctionContext_t* context, bool is_feedback)
{
  ParamState_t old_state = context->state->state;

  do {
    switch (context->state->state) {
      case PARAM_STATE_0:
        sprintf((char*) context->output_buffer, "\r\n\r\nPlease enter an integer to "
            "send to the %s between 0 and 4,294,967,295:\r\n", 
            is_feedback ? "feedback network" : "transducer");
        COMM_TransmitData(context->output_buffer, CALC_LEN, context->comm_interface);
        context->state->state = PARAM_STATE_1;
        break;
      case PARAM_STATE_1:
        uint32_t input;
        if (checkUint32(context->input, context->input_len, &input, 0, 4294967295) == false) {
          sprintf((char*) context->output_buffer, "\r\nInvalid input!\r\n");
          COMM_TransmitData(context->output_buffer, CALC_LEN, context->comm_interface);
          context->state->state = PARAM_STATE_0;
        }
        else {
          Message_t msg;
          msg.type = is_feedback ? MSG_TRANSMIT_FEEDBACK : MSG_TRANSMIT_TRANSDUCER;
          msg.timestamp = osKernelGetTickCount();
          msg.data_type = INTEGER;
          msg.length_bits = 8 * sizeof(uint32_t);
          memcpy(&msg.data[0], &input, sizeof(uint32_t));
          if (MESS_AddMessageToTxQ(&msg) == pdPASS) {
            sprintf((char*) context->output_buffer, "\r\nSuccessfully added to feedback queue!\r\n\r\n");
            COMM_TransmitData(context->output_buffer, CALC_LEN, context->comm_interface);
          }
          else {
            sprintf((char*) context->output_buffer, "\r\nError adding message to feedback queue\r\n\r\n");
            COMM_TransmitData(context->output_buffer, CALC_LEN, context->comm_interface);
          }
          context->state->state = PARAM_STATE_COMPLETE;
        }
        break;
      default:
        context->state->state = PARAM_STATE_COMPLETE;
        break;
    }
  } while (old_state > context->state->state);
}

void transmitFloat(FunctionContext_t* context, bool is_feedback)
{
  ParamState_t old_state = context->state->state;

  do {
    switch (context->state->state) {
      case PARAM_STATE_0:
        sprintf((char*) context->output_buffer, "\r\n\r\nPlease enter a float to "
            "send to the %s:\r\n", 
            is_feedback ? "feedback network" : "transducer");
        COMM_TransmitData(context->output_buffer, CALC_LEN, context->comm_interface);
        context->state->state = PARAM_STATE_1;
        break;
      case PARAM_STATE_1:
        float input;
        if (checkFloat(context->input, &input, -1e30f, 1e30f) == false) {
          sprintf((char*) context->output_buffer, "\r\nInvalid input!\r\n");
          COMM_TransmitData(context->output_buffer, CALC_LEN, context->comm_interface);
          context->state->state = PARAM_STATE_0;
        }
        else {
          Message_t msg;
          msg.type = is_feedback ? MSG_TRANSMIT_FEEDBACK : MSG_TRANSMIT_TRANSDUCER;
          msg.timestamp = osKernelGetTickCount();
          msg.data_type = FLOAT;
          msg.length_bits = 8 * sizeof(float);
          memcpy(&msg.data[0], &input, sizeof(float));
          if (MESS_AddMessageToTxQ(&msg) == pdPASS) {
            sprintf((char*) context->output_buffer, "\r\nSuccessfully added to feedback queue!\r\n\r\n");
            COMM_TransmitData(context->output_buffer, CALC_LEN, context->comm_interface);
          }
          else {
            sprintf((char*) context->output_buffer, "\r\nError adding message to feedback queue\r\n\r\n");
            COMM_TransmitData(context->output_buffer, CALC_LEN, context->comm_interface);
          }
          context->state->state = PARAM_STATE_COMPLETE;
        }
        break;
      default:
        context->state->state = PARAM_STATE_COMPLETE;
        break;
    }
  } while (old_state > context->state->state);
}
