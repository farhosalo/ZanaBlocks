#include "MainWindow.h"

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPointer>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>
#include <QToolBar>
#include <QtConcurrent>

#include "AboutDialog.h"
#include "ComponentButton.h"
#include "EspTools.h"
#include "IdeSettings.h"
#include "Interpreter.h"
#include "LicenseDialog.h"
#include "LogOutput.h"
#include "Logging.h"
#include "LoopNode.h"
#include "Protobuf.h"
#include "ReplClient.h"
#include "Scene.h"
#include "Schema.pb.h"
#include "View.h"
#include "google/protobuf/util/json_util.h"

namespace ZanaBlocks::IDE {
auto constexpr WINDOW_SIZE_WIDTH{1100};
auto constexpr WINDOW_SIZE_HEIGHT{750};
auto constexpr DisplayDurationMs = 5000;

auto constexpr WindowTitle{"ZanaBlocks IDE"};

auto MainWindow::run() {
  // NOLINTBEGIN  [bugprone-exception-escape]
  auto logMessage = [this](std::string message) {
    QMetaObject::invokeMethod(
        mLogOutput,
        [this, message = std::move(message)]() {
          if (mLogOutput != nullptr) {
            mLogOutput->appendPlainText(QString::fromStdString(message));
          } else {
            ERROR("Log message received but mLogOutput is null: " << message);
          }
        },
        Qt::QueuedConnection);
  };
  // NOLINTEND

  if (mSerialPort.empty()) {
    auto ports = EspTools::getUsbSerialPorts();
    if (ports.empty()) {
      logMessage("No serial ports found!");
      return;
    }
    mSerialPort = ports.at(0);
  }
  auto connection = EspTools::connect(mSerialPort);

  if (connection == nullptr) {
    logMessage("Failed to connect to device: " + mSerialPort);
    return;
  }
  auto replClient = std::make_shared<EspTools::ReplClient>(connection);
  replClient->setOnLogs(logMessage);
  auto probeResult = replClient->probe();

  if (!probeResult.isMicroPython) {
    logMessage(
        "Device is not running MicroPython!, press reset (EN/RST) button and "
        "try again. If the problem persists, please flash MicroPython firmware "
        "to your device.");
    return;
  }
  logMessage("Device version : " + probeResult.firmwareVersion + " (" +
             probeResult.hardwareModel + ")");

  auto schema = std::make_shared<Schema::Root>();

  mDiagramScene->serialize(schema);

  Interpreter::Interpreter interpreter;
  interpreter.interpret(schema);
  interpreter.saveToFile("main.py");

  logMessage("Uploading main.py...");

  if (!replClient->putFile("main.py", "/main.py")) {
    logMessage("Failed to upload main.py!");
    return;
  }

  logMessage("Upload complete.");

  logMessage("Resetting device...");
  replClient->resetTarget();

  logMessage("Done.");
}

void MainWindow::newDiagram() {
  if (showUnsavedDiagramWarning()) {
    mDiagramScene->clear();
    auto* node = new LoopNode(true);
    node->setPos(0, 0);
    mDiagramScene->addItem(node);
    mFileName.clear();
  }
}

void MainWindow::saveAs() {
  mFileName = QFileDialog::getSaveFileName(
      this, tr("Save File"), "", tr("Binary Files (*.bin);;All Files (*)"));
  if (!mFileName.isEmpty()) {
    save();
  }
}
void MainWindow::save() {
  if (mFileName.isEmpty()) {
    saveAs();
    return;
  }
  if (!mFileName.isEmpty()) {
    auto schema = std::make_shared<Schema::Root>();
    mDiagramScene->serialize(schema);
    // INFO(schema->DebugString());

    if (Utilities::saveProtobuf2File(mFileName.toStdString(), *schema)) {
      statusBar()->showMessage("File saved: " + mFileName, DisplayDurationMs);
      mDiagramScene->setModified(false);
    } else {
      QMessageBox::warning(this, "Save Error", "Failed to save file.");
    }
  }
}
auto MainWindow::open() {
  if (!showUnsavedDiagramWarning()) {
    return;
  }

  const QString fileName = QFileDialog::getOpenFileName(
      this, tr("Open File"), "", tr("Binary Files (*.bin);;All Files (*)"));

  if (!fileName.isEmpty()) {
    auto schema = std::make_shared<Schema::Root>();
    if (Utilities::loadProtobufFromFile(fileName.toStdString(), *schema)) {
      mDiagramScene->load(schema);

      statusBar()->showMessage("File opened: " + fileName, DisplayDurationMs);
      mFileName = fileName;
      mDiagramScene->setModified(false);
    } else {
      QMessageBox::warning(this, "Open Error", "Failed to open file.");
    }
  }
}

auto MainWindow::settings() {
  SettingsDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted) {
    auto* port = dialog.findChild<QComboBox*>("Port:");
    if (port != nullptr) {
      mSerialPort = port->currentText().toStdString();
    }
  }
}

void MainWindow::closeEvent(QCloseEvent* event) {
  if (!showUnsavedDiagramWarning()) {
    event->ignore();
    return;
  }
  event->accept();
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
  constexpr auto MainAreaStrechFactor = 8;
  mainSplitter->setStretchFactor(0, 1);                     // sidebar smaller
  mainSplitter->setStretchFactor(1, MainAreaStrechFactor);  // main area bigger

  setCentralWidget(mainSplitter);
}

auto MainWindow::initMenuAndToolbar() {
  auto* toolBar = addToolBar("Main Toolbar");
  auto* fileMenu = menuBar()->addMenu("&File");

  // File menu actions
  auto* newAction = fileMenu->addAction("➕  New");
  newAction->setShortcut(QKeySequence::New);
  fileMenu->addSeparator();

  auto* openAction = fileMenu->addAction("📂  Open");
  openAction->setShortcut(QKeySequence::Open);
  fileMenu->addSeparator();

  auto* saveAction = fileMenu->addAction("💾  Save");
  saveAction->setShortcut(QKeySequence::Save);
  auto* saveAsAction = fileMenu->addAction("💾  Save As...");
  saveAsAction->setShortcut(QKeySequence::SaveAs);

  fileMenu->addSeparator();
  auto* closeAction = fileMenu->addAction("❌  Close");
  closeAction->setShortcut(QKeySequence::Close);

  toolBar->addAction(newAction);
  toolBar->addAction(openAction);
  toolBar->addAction(saveAction);
  toolBar->addSeparator();

  connect(openAction, &QAction::triggered, this, &MainWindow::open);
  connect(saveAction, &QAction::triggered, this, &MainWindow::save);
  connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);
  connect(closeAction, &QAction::triggered, this, &QWidget::close);
  connect(newAction, &QAction::triggered, this, &MainWindow::newDiagram);

  // Run menu actions
  auto* runMenu = menuBar()->addMenu("&Run");
  auto* runAction = runMenu->addAction("▶️  Run");

  toolBar->addSeparator();
  toolBar->addAction(runAction);

  connect(runAction, &QAction::triggered, this, [this]() {
    auto future = QtConcurrent::run([this]() { run(); });
    (void)future;
  });

  // Settings
  auto* settingsMenu = menuBar()->addMenu("&Settings");
  auto* settingsAction = settingsMenu->addAction("⚙️  Settings");
  settingsAction->setShortcut(QKeySequence::Preferences);

  toolBar->addSeparator();
  toolBar->addAction(settingsAction);

  connect(settingsAction, &QAction::triggered, this, &MainWindow::settings);

  // Help
  auto* helpMenu = menuBar()->addMenu("&Help");
  auto* aboutAction = helpMenu->addAction("ℹ️  About");
  aboutAction->setShortcut(QKeySequence::HelpContents);
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

  auto* pwmButton = new ComponentButton("PWM", mSidebar);
  pwmButton->setText("⚡ PWM");
  mSidebar->addWidget(pwmButton);
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  resize(WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);

  initSidebar();
  initUI();
  initMenuAndToolbar();
  updateTitle();

  // Add main loop node to the scene
  {
    auto* node = new LoopNode(true);
    node->setPos(0, 0);
    mDiagramScene->addItem(node);
  }

  checkFirstApplicationRun();

  connect(mDiagramScene, &Scene::modified, this, [this]() { updateTitle(); });
}
void MainWindow::checkFirstApplicationRun() {
  QSettings settings("ZanaBlocks", "IDE");

  if (!settings.value("safetyNoticeAccepted", false).toBool()) {
    QMessageBox safetyMessageBox;
    safetyMessageBox.setWindowTitle("⚠️ Safety Notice");
    safetyMessageBox.setText(
        "<b>This application is provided for educational purposes "
        "only.</b><br><br>"
        "Use by children must be under <b>parental supervision</b> at all "
        "times.<br><br>"
        "The developer is not responsible for any injuries or damages caused "
        "by "
        "incorrect wiring, missing resistors, or improper use of electronic "
        "components.<br><br>"
        "Provided free and open source under the <b>Apache License "
        "2.0</b>.<br>"
        "Use at your own risk.<br><br><br>");
    safetyMessageBox.setIcon(QMessageBox::Warning);

    auto* dontShowAgainCheckBox = new QCheckBox("Don't show this again");
    safetyMessageBox.setCheckBox(dontShowAgainCheckBox);
    safetyMessageBox.setStandardButtons(QMessageBox::Ok);
    safetyMessageBox.exec();

    if (dontShowAgainCheckBox->isChecked()) {
      settings.setValue("safetyNoticeAccepted", true);
    } else {
      settings.setValue("safetyNoticeAccepted", false);
    }
  }
}

void MainWindow::updateTitle() {
  auto displayName = mFileName.isEmpty() ? "Untitled" : mFileName;
  if (mDiagramScene->isModified()) {
    displayName += "*";
  }
  setWindowTitle(QString("%1 - %2").arg(displayName).arg(WindowTitle));
}
bool MainWindow::showUnsavedDiagramWarning() {
  if (mDiagramScene->isModified()) {
    // QMessageBox::StandardButton reply;
    auto reply = QMessageBox::warning(
        this, "Unsaved Diagram",
        "The current diagram has unsaved changes. Do you want to save them?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (reply == QMessageBox::Save) {
      save();
    } else if (reply == QMessageBox::Discard) {
      mDiagramScene->setModified(false);
    } else {
      return false;
    }
  }
  return true;
}
}  // namespace ZanaBlocks::IDE