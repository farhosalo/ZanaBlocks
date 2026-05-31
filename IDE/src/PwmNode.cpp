#include "PwmNode.h"

namespace ZanaBlocks::IDE {

PwmNode::PwmNode(QGraphicsItem* parent)
    : BaseNodeSchema<Schema::Pwm>("⚡", parent) {
  addInputPort();
  addHint();
}
}  // namespace ZanaBlocks::IDE