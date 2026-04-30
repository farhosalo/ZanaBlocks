#include "MainLoop.h"

namespace KidTech::IDE {
MainLoop::MainLoop(QGraphicsItem* parent) : Node("🔄", parent) {
  createPorts();
}
void MainLoop::createPorts() {
  auto* port = new NodePort(PortType::MultiOutput, this);
  port->setPos(this->boundingRect().center().x(),
               this->boundingRect().bottom());
  port->setVisible(false);
  ports.append(port);
}
}  // namespace KidTech::IDE