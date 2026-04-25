#pragma once

#include "Node.h"

namespace KidTech::IDE {

/**
 * @brief Represents the main loop node in the visual programming diagram.
 */
class MainLoop : public Node {
 public:
  explicit MainLoop(QGraphicsItem* parent = nullptr)
      : Node("Main Loop", parent) {}

  /**
   * @brief Creates the ports for the main loop node. This includes one
   * multi-output port at the bottom. Main loop has no input ports, as it is the
   * entry point of the program. The port is initially hidden and can be shown
   * when needed.
   */
  void createPorts() override {
    auto* port = new NodePort(PortType::MultiOutput, this);
    port->setPos(this->boundingRect().center().x(),
                 this->boundingRect().bottom());
    port->setVisible(false);
    ports.append(port);
  }
};
}  // namespace KidTech::IDE