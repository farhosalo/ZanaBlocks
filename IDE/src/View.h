#pragma once
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGraphicsView>
#include <QMimeData>

#include "Scene.h"
namespace KidTech::IDE {

/**
 * @brief The View class is a custom QGraphicsView that serves as the viewport
 * for the diagram. It handles drag-and-drop events to allow users to drag
 * components from the sidebar and drop them onto the scene. The View class also
 * sets up rendering hints and interaction modes for a better user experience.
 */
class View : public QGraphicsView {
  Q_OBJECT
 public:
  explicit View(Scene* scene, QWidget* parent = nullptr);

 private:
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dragMoveEvent(QDragMoveEvent* event) override;
  void dropEvent(QDropEvent* event) override;

  Scene* mScene;
};

}  // namespace KidTech::IDE