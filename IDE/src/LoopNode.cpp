#include "LoopNode.h"

namespace KidTech::IDE {
LoopNode::LoopNode(bool isMainLoop, QGraphicsItem* parent)
    : Node("🔄", parent), mIsMainLoop(isMainLoop) {
  if (!isMainLoop) {
    addInputPort();
  }
  addMultiOutputPort();
}
}  // namespace KidTech::IDE
