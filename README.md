# LovyanGFX

[![arduino-library-badge](https://www.ardu-badge.com/badge/LovyanGFX.svg?)](https://www.ardu-badge.com/LovyanGFX)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/lovyan03/library/LovyanGFX.svg)](https://registry.platformio.org/packages/libraries/lovyan03/LovyanGFX)

[![Arduino](https://github.com/lovyan03/LovyanGFX/actions/workflows/ArduinoBuild.yml/badge.svg?branch=master)](https://github.com/lovyan03/LovyanGFX/actions/workflows/ArduinoBuild.yml)
[![Platformio](https://github.com/lovyan03/LovyanGFX/actions/workflows/PlatformioBuild.yml/badge.svg?branch=master)](https://github.com/lovyan03/LovyanGFX/actions/workflows/PlatformioBuild.yml)
[![esp-idf](https://github.com/lovyan03/LovyanGFX/actions/workflows/IDFBuild.yml/badge.svg?branch=master)](https://github.com/lovyan03/LovyanGFX/actions/workflows/IDFBuild.yml)



Display (LCD / OLED / EPD) graphics library (for ESP32 SPI, I2C, 8bitParallel / ESP8266 SPI, I2C / ATSAMD51 SPI).  
M5Stack / M5StickC / TTGO T-Watch / ODROID-GO / ESP-WROVER-KIT / WioTerminal / and more...  
[![examples](http://img.youtube.com/vi/SMOHRPqUZcQ/0.jpg)](http://www.youtube.com/watch?v=SMOHRPqUZcQ "examples")
[![examples](http://img.youtube.com/vi/F5gsp41Elac/0.jpg)](http://www.youtube.com/watch?v=F5gsp41Elac "MultiPanel")

Overview
----------------
This is a graphics library that works with a combination of ESP32 with SPI, I2C, 8-bit parallel / ESP8266 with SPI / ATSAMD51 with SPI to the Display. (see compatibility list below).

This library mimics [AdafruitGFX](https://github.com/adafruit/Adafruit-GFX-Library) and [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) APIs while aiming for higher functional coverage and performances.

This library has the following advantages.
  - ArduinoESP32 and ESP-IDF are supported.
  - Both 16bit and 24bit color modes are supported. (actual number of colors depends on display specifications)
  - Execute another process during communication operation using DMA transfer.
  - Fast rotation/expansion of the off-screen buffer (sprite).
  - Simultaneous use of multiple displays.
  - Automatic processing of color reduction drawing for monochrome displays.
  - OpenCV,SDL2 can be used as a drawing destination and can run on a PC.  
  - Composite video signal (NTSC, PAL) output (only ESP32)


|        | SPI | I2C | 8bit Para |16bit Para | RGB       | CVBS     |
|:------:|:---:|:---:|:---------:|:---------:|:---------:|:--------:|
|ESP32   | HW  | HW  | HW (I2S)  | ---       | ---       |HW(I2SDAC)|
|ESP32-S2| HW  | HW  | HW (I2S)  | HW (I2S)  | ---       | ---      |
|ESP32-S3| HW  | HW  |HW(LCD/CAM)|HW(LCD/CAM)|HW(LCD/CAM)| ---      |
|ESP32-C3| HW  | HW  | SW        | ---       | ---       | ---      |
|ESP8266 | HW  | SW  | ---       | ---       | ---       | ---      |
|SAMD51  | HW  | HW  | ---       | ---       | ---       | ---      |
|SAMD21  | HW  | HW  | ---       | ---       | ---       | ---      |
|RP2040  | HW  | --- | ---       | ---       | ---       | ---      |

※ HW = HardWare Peripheral / SW = SoftWare implementation

|        |TouchScreens|
|:------:|:----------:|
|ESP32   | supported  |
|ESP32-S2| supported  |
|ESP32-S3| supported  |
|ESP32-C3| supported  |
|ESP8266 | supported  |
|SAMD51  | supported  |
|SAMD21  | supported  |
|RP2040  | ---        |

Supported Environments
---------------
  - Platform
    - ESP-IDF
    - Arduino ESP32
    - Arduino ATSAMD51 (Seeed)
    - Arduino RP2040

  - Displays
    - GC9107 (M5AtomS3)
    - GC9A01
    - GDEW0154M09 (M5Stack CoreInk)
    - HX8357
    - ILI9163
    - ILI9225
    - ILI9341 (WioTerminal, ESP-WROVER-KIT, ODROID-GO, LoLin D32 Pro, WiFiBoy Pro)
    - ILI9342 (M5Stack, M5Stack Core2, ESP32-S3-BOX)
    - ILI9481
    - ILI9486
    - ILI9488 (Makerfabs Touch with Camera)
    - IT8951 (M5Paper)
    - NT35510/OTM8009A
    - R61529
    - RA8875
    - RM68120
    - SH110x (SH1106, SH1107, M5Stack Unit OLED)
    - S6D04K1
    - SSD1306 (SSD1309)
    - SSD1327
    - SSD1331
    - SSD1351 (SSD1357)
    - SSD1963
    - ST7735 (M5StickC, TTGO T-Wristband, TTGO TS, LoLin D32 Pro, WiFiBoy mini, ESPboy, PyBadge)
    - ST7789 (M5StickCPlus, TTGO T-Watch, ESP-WROVER-KIT, Makerfabs MakePython, DSTIKE D-duino-32 XS)
    - ST7796 (WT32-SC01)

    - M5Stack Unit LCD
    - M5Stack AtomDisplay

  - TouchScreens
    - I2C CST816S
    - I2C FT5x06 (FT5206, FT5306, FT5406, FT6206, FT6236, FT6336, FT6436)
    - I2C GSLx680 (GSL1680)
    - I2C GT911
    - I2C NS2009
    - I2C TT21xxx (TT21100)
    - SPI XPT2046
    - SPI STMPE610


Configuration examples can be found in [src/lgfx_user](src/lgfx_user).  
This library is also compatible with the above models and display panels with a similar command system,
 but only those that have been obtained and confirmed to work are officially supported.  

How to Use
----------------
Specific examples can be found in [examples](examples/).
### Basic Usage
```c

// *  If you are using a supported device in an environment other than ArduinoIDE,
// or if the supported device is not in the Board Manager (e.g., TTGO T-Wristband, ESP-WROVER-KIT, etc.),
// please write a #define LGFX_~ definition before including LovyanGFX.hpp.

// #define LGFX_M5STACK                       // M5Stack M5Stack Basic / Gray / Go / Fire
// #define LGFX_M5STACK_CORE2                 // M5Stack M5Stack Core2
// #define LGFX_M5STACK_COREINK               // M5Stack M5Stack CoreInk
// #define LGFX_M5STICK_C                     // M5Stack M5Stick C / CPlus
// #define LGFX_M5PAPER                       // M5Stack M5Paper
// #define LGFX_M5TOUGH                       // M5Stack M5Tough
// #define LGFX_M5ATOMS3                      // M5Stack M5ATOMS3
// #define LGFX_ODROID_GO                     // ODROID-GO
// #define LGFX_TTGO_TS                       // TTGO TS
// #define LGFX_TTGO_TWATCH                   // TTGO T-Watch
// #define LGFX_TTGO_TWRISTBAND               // TTGO T-Wristband
// #define LGFX_TTGO_TDISPLAY                 // TTGO T-Display
// #define LGFX_DDUINO32_XS                   // DSTIKE D-duino-32 XS
// #define LGFX_LOLIN_D32_PRO                 // LoLin D32 Pro
// #define LGFX_LOLIN_S3_PRO                  // LoLin S3 Pro
// #define LGFX_ESP_WROVER_KIT                // Espressif ESP-WROVER-KIT
// #define LGFX_ESP32_S3_BOX                  // Espressif ESP32-S3-BOX
// #define LGFX_ESP32_S3_BOX_LITE             // Espressif ESP32-S3-BOX Lite
// #define LGFX_WIFIBOY_PRO                   // WiFiBoy Pro
// #define LGFX_WIFIBOY_MINI                  // WiFiBoy mini
// #define LGFX_MAKERFABS_TOUCHCAMERA         // Makerfabs Touch with Camera
// #define LGFX_MAKERFABS_MAKEPYTHON          // Makerfabs MakePython
// #define LGFX_MAKERFABS_TFT_TOUCH_SPI       // Makerfabs TFT Touch SPI
// #define LGFX_MAKERFABS_TFT_TOUCH_PARALLEL16// Makerfabs TFT Touch Parallel 16
// #define LGFX_WT32_SC01                     // Seeed WT32-SC01
// #define LGFX_WIO_TERMINAL                  // Seeed Wio Terminal
// #define LGFX_PYBADGE                       // Adafruit PyBadge
// #define LGFX_FUNHOUSE                      // Adafruit FunHouse
// #define LGFX_FEATHER_ESP32_S2_TFT          // Adafruit Feather ESP32 S2 TFT
// #define LGFX_FEATHER_ESP32_S3_TFT          // Adafruit Feather ESP32 S3 TFT
// #define LGFX_ESPBOY                        // ESPboy
// #define LGFX_WYWY_ESP32S3_HMI_DEVKIT       // wywy ESP32S3 HMI DevKit
// #define LGFX_SUNTON_ESP32_2432S028         // Sunton ESP32 2432S028

  #define LGFX_AUTODETECT // Auto-detection (D-duino-32 XS, WT32-SC01, and PyBadge cannot read panel IDs and are excluded from auto-detection)

// By defining multiple devices or defining LGFX_AUTODETECT, the board will be automatically detected at runtime.


// Include the header.
#include <LovyanGFX.hpp>

#include <LGFX_AUTODETECT.hpp>  // Prepares the "LGFX" class
// #include <lgfx_user/LGFX_ESP32_sample.hpp> // Or prepare your own LGFX class

static LGFX lcd;                 // Create an instance of LGFX.
static LGFX_Sprite sprite(&lcd); // Create an instance of LGFX_Sprite if using sprites.

// If you are currently using TFT_eSPI and want to minimize source code changes, you can use this header.
// #include <LGFX_TFT_eSPI.hpp>
// static TFT_eSPI lcd;               // TFT_eSPI is defined as an alias for LGFX.
// static TFT_eSprite sprite(&lcd);   // TFT_eSprite is defined as an alias for LGFX_Sprite.


// For configurations not in the supported device list, refer to examples/HowToUse/2_user_setting.ino.
// Configuration examples can also be found in the src/lgfx_user folder.


void setup(void)
{
// First, call the initialization function.
  lcd.init();


// Set the rotation direction from 4 orientations: 0 to 3. (Using 4 to 7 will flip the display upside down.)
  lcd.setRotation(1);


// Set the backlight brightness in the range of 0 to 255.
  lcd.setBrightness(128);


// Set the color mode as needed. (Default is 16)
// 16-bit mode requires less SPI data transfer and runs faster, but red and blue have only 5-bit gradation.
// 24-bit mode requires more SPI data transfer, but provides finer color gradation.
//lcd.setColorDepth(16);  // Set to 16-bit RGB565
  lcd.setColorDepth(24);  // Set to 24-bit RGB888 (actual displayed colors may be 18-bit RGB666 depending on panel capabilities)


// The basic shape drawing functions are as follows.
/*
  fillScreen    (                color);  // Fill the entire screen
  drawPixel     ( x, y         , color);  // Point
  drawFastVLine ( x, y   , h   , color);  // Vertical line
  drawFastHLine ( x, y, w      , color);  // Horizontal line
  drawRect      ( x, y, w, h   , color);  // Rectangle outline
  fillRect      ( x, y, w, h   , color);  // Filled rectangle
  drawRoundRect ( x, y, w, h, r, color);  // Rounded rectangle outline
  fillRoundRect ( x, y, w, h, r, color);  // Filled rounded rectangle
  drawCircle    ( x, y      , r, color);  // Circle outline
  fillCircle    ( x, y      , r, color);  // Filled circle
  drawEllipse   ( x, y, rx, ry , color);  // Ellipse outline
  fillEllipse   ( x, y, rx, ry , color);  // Filled ellipse
  drawLine      ( x0, y0, x1, y1        , color); // Line between two points
  drawTriangle  ( x0, y0, x1, y1, x2, y2, color); // Triangle outline between three points
  fillTriangle  ( x0, y0, x1, y1, x2, y2, color); // Filled triangle between three points
  drawBezier    ( x0, y0, x1, y1, x2, y2, color); // Bezier curve through three points
  drawBezier    ( x0, y0, x1, y1, x2, y2, x3, y3, color); // Bezier curve through four points
  drawArc       ( x, y, r0, r1, angle0, angle1, color);   // Arc outline
  fillArc       ( x, y, r0, r1, angle0, angle1, color);   // Filled arc
*/


// For example, to draw a point with drawPixel, the arguments are: X coordinate, Y coordinate, color.
  lcd.drawPixel(0, 0, 0xFFFF); // Draw a white point at coordinate 0,0


// Color code generation functions are provided and can be used to specify colors.
// Arguments are red, green, blue, each specified in the range 0 to 255.
// To prevent loss of color information, using color888 is recommended.
  lcd.drawFastVLine(2, 0, 100, lcd.color888(255,   0,   0)); // Draw a vertical line in red
  lcd.drawFastVLine(4, 0, 100, lcd.color565(  0, 255,   0)); // Draw a vertical line in green
  lcd.drawFastVLine(6, 0, 100, lcd.color332(  0,   0, 255)); // Draw a vertical line in blue


// When not using color code generation functions, colors are specified as follows:
// RGB888 specified as 24-bit uint32_t type
// RGB565 specified as 16-bit uint16_t or int32_t type
// RGB332 specified as 8-bit uint8_t type

// When using uint32_t type, the color is treated as 24-bit RGB888.
// You can write it in hexadecimal with two digits each for red, green, blue.
// Use a uint32_t variable, append U to the literal, or cast to uint32_t.
  uint32_t red = 0xFF0000;
  lcd.drawFastHLine(0, 2, 100, red);            // Draw a horizontal line in red
  lcd.drawFastHLine(0, 4, 100, 0x00FF00U);      // Draw a horizontal line in green
  lcd.drawFastHLine(0, 6, 100, (uint32_t)0xFF); // Draw a horizontal line in blue


// When using uint16_t or int32_t type, the color is treated as 16-bit RGB565.
// Without special notation, values are treated as int32_t type, so this format applies.
// (This behavior is for compatibility with AdafruitGFX and TFT_eSPI.)
  uint16_t green = 0x07E0;
  lcd.drawRect(10, 10, 50, 50, 0xF800);         // Draw a rectangle outline in red
  lcd.drawRect(12, 12, 50, 50, green);          // Draw a rectangle outline in green
  lcd.drawRect(14, 14, 50, 50, (uint16_t)0x1F); // Draw a rectangle outline in blue


// When using int8_t or uint8_t type, the color is treated as 8-bit RGB332.
  uint8_t blue = 0x03;
  lcd.fillRect(20, 20, 20, 20, (uint8_t)0xE0);  // Draw a filled rectangle in red
  lcd.fillRect(30, 30, 20, 20, (uint8_t)0x1C);  // Draw a filled rectangle in green
  lcd.fillRect(40, 40, 20, 20, blue);           // Draw a filled rectangle in blue


// The color argument in drawing functions can be omitted.
// When omitted, the color set by setColor or the last used color is used as the drawing color.
// When drawing repeatedly with the same color, omitting it runs slightly faster.
  lcd.setColor(0xFF0000U);                        // Set drawing color to red
  lcd.fillCircle ( 40, 80, 20    );               // Fill circle in red
  lcd.fillEllipse( 80, 40, 10, 20);               // Fill ellipse in red
  lcd.fillArc    ( 80, 80, 20, 10, 0, 90);        // Fill arc in red
  lcd.fillTriangle(80, 80, 60, 80, 80, 60);       // Fill triangle in red
  lcd.setColor(0x0000FFU);                        // Set drawing color to blue
  lcd.drawCircle ( 40, 80, 20    );               // Draw circle outline in blue
  lcd.drawEllipse( 80, 40, 10, 20);               // Draw ellipse outline in blue
  lcd.drawArc    ( 80, 80, 20, 10, 0, 90);        // Draw arc outline in blue
  lcd.drawTriangle(60, 80, 80, 80, 80, 60);       // Draw triangle outline in blue
  lcd.setColor(0x00FF00U);                        // Set drawing color to green
  lcd.drawBezier( 60, 80, 80, 80, 80, 60);        // Draw quadratic Bezier curve in green
  lcd.drawBezier( 60, 80, 80, 20, 20, 80, 80, 60);// Draw cubic Bezier curve in green

// The color specification cannot be omitted for drawGradientLine, which draws gradient lines.
  lcd.drawGradientLine( 0, 80, 80, 0, 0xFF0000U, 0x0000FFU);// Gradient line from red to blue

  delay(1000);

// clear or fillScreen can be used to fill the entire screen.
// fillScreen is the same as calling fillRect for the entire screen, and the color is treated as a drawing color.
  lcd.fillScreen(0xFFFFFFu);  // Fill with white
  lcd.setColor(0x00FF00u);    // Set drawing color to green
  lcd.fillScreen();           // Fill with green

// clear maintains a separate background color from the drawing functions.
// The background color is rarely used, but it is also used to fill gaps when using the scroll feature.
  lcd.clear(0xFFFFFFu);       // Set background color to white and fill
  lcd.setBaseColor(0x000000u);// Set background color to black
  lcd.clear();                // Fill with black


// SPI bus acquisition and release are done automatically when drawing functions are called,
// but if drawing speed is a priority, use startWrite and endWrite before and after drawing operations.
// This suppresses SPI bus acquisition/release and improves speed.
// For e-paper (EPD), drawing after startWrite() is reflected on the screen when endWrite() is called.
  lcd.drawLine(0, 1, 39, 40, red);       // Acquire SPI bus, draw line, release SPI bus
  lcd.drawLine(1, 0, 40, 39, blue);      // Acquire SPI bus, draw line, release SPI bus
  lcd.startWrite();                      // Acquire SPI bus
  lcd.drawLine(38, 0, 0, 38, 0xFFFF00U); // Draw line
  lcd.drawLine(39, 1, 1, 39, 0xFF00FFU); // Draw line
  lcd.drawLine(40, 2, 2, 40, 0x00FFFFU); // Draw line
  lcd.endWrite();                        // Release SPI bus


// startWrite and endWrite internally count the number of calls,
// and only the first and last calls actually perform the operation.
// Always use startWrite and endWrite in pairs.
// (If it's acceptable to monopolize the SPI bus, you can call startWrite once at the beginning and never call endWrite.)
  lcd.startWrite();     // Count +1, acquire SPI bus
  lcd.startWrite();     // Count +1
  lcd.startWrite();     // Count +1
  lcd.endWrite();       // Count -1
  lcd.endWrite();       // Count -1
  lcd.endWrite();       // Count -1, release SPI bus
  lcd.endWrite();       // Does nothing
// If endWrite is called excessively, it does nothing and the count will not go below zero.


// If you want to forcibly release/acquire the SPI bus regardless of the startWrite count state,
// use endTransaction and beginTransaction.
// The count is not cleared, so be careful to keep things consistent.
  lcd.startWrite();       // Count +1, acquire SPI bus
  lcd.startWrite();       // Count +1
  lcd.drawPixel(0, 0);    // Draw
  lcd.endTransaction();   // Release SPI bus
  // Other SPI devices can be used here
  // When using another device on the same SPI bus (e.g., SD card),
  // make sure the SPI bus is released first.
  lcd.beginTransaction(); // Acquire SPI bus
  lcd.drawPixel(0, 0);    // Draw
  lcd.endWrite();         // Count -1
  lcd.endWrite();         // Count -1, release SPI bus



// In addition to drawPixel, there is a writePixel function for drawing points.
// While drawPixel acquires the SPI bus as needed,
// writePixel does not check the SPI bus state.
  lcd.startWrite();  // Acquire SPI bus
  for (uint32_t x = 0; x < 128; ++x) {
    for (uint32_t y = 0; y < 128; ++y) {
      lcd.writePixel(x, y, lcd.color888( x*2, x + y, y*2));
    }
  }
  lcd.endWrite();    // Release SPI bus
// All functions whose names start with write~ require an explicit call to startWrite beforehand.
// This applies to writePixel, writeFastVLine, writeFastHLine, and writeFillRect.

  delay(1000);

// The same drawing functions can be used for drawing to sprites (off-screen buffers).
// First, specify the sprite's color depth with setColorDepth. (If omitted, 16 is used.)
//sprite.setColorDepth(1);   // Set to 1-bit ( 2 color) palette mode
//sprite.setColorDepth(2);   // Set to 2-bit ( 4 color) palette mode
//sprite.setColorDepth(4);   // Set to 4-bit (16 color) palette mode
//sprite.setColorDepth(8);   // Set to 8-bit RGB332
//sprite.setColorDepth(16);  // Set to 16-bit RGB565
  sprite.setColorDepth(24);  // Set to 24-bit RGB888


// * After setting setColorDepth(8), calling createPalette() enables 256-color palette mode
// sprite.createPalette();


// Use createSprite to specify width and height and allocate memory.
// Memory consumption is proportional to color depth and area. Be careful as allocation will fail if it's too large.
  sprite.createSprite(65, 65); // Create a sprite with width 65 and height 65.

  for (uint32_t x = 0; x < 64; ++x) {
    for (uint32_t y = 0; y < 64; ++y) {
      sprite.drawPixel(x, y, lcd.color888(3 + x*4, (x + y)*2, 3 + y*4));  // Draw to sprite
    }
  }
  sprite.drawRect(0, 0, 65, 65, 0xFFFF);

// The created sprite can be output at any coordinate using pushSprite.
// The output destination is the LGFX passed as an argument when creating the instance.
  sprite.pushSprite(64, 0);        // Draw the sprite at lcd coordinates 64,0

// If no drawing destination pointer was passed when creating the sprite instance,
// or if there are multiple LGFXs, you can specify the output destination as the first argument to pushSprite.
  sprite.pushSprite(&lcd, 0, 64);  // Draw the sprite at lcd coordinates 0,64

  delay(1000);

  // pushRotateZoom can draw the sprite with rotation and scaling.
  // The coordinate set by setPivot is treated as the rotation center, and it is drawn so that the rotation center is positioned at the destination coordinate.
  sprite.setPivot(32, 32);    // Treat coordinate 32,32 as the center
  int32_t center_x = lcd.width()/2;
  int32_t center_y = lcd.height()/2;
  lcd.startWrite();
  for (int angle = 0; angle <= 360; ++angle) {
    sprite.pushRotateZoom(center_x, center_y, angle, 2.5, 3); // Draw at screen center with the given angle, 2.5x width, 3x height

    if ((angle % 36) == 0) lcd.display(); // Update the display for e-paper once every 36 iterations
  }
  lcd.endWrite();

  delay(1000);

  // To free the memory of a sprite that is no longer used, use deleteSprite.
  sprite.deleteSprite();

  // After deleteSprite, the same instance can be reused.
  sprite.setColorDepth(4);     // Set to 4-bit (16 color) palette mode
  sprite.createSprite(65, 65);

  // In palette mode sprites, the color argument of drawing functions is treated as a palette index.
  // When drawing with pushSprite etc., the actual drawing color is determined by referencing the palette.

  // In 4-bit (16 color) palette mode, palette indices 0 to 15 are available.
  // The default palette colors are: 0 is black, the last index is white, with a gradient from 0 to the last index.
  // To set palette colors, use setPaletteColor.
  sprite.setPaletteColor(1, 0x0000FFU);    // Set palette 1 to blue
  sprite.setPaletteColor(2, 0x00FF00U);    // Set palette 2 to green
  sprite.setPaletteColor(3, 0xFF0000U);    // Set palette 3 to red

  sprite.fillRect(10, 10, 45, 45, 1);             // Fill rectangle with palette 1
  sprite.fillCircle(32, 32, 22, 2);               // Fill circle with palette 2
  sprite.fillTriangle(32, 12, 15, 43, 49, 43, 3); // Fill triangle with palette 3

  // The last argument of pushSprite can specify a color to treat as transparent.
  sprite.pushSprite( 0,  0, 0);  // Draw sprite with palette 0 as transparent
  sprite.pushSprite(65,  0, 1);  // Draw sprite with palette 1 as transparent
  sprite.pushSprite( 0, 65, 2);  // Draw sprite with palette 2 as transparent
  sprite.pushSprite(65, 65, 3);  // Draw sprite with palette 3 as transparent

  delay(5000);

  lcd.startWrite(); // Call startWrite() here to keep the SPI bus occupied.
}

void loop(void)
{
  static int count = 0;
  static int a = 0;
  static int x = 0;
  static int y = 0;
  static float zoom = 3;
  ++count;
  if ((a += 1) >= 360) a -= 360;
  if ((x += 2) >= lcd.width()) x -= lcd.width();
  if ((y += 1) >= lcd.height()) y -= lcd.height();
  sprite.setPaletteColor(1, lcd.color888( 0, 0, count & 0xFF));
  sprite.setPaletteColor(2, lcd.color888( 0,~count & 0xFF, 0));
  sprite.setPaletteColor(3, lcd.color888( count & 0xFF, 0, 0));

  sprite.pushRotateZoom(x, y, a, zoom, zoom, 0);

  if ((count % 100) == 0) lcd.display(); // Update the display for e-paper once every 100 iterations
}
```


# Notes and Limitations
## How to Coexist with M5Stack.h (M5StickC.h)  
### Method 1
Write #include <LovyanGFX.hpp> after #include <M5Stack.h>.  
Do not use M5.Lcd; instead, create a separate LGFX instance and use that.  
### Method 2
By using [ESP32-Chimera-Core](https://github.com/tobozo/ESP32-Chimera-Core), M5.Lcd becomes LovyanGFX.  


Motivation Behind This Library
----------------
TFT_eSPI is a great library. However, it is structurally complex because it targets multiple architectures, making it very difficult to add required functions such as ESP-IDF support and 18-bit color support.  
LovyanGFX has been created to add these features and optimize performance.  


Acknowledgements
----------------
Thanks to [Bodmer](https://github.com/Bodmer/), author of the [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) library, for the inspiration to create this library.  
Thanks to [Adafruit Industries](https://github.com/adafruit/) for publishing [AdafruitGFX](https://github.com/adafruit/Adafruit-GFX-Library), which is the basis for TFT_eSPI.  
Thanks to [ChaN](http://elm-chan.org/), author of [TJpgDec](http://elm-chan.org/fsw/tjpgd/00index.html) (Tiny JPEG Decompressor).  
Thanks to [kikuchan](https://github.com/kikuchan/), author of [Pngle](https://github.com/kikuchan/pngle) (PNG Loader for Embedding).  
Thanks to [Richard Moore](https://github.com/ricmoo/), author of [QRCode](https://github.com/ricmoo/QRCode/) (QR code generation library).  
Thanks to [ciniml](https://github.com/ciniml), for many technical tips and help with validation in the ESP-IDF environment.  
Thanks to [mongonta0716](https://github.com/mongonta0716), for verifying the work from the beginning of the development with many bugs and for his advice.  
Thanks to [tobozo](https://github.com/tobozo), for testing it on various boards, translating it into English and giving me a lot of advice.  
Thanks to [TANAKA Masayuki](https://github.com/tanakamasayuki), for creating the font data.  
Thanks to [YAMANEKO](https://github.com/yamamaya), for creating the [lgfxFontSubsetGenerator](https://github.com/yamamaya/lgfxFontSubsetGenerator).  
Thanks to [yasuhirok](https://github.com/yasuhirok-git), for add Raspberry pi pico (RP2040) support.  
Thanks to [IAMLIUBO](https://github.com/imliubo), for add Linux FrameBuffer support.  
Thanks to [rossum](https://github.com/rossumur) and [Roger Cheng](https://github.com/Roger-random), published the project to output a composite video signal from ESP32.


Included Libraries  
----------------
[TJpgDec](http://elm-chan.org/fsw/tjpgd/00index.html)  [ChaN](http://elm-chan.org/)  
[Pngle](https://github.com/kikuchan/pngle)  [kikuchan](https://github.com/kikuchan/)  
[QRCode](https://github.com/ricmoo/QRCode/)  [Richard Moore](https://github.com/ricmoo/) and [Nayuki](https://www.nayuki.io/page/qr-code-generator-library)  


Credits
----------------
  - Inspiration: [Bodmer](https://github.com/Bodmer)
  - Author: [lovyan03](https://github.com/lovyan03)
  - Contributors:
    - [ciniml](https://github.com/ciniml)
    - [mongonta0716](https://github.com/mongonta0716)
    - [tobozo](https://github.com/tobozo)


License
----------------
main : [FreeBSD](license.txt)  
TJpgDec : [original](src/lgfx/utility/lgfx_tjpgd.c) ChaN  
Pngle : [MIT](https://github.com/kikuchan/pngle/blob/master/LICENSE) kikuchan  
QRCode : [MIT](https://github.com/ricmoo/QRCode/blob/master/LICENSE.txt) Richard Moore and Nayuki  
result : [MIT](https://github.com/bitwizeshift/result/blob/master/LICENSE) Matthew Rodusek  
GFX font and GLCD font : [2-clause BSD](https://github.com/adafruit/Adafruit-GFX-Library/blob/master/license.txt) Adafruit Industries  
Font 2,4,6,7,8 :  [FreeBSD](https://github.com/Bodmer/TFT_eSPI/blob/master/license.txt) Bodmer  
converted IPA font : [IPA Font License](src/lgfx/Fonts/IPA/IPA_Font_License_Agreement_v1.0.txt) IPA  
efont : [3-clause BSD](src/lgfx/Fonts/efont/COPYRIGHT.txt) The Electronic Font Open Laboratory  
TomThumb font : [3-clause BSD](src/lgfx/Fonts/GFXFF/TomThumb.h) Brian J. Swetland / Vassilii Khachaturov / Dan Marks  


Unimplemented Requests
----------------
  - Displays
    - SEPS525
    - LT7680A / LT7685
    - RA8873 / RA8876

