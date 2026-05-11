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
  QPointF startPoint = mStartPort->scenePos();
  QPointF endPoint = mEndPort->scenePos();

  QPainterPath path(startPoint);
  path.cubicTo(startPoint + QPointF(0, 50), endPoint + QPointF(0, -50),
               endPoint);
  setPath(path);
}
QPainterPath NodeConnector::shape() const {
  QPainterPathStroker stroker;
  stroker.setWidth(12);  // hit area, wider than visual pen
  return stroker.createStroke(path());
}

void NodeConnector::paint(QPainter* painter,
                          const QStyleOptionGraphicsItem* option,
                          QWidget* widget) {
  const auto selected = option->state & QStyle::State_Selected;
  setPen(QPen(selected ? Qt::red : Qt::black, selected ? 3.0 : 2.0));
  QGraphicsPathItem::paint(painter, option, widget);
}
}  // namespace ZanaBlocks::IDE