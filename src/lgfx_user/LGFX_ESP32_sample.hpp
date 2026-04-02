#pragma once

#define LGFX_USE_V1

#include <LovyanGFX.hpp>

// Configuration example for using LovyanGFX with custom settings on ESP32

/*
Duplicate this file, give it a new name, and modify the settings to match your environment.
You can use it by including the created file from your user program.

You may place the duplicated file in the library's lgfx_user folder,
but please note that it may be deleted when the library is updated.

For safe operation, create a backup or place it in your user project folder.
//*/


/// Create a class with custom settings, derived from LGFX_Device.
class LGFX : public lgfx::LGFX_Device
{
/*
 You may change the class name from "LGFX" to any other name.
 When using together with AUTODETECT, "LGFX" is already in use, so please change to a name other than LGFX.
 Also, when using multiple panels simultaneously, give each one a different name.
 Note: When changing the class name, you must also change the constructor name to match.

 You are free to choose any name, but anticipating that configurations may increase,
 for example, if configuring an ILI9341 with SPI connection on an ESP32 DevKit-C,
  LGFX_DevKitC_SPI_ILI9341
 using a name like this and matching the file name with the class name makes it easier to identify later.
//*/


// Prepare an instance that matches the type of panel to connect.
//lgfx::Panel_GC9A01      _panel_instance;
//lgfx::Panel_GDEW0154M09 _panel_instance;
//lgfx::Panel_HX8357B     _panel_instance;
//lgfx::Panel_HX8357D     _panel_instance;
//lgfx::Panel_ILI9163     _panel_instance;
  lgfx::Panel_ILI9341     _panel_instance;
//lgfx::Panel_ILI9342     _panel_instance;
//lgfx::Panel_ILI9481     _panel_instance;
//lgfx::Panel_ILI9486     _panel_instance;
//lgfx::Panel_ILI9488     _panel_instance;
//lgfx::Panel_IT8951      _panel_instance;
//lgfx::Panel_RA8875      _panel_instance;
//lgfx::Panel_SH110x      _panel_instance; // SH1106, SH1107
//lgfx::Panel_SSD1306     _panel_instance;
//lgfx::Panel_SSD1327     _panel_instance;
//lgfx::Panel_SSD1331     _panel_instance;
//lgfx::Panel_SSD1351     _panel_instance; // SSD1351, SSD1357
//lgfx::Panel_SSD1963     _panel_instance;
//lgfx::Panel_ST7735      _panel_instance;
//lgfx::Panel_ST7735S     _panel_instance;
//lgfx::Panel_ST7789      _panel_instance;
//lgfx::Panel_ST7796      _panel_instance;


// Prepare an instance that matches the type of bus to connect to the panel.
  lgfx::Bus_SPI       _bus_instance;   // SPI bus instance
//lgfx::Bus_I2C       _bus_instance;   // I2C bus instance (ESP32 only)
//lgfx::Bus_Parallel8 _bus_instance;   // 8-bit parallel bus instance (ESP32 only)

// Prepare an instance if backlight control is available. (delete if not needed)
  lgfx::Light_PWM     _light_instance;

// Prepare an instance that matches the type of touchscreen. (delete if not needed)
//lgfx::Touch_CST816S          _touch_instance;
  lgfx::Touch_FT5x06           _touch_instance; // FT5206, FT5306, FT5406, FT6206, FT6236, FT6336, FT6436
//lgfx::Touch_GSL1680E_800x480 _touch_instance; // GSL_1680E, 1688E, 2681B, 2682B
//lgfx::Touch_GSL1680F_800x480 _touch_instance;
//lgfx::Touch_GSL1680F_480x272 _touch_instance;
//lgfx::Touch_GSLx680_320x320  _touch_instance;
//lgfx::Touch_GT911            _touch_instance;
//lgfx::Touch_STMPE610         _touch_instance;
//lgfx::Touch_TT21xxx          _touch_instance; // TT21100
//lgfx::Touch_XPT2046          _touch_instance;

public:

  // Create a constructor and configure various settings here.
  // If you changed the class name, specify the same name for the constructor.
  LGFX(void)
  {
    { // Configure bus control settings.
      auto cfg = _bus_instance.config();    // Get the structure for bus configuration.

// SPI bus settings
      cfg.spi_host = VSPI_HOST;     // Select SPI to use  ESP32-S2,C3 : SPI2_HOST or SPI3_HOST / ESP32 : VSPI_HOST or HSPI_HOST
      // Note: With ESP-IDF version updates, VSPI_HOST and HSPI_HOST are deprecated. If you get errors, use SPI2_HOST or SPI3_HOST instead.
      cfg.spi_mode = 0;             // Set SPI communication mode (0 ~ 3)
      cfg.freq_write = 40000000;    // SPI clock for transmission (max 80MHz, rounded to a value that divides 80MHz evenly)
      cfg.freq_read  = 16000000;    // SPI clock for reception
      cfg.spi_3wire  = true;        // Set to true if reception is done via the MOSI pin
      cfg.use_lock   = true;        // Set to true to use transaction lock
      cfg.dma_channel = SPI_DMA_CH_AUTO; // Set DMA channel to use (0=no DMA / 1=1ch / 2=ch / SPI_DMA_CH_AUTO=auto)
      // Note: With ESP-IDF version updates, SPI_DMA_CH_AUTO (auto setting) is now recommended for DMA channel. Specifying 1ch or 2ch is deprecated.
      cfg.pin_sclk = 18;            // Set SPI SCLK pin number
      cfg.pin_mosi = 23;            // Set SPI MOSI pin number
      cfg.pin_miso = 19;            // Set SPI MISO pin number (-1 = disable)
      cfg.pin_dc   = 27;            // Set SPI D/C pin number  (-1 = disable)
     // When sharing the SPI bus with an SD card, be sure to set MISO without omitting it.
//*/
/*
// I2C bus settings
      cfg.i2c_port    = 0;          // Select I2C port to use (0 or 1)
      cfg.freq_write  = 400000;     // Clock for transmission
      cfg.freq_read   = 400000;     // Clock for reception
      cfg.pin_sda     = 21;         // Pin number connected to SDA
      cfg.pin_scl     = 22;         // Pin number connected to SCL
      cfg.i2c_addr    = 0x3C;       // I2C device address
//*/
/*
// 8-bit parallel bus settings
      cfg.i2s_port = I2S_NUM_0;     // Select I2S port to use (I2S_NUM_0 or I2S_NUM_1) (uses ESP32 I2S LCD mode)
      cfg.freq_write = 20000000;    // Transmission clock (max 20MHz, rounded to a value that divides 80MHz evenly)
      cfg.pin_wr =  4;              // Pin number connected to WR
      cfg.pin_rd =  2;              // Pin number connected to RD
      cfg.pin_rs = 15;              // Pin number connected to RS(D/C)
      cfg.pin_d0 = 12;              // Pin number connected to D0
      cfg.pin_d1 = 13;              // Pin number connected to D1
      cfg.pin_d2 = 26;              // Pin number connected to D2
      cfg.pin_d3 = 25;              // Pin number connected to D3
      cfg.pin_d4 = 17;              // Pin number connected to D4
      cfg.pin_d5 = 16;              // Pin number connected to D5
      cfg.pin_d6 = 27;              // Pin number connected to D6
      cfg.pin_d7 = 14;              // Pin number connected to D7
//*/

      _bus_instance.config(cfg);    // Apply the settings to the bus.
      _panel_instance.setBus(&_bus_instance);      // Set the bus to the panel.
    }

    { // Configure display panel control settings.
      auto cfg = _panel_instance.config();    // Get the structure for display panel configuration.

      cfg.pin_cs           =    14;  // Pin number connected to CS   (-1 = disable)
      cfg.pin_rst          =    33;  // Pin number connected to RST  (-1 = disable)
      cfg.pin_busy         =    -1;  // Pin number connected to BUSY (-1 = disable)

      // Note: The following settings have general default values for each panel, so try commenting out any unknown items.

      cfg.panel_width      =   240;  // Actual displayable width
      cfg.panel_height     =   320;  // Actual displayable height
      cfg.offset_x         =     0;  // Panel X offset
      cfg.offset_y         =     0;  // Panel Y offset
      cfg.offset_rotation  =     0;  // Rotation offset value 0~7 (4~7 are upside down)
      cfg.dummy_read_pixel =     8;  // Number of dummy read bits before pixel read
      cfg.dummy_read_bits  =     1;  // Number of dummy read bits before non-pixel data read
      cfg.readable         =  true;  // Set to true if data reading is supported
      cfg.invert           = false;  // Set to true if panel brightness is inverted
      cfg.rgb_order        = false;  // Set to true if panel red and blue are swapped
      cfg.dlen_16bit       = false;  // Set to true for panels that send data in 16-bit units via 16-bit parallel or SPI
      cfg.bus_shared       =  true;  // Set to true if sharing the bus with an SD card (performs bus control in drawJpgFile, etc.)

// Only set the following for drivers with variable pixel counts like ST7735 or ILI9163 when the display is misaligned.
//    cfg.memory_width     =   240;  // Maximum width supported by the driver IC
//    cfg.memory_height    =   320;  // Maximum height supported by the driver IC

      _panel_instance.config(cfg);
    }

//*
    { // Configure backlight control settings. (delete if not needed)
      auto cfg = _light_instance.config();    // Get the structure for backlight configuration.

      cfg.pin_bl = 32;              // Pin number connected to backlight
      cfg.invert = false;           // Set to true to invert backlight brightness
      cfg.freq   = 44100;           // Backlight PWM frequency
      cfg.pwm_channel = 7;          // PWM channel number to use

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);  // Set the backlight to the panel.
    }
//*/

//*
    { // Configure touchscreen control settings. (delete if not needed)
      auto cfg = _touch_instance.config();

      cfg.x_min      = 0;    // Minimum X value obtainable from the touchscreen (raw value)
      cfg.x_max      = 239;  // Maximum X value obtainable from the touchscreen (raw value)
      cfg.y_min      = 0;    // Minimum Y value obtainable from the touchscreen (raw value)
      cfg.y_max      = 319;  // Maximum Y value obtainable from the touchscreen (raw value)
      cfg.pin_int    = 38;   // Pin number connected to INT
      cfg.bus_shared = true; // Set to true if sharing the bus with the display
      cfg.offset_rotation = 0;// Adjustment when display and touch orientation do not match, set with values 0~7

// For SPI connection
      cfg.spi_host = VSPI_HOST;// Select SPI to use (HSPI_HOST or VSPI_HOST)
      cfg.freq = 1000000;     // Set SPI clock
      cfg.pin_sclk = 18;     // Pin number connected to SCLK
      cfg.pin_mosi = 23;     // Pin number connected to MOSI
      cfg.pin_miso = 19;     // Pin number connected to MISO
      cfg.pin_cs   =  5;     // Pin number connected to CS

// For I2C connection
      cfg.i2c_port = 1;      // Select I2C to use (0 or 1)
      cfg.i2c_addr = 0x38;   // I2C device address
      cfg.pin_sda  = 23;     // Pin number connected to SDA
      cfg.pin_scl  = 32;     // Pin number connected to SCL
      cfg.freq = 400000;     // Set I2C clock

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);  // Set the touchscreen to the panel.
    }
//*/

    setPanel(&_panel_instance); // Set the panel to use.
  }
};

