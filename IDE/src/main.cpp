#include <QApplication>

#include "IDEInfo.h"
#include "MainWindow.h"

int main(int argc, char* argv[]) {
  const QApplication app(argc, argv);
  QApplication::setStyle("Fusion");
  QApplication::setApplicationVersion(KidTech::IDE::VERSION);
  KidTech::IDE::MainWindow mainWindow;
  mainWindow.show();
  return QApplication::exec();
}