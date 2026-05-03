#pragma once

#include "Node.h"

namespace KidTech::IDE {

/** @brief Base class for nodes that are associated with a specific protobuf
 * schema type.
 *
 * This template class provides a bridge between the visual Node and the
 * underlying protobuf message used for serialization.
 *
 * @tparam SchemaType The protobuf message class from the Schema namespace.
 */
template <typename SchemaType>
class BaseNodeSchema : public Node {
 public:
  explicit BaseNodeSchema(const QString& label, QGraphicsItem* parent = nullptr)
      : Node(label, parent) {}

  /**
   * @brief Returns the protobuf message schema associated with this node.
   * @return A reference to the protobuf message.
   */
  SchemaType& schema() override { return mSchema; }

  /**
   * @brief Returns the protobuf message schema associated with this node (const
   * version).
   * @return A const reference to the protobuf message.
   */
  const SchemaType& schema() const override { return mSchema; }

 private:
  SchemaType mSchema;
};
}  // namespace KidTech::IDE