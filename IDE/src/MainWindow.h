#pragma once
#include <QMainWindow>

class QPlainTextEdit;
class LoopItem;
class QToolBar;

namespace ZanaBlocks {
namespace Schema {

class Root;
}  // namespace Schema

namespace IDE {
class Scene;
class View;

class LogOutput;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);

 private:
  auto initUI();
  auto initMenuAndToolbar();
  auto initSidebar();

  auto run();
  auto save();
  auto open();
  auto settings();

 private:
  View* mDiagramView{nullptr};
  Scene* mDiagramScene{nullptr};
  LogOutput* mLogOutput{nullptr};
  QToolBar* mSidebar{nullptr};
  LoopItem* mMainLoop{nullptr};

  std::string mSerialPort{""};
  std::shared_ptr<Schema::Root> mSchema;
};
}  // namespace IDE
}  // namespace ZanaBlocks