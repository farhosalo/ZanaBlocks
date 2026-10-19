#pragma once
#include <QMainWindow>

class QPlainTextEdit;
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
  void closeEvent(QCloseEvent* event) override;
  auto initUI();
  auto initMenuAndToolbar();
  auto initSidebar();

  auto run();
  void newDiagram();
  void saveAs();
  void save();
  auto open();
  auto settings();
  static void checkFirstApplicationRun();

  void updateTitle();
  bool showUnsavedDiagramWarning();

  View* mDiagramView{nullptr};
  Scene* mDiagramScene{nullptr};
  LogOutput* mLogOutput{nullptr};
  QToolBar* mSidebar{nullptr};
  QString mFileName;

  std::string mSerialPort;
};
}  // namespace IDE
}  // namespace ZanaBlocks