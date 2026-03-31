
#include <LovyanGFX.hpp>

// Example configuration for using LovyanGFX with custom settings on ESP32

/// Create a class for custom configuration, derived from LGFX_Device.
class LGFX : public lgfx::LGFX_Device
{
/*
 You can change the class name from "LGFX" to any other name.
 If using together with AUTODETECT, "LGFX" is already in use, so please change to a name other than LGFX.
 Also, when using multiple panels simultaneously, give each one a different name.
 Note: If you change the class name, you must also change the constructor name to match.

 You can choose any name you like, but to prepare for having more configurations,
 for example, if configuring an ILI9341 via SPI on an ESP32 DevKit-C,
  LGFX_DevKitC_SPI_ILI9341
 using a name like this and matching the file name to the class name makes it easier to identify later.
//*/


// Prepare an instance that matches the type of panel to be connected.
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


// Prepare an instance that matches the type of bus connecting to the panel.
  lgfx::Bus_SPI        _bus_instance;   // SPI bus instance
//lgfx::Bus_I2C        _bus_instance;   // I2C bus instance
//lgfx::Bus_Parallel8  _bus_instance;   // 8-bit parallel bus instance

// Prepare an instance if backlight control is available. (Delete if not needed)
  lgfx::Light_PWM     _light_instance;

// Prepare an instance that matches the type of touch screen. (Delete if not needed)
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

  // Create a constructor and perform various configurations here.
  // If you changed the class name, specify the same name for the constructor.
  LGFX(void)
  {
    { // Configure bus control settings.
      auto cfg = _bus_instance.config();    // Get the structure for bus configuration.

// SPI bus configuration
      cfg.spi_host = VSPI_HOST;     // Select the SPI to use  ESP32-S2,C3 : SPI2_HOST or SPI3_HOST / ESP32 : VSPI_HOST or HSPI_HOST
      // Note: With ESP-IDF version upgrades, VSPI_HOST and HSPI_HOST are deprecated. If you get errors, use SPI2_HOST or SPI3_HOST instead.
      cfg.spi_mode = 0;             // Set the SPI communication mode (0 ~ 3)
      cfg.freq_write = 40000000;    // SPI clock for transmission (max 80MHz, rounded to a value obtained by dividing 80MHz by an integer)
      cfg.freq_read  = 16000000;    // SPI clock for reception
      cfg.spi_3wire  = true;        // Set to true if receiving data on the MOSI pin
      cfg.use_lock   = true;        // Set to true to use transaction locking
      cfg.dma_channel = SPI_DMA_CH_AUTO; // Set the DMA channel to use (0=DMA not used / 1=1ch / 2=ch / SPI_DMA_CH_AUTO=auto)
      // Note: With ESP-IDF version upgrades, SPI_DMA_CH_AUTO (auto) is now recommended for DMA channels. Specifying 1ch or 2ch is deprecated.
      cfg.pin_sclk = 18;            // Set the SPI SCLK pin number
      cfg.pin_mosi = 23;            // Set the SPI MOSI pin number
      cfg.pin_miso = 19;            // Set the SPI MISO pin number (-1 = disable)
      cfg.pin_dc   = 27;            // Set the SPI D/C pin number  (-1 = disable)
     // When sharing the SPI bus with an SD card, MISO must be set and cannot be omitted.
//*/
/*
// I2C bus configuration
      cfg.i2c_port    = 0;          // Select the I2C port to use (0 or 1)
      cfg.freq_write  = 400000;     // Clock for transmission
      cfg.freq_read   = 400000;     // Clock for reception
      cfg.pin_sda     = 21;         // Pin number where SDA is connected
      cfg.pin_scl     = 22;         // Pin number where SCL is connected
      cfg.i2c_addr    = 0x3C;       // I2C device address
//*/
/*
// 8-bit parallel bus configuration
      cfg.i2s_port = I2S_NUM_0;     // Select the I2S port to use (I2S_NUM_0 or I2S_NUM_1) (Uses ESP32 I2S LCD mode)
      cfg.freq_write = 20000000;    // Transmission clock (max 20MHz, rounded to a value obtained by dividing 80MHz by an integer)
      cfg.pin_wr =  4;              // Pin number where WR is connected
      cfg.pin_rd =  2;              // Pin number where RD is connected
      cfg.pin_rs = 15;              // Pin number where RS(D/C) is connected
      cfg.pin_d0 = 12;              // Pin number where D0 is connected
      cfg.pin_d1 = 13;              // Pin number where D1 is connected
      cfg.pin_d2 = 26;              // Pin number where D2 is connected
      cfg.pin_d3 = 25;              // Pin number where D3 is connected
      cfg.pin_d4 = 17;              // Pin number where D4 is connected
      cfg.pin_d5 = 16;              // Pin number where D5 is connected
      cfg.pin_d6 = 27;              // Pin number where D6 is connected
      cfg.pin_d7 = 14;              // Pin number where D7 is connected
//*/

      _bus_instance.config(cfg);    // Apply the configuration to the bus.
      _panel_instance.setBus(&_bus_instance);      // Set the bus to the panel.
    }

    { // Configure display panel control settings.
      auto cfg = _panel_instance.config();    // Get the structure for display panel configuration.

      cfg.pin_cs           =    14;  // Pin number where CS is connected   (-1 = disable)
      cfg.pin_rst          =    33;  // Pin number where RST is connected  (-1 = disable)
      cfg.pin_busy         =    -1;  // Pin number where BUSY is connected (-1 = disable)

      // Note: The following settings have common default values for each panel, so try commenting out any unknown items.

      cfg.panel_width      =   240;  // Actual displayable width
      cfg.panel_height     =   320;  // Actual displayable height
      cfg.offset_x         =     0;  // Panel X-direction offset
      cfg.offset_y         =     0;  // Panel Y-direction offset
      cfg.offset_rotation  =     0;  // Rotation direction offset 0~7 (4~7 are upside down)
      cfg.dummy_read_pixel =     8;  // Number of dummy read bits before pixel readout
      cfg.dummy_read_bits  =     1;  // Number of dummy read bits before non-pixel data readout
      cfg.readable         =  true;  // Set to true if data reading is supported
      cfg.invert           = false;  // Set to true if the panel brightness is inverted
      cfg.rgb_order        = false;  // Set to true if the panel red and blue are swapped
      cfg.dlen_16bit       = false;  // Set to true for panels that send data in 16-bit units via 16-bit parallel or SPI
      cfg.bus_shared       =  true;  // Set to true if the bus is shared with an SD card (bus control is handled by drawJpgFile, etc.)

// Set the following only if the display shifts on drivers with variable pixel counts, such as ST7735 or ILI9163.
//    cfg.memory_width     =   240;  // Maximum width supported by the driver IC
//    cfg.memory_height    =   320;  // Maximum height supported by the driver IC

      _panel_instance.config(cfg);
    }

//*
    { // Configure backlight control settings. (Delete if not needed)
      auto cfg = _light_instance.config();    // Get the structure for backlight configuration.

      cfg.pin_bl = 32;              // Pin number where the backlight is connected
      cfg.invert = false;           // Set to true to invert backlight brightness
      cfg.freq   = 44100;           // Backlight PWM frequency
      cfg.pwm_channel = 7;          // PWM channel number to use

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);  // Set the backlight to the panel.
    }
//*/

//*
    { // Configure touch screen control settings. (Delete if not needed)
      auto cfg = _touch_instance.config();

      cfg.x_min      = 0;    // Minimum X value obtainable from the touch screen (raw value)
      cfg.x_max      = 239;  // Maximum X value obtainable from the touch screen (raw value)
      cfg.y_min      = 0;    // Minimum Y value obtainable from the touch screen (raw value)
      cfg.y_max      = 319;  // Maximum Y value obtainable from the touch screen (raw value)
      cfg.pin_int    = 38;   // Pin number where INT is connected
      cfg.bus_shared = true; // Set to true if sharing the bus with the display
      cfg.offset_rotation = 0;// Adjustment when display and touch orientation do not match, set with a value of 0~7

// For SPI connection
      cfg.spi_host = VSPI_HOST;// Select the SPI to use (HSPI_HOST or VSPI_HOST)
      cfg.freq = 1000000;     // Set the SPI clock
      cfg.pin_sclk = 18;     // Pin number where SCLK is connected
      cfg.pin_mosi = 23;     // Pin number where MOSI is connected
      cfg.pin_miso = 19;     // Pin number where MISO is connected
      cfg.pin_cs   =  5;     // Pin number where CS is connected

// For I2C connection
      cfg.i2c_port = 1;      // Select the I2C to use (0 or 1)
      cfg.i2c_addr = 0x38;   // I2C device address
      cfg.pin_sda  = 23;     // Pin number where SDA is connected
      cfg.pin_scl  = 32;     // Pin number where SCL is connected
      cfg.freq = 400000;     // Set the I2C clock

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);  // Set the touch screen to the panel.
    }
//*/

    setPanel(&_panel_instance); // Set the panel to use.
  }
};

// Create an instance of the prepared class.
LGFX display;

void setup(void)
{
  // Initialize the SPI bus and panel to make them ready for use.
  display.init();

  display.setTextSize((std::max(display.width(), display.height()) + 255) >> 8);

  // Perform calibration if touch is available. (Optional)
  if (display.touch())
  {
    if (display.width() < display.height()) display.setRotation(display.getRotation() ^ 1);

    // Draw instruction text on the screen.
    display.setTextDatum(textdatum_t::middle_center);
    display.drawString("touch the arrow marker.", display.width()>>1, display.height() >> 1);
    display.setTextDatum(textdatum_t::top_left);

    // When using touch, calibration is performed. Touch the tips of the arrows displayed at the four corners of the screen in sequence.
    std::uint16_t fg = TFT_WHITE;
    std::uint16_t bg = TFT_BLACK;
    if (display.isEPD()) std::swap(fg, bg);
    display.calibrateTouch(nullptr, fg, bg, std::max(display.width(), display.height()) >> 3);
  }

  display.fillScreen(TFT_BLACK);
}

uint32_t count = ~0;
void loop(void)
{
  display.startWrite();
  display.setRotation(++count & 7);
  display.setColorDepth((count & 8) ? 16 : 24);

  display.setTextColor(TFT_WHITE);
  display.drawNumber(display.getRotation(), 16, 0);

  display.setTextColor(0xFF0000U);
  display.drawString("R", 30, 16);
  display.setTextColor(0x00FF00U);
  display.drawString("G", 40, 16);
  display.setTextColor(0x0000FFU);
  display.drawString("B", 50, 16);

  display.drawRect(30,30,display.width()-60,display.height()-60,count*7);
  display.drawFastHLine(0, 0, 10);

  display.endWrite();

  int32_t x, y;
  if (display.getTouch(&x, &y)) {
    display.fillRect(x-2, y-2, 5, 5, count*7);
  }
}
