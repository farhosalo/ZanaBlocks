#pragma once

#include "Node.h"

namespace KidTech::IDE {
/**
 * @brief Represents the print node in the visual programming diagram.
 */
class PrintNode : public Node {
 public:
  explicit PrintNode(const std::string& message = "",
                     QGraphicsItem* parent = nullptr);

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

 private:
  std::string mMessage;
};
}  // namespace KidTech::IDE
