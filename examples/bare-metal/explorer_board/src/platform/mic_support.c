// Copyright 2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/* System headers */
#include <platform.h>
#include <xs1.h>
#include <string.h>
#include <xcore/port.h>
#include <xcore/clock.h>

/* SDK headers */
#include "mic_array.h"
#include "xcore_utils.h"

/* App headers */
#include "app_conf.h"
#include "mic_support.h"

void mic_array_setup_ddr(
        xclock_t pdmclk,
        xclock_t pdmclk6,
        port_t p_mclk,
        port_t p_pdm_clk,
        port_t p_pdm_mics,
        int divide)
{
    uint32_t tmp;

    clock_enable(pdmclk);
    port_enable(p_mclk);
    clock_set_source_port(pdmclk, p_mclk);
    clock_set_divide(pdmclk, divide/2);

    clock_enable(pdmclk6);
    clock_set_source_port(pdmclk6, p_mclk);
    clock_set_divide(pdmclk6, divide/4);

    port_enable(p_pdm_clk);
    port_set_clock(p_pdm_clk, pdmclk);
    port_set_out_clock(p_pdm_clk);

    port_start_buffered(p_pdm_mics, 32);
    port_set_clock(p_pdm_mics, pdmclk6);
    port_clear_buffer(p_pdm_mics);

    /* start the faster capture clock */
    clock_start(pdmclk6);

    /* wait for a rising edge on the capture clock */
    //port_clear_trigger_in(p_pdm_mics);
    asm volatile("inpw %0, res[%1], 4" : "=r"(tmp) : "r" (p_pdm_mics));

    /* start the slower output clock */
    clock_start(pdmclk);
}
