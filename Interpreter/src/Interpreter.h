#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "Schema.pb.h"
namespace InterpreterTests {
class InterpreterTest;
}
namespace ZanaBlocks::Interpreter {
auto constexpr MAX_PINS = 1024;
auto constexpr MAX_PWM_DUTY = 255;
auto constexpr MAX_PWM_FREQ = 10000;

/**
 * @brief The Interpreter class is responsible for interpreting a given schema
 * and generating corresponding Python code lines. It supports various action
 * types including loops, print statements, sleep durations, LED control, and
 * PWM configurations defined in the Schema protobuf definitions.
 *
 * The generated code maintains proper indentation levels and separates global
 * variable declarations from execution lines for clean Python output.
 */
class Interpreter {
 public:
  /**
   * @brief Interprets the provided schema and generates corresponding code
   * lines.
   *
   * This method validates the schema, resets internal state, and extracts all
   * actionable instructions from the main loop section of the schema into
   * executable Python statements.
   *
   * @param schema A shared pointer to the Schema::Root object containing the
   * execution graph.
   * @return true if interpretation completed successfully, false if schema is
   * invalid or processing fails.
   */
  [[nodiscard]] bool interpret(const std::shared_ptr<Schema::Root>& schema);

  /**
   * @brief Writes all accumulated code lines to the specified file with proper
   * formatting.
   *
   * This method exports the generated Python code in a structured format
   * including:
   * - All imported modules
   * - Global variable declarations (initialized before loop execution)
   * - Execution steps with appropriate indentation levels
   *
   * @param filename The path to the output file where generated code will be
   * saved.
   * @return true if successful, false if file write failed or invalid.
   */
  bool saveToFile(const std::string& filename);

 private:
  /**
   * @brief Extracts and processes a loop structure from the schema into Python
   * code.
   *
   * Handles both infinite loops (while True) and bounded loops (for range),
   * recursively processing nested loops and extracting all action types within
   * them.
   *
   * @param loop A reference to the Schema::Loop object containing nested
   * actions.
   * @return true if loop was successfully processed, false on error or failure.
   */
  [[nodiscard]] bool extractLoop(const Schema::Loop& loop);

  /**
   * @brief Extracts a print action from schema and formats it as Python print
   * statement.
   *
   * Creates an ostringstream to construct the print("message") statement with
   * proper string escaping for safety and correctness.
   *
   * @param print A reference to the Schema::Print object containing message
   * content.
   * @return true if print node was successfully processed, false on error or
   * failure.
   */
  bool extractPrint(const Schema::Print& print);

  /**
   * @brief Extracts a sleep action from schema and imports the time module.
   *
   * Adds the required import statement once for all sleep operations, then
   * generates the sleep_ms() call with the specified duration parameter.
   *
   * @param sleep A reference to the Schema::Sleep object containing duration
   * value.
   * @return true if sleep node was successfully processed, false on error or
   * failure.
   */
  bool extractSleep(const Schema::Sleep& sleep);

  /**
   * @brief Extracts an LED action from schema, generating Pin and value
   * assignment code.
   *
   * Imports the machine.Pin module, creates a Pin variable, assigns it to
   * output mode, and sets the initial GPIO state (HIGH/LOW) based on the
   * configured state value.
   *
   * @param led A reference to the Schema::LED object containing pin
   * configuration and state.
   * @return true if LED node was successfully processed, false on error or
   * failure.
   */
  bool extractLed(const Schema::LED& led);

  /**
   * @brief Extracts a PWM action from schema, generating Pin initialization and
   * duty cycle code.
   *
   * Creates both Pin and PWM objects with proper initialization including
   * frequency and duty cycle configuration for pulse width modulation control.
   *
   * @param pwm A reference to the Schema::Pwm object containing pin, frequency,
   * and duty values.
   * @return true if PWM node was successfully processed, false on error or
   * failure.
   */
  bool extractPwm(const Schema::Pwm& pwm);

  /**
   * @brief Resets all internal state to prepare for a new interpretation cycle.
   *
   * Clears collected lines, global variables, imports, and resets the
   * current indentation level tracking counter to zero.
   */
  void reset();
  void addGlobalVariable(const std::string& var);
  void addImport(const std::string& import);

  void validateSchema(const Schema::Loop& loop);

  /**
   * @brief Current nesting depth level used for proper code indentation.
   *
   * Tracks hierarchical structure of nested loops during extraction process.
   */
  int32_t mCurrentLevel{0};

  /**
   * @brief Collection of generated execution lines with associated indentation
   * levels.
   *
   * Stores tuples pairing each code statement with its required indentation
   * count.
   */
  std::vector<std::pair<uint32_t, std::string>> mLines;

  /**
   * @brief Container for global variable declarations that must exist before
   * loop execution.
   *
   * Holds all Pin and PWM initializations separately from the main execution
   * flow.
   */
  std::vector<std::string> mGlobalVariables;

  /**
   * @brief Accumulator for all required Python import statements across all
   * operations.
   *
   * Collects unique imports (machine, time) needed for generated code to
   * execute.
   */
  std::vector<std::string> mImports;

  // Constants for formatting and sizing configuration.
  /** @brief Number of spaces representing one indentation level. */
  static constexpr uint8_t INDENT_SIZE = 4;
};

}  // namespace ZanaBlocks::Interpreter
