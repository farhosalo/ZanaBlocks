#include "NodePort.h"

#include "Node.h"
#include "Scene.h"

constexpr QRect BoundingRect{-5, -5, 10, 10};

namespace ZanaBlocks::IDE {
NodePort::NodePort(PORT_TYPE type, QGraphicsItem* parent)
    : QGraphicsEllipseItem(BoundingRect, parent), mType(type) {
  setBrush(Qt::darkGray);
  setAcceptHoverEvents(true);
  setFlag(ItemSendsScenePositionChanges);
}

Node* NodePort::parentNode() const { return dynamic_cast<Node*>(parentItem()); }

void NodePort::hoverEnterEvent(QGraphicsSceneHoverEvent* /*event*/) {
  setBrush(Qt::green);  // highlight
}

void NodePort::hoverLeaveEvent(QGraphicsSceneHoverEvent* /*event*/) {
  setBrush(Qt::darkGray);
}
void NodePort::mousePressEvent(QGraphicsSceneMouseEvent* /*event*/) {
  if (auto* diagramScene = dynamic_cast<Scene*>(scene())) {
    diagramScene->beginConnection(this);
  }
}
}  // namespace ZanaBlocks::IDE