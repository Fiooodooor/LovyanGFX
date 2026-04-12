#ifdef BUILD_CVBS_EXAMPLES

#define LGFX_USE_V1
#define WDT_TIMEOUT_SEC 15
#include <Arduino.h>
#include <esp_task_wdt.h>
#include <esp_heap_caps.h>
#include <WiFi.h>
#include <JPEGDEC.h>
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32/Panel_CVBS.hpp>
#include "viewer_cvbs_jpeg_data.h"

class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_CVBS _panel_instance;
public:
    LGFX() {
        {
            auto cfg = _panel_instance.config();
            cfg.memory_width  = 480;
            cfg.memory_height = 320;
            cfg.panel_width   = 480;
            cfg.panel_height  = 320;
            cfg.offset_x      = 0;
            cfg.offset_y      = 0;
            cfg.offset_rotation = 0;
            _panel_instance.config(cfg);
        }
        {
            auto cfg = _panel_instance.config_detail();
            cfg.signal_type  = cfg.signal_type_t::NTSC_J;
            cfg.pin_dac      = 26;
            cfg.use_psram    = heap_caps_get_free_size(MALLOC_CAP_SPIRAM) ? 1 : 0;
            cfg.output_level = 128;
            cfg.chroma_level = 128;
            _panel_instance.config_detail(cfg);
        }
        setPanel(&_panel_instance);
    }
};

LGFX gfx;
JPEGDEC jpeg;
static constexpr const char *TAG = "viewer_cvbs";

static int onJpegDraw(JPEGDRAW *pDraw)
{
    if (pDraw == nullptr || pDraw->pPixels == nullptr) { return 0; }
    gfx.setAddrWindow(pDraw->x, pDraw->y, pDraw->x + pDraw->iWidthUsed - 1, pDraw->y + pDraw->iHeight - 1);
    gfx.startWrite();
    gfx.writePixelsDMA((lgfx::swap565_t*)pDraw->pPixels, pDraw->iWidthUsed * pDraw->iHeight, false);
    gfx.endWrite();
    return 1;
}

static int onJpegDraw2(JPEGDRAW* d) {
    gfx.pushImageDMA(d->x, d->y, d->iWidth, d->iHeight, (lgfx::swap565_t*)d->pPixels);
    return 1;
}

void setup()
{
    Serial.begin(115200);
    while (!Serial) { delay(1); }
    Serial.println("CVBS JPEG viewer starting...");
    Serial.printf("free heap 8bit=%u, dma=%u, internal=%u\n",
        heap_caps_get_free_size(MALLOC_CAP_8BIT),
        heap_caps_get_free_size(MALLOC_CAP_DMA),
        heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    Serial.printf("PSRAM free=%u\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    gfx.setColorDepth(lgfx::color_depth_t::rgb565_2Byte);
    if (!gfx.init())
    {
        Serial.println("gfx.init() failed");
        Serial.printf("post-init free heap 8bit=%u, dma=%u, internal=%u\n",
            heap_caps_get_free_size(MALLOC_CAP_8BIT),
            heap_caps_get_free_size(MALLOC_CAP_DMA),
            heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
        while (true) { delay(1000); }
    }

    gfx.fillScreen(TFT_BLACK);

    constexpr int W = 480;
    constexpr int H = 320;

    if (jpeg.openRAM(const_cast<uint8_t*>(jpeg_data), jpeg_data_len, onJpegDraw2))
    {
        jpeg.setPixelType(RGB565_BIG_ENDIAN);
        int opts = 0;
        ESP_LOGE(TAG, "JPEG width:%d height: %d", jpeg.getWidth(), jpeg.getHeight());
        if (jpeg.getWidth() >= W * 2 && jpeg.getHeight() >= H * 2)
        {
            opts = JPEG_SCALE_HALF;
            ESP_LOGE(TAG, "JPEG scaled to half size");
        }
        jpeg.decode(0, 0, opts);
        jpeg.close();
        Serial.println("JPEG rendered to CVBS framebuffer.");
    }
    else
    {
        Serial.println("JPEG open failed.");
        gfx.fillRect(0, 0, W, H, gfx.color888(255, 0, 0));
        gfx.setTextSize(2);
        gfx.setTextColor(TFT_WHITE);
        gfx.setCursor(12, 150);
        gfx.print("JPEG decode failed");
    }
}

void loop()
{
    delay(10000);
}
#endif