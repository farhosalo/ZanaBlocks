#include <filesystem>

#include "InterpreterTest.h"

const std::string FILENAME =
    (std::filesystem::temp_directory_path() / "sleep_test.txt").string();

namespace ZanaBlocks::Interpreter {

std::shared_ptr<Root> createSleep(int32_t duration) {
  auto schema = std::make_shared<Root>();
  auto* sleep = schema->mutable_mainloop()->add_actions()->mutable_sleep();

  if (duration >= 0) {
    sleep->set_duration(static_cast<uint32_t>(duration));
  }
  return schema;
}

TEST_F(InterpreterTest, ValidSleepDuration) {
  schema_ = createSleep(50);

  ASSERT_TRUE(interpret());

  EXPECT_TRUE(save(FILENAME));

  // Verify output contains correct print statement
  std::ifstream outfile(FILENAME);
  std::string line((std::istreambuf_iterator<char>(outfile)),
                   std::istreambuf_iterator<char>());

  EXPECT_TRUE(checkMainLoop(line));
  EXPECT_EQ(findPattern(line, "from time import sleep_ms"), 1);
  EXPECT_EQ(findPattern(line, "    sleep_ms(50)"), 1);
}
TEST_F(InterpreterTest, InvalidSleepDuration) {
  schema_ = createSleep(0);

  EXPECT_FALSE(interpret());
  EXPECT_FALSE(save(FILENAME));
}
TEST_F(InterpreterTest, NoSleepDuration) {
  schema_ = createSleep(-1);

  EXPECT_FALSE(interpret());
  EXPECT_FALSE(save(FILENAME));
}
}  // namespace ZanaBlocks::Interpreter