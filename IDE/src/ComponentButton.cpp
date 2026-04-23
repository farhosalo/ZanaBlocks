#include "ComponentButton.h"

namespace KidTech::IDE {
ComponentButton::ComponentButton(const QString& itemType, QWidget* parent)
    : QToolButton(parent), mType(itemType) {}

void ComponentButton::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) mDragStartPosition = event->pos();
  QToolButton::mousePressEvent(event);
}
void ComponentButton::mouseMoveEvent(QMouseEvent* event) {
  if (!(event->buttons() & Qt::LeftButton)) return;
  if ((event->pos() - mDragStartPosition).manhattanLength() <
      QApplication::startDragDistance())
    return;

  auto* drag = new QDrag(this);
  auto* mime = new QMimeData();
  mime->setText(mType);  // encode the shape type
  drag->setMimeData(mime);
  drag->setPixmap(icon().pixmap(32, 32));
  drag->exec(Qt::CopyAction);
}
}  // namespace KidTech::IDE