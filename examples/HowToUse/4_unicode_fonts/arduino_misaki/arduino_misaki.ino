
// This example requires Arduino-misakiUTF16.
//
// URL : https://github.com/Tamakichi/Arduino-misakiUTF16
//

// Include the font data of Arduino-misakiUTF16.
#include "misakiUTF16FontData.h"

#include <LovyanGFX.hpp>

static LGFX lcd;

// Use the lgfx::BDFfont class to configure Arduino-misakiUTF16 for use.
static constexpr lgfx::BDFfont misaki_font = 
  { fdata             // 1st argument: font bitmap data
  , ftable            // 2nd argument: unicode font table
  , sizeof(ftable)>>1 // 3rd argument: font table size
  , 8                 // 4th argument: font width
  , 4                 // 5th argument: font width (half-width)
  , 7                 // 6th argument: font height
  , 6                 // 7th argument: baseline height
  , 8                 // 8th argument: vertical cursor movement on newline
  };


void setup()
{
  lcd.init();

  // By passing misaki_font as an argument to the setFont function, it can be used with print, drawString, etc.
  lcd.setFont(&misaki_font);

  lcd.setTextWrap(true, true);
}

void loop()
{
  lcd.setTextColor(0x808080U|random(0xFFFFFF), 0x7F7F7FU&random(0x10000));

  lcd.print("Misaki font is a compact 8x8 font.");
  lcd.print("Arduino-misakiUTF16 contains 1,006 educational kanji (learned in elementary school) + hiragana, katakana, symbols, half-width characters, totaling 1,710 characters.");
  lcd.print("Hello");
  lcd.print("HELLO");
  lcd.print("Hello");
  delay(1000);
}
