#pragma once
#include "esp_loader_io.h"

class sp_port;

namespace ZanaBlocks::EspTools {
struct FlashPort;

FlashPort* createSerialPort(sp_port* serialPort);
esp_loader_port_t* getBaseSerialPort(FlashPort* flashPort);

}  // namespace ZanaBlocks::EspTools