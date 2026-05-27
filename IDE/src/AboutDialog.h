#pragma once
#include <QDialog>

namespace ZanaBlocks::IDE {

class AboutDialog : public QDialog {
  Q_OBJECT

 public:
  explicit AboutDialog(QWidget* parent = nullptr);
};
}  // namespace ZanaBlocks::IDE