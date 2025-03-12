#pragma once

#include <QAbstractListModel>
#include <QWidget>
#include <QtCore>

struct Command {
  QString name;
  QString description;
};

const QList<Command> commands = {
    {.name = "open", .description = "Open a url in the current tab"},
    {.name = "tabopen", .description = "Open a url in a new tab"},
    {.name = "tabnext", .description = "Go to next tab"},
    {.name = "tabbprev", .description = "Go to previous tab"},
};

class CommandsModel : public QAbstractListModel {
  Q_OBJECT

public:
  CommandsModel(QObject *parent = nullptr);
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QHash<int, QByteArray> roleNames() const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

private:
  QList<Command> items;
};
