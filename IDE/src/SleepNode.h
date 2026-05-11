#pragma once

#include "BaseNodeSchema.h"
#include "Schema.pb.h"

namespace ZanaBlocks::IDE {
/**
 * @brief Represents the sleep node on visual programming diagram.
 */
class SleepNode : public BaseNodeSchema<Schema::Sleep> {
 public:
  explicit SleepNode(const uint32_t sleepTime = 0,
                     QGraphicsItem* parent = nullptr);

  /**
   * @brief Returns the sleep time.
   * @return The sleep time.
   */
  uint32_t getSleepTime() const { return mSleepTime; }

  /**
   * @brief Sets the sleep time.
   * @param message The sleep time to be set.
   */
  void setSleepTime(const uint32_t sleepTime) { mSleepTime = sleepTime; }

  /**
   * \see Node::getDescription
   */
  std::string_view getDescription() const override {
    return "A control node that pauses the execution of the program for a "
           "specified duration in milliseconds.";
  }

 private:
  uint32_t mSleepTime;
};
}  // namespace ZanaBlocks::IDE
