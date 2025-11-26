#include <library/model.hpp>
#include <library/iteminfo.hpp>
#include <items/operationconstant.hpp>
#include <items/operationdemo1.hpp>
#include <items/operationscope.hpp>
#include <items/widgets/dial.hpp>
#include <items/widgets/textedit.hpp>

#include <qschematic/items/itemmimedata.hpp>
#include <qschematic/items/label.hpp>

#include <QMimeData>

using namespace Library;

Model::Model(QObject *parent) :
    QAbstractItemModel(parent)
{
    _rootItem = new model_item(Root, nullptr);

    createModel();
}

Model::~Model() {
    delete _rootItem;
}

void Model::createModel() {
    while(_rootItem->childCount() > 0) {
        beginRemoveRows(QModelIndex(), 0, 0);
        _rootItem->deleteChild(0);
        endRemoveRows();
    }

    auto rootOperations = new model_item(RootOperations, nullptr, _rootItem);
    beginInsertRows(QModelIndex(), _rootItem->childCount(), _rootItem->childCount());
    _rootItem->appendChild(rootOperations);
    endInsertRows();

    auto rootFlows = new model_item(RootFlows, nullptr, _rootItem);
    beginInsertRows(QModelIndex(), _rootItem->childCount(), _rootItem->childCount());
    _rootItem->appendChild(rootFlows);
    endInsertRows();

    auto rootBasics = new model_item(RootBasics, nullptr, _rootItem);
    beginInsertRows(QModelIndex(), _rootItem->childCount(), _rootItem->childCount());
    _rootItem->appendChild(rootBasics);
    endInsertRows();

    auto rootWidgets = new model_item(RootWidgets, nullptr, _rootItem);
    beginInsertRows(QModelIndex(), _rootItem->childCount(), _rootItem->childCount());
    _rootItem->appendChild(rootWidgets);
    endInsertRows();

    //TODO

    addTreeItem("Demo 1", QIcon(), new ::OperationDemo1, rootOperations);
    addTreeItem("Constant", QIcon(), new ::OperationConstant, rootOperations);
    addTreeItem("Scope", QIcon(), new ::OperationScope, rootOperations);

    addTreeItem("Dial", QIcon(), new ::Items::Widgets::Dial, rootWidgets);
    addTreeItem("TextEdit", QIcon(), new ::Items::Widgets::TextEdit, rootWidgets);

    auto label = new QSchematic::Items::Label;
    label->setHasConnectionPoint(false);
    label->setText(QStringLiteral("Label"));
    addTreeItem("Label", QIcon(), label, rootBasics);
}

void Model::addTreeItem(const QString &name, const QIcon &icon, const QSchematic::Items::Item *item, model_item *parent) {
    auto newItem = new model_item(Operation, new ItemInfo(name, icon, item), parent);
    beginInsertRows(QModelIndex(), _rootItem->childCount(), _rootItem->childCount());
    parent->appendChild(newItem);
    endInsertRows();
}


const QSchematic::Items::Item *Model::itemFromIndex(const QModelIndex &index) const {
    auto modelItem = static_cast<model_item *>(index.internalPointer());
    if(!modelItem) {
        return nullptr;
    }

    auto itemInfo = static_cast<const ItemInfo *>(modelItem->data());
    if(!itemInfo) {
        return nullptr;
    }

    return itemInfo->item;
}

QModelIndex Model::index(int row, int column, const QModelIndex &parent) const {
    if(!hasIndex(row, column, parent))
        return {};

    model_item *parentItem;
    if(!parent.isValid()) {
        parentItem = _rootItem;
    } else {
        parentItem = static_cast<model_item *>(parent.internalPointer());
    }

    model_item *childItem = parentItem->child(row);
    if(childItem)
        return createIndex(row, column, childItem);
    else
        return {};
}

QModelIndex Model::parent(const QModelIndex &child) const {
    if(!child.isValid())
        return {};

    auto childItem = static_cast<model_item *>(child.internalPointer());
    auto parentItem = childItem->parent();

    if(parentItem == _rootItem)
        return { };

    return createIndex(parentItem->row(), 0, parentItem);
}

int Model::rowCount(const QModelIndex &parent) const {
    model_item *parentItem;
    if(parent.column() > 0) {
        return 0;
    }

    if(!parent.isValid()) {
        parentItem = _rootItem;
    } else {
        parentItem = static_cast<model_item *>(parent.internalPointer());
    }

    return parentItem->childCount();
}

int Model::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);

    return 1;
}

QVariant Model::data(const QModelIndex &index, int role) const  {
    if(!index.isValid())
        return { };

    auto modelItem = static_cast<model_item *>(index.internalPointer());
    if(!modelItem)
        return { };

    const auto itemInfo = reinterpret_cast<const ItemInfo *>(modelItem->data());

    switch(modelItem->type()) {
        case Model::RootOperations: {
            switch(role) {
                case Qt::DisplayRole:
                    return "Operations";

                default:
                    return {};
            }
        }

        case Model::Operation: {
            switch(role) {
                case Qt::DisplayRole:
                    Q_ASSERT(itemInfo);
                    return itemInfo->name;

                default:
                    return {};
            }
        }

        case Model::RootFlows: {
            switch(role) {
                case Qt::DisplayRole:
                    return "Flows";

                default:
                    return {};
            }
        }

        case Model::Flow: {
            switch(role) {
                case Qt::DisplayRole:
                    Q_ASSERT(itemInfo);
                    return itemInfo->name;

                default:
                    return {};
            }
        }

        case Model::RootBasics: {
            switch(role) {
                case Qt::DisplayRole:
                    return "Basics";

                default:
                    return {};
            }
        }

        case Model::Basic: {
            switch(role) {
                case Qt::DisplayRole:
                    Q_ASSERT(itemInfo);
                    return itemInfo->name;

                default:
                    return {};
            }
        }

        case Model::RootWidgets: {
            switch(role) {
                case Qt::DisplayRole:
                    return "Widgets";

                default:
                    return {};
            }
        }

        case Model::Widgets: {
            switch(role) {
                case Qt::DisplayRole:
                    Q_ASSERT(itemInfo);
                    return itemInfo->name;

                default:
                    return {};
            }
        }

        default:
            return {};
    }
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const {
    if(!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    auto modelItem = static_cast<model_item *>(index.internalPointer());
    if(!modelItem)
        return Qt::NoItemFlags;

    switch(modelItem->type()) {
        case RootOperations:
            flags &= ~Qt::ItemIsSelectable;
            break;

        default:
            flags |= Qt::ItemIsDragEnabled;
            break;
    }

    return flags;
}

QStringList Model::mimeTypes() const {
    return { QSchematic::Items::MIME_TYPE_NODE };
}

QMimeData *Model::mimeData(const QModelIndexList &indexes) const {
    if(indexes.count() != 1)
        return new QMimeData();

    const QModelIndex &index = indexes.first();
    if(!index.isValid())
        return new QMimeData();

    auto modelItem = static_cast<model_item *>(index.internalPointer());
    if(!modelItem)
        return new QMimeData();

    switch(modelItem->type()) {
        case Operation:
        case Widgets: {
            auto itemInfo = static_cast<const ItemInfo *>(modelItem->data());
            if(!itemInfo) {
                return new QMimeData();
            }

            auto itemClone = itemInfo->item->deepCopy();

            return new QSchematic::Items::MimeData(std::move(itemClone));
        }

        default:
            break;
    }

    return new QMimeData();
}
