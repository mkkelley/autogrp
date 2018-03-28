#pragma once

#include <QStyledItemDelegate>

class Config;

class AnalyzeSgfDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    AnalyzeSgfDelegate(Config* config, QObject* parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    Config* config;
};

