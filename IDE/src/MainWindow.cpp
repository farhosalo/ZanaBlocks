#include "MainWindow.h"

#include <QAction>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QListWidget>
#include <QMenu>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QStatusBar>
#include <QToolBar>

#include "LogOutput.h"
#include "Logging.h"
#include "Loop.h"
#include "Node.h"
#include "Scene.h"

namespace KidTech::IDE {
auto constexpr WINDOW_SIZE_WIDTH{1100};
auto constexpr WINDOW_SIZE_HEIGHT{750};

using namespace Utilities;

auto MainWindow::run() {
  if (!mReplClient->connect(PORT)) {
    mLogOutput->appendPlainText("Failed to connect to device!");
    return;
  }

  mLogOutput->appendPlainText("Uploading main.py...\n");

  auto callback = [this](const std::string& message) {
    mLogOutput->appendPlainText(QString::fromStdString(message));
  };

  if (!mReplClient->putFile("main.py", "/main.py", callback)) {
    mLogOutput->appendPlainText("Failed to upload main.py!");
    return;
  }

  mLogOutput->appendPlainText("\nUpload complete.\n");

  mLogOutput->appendPlainText("Resetting device...\n");
  mReplClient->reset();

  mLogOutput->appendPlainText("Done.\n");
}
auto MainWindow::save() {
  mLogOutput->appendPlainText("Saving the program...");
}
auto MainWindow::open() { mLogOutput->appendPlainText("Opening a program..."); }

auto MainWindow::initUI() {
  // Right side: Diagram View + Log Output
  // Create splitter (vertical = top/bottom)
  auto* verticalSplitter = new QSplitter(Qt::Vertical);

  // --- Top: Graphics View ---
  mDiagramScene = new Scene(this);

  mDiagramView = new QGraphicsView(mDiagramScene, this);

  mDiagramView->setRenderHint(QPainter::Antialiasing);
  mDiagramView->setRenderHint(QPainter::SmoothPixmapTransform);
  mDiagramView->setDragMode(QGraphicsView::RubberBandDrag);
  mDiagramView->setViewportUpdateMode(
      QGraphicsView::BoundingRectViewportUpdate);
  mDiagramView->setBackgroundBrush(QColor("#eeebe5"));
  statusBar()->showMessage("Ready");

  // --- Bottom: Logs / Output ---
  mLogOutput = new LogOutput();
  mLogOutput->setReadOnly(true);

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

auto MainWindow::initMenuAndToolbar() {
  auto* toolBar = addToolBar("Main Toolbar");
  auto* fileMenu = menuBar()->addMenu("&File");

  // File menu actions
  auto* openAction = fileMenu->addAction("📂  Open");
  auto* saveAction = fileMenu->addAction("💾  Save");

  fileMenu->addSeparator();
  auto* closeAction = fileMenu->addAction("❌  Close");

  toolBar->addAction(openAction);
  toolBar->addAction(saveAction);
  toolBar->addAction(closeAction);

  connect(openAction, &QAction::triggered, this, &MainWindow::open);
  connect(saveAction, &QAction::triggered, this, &MainWindow::save);
  connect(closeAction, &QAction::triggered, this, &QWidget::close);

  // Run menu actions
  auto* runMenu = menuBar()->addMenu("&Run");
  auto* runAction = runMenu->addAction("▶️  Run");

  toolBar->addSeparator();
  toolBar->addAction(runAction);

  connect(runAction, &QAction::triggered, this, &MainWindow::run);
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  setWindowTitle("KindTech IDE");
  resize(WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);

  initUI();
  initMenuAndToolbar();
}
}  // namespace KidTech::IDE