#include "NodePort.h"

#include "Node.h"
#include "Scene.h"

namespace KidTech::IDE {
NodePort::NodePort(PortType type, QGraphicsItem* parent)
    : QGraphicsEllipseItem(-5, -5, 10, 10, parent), mType(type) {
  setBrush(Qt::darkGray);
  setAcceptHoverEvents(true);
  setFlag(ItemSendsScenePositionChanges);
}

Node* NodePort::parentNode() const { return dynamic_cast<Node*>(parentItem()); }

void NodePort::hoverEnterEvent(QGraphicsSceneHoverEvent*) {
  setBrush(Qt::green);  // highlight
}

void NodePort::hoverLeaveEvent(QGraphicsSceneHoverEvent*) {
  setBrush(Qt::darkGray);
}
void NodePort::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  if (auto* diagramScene = dynamic_cast<Scene*>(scene())) {
    diagramScene->beginConnection(this);
  }
}
}  // namespace KidTech::IDE