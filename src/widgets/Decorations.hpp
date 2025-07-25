#pragma once

#include "widgets/EdgeDecoration.hpp"
#include <QWidget>
#include <QtCore>
#include <optional>
#include <qwebengineprofile.h>

enum DecorationType : uint8_t {
  DecorationTop = 1,
  DecorationBottom = 2,
  DecorationLeft = 3,
  DecorationRight = 4,
};

class Decorations : public QWidget {
  Q_OBJECT

public:
  Decorations(QWidget *content_widget, QWebEngineProfile *profile, QWidget *parent = nullptr);

  void set_enabled(DecorationType type, bool enabled);

private:
  EdgeDecoration *decoration_top;
  EdgeDecoration *decoration_bottom;
  EdgeDecoration *decoration_left;
  EdgeDecoration *decoration_right;

  std::optional<EdgeDecoration *> get_decoration_widget(DecorationType type);
};
