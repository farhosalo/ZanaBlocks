#include "LedNode.h"

namespace ZanaBlocks::IDE {

LedNode::LedNode(const bool state, QGraphicsItem* parent)
    : BaseNodeSchema<Schema::LED>("💡", parent), mState(state) {
  addInputPort();
  addHint();
}
}  // namespace ZanaBlocks::IDE