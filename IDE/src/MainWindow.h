#pragma once
#include <QMainWindow>

#include "ReplClient.h"

class QGraphicsView;
class QPlainTextEdit;
class QListWidget;

namespace KidTech::IDE {
class Scene;

class LogOutput;

auto constexpr PORT{"/dev/tty.usbserial-0001"};

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
  Scene* mDiagramScene{nullptr};
  LogOutput* mLogOutput{nullptr};
  QListWidget* mSidebar{nullptr};

  std::unique_ptr<ReplClient::ReplClient> mReplClient{
      std::make_unique<ReplClient::ReplClient>()};
};
}  // namespace KidTech::IDE