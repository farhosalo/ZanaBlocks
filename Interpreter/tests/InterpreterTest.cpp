#include "InterpreterTest.h"

#include <filesystem>

const std::string FILENAME =
    (std::filesystem::temp_directory_path() / "interpreter_test.txt").string();

namespace ZanaBlocks::Interpreter {

TEST_F(InterpreterTest, InterpretNullSchema) {
  schema_.reset();
  EXPECT_FALSE(interpret()) << "Null schema should fail interpretation";
  EXPECT_FALSE(save("abc"));
}

TEST_F(InterpreterTest, InterpretWithoutMainloop) { EXPECT_FALSE(interpret()); }
TEST_F(InterpreterTest, InterpretEmptyMainloop) {
  schema_->mutable_mainloop();
  EXPECT_FALSE(interpret());
}
TEST_F(InterpreterTest, InterpretInvalidAction) {
  schema_->mutable_mainloop()->add_actions();
  EXPECT_FALSE(interpret());
}

TEST_F(InterpreterTest, SaveToEmptyFilename) {
  schema_ = createValidSchema();

  ASSERT_TRUE(interpret());
  EXPECT_FALSE(save("")) << "Empty FILENAME should fail";
}

TEST_F(InterpreterTest, SaveToFileInvalidPath) {
  schema_ = createValidSchema();

  ASSERT_TRUE(interpret());

  const std::string fakeFilename =
      "/nonexistent/path/that/cannot/be/written.txt";

  EXPECT_FALSE(save(fakeFilename)) << "Invalid path should fail to write";
}

TEST_F(InterpreterTest, MainLoopWithValidAndInvalidAction) {
  schema_ = createValidSchema();
  schema_->mutable_mainloop()->add_actions()->mutable_print();

  ASSERT_FALSE(interpret());

  EXPECT_FALSE(save(FILENAME));
}

TEST_F(InterpreterTest, FiniteMainLoop) {
  schema_ = createValidSchema();
  schema_->mutable_mainloop()->set_count(4);

  ASSERT_TRUE(interpret());

  EXPECT_TRUE(save(FILENAME))
      << "Should save successfully after interpretation";

  // Verify output contains correct print statement
  std::ifstream outfile(FILENAME);
  std::string line((std::istreambuf_iterator<char>(outfile)),
                   std::istreambuf_iterator<char>());

  EXPECT_TRUE(findPattern(line, "for i in range(4)") == 1);

  EXPECT_TRUE(findPattern(line, "print(\"TestPrint\")") == 1);
}

}  // namespace ZanaBlocks::Interpreter
