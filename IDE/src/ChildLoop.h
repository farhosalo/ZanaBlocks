#pragma once
#include "Node.h"

namespace KidTech::IDE {

/**
 * @brief Represents a child loop node in the visual programming diagram.
 */
class ChildLoop : public Node {
 public:
  explicit ChildLoop(QGraphicsItem* parent = nullptr)
      : Node("Child Loop", parent) {}

  /**
   * @brief Creates the ports for the child loop node. This includes one input
   * port at the top and one multi-output port at the bottom. Both ports are
   * initially hidden and can be shown when needed.
   */
  void createPorts() override {
    // Create input port
    {
      auto* port = new NodePort(PortType::Input, this);
      port->setPos(this->boundingRect().center().x(),
                   this->boundingRect().top());
      port->setVisible(false);
      ports.append(port);
    }
    // Create output port
    {
      auto* port = new NodePort(PortType::MultiOutput, this);
      port->setPos(this->boundingRect().center().x(),
                   this->boundingRect().bottom());
      port->setVisible(false);
      ports.append(port);
    }
  }
};
}  // namespace KidTech::IDE