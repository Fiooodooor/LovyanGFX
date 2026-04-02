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

#include <stdint.h>

#include "misc/enum.hpp"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  struct pixelcopy_t;

  enum bus_type_t
  {
    bus_unknown,
    bus_spi,
    bus_i2c,
    bus_parallel8,
    bus_parallel16,
    bus_stream,
    bus_image_push,
    bus_dsi,
  };

  struct IBus
  {
    virtual ~IBus(void) = default;

    virtual bus_type_t busType(void) const = 0;

    /// Prepare the peripheral.;
    virtual bool init(void) = 0;

    /// Release the peripheral.;
    virtual void release(void) = 0;

    /// Get the communication speed.;
    virtual uint32_t getClock(void) const { return 0; }

    /// Get the communication speed for receiving.;
    virtual uint32_t getReadClock(void) const { return 0; }

    /// Set the communication speed.;
    virtual void setClock(uint32_t) {};

    /// Set the communication speed for receiving.;
    virtual void setReadClock(uint32_t) {};

    /// Begin a communication transaction. (Acquire exclusive access to the peripheral);
    virtual void beginTransaction(void) = 0;

    /// End the communication transaction. (Release exclusive access to the peripheral);
    virtual void endTransaction(void) = 0;

    /// Wait for communication to complete;
    virtual void wait(void) = 0;

    /// Returns whether communication is currently in progress. true: busy;
    virtual bool busy(void) const = 0;

    /// Prepare the peripheral for DMA transfer.;
    virtual void initDMA(void) = 0;

    /// Add a DMA transfer queue entry.;
    virtual void addDMAQueue(const uint8_t* data, uint32_t length) = 0; // { writeBytes(data, length, true); }

    /// Execute transmission of the accumulated DMA transfer queue.;
    virtual void execDMAQueue(void) = 0;

    /// Get a buffer for DMA. Depending on the bus implementation, two buffers may be used alternately internally.;
    /// Note that when called repeatedly, the returned pointer differs from the previous call but may be the same as two calls ago.;
    virtual uint8_t* getDMABuffer(uint32_t length) = 0;

    /// Start sending any unsent data.;
    virtual void flush(void) = 0;

    /// Set the D/C pin low and transmit data.;
    virtual bool writeCommand(uint32_t data, uint_fast8_t bit_length) = 0;

    /// Set the D/C pin high and transmit data.;
    virtual void writeData(uint32_t data, uint_fast8_t bit_length) = 0;

    /// Set the D/C pin high and repeatedly transmit data the specified number of times.;
    virtual void writeDataRepeat(uint32_t data, uint_fast8_t bit_length, uint32_t count) = 0;

    /// Transmit pixel data using the pixelcopy struct.;
    virtual void writePixels(pixelcopy_t* pc, uint32_t length) = 0;

    /// Transmit the byte array passed as an argument.;
    virtual void writeBytes(const uint8_t* data, uint32_t length, bool dc, bool use_dma) = 0;

    virtual void beginRead(uint_fast8_t dummy_bits) { beginRead(); if (dummy_bits) { readData(dummy_bits); } }
    virtual void beginRead(void) = 0;
    virtual void endRead(void) = 0;
    virtual uint32_t readData(uint_fast8_t bit_length) = 0;
    virtual bool readBytes(uint8_t* dst, uint32_t length, bool use_dma = false) = 0;
    virtual bool readBytes(uint8_t* dst, uint32_t length, bool use_dma, bool last_nack) { (void)last_nack; return readBytes(dst, length, use_dma); }
    virtual void readPixels(void* dst, pixelcopy_t* pc, uint32_t length) = 0;
  };

  struct Bus_NULL : public IBus
  {
    bus_type_t busType(void) const override { return bus_type_t::bus_unknown; }
    bool init(void) override { return false; }
    void release(void) override {}
    void beginTransaction(void) override {}
    void endTransaction(void) override {}
    void wait(void) override {}
    bool busy(void) const override { return false; }

    void initDMA(void) override {}
    void addDMAQueue(const uint8_t*, uint32_t) override {}
    void execDMAQueue(void) override {}
    uint8_t* getDMABuffer(uint32_t) override { return nullptr; }

    void flush(void) override {}
    bool writeCommand(uint32_t, uint_fast8_t) override { return false; }
    void writeData(uint32_t, uint_fast8_t) override {}
    void writeDataRepeat(uint32_t, uint_fast8_t, uint32_t) override {}
    void writePixels(pixelcopy_t*, uint32_t) override {}
    void writeBytes(const uint8_t*, uint32_t, bool, bool) override {}

    void beginRead(void) override {}
    void endRead(void) override {}
    uint32_t readData(uint_fast8_t) override { return 0; }
    bool readBytes(uint8_t*, uint32_t, bool) override { return false; }
    void readPixels(void*, pixelcopy_t*, uint32_t) override {}
  };

  /// @brief Used as the base for bus types that have no command system and transmit the entire screen
  struct Bus_ImagePush : public Bus_NULL
  {
    bus_type_t busType(void) const override { return bus_type_t::bus_image_push; }
    virtual void setImageBuffer(void* buffer, color_depth_t depth) { (void)buffer; (void)depth; }
    virtual void setBrightness(uint8_t brightness) { (void)brightness; }
    virtual void setInvert(uint8_t invert) { (void)invert; }
  };

//----------------------------------------------------------------------------
 }
}
