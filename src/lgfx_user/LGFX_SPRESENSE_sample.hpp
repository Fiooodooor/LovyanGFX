#pragma once

#define LGFX_USE_V1

#include <LovyanGFX.hpp>

// Configuration example for using LovyanGFX with custom settings on SPRESENSE

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
 for example, if configuring an ILI9341 with SPI connection on SPRESENSE,
  LGFX_SPRESENSE_SPI_ILI9341
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


// Prepare an SPI bus instance.
  lgfx::Bus_SPI       _bus_instance;   // SPI bus instance

public:

  // Create a constructor and configure various settings here.
  // If you changed the class name, specify the same name for the constructor.
  LGFX(void)
  {
    { // Configure bus control settings.
      auto cfg = _bus_instance.config();    // Get the structure for bus configuration.

      cfg.spi_mode = 0;             // Set SPI communication mode (0 ~ 3)
      cfg.freq_write = 40000000;    // SPI clock for transmission
      cfg.freq_read  = 16000000;    // SPI clock for reception
      cfg.pin_dc   =  9;            // Set SPI D/C pin number  (-1 = disable)
      cfg.spi_port =  4;            // Use 4 for Arduino extension board

      _bus_instance.config(cfg);    // Apply the settings to the bus.
      _panel_instance.setBus(&_bus_instance);      // Set the bus to the panel.
    }

    { // Configure display panel control settings.
      auto cfg = _panel_instance.config();    // Get the structure for display panel configuration.

//    cfg.pin_cs           =    -1;  // Pin number connected to CS   (-1 = disable) Specify -1 for HW CS pin
      cfg.pin_rst          =     8;  // Pin number connected to RST  (-1 = disable)
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
      cfg.dlen_16bit       = false;  // Set to true for panels that send data in 16-bit units
      cfg.bus_shared       =  true;  // Set to true if sharing the bus with an SD card (performs bus control in drawJpgFile, etc.)

// Only set the following for drivers with variable pixel counts like ST7735 or ILI9163.
//    cfg.memory_width     =   240;  // Maximum width supported by the driver IC
//    cfg.memory_height    =   320;  // Maximum height supported by the driver IC

      _panel_instance.config(cfg);
    }

    setPanel(&_panel_instance); // Set the panel to use.
  }
};

