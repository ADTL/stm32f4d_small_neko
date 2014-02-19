/**
 * @file    LM15SGFNZ07/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for the LM15SGFNZ07 display.
 */

#include "gfx.h"

#if GFX_USE_GDISP

#include "gdisp/lld/gdisp_lld.h"
#include "LM15SGFNZ07.h"

#include "gdisp_lld_config.h"

#include "board_LM15SGFNZ07.h"


//Set parameters if they are not already set
#ifndef GDISP_SCREEN_HEIGHT
        #define GDISP_SCREEN_HEIGHT             80
#endif
#ifndef GDISP_SCREEN_WIDTH
        #define GDISP_SCREEN_WIDTH              101
#endif
#ifndef GDISP_INITIAL_CONTRAST
        #define GDISP_INITIAL_CONTRAST  50
#endif
#ifndef GDISP_INITIAL_BACKLIGHT
        #define GDISP_INITIAL_BACKLIGHT 100
#endif

//Some macros just to make reading the code easier
#define delayms(ms) gfxSleepMilliseconds(ms)
/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

LLDSPEC bool_t gdisp_lld_init(GDisplay *g)
{
    unsigned char j;
    // No private area for this controller
    g->priv = 0;
    
    // Initialise the board interface
    init_board(g);
    
    // Hardware reset
    spiUnselect(&DISP_SPI_PORT);
    CLR_RS;
    setpin_reset(g, TRUE);
    delayms(10);
    setpin_reset(g, FALSE);
    delayms(10);
    SET_RS;
    spiSelect(&DISP_SPI_PORT);
    
    for (j = 0; j < 134; j++) {
        write_cmd(g, initData[j]);
    }
    
    delayms(10);
    spiUnselect(&DISP_SPI_PORT);
    
    delayms(10);
    
    spiSelect(&DISP_SPI_PORT);
    
    write_cmd(g, 0xf0);
    write_cmd(g, 0x81);
    write_cmd(g, 0xf4);
    write_cmd(g, 0xb3);
    write_cmd(g, 0xa0);
    
    write_cmd(g, 0xf0);
    write_cmd(g, 0x06);
    write_cmd(g, 0x10);
    write_cmd(g, 0x20);
    write_cmd(g, 0x30);
    write_cmd(g, 0xf5);
    write_cmd(g, 0x0f);
    write_cmd(g, 0x1c);
    write_cmd(g, 0x2f);
    write_cmd(g, 0x34);
    
    write_cmd(g, 0xf4);
    write_cmd(g, 0x94);
    write_cmd(g, 0xb3);
    write_cmd(g, 0xa2);
    write_cmd(g, 0xd0);
    
    spiUnselect(&DISP_SPI_PORT);
    CLR_RS;
    
    spiStop(&DISP_SPI_PORT);
    spiStart(&DISP_SPI_PORT, &spi2cfg2);
    
    /* Initialise the GDISP structure to match */
    g->g.Width = GDISP_SCREEN_WIDTH;
    g->g.Height = GDISP_SCREEN_HEIGHT;
    g->g.Orientation = GDISP_ROTATE_0;
    g->g.Powermode = powerOn;
    g->g.Backlight = GDISP_INITIAL_BACKLIGHT;
    g->g.Contrast = GDISP_INITIAL_CONTRAST;
    return TRUE;
}

static inline void set_viewport(GDisplay* g) {
        uint8_t xstart = g->p.x;
        uint8_t ystart = g->p.y;
        uint8_t xend = g->p.x + g->p.cx - 1;
        uint8_t yend = g->p.y + g->p.cy - 1;
        
        xstart <<= 1;
        xstart += 6;
        xend <<= 1;
        xend += 7;
        
        SET_RS;
        spiSelect(&DISP_SPI_PORT);
        
        write_cmd(g, 0xf0);
        write_cmd(g, 0x00 | (xstart & 0x0f));
        write_cmd(g, 0x10 | (xstart >> 4));
        write_cmd(g, 0x20 | (ystart & 0x0f));
        write_cmd(g, 0x30 | (ystart >> 4));
        write_cmd(g, 0xf5);
        write_cmd(g, 0x00 | (xend & 0x0f));
        write_cmd(g, 0x10 | (xend >> 4));
        write_cmd(g, 0x20 | (yend & 0x0f));
        write_cmd(g, 0x30 | (yend >> 4));
        spiUnselect(&DISP_SPI_PORT);
        CLR_RS;
}

#if GDISP_HARDWARE_STREAM_WRITE
        LLDSPEC void gdisp_lld_write_start(GDisplay *g) {
                set_viewport(g);
                acquire_bus(g);
        }
        LLDSPEC void gdisp_lld_write_color(GDisplay *g) {
                color_t color = gdispColor2Native(g->p.color);
                write_cmd(g, color >> 8);
                write_cmd(g, color & 0xff);
        }
        LLDSPEC void gdisp_lld_write_stop(GDisplay *g) {
                release_bus(g);
        }
#endif

#endif /* GFX_USE_GDISP */