#include "ComponentButton.h"

namespace ZanaBlocks::IDE {
ComponentButton::ComponentButton(QString itemType, QWidget* parent)
    : QToolButton(parent), mType(std::move(itemType)) {}

void ComponentButton::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    mDragStartPosition = event->pos();
  }
  QToolButton::mousePressEvent(event);
}
void ComponentButton::mouseMoveEvent(QMouseEvent* event) {
  if (!(event->buttons() & Qt::LeftButton)) {
    return;
  }
  if ((event->pos() - mDragStartPosition).manhattanLength() <
      QApplication::startDragDistance()) {
    return;
  }

  auto* drag = new QDrag(this);  // NOLINT(cppcoreguidelines-owning-memory)
  auto* mime = new QMimeData();  // NOLINT(cppcoreguidelines-owning-memory)
  mime->setText(mType);          // encode the shape type
  drag->setMimeData(mime);
  constexpr int DragIconSize = 32;
  drag->setPixmap(icon().pixmap(DragIconSize, DragIconSize));
  drag->exec(Qt::CopyAction);
}
}  // namespace ZanaBlocks::IDE