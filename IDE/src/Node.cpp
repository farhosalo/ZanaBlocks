#include "Node.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "NodePort.h"
#include "NodeSettings.h"

namespace ZanaBlocks::IDE {
auto constexpr Width{120.0};
auto constexpr Height{60.0};
auto constexpr shadowOffset{3.0};
QRectF constexpr BoundingRect{-Width / 2, -Height / 2, Width, Height};
QRectF constexpr RoundedRect{-(Width / 2) + shadowOffset,
                             -(Height / 2) + shadowOffset, Width, Height};

Node::Node(QString label, QGraphicsItem* parent)
    : QGraphicsItem(parent), mLabel(std::move(label)) {
  setFlag(QGraphicsItem::ItemIsMovable);
  setFlag(QGraphicsItem::ItemIsSelectable);
  setFlag(QGraphicsItem::ItemSendsGeometryChanges);
  setAcceptHoverEvents(true);

  setCacheMode(QGraphicsItem::DeviceCoordinateCache);
  setZValue(1);
}

QRectF Node::boundingRect() const {
  return BoundingRect;  // QRectF(-Width / 2, -Height / 2, Width, Height);
}

void Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                 QWidget* /*widget*/) {
  const bool selected = (option->state & QStyle::State_Selected) != 0;

  QColor constexpr shadowColor{0, 0, 0, 40};
  auto constexpr shadowBlurRadius{5.0};
  auto constexpr Radius{8.0};

  // Shadow
  painter->setPen(Qt::NoPen);
  painter->setBrush(shadowColor);
  painter->drawRoundedRect(RoundedRect, Radius, Radius);

  // Body
  QColor fill;
  auto constexpr lighteningFactor{130};
  auto constexpr darkeningFactor{140};
  auto constexpr penWidthUnselected{1.5};
  auto constexpr penWidthSelected{2.5};

  fill = selected ? mColor.lighter(lighteningFactor) : mColor;
  painter->setBrush(fill);
  painter->setPen(QPen(fill.darker(darkeningFactor),
                       selected ? penWidthSelected : penWidthUnselected));
  painter->drawRoundedRect(BoundingRect, Radius, Radius);

  // Label
  auto constexpr FontSize{18};
  painter->setPen(Qt::white);
  painter->setFont(QFont("Arial", FontSize, QFont::Bold));
  painter->drawText(BoundingRect, Qt::AlignCenter | Qt::TextWordWrap, mLabel);
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
  if (dialog.exec() == QDialog::Accepted) {
    onSchemaChanged();
  }

  QGraphicsItem::mouseDoubleClickEvent(event);
}

void Node::addInputPort() {
  auto* port = new NodePort(PORT_TYPE::INPUT, this);
  port->setPos(this->boundingRect().center().x(), this->boundingRect().top());
  port->setVisible(false);
  mPorts.append(port);
}

void Node::addSingleOutputPort() {
  auto* port = new NodePort(PORT_TYPE::SINGLE_OUTPUT, this);
  port->setPos(this->boundingRect().center().x(),
               this->boundingRect().bottom());
  port->setVisible(false);
  mPorts.append(port);
}

void Node::addMultiOutputPort() {
  auto* port = new NodePort(PORT_TYPE::MULTI_OUTPUT, this);
  port->setPos(this->boundingRect().center().x(),
               this->boundingRect().bottom());
  port->setVisible(false);
  mPorts.append(port);
}

void Node::addHint() {
  const QString hint = QString::fromUtf8(
      getDescription().data(), static_cast<qsizetype>(getDescription().size()));
  setToolTip("<div style='max-width:500px;'>" + hint + "</div>");
}

void Node::hidePorts() {
  for (auto* port : mPorts) {
    port->setVisible(false);
  }
}

void Node::showInputPorts() {
  for (auto* port : mPorts) {
    if (port->getType() == PORT_TYPE::INPUT && port->connections().isEmpty()) {
      port->setVisible(true);
    }
  }
}

void Node::showOutputPorts() {
  for (auto* port : mPorts) {
    if (port->getType() == PORT_TYPE::MULTI_OUTPUT ||
        (port->getType() == PORT_TYPE::SINGLE_OUTPUT &&
         port->connections().isEmpty())) {
      port->setVisible(true);
    }
  }
}

QVariant Node::itemChange(const GraphicsItemChange change,
                          const QVariant& value) {
  if (change == ItemPositionHasChanged) {
    // TODO: Save the new position to the model
    for (auto* port : mPorts) {
      for (auto* conn : port->connections()) {
        conn->updatePath();
      }
    }
  }
  return QGraphicsItem::itemChange(change, value);
}
}  // namespace ZanaBlocks::IDE