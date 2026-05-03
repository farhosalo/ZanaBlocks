#include "LedNode.h"

namespace KidTech::IDE {

LedNode::LedNode(const bool state, QGraphicsItem* parent)
    : BaseNodeSchema<Schema::LED>("💡", parent), mState(state) {
  addInputPort();
  addHint();
}
}  // namespace KidTech::IDE