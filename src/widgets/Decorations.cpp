#include <QWidget>
#include <QtCore>
#include <optional>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qwidget.h>

#include "Decorations.hpp"
#include "widgets/EdgeDecoration.hpp"

Decorations::Decorations(QWidget *content_widget, QWebEngineProfile *profile, QWidget *parent)
    : QWidget(parent) {
  decoration_top = new EdgeDecoration(false, profile, this);
  decoration_bottom = new EdgeDecoration(false, profile, this);
  decoration_left = new EdgeDecoration(true, profile, this);
  decoration_right = new EdgeDecoration(true, profile, this);

  QString content = R"HTML(
    <div>
      Hello world testing testing
      <button>Btn 1</button><button>Btn 2</button>
    </div>
  )HTML";
  decoration_bottom->set_html(content);
  // decoration_bottom->set_enabled(true);

  auto *vbox = new QVBoxLayout();
  vbox->setContentsMargins(0, 0, 0, 0);
  vbox->setSpacing(0);
  setLayout(vbox);

  auto *container = new QWidget(this);
  auto *hbox = new QHBoxLayout();
  hbox->setContentsMargins(0, 0, 0, 0);
  hbox->setSpacing(0);
  container->setLayout(hbox);

  vbox->addWidget(decoration_top);
  vbox->addWidget(container);
  vbox->addWidget(decoration_bottom);

  hbox->addWidget(decoration_left);
  hbox->addWidget(content_widget);
  hbox->addWidget(decoration_right);
}

void Decorations::set_enabled(DecorationType type, bool enabled) {
  auto decoration = get_decoration_widget(type);
  if (decoration.has_value())
    decoration.value()->set_enabled(enabled);
}

bool Decorations::get_enabled(DecorationType type) {
  auto decoration = get_decoration_widget(type);
  return decoration.has_value() && decoration.value()->is_enabled();
}

std::optional<EdgeDecoration *> Decorations::get_decoration_widget(DecorationType type) {
  switch (type) {
  case DecorationType::DecorationTop:
    return decoration_top;
  case DecorationType::DecorationBottom:
    return decoration_bottom;
  case DecorationType::DecorationLeft:
    return decoration_left;
  case DecorationType::DecorationRight:
    return decoration_right;
  }
  return nullptr;
}
