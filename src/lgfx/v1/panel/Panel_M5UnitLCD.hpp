/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [FreeBSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/
#pragma once

#include "Panel_Device.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct Panel_M5UnitLCD : public Panel_Device
  {
  public:
    Panel_M5UnitLCD(void)
    {
      _cfg.memory_width  = _cfg.panel_width = 135;
      _cfg.memory_height = _cfg.panel_height = 240;
    }

    bool init(bool use_reset) override;
    void beginTransaction(void) override;
    void endTransaction(void) override;

    color_depth_t setColorDepth(color_depth_t depth) override;
    void setRotation(uint_fast8_t r) override;
    void setBrightness(uint8_t brightness) override;
    void setInvert(bool invert) override;
    void setSleep(bool flg) override;
    void setPowerSave(bool flg) override;

    void waitDisplay(void) override {}
    bool displayBusy(void) override { return false; }
    void display(uint_fast16_t, uint_fast16_t, uint_fast16_t, uint_fast16_t) override {}

    void writePixels(pixelcopy_t* param, uint32_t len, bool use_dma) override;
    void writeBlock(uint32_t rawcolor, uint32_t len) override;

    void setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye) override;
    void drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor) override;
    void writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor) override;
    void writeFillRectAlphaPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t argb8888) override;
    void writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma) override;
    void writeImageARGB(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param) override;
    void copyRect(uint_fast16_t dst_x, uint_fast16_t dst_y, uint_fast16_t w, uint_fast16_t h, uint_fast16_t src_x, uint_fast16_t src_y) override;

    uint32_t readCommand(uint_fast16_t, uint_fast8_t, uint_fast8_t) override { return 0; }
    uint32_t readData(uint_fast8_t, uint_fast8_t) override { return 0; }
    void readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param) override;

    static constexpr uint8_t CMD_NOP          = 0x00; // 1Byte No operation;
    static constexpr uint8_t CMD_READ_ID      = 0x04; // 1Byte Read ID. Response from slave is 4Bytes (0x77 0x89 0x00 0x?? (last byte is firmware version));
    static constexpr uint8_t CMD_READ_BUFCOUNT= 0x09; // 1Byte Get command buffer free space. Response is 1Byte, returning the number of receivable commands. Smaller numbers mean less buffer room.;

//  static constexpr uint8_t CMD_SCALING      = 0x18; // Display scaling [1]=X [2]=Y;

    static constexpr uint8_t CMD_INVOFF       = 0x20; // 1Byte Disable color inversion;
    static constexpr uint8_t CMD_INVON        = 0x21; // 1Byte Enable color inversion;
    static constexpr uint8_t CMD_BRIGHTNESS   = 0x22; // 2Byte Backlight data[1]==brightness 0~255
    static constexpr uint8_t CMD_COPYRECT     = 0x23; // 7Byte Rectangle copy [1]==XS [2]==YS [3]==XE [4]==YE [5]==DST_X [6]==DST_Y
    static constexpr uint8_t CMD_CASET        = 0x2A; // 3Byte X-axis range selection data[1]==XS  data[2]==XE
    static constexpr uint8_t CMD_RASET        = 0x2B; // 3Byte Y-axis range selection data[1]==YS  data[2]==YE
    static constexpr uint8_t CMD_ROTATE       = 0x36; // 2Byte Rotation [1]==rotation direction 0:normal 1:right 90° 2:180° 3:270° 4~7 are vertically flipped versions of 0~3;
    static constexpr uint8_t CMD_SET_POWER    = 0x38; // 2Byte data[1] 0:low-speed low-power / 1:normal / 2:high-speed high-power;
    static constexpr uint8_t CMD_SET_SLEEP    = 0x39; // 2Byte data[1] 0:wake from sleep / 1:enter sleep;
    static constexpr uint8_t CMD_SET_BYTESWAP = 0x3A; // 2Byte data[1] Specifies whether to byte-swap color data during transfer. Default is 0 / 0:no byte swap / 1:byte swap enabled;

    static constexpr uint8_t CMD_WRITE_RAW    = 0x40;
    static constexpr uint8_t CMD_WRITE_RAW_8  = 0x41; // Variable-length RGB332 Continuously send 1Byte pixel data;
    static constexpr uint8_t CMD_WRITE_RAW_16 = 0x42; // Variable-length RGB565 Continuously send 2Byte pixel data;
    static constexpr uint8_t CMD_WRITE_RAW_24 = 0x43; // Variable-length RGB888 Continuously send 3Byte pixel data;
    static constexpr uint8_t CMD_WRITE_RAW_32 = 0x44; // Variable-length ARGB8888 Continuously send 4Byte pixel data;
    static constexpr uint8_t CMD_WRITE_RAW_A  = 0x45; // Variable-length A8 Continuously send 1Byte pixel data (alpha channel only, drawing color reuses the last used one);

    static constexpr uint8_t CMD_WRITE_RLE    = 0x48;
    static constexpr uint8_t CMD_WRITE_RLE_8  = 0x49; // Variable-length RGB332 Continuously send 1Byte pixel data (RLE compressed);
    static constexpr uint8_t CMD_WRITE_RLE_16 = 0x4A; // Variable-length RGB565 Continuously send 2Byte pixel data (RLE compressed);
    static constexpr uint8_t CMD_WRITE_RLE_24 = 0x4B; // Variable-length RGB888 Continuously send 3Byte pixel data (RLE compressed);
    static constexpr uint8_t CMD_WRITE_RLE_32 = 0x4C; // Variable-length ARGB8888 Continuously send 4Byte pixel data (RLE compressed);
    static constexpr uint8_t CMD_WRITE_RLE_A  = 0x4D; // Variable-length A8 Continuously send 1Byte pixel data (RLE compressed, alpha channel only, drawing color reuses the last used one);

    static constexpr uint8_t CMD_RAM_FILL     = 0x50; // 1Byte Fill entire selected range with current drawing color;
    static constexpr uint8_t CMD_SET_COLOR    = 0x50;
    static constexpr uint8_t CMD_SET_COLOR_8  = 0x51; // 2Byte Set drawing color in RGB332 format;
    static constexpr uint8_t CMD_SET_COLOR_16 = 0x52; // 3Byte Set drawing color in RGB565 format;
    static constexpr uint8_t CMD_SET_COLOR_24 = 0x53; // 4Byte Set drawing color in RGB888 format;
    static constexpr uint8_t CMD_SET_COLOR_32 = 0x54; // 5Byte Set drawing color in ARGB8888 format;

    static constexpr uint8_t CMD_DRAWPIXEL    = 0x60; // 3Byte Pixel draw [1]==X [2]==Y
    static constexpr uint8_t CMD_DRAWPIXEL_8  = 0x61; // 4Byte Pixel draw [1]==X [2]==Y [3  ]==RGB332
    static constexpr uint8_t CMD_DRAWPIXEL_16 = 0x62; // 5Byte Pixel draw [1]==X [2]==Y [3~4]==RGB565
    static constexpr uint8_t CMD_DRAWPIXEL_24 = 0x63; // 6Byte Pixel draw [1]==X [2]==Y [3~5]==RGB888
    static constexpr uint8_t CMD_DRAWPIXEL_32 = 0x64; // 7Byte Pixel draw [1]==X [2]==Y [3~6]==ARGB8888

    static constexpr uint8_t CMD_FILLRECT     = 0x68; // 5Byte Rectangle fill [1]==XS [2]==YS [3]==XE [4]==YE
    static constexpr uint8_t CMD_FILLRECT_8   = 0x69; // 6Byte Rectangle fill [1]==XS [2]==YS [3]==XE [4]==YE [5  ]==RGB332
    static constexpr uint8_t CMD_FILLRECT_16  = 0x6A; // 7Byte Rectangle fill [1]==XS [2]==YS [3]==XE [4]==YE [5~6]==RGB565
    static constexpr uint8_t CMD_FILLRECT_24  = 0x6B; // 8Byte Rectangle fill [1]==XS [2]==YS [3]==XE [4]==YE [5~7]==RGB888
    static constexpr uint8_t CMD_FILLRECT_32  = 0x6C; // 9Byte Rectangle fill [1]==XS [2]==YS [3]==XE [4]==YE [5~8]==ARGB8888

    static constexpr uint8_t CMD_READ_RAW     = 0x80;
    static constexpr uint8_t CMD_READ_RAW_8   = 0x81; // 1Byte Read pixel data in RGB332 format;
    static constexpr uint8_t CMD_READ_RAW_16  = 0x82; // 1Byte Read pixel data in RGB565 format;
    static constexpr uint8_t CMD_READ_RAW_24  = 0x83; // 1Byte Read pixel data in RGB888 format;

    static constexpr uint8_t CMD_CHANGE_ADDR  = 0xA0; // 4Byte Change I2C address [0]=0xA0 [1]=new I2C address [2]=bit-inverted value of new I2C address [3]=0xA0

    static constexpr uint8_t CMD_UPDATE_BEGIN = 0xF0; // 4Byte Firmware update start   [0]=0xF0 [1]=0x77 [2]=0x89 [3]=0xF0
    static constexpr uint8_t CMD_UPDATE_DATA  = 0xF1; // 4Byte Firmware data transmission [0]=0xF1 [1]=0x77 [2]=0x89 [3]=0xF1
    static constexpr uint8_t CMD_UPDATE_END   = 0xF2; // 4Byte Firmware update complete   [0]=0xF2 [1]=0x77 [2]=0x89 [3]=0xF2
    static constexpr uint8_t CMD_RESET        = 0xFF; // 4Byte Reset (reboot)         [0]=0xFF [1]=0x77 [2]=0x89 [3]=0xFF

    static constexpr uint8_t UPDATE_RESULT_BROKEN = 0x01;
    static constexpr uint8_t UPDATE_RESULT_ERROR  = 0x00;
    static constexpr uint8_t UPDATE_RESULT_OK     = 0xF1;
    static constexpr uint8_t UPDATE_RESULT_BUSY   = 0xFF;

  protected:
  
    uint32_t _raw_color = ~0u;
    uint32_t _xpos;
    uint32_t _ypos;
    uint32_t _last_cmd;
    uint32_t _buff_free_count;
    bool _in_transaction = false;

    void _set_window(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye);
    void _fill_rect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint_fast8_t bytes);
    bool _check_repeat(uint32_t cmd = 0, uint_fast8_t limit = 64);

  };

//----------------------------------------------------------------------------
 }
}
