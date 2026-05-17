#pragma once
#include <atomic>
#include <cstdint>
#include <functional>
#include <string>

#include "esp_loader.h"

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
  std::function<void(int)> onProgress;
  std::function<void(const std::string&)> onStatus;

  /**
   * @brief Flash firmware to an ESP32 device over a serial port.
   *
   * @param device The serial port path (e.g., "/dev/ttyUSB0" or "COM3").
   * @param firmwarePath Path to the binary firmware file.
   * @return true if flashing succeeded, false otherwise.
   */
  bool flash(const std::string& device, const std::string& firmwarePath);

 private:
  Bytes loadFirmware(const std::string& path);
  void flashFirmware(esp_loader_t* loader, const uint32_t addr,
                     const Bytes& fw);
  void status(const std::string& msg) const;
  void progress(const int progress) const;
};