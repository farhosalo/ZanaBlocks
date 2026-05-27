#include "ReplClient.h"

#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <thread>
#include <vector>

#include "Base64.h"
#include "Logging.h"

namespace ZanaBlocks::EspTools {
using Utilities::base64Encode;

auto constexpr HUNDRED =
    100;  // Give the device time to reboot before the caller tries to reconnect

bool ReplClient::putFile(const std::string& localPath,
                         const std::string& remotePath,
                         ProgressCallback callback) {
  // Helper to invoke the progress callback if it's set
  auto invokeCallback = [&](const std::string& message) {
    if (callback) {
      callback(message);
    }
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
  file.read(reinterpret_cast<char*>(contents.data()),  // NOLINT
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
    const std::size_t chunkLen = std::min(CHUNK_SIZE, fileSize - sent);

    // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-pointer-arithmetic]
    const std::string b64 = base64Encode(contents.data() + sent, chunkLen);

    // Decode on-device and write
    const std::string cmd = "_f.write(ubinascii.a2b_base64(b'" + b64 + "'))";

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
                   std::to_string((sent * HUNDRED) / fileSize) + "%)");
  }

  // Close remote file
  runPythonCmd("_f.close(); del _f");
  return true;
}

void ReplClient::reset() {
  // We attempt to send the reset command. If the serial write fails (e.g.
  // device already disconnected or rebooting), we log the error but continue to
  // the sleep period to allow the hardware state to settle.
  writeAll("import machine; machine.reset()\x04");

  // Give the device time to reboot before the caller tries to reconnect
  auto constexpr RESET_PAUSE_MS = 1'500;
  std::this_thread::sleep_for(std::chrono::milliseconds(RESET_PAUSE_MS));
}

void ReplClient::writeAll(const std::string& data) {
  const auto TIMEOUT_MS = 1000;
  sp_blocking_write(mSerialConnection.get(), data.data(), data.size(),
                    TIMEOUT_MS);
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
  auto constexpr CHUNK_SIZE = 64;
  std::array<char, CHUNK_SIZE> tmp{};
  while (buffer.find(marker) == std::string::npos) {
    // Read a chunk of data from the serial port with a timeout. We use
    // sp_blocking_read which will wait until data is available or the timeout
    // is reached. The data is read into a temporary buffer and then appended to
    // the main buffer.
    const int numberOfReadBytes = sp_blocking_read(
        mSerialConnection.get(), tmp.data(), tmp.size(), timeoutMs);
    if (numberOfReadBytes < 0) {  // Error occurred during read
      buffer = "Failed to read from REPL";
      ERROR(buffer);
      return {RUN_STATE::ERROR, buffer};
    }
    if (numberOfReadBytes == 0) {  // Timeout occurred
      buffer = "REPL read timeout";
      ERROR(buffer);
      return {RUN_STATE::TIME_OUT, buffer};
    }
    buffer.append(tmp.data(), static_cast<std::size_t>(numberOfReadBytes));
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
  sp_blocking_read(mSerialConnection.get(), nullptr, 0,
                   HUNDRED);  // 100ms pause

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
  if (out.starts_with("OK")) {
    out = out.substr(2);
  }

  // Trim surrounding whitespace / CR LF
  auto trimmed = [](std::string str) {
    str.erase(0, str.find_first_not_of(" \t\r\n"));
    str.erase(str.find_last_not_of(" \t\r\n") + 1);
    return str;
  };
  out = trimmed(out);

  // Split on '|'
  auto splitPipe = [](const std::string& split) -> std::vector<std::string> {
    std::vector<std::string> parts;
    std::size_t start = 0;
    std::size_t pos = 0;
    while ((pos = split.find('|', start)) != std::string::npos) {
      parts.push_back(split.substr(start, pos - start));
      start = pos + 1;
    }
    parts.push_back(split.substr(start));
    return parts;
  };

  auto parts = splitPipe(out);
  if (parts.size() < 3) {
    INFO("Probe: unexpected response format: " + out);
    return result;
  }
  // NOLINTBEGIN [cppcoreguidelines-pro-bounds-avoid-unchecked-container-access]
  const std::string implName = trimmed(parts[0]);
  result.firmwareVersion = trimmed(parts[1]);
  result.hardwareModel = trimmed(parts[2]);
  // NOLINTEND

  std::string lower = implName;
  std::ranges::transform(lower, lower.begin(), ::tolower);
  result.isMicroPython = lower.find("micropython") != std::string::npos;

  INFO("Probe → impl: " + implName + "  fw: " + result.firmwareVersion +
       "  hw: " + result.hardwareModel);

  return result;
}
ReplClient::~ReplClient() { exitRawRepl(); }
std::pair<RUN_STATE, std::string> ReplClient::runPythonCmd(
    const std::string& code) {
  auto constexpr TimeoutMs = 3000;
  // Send the code followed by the raw REPL end marker.
  writeAll(code + "\x04");
  return readUntil("\x04>", TimeoutMs);
}
}  // namespace ZanaBlocks::EspTools