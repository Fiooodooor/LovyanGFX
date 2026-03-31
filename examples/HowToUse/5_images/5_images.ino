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


// ※ LCDへの画像データの送信は、メモリの若いアドレスにあるデータから順に1Byte単位で送信されます。
//    このため、例えばRGB565の16bit型のデータを素直にuint16_tの配列で用意すると、送信の都合としてはバイト順が入れ替わった状態になります。
//    この場合は事前にsetSwapBytes(true)を使用したり、第３引数にtrueを指定する事で、バイト順の変換が行われて正常に描画できます。
//    なお用意する画像データを予め上位下位バイトを入れ替えた状態で作成すれば、この変換は不要になり速度面で有利になります。

  lcd.setAddrWindow(40,  0, image_width, image_height);
  lcd.writePixels((uint16_t*)swap565, len, false); // 予め上位下位が入れ替わった16bitデータの場合はバイト順変換を無効にする。

  lcd.setAddrWindow(40, 40, image_width, image_height);
  lcd.writePixels((uint16_t*)swap565, len, true);  // 逆に、予め上位下位が入れ替わったデータにバイト順変換を行うと色が化ける。

  lcd.setAddrWindow(40, 80, image_width, image_height);
  lcd.writePixels((void*)rgb888, len, true);  // 24bitのデータも同様に、RGB888の青が下位側にあるデータはバイト順変換が必要。

  lcd.setAddrWindow(40, 120, image_width, image_height);
  lcd.writePixels((void*)bgr888, len, false);  // 同様に、BGR888の赤が下位側にあるデータはバイト順変換は不要。

  lcd.setAddrWindow(40, 160, image_width, image_height);
  lcd.writePixels((void*)bgr888, len, true);  // 設定を誤ると、色が化ける。（赤と青が入れ替わる）

  lcd.display();
  delay(4000);
  lcd.clear(TFT_DARKGREY);

/*
方法２．描画する座標と幅・高さを指定してデータを描画する方法

この方法では、pushImage関数を用いて描画範囲と描画データを指定して描画します。

  pushImage( x, y, w, h, *data);                  // 指定された座標に画像を描画する。

方法１と違い、画面外にはみ出す座標を指定しても描画が乱れることはありません。（はみ出した部分は描画されません。）
方法１と違い、バイト順の変換を指定する引数が無いため、事前にsetSwapBytesによる設定が必要です。
なお方法１と同様に、dataの型に応じて色変換が行われます。
*/

  lcd.setSwapBytes(true); // バイト順変換を有効にする。

  // 描画先の座標と画像の幅・高さを指定して画像データを描画します。
  lcd.pushImage(   0, 0, image_width, image_height, (uint16_t*)rgb565); // RGB565の16bit画像データを描画。

  // データとバイト順変換の指定が一致していない場合、色化けします。
  lcd.pushImage(   0, 40, image_width, image_height, (uint16_t*)swap565); // NG. バイト順変換済みデータにバイト順変換を行うと色化けする。

  // 描画範囲が画面外にはみ出すなどした場合でも、描画結果が崩れることはありません。
  lcd.pushImage(-1, 80, image_width, image_height, (uint16_t*)rgb565); // X座標-1（画面外）を指定しても描画は乱れない。

  // データと型が一致していない場合は、描画結果が崩れます。
  lcd.pushImage(0, 120, image_width, image_height, (uint8_t*)rgb565); // RGB565のデータをuint8_tにキャストし、RGB332として扱わせると描画が乱れる。

  // データと型が一致していれば、適切に形式変換が行われます。
  lcd.pushImage(0, 160, image_width, image_height, (uint8_t*)rgb332); // RGB332のデータでも正しく描画できる。


  lcd.setSwapBytes(false);   // バイト順の変換を無効にする。
  lcd.pushImage( 40,   0, image_width, image_height, (uint8_t* )rgb332);  // good. RGB332のデータはバイト順変換の影響を受けない。
  lcd.pushImage( 40,  40, image_width, image_height, (uint16_t*)rgb565);  // NG. RGB565のデータはバイト順変換が必要。
  lcd.pushImage( 40,  80, image_width, image_height, (void*    )rgb888);  // NG. RGB888のデータはバイト順変換が必要。
  lcd.pushImage( 40, 120, image_width, image_height, (uint16_t*)swap565); // good. バイト順変換済みRGB565のデータは色化けしない。
  lcd.pushImage( 40, 160, image_width, image_height, (void*    )bgr888);  // good. バイト順変換済みRGB888のデータは色化けしない。

  lcd.setSwapBytes(true);   // バイト順の変換を有効にする。
  lcd.pushImage( 80,   0, image_width, image_height, (uint8_t* )rgb332);  // good. RGB332のデータはバイト順変換の影響を受けない。
  lcd.pushImage( 80,  40, image_width, image_height, (uint16_t*)rgb565);  // good. バイト順変換が有効ならRGB565のデータは色化けしない。
  lcd.pushImage( 80,  80, image_width, image_height, (void*    )rgb888);  // good. バイト順変換が有効ならRGB888のデータは色化けしない。
  lcd.pushImage( 80, 120, image_width, image_height, (uint16_t*)swap565); // NG. バイト順変換済みデータにバイト順変換を行うと色化けする。
  lcd.pushImage( 80, 160, image_width, image_height, (void*    )bgr888);  // NG. バイト順変換済みデータにバイト順変換を行うと色化けする。

// データの型として、lgfx::名前空間に定義されている型を利用する事もできます。
// これらの型にキャストする場合はsetSwapBytesの設定は無視されます。
  lcd.pushImage(120,   0, image_width, image_height, (lgfx:: rgb332_t*) rgb332); // good  8bitデータ
  lcd.pushImage(120,  40, image_width, image_height, (lgfx:: rgb565_t*) rgb565); // good 16bitデータ
  lcd.pushImage(120,  80, image_width, image_height, (lgfx:: rgb888_t*) rgb888); // good 24bitデータ
  lcd.pushImage(120, 120, image_width, image_height, (lgfx::swap565_t*)swap565); // good バイト順変換済み16bitデータ
  lcd.pushImage(120, 160, image_width, image_height, (lgfx:: bgr888_t*) bgr888); // good バイト順変換済み24bitデータ

// 第６引数で透過色を指定できます。透過指定された色のある部分は描画されません。
  lcd.pushImage(160,   0, image_width, image_height, (lgfx:: rgb332_t*) rgb332, 0);                   // 黒を透過指定
  lcd.pushImage(160,  40, image_width, image_height, (lgfx:: rgb565_t*) rgb565, (uint8_t)0xE0);       // 赤を透過指定
  lcd.pushImage(160,  80, image_width, image_height, (lgfx:: rgb888_t*) rgb888, (uint16_t)0x07E0);    // 緑を透過指定
  lcd.pushImage(160, 120, image_width, image_height, (lgfx::swap565_t*)swap565, (uint32_t)0x0000FFU); // 青を透過指定
  lcd.pushImage(160, 160, image_width, image_height, (lgfx:: bgr888_t*) bgr888, TFT_WHITE);           // 白を透過指定

  lcd.display();
  delay(4000);
  lcd.clear(TFT_DARKGREY);

// pushImageRotateZoom関数を使うと、画像を回転拡大縮小させて描画できます。
  for (int angle = 0; angle <= 360; ++angle) {
    lcd.pushImageRotateZoom
      ( lcd.width()  >> 2  // 描画先の中心座標X
      , lcd.height() >> 1  // 描画先の中心座標Y
      , image_width  >> 1  // 画像の中心座標X
      , image_height >> 1  // 画像の中心座標Y
      , angle              // 回転角度
      , 3.0                // X方向の描画倍率 (マイナス指定で反転可能)
      , 3.0                // Y方向の描画倍率 (マイナス指定で反転可能)
      , image_width        // 画像データの幅
      , image_height       // 画像データの高さ
      , rgb332             // 画像データのポインタ
      );

// pushImageRotateZoomWithAA関数を使うと、アンチエイリアスが有効になります。
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

// pushImageAffine関数を使うと、画像をアフィン変換で変形させて描画できます。
// アフィン変換のパラメータはfloat型の配列で指定します。
  {
    float matrix[6] = // 等倍表示
      { 1.0,  0.0,  (float)lcd.width()  / 2
      , 0.0,  1.0,  (float)lcd.height() / 2 };
    lcd.pushImageAffine(matrix, image_width, image_height, rgb332);
  }

  lcd.display();
  delay(1000);
  lcd.clear(TFT_DARKGREY);

  {
    float matrix[6] = // 横２倍表示
      { 2.0,  0.0,  (float)lcd.width()  / 2
      , 0.0,  1.0,  (float)lcd.height() / 2 };
    lcd.pushImageAffine(matrix, image_width, image_height, rgb332);
  }

  lcd.display();
  delay(1000);
  lcd.clear(TFT_DARKGREY);

  {
    float matrix[6] = // 縦２倍表示
      { 1.0,  0.0,  (float)lcd.width()  / 2
      , 0.0,  2.0,  (float)lcd.height() / 2 };
    lcd.pushImageAffine(matrix, image_width, image_height, rgb332);
  }

  lcd.display();
  delay(1000);
  lcd.clear(TFT_DARKGREY);

  {
    float matrix[6] = // 斜め変形
      { 1.0, -0.4,  (float)lcd.width()  / 2
      , 0.0,  1.0,  (float)lcd.height() / 2 };
    lcd.pushImageAffine(matrix, image_width, image_height, rgb332);
  }

  lcd.display();
  delay(1000);
  lcd.clear(TFT_DARKGREY);

  // pushImageAffineWithAA関数を使用するとアンチエイリアスが有効になります。
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
