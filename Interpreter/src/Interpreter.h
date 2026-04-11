#pragma once
#include "Schema.pb.h"

namespace KidTech::Interpreter {

/**
 * The Interpreter class is responsible for interpreting a given schema and
 * generating corresponding code lines. It supports interpreting loops, print
 * statements, sleep and other actions defined in the schema. The generated
 * lines can be saved to a file for further use.
 */
class Interpreter {
  using Lines = std::vector<std::pair<uint32_t, std::string>>;
  static auto constexpr indentSize{4};

 public:
  /**
   * Interprets the given schema and generates corresponding code lines. Returns
   * true if the interpretation is successful, false otherwise.
   * @param schema The schema to interpret
   * @return true if interpretation is successful, false otherwise
   */
  bool interpret(const std::shared_ptr<Schema::Root>& schema);

  /**
   * Saves the generated code lines to a file.
   * @param filename The name of the file to save to
   * @return true if saving is successful, false otherwise
   */
  bool saveToFile(const std::string& filename);

 private:
  bool extractLoop(const Schema::Loop& loop);
  auto extractPrint(const Schema::Print& print);
  auto extractSleep(const Schema::Sleep& sleep);
  auto extractLed(const Schema::LED& led);

  void reset();
  int mCurrentLevel{0};
  Lines mLines;
};
}  // namespace KidTech::Interpreter