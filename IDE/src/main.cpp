#include <QApplication>

#include "MainWindow.h"
#include "ZanaBlocksInfo.h"

int main(int argc, char* argv[]) {
  const QApplication app(argc, argv);
  QApplication::setStyle("Fusion");
  QApplication::setApplicationVersion(ZanaBlocks::IDE::VERSION);
  ZanaBlocks::IDE::MainWindow mainWindow;
  mainWindow.show();
  return QApplication::exec();
}