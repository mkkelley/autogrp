#pragma once

#include <QAbstractTableModel>

#include "move.h"
#include "work_queue.h"

class Config;

struct SgfTableModel : public QAbstractTableModel
{
    Q_OBJECT;

public:
    SgfTableModel(Config* config, QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
public slots:
    void update_table();
private:
    std::string directory;
    std::vector<Sgf> sgfs;
    std::vector<BOT> bots_to_use;

};
