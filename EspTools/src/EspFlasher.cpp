#include "EspFlasher.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "EspTools.h"
#include "Logging.h"
#include "SerialPort.h"
#include "esp_targets.h"

#ifndef FAST_BAUD
#define FAST_BAUD 230400  // safe fallback
#endif

using namespace ZanaBlocks::Utilities;

Bytes EspFlasher::loadFirmware(const std::string& path) {
  std::ifstream firmwareFile(path, std::ios::binary | std::ios::ate);
  if (!firmwareFile) {
    status("Cannot open: " + path);
    return {};
  }
  auto size = firmwareFile.tellg();
  firmwareFile.seekg(0);
  Bytes firmware(static_cast<size_t>(size));
  firmwareFile.read(reinterpret_cast<char*>(firmware.data()), size);
  return firmware;
}

static std::optional<uint32_t> getFlashAddress(target_chip_t chip) {
  switch (chip) {
    case ESP32_CHIP:
    case ESP32S2_CHIP:
      return 0x1000;
    case ESP32S3_CHIP:
    case ESP32C3_CHIP:
    case ESP32C6_CHIP:
    case ESP32H2_CHIP:
      return 0x0000;
    default:
      return std::nullopt;
  }
}

static constexpr uint32_t BLOCK = 1024;

void EspFlasher::flashFirmware(esp_loader_t* loader, const uint32_t addr,
                               const Bytes& firmware) {
  // Must be 4-byte aligned for the flash controller
  uint32_t imageSize = static_cast<uint32_t>(firmware.size());
  if (imageSize % 4 != 0) imageSize += 4 - (imageSize % 4);

  esp_loader_flash_cfg_t cfg{};
  cfg.offset = addr;
  cfg.image_size = imageSize;
  cfg.block_size = BLOCK;
  cfg.skip_verify = false;

  if (auto error = esp_loader_flash_start(loader, &cfg);
      error != ESP_LOADER_SUCCESS) {
    status("Failed to start flash: " + std::to_string(error));
    return;
  }

  // BLOCK-sized scratch buffer — library writes its own 0xFF padding into
  // the tail of this buffer, past the actual chunk bytes
  Bytes block(BLOCK, 0xFF);

  size_t written = 0;
  while (written < firmware.size()) {
    uint32_t chunk = static_cast<uint32_t>(
        std::min<size_t>(BLOCK, firmware.size() - written));

    std::copy(firmware.begin() + written, firmware.begin() + written + chunk,
              block.begin());

    // Pass actual chunk size — library pads block[] and sends BLOCK bytes,
    // but only accumulates MD5 over `chunk` bytes
    if (auto error = esp_loader_flash_write(loader, &cfg, block.data(), chunk);
        error != ESP_LOADER_SUCCESS) {
      status("Failed to write flash: " + std::to_string(error));
      return;
    }

    written += chunk;
    progress(written * 100 / firmware.size());
    fflush(stdout);
  }

  if (auto error = esp_loader_flash_finish(loader, &cfg);
      error != ESP_LOADER_SUCCESS) {
    status("Failed to finish flash: " + std::to_string(error));
    return;
  }
  status("Flash finished successfully!");
}

bool EspFlasher::flash(const std::string& device,
                       const std::string& firmwarePath) {
  constexpr uint32_t DEFAULT_BUAD{115200};

  auto firmware = loadFirmware(firmwarePath);
  status("Firmware loaded: " + firmwarePath + " (" +
         std::to_string(firmware.size()) + " bytes) ");

  auto connection = ZanaBlocks::EspTools::connect(device, DEFAULT_BUAD);
  if (connection == nullptr) {
    status("Failed to connect to device: " + device);
    return false;
  }
  auto* port = createSerialPort(connection.get());

  if (!port) {
    status("Failed to open serial port: " + device);
    return false;
  }
  status("Serial port opened: " + device + " @" + std::to_string(DEFAULT_BUAD) +
         " baud");

  // ── connect ───────────────────────────────────────────────────────────────
  esp_loader_t loader{};

  if (auto error = esp_loader_init_uart(&loader, getBaseSerialPort(port));
      error != ESP_LOADER_SUCCESS) {
    status("Failed to init uart: " + std::to_string(error));
    return false;
  }
  status("UART initialized.");

  status("Connecting on " + device + " @" + std::to_string(DEFAULT_BUAD) +
         " baud...");
  esp_loader_connect_args_t conn = ESP_LOADER_CONNECT_DEFAULT();
  conn.trials = 20;         // default is 10, give more room
  conn.sync_timeout = 200;  // ms per attempt

  if (onEnterBootloader) {
    onEnterBootloader();
    status("Waiting for the user to put the device into bootloader mode...");
    while (!mEnteredBootloaderMode.load()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

  } else {
    status("No onEnterBootloader callback provided.");
    return false;
  }
  // Note: esp_loader_connect() internally toggles DTR/RTS to reset the chip
  // into bootloader mode, so no need to do it ourselves in op_enter_bootloader
  if (auto error = esp_loader_connect(&loader, &conn);
      error != ESP_LOADER_SUCCESS) {
    status("Failed to connect: " + std::to_string(error));
    return false;
  }

  // ── detect chip ───────────────────────────────────────────────────────────
  target_chip_t chip = esp_loader_get_target(&loader);
  if (!getFlashAddress(chip).has_value()) {
    status("Unsupported chip id=" + std::to_string(static_cast<int>(chip)));
    return false;
  }
  uint32_t flash_addr = getFlashAddress(chip).value();

  // ── bump baud ─────────────────────────────────────────────────────────────
  if (FAST_BAUD > static_cast<int>(DEFAULT_BUAD)) {
    if (esp_loader_change_transmission_rate(&loader, FAST_BAUD) ==
        ESP_LOADER_SUCCESS) {
      status("Switched to " + std::to_string(FAST_BAUD) + "baud");
    } else {
      status("Baud rate bump failed, staying at " +
             std::to_string(DEFAULT_BUAD) + "baud.");
    }
  } else {
    status("Already at max supported baud rate.");
  }

  // ── flash ─────────────────────────────────────────────────────────────────
  status("Flashing MicroPython (" + std::to_string(firmware.size()) +
         " bytes)...");
  flashFirmware(&loader, flash_addr, firmware);

  // ── reboot ────────────────────────────────────────────────────────────────
  esp_loader_reset_target(&loader);
  status("Resetting...");
  esp_loader_deinit(&loader);
  status("Done!");

  return true;
}

void EspFlasher::status(const std::string& msg) const {
  if (onStatus) {
    onStatus(msg);
  }
}

void EspFlasher::progress(const int progress) const {
  if (onProgress) {
    onProgress(progress);
  }
}
