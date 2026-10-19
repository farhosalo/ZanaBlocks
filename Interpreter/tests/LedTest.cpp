#include <filesystem>

#include "InterpreterTest.h"
namespace ZanaBlocks::Interpreter {

const std::string FILENAME =
    (std::filesystem::temp_directory_path() / "led_test.txt").string();

std::shared_ptr<Root> createLedSchema(int pin = -1, int state = -1) {
  auto schema = std::make_shared<Root>();
  auto* led = schema->mutable_mainloop()->add_actions()->mutable_led();

  if (pin > 0) {
    led->set_pin(static_cast<uint32_t>(pin));
  }
  if (state >= 0) {
    led->set_state(state > 0);
  }
  return schema;
}
TEST_F(InterpreterTest, LedWithValidPinAndSwitchedOn) {
  schema_ = createLedSchema(12, 5);

  ASSERT_TRUE(interpret());

  EXPECT_TRUE(save(FILENAME));

  // Verify output contains correct print statement
  std::ifstream outfile(FILENAME);
  std::string line((std::istreambuf_iterator<char>(outfile)),
                   std::istreambuf_iterator<char>());

  EXPECT_TRUE(checkMainLoop(line));
  EXPECT_EQ(findPattern(line, "from machine import Pin"), 1);
  EXPECT_EQ(findPattern(line, "led_12 = Pin(12, Pin.OUT)"), 1);
  EXPECT_EQ(findPattern(line, "   led_12.value(1)"), 1);
}

TEST_F(InterpreterTest, LedWithValidPinAndSwitchedOff) {
  schema_ = createLedSchema(13, 0);

  ASSERT_TRUE(interpret());

  EXPECT_TRUE(save(FILENAME));

  std::ifstream outfile(FILENAME);
  std::string line((std::istreambuf_iterator<char>(outfile)),
                   std::istreambuf_iterator<char>());

  EXPECT_TRUE(checkMainLoop(line));
  EXPECT_EQ(findPattern(line, "from machine import Pin"), 1);
  EXPECT_EQ(findPattern(line, "led_13 = Pin(13, Pin.OUT)"), 1);
  EXPECT_EQ(findPattern(line, "   led_13.value(0)"), 1);
}

TEST_F(InterpreterTest, MultipleLeds) {
  schema_ = createLedSchema(13, 0);
  auto* led2 = schema_->mutable_mainloop()->add_actions()->mutable_led();
  led2->set_pin(14);
  led2->set_state(1);

  ASSERT_TRUE(interpret());

  EXPECT_TRUE(save(FILENAME));

  std::ifstream outfile(FILENAME);
  std::string line((std::istreambuf_iterator<char>(outfile)),
                   std::istreambuf_iterator<char>());

  EXPECT_TRUE(checkMainLoop(line));
  EXPECT_EQ(findPattern(line, "from machine import Pin"), 1);
  EXPECT_EQ(findPattern(line, "led_13 = Pin(13, Pin.OUT)"), 1);
  EXPECT_EQ(findPattern(line, "led_14 = Pin(14, Pin.OUT)"), 1);
  EXPECT_EQ(findPattern(line, "   led_13.value(0)"), 1);
  EXPECT_EQ(findPattern(line, "   led_14.value(1)"), 1);
}
TEST_F(InterpreterTest, SwitchLedOnAndThenOff) {
  schema_ = createLedSchema(13, 1);

  // Switch off
  auto led = schema_->mutable_mainloop()->add_actions()->mutable_led();
  led->set_state(0);
  led->set_pin(13);

  ASSERT_TRUE(interpret());

  EXPECT_TRUE(save(FILENAME));

  std::ifstream outfile(FILENAME);
  std::string line((std::istreambuf_iterator<char>(outfile)),
                   std::istreambuf_iterator<char>());

  EXPECT_TRUE(checkMainLoop(line));
  EXPECT_EQ(findPattern(line, "from machine import Pin"), 1);
  EXPECT_EQ(findPattern(line, "led_13 = Pin(13, Pin.OUT)"), 1);
  EXPECT_EQ(findPattern(line, "   led_13.value(0)"), 1);
  EXPECT_EQ(findPattern(line, "   led_13.value(1)"), 1);
}
TEST_F(InterpreterTest, LedNoPinSet) {
  schema_ = createLedSchema(-1, 0);

  ASSERT_FALSE(interpret());

  EXPECT_FALSE(save(FILENAME));
}
TEST_F(InterpreterTest, LedPinOutOfRange) {
  schema_ = createLedSchema(MAX_PINS + 1, 0);

  ASSERT_FALSE(interpret());

  EXPECT_FALSE(save(FILENAME));
}
}  // namespace ZanaBlocks::Interpreter