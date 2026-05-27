#include "NodeSettings.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>

using google::protobuf::Descriptor;
using google::protobuf::FieldDescriptor;
using google::protobuf::Reflection;

namespace ZanaBlocks::IDE {
NodeSettings::NodeSettings(std::string_view description,
                           google::protobuf::Message& message, QWidget* parent)
    : QDialog(parent), mDescription(description), mSchema(message) {
  setWindowTitle(tr("%1 Properties")
                     .arg(QString::fromStdString(
                         std::string(mSchema.GetDescriptor()->name()))));

  auto* layout = new QFormLayout(this);

  auto* label = new QLabel(
      tr("\n%1\n\n")
          .arg(QString::fromUtf8(mDescription.data(),
                                 static_cast<qsizetype>(mDescription.size()))));

  label->setWordWrap(true);
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  layout->addRow(label);

  buildSettingsDialog(layout);

  auto* buttons = new QDialogButtonBox(
      QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
  layout->addRow(buttons);

  connect(buttons, &QDialogButtonBox::accepted, this, [this] {
    save();
    accept();
  });
  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
void NodeSettings::buildSettingsDialog(QFormLayout* layout) {
  const Descriptor* desc = mSchema.GetDescriptor();
  const Reflection* refl = mSchema.GetReflection();

  for (int i = 0; i < desc->field_count(); ++i) {
    const FieldDescriptor* field = desc->field(i);
    const QString label = QString::fromUtf8(
        field->name().data(), static_cast<qsizetype>(field->name().size()));

    switch (field->cpp_type()) {
      case FieldDescriptor::CPPTYPE_STRING: {
        auto* edit = new QLineEdit(this);
        edit->setText(QString::fromStdString(refl->GetString(mSchema, field)));
        edit->setObjectName(label);
        layout->addRow(label, edit);
        break;
      }
      case FieldDescriptor::CPPTYPE_INT32:
      case FieldDescriptor::CPPTYPE_INT64:
      case FieldDescriptor::CPPTYPE_UINT32:
      case FieldDescriptor::CPPTYPE_UINT64: {
        auto* spin = new QSpinBox(this);
        spin->setRange(INT_MIN, INT_MAX);
        spin->setValue(static_cast<int>(
            refl->GetInt32(mSchema, field)));  // cast is safe for UI
        spin->setObjectName(label);
        layout->addRow(label, spin);
        break;
      }
      case FieldDescriptor::CPPTYPE_BOOL: {
        auto* check = new QCheckBox(this);
        check->setChecked(refl->GetBool(mSchema, field));
        check->setObjectName(label);
        layout->addRow(label, check);
        break;
      }
      default:
        // nested messages, enums, etc.
        break;
    }
  }
}
void NodeSettings::save() {
  const Descriptor* desc = mSchema.GetDescriptor();
  const Reflection* refl = mSchema.GetReflection();

  for (int i = 0; i < desc->field_count(); ++i) {
    const FieldDescriptor* field = desc->field(i);
    const QString name = QString::fromUtf8(
        field->name().data(), static_cast<qsizetype>(field->name().size()));

    switch (field->cpp_type()) {
      case FieldDescriptor::CPPTYPE_STRING:
        if (auto* widget = findChild<QLineEdit*>(name)) {
          refl->SetString(&mSchema, field, widget->text().toStdString());
        }
        break;
      case FieldDescriptor::CPPTYPE_INT32:
      case FieldDescriptor::CPPTYPE_INT64:
      case FieldDescriptor::CPPTYPE_UINT32:
      case FieldDescriptor::CPPTYPE_UINT64:
        if (auto* widget = findChild<QSpinBox*>(name)) {
          refl->SetInt32(&mSchema, field, widget->value());
        }
        break;
      case FieldDescriptor::CPPTYPE_BOOL:
        if (auto* widget = findChild<QCheckBox*>(name)) {
          refl->SetBool(&mSchema, field, widget->isChecked());
        }
        break;
      default:
        break;
    }
  }
}
}  // namespace ZanaBlocks::IDE