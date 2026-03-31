// Basic usage

// * If you are using a supported board in an environment other than ArduinoIDE,
// or if the supported board is not available in the board manager (e.g. TTGO T-Wristband, ESP-WROVER-KIT),
// please add the define LGFX_~ definition before including LovyanGFX.hpp.

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
// #define LGFX_ESP32_S3_BOX_V3               // Espressif ESP32-S3-BOX-3/3B
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

  #define LGFX_AUTODETECT // Auto-detect (D-duino-32 XS, WT32-SC01, PyBadge are excluded from auto-detection because their panel IDs cannot be read)

// By defining multiple board types or defining LGFX_AUTODETECT, the board will be automatically detected at runtime.




// Include the header.
#include <LovyanGFX.hpp>

#include <LGFX_AUTODETECT.hpp>  // Prepares the "LGFX" class
// #include <lgfx_user/LGFX_ESP32_sample.hpp> // Or prepare your own custom LGFX class

static LGFX lcd;                 // Create an instance of LGFX.
static LGFX_Sprite sprite(&lcd); // To use sprites, create an instance of LGFX_Sprite.

// If you are currently using TFT_eSPI and want to minimize source code changes, you can use this header.
// #include <LGFX_TFT_eSPI.hpp>
// static TFT_eSPI lcd;               // TFT_eSPI is defined as an alias for LGFX.
// static TFT_eSprite sprite(&lcd);   // TFT_eSprite is defined as an alias for LGFX_Sprite.


// For configurations not covered by supported boards, see examples/HowToUse/2_user_setting.ino.
// Configuration examples are also available in the src/lgfx_user folder.


void setup(void)
{
// First, call the initialization function.
  lcd.init();


// Set the rotation from 4 directions: 0-3. (Using 4-7 will flip the display upside down.)
  lcd.setRotation(1);


// Set the backlight brightness in the range 0-255.
  lcd.setBrightness(128);


// Set the color mode as needed. (Default is 16)
// 16-bit requires less SPI data and runs faster, but red and blue gradation is limited to 5 bits.
// 24-bit requires more SPI data, but produces smoother color gradation.
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
  drawTriangle  ( x0, y0, x1, y1, x2, y2, color); // Triangle outline from three points
  fillTriangle  ( x0, y0, x1, y1, x2, y2, color); // Filled triangle from three points
  drawBezier    ( x0, y0, x1, y1, x2, y2, color); // Bezier curve through three points
  drawBezier    ( x0, y0, x1, y1, x2, y2, x3, y3, color); // Bezier curve through four points
  drawArc       ( x, y, r0, r1, angle0, angle1, color);   // Arc outline
  fillArc       ( x, y, r0, r1, angle0, angle1, color);   // Filled arc
*/


// For example, when drawing a point with drawPixel, the three arguments are: X coordinate, Y coordinate, and color.
  lcd.drawPixel(0, 0, 0xFFFF); // Draw a white pixel at coordinates 0,0


// Color code generation functions are provided and can be used to specify colors.
// Arguments are red, green, blue, each specified in the range 0-255.
// Using color888 is recommended to prevent loss of color information.
  lcd.drawFastVLine(2, 0, 100, lcd.color888(255,   0,   0)); // Draw a vertical line in red
  lcd.drawFastVLine(4, 0, 100, lcd.color565(  0, 255,   0)); // Draw a vertical line in green
  lcd.drawFastVLine(6, 0, 100, lcd.color332(  0,   0, 255)); // Draw a vertical line in blue


// When not using color code generation functions, colors are specified as follows.
// RGB888 24-bit: use uint32_t type
// RGB565 16-bit: use uint16_t or int32_t type
// RGB332  8-bit: use uint8_t type

// When using uint32_t type, it is treated as 24-bit RGB888.
// You can write red, green, blue in order using two hex digits each.
// Use a uint32_t variable, append U suffix, or cast to uint32_t.
  uint32_t red = 0xFF0000;
  lcd.drawFastHLine(0, 2, 100, red);            // Draw a horizontal line in red
  lcd.drawFastHLine(0, 4, 100, 0x00FF00U);      // Draw a horizontal line in green
  lcd.drawFastHLine(0, 6, 100, (uint32_t)0xFF); // Draw a horizontal line in blue


// When using uint16_t or int32_t type, it is treated as 16-bit RGB565.
// Without special type notation, values are treated as int32_t, so this format applies by default.
// (This behavior is maintained for compatibility with AdafruitGFX and TFT_eSPI.)
  uint16_t green = 0x07E0;
  lcd.drawRect(10, 10, 50, 50, 0xF800);         // Draw a rectangle outline in red
  lcd.drawRect(12, 12, 50, 50, green);          // Draw a rectangle outline in green
  lcd.drawRect(14, 14, 50, 50, (uint16_t)0x1F); // Draw a rectangle outline in blue


// When using int8_t or uint8_t type, it is treated as 8-bit RGB332.
  uint8_t blue = 0x03;
  lcd.fillRect(20, 20, 20, 20, (uint8_t)0xE0);  // Draw a filled rectangle in red
  lcd.fillRect(30, 30, 20, 20, (uint8_t)0x1C);  // Draw a filled rectangle in green
  lcd.fillRect(40, 40, 20, 20, blue);           // Draw a filled rectangle in blue


// The color argument in drawing functions can be omitted.
// When omitted, the color set by setColor or the last used color will be used as the drawing color.
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

// The color arguments for drawGradientLine cannot be omitted.
  lcd.drawGradientLine( 0, 80, 80, 0, 0xFF0000U, 0x0000FFU);// Gradient line from red to blue

  delay(1000);

// clear or fillScreen can be used to fill the entire screen.
// fillScreen is equivalent to calling fillRect for the entire screen; color specification is treated as drawing color.
  lcd.fillScreen(0xFFFFFFu);  // Fill with white
  lcd.setColor(0x00FF00u);    // Set drawing color to green
  lcd.fillScreen();           // Fill with green

// clear is separate from drawing functions and maintains a background color.
// The background color is rarely used, but it is also used to fill gaps when using the scroll feature.
  lcd.clear(0xFFFFFFu);       // Set background color to white and fill
  lcd.setBaseColor(0x000000u);// Set background color to black
  lcd.clear();                // Fill with black


// SPI bus acquisition and release are performed automatically when drawing functions are called.
// For better drawing speed, use startWrite and endWrite before and after drawing operations.
// This suppresses SPI bus acquisition and release, improving speed.
// For e-paper (EPD), drawing after startWrite() is reflected on the screen when endWrite() is called.
  lcd.drawLine(0, 1, 39, 40, red);       // Acquire SPI bus, draw line, release SPI bus
  lcd.drawLine(1, 0, 40, 39, blue);      // Acquire SPI bus, draw line, release SPI bus
  lcd.startWrite();                      // Acquire SPI bus
  lcd.drawLine(38, 0, 0, 38, 0xFFFF00U); // Draw line
  lcd.drawLine(39, 1, 1, 39, 0xFF00FFU); // Draw line
  lcd.drawLine(40, 2, 2, 40, 0x00FFFFU); // Draw line
  lcd.endWrite();                        // Release SPI bus


// startWrite and endWrite maintain an internal call count,
// so only the first and last calls actually take effect when called repeatedly.
// Always use startWrite and endWrite in matching pairs.
// (If it is acceptable to keep the SPI bus occupied, you can call startWrite once at the beginning and never call endWrite.)
  lcd.startWrite();     // Count+1, acquire SPI bus
  lcd.startWrite();     // Count+1
  lcd.startWrite();     // Count+1
  lcd.endWrite();       // Count-1
  lcd.endWrite();       // Count-1
  lcd.endWrite();       // Count-1, release SPI bus
  lcd.endWrite();       // Does nothing
// If endWrite is called excessively, it simply does nothing, and the count will not go negative.


// To forcibly release/acquire the SPI bus regardless of the startWrite count,
// use endTransaction/beginTransaction.
// The count is not cleared, so be careful to keep things consistent.
  lcd.startWrite();       // Count+1, acquire SPI bus
  lcd.startWrite();       // Count+1
  lcd.drawPixel(0, 0);    // Draw
  lcd.endTransaction();   // Release SPI bus
  // Other SPI devices can be used here.
  // When using another device on the same SPI bus (e.g. SD card),
  // make sure the SPI bus is released first.
  lcd.beginTransaction(); // Acquire SPI bus
  lcd.drawPixel(0, 0);    // Draw
  lcd.endWrite();         // Count-1
  lcd.endWrite();         // Count-1, release SPI bus



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
// All functions starting with write~ require an explicit call to startWrite beforehand.
// This applies to writePixel, writeFastVLine, writeFastHLine, and writeFillRect.

  delay(1000);

// Drawing to sprites (off-screen) uses the same drawing functions.
// First, specify the sprite's color depth with setColorDepth. (If omitted, 16 is used by default.)
//sprite.setColorDepth(1);   // Set to 1-bit ( 2 color) palette mode
//sprite.setColorDepth(2);   // Set to 2-bit ( 4 color) palette mode
//sprite.setColorDepth(4);   // Set to 4-bit (16 color) palette mode
//sprite.setColorDepth(8);   // Set to 8-bit RGB332
//sprite.setColorDepth(16);  // Set to 16-bit RGB565
  sprite.setColorDepth(24);  // Set to 24-bit RGB888


// * Calling createPalette() after setColorDepth(8) enables 256-color palette mode.
// sprite.createPalette();


// Use createSprite to allocate memory by specifying width and height.
// Memory consumption is proportional to color depth and area. Be careful as allocation will fail if the size is too large.
  sprite.createSprite(65, 65); // Create a sprite with width 65 and height 65.

  for (uint32_t x = 0; x < 64; ++x) {
    for (uint32_t y = 0; y < 64; ++y) {
      sprite.drawPixel(x, y, lcd.color888(3 + x*4, (x + y)*2, 3 + y*4));  // Draw to sprite
    }
  }
  sprite.drawRect(0, 0, 65, 65, 0xFFFF);

// Created sprites can be drawn at any coordinate using pushSprite.
// The output destination is the LGFX instance passed as an argument during instance creation.
  sprite.pushSprite(64, 0);        // Draw sprite at lcd coordinates 64,0

// If no drawing destination pointer was passed during sprite instance creation,
// or if there are multiple LGFX instances, you can specify the destination as the first argument to pushSprite.
  sprite.pushSprite(&lcd, 0, 64);  // Draw sprite at lcd coordinates 0,64

  delay(1000);

  // pushRotateZoom can draw a sprite with rotation and scaling.
  // The coordinates set with setPivot are treated as the rotation center, and the sprite is drawn so that the rotation center is positioned at the destination coordinates.
  sprite.setPivot(32, 32);    // Set coordinates 32,32 as the center
  int32_t center_x = lcd.width()/2;
  int32_t center_y = lcd.height()/2;
  lcd.startWrite();
  for (int angle = 0; angle <= 360; ++angle) {
    sprite.pushRotateZoom(center_x, center_y, angle, 2.5, 3); // Draw at screen center with given angle, 2.5x width, 3x height

    if ((angle % 36) == 0) lcd.display(); // Update e-paper display once every 36 iterations
  }
  lcd.endWrite();

  delay(1000);

  // To free the memory of a sprite that is no longer needed, use deleteSprite.
  sprite.deleteSprite();

  // After deleteSprite, the same instance can be reused.
  sprite.setColorDepth(4);     // Set to 4-bit (16 color) palette mode
  sprite.createSprite(65, 65);

  // In palette mode sprites, the color argument of drawing functions is treated as a palette index.
  // When drawing with pushSprite etc., the palette is referenced to determine the actual drawing color.

  // In 4-bit (16 color) palette mode, palette indices 0-15 are available.
  // The initial palette colors are: 0 is black, the last index is white, with a gradient from 0 to the last index.
  // Use setPaletteColor to set palette colors.
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

  if ((count % 100) == 0) lcd.display(); // Update e-paper display once every 100 iterations
}
