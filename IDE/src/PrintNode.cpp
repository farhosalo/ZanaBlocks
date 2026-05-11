#include "PrintNode.h"

namespace ZanaBlocks::IDE {

PrintNode::PrintNode(const std::string& message, QGraphicsItem* parent)
    : BaseNodeSchema<Schema::Print>("🖨️", parent), mMessage(message) {
  addInputPort();
  addHint();
}
}  // namespace ZanaBlocks::IDE