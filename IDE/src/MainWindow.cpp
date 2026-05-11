#include "MainWindow.h"

#include <QAction>
#include <QComboBox>
#include <QGraphicsScene>
#include <QMenu>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QStatusBar>
#include <QToolBar>

#include "AboutDialog.h"
#include "ComponentButton.h"
#include "IdeSettings.h"
#include "Interpreter.h"
#include "LicenseDialog.h"
#include "LogOutput.h"
#include "Logging.h"
#include "LoopNode.h"
#include "Scene.h"
#include "Schema.pb.h"
#include "View.h"
#include "google/protobuf/util/json_util.h"

using namespace ZanaBlocks::Utilities;

namespace ZanaBlocks::IDE {
auto constexpr WINDOW_SIZE_WIDTH{1100};
auto constexpr WINDOW_SIZE_HEIGHT{750};

using namespace Utilities;

auto MainWindow::run() {
  if (mSerialPort.empty()) {
    auto ports = mReplClient->getUsbSerialPorts();
    if (ports.empty()) {
      mLogOutput->appendPlainText("No serial ports found!");
      return;
    }
    mSerialPort = ports[0];
  }

  if (!mReplClient->connect(mSerialPort)) {
    mLogOutput->appendPlainText("Failed to connect to device: " +
                                QString::fromStdString(mSerialPort));
    return;
  }

  auto schema = std::make_shared<Schema::Root>();

  mDiagramScene->serialize(schema);

  Interpreter::Interpreter interpreter;
  interpreter.interpret(schema);
  interpreter.saveToFile("main.py");

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

auto MainWindow::settings() {
  SettingsDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted) {
    auto port = dialog.findChild<QComboBox*>("Port:");
    if (port) {
      mSerialPort = port->currentText().toStdString();
    }
  }
}

auto MainWindow::initUI() {
  // Right side: Diagram View + Log Output
  // Create splitter (vertical = top/bottom)
  auto* verticalSplitter = new QSplitter(Qt::Vertical);

  // --- Top: Graphics View ---
  mDiagramScene = new Scene(this);
  mDiagramView = new View(mDiagramScene, this);

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

  // Settings
  auto* settingsMenu = menuBar()->addMenu("&Settings");
  auto* settingsAction = settingsMenu->addAction("⚙️  Settings");
  toolBar->addSeparator();
  toolBar->addAction(settingsAction);

  connect(settingsAction, &QAction::triggered, this, &MainWindow::settings);

  // Help
  auto* helpMenu = menuBar()->addMenu("&Help");
  auto* aboutAction = helpMenu->addAction("ℹ️  About");
  auto* licensesAction = helpMenu->addAction("📄  Licenses");

  toolBar->addSeparator();
  toolBar->addAction(aboutAction);
  toolBar->addAction(licensesAction);

  connect(aboutAction, &QAction::triggered, this, [this]() {
    AboutDialog dialog(this);
    dialog.exec();
  });

  connect(licensesAction, &QAction::triggered, this, [this]() {
    LicensesDialog dialog(this);
    dialog.exec();
  });
}

auto MainWindow::initSidebar() {
  mSidebar = new QToolBar("Components");
  mSidebar->setOrientation(Qt::Vertical);
  mSidebar->setMovable(false);
  mSidebar->setFloatable(false);

  // Add component actions
  auto* loopButton = new ComponentButton("Loop", mSidebar);
  loopButton->setText("🔄 Loop");
  mSidebar->addWidget(loopButton);

  auto* printButton = new ComponentButton("Print", mSidebar);
  printButton->setText("🖨️ Print");
  mSidebar->addWidget(printButton);

  auto* sleepButton = new ComponentButton("Sleep", mSidebar);
  sleepButton->setText("⏰ Sleep");
  mSidebar->addWidget(sleepButton);

  auto* ledButton = new ComponentButton("LED", mSidebar);
  ledButton->setText("💡 LED");
  mSidebar->addWidget(ledButton);
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), mSchema(std::make_shared<Schema::Root>()) {
  setWindowTitle("ZanaBlocks IDE");
  resize(WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);

  initSidebar();
  initUI();
  initMenuAndToolbar();

  // Add main loop node to the scene
  // TODO: Only if new diagram is created, not when loading an existing one
  {
    auto* node = new LoopNode(true);
    node->setPos(0, 0);
    mDiagramScene->addItem(node);
  }
}
}  // namespace ZanaBlocks::IDE