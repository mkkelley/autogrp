#include "sgf_table_model.h"

#include <boost/algorithm/string.hpp>
#include <QFont>
#include "downloader.h"
#include "config.h"

SgfTableModel::SgfTableModel(Config* config, QObject* parent) : QAbstractTableModel(parent), directory(config->games_dir)
{
    std::vector<std::string> bot_strings;
    boost::split(bot_strings, config->bots_to_use, boost::is_any_of(" \t"), boost::token_compress_on);
    for (const auto& bot_string : bot_strings) {
        bots_to_use.push_back(bot_from_string(bot_string));
    }
    auto files = get_directory_contents(directory);
    for (const auto& file : files) {
        if (file.find(".sgf") == std::string::npos) continue;
        sgfs.emplace_back(directory + "/" + file);
    }
}


int SgfTableModel::rowCount(const QModelIndex& parent) const {
    return sgfs.size();
}

int SgfTableModel::columnCount(const QModelIndex& parent) const {
    // file name, white player, black player, then each bot
    return 3 + bots_to_use.size();
}

QVariant SgfTableModel::data(const QModelIndex &index, int role) const {
    const Sgf& sgf = sgfs[index.row()];
    if (role == Qt::FontRole) {
        QFont font;
        font.setBold(true);
        if (sgf.white_won && index.column() == 2) {
            return font;
        } else if (sgf.black_won && index.column() == 1) {
            return font;
        } else {
            return QVariant::Invalid;
        }
    }

    if (role != Qt::DisplayRole) return QVariant::Invalid;

    switch (index.column()) {
    case 0: {
        std::string filename = sgf.filename.substr(sgf.filename.find_last_of('/') + 1);
        return QString::fromStdString(filename); }
    case 1:
        return QString::fromStdString(sgf.black);
    case 2:
        return QString::fromStdString(sgf.white);
    default:
        if (index.column() < columnCount()) {
            std::string analyzed_file_path = sgf.filename;
            auto idx = analyzed_file_path.find_last_of('.');
            analyzed_file_path.insert(idx + 1, "r");
            analyzed_file_path.insert(idx, "_" + string_from_bot(bots_to_use[index.column() - 3]));
            return QString::fromStdString(analyzed_file_path);
        } else {
            return QVariant::Invalid;
        }
    }

}

QVariant SgfTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Vertical || role != Qt::DisplayRole) return QVariant::Invalid;
    switch (section) {
    case 0:
        return QString("File");
    case 1:
        return QString("Black");
    case 2:
        return QString("White");
    default:
        if (section < columnCount()) {
            std::string bot_string = string_from_bot(bots_to_use[section - 3]);
            return QString::fromStdString(bot_string);
        } else {
            return QVariant::Invalid;
        }
    }
}
