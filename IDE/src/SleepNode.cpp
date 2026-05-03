#include "SleepNode.h"

namespace KidTech::IDE {

SleepNode::SleepNode(const uint32_t sleepTime, QGraphicsItem* parent)
    : BaseNodeSchema<Schema::Sleep>("⏰", parent), mSleepTime(sleepTime) {
  addInputPort();
  addHint();
}
}  // namespace KidTech::IDE