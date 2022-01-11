/*-
 * BSD 2-Clause License
 *
 * Copyright (c) 2012-2018, Jan Breuer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file   scpi-def.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 *
 * @brief  SCPI parser test
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scpi/scpi.h"
#include "scpi-def.h"
#include "main.h"
#include "MP8862.h"

extern struct ops ops;
extern struct MP8862_t MP8862;

/* EN */
static scpi_result_t OPS_ConfigureOutputEn(scpi_t * context) {
    double param1;
    char _err[150] = {0};

    /* read first parameter if present */
    if (!SCPI_ParamDouble(context, &param1, TRUE)) {
        return SCPI_RES_ERR;
    }

    sprintf(_err, "conf:output:en\tP1=%lf", param1);
    CDC_Transmit_FS(_err, 150);

    return SCPI_RES_OK;
}

static scpi_result_t OPS_StatusOutputEn(scpi_t * context) {
    SCPI_ResultDouble(context, ops.en);
    return SCPI_RES_OK;
}

/* VOUT */
static scpi_result_t OPS_ConfigureVoltageDc(scpi_t * context) {
    double param1;
    char _err[150] = {0};

    /* read first parameter if present */
    if (!SCPI_ParamDouble(context, &param1, TRUE)) {
        return SCPI_RES_ERR;
    }

    //sprintf(_err, "conf:volt:dc\tP1=%lf", param1);
    //CDC_Transmit_FS(_err, 150);
    MP8862_setVoltageSetpoint_mV(&MP8862, (uint16_t)(param1*1000));

    return SCPI_RES_OK;
}

static scpi_result_t OPS_MeasureVoltageDc(scpi_t * context) {
    SCPI_ResultDouble(context, ops.vout);
    return SCPI_RES_OK;
}

/* IOUT */
static scpi_result_t OPS_ConfigureCurrentDc(scpi_t * context) {
    double param1;
    char _err[150] = {0};

    /* read first parameter if present */
    if (!SCPI_ParamDouble(context, &param1, TRUE)) {
        return SCPI_RES_ERR;
    }

    sprintf(_err, "conf:curr:dc\tP1=%lf", param1);
    CDC_Transmit_FS(_err, 150);

    return SCPI_RES_OK;
}

static scpi_result_t OPS_MeasureCurrentDc(scpi_t * context) {
    SCPI_ResultDouble(context, ops.iout);
    return SCPI_RES_OK;
}

/* TEMP*/
static scpi_result_t OPS_MeasureTemperatureTc(scpi_t * context) {
    SCPI_ResultDouble(context, ops.tc);
    return SCPI_RES_OK;
}

static scpi_result_t OPS_MeasureTemperatureRef(scpi_t * context) {
    SCPI_ResultDouble(context, ops.tref);
    return SCPI_RES_OK;
}


//static scpi_result_t EXAMPLE_MeasureVoltageIn(scpi_t * context) {
//    SCPI_ResultDouble(context, output);
//
//    return SCPI_RES_OK;
//}

//static scpi_result_t EXAMPLE_MeasureVoltageOut(scpi_t * context) {
//    SCPI_ResultUInt32(context, voltage);
//
//    return SCPI_RES_OK;
//}

//static scpi_result_t DMM_MeasureVoltageAcQ(scpi_t * context) {
//    scpi_number_t param1, param2;
//    char bf1[15];
//    char bf2[15];
//    char _err[150] = {0};
//
//    /* read first parameter if present */
//    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
//        /* do something, if parameter not present */
//    }
//
//    /* read second paraeter if present */
//    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param2, FALSE)) {
//        /* do something, if parameter not present */
//    }
//
//
//    SCPI_NumberToStr(context, scpi_special_numbers_def, &param1, bf1, 15);
//
//
//
//    SCPI_NumberToStr(context, scpi_special_numbers_def, &param2, bf2, 15);
//
//    sprintf(_err, "meas:volt:ac\tP1=%s\tP2=%s", bf1, bf2);
//    CDC_Transmit_FS(_err, 150);
//
//    SCPI_ResultDouble(context, 0);
//
//    return SCPI_RES_OK;
//}

//static scpi_result_t TEST_Bool(scpi_t * context) {
//    scpi_bool_t param1;
//    char _err[150] = {0};
//
//    /* read first parameter if present */
//    if (!SCPI_ParamBool(context, &param1, TRUE)) {
//        return SCPI_RES_ERR;
//    }
//
//    sprintf(_err, "TEST:BOOL\tP1=%d", param1);
//    CDC_Transmit_FS(_err, 150);
//
//    return SCPI_RES_OK;
//}

//scpi_choice_def_t trigger_source[] = {
//    {"BUS", 5},
//    {"IMMediate", 6},
//    {"EXTernal", 7},
//    SCPI_CHOICE_LIST_END /* termination of option list */
//};

//static scpi_result_t TEST_ChoiceQ(scpi_t * context) {
//    char _err[150] = {0};
//    int32_t param;
//    const char * name;
//
//    if (!SCPI_ParamChoice(context, trigger_source, &param, TRUE)) {
//        return SCPI_RES_ERR;
//    }
//
//    SCPI_ChoiceToName(trigger_source, param, &name);
//
//    sprintf(_err, "\tP1=%s (%ld)", name, (long int) param);
//    CDC_Transmit_FS(_err, 150);
//
//    SCPI_ResultInt32(context, param);
//
//    return SCPI_RES_OK;
//}

//static scpi_result_t TEST_Numbers(scpi_t * context) {
//    int32_t numbers[2];
//    char _err[150] = {0};
//
//    SCPI_CommandNumbers(context, numbers, 2, 1);
//
//    sprintf(_err, "TEST numbers %d %d", numbers[0], numbers[1]);
//    CDC_Transmit_FS(_err, 150);
//
//    return SCPI_RES_OK;
//}

//static scpi_result_t TEST_Text(scpi_t * context) {
//    char buffer[100];
//    char _err[150] = {0};
//    size_t copy_len;
//
//    if (!SCPI_ParamCopyText(context, buffer, sizeof(buffer), &copy_len, FALSE)) {
//        buffer[0] = '\0';
//    }
//
//    sprintf(_err, "TEXT: ***%s***", buffer);
//    CDC_Transmit_FS(_err, 150);
//
//    return SCPI_RES_OK;
//}

//static scpi_result_t TEST_ArbQ(scpi_t * context) {
//    const char * data;
//    size_t len;
//
//    if (SCPI_ParamArbitraryBlock(context, &data, &len, FALSE)) {
//        SCPI_ResultArbitraryBlock(context, data, len);
//    }
//
//    return SCPI_RES_OK;
//}

/**
 * Reimplement IEEE488.2 *TST?
 *
 * Result should be 0 if everything is ok
 * Result should be 1 if something goes wrong
 *
 * Return SCPI_RES_OK
 */
static scpi_result_t My_CoreTstQ(scpi_t * context) {

    SCPI_ResultInt32(context, 0);

    return SCPI_RES_OK;
}

const scpi_command_t scpi_commands[] = {
    /* IEEE Mandated Commands (SCPI std V1999.0 4.1.1) */
    { .pattern = "*CLS", .callback = SCPI_CoreCls,},
    { .pattern = "*ESE", .callback = SCPI_CoreEse,},
    { .pattern = "*ESE?", .callback = SCPI_CoreEseQ,},
    { .pattern = "*ESR?", .callback = SCPI_CoreEsrQ,},
    { .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
    { .pattern = "*OPC", .callback = SCPI_CoreOpc,},
    { .pattern = "*OPC?", .callback = SCPI_CoreOpcQ,},
    { .pattern = "*RST", .callback = SCPI_CoreRst,},
    { .pattern = "*SRE", .callback = SCPI_CoreSre,},
    { .pattern = "*SRE?", .callback = SCPI_CoreSreQ,},
    { .pattern = "*STB?", .callback = SCPI_CoreStbQ,},
    { .pattern = "*TST?", .callback = My_CoreTstQ,},
    { .pattern = "*WAI", .callback = SCPI_CoreWai,},

    /* Required SCPI commands (SCPI std V1999.0 4.2.1) */
    {.pattern = "SYSTem:ERRor[:NEXT]?", .callback = SCPI_SystemErrorNextQ,},
    {.pattern = "SYSTem:ERRor:COUNt?", .callback = SCPI_SystemErrorCountQ,},
    {.pattern = "SYSTem:VERSion?", .callback = SCPI_SystemVersionQ,},

    {.pattern = "STATus:QUEStionable[:EVENt]?", .callback = SCPI_StatusQuestionableEventQ,},
    {.pattern = "STATus:QUEStionable:ENABle", .callback = SCPI_StatusQuestionableEnable,},
    {.pattern = "STATus:QUEStionable:ENABle?", .callback = SCPI_StatusQuestionableEnableQ,},

    {.pattern = "STATus:PRESet", .callback = SCPI_StatusPreset,},

    /* OPS */
    {.pattern = "CONFigure:OUTput:EN", .callback = OPS_ConfigureOutputEn,},
    {.pattern = "STATus:OUTput:EN", .callback = OPS_StatusOutputEn,},
    {.pattern = "CONFigure:VOLTage:DC", .callback = OPS_ConfigureVoltageDc,},
    {.pattern = "MEASure:VOLTage:DC?", .callback = OPS_MeasureVoltageDc,},
    {.pattern = "CONFigure:CURRent:DC", .callback = OPS_ConfigureCurrentDc,},
    {.pattern = "MEASure:CURRent:DC?", .callback = OPS_MeasureCurrentDc,},
    {.pattern = "MEASure:TEMPerature:TC?", .callback = OPS_MeasureTemperatureTc,},
    {.pattern = "MEASure:TEMPerature:REF?", .callback = OPS_MeasureTemperatureRef,},

    {.pattern = "SYSTem:COMMunication:TCPIP:CONTROL?", .callback = SCPI_SystemCommTcpipControlQ,},

    SCPI_CMD_LIST_END
};

scpi_interface_t scpi_interface = {
    .error = SCPI_Error,
    .write = SCPI_Write,
    .control = SCPI_Control,
    .flush = SCPI_Flush,
    .reset = SCPI_Reset,
};

char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
scpi_error_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];

scpi_t scpi_context;
