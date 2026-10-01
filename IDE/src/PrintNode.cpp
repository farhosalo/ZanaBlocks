#include "PrintNode.h"

namespace ZanaBlocks::IDE {

PrintNode::PrintNode(std::string message, QGraphicsItem* parent)
    : BaseNodeSchema<Schema::Print>("\U0001F5A8\uFE0F", parent),
      mMessage(std::move(message)) {
  addInputPort();
  addHint();
}
}  // namespace ZanaBlocks::IDE