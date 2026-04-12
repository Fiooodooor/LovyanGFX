#define LGFX_USE_V1
#include <LovyanGFX.hpp>

static LGFX lcd;
static LGFX_Sprite canvas(&lcd);       // Off-screen drawing buffer
static LGFX_Sprite base(&canvas);      // Dial face part
static LGFX_Sprite needle(&canvas);    // Needle part

static int32_t width = 239;             // Image size
static int32_t halfwidth = width >> 1;  // Center coordinate
static auto transpalette = 0;           // Transparent color palette index
static float zoom;                      // Display scale factor

#ifdef min
#undef min
#endif

void setup(void)
{
  lcd.init();
  int lw = std::min(lcd.width(), lcd.height());

  zoom = (float)lw / width; // Adjust scale to fit the display

  int px = lcd.width() >> 1;
  int py = lcd.height() >> 1;
  lcd.setPivot(px, py); // Set drawing center to screen center

  lcd.setColorDepth(24);
  for (int i = 0; i < 180; i+=2) { // Draw outer ring
    lcd.setColor(lcd.color888(i*1.4,i*1.4+2,i*1.4+4));
    lcd.fillArc(px, py, (lw>>1)       , (lw>>1)-zoom*3,  90+i,  92+i);
    lcd.fillArc(px, py, (lw>>1)       , (lw>>1)-zoom*3,  88-i,  90-i);
  }
  for (int i = 0; i < 180; i+=2) { // Draw outer ring
    lcd.setColor(lcd.color888(i*1.4,i*1.4+2,i*1.4+4));
    lcd.fillArc(px, py, (lw>>1)-zoom*4, (lw>>1)-zoom*7, 270+i, 272+i);
    lcd.fillArc(px, py, (lw>>1)-zoom*4, (lw>>1)-zoom*7, 268-i, 270-i);
  }
  lcd.setColorDepth(16);

  canvas.setColorDepth(lgfx::palette_2bit);  // Prepare each part in 2-bit palette mode
  base  .setColorDepth(lgfx::palette_2bit);
  needle.setColorDepth(lgfx::palette_2bit);

  canvas.createSprite(width, width); // Allocate memory
  base  .createSprite(width, width);
  needle.createSprite(3, 11);

  base.setFont(&fonts::Orbitron_Light_24);    // Change font type (for dial text)
//base.setFont(&fonts::Roboto_Thin_24);       // Change font type (for dial text)

  base.setTextDatum(lgfx::middle_center);

  base.fillCircle(halfwidth, halfwidth, halfwidth - 8, 1);
  base.fillArc(halfwidth, halfwidth, halfwidth - 10, halfwidth - 11, 135,  45, 3);
  base.fillArc(halfwidth, halfwidth, halfwidth - 20, halfwidth - 23,   2,  43, 2);
  base.fillArc(halfwidth, halfwidth, halfwidth - 20, halfwidth - 23, 317, 358, 2);
  base.setTextColor(3);

  for (int i = -5; i <= 25; ++i) {
    bool flg = 0 == (i % 5);      // Flag for every 5th tick mark
    if (flg) {
      // Draw large tick marks
      base.fillArc(halfwidth, halfwidth, halfwidth - 10, halfwidth - 24, 179.8 + i*9, 180.2 + i*9, 3);
      base.fillArc(halfwidth, halfwidth, halfwidth - 10, halfwidth - 20, 179.4 + i*9, 180.6 + i*9, 3);
      base.fillArc(halfwidth, halfwidth, halfwidth - 10, halfwidth - 14, 179   + i*9, 181   + i*9, 3);
      float rad = i * 9 * 0.0174532925;
      float ty = - sin(rad) * (halfwidth * 10 / 15);
      float tx = - cos(rad) * (halfwidth * 10 / 17);
      base.drawFloat((float)i/10, 1, halfwidth + tx, halfwidth + ty); // Draw numbers
    } else {
      // Draw small tick marks
      base.fillArc(halfwidth, halfwidth, halfwidth - 10, halfwidth - 17, 179.5 + i*9, 180.5 + i*9, 3);
    }
  }

  needle.setPivot(1, 9);  // Set the rotation center for the needle part
  needle.drawRect(0, 0, 3, 11, 2);

  canvas.setPaletteColor(1, 0, 0, 15);
  canvas.setPaletteColor(2, 255, 31, 31);
  canvas.setPaletteColor(3, 255, 255, 191);

  lcd.startWrite();
}


void draw(float value)
{
  base.pushSprite(0, 0);  // Overwrite the dial face image to the drawing buffer

  float angle = 270 + value * 90.0;
  needle.pushRotateZoom( angle, 3.0, 10.0, transpalette); // Draw the needle to the buffer
  canvas.fillCircle(halfwidth, halfwidth, 7, 3);
  canvas.pushRotateZoom(0, zoom, zoom, transpalette);    // Draw the completed dial to LCD
  if (value >= 1.5)
  lcd.fillCircle(lcd.width()>>1, (lcd.height()>>1) + width * 4/10, 5, 0x007FFFU);
}

void loop(void)
{
  float value = 0;
  do {
    draw(value);
  } while (1.9 > (value += 0.005 + value * 0.05));
  do {
    draw(value);
  } while (-0.5 < (value -= 0.1));
  do {
    draw(value);
  } while (0.0 > (value += 0.05));
}

