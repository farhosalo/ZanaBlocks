#pragma once
#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "esp_loader.h"

namespace ZanaBlocks::EspTools {
using Bytes = std::vector<uint8_t>;

/**
 * * @brief High-level interface for flashing ESP32 family chips using the ESP
 * serial flasher protocol.
 *
 * This class provides a simplified wrapper around the esp_serial_flasher
 * library, handling firmware loading, chip detection, baud rate negotiation,
 * and the flashing process.
 */
class EspFlasher {
 public:
  void setOnProgress(std::function<void(int)> callback) {
    mOnProgress = std::move(callback);
  }
  void setOnStatus(std::function<void(const std::string&)> callback) {
    mOnStatus = std::move(callback);
  }
  void setOnEnterBootloader(std::function<void()> callback) {
    mOnEnterBootloader = std::move(callback);
  }

  /**
   * @brief Flash firmware to an ESP32 device over a serial port.
   *
   * @param device The serial port path (e.g., "/dev/ttyUSB0" or "COM3").
   * @param firmwarePath Path to the binary firmware file.
   * @return true if flashing succeeded, false otherwise.
   */
  bool flash(const std::string& device, const std::string& firmwarePath);
  void setEnterBootloader() { mEnteredBootloaderMode.store(true); }

 private:
  Bytes loadFirmware(const std::string& path);
  void flashFirmware(esp_loader_t* loader, uint32_t addr,
                     const Bytes& firmware);
  void status(const std::string& msg) const;
  void progress(int progress) const;
  std::atomic<bool> mEnteredBootloaderMode{false};

  std::function<void(int32_t)> mOnProgress;
  std::function<void(const std::string&)> mOnStatus;
  std::function<void()> mOnEnterBootloader;
};
}  // namespace ZanaBlocks::EspTools