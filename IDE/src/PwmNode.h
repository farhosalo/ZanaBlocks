#pragma once

#include "BaseNodeSchema.h"
#include "Schema.pb.h"

namespace ZanaBlocks::IDE {
/**
 * @brief Represents the pwm node in the visual programming diagram.
 */
class PwmNode : public BaseNodeSchema<Schema::Pwm> {
 public:
  explicit PwmNode(QGraphicsItem* parent = nullptr);

  /**
   * \see Node::getDescription
   */
  std::string_view getDescription() const override {
    return "A control node that allows you to control the pulse width of a PWM "
           "signal. The signal is determined by the pwm field and the duty "
           "cycle is determined by the duty.";
  }
};
}  // namespace ZanaBlocks::IDE
