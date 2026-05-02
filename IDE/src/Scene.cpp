#include "Scene.h"

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

#include "LedNode.h"
#include "LoopNode.h"
#include "PrintNode.h"
#include "SleepNode.h"
#include "PrintNode.h"
namespace KidTech::IDE {
Scene::Scene(QObject* parent) : QGraphicsScene(parent) {}

void Scene::beginConnection(NodePort* port) {
  mStartPort = port;
  mTempConnection = new QGraphicsPathItem();
  mTempConnection->setPen(QPen(Qt::gray, 2, Qt::DashLine));  // ← add this
  addItem(mTempConnection);

  // Make all ports hittable during drag
  for (QGraphicsItem* item : items()) {
    if (auto* node = dynamic_cast<Node*>(item)) {
      node->showInputPorts();
    }
  }
}
void Scene::updateConnection(QPointF pos) {
  if (!mStartPort) return;
  QPainterPath path(mStartPort->scenePos());
  path.lineTo(pos);
  mTempConnection->setPath(path);
}
void Scene::endConnection(NodePort* targetPort) {
  removeItem(mTempConnection);
  delete mTempConnection;
  mTempConnection = nullptr;

  if (mStartPort && targetPort) {
    createConnection(mStartPort, targetPort);
  }

  mStartPort = nullptr;

  // Hide ports again
  for (QGraphicsItem* item : items()) {
    if (auto* node = dynamic_cast<Node*>(item)) {
      node->hidePorts();
    }
  }
}

void Scene::createConnection(NodePort* from, NodePort* to) {
  if (!from || !to || from == to) return;

  Node* fromNode = from->parentNode();
  Node* toNode = to->parentNode();
  if (!fromNode || !toNode || fromNode == toNode) return;

  // Check ALL ports of fromNode, not just the one being connected
  for (auto* port : fromNode->getPorts()) {
    for (auto* conn : port->connections()) {
      Node* otherNode = (conn->getStartPort()->parentNode() == fromNode)
                            ? conn->getEndPort()->parentNode()
                            : conn->getStartPort()->parentNode();
      if (otherNode == toNode) return;
    }
  }

  for (auto* port : toNode->getPorts()) {
    if (port->getType() == PortType::Input && !port->connections().isEmpty()) {
      return;  // Target node already has a connection to a single-connector
               // port
    }
  }

  auto* connection = new NodeConnector(from, to);
  addItem(connection);
  from->addConnection(connection);
  to->addConnection(connection);
  connection->updatePath();
}

void Scene::createItemAt(const QString& type, const QPointF& pos) {
  // TODO: Use a factory pattern or registration system for extensibility
  if (type.toUpper() == "LOOP") {
    auto* node = new LoopNode();
    node->setPos(pos);
    addItem(node);
  } else if (type.toUpper() == "PRINT") {
    auto* node = new PrintNode();
    node->setPos(pos);
    addItem(node);
  } else if (type.toUpper() == "SLEEP") {
    auto* node = new SleepNode();
    node->setPos(pos);
    addItem(node);
  } else if (type.toUpper() == "LED") {
    auto* node = new LedNode();
    node->setPos(pos);
    addItem(node);
  }
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  updateConnection(event->scenePos());
  QGraphicsScene::mouseMoveEvent(event);
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  if (mStartPort) {
    NodePort* target = nullptr;
    // Search within a 10px radius so release doesn't have to be pixel-perfect
    const QRectF hitArea(event->scenePos() - QPointF(10, 10), QSizeF(20, 20));
    for (QGraphicsItem* item : items(hitArea)) {
      auto* port = dynamic_cast<NodePort*>(item);
      if (port && port != mStartPort) {
        target = port;
        break;
      }
    }
    endConnection(target);
  }
  QGraphicsScene::mouseReleaseEvent(event);
}
void Scene::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
    deleteSelectedConnections();
    deleteSelectedNodes();
  }
  QGraphicsScene::keyPressEvent(event);
}
void Scene::deleteConnection(NodeConnector* conn) {
  if (!conn) return;
  conn->getStartPort()->removeConnection(conn);
  conn->getEndPort()->removeConnection(conn);
  removeItem(conn);
  delete conn;
}
void Scene::deleteSelectedNodes() {
  for (QGraphicsItem* item : selectedItems()) {
    if (auto* node = dynamic_cast<LoopNode*>(item)) {
      if (node->isMainLoop())
        // Don't delete the main loop node
        continue;
    }
    if (auto* node = dynamic_cast<Node*>(item)) {
      for (auto* port : node->getPorts()) {
        for (auto* conn : port->connections()) {
          deleteConnection(conn);
        }
      }
      removeItem(item);
      delete item;
    }
  }
}
void Scene::deleteSelectedConnections() {
  for (QGraphicsItem* item : selectedItems()) {
    if (auto* conn = dynamic_cast<NodeConnector*>(item)) {
      deleteConnection(conn);
    }
  }
}
}  // namespace KidTech::IDE