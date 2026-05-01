#pragma once
#include "Node.h"

namespace KidTech {

namespace Schema {
class Loop;
}

namespace IDE {

/**
 * @brief Represents a child loop node in the visual programming diagram.
 */
class LoopNode : public Node {
 public:
  explicit LoopNode(bool isMainLoop = false, QGraphicsItem* parent = nullptr);

  /**
   * @brief Returns wether it is the main loop or not.
   * @return true if it is the loop main, otherwise false.
   */
  bool isMainLoop() const { return mIsMainLoop; }

  /**
   * @brief Returns the number of iterations.
   * @return The number of iterations.
   */
  uint32_t getIterations() const { return mIterations; }

  /**
   * @brief Sets the number of iterations.
   * @param iterations The number of iterations to be set.
   */
  void setIterations(const uint32_t iterations) { mIterations = iterations; }

 private:
  bool mIsMainLoop{false};
  uint32_t mIterations{0};
};
}  // namespace IDE
}  // namespace KidTech