#include "LoopNode.h"

namespace KidTech::IDE {
LoopNode::LoopNode(bool isMainLoop, QGraphicsItem* parent)
    : BaseNodeSchema<Schema::Loop>("🔄", parent), mIsMainLoop(isMainLoop) {
  if (!isMainLoop) {
    addInputPort();
  }
  addMultiOutputPort();
  addHint();
}
}  // namespace KidTech::IDE
