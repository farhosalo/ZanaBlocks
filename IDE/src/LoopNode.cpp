#include "LoopNode.h"

namespace ZanaBlocks::IDE {
LoopNode::LoopNode(bool isMainLoop, QGraphicsItem* parent)
    : BaseNodeSchema<Schema::Loop>("🔄", parent), mIsMainLoop(isMainLoop) {
  if (!isMainLoop) {
    addInputPort();
  }
  addMultiOutputPort();
  addHint();
}
}  // namespace ZanaBlocks::IDE
