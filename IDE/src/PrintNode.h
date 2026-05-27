#pragma once

#include "BaseNodeSchema.h"
#include "Schema.pb.h"

namespace ZanaBlocks::IDE {
/**
 * @brief Represents the print node in the visual programming diagram.
 */
class PrintNode : public BaseNodeSchema<Schema::Print> {
 public:
  explicit PrintNode(std::string message = "", QGraphicsItem* parent = nullptr);

  /**
   * @brief Returns the message to be printed.
   * @return The message to be printed.
   */
  const std::string& getMessage() const { return mMessage; }

  /**
   * @brief Sets the message to be printed.
   * @param message The message to be set.
   */
  void setMessage(const std::string& message) { mMessage = message; }

  /**
   * \see Node::getDescription
   */
  std::string_view getDescription() const override {
    return "A control node that allows you to print a message to the console. "
           "the message is determined by the msg field.";
  }

 private:
  std::string mMessage;
};
}  // namespace ZanaBlocks::IDE
