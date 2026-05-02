#include "LedNode.h"

namespace KidTech::IDE {

LedNode::LedNode(const bool state, QGraphicsItem* parent)
    : Node("💡", parent), mState(state) {
  addInputPort();
}
}  // namespace KidTech::IDE