#pragma once

#include <QDialog>

#include "EspFlasher.h"
class QFormLayout;
class QDialogButtonBox;
class QKeyEvent;
class QProgressBar;
class QPlainTextEdit;
class QLineEdit;
class QComboBox;
class QPushButton;

namespace ZanaBlocks::IDE {
class SettingsDialog : public QDialog {
  Q_OBJECT

 public:
  explicit SettingsDialog(QWidget* parent = nullptr);

 private:
  void keyPressEvent(QKeyEvent* event) override;

  void saveSettings();
  void addPortSettings(QFormLayout* layout);
  void addFlashSettings(QFormLayout* layout);
  void enableSettings(bool enable);

  void flash();

  void shoeEnterBootloaderMessage();

  QComboBox* mPortListCombo;
  QDialogButtonBox* mButtonBox;
  QProgressBar* mFlashProgressBar;
  QPlainTextEdit* mFlashLogs;
  QLineEdit* mFirmwarePathEdit;
  QPushButton* mFlashButton;

  std::unique_ptr<EspTools::EspFlasher> mFlasher{
      std::make_unique<EspTools::EspFlasher>()};
};
}  // namespace ZanaBlocks::IDE