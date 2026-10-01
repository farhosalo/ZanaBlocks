#include <filesystem>

#include "InterpreterTest.h"

const std::string FILENAME =
    (std::filesystem::temp_directory_path() / "loop_test.txt").string();

namespace ZanaBlocks::Interpreter {

std::shared_ptr<Root> createLoop(uint32_t count, bool withPrint) {
  auto schema = std::make_shared<Root>();
  auto* loop = schema->mutable_mainloop()->add_actions()->mutable_loop();

  loop->set_count(count);

  if (withPrint) {
    loop->add_actions()->mutable_print()->set_msg("TestLoop");
  }

  return schema;
}

TEST_F(InterpreterTest, ValidFiniteLoop) {
  schema_ = createLoop(5, true);

  ASSERT_TRUE(interpret());

  EXPECT_TRUE(save(FILENAME));

  // Verify output contains correct print statement
  std::ifstream outfile(FILENAME);
  std::string line((std::istreambuf_iterator<char>(outfile)),
                   std::istreambuf_iterator<char>());

  EXPECT_TRUE(checkMainLoop(line));
  EXPECT_EQ(findPattern(line, "    for i in range(5):"), 1);
  EXPECT_EQ(findPattern(line, "         print(\"TestLoop\")"), 1);
}
TEST_F(InterpreterTest, ValidInfiniteLoop) {
  schema_ = createLoop(0, true);

  ASSERT_TRUE(interpret());

  EXPECT_TRUE(save(FILENAME));

  // Verify output contains correct print statement
  std::ifstream outfile(FILENAME);
  std::string line((std::istreambuf_iterator<char>(outfile)),
                   std::istreambuf_iterator<char>());

  EXPECT_TRUE(checkMainLoop(line));
  EXPECT_EQ(findPattern(line, "    while True:"), 1);
  EXPECT_EQ(findPattern(line, "         print(\"TestLoop\")"), 1);
}

TEST_F(InterpreterTest, InvalidFiniteLoop) {
  schema_ = createLoop(5, false);

  EXPECT_FALSE(interpret());

  EXPECT_FALSE(save(FILENAME));
}
TEST_F(InterpreterTest, InvalidInfiniteLoop) {
  schema_ = createLoop(0, false);

  EXPECT_FALSE(interpret());

  EXPECT_FALSE(save(FILENAME));
}
}  // namespace ZanaBlocks::Interpreter