#include <LovyanGFX.hpp>

// Connection configuration example for SSD1306 (128x64)
#define TFT_SDA  12
#define TFT_SCL  13
#define I2C_PORT 0
#define I2C_ADDR 0x3c

class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_SSD1306 _panel_instance;
  lgfx::Bus_I2C       _bus_instance;   // I2C bus instance
public:
  LGFX(void)
  {
    {                                    // Configure the bus control settings.
      auto cfg = _bus_instance.config(); // Get the structure for bus configuration.

      // I2C bus configuration
      cfg.i2c_port    = I2C_PORT;   // Select the I2C port to use (0 or 1)
      cfg.freq_write  = 400000;     // Clock frequency for transmission
      cfg.freq_read   = 400000;     // Clock frequency for reception
      cfg.pin_sda     = TFT_SDA;    // Pin number connected to SDA
      cfg.pin_scl     = TFT_SCL;    // Pin number connected to SCL
      cfg.i2c_addr    = I2C_ADDR;   // I2C device address

      _bus_instance.config(cfg);              // Apply the configuration to the bus.
      _panel_instance.setBus(&_bus_instance); // Set the bus to the panel.
    }

    {                                      // Configure the display panel control settings.
      auto cfg = _panel_instance.config(); // Get the structure for display panel configuration.

      cfg.pin_cs   = -1; // Pin number where CS is connected   (-1 = disable)
      cfg.pin_rst  = -1; // Pin number where RST is connected  (-1 = disable)
      cfg.pin_busy = -1; // Pin number where BUSY is connected (-1 = disable)

      cfg.panel_width  = 128; // Actual displayable width
      cfg.panel_height = 64;  // Actual displayable height
      //cfg.offset_x     = 0;   // Panel X-direction offset
      //cfg.offset_y     = 0;   // Panel Y-direction offset

      // cfg.offset_rotation = 4;    // Rotation direction value offset 0~7 (4~7 are upside down)
      // cfg.invert          = true; // Set to true if the panel brightness is inverted
      // cfg.rgb_order       = true; // Set to true if the panel red and blue are swapped

      _panel_instance.config(cfg);
    }

    setPanel(&_panel_instance); // Set the panel to use.
  }
};
