
// When using with M5Paper or CoreInk libraries, include them before LovyanGFX.hpp.
// If you use it with M5Paper or CoreInk libraries, include it before LovyanGFX.hpp.
// #include <M5EPD.h>
// #include <M5CoreInk.h>

// Write the define corresponding to the board you are using.
// (This can be omitted if you have selected the appropriate board in the board manager.)
// #define LGFX_M5PAPER
// #define LGFX_M5STACK_COREINK
   #define LGFX_AUTODETECT      // When using auto-detection, this line alone is sufficient.

// Include LovyanGFX.hpp after the board define.
#include <LovyanGFX.hpp>

LGFX gfx;
LGFX_Sprite sp(&gfx);

int w = 200;
int h = 200;

#ifdef min
#undef min
#endif

void setup(void)
{
// M5.begin();

  gfx.init();   // Perform initialization. This is common for both LCD and EPD.

  gfx.setRotation(0);  // Rotation setting is common for both LCD and EPD. 0-3 rotates 90 degrees clockwise each step. 4-7 adds vertical flip.

  w = gfx.width();
  h = gfx.height();

  gfx.setBrightness(128); // Backlight brightness setting has no effect on EPD.

// You can set the EPD operating mode. Change it as needed depending on the drawing purpose.
// * On M5Stack CoreInk, there is no difference except for epd_quality.
  gfx.setEpdMode(epd_mode_t::epd_fastest);  // Fastest refresh, no black/white inversion, prone to ghosting
  gfx.setEpdMode(epd_mode_t::epd_fast);     // Fast refresh, no black/white inversion, prone to ghosting
  gfx.setEpdMode(epd_mode_t::epd_text);     // High quality refresh, brief black/white inversion (for white backgrounds)
  gfx.setEpdMode(epd_mode_t::epd_quality);  // High quality refresh, brief black/white inversion

// The characteristics of each mode on M5Paper (IT8951) are as follows:
// epd_fastest = DU4  Refresh time 120msec  Can only draw pure white and black; intermediate grayscale is not drawn. Also cannot update areas displaying intermediate grayscale.
// epd_fast    = DU   Refresh time 260msec  Can only draw pure white and black; intermediate grayscale is not drawn.
// epd_text    = GL16 Refresh time 450msec  Can draw with 16 levels of grayscale. For white background / black text use cases.
// epd_quality = GC16 Refresh time 450msec  Can draw with 16 levels of grayscale. For image use cases.


// All drawing functions can be used the same way as with LCD.
// Colors can also be specified the same way as LCD, but they are automatically converted to grayscale.
// (The grayscale conversion ratio is R1:G2:B1. Green appears slightly brighter.)

  int rectwidth = std::min(w, h) / 2;
  gfx.fillTriangle( w / 2, 0, 0, h - 1, w - 1, h - 1, TFT_RED);
  gfx.fillCircle(w/2, h/2, rectwidth, TFT_GREEN);
  gfx.fillRect((w-rectwidth)/2, (h-rectwidth)/2, rectwidth, rectwidth, TFT_BLUE);

  delay(3000);


// Wrapping drawing operations with startWrite/endWrite allows them to be applied to the screen all at once at the endWrite() timing.
// Alternatively, the screen can also be updated when display() is called.

  gfx.startWrite(); // Suppress immediate screen updates of drawing content.

  for (int i = 0; i < 20; ++i)
  {
    gfx.drawLine(i * w / 20, 0, w - 1, i * h / 20, TFT_BLACK);  // Not yet reflected on screen at this point.
    gfx.drawLine(0, i * h / 20, i * w / 20, h - 1, TFT_BLACK);
  }

  gfx.endWrite();   // Screen is updated here.

  delay(3000);

  gfx.startWrite(); // Suppress immediate screen updates of drawing content.

  for (int i = 0; i < 5; i++)
  {
    for (int j = 0; j < 20; j++)
    {
      gfx.fillRect(random(w-20), random(h-20), 20, 20, random(65535));  // Not yet reflected on screen at this point.
    }
    gfx.display();  // Screen is updated here.
  }

  gfx.endWrite();   // If the screen has already been updated, nothing happens at this point.

  delay(3000);

// * Precisely, the screen is only updated when display() is called,
// but a mechanism is provided to call display() when the SPI bus is released, and it is enabled by default.
// Due to this mechanism, the screen is automatically updated immediately after calling drawing functions or endWrite.
// You can enable/disable this automatic display call using setAutoDisplay(bool).

  gfx.setAutoDisplay(false);  // Disable automatic display update. (From here on, the screen is not updated until display() is called.)

  gfx.setFont(&fonts::Font4);
  gfx.setTextColor(TFT_BLACK, TFT_WHITE); // Set text color to black, background color to white
  gfx.setTextDatum(textdatum_t::top_center);
  for (int i = 0; i < 10; i++)
  {
    gfx.drawString("Hello World !!", w / 2, i * h / 10);
  }
  gfx.qrcode("Hello world !", (w-rectwidth)/2, (h-rectwidth)/2, rectwidth);

  gfx.display();   // Screen is updated here.

  gfx.setAutoDisplay(true);  // Enable automatic display update.

  delay(3000);

  gfx.fillScreen(TFT_WHITE);

// On M5Paper (IT8951), using epd_quality/epd_text enables 16-level grayscale display,
// but when using epd_fast/epd_fastest, it is limited to only 2-level black and white.
// Even in this case, LovyanGFX's tile pattern processing can simulate 17 levels of grayscale.

  gfx.setEpdMode(epd_mode_t::epd_quality); // Set to high quality refresh mode (subsequent drawing uses grayscale)
  gfx.startWrite();
  for (int i = 0; i < 16; i++)
  {
    int level = 8 + i * 16;
    gfx.fillRect(i * w / 16, 0, w / 16 + 1, h / 2, gfx.color888(level, level, level));
  }
  gfx.display();  // Display update here uses high quality mode.

  gfx.waitDisplay(); // Wait for EPD display update to complete.
  // * To check if a display update is in progress without waiting, use displayBusy().
  //   Use this when you want to perform drawing between other operations.
  // Example: while (gfx.displayBusy()) delay(10); // Replace delay with some other processing


  gfx.setEpdMode(epd_mode_t::epd_fast);  // Set to fast refresh mode (subsequent drawing uses 2-level black and white)
  for (int i = 0; i < 17; i++)
  {
    int level = std::min(255, i * 16);
    gfx.fillRect(i * w / 17, h / 2, w / 17+1, h / 2, gfx.color888(level, level, level));
  }
  gfx.display();  // Drawing here uses fast mode.

  delay(3000);

  // In high quality mode on M5Paper (IT8951), 16-level grayscale + tile pattern processing can express 241 levels.
  gfx.setEpdMode(epd_mode_t::epd_quality);

  for (int i = 0; i < 256; ++i)
  {
    gfx.fillRect(0, (i * h) / 256, w, 4, gfx.color888(i, i, i));
  }
  gfx.display();

  gfx.endWrite();

  delay(3000);

  // On M5Paper (IT8951), display updates can be performed on multiple areas simultaneously.
  gfx.startWrite();
  for (int i = 0; i < 16; ++i)
  {
    std::int32_t y1 = (i * h) / 16, y2 = ((i + 1) * h) / 16;
    std::int32_t x1 = 0, x2 = 0;
    for (int j = 0; j < 16; ++j)
    {
      x2 = ((j + 1) * w) / 16;
      std::int_fast8_t l = 255 - (i * 16 + j);
      gfx.fillRect(x1, y1, x2 - x1, y2 - y1, gfx.color888(l, l, l));
      x1 = x2;
      if ((j & 7) == 7)  gfx.display(); // Perform display update every 8 times
    }
  }
  gfx.endWrite();

  // * Be careful not to draw to areas that are currently being updated.
  //   If the content is changed during a display update, the drawing will not render correctly.

  delay(3000);

  gfx.fillScreen(TFT_WHITE);

  // When drawing to an area that overlaps with the previous display update range,
  // LovyanGFX internally waits for the display update to complete.
  // Therefore, the display will not become corrupted without any special consideration.
  gfx.setEpdMode(epd_mode_t::epd_quality);
  gfx.fillRect(0, 0, w/2, h/2, TFT_BLUE);  // These drawings overlap in area, but
  gfx.fillRect(0, 0, w/3, h/3, TFT_YELLOW);// the waiting mechanism for display updates works,
  gfx.fillRect(0, 0, w/4, h/4, TFT_BLUE);  // so the drawing is not corrupted without special handling
  gfx.fillRect(0, 0, w/5, h/5, TFT_YELLOW);
  gfx.fillRect(0, 0, w/6, h/6, TFT_BLUE);
  gfx.fillRect(0, 0, w/7, h/7, TFT_YELLOW);

  gfx.waitDisplay();

  // However, the automatic check is not perfect. Since it compares against the "previous display update range",
  // if a drawing to a different area is inserted in between, the range check will not function sufficiently.
  gfx.fillRect(w/2, 0, w/2, h/2, TFT_BLUE);  // If non-overlapping drawings alternate,
  gfx.drawPixel(0, 0);                       // the display update waiting mechanism fails,
  gfx.fillRect(w/2, 0, w/3, h/3, TFT_YELLOW);// and the drawing will not render correctly.
  gfx.drawPixel(0, 0);
  gfx.fillRect(w/2, 0, w/4, h/4, TFT_BLUE);  // * Note: even though the same colors as the previous example are used,
  gfx.drawPixel(0, 0);                       // the displayed grayscale levels may differ,
  gfx.fillRect(w/2, 0, w/5, h/5, TFT_YELLOW);// confirming that the drawing is not done correctly.
  gfx.drawPixel(0, 0);
  gfx.fillRect(w/2, 0, w/6, h/6, TFT_BLUE);
  gfx.drawPixel(0, 0);
  gfx.fillRect(w/2, 0, w/7, h/7, TFT_YELLOW);

  gfx.waitDisplay();

  // Also, when the display update mode is epd_fastest, the update range check is skipped to prioritize responsiveness.
  gfx.setEpdMode(epd_mode_t::epd_fastest);    // Set to fastest refresh mode.
  gfx.fillRect(0, h/2, w/2, h/2, TFT_BLUE);   // Subsequent drawings will not wait at all even if they overlap
  gfx.fillRect(0, h/2, w/3, h/3, TFT_YELLOW); // with the display update range.
  gfx.fillRect(0, h/2, w/4, h/4, TFT_BLUE);   // Therefore, consecutive drawings to overlapping areas
  gfx.fillRect(0, h/2, w/5, h/5, TFT_YELLOW); // will overwrite content during display update,
  gfx.fillRect(0, h/2, w/6, h/6, TFT_BLUE);   // resulting in unintended drawing results.
  gfx.fillRect(0, h/2, w/7, h/7, TFT_YELLOW);

  gfx.fillRect(w/2, h/2, w/2, h/2, TFT_BLUE);
  gfx.waitDisplay();                           // Wait with waitDisplay as needed.
  gfx.fillRect(w/2, h/2, w/3, h/3, TFT_YELLOW);// By waiting properly,
  gfx.waitDisplay();                           // drawing corruption can be prevented.
  gfx.fillRect(w/2, h/2, w/4, h/4, TFT_BLUE);
  gfx.waitDisplay();                           // Verify that the display result differs from the previous example.
  gfx.fillRect(w/2, h/2, w/5, h/5, TFT_YELLOW);
  gfx.waitDisplay();
  gfx.fillRect(w/2, h/2, w/6, h/6, TFT_BLUE);
  gfx.waitDisplay();
  gfx.fillRect(w/2, h/2, w/7, h/7, TFT_YELLOW);

  delay(3000);


  gfx.setEpdMode(epd_mode_t::epd_quality);

  gfx.fillScreen(TFT_WHITE);

  if (gfx.touch())  // You can check whether a touch controller is present by whether the touch function returns null.
  {
    gfx.startWrite();
    for (int i = 0; i < 1024; i++)
    {
      if ((i & 255) == 0)
      {
        gfx.setEpdMode(epd_mode_t::epd_fast);
        gfx.fillScreen(TFT_WHITE);
        gfx.setTextColor(TFT_BLACK, TFT_WHITE);
        gfx.setTextSize(3, 3);
        gfx.drawString("Touch Test", w / 2, h / 2);
        gfx.display();
        delay(100);
        gfx.waitDisplay();
        gfx.setEpdMode(epd_mode_t::epd_fastest);
      }
      delay(15);

      std::int32_t x, y, number = 0;
      while (gfx.getTouch(&x, &y, number))  // You can get the coordinates of active touches using getTouch.
      {
        gfx.fillCircle(x, y, 5, (std::uint32_t)(number * 0x333333u));
        gfx.display();
        ++number;
      }
    }
    gfx.endWrite();
  }

  gfx.setEpdMode(epd_mode_t::epd_fast);
  gfx.fillScreen(TFT_WHITE);

  sp.setColorDepth(4);
  if (!sp.createSprite(w / 2, h / 2)) sp.createSprite(w / 4, h / 4);
  sp.setFont(&fonts::Font8);
}

void loop(void)
{
  static int count = 0;

  if (0 == (count & 15))
  {
    switch ((count >> 4) & 3)
    {
    case 0:  gfx.setEpdMode(epd_mode_t::epd_quality);  break;
    case 1:  gfx.setEpdMode(epd_mode_t::epd_text   );  break;
    case 2:  gfx.setEpdMode(epd_mode_t::epd_fast   );  break;
    case 3:  gfx.setEpdMode(epd_mode_t::epd_fastest);  break;
    }
  }

  for (int y = 0; y < sp.height(); y += 4) {
    for (int x = 0; x < sp.width(); x += 4) {
      std::uint32_t val = ((x+count*4)|(y+count*4)) >> 4;
      sp.fillRect(x, y, 4, 4, val);
    }
  }

  int x = 2;
  int y = 2;
  sp.setTextColor(TFT_BLACK);
  sp.drawNumber(count, x-2, y-2);
  sp.drawNumber(count, x+2, y-2);
  sp.drawNumber(count, x+2, y+2);
  sp.drawNumber(count, x-2, y+2);
  sp.setTextColor(TFT_WHITE);
  sp.drawNumber(count, x, y);

  gfx.waitDisplay();
  sp.pushSprite(count & 1 ? w / 2 : 0, count & 2 ? h / 2 : 0);
  ++count;
}
