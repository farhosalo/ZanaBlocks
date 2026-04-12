#include "MainWindow.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QStatusBar>

namespace KidTech::IDE {
auto constexpr WINDOW_SIZE_WIDTH{1100};
auto constexpr WINDOW_SIZE_HEIGHT{750};

auto MainWindow::initUI() {
  // Right side: Diagram View + Log Output
  // Create splitter (vertical = top/bottom)
  auto* verticalSplitter = new QSplitter(Qt::Vertical);

  // --- Top: Graphics View ---
  mDiagramScene = new QGraphicsScene(this);
  mDiagramView = new QGraphicsView(mDiagramScene, this);

  mDiagramView->setRenderHint(QPainter::Antialiasing);
  mDiagramView->setRenderHint(QPainter::SmoothPixmapTransform);
  mDiagramView->setDragMode(QGraphicsView::RubberBandDrag);
  mDiagramView->setViewportUpdateMode(
      QGraphicsView::BoundingRectViewportUpdate);
  mDiagramView->setBackgroundBrush(QColor("#eeebe5"));
  statusBar()->showMessage("Ready");

  // --- Bottom: Logs / Output ---
  mLogOutput = new QPlainTextEdit();
  mLogOutput->setReadOnly(true);
  mLogOutput->appendPlainText("Test log output...");

  // Add widgets to splitter
  verticalSplitter->addWidget(mDiagramView);
  verticalSplitter->addWidget(mLogOutput);

  // Set initial proportions
  verticalSplitter->setStretchFactor(0, 3);  // top bigger
  verticalSplitter->setStretchFactor(1, 1);  // bottom smaller

  verticalSplitter->setSizes({WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT / 3});

  // --- LEFT SIDEBAR ---
  mSidebar = new QListWidget();

  // --- MAIN SPLITTER (LEFT + RIGHT) ---
  auto* mainSplitter = new QSplitter(Qt::Horizontal);

  // Add sidebar and main area to main splitter
  mainSplitter->addWidget(mSidebar);
  mainSplitter->addWidget(verticalSplitter);

  // Set initial proportions
  mainSplitter->setStretchFactor(0, 1);  // sidebar smaller
  mainSplitter->setStretchFactor(1, 8);  // main area bigger

  setCentralWidget(mainSplitter);
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  setWindowTitle("KindTech IDE");
  resize(WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);

  initUI();
}
}  // namespace KidTech::IDE