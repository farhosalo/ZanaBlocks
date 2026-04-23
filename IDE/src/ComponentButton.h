#pragma once
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QToolButton>

namespace KidTech::IDE {

/**
 * @brief A button in the sidebar that can be dragged onto the diagram to create
 * a new item. clicks and drags the button, it initiates a drag operation with
 * the item type encoded in the MIME data.
 */
class ComponentButton : public QToolButton {
  Q_OBJECT
 public:
  explicit ComponentButton(const QString& itemType, QWidget* parent = nullptr);

 private:
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

  QString mType;
  QPoint mDragStartPosition;
};
}  // namespace KidTech::IDE