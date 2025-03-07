// Copyright 2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/* System headers */
#include <platform.h>
#include <xs1.h>

/* Library headers */
#include "rtos/osal/api/rtos_osal.h"
#include "rtos/drivers/intertile/api/rtos_intertile.h"

/* App headers */
#include "app_conf.h"
#include "individual_tests/intertile/intertile_test.h"

static const char* test_name = "var_len_tx_test";

#define local_printf( FMT, ... )    intertile_printf("%s|" FMT, test_name, ##__VA_ARGS__)

#define INTERTILE_TX_TILE 0
#define INTERTILE_RX_TILE 1

typedef struct test_params {
    size_t len;
    uint8_t *data;
} test_params_t;

#define INTERTILE_TEST_ITERS      3
#define INTERTILE_RX_BUF_SIZE     1000

#define INTERTILE_TEST_VECTOR_2_LEN   1000
static uint8_t test_vector_0[] = {0x00, 0xFF, 0xAA, 0x55};
static uint8_t test_vector_1[] = {0xDE, 0xAD, 0xBE, 0xEF};
static uint8_t test_vector_2[INTERTILE_TEST_VECTOR_2_LEN] = {0};

static test_params_t intertile_tests[INTERTILE_TEST_ITERS] =
{
    {sizeof(test_vector_0), test_vector_0},
    {sizeof(test_vector_1), test_vector_1},
    {sizeof(test_vector_2), test_vector_2},
};

INTERTILE_MAIN_TEST_ATTR
static int main_test(intertile_test_ctx_t *ctx)
{
    local_printf("Start");

    for (int i=0; i<INTERTILE_TEST_VECTOR_2_LEN; i++)
    {
        test_vector_2[i] = (uint8_t)(0xFF & i);
    }


    for (int i=0; i<INTERTILE_TEST_ITERS; i++)
    {
        size_t test_len = intertile_tests[i].len;
        uint8_t *test_buf = intertile_tests[i].data;

        local_printf("Test iteration %d", i);

        #if ON_TILE(INTERTILE_TX_TILE)
        {
            local_printf("TX len %u", test_len);

            rtos_intertile_tx_len(ctx->intertile_ctx,
                                  INTERTILE_RPC_PORT,
                                  test_len);

            local_printf("TX data of len %u", test_len);

            size_t tx = rtos_intertile_tx_data(ctx->intertile_ctx,
                                               test_buf,
                                               test_len);
            if (tx != test_len)
            {
                local_printf("TX failed.  send %d expected %d", tx, test_len);
                return -1;
            }

            local_printf("TX done");
        }
        #endif

        #if ON_TILE(INTERTILE_RX_TILE)
        {
            uint8_t *rx_buf = NULL;
            size_t len_rx = 0;

            len_rx = rtos_intertile_rx_len(ctx->intertile_ctx,
                                           INTERTILE_RPC_PORT,
                                           RTOS_OSAL_WAIT_MS(10));

            rx_buf = (uint8_t*)rtos_osal_malloc(len_rx * sizeof(uint8_t));

            if (rx_buf == NULL)
            {
                local_printf("RX failed.  Tried to malloc %d bytes", len_rx);
                return -1;
            }

            size_t bytes_rx = rtos_intertile_rx_data(ctx->intertile_ctx,
                                                (void*)rx_buf,
                                                len_rx);

            if (bytes_rx != test_len)
            {
                local_printf("RX failed.  Got %u expected %u", bytes_rx, test_len);
                rtos_osal_free(rx_buf);
                return -1;
            } else {
                local_printf("RX passed.  Got %u expected %u", bytes_rx, test_len);
            }

            for (size_t j=0; j< bytes_rx; j++)
            {
                if (test_buf[j] != rx_buf[j])
                {
                    local_printf("RX failed at index %u.  Got %u expected %u", j, rx_buf[j], test_buf[j]);
                    rtos_osal_free(rx_buf);
                    return -1;
                }
            }
           rtos_osal_free(rx_buf);
        }
        #endif
    }

    local_printf("Done");
    return 0;
}

void register_var_len_tx_test(intertile_test_ctx_t *test_ctx)
{
    uint32_t this_test_num = test_ctx->test_cnt;

    local_printf("Register to test num %d", this_test_num);

    test_ctx->name[this_test_num] = (char*)test_name;
    test_ctx->main_test[this_test_num] = main_test;

    test_ctx->test_cnt++;
}

#undef local_printf
