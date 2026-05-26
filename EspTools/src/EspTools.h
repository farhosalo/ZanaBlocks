#pragma once

#include <libserialport.h>

#include <memory>
#include <string>
#include <vector>

namespace ZanaBlocks::EspTools {
static constexpr int DEFAULT_BAUD_RATE = 115200;

/**
 * * @brief Connects to the device via the specified serial port.
 * @param port The name of the serial port (e.g., "/dev/ttyUSB0" or "COM3").
 * @param baudRate The baud rate for the connection (defaults to 115200).
 * @return A shared pointer to the opened serial port, or nullptr if the
 * connection failed.
 */
std::shared_ptr<sp_port> connect(const std::string& port,
                                 int baudRate = DEFAULT_BAUD_RATE);

/**
 * @brief Lists all available USB serial ports on the system.
 * @return A vector of strings containing the names of the detected USB serial
 * ports.
 */
std::vector<std::string> getUsbSerialPorts();

}  // namespace ZanaBlocks::EspTools