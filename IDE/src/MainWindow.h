#pragma once
#include <QMainWindow>

class QGraphicsView;
class QGraphicsScene;
class QPlainTextEdit;
class QListWidget;

namespace KidTech::IDE {
class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);

 private:
  auto initUI();
  auto initMenuAndToolbar();

  auto run();
  auto save();
  auto open();

  QGraphicsView* mDiagramView{nullptr};
  QGraphicsScene* mDiagramScene{nullptr};
  QPlainTextEdit* mLogOutput{nullptr};
  QListWidget* mSidebar{nullptr};
};
}  // namespace KidTech::IDE