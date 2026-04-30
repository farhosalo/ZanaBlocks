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
class ChildLoop : public Node {
 public:
  explicit ChildLoop(QGraphicsItem* parent = nullptr);

  /**
   * @brief Creates the ports for the child loop node. This includes one input
   * port at the top and one multi-output port at the bottom. Both ports are
   * initially hidden and can be shown when needed.
   */
  void createPorts() override;
  /**
   *@see Node::getNodeType
   */
  NodeType getNodeType() const override { return NodeType::ChildLoop; }
};
}  // namespace IDE
}  // namespace KidTech