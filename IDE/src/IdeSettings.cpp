#include "IdeSettings.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QtConcurrent>

#include "EspTools.h"

namespace ZanaBlocks::IDE {
SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
  setWindowTitle("Settings");

  auto* layout = new QFormLayout(this);
  addPortSettings(layout);
  addFlashSettings(layout);

  mButtonBox = new QDialogButtonBox(
      QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
  layout->addRow(mButtonBox);

  connect(mButtonBox, &QDialogButtonBox::accepted, this, [this] {
    saveSettings();
    accept();
  });
  connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

  setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}
void SettingsDialog::keyPressEvent(QKeyEvent* event) {
  if (event->key() != Qt::Key_Escape) QDialog::keyPressEvent(event);
}
void SettingsDialog::saveSettings() { QSettings settings; }
void SettingsDialog::addPortSettings(QFormLayout* layout) {
  mPortListCombo = new QComboBox(this);
  mPortListCombo->setObjectName("PortList");

  auto* refreshBtn = new QPushButton(tr("Refresh"));

  auto refreshPorts = [this] {
    mPortListCombo->clear();
    auto ports = EspTools::getUsbSerialPorts();

    QStringList list;
    list.reserve(ports.size());
    for (const auto& str : ports) {
      list.append(QString::fromStdString(str));
    }

    mPortListCombo->addItems(list);
  };
  auto* portsRow = new QHBoxLayout;
  portsRow->addWidget(mPortListCombo);
  portsRow->addWidget(refreshBtn);
  layout->addRow("Port:", portsRow);

  connect(refreshBtn, &QPushButton::clicked, this, refreshPorts);
}
void SettingsDialog::addFlashSettings(QFormLayout* layout) {
  // Firmware URL row
  auto* urlRow = new QHBoxLayout;
  auto* fwUrl = new QLabel(
      "<a "
      "href=\"https://micropython.org/resources/firmware/"
      "ESP32_GENERIC-20260406-v1.28.0.bin\">Download the firmware here.</a>");

  fwUrl->setOpenExternalLinks(true);
  urlRow->addWidget(fwUrl);
  layout->addRow(urlRow);

  // Firmware row
  auto* fwRow = new QHBoxLayout;
  mFirmwarePathEdit = new QLineEdit;
  mFirmwarePathEdit->setPlaceholderText("Path to .bin firmware…");
  mFirmwarePathEdit->setEnabled(false);

  auto* browseBtn = new QPushButton("Browse…");

  mFlashButton = new QPushButton("Flash");
  mFlashButton->setEnabled(false);

  fwRow->addWidget(new QLabel("Firmware:"));
  fwRow->addWidget(mFirmwarePathEdit, 1);
  fwRow->addWidget(browseBtn);
  fwRow->addWidget(mFlashButton);
  layout->addRow(fwRow);

  // Progress
  mFlashProgressBar = new QProgressBar;
  mFlashProgressBar->setRange(0, 100);
  layout->addWidget(mFlashProgressBar);

  // Log
  mFlashLogs = new QPlainTextEdit;
  mFlashLogs->setReadOnly(true);
  mFlashLogs->setMaximumBlockCount(500);
  layout->addWidget(mFlashLogs);

  connect(mFlashButton, &QPushButton::clicked, this, [this]() {
    mFlashLogs->clear();
    mFlashProgressBar->setValue(0);

    flash();
  });

  QObject::connect(browseBtn, &QPushButton::clicked, [&] {
    const QString path = QFileDialog::getOpenFileName(
        this, "Select Firmware", {}, "Binary firmware (*.bin);;All files (*)");
    if (!path.isEmpty()) {
      mFirmwarePathEdit->setText(path);
      mFlashButton->setEnabled(true);
    }
  });
}
void SettingsDialog::enableSettings(const bool enable) {
  mButtonBox->button(QDialogButtonBox::Cancel)->setEnabled(enable);
  mButtonBox->button(QDialogButtonBox::Save)->setEnabled(enable);
  mPortListCombo->setEnabled(enable);
  mFirmwarePathEdit->setEnabled(enable);
  mFlashButton->setEnabled(enable);
  mFlashButton->setEnabled(enable);
}
bool SettingsDialog::flash() {
  if (mFirmwarePathEdit->text().isEmpty() ||
      mPortListCombo->currentText().isEmpty()) {
    mFlashLogs->appendPlainText("Select a port and firmware file first.");
    return false;
  }
  mButtonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);

  mFlasher->onProgress = [this](int progress) {
    QMetaObject::invokeMethod(
        this, [this, progress]() { mFlashProgressBar->setValue(progress); },
        Qt::QueuedConnection);
  };
  mFlasher->onStatus = [this](const std::string& msg) {
    QMetaObject::invokeMethod(
        this,
        [this, msg]() {
          mFlashLogs->appendPlainText(QString::fromStdString(msg));
        },
        Qt::QueuedConnection);
  };
  mFlasher->onEnterBootloader = [this]() {
    QMetaObject::invokeMethod(
        this, [this]() { shoeEnterBootloaderMessage(); }, Qt::QueuedConnection);
  };

  QtConcurrent::run([this]() {
    enableSettings(false);
    bool success = mFlasher->flash(mPortListCombo->currentText().toStdString(),
                                   mFirmwarePathEdit->text().toStdString());
    enableSettings(true);
    QMetaObject::invokeMethod(
        this, [this, success]() { qDebug() << "Finished:" << success; },
        Qt::QueuedConnection);
  });

  return true;
}
void SettingsDialog::shoeEnterBootloaderMessage() {
  QMessageBox enterBootloaderMessage;
  enterBootloaderMessage.setWindowTitle("ℹ Enter Bootloader Mode");
  enterBootloaderMessage.setText(
      "<b>"
      "Hold down the Boot button, press the Reset (EN/RST) button, and then "
      "release both buttons.<br>"
      "Once this is done, click OK.<br><br>");

  enterBootloaderMessage.setIcon(QMessageBox::Information);

  enterBootloaderMessage.setStandardButtons(QMessageBox::Ok);
  enterBootloaderMessage.exec();
  mFlasher->setEnterBootloader();
}
}  // namespace ZanaBlocks::IDE