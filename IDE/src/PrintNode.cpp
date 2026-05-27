#include "PrintNode.h"

namespace ZanaBlocks::IDE {

PrintNode::PrintNode(std::string message, QGraphicsItem* parent)
    : BaseNodeSchema<Schema::Print>("🖨️", parent),
      mMessage(std::move(message)) {
  addInputPort();
  addHint();
}
}  // namespace ZanaBlocks::IDE