#pragma once

#include "Node.h"

namespace KidTech::IDE {
/**
 * @brief Represents the print node in the visual programming diagram.
 */
class Print : public Node {
 public:
  explicit Print(QGraphicsItem* parent = nullptr);

 private:
  void createPorts() override;
};
}  // namespace KidTech::IDE
