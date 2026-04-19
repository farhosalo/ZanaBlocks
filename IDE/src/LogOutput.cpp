#include "LogOutput.h"

#include <QStyle>

namespace KidTech::IDE {
void LogOutput::contextMenuEvent(QContextMenuEvent* event) {
  auto* menu = createStandardContextMenu();

  auto* deleteAllAction =
      new QAction(style()->standardIcon(QStyle::SP_LineEditClearButton),
                  "Delete All", this);

  connect(deleteAllAction, &QAction::triggered, this,
          [this]() { this->clear(); });

  menu->addAction(deleteAllAction);

  // Show the context menu at the position of the event
  menu->exec(event->globalPos());

  // Clean up the menu after it's closed
  delete menu;
}
}  // namespace KidTech::IDE