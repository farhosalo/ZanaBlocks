#include <filesystem>

#include "InterpreterTest.h"

namespace ZanaBlocks::Interpreter {
const std::string FILENAME =
    (std::filesystem::temp_directory_path() / "pwm_test.txt").string();

std::shared_ptr<Root> createPwmSchema(int pin, int duty, int freq) {
  auto schema = std::make_shared<Root>();
  auto* pwm = schema->mutable_mainloop()->add_actions()->mutable_pwm();

  if (pin > 0) {
    pwm->set_pin(static_cast<uint32_t>(pin));
  }
  if (duty >= 0) {
    pwm->set_duty(static_cast<uint32_t>(duty));
  }
  if (freq >= 0) {
    pwm->set_frequency(static_cast<uint32_t>(freq));
  }
  return schema;
}
TEST_F(InterpreterTest, ValidPwm) {
  schema_ = createPwmSchema(15, 5, 120);

  ASSERT_TRUE(interpret());

  EXPECT_TRUE(save(FILENAME));

  // Verify output contains correct print statement
  std::ifstream outfile(FILENAME);
  std::string line((std::istreambuf_iterator<char>(outfile)),
                   std::istreambuf_iterator<char>());

  EXPECT_TRUE(checkMainLoop(line));
  EXPECT_EQ(findPattern(line, "from machine import PWM"), 1);
  EXPECT_EQ(findPattern(line, "pwm_15 = PWM(Pin(15))"), 1);
  EXPECT_EQ(findPattern(line, "   pwm_15.duty(5)"), 1);
  EXPECT_EQ(findPattern(line, "   pwm_15.freq(120)"), 1);
}
TEST_F(InterpreterTest, MultipleValidPwm) {
  schema_ = createPwmSchema(15, 5, 120);
  auto* pwm2 = schema_->mutable_mainloop()->add_actions()->mutable_pwm();

  pwm2->set_pin(16);
  pwm2->set_duty(234);
  pwm2->set_frequency(543);

  ASSERT_TRUE(interpret());

  EXPECT_TRUE(save(FILENAME));

  // Verify output contains correct print statement
  std::ifstream outfile(FILENAME);
  std::string line((std::istreambuf_iterator<char>(outfile)),
                   std::istreambuf_iterator<char>());

  EXPECT_TRUE(checkMainLoop(line));

  // Imports
  EXPECT_EQ(findPattern(line, "from machine import PWM"), 1);

  // vars
  EXPECT_EQ(findPattern(line, "pwm_15 = PWM(Pin(15))"), 1);
  EXPECT_EQ(findPattern(line, "pwm_16 = PWM(Pin(16))"), 1);

  // Main loop actions
  EXPECT_EQ(findPattern(line, "   pwm_15.duty(5)"), 1);
  EXPECT_EQ(findPattern(line, "   pwm_15.freq(120)"), 1);

  EXPECT_EQ(findPattern(line, "   pwm_16.duty(234)"), 1);
  EXPECT_EQ(findPattern(line, "   pwm_16.freq(543)"), 1);
}

TEST_F(InterpreterTest, ValidPwmChangeDuty) {
  schema_ = createPwmSchema(15, 5, 120);
  auto* pwm = schema_->mutable_mainloop()->add_actions()->mutable_pwm();

  pwm->set_pin(15);
  pwm->set_duty(5);
  pwm->set_frequency(543);

  ASSERT_TRUE(interpret());

  EXPECT_TRUE(save(FILENAME));

  // Verify output contains correct print statement
  std::ifstream outfile(FILENAME);
  std::string line((std::istreambuf_iterator<char>(outfile)),
                   std::istreambuf_iterator<char>());

  EXPECT_TRUE(checkMainLoop(line));
  EXPECT_EQ(findPattern(line, "from machine import PWM"), 1);
  EXPECT_EQ(findPattern(line, "pwm_15 = PWM(Pin(15))"), 1);
  EXPECT_EQ(findPattern(line, "   pwm_15.duty(5)"), 2);
  EXPECT_EQ(findPattern(line, "   pwm_15.freq(120)"), 1);
  EXPECT_EQ(findPattern(line, "   pwm_15.freq(543)"), 1);
}
TEST_F(InterpreterTest, PwmWithNoPin) {
  schema_ = createPwmSchema(-1, 5, 120);

  EXPECT_FALSE(interpret());

  EXPECT_FALSE(save(FILENAME));
}
TEST_F(InterpreterTest, PwmWithInvalidPin) {
  schema_ = createPwmSchema(0, 5, 120);

  EXPECT_FALSE(interpret());

  EXPECT_FALSE(save(FILENAME));
}
TEST_F(InterpreterTest, PwmWithOutOfRangePin) {
  schema_ = createPwmSchema(MAX_PINS + 1, 5, 120);

  EXPECT_FALSE(interpret());

  EXPECT_FALSE(save(FILENAME));
}
TEST_F(InterpreterTest, PwmWithInvalidDuty) {
  schema_ = createPwmSchema(10, MAX_PWM_DUTY + 1, 120);

  EXPECT_FALSE(interpret());

  EXPECT_FALSE(save(FILENAME));
}
TEST_F(InterpreterTest, PwmWithNoFreq) {
  schema_ = createPwmSchema(10, 120, -1);

  EXPECT_FALSE(interpret());

  EXPECT_FALSE(save(FILENAME));
}
TEST_F(InterpreterTest, PwmWithZeroFreq) {
  schema_ = createPwmSchema(10, 120, 0);

  EXPECT_FALSE(interpret());

  EXPECT_FALSE(save(FILENAME));
}
TEST_F(InterpreterTest, PwmWithInvaldFreq) {
  schema_ = createPwmSchema(10, 120, MAX_PWM_FREQ + 1);

  EXPECT_FALSE(interpret());

  EXPECT_FALSE(save(FILENAME));
}
}  // namespace ZanaBlocks::Interpreter