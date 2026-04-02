#define LGFX_AUTODETECT
#include <LovyanGFX.hpp>

LGFX lcd;
LGFX_Sprite canvas;

static constexpr char text[] = "Hello world ! こんにちは世界！ this is long long string sample. 寿限無、寿限無、五劫の擦り切れ、海砂利水魚の、水行末・雲来末・風来末、喰う寝る処に住む処、藪ら柑子の藪柑子、パイポ・パイポ・パイポのシューリンガン、シューリンガンのグーリンダイ、グーリンダイのポンポコピーのポンポコナの、長久命の長助";
static constexpr size_t textlen = sizeof(text) / sizeof(text[0]);
size_t textpos = 0;

void setup(void) 
{
  lcd.init();

  // Rotate so the screen is landscape
  if (lcd.width() < lcd.height()) lcd.setRotation(lcd.getRotation() ^ 1);

  canvas.setColorDepth(8);
  canvas.setFont(&fonts::lgfxJapanMinchoP_32);
  canvas.setTextWrap(false);        // Disable cursor wrapping when reaching the right edge
  canvas.createSprite(lcd.width() + 36, 36); // Prepare width of screen + one character
}

void loop(void)
{
  int32_t cursor_x = canvas.getCursorX() - 1;   // Get current cursor position and move 1 pixel to the left
  if (cursor_x <= 0) // If cursor has reached the left edge, it has completed one loop
  {
    textpos = 0;            // Reset the text read position
    cursor_x = lcd.width(); // Change cursor position so new text is drawn at the right edge of the screen
  }

  canvas.setCursor(cursor_x, 0); // Update cursor position
  canvas.scroll(-1, 0);          // Scroll canvas content 1 pixel to the left
  while (textpos < textlen && cursor_x <= lcd.width()) // Check if text can be drawn at the right edge of the screen
  {
    canvas.print(text[textpos++]);   // Output 1 byte at a time (this also works for multi-byte characters)
    cursor_x = canvas.getCursorX();  // Get cursor position after output
  }

  canvas.pushSprite(&lcd, 0, 0);
}
