#ifndef _SSD1331_REGISTERS_H_
#define _SSD1331_REGISTERS_H_

extern "C" {

// Fundamental commands
static const uint8_t CMD_NOP        = 0xBC;
static const uint8_t CMD_CMD_LOCK   = 0xFD;

static const uint8_t CMD_ON_DISPLAY_MODE    = 0xAF;
static const uint8_t CMD_OFF_DISPLAY_MODE   = 0xAE;
static const uint8_t CMD_DIM_DISPLAY_MODE   = 0xAC;
static const uint8_t CMD_DIM_MODE_CONFIG    = 0xAB;
static const uint8_t CMD_SET_POWERSAVE      = 0xB0; // Effect to be tested with physical measurement

static const uint8_t CMD_MASTER_CONFIG          = 0xAD;
static const uint8_t CMD_CLK_DIV_AND_OSC_FREQ   = 0xB3;
static const uint8_t CMD_P1_P2_PERIOD           = 0xB1;
static const uint8_t CMD_COM_DESEL_VOLT_LEVEL   = 0xBE;
static const uint8_t CMD_PRECHARGE_VOLTAGE      = 0xBB;
static const uint8_t CMD_PRECHARGE_SPEED_A      = 0x8A;
static const uint8_t CMD_PRECHARGE_SPEED_B      = 0x8B;
static const uint8_t CMD_PRECHARGE_SPEED_C      = 0x8C;

static const uint8_t CMD_MUX_RATIO              = 0xA8;
static const uint8_t CMD_START_LINE             = 0xA1;
static const uint8_t CMD_VERTICAL_OFFSET        = 0xA2;
static const uint8_t CMD_COL_STARTEND           = 0x15;
static const uint8_t CMD_ROW_STARTEND           = 0x75;
static const uint8_t CMD_REMAP_AND_COLOR_DEPTH	= 0xA0;

static const uint8_t CMD_MASTER_CURR			                = 0x87;
static const uint8_t CMD_CONTRAST_COLOR_A_SEG	            = 0x81;
static const uint8_t CMD_CONTRAST_COLOR_B_SEG	            = 0x82;
static const uint8_t CMD_CONTRAST_COLOR_C_SEG	            = 0x83;
static const uint8_t CMD_SET_GRAYSCALE_TABLE	            = 0xB8;
static const uint8_t CMD_RESET_TO_BUILTIN_GRAYSCALE_TABLE	= 0xB9;

static const uint8_t CMD_NORMAL_DISPLAY_MODE	= 0xA4;
static const uint8_t CMD_ALL_ON_DISPLAY_MODE	= 0xA5;
static const uint8_t CMD_ALL_OFF_DISPLAY_MODE	= 0xA6;
static const uint8_t CMD_INVERT_DISPLAY_MODE	= 0xA7;

// Graphic acceleration
static const uint8_t CMD_DRAW_LINE	                = 0x21;
static const uint8_t CMD_DRAW_RECT	                = 0x22;
static const uint8_t CMD_COPY_WIN	                  = 0x23;
static const uint8_t CMD_DIM_WIN	                  = 0x24;
static const uint8_t CMD_CLEAR_WIN	                = 0x25;
static const uint8_t CMD_FILL_ENABLE_DISABLE        = 0x26;
static const uint8_t CMD_VERT_AND_HOR_SCROLL_SETUP	= 0x27;
static const uint8_t CMD_SCROLL_ENABLE	            = 0x2F;
static const uint8_t CMD_SCROLL_DISABLE	            = 0x2E;

// Utility flags
#define POWERSAVE_ENABLE              (uint8_t)0x1A
#define POWERSAVE_DISABLE             (uint8_t)0x0B
#define REMAP_HORIZ_ADDR_INC          (uint8_t)0x00
#define REMAP_VERT_ADDR_INC           (uint8_t)0x01
#define REMAP_NO_REVERSE_COLUMN       (uint8_t)0x00
#define REMAP_REVERSE_COLUMN          (uint8_t)0x02
#define REMAP_NO_REVERSE_COLOR_ORDER  (uint8_t)0x00
#define REMAP_REVERSE_COLOR_ORDER     (uint8_t)0x04
#define REMAP_NO_LEFT_RIGHT_COM_SWAP  (uint8_t)0x00
#define REMAP_LEFT_RIGHT_COM_SWAP     (uint8_t)0x08
#define REMAP_NO_REVERSE_COM_SCAN     (uint8_t)0x00
#define REMAP_REVERSE_COM_SCAN        (uint8_t)0x10
#define REMAP_NO_SPLIT_ODD_EVEN_COM   (uint8_t)0x00
#define REMAP_SPLIT_ODD_EVEN_COM      (uint8_t)0x20
#define REMAP_256_COLOR_FORMAT        (uint8_t)0x00
#define REMAP_65K_COLOR_FORMAT        (uint8_t)0x40
#define REMAP_65K_COLOR_FORMAT2       (uint8_t)0x60

}

#endif
