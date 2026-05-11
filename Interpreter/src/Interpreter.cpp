#include "Interpreter.h"

#include <fstream>

#include "Logging.h"

using namespace KidTech::Utilities;

namespace KidTech::Interpreter {

bool Interpreter::interpret(const std::shared_ptr<Schema::Root>& schema) {
  reset();

  if (schema->has_mainloop()) {
    return extractLoop(schema->mainloop());
  }
  return false;
}

auto Interpreter::extractPrint(const Schema::Print& print) {
  auto line = "print(\"" + print.msg() + "\")";
  mLines.emplace_back(mCurrentLevel, line);
}

auto Interpreter::extractSleep(const Schema::Sleep& sleep) {
  auto line = "sleep_ms(" + std::to_string(sleep.duration()) + ")";
  mLines.emplace_back(mCurrentLevel, line);
}

auto Interpreter::extractLed(const Schema::LED& led) {
  auto line = "led = Pin(" + std::to_string(led.pin()) + ", Pin.OUT)";
  mLines.emplace_back(mCurrentLevel, line);
  const std::string ledState = led.state() ? "1" : "0";
  line = "led.value(" + ledState + ")";
  mLines.emplace_back(mCurrentLevel, line);
}

bool Interpreter::saveToFile(const std::string& filename) {
  // Save the generated lines to a file
  std::ofstream outFile(filename);
  if (!outFile.is_open()) {
    return false;
  }
  for (const auto& [indent, line] : mLines) {
    auto indentLevel = indent * indentSize;
    outFile << std::string(indentLevel, ' ') << line << "\n";
  }
  outFile.close();
  return true;
}

void Interpreter::reset() {
  mCurrentLevel = 0;
  mLines.clear();

  mLines.emplace_back(0, "from machine import Pin");
  mLines.emplace_back(0, "from time import sleep_ms");
  mLines.emplace_back(0, "");
}

bool Interpreter::extractLoop(const Schema::Loop& loop) {
  std::string line;
  if (loop.count() <= 0) {
    line = "while True:";
  } else {
    line = "for i in range(" + std::to_string(loop.count()) + "):";
  }
  mLines.emplace_back(mCurrentLevel, line);
  mCurrentLevel++;
  for (const auto& action : loop.actions()) {
    if (action.has_print()) {
      extractPrint(action.print());
    } else if (action.has_sleep()) {
      extractSleep(action.sleep());
    } else if (action.has_loop()) {
      if (!extractLoop(action.loop())) {
        return false;
      }
    } else if (action.has_led()) {
      extractLed(action.led());
    } else {
      ERROR("Unknown action type in loop");
      return false;  // Unknown action type
    }
  }

  return true;
}
}  // namespace KidTech::Interpreter