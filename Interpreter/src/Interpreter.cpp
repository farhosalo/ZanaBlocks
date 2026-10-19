#include "Interpreter.h"

#include <algorithm>
#include <fstream>
#include <iosfwd>
#include <sstream>
#include <variant>
#include <vector>

#include "Logging.h"
#include "StringUtils.h"

namespace {
/**
 * @brief Helper function that clears and resets an ostringstream for reuse.
 *
 * This helper reduces code duplication when creating new string
 * buffers. Note: Creating fresh ostringstream objects is preferred
 * over reusing to avoid any potential internal state issues.
 *
 * @param oss Reference to the ostringstream to reset.
 */
inline void resetOss(std::ostringstream& oss) {
  oss.str("");
  oss.clear();
}

}  // namespace

namespace ZanaBlocks::Interpreter {
bool Interpreter::interpret(const std::shared_ptr<Schema::Root>& schema) {
  // Validate that schema and main loop exist before attempting extraction.
  if (!schema) {
    ERROR("Schema is null");
    return false;
  }

  if (!schema->has_mainloop()) {
    ERROR("Schema is missing main loop structure");
    return false;
  }

  // Reset internal state before processing new schema to ensure clean state.
  reset();

  // Begin recursive extraction of all actions from the main loop hierarchy.
  if (!extractLoop(schema->mainloop())) {
    reset();
    return false;
  }
  return true;
}

bool Interpreter::extractPrint(const Schema::Print& print) {
  // Create fresh output stream for each extraction to avoid state pollution.
  std::ostringstream oss;
  const std::string escaped = Utilities::escapeString(print.msg());

  if (escaped.empty()) {
    ERROR("Print message is empty");  // TODO
    return false;
  }

  oss << "print(\"" << escaped << "\")";

  // Store the generated statement with current indentation level.
  mLines.emplace_back(mCurrentLevel, oss.str());
  return true;
}

bool Interpreter::extractSleep(const Schema::Sleep& sleep) {
  // Validate sleep duration before proceeding
  const auto duration = sleep.duration();
  auto constexpr MAX_DURATION = 65535;
  if (duration < 1 || duration > MAX_DURATION) {
    ERROR("Invalid sleep duration: " + std::to_string(duration));
    return false;
  }

  // Register import for time module if not already present.
  addImport("from time import sleep_ms");

  // Generate the function call with converted duration parameter.
  std::ostringstream oss;
  oss << "sleep_ms(" << Utilities::safeToString(duration) << ")";

  // Add the generated sleep statement to execution lines.
  mLines.emplace_back(mCurrentLevel, oss.str());
  return true;
}

bool Interpreter::extractLed(const Schema::LED& led) {
  // Validate LED configuration
  if (led.pin() >= MAX_PINS || led.pin() < 1) {
    ERROR("Invalid LED pin number: " << led.pin());
    return false;
  }

  addImport("from machine import Pin");
  const auto pinNumAsString = Utilities::safeToString(led.pin());
  const std::string varName = "led_" + pinNumAsString;

  // First pass: Create global variable initialization for LED Pin object.
  std::ostringstream oss;
  oss << varName << " = Pin(" << pinNumAsString << ", Pin.OUT)";
  addGlobalVariable(oss.str());

  // Second pass: Set the initial GPIO state value (HIGH/LOW based on boolean
  // state).
  const auto* ledState = led.state() ? "1" : "0";
  resetOss(oss);
  oss << varName << ".value(" << ledState << ")";

  // Add the initialization call to execution lines with proper indentation.
  mLines.emplace_back(mCurrentLevel, oss.str());
  return true;
}

bool Interpreter::extractPwm(const Schema::Pwm& pwm) {
  // Validate PWM configuration
  if (pwm.pin() >= MAX_PINS || pwm.pin() < 1) {
    ERROR("Invalid PWM pin number: " << pwm.pin());
    return false;
  }

  addImport("from machine import PWM");
  const auto pinAsString = Utilities::safeToString(pwm.pin());
  const auto duty = pwm.duty();
  const auto freq = pwm.frequency();

  if (duty > MAX_PWM_DUTY || freq < 1 || freq > MAX_PWM_FREQ) {
    ERROR("Invalid PWM configuration: duty=" << duty << ", freq=" << freq);
    return false;
  }

  const auto varName = "pwm_" + pinAsString;

  //  Generate PWM object with duty cycle assignment.
  std::ostringstream oss;
  oss << varName << " = PWM(Pin(" << pinAsString << "))";
  addGlobalVariable(oss.str());

  // Configure duty cycle on the PWM object.
  resetOss(oss);
  oss << varName << ".duty(" << Utilities::safeToString(duty) << ")";
  mLines.emplace_back(mCurrentLevel, oss.str());

  // Set the PWM frequency at end of all duty operations for this pin.
  resetOss(oss);
  oss << varName << ".freq(" << Utilities::safeToString(freq) << ")";
  mLines.emplace_back(mCurrentLevel, oss.str());
  return true;
}

bool Interpreter::saveToFile(const std::string& filename) {
  // Validate filename is not empty
  if (filename.empty()) {
    ERROR("Filename cannot be empty");
    return false;
  }

  if (mLines.empty()) {
    WARN("Nothing to save");
    return false;
  }

  /** Attempt to open the output file for writing operations. */
  std::ofstream outFile(filename);
  if (!outFile.is_open()) {
    ERROR("Failed to open file: " + filename);
    return false;
  }

  // Write all accumulated import statements first (Python requires imports at
  // top).
  for (const auto& line : mImports) {
    outFile << line << "\n";
  }

  // Add blank line separator between sections for readability.
  outFile << "\n";

  // Write all global variable declarations that must exist before loop
  // execution.
  for (const auto& line : mGlobalVariables) {
    outFile << line << "\n";
  }

  // Separator before the main execution block.
  outFile << "\n";

  auto constexpr INDENT_SIZE{4};
  /**
   * Write all executable lines with proper indentation based on nesting depth.
   * Multiplies current level by constant INDENT_SIZE for spacing calculation.
   */
  for (const auto& [indent, line] : mLines) {
    std::string indentStr;
    if (!line.empty()) {
      // Validate we don't overflow string resize
      const size_t indentSize = static_cast<size_t>(indent) * INDENT_SIZE;
      indentStr.resize(indentSize, ' ');
      outFile << std::string(indentSize, ' ') << indentStr << line << "\n";

    } else {
      // Don't write empty lines with just whitespace
      outFile << "...\n";  // Placeholder for empty code
    }
  }

  // Ensure all buffered data is flushed to disk before returning.
  outFile.flush();
  if (!outFile.good()) {
    ERROR("Error writing to file: " + filename);
    return false;
  }
  outFile.close();
  return true;
}
void Interpreter::reset() {
  // Reset all member variables to clean state for new interpretation cycle.
  mCurrentLevel = 0;
  mLines.clear();
  mGlobalVariables.clear();
  mImports.clear();
}
void Interpreter::addGlobalVariable(const std::string& var) {
  const auto iter = std::ranges::find(mGlobalVariables, var);
  if (iter == mGlobalVariables.end()) {
    mGlobalVariables.push_back(var);
  }
}
void Interpreter::addImport(const std::string& import) {
  const auto iter = std::ranges::find(mImports, import);
  if (iter == mImports.end()) {
    mImports.push_back(import);
  }
}
bool Interpreter::extractLoop(const Schema::Loop& loop) {
  if (loop.actions().size() <= 0) {
    ERROR("Empty loop not allowed, A loop must contain at least one action");
    return false;
  }
  const auto count = loop.count();

  std::ostringstream oss;

  // Choose loop type based on iteration count from schema.
  if (count == 0) {
    oss << "while True:";
  } else {
    // Create bounded loop with range count converted to string safely.
    oss << "for i in range(" << Utilities::safeToString(count) << "):";
  }

  // Add the loop keyword line at current indentation level.
  mLines.emplace_back(mCurrentLevel, oss.str());

  // Increment indentation level for code block contained within this loop.
  mCurrentLevel++;

  // Process each action in the sequential list defined by the schema.
  // bool actionsProcessed = true;
  for (const auto& action : loop.actions()) {
    switch (action.action_case()) {
      case Schema::Action::kPrint:
        if (!extractPrint(action.print())) {
          return false;
        }
        break;
      case Schema::Action::kSleep:
        if (!extractSleep(action.sleep())) {
          return false;
        }
        break;
      case Schema::Action::kLoop:
        if (action.loop().actions().empty() || !extractLoop(action.loop())) {
          ERROR("Failed to process nested loop structure");
          // actionsProcessed = false;
          return false;
        }
        break;
      case Schema::Action::kLed:
        if (!extractLed(action.led())) {
          return false;
        }
        break;
      case Schema::Action::kPwm:
        if (!extractPwm(action.pwm())) {
          return false;
        }
        break;

      default:
        ERROR("Unknown action type encountered in loop");
        return false;
    }
  }

  // Decrement indentation level when exiting the loop block scope.
  mCurrentLevel--;

  return true;
}
}  // namespace ZanaBlocks::Interpreter
