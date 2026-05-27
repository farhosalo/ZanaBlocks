#include "NodeConnector.h"

#include <QStyleOptionGraphicsItem>

#include "NodePort.h"

namespace ZanaBlocks::IDE {
NodeConnector::NodeConnector(NodePort* start, NodePort* end)
    : mStartPort(start), mEndPort(end) {
  setPen(QPen(Qt::black, 2));
  setFlag(QGraphicsItem::ItemIsSelectable);
  setZValue(0);
  updatePath();
}
void NodeConnector::updatePath() {
  const QPointF startPoint = mStartPort->scenePos();
  const QPointF endPoint = mEndPort->scenePos();

  QPainterPath path(startPoint);
  auto constexpr controlOffset = 50;
  path.cubicTo(startPoint + QPointF(0, controlOffset),
               endPoint + QPointF(0, -controlOffset), endPoint);
  setPath(path);
}
QPainterPath NodeConnector::shape() const {
  auto constexpr strokeWidth = 12;  // hit area width, wider than visual pen
  QPainterPathStroker stroker;
  stroker.setWidth(strokeWidth);  // hit area, wider than visual pen
  return stroker.createStroke(path());
}

void NodeConnector::paint(QPainter* painter,
                          const QStyleOptionGraphicsItem* option,
                          QWidget* widget) {
  auto constexpr penWidth = 2.0;
  auto constexpr selectedPenWidth = 3.0;

  // Highlight selected connections in red and make them thicker for better
  // visibility Note: QStyle::State_Selected is set by the QGraphicsScene when
  // the item is selected
  // state is a bitmask, so we check if the State_Selected bit is set using a
  // bitwise AND operation
  const bool selected =
      option->state &          // NOLINT [readability-implicit-bool-conversion]
      QStyle::State_Selected;  // NOLINT [readability-implicit-bool-conversion]
  setPen(QPen(selected ? Qt::red : Qt::black,
              selected ? selectedPenWidth : penWidth));
  QGraphicsPathItem::paint(painter, option, widget);
}
}  // namespace ZanaBlocks::IDE