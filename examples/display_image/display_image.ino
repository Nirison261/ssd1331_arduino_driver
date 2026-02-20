#include "ssd1331_driver.h"
#include "nature.h"

#define SCK 12
#define MISO 13
#define MOSI 11

#define OLED_CS 10
#define OLED_RST 4
#define OLED_DC 9

#define SPI_FREQ 1000000UL

void setup()
{
    Serial.begin(115200);
    while (!Serial);
    Serial.println("");
    Serial.println("Start");
    ssd1331_init(SCK, MISO, MOSI, OLED_CS, OLED_RST, OLED_DC, SPI_FREQ);
    ssd1331_setDisplayMode(NORMAL_MODE);
    ssd1331_powerSaveMode(false);
    ssd1331_setOrientation(true, true);
    ssd1331_setMainContrast(6);
    ssd1331_setColorFormat(COLOR_DEPTH_65K, NORMAL_COLOR_ORDER);
    ssd1331_setAllPixelState(ALL_PIXELS_NORMAL);

    ssd1331_clearWin(0, 0, 95, 63);

    ssd1331_drawBitmap(nature, 0, 0, 96, 64);
}

void loop()
{
    ssd1331_setContrastPerColor(0x91, 0x50, 0x7D);
    Serial.println("Contrast per color: 0x91 0x50 0x7D");
    delay(3000);
}