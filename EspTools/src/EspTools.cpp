#include "EspTools.h"

#include "Logging.h"

using namespace ZanaBlocks::Utilities;

namespace ZanaBlocks::EspTools {

std::shared_ptr<sp_port> connect(const std::string& port, const int baudRate) {
  sp_port* serialPort = nullptr;
  if (sp_get_port_by_name(port.c_str(), &serialPort) != SP_OK) {
    ERROR("Port not found: " + port);
    return nullptr;
  }

  if (sp_open(serialPort, SP_MODE_READ_WRITE) != SP_OK) {
    ERROR("Failed to open: " + port);
    sp_free_port(serialPort);
    return nullptr;
  }

  sp_set_baudrate(serialPort, baudRate);
  sp_set_bits(serialPort, 8);
  sp_set_parity(serialPort, SP_PARITY_NONE);
  sp_set_stopbits(serialPort, 1);
  sp_set_flowcontrol(serialPort, SP_FLOWCONTROL_NONE);

  return std::shared_ptr<sp_port>(serialPort, [](sp_port* serialPort) {
    if (serialPort != nullptr) {
      sp_close(serialPort);
      sp_free_port(serialPort);
    }
  });
}

std::vector<std::string> getUsbSerialPorts() {
  std::vector<std::string> usbPorts;

  struct sp_port** ports = nullptr;

  const enum sp_return result = sp_list_ports(&ports);
  if (result != SP_OK) {
    ERROR("sp_list_ports failed:" << result);
    return usbPorts;
  }

  for (int i = 0; ports[i] != nullptr; i++) {
    const struct sp_port* port = ports[i];

    // Only care about USB transport
    if (sp_get_port_transport(port) == SP_TRANSPORT_USB) {
      usbPorts.emplace_back(sp_get_port_name(port));
    }
  }

  sp_free_port_list(ports);
  return usbPorts;
}
}  // namespace ZanaBlocks::EspTools