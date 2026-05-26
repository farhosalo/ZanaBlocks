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

using namespace std::chrono_literals;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::steady_clock;

namespace ZanaBlocks::EspTools {

struct FlashPort {
  esp_loader_port_t mBaseFlashPort{};  // must be first
  sp_port* mSerialPort = nullptr;
  uint32_t mTimeoutMs = 0;
  steady_clock::time_point mTimerStart;
};

namespace {
/**
 * Helper to cast from esp_loader_port_t to our FlashPort struct. Assumes
 * that the esp_loader_port_t is the first member of FlashPort.
 */
FlashPort* cast(esp_loader_port_t* baseFlashPort) {
  // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-pointer-arithmetic]
  return container_of(baseFlashPort, FlashPort, mBaseFlashPort);
}

/**
 * Helper to convert libserialport return values to esp_loader_error_t.
 */
esp_loader_error_t seriallPortResult2FlashPortResult(sp_return serialPortReturn,
                                                     int expected) {
  if (serialPortReturn == static_cast<sp_return>(expected)) {
    return ESP_LOADER_SUCCESS;
  }
  if (serialPortReturn >= 0) {
    return ESP_LOADER_ERROR_TIMEOUT;  // transferred < expected within timeout}
  }
  return ESP_LOADER_ERROR_FAIL;  // serialPortReturn < 0:
                                 // SP_ERR_ARG/FAIL/MEM/SUPP
}
esp_loader_error_t opWrite(esp_loader_port_t* baseFlashPort,
                           const uint8_t* data, uint16_t size,
                           uint32_t timeoutMs) {
  auto result = sp_blocking_write(cast(baseFlashPort)->mSerialPort, data, size,
                                  timeoutMs);
  return seriallPortResult2FlashPortResult(result, size);
}

esp_loader_error_t opRead(esp_loader_port_t* baseFlashPort, uint8_t* data,
                          uint16_t size, uint32_t timeoutMs) {
  auto result =
      sp_blocking_read(cast(baseFlashPort)->mSerialPort, data, size, timeoutMs);
  return seriallPortResult2FlashPortResult(result, size);
}

void opEnterBootloader(esp_loader_port_t* /*baseFlashPort*/) {
  // No need to toggle DTR/RTS here, we'll ask the user to do it
  // manually in the UI
}
void opReset(esp_loader_port_t* baseFlashPort) {
  auto constexpr sleepDuration{100ms};
  auto* flashPort = cast(baseFlashPort);
  sp_set_rts(flashPort->mSerialPort, SP_RTS_ON);
  std::this_thread::sleep_for(sleepDuration);
  sp_set_rts(flashPort->mSerialPort, SP_RTS_OFF);
}

void opDelay(esp_loader_port_t* /*baseFlashPort*/, uint32_t delayMs) {
  std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
}

void opStartTimer(esp_loader_port_t* baseFlashPort, uint32_t timeoutMs) {
  auto* flashPort = cast(baseFlashPort);
  flashPort->mTimeoutMs = timeoutMs;
  flashPort->mTimerStart = std::chrono::steady_clock::now();
}

uint32_t opRemaining(esp_loader_port_t* baseFlashPort) {
  auto* flashPort = cast(baseFlashPort);
  auto elapsed = static_cast<uint32_t>(
      duration_cast<milliseconds>(steady_clock::now() - flashPort->mTimerStart)
          .count());
  return (elapsed >= flashPort->mTimeoutMs) ? 0U
                                            : flashPort->mTimeoutMs - elapsed;
}

void opDebug(esp_loader_port_t* /*baseFlashPort*/, const char* str) {
  fputs(str, stderr);
}

esp_loader_error_t opChangeBuad(esp_loader_port_t* baseFlashPort,
                                uint32_t baud) {
  return (sp_set_baudrate(cast(baseFlashPort)->mSerialPort,
                          static_cast<int>(baud)) == SP_OK)
             ? ESP_LOADER_SUCCESS
             : ESP_LOADER_ERROR_FAIL;
}

esp_loader_error_t opInit(esp_loader_port_t* /*baseFlashPort*/) {
  return ESP_LOADER_SUCCESS;
}
void opDeinit(esp_loader_port_t* /*baseFlashPort*/) {}
}  // namespace

static const esp_loader_port_ops_t s_ops = {
    .init = opInit,
    .deinit = opDeinit,
    .enter_bootloader = opEnterBootloader,
    .reset_target = opReset,
    .start_timer = opStartTimer,
    .remaining_time = opRemaining,
    .delay_ms = opDelay,
    .debug_print = opDebug,
    .change_transmission_rate = opChangeBuad,
    .write = opWrite,
    .read = opRead,
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
}  // namespace ZanaBlocks::EspTools