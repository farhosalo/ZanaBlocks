#include "Node.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "NodePort.h"
#include "NodeSettings.h"

namespace ZanaBlocks::IDE {
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
  QColor fill;
  fill = selected ? mColor.lighter(130) : mColor;
  painter->setBrush(fill);
  painter->setPen(QPen(fill.darker(140), selected ? 2.5 : 1.5));
  painter->drawRoundedRect(boundingRect(), 8, 8);

  // Label
  painter->setPen(Qt::white);
  painter->setFont(QFont("Arial", 18, QFont::Bold));
  painter->drawText(boundingRect(), Qt::AlignCenter | Qt::TextWordWrap, mLabel);
}
void Node::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
  showOutputPorts();
  QGraphicsItem::hoverEnterEvent(event);
}
void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
  hidePorts();
  QGraphicsItem::hoverLeaveEvent(event);
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
  NodeSettings dialog(getDescription(), schema());
  if (dialog.exec() == QDialog::Accepted) onSchemaChanged();

  QGraphicsItem::mouseDoubleClickEvent(event);
}

void Node::addInputPort() {
  auto* port = new NodePort(PortType::Input, this);
  port->setPos(this->boundingRect().center().x(), this->boundingRect().top());
  port->setVisible(false);
  ports.append(port);
}

void Node::addSingleOutputPort() {
  auto* port = new NodePort(PortType::SingleOutput, this);
  port->setPos(this->boundingRect().center().x(),
               this->boundingRect().bottom());
  port->setVisible(false);
  ports.append(port);
}

void Node::addMultiOutputPort() {
  auto* port = new NodePort(PortType::MultiOutput, this);
  port->setPos(this->boundingRect().center().x(),
               this->boundingRect().bottom());
  port->setVisible(false);
  ports.append(port);
}

void Node::addHint() {
  QString hint =
      QString::fromUtf8(getDescription().data(), getDescription().size());
  setToolTip("<div style='max-width:500px;'>" + hint + "</div>");
}

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
}  // namespace ZanaBlocks::IDE