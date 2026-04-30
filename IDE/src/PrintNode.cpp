#include "PrintNode.h"

namespace KidTech::IDE {

PrintNode::PrintNode(const std::string& message, QGraphicsItem* parent)
    : Node("🖨️", parent), mMessage(message) {
  createPorts();
}
void PrintNode::createPorts() {
  auto* port = new NodePort(PortType::Input, this);
  port->setPos(this->boundingRect().center().x(), this->boundingRect().top());
  port->setVisible(false);
  ports.append(port);
}
}  // namespace KidTech::IDE