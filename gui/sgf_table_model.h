#pragma once

#include <QAbstractTableModel>

#include "move.h"
#include "work_queue.h"

class Config;

struct SgfTableModel : public QAbstractTableModel
{
    SgfTableModel(Config* config);
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:
    std::string directory;
    std::vector<Sgf> sgfs;
    std::vector<BOT> bots_to_use;

};
