/*
 *      This library is for the ILI9341 and PJRC Teensy 3.X and LC development boards.
 *
 * Connect the LCD as follows:
 *
 *      LCD PIN | Teensy 3.[012356] and LC
 *--------------+------------------------
 *       TFT_RD | 16
 *       TFT_WR | 17
 *       TFT_RS |  3
 *       TFT_CS |  4
 *       LCD_D0 |  2
 *       LCD_D1 | 14
 *       LCD_D2 |  7
 *       LCD_D3 |  8
 *       LCD_D4 |  6
 *       LCD_D5 | 20
 *       LCD_D6 | 21
 *       LCD_D7 |  5
 *          SDA | 18 touch screen on I2C
 *          SCL | 19 touch screen on I2C
 *---------------------------------------
 * 8bit interface. Any Teensy 3.x and LC
 *---------------------------------------
 *      TFT_RST | 15
 *TFT_BACKLIGHT | 23
 *        T_CLK | 13 touch screen on SPI
 *         T_MI | 12 touch screen on SPI
 *         T_MO | 11 touch screen on SPI
 *         T_CS | 10 touch screen on SPI
 *  T_IRQ (PEN) |  9 touch screen IRQ
 *---------------------------------------
 * 16bit interface. For Teensy 3.[56]
 *---------------------------------------
 *        T_CLK | 27 touch screen on SPI
 *         T_MI | 39 touch screen on SPI
 *         T_MO | 28 touch screen on SPI
 *         T_CS | 26 touch screen on SPI
 *    IRQ (PEN) | 25 touch screen IRQ
 *      TFT_RST | 24
 *    BACKLIGHT | 35
 *       LCD_D8 | 15
 *       LCD_D9 | 22
 *      LCD_D10 | 23
 *      LCD_D11 |  9
 *      LCD_D12 | 10
 *      LCD_D13 | 13
 *      LCD_D14 | 11
 *      LCD_D15 | 12
 *
 *
 *
 * Note: If you need CAN0 on Teensy 3.[1256]
 *      You can reassign TFT_RS and TFT_CS to any unused pins, including those on the bottom of the board.
 *
 */

#ifndef _ILI9341_816_BIT_TEENSY_H_
#define _ILI9341_816_BIT_TEENSY_H_


#ifdef __cplusplus
#include <Arduino.h>
#endif


// All Teensy 3.x and LC
#define TFT_RD 16
#define TFT_WR 17
#define TFT_RS 3
#define TFT_CS 4
//PORTD
#define TFT_D0 2
#define TFT_D1 14
#define TFT_D2 7
#define TFT_D3 8
#define TFT_D4 6
#define TFT_D5 20
#define TFT_D6 21
#define TFT_D7 5
//PORTC (3.[56] only)
#define TFT_D8 15
#define TFT_D9 22
#define TFT_D10 23
#define TFT_D11 9
#define TFT_D12 10
#define TFT_D13 13
#define TFT_D14 11
#define TFT_D15 12

#if defined(KINETISL)
#define TFT_DATA_OUT_LSB FGPIOD_PDOR
#define TFT_DATA_IN_LSB FGPIOD_PDIR
#define TFT_DATA_DDR_LSB FGPIOD_PDDR
#define TFT_DATA_OUT_MSB FGPIOC_PDOR
#define TFT_DATA_IN_MSB FGPIOC_PDIR
#define TFT_DATA_DDR_MSB FGPIOC_PDDR
#else
#define TFT_DATA_OUT_LSB GPIOD_PDOR
#define TFT_DATA_IN_LSB GPIOD_PDIR
#define TFT_DATA_DDR_LSB GPIOD_PDDR

#define TFT_DATA_OUT_MSB GPIOC_PDOR
#define TFT_DATA_IN_MSB GPIOC_PDIR
#define TFT_DATA_DDR_MSB GPIOC_PDDR
#endif

// SCK 13, MO 11, MI 12
#define TOUCHSCREEN_CS_8 10
#define TOUCHSCREEN_IRQ_8 9
#define TFT_RST_8 15
#define TFT_BACKLIGHT_8 23

// SCK 27, MO 28, MI 39
#define TOUCHSCREEN_CS_16 26
#define TOUCHSCREEN_IRQ_16 25
#define TFT_RST_16 24
#define TFT_BACKLIGHT_16 35

#if defined(__MK66FX1M0__) || defined(__MK64FX512__)
#define TFT_CAN_16BIT 1
#else
#define TFT_CAN_16BIT 0
#endif

// data sheet is not consistent, says write cycle is 66ns, but shows it as 30?
#if 1
#define NANOSEXI  1
#define NANOSEXO  7
#else
#define NANOSEXI  1000
#define NANOSEXO  1000
#endif

#define RD_IDLE digitalWriteFast(TFT_RD, HIGH)
#define RD_ACTIVE digitalWriteFast(TFT_RD, LOW)
#define WR_IDLE digitalWriteFast(TFT_WR, HIGH); delayNanoseconds(NANOSEXO)
#define WR_ACTIVE digitalWriteFast(TFT_WR, LOW); delayNanoseconds(NANOSEXO)
#define CD_COMMAND digitalWriteFast(TFT_RS, LOW)
#define CD_DATA digitalWriteFast(TFT_RS, HIGH)
#define CS_IDLE digitalWriteFast(TFT_CS, HIGH)
#define CS_ACTIVE digitalWriteFast(TFT_CS, LOW)


#define ILI9341_TFTWIDTH 240
#define ILI9341_TFTHEIGHT 320

#define ILI9341_NOP 0x00
#define ILI9341_SW_RESET 0x01
#define ILI9341_RD_DISP_ID 0x04
#define ILI9341_RD_DISP_STAT 0x09
#define ILI9341_RD_DISP_PWR_MODE 0x0A
#define ILI9341_RD_DISP_MADCTL 0x0B
#define ILI9341_RD_DISP_PIX_FMT 0x0C
#define ILI9341_RD_DISP_IMGFMT 0x0D
#define ILI9341_RD_DISP_SIG_MODE 0x0E
#define ILI9341_RD_SELF_DIAG_RSLT 0x0F
#define ILI9341_ENTER_SLEEP 0x10
#define ILI9341_SLEEP_OUT 0x11
#define ILI9341_PARTL_ON 0x12
#define ILI9341_NORML_ON 0x13
#define ILI9341_DISP_INVRS_OFF 0x20
#define ILI9341_DISP_INVRS_ON 0x21
#define ILI9341_GAMMA_SET 0x26
#define ILI9341_DISP_OFF 0x28
#define ILI9341_DISP_ON 0x29
#define ILI9341_COL_ADDR_SET 0x2A
#define ILI9341_PAGE_ADDR_SET 0x2B
#define ILI9341_MEM_WR 0x2C
#define ILI9341_COLOR_SET 0x2D
#define ILI9341_MEM_RD 0x2E
#define ILI9341_PARTL_AREA 0x30
#define ILI9341_VERT_SCROL_DEF 0x33
#define ILI9341_TEAR_EFCT_LINE_OFF 0x34
#define ILI9341_TEAR_EFCT_LINE_ON 0x35
#define ILI9341_MEM_ADDR_CTL 0x36
#define ILI9341_VERT_SCROL_START_ADDR 0x37
#define ILI9341_IDLE_MODE_OFF 0x38
#define ILI9341_IDLE_MODE_ON 0x39
#define ILI9341_PIXL_FMT_SET 0x3A
#define ILI9341_WR_MEM_CONT 0x3C
#define ILI9341_RD_MEM_CONT 0x3E
#define ILI9341_SET_TEAR_SCANLN 0x44
#define ILI9341_GET_SCANLN 0x45
#define ILI9341_WR_DISP_BRIGHTNESS 0x51
#define ILI9341_RD_DISP_BRIGHTNESS 0x52
#define ILI9341_WR_CTRL_DISP 0x53
#define ILI9341_RD_CTRL_DISP 0x54
#define ILI9341_WR_CONTNT_ADATV_BRGHT_CTRL 0x55
#define ILI9341_RD_CONTNT_ADATV_BRGHT_CTRL 0x56
#define ILI9341_WR_CABC_MIN_BRGHT 0x5E
#define ILI9341_RD_CABC_MIN_BRGHT 0x5F
#define ILI9341_RGB_IF_SIG_CTRL 0xB0
#define ILI9341_FRM_CTRL_NORM_MODE 0xB1
#define ILI9341_FRM_CTRL_IDLE_MODE 0xB2
#define ILI9341_FRM_CTRL_PARTL_MODE 0xB3
#define ILI9341_DISP_INVERSN_CTRL 0xB4
#define ILI9341_BLANK_PORCH_CTRL 0xB5
#define ILI9341_DISP_FUNT_CTRL 0xB6
#define ILI9341_ENTRY_MODE_SET 0xB7
#define ILI9341_BKLGHT_CTRL_1 0xB8
#define ILI9341_BKLGHT_CTRL_2 0xB9
#define ILI9341_BKLGHT_CTRL_3 0xBA
#define ILI9341_BKLGHT_CTRL_4 0xBB
#define ILI9341_BKLGHT_CTRL_5 0xBC
//#define ILI9341_ 0xBD
#define ILI9341_BKLGHT_CTRL_7 0xBE
#define ILI9341_BKLGHT_CTRL_8 0xBF
#define ILI9341_PWR_CTRL_1 0xC0
#define ILI9341_PWR_CTRL_2 0xC1
#define ILI9341_PWR_CTRL_3 0xC2
#define ILI9341_PWR_CTRL_4 0xC3
#define ILI9341_PWR_CTRL_5 0xC4
#define ILI9341_VCOM_CTRL_1 0xC5
#define ILI9341_VCOM_CTRL_2 0xC7
#define ILI9341_PWR_CTRL_A 0xCB
#define ILI9341_PWR_CTRL_B 0xCF
#define ILI9341_NV_MEM_WR 0xD0
#define ILI9341_NV_MEM_PROT_KEY 0xD1
#define ILI9341_NV_MEM_STAT_RD 0xD2
#define ILI9341_RD_DISP_ID4 0xD3
#define ILI9341_SET_INDEX_REGISTER 0xD9
#define ILI9341_RD_ID1 0xDA
#define ILI9341_RD_ID2 0xDB
#define ILI9341_RD_ID3 0xDC
#define ILI9341_RD_ID4 0xDD
#define ILI9341_POS_GAMMA_CORRECT 0xE0
#define ILI9341_NEG_GAMMA_CORRECT 0xE1
#define ILI9341_DIGTL_GAMMA_CTRL_1 0xE2
#define ILI9341_DIGTL_GAMMA_CTRL_2 0xE3
#define ILI9341_DVR_TIME_CTRL_A1 0xE8
#define ILI9341_DVR_TIME_CTRL_A2 0xE9
#define ILI9341_DVR_TIME_CTRL_B  0xEA
#define ILI9341_PWR_ON_SEQ_CTRL 0xED
#define ILI9341_UNDOC_1 0xEF
#define ILI9341_ENABLE_3G 0xF2
#define ILI9341_IF_CTRL 0xF6
#define ILI9341_PUMP_RATIO_CTRL 0xF7

/*
#define ILI9341_PWCTR6  0xFC
 */

// Color definitions

#define ILI9341_BLACK 0x0000 /*   0,   0,   0 */
#define ILI9341_NAVY 0x000F /*   0,   0, 128 */
#define ILI9341_DARKGREEN 0x03E0 /*   0, 128,   0 */
#define ILI9341_DARKCYAN 0x03EF /*   0, 128, 128 */
#define ILI9341_MAROON 0x7800 /* 128,   0,   0 */
#define ILI9341_PURPLE 0x780F /* 128,   0, 128 */
#define ILI9341_OLIVE 0x7BE0 /* 128, 128,   0 */
#define ILI9341_LIGHTGREY 0xC618 /* 192, 192, 192 */
#define ILI9341_DARKGREY 0x7BEF /* 128, 128, 128 */
#define ILI9341_BLUE 0x001F /*   0,   0, 255 */
#define ILI9341_GREEN 0x07E0 /*   0, 255,   0 */
#define ILI9341_CYAN 0x07FF /*   0, 255, 255 */
#define ILI9341_RED 0xF800 /* 255,   0,   0 */
#define ILI9341_MAGENTA 0xF81F /* 255,   0, 255 */
#define ILI9341_YELLOW 0xFFE0 /* 255, 255,   0 */
#define ILI9341_WHITE 0xFFFF /* 255, 255, 255 */
#define ILI9341_ORANGE 0xFD20 /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5 /* 173, 255,  47 */
#define ILI9341_PINK 0xF81F


#define CL(_r,_g,_b) ((((_r)&0xF8)<<8)|(((_g)&0xFC)<<3)|((_b)>>3))
#define sint16_t int16_t

typedef struct {
        const unsigned char *index;
        const unsigned char *unicode;
        const unsigned char *data;
        unsigned char version;
        unsigned char reserved;
        unsigned char index1_first;
        unsigned char index1_last;
        unsigned char index2_first;
        unsigned char index2_last;
        unsigned char bits_index;
        unsigned char bits_width;
        unsigned char bits_height;
        unsigned char bits_xoffset;
        unsigned char bits_yoffset;
        unsigned char bits_delta;
        unsigned char line_space;
        unsigned char cap_height;
} ILI9341_TFT_font_t;

#ifdef __cplusplus

#include <SPI.h>

class ILI9341_TFT : public Print {
public:
        bool wide;
        int TFT_RST = -1;
        int TFT_BACKLIGHT = -1;
        int touch_CS_SPI = 255;
        int touch_IRQ_SPI = 255;
        int16_t _width, _height; // Display w/h as modified by current rotation
        int16_t cursor_x, cursor_y;
        uint16_t textcolor, textbgcolor;
        uint8_t textsize, rotation;
        boolean wrap; // If set, 'wrap' text at right edge of display
        const ILI9341_TFT_font_t *font;



        ILI9341_TFT(bool fast = false, int rst = -2, int backlight = -2, int t_S = -2, int t_I = -2);

        void begin(void);
        void checkIDs(void);
        void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
        void pushColor(uint16_t color);
        void fillScreen(uint16_t color);
        void drawPixel(int16_t x, int16_t y, uint16_t color);
        void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
        void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
        void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
        void setRotation(uint8_t r);
        void invertDisplay(boolean i);

        int GetTouchSPI_CS(void) {
                return touch_CS_SPI;
        };

        int GetTouch_IRQ(void) {
                return touch_IRQ_SPI;
        };
        // Pass 8-bit (each) R,G,B, get back 16-bit packed color

        static uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
                return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        uint32_t readID(void);
        uint8_t readcommand8(uint8_t reg, uint8_t index = 0);
        uint8_t tabcolor;
        uint8_t read8(void);
        uint16_t read16(void);
        void setReadDataBus(void);
        void setWriteDataBus(void);

        void write8(uint8_t);
        void writecommand(uint8_t c);
        void writedata(uint8_t d);

        void write8stream(uint8_t c);
        void writemem16(uint16_t d);
        void writemem16(uint16_t d, uint32_t times);

        void setScroll(uint16_t offset);


        void readRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *pcolors);
        void writeRect(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *pcolors);

        void setAddr(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) __attribute__ ((always_inline)) {
                CD_COMMAND;
                write8stream(ILI9341_COL_ADDR_SET); // Column addr set
                CD_DATA;
                write8stream(x0 >> 8);
                write8stream(x0 & 0xff);
                write8stream(x1 >> 8);
                write8stream(x1 & 0xff);
                CD_COMMAND;
                write8stream(ILI9341_PAGE_ADDR_SET); // Row addr set
                CD_DATA;
                write8stream(y0 >> 8);
                write8stream(y0 & 0xff);
                write8stream(y1 >> 8);
                write8stream(y1 & 0xff);
        }


        //color565toRGB         - converts 565 format 16 bit color to RGB

        static void color565toRGB(uint16_t color, uint8_t &r, uint8_t &g, uint8_t &b) {
                r = (color >> 8)&0x00F8;
                g = (color >> 3)&0x00FC;
                b = (color << 3)&0x00F8;
        }

        //color565toRGB14               - converts 16 bit 565 format color to 14 bit RGB (2 bits clear for math and sign)
        //returns 00rrrrr000000000,00gggggg00000000,00bbbbb000000000
        //thus not overloading sign, and allowing up to double for additions for fixed point delta

        static void color565toRGB14(uint16_t color, int16_t &r, int16_t &g, int16_t &b) {
                r = (color >> 2)&0x3E00;
                g = (color << 3)&0x3F00;
                b = (color << 9)&0x3E00;
        }

        //RGB14tocolor565               - converts 14 bit RGB back to 16 bit 565 format color

        static uint16_t RGB14tocolor565(int16_t r, int16_t g, int16_t b) {
                return (((r & 0x3E00) << 2) | ((g & 0x3F00) >> 3) | ((b & 0x3E00) >> 9));
        }
        uint16_t readScanLine();
        uint16_t readPixel(int16_t x, int16_t y);

        // writeRect8BPP -      write 8 bit per pixel paletted bitmap
        //                                      bitmap data in array at pixels, one byte per pixel
        //                                      color palette data in array at palette
        void writeRect8BPP(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *pixels, const uint16_t * palette);

        // writeRect4BPP -      write 4 bit per pixel paletted bitmap
        //                                      bitmap data in array at pixels, 4 bits per pixel
        //                                      color palette data in array at palette
        //                                      width must be at least 2 pixels
        void writeRect4BPP(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *pixels, const uint16_t * palette);

        // writeRect2BPP -      write 2 bit per pixel paletted bitmap
        //                                      bitmap data in array at pixels, 4 bits per pixel
        //                                      color palette data in array at palette
        //                                      width must be at least 4 pixels
        void writeRect2BPP(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *pixels, const uint16_t * palette);

        // writeRect1BPP -      write 1 bit per pixel paletted bitmap
        //                                      bitmap data in array at pixels, 4 bits per pixel
        //                                      color palette data in array at palette
        //                                      width must be at least 8 pixels
        void writeRect1BPP(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *pixels, const uint16_t * palette);

        // from Adafruit_GFX.h

        void HLine(int16_t x, int16_t y, int16_t w, uint16_t color) __attribute__ ((always_inline)) {
                if((x >= _width) || (y >= _height) || (y < 0)) return;
                if(x < 0) {
                        w += x;
                        x = 0;
                }
                if((x + w - 1) >= _width) w = _width - x;

                setAddr(x, y, x + w - 1, y);
                CD_COMMAND;
                write8stream(ILI9341_MEM_WR);
                CD_DATA;
                writemem16(color, w);
        }

        void VLine(int16_t x, int16_t y, int16_t h, uint16_t color) __attribute__ ((always_inline)) {
                if((x >= _width) || (x < 0) || (y >= _height)) return;
                if(y < 0) {
                        h += y;
                        y = 0;
                }
                if((y + h - 1) >= _height) h = _height - y;
                setAddr(x, y, x, y + h - 1);
                CD_COMMAND;
                write8stream(ILI9341_MEM_WR);
                CD_DATA;
                writemem16(color, h);
        }

        void Pixel(int16_t x, int16_t y, uint16_t color) __attribute__ ((always_inline)) {
                if((x >= _width) || (x < 0) || (y >= _height) || (y < 0)) return;
                setAddr(x, y, x, y);
                CD_COMMAND;
                write8stream(ILI9341_MEM_WR);
                CD_DATA;
                writemem16(color);
        }

        void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
        void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
        void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
        void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
        void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
        void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
        void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
        void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
        void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
        void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
        void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);

        void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
        void setCursor(int16_t x, int16_t y);
        void getCursor(int16_t *x, int16_t *y);
        void setTextColor(uint16_t c);
        void setTextColor(uint16_t c, uint16_t bg);
        void setTextSize(uint8_t s);
        uint8_t getTextSize();
        void setTextWrap(boolean w);
        boolean getTextWrap();
        virtual size_t write(uint8_t);

        int16_t width(void) {
                return _width;
        }

        int16_t height(void) {
                return _height;
        }
        
        uint8_t getRotation(void) {
                return rotation;
        };

        int16_t getCursorX(void) const {
                return cursor_x;
        }

        int16_t getCursorY(void) const {
                return cursor_y;
        }

        void setFont(const ILI9341_TFT_font_t &f) {
                font = &f;
        }

        void setFontAdafruit(void) {
                font = NULL;
        }
        void drawFontChar(unsigned int c);
        void measureChar(uint8_t c, uint16_t* w, uint16_t* h);

        uint16_t fontCapHeight() {
                return (font) ? font->cap_height : textsize * 8;
        }

        uint16_t fontLineSpace() {
                return (font) ? font->line_space : textsize * 8;
        }

        uint16_t fontGap() {
                return (font) ? font->line_space - font->cap_height : 0;
        };
        uint16_t measureTextWidth(const char* text, int chars = 0);
        uint16_t measureTextHeight(const char* text, int chars = 0);
        int16_t strPixelLen(char * str);
        void drawFontBits(uint32_t bits, uint32_t numbits, uint32_t x, uint32_t y, uint32_t repeat);

};

// To avoid conflict when also using Adafruit_GFX or any Adafruit library
// which depends on Adafruit_GFX, #include the Adafruit library *BEFORE*
// you #include ILI9341_816-bit_teensy.h.
#ifndef _ADAFRUIT_GFX_H

class Adafruit_GFX_Button {
public:

        Adafruit_GFX_Button(void) {
                _gfx = NULL;
        }
        void initButton(ILI9341_TFT *gfx, int16_t x, int16_t y,
                uint8_t w, uint8_t h,
                uint16_t outline, uint16_t fill, uint16_t textcolor,
                const char *label, uint8_t textsize);
        void drawButton(bool inverted = false);
        bool contains(int16_t x, int16_t y);

        void press(boolean p) {
                laststate = currstate;
                currstate = p;
        }

        bool isPressed() {
                return currstate;
        }

        bool justPressed() {
                return (currstate && !laststate);
        }

        bool justReleased() {
                return (!currstate && laststate);
        }
private:
        ILI9341_TFT *_gfx;
        int16_t _x, _y;
        uint16_t _w, _h;
        uint8_t _textsize;
        uint16_t _outlinecolor, _fillcolor, _textcolor;
        char _label[10];
        boolean currstate, laststate;
};
#endif

#endif

#endif  // _ADAFRUIT_ILI9341H_
