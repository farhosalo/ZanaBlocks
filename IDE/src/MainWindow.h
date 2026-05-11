#pragma once
#include <QMainWindow>

#include "ReplClient.h"

class QPlainTextEdit;
class LoopItem;
class QToolBar;

namespace KidTech {
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
  std::unique_ptr<ReplClient::ReplClient> mReplClient{
      std::make_unique<ReplClient::ReplClient>()};

  std::string mSerialPort{""};
  std::shared_ptr<Schema::Root> mSchema;
};
}  // namespace IDE
}  // namespace KidTech