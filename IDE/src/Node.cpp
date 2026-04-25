#include "Node.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "NodePort.h"

namespace KidTech::IDE {
auto constexpr Width{120.0};
auto constexpr Height{60.0};

Node::Node(const QString& label, QGraphicsItem* parent)
    : QGraphicsItem(parent), mLabel(label) {
  setFlag(QGraphicsItem::ItemIsMovable);
  setFlag(QGraphicsItem::ItemIsSelectable);
  setFlag(QGraphicsItem::ItemSendsGeometryChanges);
  setAcceptHoverEvents(true);

  setCacheMode(QGraphicsItem::DeviceCoordinateCache);
  setZValue(1);

  // Create input port
  {
    auto* port = new NodePort(PortType::SingleConnectorPort, this);
    port->setPos(this->boundingRect().center().x(), this->boundingRect().top());
    port->setVisible(false);
    ports.append(port);
  }
  // Create output port
  {
    auto* port = new NodePort(PortType::MultiConnectorPort, this);
    port->setPos(this->boundingRect().center().x(),
                 this->boundingRect().bottom());
    port->setVisible(false);
    ports.append(port);
  }
}

QRectF Node::boundingRect() const {
  return QRectF(-Width / 2, -Height / 2, Width, Height);
}

void Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                 QWidget* widget) {
  const auto selected = option->state & QStyle::State_Selected;

  // Shadow
  painter->setPen(Qt::NoPen);
  painter->setBrush(QColor(0, 0, 0, 40));
  painter->drawRoundedRect(boundingRect().translated(3, 3), 8, 8);

  // Body
  QColor fill = selected ? mColor.lighter(130) : mColor;
  painter->setBrush(fill);
  painter->setPen(QPen(fill.darker(140), selected ? 2.5 : 1.5));
  painter->drawRoundedRect(boundingRect(), 8, 8);

  // Label
  painter->setPen(Qt::white);
  painter->setFont(QFont("Arial", 14, QFont::Bold));
  painter->drawText(boundingRect(), Qt::AlignCenter | Qt::TextWordWrap, mLabel);
}
void Node::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
  showPorts(true);
  QGraphicsItem::hoverEnterEvent(event);
}
void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
  showPorts(false);
  QGraphicsItem::hoverLeaveEvent(event);
}

// TODO: only valid ports should be shown. The already connected ports should be
// hidden, and only the compatible ports should be shown when dragging a
// connection.

void Node::hidePorts() {
  for (auto* port : ports) {
    port->setVisible(false);
  }
}

void Node::showInputPorts() {
  for (auto* port : ports) {
    if (port->getType() == PortType::Input && port->connections().isEmpty()) {
      port->setVisible(true);
    }
  }
}

void Node::showOutputPorts() {
  for (auto* port : ports) {
    if (port->getType() == PortType::MultiOutput ||
        (port->getType() == PortType::SingleOutput &&
         port->connections().isEmpty())) {
      port->setVisible(true);
    }
  }
}

QVariant Node::itemChange(const GraphicsItemChange change,
                          const QVariant& value) {
  if (change == ItemPositionHasChanged) {
    // TODO: Save the new position to the model
    for (auto* port : ports) {
      for (auto* conn : port->connections()) {
        conn->updatePath();
      }
    }
  }
  return QGraphicsItem::itemChange(change, value);
}
}  // namespace KidTech::IDE