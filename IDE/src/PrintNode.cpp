#include "PrintNode.h"

namespace KidTech::IDE {

PrintNode::PrintNode(const std::string& message, QGraphicsItem* parent)
    : BaseNodeSchema<Schema::Print>("🖨️", parent), mMessage(message) {
  addInputPort();
  addHint();
}
}  // namespace KidTech::IDE