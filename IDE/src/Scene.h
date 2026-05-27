#pragma once

#include <QGraphicsScene>

#include "NodePort.h"

// TODO:     Undo/redo (QUndoStack)

namespace ZanaBlocks {
namespace Schema {

class Root;
class Loop;
class Print;
class Sleep;
class LED;
}  // namespace Schema

namespace IDE {

class MainLoop;
class LoopNode;
class PrintNode;
class SleepNode;
class LedNode;

/**
 * @brief Custom QGraphicsScene to manage node connections and interactions in
 * the IDE.
 */
class Scene : public QGraphicsScene {
  Q_OBJECT
 public:
  explicit Scene(QObject* parent = nullptr);

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
   * @param fromPort The starting port for the connection.
   * @param toPort The ending port for the connection.
   */
  void createConnection(NodePort* fromPort, NodePort* toPort);

  /**
   * @brief Creates a new item of the specified type at the given position.
   * @param type The type of item to create (e.g., "Loop", "Action").
   * @param pos The position in scene coordinates where the item should be
   * created.
   */
  void createItemAt(const QString& type, const QPointF& pos);

  /**
   * @brief Serializes the current scene into a Protobuf schema.
   * @param root The root schema object to populate.
   * @return True if serialization was successful, false otherwise.
   */
  bool serialize(const std::shared_ptr<Schema::Root>& root);

 private:
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;

  /**
   * @brief Recursively populates a Loop schema from a LoopNode and its
   * connected actions.
   * @param loop The LoopNode to serialize.
   * @param loopSchema The Protobuf Loop message to populate.
   */
  static void getLoopSchema(const Node* loop, Schema::Loop* loopSchema);

  /** @brief Populates a Print schema from a PrintNode.
   * * @param printNode The PrintNode to serialize.
   * @param printSchema The Protobuf Print message to populate.
   */
  static void getPrintSchema(const PrintNode* printNode,
                             Schema::Print* printSchema);

  /**
   * * @brief Populates a Sleep schema from a SleepNode.
   * @param sleepNode The SleepNode to serialize.
   * @param sleepSchema The Protobuf Sleep message to populate.
   */
  static void getSleepSchema(const SleepNode* sleepNode,
                             Schema::Sleep* sleepSchema);

  /**
   * * @brief Populates an LED schema from an LedNode.
   * @param ledNode The LedNode to serialize.
   * @param ledSchema The Protobuf LED message to populate.
   */
  static void getLedSChema(const LedNode* ledNode, Schema::LED* ledSchema);

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
}  // namespace ZanaBlocks