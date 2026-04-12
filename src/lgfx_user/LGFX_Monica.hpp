/**
 * @file hal_disp.hpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-05-20
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <LovyanGFX.hpp>
#include <lgfx/v1/panel/Panel_SH8601Z.hpp>


/// Create a class with custom settings, derived from LGFX_Device.
class LGFX_Monica : public lgfx::LGFX_Device {
    // Prepare an instance that matches the type of panel to connect.
    lgfx::Panel_SH8601Z     _panel_instance;
    // Prepare an instance that matches the type of bus to connect to the panel.
    lgfx::Bus_SPI           _bus_instance;

public:
    // Create a constructor and configure various settings here.
    // If you changed the class name, specify the same name for the constructor.
    LGFX_Monica(void)
    {
        { // Configure bus control settings.
            auto cfg = _bus_instance.config();    // Get the structure for bus configuration.

            // SPI bus settings
            cfg.spi_host = SPI3_HOST;     // Select SPI to use  ESP32-S2,C3 : SPI2_HOST or SPI3_HOST / ESP32 : VSPI_HOST or HSPI_HOST
            // Note: With ESP-IDF version updates, VSPI_HOST and HSPI_HOST are deprecated. If you get errors, use SPI2_HOST or SPI3_HOST instead.
            cfg.spi_mode = 1;             // Set SPI communication mode (0 ~ 3)
            //   cfg.freq_write = 1*1000*1000;    // SPI clock for transmission (max 80MHz, rounded to a value that divides 80MHz evenly)
            //   cfg.freq_write = 10*1000*1000;
            cfg.freq_write = 40*1000*1000;
            cfg.freq_read  = 16000000;    // SPI clock for reception
            cfg.spi_3wire  = true;        // Set to true if reception is done via the MOSI pin
            cfg.use_lock   = true;        // Set to true to use transaction lock
            cfg.dma_channel = SPI_DMA_CH_AUTO; // Set DMA channel to use (0=no DMA / 1=1ch / 2=ch / SPI_DMA_CH_AUTO=auto)

            cfg.pin_sclk    = 7;
            cfg.pin_io0     = 9;
            cfg.pin_io1     = 8;
            cfg.pin_io2     = 5;
            cfg.pin_io3     = 6;
            // cfg.pin_dc      = -1;

            _bus_instance.config(cfg);    // Apply the settings to the bus.
            _panel_instance.setBus(&_bus_instance);      // Set the bus to the panel.
        }

        { // Configure display panel control settings.
            auto cfg = _panel_instance.config();    // Get the structure for display panel configuration.

            cfg.pin_cs           =    13;  // Pin number connected to CS   (-1 = disable)
            cfg.pin_rst          =    1;  // Pin number connected to RST  (-1 = disable)
            cfg.pin_busy         =    -1;  // Pin number connected to BUSY (-1 = disable)

            // Note: The following settings have general default values for each panel, so try commenting out any unknown items.

            cfg.panel_width      =   368;  // Actual displayable width
            cfg.panel_height     =   448;  // Actual displayable height

            // cfg.panel_width      =   320;  // Actual displayable width
            // cfg.panel_height     =   240;  // Actual displayable height


            cfg.offset_x         =     0;  // Panel X offset
            cfg.offset_y         =     0;  // Panel Y offset
            cfg.offset_rotation  =     0;  // Rotation offset value 0~7 (4~7 are upside down)
            cfg.dummy_read_pixel =     8;  // Number of dummy read bits before pixel read
            cfg.dummy_read_bits  =     1;  // Number of dummy read bits before non-pixel data read
            cfg.readable         =  true;  // Set to true if data reading is supported
            cfg.invert           = true;  // Set to true if panel brightness is inverted
            cfg.rgb_order        = true;  // Set to true if panel red and blue are swapped
            cfg.dlen_16bit       = false;  // Set to true for panels that send data in 16-bit units via 16-bit parallel or SPI
            cfg.bus_shared       =  true;  // Set to true if sharing the bus with an SD card (performs bus control in drawJpgFile, etc.)

            // Only set the following for drivers with variable pixel counts like ST7735 or ILI9163 when the display is misaligned.
            cfg.memory_width     =   480;  // Maximum width supported by the driver IC
            cfg.memory_height    =   480;  // Maximum height supported by the driver IC

            _panel_instance.config(cfg);
        }
        setPanel(&_panel_instance); // Set the panel to use.
    }


    inline bool init(void) {

        /* PEN pin */
        gpio_reset_pin(GPIO_NUM_4);
        gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
        gpio_set_pull_mode(GPIO_NUM_4, GPIO_PULLUP_PULLDOWN);
        gpio_set_level(GPIO_NUM_4, 1);
        // vTaskDelay(pdMS_TO_TICKS(10));

        /* TE pin */
        gpio_reset_pin(GPIO_NUM_2);
        gpio_set_direction(GPIO_NUM_2, GPIO_MODE_INPUT);
        // vTaskDelay(pdMS_TO_TICKS(10));

        /* Lgfx */
        return lgfx::LGFX_Device::init();
    }
};
