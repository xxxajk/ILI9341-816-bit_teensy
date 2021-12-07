/*
        Uses hardware registers for fast speed.
 */

#include "ILI9341-816-bit_teensy.h"
extern "C" const unsigned char glcdfont[];

#ifndef swap
#define swap(a, b) { typeof(a) t = a; a = b; b = t; }
#endif
#define MADCTL_MY 0x80
#define MADCTL_MX 0x40
#define MADCTL_MV 0x20
#define MADCTL_ML 0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH 0x04

void ILI9341_TFT::write8stream(uint8_t c) {
        *((volatile uint8_t *)(&TFT_DATA_OUT_LSB)) = c;
        WR_ACTIVE;
        WR_IDLE;
}

void ILI9341_TFT::write8(uint8_t c) {
        CS_ACTIVE;
        write8stream(c);
        CS_IDLE;
}

/*
void ILI9341_TFT::write16stream(uint16_t d) {
        if(wide) {
 *((volatile uint8_t *)(&TFT_DATA_OUT_MSB)) = d >> 8;
 *((volatile uint8_t *)(&TFT_DATA_OUT_LSB)) = d & 0xffU;
                WR_ACTIVE;
                WR_IDLE;
        } else {
                write8stream(d >> 8);
                write8stream(d & 0xffU);
        }
}

void ILI9341_TFT::writedata16(uint16_t d) {
        CD_DATA;
        CS_ACTIVE;
                write8stream(d >> 8);
        CS_IDLE;
        CS_ACTIVE;
                write8stream(d & 0xffU);
        CS_IDLE;
}
 */

ILI9341_TFT::ILI9341_TFT(bool fast, int rst, int backlight, int t_S, int t_I) {
#if !TFT_CAN_16BIT
        fast = false; // disallow 16bit, no matter what the user wants
#endif
        wide = fast;
        if(rst == -2) {
                if(wide) {
                        TFT_RST = TFT_RST_16;
                } else {
                        TFT_RST = TFT_RST_8;
                }
        } else {

                TFT_RST = rst;
        }
        if(backlight == -2) {
                if(wide) {
                        TFT_BACKLIGHT = TFT_BACKLIGHT_16;

                } else {
                        TFT_BACKLIGHT = TFT_BACKLIGHT_8;
                }
        } else {
                TFT_BACKLIGHT = backlight;
        }

        if(t_S == -2) {
                if(wide) {
                        touch_CS_SPI = TOUCHSCREEN_CS_16;
                } else {
                        touch_CS_SPI = TOUCHSCREEN_CS_8;
                }
        } else {
                touch_CS_SPI = t_S;
        }
        if(t_I == -2) {
                if(wide) {
                        touch_IRQ_SPI = TOUCHSCREEN_IRQ_16;
                } else {
                        touch_IRQ_SPI = TOUCHSCREEN_IRQ_8;
                }
        } else {
                touch_IRQ_SPI = t_I;
        }
        if(wide || ((touch_CS_SPI == TOUCHSCREEN_CS_16) && (touch_IRQ_SPI == TOUCHSCREEN_IRQ_16))) {
                // SCK 27, MO 28, MI 39
                SPI.setMOSI(28);
                SPI.setMISO(39);
                SPI.setSCK(27);
        }
        _width = ILI9341_TFTWIDTH;
        _height = ILI9341_TFTHEIGHT;
        rotation = 0;
        cursor_y = cursor_x = 0;
        textsize = 1;
        textcolor = textbgcolor = 0xFFFF;
        wrap = true;
        font = NULL;
}

void ILI9341_TFT::setWriteDataBus(void) {
        *((volatile uint8_t *)(&TFT_DATA_DDR_LSB)) = 0xFFU;
        if(wide) *((volatile uint8_t *)(&TFT_DATA_DDR_MSB)) = 0xFFU;
}

void ILI9341_TFT::setReadDataBus(void) {
        *((volatile uint8_t *)(&TFT_DATA_OUT_LSB)) = 0x00U;
        *((volatile uint8_t *)(&TFT_DATA_DDR_LSB)) = 0x00U;
        if(wide) {
                *((volatile uint8_t *)(&TFT_DATA_OUT_MSB)) = 0x00U;
                *((volatile uint8_t *)(&TFT_DATA_DDR_MSB)) = 0x00U;
        }
}

void ILI9341_TFT::writecommand(uint8_t c) {
        CD_COMMAND;
        write8(c);
}

void ILI9341_TFT::writedata(uint8_t c) {
        CD_DATA;
        write8(c);
}

void ILI9341_TFT::begin(void) {
        pinMode(TFT_RD, OUTPUT);
        pinMode(TFT_WR, OUTPUT);
        pinMode(TFT_RS, OUTPUT);
        pinMode(TFT_CS, OUTPUT);

        CS_IDLE; // Set all control bits to HIGH (idle)
        CD_DATA; // Signals are ACTIVE LOW
        WR_IDLE;
        RD_IDLE;


        if(TFT_BACKLIGHT >= 0) {
                pinMode(TFT_BACKLIGHT, OUTPUT);
                digitalWriteFast(TFT_BACKLIGHT, HIGH);

        }
        // pre-condition I/O defaults

        pinMode(TFT_D0, OUTPUT);
        pinMode(TFT_D1, OUTPUT);
        pinMode(TFT_D2, OUTPUT);
        pinMode(TFT_D3, OUTPUT);
        pinMode(TFT_D4, OUTPUT);
        pinMode(TFT_D5, OUTPUT);
        pinMode(TFT_D6, OUTPUT);
        pinMode(TFT_D7, OUTPUT);

#if defined(TFT_CAN_16BIT)
        if(wide) {
                pinMode(TFT_D8, OUTPUT);
                pinMode(TFT_D9, OUTPUT);
                pinMode(TFT_D10, OUTPUT);
                pinMode(TFT_D11, OUTPUT);
                pinMode(TFT_D12, OUTPUT);
                pinMode(TFT_D13, OUTPUT);
                pinMode(TFT_D14, OUTPUT);
                pinMode(TFT_D15, OUTPUT);
        }
#endif
        setWriteDataBus();
        // hard reset
        if(TFT_RST >= 0) {
                pinMode(TFT_RST, OUTPUT);
                digitalWriteFast(TFT_RST, LOW);
                delay(20);
                digitalWriteFast(TFT_RST, HIGH);
                delay(250);
        }
        // soft reset
        writecommand(ILI9341_SW_RESET);
        delay(150);

        writecommand(ILI9341_UNDOC_1);
        writedata(0x03U);
        writedata(0x80U);
        writedata(0x02U);

        writecommand(ILI9341_PWR_CTRL_B);
        writedata(0x00U);
        writedata(0XC1U);
        writedata(0X30U);

        writecommand(ILI9341_PWR_ON_SEQ_CTRL);
        writedata(0x64U);
        writedata(0x03U);
        writedata(0X12U);
        writedata(0X81U);

        writecommand(ILI9341_DVR_TIME_CTRL_A1);
        writedata(0x85U);
        writedata(0x00U);
        writedata(0x78U);

        writecommand(ILI9341_PWR_CTRL_A);
        writedata(0x39U);
        writedata(0x2CU);
        writedata(0x00U);
        writedata(0x34U);
        writedata(0x02U);

        writecommand(ILI9341_PUMP_RATIO_CTRL);
        writedata(0x20U);

        writecommand(ILI9341_DVR_TIME_CTRL_B);
        writedata(0x00U);
        writedata(0x00U);

        writecommand(ILI9341_PWR_CTRL_1);
        writedata(0x23U); // VRH[5:0]

        writecommand(ILI9341_PWR_CTRL_2);
        writedata(0x10U); // SAP[2:0];BT[3:0]

        writecommand(ILI9341_VCOM_CTRL_1);
        writedata(0x3eU); //
        writedata(0x28U);

        writecommand(ILI9341_VCOM_CTRL_2);
        writedata(0x86U); //

        writecommand(ILI9341_MEM_ADDR_CTL);
        writedata(0x48U);

        writecommand(ILI9341_PIXL_FMT_SET);
        writedata(0x55U);

        writecommand(ILI9341_FRM_CTRL_NORM_MODE);
        writedata(0x00U);
        writedata(0x18U);

        writecommand(ILI9341_DISP_FUNT_CTRL);
        writedata(0x08U);
        writedata(0x82U);
        writedata(0x27U);

        writecommand(ILI9341_ENABLE_3G);
        writedata(0x00U);

        writecommand(ILI9341_GAMMA_SET);
        writedata(0x01U);

        writecommand(ILI9341_POS_GAMMA_CORRECT);
        writedata(0x0FU);
        writedata(0x31U);
        writedata(0x2BU);
        writedata(0x0CU);
        writedata(0x0EU);
        writedata(0x08U);
        writedata(0x4EU);
        writedata(0xF1U);
        writedata(0x37U);
        writedata(0x07U);
        writedata(0x10U);
        writedata(0x03U);
        writedata(0x0EU);
        writedata(0x09U);
        writedata(0x00U);

        writecommand(ILI9341_NEG_GAMMA_CORRECT);
        writedata(0x00U);
        writedata(0x0EU);
        writedata(0x14U);
        writedata(0x03U);
        writedata(0x11U);
        writedata(0x07U);
        writedata(0x31U);
        writedata(0xC1U);
        writedata(0x48U);
        writedata(0x08U);
        writedata(0x0FU);
        writedata(0x0CU);
        writedata(0x31U);
        writedata(0x36U);
        writedata(0x0FU);

        writecommand(ILI9341_FRM_CTRL_NORM_MODE);
        writedata(0x00U);
        writedata(0x10U);

        writecommand(ILI9341_DISP_INVRS_OFF); // Invert Off
        delay(120);

        writecommand(ILI9341_SLEEP_OUT); // Exit Sleep
        delay(120);

        writecommand(ILI9341_DISP_ON); // Display on
        delay(120);
}

void ILI9341_TFT::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
        CS_ACTIVE;
        setAddr(x0, y0, x1, y1);
        CD_COMMAND;
        write8stream(ILI9341_MEM_WR); // write to RAM
        CD_DATA;
}

void ILI9341_TFT::writemem16(uint16_t d) {
        if(wide) {
                *((volatile uint8_t *)(&TFT_DATA_OUT_MSB)) = d >> 8;
                *((volatile uint8_t *)(&TFT_DATA_OUT_LSB)) = d & 0xffU;
                WR_ACTIVE;
                WR_IDLE;
        } else {
                write8stream(d >> 8);
                write8stream(d & 0xffU);
        }
}

void ILI9341_TFT::writemem16(uint16_t d, uint32_t times) {
        if(wide) {
                *((volatile uint8_t *)(&TFT_DATA_OUT_MSB)) = d >> 8;
                *((volatile uint8_t *)(&TFT_DATA_OUT_LSB)) = d & 0xffU;
                while(times--) {
                        WR_ACTIVE;
                        WR_IDLE;
                }
        } else {
                uint8_t hi = d >> 8;
                uint8_t lo = d & 0xffU;
                while(times--) {
                        write8stream(hi);
                        write8stream(lo);
                }
        }
}

void ILI9341_TFT::pushColor(uint16_t color) {
        //writedata16(color);
        //write16stream(color);
        writemem16(color);
        //write8stream(color >> 8);
        //write8stream(color &0xff);
}

void ILI9341_TFT::drawPixel(int16_t x, int16_t y, uint16_t color) {
        if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;

        setAddrWindow(x, y, x + 1, y + 1);
        writemem16(color);
        //write16stream(color);
        //writedata16(color);
        CS_IDLE;
}

void ILI9341_TFT::readRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *pcolors) {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint16_t c = w * h;
        register uint16_t partpixels;
        register uint16_t pixelA;
        CS_ACTIVE;
        setAddr(x, y, x + w - 1, y + h - 1);
        CD_COMMAND;
        write8stream(ILI9341_MEM_RD);
        CD_DATA;
        setReadDataBus();
        // Read a DUMMY byte but only once
        r = read8();
        if(wide) {
                // g-ram can only be read in 666 mode, lovely.
                while(c) {
                        partpixels = read16(); // RG F8FC
                        pixelA = (partpixels & 0xf800U) | ((partpixels & 0x00fcU) << 3);
                        partpixels = read16(); // BR F8F8
                        *pcolors++ = pixelA | partpixels >> 11;
                        c--;
                        if(c--) {
                                pixelA = (partpixels & 0x00f8) << 8;
                                partpixels = read16(); // GB FCF8
                                *pcolors++ = pixelA | ((partpixels & 0xfc00U) >> 5) | ((partpixels & 0xf8U)) >> 3;
                        }
                }
        } else {
                while(c--) {
                        r = read8();
                        g = read8();
                        b = read8();
                        *pcolors++ = color565(r, g, b);
                }
        }
        CS_IDLE;
        setWriteDataBus();
}

void ILI9341_TFT::writeRect(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *pcolors) {
        uint16_t color;
        CS_ACTIVE;
        setAddr(x, y, x + w - 1, y + h - 1);
        CD_COMMAND;
        write8stream(ILI9341_MEM_WR);
        CD_DATA;
        for(y = h; y > 0; y--) {
                for(x = w; x > 1; x--) {
                        color = *pcolors++;
                        writemem16(color);
                        //write8stream(color >> 8);
                        //write8stream(color & 0xffU);
                }
                color = *pcolors++;
                writemem16(color);
                //write8stream(color >> 8);
                //write8stream(color & 0xffU);

        }
        CS_IDLE;
}

void ILI9341_TFT::writeRect8BPP(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *pixels, const uint16_t * palette) {
        uint16_t color;
        CS_ACTIVE;
        setAddr(x, y, x + w - 1, y + h - 1);
        CD_COMMAND;
        write8stream(ILI9341_MEM_WR);
        CD_DATA;
        for(y = h; y > 0; y--) {
                for(x = w; x > 1; x--) {
                        color = palette[*pixels++];
                        writemem16(color);
                        //write8stream(color >> 8);
                        //write8stream(color & 0xff);
                }
                color = palette[*pixels++];
                writemem16(color);
                //write8stream(color >> 8);
                //write8stream(color & 0xff);

        }
        CS_IDLE;
}

void ILI9341_TFT::writeRect4BPP(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *pixels, const uint16_t * palette) {
        uint16_t color;
        CS_ACTIVE;
        setAddr(x, y, x + w - 1, y + h - 1);
        CD_COMMAND;
        write8stream(ILI9341_MEM_WR);
        CD_DATA;
        for(y = h; y > 0; y--) {
                for(x = w; x > 2; x -= 2) {

                        color = palette[((*pixels) >> 4)&0x0f];
                        writemem16(color);
                        //write8stream(color >> 8);
                        //write8stream(color & 0xff);
                        color = palette[(*pixels++)&0x0f];
                        writemem16(color);
                        //write8stream(color >> 8);
                        //write8stream(color & 0xff);

                }
                color = palette[((*pixels) >> 4)&0x0f];
                writemem16(color);
                //write8stream(color >> 8);
                //write8stream(color & 0xff);
                color = palette[(*pixels++)&0x0f];
                writemem16(color);
                //write8stream(color >> 8);
                //write8stream(color & 0xff);

        }
        CS_IDLE;
}

void ILI9341_TFT::writeRect2BPP(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *pixels, const uint16_t * palette) {
        uint16_t color;
        CS_ACTIVE;
        setAddr(x, y, x + w - 1, y + h - 1);
        CD_COMMAND;
        write8stream(ILI9341_MEM_WR);
        CD_DATA;
        for(y = h; y > 0; y--) {
                for(x = w; x > 4; x -= 4) {

                        color = palette[((*pixels) >> 6)&0x03];
                        writemem16(color);
                        //write8stream(color >> 8);
                        //write8stream(color & 0xff);
                        color = palette[((*pixels) >> 4)&0x03];
                        writemem16(color);
                        //write8stream(color >> 8);
                        //write8stream(color & 0xff);
                        color = palette[((*pixels) >> 2)&0x03];
                        writemem16(color);
                        //write8stream(color >> 8);
                        //write8stream(color & 0xff);
                        color = palette[(*pixels++)&0x03];
                        writemem16(color);
                        //write8stream(color >> 8);
                        //write8stream(color & 0xff);

                }
                color = palette[((*pixels) >> 6)&0x03];
                writemem16(color);
                //write8stream(color >> 8);
                //write8stream(color & 0xff);
                color = palette[((*pixels) >> 4)&0x03];
                writemem16(color);
                //write8stream(color >> 8);
                //write8stream(color & 0xff);
                color = palette[((*pixels) >> 2)&0x03];
                writemem16(color);
                //write8stream(color >> 8);
                //write8stream(color & 0xff);
                color = palette[(*pixels++)&0x03];
                writemem16(color);
                //write8stream(color >> 8);
                //write8stream(color & 0xff);

        }
        CS_IDLE;
}

void ILI9341_TFT::writeRect1BPP(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *pixels, const uint16_t * palette) {
        uint16_t color;
        CS_ACTIVE;
        setAddr(x, y, x + w - 1, y + h - 1);
        CD_COMMAND;
        write8stream(ILI9341_MEM_WR);
        CD_DATA;
        for(y = h; y > 0; y--) {
                for(x = w; x > 8; x -= 8) {

                        color = palette[((*pixels) >> 7)&0x01];
                        writemem16(color);
                        //write8stream(color >> 8);
                        //write8stream(color & 0xff);
                        color = palette[((*pixels) >> 6)&0x01];
                        writemem16(color);
                        //write8stream(color >> 8);
                        //write8stream(color & 0xff);
                        color = palette[((*pixels) >> 5)&0x01];
                        writemem16(color);
                        //write8stream(color >> 8);
                        //write8stream(color & 0xff);
                        color = palette[((*pixels) >> 4)&0x01];
                        writemem16(color);
                        //write8stream(color >> 8);
                        //write8stream(color & 0xff);
                        color = palette[((*pixels) >> 3)&0x01];
                        writemem16(color);
                        //write8stream(color >> 8);
                        //write8stream(color & 0xff);
                        color = palette[((*pixels) >> 2)&0x01];
                        writemem16(color);
                        //write8stream(color >> 8);
                        //write8stream(color & 0xff);
                        color = palette[((*pixels) >> 1)&0x01];
                        writemem16(color);
                        //write8stream(color >> 8);
                        //write8stream(color & 0xff);
                        color = palette[(*pixels++)&0x01];
                        writemem16(color);
                        //write8stream(color >> 8);
                        //write8stream(color & 0xff);
                }
                color = palette[((*pixels) >> 7)&0x01];
                writemem16(color);
                //write8stream(color >> 8);
                //write8stream(color & 0xff);
                color = palette[((*pixels) >> 6)&0x01];
                writemem16(color);
                //write8stream(color >> 8);
                //write8stream(color & 0xff);
                color = palette[((*pixels) >> 5)&0x01];
                writemem16(color);
                //write8stream(color >> 8);
                //write8stream(color & 0xff);
                color = palette[((*pixels) >> 4)&0x01];
                writemem16(color);
                //write8stream(color >> 8);
                //write8stream(color & 0xff);
                color = palette[((*pixels) >> 3)&0x01];
                writemem16(color);
                //write8stream(color >> 8);
                //write8stream(color & 0xff);
                color = palette[((*pixels) >> 2)&0x01];
                writemem16(color);
                //write8stream(color >> 8);
                //write8stream(color & 0xff);
                color = palette[((*pixels) >> 1)&0x01];
                writemem16(color);
                //write8stream(color >> 8);
                //write8stream(color & 0xff);
                color = palette[(*pixels++)&0x01];
                writemem16(color);
                //write8stream(color >> 8);
                //write8stream(color & 0xff);

        }
        CS_IDLE;
}

void ILI9341_TFT::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
        int16_t f = 1 - r;
        int16_t ddF_x = 1;
        int16_t ddF_y = -2 * r;
        int16_t x = 0;
        int16_t y = r;

        drawPixel(x0, y0 + r, color);
        drawPixel(x0, y0 - r, color);
        drawPixel(x0 + r, y0, color);
        drawPixel(x0 - r, y0, color);

        while(x < y) {
                if(f >= 0) {
                        y--;
                        ddF_y += 2;
                        f += ddF_y;
                }
                x++;
                ddF_x += 2;
                f += ddF_x;

                drawPixel(x0 + x, y0 + y, color);
                drawPixel(x0 - x, y0 + y, color);
                drawPixel(x0 + x, y0 - y, color);
                drawPixel(x0 - x, y0 - y, color);
                drawPixel(x0 + y, y0 + x, color);
                drawPixel(x0 - y, y0 + x, color);
                drawPixel(x0 + y, y0 - x, color);
                drawPixel(x0 - y, y0 - x, color);
        }
}

void ILI9341_TFT::drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
        int16_t f = 1 - r;
        int16_t ddF_x = 1;
        int16_t ddF_y = -2 * r;
        int16_t x = 0;
        int16_t y = r;
        int xold;

        xold = x;
        while(x < y) {
                if(f >= 0) {
                        y--;
                        ddF_y += 2;
                        f += ddF_y;
                }
                x++;
                ddF_x += 2;
                f += ddF_x;
                if(f >= 0 || x == y) { // time to draw the new line segment
                        if(cornername & 0x4) {
                                drawFastHLine(x0 + xold + 1, y0 + y, x - xold, color);
                                drawFastVLine(x0 + y, y0 + xold + 1, x - xold, color);
                        }
                        if(cornername & 0x2) {
                                drawFastHLine(x0 + xold + 1, y0 - y, x - xold, color);
                                drawFastVLine(x0 + y, y0 - x, x - xold, color);
                        }
                        if(cornername & 0x8) {
                                drawFastVLine(x0 - y, y0 + xold + 1, x - xold, color);
                                drawFastHLine(x0 - x, y0 + y, x - xold, color);
                        }
                        if(cornername & 0x1) {
                                drawFastVLine(x0 - y, y0 - x, x - xold, color);
                                drawFastHLine(x0 - x, y0 - y, x - xold, color);
                        }
                        xold = x;
                } // draw new line segment
        }
}

void ILI9341_TFT::fillCircle(int16_t x0, int16_t y0, int16_t r,
        uint16_t color) {
        drawFastVLine(x0, y0 - r, 2 * r + 1, color);
        fillCircleHelper(x0, y0, r, 3, 0, color);
}

void ILI9341_TFT::fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
        uint8_t cornername, int16_t delta, uint16_t color) {

        int16_t f = 1 - r;
        int16_t ddF_x = 1;
        int16_t ddF_y = -2 * r;
        int16_t x = 0;
        int16_t y = r;

        while(x < y) {
                if(f >= 0) {
                        y--;
                        ddF_y += 2;
                        f += ddF_y;
                }
                x++;
                ddF_x += 2;
                f += ddF_x;

                if(cornername & 0x1) {
                        drawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
                        drawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
                }
                if(cornername & 0x2) {
                        drawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
                        drawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
                }
        }
}

void ILI9341_TFT::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
        if(y0 == y1) {
                if(x1 > x0) {
                        drawFastHLine(x0, y0, x1 - x0 + 1, color);
                } else if(x1 < x0) {
                        drawFastHLine(x1, y0, x0 - x1 + 1, color);
                } else {
                        drawPixel(x0, y0, color);
                }
                return;
        } else if(x0 == x1) {
                if(y1 > y0) {
                        drawFastVLine(x0, y0, y1 - y0 + 1, color);
                } else {
                        drawFastVLine(x0, y1, y0 - y1 + 1, color);
                }
                return;
        }

        bool steep = abs(y1 - y0) > abs(x1 - x0);
        if(steep) {
                swap(x0, y0);
                swap(x1, y1);
        }
        if(x0 > x1) {
                swap(x0, x1);
                swap(y0, y1);
        }

        int16_t dx, dy;
        dx = x1 - x0;
        dy = abs(y1 - y0);

        int16_t err = dx / 2;
        int16_t ystep;

        if(y0 < y1) {
                ystep = 1;
        } else {
                ystep = -1;
        }

        CS_ACTIVE;

        int16_t xbegin = x0;
        if(steep) {
                for(; x0 <= x1; x0++) {
                        err -= dy;
                        if(err < 0) {
                                int16_t len = x0 - xbegin;
                                if(len) {
                                        VLine(y0, xbegin, len + 1, color);
                                } else {
                                        Pixel(y0, x0, color);
                                }
                                xbegin = x0 + 1;
                                y0 += ystep;
                                err += dx;
                        }
                }
                if(x0 > xbegin + 1) {
                        VLine(y0, xbegin, x0 - xbegin, color);
                }
        } else {
                for(; x0 <= x1; x0++) {
                        err -= dy;
                        if(err < 0) {
                                int16_t len = x0 - xbegin;
                                if(len) {
                                        HLine(xbegin, y0, len + 1, color);
                                } else {
                                        Pixel(x0, y0, color);
                                }
                                xbegin = x0 + 1;
                                y0 += ystep;
                                err += dx;
                        }
                }
                if(x0 > xbegin + 1) {
                        HLine(xbegin, y0, x0 - xbegin, color);
                }
        }

        //CD_COMMAND;
        //write8stream(ILI9341_NOP);
        CS_IDLE;
}

void ILI9341_TFT::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
        // Rudimentary clipping

        if((x >= _width) || (y >= _height || h < 1)) return;

        if((y + h - 1) >= _height) h = _height - y;

        if(h < 2) {
                drawPixel(x, y, color);
                return;
        }


        setAddrWindow(x, y, x, y + h - 1);

        CD_DATA;

        //uint8_t hi = color >> 8, lo = color;
        writemem16(color, h);
        //while(h--) {
        //        writemem16(color);
        //write8stream(hi);
        //write8stream(lo);
        //}
        CS_IDLE;
}

void ILI9341_TFT::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
        // Rudimentary clipping

        if((x >= _width) || (y >= _height || w < 1)) return;

        if((x + w - 1) >= _width) w = _width - x;

        if(w < 2) {
                drawPixel(x, y, color);

                return;
        }

        setAddrWindow(x, y, x + w - 1, y);

        CD_DATA;

        //uint8_t hi = color >> 8, lo = color;
        writemem16(color, w);

        //while(w--) {
        //        writemem16(color);
        //write8stream(hi);
        //write8stream(lo);
        //}

        CS_IDLE;
}

void ILI9341_TFT::fillScreen(uint16_t color) {
        fillRect(0, 0, _width, _height, color);
}

// fill a rectangle

void ILI9341_TFT::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        // rudimentary clipping (drawChar w/big text requires this)

        if((x >= _width) || (y >= _height || h < 1 || w < 1)) return;

        if((x + w - 1) >= _width) w = _width - x;

        if((y + h - 1) >= _height) h = _height - y;

        if(w == 1 && h == 1) {
                drawPixel(x, y, color);

                return;
        }

        setAddrWindow(x, y, x + w - 1, y + h - 1);

        CD_DATA;

        uint32_t times = h*w;
        writemem16(color, times);
        CS_IDLE;
}

void ILI9341_TFT::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        CS_ACTIVE;
        HLine(x, y, w, color);
        HLine(x, y + h - 1, w, color);
        VLine(x, y, h, color);
        VLine(x + w - 1, y, h, color);
        //CD_COMMAND;
        //write8stream(ILI9341_NOP);
        CS_IDLE;

}

void ILI9341_TFT::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
        // smarter version
        drawFastHLine(x + r, y, w - 2 * r, color); // Top
        drawFastHLine(x + r, y + h - 1, w - 2 * r, color); // Bottom
        drawFastVLine(x, y + r, h - 2 * r, color); // Left
        drawFastVLine(x + w - 1, y + r, h - 2 * r, color); // Right
        // draw four corners
        drawCircleHelper(x + r, y + r, r, 1, color);
        drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
        drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
        drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
}

void ILI9341_TFT::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
        // smarter version
        fillRect(x + r, y, w - 2 * r, h, color);

        // draw four corners
        fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
        fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
}

// Draw a triangle

void ILI9341_TFT::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
        drawLine(x0, y0, x1, y1, color);
        drawLine(x1, y1, x2, y2, color);
        drawLine(x2, y2, x0, y0, color);
}

// Fill a triangle

void ILI9341_TFT::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

        int16_t a, b, y, last;

        // Sort coordinates by Y order (y2 >= y1 >= y0)
        if(y0 > y1) {
                swap(y0, y1);
                swap(x0, x1);
        }
        if(y1 > y2) {
                swap(y2, y1);
                swap(x2, x1);
        }
        if(y0 > y1) {
                swap(y0, y1);
                swap(x0, x1);
        }

        if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
                a = b = x0;
                if(x1 < a) a = x1;
                else if(x1 > b) b = x1;
                if(x2 < a) a = x2;
                else if(x2 > b) b = x2;
                drawFastHLine(a, y0, b - a + 1, color);
                return;
        }

        int32_t
        dx01 = x1 - x0,
                dy01 = y1 - y0,
                dx02 = x2 - x0,
                dy02 = y2 - y0,
                dx12 = x2 - x1,
                dy12 = y2 - y1,
                sa = 0,
                sb = 0;

        // For upper part of triangle, find scanline crossings for segments
        // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
        // is included here (and second loop will be skipped, avoiding a /0
        // error there), otherwise scanline y1 is skipped here and handled
        // in the second loop...which also avoids a /0 error here if y0=y1
        // (flat-topped triangle).
        if(y1 == y2) last = y1; // Include y1 scanline
        else last = y1 - 1; // Skip it

        for(y = y0; y <= last; y++) {
                a = x0 + sa / dy01;
                b = x0 + sb / dy02;
                sa += dx01;
                sb += dx02;
                /* longhand:
                a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
                b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
                 */
                if(a > b) swap(a, b);
                drawFastHLine(a, y, b - a + 1, color);
        }

        // For lower part of triangle, find scanline crossings for segments
        // 0-2 and 1-2.  This loop is skipped if y1=y2.
        sa = dx12 * (y - y1);
        sb = dx02 * (y - y0);
        for(; y <= y2; y++) {
                a = x1 + sa / dy12;
                b = x0 + sb / dy02;
                sa += dx12;
                sb += dx02;
                /* longhand:
                a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
                b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
                 */
                if(a > b) swap(a, b);
                drawFastHLine(a, y, b - a + 1, color);
        }
}

void ILI9341_TFT::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

        int16_t i, j, byteWidth = (w + 7) / 8;

        for(j = 0; j < h; j++) {
                for(i = 0; i < w; i++) {
                        if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
                                drawPixel(x + i, y + j, color);
                        }
                }
        }
}

void ILI9341_TFT::setRotation(uint8_t m) {
        writecommand(ILI9341_MEM_ADDR_CTL);

        rotation = m % 4; // can't be higher than 3

        switch(rotation) {
                case 0:

                        writedata(MADCTL_MX | MADCTL_BGR);

                        _width = ILI9341_TFTWIDTH;

                        _height = ILI9341_TFTHEIGHT;

                        break;

                case 1:

                        writedata(MADCTL_MV | MADCTL_BGR);

                        _width = ILI9341_TFTHEIGHT;

                        _height = ILI9341_TFTWIDTH;

                        break;

                case 2:

                        writedata(MADCTL_MY | MADCTL_BGR);

                        _width = ILI9341_TFTWIDTH;

                        _height = ILI9341_TFTHEIGHT;

                        break;

                case 3:

                        writedata(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);

                        _width = ILI9341_TFTHEIGHT;

                        _height = ILI9341_TFTWIDTH;

                        break;
        }
}

void ILI9341_TFT::setScroll(uint16_t offset) {
        CS_ACTIVE;
        CD_COMMAND;
        write8stream(ILI9341_VERT_SCROL_START_ADDR);
        CD_DATA;
        write8stream(offset >> 8);
        write8stream(offset & 0xFF);
        CS_IDLE;
}

void ILI9341_TFT::invertDisplay(boolean i) {
        writecommand(i ? ILI9341_DISP_INVRS_ON : ILI9341_DISP_INVRS_OFF);
}

size_t ILI9341_TFT::write(uint8_t c) {
        if(font) {
                if(c == '\n') {
                        cursor_y += font->line_space; // Fix linefeed. Added by T.T., SoftEgg
                        cursor_x = 0;
                } else {
                        drawFontChar(c);
                }
        } else {
                if(c == '\n') {
                        cursor_y += textsize * 8;
                        cursor_x = 0;
                } else if(c == '\r') {
                        // skip em
                } else {
                        drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
                        cursor_x += textsize * 6;
                        if(wrap && (cursor_x > (_width - textsize * 6))) {
                                cursor_y += textsize * 8;
                                cursor_x = 0;
                        }
                }
        }
        return 1;
}



// Draw a character

void ILI9341_TFT::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t fgcolor, uint16_t bgcolor, uint8_t size) {
        if((x >= _width) || // Clip right
                (y >= _height) || // Clip bottom
                ((x + 6 * size - 1) < 0) || // Clip left  TODO: is this correct?
                ((y + 8 * size - 1) < 0)) // Clip top   TODO: is this correct?
                return;
#if 0
        if(fgcolor == bgcolor) {
#endif
                int16_t xoff, yoff, count;
                uint8_t mask = 0x1;
                uint8_t *s = (uint8_t *) & glcdfont[c * 5];
                for(yoff = 0; yoff < 8; yoff++) {
                        xoff = 0;
                        while(xoff < 5) {
                                count = 0;
                                while(xoff < 5 && !(s[xoff] & mask)) { // bg
                                        count++;
                                        xoff++;
                                }
                                if((fgcolor != bgcolor) && count) {
                                        if(size == 1) {
                                                drawFastHLine(x + xoff - count, y + yoff, count, bgcolor);
                                        } else {
                                                fillRect(x + (xoff - count) * size, y + yoff * size, count * size, size, bgcolor);
                                        }
                                }
                                count = 0;
                                while(xoff < 5 && s[xoff] & mask) { // fg
                                        count++;
                                        xoff++;
                                }
                                if(count) {
                                        if(size == 1) {
                                                drawFastHLine(x + xoff - count, y + yoff, count, fgcolor);
                                        } else {
                                                fillRect(x + (xoff - count) * size, y + yoff * size, count * size, size, fgcolor);
                                        }
                                }
                        } // while xoff
                        mask = mask << 1;
                } // for y
#if 0
        } else {
                // This solid background approach is about 5 time faster
                CS_ACTIVE;
                setAddr(x, y, x + 6 * size - 1, y + 8 * size - 1);
                CS_ACTIVE;
                CD_COMMAND;
                write8stream(ILI9341_MEM_WR);
                uint8_t yr;
                uint8_t mask = 0x01;
                uint16_t color;
                for(y = 0; y < 8; y++) {
                        for(yr = 0; yr < size; yr++) {
                                for(x = 0; x < 5; x++) {
                                        color = (glcdfont[c * 5 + x] & mask) ? fgcolor : bgcolor;
                                        writemem16(color, size);
                                }
                                writemem16(bgcolor, size);
                        }
                        mask = mask << 1;
                }
                //CD_COMMAND;
                //write8stream(ILI9341_NOP);
                CS_IDLE;
        }
#endif
}

static inline uint32_t fetchbit(const uint8_t *p, uint32_t index) {
        return (p[index >> 3] & (0x80 >> (index & 7)));
}

static uint32_t fetchbits_unsigned(const uint8_t *p, uint32_t index, uint32_t required) {
        uint32_t val;
        uint8_t *s = (uint8_t *) & p[index >> 3];

#ifdef UNALIGNED_IS_SAFE
        val = *(uint32_t *)s; // read 4 bytes - unaligned is ok
        val = __builtin_bswap32(val); // change to big-endian order
#else
        val = s[0] << 24;
        val |= (s[1] << 16);
        val |= (s[2] << 8);
        val |= s[3];
#endif
        val <<= (index & 7); // shift out used bits
        if(32 - (index & 7) < required) { // need to get more bits
                val |= (s[4] >> (8 - (index & 7)));
        }
        val >>= (32 - required); // right align the bits
        return val;
}

static uint32_t fetchbits_signed(const uint8_t *p, uint32_t index, uint32_t required) {
        uint32_t val = fetchbits_unsigned(p, index, required);
        if(val & (1 << (required - 1))) {
                return (int32_t)val - (1 << required);
        }
        return (int32_t)val;
}

// Measure the dimensions for a single character

void ILI9341_TFT::measureChar(unsigned char c, uint16_t* w, uint16_t* h) {
        // Treat non-breaking space as normal space
        if(c == 0xa0) {
                c = ' ';
        }

        // Is current font a T3 font or the default Adafruit-GFX font?
        if(font) {
                // ILI9341_T3 font
                *h = font->cap_height;
                *w = 0;

                uint32_t bitoffset;
                const uint8_t *data;

                if(c >= font->index1_first && c <= font->index1_last) {
                        bitoffset = c - font->index1_first;
                        bitoffset *= font->bits_index;
                } else if(c >= font->index2_first && c <= font->index2_last) {
                        bitoffset = c - font->index2_first + font->index1_last - font->index1_first + 1;
                        bitoffset *= font->bits_index;
                } else if(font->unicode) {
                        return; // TODO: implement sparse unicode
                } else {
                        return;
                }

                data = font->data + fetchbits_unsigned(font->index, bitoffset, font->bits_index);

                uint32_t encoding = fetchbits_unsigned(data, 0, 3);

                if(encoding != 0) return;

                //uint32_t width =
                fetchbits_unsigned(data, 3, font->bits_width);
                bitoffset = font->bits_width + 3;

                //uint32_t height =
                fetchbits_unsigned(data, bitoffset, font->bits_height);
                bitoffset += font->bits_height;

                //int32_t xoffset =
                fetchbits_signed(data, bitoffset, font->bits_xoffset);
                bitoffset += font->bits_xoffset;

                //int32_t yoffset =
                fetchbits_signed(data, bitoffset, font->bits_yoffset);
                bitoffset += font->bits_yoffset;

                uint32_t delta = fetchbits_unsigned(data, bitoffset, font->bits_delta);
                *w = delta;
        } else {
                // Adafruit-GFX default font has fixed 6x8 dimensions
                *w = 6 * textsize;
                *h = 8 * textsize;
        }

}

void ILI9341_TFT::drawFontChar(unsigned int c) {
        uint32_t bitoffset;
        const uint8_t *data;

        //Serial.printf("drawFontChar %d\n", c);

        if(c >= font->index1_first && c <= font->index1_last) {
                bitoffset = c - font->index1_first;
                bitoffset *= font->bits_index;
        } else if(c >= font->index2_first && c <= font->index2_last) {
                bitoffset = c - font->index2_first + font->index1_last - font->index1_first + 1;
                bitoffset *= font->bits_index;
        } else if(font->unicode) {
                return; // TODO: implement sparse unicode
        } else {
                return;
        }
        //Serial.printf("  index =  %d\n", fetchbits_unsigned(font->index, bitoffset, font->bits_index));
        data = font->data + fetchbits_unsigned(font->index, bitoffset, font->bits_index);

        uint32_t encoding = fetchbits_unsigned(data, 0, 3);
        if(encoding != 0) return;
        uint32_t width = fetchbits_unsigned(data, 3, font->bits_width);
        bitoffset = font->bits_width + 3;
        uint32_t height = fetchbits_unsigned(data, bitoffset, font->bits_height);
        bitoffset += font->bits_height;
        //Serial.printf("  size =   %d,%d\n", width, height);

        int32_t xoffset = fetchbits_signed(data, bitoffset, font->bits_xoffset);
        bitoffset += font->bits_xoffset;
        int32_t yoffset = fetchbits_signed(data, bitoffset, font->bits_yoffset);
        bitoffset += font->bits_yoffset;
        //Serial.printf("  offset = %d,%d\n", xoffset, yoffset);

        uint32_t delta = fetchbits_unsigned(data, bitoffset, font->bits_delta);
        bitoffset += font->bits_delta;
        //Serial.printf("  delta =  %d\n", delta);

        //Serial.printf("  cursor = %d,%d\n", cursor_x, cursor_y);

        // horizontally, we draw every pixel, or none at all
        if(cursor_x < 0) cursor_x = 0;
        int32_t origin_x = cursor_x + xoffset;
        if(origin_x < 0) {
                cursor_x -= xoffset;
                origin_x = 0;
        }
        if(origin_x + (int)width > _width) {
                if(!wrap) return;
                origin_x = 0;
                if(xoffset >= 0) {
                        cursor_x = 0;
                } else {
                        cursor_x = -xoffset;
                }
                cursor_y += font->line_space;
        }
        if(cursor_y >= _height) return;
        cursor_x += delta;

        // vertically, the top and/or bottom can be clipped
        int32_t origin_y = cursor_y + font->cap_height - height - yoffset;
        //Serial.printf("  origin = %d,%d\n", origin_x, origin_y);

        // TODO: compute top skip and number of lines
        int32_t linecount = height;
        //uint32_t loopcount = 0;
        uint32_t y = origin_y;
        while(linecount) {
                //Serial.printf("    linecount = %d\n", linecount);
                uint32_t b = fetchbit(data, bitoffset++);
                if(b == 0) {
                        //Serial.println("    single line");
                        uint32_t x = 0;
                        do {
                                uint32_t xsize = width - x;
                                if(xsize > 32) xsize = 32;
                                uint32_t bits = fetchbits_unsigned(data, bitoffset, xsize);
                                drawFontBits(bits, xsize, origin_x + x, y, 1);
                                bitoffset += xsize;
                                x += xsize;
                        } while(x < width);
                        y++;
                        linecount--;
                } else {
                        uint32_t n = fetchbits_unsigned(data, bitoffset, 3) + 2;
                        bitoffset += 3;
                        uint32_t x = 0;
                        do {
                                uint32_t xsize = width - x;
                                if(xsize > 32) xsize = 32;
                                //Serial.printf("    multi line %d\n", n);
                                uint32_t bits = fetchbits_unsigned(data, bitoffset, xsize);
                                drawFontBits(bits, xsize, origin_x + x, y, n);
                                bitoffset += xsize;
                                x += xsize;
                        } while(x < width);
                        y += n;
                        linecount -= n;
                }
        }
}

//strPixelLen                   - gets pixel length of given ASCII string

int16_t ILI9341_TFT::strPixelLen(char * str) {
        //      Serial.printf("strPixelLen %s\n", str);
        if(!str) return (0);
        uint16_t len = 0, maxlen = 0;
        while(*str) {
                if(*str == '\n') {
                        if(len > maxlen) {
                                maxlen = len;
                                len = 0;
                        }
                } else {
                        if(!font) {
                                len += textsize * 6;
                        } else {

                                uint32_t bitoffset;
                                const uint8_t *data;
                                uint16_t c = *str;

                                //                              Serial.printf("char %c(%d)\n", c,c);

                                if(c >= font->index1_first && c <= font->index1_last) {
                                        bitoffset = c - font->index1_first;
                                        bitoffset *= font->bits_index;
                                } else if(c >= font->index2_first && c <= font->index2_last) {
                                        bitoffset = c - font->index2_first + font->index1_last - font->index1_first + 1;
                                        bitoffset *= font->bits_index;
                                } else if(font->unicode) {
                                        continue;
                                } else {
                                        continue;
                                }
                                //Serial.printf("  index =  %d\n", fetchbits_unsigned(font->index, bitoffset, font->bits_index));
                                data = font->data + fetchbits_unsigned(font->index, bitoffset, font->bits_index);

                                uint32_t encoding = fetchbits_unsigned(data, 0, 3);
                                if(encoding != 0) continue;
                                //                              uint32_t width = fetchbits_unsigned(data, 3, font->bits_width);
                                //                              Serial.printf("  width =  %d\n", width);
                                bitoffset = font->bits_width + 3;
                                bitoffset += font->bits_height;

                                //                              int32_t xoffset = fetchbits_signed(data, bitoffset, font->bits_xoffset);
                                //                              Serial.printf("  xoffset =  %d\n", xoffset);
                                bitoffset += font->bits_xoffset;
                                bitoffset += font->bits_yoffset;

                                uint32_t delta = fetchbits_unsigned(data, bitoffset, font->bits_delta);
                                bitoffset += font->bits_delta;
                                //                              Serial.printf("  delta =  %d\n", delta);

                                len += delta; //+width-xoffset;
                                //                              Serial.printf("  len =  %d\n", len);
                                if(len > maxlen) {
                                        maxlen = len;
                                        //                                      Serial.printf("  maxlen =  %d\n", maxlen);
                                }

                        }
                }
                str++;
        }
        //      Serial.printf("Return  maxlen =  %d\n", maxlen);
        return ( maxlen);
}

void ILI9341_TFT::drawFontBits(uint32_t bits, uint32_t numbits, uint32_t x, uint32_t y, uint32_t repeat) {
        if(bits == 0) return;
        CS_ACTIVE;
        uint32_t w;
        bits <<= (32 - numbits); // left align bits
        do {
                w = __builtin_clz(bits); // skip over zeros
                if(w > numbits) w = numbits;
                numbits -= w;
                x += w;
                bits <<= w;
                bits = ~bits; // invert to count 1s as 0s
                w = __builtin_clz(bits);
                if(w > numbits) w = numbits;
                numbits -= w;
                bits <<= w;
                bits = ~bits; // invert back to original polarity
                if(w > 0) {
                        x += w;
                        setAddr(x - w, y, x - 1, y + repeat - 1); // write a block of pixels w x repeat sized
                        CD_COMMAND;
                        write8stream(ILI9341_MEM_WR);
                        CD_DATA;

                        w *= repeat;
                        writemem16(textcolor, w);
                }
        } while(numbits > 0);
        CS_IDLE;

}

void ILI9341_TFT::setCursor(int16_t x, int16_t y) {
        if(x < 0) x = 0;
        else if(x >= _width) x = _width - 1;
        cursor_x = x;
        if(y < 0) y = 0;
        else if(y >= _height) y = _height - 1;
        cursor_y = y;
}

void ILI9341_TFT::getCursor(int16_t *x, int16_t *y) {
        *x = cursor_x;
        *y = cursor_y;
}

void ILI9341_TFT::setTextSize(uint8_t s) {
        textsize = (s > 0) ? s : 1;
}

uint8_t ILI9341_TFT::getTextSize() {
        return textsize;
}

void ILI9341_TFT::setTextColor(uint16_t c) {
        // For 'transparent' background, we'll set the bg
        // to the same as fg instead of using a flag
        textcolor = textbgcolor = c;
}

void ILI9341_TFT::setTextColor(uint16_t c, uint16_t b) {
        textcolor = c;
        textbgcolor = b;
}

void ILI9341_TFT::setTextWrap(boolean w) {
        wrap = w;
}

boolean ILI9341_TFT::getTextWrap() {
        return wrap;
}

// Return the width of a text string
// - num =  max characters to process, or 0 for entire string (null-terminated)

uint16_t ILI9341_TFT::measureTextWidth(const char* text, int num) {
        uint16_t maxH = 0;
        uint16_t currH = 0;
        uint16_t n = num;

        if(n == 0) {
                n = strlen(text);
        };

        for(int i = 0; i < n; i++) {
                if(text[i] == '\n') {
                        // For multi-line strings, retain max width
                        if(currH > maxH)
                                maxH = currH;
                        currH = 0;
                } else {
                        uint16_t h, w;
                        measureChar(text[i], &w, &h);
                        currH += w;
                }
        }
        uint16_t h = maxH > currH ? maxH : currH;
        return h;
}

// Return the height of a text string
// - num =  max characters to process, or 0 for entire string (null-terminated)

uint16_t ILI9341_TFT::measureTextHeight(const char* text, int num) {
        int lines = 1;
        uint16_t n = num;
        if(n == 0) {
                n = strlen(text);
        };
        for(int i = 0; i < n; i++) {
                if(text[i] == '\n') {
                        lines++;
                }
        }
        return ((lines - 1) * fontLineSpace() + fontCapHeight());
}

uint16_t ILI9341_TFT::read16(void) {
        uint16_t temp;
        RD_ACTIVE;
        delayNanoseconds(200);
        temp = (uint16_t)(*((volatile uint8_t *)(&TFT_DATA_IN_LSB))) | (uint16_t)((*((volatile uint8_t *)(&TFT_DATA_IN_MSB))) << 8);
        RD_IDLE;
        delayNanoseconds(90);

        return temp;
}

uint8_t ILI9341_TFT::read8(void) {
        RD_ACTIVE;

        delayNanoseconds(200);
        uint8_t temp = (*((volatile uint8_t *)(&TFT_DATA_IN_LSB)));
        delayNanoseconds(200);

        RD_IDLE;
        delayNanoseconds(90);

        return temp;
}

uint32_t ILI9341_TFT::readID(void) {
        writecommand(ILI9341_RD_DISP_ID4);
        CS_ACTIVE;
        CD_DATA;
        setReadDataBus();

        uint32_t r = read8() << 16;
        r |= read8() << 8;
        r |= read8();
        CS_IDLE;
        setWriteDataBus();

        return r;
}

uint8_t ILI9341_TFT::readcommand8(uint8_t commandByte, uint8_t index) {
        uint8_t data = 0x10 + index;
        writecommand(ILI9341_SET_INDEX_REGISTER);
        CS_ACTIVE;
        CD_DATA;
        write8stream(data);
        writecommand(commandByte);
        CS_ACTIVE;
        CD_DATA;
        setReadDataBus();
        data = read8();
        data = read8();
        CS_IDLE;
        setWriteDataBus();
        return data;
}

void ILI9341_TFT::checkIDs(void) {
        uint8_t hh;
        uint8_t lh;
        uint8_t h;
        uint8_t l;

        writecommand(ILI9341_RD_DISP_ID4);
        CS_ACTIVE;
        CD_DATA;
        setReadDataBus();
        // one faux read
        lh = read8();
        // now the ID
        lh = read8();
        h = read8();
        l = read8();
        CS_IDLE;
        setWriteDataBus();
        Serial.printf("CHIP ID 0x%2.2x 0x%2.2x 0x%2.2x\r\n", lh, h, l);
        writecommand(ILI9341_RD_ID1);
        CS_ACTIVE;
        CD_DATA;
        setReadDataBus();
        hh = read8();
        hh = read8();
        CS_IDLE;

        setWriteDataBus();
        writecommand(ILI9341_RD_ID2);
        CS_ACTIVE;
        CD_DATA;
        setReadDataBus();
        lh = read8();
        lh = read8();
        CS_IDLE;
        setWriteDataBus();
        writecommand(ILI9341_RD_ID3);
        CS_ACTIVE;
        CD_DATA;
        setReadDataBus();
        h = read8();
        h = read8();
        CS_IDLE;
        setWriteDataBus();
        writecommand(ILI9341_RD_ID4);
        CS_ACTIVE;
        CD_DATA;
        setReadDataBus();
        l = read8();
        l = read8();
        CS_IDLE;
        setWriteDataBus();
        Serial.printf("Module Vendor 0x%2.2x\r\nDriver Version 0x%2.2x\r\nDriver ID 0x%2.2x Undocumented ID 0x%2.2x\r\n", hh, lh, h, l);

        uint8_t x = readcommand8(ILI9341_RD_DISP_PWR_MODE);
        Serial.print("Display Power Mode: 0x");
        Serial.println(x, HEX);
        x = readcommand8(ILI9341_RD_DISP_MADCTL);
        Serial.print("MADCTL Mode: 0x");
        Serial.println(x, HEX);
        x = readcommand8(ILI9341_RD_DISP_PIX_FMT);
        Serial.print("Pixel Format: 0x");
        Serial.println(x, HEX);
        x = readcommand8(ILI9341_RD_DISP_IMGFMT);
        Serial.print("Image Format: 0x");
        Serial.println(x, HEX);
        x = readcommand8(ILI9341_RD_SELF_DIAG_RSLT);
        Serial.print("Self Diagnostic: 0x");
        Serial.println(x, HEX);

}
