#pragma once

#include <gtest/gtest.h>

#include <fstream>
#include <memory>
#include <string>

#include "Interpreter.h"
#include "Logging.h"

using namespace ZanaBlocks::Schema;

namespace ZanaBlocks::Interpreter {
inline uint16_t findPattern(const std::string& text,
                            const std::string& pattern) {
  uint16_t count = 0;
  std::size_t pos = 0;
  while ((pos = text.find(pattern, pos)) != std::string::npos) {
    ++count;
    pos += 4;
  }
  return count;
}

inline std::shared_ptr<Root> createValidSchema() {
  auto schema = std::make_shared<Root>();
  auto* print = schema->mutable_mainloop()->add_actions()->mutable_print();
  print->set_msg("TestPrint");

  return schema;
}

inline bool checkMainLoop(const std::string& line) {
  return line.find("while True:") != std::string::npos;
}

class InterpreterTest : public ::testing::Test {
 protected:
  void SetUp() override {
    interpreter_ = std::make_shared<Interpreter>();
    schema_ = std::make_shared<Root>();
  }

  void TearDown() override {
    interpreter_.reset();
    schema_.reset();
  }
  bool interpret() { return interpreter_->interpret(schema_); }

  bool save(const std::string& file) { return interpreter_->saveToFile(file); }

  std::shared_ptr<Interpreter> interpreter_;
  std::shared_ptr<Schema::Root> schema_;
};
}  // namespace ZanaBlocks::Interpreter