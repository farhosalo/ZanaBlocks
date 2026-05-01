#pragma once

#include <QGraphicsScene>

#include "NodePort.h"

// TODO:     Undo/redo (QUndoStack)

namespace KidTech {
namespace IDE {

class MainLoop;
class LoopNode;
class PrintNode;
class SleepNode;

/**
 * @brief Custom QGraphicsScene to manage node connections and interactions in
 * the IDE.
 */
class Scene : public QGraphicsScene {
  Q_OBJECT
 public:
  explicit Scene(QObject* parent = nullptr);
  ~Scene() override = default;

  /**
   * @brief Begins a new connection from the specified port.
   * @param port The starting port for the connection.
   */
  void beginConnection(NodePort* port);

  /**
   * @brief Updates the temporary connection line during dragging.
   * @param pos The current position of the mouse in scene coordinates.
   */
  void updateConnection(QPointF pos);

  /**
   * @brief Ends the current connection attempt.
   * @param targetPort The target port for the connection.
   */
  void endConnection(NodePort* targetPort);

  /**
   * @brief Creates a new connection between two ports.
   * @param from The starting port for the connection.
   * @param to The ending port for the connection.
   */
  void createConnection(NodePort* from, NodePort* to);

  /**
   * @brief Creates a new item of the specified type at the given position.
   * @param type The type of item to create (e.g., "Loop", "Action").
   * @param pos The position in scene coordinates where the item should be
   * created.
   */
  void createItemAt(const QString& type, const QPointF& pos);

 private:
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;

  /** @brief Deletes a connection and cleans up its references.
   * @param conn The connection to delete.
   */
  void deleteConnection(NodeConnector* conn);
  void deleteSelectedNodes();
  void deleteSelectedConnections();

  /** @brief The starting port for the current connection. */
  NodePort* mStartPort = nullptr;

  /** @brief The temporary connection line during dragging. */
  QGraphicsPathItem* mTempConnection = nullptr;
};
}  // namespace IDE
}  // namespace KidTech