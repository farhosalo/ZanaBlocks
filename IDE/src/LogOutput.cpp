#include "LogOutput.h"

#include <QStyle>
#include <memory>

namespace ZanaBlocks::IDE {
void LogOutput::contextMenuEvent(QContextMenuEvent* event) {
  std::unique_ptr<QMenu> menu(createStandardContextMenu());

  auto* deleteAllAction =
      new QAction(style()->standardIcon(QStyle::SP_LineEditClearButton),
                  "Delete All", this);

  connect(deleteAllAction, &QAction::triggered, this,
          [this]() { this->clear(); });

  menu->addAction(deleteAllAction);

  // Show the context menu at the position of the event
  menu->exec(event->globalPos());
}
}  // namespace ZanaBlocks::IDE