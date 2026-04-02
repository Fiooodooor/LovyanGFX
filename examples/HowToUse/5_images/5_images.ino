#include <LovyanGFX.hpp>

extern const uint8_t rgb888[];
extern const uint8_t bgr888[];
extern const uint16_t swap565[];
extern const uint16_t rgb565[];
extern const uint8_t rgb332[];

static constexpr int image_width = 33;
static constexpr int image_height = 31;
//----------------------------------------------------------------------------

static LGFX lcd;

void setup(void)
{
  lcd.init();
  lcd.startWrite();
}

void loop(void)
{

/*
  There are several functions for drawing image data.

Method 1: Set the drawing area in advance, then specify the data length to draw.
Method 2: Specify the coordinates, width, and height to draw the data.



Method 1: Set the drawing area in advance, then specify the data length to draw.

In this method, you set the drawing area with setWindow/setAddrWindow,
then draw image data by specifying its length with writePixels/pushPixels.

  setWindow( x0, y0, x1, y1 );   // Set drawing area. Specify top-left and bottom-right coordinates.
  setAddrWindow( x, y, w, h );   // Set drawing area. Specify top-left coordinates, width, and height.

setWindow behavior is undefined if coordinates outside the screen are specified.
setAddrWindow adjusts to fit within the screen if out-of-range values are specified.
 Note: The auto-adjusted width or height may end up smaller than the specified values.

  writePixels   ( *data, len, swap );  // Draw image. (Requires startWrite before and endWrite after)
  pushPixels    ( *data, len, swap );  // Draw image. (startWrite/endWrite not required)

 Note: writePixels originates from AdafruitGFX, and pushPixels from TFT_eSPI.
    The drawing result is the same, but they differ in whether startWrite/endWrite is called automatically.

1st argument: Pointer to image data (color format conversion is performed based on the data type.)
2nd argument: Number of pixels in the image data (note: not bytes.)
3rd argument: Byte-order swap flag (if omitted, the value set by setSwapBytes is used.)

Color format conversion is performed based on the type of the data pointer:
  uint8_t*  is treated as  8-bit color RGB332.
  uint16_t* is treated as 16-bit color RGB565.
  void*     is treated as 24-bit color RGB888.
 Note: Since there is no 3-byte primitive type, void* is treated as 24-bit color.

 Note: When drawing to the LCD, color format conversion is performed automatically based on the LCD's color mode.
*/
  lcd.clear(TFT_DARKGREY);
  lcd.setColorDepth(16);  // Set the LCD to 16-bit color mode.
  lcd.setSwapBytes(true); // Enable byte-order swap.
  int len = image_width * image_height;

  // Set the drawing area width and height with setAddrWindow in advance, then draw with writePixels.
  lcd.setAddrWindow(0, 0, image_width, image_height);         // Set the drawing area.
  lcd.writePixels((uint16_t*)rgb565, len); // Draw RGB565 16-bit image data.

  // If the data and byte-order swap setting do not match, colors will be corrupted.
  lcd.setAddrWindow(0, 40, image_width, image_height);
  // You can specify whether to swap byte order by passing false as the third argument.
  lcd.writePixels((uint16_t*)rgb565, len, false); // Drawing RGB565 image without byte-order swap causes color corruption.

  // If the drawing area extends outside the screen and doesn't match the image dimensions, the drawing result will be corrupted.
  lcd.setAddrWindow(-1, 80, image_width, image_height); // X coordinate is -1 (outside screen), so it cannot be set correctly.
  lcd.writePixels((uint16_t*)rgb565, len); // Drawing area width and image width mismatch causes corrupted output.

  // If the data and type do not match, the drawing result will also be corrupted.
  lcd.setAddrWindow(0, 120, image_width, image_height);
  // Intentionally cast RGB565 data to uint8_t, treating it as RGB332 8-bit color.
  lcd.writePixels((uint8_t*)rgb565, len);  // Image format and type mismatch causes corrupted output.

  // If the data and type match, appropriate format conversion is performed based on the destination color depth.
  lcd.setAddrWindow(0, 160, image_width, image_height);
  lcd.writePixels((uint8_t*)rgb332, len);  // RGB332 data can be correctly drawn on a 16-bit color LCD.


// Note: Image data is sent to the LCD in 1-byte units starting from the lowest memory address.
//    Therefore, if RGB565 16-bit data is stored in a uint16_t array as-is, the byte order will be swapped from the transmission perspective.
//    In this case, using setSwapBytes(true) in advance or specifying true for the 3rd argument will perform byte order conversion for correct drawing.
//    If the image data is prepared with the upper and lower bytes already swapped, this conversion is unnecessary, which is advantageous for speed.

  lcd.setAddrWindow(40,  0, image_width, image_height);
  lcd.writePixels((uint16_t*)swap565, len, false); // For 16-bit data with pre-swapped upper/lower bytes, disable byte order swap.

  lcd.setAddrWindow(40, 40, image_width, image_height);
  lcd.writePixels((uint16_t*)swap565, len, true);  // Conversely, applying byte order swap to pre-swapped data causes color corruption.

  lcd.setAddrWindow(40, 80, image_width, image_height);
  lcd.writePixels((void*)rgb888, len, true);  // Similarly for 24-bit data, byte order swap is needed when blue is on the lower side in RGB888.

  lcd.setAddrWindow(40, 120, image_width, image_height);
  lcd.writePixels((void*)bgr888, len, false);  // Similarly, byte order swap is not needed when red is on the lower side in BGR888 data.

  lcd.setAddrWindow(40, 160, image_width, image_height);
  lcd.writePixels((void*)bgr888, len, true);  // Incorrect settings cause color corruption. (Red and blue are swapped)

  lcd.display();
  delay(4000);
  lcd.clear(TFT_DARKGREY);

/*
Method 2: Draw data by specifying the coordinates, width, and height

In this method, the pushImage function is used to specify the drawing area and drawing data.

  pushImage( x, y, w, h, *data);                  // Draw an image at the specified coordinates.

Unlike Method 1, specifying coordinates that extend beyond the screen will not corrupt the drawing. (Parts that extend beyond are simply not drawn.)
Unlike Method 1, there is no argument to specify byte order swap, so setSwapBytes must be set in advance.
As with Method 1, color conversion is performed based on the data type.
*/

  lcd.setSwapBytes(true); // Enable byte order swap.

  // Draw image data by specifying the destination coordinates and image width/height.
  lcd.pushImage(   0, 0, image_width, image_height, (uint16_t*)rgb565); // Draw 16-bit RGB565 image data.

  // If the data and byte order swap setting do not match, colors will be corrupted.
  lcd.pushImage(   0, 40, image_width, image_height, (uint16_t*)swap565); // NG. Applying byte order swap to already-swapped data causes color corruption.

  // Even if the drawing area extends beyond the screen, the drawing result will not be corrupted.
  lcd.pushImage(-1, 80, image_width, image_height, (uint16_t*)rgb565); // Drawing is not corrupted even when specifying X coordinate -1 (outside screen).

  // If the data and type do not match, the drawing result will be corrupted.
  lcd.pushImage(0, 120, image_width, image_height, (uint8_t*)rgb565); // Casting RGB565 data to uint8_t and treating it as RGB332 causes corrupted drawing.

  // If the data and type match, appropriate format conversion is performed.
  lcd.pushImage(0, 160, image_width, image_height, (uint8_t*)rgb332); // Even RGB332 data can be drawn correctly.


  lcd.setSwapBytes(false);   // Disable byte order swap.
  lcd.pushImage( 40,   0, image_width, image_height, (uint8_t* )rgb332);  // good. RGB332 data is not affected by byte order swap.
  lcd.pushImage( 40,  40, image_width, image_height, (uint16_t*)rgb565);  // NG. RGB565 data requires byte order swap.
  lcd.pushImage( 40,  80, image_width, image_height, (void*    )rgb888);  // NG. RGB888 data requires byte order swap.
  lcd.pushImage( 40, 120, image_width, image_height, (uint16_t*)swap565); // good. Pre-swapped RGB565 data does not have color corruption.
  lcd.pushImage( 40, 160, image_width, image_height, (void*    )bgr888);  // good. Pre-swapped RGB888 data does not have color corruption.

  lcd.setSwapBytes(true);   // Enable byte order swap.
  lcd.pushImage( 80,   0, image_width, image_height, (uint8_t* )rgb332);  // good. RGB332 data is not affected by byte order swap.
  lcd.pushImage( 80,  40, image_width, image_height, (uint16_t*)rgb565);  // good. RGB565 data is not corrupted when byte order swap is enabled.
  lcd.pushImage( 80,  80, image_width, image_height, (void*    )rgb888);  // good. RGB888 data is not corrupted when byte order swap is enabled.
  lcd.pushImage( 80, 120, image_width, image_height, (uint16_t*)swap565); // NG. Applying byte order swap to already-swapped data causes color corruption.
  lcd.pushImage( 80, 160, image_width, image_height, (void*    )bgr888);  // NG. Applying byte order swap to already-swapped data causes color corruption.

// You can also use types defined in the lgfx:: namespace as the data type.
// When casting to these types, the setSwapBytes setting is ignored.
  lcd.pushImage(120,   0, image_width, image_height, (lgfx:: rgb332_t*) rgb332); // good  8-bit data
  lcd.pushImage(120,  40, image_width, image_height, (lgfx:: rgb565_t*) rgb565); // good 16-bit data
  lcd.pushImage(120,  80, image_width, image_height, (lgfx:: rgb888_t*) rgb888); // good 24-bit data
  lcd.pushImage(120, 120, image_width, image_height, (lgfx::swap565_t*)swap565); // good pre-swapped 16-bit data
  lcd.pushImage(120, 160, image_width, image_height, (lgfx:: bgr888_t*) bgr888); // good pre-swapped 24-bit data

// The 6th argument can specify a transparent color. Areas with the specified transparent color are not drawn.
  lcd.pushImage(160,   0, image_width, image_height, (lgfx:: rgb332_t*) rgb332, 0);                   // Set black as transparent
  lcd.pushImage(160,  40, image_width, image_height, (lgfx:: rgb565_t*) rgb565, (uint8_t)0xE0);       // Set red as transparent
  lcd.pushImage(160,  80, image_width, image_height, (lgfx:: rgb888_t*) rgb888, (uint16_t)0x07E0);    // Set green as transparent
  lcd.pushImage(160, 120, image_width, image_height, (lgfx::swap565_t*)swap565, (uint32_t)0x0000FFU); // Set blue as transparent
  lcd.pushImage(160, 160, image_width, image_height, (lgfx:: bgr888_t*) bgr888, TFT_WHITE);           // Set white as transparent

  lcd.display();
  delay(4000);
  lcd.clear(TFT_DARKGREY);

// The pushImageRotateZoom function can draw images with rotation, scaling, and zooming.
  for (int angle = 0; angle <= 360; ++angle) {
    lcd.pushImageRotateZoom
      ( lcd.width()  >> 2  // Destination center X coordinate
      , lcd.height() >> 1  // Destination center Y coordinate
      , image_width  >> 1  // Image center X coordinate
      , image_height >> 1  // Image center Y coordinate
      , angle              // Rotation angle
      , 3.0                // X-direction scale (negative value flips)
      , 3.0                // Y-direction scale (negative value flips)
      , image_width        // Image data width
      , image_height       // Image data height
      , rgb332             // Pointer to image data
      );

// The pushImageRotateZoomWithAA function enables anti-aliasing.
    lcd.pushImageRotateZoomWithAA
      ( lcd.width()*3>> 2
      , lcd.height() >> 1
      , image_width  >> 1
      , image_height >> 1
      , angle
      , 3.0
      , 3.0
      , image_width
      , image_height
      , rgb332
      );

    if ((angle % 36) == 0) { lcd.display(); }
  }

  lcd.clear(TFT_DARKGREY);

// The pushImageAffine function can draw images with affine transformation.
// Affine transformation parameters are specified as a float array.
  {
    float matrix[6] = // Display at original size
      { 1.0,  0.0,  (float)lcd.width()  / 2
      , 0.0,  1.0,  (float)lcd.height() / 2 };
    lcd.pushImageAffine(matrix, image_width, image_height, rgb332);
  }

  lcd.display();
  delay(1000);
  lcd.clear(TFT_DARKGREY);

  {
    float matrix[6] = // Display at 2x horizontal
      { 2.0,  0.0,  (float)lcd.width()  / 2
      , 0.0,  1.0,  (float)lcd.height() / 2 };
    lcd.pushImageAffine(matrix, image_width, image_height, rgb332);
  }

  lcd.display();
  delay(1000);
  lcd.clear(TFT_DARKGREY);

  {
    float matrix[6] = // Display at 2x vertical
      { 1.0,  0.0,  (float)lcd.width()  / 2
      , 0.0,  2.0,  (float)lcd.height() / 2 };
    lcd.pushImageAffine(matrix, image_width, image_height, rgb332);
  }

  lcd.display();
  delay(1000);
  lcd.clear(TFT_DARKGREY);

  {
    float matrix[6] = // Skew transformation
      { 1.0, -0.4,  (float)lcd.width()  / 2
      , 0.0,  1.0,  (float)lcd.height() / 2 };
    lcd.pushImageAffine(matrix, image_width, image_height, rgb332);
  }

  lcd.display();
  delay(1000);
  lcd.clear(TFT_DARKGREY);

  // Using the pushImageAffineWithAA function enables anti-aliasing.
  {
    float matrix[6] =
      { 1.0,  0.0,  (float)lcd.width()  / 2
      , 0.0,  1.0,  (float)lcd.height() / 2 };
    for (int i = -300; i < 300; i++) {
      float f = (float)i / 100;
      matrix[1] = f;
      matrix[3] = f;
      lcd.pushImageAffineWithAA(matrix, image_width, image_height, rgb332);

      if ((i % 30) == 0) { lcd.display(); }
    }
  }
}


//----------------------------------------------------------------------------
#define R 0x00,0x00,0xFF,
#define G 0x00,0xFF,0x00,
#define B 0xFF,0x00,0x00,
#define C 0xFF,0xFF,0x00,
#define M 0xFF,0x00,0xFF,
#define Y 0x00,0xFF,0xFF,
#define W 0xFF,0xFF,0xFF,
#define _ 0x00,0x00,0x00,
constexpr uint8_t rgb888[] = {
#include "image.h"
};
#undef R
#undef G
#undef B
#undef C
#undef M
#undef Y
#undef W
#undef _
//----------------------------------------------------------------------------
#define R 0xFF,0x00,0x00,
#define G 0x00,0xFF,0x00,
#define B 0x00,0x00,0xFF,
#define C 0x00,0xFF,0xFF,
#define M 0xFF,0x00,0xFF,
#define Y 0xFF,0xFF,0x00,
#define W 0xFF,0xFF,0xFF,
#define _ 0x00,0x00,0x00,
constexpr uint8_t bgr888[] = {
#include "image.h"
};
#undef R
#undef G
#undef B
#undef C
#undef M
#undef Y
#undef W
#undef _
//----------------------------------------------------------------------------
#define R 0x00F8,
#define G 0xE007,
#define B 0x1F00,
#define C 0xFF07,
#define M 0x1FF8,
#define Y 0xE0FF,
#define W 0xFFFF,
#define _ 0x0000,
constexpr uint16_t swap565[] = {
#include "image.h"
};
#undef R
#undef G
#undef B
#undef C
#undef M
#undef Y
#undef W
#undef _
//----------------------------------------------------------------------------
#define R 0xF800,
#define G 0x07E0,
#define B 0x001F,
#define C 0x07FF,
#define M 0xF81F,
#define Y 0xFFE0,
#define W 0xFFFF,
#define _ 0x0000,
constexpr uint16_t rgb565[] = {
#include "image.h"
};
#undef R
#undef G
#undef B
#undef C
#undef M
#undef Y
#undef W
#undef _
//----------------------------------------------------------------------------
#define R 0xE0,
#define G 0x1C,
#define B 0x03,
#define C 0x1F,
#define M 0xE3,
#define Y 0xFC,
#define W 0xFF,
#define _ 0x00,
constexpr uint8_t rgb332[] = {
#include "image.h"
};
#undef R
#undef G
#undef B
#undef C
#undef M
#undef Y
#undef W
#undef _
