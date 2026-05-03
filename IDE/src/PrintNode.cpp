#include "PrintNode.h"

namespace KidTech::IDE {

PrintNode::PrintNode(const std::string& message, QGraphicsItem* parent)
    : Node("🖨️", parent), mMessage(message) {
  addInputPort();
  addHint();
}
}  // namespace KidTech::IDE