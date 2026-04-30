#pragma once

#include <QGraphicsItem>

#include "NodePort.h"

namespace KidTech::IDE {

enum class NodeType {
  MainLoop,
  ChildLoop,
  Print,
  Sleep,
};

/**
 * @brief Represents a node in the visual programming diagram. Each node can
 * have input and output ports for connecting to other nodes. The
 * Node class is responsible for rendering itself, managing its ports, and
 * handling user interactions such as hovering and item changes.
 *
 */
class Node : public QGraphicsItem {
 public:
  Node(const QString& label, QGraphicsItem* parent = nullptr);
  ~Node() override = default;

  /**
   * @brief Returns the bounding rectangle of the node.
   * @return The bounding rectangle.
   */
  QRectF boundingRect() const override;

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
  const QList<NodePort*>& getPorts() const { return ports; }

  /**
   * @brief Returns the type of the node.
   * @return The node type.
   */
  virtual NodeType getNodeType() const = 0;

 protected:
  /**
   * @brief Creates the ports for the node. This is a pure virtual function
   * that must be implemented by derived classes to define their specific
   * port configurations.
   */
  virtual void createPorts() = 0;
  QVariant itemChange(const GraphicsItemChange change,
                      const QVariant& value) override;
  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

  /** @brief The list of ports associated with the node. */
  QList<NodePort*> ports;

 private:
  /** @brief The label for the node. */
  QString mLabel{"Node"};

  /* Node properties */
  QColor mColor{QColor(100, 150, 240)};
};
}  // namespace KidTech::IDE