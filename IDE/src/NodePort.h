#pragma once
#include <QGraphicsEllipseItem>

#include "NodeConnector.h"
#include "NodePort.h"

namespace KidTech::IDE {
class Scene;
class Node;

enum class PortType { Input, Output };

// TODO: Use 2 separate classes for input and output ports, to enforce
// connection rules at compile time

/**
 * @brief Represents a port on a node that can be connected to other ports. The
 * NodePort class manages its connections and handles user interactions for
 * initiating connections. It also provides visual feedback when hovered.
 */
class NodePort : public QGraphicsEllipseItem {
 public:
  NodePort(QGraphicsItem* parent = nullptr);
  ~NodePort() override = default;

  /**
   * @brief Adds a connection to this port.
   * @param conn The connection to add.
   */
  void addConnection(NodeConnector* conn) { mConnections.append(conn); }

  /**
   * @brief Returns the list of connections associated with this port.
   * @return The list of connections.
   */
  const QList<NodeConnector*>& connections() const { return mConnections; }

  /**
   * @brief Removes a connection from this port.
   * @param conn The connection to remove.
   */
  void removeConnection(NodeConnector* conn) { mConnections.removeOne(conn); }

  /**
   * @brief Returns the parent node of this port.
   * @return The parent node, or nullptr if the parent is not a Node.
   */
  Node* parentNode() const;

 private:
  void hoverEnterEvent(QGraphicsSceneHoverEvent*) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  /** @brief The list of connections associated with this port. */
  QList<NodeConnector*> mConnections;
};
}  // namespace KidTech::IDE