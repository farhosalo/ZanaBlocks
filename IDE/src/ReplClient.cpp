#include "ReplClient.h"

#include <unistd.h>

#include <fstream>
#include <stdexcept>
#include <vector>

#include "Base64.h"
#include "Logging.h"

namespace KidTech::ReplClient {

using namespace Utilities;

bool ReplClient::putFile(const std::string& localPath,
                         const std::string& remotePath,
                         ProgressCallback callback) {
  // Helper to invoke the progress callback if it's set
  auto invokeCallback = [&](std::string message) {
    if (callback) callback(std::move(message));
  };

  std::pair<RUN_STATE, std::string> copyState;

  // Read local file
  std::ifstream file(localPath, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    invokeCallback("Cannot open local file: " + localPath);
    return false;
  }

  auto fileSize = static_cast<std::size_t>(file.tellg());
  file.seekg(0);

  std::vector<uint8_t> contents(fileSize);
  file.read(reinterpret_cast<char*>(contents.data()),
            static_cast<std::streamsize>(fileSize));

  // Open remote file for writing (binary)
  copyState = run("import ubinascii; _f = open('" + remotePath + "', 'wb')");
  INFO(copyState.second);

  if (copyState.first != RUN_STATE::SUCCESS) {
    invokeCallback("Failed to open remote file for writing!");
    return false;
  }

  // Send in chunks, base64-encoded to keep serial clean
  std::size_t sent = 0;
  while (sent < fileSize) {
    std::size_t chunkLen = std::min(CHUNK_SIZE, fileSize - sent);

    std::string b64 = base64Encode(contents.data() + sent, chunkLen);

    // Decode on-device and write
    std::string cmd = "_f.write(ubinascii.a2b_base64(b'" + b64 + "'))";

    copyState = run(cmd);

    INFO(copyState.second);

    if (copyState.first != RUN_STATE::SUCCESS) {
      invokeCallback("Failed to write remote file!");
      run("_f.close()");
      return false;
    }

    sent += chunkLen;
    invokeCallback("Sent " + std::to_string(sent) + "/" +
                   std::to_string(fileSize) + " bytes" + "(" +
                   std::to_string((sent * 100) / fileSize) + "%)");
  }

  // Close remote file
  run("_f.close(); del _f");
  return true;
}

void ReplClient::reset() {
  try {
    writeAll("import machine; machine.reset()\x04");
  } catch (...) {
  }
  // Give the device time to reboot before the caller tries to reconnect
  ::usleep(1'500'000);
}

bool ReplClient::connect(const std::string& port, const int baudRate) {
  if (sp_get_port_by_name(port.c_str(), &mSerialPort) != SP_OK) {
    ERROR("Port not found: " + port);
    return false;
  }

  if (sp_open(mSerialPort, SP_MODE_READ_WRITE) != SP_OK) {
    ERROR("Failed to open: " + port);
    return false;
  }

  sp_set_baudrate(mSerialPort, baudRate);
  sp_set_bits(mSerialPort, 8);
  sp_set_parity(mSerialPort, SP_PARITY_NONE);
  sp_set_stopbits(mSerialPort, 1);
  sp_set_flowcontrol(mSerialPort, SP_FLOWCONTROL_NONE);

  enterRawRepl();
  return true;
}

void ReplClient::writeAll(const std::string& data) {
  sp_blocking_write(mSerialPort, data.data(), data.size(), 1000);
}

std::pair<RUN_STATE, std::string> ReplClient::readUntil(
    const std::string& marker, unsigned timeoutMs) {
  std::string buffer;

  // Read until we see the marker or hit a timeout. We read in chunks and append
  // to the buffer until the marker is found. If an error occurs or we hit a
  // timeout, we return an appropriate message and state. If we successfully
  // read the output, we check for error keywords to determine if the command
  // execution resulted in an error or success. Note: the marker is expected to
  // be at the end of the output, so we check for it in the buffer to know when
  // to stop reading.
  char tmp[64];
  while (buffer.find(marker) == std::string::npos) {
    // Read a chunk of data from the serial port with a timeout. We use
    // sp_blocking_read which will wait until data is available or the timeout
    // is reached. The data is read into a temporary buffer and then appended to
    // the main buffer.
    int n = sp_blocking_read(mSerialPort, tmp, sizeof(tmp), timeoutMs);
    if (n < 0) {  // Error occurred during read
      buffer = "Failed to read from REPL";
      ERROR(buffer);
      return {RUN_STATE::ERROR, buffer};
    }
    if (n == 0) {  // Timeout occurred
      buffer = "REPL read timeout";
      ERROR(buffer);
      return {RUN_STATE::TIME_OUT, buffer};
    }
    buffer.append(tmp, static_cast<std::size_t>(n));
  }

  if (buffer.find("Error") != std::string::npos ||
      buffer.find("SyntaxError") != std::string::npos ||
      buffer.find("Exception") != std::string::npos ||
      buffer.find("IndentationError") != std::string::npos) {
    return {RUN_STATE::ERROR, buffer};
  }
  return {RUN_STATE::SUCCESS, buffer};
}

void ReplClient::enterRawRepl() {
  writeAll("\x03");
  sp_blocking_read(mSerialPort, nullptr, 0, 100);  // 100ms pause

  // Clear any existing input/output buffers.
  writeAll("\x01");
  sp_flush(mSerialPort, SP_BUF_INPUT);
}

ReplClient::~ReplClient() {
  if (mSerialPort) {
    exitRawRepl();
    sp_close(mSerialPort);
    sp_free_port(mSerialPort);
  }
}
std::pair<RUN_STATE, std::string> ReplClient::run(const std::string& code) {
  // Send the code followed by the raw REPL end marker.
  writeAll(code + "\x04");
  return readUntil("\x04>", 3000);
}
}  // namespace KidTech::ReplClient