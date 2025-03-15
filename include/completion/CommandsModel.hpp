#pragma once

#include <QAbstractListModel>
#include <QWidget>
#include <QtCore>

struct Command {
  QString name;
  QString description;
};

// TODO: Combine commands model completion and commands evaluation
const QList<Command> commands = {
    {.name = "open", .description = "Update current URL"},
    {.name = "tabs", .description = "Select a tab"},
    {.name = "tabopen", .description = "Open a url in a new tab"},
    {.name = "tabnext", .description = "Go to next tab"},
    {.name = "tabprev", .description = "Go to previous tab"},
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
