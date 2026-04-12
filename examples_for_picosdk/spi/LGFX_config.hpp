#include <LovyanGFX.hpp>

//#define USE_BACKLIGHT

// Connection configuration example for ST7735S (80x160)
#define TFT_MISO -1
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS   17
#define TFT_DC   26
#define TFT_RST  27
#ifdef USE_BACKLIGHT
#define TFT_BLK  28
#endif
#define SPI_PORT  0

#ifdef USE_TOUCH
#define TOUCH_CS 29
#endif

class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_ST7735S _panel_instance;
  lgfx::Bus_SPI       _bus_instance; // SPI bus instance
#ifdef USE_BACKLIGHT
  lgfx::Light_PWM     _light_instance;
#endif
#ifdef USE_TOUCH
  lgfx::Touch_XPT2046 _touch_instance;
#endif

public:
  LGFX(void)
  {
    {                                    // Configure the bus control settings.
      auto cfg = _bus_instance.config(); // Get the structure for bus configuration.

      // SPI bus configuration
      cfg.spi_host = SPI_PORT;   // Select the SPI port to use (0 or 1)
      cfg.spi_mode = 0;          // Set the SPI communication mode (0 ~ 3)
      cfg.freq_write = 40000000; // SPI clock for transmission (max 80MHz, rounded to 80MHz divided by an integer)
      cfg.freq_read = 20000000;  // SPI clock for reception

      cfg.pin_sclk = TFT_SCLK; // Set the SPI SCLK pin number
      cfg.pin_mosi = TFT_MOSI; // Set the SPI MOSI pin number
      cfg.pin_miso = TFT_MISO; // Set the SPI MISO pin number (-1 = disable)
      cfg.pin_dc   = TFT_DC;   // Set the SPI D/C pin number  (-1 = disable)

      _bus_instance.config(cfg);              // Apply the configuration to the bus.
      _panel_instance.setBus(&_bus_instance); // Set the bus to the panel.
    }

    {                                      // Configure the display panel control settings.
      auto cfg = _panel_instance.config(); // Get the structure for display panel configuration.

      cfg.pin_cs = TFT_CS;   // Pin number where CS is connected   (-1 = disable)
      cfg.pin_rst = TFT_RST; // Pin number where RST is connected  (-1 = disable)
      cfg.pin_busy = -1;     // Pin number where BUSY is connected (-1 = disable)

      cfg.panel_width = 80;   // Actual displayable width
      cfg.panel_height = 160; // Actual displayable height
      cfg.offset_x = 26;      // Panel X-direction offset
      cfg.offset_y = 1;       // Panel Y-direction offset

      // cfg.offset_rotation = 4; // Rotation direction value offset 0~7 (4~7 are upside down)
      cfg.invert = true; // Set to true if the panel brightness is inverted
      // cfg.rgb_order = true;   // Set to true if the panel red and blue are swapped

      _panel_instance.config(cfg);
    }

#ifdef USE_BACKLIGHT
    { // Configure the backlight control settings. (Delete if not needed)
      auto cfg = _light_instance.config();    // Get the structure for backlight configuration.

      cfg.pin_bl = TFT_BLK;         // Pin number where the backlight is connected
      cfg.invert = false;           // Set to true to invert the backlight brightness
      cfg.freq   = 44100;           // PWM frequency for the backlight
      cfg.pwm_channel = 6;          // PWM channel number to use

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);  // Set the backlight to the panel.
    }
#endif

#ifdef USE_TOUCH
    {
      auto cfg = _touch_instance.config();
      cfg.bus_shared = true;
      cfg.spi_host = SPI_PORT;
      cfg.pin_sclk = TFT_SCLK;
      cfg.pin_mosi = TFT_MOSI;
      cfg.pin_miso = TFT_MISO;
      cfg.pin_cs   = TOUCH_CS;

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }
#endif


    setPanel(&_panel_instance); // Set the panel to use.
  }
};
