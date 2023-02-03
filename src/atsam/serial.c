// sam3/sam4 serial port
//
// Copyright (C) 2016-2018  Kevin O'Connor <kevin@koconnor.net>
//
// This file may be distributed under the terms of the GNU GPLv3 license.

#include "autoconf.h" // CONFIG_SERIAL_BAUD
#include "board/armcm_boot.h" // armcm_enable_irq
#include "board/serial_irq.h" // serial_rx_data
#include "command.h" // DECL_CONSTANT_STR
#include "internal.h" // gpio_peripheral
#include "sched.h" // DECL_INIT

// Serial port pins
#if CONFIG_MACH_SAM3X
#define UARTx_IRQn USART1_IRQn
static Usart * const Port = USART1;
static const uint32_t Pmc_id = ID_USART1;
static const uint32_t rx_pin = GPIO('A', 12), tx_pin = GPIO('A', 13);
static const char uart_periph = 'A';
DECL_CONSTANT_STR("RESERVE_PINS_serial", "PA12,PA13");
#elif CONFIG_MACH_SAM4S
#define UARTx_IRQn UART1_IRQn
static Uart * const Port = UART1;
static const uint32_t Pmc_id = ID_UART1;
static const uint32_t rx_pin = GPIO('B', 2), tx_pin = GPIO('B', 3);
static const char uart_periph = 'A';
DECL_CONSTANT_STR("RESERVE_PINS_serial", "PB2,PB3");
#elif CONFIG_MACH_SAM4E
#define UARTx_IRQn UART0_IRQn
static Uart * const Port = UART0;
static const uint32_t Pmc_id = ID_UART0;
static const uint32_t rx_pin = GPIO('A', 9), tx_pin = GPIO('A', 10);
static const char uart_periph = 'A';
DECL_CONSTANT_STR("RESERVE_PINS_serial", "PA9,PA10");
#elif CONFIG_MACH_SAME70
#define UARTx_IRQn UART2_IRQn
static Uart * const Port = UART2;
static const uint32_t Pmc_id = ID_UART2;
static const uint32_t rx_pin = GPIO('D', 25), tx_pin = GPIO('D', 26);
static const char uart_periph = 'C';
DECL_CONSTANT_STR("RESERVE_PINS_serial", "PD25,PD26");
#endif

void
UARTx_Handler(void)
{
    uint32_t status = Port->US_CSR;
    if (status & US_CSR_RXRDY)
        serial_rx_byte(Port->US_RHR);
    if (status & US_CSR_TXRDY) {
        uint8_t data;
        int ret = serial_get_tx_byte(&data);
        if (ret)
            Port->US_IDR = US_IDR_TXRDY;
        else
            Port->US_THR = data;
    }
}

void
serial_enable_tx_irq(void)
{
    Port->US_IER = US_IDR_TXRDY;
}

void
serial_init(void)
{
    gpio_peripheral(rx_pin, uart_periph, 1);
    gpio_peripheral(tx_pin, uart_periph, 0);

    // Reset uart
    enable_pclock(Pmc_id);
    Port->US_CR = (US_CR_RSTRX | US_CR_RSTTX
                     | US_CR_RXDIS | US_CR_TXDIS);
    Port->US_IDR = 0xFFFFFFFF;

    // Enable uart
    Port->US_MR = (US_MR_PAR_NO | US_MR_CHMODE_NORMAL);
    Port->US_BRGR = get_pclock_frequency(Pmc_id) / (16 * CONFIG_SERIAL_BAUD);
    Port->US_IER = US_IER_RXRDY;
    armcm_enable_irq(UARTx_Handler, UARTx_IRQn, 0);
    Port->US_CR = US_CR_RXEN | US_CR_TXEN;
}
DECL_INIT(serial_init);
