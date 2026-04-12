#include "MainWindow.h"

namespace KidTech::IDE {
auto constexpr WINDOW_SIZE_WIDTH{1100};
auto constexpr WINDOW_SIZE_HEIGHT{750};

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  setWindowTitle("KindTech IDE");
  resize(WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);
}
}  // namespace KidTech::IDE