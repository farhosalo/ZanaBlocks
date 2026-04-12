#pragma once
#include <QMainWindow>

namespace KidTech::IDE {
class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);
};
}  // namespace KidTech::IDE