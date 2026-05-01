#include "SleepNode.h"

namespace KidTech::IDE {

SleepNode::SleepNode(const uint32_t sleepTime, QGraphicsItem* parent)
    : Node("⏰", parent), mSleepTime(sleepTime) {
  createPorts();
}
void SleepNode::createPorts() {
  auto* port = new NodePort(PortType::Input, this);
  port->setPos(this->boundingRect().center().x(), this->boundingRect().top());
  port->setVisible(false);
  ports.append(port);
}
}  // namespace KidTech::IDE