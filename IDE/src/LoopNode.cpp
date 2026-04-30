#include "LoopNode.h"

namespace KidTech::IDE {
LoopNode::LoopNode(bool isMainLoop, QGraphicsItem* parent)
    : Node("🔄", parent), mIsMainLoop(isMainLoop) {
  createPorts();
}

void LoopNode::createPorts() {
  // Create input port
  if (!mIsMainLoop) {
    auto* port = new NodePort(PortType::Input, this);
    port->setPos(this->boundingRect().center().x(), this->boundingRect().top());
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
}  // namespace KidTech::IDE
