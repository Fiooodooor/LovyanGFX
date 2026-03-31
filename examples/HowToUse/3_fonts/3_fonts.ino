#include <LovyanGFX.hpp>

static LGFX lcd;

void drawGradation(void)
{
  // Draw a gradient on the background
  lcd.startWrite();
  lcd.setAddrWindow(0, 0, lcd.width(), lcd.height());
  for (int y = 0; y < lcd.height(); ++y) {
    for (int x = 0; x < lcd.width(); ++x) {
      lcd.writeColor(lcd.color888(x>>1, (x + y) >> 2, y>>1), 1);
    }
  }
  lcd.endWrite();
}

void setup(void)
{
  lcd.init();

  drawGradation();

  // There are two main ways to draw text: print-family functions and drawString-family functions.

  // In drawString, the 1st argument specifies the string, the 2nd is the X coordinate, and the 3rd is the Y coordinate.
  lcd.drawString("string!", 10, 10);

  // In drawNumber, the 1st argument is a number.
  lcd.drawNumber(123, 100, 10);

  // In drawFloat, the 1st argument is a number, the 2nd is the decimal places, the 3rd is the X coordinate, and the 4th is the Y coordinate.
  lcd.drawFloat(3.14, 2, 150, 10);

  // The print function draws at the coordinates set by setCursor (or continues from the last position drawn by print).
  lcd.setCursor(10, 20);
  lcd.print("print!");

  // The printf function can draw formatted content. (Follows C printf conventions, so strings and floats are supported.)
  int value = 123;
  lcd.printf("test %d", value);

  // The println function adds a newline after drawing the string. Same effect as print("\n").
  lcd.println("println");

  // To change the font, use the setFont function.
  // The same fonts as TFT_eSPI's setTextFont are Font0 through Font8.
  // Note: If your editor supports autocomplete, typing &fonts:: will show a list of available fonts.
  lcd.setFont(&fonts::Font4);
  lcd.println("TestFont4");

  // For TFT_eSPI compatibility, font changes by number via setTextFont are also supported.
  // Valid numbers are 0, 2, 4, 6, 7, 8. (Following TFT_eSPI conventions.)
  // Note: This method is not recommended, as it forces all numbered fonts into the binary, increasing its size.
  lcd.setTextFont(2);
  lcd.println("TestFont2");


  // You can change colors with setTextColor.
  // The 1st argument is the text color, the 2nd argument is the background color.
  lcd.setTextColor(0x00FFFFU, 0xFF0000U);
  lcd.print("CyanText RedBack");
  // Note: When repeatedly redrawing text at the same position, it is recommended to specify a background color
  //       to overwrite cleanly. Using fillRect to clear first may cause flickering.


  // If only the 1st argument is specified in setTextColor (2nd argument omitted),
  // only the text is drawn without filling the background.
  lcd.setTextColor(0xFFFF00U);
  lcd.print("YellowText ClearBack");


  // Font6 contains only clock-related characters.
  lcd.setFont(&fonts::Font6);
  lcd.print("apm.:-0369");

  // Font7 contains a 7-segment LCD style font.
  lcd.setFont(&fonts::Font7);
  lcd.print(".:-147");

  // Font8 contains only numeric characters.
  lcd.setFont(&fonts::Font8);
  lcd.print(".:-258");


  delay(3000);
  drawGradation();

// 4 types x 9 sizes = 36 Japanese fonts converted from IPA fonts are preset.
// The trailing number represents the size: 8, 12, 16, 20, 24, 28, 32, 36, 40 are available.
// fonts::lgfxJapanMincho_12      // Mincho (serif) size 12, fixed-width font
// fonts::lgfxJapanMinchoP_16     // Mincho (serif) size 16, proportional font
// fonts::lgfxJapanGothic_20      // Gothic (sans-serif) size 20, fixed-width font
// fonts::lgfxJapanGothicP_24     // Gothic (sans-serif) size 24, proportional font

// 4 types x 5 sizes = 20 Japanese/Korean/Chinese (Simplified/Traditional) fonts converted from efont are preset.
// The number represents the size: 10, 12, 14, 16, 24 are available.
// The trailing letter indicates b= bold / i= italic.
// fonts::efontJA_10              // Japanese size 10
// fonts::efontCN_12_b            // Simplified Chinese size 12 bold
// fonts::efontTW_14_bi           // Traditional Chinese size 14 bold italic
// fonts::efontKR_16_i            // Korean size 16 italic

  lcd.setCursor(0, 0);
  lcd.setFont(&fonts::lgfxJapanMincho_16);   lcd.print("Mincho 16 Hello World\nHello World\n");
//lcd.setFont(&fonts::lgfxJapanMinchoP_16);  lcd.print("Mincho P 16 Hello World\nHello World\n");
  lcd.setFont(&fonts::lgfxJapanGothic_16);   lcd.print("Gothic 16 Hello World\nHello World\n");
//lcd.setFont(&fonts::lgfxJapanGothicP_16);  lcd.print("Gothic P 16 Hello World\nHello World\n");

// Note: You can use yamaneko's [Japanese Font Subset Generator](https://github.com/yamamaya/lgfxFontSubsetGenerator)
//    to create smaller font data containing only the characters you need.


  delay(3000);
  drawGradation();


  // LovyanGFX can also use AdafruitGFX fonts via the setFont function.
  // (setFreeFont is also provided for TFT_eSPI compatibility.)
  lcd.setFont(&fonts::FreeSerif9pt7b);


  // To draw text right-aligned or center-aligned, use setTextDatum to specify the anchor position.
  // Vertical options: top, middle, baseline, bottom (4 choices). Horizontal: left, center, right (3 choices).
  // Choose from the 12 combinations of vertical and horizontal settings.
  lcd.setTextDatum( textdatum_t::top_left        );
  lcd.setTextDatum( textdatum_t::top_center      );
  lcd.setTextDatum( textdatum_t::top_right       );
  lcd.setTextDatum( textdatum_t::middle_left     );
  lcd.setTextDatum( textdatum_t::middle_center   );
  lcd.setTextDatum( textdatum_t::middle_right    );
  lcd.setTextDatum( textdatum_t::baseline_left   );
  lcd.setTextDatum( textdatum_t::baseline_center );
  lcd.setTextDatum( textdatum_t::baseline_right  );
  lcd.setTextDatum( textdatum_t::bottom_left     );
  lcd.setTextDatum( textdatum_t::bottom_center   );
  lcd.setTextDatum( textdatum_t::bottom_right    );
  // Note: "textdatum_t::" can be omitted.
  // Note: For print-family functions, only the vertical setting takes effect; the horizontal setting has no effect.

  // Bottom-right aligned
  lcd.setTextDatum( bottom_right );
  lcd.drawString("bottom_right",  lcd.width() / 2,  lcd.height() / 2);

  // Bottom-left aligned
  lcd.setTextDatum( bottom_left );
  lcd.drawString("bottom_left",  lcd.width() / 2,  lcd.height() / 2);

  // Top-right aligned
  lcd.setTextDatum( top_right );
  lcd.drawString("top_right",  lcd.width() / 2,  lcd.height() / 2);

  // Top-left aligned
  lcd.setTextDatum( top_left );
  lcd.drawString("top_left",  lcd.width() / 2,  lcd.height() / 2);


  // Draw crosshairs at the anchor coordinates
  lcd.drawFastVLine(lcd.width() / 2, 0, lcd.height(), 0xFFFFFFU);
  lcd.drawFastHLine(0, lcd.height() / 2, lcd.width(), 0xFFFFFFU);


  delay(3000);
  drawGradation();

  lcd.setFont(&Font2);
  lcd.setCursor(0, 0);


  lcd.drawRect(8, 8, lcd.width() - 16, lcd.height() - 16, 0xFFFFFFU);

  // setClipRect limits the drawing area. Nothing will be drawn outside the specified region.
  // Note: This affects all drawing functions, not just text.
  lcd.setClipRect(10, 10, lcd.width() - 20, lcd.height() - 20);


  // setTextSize specifies the text magnification.
  // The 1st argument is the horizontal scale, the 2nd is the vertical scale.
  // If the 2nd argument is omitted, the 1st argument applies to both horizontal and vertical.
  lcd.setTextSize(2.7, 4);
  lcd.println("Size 2.7 x 4");

  lcd.setTextSize(2.5);
  lcd.println("Size 2.5 x 2.5");

  lcd.setTextSize(1.5, 2);
  lcd.println("Size 1.5 x 2");

  delay(1000);

  lcd.setTextColor(0xFFFFFFU, 0);
  for (float i = 0; i < 30; i += 0.01) {
    lcd.setTextSize(sin(i)+1.1, cos(i)+1.1);
    lcd.drawString("size test", 10, 10);
  }

  lcd.setTextSize(1);

  // setTextWrap specifies the wrapping behavior when print reaches the edge of the screen (drawing area).
  // Setting the 1st argument to true wraps text from the right edge to the left edge.
  // Setting the 2nd argument to true wraps text from the bottom edge to the top edge. (Default: false)
  lcd.setTextWrap(false);
  lcd.println("setTextWrap(false) testing... long long long long string wrap test string ");
  // When false, position is not adjusted and text extending beyond the drawing area is not drawn.

  lcd.setTextWrap(true);
  lcd.setTextColor(0xFFFF00U, 0);
  lcd.println("setTextWrap(true) testing... long long long long string wrap test string ");
  // When true, coordinates are automatically adjusted to stay within the drawing area.

  delay(1000);

  // When the 2nd argument is true, text continues from the top when it reaches the bottom of the screen.
  lcd.setTextColor(0xFFFFFFU, 0);
  lcd.setTextWrap(true, true);
  lcd.println("setTextWrap(true, true) testing...");
  for (int i = 0; i < 100; ++i) {
    lcd.printf("wrap test %03d ", i);
    delay(50);
  }


  drawGradation();

  // setTextScroll specifies the scrolling behavior when text reaches the bottom of the screen.
  // setScrollRect specifies the rectangular area to scroll. (If not set, the entire screen scrolls.)
  // Note: The scroll feature requires the LCD to support pixel readback.
  lcd.setTextScroll(true);

  // Arguments 1-4 specify the X, Y, Width, Height rectangle. The 5th argument specifies the fill color after scrolling (optional).
  lcd.setScrollRect(10, 10, lcd.width() - 20, lcd.height() - 20, 0x00001FU);

  for (int i = 0; i < 50; ++i) {
    lcd.printf("scroll test %d \n", i);
  }


  // Clear the clipping rectangle set by setClipRect.
  lcd.clearClipRect();

  // Clear the scroll rectangle set by setScrollRect.
  lcd.clearScrollRect();


  lcd.setTextSize(1);
  lcd.setTextColor(0xFFFFFFU, 0);


  // setTextPadding specifies the minimum width for background fill when using drawString-family functions.
  lcd.setTextPadding(100);


  drawGradation();
}

void drawNumberTest(const lgfx::IFont* font)
{
  lcd.setFont(font);

  lcd.fillScreen(0x0000FF);

  lcd.setColor(0xFFFF00U);
  lcd.drawFastVLine( 80, 0, 240);
  lcd.drawFastVLine(160, 0, 240);
  lcd.drawFastVLine(240, 0, 240);
  lcd.drawFastHLine(0,  45, 320);
  lcd.drawFastHLine(0,  95, 320);
  lcd.drawFastHLine(0, 145, 320);
  lcd.drawFastHLine(0, 195, 320);

  for (int i = 0; i < 200; ++i) {
    lcd.setTextDatum( textdatum_t::bottom_right    );     lcd.drawNumber(i,  80,  45);
    lcd.setTextDatum( textdatum_t::bottom_center   );     lcd.drawNumber(i, 160,  45);
    lcd.setTextDatum( textdatum_t::bottom_left     );     lcd.drawNumber(i, 240,  45);
    lcd.setTextDatum( textdatum_t::baseline_right  );     lcd.drawNumber(i,  80,  95);
    lcd.setTextDatum( textdatum_t::baseline_center );     lcd.drawNumber(i, 160,  95);
    lcd.setTextDatum( textdatum_t::baseline_left   );     lcd.drawNumber(i, 240,  95);
    lcd.setTextDatum( textdatum_t::middle_right    );     lcd.drawNumber(i,  80, 145);
    lcd.setTextDatum( textdatum_t::middle_center   );     lcd.drawNumber(i, 160, 145);
    lcd.setTextDatum( textdatum_t::middle_left     );     lcd.drawNumber(i, 240, 145);
    lcd.setTextDatum( textdatum_t::top_right       );     lcd.drawNumber(i,  80, 195);
    lcd.setTextDatum( textdatum_t::top_center      );     lcd.drawNumber(i, 160, 195);
    lcd.setTextDatum( textdatum_t::top_left        );     lcd.drawNumber(i, 240, 195);
  }
}

void loop(void)
{
// Note: Fonts whose names start with "Free" are available in 4 sizes: 9pt, 12pt, 18pt, and 24pt.
  drawNumberTest( &fonts::Font0                   );
  drawNumberTest( &fonts::Font2                   );
  drawNumberTest( &fonts::Font4                   );
  drawNumberTest( &fonts::Font6                   );
  drawNumberTest( &fonts::Font7                   );
  drawNumberTest( &fonts::Font8                   );
  drawNumberTest( &fonts::TomThumb                );
  drawNumberTest( &fonts::FreeMono9pt7b           );
  drawNumberTest( &fonts::FreeMonoBold9pt7b       );
  drawNumberTest( &fonts::FreeMonoOblique9pt7b    );
  drawNumberTest( &fonts::FreeMonoBoldOblique9pt7b);
  drawNumberTest( &fonts::FreeSans9pt7b           );
  drawNumberTest( &fonts::FreeSansBold9pt7b       );
  drawNumberTest( &fonts::FreeSansOblique9pt7b    );
  drawNumberTest( &fonts::FreeSansBoldOblique9pt7b);
  drawNumberTest( &fonts::FreeSerif9pt7b          );
  drawNumberTest( &fonts::FreeSerifBold9pt7b      );
  drawNumberTest( &fonts::FreeSerifItalic9pt7b    );
  drawNumberTest( &fonts::FreeSerifBoldItalic9pt7b);
  drawNumberTest( &fonts::Orbitron_Light_24       );
  drawNumberTest( &fonts::Roboto_Thin_24          );
  drawNumberTest( &fonts::Satisfy_24              );
  drawNumberTest( &fonts::Yellowtail_32           );
}
