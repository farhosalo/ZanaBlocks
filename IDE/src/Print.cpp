#include "Print.h"

namespace KidTech::IDE {

Print::Print(QGraphicsItem* parent) : Node("🖨️", parent) { createPorts(); }
void Print::createPorts() {
  auto* port = new NodePort(PortType::Input, this);
  port->setPos(this->boundingRect().center().x(), this->boundingRect().top());
  port->setVisible(false);
  ports.append(port);
}
}  // namespace KidTech::IDE