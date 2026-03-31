# lgfx::Panel_CVBS for ESP32

 - Video signal can be output from GPIO25 or GPIO26 of the ESP32. (ESP32-S2, S3, C3 are not supported)
 - Video signal is generated using I2S channel0 and DAC.
 - Memory consumption is relatively high because a video frame buffer is allocated in the ESP32's memory. (PSRAM can be used)


Types of signals that can be output
----------------

 - NTSC
 - NTSC-J
 - PAL
 - PAL-M
 - PAL-N

NTSC and NTSC-J have different black signal levels; NTSC is set to 7.5 IRE and NTSC-J is set to 0 IRE.<br>
If you notice that blacks appear slightly washed out when using NTSC, try switching to NTSC-J.



Output resolution
----------------

 - The maximum output resolution varies depending on the signal type.
   - 720 x 480  (PAL-M,NTSC,NTSC-J)
   - 864 x 576  (PAL)
   - 720 x 576  (PAL-N)
 - Any resolution can be set as long as it is equal to or less than the maximum resolution.
 - It is recommended to specify a resolution that is the maximum resolution divided by an integer.
 - If other resolutions are specified, the pixel ratio will vary by position, causing the display to appear slightly distorted.


<TABLE>
 <TR>
  <TH></TH>
  <TH> PAL-M <BR> NTSC <BR> NTSC-J </TH>
  <TH> PAL-N </TH>
  <TH> PAL </TH>
 </TR>
 <TR align="center">
  <TH> max width </TH>
  <TD colspan="2"> 720 </TD>
  <TD> 864 </TD>
 </TR>
 <TR align="center">
  <TH> max height </TH>
  <TD> 480 </TD>
  <TD colspan="2"> 576 </TD>
 </TR>
 <TR align="center">
  <TH> recommended<BR>width</TH>
  <TD colspan="2"> 
    720/1 = 720<br>
    720/1.5=480<br>
    720/2 = 360<br>
    720/3 = 240<br>
    720/4 = 180<br>
    720/5 = 144<br>
    720/6 = 120
  </TD>
  <TD>
    864/1 = 864<br>
    864/1.5=576<br>
    864/2 = 432<br>
    864/3 = 288<br>
    864/4 = 216<br>
    864/5 = 173<br>
    864/6 = 144
  </TD>
 </TR>
 <TR align="center">
  <TH> recommended<BR>height</TH>
  <TD>
    480/1 = 480<br>
    480/2 = 240<br>
    480/3 = 160<br>
    480/4 = 120<br>
    480/5 =  96<br>
    480/6 =  80<br>
    480/8 =  60
  </TD>
  <TD colspan="2"> 
    576/1 = 576<br>
    576/2 = 288<br>
    576/3 = 192<br>
    576/4 = 144<br>
    576/5 = 113<br>
    576/6 =  96<br>
    576/8 =  72
  </TD>
 </TR>
</TABLE>




How to use
----------------
```c

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device
{
public:

  lgfx::Panel_CVBS _panel_instance;

  LGFX(void)
  {
    { // Configure display panel control settings.
      auto cfg = _panel_instance.config();    // Get the structure for display panel settings.

      // Set the output resolution;
      cfg.memory_width  = 240; // Output resolution width
      cfg.memory_height = 160; // Output resolution height

      // Set the resolution actually used;
      cfg.panel_width  = 208;  // Actual width used (set to a value equal to or less than memory_width)
      cfg.panel_height = 128;  // Actual height used (set to a value equal to or less than memory_height)

      // Set display position offset;
      cfg.offset_x = 16;       // Amount to shift the display position to the right (default 0)
      cfg.offset_y = 16;       // Amount to shift the display position downward (default 0)

      _panel_instance.config(cfg);


// Normally, set the same value for memory_width and panel_width, and use offset_x = 0.;
// To prevent the display at the screen edges from being hidden outside the screen, set panel_width to a value smaller than memory_width and adjust the horizontal position with offset_x.;
// For example, if panel_width is set to a value 32 less than memory_width, setting offset_x to 16 will center the display horizontally.;
// Adjust the vertical direction (memory_height, panel_height, offset_y) in the same way as needed.;

    }

    {
      auto cfg = _panel_instance.config_detail();

      // Set the output signal type;
      // cfg.signal_type = cfg.signal_type_t::NTSC;
      cfg.signal_type = cfg.signal_type_t::NTSC_J;
      // cfg.signal_type = cfg.signal_type_t::PAL;
      // cfg.signal_type = cfg.signal_type_t::PAL_M;
      // cfg.signal_type = cfg.signal_type_t::PAL_N;

      // Set the output GPIO pin number;
      cfg.pin_dac = 26;       // Only 25 or 26 can be selected because DAC is used;

      // PSRAM memory allocation settings;
      cfg.use_psram = 1;      // 0=No PSRAM / 1=Half PSRAM and half SRAM / 2=All PSRAM;

      // Set the output signal amplitude strength;
      cfg.output_level = 128; // Default value 128
      // * If the signal is attenuated due to protection resistors on the GPIO, increase this value.;
      // * For M5Stack Core2, 200 is recommended because it has protection resistors on the GPIO.;

      // Set the chrominance signal amplitude strength;
      cfg.chroma_level = 128; // Default value 128
      // Lowering the value decreases saturation; 0 produces black and white. Increasing the value increases saturation.;

      // Set the priority of the background task that reads from PSRAM;
      // cfg.task_priority = 25;

      // Select the CPU to run the background task that reads from PSRAM (APP_CPU_NUM or PRO_CPU_NUM);
      // cfg.task_pinned_core = PRO_CPU_NUM;

      _panel_instance.config_detail(cfg);
    }

    setPanel(&_panel_instance);
  }
};


LGFX gfx;

void setup(void)
{
// Specify color depth (default is rgb332_1Byte)
//gfx.setColorDepth( 8);        // RGB332 256 colors
//gfx.setColorDepth(16);        // RGB565 65536 colors
//gfx.setColorDepth(lgfx::color_depth_t::rgb332_1Byte);   // RGB332 256 colors
//gfx.setColorDepth(lgfx::color_depth_t::rgb565_2Byte);   // RGB565 65536 colors
//gfx.setColorDepth(lgfx::color_depth_t::grayscale_8bit); // Grayscale 256 shades

//* It is possible to change the color depth with setColorDepth during execution,
//  but the drawn content will be invalidated due to memory reallocation.

  gfx.init();

  for (int x = 0; x < gfx.width(); ++x)
  {
    int v = x * 256 / gfx.width();
    gfx.fillRect(x, 0 * gfx.height() >> 3, 7, gfx.height() >> 3, gfx.color888(v, v, v));
    gfx.fillRect(x, 1 * gfx.height() >> 3, 7, gfx.height() >> 3, gfx.color888(v, 0 ,0));
    gfx.fillRect(x, 2 * gfx.height() >> 3, 7, gfx.height() >> 3, gfx.color888(0, v, 0));
    gfx.fillRect(x, 3 * gfx.height() >> 3, 7, gfx.height() >> 3, gfx.color888(0, 0, v));
  }
  delay(1000);
}

void loop(void)
{
  gfx.fillRect(rand() % gfx.width() - 8, rand() % gfx.height() - 8, 16, 16, rand());
}
```
