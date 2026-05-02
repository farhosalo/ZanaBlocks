#pragma once

#include "Node.h"

namespace KidTech::IDE {
/**
 * @brief Represents the LED node on visual programming diagram.
 */
class LedNode : public Node {
 public:
  explicit LedNode(const bool state = false, QGraphicsItem* parent = nullptr);

  /**
   * @brief Returns the LED state.
   * @return The LED state.
   */
  bool getState() const { return mState; }

  /**
   * @brief Sets the LED state.
   * @param state The LED state to be set.
   */
  void setState(const bool state) { mState = state; }

 private:
  bool mState;
};
}  // namespace KidTech::IDE
