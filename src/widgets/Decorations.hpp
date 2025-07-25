#pragma once

#include "widgets/EdgeDecoration.hpp"
#include <QWidget>
#include <QtCore>
#include <qwebengineprofile.h>

class Decorations : public QWidget {
  Q_OBJECT

public:
  Decorations(QWidget *content_widget, QWebEngineProfile *profile, QWidget *parent = nullptr);

private:
  EdgeDecoration *decoration_top;
  EdgeDecoration *decoration_bottom;
  EdgeDecoration *decoration_left;
  EdgeDecoration *decoration_right;
};
