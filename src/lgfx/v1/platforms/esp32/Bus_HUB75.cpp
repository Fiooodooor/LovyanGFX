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
#if defined (ESP_PLATFORM)
#include <sdkconfig.h>
#if defined (CONFIG_IDF_TARGET_ESP32)

#include "Bus_HUB75.hpp"
#include "../../misc/pixelcopy.hpp"

#include <soc/dport_reg.h>
#include <esp_log.h>

#if defined ( ESP_IDF_VERSION_VAL )
 #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
  #define LGFX_IDF_V5
 #endif
#endif

namespace lgfx
{
 inline namespace v1
 {

//----------------------------------------------------------------------------

  static constexpr uint32_t _conf_reg_default = I2S_TX_MSB_RIGHT | I2S_TX_RIGHT_FIRST | I2S_RX_RIGHT_FIRST | I2S_TX_MONO;
  static constexpr uint32_t _conf_reg_start   = _conf_reg_default | I2S_TX_START;
  static constexpr uint32_t _conf_reg_reset   = _conf_reg_default | I2S_TX_RESET;
  static constexpr uint32_t _sample_rate_conf_reg_direct = 16 << I2S_TX_BITS_MOD_S | 16 << I2S_RX_BITS_MOD_S | 1 << I2S_TX_BCK_DIV_NUM_S | 1 << I2S_RX_BCK_DIV_NUM_S;
  static constexpr uint32_t _fifo_conf_default = 1 << I2S_TX_FIFO_MOD | 1 << I2S_RX_FIFO_MOD | 16 << I2S_TX_DATA_NUM_S | 16 << I2S_RX_DATA_NUM_S;
  static constexpr uint32_t _fifo_conf_dma     = _fifo_conf_default | I2S_DSCR_EN;

  static __attribute__ ((always_inline)) inline volatile uint32_t* reg(uint32_t addr) { return (volatile uint32_t *)ETS_UNCACHED_ADDR(addr); }

  __attribute__((always_inline))
  static inline i2s_dev_t* getDev(i2s_port_t port)
  {
#if SOC_I2C_NUM > 1
    return (port == 0) ? &I2S0 : &I2S1;
#else
    return &I2S0;
#endif
  }

  void Bus_HUB75::config(const config_t& cfg)
  {
    _cfg = cfg;
    _dev = getDev(cfg.i2s_port);
  }

  bool Bus_HUB75::init(void)
  {
    uint32_t dport_clk_en;
    uint32_t dport_rst;

    if (_cfg.i2s_port == I2S_NUM_0) {
      dport_clk_en = DPORT_I2S0_CLK_EN;
      dport_rst = DPORT_I2S0_RST;
    }
#if SOC_I2C_NUM > 1
    else
    {
      dport_clk_en = DPORT_I2S1_CLK_EN;
      dport_rst = DPORT_I2S1_RST;
    }
#endif

    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, dport_clk_en);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, dport_rst);

    auto i2s_dev = (i2s_dev_t*)_dev;
    //Reset I2S subsystem
    i2s_dev->conf.val = I2S_TX_RESET | I2S_RX_RESET | I2S_TX_FIFO_RESET | I2S_RX_FIFO_RESET;
    i2s_dev->conf.val = _conf_reg_default;

    i2s_dev->timing.val = 0;

    //Reset DMA
    i2s_dev->lc_conf.val = I2S_IN_RST | I2S_OUT_RST | I2S_AHBM_RST | I2S_AHBM_FIFO_RST;
    i2s_dev->lc_conf.val = I2S_OUT_EOF_MODE | I2S_OUTDSCR_BURST_EN | I2S_OUT_DATA_BURST_EN;

    i2s_dev->in_link.val = 0;
    i2s_dev->out_link.val = 0;

    i2s_dev->conf1.val = I2S_TX_PCM_BYPASS;
    i2s_dev->conf2.val = I2S_LCD_EN;
    i2s_dev->conf_chan.val = 1 << I2S_TX_CHAN_MOD_S | 1 << I2S_RX_CHAN_MOD_S;

    return true;
  }

  void Bus_HUB75::setImageBuffer(void* buffer, color_depth_t depth)
  {
    _depth = depth;
    auto fb = (DividedFrameBuffer*)buffer;
    _frame_buffer = fb;
    _panel_width = fb->getLineSize() / ((depth & color_depth_t::bit_mask) >> 3);
    _panel_height = fb->getTotalLines();
  }

  __attribute__((always_inline))
  static inline uint32_t _gcd(uint32_t a, uint32_t b)
  {
    uint32_t c = a % b;
    while (c != 0) {
      a = b;
      b = c;
      c = a % b;
    }
    return b;
  }

  static uint32_t getClockDivValue(uint32_t targetFreq)
  {
    // ToDo:get from APB clock.
    uint32_t baseClock = 80 * 1000 * 1000;
    uint32_t n = baseClock / targetFreq;
    uint32_t a = 1;
    uint32_t b = 0;

    if (n == 0)
    {
      n = 1;
    }
    else
    {
      if (n > 255)
      {
        n = 255;
      }
      uint32_t delta_hz = baseClock - targetFreq * n;
      if (delta_hz) {
        uint32_t gcd = _gcd(targetFreq, delta_hz);
        a = targetFreq / gcd;
        b = delta_hz / gcd;
        uint32_t d = a / 63 + 1;
        a /= d;
        b /= d;
      }
    }

    return       I2S_CLK_EN
          | a << I2S_CLKM_DIV_A_S
          | b << I2S_CLKM_DIV_B_S
          | n << I2S_CLKM_DIV_NUM_S
          ;
  }

  void Bus_HUB75::setBrightness(uint8_t brightness)
  {
// ESP_EARLY_LOGE("DEBUG","brightness:%d", brightness);
    _brightness = brightness;
    int br = brightness + 1;
    auto panel_width = _panel_width;
    uint32_t light_len_limit = (panel_width - 7);
    uint32_t slen = (light_len_limit * br) >> 8;

    uint32_t transfer_period_count = TRANSFER_PERIOD_COUNT_332;
    uint32_t half_start = 4;
    if (_depth == color_depth_t::rgb565_2Byte)
    {
      transfer_period_count = TRANSFER_PERIOD_COUNT_565;
      half_start = 6;
    }

    _brightness_period[transfer_period_count] = panel_width;
    for (int period = transfer_period_count - 1; period >= 0; --period)
    {
      _brightness_period[period] = slen + 4;
      if (period < half_start) { slen >>= 1; }
// ESP_EARLY_LOGE("DEBUG","period%d  = %d", period, slen);
    }
  }

  void Bus_HUB75::release(void)
  {
    endTransaction();
  }

  void Bus_HUB75::switch_gpio_control(bool switch_to_dma)
  {
    auto idx_base = SIG_GPIO_OUT_IDX;
    if (switch_to_dma)
    {
#if SOC_I2C_NUM > 1
      idx_base = (_cfg.i2s_port == I2S_NUM_0) ? I2S0O_DATA_OUT8_IDX : I2S1O_DATA_OUT8_IDX;
#else
      idx_base = I2S0O_DATA_OUT8_IDX;
#endif
    }

    for (size_t i = 0; i < 14; ++i)
    {
      if (_cfg.pin_data[i] < 0) { continue; }

#if defined ( LGFX_IDF_V5 )
      esp_rom_gpio_pad_select_gpio(_cfg.pin_data[i]);
      esp_rom_gpio_connect_out_signal(_cfg.pin_data[i  ], idx_base, 0, 0);
#else
      gpio_pad_select_gpio(_cfg.pin_data[i]);
      gpio_matrix_out(_cfg.pin_data[i  ], idx_base, 0, 0);
#endif
      if (switch_to_dma)
      {
        ++idx_base;
      }
      else
      {
        gpio_lo(_cfg.pin_data[i]);
      }
    }

    if (switch_to_dma)
    {
#if SOC_I2C_NUM > 1
      idx_base = (_cfg.i2s_port == I2S_NUM_0) ? I2S0O_WS_OUT_IDX : I2S1O_WS_OUT_IDX;
#else
      idx_base = I2S0O_WS_OUT_IDX;
#endif
#if defined ( LGFX_IDF_V5 )
      esp_rom_gpio_connect_out_signal(_cfg.pin_clk, idx_base, 1, 0); // clock Active-low
#else
      gpio_matrix_out(_cfg.pin_clk, idx_base, 1, 0); // clock Active-low
#endif
    }
    else
    {
      gpio_hi(_cfg.pin_oe);
    }
  }

  void Bus_HUB75::send_led_driver_command(uint8_t latcycle, uint16_t r, uint16_t g, uint16_t b)
  {
    for (size_t i = 0; i < _panel_width; ++i)
    {
      if (i == (_panel_width - latcycle))
      {
        gpio_hi(_cfg.pin_lat);
      }
      uint32_t mask = 0x8000 >> (i & 15);
      if (r & mask)
      {
        gpio_hi(_cfg.pin_r1);
        gpio_hi(_cfg.pin_r2);
      }
      else
      {
        gpio_lo(_cfg.pin_r1);
        gpio_lo(_cfg.pin_r2);
      }
      if (g & mask)
      {
        gpio_hi(_cfg.pin_g1);
        gpio_hi(_cfg.pin_g2);
      }
      else
      {
        gpio_lo(_cfg.pin_g1);
        gpio_lo(_cfg.pin_g2);
      }
      if (b & mask)
      {
        gpio_hi(_cfg.pin_b1);
        gpio_hi(_cfg.pin_b2);
      }
      else
      {
        gpio_lo(_cfg.pin_b1);
        gpio_lo(_cfg.pin_b2);
      }
      gpio_hi(_cfg.pin_clk);
      gpio_lo(_cfg.pin_clk);
    }
    gpio_lo(_cfg.pin_lat);
  }

  void Bus_HUB75::send_led_driver_latch(uint8_t latcycle)
  {
    gpio_hi(_cfg.pin_lat);

    for (size_t i = 0; i < latcycle; ++i)
    {
      gpio_hi(_cfg.pin_clk);
      gpio_lo(_cfg.pin_clk);
    }
    gpio_lo(_cfg.pin_lat);
  }

  void Bus_HUB75::beginTransaction(void)
  {
    if (_dmatask_handle)
    {
      return;
    }


/*  // DMA memory location table used by DMA descriptors
 This array represents the buffer memory range used by each of the 14x3 sets of DMA descriptors.
  0 : no data, x1 lighting
  1 : brightness 1/32 data, no lighting
  2 : brightness 1/16 data, 1/32 lighting
  3 : brightness 1/8 data, 1/16 lighting
  4 : brightness 1/4 data, 1/8 lighting
  5 : brightness 1/2 data, 1/4 lighting
  6 : brightness x1 data, 1/2 lighting
  7 : brightness x2 data, x1 lighting
  8 : brightness x4 data, x1 lighting
  9 : SHIFTREG_ABC coordinates, no lighting (data size is smaller compared to other periods; equivalent to panel height)
*/
    static constexpr const uint8_t dma_buf_idx_tbl_565[] = {
      9, 1, 2, 3, 4, 5, 6, 7, 0, 8, 0, 0, 0, 0,
    };

// For RGB332, the number of gradation levels is reduced to decrease total data volume.
// Data transfer occurs 5 times, no-data lighting occurs 2 times. SHIFTREG_ABC coordinates are at [6]
    static constexpr const uint8_t dma_buf_idx_tbl_332[] = {
      6, 1, 2, 3, 4, 5, 0, 0,
    };

/* The roles of each DMA descriptor for RGB565 are as follows:
  [ 0](SHIFTREG_ABC coordinate transfer, no-lighting period)
  [ 1](brightness 1/32 component data transfer, no-lighting period)
  [ 2](brightness 1/16 component data transfer, 1/32 lighting period)
  [ 3](brightness 1/8 component data transfer, 1/16 lighting period)
  [ 4](brightness 1/4 component data transfer, 1/8 lighting period)
  [ 5](brightness 1/2 component data transfer, 1/4 lighting period)
  [ 6](brightness 1/1 component data transfer, 1/2 lighting period)
  [ 7](brightness x2 component data transfer, x1 lighting period)
  [ 8](                         no data, x1 lighting period)
  [ 9](brightness x4 component data transfer, x1 lighting period)
  [10](                         no data, x1 lighting period)
  [11](                         no data, x1 lighting period)
  [12](                         no data, x1 lighting period)
  [13](                         no data, x1 lighting period)
  * After descriptor 13 finishes transfer, it links to the first descriptor of the next line.
    Also, when descriptor 13 finishes, an EOF interrupt occurs and the data generation task for the next line is executed.

  * The transfer period for descriptor 0 (SHIFTREG_ABC coordinates, no lighting) is proportional to the height of one panel; all other periods are proportional to the total panel width.

   To reproduce 8-bit color depth, data with different brightness components is sent 8 times for the same line.
   For the latter brightness x2 and x4 data, since the lighting period is longer, multiple x1 lighting periods are provided to achieve the brightness difference.
   Since the content sent during "no-data x1 lighting periods" can all be identical, the same memory range is shared to save memory.
*/

    uint32_t transfer_period_count = TRANSFER_PERIOD_COUNT_332;
    uint32_t extend_period_count = EXTEND_PERIOD_COUNT_332;
    uint32_t total_period_count = TOTAL_PERIOD_COUNT_332;
    auto dma_buf_idx_tbl = dma_buf_idx_tbl_332;
    if (_depth == color_depth_t::rgb565_2Byte)
    {
      transfer_period_count = TRANSFER_PERIOD_COUNT_565;
      extend_period_count = EXTEND_PERIOD_COUNT_565;
      total_period_count = TOTAL_PERIOD_COUNT_565;
      dma_buf_idx_tbl = dma_buf_idx_tbl_565;
    }

    _dmadesc = (lldesc_t*)heap_alloc_dma(sizeof(lldesc_t) * total_period_count * _dma_desc_set);
    if (_dmadesc == nullptr)
    {
      ESP_EARLY_LOGE("Bus_HUB75", "memory allocate error.");
      endTransaction();
      return;
    }

    uint32_t panel_width = _panel_width;

    // Allocate DMA buffer memory. Reserve a contiguous region totaling (1 no-data lighting period + 8 data transfer periods) * panel width + (1 SHIFTREG_ABC coordinate period) * (panel height * 2)
    // Although there are 5 no-data lighting periods in total, only 1 buffer is needed since the same region is reused;
    size_t buf_bytes = (((transfer_period_count + 1) * panel_width) + (2 * _panel_height)) * sizeof(uint16_t);
    _dma_transfer_len = (((transfer_period_count + extend_period_count) * panel_width) + (2 * _panel_height));

    for (size_t i = 0; i < _dma_desc_set; i++) {
      _dma_buf[i] = (uint16_t*)heap_alloc_dma(buf_bytes);
      if (_dma_buf[i] == nullptr) {
        ESP_EARLY_LOGE("Bus_HUB75", "memory allocate error.");
        endTransaction();
        return;
      }
      // Fill buffer with OE (lights off) as the initial value
      memset(_dma_buf[i], _mask_oe, buf_bytes);

      for (int j = 0; j < total_period_count; j++) {
        uint32_t idx = i * total_period_count + j;
        size_t bufidx = dma_buf_idx_tbl[j] * panel_width;
        // Set data length to panel_height * 2 only for the SHIFTREG_ABC period
        size_t buflen = ((j == 0) ? _panel_height << 1 : panel_width) * sizeof(uint16_t);
        _dmadesc[idx].buf = (volatile uint8_t*)&(_dma_buf[i][bufidx]);
        _dmadesc[idx].eof = j == (total_period_count - 1); // Generate EOF event only for the last transfer period
        _dmadesc[idx].empty = (uint32_t)(&_dmadesc[(idx + 1) % (total_period_count * _dma_desc_set)]);
        _dmadesc[idx].owner = 1;
        _dmadesc[idx].length = buflen;
        _dmadesc[idx].size = buflen;
      }
    }
    setBrightness(_brightness);

    { // Gamma correction table generation
    // Generate data that performs both gamma correction and bit spacing expansion simultaneously.
      if (_depth == color_depth_t::rgb565_2Byte)
      {
        // For RGB565, create a 64-element table used for a single color component.
        // Convert to 3-bit spacing to make it easier to combine RGB components during use.
        _pixel_tbl = (uint32_t*)heap_alloc_dma(64 * sizeof(uint32_t));
        if (_pixel_tbl == nullptr)
        {
          ESP_EARLY_LOGE("Bus_HUB75", "memory allocate error.");
          endTransaction();
          return;
        }
        for (size_t i = 0; i < 64; ++i)
        {
          uint_fast8_t v = ((i * 65) >> 4) + 1;
          v = (v * v) >> 8;
          if (v < i) { v = i; }
          else if (v > 255) { v = 255; }

          // Widen the spacing between each bit of the data
          uint32_t value = 0;
          for (size_t shift = 0; shift < 8; ++shift)
          {
            uint32_t mask = (1 << shift);
            value |= (v & mask) << (shift * 2);
          }
          _pixel_tbl[i] = value;
        }
      }
      else
      {
        // For RGB332, create a 256-element table that can convert all 3 colors at once.
        // Create data in a format where 5 sets of 6-bit groups (3-bit BGR + 3-bit padding) are arranged.
        _pixel_tbl = (uint32_t*)heap_alloc_dma(256 * sizeof(uint32_t));
        if (_pixel_tbl == nullptr)
        {
          ESP_EARLY_LOGE("Bus_HUB75", "memory allocate error.");
          endTransaction();
          return;
        }
        for (size_t rgb332 = 0; rgb332 < 256; ++rgb332)
        {
          uint_fast16_t r = 1 + (((rgb332 & 0xE0u) * 0b01001u) >> 5);
          uint_fast16_t g = 1 + (((rgb332 & 0x1Cu) * 0b01001u) >> 2);
          uint_fast16_t b = 1 + (((rgb332 & 0x03u) * 0b10101u)     );
          r = (r * r + 31) >> 7;
          g = (g * g + 31) >> 7;
          b = (b * b + 31) >> 7;
          if (r > 31) { r = 31; }
          if (g > 31) { g = 31; }
          if (b > 31) { b = 31; }

          uint32_t value = 0;
          for (size_t shift = 0; shift < 5; ++shift)
          {
            uint32_t mask = (1 << shift);
            value |= (r & mask) << (shift * 5 + 0)
                  |  (g & mask) << (shift * 5 + 1)
                  |  (b & mask) << (shift * 5 + 2);
          }
          _pixel_tbl[rgb332] = value;
        }
      }
    }

    if (_cfg.led_driver)
    { // Register settings for each LED driver type
      switch_gpio_control(false);

      switch (_cfg.led_driver)
      {
      default:
        break;

      case config_t::led_driver_t::led_driver_FM6124:
        {
          uint8_t br = _cfg.driver_brightness >> 4;
          uint16_t cmd11 = 0b0000000001100000 | br << 7;
          uint16_t cmd12 = 0b0000000001000000;
          send_led_driver_command(11, cmd11, cmd11, cmd11);
          send_led_driver_command(12, cmd12, cmd12, cmd12);
        }
        break;

/* ToDo:implement
      case config_t::led_driver_t::led_driver_FM6047:
        break;

      case config_t::led_driver_t::led_driver_ICN2038:
      case config_t::led_driver_t::led_driver_MBI5038:
          send_led_driver_latch(13);  // Pre-active cmd
        break;
//*/

      case config_t::led_driver_t::led_driver_ICN2053:
      case config_t::led_driver_t::led_driver_MBI5153:
        {
          uint16_t cmd4  = 0b0001111101110000;
          uint16_t cmd6  = 0xffff;
          uint16_t cmd8  = 0b0100000011110011;
          uint16_t cmd10 = 0;

          send_led_driver_latch(14);  // Pre-active cmd
          send_led_driver_latch(12);  // Enable all output ch
          send_led_driver_latch( 3);  // Vsync
          send_led_driver_latch(14);  // Pre-active cmd
          send_led_driver_command(4, cmd4, cmd4, cmd4); // write cfg reg 1
          send_led_driver_latch(14);  // Pre-active cmd
          send_led_driver_command(6, cmd6, cmd6, cmd6); // write cfg reg 2
          send_led_driver_latch(14);  // Pre-active cmd
          send_led_driver_command(8, cmd8, cmd8, cmd8); // write cfg reg 3
          send_led_driver_latch(14);  // Pre-active cmd
          send_led_driver_command(10, cmd10, cmd10, cmd10); // write cfg reg 4
          send_led_driver_latch(14);  // Pre-active cmd
          send_led_driver_command(2, 0, 0, 0); // write debug reg
        }
        break;
      }
    }

    switch_gpio_control(true);

    auto i2s_dev = (i2s_dev_t*)_dev;
    i2s_dev->out_link.val = 0;
    i2s_dev->fifo_conf.val = _fifo_conf_dma;
    i2s_dev->sample_rate_conf.val = _sample_rate_conf_reg_direct;

    i2s_dev->clkm_conf.val = getClockDivValue(_cfg.freq_write);
    i2s_dev->conf.val = _conf_reg_reset;
    i2s_dev->out_link.val = I2S_OUTLINK_START | ((uint32_t)_dmadesc & I2S_OUTLINK_ADDR);

    i2s_dev->int_ena.val = 0;
    i2s_dev->int_clr.val = ~0u;
    i2s_dev->int_ena.out_eof = 1;

#if portNUM_PROCESSORS > 1
    if (((size_t)_cfg.task_pinned_core) < portNUM_PROCESSORS)
    {
      xTaskCreatePinnedToCore(dmaTask, "hub75dma", 4096, this, _cfg.task_priority, &_dmatask_handle, _cfg.task_pinned_core);
    }
    else
#endif
    {
      xTaskCreate(dmaTask, "hub75dma", 4096, this, _cfg.task_priority, &_dmatask_handle);
    }
  }

  void Bus_HUB75::setRefreshRate(uint16_t refresh_rate)
  {
    // Set the transmission clock based on total transfer data volume and refresh rate
    _cfg.freq_write = (_dma_transfer_len >> 1) * _panel_height * refresh_rate;
    auto i2s_dev = (i2s_dev_t*)_dev;
    if (i2s_dev)
    {
      i2s_dev->clkm_conf.val = getClockDivValue(_cfg.freq_write);
    }
  }

  void Bus_HUB75::endTransaction(void)
  {
    gpio_hi(_cfg.pin_oe);
#if defined ( LGFX_IDF_V5 )
    esp_rom_gpio_connect_out_signal(_cfg.pin_oe, SIG_GPIO_OUT_IDX, 0, 0);
#else
    gpio_matrix_out(_cfg.pin_oe, SIG_GPIO_OUT_IDX, 0, 0);
#endif

    auto i2s_dev = (i2s_dev_t*)_dev;
    i2s_dev->int_ena.val = 0;
    i2s_dev->int_clr.val = ~0u;
    i2s_dev->out_link.stop = 1;
    i2s_dev->conf.val = _conf_reg_reset;
    i2s_dev->out_link.val = 0;

    if (_dmatask_handle)
    {
      auto handle = _dmatask_handle;
      _dmatask_handle = nullptr;
      xTaskNotify(handle, 0, eNotifyAction::eSetValueWithOverwrite);
      vTaskDelay(1);
    }

    if (_pixel_tbl)
    {
      heap_free(_pixel_tbl);
      _pixel_tbl = nullptr;
    }
    if (_dmadesc)
    {
      heap_free(_dmadesc);
      _dmadesc = nullptr;
    }
    for (size_t i = 0; i < _dma_desc_set; i++)
    {
      if (_dma_buf[i])
      {
        heap_free(_dma_buf[i]);
        _dma_buf[i] = nullptr;
      }
    }
  }

  void IRAM_ATTR Bus_HUB75::i2s_intr_handler_hub75(void *arg)
  {
    auto me = (Bus_HUB75*)arg;
    auto dev = getDev(me->_cfg.i2s_port);
    dev->int_clr.val = dev->int_st.val;

    auto desc = (lldesc_t*)dev->out_eof_des_addr;
    xTaskNotifyFromISR(me->_dmatask_handle, (uint32_t)desc->buf, eNotifyAction::eSetValueWithOverwrite, nullptr);
    portYIELD_FROM_ISR();
  }

  struct asm_work_t
  {
    uint32_t* d32;            //  0
    uint32_t* s32h;           //  4
    uint32_t* s32l;           //  8
    uint32_t* pixel_tbl;      // 12
    uint32_t* mixdata;        // 16
    uint16_t* xe_tbl;         // 20
    uint32_t len32;           // 24
    uint32_t xe_idx;          // 28
    uint32_t xe;              // 32
    uint32_t mask3bit;        // 36
    uint32_t mix_value;       // 40
    uint32_t* _retaddr;       // 44 A0 register save area
  };

  static void hub75Draw332_asm(asm_work_t* work)
  {
/* Register values immediately after the function is called:
    a0 : return address (saved to work, a0 is used for other purposes)
    a1 : stack pointer (must not be modified)
    a2 : asm_work_t*  (used as-is without modification)
*/
    __asm__ __volatile__ (
      "s32i.n  a0,  a2,  44               \n"  // Save A0
      "l32i.n  a3,  a2,  32               \n"  // a3  = xe
      "l32i.n  a0,  a2,  0                \n"  // *a0  = output destination address
      "l32i.n  a15, a2,  16               \n"  // *a15 = mixdata address
      "l32i.n  a14, a2,  24               \n"  // *a14 = len32
      "l32i.n  a13, a2,  12               \n"  // *a13 = pixel_tbl
      "l32i.n  a12, a2,  8                \n"  // *a12 = lower panel source data array
      "l32i.n  a11, a2,  4                \n"  // *a11 = upper panel source data array
      "slli    a14, a14, 2                \n"  // multiply len32 by 4 (for d32 addition)

"HUB75_DRAW332_LOOP_START:          \n"

      "loop    a3, HUB75_DRAW332_LOOP_END \n"  // loop start (a3 must contain loop count xe when reaching here)

      "l32i.n  a10, a15, 16               \n"  // *a10 = assign last value of mixdata (4*sizeof(uint32_t) = 16)

      "l8ui    a3,  a11, 0                \n"  // a3 = get rgb332 format pixel 1 from upper source data array
      "l8ui    a4,  a12, 0                \n"  // a4 = get rgb332 format pixel 1 from lower source data array
      "l8ui    a5,  a11, 1                \n"  // a5 = get rgb332 format pixel 2 from upper source data array
      "l8ui    a6,  a12, 1                \n"  // a6 = get rgb332 format pixel 2 from lower source data array
      "addi.n  a11, a11, 2                \n"  // advance source data address by 2 pixels
      "addi.n  a12, a12, 2                \n"  // advance source data address by 2 pixels

      "addx4   a3,  a3,  a13              \n"  // a3  = convert to table address
      "addx4   a4,  a4,  a13              \n"  // a4  = convert to table address
      "addx4   a5,  a5,  a13              \n"  // a5  = convert to table address
      "addx4   a6,  a6,  a13              \n"  // a6  = convert to table address
      "l32i.n  a3,  a3,  0                \n"  // a3  = pixel_tbl[RGB332] pixel 1 upper RGB component complete
      "l32i.n  a4,  a4,  0                \n"  // a4  = pixel_tbl[RGB332] pixel 1 lower RGB component complete
      "l32i.n  a5,  a5,  0                \n"  // a5  = pixel_tbl[RGB332] pixel 2 upper RGB component complete
      "l32i.n  a6,  a6,  0                \n"  // a6  = pixel_tbl[RGB332] pixel 2 lower RGB component complete

//////////////////

      "s32i.n  a10, a0,  0                \n"  // set last mixdata value to output destination
      "mov.n   a9,  a0                    \n"  // copy output destination address to a9
      "addi.n  a0,  a0,  4                \n"  // *a0 advance output destination address by 1

// At this point, a3, a4, a5, a6 contain data for a total of 4 pixels
// The data obtained from the table is in the format 0bBGR___BGR___BGR___BGR___BGR
// (6-bit units: 3-bit padding + BGR)
// From here, convert so that upper and lower panel RGB components are adjacent in 6-bit units
      "addx8   a3,  a4,  a3               \n"  // a3 = (lower pixel 1 << 3) + upper pixel 1
      "addx8   a4,  a6,  a5               \n"  // a4 = (lower pixel 2 << 3) + upper pixel 2

// Output starts here
// Combine RGB components with mixdata (Y coordinate info + OE signal) into 16-bit data, output 2 pixels as 32-bit, 5 times (transfer_period_count)

      "l32i.n   a8,  a15, 0               \n"  // a8  = mixdata[0]
      "add.n    a9,  a14, a9              \n"  // a9 output destination address += len32
      "extui    a7,  a3,  0,   6          \n"  // pixel 1: get 6 bits
      "slli     a7,  a7,  16              \n"  // shift pixel 1 data left by 16 bits
      "add.n    a8,  a7,  a8              \n"  // a8 = pixel 1 + mixdata
      "extui    a7,  a4,  0,   6          \n"  // pixel 2: get 6 bits
      "add.n    a7,  a7,  a8              \n"  // a7 = pixel 2 + a8
      "s32i.n   a7,  a9,  0               \n"  // set a7 value to output destination

      "l32i.n   a8,  a15, 4               \n"  // a8 = mixdata[1]
      "add.n    a9,  a14, a9              \n"  // a9 output destination address += len32
      "extui    a7,  a3,  6,   6          \n"  // pixel 1: get 6 bits
      "slli     a7,  a7,  16              \n"  // shift pixel 1 data left by 16 bits
      "add.n    a8,  a7,  a8              \n"  // a8 = pixel 1 + mixdata
      "extui    a7,  a4,  6,   6          \n"  // pixel 2: get 6 bits
      "add.n    a7,  a7,  a8              \n"  // a7 = pixel 2 + a8
      "s32i.n   a7,  a9,  0               \n"  // set a7 value to output destination

      "l32i.n   a8,  a15, 8               \n"  // a8  = mixdata[2]
      "add.n    a9,  a14, a9              \n"  // a9 output destination address += len32
      "extui    a7,  a3,  12,  6          \n"  // pixel 1: get 6 bits
      "slli     a7,  a7,  16              \n"  // shift pixel 1 data left by 16 bits
      "add.n    a8,  a7,  a8              \n"  // a8 = pixel 1 + mixdata
      "extui    a7,  a4,  12,  6          \n"  // pixel 2: get 6 bits
      "add.n    a7,  a7,  a8              \n"  // a7 = pixel 2 + a8
      "s32i.n   a7,  a9,  0               \n"  // set a7 value to output destination

      "l32i.n   a8,  a15, 12              \n"  // a8 = mixdata[3]
      "add.n    a9,  a14, a9              \n"  // a9 output destination address += len32
      "extui    a7,  a3,  18,  6          \n"  // pixel 1: get 6 bits
      "slli     a7,  a7,  16              \n"  // shift pixel 1 data left by 16 bits
      "add.n    a8,  a7,  a8              \n"  // a8 = pixel 1 + mixdata
      "extui    a7,  a4,  18,  6          \n"  // pixel 2: get 6 bits
      "add.n    a7,  a7,  a8              \n"  // a7 = pixel 2 + a8
      "s32i.n   a7,  a9,  0               \n"  // set a7 value to output destination

      // Skip mixdata fetch for the last iteration (reuse value previously fetched into a10)
      "add.n    a9,  a14, a9              \n"  // a9 output destination address += len32
      "extui    a7,  a3,  24,  6          \n"  // pixel 1: get 6 bits
      "slli     a7,  a7,  16              \n"  // shift pixel 1 data left by 16 bits
      "add.n    a8,  a7,  a10             \n"  // a8 = pixel 1 + mixdata
      "extui    a7,  a4,  24,  6          \n"  // pixel 2: get 6 bits
      "add.n    a7,  a7,  a8              \n"  // a7 = pixel 2 + a8
      "s32i.n   a7,  a9,  0               \n"  // set a7 value to output destination

"HUB75_DRAW332_LOOP_END:            \n"

      "l32i.n  a4,  a2,  28               \n" // a4 = assign xe_idx
      "l32i.n  a5,  a2,  20               \n" // a5 = assign xe_tbl
      "l32i.n  a9,  a2,  40               \n" // a9 = get mixdata table update value
      "beqi    a4,  5,   HUB75_EXIT332    \n" // if xe_idx has reached the end, exit processing
      "addx2   a6,  a4,  a5               \n" // a6 = address of current xe table index
      "l16ui   a3,  a6,  0                \n" // a3 = current xe value
      "srli    a3 , a3,  1                \n" // a3 >>= 1
// From here, update mixdata values and reset xe position
"HUB75_BR332LOOP_START:             \n"
        "mov     a5,  a3                    \n" // move previous xe value to a5
        "l16ui   a3,  a6,  2                \n" // a3 = assign new xe value
        "addi.n  a6,  a6,  2                \n" // a6 advance xe table position by one
        "addi.n  a4,  a4,  1                \n" // a4 ++xe_idx
        "addx4   a8,  a4,  a15              \n" // a8 = set address of target mixdata to update
        "s32i.n  a9,  a8,  0                \n" // update mixdata
        "srli    a3,  a3,  1                \n" // a3 >>= 1
      "bge     a5,  a3,  HUB75_BR332LOOP_START     \n" // if xe values are equal, retry BR_LOOP
      "s32i.n  a4,  a2,  28               \n" // save xe_idx value
      "sub     a3,  a3,  a5               \n" // subtract previous xe value from new xe value to get the difference
      "j HUB75_DRAW332_LOOP_START         \n" // a3 (xe) has been updated, loop from the beginning again

"HUB75_EXIT332:                   \n"

      "l32i   a0,  a2,  44                \n"
      );
  }

  static void hub75Draw565_asm(asm_work_t* work)
  {
/* Register values immediately after the function is called:
    a0 : return address (saved to work, a0 is used for other purposes)
    a1 : stack pointer (must not be modified)
    a2 : asm_work_t*  (used as-is without modification)
*/
    __asm__ __volatile__ (
      "s32i.n  a0,  a2,  44               \n"  // Save a0
      "l32i.n  a3,  a2,  32               \n"  // a3  = xe
      "l32i.n  a0,  a2,  0                \n"  // *a0  = output destination address
      "l32i.n  a11, a2,  4                \n"  // *a11 = upper panel source data array
      "l32i.n  a12, a2,  8                \n"  // *a12 = lower panel source data array
      "l32i.n  a13, a2,  12               \n"  // *a13 = pixel_tbl
      "l32i.n  a14, a2,  24               \n"  // *a14 = len32
      "movi    a15, 0b111000111000111000111000111   \n"  // set mask pattern in A15
      "slli    a14, a14, 2                \n"  // multiply len32 by 4 (for d32 addition)
      "s32i.n  a15, a2,  36               \n"  // save mask pattern to work

"HUB75_DRAW565_LOOP_START:          \n"

      "loop    a3, HUB75_DRAW565_LOOP_END \n"  // loop start (a3 must contain loop count xe when reaching here)

      "l32i.n  a9,  a11, 0                \n"  // a9  = fetch 2 pixels in rgb565 format from source data array
      "l32i.n  a10, a12, 0                \n"  // a10 = fetch 2 pixels in rgb565 format from source data array
      "addi.n  a11, a11, 4                \n"  // advance source data address by 2 pixels
      "addi.n  a12, a12, 4                \n"  // advance source data address by 2 pixels

//////////////////

      "extui   a5,  a9,  0,   5           \n"  // a5  = get blue component
      "extui   a6,  a9,  5,   6           \n"  // a6  = get green component
      "extui   a7,  a9,  11,  5           \n"  // a7  = get red component
      "addx8   a4,  a5,  a13              \n"  // a4  = convert to table address (x8 to index uint32_t[64] table from 5-bit data)
      "addx4   a5,  a6,  a13              \n"  // a5  = convert to table address (x4 to index uint32_t[64] table from 6-bit data)
      "addx8   a6,  a7,  a13              \n"  // a6  = convert to table address (x8 to index uint32_t[64] table from 5-bit data)
      "l32i.n  a3,  a4,  4                \n"  // a3  = pixel_tbl[blue component]
      "l32i.n  a4,  a5,  0                \n"  // a4  = pixel_tbl[green component]
      "l32i.n  a5,  a6,  4                \n"  // a5  = pixel_tbl[red component]

      // Interleave subsequent processing while waiting for load
      "extui   a6,  a10, 0,   5           \n"  // a6  = get blue component
      "extui   a7,  a10, 5,   6           \n"  // a7  = get green component
      "extui   a8,  a10, 11,  5           \n"  // a8  = get red component

      "addx2   a3,  a3,  a4               \n"  // a3  = (blue << 1) + green
      "addx2   a3,  a3,  a5               \n"  // a3  = (blue+green << 1) + red: pixel 1 upper RGB component complete
//////////////////

      "addx8   a5,  a6,  a13              \n"  // a5  = convert to table address
      "addx4   a6,  a7,  a13              \n"  // a6  = convert to table address
      "addx8   a7,  a8,  a13              \n"  // a7  = convert to table address
      "l32i.n  a4,  a5,  4                \n"  // a4  = pixel_tbl[blue component]
      "l32i.n  a5,  a6,  0                \n"  // a5  = pixel_tbl[green component]
      "l32i.n  a6,  a7,  4                \n"  // a6  = pixel_tbl[red component]

      // Interleave subsequent processing while waiting for load
      "extui   a7,  a9,  16,  5           \n"  // a7  = get blue component
      "extui   a8,  a9,  21,  6           \n"  // a8  = get green component
      "extui   a9,  a9,  27,  5           \n"  // a9  = get red component

      "addx2   a4,  a4,  a5               \n"  // a4  = (blue << 1) + green
      "addx2   a4,  a4,  a6               \n"  // a4  = (blue+green << 1) + red: pixel 1 lower RGB component complete
//////////////////

      "addx8   a6,  a7,  a13              \n"  // a6  = convert to table address
      "addx4   a7,  a8,  a13              \n"  // a7  = convert to table address
      "addx8   a8,  a9,  a13              \n"  // a8  = convert to table address
      "l32i.n  a5,  a6,  4                \n"  // a5  = pixel_tbl[blue component]
      "l32i.n  a6,  a7,  0                \n"  // a6  = pixel_tbl[green component]
      "l32i.n  a7,  a8,  4                \n"  // a7  = pixel_tbl[red component]

      // Interleave subsequent processing while waiting for load
      "extui   a8,  a10, 16,  5           \n"  // a8  = get blue component
      "extui   a9,  a10, 21,  6           \n"  // a9  = get green component
      "extui   a10, a10, 27,  5           \n"  // a10 = get red component

      "addx2   a5,  a5,  a6               \n"  // a5  = (blue << 1) + green
      "addx2   a5,  a5,  a7               \n"  // a5  = (blue+green << 1) + red: pixel 2 upper RGB component complete
//////////////////

      "addx8   a7,  a8,  a13              \n"  // a7  = convert to table address
      "addx4   a8,  a9,  a13              \n"  // a8  = convert to table address
      "addx8   a9,  a10, a13              \n"  // a9  = convert to table address
      "l32i.n  a6,  a7,  4                \n"  // a6  = pixel_tbl[blue component]
      "l32i.n  a7,  a8,  0                \n"  // a7  = pixel_tbl[green component]
      "l32i.n  a8,  a9,  4                \n"  // a8  = pixel_tbl[red component]

      // Interleave subsequent processing while waiting for load
      "l32i.n  a15, a2,  36               \n"  // load mask pattern into a15

      "addx2   a6,  a6,  a7               \n"  // a6  = (blue << 1) + green
      "addx2   a6,  a6,  a8               \n"  // a6  = (blue+green << 1) + red: pixel 2 lower RGB component complete

// At this point, a3, a4, a5, a6 contain data for a total of 4 pixels
// From here, convert so that upper and lower panel RGB components are adjacent in 6-bit units

      "and     a7,  a15, a3               \n"  // a7  = upper_1_odd
      "and     a8,  a15, a4               \n"  // a8  = lower_1_odd
      "and     a9,  a15, a5               \n"  // a9  = upper_2_odd
      "and     a10, a15, a6               \n"  // a10 = lower_2_odd
      "slli    a15, a15, 3                \n"  // invert a15 mask pattern
      "and     a3,  a15, a3               \n"  // a3  = upper_1_even
      "and     a4,  a15, a4               \n"  // a4  = lower_1_even
      "and     a5,  a15, a5               \n"  // a5  = upper_2_even
      "and     a6,  a15, a6               \n"  // a6  = lower_2_even

// Output preparation
      "l32i.n  a15, a2,  16               \n"  // assign mixdata address to a15

      "addx8   a6,  a6,  a5               \n"  // a6 = (lower_2_even << 3) + upper_2_even
      "addx8   a5,  a4,  a3               \n"  // a5 = (lower_1_even << 3) + upper_1_even
      "addx8   a4,  a10, a9               \n"  // a4 = (lower_2_odd << 3) + upper_2_odd
      "addx8   a3,  a8,  a7               \n"  // a3 = (lower_1_odd << 3) + upper_1_odd

      "l32i.n  a10, a15, 28               \n"  // assign last value of mixdata to a7 (7*sizeof(uint32_t) = 28)
      "mov.n   a9,  a0                    \n"  // copy output destination address to a9
      "addi.n  a0,  a0,  4                \n"  // *a0 advance output destination address by 1

      "s32i.n  a10, a9,  0                \n"  // set last mixdata data to output destination

// Output starts here
// Combine RGB components with mixdata (Y coordinate info + OE signal) into 16-bit data, output 2 pixels as 32-bit, 8 times (transfer_period_count)

      "l32i.n   a8,  a15, 0               \n"  // a8  = mixdata[0]
      "add.n    a9,  a14, a9              \n"  // a9 output destination address += len32
      "extui    a7,  a3,  0,   6          \n"  // a7 = a3 odd1: get 6 bits
      "slli     a7,  a7,  16              \n"  // shift odd1 data left by 16 bits
      "add.n    a8,  a7,  a8              \n"  // a8 = odd1 + mixdata
      "extui    a7,  a4,  0,   6          \n"  // a7 = a4 odd2: get 6 bits
      "add.n    a7,  a7,  a8              \n"  // a7 = odd2 + mixdata
      "s32i.n   a7,  a9,  0               \n"  // set a7 value to output destination

      "l32i.n   a8,  a15, 4               \n"  // a8 = mixdata[1]
      "add.n    a9,  a14, a9              \n"  // output destination address += len32
      "extui    a7,  a5,  3,   6          \n"  // a7 = a5 even1: get 6 bits
      "slli     a7,  a7,  16              \n"  // shift even1 data left by 16 bits
      "add.n    a8,  a7,  a8              \n"  // a8 = even1 + mixdata
      "extui    a7,  a6,  3,   6          \n"  // a7 = a6 even2: get 6 bits
      "add.n    a7,  a7,  a8              \n"  // a7 = even2 + mixdata
      "s32i.n   a7,  a9,  0               \n"  // set a7 value to output destination


      "l32i.n   a8,  a15, 8               \n"  // a8  = mixdata[2]
      "add.n    a9,  a14, a9              \n"  // a9 output destination address += len32
      "extui    a7,  a3,  6,   6          \n"  // a7 = a3 odd1: get 6 bits
      "slli     a7,  a7,  16              \n"  // shift odd1 data left by 16 bits
      "add.n    a8,  a7,  a8              \n"  // a8 = odd1 + mixdata
      "extui    a7,  a4,  6,   6          \n"  // a7 = a4 odd2: get 6 bits
      "add.n    a7,  a7,  a8              \n"  // a7 = odd2 + mixdata
      "s32i.n   a7,  a9,  0               \n"  // set a7 value to output destination

      "l32i.n   a8,  a15, 12              \n"  // a8 = mixdata[3]
      "add.n    a9,  a14, a9              \n"  // output destination address += len32
      "extui    a7,  a5,  9,   6          \n"  // a7 = a5 even1: get 6 bits
      "slli     a7,  a7,  16              \n"  // shift even1 data left by 16 bits
      "add.n    a8,  a7,  a8              \n"  // a8 = even1 + mixdata
      "extui    a7,  a6,  9,   6          \n"  // a7 = a6 even2: get 6 bits
      "add.n    a7,  a7,  a8              \n"  // a7 = even2 + mixdata
      "s32i.n   a7,  a9,  0               \n"  // set a7 value to output destination


      "l32i.n   a8,  a15, 16              \n"  // a8  = mixdata[4]
      "add.n    a9,  a14, a9              \n"  // a9 output destination address += len32
      "extui    a7,  a3,  12,  6          \n"  // a7 = a3 odd1: get 6 bits
      "slli     a7,  a7,  16              \n"  // shift odd1 data left by 16 bits
      "add.n    a8,  a7,  a8              \n"  // a8 = odd1 + mixdata
      "extui    a7,  a4,  12,  6          \n"  // a7 = a4 odd2: get 6 bits
      "add.n    a7,  a7,  a8              \n"  // a7 = odd2 + mixdata
      "s32i.n   a7,  a9,  0               \n"  // set a7 value to output destination

      "l32i.n   a8,  a15, 20              \n"  // a8 = mixdata[5]
      "add.n    a9,  a14, a9              \n"  // output destination address += len32
      "extui    a7,  a5,  15,  6          \n"  // a7 = a5 even1: get 6 bits
      "slli     a7,  a7,  16              \n"  // shift even1 data left by 16 bits
      "add.n    a8,  a7,  a8              \n"  // a8 = even1 + mixdata
      "extui    a7,  a6,  15,  6          \n"  // a7 = a6 even2: get 6 bits
      "add.n    a7,  a7,  a8              \n"  // a7 = even2 + mixdata
      "s32i.n   a7,  a9,  0               \n"  // set a7 value to output destination

      // Skip mixdata fetch for the last 2 iterations (reuse value previously fetched into a10)
      "add.n    a9,  a14, a9              \n"  // a9 output destination address += len32
      "srli     a7,  a3,  18              \n"  // a7 = a3 odd1: get 6 bits
      "slli     a7,  a7,  16              \n"  // shift odd1 data left by 16 bits
      "add.n    a8,  a7,  a10             \n"  // a8 = odd1 + mixdata
      "srli     a7,  a4,  18              \n"  // a7 = a4 odd2: get 6 bits
      "add.n    a7,  a7,  a8              \n"  // a7 = odd2 + mixdata
      "s32i.n   a7,  a9,  0               \n"  // set a7 value to output destination

      "add.n    a9,  a14, a9              \n"  // output destination address += len32
      "srli     a7,  a5,  21              \n"  // a7 = a5 even1: get 6 bits
      "slli     a7,  a7,  16              \n"  // shift even1 data left by 16 bits
      "add.n    a8,  a7,  a10             \n"  // a8 = even1 + mixdata
      "srli     a7,  a6,  21              \n"  // a7 = a6 even2: get 6 bits
      "add.n    a7,  a7,  a8              \n"  // a7 = even2 + mixdata
      "s32i.n   a7,  a9,  0               \n"  // set a7 value to output destination

"HUB75_DRAW565_LOOP_END:                     \n"

      "l32i.n  a4,  a2,  28               \n" // a4 = assign xe_idx
      "l32i.n  a5,  a2,  20               \n" // a5 = assign xe_tbl
      "l32i.n  a9,  a2,  40               \n" // a9 = get mixdata table update value
      "beqi    a4,  8,   HUB75_EXIT565    \n" // if xe_idx has reached the end, exit processing
      "addx2   a6,  a4,  a5               \n" // a6 = address of current xe table index
      "l16ui   a3,  a6,  0                \n" // a3 = current xe value
      "srli    a3 , a3,  1                \n" // a3 >>= 1
// From here, update mixdata values and reset xe position
"HUB75_BR565LOOP_START:             \n"
        "mov     a5,  a3                    \n" // move previous xe value a3 to a5
        "l16ui   a3,  a6,  2                \n" // a3 = assign new xe value
        "addi.n  a6,  a6,  2                \n" // a6 advance xe table position by one
        "addi.n  a4,  a4,  1                \n" // a4 ++xe_idx
        "addx4   a8,  a4,  a15              \n" // a8 = set address of target mixdata to update
        "srli    a3,  a3,  1                \n" // a3 >>= 1
        "s32i.n  a9,  a8,  0                \n" // update mixdata
      "bge     a5,  a3,  HUB75_BR565LOOP_START     \n" // if xe values are equal, retry BR_LOOP
      "s32i.n  a4,  a2,  28               \n" // save xe_idx value
      "sub     a3,  a3,  a5               \n" // subtract previous xe value from new xe value to get the difference
      "j HUB75_DRAW565_LOOP_START         \n" // a3 (xe) has been updated, loop from the beginning again


"HUB75_EXIT565:                   \n"

      "l32i   a0,  a2,  44                \n"
      );
  }

/*
// C++ version draw332
  static void hub75Draw332_cpp(asm_work_t* work)
  {
    uint8_t* src8_h = (uint8_t*)(work->s32h);
    uint8_t* src8_l = (uint8_t*)(work->s32l);
    uint32_t* d32 = work->d32;
    uint32_t len32 = work->len32;
    uint32_t xe = work->xe;
    uint32_t xe_idx = work->xe_idx;
    auto mixdata = work->mixdata;
    auto xe_tbl = work->xe_tbl;
    for (;;)
    {
      do
      {
        // Apply gamma table based on the RGB332 value.
        // This table not only applies gamma correction, but also
        // converts to a format where bits are arranged one per BGR in sequence.
        uint32_t rgb_upper_1 = work->pixel_tbl[*src8_h++];
        uint32_t rgb_upper_2 = work->pixel_tbl[*src8_h++];
        uint32_t rgb_lower_1 = work->pixel_tbl[*src8_l++];
        uint32_t rgb_lower_2 = work->pixel_tbl[*src8_l++];

        // Combine so that 6-bit RGB components (upper and lower halves of the screen) sent in parallel are adjacent.
        uint32_t rgb_1 = rgb_upper_1 + (rgb_lower_1 << 3);
        uint32_t rgb_2 = rgb_upper_2 + (rgb_lower_2 << 3);

        d32[len32 * 0] = mixdata[5]; // [TRANSFER_PERIOD_COUNT_332];

        int32_t i = 0;
        uint32_t pixel_2 = rgb_2 & 0x3F;
        uint32_t pixel_1 = rgb_1 & 0x3F;
        pixel_2 += mixdata[i];
        pixel_1 <<= 16;
        d32[++i * len32] = pixel_1 + pixel_2;
        do
        {
          rgb_2 >>= 6;
          rgb_1 >>= 6;
          pixel_2 = rgb_2 & 0x3F;
          pixel_1 = rgb_1 & 0x3F;
          pixel_2 += mixdata[i];
          pixel_1 <<= 16;
          // Set two columns simultaneously to the buffer
          d32[++i * len32] = pixel_1 + pixel_2;
        } while (i < 5);  // TRANSFER_PERIOD_COUNT_332;

        ++d32;
      } while (--xe);
      if (xe_idx == 5) break; // TRANSFER_PERIOD_COUNT_332
      uint32_t new_xe = xe_tbl[xe_idx] >> 1;
      uint32_t old_xe;
      do {
        old_xe = new_xe;
        mixdata[++xe_idx] = work->mix_value;
        new_xe = xe_tbl[xe_idx] >> 1;
      } while (new_xe <= old_xe);
      xe = new_xe - old_xe;
    }
  }

 // C++ version draw565
  static void hub75Draw565_cpp(asm_work_t* work)
  {
    uint32_t* d32 = work->d32;
    uint32_t len32 = work->len32;
    uint32_t xe = work->xe;
    uint32_t xe_idx = work->xe_idx;
    auto pixel_tbl = work->pixel_tbl;
    auto mixdata = work->mixdata;
    auto xe_tbl = work->xe_tbl;

    for (;;)
    {
      do
      {
        // Load 2 pixels at once into a 32-bit variable in 16-bit RGB565 format. (for upper half of screen)
        uint32_t rgb565x2_upper = *work->s32h++;
        // Load 2 pixels at once similarly for the lower half of the screen
        uint32_t rgb565x2_lower = *work->s32l++;

        // Separate into R, G, B components. Process 2x2=4 pixels at once to reduce the number of operations
        uint32_t r_upper_1 = rgb565x2_upper >> 11;
        uint32_t r_lower_1 = rgb565x2_lower >> 11;
        uint32_t g_upper_1 = rgb565x2_upper >> 5;
        uint32_t g_lower_1 = rgb565x2_lower >> 5;
        uint32_t b_upper_2 = rgb565x2_upper & 0x1F001F;
        uint32_t b_lower_2 = rgb565x2_lower & 0x1F001F;
        uint32_t r_upper_2 = r_upper_1 & 0x1F001F;
        uint32_t r_lower_2 = r_lower_1 & 0x1F001F;
        uint32_t g_upper_2 = g_upper_1 & 0x3F003F;
        uint32_t g_lower_2 = g_lower_1 & 0x3F003F;

        r_upper_1 = r_upper_2 & 0x1F;
        r_lower_1 = r_lower_2 & 0x1F;
        r_upper_2 >>= 16;
        r_lower_2 >>= 16;
        g_upper_1 = g_upper_2 & 0x3F;
        g_lower_1 = g_lower_2 & 0x3F;
        g_upper_2 >>= 16;
        g_lower_2 >>= 16;
        uint32_t b_upper_1 = b_upper_2 & 0x1F;
        uint32_t b_lower_1 = b_lower_2 & 0x1F;
        b_upper_2 >>= 16;
        b_lower_2 >>= 16;

        // Apply gamma table based on each 64-level RGB value.
        // This table not only applies gamma correction, but also
        // converts each bit to 3-bit spacing.
        // Specifically: 0bABCDEFGH -> 0bA__B__C__D__E__F__G__H__
        r_upper_1 = pixel_tbl[r_upper_1 << 1];
        r_lower_1 = pixel_tbl[r_lower_1 << 1];
        r_upper_2 = pixel_tbl[r_upper_2 << 1];
        r_lower_2 = pixel_tbl[r_lower_2 << 1];
        g_upper_1 = pixel_tbl[g_upper_1];
        g_lower_1 = pixel_tbl[g_lower_1];
        g_upper_2 = pixel_tbl[g_upper_2];
        g_lower_2 = pixel_tbl[g_lower_2];
        b_upper_1 = pixel_tbl[b_upper_1 << 1];
        b_lower_1 = pixel_tbl[b_lower_1 << 1];
        b_upper_2 = pixel_tbl[b_upper_2 << 1];
        b_lower_2 = pixel_tbl[b_lower_2 << 1];

        // Since the values loaded from the table are at 3-bit intervals,
        // R, G, B can be combined by bit-shifting each to avoid overlapping.
        g_upper_1 += b_upper_1 << 1;
        g_lower_1 += b_lower_1 << 1;
        g_upper_2 += b_upper_2 << 1;
        g_lower_2 += b_lower_2 << 1;
        uint32_t rgb_upper_1 = r_upper_1 + (g_upper_1 << 1);
        uint32_t rgb_lower_1 = r_lower_1 + (g_lower_1 << 1);
        uint32_t rgb_upper_2 = r_upper_2 + (g_upper_2 << 1);
        uint32_t rgb_lower_2 = r_lower_2 + (g_lower_2 << 1);

        // The bits in the above variables are arranged in BGRBGRBGRBGR... order.
        // Separate these into even bit (0,2,4,6) and odd bit (1,3,5,7) components of each color.
        uint32_t rgb_upper_1_even = rgb_upper_1 & 0b00111000111000111000111000111000;
        uint32_t rgb_upper_1_odd  = rgb_upper_1 & 0b11000111000111000111000111000111;
        uint32_t rgb_lower_1_even = rgb_lower_1 & 0b00111000111000111000111000111000;
        uint32_t rgb_lower_1_odd  = rgb_lower_1 & 0b11000111000111000111000111000111;
        uint32_t rgb_upper_2_even = rgb_upper_2 & 0b00111000111000111000111000111000;
        uint32_t rgb_upper_2_odd  = rgb_upper_2 & 0b11000111000111000111000111000111;
        uint32_t rgb_lower_2_even = rgb_lower_2 & 0b00111000111000111000111000111000;
        uint32_t rgb_lower_2_odd  = rgb_lower_2 & 0b11000111000111000111000111000111;

        // Combine so that 6-bit RGB components (upper and lower halves of the screen) sent in parallel are adjacent.
        uint32_t rgb_even_1 = (rgb_lower_1_even    ) + (rgb_upper_1_even >> 3);
        uint32_t rgb_odd_1  = (rgb_lower_1_odd << 3) + (rgb_upper_1_odd      );
        uint32_t rgb_even_2 = (rgb_lower_2_even    ) + (rgb_upper_2_even >> 3);
        uint32_t rgb_odd_2  = (rgb_lower_2_odd << 3) + (rgb_upper_2_odd      );

        d32[0] = mixdata[8]; // TRANSFER_PERIOD_COUNT565

        uint32_t i = 0;
        do
        {
          uint32_t odd_2 = rgb_odd_2 & 0x3F;
          uint32_t odd_1 = rgb_odd_1 & 0x3F;
          uint32_t even_2 = rgb_even_2 & 0x3F;
          uint32_t even_1 = rgb_even_1 & 0x3F;
          rgb_odd_2 >>= 6;
          rgb_odd_1 >>= 6;
          odd_2 += mixdata[i] + (odd_1 << 16);
          // Set odd bit components for two columns simultaneously to the buffer
          d32[++i * len32] = odd_2;
          even_2 += mixdata[i] + (even_1 << 16);
          rgb_even_2 >>= 6;
          rgb_even_1 >>= 6;
          // Set even bit components for two columns simultaneously to the buffer
          d32[++i * len32] = even_2;
        } while (i < 8); // TRANSFER_PERIOD_COUNT565
        ++d32;
      } while (--xe);
      if (xe_idx == 8) break; // TRANSFER_PERIOD_COUNT_565
      uint32_t new_xe = xe_tbl[xe_idx] >> 1;
      uint32_t old_xe;
      do {
        old_xe = new_xe;
        mixdata[++xe_idx] = work->mix_value;
        new_xe = xe_tbl[xe_idx] >> 1;
      } while (new_xe <= old_xe);
      xe = new_xe - old_xe;
    }
  }
//*/

  void Bus_HUB75::dmaTask(void *arg)
  {
    auto me = (Bus_HUB75*)arg;

    auto dev = getDev(me->_cfg.i2s_port);
    dev->conf.val = _conf_reg_start;
    dev->int_clr.val = ~0u;

    int intr_source = ETS_I2S0_INTR_SOURCE;
#if SOC_I2C_NUM > 1
    if (me->_cfg.i2s_port != I2S_NUM_0)
    {
      intr_source = ETS_I2S1_INTR_SOURCE;
    }
#endif

    intr_handle_t isr_handle = nullptr;

    if (esp_intr_alloc(intr_source, ESP_INTR_FLAG_LEVEL1 | ESP_INTR_FLAG_IRAM,
        i2s_intr_handler_hub75, me, &(isr_handle)) != ESP_OK) {
      ESP_EARLY_LOGE("Bus_HUB75","esp_intr_alloc failure ");
      return;
    }

    ESP_EARLY_LOGV("Bus_HUB75","esp_intr_alloc success ");

    // Task notification may have arrived late, so wait here and discard it
    ulTaskNotifyTake( pdTRUE, 1);

    me->dmaTask_inner();

    esp_intr_free(isr_handle);

    vTaskDelete( nullptr );
  }


  void Bus_HUB75::dmaTask_inner(void)
  {
    const auto panel_width = _panel_width;
    const auto panel_height = _panel_height;
    const uint32_t len32 = panel_width >> 1;
    uint_fast8_t y = 0;

    const uint16_t* xe_tbl = _brightness_period;

    asm_work_t work;

    uint32_t mixdata[TRANSFER_PERIOD_COUNT_565 + 1];
    work.xe_tbl = _brightness_period;
    work.len32 = panel_width >> 1;
    work.pixel_tbl = _pixel_tbl;
    work.mixdata = mixdata;

    auto fp_draw = hub75Draw332_asm;
    // auto fp_draw = hub75Draw332_cpp;
    auto transfer_period_count = TRANSFER_PERIOD_COUNT_332;

    if (_depth == color_depth_t::rgb565_2Byte)
    {
      fp_draw = hub75Draw565_asm;
      // fp_draw = hub75Draw565_cpp;
      transfer_period_count = TRANSFER_PERIOD_COUNT_565;
    }

    while (_dmatask_handle)
    {
// DEBUG
// lgfx::gpio_lo(15);
      auto dst = (uint32_t*)ulTaskNotifyTake( pdTRUE, portMAX_DELAY);
// DEBUG
// lgfx::gpio_hi(15);
      if (dst == nullptr) { break; }
      auto d32 = &dst[len32 * (transfer_period_count + 1)];

      y = (y + 1) & ((panel_height>>1) - 1);

      {
      // Set SHIFTREG_ABC Y coordinate information;
        uint32_t poi = (~y) & ((panel_height >> 1) - 1);
        d32[poi                      ] = _mask_pin_a_clk | _mask_oe | _mask_pin_c_dat;
        d32[poi + (panel_height >> 1)] = _mask_pin_a_clk | _mask_oe | _mask_pin_c_dat;
        for (int i = 0; i < _dma_desc_set; ++i)
        { // Clear previous Y coordinate bits;
          poi = (poi + 1) & ((panel_height >> 1) - 1);
          d32[poi                      ] = _mask_pin_a_clk | _mask_oe;
          d32[poi + (panel_height >> 1)] = _mask_pin_a_clk | _mask_oe;
        }
        // Add latch at the end;
        // Since it is unknown whether the LAT pin or B pin is used for latching due to panel specification differences, both B and LAT are asserted;
        d32[panel_height - 1] |= _mask_pin_b_lat | _mask_lat | _mask_lat << 16;
      }

      uint32_t yy = 0;
      uint32_t yy_oe = _mask_oe | _mask_pin_b_lat; // PIN B Discharge
      if (_cfg.address_mode == config_t::address_mode_t::address_binary)
      {
        yy = y << 8 | y << 24;
        yy_oe = yy | _mask_oe;
      }
      mixdata[0] = yy_oe;
      for (size_t i = 1; i <= transfer_period_count; ++i)
      {
        mixdata[i] = yy;
      }

      work.d32 = dst;
      work.s32h = (uint32_t*)(_frame_buffer->getLineBuffer(y));
      work.s32l = (uint32_t*)(_frame_buffer->getLineBuffer(y + (panel_height>>1)));
      work.xe = xe_tbl[0] >> 1;
      work.xe_idx = 0;
      work.mix_value = yy_oe;

      // Call the data generation function
      fp_draw(&work);

      d32 = &dst[len32];

      // Prevent lighting at the start of the no-data, lighting-only period
      d32[0 - len32] |= _mask_oe;
      d32[1 - len32] |= (xe_tbl[transfer_period_count - 1] & 1) ? (_mask_oe & ~0xFFFF) : _mask_oe;

      d32 += len32;
      // Data latch and lighting prevention immediately after latch
      for (int i = 0; i < transfer_period_count; ++i)
      {
        d32[len32 * i - 1] |= _mask_lat;
        d32[len32 * i + 0] |= _mask_oe;
        d32[len32 * i + 1] |= (xe_tbl[i] & 1) ? (_mask_oe & ~0xFFFF) :  _mask_oe;
      }

      // If the next interrupt occurs during drawing, skip processing to avoid sustained busy state
      ulTaskNotifyTake( pdTRUE, 0); // discard notification if one has arrived
    }
  }

//----------------------------------------------------------------------------
 }
}

#endif
#endif
