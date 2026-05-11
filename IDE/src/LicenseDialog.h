#pragma once

#include <QDialog>

class QListWidget;
class QTextEdit;

namespace ZanaBlocks {
namespace IDE {

class LicensesDialog : public QDialog {
  Q_OBJECT

 public:
  explicit LicensesDialog(QWidget* parent = nullptr);

 private:
  void loadLicense(const QString& filePath);

  QListWidget* mListWidget;
  QTextEdit* mTextEdit;
};

}  // namespace IDE
}  // namespace ZanaBlocks