#include "IdeSettings.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QSettings>

#include "ReplClient.h"

namespace ZanaBlocks::IDE {
SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
  setWindowTitle("Settings");

  auto* layout = new QFormLayout(this);
  addPortSettings(layout);

  auto* buttons = new QDialogButtonBox(
      QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
  layout->addRow(buttons);

  connect(buttons, &QDialogButtonBox::accepted, this, [this] {
    saveSettings();
    accept();
  });
  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
void SettingsDialog::saveSettings() { QSettings settings; }
void SettingsDialog::addPortSettings(QFormLayout* layout) {
  auto* portList = new QComboBox(this);

  auto ports = ReplClient::ReplClient::getUsbSerialPorts();

  QStringList list;
  list.reserve(ports.size());
  for (const auto& str : ports) {
    list.append(QString::fromStdString(str));
  }

  portList->addItems(list);
  layout->addRow("Port:", portList);
}
}  // namespace ZanaBlocks::IDE