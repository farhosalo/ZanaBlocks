#include "Scene.h"

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

#include "LedNode.h"
#include "Logging.h"
#include "LoopNode.h"
#include "PrintNode.h"
#include "Schema.pb.h"
#include "SleepNode.h"

using namespace ZanaBlocks::Utilities;

namespace ZanaBlocks::IDE {
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

bool Scene::serialize(const std::shared_ptr<Schema::Root>& root) {
  LoopNode* mainLoop = nullptr;

  // Find the main loop node
  for (auto* item : items()) {
    if (auto* node = dynamic_cast<LoopNode*>(item)) {
      if (node->isMainLoop()) {
        mainLoop = node;
        break;
      }
    }
  }

  if (!mainLoop) {
    ERROR("Main loop node not found!");
    return false;
  }

  if (mainLoop->getPorts().first()->connections().empty()) {
    ERROR("Main loop has no connections!");
    return false;  // Main loop node not found
  }

  getLoopSchema(mainLoop, root->mutable_mainloop());
  return true;
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
void Scene::getLoopSchema(const Node* loop, Schema::Loop* loopSchema) {
  // TODO: Sort loop elements according to X position

  loopSchema->CopyFrom(loop->schema());
  loopSchema->mutable_position()->set_x(loop->pos().x());
  loopSchema->mutable_position()->set_y(loop->pos().y());

  for (auto* port : loop->getPorts()) {
    if (port->getType() != PortType::Input) {
      for (auto* connection : port->connections()) {
        Node* targetNode = connection->getEndPort()->parentNode();
        if (auto* loopNode = dynamic_cast<LoopNode*>(targetNode)) {
          getLoopSchema(loopNode, loopSchema->add_actions()->mutable_loop());
        } else if (auto* printNode = dynamic_cast<PrintNode*>(targetNode)) {
          getPrintSchema(printNode, loopSchema->add_actions()->mutable_print());
        }
        if (auto* sleepNode = dynamic_cast<SleepNode*>(targetNode)) {
          getSleepSchema(sleepNode, loopSchema->add_actions()->mutable_sleep());
        }
        if (auto* ledNode = dynamic_cast<LedNode*>(targetNode)) {
          getLedSChema(ledNode, loopSchema->add_actions()->mutable_led());
        }
      }
    }
  }
}
void Scene::getPrintSchema(const PrintNode* printNode,
                           Schema::Print* printSchema) {
  printSchema->CopyFrom(printNode->schema());
  printSchema->mutable_position()->set_x(printNode->pos().x());
  printSchema->mutable_position()->set_y(printNode->pos().y());
}
void Scene::getSleepSchema(const SleepNode* sleepNode,
                           Schema::Sleep* sleepSchema) {
  sleepSchema->CopyFrom(sleepNode->schema());
  sleepSchema->mutable_position()->set_x(sleepNode->pos().x());
  sleepSchema->mutable_position()->set_y(sleepNode->pos().y());
}

void Scene::getLedSChema(const LedNode* ledNode, Schema::LED* ledSchema) {
  ledSchema->CopyFrom(ledNode->schema());
  ledSchema->mutable_position()->set_x(ledNode->pos().x());
  ledSchema->mutable_position()->set_y(ledNode->pos().y());
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
}  // namespace ZanaBlocks::IDE