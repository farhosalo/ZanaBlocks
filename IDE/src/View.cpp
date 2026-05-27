#include "View.h"

namespace ZanaBlocks::IDE {
View::View(Scene* scene, QWidget* parent)
    : QGraphicsView(scene, parent), mScene(scene) {
  setAcceptDrops(true);
  setRenderHint(QPainter::Antialiasing);
  setDragMode(QGraphicsView::RubberBandDrag);
  setRenderHint(QPainter::SmoothPixmapTransform);
  setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
  setBackgroundBrush(QColor("#eeebe5"));
}

void View::dragEnterEvent(QDragEnterEvent* event) {
  if (event->mimeData()->hasText()) {
    event->acceptProposedAction();
  }
}

void View::dragMoveEvent(QDragMoveEvent* event) {
  if (event->mimeData()->hasText()) {
    event->acceptProposedAction();
  }
}

void View::dropEvent(QDropEvent* event) {
  if (!event->mimeData()->hasText()) {
    return;
  }

  // Map viewport pixel position → scene coordinates
  const QPointF scenePos = mapToScene(event->position().toPoint());
  mScene->createItemAt(event->mimeData()->text(), scenePos);
  event->acceptProposedAction();
}
}  // namespace ZanaBlocks::IDE