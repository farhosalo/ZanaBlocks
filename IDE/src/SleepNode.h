#pragma once

#include "Node.h"

namespace KidTech::IDE {
/**
 * @brief Represents the sleep node on visual programming diagram.
 */
class SleepNode : public Node {
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

 private:
  uint32_t mSleepTime;
};
}  // namespace KidTech::IDE
