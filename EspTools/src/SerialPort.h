#pragma once
#include "esp_loader_io.h"

struct FlashPort;
class sp_port;

FlashPort* createSerialPort(sp_port* serialPort);
esp_loader_port_t* getBaseSerialPort(FlashPort* flashPort);