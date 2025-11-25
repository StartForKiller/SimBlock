#include <library/view.hpp>

#include <qschematic/items/item.hpp>
#include <qschematic/items/itemmimedata.hpp>

#include <QDrag>
#include <QPainter>

using namespace Library;

View::View(QWidget *parent) {
    _scale = 1.0;

    setDragDropMode(QAbstractItemView::DragOnly);
    setDragEnabled(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setHeaderHidden(true);
    setIconSize(QSize(28, 28));
}

void View::setPixmapScale(qreal scale) {
    _scale = scale;
}

void View::startDrag(Qt::DropActions supportedActions) {
    QModelIndexList indexes = selectedIndexes();
    if(indexes.count() != 1) {
        return;
    }

    QMimeData *data = model()->mimeData(indexes);
    if(!data) {
        return;
    }

    QSchematic::Items::MimeData *m = qobject_cast<QSchematic::Items::MimeData *>(data);
    if(!m) {
        return;
    }

    QDrag *drag = new QDrag(this);
    QPointF hotSpot;
    drag->setMimeData(data);
    drag->setPixmap(m->item()->toPixmap(hotSpot, _scale));
    drag->setHotSpot(hotSpot.toPoint());

    drag->exec(supportedActions, Qt::CopyAction);
}