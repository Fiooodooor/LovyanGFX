#define LGFX_USE_V1
#include <LovyanGFX.hpp>

static LGFX lcd;
static LGFX_Sprite canvas(&lcd);       // Off-screen drawing buffer
static LGFX_Sprite clockbase(&canvas); // Clock face part
static LGFX_Sprite needle1(&canvas);   // Hour/minute hand part
static LGFX_Sprite shadow1(&canvas);   // Hour/minute hand shadow part
static LGFX_Sprite needle2(&canvas);   // Second hand part
static LGFX_Sprite shadow2(&canvas);   // Second hand shadow part

static constexpr uint64_t oneday = 86400000; // 1 day = 1000msec x 60sec x 60min x 24hour = 86400000
static uint64_t count = rand() % oneday;    // Current time (millisecond counter)
static int32_t width = 239;             // Clock image width and height
static int32_t halfwidth = width >> 1;  // Center coordinate of clock face
static auto transpalette = 0;           // Transparent color palette index
static float zoom;                      // Display scale factor

#ifdef min
#undef min
#endif

void setup(void)
{
  lcd.init();

  zoom = (float)(std::min(lcd.width(), lcd.height())) / width; // Adjust scale to fit the display

  lcd.setPivot(lcd.width() >> 1, lcd.height() >> 1); // Set center for clock drawing to screen center

  canvas.setColorDepth(lgfx::palette_4bit);  // Prepare each part in 4-bit palette mode
  clockbase.setColorDepth(lgfx::palette_4bit);
  needle1.setColorDepth(lgfx::palette_4bit);
  shadow1.setColorDepth(lgfx::palette_4bit);
  needle2.setColorDepth(lgfx::palette_4bit);
  shadow2.setColorDepth(lgfx::palette_4bit);
// The initial palette colors are a grayscale gradient,
// with index 0 being black (0,0,0) and index 15 being white (255,255,255).
// Indices 1-14 gradually increase in brightness from black to white.
//
// When using palettes, drawing functions specify palette indices 0-15 instead of colors

  canvas.createSprite(width, width); // Allocate memory
  clockbase.createSprite(width, width);
  needle1.createSprite(9, 119);
  shadow1.createSprite(9, 119);
  needle2.createSprite(3, 119);
  shadow2.createSprite(3, 119);

  canvas.fillScreen(transpalette); // Fill background with transparent color (can be omitted since memory is zero-filled after create)
  clockbase.fillScreen(transpalette);
  needle1.fillScreen(transpalette);
  shadow1.fillScreen(transpalette);

  clockbase.setTextFont(4);           // Change font type (for clock face text)
  clockbase.setTextDatum(lgfx::middle_center);
  clockbase.fillCircle(halfwidth, halfwidth, halfwidth    ,  6); // Fill the clock face background circle
  clockbase.drawCircle(halfwidth, halfwidth, halfwidth - 1, 15);
  for (int i = 1; i <= 60; ++i) {
    float rad = i * 6 * - 0.0174532925;              // Calculate tick mark coordinates on the clock face perimeter
    float cosy = - cos(rad) * (halfwidth * 10 / 11);
    float sinx = - sin(rad) * (halfwidth * 10 / 11);
    bool flg = 0 == (i % 5);      // Flag for every 5th tick mark
    clockbase.fillCircle(halfwidth + sinx + 1, halfwidth + cosy + 1, flg * 3 + 1,  4); // Draw tick marks
    clockbase.fillCircle(halfwidth + sinx    , halfwidth + cosy    , flg * 3 + 1, 12);
    if (flg) {                    // Draw numbers
      cosy = - cos(rad) * (halfwidth * 10 / 13);
      sinx = - sin(rad) * (halfwidth * 10 / 13);
      clockbase.setTextColor(1);
      clockbase.drawNumber(i/5, halfwidth + sinx + 1, halfwidth + cosy + 4);
      clockbase.setTextColor(15);
      clockbase.drawNumber(i/5, halfwidth + sinx    , halfwidth + cosy + 3);
    }
  }
  clockbase.setTextFont(7);

  needle1.setPivot(4, 100);  // Set the rotation center for hand parts
  shadow1.setPivot(4, 100);
  needle2.setPivot(1, 100);
  shadow2.setPivot(1, 100);

  for (int i = 6; i >= 0; --i) {  // Create the hand part images
    needle1.fillTriangle(4, - 16 - (i<<1), 8, needle1.height() - (i<<1), 0, needle1.height() - (i<<1), 15 - i);
    shadow1.fillTriangle(4, - 16 - (i<<1), 8, shadow1.height() - (i<<1), 0, shadow1.height() - (i<<1),  1 + i);
  }
  for (int i = 0; i < 7; ++i) {
    needle1.fillTriangle(4, 16 + (i<<1), 8, needle1.height() + 32 + (i<<1), 0, needle1.height() + 32 + (i<<1), 15 - i);
    shadow1.fillTriangle(4, 16 + (i<<1), 8, shadow1.height() + 32 + (i<<1), 0, shadow1.height() + 32 + (i<<1),  1 + i);
  }
  needle1.fillTriangle(4, 32, 8, needle1.height() + 64, 0, needle1.height() + 64, 0);
  shadow1.fillTriangle(4, 32, 8, shadow1.height() + 64, 0, shadow1.height() + 64, 0);
  needle1.fillRect(0, 117, 9, 2, 15);
  shadow1.fillRect(0, 117, 9, 2,  1);
  needle1.drawFastHLine(1, 117, 7, 12);
  shadow1.drawFastHLine(1, 117, 7,  4);

  needle1.fillCircle(4, 100, 4, 15);
  shadow1.fillCircle(4, 100, 4,  1);
  needle1.drawCircle(4, 100, 4, 14);

  needle2.fillScreen(9);
  shadow2.fillScreen(3);
  needle2.drawFastVLine(1, 0, 119, 8);
  shadow2.drawFastVLine(1, 0, 119, 1);
  needle2.fillRect(0, 99, 3, 3, 8);

  lcd.startWrite();

//  shadow1.pushSprite(&lcd,  0, 0); // For debugging: draw parts directly to LCD
//  needle1.pushSprite(&lcd, 10, 0);
//  shadow2.pushSprite(&lcd, 20, 0);
//  needle2.pushSprite(&lcd, 25, 0);
}

void update7Seg(int32_t hour, int32_t min)
{ // Draw the digital display section of the clock face
  int x = clockbase.getPivotX() - 69;
  int y = clockbase.getPivotY();
  clockbase.setCursor(x, y);
  clockbase.setTextColor(5);  // Draw "88:88" in the erase color
  clockbase.print("88:88");
  clockbase.setCursor(x, y);
  clockbase.setTextColor(12); // Draw hour:minute in the display color
  clockbase.printf("%02d:%02d", hour, min);
}

void drawDot(int pos, int palette)
{
  bool flg = 0 == (pos % 5);      // Flag for every 5th tick mark
  float rad = pos * 6 * - 0.0174532925;              // Calculate tick mark coordinates on the clock face perimeter
  float cosy = - cos(rad) * (halfwidth * 10 / 11);
  float sinx = - sin(rad) * (halfwidth * 10 / 11);
  canvas.fillCircle(halfwidth + sinx, halfwidth + cosy, flg * 3 + 1, palette);
}

void drawClock(uint64_t time)
{ // Draw the clock
  static int32_t p_min = -1;
  int32_t sec = time / 1000;
  int32_t min = sec / 60;
  if (p_min != min) { // If the minute value has changed, update the digital display on the clock face
    p_min = min;
    update7Seg(min / 60, min % 60);
  }
  clockbase.pushSprite(0, 0);  // Overwrite the clock face image to the drawing buffer

  drawDot(sec % 60, 14);
  drawDot(min % 60, 15);
  drawDot(((min/60)*5)%60, 15);

  float fhour = (float)time / 120000;  // Hour hand angle
  float fmin  = (float)time /  10000;  // Minute hand angle
  float fsec  = (float)time*6 / 1000;  // Second hand angle
  int px = canvas.getPivotX();
  int py = canvas.getPivotY();
  shadow1.pushRotateZoom(px+2, py+2, fhour, 1.0, 0.7, transpalette); // Draw hand shadows offset to the lower-right
  shadow1.pushRotateZoom(px+3, py+3, fmin , 1.0, 1.0, transpalette);
  shadow2.pushRotateZoom(px+4, py+4, fsec , 1.0, 1.0, transpalette);
  needle1.pushRotateZoom(            fhour, 1.0, 0.7, transpalette); // Draw the hands
  needle1.pushRotateZoom(            fmin , 1.0, 1.0, transpalette);
  needle2.pushRotateZoom(            fsec , 1.0, 1.0, transpalette);

  canvas.pushRotateZoom(0, zoom, zoom, transpalette);    // Draw the completed clock face to LCD
  lcd.display();
}

void loop(void)
{
  static uint32_t p_milli = 0;
  uint32_t milli = lgfx::millis() % 1000;
  if (p_milli > milli) count += 1000 + (milli - p_milli);
  else                 count +=        (milli - p_milli);
  p_milli = milli;

  int32_t tmp = (count % 1000) >> 3;
  canvas.setPaletteColor(8, 255 - (tmp>>1), 255 - (tmp>>1), 200 - tmp); // Vary the second hand drawing color
//count += 60000;
  if ( count > oneday ) { count -= oneday; }
  drawClock(count);
}

