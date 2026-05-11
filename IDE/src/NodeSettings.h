#pragma once

#include <google/protobuf/message.h>

#include <QDialog>

class QFormLayout;

namespace ZanaBlocks::IDE {

/**
 * @brief A dialog for editing the properties of a node based on its protobuf
 schema.
 */
class NodeSettings : public QDialog {
 public:
  explicit NodeSettings(std::string_view description,
                        google::protobuf::Message& message,
                        QWidget* parent = nullptr);

  /** * @brief Dynamically builds the settings dialog based on the protobuf
   * schema fields.
   * @param layout The layout to which the settings widgets will be added.
   */
  void buildSettingsDialog(QFormLayout* layout);

  /**
   * @brief Stores the values from the dialog widgets back into the protobuf
   * schema.
   */
  void save();

 private:
  std::string_view mDescription;
  google::protobuf::Message& mSchema;
};
}  // namespace ZanaBlocks::IDE