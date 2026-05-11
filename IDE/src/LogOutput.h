#pragma once

#include <QContextMenuEvent>
#include <QMenu>
#include <QPlainTextEdit>

namespace ZanaBlocks::IDE {

/**
 *@brief A custom QPlainTextEdit that adds a "Delete All" option to the context
 * menu.
 */
class LogOutput : public QPlainTextEdit {
  Q_OBJECT

 public:
  using QPlainTextEdit::QPlainTextEdit;

 protected:
  void contextMenuEvent(QContextMenuEvent* event) override;
};
}  // namespace ZanaBlocks::IDE