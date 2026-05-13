#include "ReplClient.h"

#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "Base64.h"
#include "Logging.h"

using namespace ZanaBlocks::Utilities;

namespace ZanaBlocks::EspTools {

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
  copyState =
      runPythonCmd("import ubinascii; _f = open('" + remotePath + "', 'wb')");
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

    copyState = runPythonCmd(cmd);

    INFO(copyState.second);

    if (copyState.first != RUN_STATE::SUCCESS) {
      invokeCallback("Failed to write remote file!");
      runPythonCmd("_f.close()");
      return false;
    }

    sent += chunkLen;
    invokeCallback("Sent " + std::to_string(sent) + "/" +
                   std::to_string(fileSize) + " bytes" + "(" +
                   std::to_string((sent * 100) / fileSize) + "%)");
  }

  // Close remote file
  runPythonCmd("_f.close(); del _f");
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

void ReplClient::writeAll(const std::string& data) {
  sp_blocking_write(mSerialConnection.get(), data.data(), data.size(), 1000);
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
    int n =
        sp_blocking_read(mSerialConnection.get(), tmp, sizeof(tmp), timeoutMs);
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
  sp_blocking_read(mSerialConnection.get(), nullptr, 0, 100);  // 100ms pause

  // Clear any existing input/output buffers.
  writeAll("\x01");
  sp_flush(mSerialConnection.get(), SP_BUF_INPUT);
}

ReplClient::ReplClient(const std::shared_ptr<sp_port>& connection)
    : mSerialConnection(connection) {
  enterRawRepl();
};

ProbeResult ReplClient::probe() {
  ProbeResult result;

  // Pipe-delimited: <impl_name>|<firmware_version>|<hardware_model>
  // os.uname().release = firmware version string, e.g. "1.23.0"
  // os.uname().machine = hardware model string, e.g. "ESP32 module with ESP32"
  auto [state, output] = runPythonCmd(
      "import sys, os; u = os.uname(); "
      "print(sys.implementation.name + '|' + u.release + '|' + u.machine)");

  if (state != RUN_STATE::SUCCESS) {
    INFO("Probe failed: " + output);
    return result;
  }

  // Raw REPL wraps stdout as: OK<stdout>\x04<stderr>\x04>
  // Grab only the stdout portion
  auto endPos = output.find('\x04');
  std::string out =
      (endPos != std::string::npos) ? output.substr(0, endPos) : output;

  // Strip the leading "OK" the raw REPL prepends
  if (out.rfind("OK", 0) == 0) out = out.substr(2);

  // Trim surrounding whitespace / CR LF
  auto trimmed = [](std::string s) {
    s.erase(0, s.find_first_not_of(" \t\r\n"));
    s.erase(s.find_last_not_of(" \t\r\n") + 1);
    return s;
  };
  out = trimmed(out);

  // Split on '|'
  auto splitPipe = [](const std::string& s) -> std::vector<std::string> {
    std::vector<std::string> parts;
    std::size_t start = 0;
    std::size_t pos;
    while ((pos = s.find('|', start)) != std::string::npos) {
      parts.push_back(s.substr(start, pos - start));
      start = pos + 1;
    }
    parts.push_back(s.substr(start));
    return parts;
  };

  auto parts = splitPipe(out);
  if (parts.size() < 3) {
    INFO("Probe: unexpected response format: " + out);
    return result;
  }

  std::string implName = trimmed(parts[0]);
  result.firmwareVersion = trimmed(parts[1]);
  result.hardwareModel = trimmed(parts[2]);

  std::string lower = implName;
  std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
  result.isMicroPython = lower.find("micropython") != std::string::npos;

  INFO("Probe → impl: " + implName + "  fw: " + result.firmwareVersion +
       "  hw: " + result.hardwareModel);

  return result;
}
ReplClient::~ReplClient() { exitRawRepl(); }
std::pair<RUN_STATE, std::string> ReplClient::runPythonCmd(
    const std::string& code) {
  // Send the code followed by the raw REPL end marker.
  writeAll(code + "\x04");
  return readUntil("\x04>", 3000);
}
}  // namespace ZanaBlocks::EspTools