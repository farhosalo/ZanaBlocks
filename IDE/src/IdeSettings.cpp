#include "IdeSettings.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QtConcurrent>

#include "EspTools.h"
#include "Logging.h"

namespace ZanaBlocks::IDE {
SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent),
      mPortListCombo(new QComboBox(this)),
      mButtonBox(new QDialogButtonBox(
          QDialogButtonBox::Save | QDialogButtonBox::Cancel, this)),
      mFlashProgressBar(new QProgressBar(this)),
      mFlashLogs(new QPlainTextEdit(this)),
      mFirmwarePathEdit(new QLineEdit(this)),
      mFlashButton(new QPushButton("Flash", this)) {
  setWindowTitle("Settings");

  auto* layout = new QFormLayout(this);
  addPortSettings(layout);
  addFlashSettings(layout);

  layout->addRow(mButtonBox);

  connect(mButtonBox, &QDialogButtonBox::accepted, this, [this] {
    saveSettings();
    accept();
  });
  connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

  setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}
void SettingsDialog::keyPressEvent(QKeyEvent* event) {
  if (event->key() != Qt::Key_Escape) {
    QDialog::keyPressEvent(event);
  }
}
void SettingsDialog::saveSettings() {}
void SettingsDialog::addPortSettings(QFormLayout* layout) {
  mPortListCombo->setObjectName("PortList");

  auto* refreshBtn = new QPushButton(tr("Refresh"));

  auto refreshPorts = [this] {
    mPortListCombo->clear();
    auto ports = EspTools::getUsbSerialPorts();

    QStringList list;
    list.reserve(static_cast<qsizetype>(ports.size()));
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
  mFirmwarePathEdit->setPlaceholderText("Path to .bin firmware…");
  mFirmwarePathEdit->setEnabled(false);

  auto* browseBtn = new QPushButton("Browse…");

  mFlashButton->setEnabled(false);

  fwRow->addWidget(new QLabel("Firmware:"));
  fwRow->addWidget(mFirmwarePathEdit, 1);
  fwRow->addWidget(browseBtn);
  fwRow->addWidget(mFlashButton);
  layout->addRow(fwRow);

  // Progress
  auto constexpr progressMaxValue = 100;
  mFlashProgressBar->setRange(0, progressMaxValue);
  layout->addWidget(mFlashProgressBar);

  // Log
  auto constexpr maxLogBlockCount = 500;
  mFlashLogs->setReadOnly(true);
  mFlashLogs->setMaximumBlockCount(maxLogBlockCount);
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
void SettingsDialog::flash() {
  if (mFirmwarePathEdit->text().isEmpty() ||
      mPortListCombo->currentText().isEmpty()) {
    mFlashLogs->appendPlainText("Select a port and firmware file first.");
    return;
  }
  mButtonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);

  mFlasher->setOnProgress([this](int progress) {
    QMetaObject::invokeMethod(
        this, [this, progress]() { mFlashProgressBar->setValue(progress); },
        Qt::QueuedConnection);
  });

  // NOLINTBEGIN  [bugprone-exception-escape]
  mFlasher->setOnStatus([this](const std::string& msg) {
    const QPointer<SettingsDialog> self(this);
    QMetaObject::invokeMethod(
        this,
        [self, msg]() {
          if (self == nullptr || self->mFlashLogs == nullptr) {
            ERROR(
                "SettingsDialog was destroyed while flashing, log update "
                "skipped: "
                << msg);
            return;
          }
          try {
            self->mFlashLogs->appendPlainText(QString::fromStdString(msg));
          } catch (const std::exception& e) {
            ERROR("Exception while updating flash logs: " << e.what());
          }
        },
        Qt::QueuedConnection);
  });
  // NOLINTEND

  mFlasher->setOnEnterBootloader([this]() {
    QMetaObject::invokeMethod(
        this, [this]() { shoeEnterBootloaderMessage(); }, Qt::QueuedConnection);
  });

  // NOLINTBEGIN [clang-analyzer-cplusplus.NewDeleteLeaks]
  auto future = QtConcurrent::run([this]() {
    enableSettings(false);
    const bool success =
        mFlasher->flash(mPortListCombo->currentText().toStdString(),
                        mFirmwarePathEdit->text().toStdString());
    enableSettings(true);
    QMetaObject::invokeMethod(
        mFlashLogs,
        [this, success]() {
          mFlashLogs->appendPlainText(success ? "Flashing succeeded."
                                              : "Flashing failed.");
        },
        Qt::QueuedConnection);
  });
  (void)future;  // To avoid "unused variable" warning. The future will clean up
                 // when it goes out of scope.
  // NOLINTEND
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