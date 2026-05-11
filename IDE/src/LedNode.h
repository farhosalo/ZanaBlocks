#pragma once

#include "BaseNodeSchema.h"
#include "Schema.pb.h"

namespace ZanaBlocks::IDE {
/**
 * @brief Represents the LED node on visual programming diagram.
 */
class LedNode : public BaseNodeSchema<Schema::LED> {
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

  /**
   * \see Node::getDescription
   */
  std::string_view getDescription() const override {
    return "A control node that allows you to turn an LED on or off by setting "
           "its state. The pin to which the LED is connected is determined by "
           "the pin field and the state is determined by the state field.";
  }

 private:
  bool mState;
};
}  // namespace ZanaBlocks::IDE
