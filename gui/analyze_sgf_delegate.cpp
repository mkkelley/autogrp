#include "analyze_sgf_delegate.h"

#include <QApplication>
#include <QMouseEvent>
#include <QDialog>
#include <QFileInfo>
#include "config.h"

const int button_width = 80;

AnalyzeSgfDelegate::AnalyzeSgfDelegate(Config* config, QObject* parent) : QStyledItemDelegate(parent), config(config)
{  }

bool file_exists(QString& path) {
    QFileInfo check_file(path);
    return check_file.exists();
}


void AnalyzeSgfDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (index.column() < 3) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    QStyleOptionButton button;
    QRect r = option.rect;
    r.setWidth(80);

    button.rect = r;

    if (index.data().canConvert<QString>()) {
        QString path = qvariant_cast<QString>(index.data());
        if (file_exists(path)) {
            button.text = "Show";
        } else {
            button.text = "Analyze";
        }
    }
    button.state = QStyle::State_Enabled;
    QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
}

bool AnalyzeSgfDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    if (event->type() != QEvent::MouseButtonRelease) return false;

    QMouseEvent* e = static_cast<QMouseEvent*>(event);
    int click_x = e->x();
    int click_y = e->y();
    QRect r = option.rect;


    if (!(click_x > r.x() && click_x < r.x() + r.width() &&
            click_y > r.y() && click_y < r.y() + r.height())) {
        return false;
    }

    QDialog* d = new QDialog();
    QString path = qvariant_cast<QString>(index.data());
    d->setGeometry(0, 0, 100, 100);
    d->show();

    return true;
}

QSize AnalyzeSgfDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto size = QStyledItemDelegate::sizeHint(option, index);
    if (index.column() < 3) {
        return size;
    }

    size.setWidth(80);
    return size;
}
