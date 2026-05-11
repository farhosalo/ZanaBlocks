#pragma once
#include <QGraphicsPathItem>
#include <QPen>

namespace ZanaBlocks::IDE {

class NodePort;

/**
 * @brief Represents a connection between two NodePorts in the visual
 * programming diagram. The NodeConnector class manages the visual
 * representation of the connection and updates its path when the connected
 * ports move. It also provides a wider hit area for easier interaction.
 */
class NodeConnector : public QGraphicsPathItem {
 public:
  NodeConnector(NodePort* start, NodePort* end = nullptr);
  ~NodeConnector() override = default;

  /**
   * @brief Returns the starting port of the connection.
   * @return The starting port, or nullptr if not connected.
   */
  NodePort* getStartPort() const { return mStartPort; }

  /**
   * @brief Returns the ending port of the connection.
   * @return The ending port, or nullptr if not connected.
   */
  NodePort* getEndPort() const { return mEndPort; }

  /**
   * @brief Updates the path of the connection based on the current positions
   * of the connected ports.
   */
  void updatePath();

 private:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;

  // Make the hit area wider than the visual line for easier interaction
  QPainterPath shape() const override;

  NodePort* mStartPort;
  NodePort* mEndPort;
  QPointF
      mTempEndPoint;  // Used when dragging a connection without a target port
};
}  // namespace ZanaBlocks::IDE