#pragma once

#include <QDialog>
class QFormLayout;

namespace KidTech::IDE {
class SettingsDialog : public QDialog {
  Q_OBJECT

 public:
  explicit SettingsDialog(QWidget* parent = nullptr);
  ~SettingsDialog() = default;

 private:
  void saveSettings();
  void addPortSettings(QFormLayout* layout);
};
}  // namespace KidTech::IDE