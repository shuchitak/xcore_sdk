// Copyright (c) 2019, XMOS Ltd, All rights reserved

#include "soc.h"
#include "soc_bsp_common.h"
#include "bitstream_devices.h"

#include "spi_master_driver.h"

#include "debug_print.h"

#include "FreeRTOS.h"
#include "semphr.h"

#if ( SOC_SPI_PERIPHERAL_USED == 0 )
#define BITSTREAM_SPI_DEVICE_COUNT 0
soc_peripheral_t bitstream_spi_devices[BITSTREAM_SPI_DEVICE_COUNT];
#endif /* SOC_SPI_PERIPHERAL_USED */


RTOS_IRQ_ISR_ATTR
void spi_master_ISR(soc_peripheral_t device)
{
    QueueHandle_t queue = soc_peripheral_app_data(device);
    BaseType_t xYieldRequired = pdFALSE;
    uint32_t status;

    status = soc_peripheral_interrupt_status(device);

    if (status & SOC_PERIPHERAL_ISR_DMA_RX_DONE_BM) {
        soc_dma_ring_buf_t *rx_ring_buf;
        int length;
        uint8_t *rx_buf;

        configASSERT(device == bitstream_spi_devices[BITSTREAM_SPI_DEVICE_A]);

        rx_ring_buf = soc_peripheral_rx_dma_ring_buf(device);
        rx_buf = soc_dma_ring_rx_buf_get(rx_ring_buf, &length);
        configASSERT(rx_buf != NULL);

        if (xQueueSendFromISR(queue, &rx_buf, &xYieldRequired) == errQUEUE_FULL) {
            ;   // We do not set a new buffer
        }
    }

    portEND_SWITCHING_ISR( xYieldRequired );
}


soc_peripheral_t spi_master_driver_init(
        int device_id,
        int rx_desc_count,
        int rx_buf_size,
        int tx_desc_count,
        void *app_data,
        int isr_core,
        rtos_irq_isr_t isr)
{
    soc_peripheral_t device;

    xassert(device_id >= 0 && device_id < BITSTREAM_SPI_DEVICE_COUNT);

    device = bitstream_spi_devices[device_id];

    soc_peripheral_common_dma_init(
            device,
            rx_desc_count,
            rx_buf_size,
            tx_desc_count,
            app_data,
            isr_core,
            isr);

    return device;
}


static void spi_driver_transaction(
        soc_peripheral_t dev,
        uint8_t* rx_buf,
        uint8_t* tx_buf,
        size_t len)
{
    chanend c = soc_peripheral_ctrl_chanend(dev);

    if( rx_buf != NULL )
    {
        soc_dma_ring_buf_t *rx_ring_buf = soc_peripheral_rx_dma_ring_buf(dev);
        soc_dma_ring_rx_buf_set(rx_ring_buf, rx_buf, (uint16_t)len );
    }

    if(tx_buf == NULL)
    {
        tx_buf = (uint8_t*)pvPortMalloc( sizeof(uint8_t)*len );
        memset(tx_buf, 0x00, len);
    }
    if( tx_buf != NULL )
    {
        soc_dma_ring_buf_t *tx_ring_buf = soc_peripheral_tx_dma_ring_buf(dev);
        soc_dma_ring_tx_buf_set(tx_ring_buf, tx_buf, (uint16_t)len );
    }

    soc_peripheral_function_code_tx(c, SPI_MASTER_DEV_TRANSACTION);

    soc_peripheral_varlist_tx(
            c, 3,
            sizeof(size_t), &len,
            sizeof(uint8_t*), (uint8_t*)&rx_buf,
            sizeof(uint8_t*), (uint8_t*)&tx_buf);
}

void spi_master_device_init(
        soc_peripheral_t dev,
        unsigned cs_port_bit,
        unsigned cpol,
        unsigned cpha,
        unsigned clock_divide,
        unsigned cs_to_data_delay_ns,
        unsigned byte_setup_ns)
{
    chanend c_ctrl = soc_peripheral_ctrl_chanend(dev);

    soc_peripheral_function_code_tx(c_ctrl, SPI_MASTER_DEV_INIT);

    soc_peripheral_varlist_tx(
            c_ctrl, 6,
            sizeof(unsigned), &cs_port_bit,
            sizeof(unsigned), &cpol,
            sizeof(unsigned), &cpha,
            sizeof(unsigned), &clock_divide,
            sizeof(unsigned), &cs_to_data_delay_ns,
            sizeof(unsigned), &byte_setup_ns);
}

void spi_transmit(
        soc_peripheral_t dev,
        uint8_t* tx_buf,
        size_t len)
{
    spi_driver_transaction(dev,
                           NULL,
                           tx_buf,
                           len);
}

void spi_request(
        soc_peripheral_t dev,
        uint8_t* rx_buf,
        size_t len)
{
    spi_driver_transaction(dev,
                           rx_buf,
                           NULL,
                           len);
}

void spi_transmit_blocking(
        soc_peripheral_t dev,
        uint8_t* tx_buf,
        size_t len)
{
    spi_driver_transaction(dev,
                           NULL,
                           tx_buf,
                           len);
    /* Cleanup tx */
    soc_dma_ring_buf_t *tx_ring_buf = soc_peripheral_tx_dma_ring_buf(dev);
    uint8_t* tmpbuf;
    while( ( tmpbuf = soc_dma_ring_tx_buf_get( tx_ring_buf, NULL, NULL ) ) == tx_buf )
    {
        vPortFree(tmpbuf);
    }
}

void spi_request_blocking(
        soc_peripheral_t dev,
        uint8_t* rx_buf,
        size_t len)
{
    QueueHandle_t queue = soc_peripheral_app_data(dev);

    spi_driver_transaction(dev,
                           rx_buf,
                           NULL,
                           len);

    xQueueReceive(queue, &rx_buf, portMAX_DELAY);
}

void spi_transaction(
        soc_peripheral_t dev,
        uint8_t* rx_buf,
        uint8_t* tx_buf,
        size_t len)
{
    spi_driver_transaction(dev,
                           rx_buf,
                           tx_buf,
                           len);
}

void spi_transaction_blocking(
        soc_peripheral_t dev,
        uint8_t* rx_buf,
        uint8_t* tx_buf,
        size_t len)
{
    QueueHandle_t queue = soc_peripheral_app_data(dev);

    spi_driver_transaction(dev,
                           rx_buf,
                           tx_buf,
                           len);

    xQueueReceive(queue, &rx_buf, portMAX_DELAY);
}

