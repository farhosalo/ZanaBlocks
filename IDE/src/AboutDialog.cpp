#include "AboutDialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

#include "ZanaBlocksInfo.h"

namespace ZanaBlocks::IDE {

auto constexpr WINDOW_WIDTH{500};
auto constexpr WINDOW_HEIGHT{300};

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent) {
  setWindowTitle("About ZanaBlocks");
  resize(WINDOW_WIDTH, WINDOW_HEIGHT);

  auto* layout = new QVBoxLayout(this);

  auto* title = new QLabel(
      "<h2>ZanaBlocks</h2>"
      "<p>Version: " +
      QString::fromStdString(VERSION) +
      "</p>"
      "<p>A cross-platform desktop application.</p>"
      "<p>Licensed under Apache License 2.0.</p>"
      "<br>"
      "<b>Third-party libraries:</b><br>"
      "- Qt Framework (LGPLv3)<br>"
      "- Protocol Buffers (BSD-3-Clause)<br>"
      "- libserialport (LGPL)<br>");

  title->setTextFormat(Qt::RichText);
  title->setWordWrap(true);

  layout->addWidget(title);

  auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok);

  connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);

  layout->addWidget(buttons);
}

}  // namespace ZanaBlocks::IDE
