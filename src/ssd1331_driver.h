#ifndef _SSD1331_DRIVER_H_
#define _SSD1331_DRIVER_H_

// #define SHARED_SPI_PORT

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

#define R(x) (x << 11)
#define G(x) (x << 5)
#define B(x) (x << 0)

extern "C" {

static const int SSD1331_WIDTH   = 96;
static const int SSD1331_HEIGHT  = 64;

static const int FILL_DELAY			= 400;

typedef enum {
  DIM_MODE,
  SLEEP_MODE,
  NORMAL_MODE
}DisplayMode;

typedef enum {
  COLOR_DEPTH_256,
  COLOR_DEPTH_65K,
  COLOR_DEPTH_65K_2
}ColorDepth;

typedef enum {
  NORMAL_COLOR_ORDER,
  REVERSE_COLOR_ORDER
}ColorOrder;

typedef enum {
  ALL_PIXELS_ON,
  ALL_PIXELS_OFF,
  ALL_PIXELS_NORMAL,
  ALL_PIXELS_INVERTED
}PixelsState;

typedef enum {
  _6_FRAMES = 0,
  _10_FRAMES = 1,
  _100_FRAMES = 2,
  _200_FRAMES = 3
}SrollTimeInterval;

// Basic commands
void ssd1331_init(uint8_t _sck, uint8_t _miso, uint8_t _mosi, uint8_t _cs, uint8_t _rst, uint8_t _dc, uint32_t _spiFreq); // Execute this before any other ssd1331 command. This display supports up to 6.6MHz SPI clk
void ssd1331_setDisplayMode(DisplayMode mode);
void ssd1331_powerSaveMode(bool state); // A distinct power saving feature other than ssd1331_setDisplayMode(SLEEP_MODE). Can be used together
void ssd1331_setMainContrast(uint8_t contrast); // 0 to 15
void ssd1331_setContrastPerColor(uint8_t level_colorA, uint8_t level_colorB, uint8_t level_colorC); // 0 to 255
void ssd1331_setDimModeContrastPerColor(uint8_t level_colorA, uint8_t level_colorB, uint8_t level_colorC, uint8_t prechargeVoltage); // 0-255, 0-255, 0-255, 0x1F
void ssd1331_setOrientation(bool columnReverse, bool rowReverse);
void ssd1331_setColorFormat(ColorDepth colorDepth, ColorOrder colorOrder);
void ssd1331_setDisplayGrayTable(const uint8_t grayTableconst[32]);
void ssd1331_setAllPixelState(PixelsState pixelsState);

// Hardware accelerated commands
void ssd1331_drawLine(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint16_t color);
void ssd1331_drawRect(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint16_t lineColor, uint16_t fillColor);
void ssd1331_copyWin(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint8_t x_newStart, uint8_t y_newStart);
void ssd1331_dimWin(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end);
void ssd1331_clearWin(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end);
void ssd1331_setFillBehaviour(bool rectFillEnabled, bool reverseCopyEnabled);
void ssd1331_configScroll(uint8_t x_scrollStepSize, uint8_t y_scrollStepSize, uint8_t startRowAdressToScrollHorizontally, uint8_t rowNumbersToScrollHorizontally, SrollTimeInterval scrollTimeInterval);
void ssd1331_activateScroll();
void ssd1331_deactivateScroll();
void ssd1331_drawBitmap(const uint8_t *bitmap, uint8_t x_start, uint8_t y_start, uint8_t width, uint8_t height);

}

#endif