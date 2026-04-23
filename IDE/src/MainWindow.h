#pragma once
#include <QMainWindow>

#include "ReplClient.h"

class QPlainTextEdit;
class LoopItem;
class QToolBar;

namespace KidTech::IDE {
class Scene;
class View;

class LogOutput;

auto constexpr PORT{"/dev/tty.usbserial-0001"};

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

  View* mDiagramView{nullptr};
  Scene* mDiagramScene{nullptr};
  LogOutput* mLogOutput{nullptr};
  QToolBar* mSidebar{nullptr};
  LoopItem* mMainLoop{nullptr};
  std::unique_ptr<ReplClient::ReplClient> mReplClient{
      std::make_unique<ReplClient::ReplClient>()};
};
}  // namespace KidTech::IDE