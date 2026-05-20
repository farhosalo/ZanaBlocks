#include "SerialPort.h"

#include <libserialport.h>

#include <chrono>
#include <cstddef>
#include <cstdio>
#include <thread>

#include "esp_loader_error.h"

// container_of is not available on MSVC
#ifndef container_of
#define container_of(ptr, type, member) \
  reinterpret_cast<type*>(reinterpret_cast<char*>(ptr) - offsetof(type, member))
#endif

struct FlashPort {
  esp_loader_port_t mBaseFlashPort;  // must be first
  sp_port* mSerialPort;
  uint32_t mTimeoutMs;
  std::chrono::steady_clock::time_point mTimerStart;
};

/**
 * Helper to cast from esp_loader_port_t to our FlashPort struct. Assumes
 * that the esp_loader_port_t is the first member of FlashPort.
 */
static FlashPort* cast(esp_loader_port_t* baseFlashPort) {
  return container_of(baseFlashPort, FlashPort, mBaseFlashPort);
}

/**
 * Helper to convert libserialport return values to esp_loader_error_t.
 */
static esp_loader_error_t seriallPortResult2FlashPortResult(
    sp_return serialPortReturn, int expected) {
  if (serialPortReturn == static_cast<sp_return>(expected))
    return ESP_LOADER_SUCCESS;
  if (serialPortReturn >= 0)
    return ESP_LOADER_ERROR_TIMEOUT;  // transferred < expected within timeout
  return ESP_LOADER_ERROR_FAIL;       // serialPortReturn < 0:
                                      // SP_ERR_ARG/FAIL/MEM/SUPP
}

static esp_loader_error_t op_write(esp_loader_port_t* baseFlashPort,
                                   const uint8_t* data, uint16_t size,
                                   uint32_t timeout_ms) {
  auto result = sp_blocking_write(cast(baseFlashPort)->mSerialPort, data, size,
                                  timeout_ms);
  return seriallPortResult2FlashPortResult(result, size);
}

static esp_loader_error_t op_read(esp_loader_port_t* baseFlashPort,
                                  uint8_t* data, uint16_t size,
                                  uint32_t timeout_ms) {
  auto result = sp_blocking_read(cast(baseFlashPort)->mSerialPort, data, size,
                                 timeout_ms);
  return seriallPortResult2FlashPortResult(result, size);
}

static void op_enter_bootloader(esp_loader_port_t* baseFlashPort) {
  return;  // No need to toggle DTR/RTS here, we'll ask the user to do it
           // manually in the UI
}
static void op_reset(esp_loader_port_t* baseFlashPort) {
  auto* flashPort = cast(baseFlashPort);
  sp_set_rts(flashPort->mSerialPort, SP_RTS_ON);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  sp_set_rts(flashPort->mSerialPort, SP_RTS_OFF);
}

static void op_delay(esp_loader_port_t*, uint32_t delayMs) {
  std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
}

static void op_start_timer(esp_loader_port_t* baseFlashPort, uint32_t ms) {
  auto* flashPort = cast(baseFlashPort);
  flashPort->mTimeoutMs = ms;
  flashPort->mTimerStart = std::chrono::steady_clock::now();
}

static uint32_t op_remaining(esp_loader_port_t* baseFlashPort) {
  using namespace std::chrono;
  auto* flashPort = cast(baseFlashPort);
  auto elapsed = static_cast<uint32_t>(
      duration_cast<milliseconds>(steady_clock::now() - flashPort->mTimerStart)
          .count());
  return (elapsed >= flashPort->mTimeoutMs) ? 0u
                                            : flashPort->mTimeoutMs - elapsed;
}

static void op_debug(esp_loader_port_t*, const char* str) {
  fputs(str, stderr);
}

static esp_loader_error_t op_change_baud(esp_loader_port_t* baseFlashPort,
                                         uint32_t baud) {
  return (sp_set_baudrate(cast(baseFlashPort)->mSerialPort,
                          static_cast<int>(baud)) == SP_OK)
             ? ESP_LOADER_SUCCESS
             : ESP_LOADER_ERROR_FAIL;
}

static esp_loader_error_t op_init(esp_loader_port_t*) {
  return ESP_LOADER_SUCCESS;
}
static void op_deinit(esp_loader_port_t*) {}

static const esp_loader_port_ops_t s_ops = {
    .init = op_init,
    .deinit = op_deinit,
    .enter_bootloader = op_enter_bootloader,
    .reset_target = op_reset,
    .start_timer = op_start_timer,
    .remaining_time = op_remaining,
    .delay_ms = op_delay,
    .debug_print = op_debug,
    .change_transmission_rate = op_change_baud,
    .write = op_write,
    .read = op_read,
    .spi_set_cs = nullptr,
    .sdio_write = nullptr,
    .sdio_read = nullptr,
    .sdio_card_init = nullptr,
};

FlashPort* createSerialPort(sp_port* serialPort) {
  auto* flashPort = new FlashPort{};
  flashPort->mBaseFlashPort.ops = &s_ops;
  flashPort->mSerialPort = serialPort;
  return flashPort;
}

esp_loader_port_t* getBaseSerialPort(FlashPort* flashPort) {
  return &flashPort->mBaseFlashPort;
}