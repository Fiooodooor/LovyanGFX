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
#include <stddef.h>

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

    /// @brief Frame buffer with memory region divided into multiple blocks
  class DividedFrameBuffer
  {
  public:
    DividedFrameBuffer(void) : _block_array(nullptr), _line_size(0), _total_lines(0), _block_lines(0), _block_count(0) {}

    enum psram_setting_t
    {
      no_psram,
      half_psram,
      full_psram
    };

    /// @brief Perform initialization and allocate memory
    /// @param line_size Number of bytes in the width direction
    /// @param total_lines Number of lines in the height direction
    /// @param block_lines Number of lines per memory block
    /// @param use_psram Specify whether to use PSRAM on ESP32
    uint8_t** create(size_t line_size, size_t total_lines, size_t block_lines, psram_setting_t use_psram = no_psram);

    /// @brief Release allocated memory
    void release(void);

    inline size_t getLineSize(void) const { return _line_size; }
    inline size_t getTotalLines(void) const { return _total_lines; }
    inline size_t getBlockCount(void) const { return _block_count; }

    /// @brief Get the buffer pointer by specifying the block number
    /// @param index Block number
    /// @return Pointer to the beginning of the buffer for the specified block
    inline uint8_t* getBlockBuffer(size_t index) const { return index < _block_count ? _block_array[index] : nullptr; }

    /// @brief Get the buffer pointer by specifying the Y coordinate
    /// @param y Line number
    /// @return Pointer to the beginning of the specified line (not necessarily the beginning of a block)
    inline uint8_t* getLineBuffer(size_t y) const { return &_block_array[y / _block_lines][_line_size * (y % _block_lines)]; }

    inline bool isInitialized(void) const { return _block_array != nullptr; }

  private:
    uint8_t** _block_array;
    uint16_t _line_size;  // Number of bytes per line
    uint16_t _total_lines; // Total number of lines
    uint16_t _block_lines; // Number of lines contained in one memory block
    uint16_t _block_count; // Number of memory blocks
  };

//----------------------------------------------------------------------------
 }
}
