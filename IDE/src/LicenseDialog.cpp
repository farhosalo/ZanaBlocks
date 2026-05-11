#include "LicenseDialog.h"

#include <QFile>
#include <QHBoxLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QTextEdit>
#include <QTextStream>

namespace ZanaBlocks {
namespace IDE {

LicensesDialog::LicensesDialog(QWidget* parent) : QDialog(parent) {
  setWindowTitle("Open Source Licenses");
  resize(900, 600);

  auto* layout = new QHBoxLayout(this);

  mListWidget = new QListWidget(this);
  mTextEdit = new QTextEdit(this);

  mTextEdit->setReadOnly(true);

  mListWidget->addItem("Apache License 2.0");
  mListWidget->addItem("Qt LGPLv3");
  mListWidget->addItem("Protocol Buffers BSD-3-Clause");
  mListWidget->addItem("libserialport LGPL");

  layout->addWidget(mListWidget, 1);
  layout->addWidget(mTextEdit, 3);

  connect(mListWidget, &QListWidget::currentTextChanged, this,
          [this](const QString& text) {
            if (text == "Apache License 2.0") {
              loadLicense("licenses/Apache-2.0.txt");
            } else if (text == "Qt LGPLv3") {
              loadLicense("licenses/Qt-LGPL-3.0.txt");
            } else if (text == "Protocol Buffers BSD-3-Clause") {
              loadLicense("licenses/protobuf-BSD-3-Clause.txt");
            } else if (text == "libserialport LGPL") {
              loadLicense("licenses/libserialport-LGPL-3.0.txt");
            }
          });

  mListWidget->setCurrentRow(0);
}

void LicensesDialog::loadLicense(const QString& filePath) {
  QFile file(filePath);

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, "License Error", "Could not open: " + filePath);
    return;
  }

  QTextStream stream(&file);

  mTextEdit->setPlainText(stream.readAll());
}

}  // namespace IDE
}  // namespace ZanaBlocks