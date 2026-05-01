#include "PrintNode.h"

namespace KidTech::IDE {

PrintNode::PrintNode(const std::string& message, QGraphicsItem* parent)
    : Node("🖨️", parent), mMessage(message) {
  addInputPort();
}
}  // namespace KidTech::IDE