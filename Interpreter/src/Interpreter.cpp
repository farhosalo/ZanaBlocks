#include "Interpreter.h"

#include <fstream>

#include "Logging.h"

namespace ZanaBlocks::Interpreter {

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
  mImports.insert("from time import sleep_ms");

  auto line = "sleep_ms(" + std::to_string(sleep.duration()) + ")";
  mLines.emplace_back(mCurrentLevel, line);
}

auto Interpreter::extractLed(const Schema::LED& led) {
  mImports.insert("from machine import Pin");

  auto varName = "led_" + std::to_string(led.pin());
  auto line = varName + "  = Pin(" + std::to_string(led.pin()) + ", Pin.OUT)";

  mGlobalVariables.insert(line);

  const std::string ledState = led.state() ? "1" : "0";
  line = varName + ".value(" + ledState + ")";
  mLines.emplace_back(mCurrentLevel, line);
}

auto Interpreter::extractPwm(const Schema::Pwm& pwm) {
  mImports.insert("from machine import PWM");

  auto varName = "pwm_" + std::to_string(pwm.pin());
  auto line = varName + " = PWM(Pin(" + std::to_string(pwm.pin()) + "))";
  mGlobalVariables.insert(line);

  line = varName + ".duty(" + std::to_string(pwm.duty()) + ")";
  mLines.emplace_back(mCurrentLevel, line);
  line = varName + ".freq(" + std::to_string(pwm.frequency()) + ")";
  mLines.emplace_back(mCurrentLevel, line);
}

bool Interpreter::saveToFile(const std::string& filename) {
  // Save the generated lines to a file
  std::ofstream outFile(filename);
  if (!outFile.is_open()) {
    return false;
  }

  for (const auto& line : mImports) {
    outFile << line << "\n";
  }
  outFile << "\n";

  for (const auto& line : mGlobalVariables) {
    outFile << line << "\n";
  }
  outFile << "\n";

  for (const auto& [indent, line] : mLines) {
    auto indentLevel = indent * INDENT_SIZE;
    outFile << std::string(indentLevel, ' ') << line << "\n";
  }
  outFile.close();
  return true;
}

void Interpreter::reset() {
  mCurrentLevel = 0;
  mLines.clear();
  mGlobalVariables.clear();
  mImports.clear();
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
    } else if (action.has_pwm()) {
      extractPwm(action.pwm());
    } else {
      ERROR("Unknown action type in loop");
      return false;  // Unknown action type
    }
  }
  mCurrentLevel--;

  return true;
}
}  // namespace ZanaBlocks::Interpreter