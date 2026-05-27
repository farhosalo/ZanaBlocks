#pragma once
#include <QGraphicsEllipseItem>

#include "NodeConnector.h"

namespace ZanaBlocks::IDE {
class Scene;
class Node;

/**
 * @brief Represents a port on a node that can be connected to other ports.
 */
enum class PORT_TYPE : uint8_t {
  INPUT,          ///< Single-connector input port (only one connection allowed)
  SINGLE_OUTPUT,  ///< Single-connector output port (only one connection
                  ///< allowed)
  MULTI_OUTPUT  ///< Multi-connector output port (multiple connections allowed)
};

/**
 * @brief Represents a port on a node that can be connected to other ports. The
 * NodePort class manages its connections and handles user interactions for
 * initiating connections. It also provides visual feedback when hovered.
 */
class NodePort : public QGraphicsEllipseItem {
 public:
  explicit NodePort(PORT_TYPE type, QGraphicsItem* parent = nullptr);

  /**
   * @brief Adds a connection to this port.
   * @param conn The connection to add.
   */
  void addConnection(NodeConnector* conn) { mConnections.append(conn); }

  /**
   * @brief Returns the list of connections associated with this port.
   * @return The list of connections.
   */
  [[nodiscard]] const QList<NodeConnector*>& connections() const {
    return mConnections;
  }

  /**
   * @brief Removes a connection from this port.
   * @param conn The connection to remove.
   */
  void removeConnection(NodeConnector* conn) { mConnections.removeOne(conn); }

  /**
   * @brief Returns the parent node of this port.
   * @return The parent node, or nullptr if the parent is not a Node.
   */
  [[nodiscard]] Node* parentNode() const;

  /**
   * @brief Returns the type of this port (SingleConnectorPort or
   * MultiConnectorPort).
   * @return The type of this port.
   */
  [[nodiscard]] PORT_TYPE getType() const { return mType; }

 protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

 private:
  /** @brief The list of connections associated with this port. */
  QList<NodeConnector*> mConnections;

  /** @brief The type of this port (SingleConnectorPort or MultiConnectorPort).
   */
  PORT_TYPE mType;
};
}  // namespace ZanaBlocks::IDE