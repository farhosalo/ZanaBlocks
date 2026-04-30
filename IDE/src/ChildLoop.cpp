#include "ChildLoop.h"

namespace KidTech::IDE {
ChildLoop::ChildLoop(QGraphicsItem* parent) : Node("🔄", parent) {
  createPorts();
}

void ChildLoop::createPorts() {
  // Create input port
  {
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
