#include <filesystem>

#include "InterpreterTest.h"

const std::string FILENAME =
    (std::filesystem::temp_directory_path() / "print_test.txt").string();
namespace ZanaBlocks::Interpreter {

TEST_F(InterpreterTest, PrintWithValidMessage) {
  schema_ = createValidSchema();

  ASSERT_TRUE(interpret()) << "Should successfully interpret print action";

  EXPECT_TRUE(save(FILENAME))
      << "Should save successfully after interpretation";

  // Verify output contains correct print statement
  std::ifstream outfile(FILENAME);
  std::string line((std::istreambuf_iterator<char>(outfile)),
                   std::istreambuf_iterator<char>());

  EXPECT_TRUE(checkMainLoop(line));
  EXPECT_TRUE(findPattern(line, "print(\"TestPrint\")") == 1)
      << "Output should contain print(\\\"TestPrint\\\") statement";
}
TEST_F(InterpreterTest, PrintWithInvalidMessage) {
  auto* print = schema_->mutable_mainloop()->add_actions()->mutable_print();
  print->set_msg("");
  EXPECT_FALSE(interpret());
  EXPECT_FALSE(save(FILENAME));
}
}  // namespace ZanaBlocks::Interpreter