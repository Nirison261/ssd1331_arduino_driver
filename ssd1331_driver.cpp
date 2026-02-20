#include<Arduino.h>
#include<SPI.h>
#include"ssd1331_driver.h"
#include"ssd1331_registers.h"

extern "C" {

static void startSPITransaction() __attribute__((always_inline));
static void endSPITransaction() __attribute__((always_inline));
static void writeCmd(uint8_t cmd) __attribute__((always_inline));
static void setRegVal(uint8_t reg, uint8_t value) __attribute__((always_inline));
static void setDrawArea(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y) __attribute__((always_inline));

static uint8_t cs_pin;
static uint8_t rst_pin;
static uint8_t dc_pin;
static uint32_t spiFreq;

static uint8_t remapReg;

static const uint8_t 	default_graytable[32]= 	{0x01,0x03,0x05,0x07,0x0A,0x0D,0x10,0x13, \
                                            0x16,0x19,0x1C,0x20,0x24,0x28,0x2C,0x30, \
                                            0x34,0x38,0x3C,0x40,0x44,0x48,0x4C,0x50, \
                                            0x54,0x58,0x5C,0x60,0x64,0x68,0x6C,0x70};

void ssd1331_init(uint8_t _sck, uint8_t _miso, uint8_t _mosi, uint8_t _cs, uint8_t _rst, uint8_t _dc, uint32_t _spiFreq) {
  cs_pin    = _cs;
  rst_pin   = _rst;
  dc_pin    = _dc;
  spiFreq   = _spiFreq;
  pinMode(rst_pin, OUTPUT);
  pinMode(dc_pin, OUTPUT);
  pinMode(cs_pin, OUTPUT);

  #if defined(ESP8266)
  SPI.begin();
  #elif defined(ESP32)
  SPI.begin(_sck, _miso, _mosi, cs_pin);
  #endif

  #ifndef SHARED_SPI_PORT
  startSPITransaction(); // If the default SPI port is not shared, open it endefinitely
  #endif

  // Reset the SSD1331 IC
  digitalWrite(rst_pin, HIGH);
  delay(10);
  digitalWrite(rst_pin, LOW);
  delay(10);
  digitalWrite(rst_pin, HIGH);
  delay(40);

  // Init sequence (these values (except the dim mode contrast) are the same as in the adafruit ssd1331 init
  setRegVal(CMD_CMD_LOCK, 0x12); // Unlock display commands
  ssd1331_setDisplayMode(SLEEP_MODE); // Disable the display

  // setRegVal(CMD_CLK_DIV_AND_OSC_FREQ, 0xF0); // Highest frequency and lowest divider
  setRegVal(CMD_CLK_DIV_AND_OSC_FREQ, 0xD0); // Lower clock for less power consumption, OLED aging and EMI
  setRegVal(CMD_MUX_RATIO, 0x3F); // 63/63 mux ratio

  /*
  Set to:
    - horizontal address increment mode
    - reverse maping between graphic RAM and SEG pins
    - no reverse color order
    - 
    -
    -
    - 65k color mode instead of 256 color mode
  */
  remapReg = REMAP_HORIZ_ADDR_INC|REMAP_REVERSE_COLUMN|REMAP_NO_REVERSE_COLOR_ORDER|REMAP_NO_LEFT_RIGHT_COM_SWAP|REMAP_REVERSE_COM_SCAN|REMAP_SPLIT_ODD_EVEN_COM|REMAP_65K_COLOR_FORMAT;
  setRegVal(CMD_REMAP_AND_COLOR_DEPTH, remapReg);

  setRegVal(CMD_START_LINE, 0x00); // The RAM line mapped to ROW0 is RAM0
  setRegVal(CMD_VERTICAL_OFFSET, 0x00); // No vertical offset ROW0 corresponds to COM0 pin
  setRegVal(CMD_MASTER_CONFIG, 0x8E); // Power-up Vcc. Must be set to 0x8E after RESET and before CMD_ON_DISPLAY_MODE

  ssd1331_powerSaveMode(false); // Disable power save mode
  setRegVal(CMD_P1_P2_PERIOD, 0x31); // default value in the datasheet
  setRegVal(CMD_PRECHARGE_VOLTAGE, 0x3A);
  setRegVal(CMD_PRECHARGE_SPEED_A, 0x64);
  setRegVal(CMD_PRECHARGE_SPEED_B, 0x78);
  setRegVal(CMD_PRECHARGE_SPEED_C, 0x64);
  setRegVal(CMD_COM_DESEL_VOLT_LEVEL, 0x3E); // default value
  ssd1331_setMainContrast(6); // 0 to 15
  ssd1331_setContrastPerColor(0x91, 0x50, 0x7D); // 0-255, 0-255, 0-255
  ssd1331_setDimModeContrastPerColor(0x48, 0x28, 0x3E, 0x0F); // 0-255, 0-255, 0-255, 0x1F
  ssd1331_setAllPixelState(ALL_PIXELS_NORMAL);

  ssd1331_setDisplayGrayTable(default_graytable);
  
  delay(100); // According to the datasheet
  ssd1331_setDisplayMode(NORMAL_MODE);
}

void ssd1331_setDisplayMode(DisplayMode mode) {
  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  switch(mode) {
    case DIM_MODE:
      writeCmd(CMD_DIM_DISPLAY_MODE);
      break;
    case SLEEP_MODE:
      writeCmd(CMD_OFF_DISPLAY_MODE);
      break;
    case NORMAL_MODE:
      writeCmd(CMD_ON_DISPLAY_MODE);
      break;
  }

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

void ssd1331_powerSaveMode(bool state) {
  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  if(state)
    setRegVal(CMD_SET_POWERSAVE, POWERSAVE_ENABLE);
  else
    setRegVal(CMD_SET_POWERSAVE, POWERSAVE_DISABLE);

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

void ssd1331_setMainContrast(uint8_t contrast) {
  if (contrast > 15)
    contrast = 15;

  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  setRegVal(CMD_MASTER_CURR, contrast);

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

void ssd1331_setContrastPerColor(uint8_t level_colorA, uint8_t level_colorB, uint8_t level_colorC) {
  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  setRegVal(CMD_CONTRAST_COLOR_A_SEG, level_colorA);
  setRegVal(CMD_CONTRAST_COLOR_B_SEG, level_colorB);
  setRegVal(CMD_CONTRAST_COLOR_C_SEG, level_colorC);

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

void ssd1331_setDimModeContrastPerColor(uint8_t level_colorA, uint8_t level_colorB, uint8_t level_colorC, uint8_t prechargeVoltage) {
  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  writeCmd(CMD_DIM_MODE_CONFIG);
  writeCmd(0); // Reserved register. Always set to 0x00
  writeCmd(level_colorA); // Color A contrast level in dim mode
  writeCmd(level_colorB); // Color B contrast level in dim mode
  writeCmd(level_colorC); // Color C contrast level in dim mode
  writeCmd(prechargeVoltage); // Set precharge voltage in dim mode, max: 0x1F

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

void ssd1331_setOrientation(bool columnReverse, bool rowReverse) {
  if(columnReverse) {
    remapReg = remapReg | 0b00000010;
  } else {
    remapReg = remapReg & 0b11111101;
  }

  if(rowReverse) {
    remapReg = remapReg | 0b00010000;
  } else {
    remapReg = remapReg & 0b11101111;
  }

  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  setRegVal(CMD_REMAP_AND_COLOR_DEPTH,  remapReg);

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

void ssd1331_setColorFormat(ColorDepth colorDepth, ColorOrder colorOrder) {
  uint8_t tmpFlag;

  switch(colorDepth) {
    case COLOR_DEPTH_256:
      tmpFlag = 0x00;
      break;
    case COLOR_DEPTH_65K:
      tmpFlag = 0x40;
      break;
    case COLOR_DEPTH_65K_2:
      tmpFlag = 0x80;
      break;
    default:
      return;
      break;
  }

  switch(colorOrder) {
    case NORMAL_COLOR_ORDER:
      tmpFlag &= 0b11111011;
      break;
    case REVERSE_COLOR_ORDER:
      tmpFlag |= 0b00000100;
      break;
  }

  remapReg = (remapReg & 0b00111011) | tmpFlag;

  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif
  
  setRegVal(CMD_REMAP_AND_COLOR_DEPTH,  remapReg);

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

void ssd1331_setDisplayGrayTable(const uint8_t grayTable[32]) {
  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  writeCmd(CMD_SET_GRAYSCALE_TABLE);
  for (uint8_t i = 0; i < 32; i++) {
    writeCmd(grayTable[i]);
  }

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

void ssd1331_setAllPixelState(PixelsState pixelsState) {
  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  switch(pixelsState) {
    case ALL_PIXELS_ON:
      writeCmd(CMD_ALL_ON_DISPLAY_MODE);
      break;
    case ALL_PIXELS_OFF:
      writeCmd(CMD_ALL_OFF_DISPLAY_MODE);
      break;
    case ALL_PIXELS_NORMAL:
      writeCmd(CMD_NORMAL_DISPLAY_MODE);
      break;
    case ALL_PIXELS_INVERTED:
      writeCmd(CMD_INVERT_DISPLAY_MODE);
      break;
  }

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

void ssd1331_configScroll(uint8_t x_scrollStepSize, uint8_t y_scrollStepSize, uint8_t startRowAdressToScrollHorizontally, uint8_t rowNumbersToScrollHorizontally, SrollTimeInterval scrollTimeInterval) {
  uint8_t interval;
  
  if(x_scrollStepSize > 95)
    x_scrollStepSize = 95;

  if(y_scrollStepSize > 63)
    y_scrollStepSize = 63;

  if(startRowAdressToScrollHorizontally > 63)
    startRowAdressToScrollHorizontally = 63;

  if(startRowAdressToScrollHorizontally + rowNumbersToScrollHorizontally > 63)
      rowNumbersToScrollHorizontally = 63 - startRowAdressToScrollHorizontally;
  
  if(scrollTimeInterval > _200_FRAMES)
    scrollTimeInterval = _200_FRAMES;

  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  writeCmd(CMD_VERT_AND_HOR_SCROLL_SETUP);
  writeCmd(x_scrollStepSize);
  writeCmd(startRowAdressToScrollHorizontally);
  writeCmd(rowNumbersToScrollHorizontally);
  writeCmd(y_scrollStepSize);
  writeCmd(scrollTimeInterval);

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

void ssd1331_activateScroll() {
  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  writeCmd(CMD_SCROLL_ENABLE);

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

void ssd1331_deactivateScroll() {
  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  writeCmd(CMD_SCROLL_DISABLE);

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

void ssd1331_drawLine(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint16_t color) {
  if(x_start > 95)
    x_start = 95;
  
  if(y_start > 63)
    y_start = 63;
  
  if(x_end > 95)
    x_end = 95;
  
  if(y_end > 63)
    y_end = 63;

  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  writeCmd(CMD_DRAW_LINE);
  writeCmd(x_start);
  writeCmd(y_start);
  writeCmd(x_end);
  writeCmd(y_end);
  writeCmd(color << 1);
  writeCmd(color >> 5);
  writeCmd(color >> 10);

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

void ssd1331_drawRect(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint16_t lineColor, uint16_t fillColor) {
  if(x_start > 95)
    x_start = 95;
  
  if(y_start > 63)
    y_start = 63;
  
  if(x_end > 95)
    x_end = 95;
  
  if(y_end > 63)
    y_end = 63;

  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  writeCmd(CMD_DRAW_RECT);
  writeCmd(x_start);
  writeCmd(y_start);
  writeCmd(x_end);
  writeCmd(y_end);
  writeCmd(lineColor << 1);
  writeCmd(lineColor >> 5);
  writeCmd(lineColor >> 10);
  writeCmd(fillColor << 1);
  writeCmd(fillColor >> 5);
  writeCmd(fillColor >> 10);

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

void ssd1331_copyWin(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint8_t x_newStart, uint8_t y_newStart) {
  if(x_start > 95)
    x_start = 95;
  
  if(y_start > 63)
    y_start = 63;
  
  if(x_end > 95)
    x_end = 95;
  
  if(y_end > 63)
    y_end = 63;
  
  if(x_newStart > 95)
    x_newStart = 95;
  
  if(y_newStart > 63)
    y_newStart = 63;

  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  writeCmd(CMD_COPY_WIN);
  writeCmd(x_start);
  writeCmd(y_start);
  writeCmd(x_end);
  writeCmd(y_end);
  writeCmd(x_newStart);
  writeCmd(y_newStart);

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

void ssd1331_dimWin(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end) {
  if(x_start > 95)
    x_start = 95;
  
  if(y_start > 63)
    y_start = 63;
  
  if(x_end > 95)
    x_end = 95;
  
  if(y_end > 63)
    y_end = 63;

  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  writeCmd(CMD_DIM_WIN);
  writeCmd(x_start);
  writeCmd(y_start);
  writeCmd(x_end);
  writeCmd(y_end);

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

void ssd1331_clearWin(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end) {
  if(x_start > 95)
    x_start = 95;
  
  if(y_start > 63)
    y_start = 63;
  
  if(x_end > 95)
    x_end = 95;
  
  if(y_end > 63)
    y_end = 63;
  
  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  writeCmd(CMD_CLEAR_WIN);
  writeCmd(x_start);
  writeCmd(y_start);
  writeCmd(x_end);
  writeCmd(y_end);

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif

  delayMicroseconds(FILL_DELAY);
}

void ssd1331_setFillBehaviour(bool rectFillEnabled, bool reverseCopyEnabled) {
  uint8_t tmpReg;

  if(rectFillEnabled) {
    tmpReg = 0x01;
  } else {
    tmpReg = 0x00;
  }

  if(reverseCopyEnabled) {
    tmpReg |= 0x10;
  }

  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  setRegVal(CMD_FILL_ENABLE_DISABLE, tmpReg);

  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

void ssd1331_drawBitmap(const uint8_t *bitmap, uint8_t x_start, uint8_t y_start, uint8_t width, uint8_t height) {
  if(x_start + width > SSD1331_WIDTH)
    return;
  
  if(y_start + height > SSD1331_HEIGHT)
    return;
  
  int bufSize = width * height * 2;

  #ifdef SHARED_SPI_PORT
  startSPITransaction();
  #endif

  setDrawArea(x_start, y_start, x_start + width - 1, y_start + height - 1);
  digitalWrite(dc_pin, HIGH);

  /*
  If the buffer was a uint16_t array:

  for(int i=0; i < bufSize; i++) {
    SPI.transfer((uint8_t)(bitmap[i] >> 8));
    SPI.transfer((uint8_t)bitmap[i]);
  }
  
  But we prefer a uint8_t array because of some concern about endianness, and the ssd1331 expect
  high byte first.
  So we need a pre-formated uint8_t array to pass to this function.
  All of this was made, so we get a fast and optimized bitmap drawing.
  */
  SPI.writeBytes(bitmap, bufSize); // Works with esp32 hardware SPI
  /*
  Use this if you don't use ESP32:
  for(int i=0; i < bufSize; i++) {
    SPI.transfer((uint8_t)bitmap[i]); // Slower
  }
  */
  
  #ifdef SHARED_SPI_PORT
  endSPITransaction();
  #endif
}

// =========================================
//            STATIC FUNCTIONS
// =========================================

static void startSPITransaction() {
  SPI.beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  digitalWrite(cs_pin, LOW);
}

static void endSPITransaction() {
  digitalWrite(cs_pin, HIGH);
  SPI.endTransaction();
}

static void writeCmd(uint8_t cmd) {
  digitalWrite(dc_pin, LOW);
  SPI.transfer(cmd);
}

static void setRegVal(uint8_t reg, uint8_t value) {
  writeCmd(reg);
  SPI.transfer(value);
}

static void setDrawArea(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y) {
  writeCmd(CMD_COL_STARTEND);
  writeCmd(start_x);
  writeCmd(end_x);
  writeCmd(CMD_ROW_STARTEND);
  writeCmd(start_y);
  writeCmd(end_y);
}

}