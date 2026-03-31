
// This example requires efont.
//
// URL : https://github.com/tanakamasayuki/efont
//

// Include a header corresponding to the character set used.
//#include <efontEnableAll.h>
//#include <efontEnableAscii.h>
//#include <efontEnableCJK.h>
//#include <efontEnableCn.h>
#include <efontEnableJa.h>
//#include <efontEnableJaMini.h>
//#include <efontEnableKr.h>
//#include <efontEnableTw.h>

// Include the font data of efont.
#include <efontFontData.h>

// efont must be included before LovyanGFX.
#include <LovyanGFX.hpp>

static LGFX lcd;

void setup()
{
  lcd.init();

  // By passing efont as an argument to setFont, it can be used with print, drawString, etc.
  lcd.setFont(&fonts::efont);

  lcd.setTextWrap(true, true);
}

void loop()
{
  lcd.setTextColor(random(0x10000), random(0x10000));
  lcd.setTextSize(random(1,3), random(1,3));

  lcd.print("Hello");
  lcd.print("Hello");
  lcd.print("Hello");
  lcd.print("Hello");
  lcd.print("Good morning");
  lcd.print("Good day");
  lcd.print("Hello");
  delay(1000);
}
