#pragma once
#include <chrono>

#include "esp_loader_io.h"

struct sp_port;

namespace ZanaBlocks::EspTools {

struct FlashPort {
  esp_loader_port_t mBaseFlashPort{};  // must be first
  sp_port* mSerialPort = nullptr;
  uint32_t mTimeoutMs = 0;
  std::chrono::steady_clock::time_point mTimerStart;
};

std::unique_ptr<FlashPort> createSerialPort(sp_port* serialPort);
esp_loader_port_t* getBaseSerialPort(FlashPort* flashPort);

}  // namespace ZanaBlocks::EspTools