/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/SSD1963/board_SSD1963_template.h
 * @brief   GDISP Graphic Driver subsystem board interface for the SSD1963 display.
 *
 * @addtogroup GDISP
 * @{
 */
#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

/* Ports */
#define DISP_CTRL_PORT GPIOE
#define DISP_IO_PORT GPIOB
#define DISP_SPI_PORT SPID2

#define DISP_RST_PORT DISP_CTRL_PORT
#define DISP_RS_PORT DISP_CTRL_PORT
#define DISP_CS_PORT DISP_CTRL_PORT
/* Pins */
#define DISP_RST_PIN 10
#define DISP_RS_PIN 9
#define DISP_CS_PIN 11
#define DISP_CLK_PIN 13
#define DISP_DATA_PIN 15

#define SET_RST palSetPad(DISP_RST_PORT, DISP_RST_PIN)
#define CLR_RST palClearPad(DISP_RST_PORT, DISP_RST_PIN)

#define SET_RS palSetPad(DISP_RS_PORT, DISP_RS_PIN)
#define CLR_RS palClearPad(DISP_RS_PORT, DISP_RS_PIN)

#define SET_CS palSetPad(DISP_CS_PORT, DISP_CS_PIN)
#define CLR_CS palClearPad(DISP_CS_PORT, DISP_CS_PIN)


/*
 * SPI1 configuration structure.
 * CPHA=0, CPOL=0, 8bits frames, MSb transmitted first.
 * The slave select line is the pin 4 on the port GPIOA.
 */
static const SPIConfig spi2cfg = {
  0,
  /* HW dependent part.*/
  DISP_CTRL_PORT,
  DISP_CS_PIN,
  SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0 // SPI_CR1_DFF // 8 bit data frame
};

static const SPIConfig spi2cfg2 = {
  0,
  /* HW dependent part.*/
  DISP_CTRL_PORT,
  DISP_CS_PIN,
  SPI_CR1_BR_2 | SPI_CR1_BR_1 // SPI_CR1_DFF // 8 bit data frame
};
/**
 * @brief   Set or clear the lcd reset pin.
 *
 * @param[in] g                 The GDisplay structure
 * @param[in] state             TRUE = lcd in reset, FALSE = normal operation
 * 
 * @notapi
 */

/**
 * @brief   Initialise the board for the display.
 *
 * @param[in] g                 The GDisplay structure
 *
 * @note        Set the g->board member to whatever is appropriate. For multiple
 *                      displays this might be a pointer to the appropriate register set.
 *
 * @notapi
 */
static inline void init_board(GDisplay *g) {
        (void) g;
        /* Reset pin initialisation */
        palSetPadMode(DISP_RST_PORT, DISP_RST_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
        /* RS pin initialisation */
        palSetPadMode(DISP_RS_PORT, DISP_RS_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
        /* CS pin initialisation */
        palSetPadMode(DISP_CS_PORT, DISP_CS_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
        /*
        * Initializes the SPI driver 2. The SPI2 signals are routed as follow:
        * PB13 - SCK.
        * PB15 - MOSI.
        */
        palSetPadMode(DISP_IO_PORT, DISP_CLK_PIN, PAL_MODE_ALTERNATE(5) |
                                PAL_STM32_OSPEED_HIGHEST);           /* SCK.     */
        palSetPadMode(DISP_IO_PORT, DISP_DATA_PIN, PAL_MODE_ALTERNATE(5) |
                                PAL_STM32_OSPEED_HIGHEST);           /* MOSI.    */
        spiInit();
        spiStart(&DISP_SPI_PORT, &spi2cfg);
}

static inline void setpin_reset(GDisplay *g, bool_t state) {
        (void) g;
        if (state) {
                CLR_RST;
        } else {
                SET_RST;
        }
}

static inline void write_cmd(GDisplay *g, uint8_t cmd) {
        (void) g;
        //spiSend(&DISP_SPI_PORT, 1, &cmd);
        spi_lld_polled_exchange(&DISP_SPI_PORT, cmd);
}

/**
 * @brief   Take exclusive control of the bus
 *
 * @param[in] g                         The GDisplay structure
 *
 * @notapi
 */
static inline void acquire_bus(GDisplay *g) {
        (void) g;
        
        CLR_RS;
        spiSelect(&DISP_SPI_PORT);
}

/**
 * @brief   Release exclusive control of the bus
 *
 * @param[in] g                         The GDisplay structure
 *
 * @notapi
 */
static inline void release_bus(GDisplay *g) {
        (void) g;
        
        spiUnselect(&DISP_SPI_PORT);
        SET_RS;
}
#endif /* _GDISP_LLD_BOARD_H */
/** @} */