#pragma once
#include <QDialog>

namespace ZanaBlocks {
namespace IDE {

class AboutDialog : public QDialog {
  Q_OBJECT

 public:
  explicit AboutDialog(QWidget* parent = nullptr);
};
}  // namespace IDE
}  // namespace ZanaBlocks