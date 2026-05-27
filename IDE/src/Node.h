#pragma once

#include <google/protobuf/message.h>

#include <QGraphicsItem>

#include "NodePort.h"

namespace ZanaBlocks::IDE {

/**
 * @brief Represents a node in the visual programming diagram. Each node can
 * have input and output ports for connecting to other nodes. The
 * Node class is responsible for rendering itself, managing its ports, and
 * handling user interactions such as hovering and item changes.
 *
 */
class Node : public QGraphicsItem {
 public:
  explicit Node(QString label, QGraphicsItem* parent = nullptr);

  /**
   * @brief Returns the bounding rectangle of the node.
   * @return The bounding rectangle.
   */
  [[nodiscard]] QRectF boundingRect() const override;

  /**
   * @brief Paints the node on the scene.
   * @param painter The painter to use for drawing.
   * @param option The style options for the item.
   * @param widget The widget associated with the scene.
   */
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;

  /**
   * @brief Hides all ports of the node.
   */
  void hidePorts();

  /**
   * @brief Shows only the input ports of the node.
   */
  void showInputPorts();

  /**
   * @brief Shows only the output ports of the node.
   */
  void showOutputPorts();

  /**
   * @brief Returns the list of ports associated with this node.
   * @return The list of ports.
   */
  [[nodiscard]] const QList<NodePort*>& getPorts() const { return mPorts; }

  /** @brief Returns the protobuf message schema associated
   *            with this node.
   * @return A reference to the protobuf message.*
   */
  virtual google::protobuf::Message& schema() = 0;

  /** @brief Returns the protobuf message schema associated
   *            with this node (const version).
   * @return A const reference to the protobuf message.
   */
  [[nodiscard]] virtual const google::protobuf::Message& schema() const = 0;

  /**
   * @brief Returns the description of the node.
   * @return The description.
   */
  [[nodiscard]] virtual std::string_view getDescription() const = 0;

 protected:
  QVariant itemChange(GraphicsItemChange change,
                      const QVariant& value) override;
  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

  virtual void onSchemaChanged() { update(); }

  void addInputPort();
  void addSingleOutputPort();
  void addMultiOutputPort();

  void addHint();

 private:
  /** @brief The label for the node. */
  QString mLabel{"Node"};

  static constexpr QColor defaultColor{100, 150, 240};
  /* Node properties */
  QColor mColor{defaultColor};

  /** @brief The list of ports associated with the node. */
  QList<NodePort*> mPorts;
};
}  // namespace ZanaBlocks::IDE